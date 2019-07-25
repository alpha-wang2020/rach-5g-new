#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <time.h>
#include "stdarg.h"
#include "pdu.h"
#include<stdint.h>

#include"RACH-ConfigDedicated.h"
#include"BeamFailureRecoveryConfig.h"
#include"SubcarrierSpacing.h"


#define RAR_PAYLOAD_SIZE_MAX 128
#define MAX 80 
#define PORT 8021
#define SA struct sockaddr 
//#define PREAMBLE_INDEX 56


uint8_t PREAMBLE_INDEX = 56 ;
uint8_t PREAMBLE_TRANSMISSION_COUNTER ;
uint8_t PREAMBLE_POWER_RAMPING_COUNTER; 
uint8_t PREAMBLE_POWER_RAMPING_STEP ;
uint8_t PREAMBLE_RECEIVED_TARGET_POWER;
int DELTA_PREAMBLE; 

typedef struct {
    uint8_t payload[RAR_PAYLOAD_SIZE_MAX];
}RAR_PDU;


RAR_PDU RAR_pdu;
BeamFailureRecoveryConfig_t *BFRC;


/*Preamble format has to be acquired from prach-configindex (RACH-ConfigGeneric.h)
Temporarily creating a enum for preamble format*/
typedef enum {
        f0,
        f1,
        f2,
        f3,
        fA1,
        fA2,
        fA3,
        fB1,
        fB2,
        fB3,
        fB4,
        fC0,
        fC2}preamble_format ;


int preamble_transmit(preamble_format format, 
                       e_SubcarrierSpacing subcarrierspacing,
                       RACH_ConfigDedicated_t *attempt1,
                       int s_id,
                       int t_id,
                       int f_id,
                       int ul_carrier_id )
{
  
  long  preamble_received_targetPower = attempt1->cfra->occasions->rach_ConfigGeneric.preambleReceivedTargetPower;
  long  preamble_trans_max = attempt1->cfra->occasions->rach_ConfigGeneric.preambleTransMax;
  
    if ((PREAMBLE_TRANSMISSION_COUNTER > 1)&&((PREAMBLE_POWER_RAMPING_COUNTER <= preamble_trans_max/*$e2c*/)||(1))&& (1)/*$e2c*/)
  {
    PREAMBLE_POWER_RAMPING_COUNTER += 1 ;
  }
    
    uint8_t scs;
    /*sub-carrier spacing configuration determined by msg1-SubcarrierSpacing(IE BeamFailureRecoveryConfig)
    and Table 4.2-1 in TS 38.211 */

    switch(subcarrierspacing)
    {
      case 0  : scs = 15;
            break;
      case 1  : scs = 30;
            break;
      case 2  : scs = 60;
            break;
      case 3  : scs = 120;
            break;
    };
    

    /*DELTA_PREAMBLE values are found from Table 7.3-1 and 7.3-2 
    TS 38.321 v15.5.0 Rel 15 */
    switch(format)
    {
      case 0: DELTA_PREAMBLE   = 0;         // format = 0
          break;
        case 1: DELTA_PREAMBLE = -3;        // format = 1
          break;
      case 2: DELTA_PREAMBLE   = -6;        // format = 2
          break;
        case 3: DELTA_PREAMBLE = 0;         // format = 3
          break;
      case 4: DELTA_PREAMBLE   = 8 + 3*scs; // format = A1
          break;
      case 5: DELTA_PREAMBLE   = 5 + 3*scs; // format = A2
          break;
      case 6: DELTA_PREAMBLE   = 3 + 3*scs; // format = A3
          break;
      case 7: DELTA_PREAMBLE   = 8 + 3*scs; // format = B1
          break;
      case 8: DELTA_PREAMBLE   = 5 + 3*scs; // format = B2
          break;
      case 9: DELTA_PREAMBLE   = 3 + 3*scs; // format = B3
          break;
        case 10:DELTA_PREAMBLE = 3*scs ;    // format = B4
          break;
        case 11:DELTA_PREAMBLE = 11 + 3*scs;// format = CO
          break;
      case 12:DELTA_PREAMBLE   = 5 + 3*scs; // format = C2
          break;

    };



    PREAMBLE_RECEIVED_TARGET_POWER = preamble_received_targetPower + DELTA_PREAMBLE + (PREAMBLE_POWER_RAMPING_COUNTER -1 )* PREAMBLE_POWER_RAMPING_STEP;
    
    int RA_RNTI;
    /* Redo the below if condition properly  */
    if ((BFRC == NULL) && (attempt1->cfra == NULL))
    {
        
        RA_RNTI = 1 + s_id + (14 * t_id) + (14 * 80 * f_id) + (14 * 80 * 8 * ul_carrier_id);
    
    }
   
    /*Parameters to give to PHY (Output of this section)*/
    int PHY_parameters[3];
    PHY_parameters[0] = PREAMBLE_INDEX ;
    PHY_parameters[1] = PREAMBLE_RECEIVED_TARGET_POWER ;
    PHY_parameters[2] = RA_RNTI ;

    printf("RA-RNTI is: %d \n",RA_RNTI );
    return RA_RNTI;

}
/*=================================================================================================================*/


int rar_reception(int sockfd) 
{ 	
	
  int ACKSI_request =0;

  RA_SUBHEADER_RAPID *rarh = (RA_SUBHEADER_RAPID*) RAR_pdu.payload;
  MAC_PDU_RAR mac_pdu;
	uint8_t *rar = (uint8_t*) (RAR_pdu.payload +1);
 
	recv(sockfd, rarh, sizeof(rarh),0);

	recv(sockfd,rar,sizeof(rar),0);



	printf("%02x\n",rarh->RAPID);
 printf("Received RAR (%02x|%02x.%02x.%02x.%02x.%02x.%02x.%02x) for preamble %d\n",
          *(uint8_t *) rarh, rar[0], rar[1], rar[2],
          rar[3], rar[4], rar[5], rar[6], rarh->RAPID);


  // RAPID + RAR (header)
  mac_pdu.mac_rar_rapid.ra_subheader_rapid.E    =rarh->E;
  mac_pdu.mac_rar_rapid.ra_subheader_rapid.T    =rarh->T;
  mac_pdu.mac_rar_rapid.ra_subheader_rapid.RAPID  =rarh->RAPID;

   // RAPID only
  mac_pdu.ra_subheader_rapid.E    =rarh->E;
  mac_pdu.ra_subheader_rapid.T    =rarh->T;
  mac_pdu.ra_subheader_rapid.RAPID  =rarh->RAPID;



//RAR
  if(mac_pdu.ra_subheader_rapid.RAPID == PREAMBLE_INDEX || mac_pdu.mac_rar_rapid.ra_subheader_rapid.RAPID == PREAMBLE_INDEX)
  {
    printf("RAR reception successful\n");
    if((mac_pdu.ra_subheader_rapid.T ==1 ||mac_pdu.mac_rar_rapid.ra_subheader_rapid.T==1))// && si_requestconfig)// to check if only RAPID subheader is present.
    {
      printf("random access procedure is successfully completed\n ");
      return ACKSI_request =1;//send ack for SI request to upper layers.  
    }
    else
    {
      // section 5.2 (PROCESS RAR)
      
    }
  }


  mac_pdu.mac_rar_rapid.mac_rar.TAC       = (uint16_t)(rar[0]<<5);
  mac_pdu.mac_rar_rapid.mac_rar.TAC       |=  (uint16_t)(rar[1] & 0xf8) >> 3;
  mac_pdu.mac_rar_rapid.mac_rar.H_Hopping_Flag  =   (uint16_t)(rar[1] & 0x04) >> 2;
  mac_pdu.mac_rar_rapid.mac_rar.PUSCH_Freq    =   (uint16_t)(rar[1] & 0x03) << 12;
  mac_pdu.mac_rar_rapid.mac_rar.PUSCH_Freq    |=  (uint16_t)(rar[2] << 4);
  mac_pdu.mac_rar_rapid.mac_rar.PUSCH_Freq    |=  (uint16_t)(rar[3] & 0xf0) >> 4;
  mac_pdu.mac_rar_rapid.mac_rar.PUSCH_Time    = (uint16_t)(rar[3] & 0x0f); 
  mac_pdu.mac_rar_rapid.mac_rar.MCS       = (uint16_t)(rar[4] & 0xf0) >> 4;
  mac_pdu.mac_rar_rapid.mac_rar.TPC       =   (uint16_t)(rar[4] & 0x0e) >> 1; 
  mac_pdu.mac_rar_rapid.mac_rar.CSI_Request   = (uint16_t)(rar[4] & 0x01);
  mac_pdu.mac_rar_rapid.mac_rar.T_CRNTI     = (uint16_t)(rar[5]<<8);
  mac_pdu.mac_rar_rapid.mac_rar.T_CRNTI     |= (uint16_t)(rar[6]);


 

  printf("decoded header- E,T,RAPID, (%d | %d| %d)\n", mac_pdu.mac_rar_rapid.ra_subheader_rapid.E, mac_pdu.mac_rar_rapid.ra_subheader_rapid.T , mac_pdu.mac_rar_rapid.ra_subheader_rapid.RAPID);
  printf("decoded RAR- TAC:%d\n Hopping  flag:%d\n PUSCH_Freq:%d\n PUSCH_Time:%d\n  MCS:%d\n TPC:%d\n CSI:%d\n T_CRNTI:%d\n", mac_pdu.mac_rar_rapid.mac_rar.TAC,mac_pdu.mac_rar_rapid.mac_rar.H_Hopping_Flag,
    mac_pdu.mac_rar_rapid.mac_rar.PUSCH_Freq,mac_pdu.mac_rar_rapid.mac_rar.PUSCH_Time,mac_pdu.mac_rar_rapid.mac_rar.MCS, mac_pdu.mac_rar_rapid.mac_rar.TPC,mac_pdu.mac_rar_rapid.mac_rar.CSI_Request,mac_pdu.mac_rar_rapid.mac_rar.T_CRNTI);

return 0;

} 

int create_socket()
{
  int sockfd, connfd; 
  struct sockaddr_in servaddr; 

  sockfd = socket(AF_INET, SOCK_STREAM, 0); 
  if (sockfd == -1) { 
    printf("socket creation failed...\n"); 
    exit(0); 
  } 
  else
    printf("Socket successfully created..\n"); 
  bzero(&servaddr, sizeof(servaddr)); 

  servaddr.sin_family = AF_INET; 
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
  servaddr.sin_port = htons(PORT); 

  if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
    printf("connection with the server failed...\n"); 
    exit(0); 
  } 
  else
    printf("connected to the server..\n"); 

  return sockfd;
}



int main(int argc, char const *argv[])
{
		
int ackSI,sockfd;
  //SI_RequestConfig_t *si_reqconfig=NULL;
  
MAC_PDU_RAR m_pdu;

sockfd = create_socket();

   
RACH_ConfigDedicated_t *attempt1 = (RACH_ConfigDedicated_t*) calloc(1,sizeof(RACH_ConfigDedicated_t));
attempt1->cfra = (CFRA_t *) calloc(1,sizeof(CFRA_t *));
attempt1->cfra->occasions = (struct CFRA__occasions *) calloc(1,sizeof(struct CFRA__occasions *));


attempt1->cfra->occasions->rach_ConfigGeneric.powerRampingStep = 2;
attempt1->cfra->occasions->rach_ConfigGeneric.preambleReceivedTargetPower = -202; // Preamble received target power to be received from RACH-ConfigGeneric IE
attempt1->cfra->occasions->rach_ConfigGeneric.preambleTransMax = 8;

PREAMBLE_POWER_RAMPING_STEP = attempt1->cfra->occasions->rach_ConfigGeneric.powerRampingStep; //As given in 5.1.1
e_SubcarrierSpacing subcarspacing = SubcarrierSpacing_kHz30;
preamble_format pformat= f2;

int s_id = 0;
int t_id = 0;
int f_id = 0;
int ul_carrier_id = 0;
int RA_RNTI = 0;

RA_RNTI = preamble_transmit(pformat,
                      subcarspacing,
                      attempt1,
                      s_id,
                      t_id,
                      f_id,
                      ul_carrier_id);

// Preamble transmission assumed to be done by PHY
// Yet to know what 5.1.3 (UE) has to send to 5.1.4(gNB)


ackSI = rar_reception(sockfd); 

printf("%d\n", ackSI );

return 0;
}
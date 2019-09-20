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
#include "BeamFailureRecoveryConfig.h"
#include "RACH-ConfigCommon.h"
#include "SI-RequestConfig.h"
#include "MeasResultNR.h"



#define RAR_PAYLOAD_SIZE_MAX 128
#define UL_GRANT_FIELDS_MAX 6
#define MAX 80 
#define PORT 8004
#define SA struct sockaddr 


BeamFailureRecoveryConfig_t *BFRC;
uint8_t payload[RAR_PAYLOAD_SIZE_MAX];
uint8_t PREAMBLE_INDEX;
uint8_t PREAMBLE_TRANSMISSION_COUNTER ;
uint8_t PREAMBLE_POWER_RAMPING_COUNTER; 
uint8_t PREAMBLE_POWER_RAMPING_STEP ;
uint8_t PREAMBLE_RECEIVED_TARGET_POWER;
int DELTA_PREAMBLE; 
int F_ACKSI_request =0;   // to be sent to upper layer
uint16_t PHY_BUF[UL_GRANT_FIELDS_MAX];  // to be sent to lower layer
//e_RA_Prioritization__scalingFactorBI s = RA_Prioritization__scalingFactorBI_dot5;  // configured by RRC
int F_SI_PREAMBLE = 0; // flag indicating preamble configured for SI-request

int SI_request = 1; // assuming SIB1 contains SI_REQUEST configured
//long *numberofpreamble_SI;
int numberofpreamble_SI=16; 
MAC_PDU_RAR mac_pdu;

int choosing_preamble (int SI_request,RACH_ConfigDedicated_t *attempt1);


int uniform_distribution(int rangeLow, int rangeHigh)
{
    int myRand = (int)rand(); 
    int range = rangeHigh - rangeLow + 1; //+1 makes it [rangeLow, rangeHigh], inclusive.
    int myRand_scaled = (myRand % range) + rangeLow;
    return myRand_scaled;
}




void setTimeout(int milliseconds)
{
    // If milliseconds is less or equal to 0
    // will be simple return from function without throw error
    if (milliseconds <= 0) {
        fprintf(stderr, "Count milliseconds for timeout is less or equal to 0\n");
        return;
    }

    // a current time of milliseconds
    int milliseconds_since = clock() * 1000 / CLOCKS_PER_SEC;

    // needed count milliseconds of return from this timeout
    int end = milliseconds_since + milliseconds;

    // wait while until needed time comes
    do {
        milliseconds_since = clock() * 1000 / CLOCKS_PER_SEC;
    } while (milliseconds_since <= end);
}



/*typedef struct {
    uint8_t payload[RAR_PAYLOAD_SIZE_MAX];
}RAR_PDU;

RAR_PDU RAR_pdu;*/






/*CBRA FOR SI REQUEST ALONE-TRIAL USING RANDOM VALUES, ASSUMING FIRST 10 PREAMBLES CONFIGURED FOR SI*/
/*int  choosing_preamble (int SI_request,RACH_ConfigDedicated_t *attempt1)
{
    
    //long q=50;
    //attempt1->cfra->totalNumberOfRA_Preambles_v1530=&q; 
     //*numberofpreamble_SI=64- *(attempt1->cfra->totalNumberOfRA_Preambles_v1530);  // Number of preambles for SI-request to choose  (ERROR: 2 extra preambles !!)
     printf("Number of preamble reserved for SI-REQUEST: %lo\n", *numberofpreamble_SI );
     
     printf("Number of preamble reserved for SI-REQUEST: %lo\n", *numberofpreamble_SI );
     if (SI_request)
    {
    int selected_SSB;
    int ra_PreambleStartIndex =0; // Assuming always 0
    // chosen SSB 
    srand(time(0));
    selected_SSB=rand()%64;
    printf("SSB That crosses rsrp :%d\n",selected_SSB);
    PREAMBLE_INDEX = (ra_PreambleStartIndex + selected_SSB)%(*numberofpreamble_SI)  ;  // grouping first 10 preambles for SI-request
    F_SI_PREAMBLE=1;
    
    }
    else
    {
        int a= rand()% *(attempt1->cfra->totalNumberOfRA_Preambles_v1530);
        PREAMBLE_INDEX = 64-a;
    }
    return PREAMBLE_INDEX;
}*/

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
    BeamFailureRecoveryConfig_t *BFRC;
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


void rar_reception(int sockfd, int tim,RACH_ConfigDedicated_t *attempt1) 
{   
    int t_1,backoff_time,i,j=0, cnt=0;
  clock_t T;
  int f_rarecept=0;
  
  int PREAMBLE_BACKOFF;
  uint16_t TEMPORARY_CRNTI;

  int ra_responsewindow = 11; // assuming s11 configured by rrc
  int ra_resptime= 11*0.05; // assuming 30kHz subcarrier spacing each slot is 0.5 ms

  /*RA_SUBHEADER_RAPID *rarh_RAPID=( RA_SUBHEADER_RAPID *)malloc(sizeof(RA_SUBHEADER_RAPID));
  RA_SUBHEADER_BI *rarh_BI=( RA_SUBHEADER_BI *)malloc(sizeof(RA_SUBHEADER_BI));
  MAC_RAR *rar=( MAC_RAR *)malloc(sizeof(MAC_RAR));*/
 
 

  recv(sockfd,payload,sizeof(payload),0);
  printf("\nreceived mac_pdu for rar\n");

          for (i=0;i<RAR_PAYLOAD_SIZE_MAX;i++)
			{
				printf("0x%02x|",payload[i]);  	// TO CORRECT: junk value is being printed at the beginning

			}


      //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%5555//


     
  while(1)
  {

        T=clock();
    
        t_1=T*1000/CLOCKS_PER_SEC;
        printf("Timer:%d\n  ",t_1 );
        
        if ((t_1<=tim+ra_resptime))   // procedures to be done within ra-response window
        {
         
             // printf("\ntimes loop runs BEGIN:%d\n",j);
             // printf("\ninside ra response window\n");
              // printf("\nreceived rapid: %d \n",payload[j]&0x3f);
              // printf("\nto check t flag: %d\n",payload[j]>>6 & 0x01);
                 
                 if (j==0 && (payload[j]>>6 & 0x01) == 0)
                {
                    printf("\n inside BI loop\n");
                  	mac_pdu.ra_subheader_bi.T =  (uint16_t)payload[j]>>6 & 0x01;
                  	mac_pdu.ra_subheader_bi.E = (uint16_t) payload[j]>>7;
                  	mac_pdu.ra_subheader_bi.BI =  (uint16_t)payload[j]&0x04;

                  	int SCALING_FACTOR_BI;
           //   BFRC->ra_Prioritization->scalingFactorBI has to be actually received from gNB
           //   if the structure pointer(BFRC) is received properly,then segmentation fault won't come 
           //   when accessing BFRC->ra_Prioritization->scalingFactorBI
           //  Temporarily assuming a long varaiable and using the value
                  long scaling_Factor_BI = 0;
                  BFRC->ra_Prioritization->scalingFactorBI = &scaling_Factor_BI;

                  switch(*(BFRC->ra_Prioritization->scalingFactorBI))
                  {
                    case 0 /*RA_Prioritization__scalingFactorBI_zero*/: SCALING_FACTOR_BI = 0;
                    break;
                    case 1/*RA_Prioritization__scalingFactorBI_dot25*/:SCALING_FACTOR_BI = 0.25;
                    break;
                    case 2 /*RA_Prioritization__scalingFactorBI_dot5*/: SCALING_FACTOR_BI = 0.5;
                    break;
                    case 3/*RA_Prioritization__scalingFactorBI_dot75*/: SCALING_FACTOR_BI = 0.75;
                    break;

                  }

                  switch(mac_pdu.ra_subheader_bi.BI)
                  { 
                      case 0: PREAMBLE_BACKOFF=BACK_OFF_IND0*SCALING_FACTOR_BI;
                      break;  
                      case 1: PREAMBLE_BACKOFF=BACK_OFF_IND1*SCALING_FACTOR_BI;
                        break;  
                      case 2: PREAMBLE_BACKOFF=BACK_OFF_IND2*SCALING_FACTOR_BI;
                        break;
                      case 3: PREAMBLE_BACKOFF=BACK_OFF_IND3*SCALING_FACTOR_BI;
                        break;
                      case 4: PREAMBLE_BACKOFF=BACK_OFF_IND4*SCALING_FACTOR_BI;
                        break;
                      case 5: PREAMBLE_BACKOFF=BACK_OFF_IND5*SCALING_FACTOR_BI;
                        break;
                      case 6: PREAMBLE_BACKOFF=BACK_OFF_IND6*SCALING_FACTOR_BI;
                        break;
                      case 7: PREAMBLE_BACKOFF=BACK_OFF_IND7*SCALING_FACTOR_BI;
                        break;
                      case 8: PREAMBLE_BACKOFF=BACK_OFF_IND8*SCALING_FACTOR_BI;
                        break;    
                      case 9: PREAMBLE_BACKOFF=BACK_OFF_IND9*SCALING_FACTOR_BI;
                        break;    
                      case 10:PREAMBLE_BACKOFF=BACK_OFF_IND10*SCALING_FACTOR_BI;
                        break;    
                      case 11: PREAMBLE_BACKOFF=BACK_OFF_IND11*SCALING_FACTOR_BI;
                        break;    
                      case 12: PREAMBLE_BACKOFF=BACK_OFF_IND12*SCALING_FACTOR_BI;
                        break;    
                      case 13: PREAMBLE_BACKOFF=BACK_OFF_IND13*SCALING_FACTOR_BI;
                        break;
                      // case 14 and 15 reserved value. (?)
                   }

                    	j=j+1;

                }         
                


                if (((payload[j]>>6 & 0x01) == 1) && ((payload[j]&0x3f) == PREAMBLE_INDEX))

                {
                	
                    printf("\n inside RAPID loop\n");     // RAPID + RAR (header)
                PREAMBLE_BACKOFF=0;	
                printf("RAR reception successful\n");
                  f_rarecept=1;
                  break;

               
                  
                }
                

              	
               

                if (payload[j]&0x3f <= numberofpreamble_SI)
                {
                  j=j+1;
                }
                else
                {
                  j=j+8;
                }




               

                  // RAPID only
                  /*mac_pdu.ra_subheader_rapid.E    =rarh->E;
                  mac_pdu.ra_subheader_rapid.T    =rarh->T;
                  mac_pdu.ra_subheader_rapid.RAPID  =rarh->RAPID;*/

                //printf("TB decoded successfully\n");
                //printf("decoded header- E,T,RAPID, (%d | %d| %d)\n", mac_pdu.ra_subheader_rapid.E, mac_pdu.ra_subheader_rapid.T , mac_pdu.ra_subheader_rapid.RAPID);
                
        // MAC subPDU with BI
                

               
      		

                

              
                
              }
              else
              {
                printf("ra_window expired or payload full preamble didnt match !!\n");
                PREAMBLE_TRANSMISSION_COUNTER++;
                if (PREAMBLE_TRANSMISSION_COUNTER==attempt1->cfra->occasions->rach_ConfigGeneric.preambleTransMax)
                {
                  /* code */
                }
                 
                backoff_time=uniform_distribution(0,PREAMBLE_BACKOFF);
                printf("%d\n", backoff_time);
                // call resource selection function (Do SECTION 5.1.2)

                break;
              }
           

  }
       //printf("%02x\n",rarh->RAPID);
   if (f_rarecept==1)
          {
               // printf("\ntimes loop runs RAPID:%d\n",j);
                mac_pdu.ra_subheader_rapid.E    = (uint16_t) payload[j]>>7;
                mac_pdu.ra_subheader_rapid.T    = (uint16_t)payload[j]>>6 & 0x01;
                mac_pdu.ra_subheader_rapid.RAPID  = (uint16_t)payload[j]&0x3f;

                //printf("T flag: %d\n", (payload[0]>>6)&0x01 );
                printf("\nRAPID%d\n", mac_pdu.ra_subheader_rapid.RAPID);
                j=j+1;

            if( mac_pdu.ra_subheader_rapid.RAPID <= (numberofpreamble_SI))// checks if the random preamble is si_requestconfig)// to check if only RAPID subheader is present.
            {
                
               printf("random access procedure is successfully completed : SI REQUEST\n ");
               F_ACKSI_request =1;           //send ack for SI request to upper layers.  
            }
            else 
            {
                 // section 5.2 (process Timing advance command)
      
                //printf("\ntimes loop runs ULGRANT:%d\n",j);
        //TAC
                mac_pdu.mac_rar.TAC       = (uint16_t)(payload[j]<<5);
                mac_pdu.mac_rar.TAC       |=  (uint16_t)(payload[j+1] & 0xf8) >> 3;

        //UL-GRANT
                mac_pdu.mac_rar.H_Hopping_Flag  =   (uint16_t)(payload[j+1] & 0x04) >> 2;
                mac_pdu.mac_rar.PUSCH_Freq    =   (uint16_t)(payload[j+1] & 0x03) << 12;
                mac_pdu.mac_rar.PUSCH_Freq    |=  (uint16_t)(payload[j+2] << 4);
                mac_pdu.mac_rar.PUSCH_Freq    |=  (uint16_t)(payload[j+3] & 0xf0) >> 4;
                mac_pdu.mac_rar.PUSCH_Time    = (uint16_t)(payload[j+3] & 0x0f); 
                mac_pdu.mac_rar.MCS       = (uint16_t)(payload[j+4] & 0xf0) >> 4;
                mac_pdu.mac_rar.TPC       =   (uint16_t)(payload[j+4] & 0x0e) >> 1; 
                mac_pdu.mac_rar.CSI_Request   = (uint16_t)(payload[j+4] & 0x01);

         //temporary CRNTI
                mac_pdu.mac_rar.T_CRNTI     = (uint16_t)(payload[j+5]<<8); 
                mac_pdu.mac_rar.T_CRNTI     |= (uint16_t)(payload[j+6]);
                                 
                // decoded UL-GRANT to be sent to lower layers
                printf("decoded RAR- TAC:%d\n Hopping  flag:%d\n PUSCH_Freq:%d\n PUSCH_Time:%d\n  MCS:%d\n TPC:%d\n CSI:%d\n T_CRNTI:%d\n", 
                          mac_pdu.mac_rar.TAC,mac_pdu.mac_rar.H_Hopping_Flag,
                          mac_pdu.mac_rar.PUSCH_Freq,mac_pdu.mac_rar.PUSCH_Time,mac_pdu.mac_rar.MCS, 
                          mac_pdu.mac_rar.TPC,mac_pdu.mac_rar.CSI_Request,mac_pdu.mac_rar.T_CRNTI);

                PHY_BUF[1]=mac_pdu.mac_rar.H_Hopping_Flag;
                PHY_BUF[2]=mac_pdu.mac_rar.PUSCH_Freq;
                PHY_BUF[3]=mac_pdu.mac_rar.PUSCH_Time;
                PHY_BUF[4]=mac_pdu.mac_rar.MCS;
                PHY_BUF[5]=mac_pdu.mac_rar.TPC;
                PHY_BUF[6]=mac_pdu.mac_rar.CSI_Request;                                            
              
              if(PREAMBLE_INDEX != 0 ) // not selected by mac among CB-preambles
              {
                printf("random access procedure is successfully completed\n ");
                return;
              }
              else
              {
                  // contention based 
                TEMPORARY_CRNTI = mac_pdu.mac_rar.T_CRNTI;
              }/* code */
            }
            
      
             
            
          }
 

   
  return;
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
    
    
    
    int sockfd,tim;
    clock_t t;
  //SI_RequestConfig_t *si_reqconfig=NULL;
  
    MAC_PDU_RAR m_pdu;

    sockfd = create_socket();
    BFRC = (BeamFailureRecoveryConfig_t *) calloc(1,sizeof(BeamFailureRecoveryConfig_t)) ;
    BFRC->ra_Prioritization = (struct RA_Prioritization*) calloc(1,sizeof(struct RA_Prioritization));
   
    RACH_ConfigDedicated_t *attempt1 = (RACH_ConfigDedicated_t*) calloc(1,sizeof(RACH_ConfigDedicated_t));
    attempt1->cfra = (CFRA_t *) calloc(1,sizeof(CFRA_t *));
    attempt1->cfra->occasions = (struct CFRA__occasions *) calloc(1,sizeof(struct CFRA__occasions *));


    attempt1->cfra->occasions->rach_ConfigGeneric.powerRampingStep = 2;
    attempt1->cfra->occasions->rach_ConfigGeneric.preambleReceivedTargetPower = -202; // Preamble received target power to be received from RACH-ConfigGeneric IE
    attempt1->cfra->occasions->rach_ConfigGeneric.preambleTransMax = 8;

    PREAMBLE_POWER_RAMPING_STEP = attempt1->cfra->occasions->rach_ConfigGeneric.powerRampingStep; //As given in 5.1.1
    e_SubcarrierSpacing subcarspacing = SubcarrierSpacing_kHz30;
    preamble_format pformat= f2;

    //PREAMBLE_INDEX=choosing_preamble (SI_request,attempt1);
    srand(time(0));
    PREAMBLE_INDEX=rand()%64;
    printf("The preamble chosen by UE:%d\n", PREAMBLE_INDEX );


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
    send(sockfd,&PREAMBLE_INDEX , sizeof(PREAMBLE_INDEX),0);
// Preamble transmission assumed to be done by PHY
// Yet to know what 5.1.3 (UE) has to send to 5.1.4(gNB)


    t=clock();

    tim = t*1000/CLOCKS_PER_SEC;
    printf("time after preamble is sent: %d\n",tim );
   // setTimeout(10);
    rar_reception(sockfd,tim,attempt1); 


    return 0;



}
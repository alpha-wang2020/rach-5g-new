/*
Brief :- Implementation of section 5.1.2, 5.1.3, 5.1.4 of TS 38.321, v15.5.0
*/

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
#include <time.h>



#define RAR_PAYLOAD_SIZE_MAX 128
#define MAX 80 
#define PORT 8021
#define SA struct sockaddr 
//#define PREAMBLE_INDEX 56

uint8_t PREAMBLE_INDEX;
uint8_t PREAMBLE_TRANSMISSION_COUNTER ;
uint8_t PREAMBLE_POWER_RAMPING_COUNTER; 
uint8_t PREAMBLE_POWER_RAMPING_STEP ;
uint8_t PREAMBLE_RECEIVED_TARGET_POWER;
int DELTA_PREAMBLE; 

typedef struct {
    uint8_t payload[RAR_PAYLOAD_SIZE_MAX];
}RAR_PDU;

RAR_PDU RAR_pdu;
int get_ssrsrp(int ssb_index)
{
  MeasResultNR_t * measResultNR ;

  for(int cnt =0; cnt <(measResultNR->measResult.rsIndexResults->resultsSSB_Indexes->list.count); cnt ++)
  {
    if(measResultNR->measResult.rsIndexResults->resultsSSB_Indexes->list.array[cnt]->ssb_Index == ssb_index)
    {
      return *(measResultNR->measResult.rsIndexResults->resultsSSB_Indexes->list.array[cnt]->ssb_Results->rsrp);
    }
  }

}

int get_csirsrp(int csi_index)
{
  MeasResultNR_t * measResultNR ;

  for(int cnt =0; cnt <(measResultNR->measResult.rsIndexResults->resultsCSI_RS_Indexes->list.count); cnt ++)
  {
    if(measResultNR->measResult.rsIndexResults->resultsCSI_RS_Indexes->list.array[cnt]->csi_RS_Index == csi_index)
    {
      return *(measResultNR->measResult.rsIndexResults->resultsCSI_RS_Indexes->list.array[cnt]->csi_RS_Results->rsrp);
    }
  }

}


void get_prach_resourses(RACH_ConfigDedicated_t * rach_ConfigDedicated,
             BeamFailureRecoveryConfig_t * beamfailureRecoveryConfig,
             RACH_ConfigCommon_t *rach_ConfigCommon,SI_RequestConfig_t * si_RequestConfig )

{ int messagePowerOffsetGroupB,
    ra_Msg3SizeGroupA;

  srand (time (NULL));
    
  if (beamfailureRecoveryConfig && rach_ConfigDedicated)

  {
    // start the timer
    PRACH_ResourceDedicatedBFR_t *prach_ResourceDedicatedBFR;//shud it be der ?

    switch(beamfailureRecoveryConfig -> candidateBeamRSList -> list.array[0] -> present) {
      case PRACH_ResourceDedicatedBFR_PR_NOTHING:
      break;

      case PRACH_ResourceDedicatedBFR_PR_ssb:
      { 
        BFR_SSB_Resource_t *BFR_SSB_Resource;

        for (int cnt = 0; cnt < (beamfailureRecoveryConfig->candidateBeamRSList)->list.count; cnt++)
        {
          if(get_ssrsrp(beamfailureRecoveryConfig->candidateBeamRSList->list.array[cnt]->choice.ssb.ssb) > 
            *(beamfailureRecoveryConfig->rsrp_ThresholdSSB))
            {
              PREAMBLE_INDEX= (beamfailureRecoveryConfig->candidateBeamRSList->list.array[cnt]->choice.ssb.ra_PreambleIndex);
               

            } 

          
        }
      }

      case PRACH_ResourceDedicatedBFR_PR_csi_RS:
      { 
        CFRA_t * cFRA;
        for (int cnt = 0; cnt < beamfailureRecoveryConfig->candidateBeamRSList->list.count; cnt++)
        {
          if(get_csirsrp(beamfailureRecoveryConfig->candidateBeamRSList->list.array[cnt]->choice.csi_RS.csi_RS) >
           rach_ConfigDedicated->cfra->resources.choice.csirs.rsrp_ThresholdCSI_RS)
            {
              PREAMBLE_INDEX = *(beamfailureRecoveryConfig->candidateBeamRSList->list.array[cnt]->choice.csi_RS.ra_PreambleIndex);
               

            } 

          
        }
      }

    }
  }

  else if ( rach_ConfigDedicated && rach_ConfigCommon)
  { 
    int msg1_FDM ;//wth
    switch(rach_ConfigDedicated->cfra->occasions->rach_ConfigGeneric.msg1_FDM)
    {
    case 0:
    msg1_FDM = 0;
    break;

    case 1:
    msg1_FDM = 2;
    break;

    case 2:
    msg1_FDM = 4;
    break;
    
    case 3:
    msg1_FDM = 8;
    break;

    }

    int ssb_perRACH_Occasion;
    switch(*(rach_ConfigDedicated->cfra->occasions->ssb_perRACH_Occasion))
    { 
      case 0:
      ssb_perRACH_Occasion = 1/8;

      case 1:
      ssb_perRACH_Occasion = 1/4;

      case 2:
      ssb_perRACH_Occasion = 1/2;

      case 3:
      ssb_perRACH_Occasion = 1;

      case 4:
      ssb_perRACH_Occasion = 2;

      case 5:
      ssb_perRACH_Occasion = 4;

      case 6:
      ssb_perRACH_Occasion = 8;

      case 7:
      ssb_perRACH_Occasion = 16;


    }



    switch(rach_ConfigDedicated->cfra->resources.present){
      case CFRA__resources_PR_NOTHING:
      break;

      case CFRA__resources_PR_ssb:
      {
        int ra_ssb_occasionindex = rach_ConfigDedicated->cfra->resources.choice.ssb.ra_ssb_OccasionMaskIndex;

        CFRA_t * cFRA;
        for (int cnt = 0; cnt < rach_ConfigDedicated->cfra->resources.choice.ssb.ssb_ResourceList.list.count; cnt++)
        {
          if(get_ssrsrp(rach_ConfigDedicated->cfra->resources.choice.ssb.ssb_ResourceList.list.array[cnt]->ssb) >
           *(rach_ConfigCommon->rsrp_ThresholdSSB))
          {
            PREAMBLE_INDEX = rach_ConfigDedicated->cfra->resources.choice.ssb.ssb_ResourceList.list.array[cnt]->ra_PreambleIndex;
            
          }
        }

        if(msg1_FDM == 1)
        { 
          switch (ra_ssb_occasionindex)
          {
            case 0:
            if(ssb_perRACH_Occasion = 1/8)
            {
              int nextavailablePRO = rand()%8;
            }

            else if(ssb_perRACH_Occasion = 1/4)
            {
              int nextavailablePRO = rand()%4;
            }

            else if(ssb_perRACH_Occasion = 1/2)
            {
              int nextavailablePRO = rand()%2;
            }

            else
            {
              int nextavailablePRO = 0;
            }
            break;

            case 9:
            if(ssb_perRACH_Occasion = 1/8)
            {
              int nextavailablePRO = (rand()%16)/2;

            }

            else if(ssb_perRACH_Occasion = 1/4)
            {
              int nextavailablePRO = (rand()%8)/2;
            }

            else if(ssb_perRACH_Occasion = 1/2)
            {
              int nextavailablePRO = (rand()%4)/2;
            }

            else
            {
              int nextavailablePRO = 0;
            }
            break;

            case 10:
            if(ssb_perRACH_Occasion = 1/8)
            {
              int nextavailablePRO = (rand()%16)/2+1;

            }

            else if(ssb_perRACH_Occasion = 1/4)
            {
              int nextavailablePRO = (rand()%8)/2+1;
            }

            else if(ssb_perRACH_Occasion = 1/2)
            {
              int nextavailablePRO = (rand()%4)/2+1;
            }

            else
            {
              int nextavailablePRO = 0;
            }
            break;


          }
        }

      }

      break;

      case CFRA__resources_PR_csirs:
      {
        CFRA_t * cFRA;
        for (int cnt = 0; cnt < rach_ConfigDedicated->cfra->resources.choice.csirs.csirs_ResourceList.list.count; cnt++)
        {
          if(get_csirsrp(rach_ConfigDedicated->cfra->resources.choice.csirs.csirs_ResourceList.list.array[cnt]->csi_RS) >
           rach_ConfigDedicated->cfra->resources.choice.csirs.rsrp_ThresholdCSI_RS)
          {
            PREAMBLE_INDEX = rach_ConfigDedicated->cfra->resources.choice.csirs.csirs_ResourceList.list.array[cnt]->ra_PreambleIndex;
             
          }
        }
      }

    }

      
  }

  else if(si_RequestConfig && rach_ConfigCommon)
  {	
	  MeasResultNR_t * measResultNR;
		//int total_SSBs = measResultNR->measResult.rsIndexResults->resultsCSI_RS_Indexes->list.count
		for(int cnt =0; cnt <(measResultNR->measResult.rsIndexResults->resultsSSB_Indexes->list.count); cnt ++)
			{
				if(measResultNR->measResult.rsIndexResults->resultsSSB_Indexes->list.array[cnt]->ssb_Results->rsrp > (rach_ConfigCommon->rsrp_ThresholdSSB) )
				{
					int selected_ssb = measResultNR->measResult.rsIndexResults->resultsSSB_Indexes->list.array[cnt]->ssb_Index;

					PREAMBLE_INDEX =  si_RequestConfig->si_RequestResources.list.array[cnt]->ra_PreambleStartIndex + cnt;
				}

				else 
				{
					PREAMBLE_INDEX = ra_PreambleStartIndex;
				}
			}
    

  } 

  /*else if(si-RequestConfig)
  {
    //left to god
  }*/
  switch (rach_ConfigCommon->groupBconfigured->messagePowerOffsetGroupB) {
        case 0:
            messagePowerOffsetGroupB = -9999;
            break;
        case 1:
            messagePowerOffsetGroupB = 0;
            break;
        case 2:
            messagePowerOffsetGroupB = 5;
            break;
        case 3:
            messagePowerOffsetGroupB = 8;
            break;
        case 4:
            messagePowerOffsetGroupB = 10;
            break;
        case 5:
            messagePowerOffsetGroupB = 12;
            break;
        case 6:
            messagePowerOffsetGroupB = 15;
            break;
        case 7:
            messagePowerOffsetGroupB = 18;
            break;
        }

    switch (rach_ConfigCommon->groupBconfigured->ra_Msg3SizeGroupA) {
      case 0:
            ra_Msg3SizeGroupA = 56;
            break;
        case 1:
            ra_Msg3SizeGroupA = 144;
            break;
        case 2:
            ra_Msg3SizeGroupA = 208;
            break;
        case 3:
            ra_Msg3SizeGroupA = 256;
            break;
        case 4:
            ra_Msg3SizeGroupA = 282;
            break;
        case 5:
            ra_Msg3SizeGroupA = 480;
            break;
        case 6:
            ra_Msg3SizeGroupA = 640;
            break;      
        case 7:
            ra_Msg3SizeGroupA = 800;
            break;
        case 8:
            ra_Msg3SizeGroupA = 1000;
            break;
        case 9:
            ra_Msg3SizeGroupA = 72;
            break;
                   
      }



  uint8_t Msg3_size ;
  uint8_t RA_usedGroupA;
  uint8_t msg3_DeltaPreamble;
  uint8_t PCMX;
  uint8_t first_Msg3;
  uint8_t noGroupB=1;
  uint8_t PLThreshold =
            0 - msg3_DeltaPreamble - rach_ConfigCommon->rach_ConfigGeneric.preambleReceivedTargetPower - messagePowerOffsetGroupB;
        // Note Pcmax is set to 0 here, we have to fix this
	//why ?
  if(rach_ConfigCommon->groupBconfigured)
  {
    noGroupB=0;
  }           

  if(first_Msg3 ==1)
  {
    if(noGroupB == 1)
    {
      RA_usedGroupA = 1; 
      PREAMBLE_INDEX = rand()%*(rach_ConfigCommon->totalNumberOfRA_Preambles);
      // select group A but wer  ?
    }
    else if (Msg3_size > ra_Msg3SizeGroupA)
    {
      RA_usedGroupA = 1;
      PREAMBLE_INDEX = rand()%*(rach_ConfigCommon->totalNumberOfRA_Preambles);
      //use grp A
    } 
    if (noGroupB == 0)
    {
      if(Msg3_size > ra_Msg3SizeGroupA && PLThreshold < PCMX )
      {
        if(1)//ccch sud size
        { 
          RA_usedGroupA = 0;
          PREAMBLE_INDEX = rand()%(*(rach_ConfigCommon->totalNumberOfRA_Preambles) -
                            (rach_ConfigCommon->groupBconfigured->numberOfRA_PreamblesGroupA));
        } 
      }

      else
        {
          RA_usedGroupA = 1; 
          PREAMBLE_INDEX = rand()%*(rach_ConfigCommon->totalNumberOfRA_Preambles);
        }// use grou A
    }


  }

  else
  {
    if(RA_usedGroupA=1)
    {
      PREAMBLE_INDEX = rand()%*(rach_ConfigCommon->totalNumberOfRA_Preambles);  
    }
    else
    {
      PREAMBLE_INDEX = rand()%(*(rach_ConfigCommon->totalNumberOfRA_Preambles) -
                            (rach_ConfigCommon->groupBconfigured->numberOfRA_PreamblesGroupA)); 
    } 
  }
 }


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

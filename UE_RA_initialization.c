#include<stdio.h>
#include<stdint.h>

#include "RACH-ConfigDedicated.h"
#include "RACH-ConfigCommon.h"
#include "RSRP-Range.h"
#include "BeamFailureRecoveryConfig.h"
#include "ringBuffer.h"
#include "ServingCellConfig.h"
/*---------------------------------------------------------
KEY:-
W - Most probably wrong. Need to change
D - Doubtful, needs verfification
U - To be updated
---------------------------------------------------------*/


  int PREAMBLE_INDEX;                  //Range [0,63]
  int PREAMBLE_TRANSMISSION_COUNTER;   //Range [0,preambletransmax]
  int PREAMBLE_POWER_RAMPING_COUNTER;  //Range [0,?]
  int PREAMBLE_POWER_RAMPING_STEP;
  int PREAMBLE_RECEIVED_TARGET_POWER;
  int PREAMBLE_BACKOFF;
  int PCMAX;
  int SCALING_FACTOR_BI;
  int TEMPORARY_CRNTI;
  

  //Variable to model - beam failure instance indication has been received from lower layers
  // 1 - Beam Failure; 0 - Beam detection successful
  int Beam_fail_flag; 
  
  // [D] - Need to verify data type
  // Variable to model - RSRP of the downlink pathloss reference
  RSRP_Range_t rsrp_dl_pl_ref;
  
  // [W] -  Need to identify which structure contains PCMAX,f,c 
  // Variable to model - Pcmax of 'explicitly signalled' carrier
  int PCMAX_f_c_car_exp;

  // Variable to model - Pcmax of SUL carrier
  int PCMAX_f_c_car_SUL;
  int PCMAX_f_c_car_NUL;
  int car_exp;
   
  ServingCellConfig_t *scc;
 
void ra_init(RACH_ConfigDedicated_t *attempt1, int *car_exp)
{ 
	 
   int n=50;

   //--------------Flushing Buffer ---------------//
   //[U] Msg3 buffer to be updated in DPDK framework later
   //int* Msg3_buffer;                                                        
   //Msg3_buffer = (int*)calloc(n, sizeof(int));
    //ringBuffer *msg3;
   //uint8_t source[3]={0};
   //writeToBuffer(msg3,3,source);//Using ring buffer 
   //----------------------END----------------------------//
   

   //--------------Setting parameters for preamble----------------//                              
   PREAMBLE_TRANSMISSION_COUNTER = 1;
   PREAMBLE_POWER_RAMPING_COUNTER = 1;
   PREAMBLE_BACKOFF = 0 ;
  //----------------------------END-----------------------------------//

  //-------------------------Carrier selection--------------------//
    
  /*[W] if(carrier for random access explicitly signalled)
  Modeling "Explicit signalling of carrier" using flag : car_exp
   0 - Not Explicitly signalled
   1 -Explicitly signalled         */

   // [W] Carrier frequency to be selected is currently modeled as a float number (eg : 2.48GHz)
  float carrier_freq;

  // [W] The carrier frequency explicitly signalled (by ????) is modeled as a float
  // This variable is to be obtained from other module (RRC perhaps?)
  float carrier_explicit = 2400 ;//MHz 
  
  // [W] The SUL carrier frequency is modeled as a float
  // This variable is to be obtained from other module (RRC perhaps?)
  float carrier_SUL = 180 ;//MHz

  // [W] The SUL carrier frequency is modeled as a float
  // This variable is to be obtained from other module (RRC perhaps?)
  float carrier_NUL = 2300 ;//MHz

    //ServingCellConfigCommon *serv_cell_conf;
   //serv_cell_conf->pdcch_ServingCellConfig=0;//Given by RRC
   //serv_cell_conf->pdsch_ServingCellConfig=0;//Given by RRC
   RACH_ConfigCommon_t *rsrp_t_sul;//rsrp range from 0 to 127 
  
  
  if(*car_exp == 1)         
    {
       
       carrier_freq = carrier_explicit;
       PCMAX = PCMAX_f_c_car_exp;
    }  

 
  //[W] - Need to know which structure is for configuring Serving cell with SUL
  else if ((car_exp == 0) && (scc->supplementaryUplink!=NULL/*serv_cell_conf->pdcch_ServingCellConfig!=NULL && serv_cell_conf->pdsch_ServingCellConfig!=NULL*/) 
                   && (rsrp_dl_pl_ref < *(rsrp_t_sul->rsrp_ThresholdSSB_SUL) ))  //NULL means configured else not configured
    {
       carrier_freq = carrier_SUL;
       PCMAX = PCMAX_f_c_car_SUL;

                    
    }

    else 
    {
      carrier_freq = carrier_NUL;
      PCMAX = PCMAX_f_c_car_NUL;
    }


  //-------------------------END--------------------------------------//



 //------------------------BWP operation-------------------//
	
	/*
     To be done later, upon finishing CBRA 
  */

  //------------------------END---------------------------------------//

   
 //--------------Setting parameters for REtranmission preamble ----------------//                              
   PREAMBLE_POWER_RAMPING_STEP = attempt1->cfra->occasions->rach_ConfigGeneric.powerRampingStep;
   SCALING_FACTOR_BI = 1;
  //----------------------------END-----------------------------------//


  //------------------------Trigger event : BFRC ---------------------//
   
  
   BeamFailureRecoveryConfig_t *beamFailureRecoveryConfig = (BeamFailureRecoveryConfig_t *) calloc(1,sizeof(BeamFailureRecoveryConfig_t));
   beamFailureRecoveryConfig->ra_Prioritization = (struct RA_Prioritization *) calloc(1,sizeof(struct RA_Prioritization)); 

   if ((Beam_fail_flag ==1) && (1)) //*beamfailconfigured!= NULL
   {  
    //Possibly start the timer in a differnt process
    *(beamFailureRecoveryConfig->beamFailureRecoveryTimer) = 0;

     
   }

   else if ((1)&&(attempt1 != NULL))//1st condition - Trigger = Handover
   {
     if (beamFailureRecoveryConfig->ra_Prioritization->powerRampingStepHighPriority != -1)//Rectify the condition
     {
        PREAMBLE_POWER_RAMPING_STEP = beamFailureRecoveryConfig->ra_Prioritization->powerRampingStepHighPriority;
     }
     
    long scaling_Factor_BI = 0;
    beamFailureRecoveryConfig->ra_Prioritization->scalingFactorBI = &scaling_Factor_BI;
     if (*(beamFailureRecoveryConfig->ra_Prioritization->scalingFactorBI) != -1)//Rectify the condition
     {
       SCALING_FACTOR_BI = *(beamFailureRecoveryConfig->ra_Prioritization->scalingFactorBI);
     }

   }
   
  //----------------------------END-----------------------------------//

 
  else if ((1/*Random access procedure initiated for handover*/) && (attempt1 != NULL))
   {
    if(1/*rach-ConfigDedicated is cnfigured for selected carrier*/)

    {
      if(attempt1->ra_Prioritization->powerRampingStepHighPriority != -1) //CONFIGURED 
      {
        PREAMBLE_POWER_RAMPING_STEP=attempt1->ra_Prioritization->powerRampingStepHighPriority;
      }
    }
    if(attempt1->ra_Prioritization->scalingFactorBI!=NULL) 
    {
      SCALING_FACTOR_BI=*(attempt1->ra_Prioritization->scalingFactorBI);
    
    }

  }  

}



void main()
{
   RACH_ConfigDedicated_t *attempt1 = (RACH_ConfigDedicated_t*) malloc(sizeof(RACH_ConfigDedicated_t));
   attempt1->cfra = (CFRA_t *) calloc(1,sizeof(CFRA_t)); 
   attempt1->cfra->occasions = (struct CFRA__occasions *) calloc(1,sizeof(struct CFRA__occasions));
   attempt1->ra_Prioritization = (struct RA_Prioritization *) calloc(1,sizeof(struct RA_Prioritization));
   car_exp = 1;

   ra_init(attempt1,&car_exp);          
}


/* BWP structure related config
BWP *bwp;       
bwp->locationAndBandwidth= rand()%37949;//INTEGER (0..37949) 
bwp->SubcarrierSpacing= kHz15;
bwp->cyclicPrefix=;
RA_Prioritization

RACH_ConfigGeneric *pr_step;
PREAMBLE_POWER_RAMPING_STEP = *(pr_step->powerRampingStep);
SCALING_FACTOR_BI = 1;
//if (Random Access procedure was initiated for beam failure recovery && beamFailureRecoveryConfig is configured for the active UL BWP of the selected carrier)
{
BeamFailureRecoveryConfig *beamFailureRecoveryConfig;
if(beamFailureRecoveryConfig!=NULL) //!=NULL means configured
 {
  *(beamFailureRecoveryConfig->beamFailureRecoveryTimer)=0;//start the timer
 }   
  RACH_ConfigGeneric * power_ramping_step;
     *(power_ramping_step->powerRampingStep)=;
     *(power_ramping_step->preambleReceivedTargetPower)=;
     *(beamFailureRecoveryConfig->preambleTransMax)=;
}
//RAP initiated for Beamfailurerecovery
//beamFailureRecoveryConfig is configured for the active UL BWP of the selected carrier
*/

/*
int prach-ConfigurationIndex;// 0-255
int preambleReceivedTargetPower;//-202..-60
BeamFailureRecoveryConfig_t * beam;
*(beam->rsrp_ThresholdSSB) = 35 ;//rsrp value is frm 0 to 127
*(cfra->rsrp_ThresholdCSI_RS) = *(beam->rsrp_ThresholdSSB); //Declared in Header and rsrp value is from 0 to 127

//SEQUENCE (SIZE(1..maxNrofCandidateBeams)) OF PRACH-ResourceDedicatedBFR

*(rsrp_ThresholdSSB_SUL)=35;//rsrp range from 0 to 127
//candidateBeamRSList = SEQUENCE (SIZE(1..maxNrofCandidateBeams)) OF PRACH-ResourceDedicatedBFR
*recoverySearchSpaceId= ;//SearchSpaceId recoverySearchSpaceId ;
RACH_ConfigGeneric_t.powerRampingStep=;//powerRampingStep
RA_Prioritization_t *prshp;//powerRampingStepHighPriority
*(prshp->powerRampingStepHighPriority)={};//ENUMERATED {dB0, dB2, dB4, dB6}
RA_Prioritization_t *sfbi;//scalingFactorBI
*(sfbi->scalingFactorBI)=;
//ra-PreambleIndex - Present in both BFR-SSB-Resource.h and BFR-CSIRS-Resource.h
*ra_ssb_OccasionMaskIndex=5;//ra-ssb-OccasionMaskIndex,INTEGER (0..15)
*ra_OccasionList=;//ra-OccasionList
//ra-PreambleStartIndex;there is confusion on which header file to use between cfra-csirs.h and bfr-csirs.h
RACH_ConfigGeneric_t * ptmax;//preambleTransMax
*(ptmax->preambleTransMax)={1,2,3,4,5,6,7,8,9,10,10,11};//ENUMERATED {n3, n4, n5, n6, n7, n8, n10, n20, n50, n100, n200},Assigned values from 1-11
ssb_perRACH_OccasionAndCB_PreamblesPerSSB *rcc;//RACH_ConfigCommon_t ssb-perRACH-OccasionAndCB-PreamblesPerSSB
//groupBconfigured
*/

  
   


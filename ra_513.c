/*==============================================================
Title        : Implementation of Random Access procedure in UE 
Project      : L2 protocol stack development for 5G Testbed project
Organization : IIT Madras
Author       : Abhay Vardhan, Monisha Yuvaraj, Pranav Kumar
Standards    : TS 38.321 v15.5.0 Rel 15, TS 38.331
================================================================*/

/*=================================================================================================================*/
/* Section 5.1.3, TS 38.321, v15.5.0 Rel 15*/
/* Implementation of Random Access preamble transmission */
/* Data flow direction : UE -> gNB */

/* Pending Corrections:-
1. The 1st 'if condition' in preamble_transmit() is incomplete.
2. Initializing UE variables (linked to 5.1.1)  
3. Acquisition of preamble format for determining DELTA_PREAMBLE (Accessing Config table )
4. The if condition prior to computing RA-RNTI.
5. Implementing "Selected PRACH occasion"  
*/

/* KEY :-
   $e2c - Problem converting English to C (Requires deeper understanding )
*/



#include<stdio.h>
#include<stdlib.h> // For calloc()
#include<stdint.h> // For uint8_t
#include<time.h>   // For clock()
#include"SubcarrierSpacing.h"
#include"RACH-ConfigCommon.h"
#include"RACH-ConfigDedicated.h"
#include"BeamFailureRecoveryConfig.h"


uint8_t PREAMBLE_INDEX ;
uint8_t PREAMBLE_TRANSMISSION_COUNTER ;
uint8_t PREAMBLE_POWER_RAMPING_COUNTER; 
uint8_t PREAMBLE_POWER_RAMPING_STEP ;
uint8_t PREAMBLE_RECEIVED_TARGET_POWER;
//Max value of Backoff parameter (according to Table 7.2-1 38.321 v15.5.0) is 1920 ms
//Therefore 11 bits are sufficient. For safe side, using 16 bits
uint16_t PREAMBLE_BACKOFF ;

int DELTA_PREAMBLE;




BeamFailureRecoveryConfig_t *BFRC = NULL;
RACH_ConfigDedicated_t *Contention_free = NULL;

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
                       long  preamble_received_targetPower,
                       long  preamble_trans_max,
                       int s_id,
                       int t_id,
                       int f_id,
                       int ul_carrier_id )
{
	

    if ((PREAMBLE_TRANSMISSION_COUNTER > 1)&&((PREAMBLE_POWER_RAMPING_COUNTER <= preamble_trans_max/*$e2c*/)||(1))&& (1)/*$e2c*/)
	{
		PREAMBLE_POWER_RAMPING_COUNTER += 1 ;
	}
    

    
    uint8_t scs;
    /*sub-carrier spacing configuration determined by msg1-SubcarrierSpacing(IE BeamFailureRecoveryConfig)
    and Table 4.2-1 in TS 38.211 */

    switch(subcarrierspacing)
    {
    	case 0	: scs = 15;
    			  break;
    	case 1	: scs = 30;
    			  break;
    	case 2	: scs = 60;
    			  break;
    	case 3	: scs = 120;
    			  break;
    };
    

    /*DELTA_PREAMBLE values are found from Table 7.3-1 and 7.3-2 
    TS 38.321 v15.5.0 Rel 15 */
    switch(format)
    {
    	case 0: DELTA_PREAMBLE = 0;         // format = 0
    			break;
        case 1: DELTA_PREAMBLE = -3;        // format = 1
    			break;
    	case 2: DELTA_PREAMBLE = -6;        // format = 2
    			break;
        case 3: DELTA_PREAMBLE = 0;         // format = 3
    			break;
    	case 4: DELTA_PREAMBLE = 8 + 3*scs; // format = A1
    			break;
    	case 5: DELTA_PREAMBLE = 5 + 3*scs; // format = A2
    			break;
    	case 6: DELTA_PREAMBLE = 3 + 3*scs; // format = A3
    			break;
    	case 7: DELTA_PREAMBLE = 8 + 3*scs; // format = B1
    			break;
    	case 8: DELTA_PREAMBLE = 5 + 3*scs; // format = B2
    			break;
    	case 9: DELTA_PREAMBLE = 3 + 3*scs; // format = B3
    			break;
        case 10:DELTA_PREAMBLE = 3*scs ;    // format = B4
    			break;
        case 11:DELTA_PREAMBLE = 11 + 3*scs;// format = CO
    			break;
    	case 12:DELTA_PREAMBLE = 5 + 3*scs; // format = C2
    			break;

    };



    PREAMBLE_RECEIVED_TARGET_POWER = preamble_received_targetPower + DELTA_PREAMBLE + (PREAMBLE_POWER_RAMPING_COUNTER -1 )* PREAMBLE_POWER_RAMPING_STEP;
    
    int RA_RNTI;
    /* Redo the below if condition properly  */
    if ((BFRC == NULL) && (Contention_free == NULL))
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


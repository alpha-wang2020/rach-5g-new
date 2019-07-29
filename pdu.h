/*==============================================================
Title 		 : Implementation of RACH procedure in UE 
Authors 	 : Pranav Kumar.S, Abhay Vardhan.M
Project 	 : L2 protocol stack development for 5G Testbed project
Organization : IIT Madras
Standards 	 : TS 38.321 v15.5.0
================================================================*/

#include <stdio.h>
#include<stdint.h>



#define BACK_OFF_IND0           5
#define BACK_OFF_IND1           10
#define BACK_OFF_IND2           20
#define BACK_OFF_IND3           30
#define BACK_OFF_IND4           40
#define BACK_OFF_IND5           60
#define BACK_OFF_IND6           80
#define BACK_OFF_IND7           120
#define BACK_OFF_IND8           160
#define BACK_OFF_IND9           240
#define BACK_OFF_IND10          320
#define BACK_OFF_IND11          480
#define BACK_OFF_IND12          960
#define BACK_OFF_IND13          1920

/*
Description :- MAC subheader format for DL-SCH & UL-SCH except for
               1.Fixed size MAC CE (or)
               2.Padding (or)
               3.SDU from CCCH  
               Note -Max(length_8) = 1 byte
*/
typedef struct MAC_subheader_R_F_LCID_8
{
	uint8_t reserve: 1;         // R bit - set to 0
	uint8_t format: 1;			// F bit -set to 0 if L field is 8 bit
	uint8_t LCID: 6 ;           //Logical Channel ID
	uint8_t length_8;           //Length(in bytes) of MAC SDU or variable sized
	

}MAC_Sbhr_R_F_LCID_8;

/*
Description :- MAC subheader format for DL-SCH & UL-SCH except for
               1.Fixed size MAC CE (or)
               2.Padding (or)
               3.SDU from CCCH  
               Note -Max(length_16) = 2 bytes
*/

typedef struct MAC_subheader_R_F_LCID_16
{
	uint8_t reserve: 1;         // R bit - set to 0
	uint8_t format: 1;			// F bit -set to 0 if L field is 8 bit
	uint8_t LCID: 6 ;          // Logical Channel ID
	uint16_t length_16;        //Length(in bytes) of MAC SDU or variable sized

}MAC_Sbhr_R_F_LCID_16;


/*
Description :- MAC subheader format for DL-SCH & UL-SCH for
               1.Fixed size MAC CE (or)
               2.Padding (or)
               3.SDU from CCCH  
*/

typedef struct MAC_subheader_R_F_LCID
{
	uint8_t reserve: 1;         // R bit - set to 0
	uint8_t format: 1;         // F bit 
	uint8_t LCID: 6 ;

}MAC_Sbhr_R_F_LCID;

/*
Description :- MAC subheader format for C-RNTI CE         
*/
typedef struct MAC_CE_CRNTI
{
	uint16_t C_RNTI; // Cell RNTI given by gNB

}MAC_CE_CRNTI;

/*
Description :- MAC subheader format for UE Contention resolution IdentityCE         
*/
typedef struct MAC_CE_Cont_Res_ID
{
	uint64_t UE_Cont_Res_ID:48; // Contention resolution ID given by gNB

}MAC_CE_Cont_Res_ID;

/*
Description :- MAC subheader format for RAPID
*/
typedef struct 
{ 
  uint8_t E:1;
  uint8_t T:1;
  uint8_t RAPID:6;

} __attribute__ ((__packed__)) RA_SUBHEADER_RAPID;

/*
Description :- MAC subheader format for Backoff Indicator 
*/

typedef struct
{
  uint8_t E:1;
  uint8_t T:1;
  uint8_t R:2;
  uint8_t BI:4;

}RA_SUBHEADER_BI;

/*
Description :- MAC RAR format
*/
typedef struct 
{ 
  uint16_t R:1;
  uint16_t TAC:12;
  uint16_t H_Hopping_Flag:1;
  uint16_t PUSCH_Freq:14;
  uint16_t PUSCH_Time:4;
  uint16_t MCS:4;
  uint16_t TPC:3;
  uint16_t CSI_Request:1;
  uint16_t T_CRNTI:16;

}MAC_RAR;

typedef struct 
{
  RA_SUBHEADER_RAPID ra_subheader_rapid;
  MAC_RAR mac_rar;

}MAC_subPDU_RAR_RAPID;

typedef struct
{
  RA_SUBHEADER_BI   ra_subheader_bi;
  RA_SUBHEADER_RAPID ra_subheader_rapid;
  MAC_subPDU_RAR_RAPID mac_rar_rapid;
}MAC_PDU_RAR; 

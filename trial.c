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
#include<error.h>
#include<errno.h>
#define RAR_PAYLOAD_SIZE_MAX 128
#define MAX 80 


 uint8_t payload[RAR_PAYLOAD_SIZE_MAX];
 int preamble_index;
 int BI=0; 
 int numberofpreamble_SI=16;   // from rach config dedicated


int rar_fill(uint8_t * const dlsch_buffer,int j,int preamble_index,int temp)
{
	int i,f=temp;
	printf("inside func_flag:%d\n",f );
	uint8_t *rar = (uint8_t*) (dlsch_buffer+1);
	RA_SUBHEADER_RAPID *rarh_RAPID=(RA_SUBHEADER_RAPID*) dlsch_buffer;;
	RA_SUBHEADER_BI *rarh_BI=(RA_SUBHEADER_BI*) dlsch_buffer;;
	
	if (BI==1)
	{
			rarh_BI->E = 0;
			rarh_BI->T = 0;
			rarh_BI->BI = 0;

			rar[0] = (uint8_t) (rarh_BI->E)<<7;
			rar[0] |= (uint8_t) (rarh_BI->T)<<6;
			rar[0] |=(uint8_t)(rarh_BI->BI )>>4;

			

		
		

	}

		if (f==-1 || f==2)
		{
			rar=(uint8_t*) rar+j*1;
			
		}
		else if (f==1) 	
		{
			rar=(uint8_t*) rar+j*8;
			
		}


	if (preamble_index<=numberofpreamble_SI)
	{

		
		//printf("SIze of rar:%d\n", sizeof(rar) );
		



			rarh_RAPID->E = 0;
			rarh_RAPID->T = 1;
			rarh_RAPID->RAPID = preamble_index;

			rar[0] = (uint8_t) (rarh_RAPID->E)<<7;
			rar[0] |= (uint8_t) (rarh_RAPID->T)<<6;
			rar[0] |=(uint8_t)(rarh_RAPID->RAPID);
			return f=2;
		
		
		

	}
	else
	{

						
			rar=(uint8_t*) rar+j*8;

			rarh_RAPID->E = 1;
			rarh_RAPID->T = 1;
			rarh_RAPID->RAPID = preamble_index;

			rar[0] = (uint8_t) (rarh_RAPID->E)<<7;
			rar[0] |= (uint8_t) (rarh_RAPID->T)<<6;
			rar[0] |=(uint8_t)(rarh_RAPID->RAPID);

			
			uint16_t timing_offset=rand()%4095;
			uint16_t PUSCH_Freq=rand()%16385;
			uint16_t PUSCH_Time=rand()%15;
			uint16_t MCS=rand()%15;
			uint16_t TPC=rand()%7;
			uint16_t CSI_Request=rand()%1;
			uint16_t Hopping_Flag=rand()%1;
			uint16_t T_CRNTI=rand()%65535;

			/*uint16_t timing_offset=48;
			uint16_t PUSCH_Freq=124;
			uint16_t PUSCH_Time=4;
			uint16_t MCS=2;
			uint16_t TPC=4;
			uint16_t CSI_Request=1;
			uint16_t Hopping_Flag=1;
			uint16_t T_CRNTI=300;*/


			rar[6] = (uint8_t) (T_CRNTI >> 8);
			rar[7] = (uint8_t) (T_CRNTI & 0xff);
			rar[1] = (uint8_t) (timing_offset >> 5);
			rar[2] = (uint8_t) (timing_offset << 3) & 0xf8;

			//UL GRANT
		
			rar[2] |= (uint8_t) (Hopping_Flag <<2);

			rar[2] |= (uint8_t) (PUSCH_Freq >>12 & 0x3);

			rar[3] = (uint8_t) (PUSCH_Freq >> 4);

			rar[4] = (uint8_t) (PUSCH_Freq << 4) & 0xf0 ;

			rar[4] |= (uint8_t) (PUSCH_Time);

			rar[5] = (uint8_t) (MCS << 4) & 0xf0;

			rar[5] |= (uint8_t) (TPC << 1) & 0x0e;

			rar[5] |= (uint8_t) (CSI_Request);
			
			return f=1;
		
		


		

	}
				
	
	


	
	
	//printf("%d\n", sizeof(dlsch_buffer));

	
	
	for (i=0;i<sizeof(rar);i++)
	{
		printf("0x%02x|",rar[i]);
	}
	

	return;
	
}


int main(int argc, char const *argv[])
{
	int i,j;
	int num_user =3;
	int temp=-1;
	for (j=0;j<num_user;j++)
	{
		preamble_index=rand()%64;
		printf("PREAMBLE INDEX%d\n", preamble_index);
		temp=rar_fill(payload,j,preamble_index,temp);
		printf("FLAG: %d\n",temp);
	}
	
  	printf("\n%d\n", sizeof(payload) );
  	for (i=0;i<RAR_PAYLOAD_SIZE_MAX;i++)
	{
		printf("0x%02x|",payload[i]);  	// TO CORRECT: junk value is being printed at the beginning

	}
	
	return 0;
}
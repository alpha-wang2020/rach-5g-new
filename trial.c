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
 int BI_f=0; 
 int numberofpreamble_SI=16;   // from rach config dedicated
 int i=0,cnt=0;


 
int rar_fill(int preamble_index)
{
	
	RA_SUBHEADER_RAPID *rarh_RAPID=( RA_SUBHEADER_RAPID *)malloc(sizeof(RA_SUBHEADER_RAPID));
   RA_SUBHEADER_BI *rarh_BI=( RA_SUBHEADER_BI *)malloc(sizeof(RA_SUBHEADER_BI));
   MAC_RAR *rar=( MAC_RAR *)malloc(sizeof(MAC_RAR));
//printf("inside func_flag:%d\n",f );
	int E,T,BI,RAPID;
	  for (i=cnt;i<RAR_PAYLOAD_SIZE_MAX ;i++ ) 
	  {
        if(payload[i]==0){
      		//printf("\nto check%d\n",i ); 
      		cnt++;
      		break;    

        }
        //printf("\ncurrent position: %d\n", cnt);
        
        
       }
	
	
	if (BI_f==1 && i==0)
	{
			rarh_BI->E = 1;
			rarh_BI->T = 1;
			rarh_BI->BI = 15;
			//printf("\nbi:\n");
			payload[i] = (uint8_t) (rarh_BI->E)<<7;
			payload[i] |= (uint8_t) (rarh_BI->T)<<6;
			payload[i] |=(uint8_t)(rarh_BI->BI );
			i++;
			
			

	}

		


	if (preamble_index<=numberofpreamble_SI)
	{

		
			rarh_RAPID->E = 0;
			rarh_RAPID->T = 1;
			rarh_RAPID->RAPID = preamble_index;

			payload[i] = (uint8_t) (rarh_RAPID->E)<<7;
			payload[i] |= (uint8_t) (rarh_RAPID->T)<<6;
			payload[i] |=(uint8_t)(rarh_RAPID->RAPID);
			//printf("for si\n");
			

	}
	else
	{

						
			rarh_RAPID->E = 0;
			rarh_RAPID->T = 1;
			rarh_RAPID->RAPID = preamble_index;

			payload[i] = (uint8_t) (rarh_RAPID->E)<<7;
			payload[i] |= (uint8_t) (rarh_RAPID->T)<<6;
			payload[i] |=(uint8_t)(rarh_RAPID->RAPID);

			
			rar->TAC=rand()%4095;
			rar->PUSCH_Freq=rand()%16385;
			rar->PUSCH_Time=rand()%15;
			rar->MCS=rand()%15;
			rar->TPC=rand()%7;
			rar->CSI_Request=rand()%1;
			rar->H_Hopping_Flag=rand()%1;
			rar->T_CRNTI=rand()%65535;


			payload[i+6] = (uint8_t) (rar->T_CRNTI >> 8);
			payload[i+7] = (uint8_t) (rar->T_CRNTI & 0xff);
			payload[i+1] = (uint8_t) (rar->TAC >> 5);
			payload[i+2] = (uint8_t) (rar->TAC << 3) & 0xf8;


			//UL GRANT
		
	
			payload[i+2] |= (uint8_t) (rar->H_Hopping_Flag <<2);

			payload[i+2] |= (uint8_t) (rar->PUSCH_Freq >>12 & 0x3);

			payload[i+3] = (uint8_t) (rar->PUSCH_Freq >> 4);

			payload[i+4] = (uint8_t) (rar->PUSCH_Freq << 4) & 0xf0 ;

			payload[i+4] |= (uint8_t) (rar->PUSCH_Time);

			payload[i+5] = (uint8_t) (rar->MCS << 4) & 0xf0;

			payload[i+5] |= (uint8_t) (rar->TPC << 1) & 0x0e;

			payload[i+5] |= (uint8_t) (rar->CSI_Request);

			//printf("for rapid+rar\n");
			
			
			

			
				


		

	}
				
	
	

	/*for (int i=0;i<RAR_PAYLOAD_SIZE_MAX;i++)
	{
		printf("0x%02x|",payload[i]);  	// TO CORRECT: junk value is being printed at the beginning

	}*/
	
	
	//printf("%d\n", sizeof(dlsch_buffer));
	return 0;
	
}


int main(int argc, char const *argv[])
{
	int i,j;
	int num_user =5;
	//int temp=-1;

	srand(time(0));

	for ( int i=0;i<RAR_PAYLOAD_SIZE_MAX ;i++ ) {
        payload[i]=0;
    }

   

	for (j=0;j<num_user;j++)
	{	

		preamble_index=rand()%64;
		printf("PREAMBLE INDEX%d\n", preamble_index);
		rar_fill(preamble_index);
		//printf("FLAG: %d\n",temp);
	}
	
  	printf("\n FINAL MAC PDU RAR\n" );
  	for (int i=0;i<RAR_PAYLOAD_SIZE_MAX;i++)
	{
		printf("0x%02x|",payload[i]);  	// TO CORRECT: junk value is being printed at the beginning

	}
	
	return 0;
}

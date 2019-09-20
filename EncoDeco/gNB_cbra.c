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
#define PORT 8004
#define SA struct sockaddr 
#include"RACH-ConfigDedicated.h"

int preamble_flagger[10] = {0};
int Max_No_of_CFRA_preambles = 10;
uint8_t payload[RAR_PAYLOAD_SIZE_MAX];
int preamble_index;
 int BI_f=0; 
 int numberofpreamble_SI=16;   // from rach config dedicated
 int i=0,cnt=0;

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

 
int rar_fill(int preamble_index)
{
	
   
   RA_SUBHEADER_RAPID *rarh_RAPID=( RA_SUBHEADER_RAPID *)malloc(sizeof(RA_SUBHEADER_RAPID));
   RA_SUBHEADER_BI *rarh_BI=( RA_SUBHEADER_BI *)malloc(sizeof(RA_SUBHEADER_BI));
   MAC_RAR *rar=( MAC_RAR *)malloc(sizeof(MAC_RAR));
	//printf("check1 :%d\n",cnt );
	
	  for (i=cnt;i<RAR_PAYLOAD_SIZE_MAX ;i++ ) 
	  {
        if(payload[i]==0){
      		//printf("\nto check%d\n",i ); 
      		break;    

        }
        else{

        	cnt++;
        }
        //printf("\ncurrent position: %d\n", cnt);
        
        
       }
	
	
	if (BI_f==1 && i==0)
	{
			rarh_BI->E = 0;
			rarh_BI->T = 0;
			rarh_BI->BI = 15;
			//printf("\nbi:\n");
			payload[i] = (uint8_t) (rarh_BI->E)<<7;
			payload[i] |= (uint8_t) (rarh_BI->T)<<6;
			payload[i] |=(uint8_t)(rarh_BI->BI );
			i++;
			cnt=cnt+1;
			

	}

		


	if (preamble_index<=numberofpreamble_SI)
	{

			//printf("\ncheck inside si before: %d\n", cnt);
			
			rarh_RAPID->E = 0;
			rarh_RAPID->T = 1;
			rarh_RAPID->RAPID = preamble_index;

			payload[i] = (uint8_t) (rarh_RAPID->E)<<7;
			payload[i] |= (uint8_t) (rarh_RAPID->T)<<6;
			payload[i] |=(uint8_t)(rarh_RAPID->RAPID);
			cnt=cnt+1;
		   //printf("\ncheck inside si after:  %d\n", cnt);
			
			

	}
	else
	{

			//printf("\ncheck inside r+r before: %d\n", cnt);			
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

			printf(" RAR CONTENT- TAC:%d\n Hopping  flag:%d\n PUSCH_Freq:%d\n PUSCH_Time:%d\n  MCS:%d\n TPC:%d\n CSI:%d\n T_CRNTI:%d\n", 
                          rar->TAC,rar->H_Hopping_Flag,
                         rar->PUSCH_Freq,rar->PUSCH_Time,rar->MCS, 
                          rar->TPC,rar->CSI_Request,rar->T_CRNTI);

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
			
			cnt=cnt+8;
			//printf("\ncheck inside r+r after: %d\n", cnt);	

			
				


		

	}
	
	//printf("\ncounter at the end:%d\n",cnt );
	
	//printf("\nbefore flag check\n");
	for (int i=0;i<RAR_PAYLOAD_SIZE_MAX;i++)
		{
			
			printf("0x%02x|",payload[i]);  	// TO CORRECT: junk value is being printed at the beginning

		}
	printf("\n");
	
	

	
	
	
	//printf("%d\n", sizeof(dlsch_buffer));
	return 0;
	
}

/*========================================================================================================================*/
int create_socket()
{
	int sockfd, connfd, len; 
	/* sockfd - Socket file descriptor, the welcoming socket
	   connfd - Connection file descriptor, the actual socket 
	   over which communication with client happens 
	*/ 
	struct sockaddr_in servaddr, UE_addr; 

	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1)
    {
    	error(1,errno,"Error creating TCP socket");
    }
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 

	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n"); 

	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 
	len = sizeof(UE_addr); 

	connfd = accept(sockfd, (SA*)&UE_addr, &len); 
	if (connfd < 0) { 
		printf("server acccept failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server accept the UE_addr...\n"); 

	return connfd;  
}
/*========================================================================================================================*/

int main(int ac, char **av)
{
    int connfd;
    int i,j,EOP;
	int num_user =3;
	int timer_expired =0;
	//int temp=-1;
	int ra_responsewindow = 11;     // Configured by RRC inside RACH-ConfigGeneric (assuming s11 is configured : s11=> 11 slots)
	float slot_time=0.05;  // assuming 30KHz subcarrier spacing each slot is 0.5ms
	float ra_resptime = ra_responsewindow*slot_time; 
	
	srand(time(0));
	clock_t time1,T;
	float T1 ,t_1;

	//RACH_ConfigDedicated_t *attempt1 = (RACH_ConfigDedicated_t*) calloc(1,sizeof(RACH_ConfigDedicated_t));

	// for now just declared in network side as well
	//long *numberofpreamble_SI;
    //long q=50;
    //attempt1->cfra->totalNumberOfRA_Preambles_v1530=&q; 
    // *numberofpreamble_SI=64- *(attempt1->cfra->totalNumberOfRA_Preambles_v1530); 

    int numberofpreamble_SI=16;


    connfd = create_socket();
    recv(connfd,&preamble_index,sizeof(preamble_index),0 );
    printf("received preamble : %d\n", preamble_index);
    time1=clock();
	T1=time1*1000/CLOCKS_PER_SEC;   // ms conversion
	//printf("Timerstart:%f\n  ",T1 );

	for ( int i=0;i<RAR_PAYLOAD_SIZE_MAX ;i++ ) {
        payload[i]=0;
    }
    int d=0;
    while(1)
    {

    	T=clock();
    
        t_1=T*1000/CLOCKS_PER_SEC;
        printf("Timer:%f\n  ",t_1 );
       // setTimeout(1);
        
        if (t_1<=T1+ra_resptime)
        {

			//for (j=0;j<2;j++)
			//{	

				//preamble_index=rand()%16;
				//preamble_index=0;
				printf("PREAMBLE INDEX%d\n", preamble_index);
				EOP=rar_fill(preamble_index);
				d++;
				printf("\nnumber of users: %d\n", d);
				if (EOP==1)
				{
					printf("\npayload full\n");
					break;
				}
			//}
				printf("resptime:%f\n",ra_resptime );
				printf("Timerstart:%f\n  ",T1 );

        }
        else{
        	printf("\n TIMER EXPIRED !! no more user pls !!\n");
        	timer_expired =1;
        	break;
        }
    }
   

    int size=sizeof(payload)-8;	    // TO DO: BETTER WAY TO FILL.(FOR SAFER SIDE GIVEN ATLEAST 8 PADDING SPACE SHOULD BE THERE AT THE END) 
	printf("SIZE:%d\n", size);
    if (cnt>=size || timer_expired==1)
	{
		printf("\n end of array\n");
		//printf("\nTO CHECK FLAG:|0x%02x|\n",payload[cnt] );
		
		//%%%%%%%%% TO SET E TO 1 INDICATING LAST SUBPDU %%%%%%%%%
		if (preamble_index<=numberofpreamble_SI)
		{
			
			printf("\ntype si\n");
			payload[cnt-1] |=  (uint8_t) 0x80;
		}
		else 
		{
			printf("\ntype r+r\n");
			payload[cnt-8] |= (uint8_t) 0X80;
		}

				
	}
	
  	printf("\n FINAL MAC PDU RAR\n" );
  	for (int i=0;i<RAR_PAYLOAD_SIZE_MAX;i++)
	{
		printf("0x%02x|",payload[i]);  	// TO CORRECT: junk value is being printed at the beginning

	}
	
	

	send(connfd,payload, sizeof(payload),0);
	printf("\n RAR sent \n");
	close(connfd); 

	
	return 0;
}
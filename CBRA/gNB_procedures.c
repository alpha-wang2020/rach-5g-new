/*==============================================================
Title        : Implementation of Random Access procedure in gNB
Project      : L2 protocol stack development for 5G Testbed project
Organization : IIT Madras
Authors      : (RACH team)
Standards    : TS 38.321 , TS 38.331 [v15.5.0 Rel 15]
================================================================*/


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
#define PORT 8002
#define SA struct sockaddr 
#include"RACH-ConfigDedicated.h"

int preamble_flagger[10] = {0};
int Max_No_of_CFRA_preambles = 10;

typedef struct {
    uint8_t payload[RAR_PAYLOAD_SIZE_MAX];
}RAR_PDU;

RAR_PDU RAR_pdu;

typedef struct 
{
	int C_RNTI; 			//Unique ID used to identify a UE
	int ra_success_flag;    // Flag :{0-Failure;1-Success}

}UE_info;


void preamble_tracker(UE_info *UE_list)
{
   /*
     A proper input has to be received from some function saying that
     RA procedure for a particular UE has been successful. 
     Currently, the structure member "ra_success_flag" does this job
   */

    for (int i = 0; i < Max_No_of_CFRA_preambles; i++)
     {
     	UE_list++;
     	
     	//if RA procedure is successfully completed for the given UE
     	if (UE_list->ra_success_flag == 1)
     	{
     		printf("UE_ID : %d ;  RA procedure complete \n",UE_list->C_RNTI );
     		//To indicate that the preamble index has been freed for re-use
     		preamble_flagger[i] = 0;
     	}
     	
     } 
}


/*========================================================================================================================*/

void preamble_assigner (UE_info *UE_list )
{
	int preamble_index;
    int i;
    preamble_tracker(UE_list);

    /* Loop to find the min {index of all free preambles}
       So that, it can be assigned to the current UE
    */
    for (i = 0; i < Max_No_of_CFRA_preambles; i++)
     {
     	if (preamble_flagger[i] == 0)
     	{
     		break;
     	}
     	
     } 

     preamble_index = 63 - i;

     //To indicate that the preamble index assigned to a new UE is under use
     preamble_flagger[i] = 1;

}
/*========================================================================================================================*/
unsigned short rar_fill(uint8_t * const dlsch_buffer,uint8_t preamble_index,uint16_t rnti,int sockfd)

{
	uint16_t timing_offset=48;
	uint16_t PUSCH_Freq=124;
	uint16_t PUSCH_Time=4;
	uint16_t MCS=2;
	uint16_t TPC=4;
	uint16_t CSI_Request=1;
	uint16_t Hopping_Flag=1;
	uint16_t T_CRNTI=300;

	
	RA_SUBHEADER_RAPID *rarh = (RA_SUBHEADER_RAPID*) dlsch_buffer;
	RA_SUBHEADER_BI *rarh1 = (RA_SUBHEADER_BI*) dlsch_buffer;

	uint8_t *rar = (uint8_t*) (dlsch_buffer +1);	
	
	
	rarh1->E = 0;
	rarh1->T = 0;
	rarh1->BI = 2;



	rarh->E = 0;
	rarh->T = 0;
	rarh->RAPID = preamble_index;



	rar[5] = (uint8_t) (T_CRNTI >> 8);
	rar[6] = (uint8_t) (T_CRNTI & 0xff);
	rar[0] = (uint8_t) (timing_offset >> 5);
	rar[1] = (uint8_t) (timing_offset << 3) & 0xf8;

	//UL GRANT

	rar[1] |= (uint8_t) (Hopping_Flag <<2);

	rar[1] |= (uint8_t) (PUSCH_Freq >>12 & 0x3);

	rar[2] = (uint8_t) (PUSCH_Freq >> 4);

	rar[3] = (uint8_t) (PUSCH_Freq << 4) & 0xf0 ;

	rar[3] |= (uint8_t) (PUSCH_Time);

	rar[4] = (uint8_t) (MCS << 4) & 0xf0;

	rar[4] |= (uint8_t) (TPC << 1) & 0x0e;

	rar[4] |= (uint8_t) (CSI_Request);





 send(sockfd, rarh, sizeof(rarh),0);

 send(sockfd,rar,sizeof(rar),0);

 send(sockfd, rarh1, sizeof(rarh1),0);

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
int main(int argc, char const *argv[])
{
    int connfd;

    connfd = create_socket();
    UE_info UE_List[Max_No_of_CFRA_preambles];
	UE_info *UE_list = UE_List;
	
    preamble_assigner(UE_list); 
	rar_fill(RAR_pdu.payload,56,122,connfd);
	close(connfd); 

	
	return 0;
}
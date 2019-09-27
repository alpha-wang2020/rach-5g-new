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


#define PORT 8004
#define SA struct sockaddr 
#define MSG3_PAYLOAD_SIZE_MAX 10

uint8_t ul_payload_recvd[MSG3_PAYLOAD_SIZE_MAX];
void send_cont_resolution_id_ce();

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

void send_cont_resolution_id_ce(){
	
	
	
	printf("\nRECEIVED PAYLOAD\n");
	
	for (int i = 0; i < MSG3_PAYLOAD_SIZE_MAX; ++i)
	 	{
	 		 printf("0x%02x|",ul_payload_recvd[i]); 
	 	}

	uint8_t mac_ce_cont_res_iD[6];

	uint64_t random_num; // 39 bit random number from UE (ue_identity)

	MAC_CE_Cont_Res_ID *cont_res_id=calloc(1,sizeof(MAC_CE_Cont_Res_ID));
	
	if (ul_payload_recvd[0]==0X34)
	{
		printf("\nreceived UL_CCCH_SDU : Subhdr with LCID =52\n");   	// chechking the LCID in subhdr
	}


	//printf("\n%lu\n",(uint64_t)ul_payload_recvd[2] );

	for (int i = 0; i <6 ; i++)
	{
		
		mac_ce_cont_res_iD[i]=ul_payload_recvd[i+1];
		
		cont_res_id->UE_Cont_Res_ID |= (uint64_t)ul_payload_recvd[i+1] << (48-(i+1)*8);
		printf("number time\n");

	}

	for (int i = 0; i < 5; ++i)
	{
		printf("%d\n",(40-(i+1)*8) );
		random_num |= (uint64_t)ul_payload_recvd[i+1] << (40-(i+1)*8);
	}
	
	/*cont_res_id.UE_Cont_Res_ID=(uint64_t)ul_payload_recvd[1] << 32;
	cont_res_id.UE_Cont_Res_ID|= (uint64_t)ul_payload_recvd[2] << 24;
	cont_res_id.UE_Cont_Res_ID|= (uint64_t)ul_payload_recvd[3] << 16;
	cont_res_id.UE_Cont_Res_ID|= (uint64_t)ul_payload_recvd[4] << 8;
	cont_res_id.UE_Cont_Res_ID|= (uint64_t)ul_payload_recvd[5];*/
	
	
	printf("UE_IDENTITY: %lx \n", random_num); // to do: SOME ERROR TO BE CORRECTED IN LAST BIT (pRINTING JUST FOR REFERENCE) CCCH_SDU Random number from UE
	printf("cont_res_id : 48 bit %lx \n",cont_res_id->UE_Cont_Res_ID);

	printf("MAC_CE_Cont_Res_ID TO BE SENT TO UE\n");
	for (int i = 0; i < 6; ++i)
	 	{
	 		 printf("0x%02x|",mac_ce_cont_res_iD[i]); 
	 	}


}



int main(int argc, char const *argv[])
{
	int connfd;

	connfd =  create_socket();

	recv(connfd,ul_payload_recvd,sizeof(ul_payload_recvd),0);



	send_cont_resolution_id_ce();

	return 0;
}
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
#include<stdint.h>
#include<inttypes.h>
#include <math.h>
#include "RRCSetupRequest.h"
#include "pdu.h"
#include "UL-CCCH-Message.h"

#define MSG3_PAYLOAD_SIZE_MAX 10
uint8_t ul_payload[MSG3_PAYLOAD_SIZE_MAX];
#define PORT 8004
#define SA struct sockaddr 



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
 	
 	int sockfd;
 	sockfd = create_socket();
 	srand(time(0));
 	uint64_t a =pow(2,39);
 	a = a-1;
 	//printf("\nto check 1: %lx\n",a);
 	uint64_t ue_identity =  rand()%a  ; // Integer value in the range 0 to 2^39 - 1. for fututre reference (CBRA)
	
	printf("\nto check : 0x%lx\n",ue_identity);
 	
 	UL_CCCH_Message_t *ul_ccch_message=(UL_CCCH_Message_t *)malloc(sizeof(UL_CCCH_Message_t));
 	
 	MAC_Sbhr_R_LCID mac_sbhr;

 	ul_ccch_message->message.choice.c1.choice.rrcSetupRequest.rrcSetupRequest.ue_Identity.choice.randomValue.buf = calloc (5,sizeof(uint8_t));
 	ul_ccch_message->message.choice.c1.choice.rrcSetupRequest.rrcSetupRequest.ue_Identity.choice.randomValue.size= 39;
 	ul_ccch_message->message.choice.c1.choice.rrcSetupRequest.rrcSetupRequest.ue_Identity.choice.randomValue.buf[0]=(uint8_t) (ue_identity >> 32) & 0x7f;
 	ul_ccch_message->message.choice.c1.choice.rrcSetupRequest.rrcSetupRequest.ue_Identity.choice.randomValue.buf[1]=(uint8_t) (ue_identity >> 24) & 0xff;
 	ul_ccch_message->message.choice.c1.choice.rrcSetupRequest.rrcSetupRequest.ue_Identity.choice.randomValue.buf[2]=(uint8_t) (ue_identity >> 16) & 0xff;
 	ul_ccch_message->message.choice.c1.choice.rrcSetupRequest.rrcSetupRequest.ue_Identity.choice.randomValue.buf[3]=(uint8_t) (ue_identity >> 8) & 0xff;
 	ul_ccch_message->message.choice.c1.choice.rrcSetupRequest.rrcSetupRequest.ue_Identity.choice.randomValue.buf[4]=(uint8_t) ue_identity & 0xff;
 	ul_ccch_message->message.choice.c1.choice.rrcSetupRequest.rrcSetupRequest.ue_Identity.choice.randomValue.bits_unused=1;

 	int establishmentCause = rand()%15; // TO DO : enum data  

 		printf("ccch sdu\n");
 		for (int i = 0; i < 5; ++i)
	 	{
	 		 printf("0x%02x|",ul_ccch_message->message.choice.c1.choice.rrcSetupRequest.rrcSetupRequest.ue_Identity.choice.randomValue.buf[i]); 
	 	}
	 	
	 	
	 	
	 	mac_sbhr.LCID = 52;
	 	ul_payload[0] = mac_sbhr.LCID;
	 	
	 	for (int i = 0; i < 5; ++i)
	 	{
	 		 ul_payload[i+1]=ul_ccch_message->message.choice.c1.choice.rrcSetupRequest.rrcSetupRequest.ue_Identity.choice.randomValue.buf[i]; 
	 	}


	 	ul_payload[6] = establishmentCause ;

	 	

	 	printf("\nUL PAYLOAD WITH UL CCCH SDU (CASE: RRC SET-UP REQUEST)\n");
	 	for (int i = 0; i < MSG3_PAYLOAD_SIZE_MAX; ++i)
	 	{
	 		 printf("0x%02x|",ul_payload[i]); 
	 	}
	 	
	 send(sockfd,ul_payload,sizeof(ul_payload),0);	
 	return 0;
 } 
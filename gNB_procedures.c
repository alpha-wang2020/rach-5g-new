
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
#define RAR_PAYLOAD_SIZE_MAX 128
#define MAX 80 
#define PORT 8021
#define SA struct sockaddr 


typedef struct {
    uint8_t payload[RAR_PAYLOAD_SIZE_MAX];
}RAR_PDU;

RAR_PDU RAR_pdu;


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

 printf("%02x\n",rarh->RAPID);
 printf("Received RAR (%02x|%02x.%02x.%02x.%02x.%02x.%02x.%02x) for preamble %d/%d\n",
          *(uint8_t *) rarh, rar[0], rar[1], rar[2],
          rar[3], rar[4], rar[5], rar[6], rarh->RAPID, preamble_index);



 send(sockfd, rarh, sizeof(rarh),0);

 send(sockfd,rar,sizeof(rar),0);

 send(sockfd, rarh1, sizeof(rarh1),0);




}

int main(int argc, char const *argv[])
{

	int sockfd, connfd, len; 
	struct sockaddr_in servaddr, cli; 

	sockfd = socket(AF_INET, SOCK_STREAM, 0); 

	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
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
	len = sizeof(cli); 

	connfd = accept(sockfd, (SA*)&cli, &len); 
	if (connfd < 0) { 
		printf("server acccept failed...\n"); 
		exit(0); 
	} 
	else
		printf("server acccept the client...\n"); 

	rar_fill(RAR_pdu.payload,56,122,connfd);

	close(sockfd); 

	
	return 0;
}
/*==============================================================
Title        : Implementation of Random Access procedure in UE 
Project      : L2 protocol stack development for 5G Testbed project
Organization : IIT Madras
Author       : Pranav Kumar
Standards    : TS 38.321 , TS 38.331 [v15.5.0 Rel 15]
================================================================*/

/*
NOTE:- 
This is a temporary test file which initializes the 
parameters required for Random Access Procedure.

Ideally ,this configuration of parameters should be obtained from 
RRC of gNB and given to RRC of UE, which configures UE MAC.

INPUT :- Section 5.1.1
*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<time.h>
#include<math.h>
#include<error.h>
#include<errno.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netdb.h>
//#include"SubcarrierSpacing.h"
//#include"RACH-ConfigCommon.h"
//#include"BeamFailureRecoveryConfig.h"
#include"RACH-ConfigDedicated.h"

#define PORT 8085
#define SA struct sockaddr 

/*================================================================================*/
/*  create_socket()
Brief :- This routine is used to create socket to transmit information to UE
NOTE :- Socket creation - Temporary. Remove during integration

*/
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
		printf("server acccept the UE_addr...\n"); 

	return connfd;  
}
/*================================================================================*/
/* ra_initialize()
   Brief :- This routine is used to initialize parameters for Random Access procedure
   NOTE :- Currently assuming CFRA. For CBRA, will be updated soon (on 17th July)
   CFRA => RACHConfigDedicated IE


void ra_initialize( uint8_t prach_ConfigIndex,
				    int preamble_ReceivedTargetPower,
				    uint8_t rsrp_thresholdCSI_RS,
				    )
{

}
*/


int main(int argc, char const *argv[])
{   
	srand (time (NULL));
	int connfd;
    RACH_ConfigDedicated_t *dedicated = (RACH_ConfigDedicated_t *) calloc(1,sizeof(RACH_ConfigDedicated_t));
	//connfd = create_socket();
	dedicated->cfra = (CFRA_t *) calloc(1,sizeof(CFRA_t)); 
	dedicated->cfra->occasions = (struct CFRA__ext1 *) calloc(1,sizeof(struct CFRA__ext1)) ;
	
	dedicated->cfra->occasions->rach_ConfigGeneric.prach_ConfigurationIndex = rand()%256;
    
    

	return 0;
}

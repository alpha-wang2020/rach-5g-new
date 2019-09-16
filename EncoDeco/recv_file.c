/*Name : Jay Jagtap
3154037
Problem Statement: To achieve File transfer using TCP/IP Protocol
*/

/*
	Server Side
	Please pass port no as command line argument
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include<error.h>
#include<errno.h>
#include <ctype.h>
#include <fcntl.h>

#define PORT 8004
#define SA struct sockaddr 
#define FILE_SIZE 105  



int main(int argc, char *argv[])
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

    
          
  unsigned char buffer[ FILE_SIZE];
    FILE *fp;
         //int ch = 0;
           FILE *write_ptr;

            write_ptr = fopen("test.bin","wb");  // w for write, b for binary
            recv(connfd , buffer , sizeof(buffer),0); 
            //for(int i = 0; i< FILE_SIZE; i++)
            //printf("%x ", buffer[i]);
            fwrite(buffer,sizeof(buffer),1,write_ptr); // write 10 bytes from our buffer
          
          
            //printf("Passed integer is : %d\n" , words);      //Ignore , Line for Testing
         /* while(ch != 512)
           {
           recv(connfd , buffer , sizeof(buffer),0); 
       fprintf(fp , " %d" , buffer);   
     printf(" %d %d "  , buffer , ch); //Line for Testing , Ignore
     ch++;
     }*/
      printf("The file was received successfully\n");
    // printf("The new file created is glad5.txt");
     close(connfd);
     close(sockfd);
     return 0; 
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include<error.h>
#include<errno.h>
#include<ctype.h>



#include <arpa/inet.h>
#include <netdb.h> 




#include<stdint.h>


#define PORT 8004
#define SA struct sockaddr
#define FILE_SIZE 105  
int main(int argc, char *argv[])
{
   

    //int buffer[SIZE_max];
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
        
   int i;
    unsigned char buffer[FILE_SIZE];
    FILE *ptr;

    ptr = fopen("berout","rb");  // r for read, b for binary

    fread(buffer,sizeof(buffer),1,ptr); // read 10 bytes to our buffer
    //for(int i = 0; i<FILE_SIZE; i++)
    //printf("%x ", buffer[i]);
    send(sockfd,buffer,sizeof(buffer),0);

   
            /*      fseek(f, 0L, SEEK_END);     // tells size of the file. Not rquired for the functionality in code.
  int sz = ftell(f);        //Just written for curiosity.
  printf("Size is %d \n" , sz);
          rewind(f);  
            */
       
  /*  int ch ;
       while(ch != EOF)
      {
    
    fscanf(f , "%d" , buffer);
    printf("%d\n" , buffer);  //Ignore
    
    ch = fgetc(f);
      }*/
  printf("The file was sent successfully");
    
    close(sockfd);
    return 0;
}

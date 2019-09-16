#include <netdb.h> 
#include <unistd.h>
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <fcntl.h> 
#define MAX 80 
#define PORT 1234 
#define SA struct sockaddr 

struct MAC_subheader_CE
{
    //uint8_t reserve: 2;         
   // uint8_t LCID: 6;           
    uint8_t RN[6];

}MAC_Sbhr_CE;


struct MAC_subheader_LCID_CE
{
    uint8_t reserve: 2;         // R bit - set to 0
    uint8_t LCID: 6;           //Logical Channel ID
    uint8_t MAC_CE[8];

}MAC_Sbhr_LCID_CE;



int func(int sockfd)
{

    //Random number generation in CE
    for(int i = 0; i < 6; i++) 
     {   MAC_Sbhr_CE.RN[i]=rand();
        printf("%02x\n",MAC_Sbhr_CE.RN[i]); 
     }
    
    size_t ret=send(sockfd,&MAC_Sbhr_CE,sizeof(MAC_Sbhr_CE),0);
    if(ret==-1){
        printf("error in sending the packet\n");
        return 0;
    }
    else
        printf("no of bytes sent:%lu\n",ret);
    


    size_t ret1=recv(sockfd,&MAC_Sbhr_LCID_CE,sizeof(MAC_Sbhr_LCID_CE),0);
    if(ret1==-1){
        printf("error in sending the packet\n");
        return 0;
    }
    else
        printf("no of bytes received:%lu\n",ret1-1);


    printf("\nReserve:");
    printf("%" SCNu8,MAC_Sbhr_LCID_CE.reserve );
    printf("\nLCID:");
    printf("%" SCNu8,MAC_Sbhr_LCID_CE.LCID );
    printf("\nMAC_CE:");
    printf("%s",MAC_Sbhr_LCID_CE.MAC_CE );

    if(MAC_Sbhr_LCID_CE.LCID==62)
        printf("A wins the Contention \n");


} 
  
void main() 
{ 
   
//................................................................
    int sockfd, connfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(PORT); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0); 
    } 
    else
        printf("connected to the server..\n"); 
//.....................................................................    
    // function for chat 
    func(sockfd); 
  
    // close the socket 
    close(sockfd); 
} 

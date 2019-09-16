
#include <netdb.h> 
#include <unistd.h>
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <stdio.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <fcntl.h>  
#define MAX 80 
#define PORT 1234
#define SA struct sockaddr 



struct MAC_subheader_CE
{
   //uint8_t reserve: 2;         // R bit - set to 0
   // uint8_t LCID: 6;           //Logical Channel ID
    uint8_t RN[6];

}MAC_Sbhr_CE;


struct MAC_subheader_LCID_CE
{
    uint8_t reserve: 2;         // R bit - set to 0
    uint8_t LCID: 6;           //Logical Channel ID
    uint8_t MAC_CE[8];

}MAC_Sbhr_LCID_CE;


// Function designed for chat between client and server. 
int func(int sockfd) 
{ 
    size_t ret=recv(sockfd,&MAC_Sbhr_CE,sizeof(MAC_Sbhr_CE),0);
    if(ret==-1){
        printf("error in sending the packet\n");
        return 0;
    }
    else
        printf("no of bytes received:%lu\n",ret);
//to check pritning
    for (int i=0;i<6;i++)
    printf("Received RN: %02x \n",MAC_Sbhr_CE.RN[i]);
    

    MAC_Sbhr_LCID_CE.LCID = 62;
    MAC_Sbhr_LCID_CE.reserve=0;
    MAC_Sbhr_LCID_CE.MAC_CE[0]=0;
    MAC_Sbhr_LCID_CE.MAC_CE[1]=0;
    
    printf("%c",MAC_Sbhr_LCID_CE.MAC_CE[0]);
    printf("\n %c",MAC_Sbhr_LCID_CE.MAC_CE[1]);
    for(int i=2;i<=8;i++)
    {
        MAC_Sbhr_LCID_CE.MAC_CE[i]=MAC_Sbhr_CE.RN[i];
         printf("%c",MAC_Sbhr_LCID_CE.MAC_CE[i]);
    }

    // Storing 48 Bits in 64bit array
    size_t ret1=send(sockfd,&MAC_Sbhr_LCID_CE,sizeof(MAC_Sbhr_LCID_CE),0);
    if(ret1==-1){
        printf("error in sending the packet\n");
        return 0;
       }
    else
        printf("no. of bytes sent:%lu\n",ret1-1);

    



} 
  
// Driver function 
int main() 
{ 

//............................................................................    
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification 
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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
  
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    len = sizeof(cli); 
  
    // Accept the data packet from client and verification 
    connfd = accept(sockfd, (SA*)&cli, &len); 
    if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server acccept the client...\n"); 

//...........................................................
  


    // Function for chatting between client and server 
    func(connfd); 
  
    // After chatting close the socket 
    close(sockfd); 
} 


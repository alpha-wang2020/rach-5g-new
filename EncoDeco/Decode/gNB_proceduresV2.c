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
#define PORT 8004
#define SA struct sockaddr 
#include"RACH-ConfigDedicated.h"

int preamble_flagger[10] = {0};
int Max_No_of_CFRA_preambles = 10;
int preamble_index;

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
/*========================================================================================================================
						RACH CONFIG DEDICATED ASSIGNMENT
========================================================================================================================*/
/*
 * This is a custom function which writes the
 * encoded output into some FILE stream.
 */
static int
write_out(const void *buffer, size_t size, void *app_key) {
    FILE *out_fp = app_key;
    size_t wrote;
 
    wrote = fwrite(buffer, 1, size, out_fp);
 
    return (wrote == size) ? 0 : -1;
}

int cfrafill(int ac, char **av)
{
    RACH_ConfigDedicated_t *rach_ConfigDedicated;
    asn_enc_rval_t ec;
    CFRA_CSIRS_Resource_t * cfra_csirs_resources;
    long Msg1FDM = 0;
    long preambletransmax = 2;
    long powerrampingstep = 2;
    long raresponsewindow = 3;
    long temp =2;
    int ret;
    long *temp2=&temp;
    rach_ConfigDedicated = calloc(1,sizeof(RACH_ConfigDedicated_t));
    if(!rach_ConfigDedicated) {
      perror("calloc() failed");
      exit(71); /* better, EX_OSERR */
    }

    rach_ConfigDedicated->cfra = (CFRA_t *)calloc(1,sizeof(CFRA_t));

    rach_ConfigDedicated->ra_Prioritization = (RA_Prioritization_t *)calloc(1,sizeof(RA_Prioritization_t));

    if(!rach_ConfigDedicated->cfra) {
      perror("calloc() failed");
      exit(71); /* better, EX_OSERR */
    }
  
    if(!rach_ConfigDedicated->ra_Prioritization) {
      perror("calloc() failed");
      exit(71);
    }



   rach_ConfigDedicated->cfra->occasions =(struct CFRA__occasions *)calloc(1,sizeof(struct CFRA__occasions));
    if(!rach_ConfigDedicated->cfra->occasions){
        perror("calloc() failed for occasions");
        exit(71);
    }
    
    /* RACH CONFIG GENERIC ASSIGNING */
    rach_ConfigDedicated->cfra->occasions->rach_ConfigGeneric.prach_ConfigurationIndex =12;
    rach_ConfigDedicated->cfra->occasions->rach_ConfigGeneric.msg1_FDM = 0;
    rach_ConfigDedicated->cfra->occasions->rach_ConfigGeneric.msg1_FrequencyStart = 0;
    rach_ConfigDedicated->cfra->occasions->rach_ConfigGeneric.zeroCorrelationZoneConfig=5;

    rach_ConfigDedicated->cfra->occasions->rach_ConfigGeneric.preambleReceivedTargetPower =56;
    rach_ConfigDedicated->cfra->occasions->rach_ConfigGeneric.preambleTransMax = 3;
    rach_ConfigDedicated->cfra->occasions->rach_ConfigGeneric.powerRampingStep = 2;
    rach_ConfigDedicated->cfra->occasions->rach_ConfigGeneric.ra_ResponseWindow = 2;
    rach_ConfigDedicated->cfra->occasions->ssb_perRACH_Occasion=&temp;
    rach_ConfigDedicated->ra_Prioritization->powerRampingStepHighPriority= 1; 
    rach_ConfigDedicated->ra_Prioritization->scalingFactorBI = &temp;
    rach_ConfigDedicated->cfra->resources.present = CFRA__resources_PR_csirs;
    rach_ConfigDedicated->cfra->resources.choice.csirs.rsrp_ThresholdCSI_RS = 9;
    
    cfra_csirs_resources = calloc(1,sizeof(*cfra_csirs_resources));
    cfra_csirs_resources->ra_PreambleIndex = 65;
    cfra_csirs_resources->csi_RS = 1;
//   *( cfra_csirs_resources->ra_OccasionList.list.array[0]) = 1;
    ret = ASN_SEQUENCE_ADD(&cfra_csirs_resources->ra_OccasionList, temp2);
    assert(ret==0);
      
   ret= ASN_SEQUENCE_ADD(&rach_ConfigDedicated->cfra->resources.choice.csirs.csirs_ResourceList, cfra_csirs_resources);
    assert(ret==0);
  
    cfra_csirs_resources = calloc(1,sizeof(*cfra_csirs_resources));
    cfra_csirs_resources->ra_PreambleIndex = preamble_index;
    cfra_csirs_resources->csi_RS = 2;
    temp =3;
    ret = ASN_SEQUENCE_ADD(&cfra_csirs_resources->ra_OccasionList,temp2);
    assert(ret==0);
 
    ret= ASN_SEQUENCE_ADD(&rach_ConfigDedicated->cfra->resources.choice.csirs.csirs_ResourceList, cfra_csirs_resources);
    assert(ret==0);
    cfra_csirs_resources = calloc(1,sizeof(*cfra_csirs_resources));
    cfra_csirs_resources->ra_PreambleIndex = 85;
    cfra_csirs_resources->csi_RS = 3;
    temp =4;
    ret = ASN_SEQUENCE_ADD(&cfra_csirs_resources->ra_OccasionList,temp2);
    assert(ret==0);
 
  ret= ASN_SEQUENCE_ADD(&rach_ConfigDedicated->cfra->resources.choice.csirs.csirs_ResourceList, cfra_csirs_resources);
    assert(ret==0);
   cfra_csirs_resources = calloc(1,sizeof(*cfra_csirs_resources));
    cfra_csirs_resources->ra_PreambleIndex = preamble_index;
    cfra_csirs_resources->csi_RS = 4;
    temp =5;
    ret = ASN_SEQUENCE_ADD(&cfra_csirs_resources->ra_OccasionList,temp2);
    assert(ret==0);
 
  ret= ASN_SEQUENCE_ADD(&rach_ConfigDedicated->cfra->resources.choice.csirs.csirs_ResourceList, cfra_csirs_resources);
    assert(ret==0);
  


 



     /* BER encode the data if filename is given */
    if(ac < 2) {
      fprintf(stderr,"Specify filename for BER output\n");
    } else {
      const char *filename = av[1];
      FILE *fp = fopen(filename, "wb");   /* for BER output */
 
      if(!fp) {
        perror(filename);
        exit(71); /* better, EX_OSERR */
      }
  
      /* Encode the Rectangle type as BER (DER) */
      ec = der_encode(&asn_DEF_RACH_ConfigDedicated,
            rach_ConfigDedicated, write_out, fp);
      fclose(fp);
      if(ec.encoded == -1) {
        fprintf(stderr,
         "Could not encode Rectangle (at %s)\n",
          ec.failed_type ? ec.failed_type->name : "unknown");
        exit(65); /* better, EX_DATAERR */
      } else {
        fprintf(stderr, "Created %s with BER encoded Rectangle\n",
          filename);
      }
    }
 
    /* Also print the constructed Rectangle XER encoded (XML) */
    xer_fprint(stdout, &asn_DEF_RACH_ConfigDedicated, rach_ConfigDedicated);
    

   printf("the valure in ra prit = %ld",rach_ConfigDedicated->ra_Prioritization->powerRampingStepHighPriority); 


        return 0;
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

 printf("%02x\n",rarh->RAPID);
 printf("Received RAR (%02x|%02x.%02x.%02x.%02x.%02x.%02x.%02x) for preamble %d/%d\n",
          *(uint8_t *) rarh, rar[0], rar[1], rar[2],
          rar[3], rar[4], rar[5], rar[6], rarh->RAPID, preamble_index);



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
int main(int ac, char **av)
{
    int connfd;

    connfd = create_socket();
    UE_info UE_List[Max_No_of_CFRA_preambles];
	UE_info *UE_list = UE_List;

    preamble_assigner(UE_list); 
   int abc= cfrafill(ac,av);
	rar_fill(RAR_pdu.payload,56,122,connfd);
	close(connfd); 

	
	return 0;
}

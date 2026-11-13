#include "tftp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void handle_client(int sockfd,struct sockaddr_in client_addr,socklen_t client_len,tftp_packet *packet);

int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    socklen_t client_len =sizeof(client_addr);


    tftp_packet packet;

    sockfd = socket(AF_INET,SOCK_DGRAM,0);

    if(sockfd < 0)
    {
	perror("Socket creation failed");
	return -1;
    }


    server_addr.sin_family = AF_INET;

    server_addr.sin_port =htons(PORT);

    server_addr.sin_addr.s_addr =INADDR_ANY;

    if(bind(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
    {
	perror("Bind failed");
	return -1;
    }
    printf("TFTP Server listening on port %d...\n",
	    PORT);
    while(1)
    {

	int n;
	n = recvfrom(sockfd,&packet,sizeof(packet),0,(struct sockaddr *)&client_addr,&client_len);

	if(n < 0)
	{
	    continue;
	}

	handle_client(sockfd,client_addr,client_len,&packet);

    }

    close(sockfd);
    return 0;

}


void handle_client(int sockfd,struct sockaddr_in client_addr,socklen_t client_len,tftp_packet *packet)
{

    int opcode;
    opcode = ntohs(packet->opcode);
    int mode;
    if(strcmp(packet->body.request.mode,"normal") == 0)
    {
	mode = NORMAL_MODE;
    }
    else
    {
	mode = OCTET_MODE;
    }

    printf("\nTRANSFER MODE = %s",
       (mode == NORMAL_MODE) ? "NORMAL" : "OCTET");

    if(opcode == RRQ)
    {

	printf("\nRRQ received\n");
	printf("File : %s\n",packet->body.request.filename);

	int fd =access(packet->body.request.filename,F_OK);

	if(fd != 0)
	{

	    printf("File not found\n");
	    send_error(sockfd,client_addr,client_len,"File not found");
	    return;

	}
	send_file(sockfd,client_addr,client_len,packet->body.request.filename,mode);

    }
    else if(opcode == WRQ)
    {

	printf("\nWRQ received\n");
	printf("File : %s\n",packet->body.request.filename);
	receive_file(sockfd,client_addr,client_len,packet->body.request.filename,mode);
    }
    else
    {
	printf("Invalid request received\n");

    }
}

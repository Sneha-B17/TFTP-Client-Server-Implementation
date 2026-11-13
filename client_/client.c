#include "tftp.h"
#include "client.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


int main()
{
    tftp_client_t client;

    memset(&client, 0, sizeof(client));

    client.connected = 0;

    // Default mode
    client.mode = NORMAL_MODE;

    int choice;
    while(1)
    {
        show_menu();

        printf("Enter choice : ");
        scanf("%d",&choice);

        process_choice(&client, choice);
    }
    return 0;
}

/*display menu*/
void show_menu()
{
    printf("\n=============================\n");
    printf("        TFTP CLIENT\n");
    printf("=============================\n");

    printf("1. Connect\n");
    printf("2. Put\n");
    printf("3. Get\n");
    printf("4. Mode\n");
    printf("5. Exit\n");

    printf("=============================\n");
}

/**/
void process_choice(tftp_client_t *client, int choice)
{

    char ip[50];
    char filename[100];

    switch(choice)
    {
        case 1:

            printf("Enter server IP : ");
            scanf("%s",ip);

            connect_to_server(client,ip,PORT);
            break;

        case 2:

            if(client->connected == 0)
            {
                printf("Please connect to server first\n");
                break;
            }
            printf("Enter filename to upload : ");
            scanf("%s",filename);

            put_file(client,filename);
            break;

        case 3:

            if(client->connected == 0)
            {
                printf("Please connect to server first\n");
                break;
            }
            printf("Enter filename to download : ");
            scanf("%s",filename);

            get_file(client,filename);
            break;

        case 4:

            printf("\nSelect Transfer Mode\n");
            printf("1. Normal Mode (512 bytes)\n");
            printf("2. Octet Mode  (1 byte)\n");
            printf("Enter mode : ");

            int mode;
            scanf("%d",&mode);
	    /*NORMAL MODE*/
            if(mode == 1)
            {
                client->mode = NORMAL_MODE;

                printf("Normal Mode selected\n");
            }

	    /*OCTET MODE*/
            else if(mode == 2)
            {
                client->mode = OCTET_MODE;

                printf("Octet Mode selected\n");
            }

            else
            {
                printf("Invalid mode\n");
            }
            break;

        case 5:

            disconnect(client);
            exit(0);

        default:

            printf("Invalid choice\n");

    }

}

/*connection between server-client*/
void connect_to_server(tftp_client_t *client, char *ip, int port)
{

    client->server_addr.sin_family = AF_INET;
    client->server_addr.sin_port = htons(port);
    inet_pton(AF_INET,ip,&client->server_addr.sin_addr);

    client->server_len =sizeof(client->server_addr);

    client->sockfd =socket(AF_INET,SOCK_DGRAM,0);

    if(client->sockfd < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    strcpy(client->server_ip,ip);

    client->connected = 1;

    printf("Connected to server %s\n",ip);

}

/*PUT OPEARTION*/
void put_file(tftp_client_t *client, char *filename)
{

    tftp_packet packet,ack;

    socklen_t len =sizeof(client->server_addr);

    packet.opcode = htons(WRQ);

    strcpy(packet.body.request.filename,filename);

    if(client->mode == NORMAL_MODE)
     	strcpy(packet.body.request.mode,"normal");
    else
        strcpy(packet.body.request.mode,"octet");

    sendto(client->sockfd,&packet,sizeof(packet),0,(struct sockaddr *)&client->server_addr,len);

    recvfrom(client->sockfd,&ack,sizeof(ack),0,(struct sockaddr *)&client->server_addr,&len);

    send_file(client->sockfd,client->server_addr,len,filename,client->mode);
}

/*GET OPEARTION*/
void get_file(tftp_client_t *client,char *filename)
{
    send_request(client->sockfd, client->server_addr,filename,RRQ,client->mode);

    receive_request(client->sockfd,client->server_addr,filename,RRQ,client->mode);

}

/*TO DISCONNECT*/
void disconnect(tftp_client_t *client)
{

    close(client->sockfd);

    client->connected = 0;

    printf("Disconnected\n");

}

/*SEND REQ*/
void send_request(int sockfd,struct sockaddr_in server_addr,char *filename,int opcode,int mode)
{

    tftp_packet packet;
    packet.opcode = htons(opcode);
    strcpy(packet.body.request.filename,filename);

    if(mode == NORMAL_MODE)
        strcpy(packet.body.request.mode,"normal");
    else
        strcpy(packet.body.request.mode,"octet");

    sendto(sockfd,&packet,sizeof(packet),0,(struct sockaddr *)&server_addr,sizeof(server_addr));

}

/*RECEIVE REQ*/
void receive_request(int sockfd,struct sockaddr_in server_addr,char *filename,int opcode,int mode)
{

    tftp_packet packet,ack;
    socklen_t len =sizeof(server_addr);
    int fd;

    fd=open(filename,O_WRONLY|O_CREAT|O_TRUNC,0666);

    while(1)
    {

        int n =recvfrom(sockfd,&packet,sizeof(packet),0,(struct sockaddr *)&server_addr,&len);

        write(fd,packet.body.data_packet.data,n-4);

        ack.opcode = htons(ACK);

        ack.body.ack_packet.block_number =packet.body.data_packet.block_number;

        sendto(sockfd,&ack,sizeof(ack),0,(struct sockaddr *)&server_addr,len);

        if(mode == NORMAL_MODE && n < 516)
            break;
        if(mode == OCTET_MODE && n < 5)
            break;
    }
    close(fd);
    printf("File downloaded successfully\n");

}

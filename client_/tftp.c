/* Common file for server & client */
#include "tftp.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

void send_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename, int mode)
{

    int fd = open(filename, O_RDONLY);
    if(fd < 0)
    {
        perror("File open failed");
        return;
    }

    tftp_packet packet, ack;
    int block = 1;
    int n;
    int size = (mode == NORMAL_MODE) ? NORMAL_BLOCK_SIZE : OCTET_BLOCK_SIZE;

    while(1)
    {
        n = read(fd, packet.body.data_packet.data, size);

        if(n < 0)
        {
            perror("read");
            break;
        }

        packet.opcode = htons(DATA);
        packet.body.data_packet.block_number = htons(block);

        sendto(sockfd, &packet, n + 4, 0,
               (struct sockaddr *)&client_addr, client_len);

        recvfrom(sockfd, &ack, sizeof(ack), 0,
                 (struct sockaddr *)&client_addr, &client_len);

        /* last packet */
        if(n < size)
            break;

        block++;
    }

    close(fd);
    printf("File sent successfully\n");
}


void receive_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename, int mode)
{
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if(fd < 0)
    {
        perror("File creation failed");
        return;
    }

    tftp_packet packet, ack;
    int n;
    int size = (mode == NORMAL_MODE) ? NORMAL_BLOCK_SIZE : OCTET_BLOCK_SIZE;

    ack.opcode = htons(ACK);
    ack.body.ack_packet.block_number = htons(0);

    sendto(sockfd, &ack, 4, 0,
           (struct sockaddr *)&client_addr, client_len);

    while(1)
    {
        n = recvfrom(sockfd, &packet, sizeof(packet), 0,
                     (struct sockaddr *)&client_addr, &client_len);

        if(n < 4)
            continue;

        if(ntohs(packet.opcode) != DATA)
            continue;

        int data_size = n - 4;

        write(fd, packet.body.data_packet.data, data_size);

        ack.opcode = htons(ACK);
        ack.body.ack_packet.block_number =
            packet.body.data_packet.block_number;

        sendto(sockfd, &ack, 4, 0,
               (struct sockaddr *)&client_addr, client_len);

        if(data_size < size)
            break;
    }

    close(fd);

    printf("File received successfully\n");
}
     


void send_error(int sockfd,
                struct sockaddr_in client_addr,
                socklen_t client_len,
                char *message)
{

    tftp_packet packet;
    packet.opcode = htons(ERROR);
    packet.body.error_packet.error_code =htons(1);
    strcpy(packet.body.error_packet.error_msg,message);

    sendto(sockfd,&packet,sizeof(packet),0,(struct sockaddr *)&client_addr,client_len);
}

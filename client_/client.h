#ifndef TFTP_CLIENT_H
#define TFTP_CLIENT_H

#include "tftp.h"


typedef struct
{
    int sockfd;

    struct sockaddr_in server_addr;

    socklen_t server_len;

    char server_ip[INET_ADDRSTRLEN];

    int connected;

    int mode;


} tftp_client_t;



void connect_to_server(tftp_client_t *client,
                       char *ip,
                       int port);


void put_file(tftp_client_t *client,
              char *filename);


void get_file(tftp_client_t *client,
              char *filename);


void disconnect(tftp_client_t *client);



void show_menu();


void process_choice(tftp_client_t *client,
                    int choice);




void send_request(int sockfd,
                  struct sockaddr_in server_addr,
                  char *filename,
                  int opcode,
                  int mode);



void receive_request(int sockfd,
                     struct sockaddr_in server_addr,
                     char *filename,
                     int opcode,
                     int mode);



#endif

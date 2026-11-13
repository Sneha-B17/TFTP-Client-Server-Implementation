/* Common file for server & client*/

#ifndef TFTP_H
#define TFTP_H

#include <stdint.h>
#include <arpa/inet.h>

#define PORT 6969

#define BUFFER_SIZE 516  // TFTP data packet size (512 bytes data + 4 bytes header)

#define NORMAL_MODE 0
#define OCTET_MODE 1

#define NORMAL_BLOCK_SIZE 512
#define OCTET_BLOCK_SIZE 1

// TFTP OpCodes
typedef enum {
    RRQ = 1,  // Read Request
    WRQ = 2,  // Write Request
    DATA = 3, // Data Packet
    ACK = 4,  // Acknowledgment
    ERROR = 5 // Error Packet
} tftp_opcode;

// TFTP Packet Structure
typedef struct {
    uint16_t opcode; // Operation code (RRQ/WRQ/DATA/ACK/ERROR)

    union {
	/*RRQ/WRQ PACKET*/
        struct {
            char filename[256];
            char mode[20];
        } request;

	/*DATA PACKET*/
        struct {
            uint16_t block_number;
            char data[512];
        } data_packet;

	/*ACK PACKET*/
        struct {
            uint16_t block_number;
        } ack_packet;

	/*ERROR PACKET*/
        struct {
            uint16_t error_code;
            char error_msg[512];
        } error_packet;

    } body;
} tftp_packet;

void send_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename,int mode);
void receive_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename,int mode);
void send_error(int sockfd,struct sockaddr_in client_addr,socklen_t client_len,char*message);


#endif


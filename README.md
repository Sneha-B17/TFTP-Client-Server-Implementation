# TFTP – Client Server File Transfer

## Project Overview
This project implements a Trivial File Transfer Protocol (TFTP) based file transfer application using C programming and UDP socket programming in Linux.

It follows a client-server architecture to perform file upload and download operations over a network.

## Technologies Used
- C Programming
- Linux
- UDP Socket Programming
- TFTP Protocol
- Linux System Calls

## Features
- Client-server communication using UDP.
- File upload using PUT operation.
- File download using GET operation.
- Read Request (RRQ).
- Write Request (WRQ).
- Data (DATA) packet transmission.
- Acknowledgement (ACK) handling.
- Error (ERROR) packet handling.
- File handling using Linux system calls.
- Server communication using port 6969.

## TFTP Packet Types
| Opcode | Description |
|---|---|
| RRQ | Read Request |
| WRQ | Write Request |
| DATA | Data Transfer |
| ACK | Acknowledgement |
| ERROR | Error Handling |

## System Calls Used
- `socket()`
- `bind()`
- `sendto()`
- `recvfrom()`
- `open()`
- `read()`
- `write()`
- `close()`

## Compilation and Execution

### Server
```bash
gcc server.c tftp.c -o server
./server
```

### Client
```bash
gcc client.c tftp.c -o client
./client
```

Run the server first, followed by the client in another terminal.

*Note: Run compilation commands from the respective source directories.*

## Concepts Learned
- UDP communication
- Socket programming
- Client-server architecture
- TFTP protocol
- Linux system calls
- File handling in C

## Future Enhancements
- Timeout and retransmission support.
- Improved packet validation.
- Enhanced error handling.
- Improved transfer reliability.

## Author
Sneha B.

C Programming | Linux System Programming | Computer Networks

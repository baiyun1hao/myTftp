#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define TFTP_PORT 69

#define PACKET_SIZE 512
#define OPCODE_SIZE 2
#define BLOCK_SIZE 2
#define DATA_SIZE  PACKET_SIZE-OPCODE_SIZE-BLOCK_SIZE
#define ACK_SIZE BLOCK_SIZE+OPCODE_SIZE

typedef struct {
    uint16_t opcode;
    char data[PACKET_SIZE - BLOCK_SIZE];
} TftpPacket;

typedef enum {
    RRQ = 1,
    WRQ = 2,
    DATA = 3,
    ACK = 4,
    ERROR = 5
} TftpOpcode;


int receiveFile(int sockfd, struct sockaddr_in *server_addr, const char *filename);

int sendFile(int sockfd, struct sockaddr_in *client_addr, const char *filename);

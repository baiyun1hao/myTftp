#include"common.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <server_ip> <get/put> <filename>\n", argv[0]);
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TFTP_PORT);
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        return 1;
    }

    TftpPacket request;
    memset(&request, 0, sizeof(request));
    if (strcmp(argv[2], "get") == 0) {
        request.opcode = htons(RRQ);
    } else if (strcmp(argv[2], "put") == 0) {
        request.opcode = htons(WRQ);
    } else {
        printf("Invalid operation: %s\n", argv[2]);
        close(sockfd);
        return 1;
    }
    strncpy(request.data, argv[3], sizeof(request.data));

    // Send request
    if (sendto(sockfd, &request, sizeof(request), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("sendto");
        close(sockfd);
        return 1;
    }

    if (strcmp(argv[2], "get") == 0) {
        if (receiveFile(sockfd, &server_addr, argv[3]) != 0) {
            printf("Failed to receive file\n");
        }
    } else if (strcmp(argv[2], "put") == 0) {
        if (sendFile(sockfd, &server_addr, argv[3]) != 0) {
            printf("Failed to send file\n");
        }
    }

    close(sockfd);

    return 0;
}
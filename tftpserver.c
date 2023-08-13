
#include"common.h"



int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr, client_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TFTP_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        return 1;
    }

    while (1) {
        TftpPacket request;
        socklen_t client_addr_len = sizeof(client_addr);
        ssize_t recv_size = recvfrom(sockfd, &request, sizeof(request), 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if (recv_size < 0) {
            perror("recvfrom");
            continue;
        }

        if (ntohs(request.opcode) == RRQ || ntohs(request.opcode) == WRQ) {
            printf("Received %s request for file: %s\n", (ntohs(request.opcode) == RRQ) ? "RRQ" : "WRQ", request.data);

            if (ntohs(request.opcode) == RRQ) {
                if (sendFile(sockfd, &client_addr, request.data) != 0) {
                    printf("Failed to send file\n");
                }
            } else if( receiveFile(sockfd, &client_addr, request.data)){
                 printf("Failed to receive file\n");
            }
        }
    }

    close(sockfd);

    return 0;
}

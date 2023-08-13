#include "common.h"

int receiveFile(int sockfd, struct sockaddr_in *server_addr, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("fopen");
        return 1;
    }

    int block_number = 1;
    while (1) {
        TftpPacket response;
        socklen_t server_addr_len = sizeof(*server_addr);
        ssize_t recv_size = recvfrom(sockfd, &response, sizeof(response), 0, (struct sockaddr *)server_addr, &server_addr_len);
        if (recv_size < 0) {
            perror("recvfrom");
            fclose(file);
            return 1;
        }
        printf("recv_size %ld\n",recv_size);
        // Handle DATA packet
        if (ntohs(response.opcode) == DATA) {
            int received_block = ntohs(*(uint16_t *)response.data);
            if (received_block == block_number) {

                 fwrite(response.data + BLOCK_SIZE, 1, recv_size - OPCODE_SIZE - BLOCK_SIZE , file);

                // Send ACK
                TftpPacket ack_packet;
                ack_packet.opcode = htons(ACK);
                *(uint16_t *)ack_packet.data = htons(block_number);
                sendto(sockfd, &ack_packet, ACK_SIZE, 0, (struct sockaddr *)server_addr, sizeof(*server_addr));

                if (recv_size < PACKET_SIZE) {
                    break;  // Last data packet received
                }

                block_number++;
            }
        }
    }

    fclose(file);
    return 0;
}

int sendFile(int sockfd, struct sockaddr_in *client_addr, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("fopen");
        return 1;
    }

    int block_number = 1;
    while (1) {
        printf("block_number %d\n",block_number);
        TftpPacket data_packet;
        data_packet.opcode = htons(DATA);
        *(uint16_t *)(data_packet.data) = htons(block_number);

        size_t read_size = fread(data_packet.data + BLOCK_SIZE, 1, DATA_SIZE , file);
        if (read_size == 0) {
            break;  // End of file
        }
 
        ssize_t send_size = OPCODE_SIZE + read_size + BLOCK_SIZE ;

        printf("send_size %ld\n",send_size);
        ssize_t sent = sendto(sockfd, &data_packet, send_size, 0, (struct sockaddr *)client_addr, sizeof(*client_addr));
        if (sent != send_size) {
            perror("sendto");
            fclose(file);
            return 1;
        }

       

        printf(" Waiting for ACK...\n");

        // Wait for ACK
        TftpPacket ack_packet;
        socklen_t client_addr_len = sizeof(*client_addr);
        ssize_t recv_size = recvfrom(sockfd, &ack_packet, sizeof(ack_packet), 0, (struct sockaddr *)client_addr, &client_addr_len);
        if (recv_size < 0) {
            perror("recvfrom");
            fclose(file);
            return 1;
        }

        if (ntohs(ack_packet.opcode) == ERROR) {
            printf("Received ERROR packet from client\n");
            fclose(file);
            return 1;
        } else if (ntohs(ack_packet.opcode) == ACK && ntohs(*(uint16_t *)(ack_packet.data)) == block_number) {
            block_number++;
        }
    }

    fclose(file);
    return 0;
}

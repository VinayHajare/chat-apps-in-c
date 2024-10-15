#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"

int main(){
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof server_addr;
    
    // Create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed!");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof server_addr);

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    while (1) {
        // Send message to server
        printf("Client : ");
        fgets(buffer, BUFFER_SIZE, stdin);
        sendto(sockfd, (const char*) buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *) &server_addr,  addr_len);

        if(strncmp(buffer, "exit", 4) == 0) {
            printf("Client is shutting down...\n");
            break;
        }

        // Receive response from server
        int len = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&server_addr, &addr_len);
        buffer[len] = '\n';
        printf("Server : %s", buffer);

        if(strncmp(buffer, "exit", 4) == 0) {
            printf("Server is shutting down...\n");
            break;
        }
    }
    close(sockfd);
    return 0;
}
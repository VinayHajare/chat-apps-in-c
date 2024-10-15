#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024

int main(){
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof client_addr;

    // Create a UDP Socket
    if( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("Socket Creation Failed!");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof server_addr);
    memset(&client_addr, 0, sizeof client_addr);

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to server address
    if (bind(sockfd, (const struct sockaddr *) &server_addr, sizeof server_addr) < 0) {
        perror("Bind Failed!");
        exit(EXIT_FAILURE);
    }
    
    printf("UDP Chat server is listening on port %d..\n", PORT);

    while (1) {
        // Receive message from client
        int len = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&client_addr, &addr_len);
        buffer[len] = '\n';
        printf("Client : %s", buffer);

        if(strncmp(buffer, "exit", 4) == 0) {
            printf("Client is shutting down...\n");
            break;
        }

        // Send response to client
        printf("Server : ");
        fgets(buffer, BUFFER_SIZE, stdin);
        sendto(sockfd, (const char *) buffer, strlen(buffer), MSG_CONFIRM, (const struct sockaddr *)&client_addr, addr_len);

        if(strncmp(buffer, "exit", 4) == 0) {
            printf("Server is shutting down...\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}
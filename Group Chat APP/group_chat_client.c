#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 8888
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"

int sockfd;
struct sockaddr_in server_addr;
socklen_t addr_len;

void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        int len = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, 
                           (struct sockaddr *)&server_addr, &addr_len);
        buffer[len] = '\0';
        printf("%s\n", buffer);
    }
    return NULL;
}

int main() {
    char buffer[BUFFER_SIZE];
    addr_len = sizeof(server_addr);

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Send initial message to server to register
    char init_msg[] = "Hello, server!";
    sendto(sockfd, init_msg, strlen(init_msg), MSG_CONFIRM, 
           (const struct sockaddr *)&server_addr, addr_len);

    // Create a thread for receiving messages
    pthread_t recv_thread;
    if (pthread_create(&recv_thread, NULL, receive_messages, NULL) != 0) {
        perror("Failed to create thread");
        exit(EXIT_FAILURE);
    }

    printf("Welcome to the group chat! Type your messages below:\n");

    while (1) {
        printf("Type your message --> ");
        fgets(buffer, BUFFER_SIZE, stdin);
        sendto(sockfd, buffer, strlen(buffer), MSG_CONFIRM, 
               (const struct sockaddr *)&server_addr, addr_len);

        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Leaving the chat...\n");
            break;
        }
    }

    // Clean up
    pthread_cancel(recv_thread);
    pthread_join(recv_thread, NULL);
    close(sockfd);

    return 0;
}
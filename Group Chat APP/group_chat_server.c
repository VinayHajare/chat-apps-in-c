#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8888
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

typedef struct {
    struct sockaddr_in address;
    int id;
} client_t;

client_t clients[MAX_CLIENTS];
int client_count = 0;

void broadcast_message(int sockfd, char *message, int sender_id) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, "Client %d: %s", sender_id, message);
    
    for (int i = 0; i < client_count; i++) {
        if (clients[i].id != sender_id) {
            sendto(sockfd, buffer, strlen(buffer), MSG_CONFIRM, 
                   (const struct sockaddr *)&clients[i].address, sizeof(clients[i].address));
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to server address
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP Group Chat Server is listening on port %d...\n", PORT);

    while (1) {
        // Receive message from client
        int len = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, 
                           (struct sockaddr *)&client_addr, &addr_len);
        buffer[len] = '\0';

        // Check if it's a new client
        int client_id = -1;
        for (int i = 0; i < client_count; i++) {
            if (memcmp(&clients[i].address, &client_addr, sizeof(client_addr)) == 0) {
                client_id = clients[i].id;
                break;
            }
        }

        if (client_id == -1) {
            if (client_count < MAX_CLIENTS) {
                clients[client_count].address = client_addr;
                clients[client_count].id = client_count + 1;
                client_id = clients[client_count].id;
                client_count++;
                printf("New client connected. Assigned ID: %d\n", client_id);
                char welcome[BUFFER_SIZE];
                snprintf(welcome, BUFFER_SIZE, "Welcome! Your client ID is %d\n", client_id);
                sendto(sockfd, welcome, strlen(welcome), MSG_CONFIRM, 
                       (const struct sockaddr *)&client_addr, addr_len);
            } else {
                char full[BUFFER_SIZE] = "Server is full. Try again later.\n";
                sendto(sockfd, full, strlen(full), MSG_CONFIRM, 
                       (const struct sockaddr *)&client_addr, addr_len);
                continue;
            }
        }

        printf("Received message from Client %d: %s\n", client_id, buffer);

        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Client %d has left the chat.\n", client_id);
            char leave_msg[BUFFER_SIZE];
            snprintf(leave_msg, BUFFER_SIZE, "Client %d has left the chat.\n", client_id);
            broadcast_message(sockfd, leave_msg, client_id);
            // Remove client from the array (simplified version)
            for (int i = 0; i < client_count; i++) {
                if (clients[i].id == client_id) {
                    for (int j = i; j < client_count - 1; j++) {
                        clients[j] = clients[j + 1];
                    }
                    client_count--;
                    break;
                }
            }
        } else {
            broadcast_message(sockfd, buffer, client_id);
        }
    }

    close(sockfd);
    return 0;
}
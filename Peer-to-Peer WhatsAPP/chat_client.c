#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "chat_shared.h"

// Socket to recieve the incomming packets on the specified port
int sockfd;
// Maintain server info
struct sockaddr_in server_addr;
socklen_t addr_len;
// Current user's name
char username[MAX_USERNAME];

// Method to continuosly receive messages from the server
void *receive_message(void *arg) {
    Message msg;
    while (1) {
        recvfrom(sockfd, &msg, sizeof(Message), 0, (struct sockaddr*)&server_addr, &addr_len);
        // Print message for user according to the type
        switch (msg.type) {
            case MSG_RECEIVE_MESSAGE:
                printf("\n%s: %s\n", msg.sender, msg.content);
                break;
        
            case MSG_LIST_USERS:
                printf("\nOnline users: \n%s", msg.content);
                break;

            case MSG_ERROR:
                printf("\nError: %s\n", msg.content);
                break;

            default:
                break;
        }
    }
    return NULL;
}

// Registe the user to intiate the chat
void register_user() {
    // Create message of the type REGISTER
    Message msg;
    msg.type = MSG_REGISTER;
    // Fill the data to message
    printf("Enter username: ");
    fgets(username, MAX_USERNAME, stdin);
    username[strcspn(username, "\n")] = 0;
    strcpy(msg.sender, username);

    // send the request to server and recieve the response
    sendto(sockfd, &msg, sizeof(Message), 0, (const struct sockaddr*)&server_addr, addr_len);
    recvfrom(sockfd, &msg, sizeof(Message), 0, (struct sockaddr*)&server_addr, &addr_len);

    // Check the servers response for positive or negative acknowledgement
    if (msg.type == MSG_REGISTER) {
        printf("Registration successful\n");
    } else {
        printf("Registration failed: %s\n", msg.content);
        exit(1);
    }
    
}

// Handle the user login to create the session on the server for the current user
void login() {
    // Create the message of the type LOGIN
    Message msg;
    msg.type = MSG_LOGIN;
    // Fill the data to message
    printf("Enter username: ");
    fgets(username, MAX_USERNAME, stdin);
    username[strcspn(username, "\n")] = 0;
    strcpy(msg.sender, username);

    // Send the request and recieve response from the server 
    sendto(sockfd, &msg, sizeof(Message), 0, (const struct sockaddr*)&server_addr, addr_len);
    recvfrom(sockfd, &msg, sizeof(Message), 0, (struct sockaddr*)&server_addr, &addr_len);

    // Check the servers response for the positive or negative acknowledgement
    if (msg.type == MSG_LOGIN) {
        printf("Login successful\n");
    } else {
        printf("Login failed: %s\n", msg.content);
        exit(1);
    }
}


int main() {
    addr_len = sizeof server_addr;

    // Create the socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // set the internal address buffer to 0
    memset(&server_addr, 0, sizeof server_addr);

    // add the servers info 
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Ask the user 
    int choice;
    printf("1. Register\n2.Login\nEnter your choice: ");
    scanf("%d", &choice);
    getchar(); // Consume the newline character

    // act according
    if(choice == 1){
        register_user();
    } else {
        login();
    }
    
    // create a thread to asynchronously receive the message from the server
    pthread_t recv_thread;
    if (pthread_create(&recv_thread, NULL, receive_message, NULL) != 0) {
        perror("Failed to create thread");
        exit(EXIT_FAILURE);
    }

    // Structure to store messages
    Message msg;
    // set the sender to user's name
    msg.type = MSG_SEND_MESSAGE;
    strcpy(msg.sender, username);

    while (1) {
        // ask for the recipient name, to logout & exit type recipient as "exit"
        printf("Enter recipient (exit for logout): ");
        fgets(msg.recipient, MAX_USERNAME, stdin);
        msg.recipient[strcspn(msg.recipient, "\n")] = 0;

        if (strcmp(msg.recipient, "exit") == 0) {
            break;
        }
        
        // ask for the message 
        printf("Enter message: ");
        fgets(msg.content, BUFFER_SIZE, stdin);
        msg.content[strcspn(msg.content, "\n")] = 0;
        
        // send message to server for forwarding
        sendto(sockfd, &msg, sizeof(Message), 0, (const struct sockaddr*)&server_addr, addr_len);
    }
    
    // Send the logout message to server 
    msg.type = MSG_LOGOUT;
    sendto(sockfd, &msg, sizeof(Message), 0, (const struct sockaddr*)&server_addr, addr_len);
    
    // terminate the thread and close socket
    pthread_cancel(recv_thread);
    pthread_join(recv_thread, NULL);
    close(sockfd);

    return 0;
}

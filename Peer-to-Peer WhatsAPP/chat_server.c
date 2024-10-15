#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "chat_shared.h"

// Maintain the tracks of users currently on server
User users[MAX_USERS];
int user_count;

// To avoid the abnormal changes to users entry and provide consistent view of users info
pthread_mutex_t users_mutex = PTHREAD_MUTEX_INITIALIZER;

// Broadcast the User currently on the server
void broadcast_user_list() {
    Message msg;
    msg.type = MSG_LIST_USERS;
    strcpy(msg.sender, "SERVER");

    // Create the list of the user and store in buffer
    char user_list[BUFFER_SIZE] = "";
    for (int i = 0; i < user_count; i++) {
        strcat(user_list, users[i].username);
        strcat(user_list, "\n");
    } 
    strcpy(msg.content, user_list);

    // Broadcast users list to all users
    for (int i = 0; i < user_count; i++) {
        sendto(users[i].socket, &msg, sizeof msg, 0, (struct sockaddr*)&users[i].address, sizeof users[i].address);
    }
}

// Registers the new users on server
void handle_register(int sockfd, Message *msg, struct sockaddr_in *client_addr) {
    // Lock the shared users data 
    pthread_mutex_lock(&users_mutex);

    // See if new user can be allowed
    if(user_count < MAX_USERS) {
        // Check if user is already there
        for (int i = 0; i < user_count; i++) {
            if (strcmp(users[i].username, msg->sender) == 0) {
                msg->type = MSG_ERROR;
                strcpy(msg->content, "Username already exists");
                sendto(sockfd, msg, sizeof(Message), 0, (struct sockaddr *)client_addr, sizeof *client_addr);
                pthread_mutex_unlock(&users_mutex);
                return;
            }
        }
        // Add the user entry to track users
        users[user_count].socket = sockfd;
        users[user_count].address = *client_addr;
        strcpy(users[user_count].username, msg->sender);
        user_count++;

        // Send the acknowledgement to new user of registration successsful
        msg->type = MSG_REGISTER;
        strcpy(msg->content, "Registration successful");
        sendto(sockfd, msg, sizeof(Message), 0, (struct sockaddr*) client_addr, sizeof *client_addr);
        // inform everyone about the new user
        broadcast_user_list();
    } else {
        // Send the negative acknowledgement to new user that server is out of resources
        msg->type = MSG_ERROR;
        strcpy(msg->content, "Server is full");
        sendto(sockfd, msg, sizeof(Message), 0, (struct sockaddr*)client_addr, sizeof *client_addr);
    }
    // Unlock the shared users data 
    pthread_mutex_unlock(&users_mutex);
}

// Login the user 
void handle_login(int sockfd, Message *msg, struct sockaddr_in *client_addr) {
    // Lock the shared users data 
    pthread_mutex_lock(&users_mutex);

    // Check if user is there or not
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, msg->sender) == 0) {
            // add logged in users details
            users[i].socket = sockfd;
            users[i].address = *client_addr;
            // Send the acknowledgement to user
            msg->type =  MSG_LOGIN;
            strcpy(msg->content, "Login Successful");
            sendto(sockfd, msg, sizeof(Message), 0, (struct sockaddr*)client_addr, sizeof(*client_addr));
            broadcast_user_list();
            pthread_mutex_unlock(&users_mutex);
            return;  
        }
    }

    // else send the negative acknowledgement to user, to register first
    msg->type = MSG_ERROR;
    strcpy(msg->content, "User not found");
    sendto(sockfd, msg, sizeof(Message), 0, (struct sockaddr*) client_addr, sizeof *client_addr);

    // Unlock the shared users data 
    pthread_mutex_unlock(&users_mutex);
}

// Remove the user seesion from the server
void handle_logout(Message *msg) {
    // Lock the shared users data 
    pthread_mutex_lock(&users_mutex);

    // remove the users entry from the shared data of users
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, msg->sender) == 0) {
            for(int j = i; j < user_count - 1; j++) {
                users[j] = users[j + 1];
            }
            user_count--;
            broadcast_user_list();
            break;
        }
    }

    // Unlock the shared users data 
    pthread_mutex_unlock(&users_mutex);
}

// Send message to the recipient
void handle_send_message(int sockfd, Message *msg) {
    // Lock the shared users data 
    pthread_mutex_lock(&users_mutex);

    // Check if recipient user is online or not 
    for(int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, msg->recipient) == 0) {
            // Forward the message to recipient
            msg->type = MSG_RECEIVE_MESSAGE;
            sendto(users[i].socket, msg, sizeof(Message), 0, (struct sockaddr*)&users[i].address, sizeof(users[i].address));
            pthread_mutex_unlock(&users_mutex);
            return;
        }
    }

    // Inform sender that user is offline or not present
    msg->type = MSG_ERROR;
    strcpy(msg->content, "User not found or offline");
    sendto(sockfd, msg, sizeof(Message), 0, (struct sockaddr*)&users[0].address, sizeof(users[0].address));

    // Unlock the shared users data
    pthread_mutex_unlock(&users_mutex);
}

int main() {
    // Socket to recieve the incoming packets at specified port
    int sockfd;
    // Maintain server and client info
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof client_addr;
    // Structure to store messages
    Message msg;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // set the internal address buffer to 0
    memset(&server_addr, 0, sizeof server_addr);
    memset(&client_addr, 0, sizeof client_addr);

    // add server info
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // bind the socket to address and port and starts listening
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof server_addr) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d...\n", PORT);

    while (1) {
        // recieve message from users
        recvfrom(sockfd, &msg, sizeof(Message), 0, (struct sockaddr*)&client_addr, &addr_len);

        // check type of message and act according
        switch (msg.type) {
            case MSG_REGISTER:
                handle_register(sockfd, &msg, &client_addr);
                break;

            case MSG_LOGIN:
                handle_login(sockfd, &msg, &client_addr);
                break;

            case MSG_LOGOUT:
                handle_logout(&msg);
                break;

            case MSG_SEND_MESSAGE:
                handle_send_message(sockfd, &msg);
                break;

            default:
                break;
        }
    }
    
    // close the socket
    close(sockfd);
    return 0;
}
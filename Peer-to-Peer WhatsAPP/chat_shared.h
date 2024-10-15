#ifndef CHAT_SHARED_H
#define CHAT_SHARED_H

#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024
#define MAX_USERS 100
#define MAX_USERNAME 32
#define MAX_PASSWORD 32

typedef enum {
    MSG_REGISTER,
    MSG_LOGIN,
    MSG_LOGOUT,
    MSG_LIST_USERS,
    MSG_SEND_MESSAGE,
    MSG_RECEIVE_MESSAGE,
    MSG_ERROR
} MessageType;

typedef struct {
    MessageType type;
    char sender[MAX_USERNAME];
    char recipient[MAX_USERNAME];
    char content[BUFFER_SIZE];
} Message;

typedef struct {
    int socket;
    struct sockaddr_in address;
    char username[MAX_USERNAME];
} User;

#endif

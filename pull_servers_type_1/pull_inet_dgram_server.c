#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#include "pull_serevers_type_1.h"

#define NUMBER_OF_CLIENT 3
#define RANDPORT 7000+rand()%2001;

int count = 0;


void* create_new_socket(void* arg)
{
    PRINT_DEBUG_INFO("here I am\n");
    struct sockaddr_in new_server, client;
    int client_socket;
    int client_port = *(int*)arg;
    int recv_size = 0;
    int send_size = 0;
    socklen_t client_size;
    socklen_t new_server_size;
    char msg[SIZE_OF_MSG];

    PRINT_DEBUG_INFO("client_port = %d\n", client_port);
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1)
    {
        PRINT_ERR("Socket listen_server not success\n");
        exit(EXIT_FAILURE);
    }
    PRINT_DEBUG_INFO("I create client_socket, FD = %d\n", client_socket);

    new_server.sin_family = AF_INET;
    new_server.sin_port = htons(client_port);
    new_server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    client_size = sizeof(client);
    new_server_size = sizeof(new_server);
    if (bind(client_socket, (struct sockaddr*)&new_server, new_server_size) == -1)
    {
        PRINT_ERR("Bind new_server not success\n");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    PRINT_DEBUG_INFO("I wait msg from client\n");
    recv_size = recvfrom(client_socket, msg, sizeof(msg), 0, \
                         (struct sockaddr *)&client, &client_size);
    if (recv_size == -1)
    {
        PRINT_ERR("recvfrom client not success\n");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    PRINT_DEBUG_INFO("recv_size from client = %d\n", recv_size);

    PRINT_INFO("Recv msg: %s, sizeof msg: %d\n", msg, recv_size);
    sprintf(msg, " World! PORT:%d", client_port);

    send_size = sendto(client_socket, &msg, sizeof(msg), 0, \
                               (struct sockaddr *)&client, client_size);
    if (send_size == -1)
    {
        PRINT_ERR("send2client not success\n");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    PRINT_DEBUG_INFO("sent_size2client = %d\n", send_size);
    memset(msg, 0, SIZE_OF_MSG);
    close(client_socket);
    count--;
    return NULL;
}

int main()
{
    struct sockaddr_in server, client;
    int listen_server = 0;
    socklen_t client_size = 0;
    socklen_t server_size = 0;
    int client_port = 0;
    pid_t user_pid;
    pthread_t client_socket_pth[NUMBER_OF_CLIENT];

    client_size = sizeof(client);
    server_size = sizeof(server);

    listen_server = socket(AF_INET, SOCK_DGRAM, 0);
    if (listen_server == -1)
    {
        PRINT_ERR("Socket listen_server not success\n");
        exit(EXIT_FAILURE);
    }
    PRINT_DEBUG_INFO("I create listen_server, FD = %d\n", listen_server);

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(listen_server, (struct sockaddr*)&server, server_size) == -1)
    {
        PRINT_ERR("Bint listen_server not success\n");
        close(listen_server);
    }

    while (1) 
    {
        client_port = RANDPORT;
        PRINT_DEBUG_INFO("я зашел в цикл, жду сообщения\n");
        int recv_size = recvfrom(listen_server, &user_pid, sizeof(user_pid), 0, \
                                 (struct sockaddr *)&client, &client_size);
        if (recv_size == -1)
        {
            PRINT_ERR("recvfrom slisten_server not success\n");
            close(listen_server);
        }
        count++;
        PRINT_DEBUG_INFO("recv_size from client = %d\n", recv_size);
        PRINT_INFO("I receive new user_pid = %d\n", user_pid);
        if (count > NUMBER_OF_CLIENT) {
            PRINT_ERR("Max NUMBER_OF_CLIENT has been reached\n");
            break;
        }
        int send_size = sendto(listen_server, &client_port, sizeof(client_port), 0, \
                               (struct sockaddr *)&client, client_size);
        if (send_size == -1)
        {
            PRINT_ERR("send listen_server not success\n");
            close(listen_server);
        }
        PRINT_DEBUG_INFO("sent_size2client = %d\n", send_size);
        PRINT_INFO("I sent new connect port = %d\n", client_port);
        pthread_create(&client_socket_pth[count], NULL, create_new_socket, &client_port);
    }

    for (int i = 0; i < NUMBER_OF_CLIENT; i++)
    {
        pthread_join(client_socket_pth[i], NULL);
    }

    close(listen_server);
    return 0;
}
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_broadcast.h"

struct sockaddr_in server;
int broadcast_socket = 0;

void receive_msg()
{
    socklen_t server_size = sizeof(server);
    char msg[SIZE_OF_MSG];
    memset(msg, 0, SIZE_OF_MSG);
    int recv_size = 0;
    while (1) 
    {
        recv_size = recvfrom(broadcast_socket, msg, SIZE_OF_MSG, 0, (struct sockaddr *)&server, &server_size);
        if (recv_size == -1) 
        {
            PRINT_ERR("recvfrom broadcast_socket not success");
            close(broadcast_socket);
            exit(EXIT_FAILURE);
        }
        PRINT_INFO("i recvfrom msg = %s, recv_size = %d\n", msg, recv_size);
    }
}

void assigns_broadcast_socket()
{
    PRINT_DEBUG_INFO("here i am\n");
    if (bind(broadcast_socket, (struct sockaddr *)&server, sizeof(server)) == -1) 
    {
        PRINT_ERR("bind broadcast_socket not success\n");
        close(broadcast_socket);
        exit(EXIT_FAILURE);
    }
}

void addrin_filling()
{
    PRINT_DEBUG_INFO("here i am\n");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(BROADCAST_ADDRESS);
}

void create_broadcast_sock()
{
    PRINT_DEBUG_INFO("here i am\n");
    broadcast_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (broadcast_socket == -1) 
    {
        PRINT_ERR("Socket error");
        exit(EXIT_FAILURE);
    }
}

void init_broadcast_sock()
{
    PRINT_DEBUG_INFO("here i am\n");
    create_broadcast_sock();
    addrin_filling();
    assigns_broadcast_socket();
}

int main()
{
    init_broadcast_sock();
    receive_msg();
    close(broadcast_socket);
    return 0;
}
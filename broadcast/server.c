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

void send_message_for_all()
{
    PRINT_DEBUG_INFO("here i am\n");
    socklen_t server_size = sizeof(server);
    char msg[SIZE_OF_MSG] = "Hello world!";
    int send_size = 0;
    while (1) 
    {
        send_size = sendto(broadcast_socket, msg, sizeof(msg), 0, (struct sockaddr *)&server, server_size);
        if (send_size == -1) 
        {
            PRINT_ERR("Sendto error");
            close(broadcast_socket);
            exit(EXIT_FAILURE);
        }
        PRINT_DEBUG_INFO("I send msg; size_msg = %d\n", send_size);
        sleep(3);
    }
}

void set_socket_broadcast()
{
    PRINT_DEBUG_INFO("here i am\n");
    int optval = 1;
    if (setsockopt(broadcast_socket, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1) 
    {
        PRINT_ERR("setsockopt error");
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
        PRINT_ERR("broadcast_socket error\n");
        exit(EXIT_FAILURE);
    }
}

void init_broadcast_sock()
{
    PRINT_DEBUG_INFO("here i am\n");
    create_broadcast_sock();
    addrin_filling();
    set_socket_broadcast();
}

int main()
{
    init_broadcast_sock();
    send_message_for_all();
    close(broadcast_socket);
    return 0;
}
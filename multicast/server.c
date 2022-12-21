#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_multicast.h"

struct sockaddr_in server;
static int multicast_sock = 0;

void send_message_for_all()
{
    PRINT_DEBUG_INFO("here i am\n");
    socklen_t server_size = sizeof(server);
    char msg[SIZE_OF_MSG] = "Hello world!";
    int send_size = 0;
    while (1) 
    {
        send_size = sendto(multicast_sock, msg, sizeof(msg), 0, (struct sockaddr *)&server, server_size);
        if (send_size == -1) 
        {
            PRINT_ERR("Sendto error");
            close(multicast_sock);
            exit(EXIT_FAILURE);
        }
        PRINT_DEBUG_INFO("I send msg; size_msg = %d\n", send_size);
        sleep(3);
    }
}

void addrin_filling()
{
    PRINT_DEBUG_INFO("here i am\n");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(MULTICAST_ADDRESS);
}

void create_broadcast_sock()
{
    PRINT_DEBUG_INFO("here i am\n");
    multicast_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (multicast_sock == -1) 
    {
        PRINT_ERR("multicast_sock error\n");
        exit(EXIT_FAILURE);
    }
}

void init_broadcast_sock()
{
    PRINT_DEBUG_INFO("here i am\n");
    create_broadcast_sock();
    addrin_filling();
}

int main()
{
    init_broadcast_sock();
    send_message_for_all();
    close(multicast_sock);
    return 0;
}
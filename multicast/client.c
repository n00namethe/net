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
struct ip_mreqn group;
static int multicast_sock = 0;

void receive_msg()
{
    socklen_t server_size = sizeof(server);
    char msg[SIZE_OF_MSG];
    memset(msg, 0, SIZE_OF_MSG);
    int recv_size = 0;
    while (1) 
    {
        recv_size = recvfrom(multicast_sock, msg, SIZE_OF_MSG, 0, (struct sockaddr *)&server, &server_size);
        if (recv_size == -1) 
        {
            PRINT_ERR("recvfrom multicast_sock not success");
            close(multicast_sock);
            exit(EXIT_FAILURE);
        }
        PRINT_INFO("msg = %s, recv_size = %d\n", msg, recv_size);
    }
}

void assigns_multicast_sock()
{
    PRINT_DEBUG_INFO("here i am\n");
    if (bind(multicast_sock, (struct sockaddr *)&server, sizeof(server)) == -1) 
    {
        PRINT_ERR("bind multicast_sock not success\n");
        close(multicast_sock);
        exit(EXIT_FAILURE);
    }
}

void set_socket_broadcast()
{
    PRINT_DEBUG_INFO("here i am\n");
    if (setsockopt (multicast_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group)) == -1) 
    {
        PRINT_ERR("setsockopt error");
        close(multicast_sock);
        exit(EXIT_FAILURE);
    }
}

void multicast_arg_filling()
{
    PRINT_DEBUG_INFO("here i am\n");
    group.imr_address.s_addr = server.sin_addr.s_addr;
    group.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDRESS);
    group.imr_ifindex = 0;
}

void addrin_filling()
{
    PRINT_DEBUG_INFO("here i am\n");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
}

void create_multicast_sock()
{
    PRINT_DEBUG_INFO("here i am\n");
    multicast_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (multicast_sock == -1) 
    {
        PRINT_ERR("Socket error");
        exit(EXIT_FAILURE);
    }
}

void init_multicast_sock()
{
    PRINT_DEBUG_INFO("here i am\n");
    create_multicast_sock();
    addrin_filling();
    multicast_arg_filling();
    set_socket_broadcast();
    assigns_multicast_sock();
}

int main()
{
    init_multicast_sock();
    receive_msg();
    close(multicast_sock);
    return 0;
}
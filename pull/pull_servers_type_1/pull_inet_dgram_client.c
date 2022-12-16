#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

#include "pull_serevers_type_1.h"

int conntect_new_server(int new_port)
{
    PRINT_DEBUG_INFO("here I am\n");
    struct sockaddr_in server;
    int msg_server = 0;
    socklen_t server_size = 0;
    int recv_size = 0;
    int send_size = 0;
    char msg[SIZE_OF_MSG] = "Hello";

    msg_server = socket(AF_INET, SOCK_DGRAM, 0);
    if(msg_server == -1) 
    {
        PRINT_ERR("Socket msg_server not success\n");
        return -1;
    }
    PRINT_DEBUG_INFO("new_port = %d\n", new_port);
    server.sin_family = AF_INET;
    server.sin_port = htons(new_port);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_size = sizeof(server);

    send_size = sendto(msg_server, msg, sizeof(msg), 0, \
                       (struct sockaddr *)&server, server_size);
    if (send_size == -1)
    {
        PRINT_ERR("sent msg2msg_server not success\n");
        close(msg_server);
        return -1;
    }
    PRINT_DEBUG_INFO("i sent msg = %s; sizeof(sent_msg) = %d\n", msg, send_size);

    recv_size = recvfrom(msg_server, msg, sizeof(msg), 0, \
                         (struct sockaddr *)&server, &server_size);
    if (recv_size == -1)
    {
        PRINT_ERR("recv msg not success\n");
        close(msg_server);
        return -1;
    }
    PRINT_DEBUG_INFO("I recv msg = %s; sizeof(recv_msg) = %d\n", msg, recv_size);
    return 0;
}

int main()
{
    struct sockaddr_in server;
    int listen_server = 0;
    socklen_t server_size = 0;
    int recv_size = 0;
    int send_size = 0;
    pid_t user_pid;
    int new_port = 0;

    listen_server = socket(AF_INET, SOCK_DGRAM, 0);
    if(listen_server == -1) 
    {
        PRINT_ERR("Socket listen_server not success\n");
        return -1;
    }
    PRINT_DEBUG_INFO("FD listen_server = %d\n", listen_server);

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    server_size = sizeof(server);

    user_pid = getpid();
    send_size = sendto(listen_server, &user_pid, sizeof(user_pid), 0, \
                       (struct sockaddr *)&server, server_size);
    if (send_size == -1)
    {
        PRINT_ERR("sent pid2server not success\n");
        close(listen_server);
        return -1;
    }
    PRINT_DEBUG_INFO("i sent user_pid = %d; sizeof(sent_msg) = %d\n", user_pid, send_size);

    recv_size = recvfrom(listen_server, &new_port, sizeof(new_port), 0, \
                         (struct sockaddr *)&server, &server_size);
    if (recv_size == -1)
    {
        PRINT_ERR("recv new_port not success\n");
        close(listen_server);
        return -1;
    }
    PRINT_DEBUG_INFO("I recv msg with new_port = %d; sizeof(recv_msg) = %d\n", new_port, recv_size);

    conntect_new_server(new_port);

    close(listen_server);
    return 0;
}
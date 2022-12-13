#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define SERVER_SOCK_NUMBER 4000
#define SIZE_OF_MSG 15

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define PRINT_DEBUG_INFO(...) printf(__VA_ARGS__); printf("\n")
#define PRINT_ERR(...) printf("[errno = %d]\n", errno); printf(__VA_ARGS__); printf("\n")
#define PRINT_INFO(...) printf(__VA_ARGS__); printf("\n")

typedef struct _server_context_t
{
    int server_socket_description;
    struct sockaddr_in server_addr;
} server_context_t;
server_context_t listen_server_ctx = {0};
server_context_t communication_server_ctx = {0};

void message_exchange()
{
    PRINT_DEBUG_INFO(__FUNCTION__);
    char msg[SIZE_OF_MSG] = "Hello";
    if (send(communication_server_ctx.server_socket_description, msg, sizeof(msg), 0) == -1)
    {
        PRINT_ERR("send2communication_server not success\n");
    }
    PRINT_INFO("msg sent: %s\n", msg);
    if (recv(communication_server_ctx.server_socket_description, msg, sizeof(msg), 0) == -1)
    {
        PRINT_ERR("recv from communication_server not success\n");
    }
    PRINT_INFO("msg receive: %s\n", msg);
    close(communication_server_ctx.server_socket_description);
}

void connect2communication_server()
{
    PRINT_DEBUG_INFO(__FUNCTION__);
    communication_server_ctx.server_socket_description = socket(AF_INET, SOCK_DGRAM, 0);
    if (communication_server_ctx.server_socket_description == -1)
    {
        PRINT_ERR("communication_server_ctx.server_socket_description");
        exit(EXIT_FAILURE);
    }
    if (connect(communication_server_ctx.server_socket_description, (struct sockaddr *)&communication_server_ctx.server_addr, \
                sizeof(communication_server_ctx.server_addr)) == -1)
    {
        PRINT_ERR("connect2communication_server not success\n");
        close(communication_server_ctx.server_socket_description);
    }
}

void receive_communication_server_addr()
{
    PRINT_DEBUG_INFO(__FUNCTION__);
    pid_t my_pid = getpid();
    socklen_t server_addr_size = sizeof(listen_server_ctx.server_addr);
    if (sendto(listen_server_ctx.server_socket_description, &my_pid, sizeof(my_pid), 0, \
               (struct sockaddr *)&listen_server_ctx.server_addr, server_addr_size) == -1)
    {
        PRINT_ERR("send2listen_socket not success\n");
        close(listen_server_ctx.server_socket_description);
    }
    if (recvfrom(listen_server_ctx.server_socket_description, &communication_server_ctx.server_addr, sizeof(communication_server_ctx.server_addr), 0, \
                     (struct sockaddr *)&listen_server_ctx.server_addr, &server_addr_size) == -1)
    {
        PRINT_ERR("recv from clisten_server not success\n");
        close(listen_server_ctx.server_socket_description);
    }
    close(listen_server_ctx.server_socket_description);
}

void init_server_listen_socket_struct()
{
    PRINT_DEBUG_INFO(__FUNCTION__);
    listen_server_ctx.server_addr.sin_family = AF_INET;
    listen_server_ctx.server_addr.sin_port = htons(SERVER_SOCK_NUMBER);
    listen_server_ctx.server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
}

void init_listen_server()
{
    PRINT_DEBUG_INFO(__FUNCTION__);
    listen_server_ctx.server_socket_description = socket(AF_INET, SOCK_DGRAM, 0);
    if (listen_server_ctx.server_socket_description == -1)
    {
        PRINT_ERR("listen_server_ctx.server_socket_description");
        exit(EXIT_FAILURE);
    }
    init_server_listen_socket_struct();
}

int main()
{
    init_listen_server();
    receive_communication_server_addr();
    connect2communication_server();
    message_exchange();
    return 0;
}
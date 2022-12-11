#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_SOCK_PATH "/somepathe"
#define PEER_SOCK_PATH "/client"
#define SIZE_OF_MSG 15

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main()
{
    unlink(PEER_SOCK_PATH);
    int cfd;
    struct sockaddr_un server_addr, peer_addr;
    socklen_t address_length;
    address_length = sizeof(struct sockaddr_un);

    cfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (cfd == -1)
    {
        handle_error("socket");
    }
    
    server_addr.sun_family = AF_LOCAL;
    strncpy(server_addr.sun_path, SERVER_SOCK_PATH, sizeof(server_addr.sun_path) - 1);
    server_addr.sun_path[0] = 0;

    peer_addr.sun_family = AF_LOCAL;
    strncpy(peer_addr.sun_path, PEER_SOCK_PATH, sizeof(peer_addr.sun_path) - 1);
    peer_addr.sun_path[0] = 0;

    if (bind(cfd, (struct sockaddr *)&peer_addr, address_length) == -1)
    {
        handle_error("connect");
    }

    char msg[SIZE_OF_MSG] = "Hello";
    if (sendto(cfd, msg, sizeof(msg), 0, (struct sockaddr *)&server_addr, address_length) == -1)
    {
        handle_error("send");
    }
    printf("msg sent: %s\n", msg);
    if (recvfrom(cfd, msg, sizeof(msg), 0, (struct sockaddr *)&server_addr, &address_length) == -1)
    {
        handle_error("recv");
    }
    printf("msg receive: %s\n", msg);
    close(cfd);
    unlink(PEER_SOCK_PATH);
    return 0;
}
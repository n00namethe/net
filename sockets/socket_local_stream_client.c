#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>

#define MY_SOCK_PATH "/somepathe"
#define SIZE_OF_MSG 15

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main()
{
    int cfd;
    cfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (cfd == -1)
    {
        handle_error("socket");
    }
    struct sockaddr_un peer_addr;
    memset(&peer_addr, 0, sizeof(peer_addr));
    peer_addr.sun_family = AF_LOCAL;
    strncpy(peer_addr.sun_path, MY_SOCK_PATH, sizeof(peer_addr.sun_path) - 1);
    if (connect(cfd, (struct sockaddr *)&peer_addr, sizeof(peer_addr)) == -1)
    {
        handle_error("connect");
    }
    char msg[SIZE_OF_MSG];
    if (recv(cfd, msg, sizeof(msg), 0) == -1)
    {
        handle_error("recv");
    }
    printf("msg receive: %s\n", msg);
    strcat(msg, " World!");
    if (send(cfd, msg, sizeof(msg),0) == -1)
    {
        handle_error("send");
    }
    printf("msg sent: %s\n", msg);
    close(cfd);
    return 0;
}
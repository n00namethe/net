#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MY_SOCK_NUMBER 5555
#define SIZE_OF_MSG 15

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main()
{
    int cfd;
    cfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (cfd == -1)
    {
        handle_error("socket");
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(MY_SOCK_NUMBER);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(cfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        handle_error("connect");
    }
    char msg[SIZE_OF_MSG] = "Hello";
    if (send(cfd, msg, sizeof(msg), 0) == -1)
    {
        handle_error("send");
    }
    printf("msg sent: %s\n", msg);
    if (recv(cfd, msg, sizeof(msg), 0) == -1)
    {
        handle_error("recv");
    }
    printf("msg receive: %s\n", msg);
    close(cfd);
    return 0;
}
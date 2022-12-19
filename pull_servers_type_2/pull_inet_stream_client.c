#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "pull_serevers_type_2.h"

int main()
{
    int cfd;
    int send_size = 0;
    int recv_size = 0;
    cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cfd == -1)
    {
        PRINT_ERR("create socket not success\n");
        return -1;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (connect(cfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        PRINT_ERR("connect not success\n");
        close(cfd);
        return -1;
    }
    char msg[SIZE_OF_MSG] = "Hello!";
    sleep(2);
    send_size = send(cfd, msg, sizeof(msg), 0);
    if (send_size == -1)
    {
        PRINT_ERR("send not success\n");
        close(cfd);
        return -1;
    }
    PRINT_INFO("msg sent: %s, size send_msg = %d\n", msg, send_size);
    sleep(2);
    recv_size = recv(cfd, msg, sizeof(msg), 0);
    if (recv_size == -1)
    {
        PRINT_ERR("recv not success\n");
        close(cfd);
        return -1;
    }
    PRINT_INFO("msg receive: %s, size recv_msg = %d\n", msg, recv_size);
    
    close(cfd);
    return 0;
}
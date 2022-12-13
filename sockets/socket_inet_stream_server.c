#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MY_SOCK_NUMBER 5555
#define LISTEN_BACKLOG 5
#define SIZE_OF_MSG 15

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main()
{
	int sfd, cfd;
	struct sockaddr_in my_addr, peer_addr;
	socklen_t peer_addr_size;

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1)
	{
		handle_error("socket");
	}
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(MY_SOCK_NUMBER);
	my_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (bind(sfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1)
	{
		handle_error("bind");
	}
	if (listen(sfd, LISTEN_BACKLOG) == -1)
	{
		handle_error("listen");
	}
	peer_addr_size = sizeof(peer_addr);
	cfd = accept(sfd, (struct sockaddr *)&peer_addr, &peer_addr_size);
	if (cfd == -1)
	{
		handle_error("accept");
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
	close(sfd);
	return 0;
}
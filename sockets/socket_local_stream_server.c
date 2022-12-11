#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>

#define MY_SOCK_PATH "/somepathe"
#define LISTEN_BACKLOG 5
#define SIZE_OF_MSG 15

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)


int main()
{
	int sfd, cfd;
	struct sockaddr_un my_addr, peer_addr;
	socklen_t peer_addr_size;

	sfd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (sfd == -1)
	{
		handle_error("socket");
	}
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sun_family = AF_LOCAL;
	strncpy(my_addr.sun_path, MY_SOCK_PATH, sizeof(my_addr.sun_path) - 1);
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
	unlink(MY_SOCK_PATH);
	return 0;
}
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MY_SOCK_PATH "/somepathe"
#define SIZE_OF_MSG 15

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main()
{
	unlink(MY_SOCK_PATH);
	int sfd;
	struct sockaddr_un my_addr, peer_addr;
	socklen_t address_length;
	address_length = sizeof(struct sockaddr_un);

	sfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
	if (sfd == -1)
	{
		handle_error("socket");
	}
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sun_family = AF_LOCAL;
	strncpy(my_addr.sun_path, MY_SOCK_PATH, sizeof(my_addr.sun_path) - 1);
	my_addr.sun_path[0] = 0;
	if (bind(sfd, (struct sockaddr *)&my_addr, address_length) == -1)
	{
		handle_error("bind");
	}
	char msg[SIZE_OF_MSG];
	if (recvfrom(sfd, msg, sizeof(msg), 0, (struct sockaddr *)&peer_addr, &address_length) == -1)
	{
		handle_error("recvfrom");
	}
	printf("msg receive: %s\n", msg);
	strcat(msg, " World!");
	if (sendto(sfd, msg, sizeof(msg), 0, (struct sockaddr *)&peer_addr, address_length) == -1)
	{
		handle_error("send");
	}
	printf("msg sent: %s\n", msg);
	unlink(MY_SOCK_PATH);
	close(sfd);
	return 0;
}
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "raw_ip_udp.h"

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main()
{
	int sfd;
	struct sockaddr_in my_addr, peer_addr;
	socklen_t peer_addr_size;

	sfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sfd == -1)
	{
		handle_error("socket");
	}
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(DEST_PORT);
	my_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (bind(sfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1)
	{
		handle_error("bind");
	}
	char msg[SIZE_OF_MSG];
	peer_addr_size = sizeof(peer_addr);
	if (recvfrom(sfd, msg, sizeof(msg), 0, (struct sockaddr *)&peer_addr, &peer_addr_size) == -1)
	{
		handle_error("recvfrom");
	}
	printf("msg receive: %s\n", msg);
	strcat(msg, " World!");
	if (sendto(sfd, msg, sizeof(msg), 0, (struct sockaddr *)&peer_addr, peer_addr_size) == -1)
	{
		handle_error("send");
	}
	printf("msg sent: %s\n", msg);
	close(sfd);
	return 0;
}
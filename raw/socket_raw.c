#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>

#define SIZE_OF_MSG 255

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main()
{
	int raw_socket;
	struct sockaddr_in client;
	socklen_t client_addr_size = sizeof(client);

	raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (raw_socket == -1)
	{
		handle_error("socket");
	}
	char msg[SIZE_OF_MSG];
	memset(msg, 0, SIZE_OF_MSG);
	while (1)
	{
		if (recvfrom(raw_socket, msg, sizeof(msg), 0, (struct sockaddr *)&client, &client_addr_size) == -1)
		{
			handle_error("recvfrom");
			close(raw_socket);
			return -1;
		}
		printf("msg receive: %s\n", msg + 28);
	}
	close(raw_socket);
	return 0;
}

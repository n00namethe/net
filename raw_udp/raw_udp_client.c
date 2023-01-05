#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <malloc.h>
#include "raw_udp.h"

#define OFFSET 20

struct sockaddr_in server;
static int raw_socket = 0;

void filling_udp_header(char *message)
{
	PRINT_DEBUG_INFO("Here I am\n");
	((struct udphdr*)message)->uh_sport = htons(SOURCE_PORT);
	((struct udphdr*)message)->uh_dport = htons(DEST_PORT);
	((struct udphdr*)message)->uh_ulen = htons(SIZE_OF_MSG + sizeof(struct udphdr));
	((struct udphdr*)message)->uh_sum = 0;
}

void send_msg2serv(char *message)
{
	PRINT_DEBUG_INFO("Here I am\n");
	int send_size = 0;
	char msg[SIZE_OF_MSG] = "hello serv";
	socklen_t sockaddr_size = sizeof(server);
	filling_udp_header(message);
	strncpy((message + sizeof(struct udphdr)), msg, SIZE_OF_MSG);
	send_size = sendto(raw_socket, message, sizeof(struct udphdr) + SIZE_OF_MSG, 0, (struct sockaddr *)&server, sockaddr_size);
	if (send_size == -1)
	{
		PRINT_ERR("sendto raw_socket not success\n");
		close(raw_socket);
		free(message);
		exit(EXIT_FAILURE);
	}
	PRINT_DEBUG_INFO("I send msg: %s, sizeof msg = %d\n", msg, send_size);
	PRINT_DEBUG_INFO("send: source port: %d, dest port: %d\n", \
					  ntohs(((struct udphdr*)message)->uh_sport), ntohs(((struct udphdr*)message)->uh_dport));
	memset(message, 0, sizeof(struct udphdr) + SIZE_OF_MSG);
	PRINT_DEBUG_INFO("after memset dport = %d\n", ntohs(((struct udphdr*)message)->uh_dport));
}

void receive_msg(char *message)
{
	PRINT_DEBUG_INFO("Here I am\n");
	int recv_size = 0;
	socklen_t sockaddr_size = sizeof(server);
	char msg[SIZE_OF_MSG] = {0};
	while (1)
	{
		recv_size = recvfrom(raw_socket, msg, sizeof(msg), 0, (struct sockaddr *)&server, &sockaddr_size);
		if (recv_size == -1) 
		{
			PRINT_ERR("recvfrom raw_socket not success\n");
			close(raw_socket);
			free(message);
			exit(EXIT_FAILURE);
		}
		PRINT_DEBUG_INFO("recv_size = %d\n", recv_size);
		PRINT_DEBUG_INFO("recv: source port: %d, dest port: %d\n", \
					  	  ntohs(((struct udphdr*)message)->uh_sport), ntohs(((struct udphdr*)message)->uh_dport));
		PRINT_INFO("recv msg from serv: %s\n", msg + OFFSET + sizeof(struct udphdr));
	}
}

void filling_sockaddr()
{
	PRINT_DEBUG_INFO("Here I am\n");
	server.sin_family = AF_INET;
	server.sin_port = htons(SOURCE_PORT);
	server.sin_addr.s_addr = INADDR_ANY;
}

void create_raw_socket()
{
	PRINT_DEBUG_INFO("Here I am\n");
	raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
	if (raw_socket == -1)
	{
		PRINT_ERR("raw_socket create not success\n");
		exit(EXIT_FAILURE);
	}
}

void init_raw_socket()
{
	PRINT_DEBUG_INFO("Here I am\n");
	create_raw_socket();
	filling_sockaddr();
}

int main()
{
	PRINT_DEBUG_INFO("Here I am\n");
	init_raw_socket();
	char *message = malloc(sizeof(struct udphdr) + SIZE_OF_MSG);
	send_msg2serv(message);
	receive_msg(message);
	close(raw_socket);
	free(message);
	return 0;
}
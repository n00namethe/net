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
#include <netinet/ip.h>
#include "raw_ip_udp.h"

#define INTERNET_HEADER_LENGHT 5
#define IP_VER 4
#define UNUSED 0
#define AUTO_FILLED 0
#define NO_NEED_FRAGMENTATION 0
#define TIME_TO_LIFE 255

struct sockaddr_in server;
static int raw_socket = 0;

void send_msg2serv(char *message)
{
	PRINT_DEBUG_INFO("Here I am\n");
	int send_size = 0;
	char msg[SIZE_OF_MSG] = "hello serv";
	socklen_t sockaddr_size = sizeof(server);
	strncpy((message + sizeof(struct iphdr) + sizeof(struct udphdr)), msg, SIZE_OF_MSG);
	send_size = sendto(raw_socket, message, sizeof(struct iphdr) +sizeof(struct udphdr) + SIZE_OF_MSG, 0, \
					   (struct sockaddr *)&server, sockaddr_size);
	if (send_size == -1)
	{
		PRINT_ERR("sendto raw_socket not success\n");
		close(raw_socket);
		free(message);
		exit(EXIT_FAILURE);
	}
	PRINT_DEBUG_INFO("I send msg: %s, sizeof msg = %d\n", msg, send_size);
	message += sizeof(struct iphdr);
	PRINT_DEBUG_INFO("send: source port: %d, dest port: %d\n", \
					  ntohs(((struct udphdr*)message)->uh_sport), ntohs(((struct udphdr*)message)->uh_dport));
	memset(message, 0, sizeof(struct udphdr) + SIZE_OF_MSG);
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
		PRINT_INFO("recv msg from serv: %s\n", msg + sizeof(struct iphdr) + sizeof(struct udphdr));
	}
}

void filling_udp_header(char *message)
{
	PRINT_DEBUG_INFO("Here I am\n");
	message += sizeof(struct iphdr);
	((struct udphdr*)message)->uh_sport = htons(SOURCE_PORT);
	((struct udphdr*)message)->uh_dport = htons(DEST_PORT);
	((struct udphdr*)message)->uh_ulen = htons(sizeof(struct udphdr) + SIZE_OF_MSG);
	((struct udphdr*)message)->uh_sum = 0;
}

void filling_ip_header(char *message)
{
	PRINT_DEBUG_INFO("Here I am\n");
	((struct iphdr*)message)->ihl = INTERNET_HEADER_LENGHT;
	((struct iphdr*)message)->version = IP_VER;
	((struct iphdr*)message)->tos = UNUSED;
	((struct iphdr*)message)->id = AUTO_FILLED;
	((struct iphdr*)message)->frag_off = NO_NEED_FRAGMENTATION;
	((struct iphdr*)message)->ttl = TIME_TO_LIFE;
	((struct iphdr*)message)->protocol = IPPROTO_UDP;
	((struct iphdr*)message)->check = AUTO_FILLED;
	((struct iphdr*)message)->saddr = htonl(INADDR_LOOPBACK);
	((struct iphdr*)message)->daddr = htonl(INADDR_LOOPBACK);
	((struct iphdr*)message)->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + SIZE_OF_MSG);
}

void set_socket_ip_manual_filling()
{
    PRINT_DEBUG_INFO("here i am\n");
    int optval = 1;
    if (setsockopt(raw_socket, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) == -1) 
    {
        PRINT_ERR("setsockopt error");
        close(raw_socket);
        exit(EXIT_FAILURE);
    }
}

void filling_sockaddr()
{
	PRINT_DEBUG_INFO("Here I am\n");
	server.sin_family = AF_INET;
	server.sin_port = htons(SOURCE_PORT);
	server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
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
	set_socket_ip_manual_filling();
}

int main()
{
	PRINT_DEBUG_INFO("Here I am\n");
	init_raw_socket();
	char *message = malloc(sizeof(struct iphdr) + sizeof(struct udphdr) + SIZE_OF_MSG);
	filling_ip_header(message);
	filling_udp_header(message);
	send_msg2serv(message);
	receive_msg(message);
	close(raw_socket);
	free(message);
	return 0;
}
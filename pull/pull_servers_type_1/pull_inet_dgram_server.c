#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#define SERVER_SOCK_NUMBER 4000
#define SIZE_OF_MSG 15
#define NUBMER_OF_CLIENTS 20

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILTURE); } while (0)

#define PRINT_DEBUG_INFO(...) printf(__VA_ARGS__); printf("\n")
#define PRINT_ERR(...) printf("[errno = %d]\n", errno); printf(__VA_ARGS__); printf("\n")
#define PRINT_INFO(...) printf(__VA_ARGS__); printf("\n")

int user_port = 4001;

typedef struct _c2s
{
	struct sockaddr_in peer_addr;
	socklen_t peer_addr_size;
	pid_t user_pid;
} c2s;

typedef struct _client_t
{
	int user_socket_description;
	int user_port;
	struct sockaddr_in users_sockaddr;
	socklen_t user_sockaddr_size;
	pid_t user_pid;
	pthread_t user_socket_pth;
} client_t;
client_t users_db[NUBMER_OF_CLIENTS] = {0};

typedef struct _server_context_t
{
	int listen_socket_description;
	struct sockaddr_in listen_addr;
} server_context_t;
server_context_t server_ctx = {0};

void close_user_socket(int user_number)
{
	PRINT_DEBUG_INFO(__FUNCTION__);
	close(users_db[user_number].user_socket_description);
	PRINT_DEBUG_INFO("Я закрыл сокет users_db[%d]\n", user_number);
	if (pthread_join(users_db[user_number].user_socket_pth, NULL) != 0)
    {
        PRINT_ERR("Failed to join thread[%d]\n", user_number);
    }
    PRINT_DEBUG_INFO("pthread_join[%d] success\n", user_number);
    memset(&users_db[user_number], 0, sizeof(users_db[user_number]));
}

void close_socket()
{
	PRINT_DEBUG_INFO(__FUNCTION__);
	close(server_ctx.listen_socket_description);
}

void close_all()
{
	PRINT_DEBUG_INFO(__FUNCTION__);
	for (int i = 0; i < NUBMER_OF_CLIENTS; i++)
	{
		close(users_db[i].user_socket_description);
		PRINT_DEBUG_INFO("Я закрыл сокет users_db[%d]\n", i);
		if (pthread_join(users_db[i].user_socket_pth, NULL) != 0)
	    {
	        PRINT_ERR("Failed to join thread[%d]\n", i);
	    }
	    PRINT_DEBUG_INFO("pthread_join[%d] success\n", i);
	    memset(&users_db[i], 0, sizeof(users_db[i]));
	}
	close_socket();
	exit(EXIT_FAILURE);
}

void message_exchange(int user_number)
{
	PRINT_DEBUG_INFO(__FUNCTION__);
	char msg[SIZE_OF_MSG];
	users_db[user_number].user_sockaddr_size = sizeof(users_db[user_number].users_sockaddr);
	if (recvfrom(users_db[user_number].user_socket_description, msg, sizeof(msg), 0, \
		             (struct sockaddr *)&users_db[user_number].users_sockaddr, &users_db[user_number].user_sockaddr_size) == -1)
	{
		PRINT_ERR("users_db[%d].user_socket_description not success\n", user_number);
		close_all();
	}
	PRINT_INFO("msg receive: %s\n", msg);
	strcat(msg, " World!");
	if (sendto(users_db[user_number].user_socket_description, msg, sizeof(msg), 0, \
			   (struct sockaddr *)&users_db[user_number].users_sockaddr, users_db[user_number].user_sockaddr_size) == -1)
	{
		PRINT_ERR("send users_db[%d].user_socket_description not success\n", user_number);
		close_all();
	}
	PRINT_INFO("msg send: %s\n", msg);
	close_user_socket(user_number);
}

void init_user_socket(int user_number)
{
	PRINT_DEBUG_INFO(__FUNCTION__);
	users_db[user_number].users_sockaddr.sin_family = AF_INET;
	users_db[user_number].user_port = user_port;
	users_db[user_number].users_sockaddr.sin_port = htons(users_db[user_number].user_port);
	users_db[user_number].users_sockaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	user_port++;
}

void create_socket_for_client(int user_number)
{
	PRINT_DEBUG_INFO(__FUNCTION__);
	users_db[user_number].user_socket_description = socket(AF_INET, SOCK_DGRAM, 0);
	if (users_db[user_number].user_socket_description == -1)
	{
		PRINT_ERR("users_db[%d].user_socket_description not success\n", user_number);
		close_all();
	}
	PRINT_DEBUG_INFO("Socket for client[%d] create. description = %d\n", user_number, users_db[user_number].user_socket_description);
	init_user_socket(user_number);
	if (bind(users_db[user_number].user_socket_description, (struct sockaddr *)&users_db[user_number].users_sockaddr, \
			 sizeof(users_db[user_number].users_sockaddr)) == -1)
	{
		PRINT_ERR("users_db[%d].user_socket_description not success\n", user_number);
		close_all();
	}

}

void start_new_message_exchange(int user_number)
{
	PRINT_DEBUG_INFO(__FUNCTION__);
	if (pthread_create(&users_db[user_number].user_socket_pth, NULL, (void *)&message_exchange, &user_number) != 0)
    {
        PRINT_ERR("Failed to create thread[%d]\n", user_number);
        close_all();
    }
}

int legit_check_new_client(c2s *new_user)
{
	PRINT_DEBUG_INFO(__FUNCTION__);
	for (int i = 0; i < NUBMER_OF_CLIENTS; i++)
    {
        if (i == NUBMER_OF_CLIENTS - 1)
        {
            PRINT_INFO("Свободных мест нет, количество пользователей = %d\n", NUBMER_OF_CLIENTS);
            break;
        }
        else if (users_db[i].user_pid == 0)
        {
        	users_db[i].user_sockaddr_size = new_user->peer_addr_size;
        	users_db[i].user_pid = new_user->user_pid;
        	users_db[i].users_sockaddr = new_user->peer_addr;
        	PRINT_DEBUG_INFO("users_db[%d].user_sockaddr_size: %d\nusers_db[%d].user_pid: %d\n", \
        					  i, users_db[i].user_sockaddr_size, i, users_db[i].user_pid);
        	return i;
        }
    }
    return 0;
}

int connection_new_user()
{
	PRINT_DEBUG_INFO(__FUNCTION__);
	c2s new_user;
	int user_number;
	socklen_t peer_addr_size = sizeof(new_user.peer_addr);
	if (recvfrom(server_ctx.listen_socket_description, &new_user.user_pid, sizeof(new_user.user_pid), 0, \
	             (struct sockaddr *)&new_user.peer_addr, &peer_addr_size) == -1)
	{
		PRINT_ERR("recvfrom server_ctx.listen_socket_description not success\n");
		close_all();
	}
	PRINT_DEBUG_INFO("pid new_user = %d", new_user.user_pid);
	user_number = legit_check_new_client(&new_user);
	create_socket_for_client(user_number);
	if (sendto(server_ctx.listen_socket_description, &users_db[user_number].users_sockaddr, sizeof(users_db[user_number].users_sockaddr), 0, \
		   (struct sockaddr *)&new_user.peer_addr, peer_addr_size) == -1)
	{
		PRINT_ERR("send server_ctx.listen_socket_description not success\n");
		close_all();
	}
	PRINT_DEBUG_INFO("Я отправил сообщение с данными для подключения\n");
	memset(&new_user, 0, sizeof(new_user));
	return user_number;
}

void init_server_struct()
{
	PRINT_DEBUG_INFO(__FUNCTION__);
	server_ctx.listen_addr.sin_family = AF_INET;
	server_ctx.listen_addr.sin_port = htons(SERVER_SOCK_NUMBER);
	server_ctx.listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
}

void init_listen_socket()
{
	PRINT_DEBUG_INFO(__FUNCTION__);
	server_ctx.listen_socket_description = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_ctx.listen_socket_description == -1)
	{
		PRINT_ERR("server_ctx.listen_socket_description not success\n");
		close_all();
	}
	init_server_struct();
	if (bind(server_ctx.listen_socket_description, (struct sockaddr *)&server_ctx.listen_addr, \
			 sizeof(server_ctx.listen_addr)) == -1)
	{
		PRINT_ERR("bind server_ctx.listen_socket_description not success\n");
		close_all();
	}
}

int main()
{
	init_listen_socket();
	int user_number = connection_new_user();
	start_new_message_exchange(user_number);
	close_socket();
	return 0;
}
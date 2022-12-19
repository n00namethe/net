#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string.h>
#include <mqueue.h>
#include <errno.h>
#include <fcntl.h>
#include "pull_serevers_type_2.h"

#define SERVICE_QUEUE "/Queue_serv"
#define MQ_MAX_NUM_OF_MESSAGES 10
#define DEFAULT_VALUE 0
#define PRIORITY_OF_QUEUE 1
#define BACKLOG 5
#define NUBMER_OF_CLIENT_SERVERS 5

struct sockaddr_in server, client;
int listen_socket = 0;
mqd_t serv_queue = 0;
pthread_t client_sock_pth[NUBMER_OF_CLIENT_SERVERS];

struct mq_attr attributes_mq = 
{
        .mq_flags = DEFAULT_VALUE,
        .mq_maxmsg = MQ_MAX_NUM_OF_MESSAGES,
        .mq_msgsize = sizeof(int),
        .mq_curmsgs = DEFAULT_VALUE
};

void fail_close_all()
{
    PRINT_DEBUG_INFO("here i am\n");
    for (int i = 0; i < NUBMER_OF_CLIENT_SERVERS; i++) 
    {
        pthread_cancel(client_sock_pth[i]);
    }
    close(serv_queue);
    close(listen_socket);
    exit(EXIT_FAILURE);
}

void *new_client()
{
    PRINT_DEBUG_INFO("here i am\n");
    int client_sock = 0;
    int recv_size = 0;
    int send_size = 0;
    char client_description[1] = {0};
    char msg[SIZE_OF_MSG];
    while (1) 
    {
        PRINT_DEBUG_INFO("Жду сообщение с ФД клиента от сервера\n");
        recv_size = mq_receive(serv_queue, client_description, sizeof(client_description), NULL);
        if (recv_size == -1)
        {
            PRINT_ERR("mq_receive serv_queue not success\n");
            fail_close_all();
        }
        client_sock = (int)client_description[0];
        PRINT_DEBUG_INFO("mq_receive size = %d\n", recv_size);
        PRINT_DEBUG_INFO("client_sock = %d\n", client_sock);

        recv_size = recv(client_sock, msg, sizeof(msg), 0);
        if (recv_size == -1) 
        {
            PRINT_ERR("recv client_sock not success\n");
            fail_close_all();
        }
        PRINT_INFO("recv msg: %s, recv_size = %d\n", msg, recv_size);
        strcat(msg, " World!");

        send_size = send(client_sock, msg, sizeof(msg), 0);
        if (recv_size == -1) 
        {
            PRINT_ERR("send client_sock not success\n");
            fail_close_all();
        }
        PRINT_INFO("semt msg: %s, send size = %d\n", msg, send_size);
        memset(msg, 0, sizeof(msg));
        close(client_sock);
    }
}

void addrin_filling()
{
    PRINT_DEBUG_INFO("here i am\n");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
}

void create_new_queue()
{
    PRINT_DEBUG_INFO("here i am\n");
    serv_queue = mq_open(SERVICE_QUEUE, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &attributes_mq);
    if (serv_queue == -1)
    {
        PRINT_ERR("mq_open serv_queue not success\n");
        exit(EXIT_FAILURE);
    }
}

void assigns_address_listen_socket()
{
    PRINT_DEBUG_INFO("here i am\n");
    if (bind(listen_socket, (struct sockaddr*)&server, sizeof(server)) == -1) 
    {
        PRINT_ERR("bind listen_socket not success\n");
        fail_close_all();
    }
}

void accept_incoming_connection_listen_sock()
{
    PRINT_DEBUG_INFO("here i am\n");
    if (listen(listen_socket, BACKLOG) == -1) 
    {
        PRINT_ERR("listen listen_socket not success\n");
        fail_close_all();
    }
}
void create_listen_socket()
{
    PRINT_DEBUG_INFO("here i am\n");
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == -1)
    {
        PRINT_ERR("create listen_socket not success\n");
        close(serv_queue);
        exit(EXIT_FAILURE);
    }
}

void create_clients_sock_pth()
{
    PRINT_DEBUG_INFO("here i am\n");
    for (int i = 0; i < NUBMER_OF_CLIENT_SERVERS; i++) 
    {
        if (pthread_create(&client_sock_pth[i], NULL, new_client, NULL) != 0) 
        {
            PRINT_ERR("pthread_create not success\n");
            fail_close_all();
        }
    }
}

void init_listen_socket()
{
    PRINT_DEBUG_INFO("here i am\n");
    create_new_queue();
    create_listen_socket();
    addrin_filling();
    assigns_address_listen_socket();
    create_clients_sock_pth();
    accept_incoming_connection_listen_sock();
}

int main()
{
    socklen_t client_size;
    int client_sock = 0;
    client_size = sizeof(client);
    char client_description[1] = {0};
    init_listen_socket();
    while (1) 
    {
        client_sock = accept(listen_socket, (struct sockaddr*)&client, &client_size);
        if (client_sock == -1) 
        {
            PRINT_ERR("accept listen_socket not success\n");
            fail_close_all();
        }

        client_description[0] = (char)client_sock;

        if (mq_send(serv_queue, client_description, sizeof(client_description), PRIORITY_OF_QUEUE) == -1)
        {
            PRINT_ERR("mq_send serv_queue not success\n");
            fail_close_all();
        }
        PRINT_DEBUG_INFO("client_sock = %d\n", client_sock);
        PRINT_DEBUG_INFO("clien_sock in array = %d\n", (int)client_description[0]);
        PRINT_DEBUG_INFO("mq_send size = %ld\n", sizeof(client_description));
    }
    for (int i = 0; i < NUBMER_OF_CLIENT_SERVERS; i++) 
    {
        pthread_join(client_sock_pth[i], NULL);
    }
    close(listen_socket);
    return 0;
}
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include "queue.h"
#include <semaphore.h>
#include "types.h"
#include "server.h"
#include <assert.h>

#define BACKLOG		10
#define MAX_CLIENTS	10

client_t	clients[MAX_CLIENTS];
queue_t 	data_queues[MAX_CLIENTS];
sem_t		tosend[MAX_CLIENTS];
pthread_t 	tid[MAX_CLIENTS];

int num_clients;

void main(int argc, char** argv)
{
	assert(argv[1] != NULL);

	int i;
	char buff[20];
	num_clients = 0;

	int server_fd = server_start(atoi(argv[1]));

	while(TRUE)
	{
		printf("Sever Waiting...\n");
		int len = sizeof(clients[num_clients].addr);
		int client_fd = accept(server_fd, (struct sockaddr *)&(clients[num_clients].addr), &len);
		printf("Newly accepted connection @ %s!!!\n", inet_ntop(AF_INET, &(clients[num_clients].addr.sin_addr), buff, sizeof(buff)));
		queue_create(&(data_queues[num_clients]));
		sem_init(&(tosend[num_clients]), 0, 0);
		clients[num_clients].fd = client_fd;
		clients[num_clients].id = num_clients;
		num_clients++;
		pthread_create(&(clients[num_clients-1].sender_tid), NULL, sender, &(clients[num_clients-1]));
		pthread_create(&(clients[num_clients-1].receiver_tid), NULL, receiver, &(clients[num_clients-1]));
	}
}

int server_start(int port_num)
{
	char buff[20];
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_addr;

	server_addr.sin_family 		= AF_INET;
	server_addr.sin_addr.s_addr 	= htonl(INADDR_ANY);
	//server_addr.sin_addr.s_addr	= inet_addr("172.24.145.141");
	server_addr.sin_port		= htons(port_num);
	
	bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	listen(server_fd, BACKLOG);
	printf("Started Server @ %s:%d!!!\n", inet_ntop(AF_INET, &server_addr.sin_addr, buff, sizeof(buff)), port_num);

	return server_fd;
}

void* receiver(void* cinfo)
{
	client_t* client = (client_t*) cinfo;
	char data;
	int i, nbytes_recv;
	printf("Receiver Info: %d	%d\n", client->fd, client->id);

	while(TRUE)
	{
		printf("Receiver%d: Entered...\n", client->id);
		nbytes_recv = recv(client->fd, (void*)&data, 1, 0);
		if(nbytes_recv == 0)
		{
			pthread_cancel(client->sender_tid);
			pthread_exit(NULL);
		}
		printf("Receiver%d: Received... %c\n", client->id, data);
		for(i=0; i<num_clients; i++)
		{
			if(i != client->id)
			{
				queue_enqueue(&(data_queues[i]), &data);
				printf("Receiver%d: Enqueued... %c\n", client->id, data);
				sem_post(&(tosend[i]));
			}
		}
	}
}

void* sender(void* cinfo)
{
	client_t* client = (client_t*) cinfo;
	char data;

	printf("Sender Info: %d	%d\n", client->fd, client->id);
	while(TRUE)
	{
		printf("Sender%d: Entered...\n", client->id);
		sem_wait(&(tosend[client->id]));
		printf("Sender%d: Tosend...\n", client->id);
		while(!queue_isempty(&(data_queues[client->id])))
		{
			queue_dequeue(&(data_queues[client->id]), &data);
			send(client->fd, (void*)&data, 1, 0);
			printf("Sender%d: Sent...%c\n", client->id, data);
		}
	}
}

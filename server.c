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

group_t groups[MAX_GROUPS];

void main(int argc, char** argv)
{
	assert(argv[1] != NULL);

	char buff[20];

	groups[0].num_clients = 0;
	groups[1].num_clients = 0;

	int server_fd = server_start(atoi(argv[1]));

	int group_id = 0;

	while(TRUE)
	{
		printf("Sever Waiting...\n");
		group_id = (group_id+1)%2;	// TODO: take group id from user and authenticate
		client_t* client = &(groups[group_id].clients[groups[group_id].num_clients]);
		int len = sizeof(client->addr);

		int client_fd = accept(server_fd, (struct sockaddr *)&(client->addr), &len);
		printf("Newly accepted connection @ %s!!!\n", inet_ntop(AF_INET, &(client->addr.sin_addr), buff, sizeof(buff)));
		client_init(client, client_fd, group_id);

		pthread_create(&(client->sender_tid), NULL, sender, (void*)client);
		pthread_create(&(client->receiver_tid), NULL, receiver, (void*)client);

		groups[group_id].num_clients++;
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

void client_init(client_t* client, int client_fd, int group_id)
{
	queue_create(&(client->data_queue));
	sem_init(&(client->tosend), 0, 0);

	client->fd = client_fd;
	client->id = groups[group_id].num_clients;
	client->gid = group_id;
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
		for(i=0; i<groups[client->gid].num_clients; i++)
		{
			if(i != client->id)
			{
				client_t* friend = &(groups[client->gid].clients[i]);
				queue_enqueue(&(friend->data_queue), &data);
				printf("Receiver%d: Enqueued... %c\n", client->id, data);
				sem_post(&(friend->tosend));
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
		sem_wait(&(client->tosend));
		printf("Sender%d: Tosend...\n", client->id);
		while(!queue_isempty(&(client->data_queue)))
		{
			queue_dequeue(&(client->data_queue), &data);
			send(client->fd, (void*)&data, 1, 0);
			printf("Sender%d: Sent...%c\n", client->id, data);
		}
	}
}

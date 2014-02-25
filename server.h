/**
 * File @ server.h
 *
 * Defines the interface for server functionalities.
 */

#ifndef		__SERVER_H__
#define		__SERVER_H__

#include <pthread.h>
#include <semaphore.h>

#define BACKLOG		10
#define MAX_CLIENTS	10
#define MAX_GROUPS	10

/**
 * client_t @ defines the structure containing info about client.
 *
 * id	    	@ id identifying the specific client
 * group_id	@ id of the group, to which client belongs
 * fd       	@ file descriptor corresponding to the specific client
 * addr	    	@ contains info about the address of the client
 * sender_tid 	@ thread id of the sender for corresponding client
 * receiver_tid @ thread id of the reciver for corresponding client 
 */

typedef struct
{
	int 			id;
	int			gid;
	int 			fd;
	struct sockaddr_in	addr;
	pthread_t		sender_tid;
	pthread_t		receiver_tid;
	queue_t			data_queue;
	sem_t			tosend;
}client_t;

/**
 * group_t @ defines the structure containing info about group.
 *
 * id	   	@ id identifying the group
 * clients 	@ Contains info about clients of the group
 * num_clients 	@ no. of clients in the group
 */

typedef struct{
	int		id;
	client_t	clients[MAX_CLIENTS];
	int 		num_clients;
}group_t;

/**
 * server_start @ Initializes the server.
 *		  Creates socket and binds it the server ip address and port num.
 *		  It also starts listening on that socket.
 */

int server_start(int port_num);

/**
 * client_init @ Initializes the client.
 *		  Creates queue and initializes semaphore & other fields.
 */

void client_init(client_t* client, int client_fd, int group_id);

/**
 * sender @ Main workhorse for sending process for the client
 *	    For each client, a sender thread is created and it runs this function.
 */

void* sender(void* cinfo);

/**
 * receiver @ Main workhorse for receiving process for the client
 *	    For each client, a receiver thread is created and it runs this function.
 */

void* receiver(void* cinfo); 

#endif

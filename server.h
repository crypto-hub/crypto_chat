/**
 * File @ server.h
 *
 * Defines the interface for server functionalities.
 */

#ifndef		__SERVER_H__
#define		__SERVER_H__

/**
 * client_t @ defines the structure containing info about client.
 *
 * id	    @ id identifying the specific client
 * fd       @ file descriptor corresponding to the specific client
 */

typedef struct
{
	int id;
	int fd;
}client_t;

/**
 * server_start @ Initializes the server.
 *		  Creates socket and binds it the server ip address and port num.
 *		  It also starts listening on that socket.
 */

int server_start(int port_num);

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

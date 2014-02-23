all: server.c queue.c
	gcc server.c queue.c -o server -lpthread

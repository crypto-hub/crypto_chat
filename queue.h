/**
 * File @ queue.h
 *
 * Defines the interface for circular queue.
 * 
 * It allows client to store/access data(i.e. char) in FIFO order.
 * Its implementation is based on fixed size array.
 * QMAX_SIZE-1 is the max length of queue.
 * Running time for queue operations: O(1)
 */

#ifndef		__QUEUE_H__
#define		__QUEUE_H__

#include "types.h"

#define		QMAX_SIZE	1024		//max queue size

/**
 * queue_t @ defines the structure containing info about queue.
 *
 * elems   @ array containing elements of queue 
 * front   @ index pointing at the element at the front of queue.
 * end	   @ index pointing at next empty spot at the end of queue.
 */

typedef struct
{
	char 	elems[QMAX_SIZE];
	int 	front;
	int 	end;
}queue_t;

/**
 * queue_create @ initializes the fields of queue structure.
 */

void queue_create(queue_t* q);

/**
 * queue_destroy @ de-initializes the fields of queue and frees up any associated memory.
 */

void queue_destroy(queue_t* q);

/**
 * queue_enqueue @ Enqueues the element at end of the queue.
 *                 It ignores if queue is full.
 */

void queue_enqueue(queue_t* q, char* elem);

/**
 * queue_dequeue @ Dequeues the element from front of the queue.
 *                 It ignores if queue is empty.
 */

void queue_dequeue(queue_t* q, char *elem);

/**
 * queue_isempty @ Returns whether queue is empty or not.
 * 		   It is empty when front == end.
 */

bool queue_isempty(queue_t* q);

/**
 * queue_isfull @ Returns whether queue is full or not.
 *                It is full when end is just one position behind front.
 */

bool queue_isfull(queue_t* q);

#endif

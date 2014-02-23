#include <stdio.h>
#include <assert.h>
#include "queue.h"

void queue_create(queue_t* q)
{
	assert(q != NULL);
	
	q->front = 0;
	q->end   = 0;
}

void queue_delete(queue_t* q)
{
	assert(q != NULL);
}

void queue_enqueue(queue_t* q, char* elem)
{
	assert(q != NULL);

	if(!queue_isfull(q))
	{
		q->elems[q->end] = *elem;
		q->end = (q->end + 1) % QMAX_SIZE;
	}
}

void queue_dequeue(queue_t* q, char *elem)
{
	assert(q != NULL);

	if(!queue_isempty(q))
	{
		*elem = q->elems[q->front];
		q->front = (q->front + 1) % QMAX_SIZE;
	}
}

bool queue_isempty(queue_t* q)
{
	assert(q != NULL);

	return q->front == q->end;
}

bool queue_isfull(queue_t* q)
{
	assert(q != NULL);

	return (q->end - q->front) == -1;
}

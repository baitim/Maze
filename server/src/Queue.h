#ifndef SERVER_SRC_QUEUE_H
#define SERVER_SRC_QUEUE_H

#include "Errors.h"

typedef struct Queue_t_ {
    int x;
    int y;
    struct Queue_t_* next;
} Queue_t;

ErrorCode enqueue   (Queue_t** queue, int x, int y);
int       dequeue   (Queue_t** queue, int* x, int* y);
void      dtor_queue(Queue_t* queue, Queue_t* head);

#endif // SERVER_SRC_QUEUE_H
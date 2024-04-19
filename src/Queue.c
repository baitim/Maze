#include <stdlib.h>
#include <stdio.h>
#include "Queue.h"

ErrorCode enqueue(Queue_t** queue, int x, int y)
{
    Queue_t* new_queue = (Queue_t*) malloc(sizeof(Queue_t));
    if (!new_queue) return ERROR_ALLOC_FAIL;

    if (!*queue) {
        *new_queue = (Queue_t) {x ,y, new_queue};
        *queue = new_queue;
    }
    else {
        *new_queue = (Queue_t) {x, y, (*queue)->next};
        (*queue)->next = new_queue;
        *queue = (*queue)->next;
    }
    return ERROR_NO;
}

int dequeue(Queue_t** queue, int* x, int* y)
{
    Queue_t* next;
    if (!*queue) return 1;
    next = (*queue)->next;
    *x = next->x;
    *y = next->y;
    if (*queue == (next)) {
        free(*queue);
        *queue = NULL;
        return 0;
    } else {    
        (*queue)->next = next->next;
        free(next);
        return 0;
    }
}

void dtor_queue(Queue_t* queue, Queue_t* head)
{
    if (!queue) return;

    if (queue->next == head) {
        free(queue);
        return;
    }

    dtor_queue(queue->next, head);
    free(queue);
}
#ifndef SRC_QUEUE_H
#define SRC_QUEUE_H

typedef struct Queue_t_ {
    int x;
    int y;
    struct Queue_t_* next;
} Queue_t;

int  enqueue    (Queue_t** queue, int x, int y);
int  dequeue    (Queue_t** queue, int* x, int* y);
void dtor_queue (Queue_t* queue, Queue_t* head);

#endif // SRC_QUEUE_H
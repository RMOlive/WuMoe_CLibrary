#include <stdlib.h>
#include "queue.h"

typedef struct Queue_Node {
    void *value;
    struct Queue_Node *father;
} Queue_Node;

typedef struct Queue {
    Queue_Node *tail;
    size_t size;
} Queue;

Queue *new_Queue() {
    Queue *new_Queue = (Queue *) malloc(sizeof(Queue));
    new_Queue->tail = NULL;
    new_Queue->size = 0;
}

size_t queue_size(Queue *queue) {
    return queue->size;
}

void *queue_peek(Queue *queue) {
    if (queue->tail == NULL)
        return NULL;
    return queue->tail->value;
}

void *queue_pop(Queue *queue) {
    void *result = queue_peek(queue);
    if (result == NULL)
        return NULL;
    Queue_Node *father = queue->tail->father;
    free(queue->tail);
    queue->tail = father;
    --queue->size;
    return result;
}

void queue_push(Queue *queue, void *value) {
    Queue_Node *new_node = (Queue_Node *) malloc(sizeof(Queue_Node));
    new_node->value = value;
    new_node->father = NULL;
    ++queue->size;
    if (queue->tail == NULL) {
        queue->tail = new_node;
        return;
    }
    new_node->father = queue->tail;
    queue->tail = new_node;
}

void Queue_free(Queue *Queue) {
    while (Queue->size > 0)
        queue_pop(Queue);
    free(Queue);
}

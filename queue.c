#include <stdlib.h>
#include "queue.h"

typedef struct Queue_Node {
    void *value;
    struct Queue_Node *next;
} Queue_Node;

typedef struct Queue {
    Queue_Node *head;
    size_t size;
} Queue;

Queue *new_queue() {
    Queue *new_Queue = (Queue *) malloc(sizeof(Queue));
    new_Queue->head = NULL;
    new_Queue->size = 0;
}

size_t queue_size(Queue *queue) {
    return queue->size;
}

void *queue_peek(Queue *queue) {
    if (queue->head == NULL)
        return NULL;
    return queue->head->value;
}

void *queue_pop(Queue *queue) {
    void *result = queue_peek(queue);
    if (result == NULL)
        return NULL;
    Queue_Node *next = queue->head->next;
    free(queue->head);
    queue->head = next;
    --queue->size;
    return result;
}

void queue_push(Queue *queue, void *value) {
    Queue_Node *new_node = (Queue_Node *) malloc(sizeof(Queue_Node));
    new_node->value = value;
    new_node->next = NULL;
    ++queue->size;
    Queue_Node * temp = queue->head;
    if(!temp)
        queue->head = new_node;
    else {
        while(temp->next)
            temp = temp->next;
        temp->next = new_node;
    }
}

void queue_free(Queue *queue) {
    while (queue->size > 0)
        queue_pop(queue);
    free(queue);
}

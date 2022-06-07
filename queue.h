#ifndef __WUMOE_QUEUE_H
#define __WUMOE_QUEUE_H

#include <stddef.h>

typedef struct Queue Queue;

Queue *new_queue();

size_t queue_size(Queue *stack);

void *queue_peek(Queue *stack);

void *queue_pop(Queue *stack);

void queue_push(Queue *stack, void *value);

void queue_free(Queue *stack);

#endif

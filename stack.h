#ifndef __WUMOE_STACK_H
#define __WUMOE_STACK_H

typedef struct Stack Stack;

Stack *new_stack();

size_t stack_size(Stack *stack);

void *stack_peek(Stack *stack);

void *stack_pop(Stack *stack);

void stack_push(Stack *stack, void *value);

void stack_free(Stack *stack);

#endif

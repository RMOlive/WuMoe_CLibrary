#include <stdlib.h>
#include "stack.h"

typedef struct Stack_Node {
    void *value;
    struct Stack_Node *next;
} Stack_Node;

typedef struct Stack {
    Stack_Node *head;
    size_t size;
} Stack;

Stack *new_stack() {
    Stack *new_stack = (Stack *) malloc(sizeof(Stack));
    new_stack->head = NULL;
    new_stack->size = 0;
}

size_t stack_size(Stack *stack) {
    return stack->size;
}

void *stack_peek(Stack *stack) {
    if (stack->head == NULL)
        return NULL;
    return stack->head->value;
}

void *stack_pop(Stack *stack) {
    void *result = stack_peek(stack);
    if (result == NULL)
        return NULL;
    Stack_Node *next = stack->head->next;
    free(stack->head);
    stack->head = next;
    --stack->size;
    return result;
}

void stack_push(Stack *stack, void *value) {
    Stack_Node *new_node = (Stack_Node *) malloc(sizeof(Stack_Node));
    new_node->value = value;
    new_node->next = NULL;
    ++stack->size;
    if (stack->head == NULL) {
        stack->head = new_node;
        return;
    }
    new_node->next = stack->head;
    stack->head = new_node;
}

void stack_free(Stack *stack) {
    while (stack->size > 0)
        stack_pop(stack);
    free(stack);
}

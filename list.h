#ifndef __WUMOE_LIST_H
#define __WUMOE_LIST_H

#include <stddef.h>

typedef struct List List;

List *new_list();

void list_add(List *list, void *value);

void list_set(List *list, size_t index, void *value);

void list_insert(List *list, size_t index, void *value);

size_t list_size(List *list);

void *list_get(List *list, size_t index);

void *list_remove(List *list, size_t index);

void list_free(List *list);

#endif

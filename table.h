#ifndef __WUMOE_TABLE_H
#define __WUMOE_TABLE_H

typedef struct Table Table;

Table *new_table();

size_t table_size(Table *table);

int table_contains(Table *table, char *key);

void table_push(Table *table, char *key, void *value);

void *table_get(Table *table, char *key);

void *table_remove(Table *table, char *key);

char **table_keys(Table *table);

void **table_values(Table *table);

void table_free(Table *table);

#endif

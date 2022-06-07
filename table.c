#include <stdlib.h>
#include <string.h>
#include "table.h"

#define TABLE_DEFAULT_CAPACITY  (1 << 4)
#define TABLE_DEFAULT_LODER     0.75f
#define TABLE_MAX_CAPACITY      (1 << 30)

#define HEIGHT(node)        (node == NULL ? 0 : node->height)
#define MAX(a, b)           (a > b ? a : b)
#define GET_BALANCE(node)   (node == NULL ? 0 : HEIGHT(node->left) - HEIGHT(node->right))

typedef struct Table_Entry {
    unsigned int hash;
    char *key;
    void *value;
    struct Table_Entry *next;
} Table_Entry;

typedef struct AVL_Tree_Node {
    unsigned int key;
    int height;
    Table_Entry *entry;
    struct AVL_Tree_Node *left;
    struct AVL_Tree_Node *right;
} AVL_Tree_Node;

typedef struct Table {
    AVL_Tree_Node **avl_array;
    size_t capacity;
    size_t size;
} Table;

unsigned int table_hash(char *key);

void table_resize(Table *table);

AVL_Tree_Node *new_avl_node(unsigned int key, Table_Entry *value);

AVL_Tree_Node *avl_push(AVL_Tree_Node *node, unsigned int key, Table_Entry *entry);

AVL_Tree_Node *avl_remove(AVL_Tree_Node *base, unsigned int key);

Table_Entry *avl_get_node(AVL_Tree_Node *node, unsigned int key);

void avl_free(AVL_Tree_Node *node);

Table *new_table() {
    Table *new_table = (Table *) malloc(sizeof(Table));
    new_table->capacity = TABLE_DEFAULT_CAPACITY;
    new_table->size = 0;
    new_table->avl_array = (AVL_Tree_Node **) malloc(sizeof(AVL_Tree_Node *) * new_table->capacity);
    for (size_t i = 0; i < new_table->capacity; ++i)
        new_table->avl_array[i] = NULL;
    return new_table;
}

size_t table_size(Table *table) {
    return table->size;
}

int table_contains(Table *table, char *key) {
    unsigned int key_hash = table_hash(key) & (table->capacity - 1);
    if (table->avl_array[key_hash] == NULL)
        return 0;
    AVL_Tree_Node *avl = table->avl_array[key_hash];
    Table_Entry *entry = avl_get_node(avl, table_hash(key));
    while (entry != NULL && strcmp(key, entry->key) != 0)
        entry = entry->next;
    if (entry == NULL)
        return 0;
    return 1;
}

void *table_push(Table *table, char *key, void *value) {
    ++table->size;
    if (table->size >= table->capacity * TABLE_DEFAULT_LODER)
        table_resize(table);
    unsigned int key_hash = table_hash(key) & (table->capacity - 1);
    Table_Entry *new_entry = (Table_Entry *) malloc(sizeof(Table_Entry));
    new_entry->hash = table_hash(key);
    char *new_key = (char *) malloc(sizeof(char) * strlen(key));
    strcpy(new_key, key);
    new_entry->key = new_key;
    new_entry->value = value;
    new_entry->next = NULL;
    if (table->avl_array[key_hash] == NULL) {
        table->avl_array[key_hash] = new_avl_node(new_entry->hash, new_entry);
        goto end;
    }
    Table_Entry *entry = avl_get_node(table->avl_array[key_hash], new_entry->hash);
    if (entry == NULL) {
        table->avl_array[key_hash] = avl_push(table->avl_array[key_hash], new_entry->hash, new_entry);
        goto end;
    }
    Table_Entry *previous = NULL;
    while (entry != NULL && strcmp(new_key, entry->key) != 0) {
        previous = entry;
        entry = entry->next;
    }
    if (previous == NULL) {
        if (strcmp(new_key, entry->key) == 0) {
            table->avl_array[key_hash] = avl_remove(table->avl_array[key_hash], new_entry->hash);
            if (entry->next != NULL)
                table->avl_array[key_hash] = avl_push(table->avl_array[key_hash], new_entry->hash, entry->next);
        } else {
            Table_Entry *backups = entry;
            while (backups->next != NULL)
                backups = backups->next;
            backups->next = new_entry;
        }
    } else if (entry != NULL) {
        free(previous->next->key);
        free(previous->next);
        previous->next = new_entry;
        new_entry->next = entry->next;
    } else
        previous->next = new_entry;
    end:
    return value;
}

void *table_get(Table *table, char *key) {
    unsigned int key_hash = table_hash(key) & (table->capacity - 1);
    if (table->avl_array[key_hash] == NULL)
        return NULL;
    Table_Entry *entry = avl_get_node(table->avl_array[key_hash], table_hash(key));
    while (entry != NULL && strcmp(key, entry->key) != 0)
        entry = entry->next;
    if (entry == NULL)
        return NULL;
    return entry->value;
}

void *table_remove(Table *table, char *key) {
    --table->size;
    unsigned int key_hash = table_hash(key) & (table->capacity - 1);
    if (table->avl_array[key_hash] == NULL)
        return NULL;
    Table_Entry *entry = avl_get_node(table->avl_array[key_hash], table_hash(key));
    Table_Entry *previous = NULL;
    while (entry != NULL && strcmp(key, entry->key) != 0) {
        previous = entry;
        entry = entry->next;
    }
    if (entry == NULL)
        return NULL;
    void *result = entry->value;
    if (previous == NULL) {
        table->avl_array[key_hash] = avl_remove(table->avl_array[key_hash], table_hash(key));
        if (entry->next != NULL) {
            table->avl_array[key_hash] = avl_push(table->avl_array[key_hash], table_hash(key), entry->next);
            free(entry->key);
            free(entry);
        } else {
            avl_free(table->avl_array[key_hash]);
            table->avl_array[key_hash] = NULL;
        }

        return result;
    } else {
        previous->next = entry->next;
        free(entry->key);
        free(entry);
    }
    return result;
}

size_t table_keys_index;

void table_keys_order(char **keys, AVL_Tree_Node *node) {
    if(node != NULL) {
        keys[table_keys_index++] = node->entry->key;
        table_keys_order(keys, node->left);
        table_keys_order(keys, node->right);
    }
}

char **table_keys(Table *table) {
    char **keys = (char **) malloc(sizeof(char *) * table->size);
    table_keys_index = 0;
    for (size_t i = 0; i < table->capacity; ++i)
        table_keys_order(keys, table->avl_array[i]);
    return keys;
}

size_t table_values_index;

void table_values_order(void **values, AVL_Tree_Node *node) {
    if(node != NULL) {
        values[table_values_index++] = node->entry->value;
        table_values_order(values, node->left);
        table_values_order(values, node->right);
    }
}

void **table_values(Table *table) {
    void **values = (void **) malloc(sizeof(void *) * table->size);
    table_values_index = 0;
    for (size_t i = 0; i < table->capacity; ++i)
        table_values_order(values, table->avl_array[i]);
    return values;
}

void table_free(Table *table) {
    for (size_t i = 0; i < table->capacity; ++i)
        avl_free(table->avl_array[i]);
    free(table);
}

void table_avl_copy(Table *table, AVL_Tree_Node *node) {
    if (node != NULL) {
        table_push(table, node->entry->key, node->entry->value);
        table_avl_copy(table, node->left);
        table_avl_copy(table, node->right);
        if (node->left == NULL && node->right == NULL)
            free(node);
    }
}

void table_resize(Table *table) {
    if (TABLE_MAX_CAPACITY - table->capacity < table->capacity)
        return;
    size_t backups_capacity = table->capacity;
    AVL_Tree_Node **backups_avl_array = table->avl_array;
    table->capacity = table->capacity << 1;
    table->size = 0;
    table->avl_array = (AVL_Tree_Node **) malloc(sizeof(AVL_Tree_Node *) * table->capacity);
    for (size_t i = 0; i < table->capacity; ++i)
        table->avl_array[i] = NULL;
    for (size_t i = 0; i < backups_capacity; ++i)
        if (backups_avl_array[i] != NULL)
            table_avl_copy(table, backups_avl_array[i]);
    free(backups_avl_array);
}

unsigned int table_hash(char *key) {
    char *k = key;
    unsigned int hash = 0;
    while (*k != '\0') {
        hash += *k++;
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

AVL_Tree_Node *new_avl_node(unsigned int key, Table_Entry *entry) {
    AVL_Tree_Node *new = (AVL_Tree_Node *) malloc(sizeof(AVL_Tree_Node));
    new->key = key;
    new->entry = entry;
    new->left = NULL;
    new->right = NULL;
    new->height = 1;
    return new;
}

AVL_Tree_Node *avl_right_rotate(AVL_Tree_Node *y) {
    AVL_Tree_Node *x = y->left;
    AVL_Tree_Node *T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = MAX(HEIGHT(y->left), HEIGHT(y->right)) + 1;
    x->height = MAX(HEIGHT(x->left), HEIGHT(x->right)) + 1;
    return x;
}

AVL_Tree_Node *avl_left_rotate(AVL_Tree_Node *x) {
    AVL_Tree_Node *y = x->right;
    AVL_Tree_Node *T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = MAX(HEIGHT(x->left), HEIGHT(x->right)) + 1;
    y->height = MAX(HEIGHT(y->left), HEIGHT(y->right)) + 1;
    return y;
}

AVL_Tree_Node *avl_push(AVL_Tree_Node *node, unsigned int key, Table_Entry *value) {
    if (node == NULL)
        return new_avl_node(key, value);
    if (key < node->key)
        node->left = avl_push(node->left, key, value);
    else if (key > node->key)
        node->right = avl_push(node->right, key, value);
    else
        return node;
    node->height = MAX(HEIGHT(node->left), HEIGHT(node->right)) + 1;
    int balance = GET_BALANCE(node);
    if (balance > 1 && key < node->left->key)
        return avl_right_rotate(node);
    if (balance < -1 && key > node->right->key)
        return avl_left_rotate(node);
    if (balance > 1 && key > node->left->key) {
        node->left = avl_left_rotate(node->left);
        return avl_right_rotate(node);
    }
    if (balance < -1 && key < node->right->key) {
        node->right = avl_right_rotate(node->right);
        return avl_left_rotate(node);
    }
    return node;
}

AVL_Tree_Node *avl_min_key_node(AVL_Tree_Node *node) {
    AVL_Tree_Node *current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

AVL_Tree_Node *avl_remove(AVL_Tree_Node *base, unsigned int key) {
    if (base == NULL)
        return base;
    if (key < base->key)
        base->left = avl_remove(base->left, key);
    else if (key > base->key)
        base->right = avl_remove(base->right, key);
    else {
        if ((base->left == NULL) || (base->right == NULL)) {
            AVL_Tree_Node *temp = base->left ? base->left : base->right;
            if (temp == NULL) {
                temp = base;
                base = NULL;
            } else
                *base = *temp;
            free(temp);
        } else {
            AVL_Tree_Node *temp = avl_min_key_node(base->right);
            base->key = temp->key;
            base->right = avl_remove(base->right, temp->key);
        }
    }
    if (base == NULL)
        return base;
    base->height = MAX(HEIGHT(base->left), HEIGHT(base->right)) + 1;
    int balance = GET_BALANCE(base);
    if (balance > 1 && GET_BALANCE(base->left) >= 0)
        return avl_right_rotate(base);
    if (balance > 1 && GET_BALANCE(base->left) < 0) {
        base->left = avl_left_rotate(base->left);
        return avl_right_rotate(base);
    }
    if (balance < -1 && GET_BALANCE(base->right) <= 0)
        return avl_left_rotate(base);
    if (balance < -1 && GET_BALANCE(base->right) > 0) {
        base->right = avl_right_rotate(base->right);
        return avl_left_rotate(base);
    }
    return base;
}

Table_Entry *avl_get_node(AVL_Tree_Node *node, unsigned int key) {
    if (node == NULL)
        return NULL;
    if (key == node->key)
        return node->entry;
    if (key > node->key)
        return avl_get_node(node->right, key);
    return avl_get_node(node->left, key);
}

void avl_free(AVL_Tree_Node *node) {
    if (node != NULL) {
        if (node->left != NULL)
            avl_free(node->left);
        if (node->right != NULL)
            avl_free(node->right);
        free(node->entry->key);
        free(node->entry);
        free(node);
    }
}

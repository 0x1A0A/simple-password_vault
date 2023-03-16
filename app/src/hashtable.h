#ifndef __HASH_TABLE__
#define __HASH_TABLE__

#include <stdio.h>

typedef struct df_command command_t;
typedef struct df_hashtable hashtable_t;

struct df_command {
	char name[8];
	int (*callback)(void*);
};

struct df_hashtable {
	size_t SIZE;
	command_t *table;
	size_t (*function)(void*);
};

hashtable_t *hashtable_create(const int size);
void hashtable_destroy(hashtable_t *table);
command_t *hashtable_is_empty(const char *name, hashtable_t *table);
command_t *hashtable_search(const char *name, hashtable_t *table);
int hashtable_insert(const char *name, int (*fn)(void*), hashtable_t *table);

#endif
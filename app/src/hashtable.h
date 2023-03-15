#ifndef __HASH_TABLE__
#define __HASH_TABLE__

typedef struct df_command command_t;
typedef struct df_hashtable hashtable_t;

struct df_command {
	char name[8];
	int (*callback)(void*);
};

struct df_hashtable {
	int SIZE;
	command_t *table;
	int (*function)(void*);
};

hashtable_t *hashtable_create(const int size);
void *hashtable_destroy(hashtable_t *table);
command_t *hashtable_search(const char *name, hashtable_t *table);
int hashtable_insert(const char *name, int (*fn)(void*), hashtable_t *table);

#endif
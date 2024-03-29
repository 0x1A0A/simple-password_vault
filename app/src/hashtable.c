#include "hashtable.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

hashtable_t *hashtable_create(const int size)
{
	hashtable_t *neww = (hashtable_t*)malloc(sizeof(hashtable_t));
	neww->SIZE = size;
	neww->table = (command_t*)malloc(sizeof(command_t) * size);
	memset(neww->table, 0, sizeof(command_t) * size);
	neww->function = NULL;

	return neww;
}

void hashtable_destroy(hashtable_t *table)
{
	free(table->table);
	table->table = NULL;
	free(table);
	table = NULL;
}

command_t *hashtable_is_empty(const char *name, hashtable_t *table)
{
	size_t n = table->function( (void*)name )%table->SIZE;
	if ( !table->table[n].callback ) return NULL;
	return table->table+n;
}

command_t *hashtable_search(const char *name, hashtable_t *table)
{
	command_t *c = hashtable_is_empty(name, table);
	if (c && !strcmp( name, c->name )) return c;
	return NULL;
}

int hashtable_insert(const char *name, int (*fn)(void*), hashtable_t *table)
{
	command_t *c = hashtable_is_empty( name, table );
	if (c) { return -1; } // used space

	int n = table->function( (void*)(name) )%table->SIZE;
	strncpy(table->table[n].name, name, strlen(name));
	table->table[n].callback = fn;

	return n;
}

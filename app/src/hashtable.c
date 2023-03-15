#include "hashtable.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

command_t command_create(const char *name, int (*fn)(void*))
{
	command_t neww;
	strncpy( neww.name, name, strlen(name) );
	neww.callback = fn;

	return neww;
}

hashtable_t *hashtable_create(const int size)
{
	hashtable_t *neww = (hashtable_t*)malloc(sizeof(hashtable_t));
	neww->SIZE = size;
	neww->table = (command_t*)malloc(sizeof(command_t) * size);
	memset(neww->table, 0, sizeof(command_t) * size);
	neww->function = NULL;

	return neww;
}

void *hashtable_destroy(hashtable_t *table)
{
	free(table->table);
	table->table = NULL;
	free(table);
	table = NULL;
}

command_t *hashtable_search(const char *name, hashtable_t *table)
{
	int n = table->function( (void*)name )%table->SIZE;

	if ( !table->table[n].callback ) return NULL;

	if (!strcmp(name, table->table[n].name)) {
		return table->table+n;
	}

	return NULL;
}

int hashtable_insert(command_t command, hashtable_t *table)
{
	command_t *c = hashtable_search( command.name, table );
	if (c) { return -1; } // used space

	int n = table->function( (void*)(command.name) )%table->SIZE;
	table->table[n] = command;

	return n;
}

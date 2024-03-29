#include "email.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

email_t *email_create(const char *email, uint8_t email_length)
{
	if ( !email ) return NULL;

	uint8_t i = 0;
	email_t *nemail = NULL;

	while ( ++i ) {
		if ( email[i] == '@' ) {
			nemail = (email_t*)malloc(sizeof(email_t));

			if (!nemail) {
				fprintf(stderr, "can't allocate email\n");
				return NULL;
			}

			nemail->nl = i;
			nemail->dl = email_length-i-1;

			nemail->local = (char*)malloc( email_length );
			
			memcpy( nemail->local, email, email_length );
			nemail->domain = nemail->local + nemail->nl + 1;
			// memcpy( nemail->domain, email+i+1, nemail->dl);
			
			nemail->next = NULL;

			break;
		} 
	}

	if (!nemail) {
		fprintf(stderr,"Invalid Email!\n");
		return NULL;
	}

	nemail->id = 0;
	return nemail;
}

email_list_t* email_list_create() 
{
	email_list_t *n = (email_list_t *)malloc(sizeof(email_list_t));

	if ( !n ) {
		fprintf(stderr, "can't allocate email list\n");
		return NULL;
	}

	n->head = NULL;
	n->count = 0;

	return n;
}

void email_destroy( email_t *email ) 
{
	if ( email ) {
		memset(email->local, 0xff, email->nl + email->dl + 1);
		free( email->local );
		email->next = NULL;
		free(email);
	}
}

void email_list_reset(email_list_t *list)
{
	if (list) {
		email_t *temp;

		while (list->head) {
			temp = list->head;
			list->head = temp->next;
			email_destroy(temp);
		}
	}
	list->head = NULL;
	list->count = 0;
}

void email_list_destroy(email_list_t *list)
{
	email_list_reset(list);
	free(list);
}

int email_string_is_equal( const char *email_str, email_t *email )
{
	if ( !email ) return 999;

	int res = strncmp(email_str, email->local, email->nl);
	if (res) return 0;

	return !strncmp(email_str + email->nl+1, email->domain, email->dl);
}

void email_add(email_t *email, email_list_t *list)
{
	uint8_t id = 1;
	if (list) {
		email_t **trace = &(list->head);

		while (*trace) {
			if ((*trace)->id != id) {
				email->next = *trace;
				*trace = email;
				break;
			}
			trace = &((*trace)->next);
			++id;
		}

		email->id = id;
		(*trace) = email;

		list->count++;
	}
}

void email_remove( email_t *email, email_list_t *list )
{
	if (list && email) {
		email_t **trace = &(list->head), *temp;

		while (*trace) {
			if ( *trace == email ) { // target
				temp = *trace;
				(*trace) = temp->next;
				email_destroy(temp);
				list->count--;
				break; 
			}

			trace = &((*trace)->next);
		}
	}
}

void email_remove_id( const int id, email_list_t *list )
{
	if (list) {
		email_t **trace = &(list->head), *temp;

		while (*trace) {
			if ( (*trace)->id == id ) { // target
				temp = *trace;
				(*trace) = temp->next;
				email_destroy(temp);
				list->count--;
				break; 
			}

			trace = &((*trace)->next);
		}
	}
}

void email_remove_str(const char *email_str, email_list_t *list)
{
	if (list && email_str) {
		email_t **trace = &(list->head), *temp;

		while (*trace) {
			if ( email_string_is_equal(email_str, *trace) ) { // target
				temp = *trace;
				(*trace) = temp->next;
				email_destroy(temp);
				list->count--;
				break; 
			}

			trace = &((*trace)->next);
		}
	}
}

int8_t email_exits( const uint8_t id, email_list_t *list )
{
	if ( list ) {
		email_t **trace = &list->head;

		while (*trace) {
			if ( (*trace)->id == id ) return 1;
			trace = &(*trace)->next;
		}
	}

	return 0;
}

int8_t email_exits_name( const char *name, email_list_t *list )
{
	if ( list ) {
		email_t **trace = &list->head;

		while (*trace) {
			if ( !strncmp( name, (*trace)->local, (*trace)->dl + (*trace)->nl + 1 ) ) 
				return 1;
			trace = &(*trace)->next;
		}
	}

	return 0;
}

uint8_t email_get_id( const char *name, email_list_t *list )
{
	if ( list ) {
		email_t **trace = &list->head;

		while (*trace) {
			if ( strncmp( name, (*trace)->local, (*trace)->dl + (*trace)->nl + 1 ) ) 
				return (*trace)->id;
			trace = &(*trace)->next;
		}
	}

	return 0;
}

email_t * email_find_name(const char *name, email_list_t *list)
{
	if ( list ) {
		email_t **trace = &list->head;

		while (*trace) {
			if ( !strncmp( name, (*trace)->local, (*trace)->dl + (*trace)->nl + 1 ) ) return *trace;
			trace = &(*trace)->next;
		}
	}

	return NULL;
}

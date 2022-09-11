#include "account.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

account_t * account_create(	const char *name, const char *user, const char *password, 
							uint8_t name_length, uint8_t user_length, uint8_t password_length)
{
	if (name) {
		account_t *ac = (account_t*)malloc(sizeof(account_t));

		if ( !ac ) {
			fprintf(stderr, "can't allocate account\n");
			return NULL;
		}

		ac->name = (char*)malloc( name_length + user_length + password_length );
		ac->user = ac->name + name_length;
		ac->password = ac->user + user_length;
	
		ac->nl = name_length;
		ac->ul = user_length;
		ac->pl = password_length;
	
		strncpy( ac->name, name, name_length );
		strncpy( ac->user, user, user_length );
		strncpy( ac->password, password, password_length );

		ac->next = NULL;

		return ac;
	}

	return NULL;
}

account_list_t *account_list_create()
{
	account_list_t *list = (account_list_t*)malloc(sizeof(account_list_t));

	if ( !list ) {
		fprintf(stderr, "can't allocate account list\n");
		return NULL;
	}

	list->head = NULL;
	list->count = 0;
	
	return list;
}

void account_destroy ( account_t *account)
{
	if (account) {
		memset(account->name, 0xff, account->nl);
		free(account->name);
		free(account);
	}
}

void account_list_destroy(account_list_t *list)
{
	if (list) {
		account_t *temp;
		while(list->head) {
			temp = list->head;
			list->head = temp->next;
			account_destroy(temp);
		}
		free(list);
	}
}

void account_add( account_t *account, account_list_t *list )
{
	if (list) {
		account_t **trace = &list->head;
		
		while(*trace) trace = &(*trace)->next;

		*trace = account;

		list->count++;
	}
}

void account_remove( account_t *account, account_list_t *list )
{
	if (list && account) {
		account_t **trace = &list->head, *temp;
		
		while(*trace) {
			if (*trace == account) {
				temp = *trace;
				*trace = temp->next;
				account_destroy(temp);
				list->count--;
				break;
			}
			trace = &(*trace)->next;
		}

	}
}

void account_remove_str( const char *name, account_list_t *list )
{
	if (list && name) {
		account_t **trace = &list->head, *temp;
		
		while(*trace) {
			if ( strncmp( name, (*trace)->name, (*trace)->nl ) ) {
				temp = *trace;
				*trace = temp->next;
				account_destroy(temp);
				list->count--;
				break;
			}
			trace = &(*trace)->next;
		}

	}
}

void account_link_email( account_t *account, uint8_t email_id, email_list_t *list )
{
	if ( account && email_exits(email_id, list)) {
		account->email_id = email_id;
	}
}

void account_tag( account_t *account, uint8_t tag_id, tag_list_t *list)
{
	if ( account && tag_exits(tag_id, list)) {
		account->tag_id = tag_id;
	}
}

#ifndef __DF_PSM_ACCOUNT_H__
#define __DF_PSM_ACCOUNT_H__

#include "tag.h"
#include "email.h"
#include <stdint.h>

typedef struct df_account account_t;
typedef struct df_account_list account_list_t;

// account name suppose to be unique
struct df_account {
	char *name, *user, *password;
	account_t *next;
	uint8_t nl, ul, pl, email_id, tag_id;
};

struct df_account_list {
	account_t *head;
	uint8_t count;
};

account_t * account_create(	const char *name, const char *user, const char *password, 
							uint8_t name_length, uint8_t user_length, uint8_t password_length);
account_list_t *account_list_create();
void account_destroy ( account_t *account);
void account_list_destroy(account_list_t *list);
void account_list_reset(account_list_t *list);

void account_add( account_t *account, account_list_t *list );
void account_remove( account_t *account, account_list_t *list );
void account_remove_str( const char *name, account_list_t *list );

void account_link_email( account_t *account, uint8_t email_id, email_list_t *list);
void account_tag( account_t *account, uint8_t tag_id, tag_list_t *list);

account_t * account_find_name(const char *name, account_list_t *list);

#endif
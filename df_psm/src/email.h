#ifndef __DF_PSM_EMAIL_H__
#define __DF_PSM_EMAIL_H__

#include <stdint.h>

typedef struct df_email_address email_t;
typedef struct df_email_list email_list_t;

// struct for email address
struct df_email_address {
	char *local, *domain, *password;
	email_t *next;
	uint8_t nl, dl, pl, id;
};

// linked list wrapper for email addresses
struct df_email_list {
	email_t *head;
	uint8_t count;
};

email_t * email_create(const char *email, const char *password, uint8_t email_length, uint8_t password_length);
email_list_t *email_list_create();
void email_destroy ( email_t *email);
void email_list_destroy(email_list_t *list);

void email_add( email_t *email, email_list_t *list );
void email_remove( email_t *email, email_list_t *list );
void email_remove_str( const char *email_str, email_list_t *list );
int email_string_is_equal( const char *email_str, email_t *email );

//---checking
int8_t email_exits( const uint8_t id, email_list_t *list );
int8_t email_exits_name( const char *name, email_list_t *list );
uint8_t email_get_id( const char *name, email_list_t *list );

#endif
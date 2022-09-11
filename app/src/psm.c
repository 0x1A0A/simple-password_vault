#include "psm.h"
#include <string.h>

byte password[256] = "default please change";
byte nonce[12] = {0,0,0,0,0,0,0,0,0,0,0,0}; // use zero nonce default
byte hash[32];

account_list_t *account_list;
tag_list_t *tag_list;
email_list_t *email_list;
struct df_sha256context *sha;
struct chacha20 *cypher;

void psminit()
{
	email_list = email_list_create();
	account_list = account_list_create();
	tag_list = tag_list_create();

	sha = sha256context_create();
	cypher = chacha20_create();
}

void psmclear()
{
	sha256context_destroy(sha);
	chacha20_destroy(cypher);

	email_list_destroy(email_list);
	account_list_destroy(account_list);
	tag_list_destroy(tag_list);
}

	// sha256context_add(sha, password, strlen(password));
	// sha256context_end( sha, hash );
	// chacha20_block_init( cypher, hash, nonce );
enum INPUT psm_cmd()
{
	if ( !strcmp(cmd, "passwd") ) return PASSWD_ACC;

	return _NONE_;
}

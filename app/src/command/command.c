#include "command.h"
#include "../common.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <editline.h>
#include "src/crypto/sha256.h"
#include "src/crypto/chacha20.h"
#include "../parse.h"

extern struct df_sha256context *sha;
byte nonce[12] = {0,0,0,0,0,0,0,0,0,0,0,0}; // use zero nonce default

int leave(void *arg) { return LEAVE; }

int passwd(void *arg)
{	
	el_no_echo = 1;
	char *password = readline("  Enter password: "); puts("");
	el_no_echo = 0;

	if (password) {
		sha256_reset(sha);
		sha256context_add(sha, (byte*)password, strlen(password));
		sha256context_end(sha, hash);
		free(password);
		return LOGGEDIN;
	}

	free(password);
	return OK;
}

int load_entry(void *arg)
{
	AUTH
	lexer_t *lexer = (lexer_t*)arg;

	tag_list_reset(tag_list);
	email_list_reset(email_list);
	account_list_reset(account_list);

	chacha20_block_init(cypher, hash, nonce);
	token_t tok = parse(lexer);
	if (!tok.END) {
		char path[20];
		strncpy(path,tok.begin,tok.length);
		path[tok.length] = '\0';
		load_file(path);
		return OK;
	}

	return FAILED;
}

int save_entry(void *arg)
{
	AUTH
	lexer_t *lexer = (lexer_t*)arg;

	chacha20_block_init(cypher, hash, nonce);
	token_t tok = parse(lexer);
	if (!tok.END) {
		char path[20];
		strncpy(path,tok.begin,tok.length+1);
		save_file(path);
		return OK;
	}

	return FAILED;
}

void notallow()
{
	puts("please enter key to unlock");
	puts("  use 'passwd' to enter key");
}

void show_secret(const char *name)
{
	account_t *account = account_find_name(name, account_list);
	if (account) {
		printf("  id %.*s\n  user - %.*s\n  password - %.*s\n",
			account->nl, account->name,
			account->ul, account->user,
			account->pl, account->password
		);
	} else {
		puts("no account found");
	}
}
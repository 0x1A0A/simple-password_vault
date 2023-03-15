#include <stdio.h>
#include <string.h>
#include <editline.h>
#include "psm.h"
#include "parse.h"
#include "hashtable.h"
#include "src/crypto/chacha20.h"
#include "src/crypto/sha256.h"

account_list_t *account_list;
tag_list_t *tag_list;
email_list_t *email_list;
struct chacha20 *cypher;
byte hash[32];

struct df_sha256context *sha;

int authorized = 0;

enum __RESPONSE {
	OK,
	LOGGEDIN,
	LEAVE,
	COUNT,
};

int hfn(void *arg)
{
	char *str = (char*)arg;
	int l = strlen(str);
	return (str[0] - 'a') << 3 | (str[1] - 'a') << 2 | (str[l-2] - 'a') << 1 | (str[l-1] - 'a') << 0;
}

int leave(void *arg) { return LEAVE; }

int passwd(void *arg)
{	
	lexer_t *lexer = (lexer_t*)arg;
	
	el_no_echo = 0;
	char *password = readline("  Enter password: "); puts("");
	el_no_echo = 1;

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

#define CMD(a,b) hashtable_insert( a, b, table );

int main(int argc, char **argv)
{
	sha = sha256context_create();

	hashtable_t *table = hashtable_create(20);
	table->function = hfn;

	tag_list = tag_list_create();
	email_list = email_list_create();
	account_list = account_list_create();

	CMD("exit", leave);
	CMD("passwd", passwd);

	char *line;
	char buff[16];

	while ((line=readline( !authorized ? "?> " : ">> "))!=NULL) {
		lexer_t lexer = {.begin = line};
		token_t tok = parse(&lexer);

		if ( !tok.END ) {
			strncpy(buff, tok.begin, tok.length);
			buff[tok.length] = '\0';
			
			command_t *c = hashtable_search( buff, table );
			if (c) {
				int res = c->callback(NULL);
				if (res == LEAVE) break; //end
				if (res == LOGGEDIN) authorized = 1; //end
			} else {
				puts("unknown command");
			}
		}

		free(line);
	}

	account_list_destroy(account_list);
	email_list_destroy(email_list);
	tag_list_destroy(tag_list);

	hashtable_destroy(table);

	sha256context_destroy(sha);

	return 0;
}
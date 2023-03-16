#include <stdio.h>
#include <string.h>
#include <editline.h>
#include "common.h"
#include "parse.h"
#include "hashtable.h"
#include "src/crypto/chacha20.h"
#include "src/crypto/sha256.h"
#include "command/command.h"

account_list_t *account_list;
tag_list_t *tag_list;
email_list_t *email_list;
struct chacha20 *cypher;
byte hash[32];

struct df_sha256context *sha;

int authorized = 0;

size_t hfn(void *arg)
{
	char *str = (char*)arg;
	int l = strlen(str);
	return 	(str[0] - 'a') << 8*3	|
			(str[1] - 'a') << 8*2	|
			(str[l-2] - 'a') << 8*1	|
			(str[l-1] - 'a') << 8*0;
}

#define CMD(a,b) hashtable_insert(a,b,table);

int main(int argc, char **argv)
{
	sha = sha256context_create();
	cypher = chacha20_create();

	// I choose 19 and the result for this command
	// I get the perfect hash
	hashtable_t *table = hashtable_create(19);
	table->function = hfn;

	tag_list = tag_list_create();
	email_list = email_list_create();
	account_list = account_list_create();

	CMD("exit",		leave);
	CMD("passwd",	passwd);
	CMD("add",		add_entry);
	CMD("del",		del_entry);
	CMD("show",		show_entry);
	CMD("load",		load_entry);
	CMD("save",		save_entry);

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
				int res = c->callback((void*)&lexer);
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

	chacha20_destroy(cypher);
	sha256context_destroy(sha);

	return 0;
}

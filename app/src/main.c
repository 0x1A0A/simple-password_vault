#include <stdio.h>
#include <string.h>
#include <editline.h>
#include "psm.h"
#include "parse.h"
#include "hashtable.h"

account_list_t *account_list;
tag_list_t *tag_list;
email_list_t *email_list;
struct chacha20 *cypher;
byte hash[32];

int authorized = 0;

enum __RESPONSE {
	OK,
	LOGGEDIN,
	LEAVE,
	COUNT,
};

int leave(void *arg) { return LEAVE; }

int hfn(void *arg)
{
	char *str = (char*)arg;
	int l = strlen(str);
	return (str[0] - 'a') << 3 | (str[1] - 'a') << 2 | (str[l-2] - 'a') << 1 | (str[l-1] - 'a') << 0;
}

int main(int argc, char **argv)
{
	hashtable_t *table = hashtable_create(20);
	table->function = hfn;

	tag_list = tag_list_create();
	email_list = email_list_create();
	account_list = account_list_create();

	hashtable_insert(
		command_create("exit", leave),
		table
	);

	char *line;
	char buff[16];

	while ((line=readline("?>"))!=NULL) {
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

	return 0;
}
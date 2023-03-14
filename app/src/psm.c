#include "psm.h"
#include "parse.h"

#include <editline.h>
#include <stdlib.h>
#include <string.h>

#define M_CREATE(type) \
if(!type##_exits_name(tok, type##_list)) {\
type##_t *type = type##_create(tok, strlen(tok));\
if(type) type##_add(type,type##_list);\
else ok = 0;}\
else {fprintf(stderr, "'%s' already exits\n", tok);ok=0;}

byte password[256] = "default please change";
byte nonce[12] = {0,0,0,0,0,0,0,0,0,0,0,0}; // use zero nonce default
byte hash[32];
char cmd[512];
byte loggedin = 0;

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

#define BIT(x) 	(1<<x)

#define CREATE 	BIT(0)
#define DEL		BIT(1)
#define SHOW	BIT(2)
#define SAVE	BIT(3)
#define LOAD	BIT(4)
#define CONFIG	BIT(5)

#define TAG		BIT(0)
#define EMAIL	BIT(1)
#define ACCOUNT	BIT(2)

void show_tag()
{
	tag_t *it = tag_list->head;
	while (it) {
		printf("id %u %s\n", it->id, it->name);
		it = it->next;
	}
}

void show_email()
{
	email_t *it = email_list->head;
	while (it) {
		printf("id %u %s\n", it->id, it->local);
		it = it->next;
	}
}

void show_account()
{
	account_t *acc = account_list->head;
	while (acc) {
		printf("id %s\n", acc->name);
		acc = acc->next;
	}
}

int create(lexer_t *lexer)
{
	token_t token = parse(lexer);
}

void delete()
{
}

void psm_cmd()
{
	int run = 1;
	char *line;
	lexer_t lexer;

	while (run && (line=readline( !loggedin ? "?> " : ">>> "))!=NULL) {
		int ok = 1;
		token_t tok;
		lexer.begin = line;

		tok = parse(&lexer);
		if(!tok.END) {
			if (!strcmp(tok.begin,"exit") || !strcmp(tok.begin,"quit")) {
				run = 0;
				break;
			}

			if (!strcmp(tok.begin,"passwd")) {
				el_no_echo = 1;
				char *passwd = readline("   enter passwd : ");

				if (passwd) {
					sha256_reset(sha);
					strncpy(password, passwd, 256);

					sha256context_add(sha, password, strlen(password));
					sha256context_end(sha, hash);

					sha256_reset(sha);

					memset(password, 0, 256);
					loggedin = 1;
				}

				free(passwd);
				el_no_echo = 0;
				puts("");
				load_file("test");

				continue;
			}

			if (!loggedin) {
				puts("please enter your password first!");
				puts("\tyou can use 'passwd' command to enter your password");
				continue;
			}

			if (!strcmp(tok.begin, "create") || !(strcmp(tok.begin, "add"))) ok=create(&lexer); // create something
			if (!strcmp(tok.begin, "delete") || !(strcmp(tok.begin, "del"))); // delete something
			if (!strcmp(tok.begin, "show")); // show something
			if (!strcmp(tok.begin, "save")); // save to file something
			if (!strcmp(tok.begin, "load") || !strcmp(tok.begin, "open")); // open file something
			
		}

		free(line);
	}
}

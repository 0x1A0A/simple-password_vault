#include "psm.h"
#include "parse.h"

#include <editline.h>
#include <stdlib.h>
#include <string.h>

#define M_CREATE(type) \
if(!type##_exits_name(token.begin,type##_list)) {\
	type##_t *type=type##_create(token.begin,token.length);\
	if(type) {type##_add(type,type##_list);return 1;}\
	else return 0;}\
else {fprintf(stderr,"'%.*s' already exits\n",token.length,token.begin); return 0;}

#define CMP(a,b) strncmp(a.begin,b,strlen(b) > a.length ? strlen(b):a.length)

#define TAG_CMP !CMP(token, "tag") || !CMP(token, "t")
#define EMAIL_CMP !CMP(token, "email") || !CMP(token, "e")
#define ACCOUNT_CMP !CMP(token, "account") || !CMP(token, "acc") || !CMP(token, "a")

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
		printf("  id %u %.*s\n", it->id, it->nl, it->name);
		it = it->next;
	}
}

void show_email()
{
	email_t *it = email_list->head;
	while (it) {
		printf("  id %u %.*s\n", it->id, it->nl + it->dl + 1, it->local);
		it = it->next;
	}
}

void show_account()
{
	account_t *acc = account_list->head;
	while (acc) {
		printf("  id %.*s\n", acc->nl, acc->name);
		acc = acc->next;
	}
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

int create(lexer_t *lexer)
{
	token_t token = parse(lexer);

	if (token.END) return 0;

	if ( TAG_CMP ) {
		token = parse(lexer);
		if ( token.END ) return 0;
		M_CREATE(tag)
	}

	if ( EMAIL_CMP ) {
		token = parse(lexer);
		if ( token.END ) return 0;
		M_CREATE(email)
	}

	if ( ACCOUNT_CMP ) {
		int flag = 0;
		char name[20];
		char user[20];
		char *passwd;

		token = parse(lexer);
		while (!token.END) {
			// this is an option -- peek only next character
			if ( *(token.begin) == '-' ) {
				switch(*(token.begin+1)) {
					case 'n':
						token = parse(lexer);
						if ( token.END ) {
							// expect argument
							return 0;
						}
						strncpy(name, token.begin, token.length);
						name[token.length] = '\0';
						flag |= BIT(0);
					break;
					case 'u':
						token = parse(lexer);
						if ( token.END ) {
							// expect argument
							return 0;
						}
						strncpy(user, token.begin, token.length);
						user[token.length] = '\0';
						flag |= BIT(1);
					break;
					default: //unknown
					break;
				}
			}
			token = parse(lexer);
		}

		if (flag == 0b11) {
			// ok
			el_no_echo = 1;
			passwd = readline("    password: "); puts("");
			el_no_echo = 0;
			account_t *account = account_create( name, user, passwd, 
				strlen(name), strlen(user), strlen(passwd) );
			
			if (account) account_add(account, account_list);

			free(passwd);
		}

	}

	return 1;
}

void delete(lexer_t *lexer)
{
	token_t token = parse(lexer);

	if (token.END) {
		return;
	}

	if ( TAG_CMP ) {
		token = parse(lexer);
		if ( token.END ) return;
		tag_remove_str(token.begin, tag_list);
	}

	if ( EMAIL_CMP ) {
		token = parse(lexer);
		if ( token.END ) return;
		email_remove_str(token.begin, email_list);
	}

	if ( ACCOUNT_CMP ) {
		
	}
}

void show(lexer_t *lexer)
{
	token_t token = parse(lexer);
	
	if (token.END) {
		puts("--tag");show_tag();
		puts("--email");show_email();
		puts("--account");show_account();
		return;
	}

	if (!CMP(token, "tag")) show_tag();
	if (!CMP(token, "email")) show_email();
	if (!CMP(token, "account")) show_account();
}

void psm_cmd()
{
	int run = 1;
	char *line = NULL;
	lexer_t lexer;

	while (run && (line=readline( !loggedin ? "?> " : ">>> "))!=NULL) {
		token_t tok;
		
		lexer.begin = line;

		tok = parse(&lexer);
		if(!tok.END) {
			// printf("%.*s\n", tok.length, tok.begin);
			if (!CMP(tok,"exit") || !CMP(tok,"quit")) {
				run = 0;
				free(line);
				break;
			}

			if (!CMP(tok,"passwd")) {
				el_no_echo = 1;
				char *passwd = readline("   Enter password: ");

				if (passwd) {
					sha256_reset(sha);
					strncpy(password, passwd, 256);

					sha256context_add(sha, password, strlen(password));
					sha256context_end(sha, hash);

					sha256_reset(sha);

					memset(password, 0, 256);
					loggedin = 1;
				}

				el_no_echo = 0;
				puts("");

				tag_list_reset(tag_list);
				email_list_reset(email_list);
				account_list_reset(account_list);
				
				free(passwd);
				free(line);
				
				continue;
			}

			if (!loggedin) {
				puts("Please enter your password first!");
				puts("\tyou can use 'passwd' command to enter your password");
				
				free(line);
				continue;
			}

			if (!CMP(tok, "create") || !(CMP(tok, "add"))){
				create(&lexer);
				free(line);
				continue;
			} // create something

			if (!CMP(tok, "delete") || !(CMP(tok, "del"))){
				delete(&lexer);
				free(line);
				continue;
			} // delete something
			if (!CMP(tok, "show")){
				show(&lexer);
				free(line);
				continue;
			} // show something
			if (!CMP(tok, "get")) {
				tok = parse(&lexer);
				if (!tok.END) {
					char id[20];
					strncpy(id, tok.begin, tok.length);
					id[tok.length] = '\0';
					show_secret(id);
				}
				free(line);
				continue;
			};// delete something
			if (!CMP(tok, "save")){
				chacha20_block_init(cypher, hash, nonce);
				tok = parse(&lexer);
				if (!tok.END) {
					char path[20];
					strncpy(path,tok.begin,tok.length+1);
					save_file(path);
				}
				free(line);
				continue;
			}; // save to file something
			if (!CMP(tok, "load") || !CMP(tok, "open")){
				tag_list_reset(tag_list);
				email_list_reset(email_list);
				account_list_reset(account_list);

				chacha20_block_init(cypher, hash, nonce);
				tok = parse(&lexer);
				if (!tok.END) {
					char path[20];
					strncpy(path,tok.begin,tok.length);
					path[tok.length] = '\0';
					load_file(path);
				}
				free(line);
				continue;
			}; // open file something
			
		}
	}
}

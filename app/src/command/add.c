#include "add.h"
#include "../parse.h"
#include <editline.h>
#include <string.h>

#define FM0 ""
#define FM2 "  "
#define FM4 "    "
#define FM6 "      "

#define M_CREATE(type) \
if(!type##_exits_name(token.begin,type##_list)) {\
	type##_t *type=type##_create(token.begin,token.length);\
	if(type) {type##_add(type,type##_list);return OK;}\
	else return FAILED;}\
else {fprintf(stderr,"'%.*s' already exits\n",(int)token.length,token.begin); return FAILED;}

static void usage(void)
{
	puts(FM0"USAGE");
	puts(FM2"add t <name> -- to add new tag");
	puts(FM2"add e <name> -- to add new email");
	puts(FM2"add a options -- to add new account");
	puts(FM4"account options");
	puts(FM6"-n <name>");
	puts(FM6"-u <user>");
}

static int add_tag(lexer_t *lexer)
{
	token_t token = parse(lexer);
	
	if (token.END) {return FAILED;}
	M_CREATE(tag)
}

static int add_email(lexer_t *lexer)
{
	token_t token = parse(lexer);
	
	if (token.END) {return FAILED;}
	M_CREATE(email)
}

static int add_account(lexer_t *lexer)
{
	int flag = 0;
	char name[20];
	char user[20];
	char *passwd;

	token_t token = parse(lexer);
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

		return OK;
	}

	return FAILED;
}

int add_entry(void *arg)
{
	AUTH

	lexer_t *lexer = (lexer_t*)arg;
	token_t token = parse(lexer);

	if ( token.END ) {
		usage();
		return OK;
	}

	switch(*(token.begin)) {
		case 't': add_tag(lexer);break;
		case 'e': add_email(lexer);break;
		case 'a': add_account(lexer);break;
		default: break;
	}

	return FAILED;
}
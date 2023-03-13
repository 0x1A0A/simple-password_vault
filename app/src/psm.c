#include "psm.h"
#include "passwdPromp.h"
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

void promp(const char *str)
{
	printf("%s", str);
}

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

void printbyte(const void *str, size_t length)
{
	size_t i=0;

	while (i<length) printf("%02x", ((unsigned char *)str)[i++]);
	puts("");
}

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

void psm_cmd()
{
	byte run = 1;
	uint8_t cflag = 0;
	uint8_t tflag = 0;

	while (run) {
		cflag = tflag = 0;

		promp( loggedin ? "loggedin" : "xxx");
		promp(" _> ");
		int isEOF = scanf(" %[^\n]", cmd);

		if (isEOF == EOF) {
			printf("\nExiting ...\n");
			break;
		}

		char *tok = strtok(cmd, " ");

		if ( !strcmp(tok, "passwd") ) {
			promp("passwd : ");
			getpasswd(password);
			
			sha256_reset(sha);

			sha256context_add(sha, password, strlen(password));
			sha256context_end(sha, hash);

			sha256_reset(sha);

			memset(password, 0, 256);

			loggedin = 1;
			promp("\n");
			continue;
		}

		if ( !strcmp(tok, "exit") || !strcmp(tok, "quit") || !strcmp(tok, "q") ) {
			run = 0;
			break;
		}

		if ( !loggedin ) {
			printf("please enter master password first.\n");
			printf("use \"passwd\" to set master password.\n");
			continue;
		}

		//////////////////
		if ( !strcmp(tok, "create") || !strcmp(tok, "add") ) cflag |= CREATE;
		if ( !strcmp(tok, "delete") || !strcmp(tok, "del") ) cflag |= DEL;
		if ( !strcmp(tok, "show") ) cflag |= SHOW;
		if ( !strcmp(tok, "save") ) cflag |= SAVE;
		if ( !strcmp(tok, "load") ) cflag |= LOAD;
		//////////////////

		tok = strtok(NULL, " ");

		if ( tok ) {
			if ( !strcmp(tok, "tag") ) tflag |= TAG;
			if ( !strcmp(tok, "email") ) tflag |= EMAIL;
			if ( !strcmp(tok, "account") ) tflag |= ACCOUNT;
		}

		switch ( cflag ) {
			case CREATE:
				tok = strtok(NULL, " ");
				if (!tok) {
					printf("insufficient argument\n");
					break;
				}

				byte ok = 1;

				switch ( tflag ) {
					case TAG:M_CREATE(tag)break;
					case EMAIL:M_CREATE(email)break;
					case ACCOUNT:
						{

						}
						break;
					default: ok = 0;
				}
				if (ok) printf("created new '%s'\n",tok);
				break;
			case DEL:
				tok = strtok(NULL, " ");
				if (!tok) break;

				switch ( tflag ) {
					case TAG: tag_remove_str(tok, tag_list); break;
					case EMAIL: email_remove_str(tok, email_list); break;
				}
				break;
			case SHOW:
				switch ( tflag ) {
					case 0:
						puts("--tag"); show_tag();
						puts("--email"); show_email();
						break;
					case TAG: show_tag();
							  break;
					case EMAIL: show_email();
								break;
				}
				break;
			case SAVE:
				chacha20_block_init(cypher, hash, nonce);
				if (!tok) {
					puts("need file name");
					break;
				} save_file(tok);
				break;
			case LOAD:
				chacha20_block_init(cypher, hash, nonce);
				if (!tok) {
					puts("need file name");
					break;
				} load_file(tok);
				break;
			default:
				printf("unknow command\n");
				break;
		}
	}
}


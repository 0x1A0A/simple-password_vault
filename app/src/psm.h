#include "file.h"
#include "email.h"
#include "crypto/sha256.h"
#include "crypto/chacha20.h"

//extern char input_state[COUNT][256];
extern char cmd[512];
extern byte password[256];
extern byte nonce[12]; // use zero nonce default
extern byte hash[32];
extern byte loggedin;

extern account_list_t *account_list;
extern tag_list_t *tag_list;
extern email_list_t *email_list;
extern struct df_sha256context *sha;
extern struct chacha20 *cypher;

//void promp(const char *);
void psminit(void);
void psmclear(void);
void psm_cmd();

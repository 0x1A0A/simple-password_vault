#include "show.h"
#include "psm.h"
#include "../common.h"
#include "../parse.h"
#include <stdio.h>

static void show_tag()
{
	tag_t *it = tag_list->head;
	while (it) {
		printf("  id %u %.*s\n", it->id, it->nl, it->name);
		it = it->next;
	}
}

static void show_email()
{
	email_t *it = email_list->head;
	while (it) {
		printf("  id %u %.*s\n", it->id, it->nl + it->dl + 1, it->local);
		it = it->next;
	}
}

static void show_account()
{
	account_t *acc = account_list->head;
	while (acc) {
		printf("  id %.*s\n", acc->nl, acc->name);
		acc = acc->next;
	}
}

int show_entry(void *arg)
{
	AUTH
	
	lexer_t *lexer = (lexer_t*)arg;
	token_t token = parse(lexer);

	if (token.END) {
		puts("--tag"); show_tag();
		puts("--email"); show_email();
		puts("--account"); show_account();
		return OK;
	}

	// look only first character! t-ag, e-mail, a-ccount
	switch(*(token.begin)) {
		case 't': puts("--TAG--");show_tag();break;
		case 'e': puts("--EMAIL--");show_email();break;
		case 'a': puts("--ACCOUNT--");show_account();break;
		default: break;
	}

	return OK;
}
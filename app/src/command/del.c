#include "del.h"
#include <stdio.h>
#include "../parse.h"
#include <string.h>

static int del_tag(lexer_t *lexer)
{
	token_t token = parse(lexer);

	if ( token.END ) {return FAILED;}

	// this is an option -- peek only next character
	if ( *(token.begin) == '-' ) {
		char option = *(token.begin+1); 
		
		token = parse(lexer);

		if ( token.END ) {return FAILED;}

		char buff[16];
		strncpy(buff, token.begin, token.length);
		buff[token.length] = '\0';

		switch(option) {
			case 'n': tag_remove_str(buff, tag_list);break;
			default: break;
		}

		return OK;
	}

	char buff[16];
	strncpy(buff, token.begin, token.length);
	buff[token.length] = '\0';
	int id = atoi(buff);

	// delete using id
	tag_remove_id(id, tag_list);

	return OK;
}

static int del_email(lexer_t *lexer)
{
	token_t token = parse(lexer);

	if ( token.END ) {return FAILED;}

	// this is an option -- peek only next character
	if ( *(token.begin) == '-' ) {
		char option = *(token.begin+1); 
		
		token = parse(lexer);

		if ( token.END ) {return FAILED;}

		char buff[16];
		strncpy(buff, token.begin, token.length);
		buff[token.length] = '\0';

		switch(option) {
			case 'n': email_remove_str(buff, email_list);break;
			default: break;;
		}

		return OK;
	}

	char buff[16];
	strncpy(buff, token.begin, token.length);
	buff[token.length] = '\0';
	int id = atoi(buff);

	// delete using id
	email_remove_id(id, email_list);

	return OK;
}

static int del_account(lexer_t *lexer)
{
	token_t token = parse(lexer);

	if ( token.END ) {return FAILED;}

	// this is an option -- peek only next character
	if ( *(token.begin) == '-' ) {
		char option = *(token.begin+1); 
		
		token = parse(lexer);

		if ( token.END ) {return FAILED;}

		char buff[16];
		strncpy(buff, token.begin, token.length);
		buff[token.length] = '\0';

		switch(option) {
			case 'n': account_remove_str(buff, account_list);break;
			default: break;;
		}

		return OK;
	}

	char buff[16];
	strncpy(buff, token.begin, token.length);
	buff[token.length] = '\0';
	int id = atoi(buff);

	// delete using id
	// account_remove_id(id, account_list);

	return OK;
}

// delete using name or id
int del_entry(void* arg)
{
	AUTH
	lexer_t *lexer = (lexer_t*)arg;
	token_t token = parse(lexer);
 
	if ( token.END ) {
		// missing type print usage?
		return OK;
	}

	switch(*(token.begin)) {
		case 't': del_tag(lexer);break;
		case 'e': del_email(lexer);break;
		case 'a': del_account(lexer);break;
		default: break;
	}

	return OK;
}
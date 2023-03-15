#include <stdio.h>
#include <ctype.h>

#include "parse.h"
#include "string.h"

// return destance to the next character
static int find_until(const char target, lexer_t *lexer)
{
	size_t l = 0;

	printf("%c\n", target);

	while (*(lexer->begin)){
		if (*(lexer->begin++) != target) ++l;
		else break;
	}
	return l;
}

static int find_until_space(lexer_t *lexer) {
	size_t l = 0;

	while (*(lexer->begin)) {
		if (!isspace(*(lexer->begin++))) ++l;
		else break;
	}
	return l;
}

token_t parse(lexer_t *lexer)
{
	token_t token={.begin=0,.length=0,.END=0};

	if ( *(lexer->begin) == '\0' ) {
		token.END = 1;
		return token;
	}

	while (isspace( *(lexer->begin) )) lexer->begin++;

	if (isalnum(*(lexer->begin)) || *(lexer->begin)=='-') {
		token.begin = lexer->begin;
		token.length = find_until_space(lexer);

		return token;
	}

	switch(*(lexer->begin)) {
		case '"':
			lexer->begin++;
			token.begin = lexer->begin;
			token.length = find_until( *(lexer->begin-1), lexer );
		break;
		default:
			token.END = 1;
		break;
	}

	return token;
}

void token_print(token_t token)
{
	printf("%.*s\n", token.length, token.begin);	
}
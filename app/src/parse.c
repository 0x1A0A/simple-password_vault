#include "parse.h"
#include "string.h"
#include <stdio.h>
#include <ctype.h>

// return destance to the next character
static int find_until(const char target, lexer_t *lexer)
{
	size_t l = 0;

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
	token_t token={.begin=0,.lenght=0,.END=0};

	while (isspace( *(lexer->begin) )) lexer->begin++;

	if (isalnum(*(lexer->begin))) {
		token.begin = lexer->begin;
		token.lenght = find_until_space(lexer);

		return token;
	}

	switch(*(lexer->begin)) {
		case '"':
			lexer->begin++;
			token.begin = lexer->begin;
			token.lenght = find_until( *(lexer->begin-1), lexer );
		break;
		default:
			token.END = 1;
		break;
	}

	return token;
}

void token_print(token_t token)
{
	printf("%.*s\n", token.lenght, token.begin);	
}
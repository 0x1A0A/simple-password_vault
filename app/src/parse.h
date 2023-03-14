#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdlib.h>

typedef struct Token token_t;
typedef struct Lexer lexer_t;

struct Token {
	const char *begin;
	size_t lenght;
	int END;
};

struct Lexer {
	const char *begin;
};

token_t parse(lexer_t *lexer);
void token_print(token_t token);

#endif

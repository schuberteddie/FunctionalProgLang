#ifndef __TOKEN_HELPER__
#define __TOKEN_HELPER__

#include "ast.h"
#define YYSTYPE ast*
#include "parser.h"

int tokenize (int);
void invalid_token (const char*);
void yyerror(const char *);
extern int yylex(void);

#endif
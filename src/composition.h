#ifndef __COMPOSITION__
#define __COMPOSITION__

#include "ast.h"
#include "parser.h"

void dump_ast(FILE*,ast*,int);
void dump_idents(FILE*);
void compose(ast*);
void composeLine(ast*);

#endif
#ifndef __PARSER_HELPER__
#define __PARSER_HELPER__

#include <unordered_map>
#include <string>
#include <vector>
#include "ast.h"
#include "execution.h"
using namespace std;

extern unordered_map<string,pair<ast*,ast*>> identifiers;
extern unordered_map<string,ast*(*)(ast*)> native_functions;
extern unordered_map<string,ast*> types;

void insert_global(ast*,ast*,ast*);

void insert_native(ast*,ast*,ast*);

void declType(ast*,ast*);

ast* adopt(ast*,ast*);
ast* makeNode(ast*);
ast* adoptChildren(ast*,ast*);

ast* makeString(ast*);
ast* makeList(ast*,ast*);

#endif
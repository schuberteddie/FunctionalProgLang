#ifndef __EXECUTION__
#define __EXECUTION__

#include <unordered_map>
#include "ast.h"

extern unordered_map<string,pair<ast*,ast*>> identifiers;
extern unordered_map<string,ast*(*)(ast*)> native_functions;
extern unordered_map<string,ast*> types;
void execute(ast*);

#endif
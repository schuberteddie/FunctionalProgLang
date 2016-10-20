#include "tokenhelper.h"
#include <iostream>
#include <cstdlib>

using namespace std;

extern char* yytext;

int tokenize (int token) {
	yylval = new ast(token, yytext);
	return token;
}

void invalid_token (const char* text) {
	cerr << "Invalid Token: " << text << endl;
	exit(1);
}

void yyerror(const char * text) {
	cerr << text << endl;
}
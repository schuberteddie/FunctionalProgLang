#include "tokenhelper.h"
#include <string>
#include <cstring>
#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include "execution.h"
#include "composition.h"
using namespace std;

extern FILE* yyin;
extern int yy_flex_debug;
FILE* out;

int main(int argc, char** argv) {
	yy_flex_debug = 0;
	for (int i = 1; i < argc; ++i) {
		yyin = fopen(argv[i],"r");
		yyparse();
		fclose(yyin);
	}
	string input;
	cout << "fpli>  ";
	while (getline(cin,input)) {
		char* tmp = new char[input.size()];
		strcpy(tmp,input.c_str());
		yyin = fmemopen(tmp,input.size(),"r");
		yyparse();
		cout << "fpli>  ";
	}
}
#include "composition.h"
#include "execution.h"
#include <stdio.h>
#include <map>
using namespace std;

void dump_ast(FILE* file, ast* tree, int depth) {
	for (int i = 0; i < depth; ++i) {
		fprintf(file,"| ");
	}
	fprintf(file,"%s\n",tree->getText().c_str());
	for (auto child : tree->getChildren()) {
		dump_ast(file,child,depth+1);
	}
}

void dump_idents(FILE* file) {
	for (auto& val : identifiers) {
		fprintf(file, "%s : \n", val.first.c_str());
		ast* params = val.second.first;
		ast* exprs = val.second.second;
		fprintf(file, " -- PARAMS -- \n");
		dump_ast(file,params,0);
		fprintf(file, " -- AST -- \n");
		dump_ast(file,exprs,0);
	}
}

void compose_binary_infix(ast* node) {
	compose(node->getChildren()[0]);
	printf(" %s ", node->getText().c_str());
	compose(node->getChildren()[1]);
}

void compose_binary_infix_reverse(ast* node) {
	compose(node->getChildren()[1]);
	printf("%s", node->getText().c_str());
	compose(node->getChildren()[0]);
}

void compose_unary_prefix(ast* node) {
	printf("%s", node->getText().c_str());
	compose(node->getChildren()[0]);
}

void compose_parentheses(ast* node) {
	compose_unary_prefix(node);
	printf(")");
}

void compose_val(ast* node) {
	printf("%s", node->getText().c_str());
}

void compose_if(ast* node) {
	printf("%s ", node->getText().c_str());
	compose(node->getChildren()[0]);
	printf(" then ");
	compose(node->getChildren()[1]);
	printf(" else ");
	compose(node->getChildren()[2]);
}

void compose_args(ast* node) {
	bool first = true;
	printf("%s",node->getText().c_str());
	for (ast* arg : node->getChildren()) {
		if (!first) {
			printf(",");
		} else {
			first = false;
		}
		compose(arg);
		
	}
	printf(")");
}

void compose_ident(ast* node) {
	printf("%s",node->getText().c_str());
	if (node->getChildren().size() > 0) {
		if (node->getChildren()[0]->getToken() == TOK_OPEN_PAREN) {
			compose_args(node->getChildren()[0]);
		} else {
			printf("=");
			compose(node->getChildren()[0]);
		}
	}
}

void compose_obj(ast* node) {
	printf("%s",node->getText().c_str());
	if (node->getChildren().size() > 0) {
		bool first = true;
		for (ast* field : node->getChildren()) {
			if (!first) {
				printf(",");
			} else {
				first = false;
			}
			compose_ident(field);
		}
		printf("}");
	}
}

void compose_char(ast* node) {
	string str = node->getText();
	str = str.substr(1,str.size()-2);
	printf("%s",str.c_str());
}

void compose_str(ast* node) {
		ast* crawler = node;
		if (crawler->getChildren().size() > 0) {
			crawler = crawler->getChildren()[0];
			if (crawler->getChildren().size() > 1) {
				compose_char(crawler->getChildren()[1]);
			}
			crawler = crawler->getChildren()[0];
		}
		while (crawler->getChildren().size() > 0) {
			compose_char(crawler->getChildren()[1]);
			crawler = crawler->getChildren()[0];
		}
		printf("%s",crawler->getText().c_str());
}

void compose_list(ast* list) {
	printf("%s", list->getText().c_str());
	if (list->getText() == string("[")) {
		ast* crawler = list;
		if (crawler->getChildren().size() > 0) {
			crawler = crawler->getChildren()[0];
			if (crawler->getChildren().size() > 1) {
				compose(crawler->getChildren()[1]);
			}
			crawler = crawler->getChildren()[0];
		}
		while (crawler->getChildren().size() > 0) {
			printf("%s",crawler->getText().c_str());
			compose(crawler->getChildren()[1]);
			crawler = crawler->getChildren()[0];
		}
		printf("%s",crawler->getText().c_str());
	} else {
		compose_str(list);
	}
}

map<int,void(*)(ast*)> compositions {
    {TOK_INT,					compose_val},
    {TOK_FLOAT,					compose_val},
    {TOK_CHAR,					compose_val},
    {TOK_IF,					compose_if},
    {TOK_FALSE,					compose_val},
    {TOK_TRUE,					compose_val},
    {TOK_OPEN_PAREN,            compose_parentheses},
    {TOK_IDENT,					compose_ident},
    {TOK_START_OBJ,				compose_obj},
    {TOK_EMPTY_LIST,			compose_val},
    {TOK_START_LIST,			compose_list},
    {TOK_COMMA,					compose_val},
    {TOK_CONCAT,				compose_binary_infix},
    {TOK_OBJ_ACCESS,			compose_binary_infix},
    {TOK_LIST_PREPEND,			compose_binary_infix_reverse},
    {TOK_NOT,					compose_unary_prefix},
    {TOK_AND,					compose_binary_infix},
    {TOK_OR,					compose_binary_infix},
    {TOK_EQUALS,				compose_binary_infix},
    {TOK_NOT_EQUALS,			compose_binary_infix},
    {TOK_LESS_THAN,				compose_binary_infix},
    {TOK_LESS_THAN_OR_EQ,		compose_binary_infix},
    {TOK_GREATER_THAN,			compose_binary_infix},
    {TOK_GREATER_THAN_OR_EQ,	compose_binary_infix},
    {TOK_ADD,					compose_binary_infix},
    {TOK_SUB,					compose_binary_infix},
    {TOK_MUL,					compose_binary_infix},
    {TOK_DIV,					compose_binary_infix},
    {TOK_MOD,					compose_binary_infix},
    {TOK_EXP,					compose_binary_infix},
    {TOK_LIST_ACCESS,			compose_binary_infix}
};

void compose(ast* tree) {
	auto fn = compositions[tree->getToken()];
	if (fn != nullptr) {
		fn(tree);
	} else {
		fprintf(stderr, "ERROR: composing %s of token %d\n", tree->getText().c_str(), tree->getToken());
	}
}

void composeLine(ast* tree) {
	compose(tree);
	printf("\n");
}
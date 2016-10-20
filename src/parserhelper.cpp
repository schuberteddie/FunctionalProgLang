#include "parserhelper.h"
#include "ast.h"
#include "parser.h"
#include <iostream>
using namespace std;

void insert_global(ast* ident, ast* arglist, ast* function) {
	identifiers.emplace(ident->getText(), pair<ast*,ast*>(arglist, function)); 
}

void insert_native(ast* ident, ast* arglist, ast* nativeFunction) {
	identifiers.emplace(ident->getText(), pair<ast*,ast*>(arglist, new ast(TOK_NATIVE_START,nativeFunction->getText())));
}

void declType(ast* type, ast* definition) {
	types.emplace(type->getText(), definition);
}

ast* adopt(ast* parent, ast* child) {
	parent->addChild(child);
	return parent;
}

ast* makeNode(ast* child) {
	ast* node = new ast(0,"");
	return node;
}

ast* adoptChildren(ast* parent, ast* node) {
	auto children = node->getChildren();
	for (auto child = children.rbegin(); child != children.rend(); ++child) {
		parent->addChild(*child);
	}
	return parent;
}

ast* makeString(ast* stringNode) {
	string& str = stringNode->getText();
	ast* list = new ast(TOK_START_LIST,"\"");
	ast* crawler = list;
	for (size_t i = 1; i < str.size()-1; ++i) {
		ast* node = new ast(TOK_COMMA,",");
		ast* tmp;
		if (str[i] == '\\') {
			tmp = new ast(TOK_CHAR,"'"+str.substr(i++,2)+"'");
		} else {
			tmp = new ast(TOK_CHAR,"'"+str.substr(i,1)+"'");
		}
		crawler->addChildToFront(node);
		node->addChild(tmp);
		crawler = node;
	}
	crawler->addChildToFront(new ast(TOK_END_LIST,"\""));
	return list;
}

ast* makeList(ast* end, ast* val) {
	ast* node = new ast(TOK_COMMA,",");
	node->addChild(end);
	node->addChild(val);
	return node;
}
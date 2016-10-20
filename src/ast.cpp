#include "ast.h"
#include <vector>
#include <string>
using namespace std;

ast::ast(int symbol, string text) {
	this->token = symbol;
	this->text = text;
	this->children = new vector<ast*>();
	this->parent = nullptr;
}

ast::~ast() {
	/*for (ast* child : getChildren()) {
		delete child;
	}
	delete children;*/
}

bool ast::equals(ast& other) {
	return this->text == other.text && this->token == other.token;
}

int ast::getToken() {
	return this->token;
}

void ast::setToken(int sym) {
	this->token = sym;
}

string& ast::getText() {
	return this->text;
}

void ast::setText(string str) {
	this->text = str;
}

vector<ast*>& ast::getChildren() {
	return *(this->children);
}

ast* ast::addChild(ast* node) {
	children->push_back(node);
	node->parent = this;
	return node;
}

void ast::addChildToFront(ast* node) {
	this->children->insert(this->children->cbegin(),node);
	node->parent = this;
}

void ast::replaceChild(int idx, ast* node) {
	(*(this->children))[idx] = node;
	node->parent = this;
}

ast* ast::getParent() {
	return this->parent;
}

void ast::setParent(ast* node) {
	this->parent = node;
}
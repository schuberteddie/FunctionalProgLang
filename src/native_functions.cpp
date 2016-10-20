#include "native_functions.h"
#include "execution.h"
#include "ast.h"
#include "parser.h"

using namespace std;

ast* do_tail(ast* args) {
	ast* head = args->getChildren()[0];
	if (head->getChildren().size() > 0 && head->getChildren()[0]->getChildren().size() > 0) {
		head->replaceChild(0,head->getChildren()[0]->getChildren()[0]);
		if (head->getChildren()[0]->getToken() == TOK_END_LIST) {
			return new ast(TOK_EMPTY_LIST,(head->getText())+(head->getChildren()[0]->getText()));
		}
	}
	return head;
}

ast* do_init(ast* args) {
	ast* crawler = args->getChildren()[0];
	if (crawler->getToken() != TOK_EMPTY_LIST) {
		ast* head = crawler;
		while (crawler->getToken() != TOK_END_LIST) {
			crawler = crawler->getChildren()[0];
		}
		ast* end = crawler;
		crawler = crawler->getParent();
		if (crawler == head->getChildren()[0]) {
			return new ast(TOK_EMPTY_LIST,(head->getText())+(end->getText()));
		} else {
			crawler = crawler->getParent();
			crawler->replaceChild(0,end);
		}
		return head;
	} else {
		return crawler;
	}
}

ast* do_length(ast* args) {
	ast* crawler = args->getChildren()[0];
	size_t length = 0;
	if (crawler->getChildren().size() > 0) {
		crawler = crawler->getChildren()[0];
	}
	while (crawler->getToken() != TOK_END_LIST && crawler->getToken() != TOK_EMPTY_LIST) {
		crawler = crawler->getChildren()[0];
		++length;
	}
	return new ast(TOK_INT,to_string(length));
}

ast* do_exit(ast* args) {
	int code = atol(args->getChildren()[0]->getText().c_str());
	exit(code);
}

void init_natives() {
	native_functions.emplace("tail",do_tail);
	native_functions.emplace("init",do_init);
	native_functions.emplace("length",do_length);
	native_functions.emplace("exit",do_exit);
}
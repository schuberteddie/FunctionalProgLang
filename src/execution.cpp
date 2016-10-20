#include <cstdlib>
#include <cmath>
#include <map>
#include <iostream>
#include "execution.h"
#include "composition.h"
#include "native_functions.h"
using namespace std;

unordered_map<string,pair<ast*,ast*>> identifiers;
unordered_map<string,ast*(*)(ast*)> native_functions;
vector<size_t> indexes;
size_t index = 0;
ast* execution_ptr;
ast* execution_root;
bool changed;

unordered_map<string,ast*> types {
	{"Integer",			new ast(TOK_INT,"")},
	{"Float",			new ast(TOK_FLOAT,"")},
	{"Character",		new ast(TOK_CHAR,"")},
	{"List",			new ast(TOK_START_LIST,"")},
	{"Object",			new ast(TOK_START_OBJ,"")},
	{"String",			(((new ast(TOK_START_LIST,""))->addChild(new ast(TOK_CHAR,"")))->getParent())},
	{"Boolean",			(((((new ast(TOK_OR,""))->addChild(new ast(TOK_TRUE,"")))->getParent())->addChild(new ast(TOK_FALSE,"")))->getParent())}
};

void move_over() {
	execution_ptr = execution_ptr->getParent()->getChildren()[++index];
}

void move_up() {
	index = indexes.back();
	indexes.pop_back();
	execution_ptr = execution_ptr->getParent();
}

void move_down(int idx) {
	indexes.push_back(index);
	index = idx;
	execution_ptr = execution_ptr->getChildren()[idx];
}

void move_to_leaf() {
	bool repeat = true;
	while (repeat) {
		repeat = false;
		while (!execution_ptr->getChildren().empty()) {
			move_down(0);
		}
		if (execution_ptr->getParent() != nullptr && index < execution_ptr->getParent()->getChildren().size()-1) {
			move_over();
			repeat = true;
		} else if (execution_ptr->getParent() != nullptr) {
			move_up();
		}
	}
}

void replace(ast* node) {
	changed = true;
	if (execution_ptr->getParent() != nullptr && execution_ptr != execution_root) {
		execution_ptr->getParent()->replaceChild(index,node);
		execution_ptr = node;
	} else {
		node->setParent(nullptr);
		execution_root = node;
		execution_ptr  = node;
	}
}

void do_not() {
	ast* val = execution_ptr->getChildren()[0];
	if (val->getToken() == TOK_TRUE) {
		replace(new ast(TOK_FALSE, "false"));
	} else if (val->getToken() == TOK_FALSE) {
		replace(new ast(TOK_TRUE, "true"));
	} else {
		throw "ERROR: arguments to '!' not applicable";
	}
}

void do_and() {
	ast* left  = execution_ptr->getChildren()[0];
	ast* right = execution_ptr->getChildren()[1];
	if (left->getToken() == TOK_TRUE && right->getToken() == TOK_TRUE) {
		replace(left);
	} else {
		replace(new ast(TOK_FALSE,"false"));
	}
}

void do_or() {
	ast* left  = execution_ptr->getChildren()[0];
	ast* right = execution_ptr->getChildren()[1];
	if (left->getToken() == TOK_TRUE || right->getToken() == TOK_TRUE) {
		replace(new ast(TOK_TRUE,"true"));
	} else {
		replace(left);
	}
}

bool recursive_equals(ast* left, ast* right) {
	if (!left->equals(*right) || left->getChildren().size() != right->getChildren().size()) {
		return false;
	} else if (left->getChildren().size() > 0) {
		int idx = 0;
		for (ast* child : left->getChildren()) {
			return recursive_equals(child,right->getChildren()[idx]);
		}
	}
	return left->equals(*right);
}

void do_equals() {
	ast* left = execution_ptr->getChildren()[0];
	ast* right = execution_ptr->getChildren()[1];
	if (left->getToken() == right->getToken()) {
		if (left->equals(*right)) {
			if (left->getChildren().size() == 0 && right->getChildren().size() == 0) {
				replace(new ast(TOK_TRUE,"true"));
			} else  if (recursive_equals(left,right)) {
				replace(new ast(TOK_TRUE,"true"));
			} else {
				replace(new ast(TOK_FALSE,"false"));
			}
		} else {
			replace(new ast(TOK_FALSE,"false"));
		}
	} else {
		throw "ERROR: type mismatch on ==";
	}
}

void do_nequals() {
	do_equals();
	if (execution_ptr->getToken() == TOK_TRUE) {
		execution_ptr->setToken(TOK_FALSE);
        execution_ptr->setText("false");
	} else {
		execution_ptr->setToken(TOK_TRUE);
        execution_ptr->setText("true");
	}
}

void do_lt() {
	ast* left = execution_ptr->getChildren()[0];
	ast* right = execution_ptr->getChildren()[1];
	if (left->getToken() == TOK_INT && right->getToken() == TOK_INT) {
		long int lefti  = atol(left->getText().c_str());
		long int righti = atol(right->getText().c_str());
		if (lefti < righti) {
			replace(new ast(TOK_TRUE,"true"));
		} else {
			replace(new ast(TOK_FALSE,"false"));
		}
	} else if ((left->getToken() == TOK_INT || left->getToken() == TOK_FLOAT)
		    && (right->getToken() == TOK_INT || right->getToken() == TOK_FLOAT)) {
		double leftf  = atof(left->getText().c_str());
		double rightf = atof(right->getText().c_str());
		if (leftf < rightf) {
			replace(new ast(TOK_TRUE,"true"));
		} else {
			replace(new ast(TOK_FALSE,"false"));
		}
	} else {
		throw "ERROR: arguments to '<' not applicable";
	}
}

void do_lt_eq() {
	ast* left = execution_ptr->getChildren()[0];
	ast* right = execution_ptr->getChildren()[1];
	if (left->getToken() == TOK_INT && right->getToken() == TOK_INT) {
		long int lefti  = atol(left->getText().c_str());
		long int righti = atol(right->getText().c_str());
		if (lefti <= righti) {
			replace(new ast(TOK_TRUE,"true"));
		} else {
			replace(new ast(TOK_FALSE,"false"));
		}
	} else if ((left->getToken() == TOK_INT || left->getToken() == TOK_FLOAT)
		    && (right->getToken() == TOK_INT || right->getToken() == TOK_FLOAT)) {
		double leftf  = atof(left->getText().c_str());
		double rightf = atof(right->getText().c_str());
		if (leftf <= rightf) {
			replace(new ast(TOK_TRUE,"true"));
		} else {
			replace(new ast(TOK_FALSE,"false"));
		}
	} else {
		throw "ERROR: arguments to '<=' not applicable";
	}
}

void do_gt() {
	ast* left = execution_ptr->getChildren()[0];
	ast* right = execution_ptr->getChildren()[1];
	if (left->getToken() == TOK_INT && right->getToken() == TOK_INT) {
		long int lefti  = atol(left->getText().c_str());
		long int righti = atol(right->getText().c_str());
		if (lefti > righti) {
			replace(new ast(TOK_TRUE,"true"));
		} else {
			replace(new ast(TOK_FALSE,"false"));
		}
	} else if ((left->getToken() == TOK_INT || left->getToken() == TOK_FLOAT)
		    && (right->getToken() == TOK_INT || right->getToken() == TOK_FLOAT)) {
		double leftf  = atof(left->getText().c_str());
		double rightf = atof(right->getText().c_str());
		if (leftf > rightf) {
			replace(new ast(TOK_TRUE,"true"));
		} else {
			replace(new ast(TOK_FALSE,"false"));
		}
	} else {
		throw "ERROR: arguments to '>' not applicable";
	}
}

void do_gt_eq() {
	ast* left = execution_ptr->getChildren()[0];
	ast* right = execution_ptr->getChildren()[1];
	if (left->getToken() == TOK_INT && right->getToken() == TOK_INT) {
		long int lefti  = atol(left->getText().c_str());
		long int righti = atol(right->getText().c_str());
		if (lefti >= righti) {
			replace(new ast(TOK_TRUE,"true"));
		} else {
			replace(new ast(TOK_FALSE,"false"));
		}
	} else if ((left->getToken() == TOK_INT || left->getToken() == TOK_FLOAT)
		    && (right->getToken() == TOK_INT || right->getToken() == TOK_FLOAT)) {
		double leftf  = atof(left->getText().c_str());
		double rightf = atof(right->getText().c_str());
		if (leftf >= rightf) {
			replace(new ast(TOK_TRUE,"true"));
		} else {
			replace(new ast(TOK_FALSE,"false"));
		}
	} else {
		throw "ERROR: arguments to '>=' not applicable";
	}
}

void do_add() {
	ast* left = execution_ptr->getChildren()[0];
	ast* right = execution_ptr->getChildren()[1];
	if (left->getToken() == TOK_INT && right->getToken() == TOK_INT) {
		long int lefti  = atol(left->getText().c_str());
		long int righti = atol(right->getText().c_str());
		replace(new ast(TOK_INT, to_string(lefti+righti)));
	} else if ((left->getToken() == TOK_INT || left->getToken() == TOK_FLOAT)
		    && (right->getToken() == TOK_INT || right->getToken() == TOK_FLOAT)) {
		double leftf  = atof(left->getText().c_str());
		double rightf = atof(right->getText().c_str());
		replace(new ast(TOK_FLOAT, to_string(leftf+rightf)));
	} else {
		throw "ERROR: arguments to '+' not applicable";
	}
}

void do_sub() {
	ast* left = execution_ptr->getChildren()[0];
	ast* right = execution_ptr->getChildren()[1];
	if (left->getToken() == TOK_INT && right->getToken() == TOK_INT) {
		long int lefti  = atol(left->getText().c_str());
		long int righti = atol(right->getText().c_str());
		replace(new ast(TOK_INT, to_string(lefti-righti)));
	} else if ((left->getToken() == TOK_INT || left->getToken() == TOK_FLOAT)
		    && (right->getToken() == TOK_INT || right->getToken() == TOK_FLOAT)) {
		double leftf  = atof(left->getText().c_str());
		double rightf = atof(right->getText().c_str());
		replace(new ast(TOK_FLOAT, to_string(leftf-rightf)));
	} else {
		throw "ERROR: arguments to '-' not applicable";
	}
}

void do_mul() {
	ast* left = execution_ptr->getChildren()[0];
	ast* right = execution_ptr->getChildren()[1];
	if (left->getToken() == TOK_INT && right->getToken() == TOK_INT) {
		long int lefti  = atol(left->getText().c_str());
		long int righti = atol(right->getText().c_str());
		replace(new ast(TOK_INT, to_string(lefti*righti)));
	} else if ((left->getToken() == TOK_INT || left->getToken() == TOK_FLOAT)
		    && (right->getToken() == TOK_INT || right->getToken() == TOK_FLOAT)) {
		double leftf  = atof(left->getText().c_str());
		double rightf = atof(right->getText().c_str());
		replace(new ast(TOK_FLOAT, to_string(leftf*rightf)));
	} else {
		throw "ERROR: arguments to '*' not applicable";
	}
}

void do_div() {
	ast* left = execution_ptr->getChildren()[0];
	ast* right = execution_ptr->getChildren()[1];
	if (left->getToken() == TOK_INT && right->getToken() == TOK_INT) {
		long int lefti  = atol(left->getText().c_str());
		long int righti = atol(right->getText().c_str());
		replace(new ast(TOK_INT, to_string(lefti/righti)));
	} else if ((left->getToken() == TOK_INT || left->getToken() == TOK_FLOAT)
		    && (right->getToken() == TOK_INT || right->getToken() == TOK_FLOAT)) {
		double leftf  = atof(left->getText().c_str());
		double rightf = atof(right->getText().c_str());
		replace(new ast(TOK_FLOAT, to_string(leftf/rightf)));
	} else {
		throw "ERROR: arguments to '/' not applicable";
	}
}

void do_mod() {
	ast* left = execution_ptr->getChildren()[0];
	ast* right = execution_ptr->getChildren()[1];
	if (left->getToken() == TOK_INT && right->getToken() == TOK_INT) {
		long int lefti  = atol(left->getText().c_str());
		long int righti = atol(right->getText().c_str());
		replace(new ast(TOK_INT, to_string(lefti%righti)));
	}  else {
		throw "ERROR: arguments to '%' not applicable";
	}
}

void do_exp() {
	ast* left = execution_ptr->getChildren()[0];
	ast* right = execution_ptr->getChildren()[1];
	if ((left->getToken() == TOK_INT || left->getToken() == TOK_FLOAT)
		    && (right->getToken() == TOK_INT || right->getToken() == TOK_FLOAT)) {
		double leftf  = atof(left->getText().c_str());
		double rightf = atof(right->getText().c_str());
		replace(new ast(TOK_FLOAT, to_string(pow(leftf,rightf))));
	} else {
		throw "ERROR: arguments to '^' not applicable";
	}
}

void do_list_access() {
	ast* list = execution_ptr->getChildren()[0];
	ast* idx  = execution_ptr->getChildren()[1];
	if (idx->getToken() == TOK_INT && list->getToken() == TOK_START_LIST) {
		long int index = atol(idx->getText().c_str());
		ast* crawler = list->getChildren()[0];
		while (crawler->getToken() != TOK_END_LIST && index > 0) {
			crawler = crawler->getChildren()[0];
			--index;
		}
		if (crawler->getToken() != TOK_END_LIST && index == 0) {
			replace(crawler->getChildren()[1]);
		} else {
			throw "ERROR: index greater than size of list";
		}
	} else {
		throw "ERROR: arguments to '!!' not applicable";
	}
}

void do_list_prepend() {
	ast* val = execution_ptr->getChildren()[1];
	ast* list = execution_ptr->getChildren()[0];
	ast* oldFront;
	if (list->getToken() == TOK_START_LIST && val->getToken() == (oldFront = list->getChildren()[0])->getChildren()[1]->getToken()) {
		ast* newFront = new ast(TOK_COMMA,",");
		newFront->addChild(oldFront);
		newFront->addChild(val);
		list->replaceChild(0,newFront);
		replace(list);
	} else if (list->getToken() == TOK_EMPTY_LIST) {
		string end = list->getText().substr(1,1);
		string front = list->getText().substr(0,1);
		ast* endNode = new ast(TOK_END_LIST,end);
		ast* valNode = new ast(TOK_COMMA,",");
		ast* frontNode = new ast(TOK_START_LIST,front);
		valNode->addChild(endNode);
		valNode->addChild(val);
		frontNode->addChild(valNode);
		replace(frontNode);
	}else {
		throw "ERROR: arguments to ':' not applicable";
	}
}

void do_concat() {
	ast* left  = execution_ptr->getChildren()[0];
	ast* right = execution_ptr->getChildren()[1];
	if (left->getToken() == TOK_START_LIST && right->getToken() == TOK_START_LIST) {
		ast* crawler = left->getChildren()[0];
		while (crawler->getToken() != TOK_END_LIST) {
			crawler = crawler->getChildren()[0];
		}
		crawler = crawler->getParent();
		crawler->replaceChild(0,right->getChildren()[0]);
		replace(left);
	} else {
		throw "ERROR: arguments to '++' not applicable";
	}
}

void do_obj_access() {
	ast* obj = execution_ptr->getChildren()[0];
	ast* key = execution_ptr->getChildren()[1];
	if (obj->getToken() == TOK_START_OBJ && key->getToken() == TOK_IDENT) {
		string& strKey = key->getText();
		for (auto field : obj->getChildren()) {
			if (strKey == field->getText()) {
				replace(field->getChildren()[0]);
				return;
			}
		}
		throw "ERROR: key not found in object";
	} else {
		throw "ERROR: arguments to '.' not applicable";
	}
}

void do_parens() {
	if (execution_ptr->getToken() == TOK_OPEN_PAREN && execution_ptr->getChildren().size() >= 1
			&& (execution_ptr->getParent() == nullptr 
				|| (execution_ptr->getParent() != nullptr 
					&& execution_ptr->getParent()->getToken() != TOK_IDENT))) {
		replace(execution_ptr->getChildren()[0]);
	}
}

void do_if() {
	int condition = execution_ptr->getChildren()[0]->getToken();
	ast* then_node = execution_ptr->getChildren()[1];
	ast* else_node = execution_ptr->getChildren()[2];
	if (condition == TOK_TRUE) {
		replace(then_node);
	} else if (condition == TOK_FALSE){
		replace(else_node);
	}
	move_to_leaf();
}

bool verify_type(ast* arg, ast* paramType) {
	int typeToken = paramType->getToken();
	if (typeToken == TOK_TYPE) {
		auto type = types.find(paramType->getText());
		if (type == types.end()) {
			return false;
		} else {
			return verify_type(arg,type->second);
		}
	} else if (typeToken == TOK_OR) {
		if (verify_type(arg,paramType->getChildren()[0])) {
			return true;
		} else {
			return verify_type(arg,paramType->getChildren()[1]);
		}
	} else if (typeToken == TOK_START_LIST) {
		if (arg->getToken() != TOK_START_LIST && arg->getToken() != TOK_EMPTY_LIST) {
			return false;
		} else if (paramType->getChildren().size() > 0 && arg->getChildren().size() > 0) {
			return verify_type(arg->getChildren()[0]->getChildren()[1], paramType->getChildren()[0]);
		} else if (paramType->getChildren().size() > 0 && arg->getChildren().size() <= 0) {
			return false;
		}
	} else if (typeToken == TOK_START_OBJ) {
		if (arg->getToken() != TOK_START_OBJ) {
			return false;
		} else if (paramType->getChildren().size() > 0 && arg->getChildren().size() > 0) {
			for (ast* paramField : paramType->getChildren()) {
				bool found = false;
				for (ast* argField : arg->getChildren()) {
					if (argField->getText() == paramField->getText()) {
						found = true;
						if (paramField->getChildren().size() > 0) {
							return verify_type(argField->getChildren()[0],paramField->getChildren()[0]);
						}
						break;
					}
				}
				if (!found) {
					return false;
				}
			}
		} else if (paramType->getChildren().size() > 0 && arg->getChildren().size() <= 0) {
			return false;
		}
	} else if (arg->getToken() != typeToken) {
		return false;
	}
	return true;
}

void verify_types(ast* args, ast* params) {
	if (args->getChildren().size() != params->getChildren().size()) {
		throw "ERROR: incorrect number of arguments";
	}
	int idx = 0;
	for (ast* param : params->getChildren()) {
		if (param->getChildren().size() >= 1) {
			if (!verify_type(args->getChildren()[idx++],param->getChildren()[0])) {
				throw "ERROR: type mismatch";
			}
		}
	}
}

ast* copy_tree(ast* tree) {
	ast* copy = new ast(tree->getToken(),tree->getText());
	for (auto child : tree->getChildren()) {
		copy->addChild(copy_tree(child));
	}
	return copy;
}

ast* replace_idents(ast* node, ast* params, ast* args) {
	int i = 0;
	for (auto param : params->getChildren()) {
		if (node->equals(*param)) {
			ast* arg = args->getChildren()[i];
			node->setText(arg->getText());
			node->setToken(arg->getToken());
			for (ast* child : arg->getChildren()) {
				node->addChild(copy_tree(child));
			}
			break;
		}
		++i;
	}
	for (auto child : node->getChildren()) {
		replace_idents(child, params, args);
	}
	return node;
}

void do_call() {
	if (execution_ptr->getParent() == nullptr || (execution_ptr->getParent() != nullptr && execution_ptr->getParent()->getToken() != TOK_START_OBJ)) {
		auto function = identifiers.find(execution_ptr->getText());
		if (function != identifiers.end()) {
			ast* params = function->second.first;
			ast* impl = copy_tree(function->second.second);
			ast* args = execution_ptr->getChildren()[0];
			verify_types(args, params);
			if (impl->getToken() != TOK_NATIVE_START) {
				replace(replace_idents(impl,params,args));
			} else {
				string fnName = impl->getText();
				auto fn = native_functions[fnName];
				if (fn != nullptr) {
					replace(fn(args));
				} else {
					throw ("Function not found: "+execution_ptr->getText()).c_str();
				}
			}
			move_to_leaf();
		} else {
			throw ("Function not found: "+execution_ptr->getText()).c_str();
		}
	} else {
		move_up();
	}
}

map<int,void(*)()> operations {
	{TOK_NOT,                do_not},
	{TOK_AND,                do_and},
	{TOK_OR,                 do_or},
	{TOK_EQUALS,             do_equals},
	{TOK_NOT_EQUALS,         do_nequals},
	{TOK_LESS_THAN,          do_lt},
	{TOK_LESS_THAN_OR_EQ,    do_lt_eq},
	{TOK_GREATER_THAN,       do_gt},
	{TOK_GREATER_THAN_OR_EQ, do_gt_eq},
	{TOK_ADD,                do_add},
	{TOK_SUB,                do_sub},
	{TOK_MUL,                do_mul},
	{TOK_DIV,                do_div},
	{TOK_MOD,                do_mod},
	{TOK_EXP,                do_exp},
	{TOK_LIST_ACCESS,        do_list_access},
	{TOK_LIST_PREPEND,       do_list_prepend},
	{TOK_OBJ_ACCESS,         do_obj_access},
	{TOK_CONCAT,             do_concat},
	{TOK_OPEN_PAREN,         do_parens},
	{TOK_IF,                 do_if},
	{TOK_IDENT,              do_call}
};

void execute(ast* root) {
	init_natives();
	execution_root = root;
	execution_ptr  = root;
	move_to_leaf();
	composeLine(execution_root);
	auto fn = operations[execution_ptr->getToken()];
	while (fn != nullptr || execution_ptr != execution_root) {
		changed = false;
		if (fn != nullptr) {
			try {
				fn();
				if (changed) {
					composeLine(execution_root);
				}
			} catch (const char* msg) {
				cerr << msg << endl;
				break;
			}
		}
		if (fn != do_call && fn != do_if && execution_ptr->getParent() != nullptr 
				&& index < execution_ptr->getParent()->getChildren().size()-1 && execution_ptr->getParent()->getToken() != TOK_IF) {
			move_over();
			move_to_leaf();
		} else if (fn != do_call && fn != do_if && execution_ptr->getParent() != nullptr) {
			move_up();
		}
		fn = operations[execution_ptr->getToken()];
	}
}
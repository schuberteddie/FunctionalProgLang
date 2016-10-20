#ifndef __AST__
#define __AST__

#include <vector>
#include <string>
using namespace std;

class ast {
	private:
		int token;
		string text;
		vector<ast*>* children;
		ast* parent;
	public:
		ast(int, string);
		~ast();
		bool equals(ast&);
		int getToken();
		void setToken(int);
		string& getText();
		void setText(string);
		vector<ast*>& getChildren();
		ast* addChild(ast*);
		void replaceChild(int,ast*);
		void addChildToFront(ast*);
		ast* getParent();
		void setParent(ast*);
};

#endif
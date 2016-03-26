#ifndef __SYNTAX_TREE_H__
#define __SYNTAX_TREE_H__

typedef unsigned bool;
#define true 1
#define false 0

#define MAX_CHILDREN 10 // the number of children in c-- syntax will not exceed MAX_CHILDREN
#define MAX_LEN 20 // the length of symbol and name shall not exceed MAX_LEN
#define INDENT 2

typedef struct Node
{
	char *symbol, *name;
	// "symbol" stores the kind of the symbols, while "name" stores the specific name of the symbol in the program
	int lineno;
	union {
		int intVal;
		float floatVal;
	};
	int arity; // if arity > 0, then this Node is nonterminal, otherwise it is terminal
	struct Node *children[MAX_CHILDREN];
} Node;

Node *createNode(int arity, ...);
int deleteNode(Node *p);
void printTree(Node *p, int depth);
void deleteTree(Node *p);

#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "symbol_table.h"
#include "syntax_tree.h"
#include "common.h"

SymbolNode *HashTable[HASH_MASK];
SymbolStackNode *SymbolStackHead = NULL;

unsigned int hashSymbol(const char *name)
{
	unsigned int val = 0, i;
	for (; *name; name++) {
		val = (val << 2) + *name;
		if (i = val & ~HASH_MASK)
			val = (val ^ (i >> 12)) & HASH_MASK;
	}
	return val;
}

bool searchSymbol(const char *name)
{
	SymbolNode *temp = HashTable[hashSymbol(name)]; // generate the hash slot
	for (; temp != NULL; temp = temp->HashNext) {
		if (temp->toHead == SymbolStackHead && STREQ(temp->text, name))
			return true;
	}
	return false;
}

// Given the symbol name, return an empty SymbolNode generated at the correct position.
SymbolNode *pushinSymbol(const char *name)
{
	SymbolNode *hashslot = HashTable[hashSymbol(name)]; // generate the hash slot
	SymbolNode *stackslot = SymbolStackHead->SymbolHead;
	SymbolNode *newnode = (SymbolNode *) malloc(sizeof(SymbolNode));
	
	if (hashslot == NULL) {
		hashslot = newnode;
		newnode->HashNext = NULL;
	}
	else {
		newnode->HashNext = hashslot;
		hashslot = newnode;
	}
	
	if (stackslot == NULL) {
		stackslot = newnode;
		newnode->StackNext = NULL;
	}
	else {
		newnode->StackNext = stackslot;
		stackslot = newnode;
	}
	
	newnode->toHead = SymbolStackHead;
	
	// remember that we did not modify the original pointer
	HashTable[hashSymbol(name)] = hashslot;
	SymbolStackHead->SymbolHead = stackslot;
	
	return newnode;
}

void clearSymbolStack()
{
	while (SymbolStackHead != NULL) {
		SymbolStackNode *p = SymbolStackHead;
		SymbolStackHead = SymbolStackHead->next;
		while (p->SymbolHead != NULL) {
			SymbolNode *q = p->SymbolHead;
			p->SymbolHead = p->SymbolHead->StackNext;
			free(q);
		}
		free(p);
	}
	int i;
	for (i = 0; i < HASH_MASK; i++)
		HashTable[i] = NULL;
	 
}

// ExtDef -> Specifier ExtDecList SEMI 
//         | Specifier SEMI
//         | Specifier FunDec CompSt
void procExtDef(TreeNode *p)
{
	if (STREQ(p->children[1]->symbol, "ExtDecList")) {
	}
	else if (STREQ(p->children[1]->symbol, "FunDec")) {
		buildSymbolTable(p->children[2]);
	}
}

// Def -> Specifier Dec COMMA Dec COMMA Dec ... SEMI
// Dec -> VarDec
//      | VarDec AssignOP Exp
void procDef(TreeNode *p)
{
	// Specifier can be TYPE or StructSpecifier
	TreeNode *spec = p->children[0]->children[0];
	Type nodetype;
	if (STREQ(spec->symbol, "TYPE")) {
		nodetype.kind = BASIC;
		if (STREQ(spec->text, "INT")) nodetype.basic = B_INT;
		else nodetype.basic = B_FLOAT;
	}
	else {
		// implement structure later
		assert(0);
	}
	TreeNode *temp = p->children[1];
	while (!STREQ(temp->symbol, "DecList")) {
		procVarDec(nodetype, temp->children[0]->children[0]);
		temp = temp->children[2];
	}
	procVarDec(nodetype, temp->children[0]->children[0]);
}

// VarDec -> ID
//         | VarDec LB INT RB
void procVarDec(Type nodetype, TreeNode *p)
{
	if (p->arity == 1) {
		SymbolNode *newnode = pushinSymbol(p->children[0]->text);
		strcpy(newnode->text, p->children[0]->text);
		newnode->isfunc = false, newnode->isdef = true;
		newnode->lineno = p->children[0]->lineno;
		newnode->VarMsg = nodetype;
	}
	else {
		// implement array later
		assert(0);
	}
}

void procExp(TreeNode *p)
{
	int i;
	for (i = 0; i < p->arity; i++) {
		if (p->arity == 1 && STREQ(p->children[i]->symbol, "ID")) {
			if(!searchSymbol(p->children[i]->text))
				printf("Error type 1 at line %d: Undefined variable \"%s\"\n", p->children[i]->lineno, p->children[i]->text);
		}
		if (p->arity > 1 && STREQ(p->children[i]->symbol, "Exp"))
			procExp(p->children[i]);
	}
}



void buildSymbolTable(TreeNode *p)
{
	if (STREQ(p->symbol, "ExtDef")) {
		// if it is a global definition (can be a function or a variable)
		procExtDef(p);
		return;
	}
	if (STREQ(p->symbol, "Def")) {
		// if it is a local variable definition
		procDef(p);
		return;
	}
	if (STREQ(p->symbol, "Exp")) {
		// if it is a variable use or function call
		procExp(p);
		return;
	}
	if (STREQ(p->symbol, "LC")) {
		// push a new field into the stack
		SymbolStackNode *newstacknode = (SymbolStackNode *) malloc(sizeof(SymbolStackNode));
		newstacknode->next = SymbolStackHead;
		SymbolStackHead = newstacknode;
		SymbolStackHead->SymbolHead = NULL;
	}
	if (STREQ(p->symbol, "RC")) {
		// pop out the field at the top of the stack
		SymbolStackNode *nodetodelete = SymbolStackHead;
		SymbolStackHead = SymbolStackHead->next;
		while (nodetodelete->SymbolHead != NULL) {
			SymbolNode *temp = nodetodelete->SymbolHead;
			nodetodelete->SymbolHead = nodetodelete->SymbolHead->StackNext;
			free(temp);
		}
		free(nodetodelete);
	}
	int i;
	for (i = 0; i < p->arity; i++)
		buildSymbolTable(p->children[i]);

}


void procSymbolTable(TreeNode *p)
{
	clearSymbolStack();
	SymbolStackHead = (SymbolStackNode *) malloc(sizeof(SymbolStackNode)); // global symbol table
	SymbolStackHead->next = NULL;
	SymbolStackHead->SymbolHead = NULL;
	buildSymbolTable(p);
	printf("ohhhhhhh!%s\n", p->text);
	clearSymbolStack();
}


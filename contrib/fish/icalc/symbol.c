/*
*	symbol-table management routines for complex-number expression parser.
*	MWS, March 17, 1991.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "memory.h"
#include "complex.h"
#include "complex.tab.h"

extern void cprin(FILE *, char *prefix, char *suffix, Complex z);

/* Prototypes for static functions defined in symbol.c */
static void insert(Symbol **tree, Symbol *item);
static void traverse(Symbol *tree, short type, void (*visit)(Symbol *));
static void printval(Symbol *item);
static void printname(Symbol *item);

static Symbol	*symtree = NULL;	/* symbol table: binary tree */

Symbol *lookup(s)			/* find s in symbol table */
	char *s;
{
	Symbol *sp = symtree;
	int cmp;

	while (sp)
	{
		cmp = strcmp(s, sp->name);
		if (cmp == 0)
			break;
		else if (cmp < 0)
			sp = sp->left;
		else  /* cmp > 0 */
			sp = sp->right;
	}

	return sp;
}

static void insert(tree, item)		/* no duplicate names permitted... */
	Symbol **tree, *item;
{
	if (*tree == NULL)
	{
		*tree = item;
		item->left = item->right = NULL;
	}
	else if (strcmp(item->name, (*tree)->name) < 0)
		insert(&(*tree)->left, item);
	else
		insert(&(*tree)->right, item);
}

Symbol *allocsym(s, t)			/* allocate a symbol */
	char *s;
	int t;
{
	Symbol *sp;
	
	sp = (Symbol *) emalloc(sizeof(Symbol));
	sp->name = emalloc(strlen(s)+1);
	strcpy(sp->name, s);
	sp->type = t;

	return sp;
}

Symbol *install(s, t, cval)		/* install s in symbol table */
	char *s;
	int t;
	Complex cval;
{
	Symbol *sp = allocsym(s, t);
	sp->u.val = cval;

	insert(&symtree, sp);
	return sp;
}

static void traverse(Symbol *tree,short type, void (*visit)(Symbol *))	/* inorder traversal of tree */
{
	if (tree)
	{
		traverse(tree->left, type, visit);
		if (tree->type == type)
			(*visit)(tree);
		traverse(tree->right, type, visit);
	}
}

static void printval(item)
	Symbol *item;
{
	fprintf(stdout, "\t%s\t= ", item->name);
	cprin(stdout, NULL, "\n", item->u.val);
}

static void printname(item)
	Symbol *item;
{
	fprintf(stdout,"\t%s\n", item->name);
}

void printlist(type)		/* print names of symbols with given type */
	int type;		/* simple-minded at moment */
{
	if (type == CONST || type == VAR)
		traverse(symtree, type, printval);
	else
		traverse(symtree, type, printname);
}

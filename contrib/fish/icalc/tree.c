/*
*	tree creation, deletion and evaluation routines for complex-number
*	parser.
*
*	MWS, March 20, 1991.
*/

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "memory.h"
#include "complex.h"
#include "complex.tab.h"

#define	allocnode()	rem_malloc(sizeof(Node))

Node *n_asgn(sym, arg)			/* node for assignment operator */
	Symbol *sym;
	Node *arg;
{
	Node *n = allocnode();

	n->type = '=';
	n->contents.sym = sym;
	n->left = arg;
	n->right = NULL;

	return n;
}

Node *n_binop(op, left, right)		/* node for binary operator */
	int op;
	Node *left, *right;
{
	Node *n = allocnode();

	n->type = op;
	n->left = left;
	n->right = right;

	return n;
}

Node *n_unop(op, arg)			/* node for unary operator */
	int op;
	Node *arg;
{
	Node *n = allocnode();

	n->type = op;
	n->left = arg;
	n->right = NULL;

	return n;
}

Node *n_func(type, sym, arg)		/* node for function */
	int type;
	Symbol *sym;
	Node *arg;
{
	Node *n = allocnode();

	n->type = type;
	n->contents.sym = sym;
	n->left = arg;
	n->right = NULL;

	return n;
}

Node *n_symbol(type, sym)		/* node for symbol - VAR or CONST */
	int type;
	Symbol *sym;
{
	Node *n = allocnode();

	n->type = type;
	n->contents.sym = sym;
	n->left = NULL;
	n->right = NULL;

	return n;
}

Node *n_number(real, imag)	/* node for number */
	double real, imag;
{
	Node *n = allocnode();

	n->type = NUMBER;
	n->contents.val.real = real;
	n->contents.val.imag = imag;
	n->left = NULL;
	n->right = NULL;

	return n;
}

Complex eval_tree(n)		/* evaluate the complex value of a tree */
	Node *n;
{
	switch (n->type)
	{
	case NUMBER:	return n->contents.val;

	case C_BLTIN:	return (*(n->contents.sym->u.cptr))(eval_tree(n->left));

	case R_BLTIN: {	Complex rv;
			rv.real = (*(n->contents.sym->u.rptr))(eval_tree(n->left));
			rv.imag = 0.0;
			return rv;
		      }

	case UFUNC:   { UserFunc *uf = &n->contents.sym->u.ufunc;
			uf->param->u.val = eval_tree(n->left);
			return eval_tree(uf->tree);
		      }

	case VAR:
	case PARAMETER:
	case CONST:	return n->contents.sym->u.val;

	case '+':	return cadd(eval_tree(n->left), eval_tree(n->right));
	case '-':	return csub(eval_tree(n->left), eval_tree(n->right));
	case '*':	return cmul(eval_tree(n->left), eval_tree(n->right));
	case '/':	return cdiv(eval_tree(n->left), eval_tree(n->right));
	case '^':	return cpow(eval_tree(n->left), eval_tree(n->right));

	case '(':	return eval_tree(n->left);
	case UMINUS:	return cneg(eval_tree(n->left));
	case '\'':	return conj(eval_tree(n->left));

	case '=':	return n->contents.sym->u.val = eval_tree(n->left);

	default:	/* should NEVER see this... */
			execerror("internal - unknown node-type", NULL);
	}

	return ({Complex zero={}; zero;});
}

/* delete all nodes of a tree */
/* not used in current implementation */
/********
void delete_tree(n)
	Node *n;
{
	if (n)
	{
		delete_tree(n->left);
		delete_tree(n->right);
		free(n);
	}
}
********/

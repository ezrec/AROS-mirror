/*
*	various user-commands for complex-number expression parser.
*	MWS, March 20, 1991.
*/
#include <stdio.h>
#include <math.h>
#include "complex.h"
#include "complex.tab.h"

extern int silent;

void quiticalc()	/* guess... */
{
	exit(0);
}

void besilent()
{
	silent = 1;
}

void beverbose()
{
	silent = 0;
}

void builtins()
{
	fprintf(stdout,"complex-valued functions available:\n");
	printlist(C_BLTIN);
	fprintf(stdout,"real-valued functions available:\n");
	printlist(R_BLTIN);
}

void userfuncs()
{
	fprintf(stdout,"user-defined functions available:\n");
	printlist(UFUNC);
}

void consts()
{
	fprintf(stdout,"predefined constants available:\n");
	printlist(CONST);
}

void vars()
{
	fprintf(stdout,"user-variables defined:\n");
	printlist(VAR);
}

void help()
{
	fprintf(stdout,"commands available:\n");
	printlist(COMMAND);
}

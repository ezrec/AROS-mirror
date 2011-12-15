/*
*	initial symbol-table setup for complex-number expression parser.
*	MWS, March 17, 1991.
*/
#include <math.h>
#include "complex.h"
#include "complex.tab.h"
#define PI 3.141592653589793

extern const Complex zero, eye;

static struct {		/* keywords */
	char	*name;
	int	kval;
} keywords[] = {
	{ "func",	FUNCDEF, },
	{ "prec",	PRECISION, },
	{ "repeat", REPEAT, },
	{ NULL,	NULL },
};

static struct {		/* commands */
	char	*name;
	void	(*func)();
} commands[] = {
	{ "exit", quiticalc, },
	{ "quit", quiticalc, },
	{ "help",	help, },
	{ "vars",	vars, },
	{ "consts",consts, },
	{ "builtins",builtins, },
	{ "functions", userfuncs, },
	{ "silent", besilent, },
	{ "verbose", beverbose, },
	{ NULL,	NULL },
};

static struct {		/* constants */
	char	*name;
	Complex	cval;
} constants[] = {
	{ "ans",	{  0.0, 0.0 }, },
	{ "PI",	{  PI, 0.0 }, },
	{ "E",	{  2.71828182845904523536, 0.0 }, },
	{ "GAMMA",{  0.57721566490153286060, 0.0 }, },
	{ "DEG",	{ 57.29577951308232087680, 0.0 },	 },
	{ "PHI",	{  1.61803398874989484820, 0.0 }, },
	{ "LOG10",{  2.30258509299404568402, 0.0 }, },
	{ "LOG2",	{  0.69314718055994530942, 0.0 }, },
	{ NULL,	{  0.0, 0.0 } },
};

static struct {		/* complex-valued functions */
	char	*name;
	Complex	(*func)();
} c_builtins[] = {
	{ "sin",	csin, },
	{ "cos",	ccos, },
	{ "tan",	ctan, },
	{ "asin",	casin, },
	{ "acos",	cacos, },
	{ "atan",	catan, },
	{ "sinh",	csinh, },
	{ "cosh",	ccosh, },
	{ "tanh",	ctanh, },
	{ "sqr",	csqr, },
	{ "sqrt",	csqrt, },
	{ "conj",	conj, },
	{ "ln",	clog, },
	{ "exp",	cexp, },
	{ NULL,	NULL },
};

static struct {		/* real-valued functions */
	char	*name;
	double	(*func)();
} r_builtins[] = {
	{ "abs",	_cabs, },
	{ "norm",	norm, },
	{ "arg",	arg, },
	{ "Re",	Re, },
	{ "Im",	Im, },
	{ NULL,	NULL },
};

void init()	/* install constants and built-ins in table */
{
	extern Symbol *ans;
	unsigned short i;
	Symbol *s;

	for (i = 0; keywords[i].name; i++)
		install(keywords[i].name, keywords[i].kval, zero);

	for (i = 0; commands[i].name; i++)
	{
		s = install(commands[i].name, COMMAND, zero);
		s->u.vptr = commands[i].func;
	}

	for (i = 0; constants[i].name; i++)
		install(constants[i].name, CONST, constants[i].cval);
	ans = lookup("ans");	/* previous answer */

	for (i = 0; c_builtins[i].name; i++)
	{
		s = install(c_builtins[i].name, C_BLTIN, zero);
		s->u.cptr = c_builtins[i].func;
	}
	for (i = 0; r_builtins[i].name; i++)
	{
		s = install(r_builtins[i].name, R_BLTIN, zero);
		s->u.rptr = r_builtins[i].func;
	}
}


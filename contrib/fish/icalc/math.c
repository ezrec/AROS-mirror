/*
*	standard (real) math routines with domain/range checking, for 
*	complex-number expression parser.
*	MWS, March 17, 1991.
*/
#include <math.h>
#include <errno.h>
#include "complex.h"
//#include <libraries/mathieeedp.h>

extern	int	errno;
extern struct Library MathIeeeDoubTransBase;

double Sqrt(x)
	double x;
{
	return errcheck(sqrt(x), "sqrt");
}

double Log(x)
	double x;
{
	return errcheck(log(x), "log");
}

double Asin(x)
	double x;
{
	return errcheck(asin(x), "asin");
}

double Acos(x)
	double x;
{
	return errcheck(acos(x), "acos");
}

double errcheck(d, s)	/* check result of library call */
	double d;	/* doesn't seem to work under Lattice... */
	char *s;
{
	if (errno == EDOM) {
		errno = 0;
		execerror(s, "argument out of domain");
	} else if (errno == ERANGE) {
		errno = 0;
		execerror(s, "result out of range");
	}
	return d;
}

/*
 * $Header$
 * $Log$
 * Revision 1.1  2001/04/04 05:43:37  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lerror.h>
#include <lstring.h>

/* ---------------- Lintdiv ---------------- */
void
Lintdiv( const PLstr to, const PLstr A, const PLstr B )
{
	double	b;

	b = Lrdreal(B);

	if (b == 0) Lerror(ERR_ARITH_OVERFLOW,0);

	LINT(*to)  = (long) (Lrdreal(A) / b);
	LTYPE(*to) = LINTEGER_TY;
	LLEN(*to)  = sizeof(long);
} /* Lintdiv */

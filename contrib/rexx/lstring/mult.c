/*
 * $Header$
 * $Log$
 * Revision 1.1  2001/04/04 05:43:38  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* ------------------- Lmult ----------------- */
void
Lmult( const PLstr to, const PLstr A, const PLstr B)
{
	L2NUM(A);
	L2NUM(B);

	if ((LTYPE(*A)==LINTEGER_TY) && (LTYPE(*B)==LINTEGER_TY)) {
		LINT(*to)  = LINT(*A) * LINT(*B);
		LTYPE(*to) = LINTEGER_TY;
		LLEN(*to)  = sizeof(long);
	} else {
		LREAL(*to) = TOREAL(*A) * TOREAL(*B);
		LTYPE(*to) = LREAL_TY;
		LLEN(*to)  = sizeof(double);
	}
} /* Lmult */

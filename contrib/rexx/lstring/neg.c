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

/* ------------------- Lneg ------------------ */
void
Lneg( const PLstr to, const PLstr num )
{
	L2NUM(num);

	if (LTYPE(*num)==LINTEGER_TY) {
		LINT(*to)  = -LINT(*num);
		LTYPE(*to) = LINTEGER_TY;
		LLEN(*to)  = sizeof(long);
	} else {
		LREAL(*to) = -LREAL(*num);
		LTYPE(*to) = LREAL_TY;
		LLEN(*to)  = sizeof(double);
	}
} /* Lneg */

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

#include <lerror.h>
#include <lstring.h>

/* ------------------ Lmod ----------------- */
void
Lmod( const PLstr to, const PLstr A, const PLstr B )
{
	L2REAL(A);
	L2REAL(B);

	if (LREAL(*B) == 0) Lerror(ERR_ARITH_OVERFLOW,0);

	LREAL(*to) = (double) (LREAL(*A) - (long)(LREAL(*A) / LREAL(*B)) * LREAL(*B));
	LTYPE(*to) = LREAL_TY;
	LLEN(*to)  = sizeof(double);
} /* Lmod */

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

#include <lstring.h>

/* ------------------- Linc ------------------ */
void
Linc( const PLstr num )
{
	L2NUM(num);

	if (LTYPE(*num)==LINTEGER_TY)
		LINT(*num) += 1;
	else
		LREAL(*num) += 1.0;
} /* Linc */

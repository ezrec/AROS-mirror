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

#include <math.h>
#include <lstring.h>

/* ----------------- Lpow --------------------- */
void
Lpow( const PLstr to, const PLstr num, const PLstr p )
{
	L2REAL(num);
	L2REAL(p);
	Lrcpy(to, pow(LREAL(*num),LREAL(*p)));
} /* Lpow */

/*
 * $Header$
 * $Log$
 * Revision 1.1  2001/04/04 05:43:38  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 * Revision 1.1  1998/07/02 17:20:58  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* ----------------- Lwords ------------------ */
long
Lwords( const PLstr from )
{
	long	i=0,r=0;

	L2STR(from);
	for (;;) {
		LSKIPBLANKS(*from,i);
		if (i>=LLEN(*from)) return r;
		r++;
		LSKIPWORD(*from,i);
	}
} /* Lwords */

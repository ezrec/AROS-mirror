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

/* --------------- Lwordlength ----------------- */
long
Lwordlength( const PLstr from, long n )
{
	long	i;

	i = Lwordindex(from,n);
	if (i==0) return 0;
	n = i--;
	LSKIPWORD(*from,i);
	return i-n+1;
} /* Lwordlength */

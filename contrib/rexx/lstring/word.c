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

/* ------------------ Lword ------------------- */
void
Lword( const PLstr to, const PLstr from, long n )
{
	long	i;

	i = Lwordindex(from,n);
	if (n<=0) n = 1;
	if (i==0) {
		LZEROSTR(*to);
		return;
	}
	n = i-1;
	LSKIPWORD(*from,n);
	_Lsubstr(to,from,i,n-i+1);
} /* Lword */

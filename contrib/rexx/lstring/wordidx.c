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

/* ----------------- Lwordindex ---------------- */
long
Lwordindex( const PLstr str, long n )
{
	long	p;

	L2STR(str);

	if ((LLEN(*str)==0) || (n<=0))  return 0;
	for (p=0;;) {
		LSKIPBLANKS(*str,p);
		if (p>=LLEN(*str)) return 0;
		n--;
		if (!n) return p+1;
		LSKIPWORD(*str,p);
	}
} /* Lwordindex */

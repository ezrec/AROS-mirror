/*
 * $Header$
 * $Log$
 * Revision 1.1  2001/04/04 05:43:38  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 * Revision 1.2  1998/11/06 08:55:18  bnv
 * Corrected: When type != string
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* ------------------ Lspace --------------- */
void
Lspace( const PLstr to, const PLstr from, long n, const char pad )
{
	size_t	p,lp;
	Lstr	space, sub;

	if (LTYPE(*from) != LSTRING_TY) {
		Lstrcpy(to,from);
		return;
	}

	LINITSTR(space);
	LINITSTR(sub);

	if (n<0) n = 0;

	Lstrset(&space,(size_t)n,pad);

	p = 0;
	LZEROSTR(*to);
	LSKIPBLANKS(*from,p);

	for (;;) {
		lp = p;
		LSKIPWORD(*from,p);
		_Lsubstr(&sub,from,lp+1,p-lp);
		Lstrcat(to,&sub);
		LSKIPBLANKS(*from,p);
		if (p>=LLEN(*from)) break;
		Lstrcat(to,&space);
	}

	LFREESTR(space);
	LFREESTR(sub);
} /* Lspace */

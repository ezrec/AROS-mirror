/*
 * $Header$
 * $Log$
 * Revision 1.1  2001/04/04 05:43:37  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 * Revision 1.1  1998/07/02 17:17:00  bnv
 * Initial revision
 *
 */

#include <lstring.h>

/* ---------------- Ldelstr ------------------- */
void
Ldelstr( const PLstr to, const PLstr from, long start, long length )
{
	Lstr	tmp;

	L2STR(from);

	start--;
	if (start<0) start = 0;
	if (start>=LLEN(*from)) {
		Lstrcpy(to,from);
		return;
	}

	if (start)
		_Lsubstr(to,from,1,(size_t)start);
	else
		LZEROSTR(*to);

	if (length>0) 
	if (start+length < LLEN(*from)) {
		LINITSTR(tmp);
		_Lsubstr(&tmp, from, (size_t)(start+length+1), 0);
		Lstrcat(to,&tmp);
		LFREESTR(tmp);
	}
} /* Ldelstr */

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

/* ------------------ Lreverse ------------------- */
void
Lreverse( const PLstr s )
{
	long	i;
	char	c,*cf,*cl;

	L2STR(s);
	if (LLEN(*s)==0)
		return;
	cf = LSTR(*s);  cl = cf + LLEN(*s) - 1;
	i = LLEN(*s) / 2;
	while (i--) {
		c = *cf;
		*cf++ = *cl;
		*cl-- = c;
	}
} /* Lreverse */

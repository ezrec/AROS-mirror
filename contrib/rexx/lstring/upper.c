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

/* --------------- Lupper ------------------ */
void
Lupper( const PLstr s )
{
	size_t	i;
	L2STR(s);
	for (i=0; i<LLEN(*s); i++)
		LSTR(*s)[i] = l2u[ (byte) LSTR(*s)[i] ];
} /* Lupper */

/*
 * $Header$
 * $Log$
 * Revision 1.1  2001/04/04 05:43:37  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 * Revision 1.2  1999/11/26 09:53:28  bnv
 * Changed: To use the new macros.
 *
 * Revision 1.1  1998/07/02 17:17:00  bnv
 * Initial revision
 *
 */

#include <lstring.h>

/* ---------------- Lcharout ------------------- */
void
Lcharout( FILEP f, const PLstr line, const long start )
{
	if (start>=0)
		FSEEK(f,start,SEEK_SET);

	Lwrite(f,line,FALSE);
} /* Lcharout */

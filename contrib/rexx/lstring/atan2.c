/*
 * $Header$
 * $Log$
 * Revision 1.1  2001/04/04 05:43:37  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 * Revision 1.1  1998/07/02 17:16:35  bnv
 * Initial revision
 *
 */

#include <math.h>
#include <lstring.h>

/* ----------------- Lpow --------------------- */
void
Latan2( const PLstr to, const PLstr x, const PLstr y )
{
	Lrcpy(to, atan2(Lrdreal(x),Lrdreal(y)));
} /* Latan2 */

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

/* ------------------ Lxrange -------------------- */
void
Lxrange( const PLstr to, byte start, byte stop )
{
	word	c;
	char	*r;

	start = start & 0xFF;
	stop  = stop  & 0xFF;

	if (start <= stop) {
		Lfx(to, stop-start+1 );
		r = LSTR(*to);
		for (c=start; c<=stop; c++)
			*r++ = (char)c;
		LLEN(*to) = stop-start+1;
	} else {
		Lfx( to, 257 - (start-stop));
		r = LSTR(*to);
		for (c=start; c<=0xFF; c++)
			*r++ = (char)c;
		for (c=0x00; c<=stop; c++)
			*r++ = (char)c;
		LLEN(*to) = 257 - (start-stop);
	}
	LTYPE(*to) = LSTRING_TY;
} /* R_xrange */

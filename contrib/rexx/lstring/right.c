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

#include <string.h>
#include <lstring.h>

/* ------------------ Lright ------------------- */
void
Lright( const PLstr to, const PLstr from, const long length, const char pad)
{
	L2STR(from);

	if (length<=0) {
		LZEROSTR(*to);
		return;
	}

	if (length > LLEN(*from)) {
		Lstrset(to, length-LLEN(*from), pad );
		Lstrcat(to, from);
	} else
		Lsubstr(to, from, LLEN(*from)-length+1,LREST,pad);
} /* Lright */

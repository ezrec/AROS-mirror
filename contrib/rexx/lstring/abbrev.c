/*
 * $Header$
 * $Log$
 * Revision 1.1  2001/04/04 05:43:37  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 * Revision 1.1  1998/07/02 17:16:15  bnv
 * Initial revision
 *
 */

#include <lstring.h>

/* ------------------- Labbrev -------------------- */
bool
Labbrev(const PLstr information, const PLstr info, long length)
{
	long	i;
	bool	more;

	L2STR(information);
	L2STR(info);

	if (length<=0) length = LLEN(*info);
	if ((LLEN(*information) < LLEN(*info)) ||
		(LLEN(*info) < length))
			return FALSE;
	if (LLEN(*info)==0)
		return TRUE;
	i = 0; more = TRUE;
	while ( (i<length) && more )
		if (LSTR(*information)[i] == LSTR(*info)[i])
			i++;
		else
			more = FALSE;
	return more;
} /* Labbrev */

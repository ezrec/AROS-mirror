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

/* ------------------ Loverlay ---------------- */
void
Loverlay( const PLstr to, const PLstr newstr, const PLstr target,
	long n, long length, const char pad)
{
	Lstr	tmp;

	L2STR(newstr);
	L2STR(target);

	n--;
	if (n<0) n = 0;
	if (length==0) {
		Lstrcpy(to,target);
		return;
	}
	if (length<0) length = LLEN(*newstr);

	if (n+length > LLEN(*target))
		Lsubstr(to,target,1,n+length,pad);
	else
		Lstrcpy(to,target);

	LINITSTR(tmp);
	Lsubstr(&tmp,newstr,1,length,pad);

	MEMCPY( LSTR(*to)+n, LSTR(tmp), (size_t)length);
	LFREESTR(tmp);
} /* Loverlay */

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

/* ---------------- Lchars ------------------- */
long
Lchars( FILEP f )
{
#if defined(WCE) && !defined(__BORLANDC__)
	return GetFileSize(f->handle,NULL) - FTELL(f);
#else
	long	l,chs;

	l = FTELL(f);		/* read current position */
	FSEEK(f,0L,SEEK_END);	/* seek at the end */
	chs = FTELL(f) - l ;
	FSEEK(f,l,SEEK_SET);
	return chs;
#endif
} /* Lchars */

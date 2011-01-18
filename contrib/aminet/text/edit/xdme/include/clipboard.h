/******************************************************************************

    MODUL
	clipboard.h

    DESCRIPTION
	Include-File used by clipboard.c

******************************************************************************/

#ifndef CLIPBOARD_H
#define CLIPBOARD_H

/* Includes */

/* Globale Variable */

/* Defines und Strukturen */
#define Prototype extern

struct cbbuf {
    ULONG size;     /* size of memory-allocation */
    ULONG count;    /* no. of characters in mem[] */
    char  mem[1];   /* String */
};

#ifndef MAKE_ID
#   define MAKE_ID(a,b,c,d)    ((a << 24) | (b << 16) | (c << 8) | d)
#endif

#define ID_FORM     MAKE_ID('F','O','R','M')
#define ID_FTXT     MAKE_ID('F','T','X','T')
#define ID_CHRS     MAKE_ID('C','H','R','S')

/* Prototypes */
Prototype struct IOClipReq * CBOpen	 (ULONG);
Prototype void		     CBClose	 (struct IOClipReq *);
Prototype int		     CBWriteFTXT (struct IOClipReq *, char *);
Prototype int		     CBQueryFTXT (struct IOClipReq *);
Prototype struct cbbuf	   * CBReadCHRS  (struct IOClipReq *);
Prototype void		     CBReadDone  (struct IOClipReq *);
Prototype void		     CBFreeBuf	 (struct cbbuf *);

#endif /* CLIPBOARD_H */

/******************************************************************************
*****  ENDE clipboard.h
******************************************************************************/

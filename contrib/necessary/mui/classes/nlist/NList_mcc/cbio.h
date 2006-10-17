/***************************************************************************

 NList.mcc - New List MUI Custom Class
 Registered MUI class, Serial Number:

 Copyright (C) 1996-2004 by Gilles Masson,
                            Carsten Scholling <aphaso@aphaso.de>,
                            Przemyslaw Grunchala,
                            Sebastian Bauer <sebauer@t-online.de>,
                            Jens Langner <Jens.Langner@light-speed.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 NList classes Support Site:  http://www.sf.net/projects/nlist-classes

 $Id$

***************************************************************************/

struct cbbuf
{
  ULONG size;     /* size of memory allocation            */
  ULONG count;    /* number of characters after stripping */
  UBYTE *mem;     /* pointer to memory containing data    */
};

#define MAKE_ID_IFF(a,b,c,d) ((a<<24L) | (b<<16L) | (c<<8L) | d)

#ifndef ID_FORM
#define ID_FORM MAKE_ID_IFF('F','O','R','M')
#endif
#define ID_FTXT MAKE_ID_IFF('F','T','X','T')
#define ID_CHRS MAKE_ID_IFF('C','H','R','S')

#ifdef CBIO

/* prototypes */

struct IOClipReq        *CBOpen         ( ULONG );
void                    CBClose         (struct IOClipReq *);
int                     CBWriteFTXT     (struct IOClipReq *, char *);
int                     CBQueryFTXT     (struct IOClipReq *);
struct cbbuf            *CBReadCHRS     (struct IOClipReq *);
void                    CBReadDone      (struct IOClipReq *);
void                    CBFreeBuf       (struct cbbuf *);


/* routines which are meant to be used internally by routines in cbio */

int                     WriteLong       (struct IOClipReq *, long *);
int                     ReadLong        (struct IOClipReq *, ULONG *);
struct cbbuf            *FillCBData     (struct IOClipReq *, ULONG);

#else

/* prototypes */

extern struct IOClipReq *CBOpen         ( ULONG );
extern void             CBClose         (struct IOClipReq *);
extern int              CBWriteFTXT     (struct IOClipReq *, char *);
extern int              CBQueryFTXT     (struct IOClipReq *);
extern struct cbbuf     *CBReadCHRS     (struct IOClipReq *);
extern void             CBReadDone      (struct IOClipReq *);
extern void             CBFreeBuf       (struct cbbuf *);

#endif



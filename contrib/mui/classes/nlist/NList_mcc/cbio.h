/*
 * Copyright (c) 1992 Commodore-Amiga, Inc.
 *
 * This example is provided in electronic form by Commodore-Amiga, Inc. for
 * use with the "Amiga ROM Kernel Reference Manual: Devices", 3rd Edition,
 * published by Addison-Wesley (ISBN 0-201-56775-X).
 *
 * The "Amiga ROM Kernel Reference Manual: Devices" contains additional
 * information on the correct usage of the techniques and operating system
 * functions presented in these examples.  The source and executable code
 * of these examples may only be distributed in free electronic form, via
 * bulletin board or as part of a fully non-commercial and freely
 * redistributable diskette.  Both the source and executable code (including
 * comments) must be included, without modification, in any copy.  This
 * example may not be published in printed form or distributed with any
 * commercial product.  However, the programming techniques and support
 * routines set forth in these examples may be used in the development
 * of original executable software products for Commodore Amiga computers.
 *
 * All other rights reserved.
 *
 * This example is provided "as-is" and is subject to change; no
 * warranties are made.  All use is at your own risk. No liability or
 * responsibility is assumed.
 *
 ***********************************************************************
 *
 * cbio.h -- Include file used by clipdemo.c, changehook_test.c and cbio.c
 *
 ***********************************************************************
 */

struct cbbuf {
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



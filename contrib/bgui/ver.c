/*
 * @(#) $Header$
 *
 * BGUI library
 * ver.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 41.10  1998/02/25 21:13:29  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:10:02  mlemos
 * Ian sources
 *
 *
 */

#include "o/bgui.library_rev.h"
/*
 * Library ID and name strings.
 */
const unsigned char LibName[] = "bgui.library";
const unsigned char LibID[] = VSTRING;
const unsigned char LibVer[] = VERSTAG " ©1996-1997 Ian J. Einman, ©1993-1996 Jaba Dev.  "
#ifdef ENHANCED
"Enhanced (68020 & 3.0 required).";
#else
"Standard (68000 & 2.0 compatible).";
#endif



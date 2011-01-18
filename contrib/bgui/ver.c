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
 * Revision 42.2  2003/01/18 19:10:03  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.1  2000/05/30 03:24:35  bergers
 * Changes for build process.
 *
 * Revision 42.0  2000/05/09 22:10:37  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:55:25  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.3  2000/02/27 18:08:48  mlemos
 * Updated the copyright years of BGUI development team.
 *
 * Revision 41.10.2.2  1998/11/29 22:43:39  mlemos
 * Made the library ID string denote the respective type of build.
 *
 * Revision 41.10.2.1  1998/11/23 15:00:35  mlemos
 * Inserted new copyright notice to BGUI developers team.
 *
 * Revision 41.10  1998/02/25 21:13:29  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:10:02  mlemos
 * Ian sources
 *
 *
 */

#ifdef __AROS__
#include "bgui.library_rev.h"
#else
#include "o/bgui.library_rev.h"
#endif

/*
 * Library ID and name strings.
 */
const unsigned char LibName[] = "bgui.library";
const unsigned char LibID[] = VSTRING;
const unsigned char LibVer[] = VERSTAG " ©1998-2000 BGUI developers team, ©1996-1997 Ian J. Einman, ©1993-1996 Jaba Dev.  "
#ifdef DEBUG_BGUI 
"For debugging purposes only. "
#endif
#ifdef ENHANCED
"Enhanced (68020 & 3.0 or better required).";
#else
"Standard (68000 & 2.0 compatible).";
#endif



/*
 * @(#) $Header$
 *
 * BGUI library
 * bgui_locale.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.0  2000/05/09 22:08:27  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:53:56  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10  1998/02/25 21:11:38  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:07:34  mlemos
 * Ian sources
 *
 *
 */

#define CATCOMP_ARRAY
#include "include/classdefs.h"

makeproto UWORD  NumCatCompStrings = (sizeof(CatCompArray) / sizeof(struct CatCompArrayType));
makeproto struct CatCompArrayType CatCompArray[];



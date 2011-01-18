/*
 * @(#) $Header$
 *
 * BGUI Prefs Editor
 * BGUIPrefs.h
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997, Respect Software
 * (C) Copyright 1996-1997 Ian J. Einman.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.1  2001/06/09 10:53:11  hkiel
 * Added a newline at end of file to make compiler happy
 *
 * Revision 42.0  2000/05/09 22:16:58  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:29:48  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 19:56:48  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/08/29 17:09:55  mlemos
 * Added the definition of the pages by a symbolic name.
 *
 * Revision 1.1.2.1  1998/09/20 17:28:43  mlemos
 * Peter Bornhall sources.
 *
 *
 *
 */


#include	<exec/types.h>
#include	<exec/memory.h>
#include	<intuition/intuition.h>
#include	<libraries/asl.h>
#include	<libraries/bgui.h>
#include	<libraries/bgui_macros.h>

#include	<clib/alib_protos.h>
#include	<clib/macros.h>

#include	<proto/exec.h>
#include	<proto/dos.h>
#include	<proto/intuition.h>
#include	<proto/graphics.h>
#include	<proto/utility.h>
#include	<proto/bgui.h>

#include	<stdio.h>
#include	<string.h>
#include	<proto/iffparse.h>
#include	<libraries/iffparse.h>


#include	<bgui/bgui_prefs.h>


enum{
	MNU_OPEN	=	3200,
	MNU_SAVEAS,
	MNU_ABOUT,
	MNU_QUIT,
	MNU_DEFAULTS,
	MNU_LASTSAVED,
	MNU_RESTORE,
	MNU_PRE_NORM,
	MNU_PRE_FUZZ,
	MNU_PRE_XEN
};

enum
{
	PAGE_GENERAL=0,
	PAGE_SPACING,
	PAGE_GROUP,
	PAGE_BUTTON,
	PAGE_STRING,
	PAGE_CHECKBOX,
	PAGE_CYCLE,
	PAGE_INFO,
	PAGE_PROGRESS,
	PAGE_INDICATOR,
	PAGE_SLIDER,
	PAGE_PROPORTIONAL,
	PAGE_MX,
	PAGE_LISTVIEW,
	PAGE_COUNT
};

#define INITIAL_PAGE PAGE_GENERAL

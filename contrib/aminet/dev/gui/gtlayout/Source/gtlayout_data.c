/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
**
**	:ts=8
*/

#ifndef _GTLAYOUT_GLOBAL_H
#include "gtlayout_global.h"
#endif

#ifndef LINK_LIB
struct Library * SysBase;
#endif /* !LINK_LIB */

struct Library *	IntuitionBase;
struct Library *	GfxBase;
struct Library *	UtilityBase;
struct Library *	GadToolsBase;
struct Library *	KeymapBase;
struct SignalSemaphore	LTP_LockSemaphore;
struct MinList		LTP_LockList;
struct IClass *		LTP_ImageClass;
struct IClass *		LTP_LevelClass;
struct IClass *		LTP_PopupClass;
struct IClass *		LTP_TabClass;
struct MinList		LTP_EmptyList;

STRPTR			LTP_DecimalPoint;
STRPTR			LTP_NumberFormat;

BOOLEAN			V39;
BOOLEAN			V40;

#ifdef DO_PICKSHORTCUTS
UBYTE *			LTP_Keys[2];
struct SignalSemaphore	LTP_KeySemaphore;
#endif /* DO_PICKSHORTCUTS */

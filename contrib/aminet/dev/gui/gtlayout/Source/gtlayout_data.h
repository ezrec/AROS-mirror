/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
*/

#ifndef _GTLAYOUT_DATA_H
#define _GTLAYOUT_DATA_H 1

extern BOOLEAN			V39,
				V40;

#ifndef __AROS__
extern struct Library *		SysBase;
extern struct Library *		IntuitionBase;
extern struct Library *		GfxBase;
extern struct Library *		UtilityBase;
#else
#include <proto/exec.h>
/* SysBase is inited and defined by libinit in AROS */
extern struct IntuitionBase *	IntuitionBase;
extern struct GfxBase *		GfxBase;
extern struct UtilityBase *	UtilityBase;
#endif

extern struct Library *		GadToolsBase;
extern struct Library *		KeymapBase;

extern struct SignalSemaphore	LTP_LockSemaphore;
extern struct MinList		LTP_LockList;
extern struct IClass *		LTP_ImageClass;
extern struct IClass *		LTP_LevelClass;
extern struct IClass *		LTP_PopupClass;
extern struct IClass *		LTP_TabClass;
extern struct MinList		LTP_EmptyList;

extern STRPTR			LTP_DecimalPoint;
extern STRPTR			LTP_NumberFormat;

#ifdef DO_PICKSHORTCUTS
extern UBYTE *			LTP_Keys[2];
extern struct SignalSemaphore	LTP_KeySemaphore;
#endif	/* DO_PICKSHORTCUTS */

#endif	/* _GTLAYOUT_DATA_H */

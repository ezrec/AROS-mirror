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

extern struct Library *		SysBase;
extern struct Library *		IntuitionBase;
extern struct Library *		GfxBase;
extern struct Library *		UtilityBase;
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

/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
**
**	:ts=4
*/

#ifndef _GTLAYOUT_GLOBAL_H
#include "gtlayout_global.h"
#endif

/*****************************************************************************/

#define NO !

/*****************************************************************************/

#include <exec/memory.h>
#include <libraries/locale.h>

#include <clib/alib_protos.h>	/* For NewList */

#include <clib/locale_protos.h>
#include <pragmas/locale_pragmas.h>

#ifdef __AROS__
#include <aros/symbolsets.h>
#include LC_LIBDEFS_FILE
#endif

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

STATIC struct LocaleBase *	LocaleBase;
STATIC struct Locale *		DefaultLocale;

/*****************************************************************************/


/****** gtlayout.library/LT_Init ******************************************
*
*   NAME
*	LT_Init -- Initialize user interface code.
*
*   SYNOPSIS
*	LT_Init();
*
*	VOID LT_Init(VOID);
*
*   FUNCTION
*	You need to initialize the user interface code only once,
*	so it can set up its internals, open libraries, etc.
*	The code has to be initialized before any user interface
*	creation can take place.
*
*   NOTES
*	This function is not present in the shared library, only
*	in the link library. You need not and cannot invoke it in
*	the shared library.
*
*   SEE ALSO
*	gtlayout.library/LT_Exit
*
******************************************************************************
*
*/

/*****************************************************************************/

struct LibraryInitEntry
{
	BOOL				MustOpen;
	STRPTR				Name;
	struct Library **	Library;
};

struct ClassInitEntry
{
	STRPTR				ClassName;
	LONG				InstanceSize;
	HOOKFUNC			Dispatcher;
	Class **			ClassPtr;
};

/*****************************************************************************/

BOOL LIBENT
LT_Init(VOID)
{
	BOOL success = TRUE;

#ifndef LINK_LIB
# ifndef __AROS__
	{
		extern struct Library __far * AbsExecBase;
		SysBase = AbsExecBase;
	}
# endif
#endif	// LINK_LIB

	if(SysBase == NULL
#ifndef __AROS__
	 || SysBase->lib_Version < 37
#endif
	 )
	{
		success = FALSE;
	}
	else
	{
		STATIC struct LibraryInitEntry InitTable[] =
		{
			TRUE,	"intuition.library",	&IntuitionBase,
			TRUE,	"graphics.library",		&GfxBase,
			TRUE,	"utility.library",		&UtilityBase,
			TRUE,	"gadtools.library",		&GadToolsBase,
			TRUE,	"keymap.library",		&KeymapBase,
			FALSE,	"locale.library",		(struct Library **)&LocaleBase
		};

		STATIC struct ClassInitEntry ClassInitTable[] =
		{
			IMAGECLASS,	
			sizeof(ImageInfo),
			(HOOKFUNC)LTP_ImageDispatch,
			&LTP_ImageClass,

			#ifdef DO_LEVEL_KIND
			{
				GADGETCLASS,
				sizeof(struct SliderClassData),
				(HOOKFUNC)LTP_LevelClassDispatcher,
				&LTP_LevelClass,
			},
			#endif	/* DO_LEVEL_KIND */

			#ifdef DO_POPUP_KIND
			{
				GADGETCLASS,
				sizeof(PopInfo),
				(HOOKFUNC)LTP_PopupClassDispatcher,
				&LTP_PopupClass,
			},
			#endif	/* DO_POPUP_KIND */

			#ifdef DO_TAB_KIND
			{
				GADGETCLASS,
				sizeof(TabInfo),
				(HOOKFUNC)LTP_TabClassDispatcher,
				&LTP_TabClass,
			},
			#endif	/* DO_TAB_KIND */
		};

		LONG i;
#ifndef __AROS__
		V39 = (BOOLEAN)(SysBase->lib_Version >= 39);
		V40 = (BOOLEAN)(SysBase->lib_Version >= 40);
#else
		V39 = TRUE;
		V40 = TRUE;
#endif

		InitSemaphore(&LTP_LockSemaphore);
		NewList((struct List *)&LTP_LockList);

		NewList((struct List *)&LTP_EmptyList);

		#ifdef DO_PICKSHORTCUTS
		{
			InitSemaphore(&LTP_KeySemaphore);
	
			if(!(LTP_Keys[0] = (UBYTE *)AllocMem(512,MEMF_ANY|MEMF_CLEAR)))
			{
				success = FALSE;
			}
		}
		#endif	/* DO_PICKSHORTCUTS */

		for(i = 0 ; i < NUMELEMENTS(InitTable) ; i++)
		{
			(*InitTable[i].Library) = OpenLibrary(InitTable[i].Name,37);
			if((*InitTable[i].Library) == NULL && InitTable[i].MustOpen)
			{
				success = FALSE;
			}
		}

		if(success)
		{
			LTP_NumberFormat = "%ld";
			LTP_DecimalPoint = ".";

			if(LocaleBase != NULL)
			{
				/* Guaranteed to open. */
				DefaultLocale = OpenLocale(NULL);

				LTP_DecimalPoint = DefaultLocale->loc_DecimalPoint;

				if(LocaleBase->lb_SysPatches)
					LTP_NumberFormat = "%lD";
			}

			for(i = 0 ; i < NUMELEMENTS(ClassInitTable) ; i++)
			{
				(*ClassInitTable[i].ClassPtr) = MakeClass(NULL,ClassInitTable[i].ClassName,NULL,ClassInitTable[i].InstanceSize,0);
				if((*ClassInitTable[i].ClassPtr) != NULL)
				{
					(*ClassInitTable[i].ClassPtr)->cl_Dispatcher.h_Entry = ClassInitTable[i].Dispatcher;
				}
				else
				{
					success = FALSE;
					break;
				}
			}
		}
	}

	if(NO success)
	{
		LT_Exit();
	}

	return(success);
}


/*****************************************************************************/


/****** gtlayout.library/LT_Exit ******************************************
*
*   NAME
*	LT_Exit -- Clean up user interface allocations
*
*   SYNOPSIS
*	LT_Exit();
*
*	VOID LT_Exit(VOID);
*
*   FUNCTION
*	When you are finished with user interface creation and
*	do not not need the code any more you should call this
*	routine. It will free all the memory allocated by
*	LT_Init(), close libraries, etc.
*
*   INPUTS
*	none
*
*   RESULT
*	none
*
*   NOTES
*	This function is not present in the shared library, only
*	in the link library. You need not and cannot invoke it in
*	the shared library.
*
*   SEE ALSO
*	gtlayout.library/LT_Init
*
******************************************************************************
*
*/

VOID LIBENT
LT_Exit(VOID)
{
	if(SysBase != NULL 
#ifndef __AROS__
	   && SysBase->lib_Version >= 37
#endif
	   )
	{
		#ifdef DO_PICKSHORTCUTS
		{
			FreeMem(LTP_Keys[0],512);
			LTP_Keys[0] = LTP_Keys[1] = NULL;
		}
		#endif

		#ifdef DO_LEVEL_KIND
		{
			if(LTP_LevelClass)
			{
				FreeClass(LTP_LevelClass);
				LTP_LevelClass = NULL;
			}
		}
		#endif	/* DO_LEVEL_KIND */

		#ifdef DO_POPUP_KIND
		{
			if(LTP_PopupClass)
			{
				FreeClass(LTP_PopupClass);
				LTP_PopupClass = NULL;
			}
		}
		#endif	// DO_POPUP_KIND

		#ifdef DO_TAB_KIND
		{
			if(LTP_TabClass)
			{
				FreeClass(LTP_TabClass);
				LTP_TabClass = NULL;
			}
		}
		#endif	// DO_TAB_KIND

		if(LTP_ImageClass)
		{
			FreeClass(LTP_ImageClass);
			LTP_ImageClass = NULL;
		}

		if(DefaultLocale)
		{
			CloseLocale(DefaultLocale);
			DefaultLocale = NULL;
		}

		CloseLibrary(LocaleBase);
		LocaleBase = NULL;

		CloseLibrary(KeymapBase);
		KeymapBase = NULL;

		CloseLibrary(GadToolsBase);
		GadToolsBase = NULL;

		CloseLibrary(UtilityBase);
		UtilityBase = NULL;

		CloseLibrary(GfxBase);
		GfxBase = NULL;

		CloseLibrary(IntuitionBase);
		IntuitionBase = NULL;
	}
}

#ifdef __AROS__
ADD2INIT(LT_Init, 0);
ADD2EXIT(LT_Exit, 0);
#endif

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

#include "Assert.h"

/****** gtlayout.library/LT_BeginRefresh ******************************************
*
*   NAME
*	LT_BeginRefresh -- Optimized window refreshing
*
*   SYNOPSIS
*	LT_BeginRefresh(Handle)
*	                  A0
*
*   FUNCTION
*	If you wish to handle window refreshing all on your own, you
*	might want to use the LT_BeginRefresh...LT_EndRefresh pair
*	in your program. By default the user interface layout engine
*	will automatically intercept IDCMP_REFRESHWINDOW events and
*	react to them accordingly.
*
*   INPUTS
*	Handle - Pointer to a LayoutHandle structure.
*
*   RESULT
*	none
*
*   SEE ALSO
*	intuition.library/BeginRefresh
*	intuition.library/EndRefresh
*	gtlayout.library/LT_EndRefresh
*
******************************************************************************
*
*/

VOID LIBENT
LT_BeginRefresh(REG(a0) LayoutHandle *handle)
{
	if(handle)
	{
		GT_BeginRefresh(handle->Window);

		LTP_DrawGroup(handle,handle->TopGroup);
	}
}


/*****************************************************************************/


/****** gtlayout.library/LT_EndRefresh ******************************************
*
*   NAME
*	LT_EndRefresh -- Optimized window refreshing
*
*   SYNOPSIS
*	LT_EndRefresh(Handle)
*	                A0
*
*	VOID LT_EndRefresh(LayoutHandle *);
*
*   FUNCTION
*	If you wish to handle window refreshing all on your own, you
*	might want to use the LT_BeginRefresh...LT_EndRefresh pair
*	in your program. By default the user interface layout engine
*	will automatically intercept IDCMP_REFRESHWINDOW events and
*	react to them accordingly.
*
*   INPUTS
*	Handle - Pointer to a LayoutHandle structure.
*
*   RESULT
*	none
*
*   SEE ALSO
*	gtlayout.library/LT_BeginRefresh
*	intuition.library/BeginRefresh
*	intuition.library/EndRefresh
*
******************************************************************************
*
*/

VOID LIBENT
LT_EndRefresh(REG(a0) LayoutHandle *handle,REG(d0) BOOL complete)
{
	if(handle)
		GT_EndRefresh(handle->Window,complete);
}

/****** gtlayout.library/LT_Refresh *****************************************
*
*   NAME
*	LT_Refresh -- Redraws the entire window contents.
*
*   SYNOPSIS
*	LT_Refresh(Handle)
*	             A0
*
*	VOID LT_Refresh(LayoutHandle *);
*
*   FUNCTION
*	Redraws the contents of the window, this includes both gadgets
*	and imagery.
*
*   INPUTS
*	Handle - Pointer to a LayoutHandle structure.
*
*   RESULT
*	none
*
*   SEE ALSO
*       gadtools.library/GT_RefreshWindow
*       intuition.library/RefreshGList
*
******************************************************************************
*
*/

VOID LIBENT
LT_Refresh(REG(a0) LayoutHandle *handle)
{
	if(handle)
	{
		#ifdef DO_BOOPSI_KIND
		{
			if(handle->BOOPSIList)
				RefreshGList((struct Gadget *)handle->BOOPSIList,handle->Window,NULL,(UWORD)-1);
		}
		#endif	/* DO_BOOPSI_KIND */

		RefreshGList(handle->List,handle->Window,NULL,(UWORD)-1);

		GT_RefreshWindow(handle -> Window,NULL);

		LTP_DrawGroup(handle,handle -> TopGroup);
	}
}

/****** gtlayout.library/LT_CatchUpRefresh **************************************
*
*   NAME
*	LT_CatchUpRefresh -- Repair the display after missing the
*	                     Window refresh message (V34).
*
*   SYNOPSIS
*	LT_CatchUpRefresh(Handle)
*	                    A0
*
*	VOID LT_CatchUpRefresh(LayoutHandle *);
*
*   FUNCTION
*	In case an application missed an IDCMP_REFRESHWINDOW event
*	the display may be damaged and in need of repair. This can
*	happen if the ASL requester is attached to the window,
*	sharing its IDCMP with the window. Any refresh events will then
*	go straight to nil. Call this routine if you believe that
*	you missed a refresh event. The display will be repaired only
*	if it is in need of repair, so there is no harm in calling it
*	if not actually necessary.
*
*   INPUTS
*	Handle - Pointer to a LayoutHandle structure.
*
*   RESULT
*	none
*
*   SEE ALSO
*	gtlayout.library/LT_BeginRefresh
*	intuition.library/BeginRefresh
*	intuition.library/EndRefresh
*
******************************************************************************
*
*/

VOID LIBENT
LT_CatchUpRefresh(REG(a0) LayoutHandle *handle)
{
	if(handle)
	{
			/* Do we have damage? */

		if(handle->Window->WLayer->Flags & LAYERREFRESH)
		{
				/* Do the usual bit. */

			GT_BeginRefresh(handle->Window);

			LTP_DrawGroup(handle,handle->TopGroup);

			GT_EndRefresh(handle->Window,TRUE);
		}
	}
}

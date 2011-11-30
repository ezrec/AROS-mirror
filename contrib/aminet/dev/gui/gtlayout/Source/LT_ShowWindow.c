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

/****** gtlayout.library/LT_ShowWindow ******************************************
*
*   NAME
*	LT_ShowWindow -- Make a window visible
*
*   SYNOPSIS
*	LT_ShowWindow(Handle,Activate);
*	                A0      A1
*
*	VOID LT_ShowWindow(LayoutHandle *,BOOL);
*
*   FUNCTION
*	The window attached to a LayoutHandle is made visible, this
*	involves bringing it to the front, bringing the screen to
*	the front the window resides on, unzooming the window and
*	also moving the visible part of an autoscrolling screen.
*
*   INPUTS
*	Window - Pointer to Window structure.
*
*	Activate - If TRUE the window will be activated as soon
*	    as it has been brought to the front.
*
*   RESULT
*	none
*
*   NOTES
*	The arguments are passed in A0 and A1, this is *not* a
*	typo.
*
*   BUGS
*	In revisions earlier than v21 this routine consistently
*	failed to reliably unzip a window in zoomed state. This
*	could cause the calling application to wait for about
*	five seconds before continuing execution.
*
*   SEE ALSO
*	intuition.library/MoveScreen
*	intuition.library/ScreenPosition
*	intuition.library/ZipWindow
*
******************************************************************************
*
*/

VOID LIBENT
LT_ShowWindow(REG(a0) LayoutHandle *handle,REG(a1) BOOL activate)
{
	if(handle)
	{
		struct Window *window;
		ULONG flags,mask;

		window = handle->Window;
		flags = 0;
		mask = 0;

		WindowToFront(window);

		if(activate)
		{
				// activate the window

			ActivateWindow(window);

				// wait for the window to become active

			flags |= WFLG_WINDOWACTIVE;
			mask |= WFLG_WINDOWACTIVE;
		}

		ScreenToFront(window->WScreen);

		if(!handle->ResizeObject && (handle->Window->Flags & (WFLG_HASZOOM | WFLG_ZOOMED)) == (WFLG_HASZOOM | WFLG_ZOOMED))
		{
				// make the window full-sized

			ZipWindow(window);

				// wait for the zoom bit to get cleared

			flags &= ~WFLG_ZOOMED;
			mask |= WFLG_ZOOMED;
		}

			// wait for the window to change state?

		if(mask)
		{
			LONG i;

				// wait for the window to change state

			for(i = 0 ; i < 300 ; i++)
			{
				if((handle->Window->Flags & mask) == flags)
					break;
				else
					WaitTOF();
			}
		}

			// make the window visible on the screen by
			// scrolling it into view

		if(handle->MoveToWindow)
			LTP_MoveToWindow(handle);
	}
}

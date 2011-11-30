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

#include <exec/memory.h>

/*****************************************************************************/

#include "Assert.h"


/*****************************************************************************/


VOID
LTP_DeleteWindowLock(LockNode *Node)
{
	struct Window *Window = Node->Window;

	Remove((struct Node *)Node);

	if(Window->MinWidth == Window->MaxWidth && Window->MinHeight == Window->MaxHeight && Window->Width == Window->MaxWidth && Window->Height == Window->MaxHeight)
		WindowLimits(Window,Node->MinWidth,Node->MinHeight,Node->MaxWidth,Node->MaxHeight);

	EndRequest(&Node->Requester,Window);

	if(V39)
		SetWindowPointerA(Window,NULL);
	else
		ClearPointer(Window);

	ModifyIDCMP(Window,Node->OldIDCMPFlags);

	FreeMem(Node,sizeof(LockNode));
}


/*****************************************************************************/


/****** gtlayout.library/LT_LockWindow ******************************************
*
*   NAME
*	LT_LockWindow -- Block user access to a window.
*
*   SYNOPSIS
*	LT_LockWindow(Window);
*	                A0
*
*	VOID LT_LockWindow(struct Window *);
*
*   FUNCTION
*	The window will get a wait mouse pointer attached and a blank
*	Requester, preventing any user gadgets from getting used. The
*	window minimum and maximum sizes are set to the current window
*	size so the user will be unable to resize the window or click
*	on the zoom gadget.
*
*	This routine nests, multiple calls to LT_LockWindow() using the
*	same window will increment a usage counter, so exactly the
*	same number of calls to LT_UnlockWindow() will be required to
*	unlock the window.
*
*   INPUTS
*	Window - Pointer to window structure; passign NULL is harmless.
*
*   RESULT
*	none
*
*   NOTES
*	Do not close the window you have locked unless all the
*	outstanding locks are freed or memory will be lost which
*	can never be reclaimed. Before you close the window,
*	call gtlayout.library/LT_DeleteWindowLock.
*
*   SEE ALSO
*	gtlayout.library/LT_DeleteWindowLock
*
******************************************************************************
*
*/

VOID LIBENT
LT_LockWindow(REG(a0) struct Window *window)
{
	if(window)
	{
		LockNode	*lockNode;
		BOOL	 	 found;

		found = FALSE;

		ObtainSemaphore(&LTP_LockSemaphore);

		SCANLIST(&LTP_LockList,lockNode)
		{
			if(lockNode->Window == window)
			{
				lockNode->Count++;

				found = TRUE;

				break;
			}
		}

		if(!found)
		{
			if(lockNode = AllocMem(sizeof(LockNode),MEMF_PUBLIC | MEMF_ANY | MEMF_CLEAR))
			{
				AddTail((struct List *)&LTP_LockList,(struct Node *)lockNode);

				lockNode->Window		= window;
				lockNode->OldIDCMPFlags	= window->IDCMPFlags;
				lockNode->Count			= 1;
				lockNode->MinWidth		= window->MinWidth;
				lockNode->MaxWidth		= window->MaxWidth;
				lockNode->MinHeight		= window->MinHeight;
				lockNode->MaxHeight		= window->MaxHeight;

				WindowLimits(window,window->Width,window->Height,window->Width,window->Height);

				Request(&lockNode->Requester,window);

				/* Now, this needs a bit of explanation. The requester will shut down
				 * menu processing for this window and still we enable IDCMP_MENUPICK.
				 * This is done in order to avoid setting the IDCMP flags to 0. One
				 * side-effect of setting the IDCMP flags to 0 is that the window message
				 * queue will be cleared, which can be undesirable in certain cases.
				 * With the menu strip blocked IDCMP_MENUPICK cannot do any harm.
				 */

				ModifyIDCMP(window,IDCMP_MENUPICK | (window->IDCMPFlags & (IDCMP_REFRESHWINDOW|IDCMP_CHANGEWINDOW)));

				if(V39)
				{
					STATIC const struct TagItem tags[3] =
					{
					    {
						WA_BusyPointer,		TRUE,
					    }, {
						WA_PointerDelay,	TRUE,
					    }, {
						TAG_DONE
					    }
					};

					SetWindowPointerA(window,(struct TagItem *)tags);
				}
				else
				{
					STATIC const UWORD __chip Stopwatch[(1 + 16 + 1) * 2] =
					{
						0x0000,0x0000,

						0x0400,0x07C0,
						0x0000,0x07C0,
						0x0100,0x0380,
						0x0000,0x07E0,
						0x07C0,0x1FF8,
						0x1FF0,0x3FEC,
						0x3FF8,0x7FDE,
						0x3FF8,0x7FBE,
						0x7FFC,0xFF7F,
						0x7EFC,0xFFFF,
						0x7FFC,0xFFFF,
						0x3FF8,0x7FFE,
						0x3FF8,0x7FFE,
						0x1FF0,0x3FFC,
						0x07C0,0x1FF8,
						0x0000,0x07E0,

						0x0000,0x0000
					};

					SetPointer(window,(UWORD *)Stopwatch,16,16,-6,0);
				}
			}
		}

		ReleaseSemaphore(&LTP_LockSemaphore);
	}
}


/****************************************************************************/


/****** gtlayout.library/LT_UnlockWindow ******************************************
*
*   NAME
*	LT_UnlockWindow -- The complement to LT_LockWindow().
*
*   SYNOPSIS
*	LT_UnlockWindow(Window);
*	                  A0
*
*	VOID LT_UnlockWindow(struct Window *);
*
*   FUNCTION
*	This routine unlocks a window locked using LT_LockWindow, freeing
*	allocated memory, restoring the window characteristics to their
*	original values.
*
*   INPUTS
*	Window - Pointer to window structure; passing NULL is harmless.
*
*   RESULT
*	none
*
*   SEE ALSO
*	gtlayout.library/LT_LockWindow
*
******************************************************************************
*
*/

VOID LIBENT
LT_UnlockWindow(REG(a0) struct Window *window)
{
	if(window)
	{
		LockNode *lockNode;

		ObtainSemaphore(&LTP_LockSemaphore);

		SCANLIST(&LTP_LockList,lockNode)
		{
			if(lockNode->Window == window)
			{
				if(--lockNode->Count < 1)
					LTP_DeleteWindowLock(lockNode);

				break;
			}
		}

		ReleaseSemaphore(&LTP_LockSemaphore);
	}
}


/*****************************************************************************/


/****** gtlayout.library/LT_DeleteWindowLock ******************************************
*
*   NAME
*	LT_DeleteWindowLock -- Remove all locks from a window
*
*   SYNOPSIS
*	LT_DeleteWindowLock(Window);
*	                      A0
*
*	VOID LT_DeleteWindowLock(struct Window *);
*
*   FUNCTION
*	Before closing a locked window you should call this routine
*	which will remove all outstanding locks from it.
*
*   INPUTS
*	Window - Pointer to window structure; passing NULL is
*	harmless.
*
*   RESULT
*	none
*
******************************************************************************
*
*/

VOID LIBENT
LT_DeleteWindowLock(REG(a0) struct Window *window)
{
	if(window)
	{
		LockNode *lockNode;

		ObtainSemaphore(&LTP_LockSemaphore);

		SCANLIST(&LTP_LockList,lockNode)
		{
			if(lockNode->Window == window)
			{
				LTP_DeleteWindowLock(lockNode);
				break;
			}
		}

		ReleaseSemaphore(&LTP_LockSemaphore);
	}
}

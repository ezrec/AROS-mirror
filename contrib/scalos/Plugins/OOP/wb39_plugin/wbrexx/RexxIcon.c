// RexxIcon.c
// $Date$
// $Revision$


#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <dos/rdargs.h>

#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/memory.h>

#include <graphics/text.h>
#include <workbench/workbench.h>
#include <intuition/intuitionbase.h>
#include <intuition/gadgetclass.h>

#include <rexx/rxslib.h>
#include <rexx/errors.h>

#include <clib/alib_protos.h>

#include <proto/rexxsyslib.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/scalos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/wb.h>

#include <scalos/scalos.h>

#include <string.h>
#include <stdio.h>


#include "wbrexx.h"
#include "Scalos_Helper.h"

// aus wb39.c
extern struct ScaRootList *rList;

extern T_UTILITYBASE UtilityBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct Library *LayersBase;

struct IconListNode
	{
	struct MinNode iln_Node;
	struct ScaIconNode *iln_IconNode;
	};

static struct ScaIconNode *FindActiveIcon(struct ScaWindowStruct *swi);
static void OpenIcon(struct ScaWindowStruct *swi, struct ScaIconNode *sIcon);


#if 0
/*
------------------------------------------------------------------------------
ICON command

Purpose:

This command is for manipulating the icons displayed in a window.

Format:

ICON [WINDOW] <Window name> <Icon name> .. <Icon name> [OPEN] [MAKEVISIBLE]
[SELECT] [UNSELECT] [UP <Pixels>] [DOWN <Pixels>] [LEFT <Pixels>] [RIGHT
<Pixels>] [X <Horizontal position>] [Y <Vertical position>] [ACTIVATE
UP|DOWN|LEFT|RIGHT] [CYCLE PREVIOUS|NEXT] [MOVE IN|OUT]

Template:

ICON WINDOW,NAMES/M,OPEN/S,MAKEVISIBLE/S,SELECT/S,UNSELECT/S,
UP/N,DOWN/N,LEFT/N,RIGHT/N,X/N,Y/N,ACTIVATE/K,CYCLE/K, MOVE/K

Parameters:

WINDOW

Name of the window whose icons should be manipulated. This can be ROOT" to
work on the Workbench root window (where volume icons and AppIcons live),
ACTIVE" to work on the currently active Workbench window or the fully
qualified name of a drawer window. Note that the drawer window must already
be open.

If no WINDOW parameter is specified, this command will try to operate on the
currently active Workbench window.

NAMES

Names of the icons to manipulate.

OPEN

Specifies that the named icons should be opened.

MAKEVISIBLE

Specifies that the named icons should be made visible. This generally works
well for the first icon in a list but does not always work for a whole list.

SELECT

Select the named icons.

UNSELECT

Unselect the named icons.

UP, DOWN, LEFT, RIGHT

Move the named icons by the specified number of pixels.

X, Y

Move the named icons to the specified position.

ACTIVATE

This command is for activating the icon closest to the currently selected
icon in the window. Activating" in this context means selecting an icon,
whilst at the same time unselecting all others". Thus, the active" icon is
the only selected icon in the window.

You can indicate which direction the next icon to be activated should be
searched for, relative to the currently active icon. UP" searches upwards,
DOWN" searches downwards, LEFT" searches to the left and RIGHT" searches to
the right.

CYCLE

This command is for cycling through all icons in a window, making each one
the active one in turn (for a description of what active" means in this
context, see the ACTIVATE" description above). You must indicate in which
direction you want to cycle through the icons: you can either specify
PREVIOUS" or NEXT".

MOVE

This command is not for moving icons but for moving through a file system
hierarchy. Thus, moving in" will open a drawer and moving out" will open the
drawer`s parent directory. The IN" parameter will cause the drawer
represented by the active icon to be opened. Please note that an icon must be
selected and it must be a drawer. The OUT" parameter will open the drawer`s
parent directory, and it also requires that in the drawer there is an icon
selected. This may sound strange, but this feature is not meant as a
replacement for the Open Parent" menu item.

Errors:

10 - If the named window cannot be found, none of the Workbench windows are
currently active and the command was set to work on the currently active
Workbench window. The error code will be placed in the WORKBENCH.LASTERROR
variable.

Result:

-

Example:

* Select the icons of the Workbench" and Work" volumes * displayed in the
* root window. 
ADDRESS workbench

ICON WINDOW root
NAMES Workbench Work SELECT

* Open the Workbench" volume icon displayed in the root * window. 
ICON WINDOW root
NAMES Workbench OPEN

------------------------------------------------------------------------------
*/
#endif
LONG Cmd_Icon(APTR UserData, struct RexxMsg *Message, STRPTR *Args)
{
	enum	{ ARG_WINDOW, ARG_NAMES, ARG_OPEN, ARG_MAKEVISIBLE, ARG_SELECT, 
		ARG_UNSELECT, ARG_UP, ARG_DOWN, ARG_LEFT, ARG_RIGHT, ARG_X,
		ARG_Y, ARG_ACTIVATE, ARG_CYCLE, ARG_MOVE };
	LONG Result = RETURN_OK;
	char *ResultString = NULL;
	struct ScaWindowStruct *swi;

	d1(kprintf(__FUNC__ "/%ld: Window=<%s>\n", __LINE__, Args[ARG_WINDOW] ? Args[ARG_WINDOW] : (STRPTR) "");)

	if (Args[ARG_WINDOW])
		swi = FindWindowByName(Args[ARG_WINDOW]);
	else
		swi = FindWindowByName("active");

	if (NULL == swi)
		return(ERROR_OBJECT_NOT_FOUND);

	if (Args[ARG_NAMES])
		{
		STRPTR *NamesPtr = (STRPTR *) Args[ARG_NAMES];
		while (*NamesPtr && RETURN_OK == Result)
			{
			struct ScaIconNode *sIcon;

			d(kprintf(__FUNC__ "/%ld: Name=<%s>\n", __LINE__, *NamesPtr);)

			sIcon = GetIconByName(swi, *NamesPtr);
			d(kprintf(__FUNC__ "/%ld: sIcon=%08lx\n", __LINE__, sIcon);)

			if (NULL == sIcon)
				return ERROR_OBJECT_NOT_FOUND;

			if (Args[ARG_OPEN])
				{
				d(kprintf(__FUNC__ "/%ld: OPEN\n", __LINE__);)

				if (NULL == sIcon)
					Result = ERROR_OBJECT_NOT_FOUND;
				else
					{
					OpenIcon(swi, sIcon);
					}
				}
			if (Args[ARG_MAKEVISIBLE])
				{
				d(kprintf(__FUNC__ "/%ld: MAKEVISIBLE\n", __LINE__);)

				if (NULL == sIcon)
					Result = ERROR_OBJECT_NOT_FOUND;
				else
					{
					MakeIconVisible(swi, sIcon);
					}
				}
			if (Args[ARG_SELECT])
				{
				d(kprintf(__FUNC__ "/%ld: SELECT\n", __LINE__);)

				if (NULL == sIcon)
					Result = ERROR_OBJECT_NOT_FOUND;
				else
					{
					SelectIcon(swi, sIcon, TRUE);
					}
				}
			if (Args[ARG_UNSELECT])
				{
				d(kprintf(__FUNC__ "/%ld: UNSELECT\n", __LINE__);)

				if (NULL == sIcon)
					Result = ERROR_OBJECT_NOT_FOUND;
				else
					{
					SelectIcon(swi, sIcon, FALSE);
					}
				}
			if (Args[ARG_X] || Args[ARG_Y] || Args[ARG_LEFT] || Args[ARG_RIGHT] || Args[ARG_UP] || Args[ARG_DOWN])
				{
				// Move Icon
				if (NULL == sIcon)
					Result = ERROR_OBJECT_NOT_FOUND;
				else
					{
					struct Gadget *gg = (struct Gadget *) sIcon->in_Icon;
					LONG x, y;

					x = gg->LeftEdge;
					y = gg->TopEdge;

					if (Args[ARG_X])
						x = *((LONG *) (Args[ARG_X]));
					if (Args[ARG_Y])
						y = *((LONG *) (Args[ARG_Y]));
					if (Args[ARG_RIGHT])
						x += *((LONG *) (Args[ARG_RIGHT]));
					if (Args[ARG_LEFT])
						x -= *((LONG *) (Args[ARG_LEFT]));
					if (Args[ARG_UP])
						y -= *((LONG *) (Args[ARG_UP]));
					if (Args[ARG_DOWN])
						y += *((LONG *) (Args[ARG_DOWN]));

					MoveIcon(swi, sIcon, x, y);
					}
				}

			ReleaseSemaphore(swi->ws_WindowTask->wt_IconSemaphore);

			NamesPtr++;
			}
		}
	else
		{
 		struct ScaIconNode *sIcon = NULL;

		if (Args[ARG_ACTIVATE])
			{
			d1(kprintf(__FUNC__ "/%ld: ACTIVATE\n", __LINE__);)

			sIcon = FindActiveIcon(swi);

			if (sIcon)
				{
				if (0 == Stricmp(Args[ARG_ACTIVATE], "UP"))
					{
					DoMethod(swi->ws_WindowTask->mt_MainObject, SCCM_IconWin_ActivateIconUp);
					}
				else if (0 == Stricmp(Args[ARG_ACTIVATE], "DOWN"))
					{
					DoMethod(swi->ws_WindowTask->mt_MainObject, SCCM_IconWin_ActivateIconDown);
					}
				else if (0 == Stricmp(Args[ARG_ACTIVATE], "LEFT"))
					{
					DoMethod(swi->ws_WindowTask->mt_MainObject, SCCM_IconWin_ActivateIconLeft);
					}
				else if (0 == Stricmp(Args[ARG_ACTIVATE], "RIGHT"))
					{
					DoMethod(swi->ws_WindowTask->mt_MainObject, SCCM_IconWin_ActivateIconRight);
					}
				else
					Result = ERROR_REQUIRED_ARG_MISSING;
				}
			else
				Result = ERROR_OBJECT_NOT_FOUND;
			}
		if (Args[ARG_CYCLE])
			{
			d(kprintf(__FUNC__ "/%ld: CYCLE  Result=%ld\n", __LINE__, Result);)

			if (0 == Stricmp(Args[ARG_CYCLE], "PREVIOUS"))
				{
				DoMethod(swi->ws_WindowTask->mt_MainObject, SCCM_IconWin_ActivateIconPrevious);
				}
			else if (0 == Stricmp(Args[ARG_CYCLE], "NEXT"))
				{
				DoMethod(swi->ws_WindowTask->mt_MainObject, SCCM_IconWin_ActivateIconNext);
				}
			else
				Result = ERROR_REQUIRED_ARG_MISSING;
			}
		if (Args[ARG_MOVE])
			{
			d1(kprintf(__FUNC__ "/%ld: MOVE\n", __LINE__);)

			sIcon = FindActiveIcon(swi);

			if (0 == Stricmp(Args[ARG_MOVE], "IN"))
				{
				IPTR IconType;

				d1(kprintf(__FUNC__ "/%ld: MOVE IN\n", __LINE__);)

				if (sIcon)
					{
					GetAttr(IDTA_Type, sIcon->in_Icon, &IconType);

					if (WBDRAWER == IconType)
						OpenIcon(swi, sIcon);
					else
						Result = ERROR_OBJECT_WRONG_TYPE;
					}
				else
					Result = ERROR_OBJECT_WRONG_TYPE;
				}
			else if (0 == Stricmp(Args[ARG_MOVE], "OUT"))
				{
				d1(kprintf(__FUNC__ "/%ld: MOVE IN\n", __LINE__);)

				if (sIcon)
					Result = MenuOpenParentWindow(swi);
				else
					Result = ERROR_OBJECT_WRONG_TYPE;
				}
			}

		if (sIcon)
			ReleaseSemaphore(swi->ws_WindowTask->wt_IconSemaphore);
		}

	d(kprintf(__FUNC__ "/%ld: Result=%ld\n", __LINE__, Result);)

	if (RETURN_OK == Result)
		ReturnRexxMsg(Message, ResultString);

	return Result;
}


// ! wt->wt_IconSemaphore WILL STAY LOCKED IF ICON FOUND !
static struct ScaIconNode *FindActiveIcon(struct ScaWindowStruct *swi)
{
	struct ScaWindowTask *wt = swi->ws_WindowTask;
	struct ScaIconNode *icon, *iconFound=NULL;

	d(kprintf(__FUNC__ "/%ld: swi=%08lx\n", __LINE__, swi);)

	ObtainSemaphore(wt->wt_IconSemaphore);

	for (icon=wt->wt_IconList; (NULL == iconFound) && icon; icon = (struct ScaIconNode *) icon->in_Node.mln_Succ)
		{
		struct Gadget *gg = (struct Gadget *) icon->in_Icon;

		d(kprintf(__FUNC__ "/%ld: icon=%08lx  Selected=%08lx\n", __LINE__, \
			icon, gg->Flags & GFLG_SELECTED);)

		if (gg->Flags & GFLG_SELECTED)
			{
			iconFound = icon;
			}
		}

	if (NULL == iconFound)
		ReleaseSemaphore(wt->wt_IconSemaphore);

	d(kprintf(__FUNC__ "/%ld: iconFound=%08lx\n", __LINE__, iconFound);)

	return iconFound;
}


static void OpenIcon(struct ScaWindowStruct *swi, struct ScaIconNode *sIcon)
{
	BPTR dirLock = (BPTR)NULL;
	char *Name = NULL;

	if (swi->ws_Lock)
		dirLock = CurrentDir(swi->ws_Lock);

	GetAttr(IDTA_Text, sIcon->in_Icon, (APTR) &Name);

	if (Name)
		WaitOpen(SCA_OpenDrawerByName(Name, NULL));

	if (dirLock)
		CurrentDir(dirLock);
}



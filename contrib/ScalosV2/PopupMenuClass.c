// :ts=4 (Tabsize)

#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/alib.h>
#include <proto/pm.h>
#include <string.h>
#include <exec/memory.h>

#include <libraries/pm.h>
// #include <proto/pm.h>

#include "Scalos.h"
#include "PopupMenuClass.h"
#include "ScalosIntern.h"

#include "scalos_protos.h"
#include "Debug.h"
#include "SubRoutines.h"

/****** PopupMenu.scalos/--background ***************************************
*
*   PopupMenu class is used to generate popup menus and to display them
*   on screen.
*
*****************************************************************************
*/
// /

/****** PopupMenu.scalos/SCCA_PopupMenu_Item ********************************
*
*  NAME
*   SCCA_PopupMenu_Item -- (V40) IS. (Object *)
*
*  FUNCTION
*   This attribute addes an popup menu item to the menu.
*
*****************************************************************************
*/
// /
/****** PopupMenu.scalos/SCCA_PopupMenu_Window ******************************
*
*  NAME
*   SCCA_PopupMenu_Window -- (V40) IS. (Object *)
*
*  FUNCTION
*   Sets the window object where the popup menu should belong to. This
*   attribute must be given.
*
*****************************************************************************
*/
// /

/** Init a PopupMenu
*/
static ULONG PopupMenu_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct PopupMenuInst *inst)
{
	if (SC_DoSuperMethodA(cl,obj, (Msg) msg))
	{
		struct TagItem *tags = msg->ops_AttrList;
		struct TagItem **taglist = &tags;
		struct TagItem *tag;
		struct MenuItemNode *node;

		NewList((struct List *) &inst->itemlist);

		while ((tag = NextTagItem(taglist)))
		{
			switch (tag->ti_Tag)
			{

			  case SCCA_PopupMenu_Item:
				if ((node = (struct MenuItemNode *) AllocNode(&inst->itemlist,sizeof(struct MenuItemNode))))
				{
					node->itemobj = (Object *) tag->ti_Data;
				}
				break;

			  case SCCA_PopupMenu_Window:
				inst->windowobj = (Object *) tag->ti_Data;
				break;
			}
		}
		return(TRUE);
	}
	return(FALSE);
}
// /

/** Exit a PopupMenu
*/
static ULONG PopupMenu_Exit(struct SC_Class *cl, Object *obj, Msg msg, struct PopupMenuInst *inst)
{
	struct MenuItemNode *node;

	if (inst->popupmenu)
		PM_FreePopupMenu(inst->popupmenu);

	for (node = (struct MenuItemNode *) inst->itemlist.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct MenuItemNode *) ((struct MinNode *) node)->mln_Succ)
	{
		SC_DisposeObject(node->itemobj);
	}

	FreeAllNodes(&inst->itemlist);

	return(SC_DoSuperMethodA(cl,obj, (Msg) msg));
}
// /

/****** PopupMenu.scalos/SCCM_PopupMenu_Open ********************************
*
*  NAME
*   SCCM_PopupMenu_Open
*
*  SYNOPSIS
*   ULONG SC_DoMethod(obj,SCCM_Popup_Open);
*
*  FUNCTION
*   This method opens the popup menu relative to the mouse position.
*   The method will return first after the menu was closed.
*
*  INPUTS
*
*  RESULT
*   TRUE if the popup was successfully open else FALSE.
*
*  SEE ALSO
*
*****************************************************************************
*/

static ULONG PopupMenu_Open(struct SC_Class *cl, Object *obj, Msg msg, struct PopupMenuInst *inst)
{
	if (!(IsMinListEmpty(&inst->itemlist)))
	{
		struct MenuItemNode *node;
		int entrycount = 1;
		struct TagItem *taglist;

		if (!(inst->popupmenu))
		{
			for (node = (struct MenuItemNode *) inst->itemlist.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct MenuItemNode *) ((struct MinNode *) node)->mln_Succ)
			{
				entrycount++;
			}

			if ((taglist = (struct TagItem *) AllocVec(entrycount*8,MEMF_ANY)))
			{
				struct TagItem *tag = taglist;

				for (node = (struct MenuItemNode *) inst->itemlist.mlh_Head; ((struct MinNode *) node)->mln_Succ; node = (struct MenuItemNode *) ((struct MinNode *) node)->mln_Succ)
				{
					tag->ti_Tag = PM_Item;
					if (!(node->item))
						node->item = (APTR) get(node->itemobj,SCCA_MenuItem_PopupItem);
					tag->ti_Data = (ULONG) node->item;
					tag++;
				}
				tag->ti_Tag = TAG_DONE;

				inst->popupmenu = PM_MakeMenuA(taglist);

				FreeVec(taglist);
			}
		}

		if (inst->popupmenu)
		{
			ULONG userdata;

			if ((userdata = PM_OpenPopupMenu((struct Window *) get(inst->windowobj,SCCA_Window_Window),
											PM_Menu, inst->popupmenu, TAG_DONE)))
			{
				SC_SetAttrs((Object *) userdata, SCCA_MenuItem_Activated,TRUE,TAG_DONE);
			}

			return(TRUE);
		}

	}

	return(FALSE);
}
// /


/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData PopupMenuMethods[] =
{
	{ SCCM_Init,(ULONG) PopupMenu_Init, 0, 0, NULL },
	{ SCCM_Exit,(ULONG) PopupMenu_Exit, 0, 0, NULL },
	{ SCMETHOD_DONE, NULL, 0, 0, NULL }
};


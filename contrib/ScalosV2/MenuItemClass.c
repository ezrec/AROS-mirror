// :ts=4 (Tabsize)

#include <proto/utility.h>
#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/pm.h>
#include <string.h>

#include <libraries/pm.h>
// #include <proto/pm.h>


#include "Scalos.h"
#include "MenuItemClass.h"
#include "ScalosIntern.h"

#include "scalos_protos.h"
#include "Debug.h"
#include "SubRoutines.h"

/****** MenuItem.scalos/--background ****************************************
*
*   This class provides menu items for menus.
*
*****************************************************************************
*/
// /

/****** MenuItem.scalos/SCCA_MenuItem_Name **********************************
*
*  NAME
*   SCCA_MenuItem_Name -- (V40) I.. (char *)
*
*  FUNCTION
*   This attribute will set the name of the item. The one which will be
*   displayed. The string will be copied.
*
*****************************************************************************
*/
// /
/****** MenuItem.scalos/SCCA_MenuItem_PopupItem *****************************
*
*  NAME
*   SCCA_MenuItem_PopupItem -- (V40) ..G (APTR)
*
*  FUNCTION
*   Getting this attribute will return a specific structure to generate
*   a popup menu item. This is for internal use only.
*
*****************************************************************************
*/
// /
/****** MenuItem.scalos/SCCA_MenuItem_Menu **********************************
*
*  NAME
*   SCCA_MenuItem_Menu -- (V40) I.. (Object *)
*
*  FUNCTION
*   This attribute is used to generate a sub menu item.
*
*****************************************************************************
*/
// /
/****** MenuItem.scalos/SCCA_MenuItem_Activated *****************************
*
*  NAME
*   SCCA_MenuItem_Activated -- (V40) .SG (ULONG)
*
*  FUNCTION
*   This attribute will be set after it was selected from user. It can be
*   used for notifies.
*
*****************************************************************************
*/
// /

/** Init a MenuItem
*/
static ULONG MenuItem_Init(struct SC_Class *cl, Object *obj, struct SCCP_Init *msg, struct MenuItemInst *inst)
{
	if (SC_DoSuperMethodA(cl,obj, (Msg) msg))
	{
		char *name;

		if ((name = (char *) GetTagData(SCCA_MenuItem_Name,0,msg->ops_AttrList)))
		{
			inst->name = AllocCopyString(name);
		}
		inst->submenu = (Object *) GetTagData(SCCA_MenuItem_Menu,0,msg->ops_AttrList);

		return(TRUE);
	}
	return(FALSE);
}
// /

/** Exit a MenuItem
*/
static ULONG MenuItem_Exit(struct SC_Class *cl, Object *obj, Msg msg, struct MenuItemInst *inst)
{
	if (inst->name)
		FreeVec(inst->name);

	if (inst->submenu)
		SC_DisposeObject(inst->submenu);

	return(SC_DoSuperMethodA(cl,obj, (Msg) msg));
}
// /

static ULONG MenuItem_Get( struct SC_Class *cl, Object *obj, struct opGet *msg, struct MenuItemInst *inst )
{
	if (msg->opg_AttrID == SCCA_MenuItem_PopupItem)
	{
		APTR pm;

		pm = PM_MakeItem(PM_Title,inst->name,PM_UserData,obj,TAG_DONE);

		*(msg->opg_Storage) = (ULONG) pm;
		return(TRUE);
	}

	return(SC_DoSuperMethodA(cl, obj, (Msg) msg));
}

/*-------------------------- MethodList --------------------------------*/

struct SC_MethodData MenuItemMethods[] =
{
	{ SCCM_Init,(ULONG) MenuItem_Init, 0, 0, NULL },
	{ SCCM_Exit,(ULONG) MenuItem_Exit, 0, 0, NULL },
	{ OM_GET,(ULONG) MenuItem_Get, 0, 0, NULL },
	{ SCMETHOD_DONE, NULL, 0, 0, NULL }
};


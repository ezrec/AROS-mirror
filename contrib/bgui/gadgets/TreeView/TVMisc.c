/*
 * @(#) $Header$
 *
 * BGUI Tree List View class
 *
 * (C) Copyright 1999 Manuel Lemos.
 * (C) Copyright 1996-1999 Nick Christie.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.2  2004/06/16 20:16:49  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.1  2000/05/15 19:29:08  stegerg
 * replacements for REG macro
 *
 * Revision 42.0  2000/05/09 22:21:51  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:37  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:35  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/05/31 01:15:50  mlemos
 * Made the method functions take the arguments in the apropriate registers.
 * Commented out debugging code.
 *
 * Revision 1.1.2.1  1999/02/21 04:07:50  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
*******************  TREEVIEW CLASS: MISCELLANEOUS  *********************
************************************************************************/

/************************************************************************
******************************  INCLUDES  *******************************
************************************************************************/

#include "TreeViewPrivate.h"
#include "TVUtil.h"

/************************************************************************
**************************  LOCAL DEFINITIONS  **************************
************************************************************************/


/************************************************************************
*************************  EXTERNAL REFERENCES  *************************
************************************************************************/

/*
 * Functions from TVUtil are listed in TVUtil.h
 */

/************************************************************************
*****************************  PROTOTYPES  ******************************
************************************************************************/

//ASM ULONG TV_Clear(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc);
ASM REGFUNCPROTO3(ULONG, TV_Clear,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc));

//ASM ULONG TV_Lock(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) Msg msg);
ASM REGFUNCPROTO3(ULONG, TV_Lock,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, Msg, msg));

//ASM ULONG TV_Unlock(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc);
ASM REGFUNCPROTO3(ULONG, TV_Unlock,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc));


//ASM ULONG TV_Sort(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc);
ASM REGFUNCPROTO3(ULONG, TV_Sort,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc));

//ASM ULONG TV_Redraw(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc);
ASM REGFUNCPROTO3(ULONG, TV_Redraw,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc));

//ASM ULONG TV_Refresh(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc);
ASM REGFUNCPROTO3(ULONG, TV_Refresh,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc));

//ASM ULONG TV_Rebuild(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc);
ASM REGFUNCPROTO3(ULONG, TV_Rebuild,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc));

/************************************************************************
*****************************  LOCAL DATA  ******************************
************************************************************************/


/************************************************************************
*****************************  TV_CLEAR()  ******************************
************************************************************************/

//ASM ULONG TV_Clear(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc)
ASM REGFUNC3(ULONG, TV_Clear,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc))
{
TVData	*tv;

tv = INST_DATA(cl,obj);

DoMethod(tv->tv_Listview,LVM_CLEAR,tvc->tvc_GInfo);
TV_FreeTreeNodeList(tv,RootList(tv));
NewList(RootList(tv));
tv->tv_NumEntries = 0;

return(0);
}
REGFUNC_END

/************************************************************************
******************************  TV_LOCK()  ******************************
************************************************************************/

//ASM ULONG TV_Lock(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) Msg msg)
ASM REGFUNC3(ULONG, TV_Lock,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, Msg, msg))
{
TVData	*tv;

tv = INST_DATA(cl,obj);

if (!tv->tv_LockCount)
	DoMethod(tv->tv_Listview,LVM_LOCKLIST,NULL);

tv->tv_LockCount++;

return(0);
}
REGFUNC_END

/************************************************************************
*****************************  TV_UNLOCK()  *****************************
************************************************************************/

//ASM ULONG TV_Unlock(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc)
ASM REGFUNC3(ULONG, TV_Unlock,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc))
{
TVData	*tv;

tv = INST_DATA(cl,obj);

if (tv->tv_LockCount)
	{
	tv->tv_LockCount--;

	if (!tv->tv_LockCount)
		DoMethod(tv->tv_Listview,LVM_UNLOCKLIST,tvc->tvc_GInfo);
	}

return(0);
}
REGFUNC_END

/************************************************************************
******************************  TV_SORT()  ******************************
*************************************************************************
* STUB! TV_SORT
*
*************************************************************************/

//ASM ULONG TV_Sort(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc)
ASM REGFUNC3(ULONG, TV_Sort,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc))
{
return(0);
}
REGFUNC_END

/************************************************************************
*****************************  TV_REDRAW()  *****************************
*************************************************************************
* STUB! TV_Redraw is using LVM_REFRESH for the moment because of lv bugs.
*
*************************************************************************/

//ASM ULONG TV_Redraw(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc)
ASM REGFUNC3(ULONG, TV_Redraw,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc))
{
TVData	*tv;

tv = INST_DATA(cl,obj);

return(DoMethod(tv->tv_Listview,LVM_REFRESH,tvc->tvc_GInfo));
}
REGFUNC_END

/************************************************************************
****************************  TV_REFRESH()  *****************************
************************************************************************/

//ASM ULONG TV_Refresh(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc)
ASM REGFUNC3(ULONG, TV_Refresh,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc))
{
TVData	*tv;

tv = INST_DATA(cl,obj);

return(DoMethod(tv->tv_Listview,LVM_REFRESH,tvc->tvc_GInfo));
}
REGFUNC_END

/************************************************************************
****************************  TV_REBUILD()  *****************************
*************************************************************************
* For debugging use only, clears the listview and reconstructs the
* entries that should be in it from the tree.
*
*************************************************************************/

//ASM ULONG TV_Rebuild(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvCommand *tvc)
ASM REGFUNC3(ULONG, TV_Rebuild,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCommand *, tvc))
{
TVData	*tv;
TNPTR	tn;
BOOL	ok;

tv = INST_DATA(cl,obj);

DoMethod(tv->tv_Listview,LVM_CLEAR,NULL);

tn = FirstChildIn(RootList(tv));
ok = TRUE;

while(tn && ok)
	{
	/* debug
	KPrintF("TVM_REBUILD: Parent:%-16s  Node:%-16s\n",
		tn->tn_Parent ? (STRPTR) tn->tn_Parent->tn_Entry : "Root",
		tn->tn_Entry);
	*/

	ok = (BOOL) DoMethod(tv->tv_Listview,LVM_ADDSINGLE,NULL,tn,LVAP_TAIL,0);
	tn = TV_GetNextTreeNode(tn,TVF_VISIBLE,0,~0);
	}

	/* debug
if (!ok)
	KPrintF("TVM_REBUILD: *** Failed to add to listview! ***\n");
	*/

DoMethod(tv->tv_Listview,LVM_REFRESH,tvc->tvc_GInfo);

return((ULONG) ok);
}
REGFUNC_END


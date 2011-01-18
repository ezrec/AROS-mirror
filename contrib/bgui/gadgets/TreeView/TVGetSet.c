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
 * Revision 42.0  2000/05/09 22:21:41  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:25  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:25  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/05/31 01:12:47  mlemos
 * Made the method functions take the arguments in the apropriate registers.
 *
 * Revision 1.1.2.1  1999/02/21 04:07:39  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
***********************  TREEVIEW CLASS: GET/SET  ***********************
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

//ASM ULONG TV_Get(REG(a0) Class *cl, REG(a2) Object *obj,REG(a1) struct opGet *opg);
ASM REGFUNCPROTO3(ULONG, TV_Get,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opGet *, opg));

//ASM ULONG TV_Set(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct opSet *ops);
ASM REGFUNCPROTO3(ULONG, TV_Set,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opSet *, ops));

/************************************************************************
*****************************  LOCAL DATA  ******************************
************************************************************************/


/************************************************************************
*****************************  TV_GET()  ********************************
*************************************************************************
* Handle OM_GET: return an object attribute. Supply our class ptr,
* object ptr and an opGet message. Returns TRUE if attribute was
* recognized, FALSE otherwise.
*
*************************************************************************/

//ASM ULONG TV_Get(REG(a0) Class *cl, REG(a2) Object *obj,REG(a1) struct opGet *opg)
ASM REGFUNC3(ULONG, TV_Get,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opGet *, opg))
{
TVData	*tv;
ULONG	rc;

tv = (TVData *) INST_DATA(cl,obj);
rc = 1;

switch (opg->opg_AttrID)
	{
	case TVA_Indentation:
		*(opg->opg_Storage) = tv->tv_Indentation;
		break;

	case TVA_Top:
		{
		ULONG	top;

		GetAttr(LISTV_NumEntries,tv->tv_Listview,&top);

		if (top)
			{
			GetAttr(LISTV_Top,tv->tv_Listview,&top);
			*(opg->opg_Storage) = (ULONG) TV_IndexToTreeNode(tv,top);
			}
		else
			*(opg->opg_Storage) = NULL;
		}
		break;

	case TVA_NumEntries:
		*(opg->opg_Storage) = tv->tv_NumEntries;
		break;

	case TVA_LastClicked:
		*(opg->opg_Storage) = (ULONG) tv->tv_LastClicked;
		break;

	case TVA_ViewBounds:
		GetAttr(LISTV_ViewBounds,tv->tv_Listview,opg->opg_Storage);
		break;

	default:
		rc = DoSuperMethodA(cl,obj,(Msg) opg);
		break;
	}

return(rc);
}
REGFUNC_END

/************************************************************************
*****************************  TV_SET()  ********************************
*************************************************************************
* Handle OM_SET: set an object attribute. Supply our class ptr,
* object ptr and an opGet message. Returns TRUE if new attribute(s)
* caused a visual update.
*
*************************************************************************/

//ASM ULONG TV_Set(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct opSet *ops)
ASM REGFUNC3(ULONG, TV_Set,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opSet *, ops))
{
struct TagItem	*tag;
TVData			*tv;
ULONG			rc;
BOOL			refresh;

tv = (TVData *) INST_DATA(cl,obj);
refresh = FALSE;

/*
 * Let our superclass react to OM_SET first
 */

rc = DoSuperMethodA(cl,obj,(Msg) ops);

/*
 * Now we look at the taglist
 */

if (tag = FindTagItem(TVA_Indentation,ops->ops_AttrList))
	{
	tv->tv_Indentation = max(tag->ti_Data,8);
	refresh = TRUE;
	rc = 1;
	}

if (refresh)
	DoMethod(obj,TVM_REFRESH,ops->ops_GInfo);

if (tag = FindTagItem(TVA_Top,ops->ops_AttrList))
	{
	struct TagItem	tags[2];
	TNPTR			tn;
	ULONG			pos;

	if (tn = TV_FindTreeNode(RootList(tv),(APTR) tag->ti_Data))
		{
		if ((pos = TV_TreeNodeToIndex(tv,tn)) != ~0)
			{
			/* debug
			KPrintF("OM_SET: tn=%08lx  pos=%lu\n",tn,pos);
			*/

			tags[0].ti_Tag = LISTV_Top;
			tags[0].ti_Data = pos;
			tags[1].ti_Tag = TAG_DONE;
			DoMethod(tv->tv_Listview,OM_SET,tags,ops->ops_GInfo);
			}

		rc = 1;
		}
	}

return(rc);
}
REGFUNC_END


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
 * Revision 42.0  2000/05/09 22:22:09  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:56  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:52  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/05/31 01:12:59  mlemos
 * Made the method functions take the arguments in the apropriate registers.
 *
 * Revision 1.1.2.1  1999/02/21 04:08:10  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
***********************  TREEVIEW CLASS: VISIBLE  ***********************
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

//ASM ULONG TV_Visible(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvEntry *tve);
ASM REGFUNCPROTO3(ULONG, TV_Visible,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvEntry *, tve));

/************************************************************************
*****************************  LOCAL DATA  ******************************
************************************************************************/


/************************************************************************
****************************  TV_VISIBLE()  *****************************
*************************************************************************
* Make an entry visible in the treeview, by a combination of scrolling
* the view area, and expanding parent entries. In the message structure
* for this method, the user supplies a reference to an entry, a code to
* indicate which entries to affect and some flags that allow further
* operations, eg. selecting the entry. This method returns non-zero if
* successful, zero on failure.
*
* Special values allowed for the reference entry are:
*	TV_ROOT			to represent the root of the tree (a dummy entry).
*	TV_SELECTED		for the (first) currently selected entry.
*
* If the relation entry is TV_SELECTED, but no entry is currently
* selected, nothing is done and zero is returned.
*
* Permitted values for the relationship code are as follows:
*	TVW_ENTRY				specified entry only
*	TVW_PARENT				parent of entry
*	TVW_CHILD_FIRST			first child of entry
*	TVW_CHILD_LAST			last child of entry
*	TVW_SIBLING_FIRST		first sibling of entry
*	TVW_SIBLING_LAST		last sibling of entry
*	TVW_SIBLING_NEXT		next sibling of entry
*	TVW_SIBLING_PREV		prev. sibling of entry
*	TVW_TREE_NEXT			next in tree from entry
*	TVW_TREE_PREV			prev. in tree from entry
*	TVW_TREE_PAGE_UP		page up in tree from entry
*	TVW_TREE_PAGE_DOWN		page down in tree from entry
*
* If the relation entry is TV_ROOT, only the TVW_CHILD_??? codes are
* permitted.
*
* Permitted bits for the flags parameter are:
*	TVF_SELECTED			affect only selected entries
*	TVF_SELECT				select entry
*	TVF_DESELECT			deselect entry
*	TVF_MULTISELECT			multi-select entry (with TVF_SELECT)
*	TVF_EXPAND				expand entry
*	TVF_CONTRACT			contract entry
*
*************************************************************************/

//ASM ULONG TV_Visible(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvEntry *tve)
ASM REGFUNC3(ULONG, TV_Visible,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvEntry *, tve))
{
struct tvAnchor	tva;
struct TagItem	tags[2];
TVData			*tv;
TNPTR			tn,pn;
ULONG			rc,pos,flags;

TV_DebugDumpMethod((Msg) tve);

tv = (TVData *) INST_DATA(cl,obj);
rc = 0;
flags = tve->tve_Flags & ~TVF_VISIBLE;
memset(&tva,0,sizeof(tva));

/*
 * Find the treenode corresponding to this tvEntry.
 */

if (tn = TV_MatchNextEntry(tv,tve->tve_Entry,tve->tve_Which,flags,&tva))
	{
	/*
	 * Expand all unexpanded parents of this entry
	 */

	pn = tn;

	while(pn = ParentOf(pn))
		{
		if (!IsExpanded(pn))
			DoMethod(obj,TVM_EXPAND,NULL,pn,TVW_ENTRY,TVF_INTERNAL);
		}

	/*
	 * Find this entry's position in the listview
	 * and tell the listview to make it visible.
	 */

	pos = TV_TreeNodeToIndex(tv,tn);

	tags[0].ti_Tag = LISTV_MakeVisible;
	tags[0].ti_Data = pos;
	tags[1].ti_Tag = TAG_DONE;
	DoMethod(tv->tv_Listview,OM_SET,tags,tve->tve_GInfo);

	rc = 1;

	/*
	 * User wants us to select the entry as well?
	 */

	if (tve->tve_Flags & (TVF_SELECT | TVF_DESELECT))
		DoMethod(obj,TVM_SELECT,tve->tve_GInfo,tn,TVW_ENTRY,TVF_INTERNAL |
			(tve->tve_Flags & (TVF_MULTISELECT | TVF_DESELECT)));

	/*
	 * User wants us to expand the entry as well?
	 */

	if (tve->tve_Flags & (TVF_EXPAND | TVF_CONTRACT))
		DoMethod(obj,TVM_EXPAND,tve->tve_GInfo,tn,TVW_ENTRY,TVF_INTERNAL |
			(tve->tve_Flags & TVF_CONTRACT));

	} /* endif matched entry */

return(rc);
}
REGFUNC_END


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
 * Revision 42.0  2000/05/09 22:21:58  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:45  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:42  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/05/31 01:12:54  mlemos
 * Made the method functions take the arguments in the apropriate registers.
 *
 * Revision 1.1.2.1  1999/02/21 04:07:58  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
***********************  TREEVIEW CLASS: REMOVE  ************************
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

//ASM ULONG TV_Remove(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvEntry *tve);
ASM REGFUNCPROTO3(ULONG, TV_Remove,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvEntry *, tve));

/************************************************************************
*****************************  LOCAL DATA  ******************************
************************************************************************/


/************************************************************************
*****************************  TV_REMOVE()  *****************************
*************************************************************************
* Remove one or more entries from the treeview. In the message structure
* for this method, the user supplies a reference to an entry, a code to
* indicate which entries to affect and some flags that allow further
* conditions, eg. removing only selected entries. This method returns
* the number of entries removed if successful, zero on failure.
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
*	TVW_CHILD_ALL			all children of entry
*	TVW_SIBLING_FIRST		first sibling of entry
*	TVW_SIBLING_LAST		last sibling of entry
*	TVW_SIBLING_NEXT		next sibling of entry
*	TVW_SIBLING_PREV		prev. sibling of entry
*	TVW_SIBLING_ALL			entry and all siblings
*	TVW_TREE_NEXT			next in tree from entry
*	TVW_TREE_PREV			prev. in tree from entry
*
* If the relation entry is TV_ROOT, only the TVW_CHILD_??? codes are
* permitted. When you remove an entry, all of its children are
* automatically removed first. To remove all entries in the treeview
* you are better off using the TVM_CLEAR method.
*
* Permitted bits for the flags parameter are:
*	TVF_SELECTED			affect only selected entries
*	TVF_VISIBLE				affect only visible entries
*
*************************************************************************/

//ASM ULONG TV_Remove(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvEntry *tve)
ASM REGFUNC3(ULONG, TV_Remove,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvEntry *, tve))
{
struct tvAnchor	tva;
TVData			*tv;
TNPTR			pn,tn,tn2;
ULONG			rc;
ULONG			refreshmethod;

TV_DebugDumpMethod((Msg) tve);

tv = (TVData *) INST_DATA(cl,obj);
rc = 0;
memset(&tva,0,sizeof(tva));
refreshmethod = 0;

/*
 * Find each treenode corresponding to this tvEntry.
 */

tn2 = TV_MatchNextEntry(tv,tve->tve_Entry,tve->tve_Which,tve->tve_Flags,&tva);

while(tn = tn2)		/* assign and test */
	{
	/*
	 * Find next match now, before we remove this one
	 */

	tn2 = TV_MatchNextEntry(tv,tve->tve_Entry,tve->tve_Which,tve->tve_Flags,&tva);

	/*
	 * Recurse down child nodes of this entry, removing each.
	 */

	while (HasChildren(tn))
		rc += DoMethod(obj,TVM_REMOVE,NULL,FirstChildOf(tn),TVW_ENTRY,TVF_INTERNAL);

	/*
	 * If it's displayed in the listview, remove it
	 */

	if (TV_IsDisplayed(tn))
		{
		DoMethod(tv->tv_Listview,LVM_REMENTRY,NULL,tn);
		refreshmethod = TVM_REFRESH;
		}

	/*
	 * Now remove the entry from the tree
	 */

	if (pn = ParentOf(tn))
		pn->tn_NumChildren--;
	tv->tv_NumEntries--;
	Remove((NODEPTR) tn);
	TV_FreeTreeNode(tv,tn);
	rc++;

	/*
	 * If tv_NoLeafImage is set and this was the last child of its
	 * parent, we need to refresh in order for the parent's image
	 * to be changed.
	 */

	if (tv->tv_NoLeafImage && pn && !HasChildren(pn))
		refreshmethod = TVM_REFRESH;

	} /* endwhile match another entry */

if (refreshmethod)
	{
	/*
	 * Refresh the listview:
	 */

	if (tve->tve_GInfo)
		DoMethod(obj,refreshmethod,tve->tve_GInfo);
	}

return(rc);
}
REGFUNC_END


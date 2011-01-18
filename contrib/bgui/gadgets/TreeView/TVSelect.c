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
 * Revision 42.0  2000/05/09 22:22:02  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:50  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:46  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/05/31 01:12:58  mlemos
 * Made the method functions take the arguments in the apropriate registers.
 *
 * Revision 1.1.2.1  1999/02/21 04:08:04  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
***********************  TREEVIEW CLASS: SELECT  ************************
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

//ASM ULONG TV_Select(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvEntry *tve);
ASM REGFUNCPROTO3(ULONG, TV_Select,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvEntry *, tve));

/************************************************************************
*****************************  LOCAL DATA  ******************************
************************************************************************/


/************************************************************************
*****************************  TV_SELECT()  *****************************
*************************************************************************
* Select or deselect entries in the treeview. In the message structure
* for this method, the user supplies a reference to an entry, a code
* to indicate which entries to consider, and flags to further alter the
* mode of operation. This method selects or deselects the matching
* entries and returns the number of entries affected. Only visible
* entries may be selected.
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
*	TVW_CHILD_TREE			all children, recursively
*	TVW_SIBLING_FIRST		first sibling of entry
*	TVW_SIBLING_LAST		last sibling of entry
*	TVW_SIBLING_NEXT		next sibling of entry
*	TVW_SIBLING_PREV		prev. sibling of entry
*	TVW_SIBLING_ALL			entry and all siblings
*	TVW_SIBLING_TREE		entry, siblings and all children recursively
*	TVW_TREE_NEXT			next in tree from entry
*	TVW_TREE_PREV			prev. in tree from entry
*	TVW_TREE_PAGE_UP		page up in tree from entry
*	TVW_TREE_PAGE_DOWN		page down in tree from entry
*
* If the relation entry is TV_ROOT, only the TVW_CHILD_??? codes are
* permitted.
*
* Permitted bits for the flags parameter are:
*	TVF_DESELECT			deselect entry instead of select
*	TVF_TOGGLE				toggle select/deselect
*	TVF_MULTISELECT			multi-(de)select entry
*	TVF_MAKEVISIBLE			make entry visible
*	TVF_EXPAND				expand entry
*	TVF_CONTRACT			contract entry
*
*************************************************************************/

//ASM ULONG TV_Select(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvEntry *tve)
ASM REGFUNC3(ULONG, TV_Select,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvEntry *, tve))
{
struct tvAnchor	tva;
TVData			*tv;
TNPTR			tn;
ULONG			rc,pos,flags,tnflags,lvattr;
BOOL			chg;

TV_DebugDumpMethod((Msg) tve);

tv = (TVData *) INST_DATA(cl,obj);
tn = NULL;
rc = 0;
flags = (tve->tve_Flags | TVF_VISIBLE) & ~TVF_SELECTED;
memset(&tva,0,sizeof(tva));

/*
 * Update the TNF_SELECTED flag of all TreeNodes
 * by interrogating the listview.
 */

TV_UpdateSelected(tv);

/*
 * Find each treenode corresponding to this tvEntry.
 */

while(tn = TV_MatchNextEntry(tv,tve->tve_Entry,tve->tve_Which,flags,&tva))
	{
	/*
	 * User wants us to make the entry visible?
	 */

	if (tve->tve_Flags & TVF_MAKEVISIBLE)
		DoMethod(obj,TVM_VISIBLE,tve->tve_GInfo,tn,TVW_ENTRY,TVF_INTERNAL);

	/*
	 * Find it's position in the listview
	 */

	if ((pos = TV_TreeNodeToIndex(tv,tn)) != ~0)
		{
		/*
		 * See if we are required to select, deselect or toggle the
		 * entry, and whether that's a change from the current state.
		 */

		if (flags & TVF_TOGGLE)
			{
			tnflags = tn->tn_Flags ^ TNF_SELECTED;
			chg = TRUE;
			}
		else if ((flags & TVF_DESELECT) && IsSelected(tn))
			{
			tnflags = tn->tn_Flags & ~TNF_SELECTED;
			chg = TRUE;
			}
		else if (!(flags & TVF_DESELECT) && !IsSelected(tn))
			{
			tnflags = tn->tn_Flags | TNF_SELECTED;
			chg = TRUE;
			}
		else
			chg = FALSE;

		if (chg)	/* yes, it's a change of state */
			{
			struct TagItem	tags[2];

			rc++;					/* one more entry affected */
			tn->tn_Flags = tnflags;

			if (IsSelected(tn))
				{
				switch(flags & (TVF_MAKEVISIBLE|TVF_MULTISELECT))
					{
					case TVF_MULTISELECT:
						lvattr = LISTV_SelectMultiNotVisible;
						break;

					case TVF_MAKEVISIBLE:
						lvattr = LISTV_Select;
						break;

					case TVF_MAKEVISIBLE|TVF_MULTISELECT:
						lvattr = LISTV_SelectMulti;
						break;

					default:
						lvattr = LISTV_SelectNotVisible;
						break;
					}
				}
			else	/* deselect entry */
				lvattr = LISTV_DeSelect;

			tags[0].ti_Tag = lvattr; tags[0].ti_Data = pos;
			tags[1].ti_Tag = TAG_DONE;
			DoMethod(tv->tv_Listview,OM_SET,tags,tve->tve_GInfo);

			} /* endif change of state */

		/*
		 * User wants us to expand the selected entry, as well?
		 */

		if (tve->tve_Flags & (TVF_EXPAND|TVF_CONTRACT))
			DoMethod(obj,TVM_EXPAND,tve->tve_GInfo,tn,TVW_ENTRY,
				TVF_INTERNAL | (tve->tve_Flags & TVF_CONTRACT));

		} /* endif got pos in listview */

	} /* endwhile match another entry */

return(rc);
}
REGFUNC_END


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
 * Revision 42.5  2004/06/20 12:24:32  verhaegs
 * Use REGFUNC macro's in BGUI source code, not AROS_UFH
 *
 * Revision 42.4  2004/06/16 20:16:49  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.3  2003/01/18 19:10:20  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.2  2000/07/09 03:05:09  bergers
 * Makes the gadgets compilable.
 *
 * Revision 42.1  2000/05/15 19:29:08  stegerg
 * replacements for REG macro
 *
 * Revision 42.0  2000/05/09 22:21:35  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:20  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:20  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/05/31 01:12:43  mlemos
 * Made the method functions take the arguments in the apropriate registers.
 *
 * Revision 1.1.2.1  1999/02/21 04:07:34  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
***********************  TREEVIEW CLASS: EXPAND  ************************
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

//ASM ULONG TV_Expand(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvEntry *tve);
ASM REGFUNCPROTO3(ULONG, TV_Expand,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvEntry *, tve));

/************************************************************************
*****************************  LOCAL DATA  ******************************
************************************************************************/


/************************************************************************
*****************************  TV_EXPAND()  *****************************
*************************************************************************
* Expand or contract one or more entries in the treeview. In the message
* structure for this method, the user supplies a reference to an entry,
* a code to indicate which entries to affect and some flags that allow
* further operations, eg. making an entry visible. This method returns
* non-zero if successful, zero on failure.
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
* permitted. To expand or contract all entries in the treeview,
* pass TV_ROOT as the reference entry and TVW_CHILD_TREE as the code.
*
* Permitted bits for the flags parameter are:
*	TVF_SELECTED			affect only selected entries
*	TVF_VISIBLE				affect only visible entries
*	TVF_SELECT				select entry
*	TVF_DESELECT			deselect entry
*	TVF_MULTISELECT			multi-select entry (with TVF_SELECT)
*	TVF_MAKEVISIBLE			make entry visible
*	TVF_EXPAND				expand entry (not required)
*	TVF_CONTRACT			contract entry
*	TVF_TOGGLE				toggle expand/contract
*
* The entry will be displayed in the treeview only if all its parents
* are expanded, or the flag TVF_MAKEVISIBLE is used. In that case,
* parents will be expanded as required to bring the new entry into view.
*
*************************************************************************/

//ASM ULONG TV_Expand(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvEntry *tve)
ASM REGFUNC3(ULONG, TV_Expand,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvEntry *, tve))
{
struct tvExpand	tvexp;
struct tvAnchor	tva;
TVData			*tv;
TNPTR			tn,cn;
ULONG			rc,pos;
ULONG			lvflags,tnflags;
ULONG			refreshmethod;
ULONG			depth; 
BOOL			chg,ok;

TV_DebugDumpMethod((Msg) tve);

tv = (TVData *) INST_DATA(cl,obj);
rc = 0;
lvflags = 0;
tn = NULL;
memset(&tva,0,sizeof(tva));
refreshmethod = TVM_REDRAW;

/*
 * Find each treenode corresponding to this tvEntry.
 */

while(tn = TV_MatchNextEntry(tv,tve->tve_Entry,tve->tve_Which,tve->tve_Flags,&tva))
	{
	/*
	 * Find it's position in the listview (~0 if not displayed)
	 */

	pos = TV_TreeNodeToIndex(tv,tn);

	/*
	 * See if we are required to expand, contract or toggle the
	 * entry, and whether that's a change from the current state.
	 */

	if (tve->tve_Flags & TVF_TOGGLE)
		{
		tnflags = tn->tn_Flags ^ TNF_EXPANDED;
		chg = TRUE;
		}
	else if ((tve->tve_Flags & TVF_CONTRACT) && (tn->tn_Flags & TNF_EXPANDED))
		{
		tnflags = tn->tn_Flags & ~TNF_EXPANDED;
		chg = TRUE;
		}
	else if (!(tn->tn_Flags & TNF_EXPANDED))
		{
		tnflags = tn->tn_Flags | TNF_EXPANDED;
		chg = TRUE;
		}
	else
		chg = FALSE;

	if (chg)	/* yes, it's a change of state */
		{
		/*
		 * Confirm the change with the user's expand hook
		 */

		tvexp.tve_Command = tnflags & TNF_EXPANDED ? TVEC_EXPAND : TVEC_CONTRACT;
		tvexp.tve_Entry = tve->tve_Entry;
		tvexp.tve_Flags = 0;
		if (tve->tve_Flags & TVF_USER_ACTION)
			tvexp.tve_Flags = TVEF_USER_ACTION;

		if (CallHookPkt(tv->tv_ExpandHook,tv->tv_TreeView,&tvexp))
			{
			/*
			 * Set the new state in the node
			 */

			tn->tn_Flags = tnflags;
			rc = tnflags & TNF_EXPANDED ? 1 : ~0;

			/*
			 * See if the node has children and if it's displayed
			 * in the listview. If not, we don't have to insert
			 * its children in the listview.
			 */

			if (HasChildren(tn) && (pos != ~0))
				{
				if (rc == 1)		/* expand node */
					{
					/*
					 * Insert each child, recursively, into the
					 * listview at incrementing position after
					 * the parent node that we expanded.
					 */

					cn = FirstChildOf(tn);
					pos++;
					depth = TV_TreeNodeDepth(cn);
					ok = TRUE;

					while(ok && cn)
						{
						ok = (BOOL) DoMethod(tv->tv_Listview,LVM_INSERTSINGLE,
							NULL,pos,cn,lvflags);
						pos++;
						cn = TV_GetNextTreeNode(cn,TVF_VISIBLE,depth,~0);
						}
					}
				else	/* contract node */
					{
					/*
					 * Remove each child, recursively, from the listview
					 */

					cn = FirstChildOf(tn);
					depth = TV_TreeNodeDepth(cn);

					while(cn)
						{
						DoMethod(tv->tv_Listview,LVM_REMENTRY,NULL,cn);
						cn = TV_GetNextTreeNode(cn,TVF_VISIBLE,depth,~0);
						}
					}

				/*
				 * We will want the listview to refresh completely
				 * because we have inserted/removed entries
				 */

				refreshmethod = TVM_REFRESH;

				} /* endif node has children and is displayed */

			/*
			 * User wants us to make the expanded entry visible?
			 */

			if (tve->tve_Flags & TVF_MAKEVISIBLE)
				DoMethod(obj,TVM_VISIBLE,NULL,tn,TVW_ENTRY,TVF_INTERNAL);

			/*
			 * User wants us to select the expanded entry, as well?
			 */

			if (tve->tve_Flags & (TVF_SELECT|TVF_DESELECT))
				DoMethod(obj,TVM_SELECT,NULL,tn,TVW_ENTRY,TVF_INTERNAL |
					(tve->tve_Flags & (TVF_MULTISELECT | TVF_DESELECT)));

			} /* endif expandhook return true */

		} /* endif expand flag changed */

	} /* endwhile match another entry */

if (rc)
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

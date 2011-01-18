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
 * Revision 42.0  2000/05/09 22:21:45  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:31  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:30  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/05/31 01:12:51  mlemos
 * Made the method functions take the arguments in the apropriate registers.
 *
 * Revision 1.1.2.1  1999/02/21 04:07:44  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
***********************  TREEVIEW CLASS: INSERT  ************************
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

//ASM ULONG TV_Insert(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvInsert *tvi);
ASM REGFUNCPROTO3(ULONG, TV_Insert,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvInsert *, tvi));

/************************************************************************
*****************************  LOCAL DATA  ******************************
************************************************************************/


/************************************************************************
*****************************  TV_INSERT()  *****************************
*************************************************************************
* Insert a new entry into the treeview. In the message structure for
* this method, the user includes the entry to insert, a relation entry
* to serve as reference for the insertion position, a code to indicate
* what kind of relationship there should be between the new entry and
* the relation, plus some flags to expand the new node, select it, etc.
* This method returns non-zero if successful, zero on failure.
*
* Special values allowed for the relation entry are:
*	TV_ROOT			to represent the root of the tree (a dummy entry).
*	TV_SELECTED		for the (first) currently selected entry.
*
* If the relation entry is TV_SELECTED, but no entry is currently
* selected, the entry is not inserted and zero is returned.
*
* Permitted values for the relationship code are as follows:
*	TVW_CHILD_FIRST			first child of relation
*	TVW_CHILD_LAST			last child of relation
*	TVW_CHILD_SORTED		child of relation, sorted
*	TVW_SIBLING_FIRST		first sibling of relation
*	TVW_SIBLING_LAST		last sibling of relation
*	TVW_SIBLING_NEXT		next sibling of relation
*	TVW_SIBLING_PREV		prev. sibling of relation
*	TVW_SIBLING_SORTED		sibling of relation, sorted
*
* If the relation entry is TV_ROOT, only the TVW_CHILD_??? codes are
* permitted.
*
* Permitted bits for the flags parameter are:
*	TVF_SELECT				select entry
*	TVF_MULTISELECT			multi-select entry (with TVF_SELECT)
*	TVF_MAKEVISIBLE			make entry visible
*	TVF_EXPAND				expand entry
*
* The new entry will be displayed in the treeview only if all its parents
* are expanded, or a flag such as TVF_SELECT is used. In that case,
* parents will be expanded as required to bring the new entry into view
* and/or select it.
*
*************************************************************************/

//ASM ULONG TV_Insert(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct tvInsert *tvi)
ASM REGFUNC3(ULONG, TV_Insert,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvInsert *, tvi))
{
TVData	*tv;
TNPTR	tn,prevtn,par,tr,sr;
LISTPTR	list;
ULONG	rc,tvflags,lvflags,pos;

TV_DebugDumpMethod((Msg) tvi);

tv = (TVData *) INST_DATA(cl,obj);
rc = 0;
pos = ~0;
tvflags = 0;
lvflags = 0;

/*
 * Set expanded flag if requested
 */

if (tvi->tvi_Flags & TVF_EXPAND)
	tvflags |= TNF_EXPANDED;

/*
 * Allocate a new node for the entry and initialise it's child list.
 */

if (tn = TV_AllocTreeNode(tv,tvi->tvi_Entry,tvflags))
	{
	/*
	 * Locate the treenode corresponding to the relation entry,
	 * which may be TV_SELECTED for the first selected entry.
	 */

	list = NULL;
	tr = NULL;

	if (tvi->tvi_Relation == TV_ROOT)
		{
		/*
		 * Relation is the dummy root node, only TVWS_CHILD_? permitted
		 */

		if ((tvi->tvi_Where & TVWC_MASK) == TVWC_CHILD)
			tr = &tv->tv_RootNode;
		}
	else if (tvi->tvi_Relation == TV_SELECTED)
		/* relation is the first selected entry */
		tr = (TNPTR) DoMethod(tv->tv_Listview,LVM_FIRSTENTRY,NULL,LVGEF_SELECTED);
	else if (tvi->tvi_Flags & TVF_INTERNAL)
		/* internal reference, already a TreeNode ptr */
		tr = tvi->tvi_Relation;
	else
		/* a user's entry: find its TreeNode */
		tr = TV_FindTreeNode(RootList(tv),tvi->tvi_Relation);

	if (tr)
		{
		if ((tvi->tvi_Where & TVWC_MASK) == TVWC_CHILD)
			par = tr;				/* parent is relation */
		else if ((tvi->tvi_Where & TVWC_MASK) == TVWC_SIB)
			par = ParentOf(tr);		/* parent is relation's parent */
		else
			par = NULL;				/* no other category allowed */

		if (par)
			{
			list = ChildListOf(par);
			pos = ~0;

			switch(tvi->tvi_Where & TVWS_MASK)
				{
				case TVWS_FIRST:	/* first child/sibling of relation */
					prevtn = NULL;
					sr = par;
					break;

				case TVWS_LAST:		/* last child/sibling of relation */
					prevtn = LastChildIn(list);
					sr = TV_LastDisplayedChild(par);
					break;

				case TVWS_NEXT:		/* next sibling of relation */
					if ((tvi->tvi_Where & TVWC_MASK) == TVWC_SIB)
						{
						prevtn = tr;
						sr = TV_LastDisplayedChild(prevtn);
						}
					else
						list = NULL;
					break;

				case TVWS_PREV:		/* prev. sibling of relation */
					if ((tvi->tvi_Where & TVWC_MASK) == TVWC_SIB)
						{
						prevtn = PrevSiblingOf(tr);
						sr = prevtn ? prevtn : par;
						}
					else
						list = NULL;
					break;

				case TVWS_SORT:		/* sorted child/sibling of relation */
					prevtn = TV_SortedPrev(tv,list,tn);
					sr = prevtn ? TV_LastDisplayedChild(prevtn) : par;
					break;

				default:			/* no other tvi_Where allowed */
					list = NULL;
					break;

				} /* end switch(tvi->tvi_Where & TVWS_MASK) */

			if (list)
				{
				if (TV_IsDisplayed(prevtn ? prevtn : par))
					pos = TV_TreeNodeToIndex(tv,sr) + 1;
				}

			} /* endif got parent */

		} /* endif found relation */

	if (list)	/* passed all tests above */
		{
		/*
		 * Add the new entry to the child list of the parent node
		 * (the rootlist if there is no parent) and bump up the total
		 * count of entries. If the new entry has a parent, put in
		 * the link to the parent and increment the parent's child count.
		 */

		Insert(list,(NODEPTR) tn,(NODEPTR) prevtn);
		tv->tv_NumEntries++;

		if (par)
			{
			tn->tn_Parent = par;
			par->tn_NumChildren++;
			}

		/*
		 * If we have a position for the entry in the listview,
		 * and the parent is either expanded, or the root entry,
		 * insert the entry in the listview.
		 */

		if ((pos != ~0) && (!par || IsExpanded(par)))
			rc = DoMethod(tv->tv_Listview,LVM_INSERTSINGLE,tvi->tvi_GInfo,
					pos, tn, lvflags);
		else
			{
			/*
			 * If tv_NoLeafImage is set, and this new entry is the
			 * first child of its parent, we need to refresh the
			 * parent as the image will be applicable.
			 */

			if (tv->tv_NoLeafImage && par && (par->tn_NumChildren == 1))
				DoMethod(obj,TVM_REFRESH,tvi->tvi_GInfo);

			rc = 1;
			}

		if (rc)		/* new entry added to listview */
			{
			/*
			 * User wants us to make the new entry visible?
			 */

			if (tvi->tvi_Flags & TVF_MAKEVISIBLE)
				DoMethod(obj,TVM_VISIBLE,tvi->tvi_GInfo,tn,
					TVW_ENTRY,TVF_INTERNAL);
			/*
			 * User wants us to select the new entry, as well?
			 */

			if (tvi->tvi_Flags & TVF_SELECT)
				DoMethod(obj,TVM_SELECT,tvi->tvi_GInfo,tn,TVW_ENTRY,
					TVF_INTERNAL|(tvi->tvi_Flags & TVF_MULTISELECT));
			}
		else		/* failed to add to listview */
			{
			Remove((NODEPTR) tn);
			TV_FreeTreeNode(tv,tn);
			tv->tv_NumEntries--;
			if (par)
				par->tn_NumChildren--;
			}

		} /* endif got list */
	else
		TV_FreeTreeNode(tv,tn);

	} /* endif alloctreenode */

return(rc);
}
REGFUNC_END


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
 * Revision 42.0  2000/05/09 22:22:05  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:52  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:49  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.2  1999/05/25 00:03:58  mlemos
 * Fixed bug of not testing the end of the tree properly when fetching the
 * next node.
 *
 * Revision 1.1.2.1  1999/02/21 04:08:07  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
*********************  TREEVIEW UTILITY ROUTINES  ***********************
************************************************************************/

/************************************************************************
******************************  INCLUDES  *******************************
************************************************************************/

#include "TreeViewPrivate.h"
#include "TVUtil.h"

/************************************************************************
**************************  LOCAL DEFINITIONS  **************************
*************************************************************************/


/************************************************************************
*****************************  REFERENCES  ******************************
*************************************************************************/


/************************************************************************
*****************************  PROTOTYPES  ******************************
************************************************************************/

/*
 * All global, listed in TVUtil.h
 */

/************************************************************************
*****************************  LOCAL DATA  ******************************
************************************************************************/


/************************************************************************
***************************  TV_DOSUPERNEW()  ***************************
*************************************************************************
* Calls OM_NEW on the superclass, allowing a taglist to be built on
* the stack. Returns the new object, or NULL on failure.
*
*************************************************************************/

ULONG TV_DoSuperNew(Class *cl,Object *obj,Tag tag1,...)
{
return(DoSuperMethod(cl,obj,OM_NEW,&tag1,NULL));
}

/************************************************************************
************************  TV_NOTIFYATTRCHANGE()  ************************
*************************************************************************
* Notify about an attribute change, allowing a taglist to be built on
* the stack. Returns the result of the OM_NOTIFY method.
*
*************************************************************************/

ULONG TV_NotifyAttrChange(Object *obj,struct GadgetInfo *gi,
	ULONG flags,Tag tag1,...)
{
return(DoMethod(obj,OM_NOTIFY,&tag1,gi,flags));
}


/************************************************************************
******************************  GETXXXX()  ******************************
*************************************************************************
* Small list/node manipulation routines.
*
* TV_GetHead: returns first node, NULL if empty list.
* TV_GetTail: returns last node, NULL if empty list.
* TV_GetSucc: returns next node, NULL if no next.
* TV_GetPred: returns prev node, NULL if no prev.
*
* In all cases, node passed in may not be NULL, list must be
* correctly initialized, and node must be node (not list header).
*
*************************************************************************/

#ifndef TV_NODE_INLINE

NODEPTR TV_GetHead(LISTPTR list)
{
return(list->lh_Head->ln_Succ ? list->lh_Head : NULL);
}

NODEPTR TV_GetTail(LISTPTR list)
{
return(list->lh_TailPred->ln_Pred ? list->lh_TailPred : NULL);
}

NODEPTR TV_GetSucc(NODEPTR node)
{
return(node->ln_Succ->ln_Succ ? node->ln_Succ : NULL);
}

NODEPTR TV_GetPred(NODEPTR node)
{
return(node->ln_Pred->ln_Pred ? node->ln_Pred : NULL);
}

#endif

/************************************************************************
****************************  TV_ALLOCVEC()  ****************************
*************************************************************************
* Like Exec/AllocVec(), but in a memory pool attached to the TVData
* supplied, if available. Memory type returned is always MEMF_ANY and
* MEMF_CLEAR.
*
*************************************************************************/

void *TV_AllocVec(TVData *tv,ULONG size)
{
ULONG	*mem;

#ifdef MEMDBG

static ULONG Cookie[] = { 0xdeadbeef, 0xabadcafe };

if (tv->tv_MemPool)
	mem = AllocPooled(tv->tv_MemPool,size + 4 + 2 * 8);
else
	mem = AllocMem(size + 4 + 2 * 8,MEMF_STD);

if (mem)
	{
	mem[0] = size;
	memcpy(&mem[1],Cookie,8);
	mem = &mem[3];
	memcpy(((UBYTE *)mem) + size,Cookie,8);
	}

#else	/* !MEMDBG */

if (tv->tv_MemPool)
	{
	if (mem = AllocPooled(tv->tv_MemPool,size + 4))
		*mem++ = size;
	}
else
	mem = AllocVec(size,MEMF_STD);

#endif	/* !MEMDBG */

return((void *)mem);
}

/************************************************************************
****************************  TV_FREEVEC()  *****************************
*************************************************************************
* Like Exec/FreeVec(), but in a memory pool attached to the TVData
* supplied, if available.
*
*************************************************************************/

void TV_FreeVec(TVData *tv,void *mem)
{
ULONG	*p,size;

#ifdef MEMDBG

BOOL	ok;
static ULONG Cookie[2] = { 0xdeadbeef, 0xabadcafe };

if (mem)
	{
	ok = FALSE;
	p = (ULONG *) (((UBYTE *) mem) - 12);
	size = p[0];
	if ((p[1] == Cookie[0]) && (p[2] == Cookie[1]) &&
		/* WARNING! These two might not be longword aligned! */
			(*((ULONG *)(((UBYTE *)&p[3]) + size)) == Cookie[0]) &&
			(*((ULONG *)(((UBYTE *)&p[4]) + size)) == Cookie[1]))
		ok = TRUE;

	if (tv->tv_MemPool)
		FreePooled(tv->tv_MemPool,p,size + 4 + 2 * 8);
	else
		FreeMem(p,size + 4 + 2 * 8);

	if (!ok)
		KPrintF("Treeview: Memory (size %lu) munged at 0x%08lx!\n",size,p);
	}

#else	/* !MEMDBG */

if (mem)
	{
	if (tv->tv_MemPool)
		{
		p = (ULONG *)mem;
		size = *(--p);
		FreePooled(tv->tv_MemPool,p,size + 4);
		}
	else
		FreeVec(mem);
	}

#endif	/* !MEMDBG */
}

/************************************************************************
***************************  TV_ALLOCHOOK()  ****************************
************************************************************************/

HOOKPTR TV_AllocHook(HOOKFUNC handler,APTR data)
{
HOOKPTR	hook;

if (hook = AllocMem(sizeof(struct Hook),MEMF_STD))
	{
	hook->h_Entry = handler;
	hook->h_Data = data;
	}

return(hook);
}

/************************************************************************
****************************  TV_FREEHOOK()  ****************************
************************************************************************/

void TV_FreeHook(HOOKPTR hook)
{
if (hook)
	FreeMem(hook,sizeof(struct Hook));
}

/************************************************************************
***************************  TV_ALLOCSTRCPY()  **************************
*************************************************************************
* Copy a string to AllocPool'd memory. Returns the new copy or NULL if
* alloc failed. You can ask for 'extra' bytes to be allocated after the
* string as well, these will be initialized to NULL.
*
*************************************************************************/

STRPTR TV_AllocStrCpy(TVData *tv,STRPTR s,ULONG extra)
{
STRPTR	t;

t = NULL;

if (s)
	{
	if (t = TV_AllocVec(tv,strlen(s) + extra + 1))
		strcpy(t,s);
	}

return(t);
}

/************************************************************************
*************************  TV_ALLOCTREENODE()  **************************
*************************************************************************
* Allocate a new TreeNode, passing the supplied 'entry' through the
* user's resource hook. Places the translated entry and the supplied
* flags in the treenode, but doesn't set the node's parent, or attach
* it to any list. The treenode's child list is initialised. Returns the
* new TreeNode, or NULL on failure.
*
*************************************************************************/

TNPTR TV_AllocTreeNode(TVData *tv,APTR entry,ULONG flags)
{
struct tvResource	tvr;
TNPTR				tn;
ULONG				xentry;

tvr.tvr_Command = TVRC_MAKE;
tvr.tvr_Entry = entry;

if (tn = TV_AllocVec(tv,sizeof(struct TreeNode)))
	{
	if (xentry = CallHookPkt(tv->tv_ResourceHook,tv->tv_TreeView,&tvr))
		{
		tn->tn_Entry = (APTR) xentry;
		tn->tn_Flags = flags;
		NewList(ChildListOf(tn));
		}
	else
		{
		TV_FreeVec(tv,tn);
		tn = NULL;
		}
	}

return(tn);
}

/************************************************************************
**************************  TV_FREETREENODE()  **************************
*************************************************************************
* Free a TreeNode allocated by TV_AllocTreeNode(). Can be safely passed
* a NULL pointer. Does not detach the node from any list. Does not free
* any child treenodes that may be attached. Frees the node's entry via
* the user's custom resource hook.
*
*************************************************************************/

void TV_FreeTreeNode(TVData *tv,TNPTR tn)
{
struct tvResource	tvr;

if (tn)
	{
	tvr.tvr_Command = TVRC_KILL;
	tvr.tvr_Entry = (APTR) tn->tn_Entry;
	CallHookPkt(tv->tv_ResourceHook,tv->tv_TreeView,&tvr);
	TV_FreeVec(tv,tn);
	}
}

/************************************************************************
************************  TV_FREETREENODELIST()  ************************
*************************************************************************
* Free a list of TreeNodes, and all children. Does not re-initialise
* the list.
*
*************************************************************************/

void TV_FreeTreeNodeList(TVData *tv,LISTPTR list)
{
TNPTR	tn,tn2;

tn2 = FirstChildIn(list);

while(tn = tn2)		/* assign and test */
	{
	tn2 = NextSiblingOf(tn);
	TV_FreeTreeNodeList(tv,ChildListOf(tn));
	TV_FreeTreeNode(tv,tn);
	}
}

/************************************************************************
*************************  TV_TREENODEDEPTH()  **************************
*************************************************************************
* Return the 'depth' of the given TreeNode within its hierarchy, ie.
* how many parents it has.
*
*************************************************************************/

ULONG TV_TreeNodeDepth(TNPTR tn)
{
ULONG	depth;

depth = 0;

while(tn = ParentOf(tn))
	depth++;

return(depth);
}

/************************************************************************
************************  TV_INDEXTOTREENODE()  *************************
*************************************************************************
* Convert a ULONG 0-based index into the set of entries displayed in the
* listview, into a pointer to the node at that index. Returns NULL if
* there is no node with that index.
*
*************************************************************************/

TNPTR TV_IndexToTreeNode(TVData *tv,ULONG idx)
{
TNPTR	tn;

tn = FirstChildIn(RootList(tv));

while(tn && idx)
	{
	tn = TV_GetNextTreeNode(tn,TVF_VISIBLE,0,~0);
	idx--;
	}

return(tn);
}

/************************************************************************
************************  TV_TREENODETOINDEX()  *************************
*************************************************************************
* Given a ptr to a TreeNode, determine its 0-based index in the listview,
* returning ~0 if the node isn't displayed in the list.
*
*************************************************************************/

ULONG TV_TreeNodeToIndex(TVData *tv,TNPTR in)
{
TNPTR	tn;
ULONG	idx;

idx = 0;
tn = FirstChildIn(RootList(tv));

while(tn && (tn != in))
	{
	idx++;
	tn = TV_GetNextTreeNode(tn,TVF_VISIBLE,0,~0);
	}

return(tn ? idx : ~0);
}

/************************************************************************
************************  TV_GETNEXTTREENODE()  *************************
*************************************************************************
* Return the next node after the given one, descending into child nodes
* where available, and ascending back to parent nodes when the children
* run out. If TVF_VISIBLE is set in flags, descends only into child nodes
* when the parent node is expanded. If TVF_SELECTED is set in flags,
* repeats until a selected treenode is found. Stops if the depth of the
* search becomes less than mindepth, does not descend into child nodes
* when the depth would become greater than maxdepth. Returns a ptr to the
* next treenode, or NULL if at end.
*
*************************************************************************/

TNPTR TV_GetNextTreeNode(TNPTR tn,ULONG flags,ULONG mindepth,ULONG maxdepth)
{
TNPTR	pn;
BOOL	hidden;
BOOL	selected;

hidden = (flags & TVF_VISIBLE) ? FALSE : TRUE;
selected = (flags & TVF_SELECTED) ? TRUE : FALSE;

do
	{
	if (HasChildren(tn) && (hidden || IsExpanded(tn))
			&& (TV_TreeNodeDepth(tn) < maxdepth))
		{
		/* descend into child nodes of current node */
		tn = FirstChildOf(tn);
		}
	else
		{
		pn = ParentOf(tn);

		/* get next node at current level */
		tn = NextSiblingOf(tn);

		/* end of nodes at current level? then get next of parent, if any */
		while(!tn && pn)
			{
			if (tn = NextSiblingOf(pn))
				{
				pn = tn->tn_Parent;

				/* don't rise above mindepth */
				if (TV_TreeNodeDepth(tn) < mindepth)
					{ tn = pn = NULL; }
				}
			else
				if((pn = ParentOf(pn))
				&& ParentOf(pn)==NULL)
					break;
			}
		}

	} while(tn && selected && !IsSelected(tn));

return(tn);
}

/************************************************************************
************************  TV_GETPREVTREENODE()  *************************
*************************************************************************
* Return the previous node after the given one, descending into child
* nodes where available, and ascending back to parent nodes when the
* children run out. If flags bit TVF_VISIBLE is set, descends only into
* child nodes when the parent node is expanded. If flags bit TVF_SELECTED
* is set, repeats until a selected treenode is found. Stops if the depth
* of the search becomes less than mindepthdoes not descend into child nodes
* when the depth would become greater than maxdepth. Returns a ptr to the
* suitable previous treenode, or NULL if none found.
*
*************************************************************************/

TNPTR TV_GetPrevTreeNode(TNPTR tn,ULONG flags,ULONG mindepth,ULONG maxdepth)
{
TNPTR	tn2;
BOOL	hidden;
BOOL	selected;

hidden = (flags & TVF_VISIBLE) ? FALSE : TRUE;
selected = (flags & TVF_SELECTED) ? TRUE : FALSE;
tn2 = NULL;

do
	{
	/* get prev node at current level */

	if (tn2 = PrevSiblingOf(tn))
		{
		tn = tn2;

		/*
		 * We got a prev node, see if it's got children and
		 * descend to the lowest (allowed) level of them if so.
		 */

		while(tn && HasChildren(tn) && (hidden || IsExpanded(tn))
				&& (TV_TreeNodeDepth(tn) < maxdepth))
			{
			tn = LastChildOf(tn);
			}
		}
	else
		{
		/* no prev at this level: ascend to parent */
		if (TV_TreeNodeDepth(tn) > mindepth)
			{
			tn = ParentOf(tn);
			/* disallow ascent to root node */
			if (!ParentOf(tn))
				tn = NULL;
			}
		else
			tn = NULL;
		}

	} while(tn && selected && !IsSelected(tn));

return(tn);
}

/************************************************************************
**************************  TV_FINDTREENODE()  **************************
*************************************************************************
* Find the treenode by entry, descending into all child nodes of the
* supplied list. Returns NULL if the entry isn't found. Matching is
* done by value, not content, ie. entries are matched for as pointers
* for equality.
*
*************************************************************************/

TNPTR TV_FindTreeNode(LISTPTR list,APTR entry)
{
TNPTR	tn;

tn = FirstChildIn(list);

while(tn && (tn->tn_Entry != entry))
	tn = TV_GetNextTreeNode(tn,TVF_ALL,0,~0);

return(tn);
}

/************************************************************************
**************************  TV_ISDISPLAYED()  ***************************
*************************************************************************
* Returns TRUE if the given treenode is displayed, ie. all it's parents
* are expanded. FALSE otherwise.
*
*************************************************************************/

BOOL TV_IsDisplayed(TNPTR tn)
{
BOOL	disp;

disp = TRUE;

while(disp && (tn = ParentOf(tn)))
	disp = IsExpanded(tn) ? TRUE : FALSE;

return(disp);
}

/************************************************************************
***********************  TV_LASTDISPLAYEDCHILD()  ***********************
*************************************************************************
* Given a parent TreeNode, find the last TreeNode among its children
* (and it's children's children, etc), which is displayed in the
* listview. Returns the parent if no children are present/displayed.
*
*************************************************************************/

TNPTR TV_LastDisplayedChild(TNPTR pn)
{
TNPTR	tn,tn2;
ULONG	depth;

if ((tn = FirstChildOf(pn)) && IsExpanded(pn))
	{
	depth = TV_TreeNodeDepth(tn);

	while(tn2 = TV_GetNextTreeNode(tn,TVF_VISIBLE,depth,~0))
		tn = tn2;
	}
else
	tn = pn;

return(tn);
}

/************************************************************************
***************************  TV_SORTEDPREV()  ***************************
*************************************************************************
* Returns the TreeNode in the supplied list that should be prior to
* the given treenode, when sorted alphabetically, or NULL if the TreeNode
* should go at the head of the list. Assumes the existing nodes are
* already in sort order. Uses the compare hook in the TreeView instance
* data to perform comparisons.
*
*************************************************************************/

TNPTR TV_SortedPrev(TVData *tv,LISTPTR list,TNPTR in)
{
struct tvCompare	tvc;
TNPTR				tn,pn;

tn = FirstChildIn(list);
pn = NULL;
tvc.tvc_EntryB = in->tn_Entry;

while(tn)
	{
	tvc.tvc_EntryA = tn->tn_Entry;

	if (((LONG) CallHookPkt(tv->tv_CompareHook,tv->tv_TreeView,&tvc)) < 0)
		{
		pn = tn;
		tn = NextSiblingOf(tn);
		}
	else
		tn = NULL;
	}

while(tn && (Stricmp(tn->tn_Entry,in->tn_Entry) < 0))
	{
	pn = tn;
	tn = NextSiblingOf(tn);
	}

return(pn);
}

/************************************************************************
*************************  TV_MATCHNEXTENTRY()  *************************
*************************************************************************
* Find the next entry in the treeview, after the supplied entry,
* matching a given specification. Supply a tvAnchor cleared with zeros
* when calling this function for the first time. Then repeatedly call
* this function with that anchor to get all matching entries. When there
* are no more matches, this function returns NULL.
*
* The match specifications are:
*
*	APTR	refentry;
*
* This is a reference entry to start matching from. It is either a ptr
* to the parent entry, or a sibling entry, of the entries to consider for
* matching. It could also be one of these special values: TV_ROOT, to
* represent the root of the tree (a dummy entry); TV_SELECTED, for the
* (first) currently selected entry. If TVF_INTERNAL is set in flags,
* then refentry must be a ptr to a TreeNode, rather than the entry in
* a TreeNode.
*
* If the reference entry is TV_SELECTED, but no entry is currently
* selected, NULL is returned.
*
*	ULONG	which;
*
* This is a code that indicates the relationship between the entries to
* be considered for matching and the reference entry. It can take these
* values:
*
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
*	TVW_TREE_FIRST			first in tree, entry ignored
*	TVW_TREE_LAST			last in tree, entry ignored
*	TVW_TREE_NEXT			next in tree from entry
*	TVW_TREE_PREV			prev. in tree from entry
*	TVW_TREE_PAGE_UP		page up in tree from entry
*	TVW_TREE_PAGE_DOWN		page down in tree from entry
*
* If the relation entry is TV_ROOT, only the TVW_CHILD_??? codes, and
* TVW_TREE_FIRST/LAST are permitted. To have all entries in the treeview
* considered, pass TV_ROOT as the reference entry and TVW_CHILD_TREE
* as the code.
*
*	ULONG	flags;
*
* These flag bits are considered:
*
*	TVF_SELECTED	consider only selected entries
*	TVF_VISIBLE		consider only visible entries
*	TVF_INTERNAL	the reference entry is a TreeNode ptr, rather
*					than a ptr to one of the user's entries.
*
* If TVF_SELECTED is used, then the first time this function is called
* the TNF_SELECTED bit of the tn_Flags member of all the TreeNodes in
* the treeview are updated, by interrogating the embedded listview.
*
* The TVF_VISIBLE flag is most useful when used with TVW_TREE_NEXT
* and TVW_TREE_PREV, to iterate through the treeview entries that are
* actually displayed in the gadget.
*
* STUB! TV_MatchNextEntry doesn't do tree page up or down yet.
*
*************************************************************************/

TNPTR TV_MatchNextEntry(TVData *tv,APTR refentry,ULONG which,ULONG flags,
	struct tvAnchor *tva)
{
TNPTR	tn;
ULONG	maxdepth,mindepth;
BOOL	checkmatch,more,next;

tn = NULL;

/*
 * If this is the first call to this function, we have some initial
 * setup work to do.
 */

if (!tva->tva_Ref)
	{
	/*
	 * If the TVF_SELECTED flag has been used, update the TNF_SELECTED
	 * flag of all TreeNodes by interrogating the listview.
	 */

	if (flags & TVF_SELECTED)
		TV_UpdateSelected(tv);

	/*
	 * Setup the anchor struct
	 */

	tva->tva_Last = NULL;
	tva->tva_Category = which & TVWC_MASK;
	tva->tva_Style = which & TVWS_MASK;
	tva->tva_Flags = flags;
	tva->tva_Visible = (flags & TVF_VISIBLE) ? TRUE : FALSE;
	tva->tva_Selected = (flags & TVF_SELECTED) ? TRUE : FALSE;
	tva->tva_Multiple = FALSE;

	if ((tva->tva_Style == TVWS_ALL) || (tva->tva_Style == TVWS_TREE))
		tva->tva_Multiple = TRUE;

	if (tva->tva_Category == TVWC_TREE)
		{
		switch(tva->tva_Style)
			{
			/*
			 * TVW_TREE_FIRST and TVW_TREE_LAST are converted here
			 * into other categories/styles, with the supplied entry
			 * ignored.
			 */

			case TVWS_FIRST:
				refentry = &tv->tv_RootNode;
				tva->tva_Category = TVWC_CHILD;
				tva->tva_Flags |= TVF_INTERNAL;
				break;

			case TVWS_LAST:
				refentry = LastChildOf(&tv->tv_RootNode);
				if (tva->tva_Visible)
					refentry = TV_LastDisplayedChild((TNPTR) refentry);
				else
					{
					while(HasChildren(refentry))
						refentry = LastChildOf(refentry);
					}
				tva->tva_Category = TVWC_ENTRY;
				tva->tva_Flags |= TVF_INTERNAL;
				break;

			default:
				break;

			} /* endswitch style */

		} /* endif TVWC_TREE */

	/*
	 * Determine what kind of reference entry has been given and obtain
	 * a ptr to its TreeNode, if necessary.
	 */

	if (refentry == TV_ROOT)
		{
		/*
		 * Reference is the dummy root node, only TVWS_CHILD_? permitted
		 */

		if (tva->tva_Category == TVWC_CHILD)
			tva->tva_Ref = &tv->tv_RootNode;
		}
	else if (refentry == TV_SELECTED)
		/* reference is the first selected entry */
		tva->tva_Ref = (TNPTR) DoMethod(tv->tv_Listview,LVM_FIRSTENTRY,NULL,LVGEF_SELECTED);
	else if (tva->tva_Flags & TVF_INTERNAL)
		/* internal reference, already a TreeNode ptr */
		tva->tva_Ref = refentry;
	else
		/* a user's entry: find its TreeNode */
		tva->tva_Ref = TV_FindTreeNode(RootList(tv),refentry);

	if (tva->tva_Ref)
		{
		tva->tva_RefDepth = TV_TreeNodeDepth(tva->tva_Ref);
		checkmatch = TRUE;
		more = TRUE;
		next = TRUE;
		mindepth = 0;
		maxdepth = ~0;

		switch(tva->tva_Category)
			{
			case TVWC_ENTRY:
				tva->tva_Last = tva->tva_Ref;
				more = FALSE;
				break;

			case TVWC_PARENT:
				tva->tva_Last = ParentOf(tva->tva_Ref);
				more = FALSE;
				break;

			case TVWC_CHILD:
				if (HasChildren(tva->tva_Ref))
					{
					switch(tva->tva_Style)
						{
						case TVWS_FIRST:	/* first child of entry */
						case TVWS_ALL:		/* all children of entry */
							tva->tva_Last = FirstChildOf(tva->tva_Ref);
							mindepth = maxdepth = tva->tva_RefDepth + 1;
							break;

						case TVWS_LAST:		/* last child of entry */
							tva->tva_Last = LastChildOf(tva->tva_Ref);
							next = FALSE;
							mindepth = maxdepth = tva->tva_RefDepth + 1;
							break;

						case TVWS_TREE:		/* all children, recursively */
							tva->tva_Last = FirstChildOf(tva->tva_Ref);
							mindepth = tva->tva_RefDepth + 1;
							break;

						default:
							break;

						} /* endswitch tva_Style */

					} /* endif tva_Ref has children */

				break; /* endcase TVWC_CHILD */

			case TVWC_SIB:
				mindepth = maxdepth = tva->tva_RefDepth;

				switch(tva->tva_Style)
					{
					case TVWS_FIRST:	/* first sibling of entry */
					case TVWS_ALL:		/* entry and all siblings */
						tva->tva_Last = FirstSiblingOf(tva->tva_Ref);
						break;

					case TVWS_LAST:		/* last sibling of entry */
						tva->tva_Last = LastSiblingOf(tva->tva_Ref);
						next = FALSE;
						break;

					case TVWS_NEXT:		/* next sibling of entry */
						checkmatch = FALSE;
						tva->tva_Last = TV_GetNextTreeNode(tva->tva_Ref,
							tva->tva_Flags,mindepth,maxdepth);
						break;

					case TVWS_PREV:		/* prev. sibling of entry */
						checkmatch = FALSE;
						tva->tva_Last = TV_GetPrevTreeNode(tva->tva_Ref,
							tva->tva_Flags,mindepth,maxdepth);
						break;

					case TVWS_TREE:		/* entry, siblings and all children recursively */
						tva->tva_Last = FirstSiblingOf(tva->tva_Ref);
						maxdepth = ~0;
						break;

					default:
						break;
					}

				break; /* endcase TVWC_SIB */

			case TVWC_TREE:
				checkmatch = FALSE;

				switch(tva->tva_Style)
					{
					/*case TVWS_FIRST:*//* first in tree, entry ignored */
					/*case TVWS_LAST:*/	/* first in tree, entry ignored */
					/* these were converted to other forms, above */

					case TVWS_NEXT:		/* next in tree from entry */
						tva->tva_Last = TV_GetNextTreeNode(tva->tva_Ref,tva->tva_Flags,0,~0);
						break;

					case TVWS_PREV:		/* prev. in tree from entry */
						tva->tva_Last = TV_GetPrevTreeNode(tva->tva_Ref,tva->tva_Flags,0,~0);
						break;

					case TVWS_PGUP:		/* page up in tree from entry */
						/* stub! */
						break;

					case TVWS_PGDN:		/* page down in tree from entry */
						/* stub! */
						break;

					default:
						break;

					} /* endswitch tva_Style */

				break; /* endcase TVWC_TREE */

			default:
				break;

			} /* endswitch tva_Category */

		if (tva->tva_Last && checkmatch)
			{
			if ((tva->tva_Visible && !TV_IsDisplayed(tva->tva_Last)) ||
				(tva->tva_Selected && !IsSelected(tva->tva_Last)))
				{
				if (more)
					{
					if (next)
						tva->tva_Last = TV_GetNextTreeNode(tva->tva_Last,
							tva->tva_Flags,mindepth,maxdepth);
					else
						tva->tva_Last = TV_GetPrevTreeNode(tva->tva_Last,
							tva->tva_Flags,mindepth,maxdepth);
					}
				else
					tva->tva_Last = NULL;
				}
			}

		} /* endif tva_Ref */

	} /* endif first time */
else
	{
	/*
	 * If this is not the first call, check that the code
	 * specifies multiple entries and that there was a
	 * previous match if so. Simply return NULL if not.
	 */

	if (tva->tva_Multiple && tva->tva_Last)
		{
		switch(tva->tva_Category | tva->tva_Style)
			{
			case TVW_CHILD_ALL:			/* all children */
				mindepth = maxdepth = tva->tva_RefDepth + 1;
				break;


			case TVW_SIBLING_ALL:		/* all siblings */
				mindepth = maxdepth = tva->tva_RefDepth;
				break;

			case TVW_CHILD_TREE:		/* all children, recursively */
				mindepth = tva->tva_RefDepth + 1;
				maxdepth = ~0;
				break;

			case TVW_SIBLING_TREE:		/* siblings and all children recursively */
				mindepth = tva->tva_RefDepth;
				maxdepth = ~0;
				break;

			default:
				tva->tva_Last = NULL;
				break;

			} /* endswitch category|style */

		if (tva->tva_Last)
			tva->tva_Last = TV_GetNextTreeNode(tva->tva_Last,
						tva->tva_Flags,mindepth,maxdepth);

		} /* endif multiple and tva_Last */
	else
		tva->tva_Last = NULL;

	} /* endelse tva_Ref */

return(tva->tva_Last);
}

/************************************************************************
*************************  TV_UPDATESELECTED()  *************************
*************************************************************************
* Update the setting of the TNF_SELECTED flag in all the treeview's
* TreeNodes by interrogating the listview. Returns the number of
* selected entries.
*
*************************************************************************/

ULONG TV_UpdateSelected(TVData *tv)
{
TNPTR	tn;
ULONG	count;

/*
 * First reset the selected bit of all treenodes.
 */

tn = FirstChildIn(RootList(tv));

while(tn)
	{
	tn->tn_Flags &= ~TNF_SELECTED;
	tn = TV_GetNextTreeNode(tn,TVF_ALL,0,~0);
	}

/*
 * Now step through every selected entry in the listview,
 * setting the selected bit.
 */

tn = (TNPTR) DoMethod(tv->tv_Listview,LVM_FIRSTENTRY,NULL,LVGEF_SELECTED);
count = 0;

while(tn)
	{
	tn->tn_Flags |= TNF_SELECTED;
	count++;
	tn = (TNPTR) DoMethod(tv->tv_Listview,LVM_NEXTENTRY,tn,LVGEF_SELECTED);
	}

return(count);
}

/************************************************************************
************************  TV_DEBUGDUMPMETHOD()  *************************
************************************************************************/

typedef struct {
	ULONG	ID;
	STRPTR	Name;
} Pair;

void TV_DebugDumpMethod(Msg msg)
{
#ifdef DUMPMETHOD

Pair	*pair;
STRPTR	method,category,style,entry,relation;
ULONG	methodid,ginfo,whichid,relationid,entryid,flags;

static Pair Methods[] = {
	TVM_INSERT, "TVM_INSERT", TVM_REMOVE, "TVM_REMOVE",
	TVM_REPLACE, "TVM_REPLACE", TVM_MOVE, "TVM_MOVE",
	TVM_GETENTRY, "TVM_GETENTRY", TVM_SELECT, "TVM_SELECT",
	TVM_VISIBLE, "TVM_VISIBLE", TVM_EXPAND, "TVM_EXPAND",
	TVM_CLEAR, "TVM_CLEAR", TVM_LOCK, "TVM_LOCK",
	TVM_UNLOCK, "TVM_UNLOCK", TVM_SORT, "TVM_SORT",
	TVM_REDRAW, "TVM_REDRAW", TVM_REFRESH, "TVM_REFRESH",
	TVM_REBUILD, "TVM_REBUILD", 0, NULL
};

static Pair Categories[] = {
	TVWC_ENTRY, "TVW_ENTRY", TVWC_PARENT, "TVW_PARENT",
	TVWC_CHILD, "TVW_CHILD", TVWC_SIB, "TVW_SIB",
	TVWC_TREE, "TVW_TREE", 0, NULL
};

static Pair Styles[] = {
	TVWS_FIRST, "_FIRST", TVWS_LAST, "_LAST", TVWS_NEXT, "_NEXT",
	TVWS_PREV, "_PREV", TVWS_PGUP, "_PGUP", TVWS_PGDN, "_PGDN",
	TVWS_SORT, "_SORT", TVWS_ALL, "_ALL", TVWS_TREE, "_TREE",
	0, NULL
};

methodid = ((struct tvEntry *) msg)->tve_MethodID;
ginfo = (ULONG) ((struct tvEntry *) msg)->tve_GInfo;

switch(methodid)
	{
	case TVM_INSERT:
	case TVM_MOVE:
		entryid = (ULONG) ((struct tvInsert *) msg)->tvi_Entry;
		relationid = (ULONG) ((struct tvInsert *) msg)->tvi_Relation;
		whichid = ((struct tvInsert *) msg)->tvi_Where;
		flags = ((struct tvInsert *) msg)->tvi_Flags;
		break;

	case TVM_REPLACE:
		relationid = (ULONG) ((struct tvReplace *) msg)->tvr_OldEntry;
		entryid = (ULONG) ((struct tvReplace *) msg)->tvr_NewEntry;
		whichid = ((struct tvReplace *) msg)->tvr_Where;
		flags = ((struct tvReplace *) msg)->tvr_Flags;
		break;

	case TVM_GETENTRY:
		relationid = ~0;
		entryid = (ULONG) ((struct tvGet *) msg)->tvg_Entry;
		whichid = ((struct tvGet *) msg)->tvg_Which;
		flags = ((struct tvGet *) msg)->tvg_Flags;
		break;

	case TVM_REMOVE:
	case TVM_SELECT:
	case TVM_VISIBLE:
	case TVM_EXPAND:
		relationid = ~0;
		entryid = (ULONG) ((struct tvEntry *) msg)->tve_Entry;
		whichid = ((struct tvEntry *) msg)->tve_Which;
		flags = ((struct tvEntry *) msg)->tve_Flags;
		break;

	default:
		entryid = relationid = whichid = ~0;
		flags = 0;
		break;
	}

pair = Methods;
while(pair->Name && (pair->ID != methodid))
	pair++;

method = pair->Name ? pair->Name : "Unknown!";

pair = Categories;
while(pair->Name && (pair->ID != (whichid & TVWC_MASK)))
	pair++;
category = pair->Name ? pair->Name : "N/A!";

pair = Styles;
while(pair->Name && (pair->ID != (whichid & TVWS_MASK)))
	pair++;
style = pair->Name ? pair->Name : "\0";

if (entryid != ~0)
	{
	if (entryid)
		{
		if (entryid == (ULONG) TV_SELECTED)
			entry = "TV_SELECTED";
		else
			entry = flags & TVF_INTERNAL ?
				(STRPTR) ((TNPTR) entryid)->tn_Node.ln_Name :
				(STRPTR) entryid;
		}
	else
		entry = "TV_ROOT";
	}
else
	entry = "N/A";

if (relationid != ~0)
	{
	if (relationid)
		{
		if (relationid == (ULONG) TV_SELECTED)
			relation = "TV_SELECTED";
		else
			relation = flags & TVF_INTERNAL ?
				(STRPTR) ((TNPTR) relationid)->tn_Node.ln_Name :
				(STRPTR) relationid;
		}
	else
		relation = "TV_ROOT";
	}
else
	relation = "N/A";

KPrintF("%s:  GI:%08lx  Rel:%s  Entry:%s   %s%s  ",
	method,ginfo,relation,entry,category,style);

if (flags & TVF_SELECTED)
	KPrintF("TVF_SELECTED ");
if (flags & TVF_SELECT)
	KPrintF("TVF_SELECT ");
if (flags & TVF_DESELECT)
	KPrintF("TVF_DESELECT ");
if (flags & TVF_MULTISELECT)
	KPrintF("TVF_MULTISELECT ");
if (flags & TVF_MAKEVISIBLE)
	KPrintF("TVF_MAKEVISIBLE ");
if (flags & TVF_EXPAND)
	KPrintF("TVF_EXPAND ");
if (flags & TVF_CONTRACT)
	KPrintF("TVF_CONTRACT ");
if (flags & TVF_TOGGLE)
	KPrintF("TVF_TOGGLE ");

KPrintF("\n");

#endif
}



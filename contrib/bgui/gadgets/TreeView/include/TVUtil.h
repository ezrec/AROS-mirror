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
 * Revision 42.0  2000/05/09 22:22:24  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:36:12  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:01:06  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1999/02/21 04:08:30  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
****************  TREEVIEW UTILITY FUNCTION PROTOTYPES  *****************
************************************************************************/

ULONG TV_DoSuperNew(Class *cl,Object *obj,Tag tag1,...);
ULONG TV_NotifyAttrChange(Object *obj,struct GadgetInfo *gi,
	ULONG flags,Tag tag1,...);

#ifdef TV_NODE_INLINE
#define TV_GetHead(list)	((list)->lh_Head->ln_Succ ? (list)->lh_Head : NULL)
#define TV_GetTail(list)	((list)->lh_TailPred->ln_Pred ? (list)->lh_TailPred : NULL)
#define TV_GetSucc(node)	((node)->ln_Succ->ln_Succ ? (node)->ln_Succ : NULL)
#define TV_GetPred(node)	((node)->ln_Pred->ln_Pred ? (node)->ln_Pred : NULL)
#else
NODEPTR TV_GetHead(LISTPTR list);
NODEPTR TV_GetTail(LISTPTR list);
NODEPTR TV_GetSucc(NODEPTR node);
NODEPTR TV_GetPred(NODEPTR node);
#endif

void *TV_AllocVec(TVData *tv,ULONG size);
void TV_FreeVec(TVData *tv,void *mem);

HOOKPTR TV_AllocHook(HOOKFUNC handler,APTR data);
void TV_FreeHook(HOOKPTR hook);

STRPTR TV_AllocStrCpy(TVData *tv,STRPTR s,ULONG extra);

TNPTR TV_AllocTreeNode(TVData *tv,APTR entry,ULONG flags);
void TV_FreeTreeNode(TVData *tv,TNPTR tn);
void TV_FreeTreeNodeList(TVData *tv,LISTPTR list);

ULONG TV_TreeNodeDepth(TNPTR tn);
TNPTR TV_IndexToTreeNode(TVData *tv,ULONG idx);
ULONG TV_TreeNodeToIndex(TVData *tv,TNPTR tn);
TNPTR TV_GetNextTreeNode(TNPTR tn,ULONG flags,ULONG mindepth,ULONG maxdepth);
TNPTR TV_GetPrevTreeNode(TNPTR tn,ULONG flags,ULONG mindepth,ULONG maxdepth);
TNPTR TV_FindTreeNode(LISTPTR list,APTR entry);
BOOL TV_IsDisplayed(TNPTR tn);
TNPTR TV_LastDisplayedChild(TNPTR par);
TNPTR TV_SortedPrev(TVData *tv,LISTPTR list,TNPTR tn);
TNPTR TV_MatchNextEntry(TVData *tv,APTR refentry,ULONG which,ULONG flags,
	struct tvAnchor *tva);
ULONG TV_UpdateSelected(TVData *tv);

void TV_DebugDumpMethod(Msg msg);


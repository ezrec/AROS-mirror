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
 * Revision 42.2  2003/01/18 19:10:21  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.1  2000/07/07 17:14:13  stegerg
 * again STACKULONG/STACKUWORD/STACK??? stuff.
 *
 * Revision 42.0  2000/05/09 22:22:26  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:36:14  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:01:08  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1999/02/21 04:08:33  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
***************************  TREEVIEW CLASS  ****************************
*************************************************************************
*
* NAME
*
* Class:		treeviewclass [private]
* SuperClass:	groupclass [BGUI, public]
*
*
* FUNCTION
*
* This is a BGUI gadget class that can display data with a hierarchical
* or tree-like structure. In general appearance, it looks like a BGUI
* listview, in fact, it currently uses a listview for basic control
* and display purposes.
*
* The name of each member of the tree is displayed appropriately indented,
* with a small box next to it containing a plus sign if the children of
* the member have been expanded, a minus sign if not. By clicking on the
* box, the user can expand or contract the child list.
*
* Like the BGUI listview, the treeview's native datatype for its
* members is a C-style string. However, hooks are provided to allow
* any type of data to be added to the tree.
*
*
* NEW METHODS
*
* 
*
*
* CHANGED METHODS
*
*
*
* ATTRIBUTES
*
* TV_??? -- (ULONG)				(ISG--)
*
*
*
* EXTERNAL REQUIREMENTS
*
* Client must open these libraries: intuition.library V37+,
* utility.library V37+ and bgui.library V40+ and provide their
* base pointers.
*
*
*************************************************************************/

/************************************************************************
*************************  PUBLIC DEFINITIONS  **************************
************************************************************************/

/*
 * Tags for class attributes
 */

#define TAG_TVA_BASE		TAG_USER+0x12340000

#define TVA_ResourceHook	(TAG_TVA_BASE+1)	/* (I----) */
#define TVA_DisplayHook		(TAG_TVA_BASE+2)	/* (I----) */
#define TVA_CompareHook		(TAG_TVA_BASE+3)	/* (I----) */
#define TVA_ExpandHook		(TAG_TVA_BASE+4)	/* (I----) */
#define TVA_ImageStyle		(TAG_TVA_BASE+5)	/* (I----) */
#define TVA_LineStyle		(TAG_TVA_BASE+6)	/* (I----) */
#define TVA_ExpandedImage	(TAG_TVA_BASE+7)	/* (I----) */
#define TVA_ContractedImage	(TAG_TVA_BASE+8)	/* (I----) */
#define TVA_LeftAlignImage	(TAG_TVA_BASE+9)	/* (I----) */
#define TVA_NoLeafImage		(TAG_TVA_BASE+10)	/* (I----) */
#define TVA_CopyEntries		(TAG_TVA_BASE+11)	/* (I----) */
#define TVA_Entry			(TAG_TVA_BASE+20)	/* (---N-) */
#define TVA_Parent			(TAG_TVA_BASE+21)	/* (---N-) */
#define TVA_Moved			(TAG_TVA_BASE+22)	/* (---N-) */
#define TVA_NumEntries		(TAG_TVA_BASE+30)	/* (--G--) */
#define TVA_LastClicked		(TAG_TVA_BASE+31)	/* (--G--) */
#define TVA_ViewBounds		(TAG_TVA_BASE+32)	/* (--G--) */
#define TVA_Indentation		(TAG_TVA_BASE+40)	/* (ISG--) */
#define TVA_Top				(TAG_TVA_BASE+41)	/* (-SG--) */

/*
 * Tags for class methods
 */

#define TAG_TVM_BASE		TAG_TVA_BASE+512

#define TVM_INSERT			(TAG_TVM_BASE+1)
#define TVM_REMOVE			(TAG_TVM_BASE+2)
#define TVM_REPLACE			(TAG_TVM_BASE+3)
#define TVM_MOVE			(TAG_TVM_BASE+4)
#define TVM_GETENTRY		(TAG_TVM_BASE+5)
#define TVM_SELECT			(TAG_TVM_BASE+6)
#define TVM_VISIBLE			(TAG_TVM_BASE+7)
#define TVM_EXPAND			(TAG_TVM_BASE+8)
#define TVM_CLEAR			(TAG_TVM_BASE+9)
#define TVM_LOCK			(TAG_TVM_BASE+10)
#define TVM_UNLOCK			(TAG_TVM_BASE+11)
#define TVM_SORT			(TAG_TVM_BASE+12)
#define TVM_REDRAW			(TAG_TVM_BASE+13)
#define TVM_REFRESH			(TAG_TVM_BASE+14)
#define TVM_REBUILD			(TAG_TVM_BASE+15)		/* debug only! */

/*
 * Values for TVA_ImageStyle:
 */

#define TVIS_BOX			0
#define TVIS_ARROW			1

/*
 * Values for TVA_LineStyle:
 */

#define TVLS_NONE			0
#define TVLS_DOTS			1
#define TVLS_DASH			2
#define TVLS_SOLID			3

/*
 * Message structure for TVM_INSERT, TVM_MOVE:
 */

#ifndef __AROS__
#undef STACKULONG
#define STACKULONG ULONG
#endif

struct tvInsert {
	STACKULONG		tvi_MethodID;	/* TVM_INSERT, TVM_MOVE */
	struct GadgetInfo	*tvi_GInfo;		/* GadgetInfo */
	APTR			tvi_Entry;		/* entry to insert or move */
	APTR			tvi_Relation;	/* parent or sibling entry */
	STACKULONG		tvi_Where;		/* see below */
	STACKULONG		tvi_Flags;		/* see below */
};

/*
 * Message structure for TVM_REPLACE
 */

struct tvReplace {
	STACKULONG				tvr_MethodID;	/* TVM_REPLACE */
	struct GadgetInfo	*tvr_GInfo;		/* GadgetInfo */
	APTR				tvr_NewEntry;	/* entry to replace with */
	APTR				tvr_OldEntry;	/* entry to replace */
	STACKULONG				tvr_Where;		/* see below */
	STACKULONG				tvr_Flags;		/* see below */
};

/*
 * Message structure for TVM_GETENTRY:
 */

struct tvGet {
	STACKULONG				tvg_MethodID;	/* TVM_GET */
	APTR				tvg_Entry;		/* parent or sibling, etc */
	STACKULONG				tvg_Which;		/* see below */
	STACKULONG				tvg_Flags;		/* see below */
};

/*
 * Message structure for TVM_REMOVE, TVM_SELECT,
 * TVM_VISIBLE, TVM_EXPAND:
 */

struct tvEntry {
	STACKULONG				tve_MethodID;	/* TVM_REMOVE, etc */
	struct GadgetInfo	*tve_GInfo;		/* GadgetInfo */
	APTR				tve_Entry;		/* entry, parent or sibling, etc */
	STACKULONG				tve_Which;		/* see below */
	STACKULONG				tve_Flags;		/* see below */
};

/*
 * Message structure for TVM_LOCK consists of only the MethodID.
 */

/*
 * Message structure for TVM_SORT, TVM_REDRAW, TVM_REFRESH,
 * TVM_CLEAR and TVM_UNLOCK:
 */

struct tvCommand {
	STACKULONG				tvc_MethodID;	/* method ID value */
	struct GadgetInfo	*tvc_GInfo;		/* GadgetInfo */
};

/*
 * Special values for any pointer to an existing entry:
 */

#define TV_ROOT			((APTR) 0L)		/* dummy root entry */
#define TV_SELECTED		((APTR) 1L)		/* first selected entry */

/*
 * For internal use only
 */

#define TVWC_ENTRY			(0L<<16)
#define TVWC_PARENT			(1L<<16)
#define TVWC_CHILD			(2L<<16)
#define TVWC_SIB			(3L<<16)
#define TVWC_TREE			(4L<<16)
#define TVWC_MASK			(7L<<16)

#define TVWS_FIRST			1
#define TVWS_LAST			2
#define TVWS_NEXT			3
#define TVWS_PREV			4
#define TVWS_PGUP			5
#define TVWS_PGDN			6
#define TVWS_SORT			7
#define TVWS_ALL			8
#define TVWS_TREE			9
#define TVWS_MASK			15

/*
 * Values for tv?_Which and tv?_Where:
 *
 * Key: I = insert, R = remove, P = replace, M = move, G = get,
 *		S = select, V = visible, E = expand
 */

#define TVW_ENTRY			(TVWC_ENTRY)			/* specified entry only (R,P,G,S,V,E) */
#define TVW_PARENT			(TVWC_PARENT)			/* parent of entry (R,P,G,S,V,E) */
#define TVW_CHILD_FIRST		(TVWC_CHILD|TVWS_FIRST)	/* first child of entry (I,R,P,M,G,S,V,E) */
#define TVW_CHILD_LAST		(TVWC_CHILD|TVWS_LAST)	/* last child of entry (I,R,P,M,G,S,V,E) */
#define TVW_CHILD_SORTED	(TVWC_CHILD|TVWS_SORT)	/* child of entry, sorted (I,P,M) */
#define TVW_CHILD_ALL		(TVWC_CHILD|TVWS_ALL)	/* all children of entry (R,P,S,E) */
#define TVW_CHILD_TREE		(TVWC_CHILD|TVWS_TREE)	/* all children, recursively (S,E) */
#define TVW_SIBLING_FIRST	(TVWC_SIB|TVWS_FIRST)	/* first sibling of entry (I,R,P,M,G,S,V,E) */
#define TVW_SIBLING_LAST	(TVWC_SIB|TVWS_LAST)	/* last sibling of entry (I,R,P,M,G,S,V,E) */
#define TVW_SIBLING_NEXT	(TVWC_SIB|TVWS_NEXT)	/* next sibling of entry (I,R,P,M,G,S,V,E) */
#define TVW_SIBLING_PREV	(TVWC_SIB|TVWS_PREV)	/* prev. sibling of entry (I,R,P,M,G,S,V,E) */
#define TVW_SIBLING_SORTED	(TVWC_SIB|TVWS_SORT)	/* sibling of entry, sorted (I,P,M) */
#define TVW_SIBLING_ALL		(TVWC_SIB|TVWS_ALL)		/* entry and all siblings (R,P,S,E) */
#define TVW_SIBLING_TREE	(TVWC_SIB|TVWS_TREE)	/* entry, siblings and all children recursively (S,E) */
#define TVW_TREE_FIRST		(TVWC_TREE|TVWS_FIRST)	/* first in tree, entry ignored (R,G,S,V,E) */
#define TVW_TREE_LAST		(TVWC_TREE|TVWS_LAST)	/* last in tree, entry ignored (R,G,S,V,E) */
#define TVW_TREE_NEXT		(TVWC_TREE|TVWS_NEXT)	/* next in tree from entry (R,G,S,V,E) */
#define TVW_TREE_PREV		(TVWC_TREE|TVWS_PREV)	/* prev. in tree from entry (R,G,S,V,E) */
#define TVW_TREE_PAGE_UP	(TVWC_TREE|TVWS_PGDN)	/* page up in tree from entry (G,S,V,E) */
#define TVW_TREE_PAGE_DOWN	(TVWC_TREE|TVWS_PGUP)	/* page down in tree from entry (G,S,V,E) */

/*
 * Bits for tv?_Flags:
 */

#define TVF_ALL				0L			/* dummy flag for all entries */
#define TVF_SELECTED		(1L<<0)		/* affect only selected entries (R,P,G,V,E) */
#define TVF_VISIBLE			(1L<<1)		/* affect only visible entries (R,P,G,E) */

#define TVF_SELECT			(1L<<8)		/* select entry (I,P,M,V,E) */
#define TVF_DESELECT		(1L<<9)		/* deselect entry (P,M,S,V,E) */
#define TVF_MULTISELECT		(1L<<10)	/* multi-select entry (with TVF_SELECT) */
#define TVF_MAKEVISIBLE		(1L<<11)	/* make entry visible (I,P,M,S,E) */
#define TVF_EXPAND			(1L<<12)	/* expand entry (I,P,M,S,V) */
#define TVF_CONTRACT		(1L<<13)	/* contract entry (P,M,S,V,E) */
#define TVF_TOGGLE			(1L<<14)	/* toggle selection/expansion (S,E) */

#define TVF_USER_ACTION		(1L<<30)	/* private */
#define TVF_INTERNAL		(1L<<31)	/* private */

/*
 * The TV_ResourceHook is called as follows:
 *
 * rc = hookFunc( REG(A0) struct Hook *hook,
 *				  REG(A2) Object *tv_object,
 *				  REG(A1) struct tvResource *message );
 *
 * On make, return a pointer to the entry to be added, or NULL.
 * On kill, return 0.
 */

struct tvResource {
	UWORD			tvr_Command;		/* whether to make or kill */
	APTR			tvr_Entry;			/* entry to be added or removed */
};

/*
 * TV_ResourceHook commands in tvr_Command
 */

#define TVRC_MAKE		1				/* Build the entry. */
#define TVRC_KILL		2				/* Kill the entry.  */

/*
 * The TV_DisplayHook is called as follows:
 *
 * rc = hookFunc( REG(A0) struct Hook *hook,
 *				  REG(A2) Object *tv_object,
 *				  REG(A1) struct tvRender *message );
 *
 * If your hook completes all rendering, return 0. If you wish
 * the treeview to render a string, return the string, which
 * may have BGUI-style formatting sequences embedded in it.
 */

struct tvRender {
	struct RastPort *tvr_RPort;			/* rastport to render in */
	struct DrawInfo *tvr_DrawInfo;		/* all you need to render */
	struct Rectangle tvr_Bounds;		/* bounds to render in */
	APTR			tvr_Entry;			/* entry to render */
	UWORD			tvr_State;			/* see below */
	UWORD			tvr_Flags;			/* see below */
};

/*
 * Rendering states in lvr_State
 */

#define TVRS_NORMAL				0		/* normal, enabled state */
#define TVRS_SELECTED			1		/* selected and enabled */
#define TVRS_NORMAL_DISABLED	2		/* normal disabled state */
#define TVRS_SELECTED_DISABLED	3		/* selected and disabled */

/*
 * Flag bits in lvr_Flags
 */

#define TVRF_EXPANDED		(1L<<0)		/* entry is expanded */

/*
 * The TV_CompareHook is called as follows:
 *
 * rc = hookFunc( REG(A0) struct Hook *hook,
 *				  REG(A2) Object *tv_object,
 *				  REG(A1) struct tvCompare *message );
 *
 * Your hook must return -1 when entry A should come before entry B, 0 when
 * entry A is equal to entry B and 1 when entry A should come after entry B.
 */

struct tvCompare {
	APTR			tvc_EntryA;			/* first entry */
	APTR			tvc_EntryB;			/* second entry */
};

/*
 * The TV_ExpandHook is called as follows:
 *
 * rc = hookFunc( REG(A0) struct Hook *hook,
 *				  REG(A2) Object *tv_object,
 *				  REG(A1) struct tvExpand *message );
 *
 * Your hook should return non-zero if the expansion or contraction
 * is to go ahead, zero if it should be cancelled.
 *
 * Important: Your hook may *not* invoke any methods on the treeview.
 */

struct tvExpand {
	UWORD			tve_Command;		/* whether to expand or contract */
	APTR			tve_Entry;			/* entry to be expanded/contracted */
	ULONG			tve_Flags;			/* see below */
};

/*
 * tvExpand commands in tve_Command
 */

#define TVEC_EXPAND			1			/* entry is to be expanded */
#define TVEC_CONTRACT		2			/* entry is to be contracted */

/*
 * Flag bits in tve_Flags
 */

#define TVEF_USER_ACTION	(1L<<0)		/* hook called due to user action */

/************************************************************************
**************************  PUBLIC PROTOTYPES  **************************
************************************************************************/

Class *MakeTreeViewClass(void);

BOOL FreeTreeViewClass(Class *cl);



#ifndef NLISTTREE_MCC_PRIVATE_H
#define NLISTTREE_MCC_PRIVATE_H

/***************************************************************************

 NListtree.mcc - New Listtree MUI Custom Class
 Copyright (C) 2001-2004 by Carsten Scholling <aphaso@aphaso.de>,
                            Sebastian Bauer <sebauer@t-online.de>,
                            Jens Langner <Jens.Langner@light-speed.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 NList classes Support Site:  http://www.sf.net/projects/nlist-classes

 $Id$

***************************************************************************/

/*** Include stuff ***/

#ifndef LIBRARIES_MUI_H
#include "libraries/mui.h"
#endif

#include "../NList_mcc/NList_mcc.h"

#include <mcc_common.h>
#include <mcc_debug.h>

/*** MUI Defines ***/

#define MUIC_NListtree	"NListtree.mcc"
#define NListtreeObject	MUI_NewObject(MUIC_NListtree

#define MUIC_NListtreeP "NListtree.mcp"	/*i*/
#define NListtreePObject MUI_NewObject(MUIC_NListtreeP	/*i*/

#define	SERNR_CASI		0x7ec8										// ***	My serial number. *ic*
#define	CLASS_TAGBASE	( TAG_USER | (SERNR_CASI<<16) | 0x1000 )	// ***	Start of class tags. *ic*
#define	MUIM_TB			( CLASS_TAGBASE + 0x0100 )					// ***	Start of method tags. *ic*
#define	MUIA_TB			( CLASS_TAGBASE + 0x0200 )					// ***	Start of attribute tags. *ic*
#define	MUICFG_TB		( CLASS_TAGBASE + 0x0000 )					// ***	Start of config value tags. *ic*

#define DEFAULT_PEN_LINES		MPEN_SHINE	/*i*/
#define DEFAULT_PEN_SHADOW		MPEN_SHADOW	/*i*/
#define DEFAULT_PEN_DRAW		"rECECECEC,E4E4E4E4,14141414"	/*i*/

/*** Attributes ***/

#define MUIA_NListtree_Active								( MUIA_TB | 0x0001 )	// *** [.SGN]
#define MUIA_NListtree_ActiveList							( MUIA_TB | 0x0002 )	// *** [..GN]
#define MUIA_NListtree_CloseHook							( MUIA_TB | 0x0003 )	// *** [IS..]
#define MUIA_NListtree_ConstructHook						( MUIA_TB | 0x0004 )	// *** [IS..]
#define MUIA_NListtree_DestructHook							( MUIA_TB | 0x0005 )	// *** [IS..]
#define MUIA_NListtree_DisplayHook							( MUIA_TB | 0x0006 )	// *** [IS..]
#define MUIA_NListtree_DoubleClick							( MUIA_TB | 0x0007 )	// *** [ISGN]
#define MUIA_NListtree_DragDropSort							( MUIA_TB | 0x0008 )	// *** [IS..]
#define MUIA_NListtree_DupNodeName							( MUIA_TB | 0x0009 )	// *** [IS..]
#define MUIA_NListtree_EmptyNodes							( MUIA_TB | 0x000a )	// *** [IS..]
#define MUIA_NListtree_Format								( MUIA_TB | 0x000b )	// *** [IS..]
#define MUIA_NListtree_OpenHook								( MUIA_TB | 0x000c )	// *** [IS..]
#define MUIA_NListtree_Quiet								( MUIA_TB | 0x000d )	// *** [.S..]
#define MUIA_NListtree_CompareHook							( MUIA_TB | 0x000e )	// *** [IS..]
#define MUIA_NListtree_Title								( MUIA_TB | 0x000f )	// *** [IS..]
#define MUIA_NListtree_TreeColumn							( MUIA_TB | 0x0010 )	// *** [ISG.]
#define MUIA_NListtree_AutoVisible							( MUIA_TB | 0x0011 )	// *** [ISG.]
#define	MUIA_NListtree_FindNameHook							( MUIA_TB | 0x0012 )	// *** [IS..]
#define MUIA_NListtree_MultiSelect							( MUIA_TB | 0x0013 )	// *** [I...]
#define MUIA_NListtree_MultiTestHook						( MUIA_TB | 0x0014 )	// *** [IS..]
#define MUIA_NListtree_CopyToClipHook						( MUIA_TB | 0x0017 )	// *** [IS..]
#define MUIA_NListtree_DropType								( MUIA_TB | 0x0018 )	// *** [..G.]
#define MUIA_NListtree_DropTarget							( MUIA_TB | 0x0019 )	// *** [..G.]
#define MUIA_NListtree_DropTargetPos						( MUIA_TB | 0x001a )	// *** [..G.]
#define	MUIA_NListtree_FindUserDataHook						( MUIA_TB | 0x001b )	// *** [IS..]
#define	MUIA_NListtree_ShowTree								( MUIA_TB | 0x001c )	// *** [ISG.]
#define	MUIA_NListtree_SelectChange							( MUIA_TB | 0x001d )	// *** [ISGN]
#define	MUIA_NListtree_NoRootTree							( MUIA_TB | 0x001e )	// *** [I...]

/*i* Private attributes & configs ***/

#define MUIA_NListtree_OnlyTrigger							( MUIA_TB | 0x0015 )	// *** $$$ Was ListtreeCompatibility before!
#define MUIA_NListtree_IsMCP								( MUIA_TB | 0x0016 )

#define	MUICFG_NListtree_ImageSpecClosed					( MUICFG_TB | 0x0001 )
#define	MUICFG_NListtree_ImageSpecOpen						( MUICFG_TB | 0x0002 )
#define	MUICFG_NListtree_ImageSpecSpecial					( MUICFG_TB | 0x0003 )

#define	MUICFG_NListtree_PenSpecLines						( MUICFG_TB | 0x0004 )
#define	MUICFG_NListtree_PenSpecShadow						( MUICFG_TB | 0x0005 )
#define	MUICFG_NListtree_PenSpecDraw						( MUICFG_TB | 0x0006 )
#define	MUICFG_NListtree_PenSpecDraw2						( MUICFG_TB | 0x000b )

#define MUICFG_NListtree_RememberStatus						( MUICFG_TB | 0x0007 )
#define MUICFG_NListtree_Space								( MUICFG_TB | 0x0008 )
#define MUICFG_NListtree_Style								( MUICFG_TB | 0x0009 )
#define MUICFG_NListtree_OpenAutoScroll						( MUICFG_TB | 0x000a )


/*** Special attribute values ***/

#define MUIV_NListtree_Active_Off							0
#define	MUIV_NListtree_Active_Parent						-2
#define	MUIV_NListtree_Active_First							-3
#define	MUIV_NListtree_Active_FirstVisible					-4
#define	MUIV_NListtree_Active_LastVisible					-5

#define MUIV_NListtree_ActiveList_Off						0

#define	MUIV_NListtree_AutoVisible_Off						0
#define	MUIV_NListtree_AutoVisible_Normal					1
#define	MUIV_NListtree_AutoVisible_FirstOpen				2
#define	MUIV_NListtree_AutoVisible_Expand					3

#define MUIV_NListtree_CompareHook_Head						0
#define MUIV_NListtree_CompareHook_Tail						-1
#define MUIV_NListtree_CompareHook_LeavesTop				-2
#define MUIV_NListtree_CompareHook_LeavesMixed				-3
#define MUIV_NListtree_CompareHook_LeavesBottom				-4

#define MUIV_NListtree_ConstructHook_String					-1
#define	MUIV_NListtree_ConstructHook_Flag_AutoCreate		(1<<15)

#define MUIV_NListtree_CopyToClipHook_Default				0

#define MUIV_NListtree_DestructHook_String					-1

#define MUIV_NListtree_DisplayHook_Default					-1

#define MUIV_NListtree_DoubleClick_Off						-1
#define MUIV_NListtree_DoubleClick_All						-2
#define MUIV_NListtree_DoubleClick_Tree						-3
#define MUIV_NListtree_DoubleClick_NoTrigger				-4

#define	MUIV_NListtree_DropType_None						0
#define	MUIV_NListtree_DropType_Above						1
#define	MUIV_NListtree_DropType_Below						2
#define	MUIV_NListtree_DropType_Onto						3
#define	MUIV_NListtree_DropType_Sorted						4

#define MUIV_NListtree_FindNameHook_CaseSensitive			0
#define MUIV_NListtree_FindNameHook_CaseInsensitive			-1
#define	MUIV_NListtree_FindNameHook_PartCaseSensitive		-2
#define MUIV_NListtree_FindNameHook_PartCaseInsensitive		-3
#define MUIV_NListtree_FindNameHook_PointerCompare			-4
#define MUIV_NListtree_FindNameHook_Part					MUIV_NListtree_FindNameHook_PartCaseSensitive /* obsolete */

#define MUIV_NListtree_FindUserDataHook_CaseSensitive		0
#define MUIV_NListtree_FindUserDataHook_CaseInsensitive		-1
#define MUIV_NListtree_FindUserDataHook_Part				-2
#define MUIV_NListtree_FindUserDataHook_PartCaseInsensitive	-3
#define MUIV_NListtree_FindUserDataHook_PointerCompare		-4

#define	MUIV_NListtree_MultiSelect_None						0
#define	MUIV_NListtree_MultiSelect_Default					1
#define	MUIV_NListtree_MultiSelect_Shifted					2
#define	MUIV_NListtree_MultiSelect_Always					3

#define	MUIV_NListtree_MultiSelect_Flag_AutoSelectChilds	(1<<31) /*i*/

#define	MUIV_NListtree_ShowTree_Toggle						-1


/*i* Private attribute & config values ***/

#define	MUICFGV_NListtree_Style_Normal						0
#define	MUICFGV_NListtree_Style_Inserted					1
#define	MUICFGV_NListtree_Style_Lines						2
#define	MUICFGV_NListtree_Style_Win98						3
#define	MUICFGV_NListtree_Style_Mac							4
#define	MUICFGV_NListtree_Style_Lines3D						5
#define	MUICFGV_NListtree_Style_Win98Plus					6
#define	MUICFGV_NListtree_Style_Glow						7

#define	MUICFGV_NListtree_Space_Default						4



/*** Structures & Flags ***/

struct MUI_NListtree_TreeNode {
	struct	MinNode	tn_Node;	// ***	To make it a node.
	STRPTR	tn_Name;			// ***	Simple name field.
	UWORD	tn_Flags;			// ***	Used for the flags below.
	APTR	tn_User;			// ***	Free for user data.
	UWORD	tn_Space;			// ***	For counting pixel and saving string space in InsertTreeImages(). *i*
	UWORD	tn_IFlags;			// ***	Internal flags *i*
	UWORD	tn_ImagePos;		// ***	Internal: Open/closed image position *i*
	struct	MUI_NListtree_TreeNode	*tn_Parent;		// ***	Parent node *i*
	struct	NListEntry *tn_NListEntry;				// ***	NList TypeEntry *i*
};


#define TNF_OPEN					(1<<0)
#define TNF_LIST					(1<<1)
#define TNF_FROZEN					(1<<2)
#define TNF_NOSIGN					(1<<3)
#define TNF_SELECTED				(1<<4)



struct MUI_NListtree_TestPos_Result {
	struct	MUI_NListtree_TreeNode *tpr_TreeNode;
	UWORD	tpr_Type;
	LONG	tpr_ListEntry;
	UWORD	tpr_ListFlags;
	WORD	tpr_Column;

};

#define	tpr_Flags tpr_Type		/* OBSOLETE */


/*i* Private Structures & Flags ***/

/*
**	NList TypeEntry for entry position
*/
struct NListEntry {
  APTR  Entry;
  BYTE  Select;
  UBYTE Wrap;
  WORD  PixLen;
  WORD  pos;
  WORD  len;
  WORD  style;
  UWORD dnum;
  ULONG entpos;
};


struct Table {
	struct	MUI_NListtree_TreeNode	**tb_Table;		// ***	Table of entries.
	LONG	tb_Entries,								// ***	Number of entries in this list.
			tb_Size,								// ***	Size of the list.
			tb_Current;								// ***	Current entry (not used everytime)
};

struct MUI_NListtree_ListNode {
	struct	MinNode	ln_Node;	// ***	To make it a node.
	STRPTR	ln_Name;			// ***	Simple name field.
	UWORD	ln_Flags;			// ***	Used for some flags.
	APTR	ln_User;			// ***	Free for user data.
	UWORD	tn_Space;			// ***	For saving string space in InertTreeImages().
	UWORD	ln_IFlags;			// ***	Internal flags.
	UWORD	ln_ImagePos;		// ***	Internal: Open/closed image position.
	struct	MUI_NListtree_TreeNode	*ln_Parent;		// ***	Parent node.
	struct	NListEntry *tn_NListEntry;				// ***	NList TypeEntry
	struct	MinList					ln_List;		// ***	List which holds all the child entries.
	struct	Table					ln_Table;		// ***	Table of entries.
};


#define	TNIF_COLLAPSED				(1<<0)	/* Entry is collapsed. */
#define	TNIF_VISIBLE				(1<<1)	/* Entry is currently visible. */
#define	TNIF_TEMPFLAG				(1<<2)	/* Multi purpose flag for signalling we have to do something with this node */
#define	TNIF_ALLOCATED				(1<<3)	/* Indicates, that memory was previsously allocated for tn_Name field. */
#define	TNIF_xxx					(1<<4)	/* */
#define	TNIF_FREE3					(1<<5)
#define	TNIF_ROOT					(1<<6)	/* This entry is the root list. */



/*** Methods ***/

#define MUIM_NListtree_Open									( MUIM_TB | 0x0001 )
#define MUIM_NListtree_Close								( MUIM_TB | 0x0002 )
#define MUIM_NListtree_Insert								( MUIM_TB | 0x0003 )
#define MUIM_NListtree_Remove								( MUIM_TB | 0x0004 )
#define MUIM_NListtree_Exchange								( MUIM_TB | 0x0005 )
#define MUIM_NListtree_Move									( MUIM_TB | 0x0006 )
#define MUIM_NListtree_Rename								( MUIM_TB | 0x0007 )
#define MUIM_NListtree_FindName								( MUIM_TB | 0x0008 )
#define MUIM_NListtree_GetEntry								( MUIM_TB | 0x0009 )
#define MUIM_NListtree_GetNr								( MUIM_TB | 0x000a )
#define MUIM_NListtree_Sort									( MUIM_TB | 0x000b )
#define MUIM_NListtree_TestPos								( MUIM_TB | 0x000c )
#define MUIM_NListtree_Redraw								( MUIM_TB | 0x000d )
#define MUIM_NListtree_NextSelected							( MUIM_TB | 0x0010 )
#define MUIM_NListtree_MultiTest							( MUIM_TB | 0x0011 )
#define MUIM_NListtree_Select								( MUIM_TB | 0x0012 )
#define MUIM_NListtree_Copy									( MUIM_TB | 0x0013 )
#define	MUIM_NListtree_InsertStruct							( MUIM_TB | 0x0014 )	// *** Insert a struct (like a path) into the list.
#define	MUIM_NListtree_Active								( MUIM_TB | 0x0015 )	// *** Method which gives the active node/number.
#define	MUIM_NListtree_DoubleClick							( MUIM_TB | 0x0016 )	// *** Occurs on every double click.
#define MUIM_NListtree_PrevSelected							( MUIM_TB | 0x0018 )	// *** Like reverse NextSelected.
#define	MUIM_NListtree_CopyToClip							( MUIM_TB | 0x0019 )	// *** Copy an entry or part to the clipboard.
#define MUIM_NListtree_FindUserData							( MUIM_TB | 0x001a )	// *** Find a node upon user data.
#define MUIM_NListtree_Clear								( MUIM_TB | 0x001b )	// *** Clear complete tree.
#define MUIM_NListtree_DropType								( MUIM_TB | 0x001e )	// ***
#define MUIM_NListtree_DropDraw								( MUIM_TB | 0x001f )	// ***


/*i* Private Methods ***/

#define MUIM_NListtree_GetListActive						( MUIM_TB | 0x000e )	// *** NEW: Internal! Get MUIA_NList_Active
#define	MUIM_NListtree_Data									( MUIM_TB | 0x000f )	// *** Get or set some internal data.
#define MUIM_NListtree_GetDoubleClick						( MUIM_TB | 0x0017 )	// *** NEW: Internal! Get MUIA_NList_DoubleClick


/*** Method structs ***/

struct MUIP_NListtree_Open {
	ULONG MethodID;
	struct MUI_NListtree_TreeNode *ListNode;
	struct MUI_NListtree_TreeNode *TreeNode;
	ULONG Flags;
};


struct MUIP_NListtree_Close {
	ULONG MethodID;
	struct MUI_NListtree_TreeNode *ListNode;
	struct MUI_NListtree_TreeNode *TreeNode;
	ULONG Flags;
};


struct MUIP_NListtree_Insert {
	ULONG	MethodID;
	STRPTR	Name;
	APTR	User;
	struct	MUI_NListtree_TreeNode *ListNode;
	struct	MUI_NListtree_TreeNode *PrevNode;
	ULONG	Flags;
};


struct MUIP_NListtree_Remove {
	ULONG MethodID;
	struct MUI_NListtree_TreeNode *ListNode;
	struct MUI_NListtree_TreeNode *TreeNode;
	ULONG Flags;
};


struct MUIP_NListtree_Clear {
	ULONG MethodID;
	struct MUI_NListtree_TreeNode *ListNode;
	ULONG Flags;
};


struct MUIP_NListtree_FindName {
	ULONG	MethodID;
	struct	MUI_NListtree_TreeNode *ListNode;
	STRPTR	Name;
	ULONG	Flags;
};


struct MUIP_NListtree_FindUserData {
	ULONG	MethodID;
	struct	MUI_NListtree_TreeNode *ListNode;
	APTR	User;
	ULONG	Flags;
};


struct MUIP_NListtree_GetEntry {
	ULONG MethodID;
	struct MUI_NListtree_TreeNode *Node;
	LONG  Position;
	ULONG Flags;
};


struct MUIP_NListtree_GetNr {
	ULONG MethodID;
	struct MUI_NListtree_TreeNode *TreeNode;
	ULONG Flags;
};


struct MUIP_NListtree_Move {
	ULONG MethodID;
	struct MUI_NListtree_TreeNode *OldListNode;
	struct MUI_NListtree_TreeNode *OldTreeNode;
	struct MUI_NListtree_TreeNode *NewListNode;
	struct MUI_NListtree_TreeNode *NewTreeNode;
	ULONG Flags;
};


struct MUIP_NListtree_Exchange {
	ULONG MethodID;
	struct MUI_NListtree_TreeNode *ListNode1;
	struct MUI_NListtree_TreeNode *TreeNode1;
	struct MUI_NListtree_TreeNode *ListNode2;
	struct MUI_NListtree_TreeNode *TreeNode2;
	ULONG Flags;
};


struct MUIP_NListtree_Rename {
	ULONG	MethodID;
	struct	MUI_NListtree_TreeNode *TreeNode;
	STRPTR	NewName;
	ULONG	Flags;
};


struct MUIP_NListtree_Sort {
	ULONG MethodID;
	struct MUI_NListtree_TreeNode *ListNode;
	ULONG Flags;
};


struct MUIP_NListtree_TestPos {
	ULONG MethodID;
	LONG  X;
	LONG  Y;
	APTR  Result;
};


struct MUIP_NListtree_Redraw {
	ULONG MethodID;
	struct MUI_NListtree_TreeNode *TreeNode;
	ULONG Flags;
};


struct MUIP_NListtree_Select {
	ULONG MethodID;
	struct MUI_NListtree_TreeNode *TreeNode;
	LONG	SelType,
			SelFlags,
			*State;
};


struct MUIP_NListtree_NextSelected {
	ULONG MethodID;
	struct MUI_NListtree_TreeNode **TreeNode;
};


struct MUIP_NListtree_MultiTest {
	ULONG MethodID;
	struct MUI_NListtree_TreeNode *TreeNode;
	LONG	SelType,
			SelFlags,
			CurrType;
};


struct MUIP_NListtree_Copy {
	ULONG MethodID;
	struct MUI_NListtree_TreeNode *SourceListNode;
	struct MUI_NListtree_TreeNode *SourceTreeNode;
	struct MUI_NListtree_TreeNode *DestListNode;
	struct MUI_NListtree_TreeNode *DestTreeNode;
	ULONG Flags;
};


struct MUIP_NListtree_InsertStruct {
	ULONG	MethodID;
	STRPTR	Name;
	APTR	User;
	STRPTR	Delimiter;
	ULONG	Flags;
};


struct MUIP_NListtree_Active {
	ULONG MethodID;
	LONG Pos;
	struct MUI_NListtree_TreeNode *ActiveNode;
};


struct MUIP_NListtree_DoubleClick {
	ULONG MethodID;
	struct MUI_NListtree_TreeNode *TreeNode;
	LONG Entry;
	LONG Column;
};


struct MUIP_NListtree_PrevSelected {
	ULONG MethodID;
	struct MUI_NListtree_TreeNode **TreeNode;
};


struct MUIP_NListtree_CopyToClip {
	ULONG MethodID;
	struct MUI_NListtree_TreeNode *TreeNode;
	LONG Pos;
	LONG Unit;
};


struct  MUIP_NListtree_DropType {
	ULONG MethodID;
	LONG *Pos;
	LONG *Type;
	LONG MinX, MaxX, MinY, MaxY;
	LONG MouseX, MouseY;
};


struct  MUIP_NListtree_DropDraw {
	ULONG MethodID;
	LONG Pos;
	LONG Type;
	LONG MinX, MaxX, MinY, MaxY;
};


/*i* Private method structs ***/

struct MUIP_NListtree_GetListActive {
	ULONG MethodID;
};


struct MUIP_NListtree_Data {
	ULONG MethodID;
	LONG ID;
	LONG Set;
};



/*** Special method values ***/

#define MUIV_NListtree_Close_ListNode_Root					0
#define MUIV_NListtree_Close_ListNode_Parent				-1
#define MUIV_NListtree_Close_ListNode_Active				-2

#define MUIV_NListtree_Close_TreeNode_Head					0
#define MUIV_NListtree_Close_TreeNode_Tail					-1
#define MUIV_NListtree_Close_TreeNode_Active				-2
#define MUIV_NListtree_Close_TreeNode_All					-3

#define MUIV_NListtree_Close_Flag_Nr						(1<<15) /*i*/
#define MUIV_NListtree_Close_Flag_Visible					(1<<14) /*i*/


#define MUIV_NListtree_Exchange_ListNode1_Root				0
#define MUIV_NListtree_Exchange_ListNode1_Active			-2

#define MUIV_NListtree_Exchange_TreeNode1_Head				0
#define MUIV_NListtree_Exchange_TreeNode1_Tail				-1
#define MUIV_NListtree_Exchange_TreeNode1_Active			-2

#define MUIV_NListtree_Exchange_ListNode2_Root				0
#define MUIV_NListtree_Exchange_ListNode2_Active			-2

#define MUIV_NListtree_Exchange_TreeNode2_Head				0
#define MUIV_NListtree_Exchange_TreeNode2_Tail				-1
#define MUIV_NListtree_Exchange_TreeNode2_Active			-2
#define MUIV_NListtree_Exchange_TreeNode2_Up				-5
#define MUIV_NListtree_Exchange_TreeNode2_Down				-6


#define MUIV_NListtree_FindName_ListNode_Root				0
#define MUIV_NListtree_FindName_ListNode_Active				-2

#define MUIV_NListtree_FindName_Flag_SameLevel				(1<<15)
#define MUIV_NListtree_FindName_Flag_Visible				(1<<14)
#define MUIV_NListtree_FindName_Flag_Activate				(1<<13)
#define MUIV_NListtree_FindName_Flag_FindPart				(1<<12) /*i*/
#define MUIV_NListtree_FindName_Flag_Selected				(1<<11)
#define	MUIV_NListtree_FindName_Flag_StartNode				(1<<10)
#define	MUIV_NListtree_FindName_Flag_Reverse				(1<<9)


#define MUIV_NListtree_FindUserData_ListNode_Root			0
#define MUIV_NListtree_FindUserData_ListNode_Active			-2

#define MUIV_NListtree_FindUserData_Flag_SameLevel			(1<<15)
#define MUIV_NListtree_FindUserData_Flag_Visible			(1<<14)
#define MUIV_NListtree_FindUserData_Flag_Activate			(1<<13)
#define MUIV_NListtree_FindUserData_Flag_FindPart			(1<<12) /*i*/
#define MUIV_NListtree_FindUserData_Flag_Selected			(1<<11)
#define	MUIV_NListtree_FindUserData_Flag_StartNode			(1<<10)
#define	MUIV_NListtree_FindUserData_Flag_Reverse			(1<<9)


#define MUIV_NListtree_GetEntry_ListNode_Root				0
#define MUIV_NListtree_GetEntry_ListNode_Active				-2
#define	MUIV_NListtree_GetEntry_TreeNode_Active				-3

#define MUIV_NListtree_GetEntry_Position_Head				0
#define MUIV_NListtree_GetEntry_Position_Tail				-1
#define MUIV_NListtree_GetEntry_Position_Active				-2
#define MUIV_NListtree_GetEntry_Position_Next				-3
#define MUIV_NListtree_GetEntry_Position_Previous			-4
#define MUIV_NListtree_GetEntry_Position_Parent				-5
#define MUIV_NListtree_GetEntry_Position_Root				-15 /*i*/

#define MUIV_NListtree_GetEntry_Flag_SameLevel				(1<<15)
#define MUIV_NListtree_GetEntry_Flag_Visible				(1<<14)


#define MUIV_NListtree_GetNr_TreeNode_Root					0
#define MUIV_NListtree_GetNr_TreeNode_Active				-2

#define MUIV_NListtree_GetNr_Flag_CountAll					(1<<15)
#define MUIV_NListtree_GetNr_Flag_CountLevel				(1<<14)
#define MUIV_NListtree_GetNr_Flag_CountList					(1<<13)
#define MUIV_NListtree_GetNr_Flag_ListEmpty					(1<<12)
#define	MUIV_NListtree_GetNr_Flag_Visible					(1<<11)


#define MUIV_NListtree_Insert_ListNode_Root					0
#define MUIV_NListtree_Insert_ListNode_Active				-2
#define	MUIV_NListtree_Insert_ListNode_LastInserted			-3
#define MUIV_NListtree_Insert_ListNode_ActiveFallback		-4

#define MUIV_NListtree_Insert_PrevNode_Head					0
#define MUIV_NListtree_Insert_PrevNode_Tail					-1
#define MUIV_NListtree_Insert_PrevNode_Active				-2
#define MUIV_NListtree_Insert_PrevNode_Sorted				-4

#define MUIV_NListtree_Insert_Flag_Nr						(1<<15) /*i*/
#define MUIV_NListtree_Insert_Flag_Visible					(1<<14) /*i*/
#define MUIV_NListtree_Insert_Flag_Active					(1<<13)
#define MUIV_NListtree_Insert_Flag_NextNode					(1<<12)


#define MUIV_NListtree_Move_OldListNode_Root				0
#define MUIV_NListtree_Move_OldListNode_Active				-2

#define MUIV_NListtree_Move_OldTreeNode_Head				0
#define MUIV_NListtree_Move_OldTreeNode_Tail				-1
#define MUIV_NListtree_Move_OldTreeNode_Active				-2

#define MUIV_NListtree_Move_NewListNode_Root				0
#define MUIV_NListtree_Move_NewListNode_Active				-2

#define MUIV_NListtree_Move_NewTreeNode_Head				0
#define MUIV_NListtree_Move_NewTreeNode_Tail				-1
#define MUIV_NListtree_Move_NewTreeNode_Active				-2
#define MUIV_NListtree_Move_NewTreeNode_Sorted				-4

#define MUIV_NListtree_Move_Flag_Nr							(1<<15) /*i*/
#define MUIV_NListtree_Move_Flag_Visible					(1<<14) /*i*/
#define MUIV_NListtree_Move_Flag_KeepStructure				(1<<13)


#define MUIV_NListtree_Open_ListNode_Root					0
#define MUIV_NListtree_Open_ListNode_Parent					-1
#define MUIV_NListtree_Open_ListNode_Active					-2
#define MUIV_NListtree_Open_TreeNode_Head					0
#define MUIV_NListtree_Open_TreeNode_Tail					-1
#define MUIV_NListtree_Open_TreeNode_Active					-2
#define MUIV_NListtree_Open_TreeNode_All					-3

#define MUIV_NListtree_Open_Flag_Nr							(1<<15) /*i*/
#define MUIV_NListtree_Open_Flag_Visible					(1<<14) /*i*/


#define MUIV_NListtree_Remove_ListNode_Root					0
#define MUIV_NListtree_Remove_ListNode_Active				-2
#define MUIV_NListtree_Remove_TreeNode_Head					0
#define MUIV_NListtree_Remove_TreeNode_Tail					-1
#define MUIV_NListtree_Remove_TreeNode_Active				-2
#define MUIV_NListtree_Remove_TreeNode_All					-3
#define MUIV_NListtree_Remove_TreeNode_Selected				-4

#define MUIV_NListtree_Remove_Flag_Nr						(1<<15) /*i*/
#define MUIV_NListtree_Remove_Flag_Visible					(1<<14) /*i*/
#define MUIV_NListtree_Remove_Flag_NoActive					(1<<13)


#define MUIV_NListtree_Clear_TreeNode_Root					0 /*i*/
#define MUIV_NListtree_Clear_TreeNode_Active				-2 /*i*/


#define MUIV_NListtree_Rename_TreeNode_Active				-2

#define MUIV_NListtree_Rename_Flag_User						(1<<8)
#define MUIV_NListtree_Rename_Flag_NoRefresh				(1<<9)


#define MUIV_NListtree_Sort_ListNode_Root					0
#define MUIV_NListtree_Sort_ListNode_Active					-2
#define	MUIV_NListtree_Sort_TreeNode_Active					-3

#define MUIV_NListtree_Sort_Flag_Nr							(1<<15) /*i*/
#define MUIV_NListtree_Sort_Flag_Visible					(1<<14) /*i*/
#define MUIV_NListtree_Sort_Flag_RecursiveOpen				(1<<13)
#define MUIV_NListtree_Sort_Flag_RecursiveAll				(1<<12)


#define MUIV_NListtree_TestPos_Result_None					0
#define MUIV_NListtree_TestPos_Result_Above					1
#define MUIV_NListtree_TestPos_Result_Below					2
#define MUIV_NListtree_TestPos_Result_Onto					3
#define MUIV_NListtree_TestPos_Result_Sorted				4

#define	MUIV_NListtree_Redraw_Active						-1
#define	MUIV_NListtree_Redraw_All							-2

#define MUIV_NListtree_Redraw_Flag_Nr						(1<<15)

#define	MUIV_NListtree_Select_Active						-1
#define	MUIV_NListtree_Select_All							-2
#define	MUIV_NListtree_Select_Visible						-3

#define	MUIV_NListtree_Select_Off							0
#define	MUIV_NListtree_Select_On							1
#define	MUIV_NListtree_Select_Toggle						2
#define	MUIV_NListtree_Select_Ask							3

#define	MUIV_NListtree_Select_Flag_Force					(1<<15)


#define	MUIV_NListtree_NextSelected_Start					-1
#define	MUIV_NListtree_NextSelected_End						-1


#define MUIV_NListtree_Copy_SourceListNode_Root				0
#define MUIV_NListtree_Copy_SourceListNode_Active			-2

#define MUIV_NListtree_Copy_SourceTreeNode_Head				0
#define MUIV_NListtree_Copy_SourceTreeNode_Tail				-1
#define MUIV_NListtree_Copy_SourceTreeNode_Active			-2

#define MUIV_NListtree_Copy_DestListNode_Root				0
#define MUIV_NListtree_Copy_DestListNode_Active				-2

#define MUIV_NListtree_Copy_DestTreeNode_Head				0
#define MUIV_NListtree_Copy_DestTreeNode_Tail				-1
#define MUIV_NListtree_Copy_DestTreeNode_Active				-2
#define MUIV_NListtree_Copy_DestTreeNode_Sorted				-4

#define MUIV_NListtree_Copy_Flag_Nr							(1<<15) /*i*/
#define MUIV_NListtree_Copy_Flag_Visible					(1<<14) /*i*/
#define MUIV_NListtree_Copy_Flag_KeepStructure				(1<<13)


#define	MUIV_NListtree_InsertStruct_Flag_AllowDuplicates	(1<<11)


#define	MUIV_NListtree_PrevSelected_Start					-1
#define	MUIV_NListtree_PrevSelected_End						-1


#define MUIV_NListtree_CopyToClip_Active					-1


/*i* Private method values ***/

#define	MUIV_NListtree_Data_MemPool							1
#define	MUIV_NListtree_Data_VersInfo						2
#define	MUIV_NListtree_Data_Sample							3



/*** Hook message structs ***/

struct MUIP_NListtree_CloseMessage
{
	ULONG HookID;
	struct MUI_NListtree_TreeNode *TreeNode;
};


struct MUIP_NListtree_CompareMessage
{
	ULONG HookID;
	struct MUI_NListtree_TreeNode *TreeNode1;
	struct MUI_NListtree_TreeNode *TreeNode2;
	LONG SortType;
};


struct MUIP_NListtree_ConstructMessage
{
	ULONG HookID;
	STRPTR Name;
	APTR UserData;
	APTR MemPool;
	ULONG Flags;
};


struct MUIP_NListtree_DestructMessage
{
	ULONG HookID;
	STRPTR Name;
	APTR UserData;
	APTR MemPool;
	ULONG Flags;
};


struct MUIP_NListtree_DisplayMessage
{
	ULONG	HookID;
	struct	MUI_NListtree_TreeNode *TreeNode;
	LONG	EntryPos;
	STRPTR	*Array;
	STRPTR	*Preparse;
};


struct MUIP_NListtree_CopyToClipMessage
{
	ULONG	HookID;
	struct	MUI_NListtree_TreeNode *TreeNode;
	LONG	Pos;
	LONG	Unit;
};


struct MUIP_NListtree_FindNameMessage
{
	ULONG	HookID;
	STRPTR	Name;
	STRPTR	NodeName;
	APTR	UserData;
	ULONG	Flags;
};


struct MUIP_NListtree_FindUserDataMessage
{
	ULONG	HookID;
	APTR	User;
	APTR	UserData;
	STRPTR	NodeName;
	ULONG	Flags;
};


struct MUIP_NListtree_OpenMessage
{
	ULONG HookID;
	struct MUI_NListtree_TreeNode *TreeNode;
};


struct MUIP_NListtree_MultiTestMessage
{
	ULONG HookID;
	struct MUI_NListtree_TreeNode *TreeNode;
	LONG	SelType,
			SelFlags,
			CurrType;
};

#endif /* NLISTTREE_MCC_PRIVATE_H */

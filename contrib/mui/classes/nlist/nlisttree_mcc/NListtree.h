#ifndef NLISTTREE_H
#define	NLISTTREE_H

/***************************************************************************

 NListtree.mcc - New Listtree MUI Custom Class
 Copyright (C) 1999-2004 by Carsten Scholling <aphaso@aphaso.de>,
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

#include <clib/macros.h>

#include <datatypes/pictureclass.h>
#include <graphics/gfxmacros.h>
#include <libraries/gadtools.h>
#include <libraries/mui.h>
#include <devices/clipboard.h>
#include <devices/timer.h>
#include <utility/utility.h>
#include <utility/tagitem.h>
#include <utility/hooks.h>
#include <exec/libraries.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/types.h>
#include <exec/io.h>

#include <string.h>

#if defined(__PPC__)
 #pragma pack(2)
 #include "private.h"
 #pragma pack()
#else
 #include "private.h"
#endif

#include <MUI/NList_mcc.h>

#define MUIA_TI_Spec		0xfedc
#define	MUIA_TI_MaxWidth	0xfedb
#define	MUIA_TI_MaxHeight	0xfeda
#define	MUIA_TI_Style		0xfed9
#define	MUIA_TI_Space		0xfed8
#define	MUIA_TI_Pen			0xfed7

struct MUI_ImageSpec
{
  char buf[64];
};

//extern struct Library *MUIMasterBase;

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

struct NListtree_HookMessage
{
	ULONG HookID;
};


struct MyImage
{
	Object *Image;      // ***	Image objects.
	Object *ListImage;	// ***	List image objects (created by ...CreateImage).

	struct NListtree_Data	*nltdata;
};


struct MyContextMenuChoice
{
	LONG unit;
  LONG pos;
	LONG col;
	LONG ontop;
};

struct NListtree_Data
{
	/*
	**	General object related vars.
	*/
	APTR   MemoryPool;                  // ***	Our global memory pool
	APTR   TreePool;		                // ***	Our tree memory pool
	Object *Obj;				                // ***	The listtree object itself
	STRPTR buf;					                // ***	Multi purpose buffer (most image include)
	struct MUI_EventHandlerNode EHNode;	// ***	Event handler node.


	/*
	**	Image custom class stuff.
	*/
	struct MUI_CustomClass *CL_TreeImage;
	struct MUI_CustomClass *CL_NodeImage;


	/*
	**	List handling.
	*/
	struct MUI_NListtree_ListNode RootList;           // ***	The head root list.
	struct MUI_NListtree_ListNode *ActiveList;        // ***	The active list node
	struct MUI_NListtree_ListNode *LastInsertedList;  // ***	The list node, the last entry was inserted

	struct MUI_NListtree_TreeNode	*ActiveNode;			  // ***	Current active tree node
	struct MUI_NListtree_TreeNode	*OpenCloseEntry;		// ***	Entry to open/close in handler.
  struct MUI_NListtree_TreeNode	*TempActiveNode;		// ***	New active node after Remove for example.

	struct Table SelectedTable;			                  // ***	Table of selected entries.
	struct MUI_NListtree_TreeNode *TempSelected;		  // ***	Temporary selected entry.

	ULONG ActiveNodeNum;			                        // ***	Number of the active node.


	/*
	**	Rendering information, images etc.
	*/
	struct	MUI_RenderInfo			*MRI;					// ***	MUI render info structure (got in _Setup())

	struct	MyImage					Image[4];				// ***	My special image structure.

	UWORD							MaxImageWidth,			// ***	Maximum image width.
									MaxImageHeight;			// ***	Maximum image height.

	/*
	**	Pen specific stuff.
	*/
	LONG							Pen[4];					// ***	Obtained pen for lines/shadow/draw/draw2 drawing.


	/*
	**	Other configuration stuff.
	*/
	UBYTE							Style,					// ***	The style the list tree should appear
									Space;					// ***	Number of space pixels between image and entry

	UBYTE							MultiSelect;			// ***	Multi selection kind and flags.
	UWORD							MultiSelectFlags;

	struct MyContextMenuChoice		MenuChoice;

	/*
	**	All hooks used.
	*/
	struct	Hook					*IntDisplayHook;		// *** My own internal display hook.

	struct	Hook					*OpenHook,				// ***	Safe place for all hooks the user wants to be called
									*CloseHook,

									*ConstructHook,
									*DestructHook,

									*MultiTestHook,

									*DisplayHook,
									*CopyToClipHook,

									*CompareHook,
									*FindNameHook,
									*FindUserDataHook;

	struct Hook IntCompareHook; /* The internal compare hook, is no pointer to avoid unnecessary allocations */


	/*
	**	List format save.
	*/
	STRPTR							Format;					// ***	Here the list format is saved


	/*
	**	Input handling stuff (double click etc.)
	*/
	BYTE							DoubleClick;			// ***	Holds specified double click value
	UWORD							TreeColumn;				// ***	Holds specified tree column

	struct	timeval					LDClickTime;			// ***	Left mouse button click handling
	LONG							LDClickEntry;
	WORD							LDClickColumn;

	struct	timeval					MDClickTime;			// ***	Middle mouse button click handling
	LONG							MDClickEntry;
	WORD							MDClickColumn;


	/*
	**	Auto visualization stuff
	*/
	UBYTE							AutoVisible;			// ***	Holds information about autovisiblization


	/*
	**	Drag'n Drop stuff.
	*/
	struct	MUI_NListtree_TreeNode  *DropTarget;			// ***	The entry which is currently the drop target.

	ULONG							DropTargetPos;			// ***	Position of target drop entry.
	ULONG							DropType;				// ***	The dropping type (above, below, onto...)


	/*
	**	Number of entries total.
	*/
	ULONG							NumEntries;				// ***	Global number of entries in the whole list tree


	/*
	**	Some flags and error var.
	*/
	ULONG							Flags;					// ***	Some flags (see below)
	ULONG							QuietCounter;			// ***	Quiet nesting count.
	ULONG							ForceActiveNotify; // *** next MUIA_NListtree_Notify notify will be enforced
	ULONG							IgnoreSelectionChange; /* *** Ignores the next selection change */
	UWORD							Error;
};


/*
**	Flag values for Flags field
*/
#define	NLTF_EMPTYNODES					(1<<0)		// ***	Display empty nodes as leafs.
#define	NLTF_DUPNODENAMES				(1<<1)		// ***	Do not copy "name"-field. Use supplied pointer.
#define	NLTF_QUIET						(1<<2)		// ***	NListtree is in quiet state.
#define	NLTF_TITLE						(1<<3)		// ***	Show title.
#define	NLTF_REFRESH					(1<<4)		// ***	Do an general object refresh.
#define	NLTF_REMEMBER_STATUS			(1<<5)		// ***	Config: Remember open/close status of nodes.
#define	NLTF_DRAGDROPSORT				(1<<6)		// ***	Enabled drag'n drop sort.
#define	NLTF_REDRAW						(1<<7)		// ***	Redraw specified line.
#define	NLTF_AUTOVISIBLE				(1<<8)		// ***	Activated entries will always be shown.
#define	NLTF_SELECT_METHOD				(1<<9)		// ***	Selection through Select method.
#define	NLTF_OVER_ARROW					(1<<10)		// ***	Mouse clicked over arrow. No drag!!
#define	NLTF_DRAGDROP					(1<<11)		// ***	Drag&Drop in in progress.
#define	NLTF_OPENAUTOSCROLL				(1<<12)		// ***	Config: Auto scroll entries when opening a node with a large list.
#define	NLTF_NO_TREE					(1<<13)		// ***	Do not show a tree.
#define	NLTF_NLIST_NO_SCM_SUPPORT		(1<<14)		// ***	NList.mcc has no SelectChange method support implemented!
#define	NLTF_INT_COMPAREHOOK			(1<<15)		// ***	Internal compare hook used!
#define	NLTF_INT_CONSTRDESTRHOOK		(1<<16)		// ***	Internal construct/destruct hooks!
#define	NLTF_ACTIVENOTIFY				(1<<17)		// ***	Active notify activated?
#define	NLTF_ISMCP						(1<<18)		// ***	MCP is here ;-)
#define	NLTF_NLIST_DIRECT_ENTRY_SUPPORT	(1<<19)		// ***	NList version supports direct entry request via backpointer
#define	NLTF_NO_ROOT_TREE				(1<<20)		// ***	Do not display root tree gfx.
#define	NLTF_SETACTIVE					(1<<21)		// ***	Set the active entry.
#define	NLTF_AUTOSELECT_CHILDS			(1<<22)		// ***	Automatically select childs if their parents selected.
#define	NLTF_GET_PARENT_ATTR			(1<<23)		// ***	Do not respond to attr queries.

#define	NLTF_QUALIFIER_LCOMMAND			(1<<28)		// ***	Rawkey qualifier information.
#define	NLTF_QUALIFIER_LALT				(1<<29)
#define	NLTF_QUALIFIER_CONTROL			(1<<30)
#define	NLTF_QUALIFIER_LSHIFT			(1<<31)


#define	INSERT_POS_HEAD				-1
#define	INSERT_POS_TAIL				-2


#define	IMAGE_Closed				0
#define	IMAGE_Open					1
#define	IMAGE_Special				2
#define	IMAGE_Tree					3

#define	SPEC_Vert					1
#define	SPEC_VertT					2
#define	SPEC_VertEnd				3
#define	SPEC_Space					4
#define	SPEC_Hor					5

#define	PEN_Shadow					0
#define	PEN_Line					1
#define	PEN_Draw					2
#define	PEN_Draw2					3


// Prototypes for help functions
LONG xget( Object *obj, ULONG attribute );

#endif /* NLISTTREE_H */


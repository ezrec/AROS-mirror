/*
 * @(#) $Header$
 *
 * BGUI library
 * listclass.c
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.4  2004/06/16 20:16:48  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.3  2000/08/17 15:09:18  chodorowski
 * Fixed compiler warnings.
 *
 * Revision 42.2  2000/05/15 19:27:01  stegerg
 * another hundreds of REG() macro replacements in func headers/protos.
 *
 * Revision 42.1  2000/05/14 23:32:47  stegerg
 * changed over 200 function headers which all use register
 * parameters (oh boy ...), because the simple REG() macro
 * doesn't work with AROS. And there are still hundreds
 * of headers left to be fixed :(
 *
 * Many of these functions would also work with stack
 * params, but since i have fixed every single one
 * I encountered up to now, I guess will have to do
 * the same for the rest.
 *
 * Revision 42.0  2000/05/09 22:09:28  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 19:54:37  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.17  1999/08/08 23:05:11  mlemos
 * Assured that the list is redrawn when the list font or the default font
 * changes.
 * Made the entry height be always recomputed every time the list is rendered.
 *
 * Revision 41.10.2.16  1999/07/28 06:34:32  mlemos
 * Added rendering damage list after the ScrollRaster() call.
 *
 * Revision 41.10.2.15  1999/07/18 06:15:42  mlemos
 * Fixed the problem of very quick drop without dragging.
 *
 * Revision 41.10.2.14  1999/07/03 15:17:40  mlemos
 * Replaced the calls to CallHookPkt to BGUI_CallHookPkt.
 *
 * Revision 41.10.2.13  1999/05/24 20:20:43  mlemos
 * Added fix for attempt to free BaseInfo handle twice when dragging 10 or
 * more entries.
 *
 * Revision 41.10.2.12  1998/12/20 01:37:42  mlemos
 * Ensured that the list is fully redrawn when a change like adding a new
 * entry that makes the list partially invisible, makes the list overhead
 * increase.
 * Fixed computation of the top overhead area of a list view with title to
 * include the height of the separator line (2 pixels).
 *
 * Revision 41.10.2.11  1998/12/07 03:07:03  mlemos
 * Replaced OpenFont and CloseFont calls by the respective BGUI debug macros.
 *
 * Revision 41.10.2.10  1998/12/07 00:49:52  mlemos
 * Fixed range checking bug of new list top entry when the number of visible
 * entries is larger than the total entries.
 *
 * Revision 41.10.2.9  1998/12/06 21:38:21  mlemos
 * Ensured that when the parent view and window are passed to the listview
 * scroller gadget on its creation or when are set by OM_SET OM_UPDATE.
 *
 * Revision 41.10.2.8  1998/12/06 21:20:41  mlemos
 * Ensured that the key activation selects the previous or the next selected
 * entries in Multi Selection lists.
 * Ensured that selecting the previous or the next selected entries in Multi
 * Selection lists always deselects any previously selected entries.
 *
 * Revision 41.10.2.7  1998/11/28 13:45:17  mlemos
 * Made the default flag of the list to pre clear the entries.
 *
 * Revision 41.10.2.6  1998/11/12 16:23:33  mlemos
 * Passed NULL rastport pointer to AllocBaseInfo in the methods
 * BASE_DRAGACTIVE, BASE_DRAGINACTIVE and BASE_DRAGUPDATE to ensure that it
 * legally obtains the Rastport.
 *
 * Revision 41.10.2.5  1998/10/11 15:28:58  mlemos
 * Enforced the columns minimum width value.
 *
 * Revision 41.10.2.4  1998/10/11 14:50:21  mlemos
 * Fixed the computation of the initial offset of the column bars when the
 * user starts to drag them.
 *
 * Revision 41.10.2.3  1998/06/16 01:22:10  mlemos
 * Passed NULL rastport pointer to AllocBaseInfo in DrawDragLine to ensure
 * that it legally obtains the Rastport.
 *
 * Revision 41.10.2.2  1998/06/15 21:48:43  mlemos
 * Passed NULL rastport pointer to AllocBaseInfo in NewTop to ensure that it
 * legally obtains the Rastport.
 *
 * Revision 41.10.2.1  1998/03/01 15:38:05  mlemos
 * Added support to track BaseInfo memory leaks.
 *
 * Revision 41.10  1998/02/25 21:12:31  mlemos
 * Bumping to 41.10
 *
 * Revision 1.1  1998/02/25 17:09:00  mlemos
 * Ian sources
 *
 *
 */
 
/*
 * Incorporates MultiColumnListview by Nick Christie.
 * New code, but the interface is the same.
 */

#include "include/classdefs.h"

/*
 * Internal entry storage.
 */
typedef struct lvEntry {
   struct lvEntry       *lve_Next;     /* Next entry.    */
   struct lvEntry       *lve_Prev;     /* Previous entry. */
   ULONG                 lve_Flags;    /* See below.     */
   APTR                  lve_Entry;    /* Entry data.    */
}  LVE;

#define LVEF_SELECTED      (1<<0)      /* Entry is selected.      */
#define LVEF_DISABLED      (1<<1)      /* Entry is disabled.      */
#define LVEF_HIDDEN        (1<<2)      /* Entry is hidden.        */
#define LVEF_REFRESH       (1<<31)     /* Entry must be redrawn.  */

typedef struct {
   LVE            *lvl_First;    /* First entry. */
   LVE            *lvl_EndMarker;         /* End marker. */
   LVE            *lvl_Last;     /* Last entry. */
}  LVL;

typedef struct cd_ {
   UWORD              cd_Offset;       /* Left offset of column.     */
   UWORD              cd_Width;        /* Width of column.           */
   UWORD              cd_MinWidth;     /* Minimum column width.      */
   UWORD              cd_MaxWidth;     /* Maximum column width.      */
   UWORD              cd_Weight;       /* Relative Weight of column. */
   ULONG              cd_Flags;        /* Column Flags.              */
}  CD;

#define LVCF_NOSEPARATOR   (1<<0)      /* No column separator.       */
#define LVCF_HIDDEN        (1<<1)      /* Column is not visible.     */
#define LVCF_DRAGGABLE     (1<<2)      /* Column is draggable.       */
#define LVCF_PRECLEAR      (1<<3)      /* Column is pre-cleared.     */

/*
 * Object instance data.
 */
typedef struct ld_ {
   ULONG              ld_Flags;        /* See below.                 */
   Object            *ld_Prop;         /* Scroller.                  */
   BC                *ld_BC;           /* Bounding boxes.            */
   struct IBox        ld_ListArea;     /* Area bounds.               */
   LVL                ld_Entries;      /* List of entries.           */
// LVL                ld_Hidden;       /* Entries filtered out.      */
   LVE               *ld_TopEntry;     /* Top entry.                 */
   ULONG              ld_ActiveEntry;  /* Active entry number.       */
   LVE               *ld_LastActive;   /* Last active entry.         */
   ULONG              ld_LastNum;      /* Last active entry number.  */
   UWORD              ld_LastCol;      /* Last active column.        */
   LVE               *ld_LastAdded;    /* Last added entry.          */
   
   UWORD              ld_EntryHeight;  /* Height of a single entry.  */
   UWORD              ld_Overhead;     /* View area overhead.        */
   ULONG              ld_Top;          /* Top entry visible.         */
   ULONG              ld_Total;        /* Number of entries.         */
   LONG               ld_Visible;      /* Number of visible entries. */
   struct Hook       *ld_Resource;     /* Resource hook.             */
   struct Hook       *ld_Display;      /* Display hook.              */
   struct Hook       *ld_Compare;      /* Comparisson hook.          */
   struct Hook       *ld_TitleHook;    /* Title hook.                */
   UBYTE             *ld_Title;        /* Title.                     */
// struct Hook       *ld_Filter;       /* Filter hook.               */
   struct TextAttr   *ld_ListFont;     /* List font.                 */
   struct TextFont   *ld_Font;         /* List font.                 */
   APTR               ld_MemoryPool;   /* OS 3.0 memory pool.        */
   ULONG              ld_MultiStart;   /* Start of multi-(de)select. */
   UWORD              ld_MultiMode;    /* Mode of multi-(de)select.  */
   UWORD              ld_MinShown;     /* Min # lines shown.         */
   ULONG              ld_Secs[2];      /* Double-clicks.             */
   ULONG              ld_Mics[2];
   ULONG              ld_NewPos;       /* New entry position.        */
   APTR               ld_ScanEntry;    /* Last scanned entry.        */
   LVE               *ld_ScanNode;     /* And it's node.             */
   ULONG              ld_DropSpot;     /* Place where dropped.       */
   ULONG              ld_DrawSpot;     /* Place where line drawn.    */
   struct RastPort   *ld_DragRP;       /* Draggable bitmap+rport.    */
   struct RastPort   *ld_LineBuffer;   /* Place for drop line.       */
   
   UWORD              ld_Columns;      /* number of columns, > 0           */
   UWORD              ld_OneColumn;    /* Render only one column.          */
   UWORD              ld_DragColumn;   /* column # being dragged, 0...n    */
   WORD               ld_DragXLine;    /* current x position of drag line  */
   CD                *ld_CD;           /* Column Descriptors.              */

}  LD;

#define ld_InnerBox ld_BC->bc_InnerBox
#define ld_OuterBox ld_BC->bc_OuterBox
#define ld_HitBox   ld_BC->bc_HitBox
#define ld_Frame    ld_BC->bc_Frame

#define LDF_READ_ONLY         (1 << 0 )   /* Read-only list.               */
#define LDF_MULTI_SELECT      (1 << 1 )   /* Multi-select list.            */
#define LDF_REFRESH_ALL       (1 << 2 )   /* Refresh whole list.           */
#define LDF_PROPACTIVE        (1 << 4 )   /* Prop gadget is active.        */
#define LDF_LIST_BUSY         (1 << 5 )   /* We are busy with the list.    */
#define LDF_NOSHIFT           (1 << 6 )   /* Multi-select without shift    */
#define LDF_CUSTOMDISABLE     (1 << 7 )   /* Display hook renders disabled state.   */
#define LDF_SHOWDROPSPOT      (1 << 8 )   /* Show where we they drop.      */
#define LDF_DRAGGABLE         (1 << 9 )   /* We are in draggable mode.     */
#define LDF_ALLOW_DRAG        (1 << 10)   /* Allow column dragging by user */
#define LDF_OFFSETS_VALID     (1 << 11)   /* Column offsets are valid      */
#define LDF_DRAGGING_COLUMN   (1 << 12)   /* Currently dragging a column   */
#define LDF_NEW_COLUMN_POS    (1 << 13)   /* Set when user releases column */
#define LDF_PRE_CLEAR         (1 << 14)   /* Pre-clear entry rectangle.    */
#define LDF_THIN_FRAMES       (1 << 15)   /* 1:1 Aspect ratio frames.      */
#define LDF_MOVE_DROPBOX      (1 << 16)   /* Move the dragbox around?      */
#define LDF_ONE_COLUMN        (1 << 17)   /* Redraw only one column.       */

#define LD_ENTRY(tag, offset, flags) PACK_ENTRY(LISTV_TAGSTART, tag, ld_, offset, flags)
#define LD_FLAG(tag, flag) PACK_LONGBIT(LISTV_TAGSTART, tag, ld_, ld_Flags, PKCTRL_BIT, flag)

static ULONG ListPackTable[] =
{
   PACK_STARTTABLE(LISTV_TAGSTART),

   LD_ENTRY(LISTV_ResourceHook,       ld_Resource,   PKCTRL_ULONG),
   LD_ENTRY(LISTV_DisplayHook,        ld_Display,    PKCTRL_ULONG),
   LD_ENTRY(LISTV_CompareHook,        ld_Compare,    PKCTRL_ULONG),
   LD_ENTRY(LISTV_TitleHook,          ld_TitleHook,  PKCTRL_ULONG),
   LD_ENTRY(LISTV_MinEntriesShown,    ld_MinShown,   PKCTRL_UWORD),
   LD_ENTRY(LISTV_Top,                ld_Top,        PKCTRL_ULONG),
   LD_ENTRY(LISTV_Columns,            ld_Columns,    PKCTRL_UWORD),
   LD_ENTRY(LISTV_Title,              ld_Title,      PKCTRL_ULONG),
   LD_ENTRY(LISTV_PropObject,         ld_Prop,       PKCTRL_ULONG),
   LD_ENTRY(LISTV_ListFont,           ld_ListFont,   PKCTRL_ULONG),

   LD_FLAG(LISTV_ReadOnly,            LDF_READ_ONLY),
   LD_FLAG(LISTV_MultiSelect,         LDF_MULTI_SELECT),
   LD_FLAG(LISTV_ThinFrames,          LDF_THIN_FRAMES),
   LD_FLAG(LISTV_MultiSelectNoShift,  LDF_NOSHIFT),
   LD_FLAG(LISTV_ShowDropSpot,        LDF_SHOWDROPSPOT),
   LD_FLAG(LISTV_CustomDisable,       LDF_CUSTOMDISABLE),
   LD_FLAG(LISTV_DragColumns,         LDF_ALLOW_DRAG),
   LD_FLAG(LISTV_PreClear,            LDF_PRE_CLEAR),

   PACK_ENDTABLE
};

#define LC_ENTRY(tag, offset, flags) PACK_ENTRY(LISTC_TAGSTART, tag, cd_, offset, flags)
#define LC_FLAG(tag, flag) PACK_LONGBIT(LISTC_TAGSTART, tag, cd_, cd_Flags, PKCTRL_BIT, flag)

static ULONG ColumnPackTable[] =
{
   PACK_STARTTABLE(LISTV_TAGSTART),

   LC_ENTRY(LISTC_MinWidth,           cd_MinWidth,   PKCTRL_UWORD),
   LC_ENTRY(LISTC_MaxWidth,           cd_MaxWidth,   PKCTRL_UWORD),
   LC_ENTRY(LISTC_Weight,             cd_Weight,     PKCTRL_UWORD),

   LC_FLAG(LISTC_Draggable,           LVCF_DRAGGABLE),
   LC_FLAG(LISTC_Hidden,              LVCF_HIDDEN),
   LC_FLAG(LISTC_NoSeparator,         LVCF_NOSEPARATOR),
   LC_FLAG(LISTC_PreClear,            LVCF_PRECLEAR),

   PACK_ENDTABLE
};

/*
 * Prop map-list.
 */
STATIC struct TagItem PGA2LISTV[] = {
   PGA_Top,       LISTV_Top,
   TAG_END
};

STATIC VOID RenderEntry(Object *obj, LD *ld, struct BaseInfo *bi, LVE *lve, ULONG top);
#define REL_ZERO (0x80000000)


//VOID ASM ColumnSeparators(REG(a0) LD *ld, REG(a1) struct BaseInfo *bi, REG(d0) ULONG x, REG(d1) ULONG y, REG(d2) ULONG h)
ASM REGFUNC5(VOID, ColumnSeparators,
	REGPARAM(A0, LD *, ld),
	REGPARAM(A1, struct BaseInfo *, bi),
	REGPARAM(D0, ULONG, x),
	REGPARAM(D1, ULONG, y),
	REGPARAM(D2, ULONG, h))
{
   int col, pena, penb, x2, y2;

   struct RastPort *rp = bi->bi_RPort;

   x2 = x + ld->ld_InnerBox.Width - 4;
   y2 = y + h - 1;

   if (ld->ld_Flags & LDF_READ_ONLY)
   {
      pena = SHINEPEN;
      penb = SHADOWPEN;
   }
   else
   {
      pena = SHADOWPEN;
      penb = SHINEPEN;
   };

   for (col = 0; col < ld->ld_Columns; col++)
   {
      if (!(ld->ld_CD[col].cd_Flags & LVCF_HIDDEN))
      {
	 x += ld->ld_CD[col].cd_Width - 2;

	 if (!(ld->ld_CD[col].cd_Flags & LVCF_NOSEPARATOR) && (x < x2))
	 {
	    BSetDPenA(bi, pena);
	    Move(rp, x,   y);
	    Draw(rp, x++, y2);

	    BSetDPenA(bi, penb);
	    Move(rp, x,   y);
	    Draw(rp, x++, y2);
	 };
      };
   };
}
REGFUNC_END
/*
 * Find a node by it's number (slow!).
 */
//STATIC ASM LVE *FindNode( REG(a0) LD *ld, REG(d0) ULONG num )
STATIC ASM REGFUNC2(LVE *, FindNode,
	REGPARAM(A0, LD *, ld),
	REGPARAM(D0, ULONG, num))
{
   LVE         *lve;
   ULONG        lnum = 0L;

   /*
    * List empty?
    */
   if ( ! ld->ld_Entries.lvl_First->lve_Next )
      return( NULL );

   /*
    * Scan the list top-down to find
    * the correct entry.
    */
   for ( lve = ld->ld_Entries.lvl_First; lve->lve_Next; lve = lve->lve_Next, lnum++ ) {
      if ( lnum == num )
	 break;
   }

   return( lve );
}
REGFUNC_END

/*
 * Find a node by it's number (quickly).
 */
//STATIC ASM LVE *FindNodeQuick( REG(a0) LD *ld, REG(d0) ULONG num )
STATIC ASM REGFUNC2(LVE *, FindNodeQuick,
	REGPARAM(A0, LD *, ld),
	REGPARAM(D0, ULONG, num))
{
   LVE         *lve = ld->ld_TopEntry;
   ULONG        top = ld->ld_Top;

   /*
    * List empty?
    */
   if ( ! ld->ld_Entries.lvl_First->lve_Next )
      return( NULL );

   /*
    * Scan the list from the current node
    * to find the correct entry.
    */
   if ( num > top ) {
      for ( ; lve->lve_Next; lve = lve->lve_Next, top++ ) {
	 if ( top == num )
	    break;
      }
   } else if ( num < top ) {
      for ( ; lve->lve_Prev != ( LVE * )&ld->ld_Entries; lve = lve->lve_Prev, top-- ) {
	 if ( top == num )
	    break;
      }
   }

   return( lve );
}
REGFUNC_END

/*
 * Add an entry in the list. Ugly code with
 * lotsa goto's :)
 */
//STATIC ASM VOID AddEntryInList( REG(a0) LD *ld, REG(a1) Object *obj, REG(a2) LVE *lve, REG(d0) ULONG how )
STATIC ASM REGFUNC4(VOID, AddEntryInList,
	REGPARAM(A0, LD *, ld),
	REGPARAM(A1, Object *, obj),
	REGPARAM(A2, LVE *, lve),
	REGPARAM(D0, ULONG, how))
{
   LVE                *tmp;
   struct lvCompare    lvc;

   /*
    * Save it.
    */
   ld->ld_LastAdded = lve;

   /*
    * Set top entry if not
    * defined yet.
    */
   if (!ld->ld_TopEntry)
      ld->ld_TopEntry = lve;

   switch (how)
   {
   case LVAP_SORTED:
      /*
       * Entries available?
       */
      if (!ld->ld_Entries.lvl_First->lve_Next)
	 /*
	  * No. Simply AddTail the entry.
	  */
	 goto tailIt;

      /*
       * Scan through the entries.
       */
      for (tmp = ld->ld_Entries.lvl_Last; ; tmp = tmp->lve_Prev)
      {
	 /*
	  * Comparrison hook?
	  */
	 if (!ld->ld_Compare)
	 {
	    /*
	     * No. Simple string comparrison.
	     */
	    if (Stricmp((STRPTR)lve->lve_Entry, (STRPTR)tmp->lve_Entry) >= 0)
	       break;
	 }
	 else
	 {
	    lvc.lvc_EntryA = lve->lve_Entry;
	    lvc.lvc_EntryB = tmp->lve_Entry;
	    if ((LONG)BGUI_CallHookPkt(ld->ld_Compare, (VOID *)obj, (VOID *)&lvc) >= 0)
	       break;
	 };
	 /*
	  * Done?
	  */
	 if (tmp == ld->ld_Entries.lvl_First)
	 {
	    ld->ld_TopEntry = lve;
	    /*
	     * First entry is AddHead'ed.
	     */
	    goto headIt;
	 };
      };

      insertIt:
      ld->ld_Flags |= LDF_REFRESH_ALL;
      Insert((struct List *)&ld->ld_Entries, (struct Node *)lve, (struct Node *)tmp);
      break;

   case LVAP_HEAD:
      headIt:
      ld->ld_Flags |= LDF_REFRESH_ALL;
      AddHead((struct List *)&ld->ld_Entries, (struct Node *)lve);
      ld->ld_TopEntry = lve;
      break;

   case LVAP_TAIL:
   default:
      tailIt:
      AddTail((struct List *)&ld->ld_Entries, (struct Node *)lve);
      break;
   }
}
REGFUNC_END

/*
 * Add entries to the list.
 */
//STATIC ASM BOOL AddEntries(REG(a0) LD *ld, REG(a1) APTR *entries, REG(a2) Object *obj, REG(d0) ULONG how, REG(a3) LVE *pred)
STATIC ASM REGFUNC5(BOOL, AddEntries,
	REGPARAM(A0, LD *, ld),
	REGPARAM(A1, APTR *, entries),
	REGPARAM(A2, Object *, obj),
	REGPARAM(D0, ULONG, how),
	REGPARAM(A3, LVE *, pred))
{
   LVE               *lve;
   struct lvResource  lvr;
   BOOL               success = TRUE;

   /*
    * Entries valid?
    */
   if (!entries) return FALSE;

   /*
    * Initialize lvResource structure to make entries.
    */
   lvr.lvr_Command = LVRC_MAKE;

   ld->ld_Flags |= LDF_LIST_BUSY;

   /*
    * Loop through the entries.
    */
   while (lvr.lvr_Entry = *entries++)
   {
      /*
       * Create a node.
       */
      if (lve = (LVE *)BGUI_AllocPoolMem(sizeof(LVE)))
      {
	 /*
	  * Do we have a resource hook?
	  */
	 if (ld->ld_Resource)
	 {
	    /*
	     * Let the hook make the entry.
	     */
	    lve->lve_Entry = (APTR)BGUI_CallHookPkt(ld->ld_Resource, (void *)obj, (void *)&lvr);
	 }
	 else
	 {
	    /*
	     * Simple string copy.
	     */
	    if (lve->lve_Entry = (APTR)BGUI_AllocPoolMem(strlen((STRPTR)lvr.lvr_Entry) + 1))
	       strcpy((STRPTR)lve->lve_Entry, (STRPTR)lvr.lvr_Entry);
	 }
	 /*
	  * All ok?
	  */
	 if (lve->lve_Entry)
	 {
	    if (!pred)
	    {
	       AddEntryInList(ld, obj, lve, how);
	       if (how == LVAP_HEAD)
		  pred = lve;
	    }
	    else
	    {
	       Insert((struct List *)&ld->ld_Entries, (struct Node *)lve, (struct Node *)pred);
	       pred = lve;
	    };
	    ld->ld_LastAdded = lve;
	    ld->ld_Total++;
	 }
	 else
	 {
	    success = FALSE;
	    BGUI_FreePoolMem(lve);
	 }
      }
   }
   ld->ld_Flags &= ~LDF_LIST_BUSY;

   return success;
}
REGFUNC_END

/*
 * Make an entry visible.
 */
//STATIC ASM ULONG MakeVisible(REG(a0) LD *ld, REG(d0) ULONG entry)
STATIC ASM REGFUNC2(ULONG, MakeVisible,
	REGPARAM(A0, LD *, ld),
	REGPARAM(D0, ULONG, entry))
{
   ULONG       new_top;

   /*
    * # of visible items known?
    */
   if (!ld->ld_Visible)
      ld->ld_Visible = 1;

   /*
    * Otherwise adjust the top to
    * make it visible.
    */
   if      (entry <  ld->ld_Top)                    new_top = entry;
   else if (entry >= (ld->ld_Top + ld->ld_Visible)) new_top = max((LONG)(entry - (ld->ld_Visible - 1)), 0);
   else if (entry >= ld->ld_Total)                  new_top = max((LONG)ld->ld_Total - (LONG)(ld->ld_Visible - 1), 0);
   else                                             new_top = ld->ld_Top;

   return new_top;
}
REGFUNC_END

/*
 * De-select node list (slow!).
 */
//STATIC ASM VOID DeSelect(REG(a0) LD *ld)
STATIC ASM REGFUNC1(VOID, DeSelect,
	REGPARAM(A0, LD *, ld))
{
   LVE         *lve;

   for (lve = ld->ld_Entries.lvl_First; lve->lve_Next; lve = lve->lve_Next)
   {
      if (lve->lve_Flags & LVEF_SELECTED)
      {
	 lve->lve_Flags &= ~LVEF_SELECTED;
	 lve->lve_Flags |= LVEF_REFRESH;
      };
   };
}
REGFUNC_END

/*
 * Select node list (slow!).
 */
//STATIC ASM VOID Select(REG(a0) LD *ld)
STATIC ASM REGFUNC1(VOID, Select,
	REGPARAM(A0, LD *, ld))
{
   LVE         *lve;

   for ( lve = ld->ld_Entries.lvl_First; lve->lve_Next; lve = lve->lve_Next ) {
      if ( ! ( lve->lve_Flags & LVEF_SELECTED ))
	 lve->lve_Flags |= LVEF_SELECTED | LVEF_REFRESH;
   }
}
REGFUNC_END

/*
 * Setup a new top-value.
 */
//STATIC ASM VOID NewTop(REG(a0) LD *ld, REG(a1) struct GadgetInfo *gi, REG(a2) Object *obj, REG(d0) ULONG new_top)
STATIC ASM REGFUNC4(VOID, NewTop,
	REGPARAM(A0, LD *, ld),
	REGPARAM(A1, struct GadgetInfo *, gi),
	REGPARAM(A2, Object *, obj),
	REGPARAM(D0, ULONG, new_top))
{
   struct BaseInfo   *bi;
   int                i;
   BC                *bc = BASE_DATA(obj);

   /*
    * How much do we need to go up?
    */
   int diff = new_top - ld->ld_Top;

   /*
    * Gadget info present?
    * New position different than old one?
    */
   if (gi && diff)
   {
      /*
       * Create rastport.
       */
#ifdef DEBUG_BGUI
      if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_GadgetInfo, gi, BI_RastPort, NULL, TAG_DONE))
#else
      if (bi = AllocBaseInfo(BI_GadgetInfo, gi, BI_RastPort, NULL, TAG_DONE))
#endif
      {
	 BOOL needs_refresh=FALSE;

	 /*
	  * Setup font.
	  */
	 BSetFont(bi, ld->ld_Font);

	 /*
	  * Set top position.
	  */
	 ld->ld_TopEntry = FindNodeQuick(ld, new_top);
	 ld->ld_Top = new_top;

	 if (diff <= -ld->ld_Visible)
	 {
	    /*
	     * More than a single view?
	     */
	    diff = -ld->ld_Visible;
	 }
	 else if (diff >= +ld->ld_Visible)
	 {
	    /*
	     * More than a single view?
	     */
	    diff = +ld->ld_Visible;
	 }
	 else
	 {
	    /*
	     * Scroll view 'diff' lines.
	     */
	    ScrollRaster(bi->bi_RPort, 0, diff * ld->ld_EntryHeight,
		 ld->ld_ListArea.Left,  ld->ld_ListArea.Top,
		 ld->ld_ListArea.Left + ld->ld_ListArea.Width - 1,
		 ld->ld_ListArea.Top  + ld->ld_ListArea.Height - 1);
	    needs_refresh=(bi->bi_RPort->Layer->Flags & LAYERREFRESH);
	 };             

	 ColumnSeparators(ld, bi, bc->bc_InnerBox.Left, bc->bc_InnerBox.Top, bc->bc_InnerBox.Height);

	 /*
	  * Re-render first 'diff' lines.
	  */
	 if (diff < 0)
	 {
	    i = -diff;
	 }
	 else
	 {
	    i = diff;
	    new_top += ld->ld_Visible - diff;
	 };

	 while (i--)
	 {
	    RenderEntry(obj, ld, bi, FindNodeQuick(ld, new_top), new_top - ld->ld_Top);
	    new_top++;
	 };

	 /*
	  * Dump BaseInfo.
	  */
	 FreeBaseInfo(bi);

	 if(needs_refresh)
	 {
	    BOOL update;

	    if(!(update=BeginUpdate(gi->gi_Layer)))
	       EndUpdate(gi->gi_Layer,FALSE);
	    DoRenderMethod( obj, gi, GREDRAW_REDRAW );
	    if(update)
	       EndUpdate(gi->gi_Layer,TRUE);
	 }

	 /*
	  * Needed by RenderEntry().
	  */
	 ld->ld_Flags &= ~LDF_REFRESH_ALL;
      }
   }
   else
   {
      ld->ld_TopEntry = FindNodeQuick(ld, new_top);
      ld->ld_Top = new_top;
   }
}
REGFUNC_END



/************************************************************************
**********************  MCLV_GETCOLUMNPOSITIONS()  **********************
*************************************************************************
* Work out the left offset (relative to listview view area) of each
* column separator. Put the results in ld_Offsets array, starting
* with the first separator (ie. the left offset of the *second* column
* from the left). Returns TRUE or FALSE to indicate success. Currently
* this can only fail if the object is not displayed when this function
* is called. If all goes well, LDF_OFFSETS_VALID is set to TRUE.
*
*************************************************************************/

STATIC BOOL GetColumnPositions(Object *obj, LD *ld)
{
   int          w, dw, x;
   int          totalwidth, columnwidth, totalweight, lastwidth;
   int          listwidth = ld->ld_ListArea.Width;
   CD          *cd, *fcd = ld->ld_CD, *lcd = fcd + ld->ld_Columns - 1;

   /*
    * Start widths at minimum and compute width of all columns.
    */
   columnwidth = 0;
   for (cd = fcd; cd <= lcd; ++cd)
   {
      cd->cd_Width = cd->cd_MinWidth;
      columnwidth += cd->cd_Width;
   };

   /*
    * Cycle through until all extra space is used.
    */
   while (columnwidth < listwidth)
   {
      lastwidth = columnwidth;
      totalwidth  = listwidth;
      totalweight = 0;
      for (cd = fcd; cd <= lcd; ++cd)
      {
	 if (!(cd->cd_Flags & LVCF_HIDDEN))
	 {
	    if (cd->cd_Width < cd->cd_MaxWidth)
	    {
	       totalweight += cd->cd_Weight;
	    }
	    else
	    {
	       cd->cd_Width = cd->cd_MaxWidth;
	       totalwidth  -= cd->cd_Width;
	    };
	 };
      };

      if (totalweight == 0) break;

      for (cd = fcd; cd <= lcd; ++cd)
      {
	 if (!(cd->cd_Flags & LVCF_HIDDEN))
	 {
	    if (cd->cd_Width < cd->cd_MaxWidth)
	    {
	       w = (cd->cd_Weight * totalwidth) / totalweight;
	       if (w > cd->cd_MaxWidth) w = cd->cd_MaxWidth;
	       if (w < cd->cd_MinWidth) w = cd->cd_MinWidth;

	       dw = w - cd->cd_Width;
	       if (dw > listwidth - columnwidth)
	       {
		  dw = listwidth - columnwidth;
		  cd->cd_Width += dw;
		  columnwidth  += dw;
		  break;
	       };
	       cd->cd_Width += dw;
	       columnwidth  += dw;
	    };
	 };
      };
      if (columnwidth == lastwidth) break;
   };

   x = 0;
   for (cd = fcd; cd <= lcd; ++cd)
   {
      if (!(cd->cd_Flags & LVCF_HIDDEN))
      {
	 cd->cd_Offset = x;
	 x += cd->cd_Width;
      };
   };
   cd->cd_Offset = x;  // Last "dummy" column needs right offset of list.

   ld->ld_Flags |= LDF_OFFSETS_VALID|LDF_REFRESH_ALL;
   
   return TRUE;
}

/************************************************************************
*************************  MCLV_DRAWDRAGLINE()  *************************
*************************************************************************
* Draw, using complement mode, the line that shows the user the position
* s/he has dragged the column separator to within the mclv.
*
*************************************************************************/

//STATIC ASM VOID DrawDragLine(REG(a0) LD *ld, REG(a1) struct GadgetInfo *gi)
STATIC ASM REGFUNC2(VOID, DrawDragLine,
	REGPARAM(A0, LD *, ld),
	REGPARAM(A1, struct GadgetInfo *, gi))
{
   WORD x1 = ld->ld_InnerBox.Left + ld->ld_DragXLine - 2;
   WORD x2 = x1 + 1;
   WORD y1 = ld->ld_InnerBox.Top;
   WORD y2 = ld->ld_InnerBox.Top + ld->ld_InnerBox.Height - 1;

   struct BaseInfo *bi;

#ifdef DEBUG_BGUI
   if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_GadgetInfo, gi, BI_RastPort, NULL, TAG_DONE))
#else
   if (bi = AllocBaseInfo(BI_GadgetInfo, gi, BI_RastPort, NULL, TAG_DONE))
#endif
   {
      BSetDrMd(bi, COMPLEMENT);
      BRectFill(bi, x1, y1, x2, y2);
      FreeBaseInfo(bi);
   };
}
REGFUNC_END

/// OM_NEW
/*
 * Create a new object.
 */
METHOD(ListClassNew, struct opSet *, ops)
{
   LD       *ld;
   struct TagItem *tags, *tstate, *tag;
   ULONG        rc, sort = LVAP_TAIL, data;
   ULONG       *new_weights = NULL;
   APTR        *new_entries = NULL;
   int          i;

   tags = DefTagList(BGUI_LISTVIEW_GADGET, ops->ops_AttrList);

   /*
    * Let the superclass make the object.
    */
   if (rc = NewSuperObject(cl, obj, tags))
   {
      /*
       * Get the instance data.
       */
      ld              = INST_DATA(cl, rc);
      ld->ld_BC       = BASE_DATA(rc);

      ld->ld_Flags    = LDF_REFRESH_ALL|LDF_PRE_CLEAR;
      ld->ld_MinShown = 3;
      ld->ld_Columns  = 1;
      ld->ld_Prop     = (Object *)~0;

      /*
       * Initialize the list of entries.
       */
      NewList((struct List *)&ld->ld_Entries);
   // NewList((struct List *)&ld->ld_Hidden);
      
      BGUI_PackStructureTags((APTR)ld, ListPackTable, tags);

      tstate = tags;

      while (tag = NextTagItem(&tstate))
      {
	 data = tag->ti_Data;
	 switch (tag->ti_Tag)
	 {
	 case LISTV_EntryArray:
	    new_entries = (APTR *)data;
	    break;

	 case LISTV_SortEntryArray:
	    sort = LVAP_SORTED;
	    break;
	    
	 case LISTV_ColumnWeights:
	    new_weights = (ULONG *)data;
	    break;
	 };
      };

      if (ld->ld_Columns < 1) ld->ld_Columns = 1;

      ld->ld_CD = BGUI_AllocPoolMem((ld->ld_Columns + 1) * sizeof(CD));

      /*
       * Verify array allocated.
       */
      if (ld->ld_CD)
      {
	 for (i = 0; i <= ld->ld_Columns; i++)
	 {
	    ld->ld_CD[i].cd_MinWidth = 24;
	    ld->ld_CD[i].cd_MaxWidth = 0xFFFF;
	    ld->ld_CD[i].cd_Weight   = new_weights ? (new_weights[i] ? new_weights[i] : 1) : DEFAULT_WEIGHT;
	 };
	 
	 if (ld->ld_Prop == (Object *)~0)
	 {
	    /*
	     * Filter out frame and label attributes.
	     */
	    tstate = tags;

	    while (tag = NextTagItem(&tstate))
	    {
	       switch (tag->ti_Tag)
	       {
	       /*
		* Don't disable prop!
		*/
	       case GA_Disabled:
	       /*
		* No drag'n'drop on the prop.
		*/
	       case BT_DragObject:
	       case BT_DropObject:
		  tag->ti_Tag = TAG_IGNORE;
		  break;

	       default:
		  if (FRM_TAG(tag->ti_Tag) || LAB_TAG(tag->ti_Tag))
		     tag->ti_Tag = TAG_IGNORE;
		  break;
	       };
	    };

	    /*
	     * Create a scroller.
	     */
	    ld->ld_Prop = BGUI_NewObject(BGUI_PROP_GADGET, GA_ID, GADGET(rc)->GadgetID,
	       PGA_DontTarget, TRUE, BT_ParentView, ld->ld_BC->bc_View, BT_ParentWindow, ld->ld_BC->bc_Window,TAG_MORE, tags);
	 };

	 if (ld->ld_Prop)
	 {
	    /*
	     * Setup scroller notification.
	     */
	    AsmDoMethod(ld->ld_Prop, BASE_ADDMAP, rc, PGA2LISTV);
	 };

	 if (ld->ld_Frame)
	 {
	    /*
	     * Set frame attributes to match list attributes.
	     */
	    DoSetMethodNG(ld->ld_Frame, FRM_Recessed,  ld->ld_Flags & LDF_READ_ONLY,
					FRM_ThinFrame, ld->ld_Flags & LDF_THIN_FRAMES,
					TAG_DONE);
	 };

	 /*
	  * Add entries.
	  */
	 AddEntries(ld, new_entries, (Object *)rc, sort, NULL);
      }
      else
      {
	 AsmCoerceMethod(cl, (Object *)rc, OM_DISPOSE);
	 rc = 0;
      };
   };
   FreeTagItems(tags);

   return rc;
}
METHOD_END
///
/// OM_SET, OM_UPDATE
/*
 * Set or update some attributes.
 */
METHOD(ListClassSetUpdate, struct opUpdate *, opu)
{
   LD             *ld = INST_DATA( cl, obj );
   struct TagItem *tstate = opu->opu_AttrList, *tag;
   LVE            *lve;
   ULONG           data, otop = ld->ld_Top, ntop = otop, num, oldcol = ld->ld_Columns;
   WORD            dis = GADGET(obj)->Flags & GFLG_DISABLED;
   BOOL            vc = FALSE;
   ULONG          *new_weights;

   int i;

   /*
    * First the superclass...
    */
   AsmDoSuperMethodA(cl, obj, (Msg)opu);

   BGUI_PackStructureTags((APTR)ld, ListPackTable, tstate);

   /*
    * Scan for known attributes.
    */
   while (tag = NextTagItem(&tstate))
   {
      data = tag->ti_Data;
      switch (tag->ti_Tag)
      {
      case LISTV_PropObject:
	 if (ld->ld_Prop) DisposeObject(ld->ld_Prop);
	 ld->ld_Flags &= ~LDF_PROPACTIVE;

	 if (ld->ld_Prop = (Object *)data)
	 {
	    /*
	     * Setup scroller notification.
	     */
	    AsmDoMethod(ld->ld_Prop, BASE_ADDMAP, obj, PGA2LISTV);
	 };
	 vc = TRUE;
	 break;

      case FRM_ThinFrame:
	 /*
	  * Set frame thickness.
	  */
	 if (ld->ld_Frame) DoSetMethodNG(ld->ld_Frame, FRM_ThinFrame, data, TAG_END);
	 if (ld->ld_Prop)  DoSetMethodNG(ld->ld_Prop,  FRM_ThinFrame, data, TAG_END);
	 break;

      case BT_TextAttr:
	 /*
	  * Pickup superclass font.
	  */
	 if (ld->ld_Font)
	 {
	    BGUI_CloseFont(ld->ld_Font);
	    ld->ld_Font = NULL;
	 };
	 vc=TRUE;
	 break;

      case LISTV_ListFont:
	 ld->ld_ListFont = (struct TextAttr *)data;
	 if (ld->ld_Font)
	 {
	    BGUI_CloseFont(ld->ld_Font);
	    ld->ld_Font = NULL;
	 };
	 vc=TRUE;
	 break;

      case LISTV_Top:
	 ld->ld_Top = otop;  // Needed to circumvent PackStructureTags.
	 /*
	  * Make sure we stay in range.
	  */
	 ntop = range(data, 0, (ld->ld_Total > ld->ld_Visible) ? ld->ld_Total - ld->ld_Visible : 0);
	 break;

      case LISTV_MakeVisible:
	 /*
	  * Stay in range.
	  */
	 if ((num = data) >= ld->ld_Total)
	    num = ld->ld_Total - 1;

	 /*
	  * Make it visible.
	  */
	 ntop = MakeVisible( ld, num );
	 vc = TRUE;
	 break;

      case LISTV_DeSelect:
	 /*
	  * Get the node number.
	  */
	 if (( num = data ) == ~0 ) {
	    /*
	     * A value of -1 means deselect
	     * all entries.
	     */
	    DeSelect( ld );
	    vc = TRUE;
	 } else {
	    /*
	     * Stay in range.
	     */
	    num = num >= ld->ld_Total ? ld->ld_Total - 1 : num;

	    /*
	     * Find the node to deselect.
	     */
	    if ( lve = FindNodeQuick( ld, num )) {
	       /*
		* Set it up.
		*/
	       ld->ld_LastActive = lve;
	       ld->ld_LastNum   = num;

	       /*
		* Already unselected?
		*/
	       if (( lve->lve_Flags & LVEF_SELECTED ) && ( ! ( ld->ld_Flags & LDF_READ_ONLY ))) {
		  /*
		   * Mark it for a refresh.
		   */
		  lve->lve_Flags &= ~LVEF_SELECTED;
		  lve->lve_Flags |=  LVEF_REFRESH;

		  /*
		   * Notify change.
		   */
		  DoNotifyMethod( obj, opu->opu_GInfo, opu->MethodID == OM_UPDATE ? opu->opu_Flags : 0L, GA_ID, GADGET( obj )->GadgetID, LISTV_Entry, lve->lve_Entry, LISTV_EntryNumber, ld->ld_LastNum, TAG_END );
		  vc = TRUE;
	       }
	    }
	 }
	 break;

      case LISTV_SelectMulti:
      case LISTV_SelectMultiNotVisible:
	 /*
	  * Must be a multi-select object.
	  */
	 if ( ! ( ld->ld_Flags & LDF_MULTI_SELECT ))
	    break;

	 /*
	  * LISTV_Select_All?
	  */
	 if (data == LISTV_Select_All)
	 {
	    Select(ld);
	    vc = TRUE;
	    break;
	 }

	 /* Fall through. */

      case LISTV_Select:
      case LISTV_SelectNotVisible:
	 /*
	  * Do some Magic?
	  */
	 switch (data)
	 {
	    case  LISTV_Select_First:
	       num = 0;
	       goto doIt;

	    case  LISTV_Select_Last:
	       num = ld->ld_Total - 1;
	       goto doIt;

	    case  LISTV_Select_Next:
	       if (!ld->ld_LastActive) num = ld->ld_Top;
	       else                    num = ld->ld_LastNum + 1;
	       goto doIt;

	    case  LISTV_Select_Previous:
	       if (!ld->ld_LastActive) num = ld->ld_Top;
	       else                    num = max((LONG)ld->ld_LastNum - 1, 0);
	       goto doIt;

	    case  LISTV_Select_Top:
	       num = ld->ld_Top;
	       goto doIt;

	    case  LISTV_Select_Page_Up:
	       if (!ld->ld_LastActive) num = ld->ld_Top;
	       else
	       {
		  num = ld->ld_LastNum;
		  if (num > ld->ld_Top) num = ld->ld_Top;
		  else                  num = max((LONG)(num - ld->ld_Visible + 1), 0);
	       }
	       goto doIt;

	    case  LISTV_Select_Page_Down:
	       if (!ld->ld_LastActive ) num = ld->ld_Top;
	       else
	       {
		  num = ld->ld_LastNum;
		  if (num < (ld->ld_Top + ld->ld_Visible - 1)) num = ld->ld_Top + ld->ld_Visible - 1;
		  else    num = min((LONG)(num + (ld->ld_Visible - 1)), ld->ld_Total - 1);
	       }
	       goto doIt;

	    default:
	       num = data;

	       doIt:

	       /*
		* Make sure we stay in range.
		*/
	       num = (num >= ld->ld_Total) ? ld->ld_Total - 1 : num;

	       /*
		* Find the node to select.
		*/
	       if (lve = ld->ld_LastActive = FindNodeQuick(ld, num))
	       {
		  /*
		   * Setup the number as the last
		   * selected one.
		   */
		  ld->ld_LastNum = num;

		  /*
		   * Already selected?
		   */
		  if (((( tag->ti_Tag != LISTV_SelectMulti ) && ( tag->ti_Tag != LISTV_SelectMultiNotVisible )) || ! ( lve->lve_Flags & LVEF_SELECTED )) && ( ! ( ld->ld_Flags & LDF_READ_ONLY ))) {
		     /*
		      * No? DeSelect all other labels if we are not
		      * multi-selecting.
		      */
		     if (( tag->ti_Tag != LISTV_SelectMulti ) && ( tag->ti_Tag != LISTV_SelectMultiNotVisible ))
			DeSelect( ld );

		     /*
		      * Mark this entry as selected.
		      */
		     lve->lve_Flags |= LVEF_SELECTED | LVEF_REFRESH;

		     /*
		      * Notify change.
		      */
		     DoNotifyMethod( obj, opu->opu_GInfo, opu->MethodID == OM_UPDATE ? opu->opu_Flags : 0L, GA_ID, GADGET( obj )->GadgetID, LISTV_Entry, lve->lve_Entry, LISTV_EntryNumber, ld->ld_LastNum, TAG_END );
		     vc = TRUE;
		  }
		  /*
		   * Make the entry visible if requested.
		   */
		  if (( tag->ti_Tag != LISTV_SelectNotVisible ) && ( tag->ti_Tag != LISTV_SelectMultiNotVisible ))
		     ntop = MakeVisible( ld, num );
	       }
	       break;
	 }
	 break;

      case LISTV_ColumnWeights:
	 if (!(ld->ld_Flags & LDF_DRAGGING_COLUMN))
	 {
	    new_weights = (ULONG *)data;
	    for (i = 0; i < ld->ld_Columns; i++)
	    {
	       ld->ld_CD[i].cd_Weight = new_weights ? (new_weights[i] ? new_weights[i] : 1) : DEFAULT_WEIGHT;
	    };
	    ld->ld_Flags &= ~LDF_OFFSETS_VALID;
	    vc = TRUE;
	 };
	 break;

      case LISTV_Columns:
	 ld->ld_Columns = oldcol;  // can't change yet
	 break;

      case BT_ParentWindow:
      case BT_ParentView:
	 DoSetMethodNG(ld->ld_Prop, tag->ti_Tag, data, TAG_DONE);
	 break;
      };
   };

   /*
    * Disabled state changed?
    */
   if ((GADGET(obj)->Flags & GFLG_DISABLED) != dis)
   {
      if ( opu->opu_GInfo ) {
	 ld->ld_Flags |= LDF_REFRESH_ALL;
	 DoRenderMethod( obj, opu->opu_GInfo, GREDRAW_REDRAW );
	 vc = FALSE;
      }
   }

   /*
    * Top value changed?
    */
   if (vc)
   {
      DoRenderMethod(obj, opu->opu_GInfo, GREDRAW_UPDATE);
      if (ntop != otop)
      {
	 NewTop(ld, opu->opu_GInfo, obj, ntop);
	 if (ld->ld_Prop) DoSetMethod(ld->ld_Prop, opu->opu_GInfo, PGA_Top, ntop, TAG_DONE);
      }
   }
   else if (ntop != otop)
   {
      if (!FindTagItem(GA_ID, opu->opu_AttrList))
	 if (ld->ld_Prop) DoSetMethod(ld->ld_Prop, opu->opu_GInfo, PGA_Top, ntop, TAG_END);
      NewTop(ld, opu->opu_GInfo, obj, ntop);
   };

   return 1;
}
METHOD_END
///
/// OM_GET
/*
 * They want to know something.
 */
METHOD(ListClassGet, struct opGet *, opg)
{
   LD          *ld = INST_DATA( cl, obj );
   ULONG        rc = TRUE, num = ~0;
   ULONG        tag = opg->opg_AttrID, *store = opg->opg_Storage;

   switch (tag)
   {
   case LISTV_LastClicked:
      if (ld->ld_LastActive) STORE ld->ld_LastActive->lve_Entry;
      else                   STORE 0;
      break;

   case LISTV_LastClickedNum:
      if (ld->ld_LastActive)
	 num = ld->ld_LastNum;
      STORE num;
      break;

   case LISTV_NumEntries:
      STORE ld->ld_Total;
      break;

   case LISTV_DropSpot:
      STORE ld->ld_DropSpot;
      break;

   case LISTV_NewPosition:
      STORE ld->ld_NewPos;
      break;

   case LISTV_ViewBounds:
      STORE &ld->ld_InnerBox;
      break;

   case LISTV_LastColumn:
      STORE ld->ld_LastCol;
      break;

   default:
      rc = BGUI_UnpackStructureTag((UBYTE *)ld, ListPackTable, tag, store);
      if (!rc) rc = AsmDoSuperMethodA(cl, obj, (Msg)opg);
      break;
   }
   return rc;
}
METHOD_END
///
/// OM_DISPOSE
/*
 * Dispose of the object.
 */
METHOD(ListClassDispose, Msg, msg)
{
   LD                *ld = INST_DATA(cl, obj);
   LVE               *lve;
   struct lvResource  lvr;

   /*
    * Free all entries.
    */
   while (lve = (LVE *)RemHead((struct List *)&ld->ld_Entries))
   {
      /*
       * Do we have a resource hook?
       */
      if (ld->ld_Resource)
      {
	 /*
	  * Initialize lvResource structure to kill the entry.
	  */
	 lvr.lvr_Command = LVRC_KILL;
	 lvr.lvr_Entry   = lve->lve_Entry;

	 /*
	  * Call the hook.
	  */
	 BGUI_CallHookPkt(ld->ld_Resource, (VOID *)obj, (VOID *)&lvr);
      }
      else
      {
	 /*
	  * Simple deallocation.
	  */
	 BGUI_FreePoolMem(lve->lve_Entry);
      };

      /*
       * Free the entry node.
       */
      BGUI_FreePoolMem(lve);
   }

   /*
    * Kill the scroller.
    */
   if (ld->ld_Prop) AsmDoMethod(ld->ld_Prop, OM_DISPOSE);

   if (ld->ld_CD) BGUI_FreePoolMem(ld->ld_CD);

   if (ld->ld_Font) BGUI_CloseFont(ld->ld_Font);
   
   /*
    * The superclass takes care of the rest.
    */
   return AsmDoSuperMethodA(cl, obj, msg);
}
METHOD_END
///

/// ListAreaBounds
/*
 * Setup the list area bounds.
 */
//STATIC ASM VOID ListAreaBounds(REG(a0) Object *obj, REG(a1) LD *ld)
STATIC ASM REGFUNC2(VOID, ListAreaBounds,
	REGPARAM(A0, Object *, obj),
	REGPARAM(A1, LD *, ld))
{
   int          fh = ld->ld_EntryHeight;
   int          overhead;

   if (ld->ld_ListArea.Width != ld->ld_InnerBox.Width)
      ld->ld_Flags &= ~LDF_OFFSETS_VALID;
      
   /*
    * Setup list display area and view bounds.
    */
   ld->ld_ListArea = ld->ld_InnerBox;
   
   /*
    * Title?
    */
   if (ld->ld_Title || ld->ld_TitleHook)
   {
      ld->ld_ListArea.Top    += (fh + 2);
      ld->ld_ListArea.Height -= (fh + 2);
   };

   /*
    * Setup the amount of visible entries
    * and the amount of pixels to adjust.
    */
   ld->ld_Visible = ld->ld_ListArea.Height / fh;
   overhead       = ld->ld_ListArea.Height % fh;

   /*
    * If the list area is larger than the
    * total amount of entries we set overhead
    * to 0.
    */
   if (ld->ld_Total <= ld->ld_Visible)
   {
      overhead = 0;
   }
   else
   {
      ld->ld_ListArea.Top    += overhead / 2;
      ld->ld_ListArea.Height -= overhead;
   };

   /*
    * Any change in the overhead?
    */
   if (overhead != ld->ld_Overhead)
   {
      ld->ld_Overhead = overhead;
      ld->ld_Flags   |= LDF_REFRESH_ALL;
   };

   /*
    * Recompute the columns.
    */
   if (!(ld->ld_Flags & LDF_OFFSETS_VALID))
      GetColumnPositions(obj, ld);

   /*
    * Check top setting.
    */
   ld->ld_Top = max(0, min(ld->ld_Top, ld->ld_Total - ld->ld_Visible));

   /*
    * Get top entry.
    */
   ld->ld_TopEntry = FindNode(ld, ld->ld_Top);

}
REGFUNC_END
///
/// RenderColumn

//STATIC ASM SAVEDS VOID RenderColumn(REG(a0) char *text, REG(a2) Object *obj, REG(a1) struct lvRender *lvr)
STATIC ASM SAVEDS REGFUNC3(VOID, RenderColumn,
	REGPARAM(A0, char *, text),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvRender *, lvr))
{
   int                col  = lvr->lvr_Column;   
   struct BaseInfo   *bi;
   struct IBox        area;
   char              *term = NULL;

   /*
    * Setup rendering area.
    */
   area.Left   = lvr->lvr_Bounds.MinX + 2;
   area.Width  = lvr->lvr_Bounds.MaxX - lvr->lvr_Bounds.MinX - 3;
   area.Top    = lvr->lvr_Bounds.MinY;
   area.Height = lvr->lvr_Bounds.MaxY - lvr->lvr_Bounds.MinY + 1;

   if (strchr(text, '\t'))
   {
      while (col--)
      {
	 text = strchr(text, '\t');
	 if (text) text++;
	 else break;
      };
      if (text)
      {
	 term = strchr(text, '\t');
	 if (term) *term = 0;
      };
   }
   
#ifdef DEBUG_BGUI
   if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_DrawInfo, lvr->lvr_DrawInfo, BI_RastPort, lvr->lvr_RPort, TAG_DONE))
#else
   if (bi = AllocBaseInfo(BI_DrawInfo, lvr->lvr_DrawInfo, BI_RastPort, lvr->lvr_RPort, TAG_DONE))
#endif
   {
      /*
       * Render entry.
       */
      if (text)
      {
	 RenderText(bi, text, &area);
	 if (term) *term = '\t';
      };

      if (GADGET(obj)->Flags & GFLG_DISABLED)
      {
	 area.Left  -= 2;
	 area.Width += 4;
	 /*
	  * Disable it.
	  */
	 BDisableBox(bi, &area);
      };
      FreeBaseInfo(bi);
   };
}
REGFUNC_END
///
/// RenderEntry
/*
 * Render a single entry.
 */
STATIC VOID RenderEntry(Object *obj, LD *ld, struct BaseInfo *bi, LVE *lve, ULONG top)
{
   BC                *bc = BASE_DATA(obj);
   struct lvRender    lvr;
   UBYTE             *txt;
   UWORD              col;
   int                cx, cw, xoff, yoff;
   BOOL               sel, clear;
   struct Hook       *hook;
   struct RastPort   *rp = bi->bi_RPort;

   if (top & REL_ZERO)
   {
      top &= ~REL_ZERO;
      xoff = yoff = 0;
   }
   else
   {
      xoff = ld->ld_ListArea.Left;
      yoff = ld->ld_ListArea.Top;
   };

   /*
    * Initialize the lvRender structure.
    */
   lvr.lvr_RPort    = rp;
   lvr.lvr_DrawInfo = bi->bi_DrInfo;

   if (lve)
   {
      sel  = lve->lve_Flags & LVEF_SELECTED;
      hook = ld->ld_Display;

      lvr.lvr_Entry       = lve->lve_Entry;
      lvr.lvr_Bounds.MinY = yoff + ld->ld_EntryHeight * top;
      lvr.lvr_Bounds.MaxY = lvr.lvr_Bounds.MinY + ld->ld_EntryHeight - 1;
   }
   else
   {
      /*
       * A NULL entry means render the title.
       */
      sel  = FALSE;
      hook = ld->ld_TitleHook;

      lvr.lvr_Entry       = ld->ld_Title;
      lvr.lvr_Bounds.MinY = bc->bc_InnerBox.Top;
      lvr.lvr_Bounds.MaxY = bc->bc_InnerBox.Top + ld->ld_EntryHeight - 1;
   };

   /*
    * Setup state information.
    */
   if (GADGET(obj)->Flags & GFLG_DISABLED)
      lvr.lvr_State = sel ? LVRS_SELECTED_DISABLED : LVRS_NORMAL_DISABLED;
   else
      lvr.lvr_State = sel ? LVRS_SELECTED : LVRS_NORMAL;

   for (col = 0, cx = xoff; col < ld->ld_Columns; col++)
   {
      if (!(ld->ld_CD[col].cd_Flags & LVCF_HIDDEN))
      {
	 cw = ld->ld_CD[col].cd_Width;
      
	 lvr.lvr_Bounds.MinX  = cx;
	 lvr.lvr_Bounds.MaxX  = cx + cw - ((col < (ld->ld_Columns - 1)) ? 3 : 1);
	 lvr.lvr_Column       = col;

	 cx += cw;

	 if (!(ld->ld_Flags & LDF_ONE_COLUMN) || (col == ld->ld_OneColumn))
	 {
	    /*
	     * Do we have a display hook?
	     */
	    if (hook)
	    {
	       clear = (ld->ld_Flags & LDF_PRE_CLEAR) || (ld->ld_CD[col].cd_Flags & LVCF_PRECLEAR);

	       if (clear)
	       {
		  AsmDoMethod(ld->ld_Frame, FRAMEM_BACKFILL, bi, &lvr.lvr_Bounds, sel ? IDS_SELECTED : IDS_NORMAL);
		  clear = FALSE;
	       };
	       /*
		* Call the hook.
		*/
	       txt = (UBYTE *)BGUI_CallHookPkt(hook, (void *)obj, (void *)&lvr);
	    }
	    else
	    {
	       /*
		* Pick up the entry text.
		*/
	       clear = TRUE;
	       txt = lvr.lvr_Entry;
	    };
	    if (txt)
	    {
	       if (clear)
	       {
		  AsmDoMethod(ld->ld_Frame, FRAMEM_BACKFILL, bi, &lvr.lvr_Bounds, sel ? IDS_SELECTED : IDS_NORMAL);
	       };
	       BSetDPenA(bi, sel ? FILLTEXTPEN : TEXTPEN);
	       RenderColumn(txt, obj, &lvr);
	    };
	 };
      };
   };
   /*
    * Done this one.
    */
   if (lve) lve->lve_Flags &= ~LVEF_REFRESH;
}
///
/// BASE_LAYOUT
/*
 * Render the list.
 */
METHOD(ListClassLayout, struct bmLayout *, bml)
{
   LD                *ld = INST_DATA(cl, obj);
   BC                *bc = BASE_DATA(obj);
   struct TextAttr   *lf;
   int                sw = 0;
   ULONG              rc;

    if (ld->ld_Prop)
   {
      lf = ld->ld_ListFont ? ld->ld_ListFont : bc->bc_TextAttr;
      if (lf)
      {
	 sw = max((lf->ta_YSize * 2) / 3, 16);
      }
      else
      {
	 sw = 16;
      };
      /*
       * Setup offset.
       */
      bml->bml_Bounds->Width -= sw;
   };

   /*
    * First the superclass.
    */
   rc = AsmDoSuperMethodA(cl, obj, (Msg)bml);

   if (sw) bml->bml_Bounds->Width += sw;
  
   return rc;
}
METHOD_END
///
/// BASE_RENDER
/*
 * Render the list.
 */
METHOD(ListClassRender, struct bmRender *, bmr)
{
   LD                *ld = INST_DATA(cl, obj);
   BC                *bc = BASE_DATA(obj);
   struct BaseInfo   *bi = bmr->bmr_BInfo;
   struct RastPort   *rp = bi->bi_RPort;
   struct TextFont   *tf = ld->ld_Font, *of;
   struct TextAttr   *lf = ld->ld_ListFont;
   struct Rectangle   rect;
   ULONG              num, a;
   LVE               *lve;
   int                x, y, w, h, sw;
   UWORD              overhead;

   /*
    * Render the baseclass.
    */
   AsmDoSuperMethodA(cl, obj, (Msg)bmr);

   if (!lf) lf = bc->bc_TextAttr;

   sw = lf ? ((lf->ta_YSize * 2) / 3) : 0;
   if (sw < 16) sw = 16;

   /*
    * Setup the font.
    */
   of = rp->Font;
   if (!tf)
   {
      if (lf)
      {
	 if (tf = BGUI_OpenFont(lf))
	 {
	    ld->ld_Font = tf;
	    BSetFont(bi, tf);
	 }
	 else
	 {
	    tf = of;
	 };
      }
      else
      {
	 tf = of;
      };
   };

   /*
    * Setup entry height always even.
    */
   ld->ld_EntryHeight = (tf->tf_YSize + 1) & (ULONG)~1;

   overhead=ld->ld_Overhead;

   /*
    * Setup the list area bounds.
    */
   ListAreaBounds(obj, ld);

   x = bc->bc_InnerBox.Left;
   w = bc->bc_InnerBox.Width;

   /*
    * Complete redraw?
    */
   if (overhead<ld->ld_Overhead
   || bmr->bmr_Flags == GREDRAW_REDRAW)
   {
      if(overhead<ld->ld_Overhead)
	 overhead=ld->ld_Overhead;

      /*
       * Clear the overhead.
       */
      if (h = overhead >> 1)
      {
	 y = bc->bc_InnerBox.Top;
	 if (ld->ld_Title || ld->ld_TitleHook) y += ld->ld_EntryHeight+2;

	 rect.MinX = x;  rect.MaxX = x + w - 1;
	 rect.MinY = y;  rect.MaxY = y + h - 1;

	 AsmDoMethod(bc->bc_Frame, FRAMEM_BACKFILL, bi, &rect, IDS_NORMAL);
      };

      if (h = overhead - h)
      {
	 y = bc->bc_InnerBox.Top + bc->bc_InnerBox.Height - h;

	 rect.MinX = x;  rect.MaxX = x + w - 1;
	 rect.MinY = y;  rect.MaxY = y + h - 1;

	 AsmDoMethod(bc->bc_Frame, FRAMEM_BACKFILL, bi, &rect, IDS_NORMAL);
      };

      /*
       * Draw the separators.
       */
      ColumnSeparators(ld, bi, bc->bc_InnerBox.Left, bc->bc_InnerBox.Top, bc->bc_InnerBox.Height);

      /*
       * If we have a title render it.
       */
      if (ld->ld_Title || ld->ld_TitleHook)
      {
	 /*
	  * Just in case the font changed.
	  */
	 if (tf) BSetFont(bi, tf);

	 RenderEntry(obj, ld, bi, NULL, 0);

	 if (ld->ld_Columns > 1)
	 {
	    y = bc->bc_InnerBox.Top + ld->ld_EntryHeight;
	    BSetDPenA(bi, ld->ld_Flags & LDF_READ_ONLY ? SHINEPEN : SHADOWPEN);
	    HLine(rp, x, y++, x + w - 1);
	    BSetDPenA(bi, ld->ld_Flags & LDF_READ_ONLY ? SHADOWPEN : SHINEPEN);
	    HLine(rp, x, y,   x + w - 1);
	 };
      };

      /*
       * Just do it.
       */
      ld->ld_Flags |= LDF_REFRESH_ALL;
   };

   /*
    * Just in case the font changed.
    */
   if (tf) BSetFont(bi, tf);

   /*
    * Loop through the entries.
    */
   for (num = ld->ld_Top, a = 0; (a < ld->ld_Visible) && (num < ld->ld_Total); num++, a++)
   {
      /*
       * Render the entry.
       */
      if (lve = FindNodeQuick(ld, num))
      {
	 /*
	  * Only render when necessary.
	  */
	 if ((ld->ld_Flags & LDF_REFRESH_ALL) || (lve->lve_Flags & LVEF_REFRESH))
	 {
	    RenderEntry(obj, ld, bi, lve, a);
	 };
      };
   };

   /*
    * Disabled?
    */
   if ((!(ld->ld_Flags & LDF_CUSTOMDISABLE)) && ld->ld_Display)
   {
      if (GADGET(obj)->Flags & GFLG_DISABLED)
      {
	 BDisableBox(bi, &bc->bc_HitBox);
      };
   };

   /*
    * Done.
    */
   ld->ld_Flags &= ~LDF_REFRESH_ALL;

   /*
    * Replace the font.
    */
   BSetFont(bi, of);

   if (ld->ld_Prop)
   {
      /*
       * Setup scroller bounds.
       */
      GADGETBOX(ld->ld_Prop)->Left   = ld->ld_HitBox.Left + ld->ld_HitBox.Width;
      GADGETBOX(ld->ld_Prop)->Top    = ld->ld_HitBox.Top;
      GADGETBOX(ld->ld_Prop)->Width  = sw;
      GADGETBOX(ld->ld_Prop)->Height = ld->ld_HitBox.Height;

      /*
       * Set top, total etc.
       */
      DoSetMethodNG(ld->ld_Prop, PGA_Top,     ld->ld_Top,      PGA_Total, ld->ld_Total,
				 PGA_Visible, ld->ld_Visible,  TAG_DONE);

      /*
       * Re-render the scroller.
       */
      AsmDoMethod(ld->ld_Prop, GM_RENDER, bi, rp, bmr->bmr_Flags);
   };
   return 1;
}
METHOD_END
///

/*
 * Find out over which entry the mouse is located.
 */
//STATIC ASM LONG MouseOverEntry(REG(a0) LD *ld, REG(d0) LONG t)
STATIC ASM REGFUNC2(LONG, MouseOverEntry,
	REGPARAM(A0, LD *, ld),
	REGPARAM(D0, LONG, t))
{
   t -= ld->ld_ListArea.Top;
   if (t < 0) return -1;

   t = (t / ld->ld_EntryHeight) + ld->ld_Top;

   if (t > ld->ld_Total) t = ld->ld_Total;
   
   return t;
}
REGFUNC_END

/*
 * Perform multi-(de)selection.
 */
//STATIC ASM BOOL MultiSelect( REG(a0) LD *ld, REG(d0) ULONG active )
STATIC ASM REGFUNC2(BOOL, MultiSelect,
	REGPARAM(A0, LD *, ld),
	REGPARAM(D0, ULONG, active))
{
   LVE         *node = FindNodeQuick( ld, ld->ld_MultiStart ), *anode = FindNodeQuick( ld, active );
   BOOL         rc = FALSE;

   /*
    * Backward?
    */
   if ( ld->ld_MultiStart > active ) {
      /*
       * Loop through the entries.
       */
      for ( ; ; node = node->lve_Prev ) {
	 /*
	  *  Select entries?
	  */
	 if ( ! ld->ld_MultiMode ) {
	    /*
	     * Yes.
	     */
	    if ( ! ( node->lve_Flags & LVEF_SELECTED )) {
	       node->lve_Flags |= LVEF_SELECTED | LVEF_REFRESH;
	       rc = TRUE;
	    }
	 }
	 else
	 {
	    if (node->lve_Flags & LVEF_SELECTED)
	    {
	       node->lve_Flags &= ~LVEF_SELECTED;
	       node->lve_Flags |= LVEF_REFRESH;
	       rc = TRUE;
	    }
	 }
	 /*
	  * Done?
	  */
	 if ( node == anode )
	    return( rc );
      }
   } else {
      /*
       * Loop through the entries.
       */
      for ( ; ; node = node->lve_Next ) {
	 /*
	  *  Select entries?
	  */
	 if ( ! ld->ld_MultiMode ) {
	    /*
	     * Yes.
	     */
	    if ( ! ( node->lve_Flags & LVEF_SELECTED )) {
	       node->lve_Flags |= LVEF_SELECTED | LVEF_REFRESH;
	       rc = TRUE;
	    }
	 } else {
	    if ( node->lve_Flags & LVEF_SELECTED ) {
	       node->lve_Flags &= ~LVEF_SELECTED;
	       node->lve_Flags |= LVEF_REFRESH;
	       rc = TRUE;
	    }
	 }
	 /*
	  * Done?
	  */
	 if ( node == anode )
	    return( rc );
      }
   }
   return rc;
}
REGFUNC_END
/// GM_HITTEST
/*
 * Test if the gadget was hit.
 */
METHOD(ListClassHitTest, struct gpHitTest *, gph)
{
   LD               *ld = INST_DATA(cl, obj);
   BC               *bc = BASE_DATA(obj);
   ULONG             rc = 0;

   /*
    * Get absolute click position.
    */
   WORD l = GADGET(obj)->LeftEdge + gph->gpht_Mouse.X;
   WORD t = GADGET(obj)->TopEdge  + gph->gpht_Mouse.Y;

   if (PointInBox(&bc->bc_InnerBox, l, t))
      /*
       * Hit inside the list area?
       */
      return GMR_GADGETHIT;

   if (ld->ld_Prop)
   {
      /*
       * Route the message.
       */
      rc = ForwardMsg(obj, ld->ld_Prop, (Msg)gph);

      if (rc == GMR_GADGETHIT)
	 /*
	  * Mark the scroller active.
	  */
	 ld->ld_Flags |= LDF_PROPACTIVE;
   };
   return rc;
}
METHOD_END
///
/// GM_GOACTIVE
/*
 * They want us to go active.
 */
METHOD(ListClassGoActive, struct gpInput *, gpi)
{
   LD                *ld = INST_DATA(cl, obj);
   LVE               *lve;
   ULONG              rc = GMR_NOREUSE, last = ld->ld_LastNum;
   int                col, dx;
   struct GadgetInfo *gi = gpi->gpi_GInfo;

   /*
    * Get hit position.
    */
   int x = gpi->gpi_Mouse.X;
   int y = gpi->gpi_Mouse.Y;
   int l = x + GADGET(obj)->LeftEdge;
   int t = y + GADGET(obj)->TopEdge;

   /*
    * We do not go active if we were activated by ActivateGadget().
    */
   if (!gpi->gpi_IEvent)
      return GMR_NOREUSE;

   /*
    * Check for left mouse button down event
    */
   if ((gpi->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE) && (gpi->gpi_IEvent->ie_Code == SELECTDOWN))
   {
      /*
       * Update column positions if required
       */
      if (!(ld->ld_Flags & LDF_OFFSETS_VALID))
	 GetColumnPositions(obj, ld);

      /*
       * Step through column positions array, marking a hit if
       * mouse is within 4 pixels either side of column separator.
       */
      col = ld->ld_Columns - 1;
      while (col--)
      {
	 if (!(ld->ld_CD[col].cd_Flags & LVCF_HIDDEN))
	 {
	    dx = x - ld->ld_CD[col + 1].cd_Offset;
	    /*
	     * If hit column separator, set Dragging to TRUE, record
	     * drag column and initial drag position, draw first
	     * drag line and return GMR_MEACTIVE.
	     */
	    if ((dx >= -4) && (dx <= 4))
	    {
	       /*
		* Check for column dragging enabled.
		*/
	       if ((ld->ld_Flags & LDF_ALLOW_DRAG) || (ld->ld_CD[col].cd_Flags & LVCF_DRAGGABLE))
	       {
		  ld->ld_Flags |= LDF_DRAGGING_COLUMN;
		  ld->ld_DragColumn = col;
		  ld->ld_DragXLine  = ld->ld_CD[col+1].cd_Offset;
		  DrawDragLine(ld, gi);

		  return GMR_MEACTIVE;
	       };
	    };
	 };
      };
   };

   if (ld->ld_Prop)
   {
      /*
       * Prop hit?
       */
      if (ld->ld_Flags & LDF_PROPACTIVE)
      {
	 /*
	  * Adjust coordinates and re-direct message.
	  */
	 return ForwardMsg(obj, ld->ld_Prop, (Msg)gpi) & ~GMR_VERIFY;
      };
   }

   /*
    * Can we go active?
    */
   if (( GADGET( obj )->Flags & GFLG_DISABLED ) || ( ld->ld_Flags & LDF_READ_ONLY ) || ( ld->ld_Flags & LDF_LIST_BUSY ))
      return( GMR_NOREUSE );

   /*
    * Get the entry which lays under the mouse.
    */
   ld->ld_ActiveEntry = (ULONG)MouseOverEntry(ld, t);

   col = ld->ld_Columns;
   while (--col)
   {
      if (x >= ld->ld_CD[col].cd_Offset)
      break;
   };
   ld->ld_LastCol = col;

   /*
    * Is it in range?
    */
   if (ld->ld_ActiveEntry < ld->ld_Total)
   {
      if (lve = ld->ld_LastActive = FindNodeQuick(ld, ld->ld_ActiveEntry))
      {
	 ld->ld_LastNum = ld->ld_ActiveEntry;
	 /*
	  * If we are not a multi-select object we
	  * de-select all entries. Otherwise we mark the
	  * entry which initiated the multi-(de)select.
	  */
	 if (!(ld->ld_Flags & LDF_MULTI_SELECT)) DeSelect(ld);
	 else {
	    /*
	     * De-select entries if shift isn't down.
	     */
	    if (!(ld->ld_Flags & LDF_NOSHIFT))
	    {
	       if (!(gpi->gpi_IEvent->ie_Qualifier & (IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT)))
		  DeSelect(ld);
	    }

	    /*
	     * Multi-selection. When MultiMode is 1 we need
	     * to multi-deselect. Otherwise we multi-select.
	     */
	    if (lve->lve_Flags & LVEF_SELECTED) ld->ld_MultiMode = 1;
	    else                 ld->ld_MultiMode = 0;

	    /*
	     * Setup starting position.
	     */
	    ld->ld_MultiStart = ld->ld_ActiveEntry;
	 }
	 /*
	  * Select entry if necessary.
	  */
	 if ( ! ld->ld_MultiMode ) {
	    /*
	     * Note the time we got clicked.
	     */
	    CurrentTime( &ld->ld_Secs[ 0 ], &ld->ld_Mics[ 0 ] );
	    lve->lve_Flags |= LVEF_SELECTED | LVEF_REFRESH;
	 } else {
	    /*
	     * De-selection only in multi-mode.
	     */
	    if ( ld->ld_Flags & LDF_MULTI_SELECT ) {
	       /*
		* The same selection as the previous?
		*/
	       if ( ld->ld_ActiveEntry == last ) {
		  /*
		   * Yes, time it.
		   */
		  CurrentTime( &ld->ld_Secs[ 1 ], &ld->ld_Mics[ 1 ] );

		  /*
		   * Double clicked the selection?
		   */
		  if ( ! DoubleClick( ld->ld_Secs[ 0 ], ld->ld_Mics[ 0 ], ld->ld_Secs[ 1 ], ld->ld_Mics[ 1 ] )) {
		     /*
		      * No, deselect it.
		      */
		     lve->lve_Flags &= ~LVEF_SELECTED;
		     lve->lve_Flags |= LVEF_REFRESH;
		  }
	       } else {
		  /*
		   * Deselect the entry.
		   */
		  lve->lve_Flags &= ~LVEF_SELECTED;
		  lve->lve_Flags |= LVEF_REFRESH;
	       }
	    } else
	       lve->lve_Flags |= LVEF_SELECTED | LVEF_REFRESH;
	 }
	 /*
	  * Notify & Re-render.
	  */
	 DoNotifyMethod(obj, gi, OPUF_INTERIM, GA_ID, GADGET(obj)->GadgetID, LISTV_Entry, lve->lve_Entry,
				 LISTV_EntryNumber, ld->ld_LastNum, LISTV_LastColumn, ld->ld_LastCol, TAG_DONE);
	 DoRenderMethod(obj, gi, GREDRAW_UPDATE );
      }
      /*
       * Setup any drag and drop buffers we may need.
       */
      if ( AsmDoSuperMethodA( cl, obj, ( Msg )gpi ) == GMR_MEACTIVE )
	 ld->ld_Flags |= LDF_DRAGGABLE;

      rc = GMR_MEACTIVE;
   }
   else if (ld->ld_ActiveEntry == (ULONG)-1)
   {
      /*
       * Notify.
       */
      DoNotifyMethod(obj, gi, OPUF_INTERIM, GA_ID, GADGET(obj)->GadgetID, LISTV_Entry, -1,
			      LISTV_EntryNumber, -1, LISTV_LastColumn, ld->ld_LastCol, TAG_DONE);
   };
   return rc;
}
METHOD_END
///
/// GM_HANDLEINPUT
/*
 * Handle user input.
 */
METHOD(ListClassHandleInput, struct gpInput *, gpi)
{
   LD                *ld = INST_DATA(cl, obj);
   LVE               *lve;
   struct gpInput     hit;
   int                vc, xmin, xmax, dx;
   ULONG              rc = GMR_MEACTIVE, otop = ld->ld_Top, ntop = ld->ld_Top;
   LONG               nc;
   int                dcol = ld->ld_DragColumn;
   int                totalwidth, totalweight;
   struct GadgetInfo *gi = gpi->gpi_GInfo;
   CD                *cd, *cd2, *cd3;


   /*
    * Get mouse position.
    */
   int x = gpi->gpi_Mouse.X;
   int y = gpi->gpi_Mouse.Y;
   int l = x + GADGET(obj)->LeftEdge;
   int t = y + GADGET(obj)->TopEdge;

   /*
    * List in use?
    */
   if (ld->ld_Flags & LDF_LIST_BUSY)
      return rc;

   /*
    * Dragging column?
    */
   if (ld->ld_Flags & LDF_DRAGGING_COLUMN)
   {
      cd  = ld->ld_CD + dcol;
      cd2 = cd + 1;
      cd3 = cd + 2;

      /*
       * Return this by default, implying we want to
       * remain the active gadget.
       */
      rc = GMR_MEACTIVE;

      /*
       * If this is a mouse event (movement or button)
       */

      if (gpi->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE)
      {
	 /*
	  * Update column offsets if required
	  */
	 if (!(ld->ld_Flags & LDF_OFFSETS_VALID))
	    GetColumnPositions(obj, ld);

	 /*
	  * gpi_Mouse.X has mouse-x relative to left side of gadget
	  * hitbox. Set minimum and maximum values for x at positions
	  * of columns either side of the one being dragged. Add a
	  * minimum column width to those limits as well.
	  */
	 xmin = cd->cd_Offset + cd->cd_MinWidth;
	 xmax = min(cd->cd_Offset + cd->cd_MaxWidth, cd3->cd_Offset - cd2->cd_MinWidth);

	 if (xmax < xmin)                  /* just in case column is */
	 {                                 /* already very narrow,       */
	    xmin = xmax = cd2->cd_Offset;  /* stop user from adjusting it */
	 };

	 /*
	  * Prevent dragline from wandering outside of limits.
	  */
	 if (x < xmin) x = xmin;
	 if (x > xmax) x = xmax;

	 /*
	  * Don't update if dragline position hasn't changed.
	  */

	 if (x != ld->ld_DragXLine)
	 {
	    /*
	     * Reposition dragline by drawing again at old position
	     * using complement mode, then drawing at new position.
	     */

	    DrawDragLine(ld, gi);
	    ld->ld_DragXLine = x;
	    DrawDragLine(ld, gi);
	 }

	 /*
	  * Check for left mouse button release: implies user
	  * has stopped dragging column and it's time to recalculate
	  * the column weights.
	  */
	 if (gpi->gpi_IEvent->ie_Code == SELECTUP)
	 {
	    rc = GMR_NOREUSE;    /* we will use LMB-up event */
	    
	    /*
	     * No need to do anything if column position not changed.
	     */
	    if (dx = x - cd2->cd_Offset)
	    {
	       /*
		* Set new column position at x.
		*/
	       cd2->cd_Offset = x;
	       cd2->cd_Width -= dx;
	       cd->cd_Width  += dx;

	       /*
		* Set new weights for dragged column and the one to the
		* right of it, by redistributing the total of their
		* original weights in the ratio of their new positions
		* over the original total distance between them. Both
		* total weight and total distance remain unchanged.
		*/
	       totalweight = cd->cd_Weight + cd2->cd_Weight;
	       totalwidth  = cd3->cd_Offset - cd->cd_Offset;

	       cd2->cd_Weight = ((cd3->cd_Offset - x) * totalweight) / totalwidth;
	       cd->cd_Weight  = totalweight - cd2->cd_Weight;

	       /*
		* If we have a GadgetInfo, invoke GM_RENDER
		* to update gadget visuals.
		*/

	       if (gi)
	       {
		  DoRenderMethod(obj, gi, GREDRAW_REDRAW);

		  /*
		   * No need for GoInactive() to erase dragline:
		   */
		  ld->ld_Flags |= LDF_NEW_COLUMN_POS;
	       };
	       
	    } /* endif posn changed */
	    
	 }  /* endif LMB down */
	 /*
	  * If event was menu button down, abandon dragging
	  * and let Intuition activate menus.
	  */
	  
	 if (gpi->gpi_IEvent->ie_Code == MENUDOWN)
	    rc = GMR_REUSE;

      } /* endif mouse event */

      /*
       * If event was either shift key going down, abandon
       * dragging as per BGUI standard.
       */

      if (gpi->gpi_IEvent->ie_Class == IECLASS_RAWKEY)
      {
	 if ((gpi->gpi_IEvent->ie_Code == 0x60) || (gpi->gpi_IEvent->ie_Code == 0x61))
	    rc = GMR_REUSE;
      };
      return rc;
   };

   /*
    * Prop gadget active?
    */
   if (ld->ld_Prop && (ld->ld_Flags & LDF_PROPACTIVE))
   {
      /*
       * Adjust coordinates and route message.
       */
      return ForwardMsg(obj, ld->ld_Prop, (Msg)gpi) & ~GMR_VERIFY;
   };

   hit = *gpi;
   hit.MethodID = BASE_DRAGGING;

   nc = AsmDoMethodA(obj, (Msg)&hit);

   switch (nc)
   {
      case BDR_DROP:
	 DoNotifyMethod(obj, gi, 0, GA_ID, GADGET(obj)->GadgetID, LISTV_Entry, ld->ld_LastActive ? ld->ld_LastActive->lve_Entry : NULL, LISTV_EntryNumber, ld->ld_LastNum, TAG_END);
	 rc = GMR_VERIFY;
	 break;

      case BDR_CANCEL:
	 rc = GMR_NOREUSE;
	 break;

      case BDR_NONE:
	 /*
	  * Get the entry which lies under the mouse.
	  */
	 nc = MouseOverEntry(ld, t);
	 
	 /*
	  * There are only so much entries...
	  */
	 if (nc < 0)             nc = ld->ld_Top - 1;
	 if (nc < 0)             nc = 0;
	 if (nc >= ld->ld_Total) nc = ld->ld_Total - 1;

	 /*
	  * Let's see what we got...
	  */
	 if (gpi->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE)
	 {
	    /*
	     * We do not support drag-selection when we
	     * are in drag and drop mode.
	     */
	    if (!(ld->ld_Flags & LDF_DRAGGABLE))
	    {
	       /*
		* We only respond when:
		*    A) The entry under the mouse changed.
		*    B) The entry under the mouse is in the visible area.
		*/
	       if ((nc != ld->ld_ActiveEntry) && (nc >= ld->ld_Top) && (nc < (ld->ld_Top + ld->ld_Visible)))
	       {
		  /*
		   * Mark the new entry.
		   */
		  ld->ld_ActiveEntry = nc;

		  /*
		   * Get the node.
		   */
		  if (lve = ld->ld_LastActive = FindNodeQuick(ld, nc))
		  {
		     ld->ld_LastNum = nc;
		     /*
		      * Are we a multi-select object?
		      */
		     if (!(ld->ld_Flags & LDF_MULTI_SELECT))
		     {
			/*
			 * No. Deselect other entries.
			 */
			DeSelect(ld);

			/*
			 * We need a visual change.
			 */
			vc = TRUE;

			/*
			 * Select entry.
			 */
			lve->lve_Flags |= LVEF_SELECTED | LVEF_REFRESH;
		     } else
			/*
			 * Do a multi-(de)select.
			 */
			vc = MultiSelect( ld, nc );

		     /*
		      * Update visuals if necessary.
		      */
		     if (vc) DoRenderMethod(obj, gi, GREDRAW_UPDATE);
		     DoNotifyMethod(obj, gi, OPUF_INTERIM, GA_ID, GADGET(obj)->GadgetID, LISTV_Entry, lve->lve_Entry, LISTV_EntryNumber, ld->ld_LastNum, TAG_END );
		  }
	       }
	    }

	    /*
	     * What code do we have...
	     */
	    switch (gpi->gpi_IEvent->ie_Code)
	    {
	       case  SELECTUP:
		  /*
		   * Releasing the left button
		   * de-activates the object.
		   */
		  rc = GMR_NOREUSE | GMR_VERIFY;
		  CurrentTime(&ld->ld_Secs[0], &ld->ld_Mics[0]);
		  DoNotifyMethod(obj, gi, 0, GA_ID, GADGET(obj)->GadgetID, LISTV_Entry, ld->ld_LastActive->lve_Entry, LISTV_EntryNumber, ld->ld_LastNum, TAG_END );
		  break;

	       case  MENUDOWN:
		  /*
		   * Reuse menu events.
		   */
		  rc = GMR_REUSE | GMR_VERIFY;
		  DoNotifyMethod(obj, gi, 0, GA_ID, GADGET(obj)->GadgetID, LISTV_Entry, ld->ld_LastActive->lve_Entry, LISTV_EntryNumber, ld->ld_LastNum, TAG_END );
		  break;
	    }
	 }
	 else if (gpi->gpi_IEvent->ie_Class == IECLASS_TIMER)
	 {
	    /*
	     * When the mouse is moved above or below
	     * the visible area the entries scroll up
	     * or down using timer events for a delay.
	     */
	    if ((nc != ld->ld_ActiveEntry) && (!(ld->ld_Flags & LDF_DRAGGABLE)))
	    {
	       /*
		* When the active entry is located before
		* the top entry we scroll up one entry. When the
		* entry is located after the last visible entry
		* we scroll down one entry.
		*/
	       vc = FALSE;
	       if (nc >= ntop + ld->ld_Visible)
	       {
		  nc = ntop++ + ld->ld_Visible;
		  vc = TRUE;
	       }
	       else if (nc < ntop)
	       {
		  nc = --ntop;
		  vc = TRUE;
	       };
	       
	       if (vc)
	       {
		  /*
		   * Set the new entry.
		   */
		  ld->ld_LastNum = ld->ld_ActiveEntry = nc;

		  /*
		   * Find the entry.
		   */
		  if (lve = ld->ld_LastActive = FindNodeQuick(ld, nc))
		  {
		     /*
		      * Are we a multi-select object?
		      */
		     if (ld->ld_Flags & LDF_MULTI_SELECT)
		     {
			/*
			 * Do a multi-(de)select.
			 */
			vc = MultiSelect(ld, nc);
		     }
		     else
		     {
			/*
			 * No. Deselect all entries.
			 */
			DeSelect(ld);
			/*
			 * Select the entry.
			 */
			lve->lve_Flags |= LVEF_SELECTED | LVEF_REFRESH;
			/*
			 * We need a visual change.
			 */
			vc = TRUE;
		     };

		     /*
		      * Update visuals when necessary.
		      */
		     if (vc || (otop != ntop))
		     {
			/*
			 * Top changed?
			 */
			DoRenderMethod(obj, gi, GREDRAW_UPDATE);
			if (otop != ntop)
			{
			   NewTop(ld, gi, obj, ntop);
			   if (ld->ld_Prop)
			      DoSetMethod(ld->ld_Prop, gi, PGA_Top, ntop, TAG_END);
			};
			DoNotifyMethod(obj, gi, OPUF_INTERIM, GA_ID, GADGET(obj)->GadgetID, LISTV_Entry, lve->lve_Entry, LISTV_EntryNumber, ld->ld_LastNum, TAG_END);
		     };
		  };
	       };
	    };
	 };
	 break;
   }
   return rc;
}
METHOD_END
///
/// GM_GOINACTIVE
/*
 * Go inactive.
 */
METHOD(ListClassGoInActive, struct gpGoInactive *, ggi)
{
   LD       *ld = INST_DATA(cl, obj);

   if (ld->ld_Flags & LDF_DRAGGING_COLUMN)
   {
      if (!(ld->ld_Flags & LDF_NEW_COLUMN_POS))
	 DrawDragLine(ld, ggi->gpgi_GInfo);

      ld->ld_Flags &= ~(LDF_DRAGGING_COLUMN|LDF_NEW_COLUMN_POS);
      return 0;
   }
   else
   {
      /*
       * Clear draggable bit.
       */
      ld->ld_Flags &= ~LDF_DRAGGABLE;

      if (ld->ld_Prop)
      {
	 /*
	  * If the scroller was active pass this message on for compatibility reasons.
	  */
	 if (ld->ld_Flags & LDF_PROPACTIVE)
	 {
	    /*
	     * Mark the scroller as not active.
	     */
	    ld->ld_Flags &= ~LDF_PROPACTIVE;

	    return AsmDoMethodA(ld->ld_Prop, (Msg)ggi);
	 };
      };
      return AsmDoSuperMethodA(cl, obj, (Msg)ggi);
   };
}
METHOD_END
///
/// BASE_DIMENSIONS
/*
 * They want our minimum dimensions.
 */
METHOD(ListClassDimensions, struct bmDimensions *, bmd)
{
   LD                *ld = INST_DATA(cl, obj);
   struct BaseInfo   *bi = bmd->bmd_BInfo;
   UWORD              my, mx = 0, mpx = 0, mpy = 0;
   int                th = ld->ld_EntryHeight, col;
   struct TextAttr   *lf = ld->ld_ListFont;

   /*
    * First the prop...
    */
   if (ld->ld_Prop) AsmDoMethod(ld->ld_Prop, GRM_DIMENSIONS, bi, bi->bi_RPort, &mpx, &mpy, 0);

   if (!lf)
   {
      /*
       * Pickup superclass font.
       */
      Get_SuperAttr(cl, obj, BT_TextAttr, &lf);
   };
   /*
    * Setup entry height always even.
    */
   ld->ld_EntryHeight = (lf->ta_YSize + 1) & (ULONG)~1;

   /*
    * Calculate minimum x&y size.
    */
   my = ld->ld_EntryHeight * ld->ld_MinShown;

   for (col = 0; col < ld->ld_Columns; col++)
   {
      mx += ld->ld_CD[col].cd_MinWidth;
   };

   /*
    * The listview grows when a title
    * hook is specified.
    */
   if (ld->ld_Title)
      my += th + 4;

   mpx += mx;
   if (my > mpy) mpy = my;

   /*
    * Set it.
    */
   return CalcDimensions(cl, obj, bmd, mpx, mpy);
}
METHOD_END
///
/// WM_KEYACTIVE
/*
 * Key activation.
 */
//STATIC ASM ULONG ListClassKeyActive( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct wmKeyInput *wmki )
STATIC ASM REGFUNC3(ULONG, ListClassKeyActive,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct wmKeyInput *, wmki))
{
   LD       *ld = ( LD * )INST_DATA( cl, obj );
   UWORD        qual = wmki->wmki_IEvent->ie_Qualifier;
   LONG         nnum = 0, otop = ld->ld_Top, newtop = otop;
   LVE         *node;
   BOOL         sel = FALSE;

   /*
    * Read-only?
    */
   if ( ld->ld_Flags & LDF_READ_ONLY ) {
      /*
       * Shifted scrolls up.
       */
      if ( qual & ( IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT )) {
	 if ( newtop ) newtop--;
      } else {
	 if ( newtop < ( ld->ld_Total - ld->ld_Visible )) newtop++;
      }

      /*
       * Top changed?
       */
      if ( newtop != otop )
	 DoSetMethod( obj, wmki->wmki_GInfo, LISTV_Top, newtop, TAG_END );

      /*
       * Ignore this.
       */
      *( wmki->wmki_ID ) = WMHI_IGNORE;
   } else {
      /*
       * Find the selected node.
       */
      for ( node = ld->ld_Entries.lvl_First; node->lve_Next; node = node->lve_Next, nnum++ ) {
	 if ( node->lve_Flags & LVEF_SELECTED ) {
	    sel = TRUE;
	    break;
	 }
      }

      /*
       * Found?
       */
      if ( ! sel )
	 /*
	  * Select first entry.
	  */
	 DoSetMethod( obj, wmki->wmki_GInfo, LISTV_Select, 0L, TAG_END );
      else {
	 if ( qual & ( IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT )) {
	    /*
	     * Shifted selectes the previous entry.
	     */
	    nnum=LISTV_Select_Previous;
	 } else {
	    /*
	     * Normal the next entry.
	     */
	    nnum=LISTV_Select_Next;
	 }

	 /*
	  * Select the new entry.
	  */
	 DoSetMethod( obj, wmki->wmki_GInfo, LISTV_Select, nnum, TAG_END );
      }
      /*
       * Ignore this..
       */
      *( wmki->wmki_ID ) = WMHI_IGNORE;
   }

   return( WMKF_VERIFY );
}
REGFUNC_END

/*
 * Get entry predecessor, position and add method.
 */
//STATIC ASM VOID EntryPosHow( REG(a0) LD *ld, REG(a1) LVE **lve, REG(d0) ULONG pos, REG(a2) ULONG *how )
STATIC ASM REGFUNC4(VOID, EntryPosHow,
	REGPARAM(A0, LD *, ld),
	REGPARAM(A1, LVE **, lve),
	REGPARAM(D0, ULONG, pos),
	REGPARAM(A2, ULONG, *how))
{
   if ( ! pos ) {
      /*
       * Position of 0 means add to the head.
       */
      *how = LVAP_HEAD;
      *lve = NULL;
   } else   if ( pos >= ld->ld_Total ) {
      /*
       * A position larger or equal to the
       * number of available entries means
       * add to the tail.
       */
      *how = LVAP_TAIL;
      *lve = NULL;
   } else {
      /*
       * Look up the entry to insert after.
       */
      *how = 0L;
      *lve = FindNodeQuick( ld, pos - 1 );
   }
}
REGFUNC_END
///

/*
 * (Multi)Select entry and/or make it visible.
 */
//SAVEDS ASM VOID DoEntry(REG(a0) struct GadgetInfo *gi, REG(a1) Object *obj, REG(a2) LD *ld, REG(d0) ULONG flags, REG(d1) ULONG number)
SAVEDS ASM REGFUNC5(VOID, DoEntry,
	REGPARAM(A0, struct GadgetInfo *, gi),
	REGPARAM(A1, Object *, obj),
	REGPARAM(A2, LD *, ld),
	REGPARAM(D0, ULONG, flags),
	REGPARAM(D1, ULONG, number))
{
   ULONG       tag;

   /*
    * Make visible? Select?
    */
   if (flags & LVASF_SELECT)
   {
      /*
       * Visible or not?
       */
      tag = (flags & LVASF_NOT_VISIBLE) ? LISTV_SelectNotVisible : LISTV_Select;

      DoSetMethod(obj, gi, tag, number, TAG_DONE);
   }
   else if (flags & LVASF_MULTISELECT)
   {
      /*
       * Visible or not?
       */
      tag = (flags & LVASF_NOT_VISIBLE) ? LISTV_SelectMultiNotVisible : LISTV_SelectMulti;

      DoSetMethod(obj, gi, tag, number, TAG_DONE);
   }
   else if (flags & LVASF_MAKEVISIBLE)
   {
      /*
       * Just make it visible.
       */
      DoSetMethod(obj, gi, LISTV_MakeVisible, number, TAG_DONE);
   }
   else
   {
      /*
       * Re-render list.
       */
      ld->ld_Flags |= LDF_REFRESH_ALL;
      DoRenderMethod(obj, gi, GREDRAW_UPDATE);
   }
}
REGFUNC_END
/// LVM_INSERTENTRIES
/*
 * Insert entries.
 */
METHOD(ListClassInsertEntries, struct lvmInsertEntries *, lvmi)
{
   LD          *ld = INST_DATA( cl, obj );
   LVE         *lve;
   ULONG        rc, pos = lvmi->lvmi_Pos, how;

   /*
    * Where do we insert them.
    */
   EntryPosHow(ld, &lve, pos, &how);

   /*
    * Insert the entries.
    */
   rc = AddEntries(ld, lvmi->lvmi_Entries, obj, how, lve);

   /*
    * We want the list completely refreshed.
    */
   ld->ld_Flags |= LDF_REFRESH_ALL;

   /*
    * Render the object.
    */
   DoRenderMethod(obj, lvmi->lvmi_GInfo, GREDRAW_UPDATE);

   return rc;
}
METHOD_END
///
/// LVM_INSERTSINGLE
/*
 * Insert a single entry.
 */
METHOD(ListClassInsertSingle, struct lvmInsertSingle *, lvis)
{
   LD                      *ld = INST_DATA(cl, obj);
   struct lvmInsertEntries  lvmi;
   APTR                     entries[2];
   ULONG                    rc;

   /*
    * Set it up.
    */
   entries[0] = lvis->lvis_Entry;
   entries[1] = NULL;

   lvmi.MethodID    = LVM_INSERTENTRIES;
   lvmi.lvmi_GInfo   = lvis->lvis_GInfo;
   lvmi.lvmi_Entries = entries;
   lvmi.lvmi_Pos    = lvis->lvis_Pos;

   /*
    * Insert them.
    */
   rc = ListClassInsertEntries( cl, obj, &lvmi );

   /*
    * Select the entry or make it visible
    * or just redraw the list.
    */
   ld->ld_LastAdded->lve_Flags |= LVEF_REFRESH;

   /*
    * Make visible? Select?
    */
   DoEntry(lvis->lvis_GInfo, obj, ld, lvis->lvis_Flags, lvis->lvis_Pos >= ld->ld_Total ? ld->ld_Total - 1 : lvis->lvis_Pos );

   return rc;
}
METHOD_END
///
/// LVM_ADDENTRIES
/*
 * Add entries.
 */
METHOD(ListClassAddEntries, struct lvmAddEntries *, lva)
{
   LD          *ld = INST_DATA( cl, obj );
   ULONG        rc;

   /*
    * Add the entries.
    */
   rc = AddEntries(ld, lva->lvma_Entries, obj, lva->lvma_How, NULL);

   /*
    * We want the list completely refreshed.
    */
   ld->ld_Flags |= LDF_REFRESH_ALL;

   /*
    * Render the object.
    */
   DoRenderMethod(obj, lva->lvma_GInfo, GREDRAW_UPDATE);

   return rc;
}
METHOD_END
///
/// LVM_ADDSINGLE
/*
 * Add a single entry.
 */
METHOD(ListClassAddSingle, struct lvmAddSingle *, lva)
{
   LD          *ld = INST_DATA(cl, obj);
   APTR         entries[2];
   ULONG        number, rc;
   LVE         *tmp;

   /*
    * Set it up.
    */
   entries[0] = lva->lvma_Entry;
   entries[1] = NULL;

   /*
    * Add it.
    */
   ld->ld_LastAdded = NULL;
   rc = AddEntries(ld, entries, obj, lva->lvma_How, NULL);

   /*
    * No need to  compute the number of the
    * entry when it is added to the start or
    * the end of the list.
    */
   if      (lva->lvma_How == LVAP_HEAD) number = 0;
   else if (lva->lvma_How == LVAP_TAIL) number = ld->ld_Total - 1;
   else
   {
      /*
       * Get it's number.
       */
      for (tmp = ld->ld_Entries.lvl_First, number = 0; tmp->lve_Next; tmp = tmp->lve_Next, number++)
      {
	 if (tmp == ld->ld_LastAdded)
	    break;
      }
      /*
       * Select the entry or make it visible
       * or just redraw the list.
       */
      tmp->lve_Flags |= LVEF_REFRESH;
   }

   /*
    * New top visible entry?
    */
   if (number == ld->ld_Top)
      ld->ld_TopEntry = ld->ld_LastAdded;

   /*
    * Make visible? Select?
    */
   DoEntry(lva->lvma_GInfo, obj, ld, lva->lvma_Flags, number);

   return rc;
}
METHOD_END
///
/// LVM_CLEAR
/*
 * Clear the entire list.
 */
METHOD(ListClassClear, struct lvmCommand *, lvc)
{
   LD                *ld = INST_DATA(cl, obj);
   LVE               *lve;
   struct lvResource  lvr;

   /*
    * Initialize lvResource structure to kill the entry.
    */
   lvr.lvr_Command = LVRC_KILL;

   /*
    * Say were busy.
    */
   ld->ld_Flags |= LDF_LIST_BUSY;

   /*
    * Free all entries.
    */
   while (lve = (LVE *)RemHead((struct List *)&ld->ld_Entries))
   {
      /*
       * Do we have a resource hook?
       */
      if (ld->ld_Resource)
      {
	 /*
	  * Setup the entry.
	  */
	 lvr.lvr_Entry = lve->lve_Entry;

	 /*
	  * Call the hook.
	  */
	 BGUI_CallHookPkt(ld->ld_Resource, (VOID *)obj, (VOID *)&lvr);
      }
      else
      {
	 /*
	  * Simple deallocation.
	  */
	 BGUI_FreePoolMem(lve->lve_Entry);
      };
      /*
       * Free the entry node.
       */
      BGUI_FreePoolMem(lve);
   }

   /*
    * Zero entries.
    */
   ld->ld_Total      = ld->ld_Top = 0;
   ld->ld_TopEntry   = NULL;
   ld->ld_LastActive = NULL;
   ld->ld_LastNum    = 0;
   ld->ld_LastAdded  = NULL;

   /*
    * We ain't busy no more.
    */
   ld->ld_Flags &= ~LDF_LIST_BUSY;

   /*
    * Notify a NULL entry.
    */
   DoNotifyMethod(obj, lvc->lvmc_GInfo, 0, GA_ID, GADGET(obj)->GadgetID, LISTV_Entry, NULL, TAG_END);

   /*
    * We need a complete refresh.
    */
   DoRenderMethod(obj, lvc->lvmc_GInfo, GREDRAW_REDRAW);

   return 1;
}
METHOD_END
///
/*
 * Find a node by it's entry data (slow!).
 */
//STATIC ASM LVE *FindEntryData(REG(a0) LD *ld, REG(a1) APTR data, REG(a2) ULONG *number)
STATIC ASM REGFUNC3(LVE *, FindEntryData,
	REGPARAM(A0, LD *, ld),
	REGPARAM(A1, APTR, data),
	REGPARAM(A2, ULONG *, number))
{
   LVE         *lve;
   ULONG        num = 0;

   for (lve = ld->ld_Entries.lvl_First; lve->lve_Next; lve = lve->lve_Next, num++)
   {
      if (lve->lve_Entry == data)
      {
	 if (number) *number = num;
	 return lve;
      }
   }
   return NULL;
}
REGFUNC_END

/*
 * Find a node by it's entry data (can be fast!).
 */
//STATIC ASM LVE *FindEntryDataF(REG(a0) LD *ld, REG(a1) APTR data)
STATIC ASM REGFUNC2(LVE *, FindEntryDataF,
	REGPARAM(A0, LD *, ld),
	REGPARAM(A1, APTR, data))
{
   LVE         *lve;

   if (data == ld->ld_ScanEntry)
      return ld->ld_ScanNode;

   for (lve = ld->ld_Entries.lvl_First; lve->lve_Next; lve = lve->lve_Next)
   {
      if (lve->lve_Entry == data)
	 return lve;
   }
   return NULL;
}
REGFUNC_END

/// LVM_GETENTRY
/*
 * Get an entry.
 */
//STATIC ASM ULONG ListClassGetEntry( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct lvmGetEntry *lvg )
STATIC ASM REGFUNC3(ULONG, ListClassGetEntry,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvmGetEntry *, lvg))
{
   LD       *ld = ( LD * )INST_DATA( cl, obj );
   LVE         *lve;
   ULONG        rc = 0L;

   /*
    * Were busy.
    */
   ld->ld_Flags |= LDF_LIST_BUSY;

   /*
    * What do they want?
    */
   switch ( lvg->MethodID ) {

      case  LVM_FIRSTENTRY:
	 /*
	  * The first entry.
	  */
	 if ( ld->ld_Total ) {
	    /*
	     * Selected entry?
	     */
	    if ( lvg->lvmg_Flags & LVGEF_SELECTED ) {
	       /*
		* Scan the list.
		*/
	       for ( lve = ld->ld_Entries.lvl_First; lve->lve_Next; lve = lve->lve_Next ) {
		  /*
		   * Found?
		   */
		  if ( lve->lve_Flags & LVEF_SELECTED ) {
		     ld->ld_ScanNode  = lve;
		     ld->ld_ScanEntry = lve->lve_Entry;
		     rc = ( ULONG )lve->lve_Entry;
		     break;
		  }
	       }
	    } else {
	       /*
		* Normal first entry.
		*/
	       if ( ld->ld_Entries.lvl_First->lve_Next ) {
		  ld->ld_ScanNode  = ld->ld_Entries.lvl_First;
		  ld->ld_ScanEntry = ld->ld_ScanNode->lve_Entry;
		  rc = ( ULONG )ld->ld_ScanEntry;
	       }
	    }
	 }
	 break;

      case  LVM_LASTENTRY:
	 /*
	  * The last entry.
	  */
	 if ( ld->ld_Total ) {
	    /*
	     * Selected entry?
	     */
	    if ( lvg->lvmg_Flags & LVGEF_SELECTED ) {
	       /*
		* Scan the list.
		*/
	       for ( lve = ld->ld_Entries.lvl_Last; ; lve = lve->lve_Prev ) {
		  /*
		   * Found?
		   */
		  if ( lve->lve_Flags & LVEF_SELECTED ) {
		     ld->ld_ScanNode  = lve;
		     ld->ld_ScanEntry = lve->lve_Entry;
		     rc = ( ULONG )lve->lve_Entry;
		     break;
		  }
		  /*
		   * Done?
		   */
		  if ( lve == ld->ld_Entries.lvl_First )
		     break;
	       }
	    } else {
	       /*
		* Normal last entry.
		*/
	       if ( ld->ld_Entries.lvl_First->lve_Next ) {
		  ld->ld_ScanNode  = ld->ld_Entries.lvl_Last;
		  ld->ld_ScanEntry = ld->ld_ScanNode->lve_Entry;
		  rc = ( ULONG )ld->ld_ScanEntry;
	       }
	    }
	 }
	 break;

      case  LVM_NEXTENTRY:
	 /*
	  * Valid entry?
	  */
	 if ( lve = FindEntryDataF( ld, lvg->lvmg_Previous )) {
	    /*
	     * Is there a next one?
	     */
	    if ( lve != ld->ld_Entries.lvl_Last ) {
	       /*
		* Selected entry?
		*/
	       if ( lvg->lvmg_Flags & LVGEF_SELECTED ) {
		  /*
		   * Scan the list.
		   */
		  for ( lve = lve->lve_Next; lve->lve_Next; lve = lve->lve_Next ) {
		     /*
		      * Found?
		      */
		     if ( lve->lve_Flags & LVEF_SELECTED ) {
			ld->ld_ScanNode  = lve;
			ld->ld_ScanEntry = lve->lve_Entry;
			rc = ( ULONG )lve->lve_Entry;
			break;
		     }
		  }
	       } else {
		  /*
		   * Normal next entry.
		   */
		  ld->ld_ScanNode  = lve->lve_Next;
		  ld->ld_ScanEntry = lve->lve_Next->lve_Entry;
		  rc = ( ULONG )ld->ld_ScanEntry;
	       }
	    }
	 }
	 break;

      case  LVM_PREVENTRY:
	 /*
	  * Valid entry?
	  */
	 if ( lve = FindEntryDataF( ld, lvg->lvmg_Previous )) {
	    /*
	     * Is there a previous one?
	     */
	    if ( lve != ld->ld_Entries.lvl_First ) {
	       /*
		* Selected entry?
		*/
	       if ( lvg->lvmg_Flags & LVGEF_SELECTED ) {
		  /*
		   * Scan the list.
		   */
		  for ( lve = lve->lve_Prev; ; lve = lve->lve_Prev ) {
		     /*
		      * Found?
		      */
		     if ( lve->lve_Flags & LVEF_SELECTED ) {
			ld->ld_ScanNode  = lve;
			ld->ld_ScanEntry = lve->lve_Entry;
			rc = ( ULONG )lve->lve_Entry;
			break;
		     }
		     /*
		      * Done?
		      */
		     if ( lve == ld->ld_Entries.lvl_First )
			break;
		  }
	       } else {
		  /*
		   * Normal previous entry.
		   */
		  ld->ld_ScanNode  = lve->lve_Prev;
		  ld->ld_ScanEntry = lve->lve_Prev->lve_Entry;
		  rc = ( ULONG )ld->ld_ScanEntry;
	       }
	    }
	 }
	 break;
   }

   /*
    * Were not busy anymore.
    */
   ld->ld_Flags &= ~LDF_LIST_BUSY;
   return rc;
}
REGFUNC_END
///
/// LVM_REMENTRY
/*
 * Remove an entry from the list.
 */
//STATIC ASM ULONG ListClassRemEntry( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct lvmRemEntry *lvmr )
STATIC ASM REGFUNC3(ULONG, ListClassRemEntry,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvmRemEntry *, lvmr))
{
   LD          *ld = INST_DATA(cl, obj);
   LVE         *lve;
   struct lvResource  lvr;
   ULONG        rc = 0L;

   /*
    * Mark us as busy.
    */
   ld->ld_Flags |= LDF_LIST_BUSY;

   /*
    * Find the entry node.
    */
   if (lve = FindEntryData(ld, lvmr->lvmr_Entry, NULL))
   {
      /*
       * Remove the node.
       */
      Remove(( struct Node * )lve );

      /*
       * The last clicked one?
       */
      if ( lve == ld->ld_LastActive ) {
	 ld->ld_LastActive = NULL;
	 ld->ld_LastNum   = 0L;
      }

      /*
       * Send NULL notification when this is
       * a single-select listview and the entry
       * is selected.
       */
      if (( ! ( ld->ld_Flags & LDF_MULTI_SELECT )) && ( lve->lve_Flags & LVEF_SELECTED ))
	 DoNotifyMethod( obj, lvmr->lvmr_GInfo, 0L, GA_ID, GADGET( obj )->GadgetID, LISTV_Entry, NULL, TAG_END );

      /*
       * Resource hook?
       */
      if ( ld->ld_Resource ) {
	 /*
	  * Init structure.
	  */
	 lvr.lvr_Command = LVRC_KILL;
	 lvr.lvr_Entry  = lve->lve_Entry;

	 /*
	  * Call the hook.
	  */
	 rc = ( ULONG )BGUI_CallHookPkt(( void * )ld->ld_Resource, ( void * )obj, ( void * )&lvr );
      } else {
	 /*
	  * Simple de-allocation
	  */
	 BGUI_FreePoolMem( lve->lve_Entry );
	 rc = 1L;
      }
      /*
       * Free node.
       */
      BGUI_FreePoolMem( lve );

      /*
       * One less entry.
       */
      ld->ld_Total--;

      /*
       * Not busy anymore.
       */
      ld->ld_Flags &= ~LDF_LIST_BUSY;

      /*
       * Refresh list.
       */
      DoRenderMethod( obj, lvmr->lvmr_GInfo, GREDRAW_REDRAW );
   }
   ld->ld_Flags &= ~LDF_LIST_BUSY;
   return rc;
}
REGFUNC_END
///
/// LVM_REMSELECTED
/*
 * Remove the selected entry from the list and
 * select the next/previous one.
 */
METHOD(ListClassRemSelected, struct lvmCommand *, lvmc)
{
   LD                 *ld = INST_DATA(cl, obj);
   LVE                *lve, *sel = NULL;
   ULONG               rc = 0;

   /*
    * Scan the selected entry.
    */
   while (FirstSelected(obj))
   {
      /*
       * Pick up it's node.
       */
      lve = ld->ld_ScanNode;

      /*
       * Was it the last one?
       */
      if (lve == ld->ld_Entries.lvl_Last)
      {
	 /*
	  * Also the first one?
	  */
	 if (lve != ld->ld_Entries.lvl_First)
	 {
	    /*
	     * No. Deselect entry and select
	     * it's predecessor.
	     */
	    lve->lve_Flags &= ~LVEF_SELECTED;
	    sel             = lve->lve_Prev;

	    /*
	     * Setup selection data.
	     */
	    if (lve == ld->ld_LastActive)
	    {
	       ld->ld_LastActive = sel;
	       ld->ld_LastNum--;
	    };
	 }
	 else
	 {
	    sel = NULL;
	 };
      }
      else
      {
	 /*
	  * Deselect entry and select it's successor.
	  */
	 lve->lve_Flags &= ~LVEF_SELECTED;
	 sel             = lve->lve_Next;

	 /*
	  * Setup selection data.
	  */
	 if (lve == ld->ld_LastActive)
	    ld->ld_LastActive = sel;
      }

      /*
       * Remove entry.
       */
      AsmDoMethod(obj, LVM_REMENTRY, NULL, lve->lve_Entry);

      rc++;
   }
   if (sel)
   {
      /*
       * Notify new entry.
       */
      sel->lve_Flags |= (LVEF_SELECTED | LVEF_REFRESH);
      DoNotifyMethod(obj, lvmc->lvmc_GInfo, 0L, GA_ID, GADGET(obj)->GadgetID, LISTV_Entry, sel->lve_Entry, TAG_END);
   };
   if (rc)
   {
      AsmDoMethod(obj, LVM_REFRESH, lvmc->lvmc_GInfo);
   };
   return rc;
}
METHOD_END
///
/// LVM_REFRESH
/*
 * Refresh the listview.
 */
METHOD(ListClassRefresh, struct lvmCommand *, lvmc)
{
   return DoRenderMethod(obj, lvmc->lvmc_GInfo, GREDRAW_REDRAW);
}
METHOD_END
///
/// LVM_REDRAW
/*
 * Redraw the listview entries.
 */
METHOD(ListClassRedraw, struct lvmCommand *, lvmc)
{
   LD       *ld = INST_DATA(cl, obj);

   ld->ld_Flags |= LDF_REFRESH_ALL;
   return DoRenderMethod(obj, lvmc->lvmc_GInfo, GREDRAW_UPDATE);
}
METHOD_END
///
/// LVM_REDRAWSINGLE
METHOD(ListClassRedrawSingle, struct lvmRedrawSingle *, lvrs)
{
   LD                *ld = INST_DATA(cl, obj);
   LVE               *lve;
   ULONG              rc;

   if (!(lvrs->lvrs_Flags & LVRF_ALL_COLUMNS))
   {
      ld->ld_OneColumn = lvrs->lvrs_Column;
      ld->ld_Flags |= LDF_ONE_COLUMN;
   };

   if (!(lvrs->lvrs_Flags & LVRF_ALL_ENTRIES))
   {
      if (lve = FindEntryDataF(ld, lvrs->lvrs_Entry))
	 lve->lve_Flags |= LVEF_REFRESH;
   }
   else
   {
      ld->ld_Flags |= LDF_REFRESH_ALL;
   };
   rc = DoRenderMethod(obj, lvrs->lvrs_GInfo, GREDRAW_UPDATE);

   ld->ld_Flags &= ~LDF_ONE_COLUMN;

   return rc;
}
METHOD_END
///
/// LVM_SORT
/*
 * Sort the list.
 */
//STATIC ASM ULONG ListClassSort( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct lvmCommand *lvmc )
STATIC ASM REGFUNC3(ULONG, ListClassSort,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvmCommand *, lvmc))
{
   LD       *ld = ( LD * )INST_DATA( cl, obj );
   LVE         *lve;
   LVL          buffer;

   /*
    * Do we have entries?
    */
   if ( ld->ld_Total ) {
      /*
       * Were busy.
       */
      ld->ld_Flags |= LDF_LIST_BUSY;

      /*
       * Initialize buffer.
       */
      NewList(( struct List * )&buffer );

      /*
       * Attach all entries to the buffer.
       */
      while ( lve = ( LVE * )RemHead(( struct List * )&ld->ld_Entries ))
	 AddTail(( struct List * )&buffer, ( struct Node * )lve );

      /*
       * And put them back again sorted.
       */
      while ( lve = ( LVE * )RemHead(( struct List * )&buffer ))
	 AddEntryInList( ld, obj, lve, LVAP_SORTED );

      /*
       * Were not busy anymore.
       */
      ld->ld_Flags &= ~LDF_LIST_BUSY;

      /*
       * Refresh the list.
       */
      ld->ld_Flags |= LDF_REFRESH_ALL;
      DoRenderMethod( obj, lvmc->lvmc_GInfo, GREDRAW_UPDATE );
   }
   return 1;
}
REGFUNC_END
///
/// LVM_LOCK, LVM_UNLOCK
/*
 * (Un)lock the list.
 */
//STATIC ASM ULONG ListClassLock( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct lvmCommand *lvmc )
STATIC ASM REGFUNC3(ULONG, ListClassLock,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvmCommand *, lvmc))
{
   LD       *ld = INST_DATA(cl, obj);

   switch (lvmc->MethodID)
   {
   case LVM_LOCKLIST:
      ld->ld_Flags |= LDF_LIST_BUSY;
      break;

   case LVM_UNLOCKLIST:
      ld->ld_Flags &= ~LDF_LIST_BUSY;
      DoRenderMethod(obj, lvmc->lvmc_GInfo, GREDRAW_REDRAW);
      break;
   }
   return 1;
}
REGFUNC_END
///
/// LVM_MOVE
/*
 * Move an entry.
 */
//STATIC ASM ULONG ListClassMove( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct lvmMove *lvm )
STATIC ASM REGFUNC3(ULONG, ListClassMove,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvmMove *, lvm))
{
   LD       *ld = ( LD * )INST_DATA( cl, obj );
   LVE         *lve, *tmp;
   ULONG        rc = 0L, num = 0L, cpos;

   /*
    * Look up the entry.
    */
   if ( ! lvm->lvmm_Entry ) lve = ld->ld_LastActive;
   else         lve = FindEntryData( ld, lvm->lvmm_Entry, &cpos );

   if ( lve ) {
      /*
       * Lock the list.
       */
      ld->ld_Flags |= LDF_LIST_BUSY;

      /*
       * Move to?
       */
      switch ( lvm->lvmm_Direction ) {

	 case  LVMOVE_UP:
	    /*
	     * Already at the top?
	     */
	    if ( lve != ld->ld_Entries.lvl_First ) {
	       /*
		* Pick up new predeccessor.
		*/
	       tmp = lve->lve_Prev->lve_Prev;

	       /*
		* Do it.
		*/
	       goto insertIt;
	    }
	    break;

	 case  LVMOVE_DOWN:
	    /*
	     * Already at the bottom?
	     */
	    if ( lve != ld->ld_Entries.lvl_Last ) {
	       /*
		* Pick up new predeccessor.
		*/
	       tmp = lve->lve_Next;

	       insertIt:

	       /*
		* Remove the node.
		*/
	       Remove(( struct Node * )lve );

	       /*
		* Insert it into it's new spot.
		*/
	       Insert(( struct List * )&ld->ld_Entries, ( struct Node * )lve, ( struct Node * )tmp );
	       rc = 1L;
	    }
	    break;

	 case  LVMOVE_TOP:
	    /*
	     * Already at the top?
	     */
	    if ( lve != ld->ld_Entries.lvl_First ) {
	       /*
		* Remove the node.
		*/
	       Remove(( struct Node * )lve );

	       /*
		* Insert it into it's new spot.
		*/
	       AddHead(( struct List * )&ld->ld_Entries, ( struct Node * )lve );

	       /*
		* The number is known.
		*/
	       num = 0;
	       rc  = 1L;
	       ld->ld_Flags &= ~LDF_LIST_BUSY;
	       goto gotNum;
	    }
	    break;

	 case  LVMOVE_BOTTOM:
	    /*
	     * Already at the bottom?
	     */
	    if ( lve != ld->ld_Entries.lvl_Last ) {
	       /*
		* Remove the node.
		*/
	       Remove(( struct Node * )lve );

	       /*
		* Insert it into it's new spot.
		*/
	       AddTail(( struct List * )&ld->ld_Entries, ( struct Node * )lve );

	       /*
		* The number is known.
		*/
	       num = ld->ld_Total - 1;
	       rc  = 1L;
	       ld->ld_Flags &= ~LDF_LIST_BUSY;
	       goto gotNum;
	    }
	    break;

	 case  LVMOVE_NEWPOS:
	    /*
	     * Current position changed?
	     */
	    if ( cpos != lvm->lvmm_NewPos ) {
	       /*
		* New position 0?
		*/
	       if ( lvm->lvmm_NewPos == 0 ) {
		  Remove(( struct Node * )lve );
		  AddHead(( struct List * )&ld->ld_Entries, ( struct Node * )lve );
		  num = 0;
	       } else if ( lvm->lvmm_NewPos >= ld->ld_Total ) {
		  Remove(( struct Node * )lve );
		  AddTail(( struct List * )&ld->ld_Entries, ( struct Node * )lve );
		  num = ld->ld_Total - 1;
	       } else {
		  /*
		   * Not at the start and not at the end. Find the predecessor
		   * of the place we drop the this node.
		   */
		  tmp = FindNodeQuick( ld, lvm->lvmm_NewPos - 1 );

		  /*
		   * If we are our precedecessor ourselves
		   * we take the one before us.
		   */
		  if ( tmp == lve ) tmp = tmp->lve_Prev;

		  /*
		   * Remove the node from it's current location
		   * and insert back in it's new place.
		   */
		  Remove(( struct Node * )lve );
		  Insert(( struct List * )&ld->ld_Entries, ( struct Node * )lve, ( struct Node * )tmp );

		  /*
		   * The number is known.
		   */
		  num = lvm->lvmm_NewPos;
	       }
	       rc = 1L;
	       ld->ld_Flags &= ~LDF_LIST_BUSY;
	       goto gotNum;
	    }
	    break;
      }
      /*
       * List changed?
       */
      if ( rc ) {
	 /*
	  * Not busy anymore.
	  */
	 ld->ld_Flags &= ~LDF_LIST_BUSY;

	 /*
	  * Find out it's number.
	  */
	 for ( tmp = ld->ld_Entries.lvl_First; tmp->lve_Next; tmp = tmp->lve_Next, num++ ) {
	    /*
	     * Is this the one?
	     */
	    if ( tmp == lve ) {
	       gotNum:
	       /*
		* Was it selected?
		*/
	       if (lve->lve_Flags & LVEF_SELECTED)
		  /*
		   * Setup it's number.
		   */
		  ld->ld_LastNum = num;

	       /*
		* Make the moved entry visible.
		*/
	       ld->ld_Flags |= LDF_REFRESH_ALL;
	       DoSetMethod( obj, lvm->lvmm_GInfo, LISTV_MakeVisible, num, TAG_END );

	       /*
		* Notify and setup the new position.
		*/
	       DoNotifyMethod( obj, lvm->lvmm_GInfo, 0L, GA_ID, GADGET( obj )->GadgetID, LISTV_NewPosition, num, TAG_END );
	       ld->ld_NewPos = num;
	       return rc;
	    }
	 }
      }
   }
   /*
    * Not busy anymore.
    */
   ld->ld_Flags &= ~LDF_LIST_BUSY;
   return rc;
}
REGFUNC_END
///
/// LVM_REPLACE
/*
 * Replace an entry.
 */
//STATIC ASM ULONG ListClassReplace( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct lvmReplace *lvmr )
STATIC ASM REGFUNC3(ULONG, ListClassReplace,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvmReplace *, lvmr))
{
   LD       *ld = ( LD * )INST_DATA( cl, obj );
   LVE         *lvo;
   struct lvResource  lvr;
   APTR         newdata;
   ULONG        rc = 0L;

   /*
    * Data OK?
    */
   if (( ! lvmr->lvmr_OldEntry ) || ( ! lvmr->lvmr_NewEntry ))
      return( 0L );

   /*
    * Were busy.
    */
   ld->ld_Flags |= LDF_LIST_BUSY;

   /*
    * Find the old entry.
    */
   if (lvo = FindEntryData(ld, lvmr->lvmr_OldEntry, NULL))
   {
      /*
       * Create the new entry.
       */
      if ( ld->ld_Resource ) {
	 /*
	  * Init structure.
	  */
	 lvr.lvr_Command = LVRC_MAKE;
	 lvr.lvr_Entry  = lvmr->lvmr_NewEntry;

	 /*
	  * Call the hook.
	  */
	 if ( newdata = ( APTR )BGUI_CallHookPkt(( void * )ld->ld_Resource, ( void * )obj, ( void * )&lvr )) {
	    /*
	     * Free the old entry and setup the new one.
	     */
	    lvr.lvr_Command = LVRC_KILL;
	    lvr.lvr_Entry  = lvmr->lvmr_OldEntry;
	    BGUI_CallHookPkt(( void * )ld->ld_Resource, ( void * )obj, ( void * )&lvr );
	    lvo->lve_Entry = newdata;
	    lvo->lve_Flags |= LVEF_REFRESH;
	    rc = ( ULONG )newdata;
	 }
      } else {
	 /*
	  * Allocate a string copy of the new data.
	  */
	 if ( newdata = ( APTR )BGUI_AllocPoolMem( strlen(( UBYTE * )lvmr->lvmr_NewEntry ) + 1 )) {
	    /*
	     * Copy it.
	     */
	    strcpy(( UBYTE * )newdata, ( UBYTE * )lvmr->lvmr_NewEntry );

	    /*
	     * Free the old entry, and setup the new one.
	     */
	    BGUI_FreePoolMem( lvmr->lvmr_OldEntry );
	    lvo->lve_Entry = newdata;
	    lvo->lve_Flags |= LVEF_REFRESH;
	    rc = ( ULONG )newdata;
	 }
      }
   }

   /*
    * Were not busy anymore.
    */
   ld->ld_Flags &= ~LDF_LIST_BUSY;

   if ( rc ) DoRenderMethod( obj, lvmr->lvmr_GInfo, GREDRAW_UPDATE );
   return( rc );
}
REGFUNC_END
///
/// LVM_SETCOLUMNATTRS

METHOD(ListClassSetColumnAttrs, struct lvmColumnAttrs *, lvca)
{
   LD     *ld = INST_DATA(cl, obj);
   ULONG   rc;

   rc = BGUI_PackStructureTags(&ld->ld_CD[lvca->lvca_Column], ColumnPackTable, (struct TagItem *)&lvca->lvca_AttrList);

   if (rc) AsmDoMethod(obj, LVM_REDRAW, lvca->lvca_GInfo);

   return rc;
}
METHOD_END
///
/// LVM_GETCOLUMNATTRS

METHOD(ListClassGetColumnAttrs, struct lvmColumnAttrs *, lvca)
{
   LD     *ld = INST_DATA(cl, obj);

   return BGUI_UnpackStructureTags(&ld->ld_CD[lvca->lvca_Column], ColumnPackTable, (struct TagItem *)&lvca->lvca_AttrList);
}
METHOD_END
///
/// BASE_DRAGQUERY
/*
 * Query if we accept data from the dragged object. We only accept when:
 *
 * A) The querying object is us.
 * B) We are in LISTV_ShowDropSpot mode.
 * C) The mouse is located inside the list view area.
 *
 * All other instances are refused.
 */
METHOD(ListClassDragQuery, struct bmDragPoint *, bmdp)
{
   LD       *ld = INST_DATA( cl, obj );

   if (bmdp->bmdp_Source == obj && ld->ld_Flags & LDF_SHOWDROPSPOT)
   {
      if (bmdp->bmdp_Mouse.X >= 0 &&
	  bmdp->bmdp_Mouse.Y >= 0 &&
	  bmdp->bmdp_Mouse.X < ld->ld_InnerBox.Width &&
	  bmdp->bmdp_Mouse.Y < ld->ld_InnerBox.Height)
	 return BQR_ACCEPT;
   }
   return BQR_REJECT;
}
METHOD_END
///
/// BASE_DRAGACTIVE
/*
 * Show us being the active drop object.
 */
METHOD(ListClassDragActive, struct bmDragMsg *, bmdm)
{
   LD                *ld = INST_DATA(cl, obj);
   struct BaseInfo   *bi;

   ld->ld_DropSpot=ld->ld_DrawSpot = ~0;
   
   /*
    * Drop anywhere or do we have to mark the spot?
    */
   if ((!(ld->ld_Flags & LDF_SHOWDROPSPOT)) || (!ld->ld_Entries.lvl_First->lve_Next))
   {
      /*
       * Anywhere or the list is empty. Simply place a dotted line around the view area.
       */
#ifdef DEBUG_BGUI
      if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_GadgetInfo, bmdm->bmdm_GInfo, BI_RastPort, NULL, TAG_DONE))
#else
      if (bi = AllocBaseInfo(BI_GadgetInfo, bmdm->bmdm_GInfo, BI_RastPort, NULL, TAG_DONE))
#endif
      {
	 ld->ld_Flags |= LDF_MOVE_DROPBOX;
	 /*
	  * Draw the box.
	  */
	 DottedBox(bi, &ld->ld_InnerBox);
	 FreeBaseInfo(bi);
      };
   };
   return 1;
}
METHOD_END
///
/// BASE_DRAGINACTIVE
/*
 * Deactivate.
 */
//STATIC ASM ULONG ListClassDragInactive( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct bmDragMsg *bmdm )
STATIC ASM REGFUNC3(ULONG, ListClassDragInactive,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct bmDragMsg *, bmdm))
{
   LD                   *ld = INST_DATA(cl, obj);

   /*
    * Clear drop spot.
    */
   if (ld->ld_LineBuffer)
   {
      BGUI_FreeRPortBitMap(ld->ld_LineBuffer);
      ld->ld_LineBuffer = NULL;
   };
   ld->ld_DropSpot = ~0;
   ld->ld_Flags &= ~LDF_MOVE_DROPBOX;

   return AsmDoSuperMethodA(cl, obj, (Msg)bmdm);
}
REGFUNC_END
///
/// BASE_DRAGUPDATE
/*
 * Update drop position.
 */
METHOD(ListClassDragUpdate, struct bmDragPoint *, bmdp)
{
   LD                   *ld = INST_DATA(cl, obj);
   struct IBox          *ib;
   struct GadgetInfo    *gi = bmdp->bmdp_GInfo;
   struct BaseInfo      *bi;
   
   int       dpos, otop = ld->ld_Top, ntop = otop;
   
   int x = bmdp->bmdp_Mouse.X;
   int y = bmdp->bmdp_Mouse.Y;
   int x1, y1, w, h;

   if (ld->ld_Flags & LDF_MOVE_DROPBOX)
   {
#ifdef DEBUG_BGUI
      if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_GadgetInfo, gi, BI_RastPort, NULL, TAG_DONE))
#else
      if (bi = AllocBaseInfo(BI_GadgetInfo, gi, BI_RastPort, NULL, TAG_DONE))
#endif
      {
	 DottedBox(bi, &ld->ld_InnerBox);
	 FreeBaseInfo(bi);
      };
   };

   /*
    * Keep track of the drop position?
    */
   if (ld->ld_Flags & LDF_SHOWDROPSPOT && ld->ld_Entries.lvl_First->lve_Next)
   {
      /*
       * Reject when we are out of the list bounds.
       */
      if ((x < 0) || (x >= ld->ld_InnerBox.Width))
      {
	 /*
	  * We deactivate when the mouse has left us out of the hitbox.
	  */
	 ld->ld_DropSpot = ld->ld_DrawSpot = ~0;
	 return BUR_ABORT;
      };

      /*
       * Make the y position relative to the window.
       */
      Get_SuperAttr(cl, obj, BT_HitBox, &ib);
      y += ib->Top + (ld->ld_EntryHeight >> 1);

      /*
       * Get the entry under the mouse.
       */
      dpos = MouseOverEntry(ld, y);

      /*
       * New position above or below the
       * visible entries?
       */
      if (dpos < ntop)
      {
	 dpos = --ntop;
      }
      else if (dpos > (ntop + ld->ld_Visible))
      {
	 dpos = ++ntop + ld->ld_Visible;
      }

      if (ntop > ld->ld_Total - ld->ld_Visible)
	  ntop = ld->ld_Total - ld->ld_Visible;
      if (ntop < 0)            ntop = 0;

      if (dpos < 0)            dpos = 0;
      if (dpos > ld->ld_Total) dpos = ld->ld_Total;

      /*
       * Position changed?
       */
      if (dpos != ld->ld_DropSpot)
      {
	 /*
	  * Yes. Get RastPort.
	  */
	 if (gi)
	 {
	    x1 = ld->ld_ListArea.Left;
	    y1 = ld->ld_ListArea.Top;
	    w  = ld->ld_ListArea.Width;
	    h  = ld->ld_ListArea.Height;

	    /*
	     * Re-render the current entry.
	     */
	    if ((ld->ld_DrawSpot != (UWORD)~0) && ld->ld_LineBuffer)
	    {
#ifdef DEBUG_BGUI
	       if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_GadgetInfo, gi, BI_RastPort, NULL, TAG_DONE))
#else
	       if (bi = AllocBaseInfo(BI_GadgetInfo, gi, BI_RastPort, NULL, TAG_DONE))
#endif
	       {
		  /*
		   * Fix line at the old position.
		   */
		  ClipBlit(ld->ld_LineBuffer, 0, 0, bi->bi_RPort, x1, ld->ld_DrawSpot, w, 1, 0xC0);
		  FreeBaseInfo(bi);
	       };
	    };
	    
	    /*
	     * Scroll if necessary.
	     */
	    if (ntop != otop)
	    {
	       DoSetMethod(obj, gi, LISTV_Top, ntop, TAG_DONE);
	    };

	    /*
	     * Mark new position.
	     */
	    ld->ld_DropSpot = dpos;

	    /*
	     * Setup y position.
	     */
	    y = range((dpos - ld->ld_Top) * ld->ld_EntryHeight, 0, h - 1) + y1;

	    ld->ld_DrawSpot = y;

#ifdef DEBUG_BGUI
	    if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_GadgetInfo, gi, BI_RastPort, NULL, TAG_DONE))
#else
	    if (bi = AllocBaseInfo(BI_GadgetInfo, gi, BI_RastPort, NULL, TAG_DONE))
#endif
	    {
	       if (!ld->ld_LineBuffer) ld->ld_LineBuffer = BGUI_CreateRPortBitMap(bi->bi_RPort, w, 1, 0);

	       if (ld->ld_LineBuffer)
	       {
		  /*
		   * Copy the old line to a buffer.
		   */
		  ClipBlit(bi->bi_RPort, x1, y, ld->ld_LineBuffer, 0, 0, w, 1, 0xC0);

		  /*
		   * Render line at the new position.
		   */
		  SetDashedLine(bi, 0);
		  HLine(bi->bi_RPort, x1, y, x1 + w - 1);
	       };
	       FreeBaseInfo(bi);
	    };
	 };
      };
   }
   else
   {
      /*
       * Reject when we are out of the list bounds.
       */
      if ((x < 0) || (x >= ld->ld_InnerBox.Width) ||
	  (y < 0) || (y >= ld->ld_InnerBox.Height))
      {
	 /*
	  * We deactivate when the mouse has left us out of the hitbox.
	  */
	 return BUR_ABORT;
      };
   };
   return BUR_CONTINUE;
}
METHOD_END
///
/// BASE_DROPPED
/*
 * We have been dropped upon.
 */
//STATIC ASM ULONG ListClassDropped( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct bmDropped *bmd )
STATIC ASM REGFUNC3(ULONG, ListClassDropped,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct bmDropped *, bmd))
{
   LD       *ld = ( LD * )INST_DATA( cl, obj );
   struct MinList     buffer;
   LVE         *lve, *tmp, *pred = NULL;
   ULONG        spot = ld->ld_DropSpot, pos = 0;

   if(spot==~0)
      return(1);
   /*
    * Initialize buffer list.
    */
   NewList(( struct List * )&buffer );

   /*
    * Were busy buddy.
    */
   ld->ld_Flags |= LDF_LIST_BUSY;

   /*
    * Were we dropped in the list?
    */
   if ( spot ) {
      /*
       * Find the drop-position node.
       */
      if ( spot == 1 ) {
	 pos  = 0;
	 pred = ld->ld_Entries.lvl_First;
	 /*
	  * We do make this the predecessor
	  * if it's selected.
	  */
	 if ( pred->lve_Flags & LVEF_SELECTED )
	    pred = NULL;
      } else if ( spot >= ld->ld_Total ) {
	 pos  = ld->ld_Total - 1;
	 pred = ld->ld_Entries.lvl_Last;
      } else {
	 pred = FindNodeQuick( ld, spot - 1 );
	 if ( spot > ld->ld_LastNum ) pos = spot - 1;
	 else             pos = spot;
      }

      /*
       * Now we have to scan back from the drop
       * position to find the first not selected
       * entry node.
       */
      if ( pred ) {
	 while ( pred->lve_Flags & LVEF_SELECTED ) {
	    /*
	     * Get previous entry.
	     */
	    pred = pred->lve_Prev;

	    /*
	     * Is it the first one and still selected?
	     */
	    if ( pred == ld->ld_Entries.lvl_First && pred->lve_Flags & LVEF_SELECTED ) {
	       pred = NULL;
	       break;
	    }
	 }
      }
   }

   /*
    * Remove all selected entries and append them to the buffer.
    */
   lve = ld->ld_Entries.lvl_First;

   /*
    * Clear the last-active data.
    */
   ld->ld_LastActive = NULL;
   ld->ld_LastNum   = 0;

   while ( lve->lve_Next ) {
      /*
       * Is this a selected entry?
       */
      if ( lve->lve_Flags & LVEF_SELECTED ) {
	 /*
	  * Mark successor.
	  */
	 tmp = lve->lve_Next;

	 /*
	  * Remove from the list and
	  * append it to the buffer.
	  */
	 Remove(( struct Node * )lve );
	 AddTail(( struct List * )&buffer, ( struct Node * )lve );

	 /*
	  * Make the successor current.
	  */
	 lve = tmp;
      } else
	 /*
	  * Next please.
	  */
	 lve = lve->lve_Next;
   }

   /*
    * Move 'm back into their new position.
    */
   while ( lve = ( LVE * )RemHead(( struct List * )&buffer )) {
      if ( ! ld->ld_LastActive ) {
	 ld->ld_LastActive = lve;
	 ld->ld_LastNum   = pos;
      }
      if ( ! pred ) AddHead(( struct List * )&ld->ld_Entries, ( struct Node * )lve );
      else        Insert(( struct List * )&ld->ld_Entries, ( struct Node * )lve, ( struct Node * )pred );
      pred = lve;
   }

   /*
    * We are not busy anymore.
    */
   ld->ld_Flags   &= ~LDF_LIST_BUSY;
   ld->ld_DropSpot = ~0;

   return 1;
}
REGFUNC_END
///
/// BASE_GETOBJECT
/*
 * Create a rastport in which the selected entries(s) are rendered.
 */
METHOD(ListClassGetObject, struct bmGetDragObject *, bmgo)
{
   LD                *ld = INST_DATA(cl, obj);
   struct GadgetInfo *gi = bmgo->bmgo_GInfo;
   struct RastPort   *drag_rp, *rp = &gi->gi_Screen->RastPort;
   APTR               entry;
   struct IBox        box;
   ULONG              rc = 0, num, i = 0;
   int                depth = gi->gi_DrInfo->dri_Depth;
   struct BaseInfo   *bi;

   int lx = ld->ld_ListArea.Left;
   int ly = ld->ld_ListArea.Top;
   int lw = ld->ld_ListArea.Width;
   int lh = ld->ld_ListArea.Height;
   int eh = ld->ld_EntryHeight;
   int mx = gi->gi_Window->MouseX;
   int my = gi->gi_Window->MouseY;

   /*
    * Do we have any selected entries?
    */
   if (entry = (APTR)FirstSelected(obj))
   {
      /*
       * Count the number of selected entries.
       */
      for (num = 0; entry && (num <= 10); num++)
      {
	 entry = (APTR)NextSelected(obj, entry);
      };

      /*
       * Less than the maximum?
       */
      if (num <= 10)
      {
	 /*
	  * Allocate the rastport.
	  */
	 if (drag_rp = BGUI_CreateRPortBitMap(rp, lw, num * lh, depth))
	 {
#ifdef DEBUG_BGUI
	    if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_GadgetInfo, gi, BI_RastPort, drag_rp, TAG_DONE))
#else
	    if (bi = AllocBaseInfo(BI_GadgetInfo, gi, BI_RastPort, drag_rp, TAG_DONE))
#endif
	    {
	       BSetFont(bi, ld->ld_Font);
	       ColumnSeparators(ld, bi, 0, 0, num * ld->ld_EntryHeight);

	       /*
		* Render them...
		*/
	       entry = (APTR)FirstSelected(obj);
	       do
	       {
		  RenderEntry(obj, ld, bi, ld->ld_ScanNode, REL_ZERO | i++);
	       }  while (entry = (APTR)NextSelected(obj, entry));
	       setIt:

	       /*
		* Setup the rastport so we can
		* deallocate it later.
		*/
	       ld->ld_DragRP = drag_rp;

	       /*
		* Setup bounds.
		*/
	       bmgo->bmgo_Bounds->Left   = lx;
	       bmgo->bmgo_Bounds->Top    = my - ((num * eh) >> 1);
	       bmgo->bmgo_Bounds->Width  = lw;
	       bmgo->bmgo_Bounds->Height = num * eh;

	       /*
		* Return a pointer to the bitmap.
		*/
	       rc = (ULONG)drag_rp->BitMap;

	       if(bi) FreeBaseInfo(bi);
	    };
	 }
      }
      else
      {
	 /*
	  * More than 10 entries is a special case.
	  */
	 if (drag_rp = BGUI_CreateRPortBitMap(rp, lw, 3 * eh, depth))
	 {
#ifdef DEBUG_BGUI
	    if (bi = AllocBaseInfoDebug(__FILE__,__LINE__,BI_GadgetInfo, gi, BI_RastPort, drag_rp, TAG_DONE))
#else
	    if (bi = AllocBaseInfo(BI_GadgetInfo, gi, BI_RastPort, drag_rp, TAG_DONE))
#endif
	    {
	       BSetFont(bi, ld->ld_Font);
	       ColumnSeparators(ld, bi, 0, 0, 3 * ld->ld_EntryHeight);

	       /*
		* Render the first entry...
		*/
	       FirstSelected(obj);
	       RenderEntry(obj, ld, bi, ld->ld_ScanNode, REL_ZERO | 0);

	       /*
		* Setup rendering bounds.
		*/
	       box.Left    = 0;
	       box.Top     = eh;
	       box.Width   = lw;
	       box.Height  = eh;

	       /*
		* Jam special text...
		*/
	       RenderText(bi, "\33d3\33D5--->", &box);

	       /*
		* Render the last entry...
		*/
	       LastSelected(obj);
	       RenderEntry(obj, ld, bi, ld->ld_ScanNode, REL_ZERO | 2);

	       FreeBaseInfo(bi); bi=NULL;
	    };

	    num = 3;
	    goto setIt;
	 }
      }
   }
   return rc;
}
METHOD_END
///
/// BASE_FREEOBJECT
/*
 * Free the dragged object.
 */
//STATIC ASM ULONG ListClassFreeObject( REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) struct bmFreeDragObject *bmfo )
STATIC ASM REGFUNC3(ULONG, ListClassFreeObject,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct bmFreeDragObject *, bmfo))
{
   LD       *ld = INST_DATA( cl, obj );

   /*
    * Simply deallocate the bitmap and rastport.
    */
   BGUI_FreeRPortBitMap( ld->ld_DragRP );
   ld->ld_DragRP = NULL;

   return 1;
}
REGFUNC_END
///

/// Class initialization.
/*
 * Function table.
 */
STATIC DPFUNC ClassFunc[] = {
   BASE_RENDER,            (FUNCPTR)ListClassRender,
   BASE_LAYOUT,            (FUNCPTR)ListClassLayout,
   BASE_DIMENSIONS,        (FUNCPTR)ListClassDimensions,

   OM_NEW,                 (FUNCPTR)ListClassNew,
   OM_SET,                 (FUNCPTR)ListClassSetUpdate,
   OM_UPDATE,              (FUNCPTR)ListClassSetUpdate,
   OM_GET,                 (FUNCPTR)ListClassGet,
   OM_DISPOSE,             (FUNCPTR)ListClassDispose,
   GM_HITTEST,             (FUNCPTR)ListClassHitTest,
   GM_GOACTIVE,            (FUNCPTR)ListClassGoActive,
   GM_HANDLEINPUT,         (FUNCPTR)ListClassHandleInput,
   GM_GOINACTIVE,          (FUNCPTR)ListClassGoInActive,
   WM_KEYACTIVE,           (FUNCPTR)ListClassKeyActive,
   LVM_ADDENTRIES,         (FUNCPTR)ListClassAddEntries,
   LVM_INSERTENTRIES,      (FUNCPTR)ListClassInsertEntries,
   LVM_ADDSINGLE,          (FUNCPTR)ListClassAddSingle,
   LVM_INSERTSINGLE,       (FUNCPTR)ListClassInsertSingle,
   LVM_CLEAR,              (FUNCPTR)ListClassClear,
   LVM_FIRSTENTRY,         (FUNCPTR)ListClassGetEntry,
   LVM_LASTENTRY,          (FUNCPTR)ListClassGetEntry,
   LVM_NEXTENTRY,          (FUNCPTR)ListClassGetEntry,
   LVM_PREVENTRY,          (FUNCPTR)ListClassGetEntry,
   LVM_REMENTRY,           (FUNCPTR)ListClassRemEntry,
   LVM_REFRESH,            (FUNCPTR)ListClassRefresh,
   LVM_REDRAW,             (FUNCPTR)ListClassRedraw,
   LVM_REDRAWSINGLE,       (FUNCPTR)ListClassRedrawSingle,
   LVM_SORT,               (FUNCPTR)ListClassSort,
   LVM_LOCKLIST,           (FUNCPTR)ListClassLock,
   LVM_UNLOCKLIST,         (FUNCPTR)ListClassLock,
   LVM_REMSELECTED,        (FUNCPTR)ListClassRemSelected,
   LVM_MOVE,               (FUNCPTR)ListClassMove,
   LVM_REPLACE,            (FUNCPTR)ListClassReplace,
   LVM_SETCOLUMNATTRS,     (FUNCPTR)ListClassSetColumnAttrs,
   LVM_GETCOLUMNATTRS,     (FUNCPTR)ListClassGetColumnAttrs,
   BASE_DRAGQUERY,         (FUNCPTR)ListClassDragQuery,
   BASE_DRAGACTIVE,        (FUNCPTR)ListClassDragActive,
   BASE_DRAGINACTIVE,      (FUNCPTR)ListClassDragInactive,
   BASE_DRAGUPDATE,        (FUNCPTR)ListClassDragUpdate,
   BASE_DROPPED,           (FUNCPTR)ListClassDropped,
   BASE_GETDRAGOBJECT,     (FUNCPTR)ListClassGetObject,
   BASE_FREEDRAGOBJECT,    (FUNCPTR)ListClassFreeObject,
   DF_END,                 NULL
};

/*
 * Simple class initialization.
 */
makeproto Class *InitListClass(void)
{
   return BGUI_MakeClass(CLASS_SuperClassBGUI, BGUI_BASE_GADGET,
			 CLASS_ObjectSize,     sizeof(LD),
			 CLASS_DFTable,        ClassFunc,
			 TAG_DONE);
}
///

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
 * Revision 42.7  2003/01/18 19:10:21  chodorowski
 * Instead of using the _AROS or __AROS preprocessor symbols, use __AROS__.
 *
 * Revision 42.6  2000/08/09 10:17:25  chodorowski
 * #include <bgui/bgui_compilerspecific.h> for the REGFUNC and REGPARAM
 * macros. Some of these files didn't need them at all...
 *
 * Revision 42.5  2000/08/08 20:23:21  stegerg
 * added "#define LOCAL static"
 *
 * Revision 42.4  2000/08/08 14:02:08  chodorowski
 * Removed all REGFUNC, REGPARAM and REG macros. Now includes
 * contrib/bgui/compilerspecific.h where they are defined.
 *
 * Revision 42.3  2000/08/07 21:49:51  stegerg
 * fixed/activated REGPARAM/REGFUNC macros.
 *
 * Revision 42.2  2000/07/09 03:05:10  bergers
 * Makes the gadgets compilable.
 *
 * Revision 42.1  2000/05/15 19:28:19  stegerg
 * REG() macro replacementes
 *
 * Revision 42.0  2000/05/09 22:22:29  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:36:17  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:01:11  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.1  1999/02/21 04:08:36  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
******************  TREEVIEWCLASS PRIVATE DEFINITIONS  ******************
************************************************************************/

/************************************************************************
******************************  INCLUDES  *******************************
************************************************************************/

#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <graphics/gfxmacros.h>
#include <libraries/bgui.h>
#include <libraries/bgui_macros.h>

#include <clib/alib_protos.h>
#include <clib/macros.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/bgui.h>

#include <stdlib.h>
#include <string.h>

#include "TreeViewClass.h"

#define LOCAL static

/************************************************************************
*************************  GLOBAL REFERENCES  ***************************
************************************************************************/

/*
 * Client code must open these libraries:
 */

extern struct IntuitionBase     *IntuitionBase;         /* V37+ */
extern struct GfxBase   *GfxBase;                               /* V37+ */
#ifdef __AROS__
extern struct UtilityBase       *UtilityBase;                   /* V37+ */
#else
extern struct Library   *UtilityBase;                   /* V37+ */
#endif
extern struct Library   *BGUIBase;                              /* V40+ */

/*
 * Only referenced while debugging:
 */

#ifdef __AROS__
#else
extern void __stdargs KPrintF(char *fmt,...);
#endif

/************************************************************************
*************************  GLOBAL DEFINITIONS  **************************
************************************************************************/

/* Members of the treeview are held in this derivation of an Exec
 * node. The node's ln_Type is used as a flags field, the flag bits
 * are defined below. The ln_Pri is unused.
 */

struct TreeNode {
        struct Node             tn_Node;                /* next/prev/flags/pri/name */
        struct TreeNode *tn_Parent;             /* ptr to the parent treenode */
        struct MinList  tn_Children;    /* list of child treenodes */
        ULONG                   tn_NumChildren; /* count of child treenodes */
};

#define tn_Flags                tn_Node.ln_Type  /* redefinition of Node member */
#define tn_Entry                tn_Node.ln_Name  /* abbreviation for entry */

#define TNF_EXPANDED    (1<<0)          /* entry's children are expanded */
#define TNF_SELECTED    (1<<1)          /* entry is currently selected */

/*
 * Object instance data.
 */

typedef struct {
                struct TreeNode tv_RootNode;            /* dummy root level treenode */

                Class                   *tv_TreeViewClass;      /* back-reference to class */
                Object                  *tv_TreeView;           /* back-reference to object */

                APTR                    tv_MemPool;                     /* private memory pool */

                struct Hook             *tv_ResourceHook;       /* user's add/delete hook */
                struct Hook             *tv_DisplayHook;        /* user's custom rendering hook */
                struct Hook             *tv_CompareHook;        /* user's custom sorting hook */
                struct Hook             *tv_ExpandHook;         /* user's expand/contract hook */

                Object                  *tv_Listview;           /* embedded listview object */
                struct Hook             *tv_LVRsrcHook;         /* internal LV resource hook */
                struct Hook             *tv_LVDispHook;         /* internal LV display hook */
                struct Hook             *tv_LVCompHook;         /* internal LV compare hook */
                struct Hook             *tv_LVNotifyHook;       /* internal LV notification hook */

                Object                  *tv_ExpandedImage;       /* image for expanded nodes */
                Object                  *tv_ContractedImage; /* image for contracted nodes */
                BOOL                    tv_DefExpImage;         /* used built-in expanded image */
                BOOL                    tv_DefConImage;         /* used built-in contracted image */

                ULONG                   tv_CopyEntries;         /* copy (string) entries */
                ULONG                   tv_Indentation;         /* pixel indent of children */
                ULONG                   tv_LineStyle;           /* lines connecting entries */
                ULONG                   tv_LeftAlignImage;      /* align images with LHS */
                ULONG                   tv_NoLeafImage;         /* no image for leaf nodes */

                ULONG                   tv_NumEntries;          /* total number of entries */

                ULONG                   tv_LockCount;           /* lock count for node list */

                BOOL                    tv_GoingActive;         /* during GM_GOACTIVE? */
                WORD                    tv_LastClickX;          /* mouse x of last LMB click */
                WORD                    tv_LastClickY;          /* mouse y of last LMB click */
                struct TreeNode *tv_LastClicked;        /* last entry clicked on */
                struct TreeNode *tv_ImageClicked;       /* entry whose image was clicked */
                ULONG                   tv_LastClickTime[2]; /* timestamp of last click */
} TVData;

/*
 * Convenient type-casts
 */

typedef struct Node             *NODEPTR;
typedef struct TreeNode *TNPTR;
typedef struct List             *LISTPTR;
typedef struct Gadget   *GADPTR;
typedef struct Image    *IMGPTR;
typedef struct Hook             *HOOKPTR;

/*
 * Macros for walking trees, inspecting treenodes, etc
 */

#define RootList(tv)            ((LISTPTR) &(tv)->tv_RootNode.tn_Children)

#define ParentOf(child)         (((TNPTR) (child))->tn_Parent)
#define ChildListOf(child)      ((LISTPTR) &((TNPTR) (child))->tn_Children)

#define FirstChildIn(list)      ((TNPTR) TV_GetHead((LISTPTR) (list)))
#define FirstChildOf(child)     ((TNPTR) TV_GetHead(ChildListOf(child)))

#define LastChildIn(list)       ((TNPTR) TV_GetTail((LISTPTR) (list)))
#define LastChildOf(child)      ((TNPTR) TV_GetTail(ChildListOf(child)))

#define FirstSiblingOf(child) (FirstChildOf(ParentOf(child)))
#define LastSiblingOf(child)  (LastChildOf(ParentOf(child)))

#define NextSiblingOf(child)  ((TNPTR) TV_GetSucc((NODEPTR) (child)))
#define PrevSiblingOf(child)  ((TNPTR) TV_GetPred((NODEPTR) (child)))

#define IsExpanded(child)       (((TNPTR) (child))->tn_Flags & TNF_EXPANDED)
#define IsSelected(child)       (((TNPTR) (child))->tn_Flags & TNF_SELECTED)
#define HasChildren(child)      (((TNPTR) (child))->tn_NumChildren)

/*
 * Anchor used by TV_MatchNextEntry() for matching Treeview entries
 * starting from a tvEntry-type method.
 */

struct tvAnchor {
        TNPTR                           tva_Ref;                /* reference entry */
        TNPTR                           tva_Last;               /* last match */
        ULONG                           tva_Category;   /* TVWC bits of tve_Which */
        ULONG                           tva_Style;              /* TVWS bits of tve_Which */
        ULONG                           tva_Flags;              /* same as tve_Flags */
        ULONG                           tva_RefDepth;   /* depth of reference entry */
        BOOL                            tva_RefRoot;    /* reference entry is root */
        BOOL                            tva_Multiple;   /* potentially multiple matches */
        BOOL                            tva_Visible;    /* match visible entries only */
        BOOL                            tva_Selected;   /* match selected entries only */
};

/*
 * Useful abbreviations
 */

#define MEMF_STD                (MEMF_ANY|MEMF_CLEAR)

#ifndef max
#define max(a,b)                ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)                ((a) <= (b) ? (a) : (b))
#endif

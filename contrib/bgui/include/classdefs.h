#ifndef CLASSLIB_H
#define CLASSLIB_H
/*
 * @(#) $Header$
 *
 * $VER: libraries/classdefs.h 41.10 (25.2.98)
 * bgui.library classes definitions.
 *
 * (C) Copyright 1998 Manuel Lemos.
 * (C) Copyright 1996-1997 Ian J. Einman.
 * (C) Copyright 1993-1996 Jaba Development.
 * (C) Copyright 1993-1996 Jan van den Baard.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 1.1  1998/02/25 17:16:15  mlemos
 * Ian sources
 *
 *
 */

/*
 * Include a lot of system stuff.
 */
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>
#include <intuition/cghooks.h>
#include <intuition/icclass.h>
#include <dos/rdargs.h>
#include <dos/dostags.h>
#include <graphics/gfxmacros.h>
#include <graphics/displayinfo.h>
#include <graphics/rpattr.h>
#include <graphics/gels.h>
#include <libraries/gadtools.h>
#include <libraries/amigaguide.h>
#include <libraries/commodities.h>
#include <libraries/locale.h>
#include <libraries/iffparse.h>
#include <devices/timer.h>
#include <workbench/workbench.h>
#include <gadgets/colorwheel.h>
#include <gadgets/gradientslider.h>
#include <utility/pack.h>

#include <clib/macros.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/asl.h>
#include <proto/layers.h>
#include <proto/gadtools.h>
#include <proto/amigaguide.h>
#include <proto/commodities.h>
#include <proto/locale.h>
#include <proto/keymap.h>
#include <proto/dos.h>
#include <proto/wb.h>
#include <proto/datatypes.h>
#include <proto/iffparse.h>

#define abs x_abs
#define min x_min
#define max x_max

#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>

#undef abs
#undef min
#undef max

#include <bgui/bgui.h>
#include <bgui/bgui_macros.h>

#include "include/bgui_prefs.h"

/*
 * Locale muck.
 */
#define CATCOMP_NUMBERS
#include "bgui_locale.h"

#ifndef CATCOMP_ARRAY
struct CatCompArrayType
{
    LONG   cca_ID;
    STRPTR cca_Str;
};
#endif

#define LOCSTR(n)    CatCompArray[n].cca_Str

/*
 * Yeck...
 */
#ifdef DEBUG
extern __stdargs VOID kprintf( UBYTE *, ... );
#define debug kprintf
#else
#define debug(x)
#endif

/*
 * Misc macros.
 */
#define GADGET(g)    ((struct Gadget *)g)
#define IMAGE(i)     ((struct Image *)i)
#define IBOX(b)      ((struct IBox *)b)
#define GADGETBOX(g) IBOX(&(GADGET(g)->LeftEdge))
#define IMAGEBOX(i)  IBOX(&(IMAGE(i)->LeftEdge))
#define ALLOCPUB(s)  AllocVec(s, MEMF_PUBLIC | MEMF_CLEAR)
#define PENS(d)      ((d) ? d->dri_Pens : DefDriPens)
#define FRM_TAG(v)   (v >= FRM_TAGSTART && v <= FRM_TAGDONE)
#define LAB_TAG(v)   (v >= LAB_TAGSTART && v <= LAB_TAGDONE)
#define VIT_TAG(v)   (v >= VIT_TAGSTART && v <= VIT_TAGDONE)
#define LGO_TAG(v)   (v >= LGO_TAGSTART && v <= LGO_TAGDONE)

#define test(exp) ((exp) ? 1 : 0)
#define STORE *store = (ULONG)

/*
 * Task information tracking.
 */
typedef struct WinInfo {
   struct WinInfo *wi_Next;
   struct WinInfo *wi_Prev;
   ULONG           wi_WinID;
   struct IBox     wi_Bounds;
}  WI;

typedef struct WinList {
   WI *wl_First;
   WI *wl_EndMark;
   WI *wl_Last;
}  WL;

typedef struct PrefInfo {
   struct PrefInfo   *pi_Next;
   struct PrefInfo   *pi_Prev;
   ULONG              pi_PrefID;
   struct TagItem    *pi_Defaults;
}  PI;

typedef struct PrefList {
   PI *pl_First;
   PI *pl_EndMark;
   PI *pl_Last;
}  PRL;

typedef struct ReportID {
   struct ReportID   *rid_Next;
   struct ReportID   *rid_Prev;
   ULONG              rid_ID;
   struct Window     *rid_Window;
}  RID;

typedef struct {
   RID             *ril_First;
   RID             *ril_EndMark;
   RID             *ril_Last;
}  RIL;

typedef struct TaskMember {
   struct TaskMember *tm_Next;
   struct TaskMember *tm_Prev;
   struct Task       *tm_TaskAddr;
   ULONG              tm_Counter;
   WL                 tm_Windows;
   PRL                tm_Prefs;
   RIL                tm_RID;
}  TM;

typedef struct WindowNode {
   struct WindowNode *wn_Next;
   struct WindowNode *wn_Prev;
   Object            *wn_WindowObject;
   struct Window     *wn_Window;
}  WNODE;

typedef struct {
   WNODE *wnl_First;
   WNODE *wnl_EndMark;
   WNODE *wnl_Last;
}  WLIST;

typedef struct {
   TM       *tl_First;
   TM       *tl_EndMark;
   TM       *tl_Last;
   WLIST     tl_WindowList;
}  TL;

/*
 * Not defined in any of the system
 * headers. A pointer to this information
 * is passed as the message to the hook.
 */
typedef struct {
   struct Layer     *bf_Layer;     /* Layer to fill.               */
   struct Rectangle  bf_Rect;      /* Rect to fill.                */
   LONG              bf_X, bf_Y;   /* Offsets for fancy backfills. */
}  BFINFO;

#define METHOD(f,m) STATIC ASM ULONG f(REG(a0) Class *cl, REG(a2) Object *obj, REG(a1) m)

#define BGUI_PMB  0x000FF000
#define BGUI_PB   0x800FF000
/*
 * Very private class attributes/methods.
 */
#define DGM_DoMethod          (BGUI_PB+1)       /* DGMClass.     */
#define DGM_Object            (BGUI_PB+2)       /* DGMClass.     */
#define DGM_Result            (BGUI_PB+3)       /* DGMClass.     */
#define DGM_IntuiMsg          (BGUI_PB+4)       /* DGMClass.     */

#define BT_IsActive           (BGUI_PB+101)     /* BaseClass.     */
#define BT_WindowObj          (BGUI_PB+102)     /* BaseClass.     */
#define BT_TextFont           (BGUI_PB+103)     /* BaseClass.     */
#define BT_ParentWindow       (BGUI_PB+104)     /* BaseClass.     */
#define BT_ReportID           (BGUI_PB+105)     /* BaseClass.     */
#define BT_ParentView         (BGUI_PB+106)     /* BaseClass.     */
#define BT_ParentGroup        (BGUI_PB+107)     /* BaseClass.     */
#define BT_GroupMember        (BGUI_PB+109)     /* BaseClass.     */
#define BT_MustSpecifyFrame   (BGUI_PB+108)     /* BaseClass.     */

#define GROUP_IsMaster        (BGUI_PB+201)     /* GroupClass.    */
#define GROUP_TopBorder       (BGUI_PB+203)     /* GroupClass.    */

#define LGO_Object            (BGUI_PB+251)     /* GroupMember.   */
#define LGO_Group             (BGUI_PB+252)     /* GroupMember.   */
#define LGO_SpaceObject       (BGUI_PB+253)     /* GroupMember.   */

#define IMAGE_ErasePen        (BGUI_PB+301)     /* ImageClasses.  */
#define IMAGE_InBorder        (BGUI_PB+302)     /* ImageClasses.  */
#define IMAGE_TextFont        (BGUI_PB+303)     /* ImageClasses.  */

#define FRM_GetOffsets        (BGUI_PB+401)     /* FrameClass.    */
#define FRM_FillOuter         (BGUI_PB+402)     /* FrameClass.    */

#define LAB_KeyChar           (BGUI_PB+502)     /* LabelClass.    */

#define WINDOW_GadgetOK       (BGUI_PB+601)     /* WindowClass.   */
#define WINDOW_BufferRP       (BGUI_PB+602)     /* WindowClass.   */

#define VIEW_ObjectBuffer     (BGUI_PB+701)     /* ViewClass.     */

#define BUTTON_ScaleMinWidth  (BGUI_PB+1001)    /* ButtonClass.   */
#define BUTTON_ScaleMinHeight (BGUI_PB+1002)    /* ButtonClass.   */

#define PGA_DontTarget        (BGUI_PB+1101)    /* PropClass.     */
#define PGA_Slider            (BGUI_PB+1102)    /* PropClass.     */

#define STRINGA_Tabbed        (BGUI_PB+1201)    /* StringClass.   */
#define STRINGA_ShiftTabbed   (BGUI_PB+1202)    /* StringClass.   */

#define ASLPOP_Class          (BGUI_PB+1301)    /* AslPopClass.   */

#define RM_SETLOOP            (BGUI_PMB+1)      /* RootClass.     */
#define RM_CLEARLOOP          (BGUI_PMB+2)      /* RootClass.     */

#define BASE_RENDER_LABEL     (BGUI_PMB+101)    /* BaseClass.     */
#define BASE_LEFTEXT          (BGUI_PMB+105)    /* BaseClass.     */
#define BASE_IS_MULTI         (BGUI_PMB+106)    /* BaseClass.     */
#define BASE_RENDERBUFFER     (BGUI_PMB+107)    /* BaseClass.     */
#define BASE_MOVEBOUNDS       (BGUI_PMB+108)    /* BaseClass.     */

#define GRM_RELAYOUT          (BGUI_PMB+201)    /* GroupClass.    */

#define FRAMEM_SETUPBOUNDS    (BGUI_PMB+401)    /* FrameClass.    */

#define WM_SECURE             (BGUI_PMB+601)    /* WindowClass.   */
#define WM_RELEASE            (BGUI_PMB+608)    /* WindowClass.   */
#define WM_RELAYOUT           (BGUI_PMB+602)    /* WindowClass.   */
#define WM_LOCK               (BGUI_PMB+603)    /* WindowClass.   */
#define WM_UNLOCK             (BGUI_PMB+604)    /* WindowClass.   */
#define WM_CLOSETOOLTIP       (BGUI_PMB+605)    /* WindowClass.   */
#define WM_CLIP               (BGUI_PMB+606)    /* WindowClass.   */

#define VIEW_CLIP             (BGUI_PMB+701)    /* ViewClass.     */

#define BGUI_SPACING_OBJECT   0xF001
#define BGUI_DGM_OBJECT       0xF002

struct bmLeftExt {
   ULONG              MethodID;
   struct RastPort   *bmle_RPort;
   UWORD             *bmle_Extention;
};

struct bmRenderBuffer {
   ULONG              MethodID;
   struct Screen     *bmrb_Screen;
   ULONG              bmrb_Width;
   ULONG              bmrb_Height;
};

struct bmMoveBounds {
   ULONG              MethodID;
   LONG               bmmb_ChangeX;
   LONG               bmmb_ChangeY;
   LONG               bmmb_ChangeW;
   LONG               bmmb_ChangeH;
};

struct fmSetupBounds {
   ULONG              MethodID;
   struct IBox       *fmsb_HitBox;
   struct IBox       *fmsb_InnerBox;
};

/* Get the window rendering buffer. */
struct wmClip {
   ULONG              MethodID;      /* WM_CLIP                  */
   struct Rectangle  *wmc_Rectangle; /* Rectangle for operation. */
   ULONG              wmc_Action;    /* See below.               */
};

#define CLIPACTION_NONE  0
#define CLIPACTION_CLEAR 1
#define CLIPACTION_OR    2
#define CLIPACTION_AND   3

/* Compute the cliprect for a view object. */
struct vmClip {
   ULONG              MethodID;      /* VIEW_CLIP               */
   struct Rectangle  *vmc_Rectangle;
};

struct grmRelayout {
   ULONG              MethodID;      /* GRM_RELAYOUT */
   struct GadgetInfo *grmr_GInfo;
   struct RastPort   *grmr_RPort;
};

#define GDIMF_DIMENSIONS        (1<<31)  /* No relativity.       */

/*
 * This is a buffer rastport (RP_User).
 */
#define ID_BFRP   (APTR)MAKE_ID('B','F','R','P')

/*
 * BitMaps to drag around.
 */
typedef struct {
   struct SignalSemaphore bmo_Lock;

   struct Process    *bmo_Process;
   struct Screen     *bmo_Screen;
   struct Window     *bmo_Window;
   struct BitMap     *bmo_ObjectBuffer;
   struct BitMap     *bmo_ScreenBuffer;

   Object      *bmo_Object;

   UWORD        bmo_CW, bmo_CH;
   BOOL         bmo_Locked;
   
   WORD         bmo_LX, bmo_LY;           /* Last mouse position.    */
   WORD         bmo_CX, bmo_CY;           /* Current mouse position. */
   WORD         bmo_IX, bmo_IY;           /* Internal offsets.       */
   WORD         bmo_MaxX, bmo_MaxY;       /* Maximum drag position.  */

   struct Window *bmo_BMWindow;
   UWORD          bmo_BMDelay;

   Object        *bmo_ActWin;             /* Current drop window object.  */
   Object        *bmo_ActRec;             /* Current drop object.         */
   struct Window *bmo_ActPtr;             /* Current drop window pointer. */

}  BMO;

struct OBox
{
   WORD Left;
   WORD Right;
   WORD Top;
   WORD Bottom;
};

typedef struct BaseClassData
{
   ULONG            bc_Flags;
   struct IBox      bc_OuterBox;
   struct IBox      bc_InnerBox;
   struct IBox      bc_HitBox;
   struct IBox      bc_Extensions;
   Object          *bc_Frame;
   Object          *bc_Label;
   Object          *bc_Window;
   Object          *bc_Group;
   Object          *bc_View;
   struct RastPort *bc_RPort;
   struct TextAttr *bc_TextAttr;
   struct TextFont *bc_TextFont;
   Object          *bc_GroupNode;
}  BC;

#define BASE_DATA(o) ((BC *)INST_DATA(BaseClass, o))

#define MAX_DRAWER   512
#define MAX_FILE     108
#define MAX_PATH     (MAX_DRAWER + MAX_FILE)
#define MAX_PATTERN  256

/*
 * Task-member control results.
 */
#define TASK_FAILED     0
#define TASK_ADDED      2
#define TASK_REOPENED   3

/*
 * Border gadget test.
 */
#define BORDERMASK      (GACT_LEFTBORDER|GACT_TOPBORDER|GACT_RIGHTBORDER|GACT_BOTTOMBORDER)

/*
 * Call the IM_EXTENT method.
 */
#define DoExtentMethod(obj, rp, ibox, lw, lh, flags) \
AsmDoMethod(obj, IM_EXTENT, rp, ibox, lw, lh, flags << 16)

#define BGUI_DoGadgetMethod myDoGadgetMethod


#define BI_BaseInfo    (TAG_USER + 1)
#define BI_GadgetInfo  (TAG_USER + 2)
#define BI_RastPort    (TAG_USER + 3)
#define BI_DrawInfo    (TAG_USER + 4)
#define BI_Pens        (TAG_USER + 5)
#define BI_Screen      (TAG_USER + 6)


/*
 * Library constants.
 */
#include "o/bgui.library_rev.h"
extern const UBYTE LibName[];
extern const UBYTE LibID[];

#define LIBVER       VERSION
#define LIBREV       REVISION
#define LIBREVSTR    VERS " (" DATE ")"

#define IsMulti(o) AsmDoMethod(o, BASE_IS_MULTI)

/*
 * Generated by MakeProto.
 */
#include "o/class-protos.h"

#endif

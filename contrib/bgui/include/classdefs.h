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
 * Revision 41.11  2000/05/09 20:02:04  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 41.10.2.15  1999/08/30 05:00:16  mlemos
 * Added definitions for the WM_SETUPGADGET method.
 *
 * Revision 41.10.2.14  1999/08/29 20:23:50  mlemos
 * Moved the GRM_RELAYOUT method defintions to bgui.h and renamed it to
 * BASE_RELAYOUT.
 *
 * Revision 41.10.2.13  1999/08/01 04:17:40  mlemos
 * Added a field to the VIEW_CLIP method message to determine if it is
 * requested to clip against a parent view.
 *
 * Revision 41.10.2.12  1999/07/18 03:00:25  mlemos
 * Added missing semi-colon in BRectFill declaration for when DEBUG_BGUI is
 * not set.
 *
 * Revision 41.10.2.11  1999/07/04 05:15:36  mlemos
 * Added debuging alias for BRectFill function.
 *
 * Revision 41.10.2.10  1998/12/07 03:08:40  mlemos
 * Added the definitions of BGUI open and close font macros.
 *
 * Revision 41.10.2.9  1998/11/29 23:00:22  mlemos
 * Fixed missing ; in the prototypes of SRectFill() and RenderBackFillRaster()
 * functions.
 *
 * Revision 41.10.2.8  1998/10/12 01:43:54  mlemos
 * Added the inclusion of the rexxsyslib.library prototypes include.
 *
 * Revision 41.10.2.7  1998/09/20 22:44:52  mlemos
 * Change the debugging from kprintf to tprintf.
 *
 * Revision 41.10.2.6  1998/07/05 19:38:37  mlemos
 * Added definitions for debug function alias to trap invalid RectFill calls.
 *
 * Revision 41.10.2.5  1998/03/01 15:40:52  mlemos
 * Added support to track BaseInfo memory leaks.
 *
 * Revision 41.10.2.4  1998/03/01 02:26:24  mlemos
 * Added memory allocation function aliases for debugging
 *
 * Revision 41.10.2.3  1998/02/28 02:45:54  mlemos
 * Added definitions for assert kprintf macros
 *
 * Revision 41.10.2.2  1998/02/26 19:00:30  mlemos
 * Corrected include paths
 *
 * Revision 41.10.2.1  1998/02/26 18:03:47  mlemos
 * Moved LGO_Object tag definition to bgui.h
 *
 * Revision 41.10  1998/02/25 21:14:06  mlemos
 * Bumping to 41.10
 *
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
#include <proto/rexxsyslib.h>

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

#define bug tprintf

#ifdef DEBUG_BGUI
#define D(x) (x)
#else
#define D(x) ;
#endif

#define DB(x) (x)

#include <libraries/bgui.h>
#include <libraries/bgui_macros.h>

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
#define debug tprintf
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

#define FRAMEM_SETUPBOUNDS    (BGUI_PMB+401)    /* FrameClass.    */

#define WM_SECURE             (BGUI_PMB+601)    /* WindowClass.   */
#define WM_RELEASE            (BGUI_PMB+608)    /* WindowClass.   */
#define WM_RELAYOUT           (BGUI_PMB+602)    /* WindowClass.   */
#define WM_LOCK               (BGUI_PMB+603)    /* WindowClass.   */
#define WM_UNLOCK             (BGUI_PMB+604)    /* WindowClass.   */
#define WM_CLOSETOOLTIP       (BGUI_PMB+605)    /* WindowClass.   */
#define WM_CLIP               (BGUI_PMB+606)    /* WindowClass.   */
#define WM_SETUPGADGET        (BGUI_PMB+607)    /* WindowClass.   */

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
   ULONG              vmc_WindowClip;
};

/* Setup a gadget with the window default values. */
struct wmSetupGadget {
   ULONG              MethodID;      /* WM_SETUPGADGET           */
   Object            *wmsg_Object;   /* Gadget object.           */
   struct TagItem    *wmsg_Tags;     /* Tag options.             */
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

#ifndef NO_MEMORY_ALLOCATION_DEBUG_ALIASING
#define BGUI_AllocPoolMem(size) BGUI_AllocPoolMemDebug(size,__FILE__,__LINE__)
#define BGUI_FreePoolMem(memPtr) BGUI_FreePoolMemDebug(memPtr,__FILE__,__LINE__)
#ifdef DEBUG_BGUI
#define BGUI_AllocBaseInfoA(tags) BGUI_AllocBaseInfoDebugA(tags,__FILE__,__LINE__)
#define FreeBaseInfo(bi) FreeBaseInfoDebug(bi,__FILE__,__LINE__)
#endif
#endif

#ifdef DEBUG_BGUI
#define BGUI_OpenFont(textAttr) BGUI_OpenFontDebug(textAttr,__FILE__,__LINE__)
#define BGUI_CloseFont(font) BGUI_CloseFontDebug(font,__FILE__,__LINE__)
#else
#define BGUI_OpenFont(textAttr) OpenFont(textAttr)
#define BGUI_CloseFont(font) CloseFont(font)
#endif

#ifdef DEBUG_BGUI
#define SRectFill(rp,l,t,r,b) SRectFillDebug(rp,l,t,r,b,__FILE__,__LINE__)
#define BRectFill(bi,l,t,r,b) BRectFillDebug(bi,l,t,r,b,__FILE__,__LINE__)
#define RenderBackFillRaster(rp,ib,apen,bpen) RenderBackFillRasterDebug(rp,ib,apen,bpen,__FILE__,__LINE__)
#else
ASM VOID SRectFill(REG(a0) struct RastPort *rp, REG(d0) LONG l, REG(d1) LONG t, REG(d2) LONG r, REG(d3) LONG b);
ASM VOID BRectFill(REG(a0) struct BaseInfo *bi, REG(d0) LONG l, REG(d1) LONG t, REG(d2) LONG r, REG(d3) LONG b);
ASM VOID RenderBackFillRaster(REG(a0) struct RastPort *rp, REG(a1) struct IBox *ib, REG(d0) UWORD apen, REG(d1) UWORD bpen);
#endif

#endif

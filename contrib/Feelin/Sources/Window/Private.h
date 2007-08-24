#include <string.h>

#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/wb.h>
#include <proto/feelin.h>

#include <exec/memory.h>
#include <intuition/intuition.h>
#include <workbench/workbench.h>
#include <libraries/feelin.h>
#include <feelin/preference.h>

#include "_locale/enums.h"

#define GfxBase                     FeelinBase -> Graphics
#define LayersBase                  FeelinBase -> Layers
#define UtilityBase                 FeelinBase -> Utility

#ifndef __MORPHOS__
#define IntuitionBase               FeelinBase -> Intuition
#endif

/***************************************************************************/
/*** Structures ************************************************************/
/***************************************************************************/

typedef struct FeelinCycleNode
{
   struct FeelinCycleNode          *Next;
   struct FeelinCycleNode          *Prev;
   FObject                          Object;
   FAreaPublic                     *AreaPublic;
}
FCycleNode;

typedef struct FeelinRethinkNode
{
    struct FeelinRethinkNode       *Next;
    struct FeelinRethinkNode       *Prev;
    FObject                         Object;
}
FRethinkNode;

struct LocalObjectData
{
    FBox                            Box;
    FBox                            UserBox;
    FBox                            FrameBox;
    FBox                            Zoom;
    FInner                          Borders;
    FMinMax                         MinMax;
    FList                           EventHandlers;
    FList                           CycleChain;

    bits8                           SYSFlags;
    bits8                           GADFlags;
    bits8                           WINFlags;
    bits8                           BOXFlags;
    
    STRPTR                          WinTitle;
    STRPTR                          ScrTitle;

    FFramePublic                   *FramePublic;
    FObject                         Root;
    FAreaPublic                    *root_area_public;
    FObject                         Decorator;
    FAreaPublic                    *decorator_area_public;

    FObject                         ActiveObj;
    FObject                         PopHelp;
    STRPTR                          ContextHelp;
    FObject                         ContextMenu;
    
    uint16                          RethinkNest;
    uint16                          _reserved2;
    
/** FM_Connect / FM_Disconnect *********************************************/

    FObject                         Application;

/** FM_Window_Setup / FM_Window_Cleanup ************************************/

    FRender                        *Render;
    struct Screen                  *Screen;
    APTR                            CyclePool;
    FPalette                       *Scheme;
    FPalette                       *DisabledScheme;

/** FM_Window_Open / FM_Window_Close ***************************************/

    struct Window                  *Win;
    struct AppWindow               *AppWin;

/** Preferences ************************************************************/

    STRPTR                          p_Scheme;
    STRPTR                          p_Decorator;
};

#define FF_WINDOW_SYS_OPENREQUEST               (1 << 0)
#define FF_WINDOW_SYS_KNOWNMINMAX               (1 << 1)
#define FF_WINDOW_SYS_COMPLEX                   (1 << 2) // Use complex refreshmode when some part of the window need to be redrawn
#define FF_WINDOW_SYS_REFRESH_SIMPLE            (1 << 3)
#define FF_WINDOW_SYS_REFRESH_DONE              (1 << 4)
#define FF_WINDOW_SYS_REFRESH_NEED              (1 << 5)
#define FF_WINDOW_SYS_LAYOUT_EVENT              (1 << 6)

/*

FF_WINDOW_SYS_KNOWNMINMAX

    is used to reduce  to  number  of  FM_AskMinMax  invokation.  Only  the
    function  Window_Resizable()  invokes  this  method,  but  the function
    Window_Resizable() is called 3 times when the window  is  opened,  thus
    the  function Window_Resizable(). This flag is set the first the method
    is invoked, then if the flag is  set  the  method  FM_AskMinMax  is  no
    longer   invoked.   The  flag  *MUST*  be  cleared  if  another  MinMax
    calculation is needed (rethink layout).

FF_WINDOW_SYS_LAYOUT_EVENT

    Is used to avoid superfulous drawings during  layout  operations.  This
    flag  is  set during FM_HandleEvent when a FF_EVENT_WINDOW_RESIZE event
    occurs. Because rendering is forbid nest count is  increased  when  the
    event  occurs,  the flag must be used to decrease FA_Render_Forbid when
    the     FF_EVENT_WINDOW_REFRESH     occurs.     This     is     because
    FF_EVENT_WINDOW_REFRESH  may happens at any time if window is in simple
    refresh mode.

*/

#define FF_WINDOW_GAD_DRAG                      (1 << 0)
#define FF_WINDOW_GAD_CLOSE                     (1 << 1)
#define FF_WINDOW_GAD_DEPTH                     (1 << 2)
#define FF_WINDOW_GAD_ICONIFY                   (1 << 3)

#define FF_WINDOW_WIN_ACTIVE                    (1 << 0)
#define FF_WINDOW_WIN_RESIZABLE                 (1 << 1)
#define FF_WINDOW_WIN_BORDERLESS                (1 << 2)
#define FF_WINDOW_WIN_BACKDROP                  (1 << 3)

#define FF_WINDOW_BOX_XPERCENT                  (1 << 0)
#define FF_WINDOW_BOX_YPERCENT                  (1 << 1)
#define FF_WINDOW_BOX_WPERCENT                  (1 << 2)
#define FF_WINDOW_BOX_HPERCENT                  (1 << 3)

/* FF_WINDOW_WIN_RESIZABLE is set by Window_MinMax() if the root object  is
not  fixed, and if the window is not borderless. The flag is made public by
the FA_Window_Resizable attribute. This attribute is used by decorators  to
know if they have to create size and zoom gadgets */

#define _root_area_public                       (LOD -> root_area_public)
#define _root_box                               (_root_area_public -> Box)
#define _root_x                                 (_root_box.x)
#define _root_y                                 (_root_box.y)
#define _root_w                                 (_root_box.w)
#define _root_h                                 (_root_box.h)
#define _root_minmax                            (_root_area_public -> MinMax)
#define _root_minw                              (_root_minmax.MinW)
#define _root_minh                              (_root_minmax.MinH)
#define _root_maxw                              (_root_minmax.MaxW)
#define _root_maxh                              (_root_minmax.MaxH)

#define _deco_area_public                       (LOD -> decorator_area_public)
#define _deco_box                               (_deco_area_public -> Box)
#define _deco_x                                 (_deco_box.x)
#define _deco_y                                 (_deco_box.y)
#define _deco_w                                 (_deco_box.w)
#define _deco_h                                 (_deco_box.h)
#define _deco_inner                             (_deco_area_public -> Inner)
#define _deco_il                                (_deco_inner.l)
#define _deco_it                                (_deco_inner.t)
#define _deco_ir                                (_deco_inner.r)
#define _deco_ib                                (_deco_inner.b)

/***************************************************************************/
/*** Prototypes ************************************************************/
/***************************************************************************/

bits32  window_collect_events   (FClass *Class,FObject Obj);
int32   window_resizable        (FClass *Class,FObject Obj);
void    window_minmax           (FClass *Class,FObject Obj);

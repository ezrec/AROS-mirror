/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <libraries/feelin.h>
#include <feelin/preference.h>

#define LayersBase                  FeelinBase -> Layers
#define GfxBase                     FeelinBase -> Graphics
#define UtilityBase                 FeelinBase -> Utility

#ifndef __MORPHOS__
#define IntuitionBase               FeelinBase -> Intuition
#endif

extern FClass                      *GadgetClass;
extern uint32                       FC_Gadget_Offset;

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

//#define F_USE_LAST_DAWN

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;

    bits8                           flags;
    uint8                           gadgets_w;
    uint8                           gadgets_h;
    uint8                           border;

    FObject                         gadget_close;
    FObject                         gadget_iconify;
    FObject                         gadget_zoom;
    FObject                         gadget_depth;
    FObject                         gadget_drag;
    FObject                         gadget_size;

    FNotifyHandler                 *nn_window_active;
    FNotifyHandler                 *nn_window_title;

    FPalette                       *scheme_active;
    FPalette                       *scheme_inactive;

    #ifdef F_USE_LAST_DAWN
    uint8                           last_drawn;
    #endif
};

#ifdef F_USE_LAST_DAWN
enum    {

        FV_Shade_LastDrawn_None,
        FV_Shade_LastDrawn_Active,
        FV_Shade_LastDrawn_Inactive

        };
#endif

#define FF_SHADE_ZOOM_IS_ICONIFY                (1 << 0)
#define FF_SHADE_TITLEBARONLY                   (1 << 1)

/* le drapeau FF_SHADE_TITLEBARONLY est posé lorsque  les  gadgets  (close,
depth...)  peuvent  être  dessinés  (ils  ne  tiennent  pas  dans  l'espace
disponible). Dans ce cas, seule la barre de titre doit être affichée */

#define FF_SHADE_DRAG                           (1 << 2)
#define FF_SHADE_SIZE                           (1 << 3)

/***/

struct GAD_LocalObjectData
{
    FAreaPublic                    *AreaPublic;
    uint8                           Type;
    bits8                           Flags;
    uint16                          _pad0;
    APTR                            Gadget;
//  the following is only used by title bar gadget.
    FObject                         TDisplay;
    STRPTR                          Prep[2];
};

/***************************************************************************/

#define DEF_TICKNESS                5
#define TITLEX1                     10
#define TITLEY1                     3
#define TITLEX2                     10
#define TITLEY2                     3
#define FV_GADSIZE_WIDTH            (DEF_TICKNESS * 4)
#define FV_DRAGBAR_MIN_WIDTH        8

enum    {

        FV_GadgetType_Close = 1,
        FV_GadgetType_Iconify,
        FV_GadgetType_Dragbar,
        FV_GadgetType_Zoom,
        FV_GadgetType_Depth,
        FV_GadgetType_Size
        
        };

#define FA_Gadget_Type                          (FCCA_BASE + 0)

#define FM_Gadget_Active                        (FCCM_BASE + 0)
#define FM_Gadget_ZoomWindow                    (FCCM_BASE + 1)
#define FM_Gadget_DepthWindow                   (FCCM_BASE + 2)

#define FF_Draw_Title                           FF_Draw_Custom_1
#define FF_Draw_Active                          FF_Draw_Custom_2

#define _Line(x1,y1,x2,y2)    _Move(x1,y1); _Draw(x2,y2)


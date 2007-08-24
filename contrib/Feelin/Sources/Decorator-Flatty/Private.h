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

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

#define DEF_SPACING_HORIZONTAL                  6
#define DEF_SPACING_VERTICAL                    3
#define DEF_DRAGBAR_BACK                        "<image type='gradient' start='shine' end='halfdark' />"
#define DEF_SIZEBAR_HEIGHT                      6
#define DEF_SIZEBAR_BACK                        "<image type='gradient' start='shine' end='halfdark' />"
#define DEF_PREPARSE_ACTIVE                     NULL
#define DEF_PREPARSE_INACTIVE                   NULL

#define DEF_IMAGE_CLOSE                         "<image type='picture' src='feelin:resources/decorators/flatty/close-render.png' />;<image type='picture' src='feelin:resources/decorators/flatty/close-select.png' />"
#define DEF_IMAGE_ZOOM                          "<image type='picture' src='feelin:resources/decorators/flatty/zoom-render.png' />;<image type='picture' src='feelin:resources/decorators/flatty/zoom-select.png' />"
#define DEF_IMAGE_DEPTH                         "<image type='picture' src='feelin:resources/decorators/flatty/depth-render.png' />;<image type='picture' src='feelin:resources/decorators/flatty/depth-select.png' />"

#define FF_DECO_DRAGBAR             (1 << 0)
#define FF_DECO_SIZEBAR             (1 << 1)
#define FF_DECO_SIZEBAR_ADDED       (1 << 2)
#define FF_DECO_TITLEONLY           (1 << 3)

#define FV_DRAGBAR_MIN_WIDTH        10

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;

    bits8                           flags;

    uint8                           sizebar_h;
    uint8                           gadgets_w;
    uint8                           gadgets_h;
    uint16                          dragbar_x1;
    uint16                          dragbar_x2;
    uint8                           spacing_horizontal;
    uint8                           spacing_vertical;

    FPalette                       *scheme_active;
    FPalette                       *scheme_inactive;
    STRPTR                          preparse_active;
    STRPTR                          preparse_inactive;

    FBox                            dragbar_box;

    FObject                         td;
    #if 0
    FObject                         dragbar_back;
    #endif
    FObject                         sizebar_back;

    APTR                            notify_handler_window_active;
    APTR                            notify_handler_window_title;

    FObject                         gadget_close;
    FObject                         gadget_zoom;
    FObject                         gadget_depth;
    APTR                            intuition_gadget_size;
    APTR                            intuition_gadget_drag;
};

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

#define FP_SPACING_HORIZONTAL                   "$decorator-flatty-spacing-horizontal"
#define FP_SPACING_VERTICAL                     "$decorator-flatty-spacing-vertical"
#define FP_DRAGBAR_BACK                         "$decorator-flatty-dragbar-back"
#define FP_SIZEBAR_HEIGHT                       "$decorator-flatty-sizebar-height"
#define FP_SIZEBAR_BACK                         "$decorator-flatty-sizebar-back"
#define FP_IMAGE_CLOSE                          "$decorator-flatty-image-close"
#define FP_IMAGE_ZOOM                           "$decorator-flatty-image-zoom"
#define FP_IMAGE_DEPTH                          "$decorator-flatty-image-depth"


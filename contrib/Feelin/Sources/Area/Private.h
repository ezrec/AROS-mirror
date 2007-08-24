/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/exec.h>
#include <proto/feelin.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/utility.h>

#include <libraries/feelin.h>
#include <graphics/gfxmacros.h>
#include <feelin/preference.h>

/****************************************************************************
*** Shared Variables ********************************************************
****************************************************************************/

#define       GfxBase               FeelinBase -> Graphics
#define       IntuitionBase         FeelinBase -> Intuition
#define       UtilityBase           FeelinBase -> Utility

/****************************************************************************
*** Private: Structures, Flags, Methods... **********************************
****************************************************************************/

/* all  reference  to  _render  check  (_render)  from  being  NULL  before
accessing to any field */

#undef _areadata
#define _areadata                               LOD->Public.

#undef _app
#define _app                                    ( (_render) ? _render -> Application : NULL)
#undef _win
#define _win                                    ( (_render) ? _render -> Window : NULL)
#undef _rp
#define _rp                                     ( (_render) ? _render -> RPort : NULL)
/*
#undef  _box
#undef  _inner
#undef  _minmax
#undef  _flags
#undef  _font
#undef  _render
#undef  _app
#undef  _rp
#undef  _pens
#undef  _weight
#undef  _parent
#undef  _win

#define _areadata                               LOD -> Public.
#define _parent                                 (_areadata Parent)
#define _box                                    (_areadata Box)
#define _inner                                  (_areadata Inner)
#define _minmax                                 (_areadata MinMax)
#define _flags                                  (_areadata Flags)

#ifdef F_CODE_AREA_PALETTE
#undef _palette
#define _palette                                (_areadata Palette)
#define _pens                                   (_palette->Pens)
#else
#define _pens                                   (_areadata Pens)
#endif
#define _font                                   (_areadata Font)
#define _weight                                 (_areadata Weight)
#define _render                                 (_areadata Render)
#define _app                                    ( (_render) ? _render -> Application : NULL)
#define _win                                    ( (_render) ? _render -> Window : NULL)
#define _rp                                     ( (_render) ? _render -> RPort : NULL)
*/

struct LocalObjectData
{
    FAreaPublic                     Public;

    uint8                           InputMode;
    uint8                           ControlChar;

    // 32bit aligned now

    int8                            HiddenCount;
    int8                            DisabledCount;
    bits16                          Flags;

    FEventHandler                  *Handler;

    FPalette                       *Scheme;
    FPalette                       *DisabledScheme;
    FFramePublic                   *FramePublic;
    
    STRPTR                          ContextHelp;
    FObject                         ContextMenu;

    FMinMax                         UserMinMax;
    FBox                            RethinkBox;

// Preferences
    STRPTR                          p_Font;
    STRPTR                          p_Back;
    STRPTR                          p_Scheme;
    STRPTR                          p_DisabledScheme;
};

/* PRIVATE FLAGS */

#define FF_AREA_CHAIN                           (1 << 0)
#define FF_AREA_NOFILL                          (1 << 1)
#define FF_AREA_INHERITED_PENS                  (1 << 2)
#define FF_AREA_INHERITED_DISABLEDPENS          (1 << 3)
#define FF_AREA_INHERITED_BACK                  (1 << 4)
#define FF_AREA_DRAGGABLE                       (1 << 5)
#define FF_AREA_DROPABLE                        (1 << 6)
#define FF_AREA_DRAGGING                        (1 << 7)
#define FF_AREA_HANDLEACTIVE                    (1 << 8)

/* FF_AREA_HANDLEACTIVE is set by FM_GoActive. It indicates that the active
state  representation  of  the  object  is  handled  by FC_Area (The method
FM_Active has not been patched by a subclass).  This  flag  is  cleared  by
FM_Setup. */

void area_try_add_handler(FClass *Class,FObject Obj,struct FeelinBase *FeelinBase);
void area_try_rem_handler(FClass *Class,FObject Obj,struct FeelinBase *FeelinBase);

/****************************************************************************
*** Preferences *************************************************************
****************************************************************************/

#define DEF_SCHEME_DATA                         NULL
#define DEF_DISABLEDSCHEME_DATA                 "<scheme shine='halfshine' dark='shadow' text='halfshadow' highlight='fill' />"


/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/feelin.h>

#include <libraries/feelin.h>
#include <feelin/preference.h>

#define DOSBase                     FeelinBase -> DOS
#define LayersBase                  FeelinBase -> Layers
#define GfxBase                     FeelinBase -> Graphics

#ifndef __MORPHOS
#define IntuitionBase               FeelinBase -> Intuition
#endif

extern struct Hook                  Hook_MinMax;

/****************************************************************************
*** Attributes & Methods ****************************************************
****************************************************************************/

enum    {

        FA_Cycle_Active,
        FA_Cycle_Entries,
        FA_Cycle_EntriesType,
        FA_Cycle_PreParse,
        FA_Cycle_Position,
        FA_Cycle_Layout,
        FA_Cycle_Level,
        FA_Cycle_Separator
        
        };
 
/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct PopMenuData
{
    FRender                        *Render;
    FObject                         TD;
    FObject                         menu_frame;
    FObject                         menu_back;
    FObject                         item_frame;
    
    struct Window                  *Win;
    FBox                            WinBox;
    
    FBox                            menu_box;
    FInner                          menu_brd;
    FInner                          item_brd[2];
    uint16                          item_height;
    uint16                          item_width;
    uint16                          Active;
};
 
struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;

    bits16                          flags;

    uint16                          level;
    STRPTR                         *strings;
    STRPTR                          preparse;

    uint16                          active;
    uint16                          numstrings;

    FObject                         text;
    FObject                         image;
    FAreaPublic                    *text_area_public;
    FAreaPublic                    *image_area_public;
    
    struct PopMenuData             *PMD;
//
    STRPTR                          p_PreParse;
    STRPTR                          p_Position;
    STRPTR                          p_Layout;
    STRPTR                          p_level;
};

#define FF_CYCLE_CREATED                        (1 << 0)
#define FF_CYCLE_BELOW                          (1 << 1)

#define _text_area_public                       (LOD -> text_area_public)
#define _text_box                               (_text_area_public -> Box)
#define _text_x                                 (_text_box.x)
#define _text_y                                 (_text_box.y)
#define _text_w                                 (_text_box.w)
#define _text_h                                 (_text_box.h)
#define _text_minmax                            (_text_area_public -> MinMax)
#define _text_minw                              (_text_minmax.MinW)
#define _text_minh                              (_text_minmax.MinH)
#define _text_maxw                              (_text_minmax.MaxW)
#define _text_maxh                              (_text_minmax.MaxH)

/****************************************************************************
*** Preferences *************************************************************
****************************************************************************/

#define DEF_CYCLE_FONT                          NULL
#define DEF_CYCLE_IMAGE                         "<image type='brush' src='defaults/cycle.bsh' />"
#define DEF_CYCLE_PREP                          NULL
#define DEF_CYCLE_LAYOUT                        FV_Cycle_Layout_Right
#define DEF_CYCLE_POSITION                      0
#define DEF_CYCLE_LEVEL                         3
#define DEF_CYCLE_BUTTON_FRAME                  "<frame id='35' padding-left='4' padding-right='6' />\n<frame id='36' padding-left='5' padding-right='6' padding-top='1' />"
#define DEF_CYCLE_BUTTON_BACK                   "fill;halfshadow"
#define DEF_CYCLE_MENU_FRAME                    "<frame id='23' padding='2' />"
#define DEF_CYCLE_MENU_BACK                     "fill"
#define DEF_CYCLE_ITEM_FRAME                    "<frame id='0' padding-width='2' padding-height='1' />\n<frame id='1' padding-width='1' />"
#define DEF_CYCLE_ITEM_BACK                     "halfshine"


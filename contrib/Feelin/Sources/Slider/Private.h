/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/feelin.h>

#include <intuition/intuition.h>
#include <libraries/feelin.h>
#include <feelin/preference.h>

#define GfxBase                     FeelinBase -> Graphics
#define UtilityBase                 FeelinBase -> Utility

/****************************************************************************
*** Attributes & Methods ****************************************************
****************************************************************************/

enum    { // attributes

        FA_Slider_Knob

        };

enum    { // resolves

        FM_Numeric_Reset,
        FM_Numeric_Stringify,
        FA_Numeric_Value,
        FA_Numeric_Min,
        FA_Numeric_Max,
        FA_Numeric_Buffer,
        FA_Numeric_StringArray

        };

#define FF_Slider_Scroll            (1 << 0)
#define FF_Slider_Buffer            (1 << 1)

#define FF_Draw_MoveKnob            FF_Draw_Custom_1

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;
    FObject                        *Knob;       // TextObject
    FAreaPublic                    *KnobAreaPublic;

    bits32                          Flags;

    int16                           kx;
    int16                           ky;
    uint16                          kw;
    uint16                          kh;

    int16                           PreviousPos;
    uint16                          MouseOff;
    uint32                          Saved;
    int32                           LastLayoutValue;
    int32                           last_drawn_value;

    FRender                        *OwnRender;
    struct RastPort                *OwnRPort;
    struct BitMap                  *OwnBitMap;

//  preferences

    STRPTR                          p_knob_preparse;
};

#define _knob_area_public                       (LOD -> KnobAreaPublic)
#define _knob_box                               (_knob_area_public -> Box)
#define _knob_x                                 (_knob_area_public -> Box.x)
#define _knob_y                                 (_knob_area_public -> Box.y)
#define _knob_w                                 (_knob_area_public -> Box.w)
#define _knob_h                                 (_knob_area_public -> Box.h)
#define _knob_minmax                            (_knob_area_public -> MinMax)
#define _knob_minw                              (_knob_minmax.MinW)
#define _knob_minh                              (_knob_minmax.MinH)
#define _knob_maxw                              (_knob_minmax.MaxW)
#define _knob_maxh                              (_knob_minmax.MaxH)
#define _knob_font                              (_knob_area_public -> Font)

void slider_cache_update(FClass *Class,FObject Obj);

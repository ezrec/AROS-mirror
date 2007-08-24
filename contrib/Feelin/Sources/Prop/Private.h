/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <intuition/intuition.h>
#include <libraries/feelin.h>
#include <feelin/preference.h>

#include <proto/graphics.h>
#include <proto/feelin.h>

#define GfxBase                     FeelinBase -> Graphics

/****************************************************************************
*** Attributes & Methods ****************************************************
****************************************************************************/

enum    {

        FA_Prop_Entries,
        FA_Prop_Visible,
        FA_Prop_First,
        FA_Prop_Step,
        FA_Prop_Knob,
        FA_Prop_Useless

        };

enum    {

        FM_Prop_Decrease,
        FM_Prop_Increase

        };

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
   FAreaPublic                     *AreaPublic;

   uint32                           Entries;
   uint32                           Visible;
   uint32                           First;
   uint32                           Step;

   bits16                           Flags;
   uint16                           PreviousPos;
   uint32                           MouseOff;
   uint32                           Saved;

   FObject                          Knob;
   FAreaPublic                     *KnobAreaPublic;
};

#define FF_Prop_Scroll                          (1 << 0)

#define _knob_area_public                         (LOD -> KnobAreaPublic)
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

enum    {

        FK_PROP_NONE,
        FK_PROP_MORE,
        FK_PROP_LESS,
        FK_PROP_STEP_MORE,
        FK_PROP_STEP_LESS,
        FK_PROP_MIN,
        FK_PROP_MAX

        };


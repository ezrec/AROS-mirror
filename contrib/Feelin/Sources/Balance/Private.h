/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <proto/graphics.h>
#include <proto/feelin.h>

#include <devices/inputevent.h>
#include <libraries/feelin.h>

#define       GfxBase               FeelinBase -> Graphics

/****************************************************************************
*** Attributes & Methods ****************************************************
****************************************************************************/

enum    { // attributes

        FA_Balance_QuickDraw

        };

/****************************************************************************
*** Object ******************************************************************
****************************************************************************/

struct LocalObjectData
{
    FAreaPublic                    *AreaPublic;

    bits32                          Flags;
    uint16                          Offset;
    int16                           Mouse;

    uint16                          BNum,ANum;
    uint16                          BMin,AMin;
    uint16                          BMax,AMax;
    uint16                          BSize,ASize;

    FObject                         ActiveObj;
};

#define FF_Balance_Moving                       (1 << 0)
#define FF_Balance_Vertical                     (1 << 1)
#define FF_Balance_QuickDraw                    (1 << 2)
#define FF_Balance_Couple                       (1 << 3)
#define FF_Balance_ComplexSet                   (1 << 4)

/* FF_Balance_ComplexSet is set when the rendering  mode  was  not  set  to
complex, and so have been modified for a better rendering */

#define _area_next                              (area -> Next)
#define _area_prev                              (area -> Prev)
#define _area_x                                 (area -> AreaPublic -> Box.x)
#define _area_y                                 (area -> AreaPublic -> Box.y)
#define _area_w                                 (area -> AreaPublic -> Box.w)
#define _area_h                                 (area -> AreaPublic -> Box.h)
#define _area_x2                                (area -> AreaPublic -> Box.x + area -> AreaPublic -> Box.w - 1)
#define _area_y2                                (area -> AreaPublic -> Box.y + area -> AreaPublic -> Box.h - 1)
#define _area_minw                              (area -> AreaPublic -> MinMax.MinW)
#define _area_minh                              (area -> AreaPublic -> MinMax.MinH)
#define _area_maxw                              (area -> AreaPublic -> MinMax.MaxW)
#define _area_maxh                              (area -> AreaPublic -> MinMax.MaxH)
#define _area_fixw                              (area -> AreaPublic -> MinMax.MinW == area -> AreaPublic -> MinMax.MaxW)
#define _area_fixh                              (area -> AreaPublic -> MinMax.MinH == area -> AreaPublic -> MinMax.MaxH)
#define _area_flags                             area -> AreaPublic -> Flags
#define _area_weight                            (area -> AreaPublic -> Weight)

#define _prev_x2                                (prev -> AreaPublic -> Box.x + prev -> AreaPublic -> Box.w - 1)
#define _prev_y2                                (prev -> AreaPublic -> Box.y + prev -> AreaPublic -> Box.h - 1)
#define _next_x                                 (next -> AreaPublic -> Box.x)
#define _next_y                                 (next -> AreaPublic -> Box.y)

#define _head_x                                 (head -> AreaPublic -> Box.x)
#define _head_y                                 (head -> AreaPublic -> Box.y)
#define _tail_x                                 (tail -> AreaPublic -> Box.x)
#define _tail_y                                 (tail -> AreaPublic -> Box.y)
#define _tail_x2                                (tail -> AreaPublic -> Box.x + tail -> AreaPublic -> Box.w - 1)
#define _tail_y2                                (tail -> AreaPublic -> Box.y + tail -> AreaPublic -> Box.h - 1)
#define _tail_flags                             (tail -> AreaPublic -> Flags)

#define _each                                   (area = head ; area ; area = area -> Next)

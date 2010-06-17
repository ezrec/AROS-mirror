#ifndef DEFINES_COLORWHEEL_H
#define DEFINES_COLORWHEEL_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/classes/gadgets/colorwheel/colorwheel.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for colorwheel
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __ConvertHSBToRGB_WB(__ColorWheelBase, __arg1, __arg2) \
        AROS_LC2NR(void, ConvertHSBToRGB, \
                  AROS_LCA(struct ColorWheelHSB*,(__arg1),A0), \
                  AROS_LCA(struct ColorWheelRGB*,(__arg2),A1), \
        struct Library *, (__ColorWheelBase), 5, ColorWheel)

#define ConvertHSBToRGB(arg1, arg2) \
    __ConvertHSBToRGB_WB(ColorWheelBase, (arg1), (arg2))

#define __ConvertRGBToHSB_WB(__ColorWheelBase, __arg1, __arg2) \
        AROS_LC2NR(void, ConvertRGBToHSB, \
                  AROS_LCA(struct ColorWheelRGB*,(__arg1),A0), \
                  AROS_LCA(struct ColorWheelHSB*,(__arg2),A1), \
        struct Library *, (__ColorWheelBase), 6, ColorWheel)

#define ConvertRGBToHSB(arg1, arg2) \
    __ConvertRGBToHSB_WB(ColorWheelBase, (arg1), (arg2))

__END_DECLS

#endif /* DEFINES_COLORWHEEL_H*/

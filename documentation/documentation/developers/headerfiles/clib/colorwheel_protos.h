#ifndef CLIB_COLORWHEEL_PROTOS_H
#define CLIB_COLORWHEEL_PROTOS_H

/*
    *** Automatically generated from 'colorwheel.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <intuition/gadgetclass.h>
#include <gadgets/colorwheel.h>
AROS_LP2(void, ConvertHSBToRGB,
         AROS_LPA(struct ColorWheelHSB*, hsb, A0),
         AROS_LPA(struct ColorWheelRGB*, rgb, A1),
         LIBBASETYPEPTR, ColorWheelBase, 5, ColorWheel
);
AROS_LP2(void, ConvertRGBToHSB,
         AROS_LPA(struct ColorWheelRGB*, rgb, A0),
         AROS_LPA(struct ColorWheelHSB*, hsb, A1),
         LIBBASETYPEPTR, ColorWheelBase, 6, ColorWheel
);

#endif /* CLIB_COLORWHEEL_PROTOS_H */

/* this file contains definition for the screen */

#include <aros/oldprograms.h>
#include <exec/types.h>
#include <intuition/intuition.h>
#include <graphics/gfxmacros.h>
#ifdef MANX
#include <functions.h>
#endif

#include "scrndef.h"

struct TextAttr myfont1 = {
    (UBYTE *)"topaz.font", 8, 0, 0
};



struct NewScreen SurfScrnDef = {
    0,0,   /* left and top edge */
    SurfInitW, SurfInitH+ButHeight,  /* width and height */
    SurfInitD,   /* num bitplanes bit planes */
    0,1, /* detail, block pen */
    SurfInitType, /* lores non interlaced */
    CUSTOMSCREEN,
    &myfont1,
    NULL /*(UBYTE *) "Screen" */,
    NULL,
    NULL
};


struct NewWindow SurfWinDef = {
    0, ButHeight, /* left, top */
    SurfInitW, SurfInitH, /* width, height */
    -1, -1, /* default detail and block pen */
    MOUSEBUTTONS | MOUSEMOVE| INTUITICKS /* | CLOSEWINDOW */,
    RMBTRAP | SIMPLE_REFRESH | GIMMEZEROZERO | BORDERLESS
     | BACKDROP,
    NULL, NULL, /* gadget, checkmark */
    NULL /* (UBYTE *) "BezSurf - By Eric Davies" */,
    NULL, /* pointer to screen */
    NULL, /* pointer to super bitmap */
    10, 10, 640, 200, /* min and max dimensions */
    CUSTOMSCREEN
};

struct NewWindow GadWinDef = {
    0, 0, /* left, top */
    SurfInitW, ButHeight, /* width, height */
    -1, 0, /* default detail and block pen */
    GADGETUP,
    SIMPLE_REFRESH | BORDERLESS | BACKDROP,
    NULL, NULL, /* gadget, checkmark */
    NULL /*(UBYTE *) "Gadwin" */,
    NULL, /* pointer to screen */
    NULL, /* pointer to super bitmap */
    10, 10, 10, 10, /* min and max dimensions */
    CUSTOMSCREEN
};



struct NewWindow CntrlWinDef = {
    0, 0, /* left, top */
    640, 180, /* width, height */
    -1, -1, /* default detail and block pen */
    CLOSEWINDOW| GADGETUP| MENUPICK,
    SIMPLE_REFRESH | WINDOWCLOSE | WINDOWDEPTH
    | WINDOWDRAG | WINDOWSIZING,
    NULL, NULL, /* gadget(set by program), checkmark */
    (UBYTE *)  "BezSurf - Control Panel - By Eric Davies",
    NULL, /* pointer to screen */
    NULL, /* pointer to super bitmap */
    10, 10, 640, 200, /* min and max dimensions */
    WBENCHSCREEN
};

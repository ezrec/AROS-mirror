#ifndef GADGETDEF_H_FILE
#define GADGETDEF_H_FILE
#include "mytypes.h"

#define MPtrXOffset -1L
#define MPtrYOffset -1L
#define MPtrCol17 623L
#define MPtrCol18 0L
#define MPtrCol19 976L

extern UWORD *HourGlass;

extern void InitGadgets();
extern void EndGadgets();
extern void GadgetHandler();

enum GadgetName { N_DefLines, N_EditLines, N_EditBez, N_Map, N_Wire,
                  N_Shaded, N_TiltAng, N_PtIntens, N_BkIntens, N_SurfDist,
                  N_PtLocX, N_PtLocY, N_PtLocZ, N_BezSlices, N_RevSlices,
                  N_RevAngle, N_Kspec, N_Kdiffuse, N_GoPanel, N_GoSurf,
                  N_BackPlane, N_RevStart, N_RepH, N_RepV };

struct GadExtens {
    float minfval, maxfval, curfval; /* min,max, and cur float values */
    short minival, maxival, curival; /* same but short ints */
    bool isfloat;   /* true if value is floating, false if short */
};

#endif /* GADGETDEF_H_FILE */

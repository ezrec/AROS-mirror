#ifndef POLY_H_FILE
#define POLY_H_FILE
#include <exec/types.h>
#include "mytypes.h"
#include "revolve.h"


#define RhomVxNum 4

typedef struct {
    ScrnPair    pt[RhomVxNum];
    float       depth;
    float       intensity;
} Rhomboid;

typedef struct {
    Rhomboid   rhom;
    short   bezindex,
            revindex;
} MapRhomboid;

extern bool SpecOn;

#define DefLightSrcX 0.0
#define DefLightSrcY 0.0
#define DefLightSrcZ 0.0
#define DefIntensity 0.8
#define DefAmbience 0.15
#define DefKd   0.8
#define DefKs   0.2


extern float Ambience,
             PtIntensity,
             Kd, Ks;

extern Pt3 LightSrc;

extern void RevNoShade();
extern void RevShade();
extern void RevMap();

#endif /* !POLY_H_FILE */

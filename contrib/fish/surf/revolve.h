#ifndef REVOLVE_H_FILE

#define REVOLVE_H_FILE
#include "mytypes.h"

typedef enum { RevX, RevY } RevAxisType;
extern RevAxisType RevAxis;

#define DefRevMeshVal 30
#define DefSurfDist 3000
#define DefTilt 15
#define DefRotRange 360
#define DefRotStart 0
#define DefPersp false
#define DefViewDist 3000

typedef struct {
    short x, y;
} ScrnPair;




typedef struct {
        float x, y, z;
} Pt3;

typedef struct {
        Pt3 d3;
        ScrnPair d2;
} PtGen;

extern short RevMesh;
extern short RevImageR,
             RevImageB;

extern void SetRotStart();
extern void SetRotRange(/* Panel_item, int, struct input_event */);
extern void SetSecAng(/* Panel_item, int, struct input_event */);
extern void SetPolyMode( /* Panel_item, int, struct input_event */);
extern void SetFitBez( /* Panel_item, int, struct input_event */);
extern bool Revolve( /* Panel_item, int, struct input_event */);
extern void SetRevAxis( /* Panel_item, int, struct input_event */);
extern void SetRevMesh( /* Panel_item, int, struct input_event */);
extern void SetSurfDist(/* Panel_item, int, struct input_event */);
extern void SetViewDist();
extern void SetPerspective();
#endif /* !REVOLVE_H_FILE */

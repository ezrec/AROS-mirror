/* this file contains definition for the screen */

#include <aros/oldprograms.h>
#include "scrnio.ih"
#include <exec/memory.h>
#ifdef MANX
#include <functions.h>
#endif

#include "scrndef.h"
#include "gadgetdef.h"
#include "mytypes.h"
#include "bezpt.h"
#include "poly.h"
#include "revolve.h"
#include "readilbm.h"

void OutErr(char *);
void CloseDisplay();
void GadgetSetVal(struct Gadget *);
void GadgetUpdate(struct Gadget *, bool);

/*
 * definition for image of hourglass
 */
static UWORD HourGlassDat[] = {
 0x0, 0x0, 0x0, 0xffff,
 0x7ffe, 0x8001, 0x6006, 0x9ff9,
 0x2004, 0x5ffa, 0x1c38, 0x2ff4,
 0xf70, 0x17e8, 0x7e0, 0xbd0,
 0x2c0, 0x5a0, 0x240, 0x5a0,
 0x420, 0xbd0, 0x910, 0x17e8,
 0x1788, 0x2ff4, 0x2fe4, 0x5ffa,
 0x7ff6, 0x9ff9, 0x7ffe, 0x8001,
 0x0, 0xffff, 0x0, 0x0
 };

UWORD *HourGlass = NULL;




/*
 * actual gadget definitions
 */
#define TOP 13
#define ROW 9
#define COL 8
#define TxOff 1
       /*
        * start of boolean gadgets
        */
#define BOOLGADFLAGS (RELVERIFY|TOPBORDER)
#define BOOLTOP 0
#define BOOLLEFT 0

static struct IntuiText TextDefLines = {
    -1,-1,JAM1, 0, TxOff, NULL,(UBYTE *) "Lines", NULL };

static struct Gadget G_DefLines = {
    NULL,
    BOOLLEFT , BOOLTOP , 5*COL, ROW, /* loc and size of hit box */
    GADGHBOX,    /* complemented when pressed */
    BOOLGADFLAGS,    /* just get gadget up messages */
    BOOLGADGET,
    NULL, NULL,
    &TextDefLines,
    0, NULL,
    (int)N_DefLines,
    NULL
 };


static struct IntuiText TextEditBez = {
    -1,-1,JAM1, 0, TxOff, NULL,(UBYTE *) "Curves", NULL };

static struct Gadget G_EditBez = {
    &G_DefLines,
    BOOLLEFT+COL*6, BOOLTOP, 6*COL, ROW, /* loc and size of hit box */
    GADGHBOX,    /* complemented when pressed */
    RELVERIFY,    /* just get gadget up messages */
    BOOLGADGET,
    NULL, NULL,
    &TextEditBez,
    0, NULL,
    (int)N_EditBez,
    NULL
 };


static struct IntuiText TextMap = {
    -1,-1,JAM1, 0, TxOff, NULL,(UBYTE *) "Map", NULL };

static struct Gadget G_Map = {
    &G_EditBez,
    BOOLLEFT +24*COL, BOOLTOP, 3*COL, ROW, /* loc and size of hit box */
    GADGHBOX,    /* complemented when pressed */
    RELVERIFY,    /* just get gadget up messages */
    BOOLGADGET,
    NULL, NULL,
    &TextMap,
    0, NULL,
    (int)N_Map,
    NULL
 };


static struct IntuiText TextWire = {
    -1,-1,JAM1, 0, TxOff, NULL,(UBYTE *) "Wire", NULL };

static struct Gadget G_Wire = {
    &G_Map,
    BOOLLEFT +13*COL, BOOLTOP, 4*COL, ROW, /* loc and size of hit box */
    GADGHBOX,    /* complemented when pressed */
    RELVERIFY,    /* just get gadget up messages */
    BOOLGADGET,
    NULL, NULL,
    &TextWire,
    0, NULL,
    (int)N_Wire,
    NULL
 };

static struct IntuiText TextShaded = {
    -1,-1,JAM1, 0, TxOff, NULL,(UBYTE *) "Shade", NULL };

static struct Gadget G_Shaded = {
    &G_Wire,
    BOOLLEFT +18*COL, BOOLTOP, 5*COL, ROW, /* loc and size of hit box */
    GADGHBOX,    /* complemented when pressed */
    RELVERIFY,    /* just get gadget up messages */
    BOOLGADGET,
    NULL, NULL,
    &TextShaded,
    0, NULL,
    (int)N_Shaded,
    NULL
 };

static struct IntuiText TextGoPanel = {
    -1,-1,JAM1, 0, TxOff, NULL,(UBYTE *) "Panel", NULL };

static struct Gadget G_GoPanel = {
    &G_Shaded,
    BOOLLEFT +28*COL, BOOLTOP, 5*COL, ROW, /* loc and size of hit box */
    GADGHBOX,    /* complemented when pressed */
    RELVERIFY,    /* just get gadget up messages */
    BOOLGADGET,
    NULL, NULL,
    &TextGoPanel,
    0, NULL,
    (int)N_GoPanel,
    NULL
 };

static struct IntuiText TextGoSurf = {
    1,-1,JAM1, 0, TxOff, NULL,(UBYTE *) "Display", NULL };

static short S_GoSurf[] = {
    -2,-1,  -2,ROW+1,  7*COL+2,ROW+1,  7*COL+2,-1, -2,-1
 };

static struct Border B_GoSurf = { 0, 0, 1, 0, JAM1, 5, S_GoSurf, NULL };

static struct Gadget G_GoSurf = {
    NULL,
    2*COL, TOP, 7*COL, ROW, /* loc and size of hit box */
    GADGHBOX,    /* complemented when pressed */
    RELVERIFY,    /* just get gadget up messages */
    BOOLGADGET,
    (APTR)&B_GoSurf, NULL,
    &TextGoSurf,
    0, NULL,
    (int)N_GoSurf,
    NULL
 };

        /*
         * beginning of slider gadgets
         */
#define GadFlags    (GADGIMAGE | GADGHNONE)  /* my gadget flags */
#define MyPropType (PROPGADGET)

/*
 *  definitions for the number of Revolution Slices
 */

static char DispRevSlices[] = "Rev Slices=[   ]";

static struct IntuiText TextRevSlices = {
    1,1,JAM1, -16*COL, TxOff, NULL,
    (UBYTE *)DispRevSlices, NULL
 };

struct GadExtens VRevSlices = {0.0, 0.0, 0.0, 1, 360, DefRevMeshVal, false };

static struct Gadget G_RevSlices = {
    &G_GoSurf,
    COL*57 , TOP+ROW*2, COL*15, ROW, /* loc and size */
    GadFlags,
    RELVERIFY,
    MyPropType,
    NULL,
    NULL , /* high lighted */
    &TextRevSlices,
    0x0,
    NULL,
    (int)N_RevSlices,
    (APTR) &VRevSlices
 };

/*
 * definition for the background shade
 */
static char DispBackPlane[] = { "Background = [  ]" };

static struct IntuiText TextBackPlane = {
    1,1,JAM1, -17*COL, TxOff, NULL,
    (UBYTE *)DispBackPlane, NULL
 };

struct GadExtens VBackPlane = {0.0, 0.0, 0.0, 0, 31, DefBkPlane, false};


static struct Gadget G_BackPlane = {
    &G_RevSlices,
    COL*57 , TOP+ROW*13, COL*15, ROW, /* loc and size */
    GadFlags,
    RELVERIFY,
    MyPropType,
    NULL,
    NULL , /* high lighted */
    &TextBackPlane,
    0x0,
    NULL,
    (int)N_BackPlane,
    (APTR) &VBackPlane
 };


/*
 *  definitions for the number of Revolution Angles
 */

static char DispRevAngle[] = { "Rev Angle=[   ]" };

static struct IntuiText TextRevAngle = {
    1,1,JAM1, -15*COL, TxOff, NULL,
    (UBYTE *)DispRevAngle, NULL
 };

struct GadExtens VRevAngle = { 0.0, 0.0, 0.0, 0, 360, DefRotRange, false };


static struct Gadget G_RevAngle = {
    &G_BackPlane,
    COL*57 , TOP+ROW*3, COL*15, ROW, /* loc and size */
    GadFlags,
    RELVERIFY,
    MyPropType ,
    NULL,
    NULL, /* high lighted */
    &TextRevAngle,
    0x0,
    NULL,
    (int)N_RevAngle,
    (APTR) &VRevAngle
 };
/*
 *  definitions for start rotation angle
 */

static char DispRevStart[] = { "Start Angle=[   ]" };

static struct IntuiText TextRevStart = {
    1,1,JAM1, -17*COL, TxOff, NULL,
    (UBYTE *)DispRevStart, NULL
 };

struct GadExtens VRevStart = { 0.0, 0.0, 0.0, 0, 360, DefRotStart, false };


static struct Gadget G_RevStart = {
    &G_RevAngle,
    COL*57 , TOP+ROW*4, COL*15, ROW, /* loc and size */
    GadFlags,
    RELVERIFY,
    MyPropType ,
    NULL,
    NULL, /* high lighted */
    &TextRevStart,
    0x0,
    NULL,
    (int)N_RevStart,
    (APTR) &VRevStart
 };

/*
 *  definitions for the number of Bezier Slices
 */

static char DispBezSlices[] = { "Bez Slices=[   ]" };

static struct IntuiText TextBezSlices = {
    1,1,JAM1, -16*COL, TxOff, NULL,
    (UBYTE *)DispBezSlices, NULL
 };


struct GadExtens VBezSlices = { 0.0, 0.0, 0.0, 1, 100, DefBezMeshVal, false };

static struct Gadget G_BezSlices = {
    &G_RevStart,
    COL*20 , TOP+ROW*2, COL*15, ROW, /* loc and size */
    GadFlags,
    RELVERIFY,
    MyPropType ,
    NULL,
    NULL, /* high lighted */
    &TextBezSlices,
    0x0,
    NULL,
    (int)N_BezSlices,
    (APTR)&VBezSlices
 };

/*
 *  definitions for tilt angle
 */

static char DispTiltAng[] = { "Tilt Angle=[   ]" };

static struct IntuiText TextTiltAng = {
    1,1,JAM1, -16*COL, TxOff, NULL,
    (UBYTE *)DispTiltAng, NULL
 };


struct GadExtens VTiltAng = { 0.0, 0.0, 0.0, 0, 360, DefTilt, false };

static struct Gadget G_TiltAng = {
    &G_BezSlices,
    COL*20 , TOP+ROW*3, COL*15, ROW, /* loc and size */
    GadFlags,
    RELVERIFY,
    MyPropType ,
    NULL,
    NULL, /* high lighted */
    &TextTiltAng,
    0x0,
    NULL,
    (int)N_TiltAng,
    (APTR) &VTiltAng
 };


/*
 * definitions for K Specular
 */

static char DispKspec[] = {"Kspecular=[     ] " };

static struct IntuiText TextKspec = {
    1,1,JAM1, -sizeof(DispKspec)*COL, TxOff, NULL,
    (UBYTE *)DispKspec, NULL
 };


struct GadExtens VKspec = { 0.0, 1.0, DefKs, 0, 0, 0, true };

static struct Gadget G_Kspec = {
    &G_TiltAng,
    49*COL , TOP+ROW*9, COL*15, ROW, /* loc and size */
    GadFlags,
    RELVERIFY,
    MyPropType ,
    NULL,
    NULL, /* high lighted */
    &TextKspec,
    0x0,
    NULL,
    (int)N_Kspec,
    (APTR) &VKspec
 };

/*
 * definitions for K diffuse
 */

static char DispKdiffuse[] = {"Kdiffuse=[     ] " };

static struct IntuiText TextKdiffuse = {
    1,1,JAM1, -sizeof(DispKdiffuse)*COL, TxOff, NULL,
    (UBYTE *)DispKdiffuse, NULL
 };


struct GadExtens VKdiffuse = { 0.0, 1.0, DefKd, 0, 0, 0, true };

static struct Gadget G_Kdiffuse = {
    &G_Kspec,
    49*COL , TOP+ROW*10, COL*15, ROW, /* loc and size */
    GadFlags,
    RELVERIFY,
    MyPropType ,
    NULL,
    NULL, /* high lighted */
    &TextKdiffuse,
    0x0,
    NULL,
    (int)N_Kdiffuse,
    (APTR) &VKdiffuse
 };



/*
 * definitions for DispPtIntens
 */


static struct IntuiText PtProperties = {
    1,1,JAM1, -10*COL, -ROW, NULL,
    (UBYTE *)"Properties", NULL
 };

static char DispPtIntens[] = {"Intensity=[     ] " };

static struct IntuiText TextPtIntens = {
    1,1,JAM1, -sizeof(DispPtIntens)*COL, TxOff, NULL,
    (UBYTE *)DispPtIntens, &PtProperties
 };


struct GadExtens VPtIntens = { 0.0, 2.5, DefIntensity , 0, 0, 0, true };

static struct Gadget G_PtIntens = {
    &G_Kdiffuse,
    49*COL , TOP+ROW*8, COL*15, ROW, /* loc and size */
    GadFlags,
    RELVERIFY,
    MyPropType ,
    NULL,
    NULL, /* high lighted */
    &TextPtIntens,
    0x0,
    NULL,
    (int)N_PtIntens,
    (APTR) &VPtIntens
 };




/*
 * definitions for background intensity
 */
static struct IntuiText DispAmbient= {
    1,1,JAM1,- COL*3, -ROW, NULL,
    (UBYTE *) "Other", NULL
 };

static char DispBkIntens[] = { "Ambience=[     ] " };

static struct IntuiText TextBkIntens = {
    1,1,JAM1, -sizeof(DispBkIntens)*COL, TxOff, NULL,
    (UBYTE *) DispBkIntens, &DispAmbient
 };

struct GadExtens VBkIntens = { 0.0, 1.0, DefAmbience, 0, 0, 0, true };


static struct Gadget G_BkIntens = {
    &G_PtIntens,
    20*COL , TOP+ROW*13, COL*15, ROW, /* loc and size */
    GadFlags,
    RELVERIFY,
    MyPropType,
    NULL,
    NULL, /* high lighted */
    &TextBkIntens,
    0x0,
    NULL,
    (int)N_BkIntens,
    (APTR)&VBkIntens
 };




/*
 * Definitions for Surface distance
 */

static char DispSurfDist[] = {"Surf Dist=[     ] " };

static struct IntuiText TextSurfDist = {
    1,1,JAM1, -sizeof(DispSurfDist)*COL, TxOff, NULL,
    (UBYTE *) DispSurfDist, NULL
 };


struct GadExtens VSurfDist = {0.0, 0.0, 0.0, -500, 3000, DefSurfDist, false};

static struct Gadget G_SurfDist = {
    &G_BkIntens,
    20*COL , TOP+ROW*4, COL*15, ROW, /* loc and size */
    GadFlags,
    RELVERIFY,
    MyPropType,
    NULL,
    NULL, /* high lighted */
    &TextSurfDist,
    0x0,
    NULL,
    (int)N_SurfDist,
    (APTR)&VSurfDist
 };




/*
 * definitions for location.x
 */
static struct IntuiText PointLight = {
    1,1,JAM1, 9*COL, -ROW*2, NULL,
    (UBYTE *) "Point Light Source", NULL
 };

static struct IntuiText TextPtLocXYZ = {
    1,1,JAM1, -4, -ROW, NULL,
    (UBYTE *) "Location", &PointLight
 };

static char DispPtLocX[] = { "x=[     ] " };

static struct IntuiText TextPtLocX = {
    1,1,JAM1, -sizeof(DispPtLocX)*COL, TxOff, NULL,
    (UBYTE *) DispPtLocX, &TextPtLocXYZ
 };


struct GadExtens VPtLocX = {0.0, 0.0, 0.0, -6000, 6000, DefLightSrcX, false };

static struct Gadget G_PtLocX = {
    &G_SurfDist,
    12*COL , TOP+ROW*8, COL*15, ROW, /* loc and size */
    GadFlags,
    RELVERIFY,
    MyPropType,
    NULL,
    NULL, /* high lighted */
    &TextPtLocX,
    0x0,
    NULL,
    (int)N_PtLocX,
    (APTR) &VPtLocX
 };



/*
 * definitions for location.x
 */

static char DispPtLocY[] = "y=[     ] ";

static struct IntuiText TextPtLocY = {
    1,1,JAM1, -sizeof(DispPtLocY)*COL, TxOff, NULL,
    (UBYTE *) DispPtLocY, NULL
 };


struct GadExtens VPtLocY = { 0.0, 0.0, 0.0, -6000, 6000, DefLightSrcY, false };

static struct Gadget G_PtLocY = {
    &G_PtLocX,
    12*COL , TOP+ROW*9, COL*15, ROW, /* loc and size */
    GadFlags,
    RELVERIFY,
    MyPropType,
    NULL,
    NULL, /* high lighted */
    &TextPtLocY,
    0x0,
    NULL,
    (int)N_PtLocY,
    (APTR) &VPtLocY
 };



 /*
  * definitions for location.z
  */

static char DispPtLocZ[] = { "z=[     ] " };

static struct IntuiText TextPtLocZ = {
    1,1,JAM1, -sizeof(DispPtLocZ)*COL, TxOff, NULL,
    (UBYTE *)DispPtLocZ, NULL
 };


struct GadExtens VPtLocZ ={ 0.0, 0.0, 0.0, -1000, 3000, DefLightSrcZ, false };

static struct Gadget G_PtLocZ = {
    &G_PtLocY,
    12*COL , TOP+ROW*10, COL*15, ROW, /* loc and size */
    GadFlags,
    RELVERIFY,
    MyPropType,
    NULL,
    NULL, /* high lighted */
    &TextPtLocZ,
    0x0,
    NULL,
    (int)N_PtLocZ,
    (APTR) &VPtLocZ
 };

 /*
  * definitions for Map replication in horizontal axis
  */

static char DispRepH[] = { "Map RepH=[     ] " };

static struct IntuiText TextRepH = {
    1,1,JAM1, -sizeof(DispRepH)*COL, TxOff, NULL,
    (UBYTE *)DispRepH, NULL
 };


struct GadExtens VRepH ={ 0.0, 0.0, 0.0, 1, 300, DefRepH, false };

static struct Gadget G_RepH = {
    &G_PtLocZ,
    20*COL , TOP+ROW*15, COL*15, ROW, /* loc and size */
    GadFlags,
    RELVERIFY,
    MyPropType,
    NULL,
    NULL, /* high lighted */
    &TextRepH,
    0x0,
    NULL,
    (int)N_RepH,
    (APTR) &VRepH
 };


 /*
  * definitions for map replication in vertical axis
  */

static char DispRepV[] = { "Map RepV=[     ] " };

static struct IntuiText TextRepV = {
    1,1,JAM1, -sizeof(DispRepV)*COL, TxOff, NULL,
    (UBYTE *)DispRepV, NULL
 };


struct GadExtens VRepV ={ 0.0, 0.0, 0.0, 1, 300, DefRepV, false };

static struct Gadget G_RepV = {
    &G_RepH,
    57*COL , TOP+ROW*15, COL*15, ROW, /* loc and size */
    GadFlags,
    RELVERIFY,
    MyPropType,
    NULL,
    NULL, /* high lighted */
    &TextRepV,
    0x0,
    NULL,
    (int)N_RepV,
    (APTR) &VRepV
 };






void LoadChipMem( dest, src, srcsize)
    UWORD **dest;
    UWORD *src;
    int srcsize;
{
    UWORD *d;

    d = (UWORD *)AllocMem( srcsize, MEMF_CHIP );
    if( !d ) {
        OutErr("not enough chip mem");
        CloseDisplay();
        exit(-1);
    }
    *dest = d;
    while( srcsize ) {
        *d++ = *src++;
        srcsize -= sizeof( UWORD );
    }
}


void InitGadgets()
{
    struct Gadget *vp;
    LoadChipMem( &HourGlass, HourGlassDat, sizeof(HourGlassDat));


    CntrlWinDef.FirstGadget = &G_RepV;
    GadWinDef.FirstGadget = &G_GoPanel;
    /*
     * fill in numeric fields
     */
    for( vp = CntrlWinDef.FirstGadget; vp; vp = vp->NextGadget ) {
        GadgetSetVal( vp );
        GadgetUpdate(vp, false );
    }
}


void EndGadgets()
{
    if( HourGlass ) {
        FreeMem(HourGlass, sizeof(HourGlassDat ));
    }
}

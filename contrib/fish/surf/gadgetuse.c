/* this file contains definition for the screen */

#include <aros/oldprograms.h>
#include "scrnio.ih"
#include <exec/memory.h>
#ifdef MANX
#include <functions.h>
#endif
#include <stdio.h>
#include <string.h>

#include "scrndef.h"
#include "gadgetdef.h"
#include "mytypes.h"
#include "poly.h"
#include "readilbm.h"

#define GetExtens(gei) ((struct GadExtens *)gei->UserData)
extern struct GfxBase * GfxBase;

void SetPolyDraw();
void SetBezMesh( int );
void OutErr(char *);
void CloseDisplay();

void GadgetSetVal(gad)
    struct Gadget *gad;
{
    struct GadExtens *vp;
    struct PropInfo *prop;
    long gadval;

    if( !(gad->GadgetType & PROPGADGET) ) {
        return;
    }

    if( !gad->GadgetRender ) {
        gad->GadgetRender = (APTR) malloc(sizeof(struct Image));
        if( !gad->GadgetRender ) {
            OutErr("Not enough memory for gadgets");
            CloseDisplay();
            exit( 0 );
        }
    }
    if( !gad->SpecialInfo ) {
        static struct PropInfo dummyprop = {
            FREEHORIZ|AUTOKNOB,
            0x8000, 1, /* HorizPot = initial value */
            0xff, 0, /* not really of interest */
            0,0,0,0,0,0
        };

        gad->SpecialInfo = (APTR) malloc(sizeof(struct PropInfo));
        if( !gad->SpecialInfo ) {
            OutErr("Not enough memory for gadgets");
            CloseDisplay();
            exit( 0 );
        }
        *(struct PropInfo *)gad->SpecialInfo = dummyprop;
    }

    vp = GetExtens( gad );
    if(!vp)  {
        return;
    }
    prop = (struct PropInfo *) gad->SpecialInfo;

    if( vp->isfloat ) {
        gadval = (long)( (long)MAXPOT *
            ( vp->curfval - vp->minfval )/(vp->maxfval - vp->minfval));
        prop->HorizBody = MAXPOT /( 15 * 8 );
    }
    else {
        gadval = ( (long)MAXPOT *
            ( (long) vp->curival - vp->minival))/(vp->maxival - vp->minival);
        prop->HorizBody = MAXPOT /( vp->maxival - vp->minival );
    }

    prop->HorizPot = gadval;
}



void GadgetUpdate(gad, exists)
    struct Gadget *gad;
    bool exists; /* has the gadget already been displayed? */
{
    struct GadExtens *vp;
    long potvalue;
    char dbuff[25];
    char *tx, *dx;
    struct IntuiText *it;

    if(!( gad->GadgetType & PROPGADGET) ) {
        return;
    }

    vp = GetExtens( gad );
    if(!vp) return;

    potvalue = ((struct PropInfo *) gad->SpecialInfo)->HorizPot;

    if( vp->isfloat ) {
        float temp;
        temp = ( potvalue * (vp->maxfval - vp->minfval ))/ MAXPOT
                + vp->minfval;
        vp->curfval = temp;
#if !MANX
        sprintf(dbuff,"%f   ", temp);
#else
        ftoa(temp, dbuff, sizeof(dbuff)-4, 1);
#endif /* !MANX */
    }
    else {
        long temp;
        temp = (long)( potvalue * (vp->maxival - vp->minival ))/ MAXPOT
                + vp->minival;
        vp->curival = temp;
        sprintf(dbuff,"%-12d", temp);
    }
    /*
     * find '['
     */
    it = gad->GadgetText;
    for( tx = (char *)it->IText; *tx && *tx != '['; tx++ ) {
        ;
    }
    if( !*tx ) {
        return; /* something screwy */
    }
    tx++; /* skip past opening bracket */
    dx = dbuff;
    while( *tx != ']' ) {
        *tx++ = *dx++;
    }

    if(exists){
        long tempx, tempy;
        tempx = it->LeftEdge + gad->LeftEdge;
        tempy = it->TopEdge + gad->TopEdge + 6; /*fudge factor for baseline*/
        Move( CntrlWin->RPort, tempx, tempy );
        SetAPen(CntrlWin->RPort,it->FrontPen );
        Text( CntrlWin->RPort, it->IText, strlen(it->IText ));
    }
}

void SetHourGlass() {
    SetPointer( SurfWin, HourGlass, 16, 16, MPtrXOffset, MPtrYOffset);
    SetPointer( GadWin, HourGlass, 16, 16, MPtrXOffset, MPtrYOffset);
    SetPointer( CntrlWin, HourGlass, 16, 16, MPtrXOffset, MPtrYOffset);
}

void ClearHourGlass() {
    ClearPointer(SurfWin);
    ClearPointer(GadWin);
    ClearPointer(CntrlWin);
}

void GadgetHandler(gadaddr)
    struct Gadget *gadaddr;
{
    short curival;
    float curfval;

    if( gadaddr->UserData ) {
        GadgetUpdate( gadaddr, true );
        curival = ((struct GadExtens *) gadaddr->UserData)->curival;
        curfval = ((struct GadExtens *) gadaddr->UserData)->curfval;
    }

    switch( (enum GadgetName) gadaddr->GadgetID ) {
    case N_PtLocX:
        LightSrc.x = (float)curival;
        break;
    case N_PtLocY:
        LightSrc.y = (float)curival;
        break;
    case N_BackPlane:
        BackColor = curival;
        break;
    case N_PtLocZ:
        LightSrc.z = (float)curival;
        break;
    case N_BkIntens:
        Ambience = curfval;
        break;
    case N_PtIntens:
        PtIntensity = curfval;
        break;
    case N_Kdiffuse:
        Kd = curfval;
        break;
    case N_Kspec:
        Ks = curfval;
        break;
    case N_Map:
        /* ResetCurve(); */
        SetHourGlass();
        RevMap();
        ClearHourGlass();
        break;
    case N_Wire:
        SetHourGlass();
        RevNoShade();
        ClearHourGlass();
        break;
    case N_Shaded:
        SetHourGlass();
        RevShade();
        ClearHourGlass();
        break;
    case N_EditBez:
        SetFitBez();
        break;
    case N_DefLines:
        SetPolyDraw();
        break;
    case N_RevAngle:
        SetRotRange( curival );
        break;
    case N_RevStart:
        SetRotStart( curival );
        break;
    case N_TiltAng:
        SetSecAng( curival );
        break;
    case N_RevSlices:
        SetRevMesh( curival );
        break;
    case N_BezSlices:
        SetBezMesh( curival );
        break;
    case N_SurfDist:
        SetSurfDist( curival);
        break;
    case N_RepV:
        MapRepV = curival;
        PrepImgPix();
        break;
    case N_RepH:
        MapRepH = curival;
        PrepImgPix();
        break;
    case N_GoSurf:
        ScreenToFront( SurfScrn );
        break;
    case N_GoPanel:
        WBenchToFront();
        WindowToFront( CntrlWin );
        break;
    default:
        break;
    }
}

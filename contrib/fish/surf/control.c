#include "bezpt.h"
#include "control.h"
#include "scrnio.h"
#include "mytypes.h"

CURMODE CurMode  = DRAWPOLY;

void DrawBox(int, int, int);
void DrawSqr(int, int, int);
void ClrWindow(bool);
void ClearSegments();

void SetFitBez()
{
    if( GetNumSegs() < 1 )  {
        return;
    }
    ClrWindow(true);
    CurMode = FITBEZIER;
    XdrawAllBezSegs();
}




void SetPolyDraw()
{
    ClrWindow(true);
    CurMode = DRAWPOLY;
    ClearSegments();
}

/* one of Leonaards Additions */

void EditStartPt(x,y)
    int x, y;
{
    DrawStartPt();
    XdrawBezSeg();

    if( PrevSegNo() != GetNumSegs() ) {
        PrevSeg();
        XdrawBezSeg();
        NextSeg();
    }

    StartPtX( GetCurSeg()) = x;
    StartPtY( GetCurSeg()) = y;

    if( PrevSegNo() != GetNumSegs() ) {
        PrevSeg();
        XdrawBezSeg();
        NextSeg();
    }

    XdrawBezSeg();
    DrawStartPt();
}

void EditEndPt(x,y)
    int x, y;
{
    DrawEndPt();
    XdrawBezSeg();

    if( NextSegNo() != GetFirstSeg() ) {
        NextSeg();
        XdrawBezSeg();
        PrevSeg();
    }

    EndPtX( GetCurSeg()) = x;
    EndPtY( GetCurSeg()) = y;

    if( NextSegNo() != GetFirstSeg() ) {
        NextSeg();
        XdrawBezSeg();
        PrevSeg();
    }

    XdrawBezSeg();
    DrawEndPt();
}



void EditControl0(x,y)
    int x, y;
{
        DrawControl0();
        XdrawBezSeg();
        Cntrl1X(GetCurSeg()) = x;
        Cntrl1Y(GetCurSeg()) = y;
        XdrawBezSeg();
        DrawControl0();
}

void EditControl1(x,y)
int x, y;
{
        DrawControl1();
        XdrawBezSeg();
        Cntrl2X(GetCurSeg()) = x;
        Cntrl2Y(GetCurSeg()) = y;
        XdrawBezSeg();
        DrawControl1();
}

#include <aros/oldprograms.h>
#include "mytypes.h"
#include "scrnio.h"
#include "control.h"
#include "bezpt.h"

BezCoord Bezpt[MaxSegs+1];

int NumBezPts;
int ActSeg;
bool SegDrawn;
int BezMesh = DefBezMeshVal;
float BezStepSize = 1.0/DefBezMeshVal;

void DrawBox(int, int, int);
void DrawSqr(int, int, int);
void DrawPnt(int, int, int);
void DrawLine(int, int, int, int, int);
void ClrWindow(bool);

void SetBezMesh( value )
    int value;
{
    BezMesh = value;
    BezStepSize = 1.0/value;
}


void ClearSegments()
{
    NumBezPts = 0;
    ActSeg = 0;
}


static  float xa, xb, xc, xd;
static  float ya, yb, yc, yd;
/*
 * start up calculations that must be performed before calling
 * CalcBezPt on any given segment
 */
void InitCalcBez()
{
        xa = -StartPtX(ActSeg) + 3.0*( Cntrl1X(ActSeg) - Cntrl2X(ActSeg))
            + EndPtX(ActSeg);
        xb = 3.0 *( StartPtX(ActSeg) + Cntrl2X(ActSeg) - 2.0*Cntrl1X(ActSeg));
        xc = 3.0*( Cntrl1X(ActSeg) - StartPtX(ActSeg));
        xd = StartPtX(ActSeg);

        ya = -StartPtY(ActSeg) + 3.0*( Cntrl1Y(ActSeg) - Cntrl2Y(ActSeg))
            + EndPtY(ActSeg);
        yb = 3.0 *( StartPtY(ActSeg) + Cntrl2Y(ActSeg) - 2.0*Cntrl1Y(ActSeg));
        yc = 3.0*( Cntrl1Y(ActSeg) - StartPtY(ActSeg));
        yd = StartPtY(ActSeg);
}

/*
 * calculate a point on the bezier curve of a segment
 */
void CalcBezPt( t, xvp, yvp)
    float t;
    float *xvp, *yvp;
{
    *xvp = (( t*xa + xb) * t + xc) *t + xd;

    *yvp = (( t*ya + yb) * t + yc) *t + yd;
}



void XdrawBezSeg()
{
    float t;
    float ftox, ftoy;
    int fromx, fromy, tox, toy;

    InitCalcBez();
    for( fromx = StartPtX(ActSeg), fromy = StartPtY(ActSeg), t=BezStepSize;
        t < 1.0; fromx = tox, fromy = toy, t+= BezStepSize ) {

        CalcBezPt( t, &ftox, &ftoy );
        tox = (int)ftox;
        toy = (int)ftoy;
        DrawLine( fromx, fromy, tox, toy, XOR );
        DrawPnt( tox, toy, XOR );
    }
    DrawLine( fromx, fromy, EndPtX(ActSeg), EndPtY(ActSeg),XOR);
}



void XdrawAllBezSegs()
{
    ResetActSeg();
    do {
        XdrawBezSeg();
        NextSeg();
    } while( ActSeg);

    DrawStartPt();      /* Leonards changes */
    DrawEndPt();        /* Leonards changes */
    DrawControl0();
    DrawControl1();
}



void ResetCurve()
{
    if( NumBezPts > 0 && CurMode == FITBEZIER ) {
        int i;

        ClrWindow(true);
        for( i = 0; i < NumBezPts; i++ ) {
            Bezpt[i].x.cur1 = Bezpt[i].x.prev2 = Bezpt[i].x.cur0;
            Bezpt[i].y.cur1 = Bezpt[i].y.prev2 = Bezpt[i].y.cur0;
        }
        ActSeg = 0;

        XdrawAllBezSegs();
    }
}


/*
 * set the value of a bezpt element
 */
static void SetBezPt( xval, yval )
    int xval, yval;
{
    BezVal *i;

    i = &Bezpt[NumBezPts-1].x;
    i->cur0 = i->prev2 = i->cur1 = xval;
    i = &Bezpt[NumBezPts-1].y;
    i->cur0 = i->prev2 = i->cur1 = yval;
}




void InitBezPt(xval,yval)
int xval, yval;
{
    int segno;

    NumBezPts++;
    SetBezPt(xval,yval);
    segno = NumBezPts -2;
    if( segno >= 0) {

        DrawLine( StartPtX( segno), StartPtY(segno),
                EndPtX( segno), EndPtY(segno), XOR );
    }
}





void EditBezPt(xval, yval)
{
    int segno = NumBezPts -2;

    DrawLine( StartPtX(segno), StartPtY(segno),
          EndPtX( segno ), EndPtY(segno), XOR );

    SetBezPt(xval, yval);

    DrawLine( StartPtX( segno), StartPtY(segno),
          EndPtX( segno), EndPtY(segno), XOR );
}


#include "scrnio.ih"
#ifdef MANX
#include <functions.h>
#endif
#include "scrnio.h"
#include "mytypes.h"

#include "bezpt.h"
#include "control.h"


static bool buttondown;  /* is left button down */
static short mousex, mousey;
static enum BezPtEnum selbezpt;  /* selected bez pt */

void DrawBox( int, int, int);
void DrawSqr( int, int, int);
void EditStartPt(int, int);
void EditEndPt(int, int);

void HandleTicks(mesg)
struct IntuiMessage *mesg;
{
    int x, y;

    x = CntrX(mesg->MouseX);
    y = CntrY(mesg->MouseY);

    if(!buttondown || (mousex == x && mousey == y )){
        return;
    }

    mousex = x;
    mousey = y;

    switch( CurMode ) {

    case DRAWPOLY:
         EditBezPt( mousex, mousey);
         break;

    case FITBEZIER:
        switch(selbezpt) {
        case BPEStartPt:
            EditStartPt(mousex, mousey);
            break;

        case BPEEndPt:
            EditEndPt(mousex, mousey);
            break;

        case BPECntrlPt1:
            EditControl0(mousex, mousey);
            break;

        case BPECntrlPt2:
            EditControl1(mousex, mousey);
            break;
        }
        break;

    default:
        break;
    }
}



void HandleMButtons(mesg)
struct IntuiMessage *mesg;
{

    long startdist, enddist, leftdist, rightdist;
    long tx, ty;

    mousex = CntrX(mesg->MouseX);
    mousey = CntrY(mesg->MouseY);

    switch( mesg->Code) {
    case SELECTDOWN:
        buttondown = true;  /* down */

        switch( CurMode ) {
        case DRAWPOLY:
            InitBezPt( mousex, mousey);
            if( GetNumSegs() == 0 ) {
                InitBezPt( mousex, mousey );
            }
            break;

        case FITBEZIER:
            tx = mousex - StartPtX(GetCurSeg());
            ty = mousey - StartPtY(GetCurSeg());
            startdist = tx * tx + ty * ty;

            tx = mousex - EndPtX(GetCurSeg());
            ty = mousey - EndPtY(GetCurSeg());
            enddist = tx * tx + ty * ty;

            tx = mousex - Cntrl1X(GetCurSeg());
            ty = mousey - Cntrl1Y(GetCurSeg());
            leftdist = tx * tx + ty * ty;

            tx = mousex - Cntrl2X(GetCurSeg());
            ty = mousey - Cntrl2Y(GetCurSeg());
            rightdist = tx *tx + ty * ty;

            if( leftdist <= rightdist &&
                 leftdist <= startdist &&
                 leftdist <= enddist ) {
                selbezpt = BPECntrlPt1;
                EditControl0( mousex, mousey );
                }
            else if ( rightdist <= startdist &&
                    rightdist <= enddist ) {
                selbezpt = BPECntrlPt2;
                EditControl1( mousex, mousey );
                }
            else if ( startdist <= enddist ) {
                selbezpt = BPEStartPt;
                EditStartPt(mousex, mousey);
            }
            else {
                selbezpt = BPEEndPt;
                EditEndPt(mousex, mousey);
            }
            break;

        default:
            break;
        }
        break;


    case SELECTUP:
        buttondown = false; /* up */
        break;

    case MENUUP:
        if( CurMode == FITBEZIER ) {
            DrawStartPt();
            DrawEndPt();
            DrawControl0();
            DrawControl1();
            NextSeg();
            DrawStartPt();
            DrawEndPt();
            DrawControl0();
            DrawControl1();
        }
        break;

    default:
        break;
    }
}

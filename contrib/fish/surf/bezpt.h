#include "fasttrig.h"
/*
 * structures and such for manipulating bezier curves
 */

#define MaxSegs  50  /* max number of bezier segments */

typedef struct {
    int prev2;  /* second control point of segment n-1 */
    int cur0;   /* begin of segment n, end of segment n-1 */
    int cur1;   /* first control point of segment n */
} BezVal;


typedef struct {
    BezVal  x, y;
} BezCoord;

#define DefBezMeshVal 10

extern BezCoord Bezpt[MaxSegs+1];
extern void SetBezMesh( /* Panel_item, int, struct input_event */);
extern int BezMesh;
extern float BezStepSize;

extern void InitCalcBez( /* void */ );
extern void CalcBezPt(/* float, float *, float * */);
extern void XdrawAllBezSegs(/* void */);
extern void XdrawBezSeg(/* void */);
extern void InitBezPt( /* int, int */);
extern void EditBezPt( /* int, int */);
extern void ResetCurve( /* void */ );

extern int NumBezPts;
extern int ActSeg;
#define BezRings() (GetNumSegs() * BezMesh + 1)
#define GetFirstSeg()   (0)
#define GetNumSegs()    (NumBezPts-1)
#define ResetActSeg()   { ActSeg = GetFirstSeg(); }
#define GetCurSeg()     (ActSeg)
#define PrevSegNo()   (ActSeg -1 < 0 ? GetNumSegs(): ActSeg -1 )
#define PrevSeg()   { ActSeg = PrevSegNo(); }
#define NextSegNo() ((ActSeg+1) % GetNumSegs())
#define NextSeg()       { ActSeg = NextSegNo(); }

enum BezPtEnum { BPEStartPt, BPEEndPt, BPECntrlPt1, BPECntrlPt2 };

#define StartPtX(XSegNo) Bezpt[XSegNo].x.cur0
#define StartPtY(XSegNo) Bezpt[XSegNo].y.cur0

#define Cntrl1X(XSegNo) Bezpt[XSegNo].x.cur1
#define Cntrl1Y(XSegNo) Bezpt[XSegNo].y.cur1

#define Cntrl2X(XSegNo) Bezpt[XSegNo+1].x.prev2
#define Cntrl2Y(XSegNo) Bezpt[XSegNo+1].y.prev2

#define EndPtX(XSegNo) Bezpt[XSegNo+1].x.cur0
#define EndPtY(XSegNo) Bezpt[XSegNo+1].y.cur0

#define DrawStartPt() DrawBox( Bezpt[ActSeg].x.cur0, \
                               Bezpt[ActSeg].y.cur0, XOR )

#define DrawEndPt() DrawBox( Bezpt[ActSeg+1].x.cur0, \
                               Bezpt[ActSeg+1].y.cur0, XOR )

#define DrawControl0() DrawSqr( Bezpt[ActSeg].x.cur1,\
                                Bezpt[ActSeg].y.cur1, XOR )

#define DrawControl1() DrawSqr( Bezpt[ActSeg+1].x.prev2,\
                                Bezpt[ActSeg+1].y.prev2, XOR )


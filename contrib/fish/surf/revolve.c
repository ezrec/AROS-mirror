#include <aros/oldprograms.h>
#include <stdio.h>
#include "fasttrig.h"
#include "bezpt.h"
#include "revolve.h"
#include "mytypes.h"


RevAxisType RevAxis;

void OutErr(char *);

extern struct Library * MathTransBase;

short RevMesh = DefRevMeshVal;
short RevImageR, /* revolution index */
      RevImageB; /* bezier index */

static int RotRange = DefRotRange;
static int RotStart = DefRotStart;
static int SecAngle = DefTilt;
static float SurfDist = DefSurfDist;
static float ViewDist = DefViewDist;
static bool Perspective = DefPersp;

void SetPerspective(  value)
    int value;
{
    Perspective = value;
}



void SetRevAxis( value)

{
    RevAxis = (value)? RevY : RevX;
}


void SetRotStart( value )
    int value;
{
    RotStart = value;
}

void SetRotRange(  value )
    int value;
{
    RotRange = value;
}

void SetSecAng( value )
    int value;
{
    SecAngle = value;
}

void SetRevMesh( value )
    int value;
{
    RevMesh = value;
}



void SetSurfDist( value )
    int value;
{
    SurfDist = (float )value;
}


void SetViewDist( value )
    int value;
{
    ViewDist = (float )value;
}



static
float secsin, seccos; /* trig values of secondary angle */

static
int sizeptlist = 0;

static
PtGen *ptlist1 = 0,
      *ptlist2 = 0;

static
int NumEnts; /* number of angle slices */


static
bool PrepRev()
{
    NumEnts = RevMesh+1;

    /*
     * allocate space 3d descriptions of a point revolved x degrees
     */
    if( NumEnts > sizeptlist ) {
        if( ptlist1 ) free(ptlist1);
        if( ptlist2 ) free(ptlist2);

        ptlist1 =(PtGen *) malloc( NumEnts * sizeof(PtGen)  );
        ptlist2 =(PtGen *) malloc( NumEnts * sizeof(PtGen)  );
        if( !ptlist1 || !ptlist2 ) {
            OutErr("PrepRev:not enough memory");
            return(true);
        }
    }


    if( InitFastTrig( RotStart, RotRange, NumEnts)) return(true);
    secsin = sin((float)SecAngle*PI/180);
    seccos = cos((float)SecAngle*PI/180);
    return (false);
}


static
void CalcRing(ptlist, xpos, ypos)
register PtGen *ptlist;
float xpos, ypos;
{
    int angle;

    for( angle = 0; angle < NumEnts; angle++, ptlist++) {
        float temp;
        /*
         * calculate 3d coordinate of point revolved
         */
        if( RevAxis == RevX) {
            ptlist->d3.y = ypos * fcos(angle);
            temp = ypos * fsin(angle);
            ptlist->d3.x = xpos* seccos - temp *secsin;
            ptlist->d3.z = xpos * secsin + temp * seccos;
        }
        else {
            ptlist->d3.x = xpos * fcos(angle);
            temp = xpos * fsin( angle);
            ptlist->d3.y = ypos * seccos + temp * secsin;
            ptlist->d3.z = secsin * ypos - temp * seccos;
        }
        ptlist->d3.z -= SurfDist;

/*        if( Perspective ) {
            float PerspScale;

            PerspScale = fabs(ViewDist / ptlist->d3.z);
            ptlist->d3.x *= PerspScale;
            ptlist->d3.y *= PerspScale;
        }
 */
        /*
         * calculate the 2d screen coordinate equvalent
         */
      /*
        ptlist->d2.x = (short) ptlist->d3.x;
        ptlist->d2.y = (short) ptlist->d3.y;
       */
        ptlist->d2.x = (short) (ptlist->d3.x + 0.5);
        ptlist->d2.y = (short) (ptlist->d3.y + 0.5);
    }
}






/*
 * return true on failure
 */
bool Revolve(acceptfunc)
    void (*acceptfunc)();
{
    float tparm, deltat;
    int subseg;

    if( PrepRev() ) {
        return(true);
    }

    deltat = 1.0/BezMesh;
    RevImageB = 0;
    ResetActSeg();
    do {
        float xpos, ypos;


        InitCalcBez();
        xpos = StartPtX(GetCurSeg());
        ypos = StartPtY(GetCurSeg());
        CalcRing(ptlist1, xpos, ypos );
        for( subseg = 1; subseg <= BezMesh; subseg++ ) {
            register PtGen *ptlista, *ptlistb;
            register int numpoly;

            tparm = subseg * deltat;
            if( subseg & 1 ) {
                ptlista = ptlist2; ptlistb = ptlist1;
            }
            else {
                ptlista = ptlist1; ptlistb = ptlist2;
            }

            CalcBezPt(tparm, &xpos, &ypos );
            CalcRing( ptlista, xpos, ypos );
            RevImageR = 0;
            for( numpoly = NumEnts -1; numpoly--; ) {
                (* acceptfunc)(ptlistb, ptlista, ptlista+1, ptlistb+1);
                ptlista++;
                ptlistb++;
                RevImageR++;
            }
            RevImageB++;
        }
        NextSeg();

    } while( ActSeg );
    return( false );
}

/*
 * write a ring of points
 */
static void WriteRing(fileout, ptlist, numpnts)
FILE *fileout;
PtGen *ptlist;
int numpnts;
{
    while( numpnts-- ) {
        fprintf(fileout, "%f %f %f \n", ptlist->d3.x,
            ptlist->d3.y, ptlist->d3.z );
            ptlist++;
    }
}

/*
 * write the list of vertices to file pointer
 * this function performs a similar function to revolve
 */
static void WriteRevolve(fileout)
    FILE *fileout;
{
    float tparm, deltat;
    float OldSurfDist;
    int subseg;
    float xpos, ypos;


    OldSurfDist = SurfDist;
    SurfDist = 0;
    deltat = 1.0/BezMesh;
    RevImageB = 0;
    ResetActSeg();
    /*
     * write out the starting ring
     */
    InitCalcBez();
    xpos = StartPtX(GetCurSeg());
    ypos = StartPtY(GetCurSeg());
    CalcRing(ptlist1, xpos, ypos );
    WriteRing(fileout, ptlist1, NumEnts);
    /*
     * loop to write out all the other rings
     */
    do {

        InitCalcBez();
        for( subseg = 1; subseg <= BezMesh; subseg++ ) {
            tparm = subseg * deltat;
            CalcBezPt(tparm, &xpos, &ypos );
            CalcRing( ptlist1, xpos, ypos );
            WriteRing(fileout, ptlist1, NumEnts);
            RevImageB++;
        }
        NextSeg();

    } while( ActSeg );
    SurfDist = OldSurfDist;

    return;
}

/*
 * this function writes a list of vertices to "filename"
 */
void WriteData(filename)
    char *filename;
{
        FILE *fileout;

        if(( GetNumSegs() < 1) || PrepRev()){
                return;
        }

        fileout = fopen(filename,"w");
        if(!fileout) {
            OutErr("could not open data file");
            return;
        }


        fprintf(fileout, "%d   * number of Rings\n", BezRings());
        fprintf(fileout, "%d   * Rev mesh\n", RevMesh);
        fprintf(fileout, "%d   * Rotation range\n", RotRange);
        WriteRevolve(fileout);
        fprintf(fileout, "end\n");
        fclose(fileout);
}




#include <proto/mathtrans.h>
#include "mytypes.h"
#include "scrnio.h"
#include "bezpt.h"
#include "revolve.h"
#include "control.h"
#include "poly.h"
#include "readilbm.h"
#include "menuexp.h"

bool  SpecOn = false; /* specular lighting enable - default off */

float Ambience = DefAmbience;
float PtIntensity = (float)DefIntensity;
float Kd = DefKd,
      Ks = DefKs;

Pt3 LightSrc = {
        DefLightSrcX,
        DefLightSrcY,
        DefLightSrcZ
        };

extern struct Library * MathTransBase;

static
Rhomboid *polylist = null,
         *nextpoly;

MapRhomboid *mpolylist = null,
        *mnextpoly;


void OutErr(char *);
void ClrWindow(bool);
void DrawRhomShade(Rhomboid *);
void DrawRhomFrame(ScrnPair *);
void DrawRhomMap(MapRhomboid *);
bool InitMapping();

static bool shadeflag;

long CalcMaxPolyNum() {
    return( GetNumSegs() * BezMesh * RevMesh );
}

static
bool PrepPoly()
{
        if( polylist ) free( (char *)polylist );
        polylist = (Rhomboid *)malloc( CalcMaxPolyNum() * sizeof(Rhomboid));
        if( !polylist ) {
            OutErr("PrepPoly: not enough memory");
            return( true );
        }
        nextpoly = polylist;
        return(false); /* success = 0 */
}


static bool
PrepMPoly()
{
        if( mpolylist ) free( (char *)mpolylist );
        mpolylist = (MapRhomboid *)
                    malloc( CalcMaxPolyNum() * sizeof(MapRhomboid));
        if( !mpolylist ) {
            OutErr("PrepPoly: not enough memory");
            return( true );
        }
        mnextpoly = mpolylist;
        return(false); /* success = 0 */
}


/*
 * Multiply a vector by scalar quantity
 */
void ScaleVec( fact, src, dst )
        float fact;
        Pt3 *src, *dst;
{
        dst->x = src->x * fact;
        dst->y = src->y * fact;
        dst->z = src->z * fact;
}
/*
 * convert a vector to a unitized vector
 * if possible
 */
void Unitize( vec )
        Pt3 *vec;
{
        float len;

        len = vec->x*vec->x + vec->y*vec->y + vec->z*vec->z;
        len = sqrt( len );
        if( len != 0.0 ) {
                vec->x /= len;
                vec->y /= len;
                vec->z /= len;
        }
}

/*
 * calculate a vector from two points
 */
void CalcVector( src1, src2, dest )
Pt3 *src1, *src2, *dest ;
{
        dest->x = src1->x - src2->x;
        dest->y = src1->y - src2->y;
        dest->z = src1->z - src2->z;
}




/*
 * calculate a normal from a list of polygons. This routine does the
 * logical trick of trying to exclude each point in turn if the
 * normal can not be calculated, or something of the sort.
 * a value of true is returned if a normal with a nonzero z component
 * could not be calculated
 */

bool CalcNormal( vxlist, normal)
        PtGen *vxlist[];
        register Pt3 *normal;
{
        int i, k, m;
        Pt3 *j[3];
        Pt3 v1, v2;

        for( i = 0; i < RhomVxNum; i++ ) {
                for( k = 0, m = 3; m--; k++ ) {
                        if( k == i ) {
                                k++;
                        }
                        j[m] = &vxlist[k]->d3;
                }
                CalcVector( j[1], j[0], &v1 );
                CalcVector( j[2], j[1], &v2 );

                normal->z = v1.x*v2.y - v1.y*v2.x;
                if( normal->z == 0 ) {
                        continue;
                }
                normal->x = v1.y*v2.z - v1.z*v2.y;
                normal->y = v1.z*v2.x - v1.x*v2.z;
                if( normal->z < 0 ) {
                        normal->x = -normal->x;
                        normal->y = -normal->y;
                        normal->z = -normal->z;
                }
                Unitize(normal);
                return( false );
        }
        return(true);
}


/*
 * Euclidean dot product.
 * I wonder what the minkowski dot product would look like
 */
float DotProd( v1, v2 )
        Pt3 *v1, *v2;
{
        return( v1->x*v2->x + v1->y*v2->y + v1->z*v2->z );
}





/*
 * define a polygon as a set of four points
 * returns true if polygon created
 */
static bool CreatePoly(curpoly, p0, p1, p2, p3)
    register Rhomboid *curpoly;
    PtGen *p0, *p1, *p2, *p3;
{

    Pt3 normal;
    PtGen *list[RhomVxNum];

    list[0] = p0; list[1] = p1; list[2] = p2; list[3] = p3;
    /*
     * compute stuff needed only if producing shaded image
     */
    if( shadeflag ) {
            Pt3 lvec;
            Pt3 center;
            float ptintens;
            float ldotn; /* light vector dot normal */
            /*
            * if cant compute normal, then junk polygon
            */
            if( CalcNormal( list, &normal )) {
                return(false);
            }

            curpoly->intensity = Ambience;
            center.x = ( list[0]->d3.x
                    + list[1]->d3.x
                    + list[2]->d3.x
                    + list[3]->d3.x)/4.0;
            center.y = ( list[0]->d3.y
                    + list[1]->d3.y
                    + list[2]->d3.y
                    + list[3]->d3.y)/4.0;
            center.z = ( list[0]->d3.z
                    + list[1]->d3.z
                    + list[2]->d3.z
                    + list[3]->d3.z)/4.0;

            curpoly->depth = center.z;

            CalcVector( &center, &LightSrc, &lvec );
            Unitize( &lvec );
            ldotn = DotProd( &lvec, &normal );
            if( ldotn < 0 ) {
                ptintens = PtIntensity * Kd * -ldotn;
                curpoly->intensity += ptintens;
            }
            /*
             * calculate specular component
             */
            if( SpecOn && ldotn < 0 ) {
                float Kspec, Is;
                Pt3 rvec; /* lvec reflected through poly */

                ScaleVec( 2*ldotn, &normal, &rvec );
                CalcVector(&lvec, &rvec, &rvec );
                Unitize( &center );
                Kspec = DotProd( &rvec, &center);

                if( Kspec <= 0.0 ) {
                    Is = Ks * Kspec * Kspec* PtIntensity;
                    curpoly->intensity += Is;
                }
            }

            if( curpoly->intensity > 1.0 ) {
                curpoly->intensity = 1.0;
            }
    }
    else {
        /*
         * calculate depth of polygon
         * for now, try an average of the vertex depths
         */
        curpoly->depth =( list[0]->d3.z
                + list[1]->d3.z
                + list[2]->d3.z
                + list[3]->d3.z)/4.0;
    }
    /*
     * store index to screen coordinates
     */
    curpoly->pt[0] = p0->d2;
    curpoly->pt[1] = p1->d2;
    curpoly->pt[2] = p2->d2;
    curpoly->pt[3] = p3->d2;

    return(true);
}
/*
 * passable procedure for creating polygons without mapping
 */
static
void AcceptPoly(p0, p1, p2, p3)
    PtGen *p0, *p1, *p2, *p3;
{
    if( CreatePoly(nextpoly, p0, p1, p2,p3)) {
        nextpoly++;
    }
}

static
void AcceptMPoly( p0, p1, p2, p3)
    PtGen *p0, *p1, *p2, *p3;
{
    if( CreatePoly(&mnextpoly->rhom, p0, p1, p2,p3)) {
        mnextpoly->bezindex = RevImageB;
        mnextpoly->revindex = RevImageR;
        mnextpoly++;
    }
}





/*
 * compare the depth of two polygons for SortPoly
 */
static int CmpDepth( a, b )
        Rhomboid *a, *b;
{
        if( a->depth < b->depth ) return(-1);
        else if( a->depth > b->depth ) return(1);
        else return(0);
}

static int CmpMDepth( a, b )
        MapRhomboid *a, *b;
{
        if( a->rhom.depth < b->rhom.depth ) return(-1);
        else if( a->rhom.depth > b->rhom.depth ) return(1);
        else return(0);
}





void RevNoShade() {
        Rhomboid *i;
        if( GetNumSegs() < 1 ) {
                return;
        }

        ClrAbort();
        shadeflag = false;
        if( PrepPoly() ) return;
        if( Revolve(AcceptPoly) ) return;
        CurMode = NOTACTIVE;

        qsort( (char *)polylist, nextpoly - polylist,
                sizeof(Rhomboid), CmpDepth);

        ClrWindow(false);

        for( i = polylist; i< nextpoly; i++ ) {
            if( AbortDraw ) return;
                DrawRhomFrame( i->pt );
        }
}




void RevShade() {
        register Rhomboid *i;

        if( !AllocDither()) return; /* not enough memory */
        if( GetNumSegs() < 1 ) {
                return;
        }
        CurMode = NOTACTIVE;
        ClrAbort();
        shadeflag = true;
        if( PrepPoly() || Revolve(AcceptPoly) ) {
            return;
        }
        qsort( (char *)polylist, nextpoly-polylist,
                sizeof(Rhomboid), CmpDepth);

        ClrWindow(false);

        for( i = polylist; i< nextpoly; i++ ) {
                if( AbortDraw ) return;
                DrawRhomShade( i );
        }
}



void RevMap() {
    register MapRhomboid *i;

    ClrAbort();
    if( GetNumSegs() < 1 ) {
            return;
    }
    if( InitMapping() ) {
        return;
    }
    CurMode = NOTACTIVE;
    shadeflag = true;


    if( PrepMPoly() || Revolve(AcceptMPoly) ) {
        return;
    }
    qsort( (char *)mpolylist, mnextpoly-mpolylist,
        sizeof(MapRhomboid), CmpMDepth);
    ClrWindow(false);

    for( i = mpolylist; i< mnextpoly; i++ ) {
            if( AbortDraw ) return;
            DrawRhomMap(i);
    }
}

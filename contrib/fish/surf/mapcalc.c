#include <aros/oldprograms.h>
#include <stdio.h>
#include "mytypes.h"
#include "revolve.h"       /* need to get scrnpair from here */
#include "mapstuff.h"
#include "menuexp.h"

#ifdef TEST
#undef DebugOn
#define DebugOn 1
#endif /* TEST */

extern struct Library * MathBase;
extern struct Library * MathTransBase;

#define DEBUG
/*
 * this section of code derived from:
 * "The essential algorithms of ray tracing" by Eric Haines
 * presented in Sigraph proceedings on Ray Tracing
 * my major change has been to simplify it for two dimensions
 */

typedef struct {
    float x, y;
} Vector;

static float DotVector(a,b)
    Vector *a, *b;
{
    return( a->x * b->x + a->y * b->y);
}

static void DivVector(in, scale, out)
    Vector *in, *out;
    float scale;
{
    if ( fabs(scale) < SingleTinyVal ) {
        out->x = SingleLargeVal;
        out->y = SingleLargeVal;
    }
    else {
        out->x = in->x / scale;
        out->y = in->y / scale;
    }
}



static Vector Na, Nb, Nc;
static float Du0, Du1, Du2,
             Dv0, Dv1, Dv2;
static Vector Qux, Quy,
              Qvx, Qvy;
static float Dux, Duy,
             Dvx, Dvy;
static bool IsQuadu, IsQuadv;

void CalcMapConsts(vp)
    register ScrnPair *vp;
#define p00 vp[0]
#define p01 vp[1]
#define p11 vp[2]
#define p10 vp[3]
{
    Vector Pa, Pb, Pc, Pd;

    Pa.x = p00.x - p10.x + p11.x - p01.x;
    Pa.y = p00.y - p10.y + p11.y - p01.y;

    Pb.x = p10.x - p00.x;
    Pb.y = p10.y - p00.y;

    Pc.x = p01.x - p00.x;
    Pc.y = p01.y - p00.y;

    Pd.x = p00.x;
    Pd.y = p00.y;

    Na.x = Pa.y; Na.y = -Pa.x;
    Nc.x = Pc.y; Nc.y = -Pc.x;
    Nb.x = Pb.y; Nb.y = -Pb.x;

    Du0 = DotVector(&Nc, &Pd);
    Du1 = DotVector(&Na, &Pd) + DotVector(&Nc, &Pb);
    Du2 = DotVector( &Na, &Pb);

    if( fabs( Du2 ) > SingleTinyVal ) {
        float TwoDu2;

        IsQuadu = true;
        TwoDu2 = 2.0 * Du2;
        DivVector( &Na, TwoDu2, &Qux );
        DivVector( &Nc, -Du2, &Quy );
        Duy = Du0/Du2;
        Dux = -Du1/TwoDu2;
    }
    else {
        IsQuadu = false;
    }

    Dv0 = DotVector( &Nb, &Pd);
    Dv1 = DotVector(&Na, &Pd) + DotVector(&Nb, &Pc);
    Dv2 = DotVector( &Na, &Pc);
    if( fabs( Dv2 ) > SingleTinyVal ) {
        float TwoDv2;

        IsQuadv = true;
        TwoDv2 = 2.0 * Dv2;
        DivVector( &Na, TwoDv2, &Qvx);
        DivVector( &Nb, -Dv2, &Qvy);
/*      DivVector( &Nc, -Dv2, &Qvy); */
        Dvx = - Dv1/TwoDv2;
        Dvy = Dv0/Dv2;
    }
    else {
        IsQuadv = false;
    }
#ifdef DEBUG
    if( DebugOn ) {
        printf("du2 %f, du1 %f, du0 %f\n", Du2, Du1, Du0 );
        printf("dv2 %f, dv1 %f, dv0 %f\n", Dv2, Dv1, Dv0 );
        printf("Na = (%f, %f), Nb = (%f,%f), Nc = (%f,%f)\n",
        Na.x, Na.y, Nb.x, Nb.y, Nc.x, Nc.y );
        printf("IsQuad =(%c, %c)\n", IsQuadu?'t':'f', IsQuadv? 't': 'f' );
    }
#endif /* DEBUG */

}


/*
 * given points px,py in the quadrilateral, map them to points inside a
 * unit square
 */
void  MapXYRatio(px, py, outx, outy, SweepCode)
    float px, py;
    float *outx, *outy;
    short SweepCode;
{
    float resu, resv;
    Vector Ri;


    Ri.x = px; Ri.y = py;

    if( !IsQuadu ) {
        float denom;
        denom = (Du1 - DotVector(&Na, &Ri));
        if( fabs(denom) < SingleTinyVal )
            resu = 2.0;
        else
            resu = (DotVector(&Nc, &Ri) - Du0)/denom;
    } else {
        float Ka, Kb;
        float discrim;

        Ka = Dux + DotVector( &Qux, &Ri);
        Kb = Duy + DotVector( &Quy, &Ri);
        discrim = sqrt(fabs(Ka * Ka - Kb));
        resu = Ka + ((discrim > Ka)? discrim: (-discrim));
#ifdef DEBUG
        if( DebugOn ) {
            printf("dux=%f, duy = %f, ka = %f, kb = %f\n",
                Dux, Duy, Ka, Kb );
        }
#endif /* DEBUG */

    }

    if( !IsQuadv ) {
        float denom;
        denom = (Dv1 - DotVector(&Na, &Ri));
        if( fabs(denom) < SingleTinyVal )
            resv = 2.0;
        else
            resv = (DotVector(&Nb, &Ri) - Dv0)/denom;
    } else {
        float Ka, Kb;
        float discrim;

        Ka = Dvx + DotVector( &Qvx, &Ri);
        Kb = Dvy + DotVector( &Qvy, &Ri);
        discrim = sqrt(fabs( Ka * Ka - Kb));
        resv = Ka + ((discrim > Ka)? discrim: (-discrim));
#ifdef DEBUG
        if( DebugOn ) {
            printf("dvx=%f, dvy = %f, ka = %f, kb = %f\n",
                Dvx, Dvy, Ka, Kb );
        }
#endif /* DEBUG */
    }

#ifdef DEBUG
    if( DebugOn ) {
        printf("(%f,%f) -> (%f, %f)\n", px, py, resu, resv );
    }
#endif /* DEBUG */

    if( resu > 1.0 || resu < 0.0 ) {
        resu = ( SweepCode & MP_XMAX)? 1.0: 0.0;
    }
    if( resv > 1.0 || resv < 0.0 ) {
        resv = ( SweepCode & MP_YMAX)? 1.0: 0.0;
    }

    *outx = resu; *outy = resv;
}



/*
 * here abides testcode
 */
#ifdef TEST
#include <stdio.h>

ReadScrnPair(set, a)
    char *set;
    ScrnPair *a;
{
    int tx, ty;
    printf("enter screen pair %s\n",set);
    scanf("%d %d", &tx, &ty);
    a->x = tx; a->y = ty;
}

ReadLimits(a)
ScrnPair a[];
{
    ReadScrnPair("a", &a[0]);
    ReadScrnPair("b", &a[1]);
    ReadScrnPair("c", &a[2]);
    ReadScrnPair("d", &a[3]);
}

main() {
    float inx, iny;
    float outy, outx;
    ScrnPair pts[4];

    ReadLimits(pts);
    CalcMapConsts(pts);
    while(!feof(stdin)) {
        printf("enter quadrilateral points\n");
        scanf("%f %f", &inx, &iny );
        MapXYRatio( inx, iny, &outx, &outy, 0);
        printf("p(%f,%f)->p(%f,%f)\n", inx, iny, outx, outy);
    }
}
#endif /* TEST */

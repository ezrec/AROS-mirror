#include <proto/mathffp.h>
#include <proto/mathtrans.h>
#include <aros/oldprograms.h>
#include "mytypes.h"
#include "poly.h"
#include "bezpt.h"
#include "revolve.h"
#include "readilbm.h"
#include "mapstuff.h"
#include "menuexp.h"

#define FarRight   1e6
#define FarLeft   -1e6
#define FarTop     0x7fff
#define FarBottom -0x7fff


#ifndef MANX
#include <libraries/mathffp.h>
#define ceil    SPCeil
#define floor   SPFloor
#define fabs    SPAbs
#endif

extern struct Library * MathBase;
extern struct Library * MathTransBase;

void PaintPoint(short, short, float);

typedef struct { float left, right; } Hedge;

static float *BezMapping = null,
             *RevMapping = null;
static float revmin, revdiff,
             bezmin, bezdiff;


/*
 * given the ptlist of a polygon, find its vertical range
 */
static void FindVRange(scrnlist, top, bottom)
    register ScrnPair *scrnlist;
    short *top, *bottom;
{
    short i;
    short localtop, localbot;

    localtop = FarBottom;
    localbot = FarTop;

    for( i = 4; i--; scrnlist++ ) {
        if( localtop < scrnlist->y ) localtop = scrnlist->y;
        if( localbot > scrnlist->y ) localbot = scrnlist->y;
    }
    *top = localtop;
    *bottom = localbot;
}
/*
 * allocate table to store a quick and dirty representation of the
 * quadrilateral segments
 */
static Hedge *InitVRange( depth, tabptr, olddepth )
    short depth, *olddepth;
    Hedge *tabptr;
{
    Hedge *edgel, *tab;
    if( *olddepth < depth || !tabptr ) {
        if( tabptr ) free( tabptr);
        tab = (Hedge *) malloc(sizeof(Hedge)*depth);
        *olddepth = depth;
    }
    else {
        tab = tabptr;
    }
    if( !tab ) return( null);

    for( edgel = tab; depth--; edgel++) {
        edgel->left = FarRight;
        edgel->right = FarLeft;
    }
    return( tab );
}


/*
 * add line to quadrilateral descriptions
 */
static void AddVLine( tab, x1, y1, x2, y2 )
    Hedge *tab;
    short x1, y1, x2, y2;
{
    short dy;
    float curx, slope;
    /*
     * want y1 to have smaller value, ie, y1 below y2
     */
    if( y1 > y2 ) {
        short temp;
        temp = y1; y1 = y2; y2 = temp;
        temp = x1; x1 = x2; x2 = temp;
    }
    dy = y2 - y1;
    tab += y1;

    if( !dy ) {
        if ( x1 < x2 ) {
            short tempx;
            tempx = x1; x1 = x2; x2 = tempx;
        }
        if( x2 < tab->left ) tab->left = x2;
        if( x1 > tab->right ) tab->right = x1;
        return;
    }
    slope = (float)(x2 - x1)/dy;

    curx = x1;
#define ZipIt(xxx) { if( xxx < tab->left) tab->left = xxx; \
                     if( xxx > tab->right ) tab->right = xxx; }
    ZipIt(curx);
    while( dy--) {
        curx += slope;
        tab++;
        ZipIt(curx);
    }
}


static void AdjMapXY( inx, iny, outpair)
    float inx, iny;
    ScrnPair *outpair;
{
    float outx, outy;
    MapXYRatio( inx, iny, &outx, &outy);

    outpair->y = MapImageH * (bezmin + bezdiff * outy);
    outpair->x = MapImageV * (revmin + revdiff * outx);

/*
    if( RevAxis == RevX ) {
        outpair->y = MapImageH * (bezmin + bezdiff * outy);
        outpair->x = MapImageV * (revmin + revdiff * outx);
    } else {
        outpair->x = MapImageH * (bezmin + bezdiff * outy);
        outpair->y = MapImageV * (revmin + revdiff * outx);
    }
 */
}

static void ScanCnvQuad( tab, pt)
    Hedge *tab;
    ScrnPair pt[];
{
    register int i;
    ScrnPair *listb, *liste;

    liste = pt;
    listb = liste + 3;
    for ( i = 4; i--;) {
        AddVLine( tab, listb->x, listb->y, liste->x, liste->y);
        listb = liste++;
    }
}

static float AverageShade(pts)
    ScrnPair pts[];
{
    register Hedge *tab;
    static Hedge *tabfree = null;
    static short olddepth = 0;
    short top, bot;
    long shade = 0,
         pixcnt = 0;

    FindVRange( pts, &top, &bot);
    tabfree = tab = InitVRange( top - bot + 1, tabfree, &olddepth);
    if(!tabfree) return(0.0);

    ScanCnvQuad( tab-bot, pts );
#if DEBUG
    if( DebugOn ) {
        printf("AverageShade top is %d, bot = %d\n", top, bot );
    }
#endif /* DEBUG */

    while( bot <= top ) {
        register int hori;
        int right, left;
#if DEBUG
    if( DebugOn ) {
        printf("....row %d    \t%d -> %d\n", bot, left, right );
    }
#endif /* DEBUG */

        left =  (int) ceil(tab->left - SingleTinyVal);
        right = (int)floor(tab->right+ SingleTinyVal);

        for( hori= left; hori <= right; hori++ ) {
            shade += GetImgPix( bot, hori);
            pixcnt++;
        }

    /*
        if( RevAxis == RevX ) {
            for( hori= left; hori <= right; hori++ ) {
                shade += GetImgPix( bot, hori);
                pixcnt++;
            }
        }
        else {
            for( hori= left; hori <= right; hori++ ) {
                shade += GetImgPix( hori, bot);
                pixcnt++;
            }
        }
    */
        tab++;
        bot++;
    }
    return( (float)shade / (pixcnt *(15 *16)) );
}

/*
 * mess with the number so truncation doesn't
 * do nasty things to a float containing an int
 */
static int NearestInt( afloat )
    float afloat;
{
    afloat += ( afloat > 0 )? 1e-2 : -1e-2;
    return( (int)afloat );
}


static void ShadeQuad(tab, top, bot, intensity)
    register Hedge *tab;
    short top, bot;
    float intensity;
{
    short vert;
    float rowminl, rowminr,
          rowmaxl, rowmaxr;
    Hedge *oldtab = NULL, *nexttab;

    for ( vert =  bot;
        nexttab = tab+1, vert <= top;
        vert++, oldtab = tab, tab++ ) {
        float hori;
        float colmin, colmax;
        float leftmost, rightmost;
        int ihori, ileftmost, irightmost;
        ScrnPair MpPnts[4];

#define lefttop MpPnts[0]
#define leftbot MpPnts[3]
#define righttop MpPnts[1]
#define rightbot MpPnts[2]


        rowminl = (float)vert;
        rowmaxr = rowmaxl = rowminr = rowminl;

        if( vert > bot && oldtab->left < tab->left ) {
            rowminl -= 0.5;
        }
        if( vert > bot && oldtab->right > tab->right ) {
            rowminr -= 0.5;
        }
        if( vert < top && nexttab->left < tab->left ) {
            rowmaxl += 0.5;
        }
        if( vert < top && nexttab->right > tab->right ) {
            rowmaxr += 0.5;
        }

        irightmost = NearestInt( tab->right );
        rightmost = irightmost;
        ileftmost = NearestInt( tab->left );
        leftmost = ileftmost;
        if( irightmost < ileftmost ) {
            irightmost = ileftmost;
        }
        for( ihori = leftmost, hori = leftmost;
            ihori <= irightmost;
            ihori += 1, hori += 1.0 ) {


            if( AbortDraw ) { return; }

            colmin = hori - 0.5;
            colmax = hori + 0.5;

            colmin =(colmin > leftmost)?colmin: tab->left;
            colmax =(colmax < rightmost)?colmax: tab->right;

            AdjMapXY( colmin, rowmaxl, &lefttop,  MP_XMIN| MP_YMAX);
            AdjMapXY( colmax, rowmaxr, &righttop, MP_XMAX| MP_YMAX);
            AdjMapXY( colmin, rowminl, &leftbot,  MP_XMIN| MP_YMIN);
            AdjMapXY( colmax, rowminr, &rightbot, MP_XMAX| MP_YMIN);

            PaintPoint(ihori, vert, AverageShade(MpPnts) *intensity);
        }
    }
#undef lefttop
#undef righttop
#undef rightbot
#undef leftbot
}


void DrawRhomMap(mpr)
    MapRhomboid *mpr;
{
    short top, bottom;
    static Hedge *tab = null;
    static short olddepth = 0;

    CalcMapConsts( mpr->rhom.pt );
    FindVRange( mpr->rhom.pt, &top, &bottom );
    tab = InitVRange( top - bottom + 1, tab, &olddepth );
    if(!tab) return;
    ScanCnvQuad( tab -bottom, mpr->rhom.pt );

    bezmin = BezMapping[mpr->bezindex];/* make it global */
    bezdiff = BezMapping[mpr->bezindex+1] - bezmin;
    revmin = RevMapping[mpr->revindex];
    revdiff = RevMapping[mpr->revindex+1] - revmin;
#if DEBUG
    if( DebugOn ) {
        DBMAP(mpr->rhom.pt, mpr->bezindex, mpr->revindex);
    }
#endif /* DEBUG */
    ShadeQuad(tab, top, bottom, mpr->rhom.intensity);
}

#ifdef DEBUG
DBMAP(ptlist, bindex, rindex)
ScrnPair ptlist[];
short bindex, rindex;
{
    int i;

    printf("...................................\n");
    for( i = 0; i < 4; i++ ) {
        printf("%10d", ptlist[i].x);
    };
    printf("\n");
    for( i = 0; i < 4; i++ ) {
        printf("%10d", ptlist[i].y);
    };
    printf("\n");
    printf(" bezmin %f  bezdiff %f index = %d \n", bezmin, bezdiff, bindex );
    printf(" revmin %f  revdiff %f index = %d \n", revmin, revdiff, rindex );
}
#endif /* DEBUG */


/*
 * return true if image mappings could not be performed
 * false if successful
 */
bool InitMapping() {
    float *vfmptr;
    float totallen = 0,
          scaling;
    short numvslices;

    if( BezMapping ) free( BezMapping );
    if( RevMapping ) free( RevMapping );

    /*
     * compute width of each bezier segment
     */
    numvslices = BezMesh*GetNumSegs() +1;
    vfmptr = BezMapping = (float *) malloc(sizeof(float) * numvslices);
    if( !BezMapping ) return(true);

    *vfmptr++ = totallen = 0.0;
    ResetActSeg();
    do {
        float t, ffromx, ftox, ffromy, ftoy;
        int i;
        InitCalcBez();
        for( i = 1, ffromx = StartPtX(ActSeg), ffromy = StartPtY(ActSeg);
            i <= BezMesh; i++, ffromx = ftox, ffromy = ftoy ) {
            float diffx, diffy;

            t = (float)i/BezMesh;

            CalcBezPt( t, &ftox, &ftoy );
            diffx = ftox - ffromx;
            diffy = ftoy - ffromy;
            totallen += sqrt( diffx * diffx + diffy * diffy );
            *vfmptr++ = totallen;
        }
        NextSeg();
    } while( ActSeg);
    /*
     * convert scale floating point values to integer pixel positions
     */
    scaling = 1.0 / totallen;
    for( vfmptr = BezMapping; numvslices; numvslices--, vfmptr++ ) {
        *vfmptr *= scaling;
    }
    /*
     * compute height of each revolution segment
     */
    RevMapping = (float *) malloc( sizeof(float) * (RevMesh + 1));
    if( !RevMapping ) return( true );
    {
        short i;
        for( i = 0; i <= RevMesh; i++ ) {
            RevMapping[i] =  ((float) i)/RevMesh;
        }
    }

    return(false);
}

#include <aros/oldprograms.h>
#include "mytypes.h"
#include "scrnio.ih"
/*
 * if manx supports rand, these ifdefs could be removed (somewhat)
 */
#ifdef MANX
extern double ran(); /* not in manx  math.h */
#include <functions.h>
#else
extern int rand();
#endif
#include <string.h>

#include "scrndef.h"
#include "scrnio.h"
#include "gadgetdef.h"
#include "menudef.h"

#include "bezpt.h"
#include "revolve.h"
#include "control.h"
#include "poly.h"


long BackColor = DefBkPlane;
short DitherPower = 2;/* default gray levels = 4 */
static UWORD *GrayPat = null;
short DitherLevels, DitherMask;
#define RowPower 4  /* 2**4 = 16 rows */
#define DitherLoc(LEVEL) ((LEVEL)<<RowPower)


extern struct GfxBase * GfxBase;
void OutErr(char *);
void HandleMButtons(struct IntuiMessage *);
void HandleTicks(struct IntuiMessage *);


/*
 * free memory associated with
 * old dither patterns;
 */
void FreeOldDither() {
    if( GrayPat ) free(GrayPat);
    GrayPat = null;
}
/*
 * create grey level patterns
 */
bool AllocDither()
{
    int i;
    float reallevels;
    if( GrayPat) return(true);

    DitherLevels = 1 << DitherPower;
    reallevels = (float) DitherLevels - 0.5;
    DitherMask = DitherLevels -1;
    GrayPat = (UWORD *)malloc(DitherLoc(DitherLevels)*sizeof(UWORD));
    if( !GrayPat ) {
        OutErr("not enough memory for options chosen");
        return(false);
    }
    ColorMax = (NumColors -1) * DitherLevels +1;
    /*
     * compute gray values for each grey level
     */
    for( i = 0; i < DitherLevels; i++ ) {
        /*
         * compute gray values for each row
         */
        int j;
#ifdef MANX
        float fracti;
        fracti = (float)i/ reallevels;
        for( j = 0; j < DitherLoc(1); j++ ) {
            register long k, x;
            for( k = 16, x = 0; k--; ) {
                x <<= 1;
                x |= (ran() < fracti) ? 1: 0;
            }
            GrayPat[DitherLoc(i)+j] = x;
        }
#else /* MANX */
        for( j = 0; j < DitherLoc(1); j++ ) {
            register long k, x;
            for( k = 16, x = 0; k--; ) {
                x <<= 1;
                x |= ((rand()%DitherLevels)< i) ? 1: 0;
            }
            GrayPat[DitherLoc(i)+j] = x;
        }
#endif /* MANX */
    }
    return(true);
}


void SetMono( maxrval, maxgval, maxbval )
    short maxrval,
          maxgval,
          maxbval;
{
    long i;
    short range;
    long rval, gval, bval;

    range = (NumColors -1) & 0x1f;  /* max 32 colours */
    for( i = 0; i <= range; i++ ) {
        rval = (maxrval * i )/range;
        gval = (maxgval * i )/range;
        bval = (maxbval * i )/range;

        SetRGB4( &(SurfScrn->ViewPort),  i, rval, gval, bval );
    }
}


void SetRainbow()
{
    long i;
    short range;
    long rval, gval, bval;

    range = NumColors>> 1;
    /*
     * can't do a rainbow with only 2 colors
     */
    if( range < 2) {
        return;
    }

    for( i = 0; i < range; i++ ) {
        long diff;

        diff = (0xf * i )/(range-1);
        rval = 0xf - diff;
        bval = diff;
        gval = 0xf;
        SetRGB4( &(SurfScrn->ViewPort), i, rval, gval, bval);
    }
    for( i = 0; i < range; i++ ) {
        long diff;

        diff = (0xf * i )/(range-1);
        rval = diff;
        bval = 0xf;
        gval = 0xf - diff;
        SetRGB4( &(SurfScrn->ViewPort), i+range, rval, gval, bval);
    }
    SetRGB4( &(SurfScrn->ViewPort), 0L, 0L, 0L, 0L);
}

/*
 * set colours for hourglass pointer
 */
void SetHourGlassCol()
{
    SetRGB4( &(SurfScrn->ViewPort),17L, 6L, 2L, 3L );
    SetRGB4( &(SurfScrn->ViewPort),18L, 0L, 0L, 0L );
    SetRGB4( &(SurfScrn->ViewPort),19L, 9L, 7L, 6L );
}



void ClrWindow(drawaxis)
bool drawaxis;
{
    long BkColAdj; /* background color adjusted for number of bit planes */

    BkColAdj = (BackColor * NumColors) / 32;
    SetRast(rp, BackColor);  /* clear the window to colour 0 */
    SetAPen(rp, WinFgCol );
    /*
     * Draw axis on screen
     */
    if( drawaxis) {
        Move( rp, 0, WinVOrig);    /* x axis */
        Draw( rp, (long)SurfWinDef.Width, (long)WinVOrig );

        Move( rp, WinHOrig, 0);     /* y axis */
        Draw( rp, (long)WinHOrig, (long)SurfWinDef.Height );
    }
}





void DrawLine( x1, y1, x2, y2, mode )
int x1, y1, x2, y2;
int mode;
{
    SetDrMd( rp, mode );
    Move( rp, (long)UCntrX(x1), (long)UCntrY(y1));
    Draw(rp, (long)UCntrX(x2), (long)UCntrY(y2) );
}



void PaintPoint(x,y,forecolor)
    short x, y;
    float forecolor;
{
    long shade;
    shade = forecolor * (float) (NumColors-1);

    if( shade >= NumColors) {
        shade = NumColors-1;
    }
    else if ( shade < 0 ) {
        shade = 0;
    }

    SetAPen( rp, shade );
    WritePixel( rp, (long)UCntrX(x), (long)UCntrY(y));
}



void DrawPnt( x, y, op )
int x, y, op;
{
    x = UCntrX(x);
    y = UCntrY(y);
    SetDrMd(rp, op );
    RectFill( rp, (long)x, (long)y, (long)x, (long)y);
}


void DrawSqr(x, y, op )
int x, y, op;
{
    x = UCntrX(x);
    y = UCntrY(y);
    SetDrMd(rp, op );
    RectFill( rp, x - 2L, y -2L, x+ 2L, y+2L );
}


void DrawBox( x, y, op)
int x, y, op;
{
    x = UCntrX(x);
    y = UCntrY(y);
    SetDrMd(rp, op );
    RectFill( rp, x - 4L, y -4L, x+ 4L, y+4L );
    RectFill( rp, x - 3L, y -3L, x+ 3L, y+3L );
}


void DrawRhomShade( poly)
Rhomboid *poly;
{
    int i;
    int shade;
    long backcolor, forecolor = 0;
    static UWORD FullBits = 0xffff;

    shade = (int)((poly->intensity) * ColorMax);

    if( shade >= ColorMax -1 ) {
        shade = 0;
        backcolor = NumColors -1;
    }
    else {
        backcolor = shade >> DitherPower;
        forecolor = backcolor +1;
        if( forecolor >= NumColors ) {
            forecolor = backcolor;
        }
    }
    SetDrMd( rp, JAM2);
    SetAPen( rp, forecolor );
    SetBPen( rp, backcolor );
    SetAfPt( rp, &GrayPat[DitherLoc((shade & DitherMask))], RowPower);

    AreaMove( rp, UCntrX(poly->pt[0].x), UCntrY(poly->pt[0].y));

    for( i = 1; i < 4; i++ ) {
        AreaDraw( rp, UCntrX(poly->pt[i].x), UCntrY(poly->pt[i].y) );
    }
    AreaEnd(rp);

    SetAfPt( rp, &FullBits, 0);  /* reset back to solid */
}

void DrawRhomFrame( inlist )
ScrnPair inlist[];
{
    int i;

    SetDrMd( rp, JAM1);
    SetAPen( rp, 0L );
    SetOutlinePen( rp, WinFgCol );

    AreaMove( rp, UCntrX(inlist[0].x), UCntrY(inlist[0].y));

    for( i = 1; i < 4; i++ ) {
        AreaDraw( rp, UCntrX(inlist[i].x), UCntrY(inlist[i].y) );
    }
    AreaEnd(rp);
    BNDRYOFF( rp ); /* turn off outlining */
}




void SwitchBox()
{
    struct IntuiMessage mycopy,
                        *orig;

    RefreshGadgets(SurfWinDef.FirstGadget, SurfWin, NULL );

    while(1) {
        long wakeupmask;

        wakeupmask = Wait( SignalMask );
        /*
         * for now, we ignore the wakeupmask,
         * just read messages from each. if I notice a performance problem,
         * I'll fix it then
         */

        /*
         * handle messages for the control window
         */
        while((orig =(struct IntuiMessage *) GetMsg( CntrlWin->UserPort )) ) {

            mycopy = *orig;
            ReplyMsg( (struct Message*) orig );

            switch( mycopy.Class ) {
                case MENUPICK:
                    MenuHandler( mycopy.Code );
                    break;

                case GADGETUP:
                    GadgetHandler( (struct Gadget*)mycopy.IAddress );
                    break;

                case CLOSEWINDOW:
                    return;

                default:
                    break;
            }
        }
        /*
         * handle the button window
         */
        while( (orig =(struct IntuiMessage *) GetMsg( GadWin->UserPort )) ) {

            mycopy = *orig;
            ReplyMsg( (struct Message *)orig );

            switch( mycopy.Class ) {
                case GADGETUP:
                    GadgetHandler( (struct Gadget*)mycopy.IAddress );
                    RefreshGadgets(SurfWinDef.FirstGadget, SurfWin, NULL );
                    break;

                default:
                    break;
            }
        }

        /*
         * handle messages for the other window
         */
        while( (orig =(struct IntuiMessage *) GetMsg( SurfWin->UserPort )) ) {

            mycopy = *orig;
            ReplyMsg( (struct Message *)orig );

            switch( mycopy.Class ) {
                case MOUSEBUTTONS:
                    HandleMButtons(&mycopy);
                    break;

                case INTUITICKS:
                    HandleTicks(&mycopy);
                    break;

                case MOUSEMOVE:
                    break;

                default:
                    break;
            }
        }
    }
}

/*
 * display error messages inside a requestor
 */
void OutErr(errstr)
    char *errstr;
{
    static struct IntuiText errtext =
        { -1, -1, JAM1, 10, 10, NULL, NULL, NULL };
    static struct IntuiText negtext =
        { -1, -1, JAM1, 80, 20, NULL,(UBYTE *)"Onwards", NULL };

    errtext.IText = (UBYTE *)errstr;

    WBenchToFront();
    AutoRequest(CntrlWin, &errtext, NULL, &negtext, NULL, NULL,
        8*strlen(errstr)+ 40, 60 );
    WindowToFront( CntrlWin );

}


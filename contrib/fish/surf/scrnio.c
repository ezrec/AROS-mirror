/* main program */

#include <aros/oldprograms.h>
#include "scrnio.ih"
#ifdef MANX
#include <functions.h>
#endif /* MANX */

#include "scrndef.h"
#include "scrnio.h"
#include "gadgetdef.h"
#include "menudef.h"

struct Screen *SurfScrn = NULL;
struct Window *SurfWin = NULL;
struct Window *CntrlWin = NULL;
struct Window *GadWin = NULL;
/*
 * bit masks for waiting for signals
 */
short CntrlSigBit, SurfSigBit, GadSigBit;
long SignalMask = 0;

struct RastPort *rp;
struct ViewPort *vp;

struct GfxBase *GfxBase = 0;
struct IntuitionBase *IntuitionBase = 0;

void OutErr(char *);
void MenuSetColMap();
void MenuSetScrn();

int XOR = COMPLEMENT, /* so my other modules don't need to */
    WRITE = JAM1;     /* include so many amiga includes */

int WinHOrig, WinVOrig;
int WinFgCol;
int ColorMax;
int NumColors;

/*
 * data structures needed to use amiga polygons routines
 */
static WORD PolyArea[5*5];
static struct AreaInfo PolyInfo;
static PLANEPTR PolyWorkSpace;
static struct TmpRas PolyTmpRas;



static void ResetWinDat() {

    WinHOrig = SurfWinDef.Width >>1;
    WinVOrig = SurfWinDef.Height >>1;


    rp = SurfWin->RPort;
    SetDrMd( rp,  JAM1 );
}



/*
 * open surface window/screen
 */
void OpenSurf() {
    NumColors = 1 << SurfScrnDef.Depth;
    ColorMax = ( NumColors -1) * DitherLevels + 1;
    WinFgCol = (NumColors - 1) & 0x1f;

    SurfScrnDef.BlockPen = ( WinFgCol *3) /4;
    SurfScrnDef.DetailPen = WinFgCol>>2;

    SurfScrn = OpenScreen( &SurfScrnDef );
    MenuSetColMap();
    SurfWinDef.Screen = GadWinDef.Screen = SurfScrn;
    SurfWinDef.Width = GadWinDef.Width = SurfScrnDef.Width;
    SurfWinDef.Height = SurfScrnDef.Height - ButHeight;

    SurfWin = OpenWindow( &SurfWinDef );
    GadWin = OpenWindow( &GadWinDef );
    SurfSigBit = SurfWin->UserPort->mp_SigBit;
    GadSigBit = GadWin->UserPort->mp_SigBit;
    SignalMask = (1<<CntrlSigBit) | (1<<SurfSigBit)| (1<<GadSigBit);

    ResetWinDat();
    ShowTitle( SurfScrn, 0L ); /* hide screen title behind backdrop */

    InitArea( &PolyInfo, PolyArea, 5);
    rp->AreaInfo = &PolyInfo;

    PolyWorkSpace = AllocRaster( SurfWinDef.Width, SurfWinDef.Height);

    if( !PolyWorkSpace ) {
        CloseDisplay();
        OutErr("no space for temporary rastern");
        exit(0);
    }
    else {
        InitTmpRas( &PolyTmpRas, PolyWorkSpace,
                    RASSIZE( SurfWinDef.Width, SurfWinDef.Height ));
        rp->TmpRas = &PolyTmpRas;
    }
}


void InitWindow()
{
    GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0);
    if( GfxBase == 0 ) {
        OutErr("graphics library won't open");
        exit(10);
    }

    IntuitionBase = OpenLibrary("intuition.library",0);
    if( IntuitionBase == 0 ) {
        OutErr("intuition library won't open");
        exit(10);
    }

    InitGadgets();
    CntrlWin = OpenWindow( &CntrlWinDef );
    CntrlSigBit = CntrlWin->UserPort->mp_SigBit;

    MenuSetScrn();

    if( !SurfScrn || !SurfWin || !CntrlWin ) {
        OutErr("couldn't open at least one window or screen");
        CloseDisplay();
        exit( 0 );
    }

    SetMenuStrip(CntrlWin, menu );
}


/*
 * remove surface window/screen
 */
void CloseSurf() {

    if( PolyWorkSpace)
        FreeRaster( PolyWorkSpace, SurfWinDef.Width, SurfWinDef.Height );

    if( SurfWin )
        CloseWindow( SurfWin );

    if( GadWin )
        CloseWindow( GadWin );

    if( SurfScrn )
        CloseScreen( SurfScrn );
}


void CloseDisplay()
{

    CloseSurf();


    if( CntrlWin ) {
        ClearMenuStrip( CntrlWin );
        CloseWindow( CntrlWin );
    }

    EndGadgets();

    if ( IntuitionBase )
        CloseLibrary((struct Library *)IntuitionBase);

    if ( GfxBase )
        CloseLibrary((struct Library *)GfxBase);
}



/*
    Example for bitmaps
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include <graphics/gfxmacros.h>

#include <stdlib.h>
#include <stdio.h>

static struct Window *window;
static struct ColorMap *cm;
static struct RastPort *win_rp;

#define BMWIDTH (50)
#define BMHEIGHT (50)
struct BitMap *bm;
struct RastPort *bm_rp;

/*
    ObtainBestPen() returns -1 when it fails, therefore we
    initialize the pen numbers with -1 to simplify cleanup.
*/
static LONG pen1=-1;
static LONG pen2=-1;

static void draw_bitmap(void);
static void clean_exit(CONST_STRPTR s);
static void handle_events(void);


int main(void)
{
    window = OpenWindowTags(NULL,
        WA_Left,   50,
        WA_Top,    70,
        WA_Width,  400,
        WA_Height, 350,
    
        WA_Title,         "Bitmap Graphics",
        WA_Activate,      TRUE,
        WA_SmartRefresh,  TRUE,
        WA_NoCareRefresh, TRUE,
        WA_GimmeZeroZero, TRUE,
        WA_CloseGadget,   TRUE,
        WA_DragBar,       TRUE,
        WA_DepthGadget,   TRUE,
        WA_IDCMP,         IDCMP_CLOSEWINDOW,
        TAG_END);
    
    if (! window) clean_exit("Can't open window\n");
    
    win_rp = window->RPort;
    cm = window->WScreen->ViewPort.ColorMap;

    // Let's obtain two pens
    pen1 = ObtainBestPen(cm, 0xFFFF0000 , 0 , 0 , TAG_END);
    pen2 = ObtainBestPen(cm, 0 , 0 , 0xFFFF0000 , TAG_END);
    if ( !pen1 || !pen2) clean_exit("Can't allocate pen\n");
    
    draw_bitmap();
    handle_events();

    clean_exit(NULL);

    return 0;
}


static void draw_bitmap(void)
{
    /*
        Get the depth of the screen. Don't peek in the structures, always use
        GetBitMapAttr().
    */
    UWORD depth = GetBitMapAttr(win_rp->BitMap, BMA_DEPTH);

    /*
        Create new bitmap. With BMF_MINPLANES and the bitmap pointer we indicate
        we want a bitmap which is similar to the target bitmap.
    */
    bm = AllocBitMap(BMWIDTH , BMHEIGHT, depth, BMF_MINPLANES, win_rp->BitMap);
    if (!bm) clean_exit("Can't allocate bitmap\n");
    
    bm_rp = CreateRastPort();      // Create rastport for our bitmap
    if (!bm_rp) clean_exit("Can't allocate rastport!\n");
    bm_rp->BitMap = bm;            // Link bitmap to rastport

    /*
        Now we can draw into our bitmap. Take care that the bitmap has no
        clipping rectangle. This means we must not draw over the limits.
    */
    SetRast(bm_rp, 0);                // fill whole bitmap with color 0
    SetAPen(bm_rp, pen1);
    DrawCircle(bm_rp, 24, 24, 24);
    SetAPen(bm_rp, pen2);
    Move(bm_rp, 0, 0);
    Draw(bm_rp, 49, 49);
    Move(bm_rp, 49, 0);
    Draw(bm_rp, 0, 49);
    Draw(bm_rp, 49, 49);
    Draw(bm_rp, 49, 0);
    Draw(bm_rp, 0, 0);
    Draw(bm_rp, 0, 49);
    
    int x;
    for (x=20; x<400 ; x+=30)
    {
        // Blit the bitmap into the window
        ClipBlit(bm_rp, 0, 0, win_rp, x, x/2, BMWIDTH, BMHEIGHT, 0xC0);
    }
}


static void handle_events(void)
{
    /*
        A simple event handler. This will be explained more detailed
        in the Intuition examples.
    */
    struct IntuiMessage *imsg;
    struct MsgPort *port = window->UserPort;
    BOOL terminated = FALSE;
        
    while (!terminated)
    {
        Wait(1L << port->mp_SigBit);
        if ((imsg = (struct IntuiMessage *)GetMsg(port)) != NULL)
        {
            switch (imsg->Class)
            {
                case IDCMP_CLOSEWINDOW:
                    terminated = TRUE;
                    break;
            }
            ReplyMsg((struct Message *)imsg);
        }
    }
}


static void clean_exit(CONST_STRPTR s)
{
    if (s) PutStr(s);

    // Give back allocated resourses
    if (bm) FreeBitMap(bm);
    if (bm_rp) FreeRastPort(bm_rp);
    if (pen1 != -1) ReleasePen(cm, pen1);
    if (pen2 != -1) ReleasePen(cm, pen2);
    if (window) CloseWindow(window);

    exit(0);
}

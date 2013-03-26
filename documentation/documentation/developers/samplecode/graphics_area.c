/*
    Example for area drawing routines
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include <graphics/gfxmacros.h>

#include <stdlib.h>

static struct Window *window;
static struct ColorMap *cm;
static struct RastPort *rp;

/*
    ObtainBestPen() returns -1 when it fails, therefore we
    initialize the pen numbers with -1 to simplify cleanup.
*/
static LONG pen1=-1;
static LONG pen2=-1;

#define MAX_POINTS 50
static struct AreaInfo ai;
static struct TmpRas tr;
static void *trbuf;
static UBYTE aibuf[(MAX_POINTS + 1) * 5];

static void draw_area(void);
static void clean_exit(CONST_STRPTR s);
static void handle_events(void);


int main(void)
{
    window = OpenWindowTags(NULL,
        WA_Left,   50,
        WA_Top,    70,
        WA_Width,  400,
        WA_Height, 350,
    
        WA_Title,         "Area Graphics",
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
    
    rp = window->RPort;
    cm = window->WScreen->ViewPort.ColorMap;

    // Let's obtain two pens
    pen1 = ObtainBestPen(cm, 0xFFFF0000 , 0 , 0 , TAG_END);
    pen2 = ObtainBestPen(cm, 0 , 0 , 0xFFFF0000 , TAG_END);

    if ( !pen1 || !pen2) clean_exit("Can't allocate pen\n");
    
    draw_area();
    handle_events();

    clean_exit(NULL);

    return 0;
}


static void draw_area(void)
{
    /*
        The area drawing functions need two additional
        structures, which have to be linked with the rastport.
    
        First we set the AreaInfo.
        The size of 'aibuf' must be at least 5 times the number
        of vertexes.
        Take care: when you define the variable 'aibuf' locally, you
        have to set all fields to 0.
    */
    InitArea(&ai, aibuf, sizeof(aibuf) / 5);

    /*
        Then we allocate a raster. It must have the size of
        the drawing area. We have a GimmeZeroZero window with
        no size gadget, therefore we can use the GZZ sizes.
    */
    trbuf = AllocRaster(window->GZZWidth, window->GZZHeight);
    if (!trbuf) clean_exit("TmpRas buffer allocation failed!\n");
    
    /*
        The raster must be initialized. The reason for RASSIZE() is
        that we must round up the width to a 16 bit value.
    */
    InitTmpRas(&tr, trbuf, RASSIZE(window->GZZWidth, window->GZZHeight));

    rp->AreaInfo = &ai;              // Link areainfo to rastport
    rp->TmpRas = &tr;                // Link tempras to rastport

    SetAPen(rp, pen1);               // Set foreground color
    SetOPen(rp, pen2);               // Set outline color

    AreaMove(rp, 50, 200);           // Set start point of 1st triangle
    AreaDraw(rp, 300, 100);
    AreaDraw(rp, 280, 300);

    AreaMove(rp, 200, 50);           // Set start point of 2nd triangle
    AreaDraw(rp, 210, 100);
    AreaDraw(rp, 300, 75);
    
    AreaEllipse(rp, 70, 70, 40, 30); // Add an ellipse

    AreaEnd(rp);                     // Do the rendering
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
    if (trbuf) FreeRaster(trbuf, window->GZZWidth, window->GZZHeight);
    if (pen1 != -1) ReleasePen(cm, pen1);
    if (pen2 != -1) ReleasePen(cm, pen2);
    if (window) CloseWindow(window);

    exit(0);
}

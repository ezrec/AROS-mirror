/*
    Example for simple drawing routines
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

static void draw_simple(void);
static void clean_exit(CONST_STRPTR s);
static void handle_events(void);


int main(void)
{
    window = OpenWindowTags(NULL,
	WA_Left,   50,
	WA_Top,    70,
	WA_Width,  400,
	WA_Height, 350,
    
	WA_Title,         "Simple Graphics",
	WA_Activate,      TRUE,
	WA_SmartRefresh,  TRUE,
	WA_GimmeZeroZero, TRUE,
	WA_CloseGadget,   TRUE,
	WA_DragBar,       TRUE,
	WA_IDCMP,         IDCMP_CLOSEWINDOW,
	WA_DepthGadget,   TRUE,
	TAG_END);
    
    if (! window) clean_exit("Can't open window\n");
    
    rp = window->RPort;
    cm = window->WScreen->ViewPort.ColorMap;

    // Let's obtain two pens
    pen1 = ObtainBestPen(cm, 0xFFFF0000 , 0 , 0 , TAG_END);
    pen2 = ObtainBestPen(cm, 0 , 0 , 0xFFFF0000 , TAG_END);
    if ( !pen1 || !pen2) clean_exit("Can't allocate pen\n");
    
    draw_simple();
    handle_events();

    clean_exit(NULL);

    return 0;
}


static void draw_simple(void)
{
    WORD array[] = {                // Polygon for PolyDraw()
	50, 200,
	80, 180,
	90, 220,
	50, 200,
    };
    
    SetAPen(rp, pen1);              // Set foreground color
    SetBPen(rp, pen2);              // Set background color
    
    WritePixel(rp, 30, 70);         // Plot a point
    
    SetDrPt(rp, 0xFF00);            // Change line pattern. Set pixels are drawn
                                    // with APen, unset with BPen
    
    Move(rp, 20, 50);               // Move cursor to given point
    Draw(rp, 100, 80);              // Draw a line from current to given point
    
    DrawEllipse(rp, 70,30, 15, 10); // Draw an ellipse

    /*
	Draw a polygon. Note that the first line is draw from the
	end of the last Move() or Draw() command.
    */
    PolyDraw(rp, sizeof(array)/sizeof(WORD)/2, array);
    
    SetDrMd(rp, JAM1);             // We want to use only the foreground pen.
    Move(rp, 200, 80);
    Text(rp, "Text in default font", 20);
    
    SetDrPt(rp, 0xFFFF);           // Reset line pattern
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
    if (pen1 != -1) ReleasePen(cm, pen1);
    if (pen2 != -1) ReleasePen(cm, pen2);
    if (window) CloseWindow(window);

    exit(0);
}

/*
    Example of a custom screen
    
    This shows how to create a screen for an application.
    It uses the standard 3D look. An additional pen for drawing will be requested.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include <stdlib.h>

static struct Window *window;
static struct Screen *screen;
static struct DrawInfo *drawinfo;
static struct RastPort *rp;
static struct ColorMap *cm;

/*
    ObtainBestPen() returns -1 when it fails, therefore we
    initialize the pen numbers with -1 to simplify cleanup.
*/
static LONG pen=-1;

static void clean_exit(CONST_STRPTR s);
static void draw_stuff(void);
static void handle_events(void);


static UWORD pens[] = {~0};

int main(void)
{
    screen = OpenScreenTags(NULL,
        SA_Width,  800,
        SA_Height, 600,
        SA_Depth,  16,
        SA_Pens,   pens, // Enables default 3D look
        SA_Title,  "Default screen title",
        TAG_END);

    if (! screen) clean_exit("Can't open screen\n");
    
    window = OpenWindowTags(NULL,
        WA_Left,          100,
        WA_Top,           70,
        WA_InnerWidth,    600,
        WA_InnerHeight,   300,
        WA_Title,         "Custom screen",
        WA_ScreenTitle,   "Screen title", // Screen title when window is active
        WA_Activate,      TRUE,
        WA_CloseGadget,   TRUE,
        WA_DragBar,       TRUE,
        WA_DepthGadget,   TRUE,
        WA_IDCMP,         IDCMP_CLOSEWINDOW,
        WA_SmartRefresh,  TRUE,   // Lets Intuition handle exposed regions
        WA_NoCareRefresh, TRUE,   // We don't want to listen to refresh messages
        WA_CustomScreen,  screen, // Link to screen
        TAG_END);

    if (! window) clean_exit("Can't open window\n");
    
    if ( ! (drawinfo = GetScreenDrawInfo(screen)))
        clean_exit("Can't get screendrawinfo\n");
    
    rp = window->RPort;
    cm = screen->ViewPort.ColorMap;
    
    draw_stuff();
    
    handle_events();
    
    clean_exit(NULL);

    return 0;
}

static void draw_stuff(void)
{
    SetAPen(rp, drawinfo->dri_Pens[TEXTPEN]);
    Move(rp, 100, 100);
    Text(rp, "This text is written in default color 'TEXTPEN'.", 48);

    SetAPen(rp, drawinfo->dri_Pens[SHINEPEN]);
    Move(rp, 100, 130);
    Text(rp, "This text is written in default color 'SHINEPEN'.", 49);

    // We ask nicely for a new pen...
    pen = ObtainBestPen(cm, 0 , 0 , 0xFFFF0000 , TAG_END);
    if ( !pen) clean_exit("Can't allocate pen\n");
    
    // ... and use it to draw a line.
    SetAPen(rp, pen);
    Move(rp, 100, 200);
    Draw(rp, 500, 200);
}


static void handle_events(void)
{
    struct IntuiMessage *imsg;
    struct MsgPort *port = window->UserPort;

    ULONG signals;
    
    BOOL terminated = FALSE;
    
    while (! terminated)
    {
        signals = Wait(1L << port->mp_SigBit);

        while ((imsg = (struct IntuiMessage *)GetMsg(port)) != NULL)
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
    if (pen != -1) ReleasePen(cm, pen);
    if (drawinfo) FreeScreenDrawInfo(screen, drawinfo);
    if (window) CloseWindow(window);
    if (screen) CloseScreen(screen);

    exit(0);
}

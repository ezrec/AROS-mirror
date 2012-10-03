/*
    Example for refresh handling of intuition windows

    Two windows are opened, one with simplerefresh, the other one with
    smartrefresh. You can watch what messages are sent when you move around
    or resize the windows. There is intentionally no redrawing in the
    event handler, so that you can clearly see the result of your window
    manipulation.
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include <stdlib.h>
#include <stdio.h>

static struct Window *window1, *window2;
static struct ColorMap *cm;

/*
    ObtainBestPen() returns -1 when it fails, therefore we
    initialize the pen numbers with -1 to simplify cleanup.
*/
static LONG pen1=-1;
static LONG pen2=-1;

static void draw_stuff(struct Window *win);
static void clean_exit(CONST_STRPTR s);
static BOOL handle_events(struct Window *win, BOOL terminated);


int main(void)
{
    /*
        We open two windows, the first with simple refresh,
        the second with smart refresh.
    */
    window1 = OpenWindowTags(NULL,
        WA_Left,          50,
        WA_Top,           70,
        WA_InnerWidth,    300,
        WA_InnerHeight,   300,
        
        /*
            When we have a size gadget we must additionally
            define the limits.
        */
        WA_MinWidth,      100,
        WA_MinHeight,     100,
        WA_MaxWidth,      -1,
        WA_MaxHeight,     -1,
        
        WA_Title,         "Simplerefresh window",
        WA_Activate,      TRUE,
        WA_SimpleRefresh, TRUE,
        WA_CloseGadget,   TRUE,
        WA_SizeGadget,    TRUE,
        WA_DragBar,       TRUE,
        WA_DepthGadget,   TRUE,
        WA_GimmeZeroZero, TRUE,
        WA_IDCMP,         IDCMP_CLOSEWINDOW | IDCMP_CHANGEWINDOW | IDCMP_NEWSIZE |
                          IDCMP_REFRESHWINDOW | IDCMP_SIZEVERIFY,
        TAG_END);

    if (! window1) clean_exit("Can't open window 1\n");
    
    window2 = OpenWindowTags(NULL,
        WA_Left,          400,
        WA_Top,           70,
        WA_InnerWidth,    300,
        WA_InnerHeight,   300,
        WA_MinWidth,      100,
        WA_MinHeight,     100,
        WA_MaxWidth,      -1,
        WA_MaxHeight,     -1,
        WA_Title,         "Smartrefresh window",
        WA_SmartRefresh,  TRUE,
        WA_CloseGadget,   TRUE,
        WA_SizeGadget,    TRUE,
        WA_DragBar,       TRUE,
        WA_DepthGadget,   TRUE,
        WA_GimmeZeroZero, TRUE,
        WA_IDCMP,         IDCMP_CLOSEWINDOW | IDCMP_CHANGEWINDOW | IDCMP_NEWSIZE |
                          IDCMP_REFRESHWINDOW | IDCMP_SIZEVERIFY,
        TAG_END);

    if (! window2) clean_exit("Can't open window 2\n");
    
    cm = window1->WScreen->ViewPort.ColorMap;

    // Let's obtain two pens
    pen1 = ObtainBestPen(cm, 0xFFFF0000 , 0 , 0 , TAG_END);
    pen2 = ObtainBestPen(cm, 0 , 0 , 0xFFFF0000 , TAG_END);
    if ( !pen1 || !pen2) clean_exit("Can't allocate pen\n");
    
    draw_stuff(window1);
    draw_stuff(window2);

    ULONG signals;
    BOOL terminated = FALSE;
    
    while (! terminated)
    {
        /*
            If we want to wait for signals of more than one window
            we have to combine the signal bits.
        */
        signals = Wait(
            (1L << window1->UserPort->mp_SigBit) |
            (1L << window2->UserPort->mp_SigBit)
        );
        /*
            Now we can check which window has received the signal and
            then we call the event handler for that window.
        */
        if (signals & (1L << window1->UserPort->mp_SigBit))
            terminated = handle_events(window1, terminated);
        else if (signals & (1L << window2->UserPort->mp_SigBit))
            terminated = handle_events(window2, terminated);
    }
    
    clean_exit(NULL);

    return 0;
}


static void draw_stuff(struct Window *win)
{
    int x;
    
    struct RastPort *rp = win->RPort;
        
    for (x=10 ; x<=290 ; x+=10)
    {
        SetAPen(rp, pen1);
        Move(rp, x, 10);
        Draw(rp, 300-x, 290);
        SetAPen(rp, pen2);
        Move(rp, 10, x);
        Draw(rp, 290, 300-x);
    }
}


static BOOL handle_events(struct Window *win, BOOL terminated)
{
    struct IntuiMessage *imsg;
    struct MsgPort *port = win->UserPort;

    static ULONG event_nr = 0;

    while ((imsg = (struct IntuiMessage *)GetMsg(port)) != NULL)
    {
        event_nr++;
        printf("Event # %d ", event_nr);
        
        if (win == window1)
            printf("Window #1 ");
        else
            printf("Window #2 ");
        
        switch (imsg->Class)
        {
            case IDCMP_CLOSEWINDOW:
                puts("IDCMP_CLOSEWINDOW");
                terminated = TRUE;
                break;
            case IDCMP_CHANGEWINDOW:
                // Window has been moved or resized
                puts("IDCMP_CHANGEWINDOW");
                break;
            case IDCMP_NEWSIZE:
                puts("IDCMP_NEWSIZE");
                break;
            case IDCMP_REFRESHWINDOW:
                puts("IDCMP_REFRESHWINDOW");
                BeginRefresh(win);
                /*
                    Here you can add code which redraws
                    exposed parts of the window.
                */
                EndRefresh(win, TRUE);
                break;
            case IDCMP_SIZEVERIFY:
                // SIZEVERIFY blocks a window until the message has been replied
                puts("IDCMP_SIZEVERIFY");
                break;
        }
        // Every message must be replied.
        ReplyMsg((struct Message *)imsg);
    }
    return terminated;
}


static void clean_exit(CONST_STRPTR s)
{
    if (s) puts(s);

    // Give back allocated resourses
    if (pen1 != -1) ReleasePen(cm, pen1);
    if (pen2 != -1) ReleasePen(cm, pen2);
    if (window1) CloseWindow(window1);
    if (window2) CloseWindow(window2);

    exit(0);
}

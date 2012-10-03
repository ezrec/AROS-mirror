/*
    Example for fonts
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/diskfont.h>

#include <stdlib.h>
#include <string.h>

static struct Window *window;
static struct ColorMap *cm;
static struct RastPort *rp;
static struct TextFont *font;
/*
    ObtainBestPen() returns -1 when it fails, therefore we
    initialize the pen numbers with -1 to simplify cleanup.
*/
static LONG pen1=-1;
static LONG pen2=-1;

static void draw_font(void);
static void write_text(CONST_STRPTR s, WORD x, WORD y, ULONG mode);
static void clean_exit(CONST_STRPTR s);
static void handle_events(void);


int main(void)
{
    window = OpenWindowTags(NULL,
        WA_Left,   50,
        WA_Top,    70,
        WA_Width,  400,
        WA_Height, 350,
    
        WA_Title,         "Fonts",
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
    
    draw_font();
    handle_events();

    clean_exit(NULL);

    return 0;
}


static void draw_font(void)
{
    ULONG style;
    struct TextAttr ta = {
        "arial.font",             // Font name
        15,                       // Font size
        FSF_ITALIC | FSF_BOLD,    // Font style
        0
    };

    if ( ! (font = OpenDiskFont(&ta)))
    {
        clean_exit("Can't open font\n");
    }
    
    SetAPen(rp, pen1);
    SetBPen(rp, pen2);
    
    SetFont(rp, font);            // Linking the font to the rastport
    
    
    /*
        In the TextAttr above we've queried a font with the styles italic and
        bold. OpenDiskFont() tries to open a font with these styles. If this
        fails the styles have to be generated algorithmically. To avoid adding a
        style to a font that already has that style intrinsically, we first have
        to ask. AskSoftStyle() returns a mask where all bits for styles which have
        to be added algorithmically are set.
    */
    style = AskSoftStyle(rp);
    
    /*
        We finally set the style. SetSoftStyle() compares with the mask from
        AskSoftStyle() to avoid that an intrinsic style is applied again.
    */
    SetSoftStyle(rp, style, FSF_ITALIC | FSF_BOLD);
    
    /*
        Now we write some text. Additionally the effects of the
        rastport modes are demonstrated.
    */
    write_text("JAM1", 100, 60, JAM1);
    write_text("JAM2", 100, 80, JAM2);
    write_text("COMPLEMENT", 100, 100, COMPLEMENT);
    write_text("INVERSVID", 100, 120, INVERSVID);
    write_text("JAM1|INVERSVID", 100, 140, JAM1|INVERSVID);   
    write_text("JAM2|INVERSVID", 100, 160, JAM2|INVERSVID);   
    write_text("COMPLEMENT|INVERSVID", 100, 180, COMPLEMENT|INVERSVID);   
}


static void write_text(CONST_STRPTR txt, WORD x, WORD y, ULONG mode)
{
    SetDrMd(rp, mode);
    Move(rp, x, y);
    Text(rp, txt, strlen(txt));
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
    if (font) CloseFont(font);
    if (window) CloseWindow(window);

    exit(0);
}

/*
    File-, Font- and Screenmoderequester
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/diskfont.h>
#include <proto/asl.h>

#include <stdlib.h>
#include <string.h>

static void load_file(CONST_STRPTR filename);
static void handle_events(void);
static void clean_exit(CONST_STRPTR s);

static struct Screen *screen;
static struct Window *window;
static struct RastPort *rp;
static struct TextFont *font;

static struct FileRequester *filereq;
static TEXT filename[1000];
TEXT buffer[100];

static struct FontRequester *fontreq;
static struct ScreenModeRequester *screenreq;

static UWORD pens[] = {~0};

int main(void)
{
    /*
	Filerequester
    */
    filereq = (struct FileRequester *)AllocAslRequestTags(ASL_FileRequest,
	ASLFR_TitleText,     "Select a file for reading",
	ASLFR_DoPatterns,    TRUE,
	ASLFR_InitialDrawer, "S:",
	TAG_END);
    if ( ! filereq) clean_exit("Can't open filerequester\n");
    
    if (AslRequest(filereq, NULL))
    {
	strcpy(filename, filereq->rf_Dir);
	if ( ! AddPart(filename, filereq->rf_File, sizeof(filename)))
	{
	    clean_exit("AddPart() failed\n");
	}
    }
    else
    {
	clean_exit("Filerequester cancelled\n");
    }

    /*
	Fontrequester
    */
    fontreq = (struct FontRequester *)AllocAslRequestTags(ASL_FontRequest,
	ASLFO_TitleText,  "Select a font",
	ASLFO_DoFrontPen, TRUE,
	ASLFO_DoStyle,    TRUE,
	TAG_END);
    if ( ! fontreq) clean_exit("Can't open fontrequester\n");
    
    if (AslRequest(fontreq, NULL))
    {
    }
    else
    {
	clean_exit("Fontrequester cancelled\n");
    }

    /*
	ScreenModerequester
    */
    screenreq = (struct ScreenModeRequester *)AllocAslRequestTags(ASL_ScreenModeRequest,
	ASLSM_TitleText, "Select a screenmode",
	ASLSM_DoDepth,   TRUE,
	TAG_END);
    if ( ! screenreq) clean_exit("Can't open screenmoderequester\n");
    
    if (AslRequest(screenreq, NULL))
    {
    }
    else
    {
	clean_exit("Screenmoderequester cancelled\n");
    }

    /*
	Open the screen
    */
    screen = OpenScreenTags(NULL,
	SA_Width,  screenreq->sm_DisplayWidth,
	SA_Height, screenreq->sm_DisplayHeight,
	SA_Depth,  screenreq->sm_DisplayDepth,
	SA_DisplayID, screenreq->sm_DisplayID,
	SA_Pens,   pens, // Enables default 3D look
	SA_Title,  "ASL Demo",
	TAG_END);

    if (! screen) clean_exit("Can't open screen\n");
    
    /*
	Open the window
    */
    window = OpenWindowTags(NULL,
	WA_Left,          0,
	WA_Top,           50,
	WA_Title,         "ASL Demo",
	WA_Activate,      TRUE,
	WA_CloseGadget,   TRUE,
	WA_DragBar,       TRUE,
	WA_DepthGadget,   TRUE,
	WA_SmartRefresh,  TRUE,
	WA_NoCareRefresh, TRUE,
	WA_IDCMP,         IDCMP_CLOSEWINDOW,
	WA_GimmeZeroZero, TRUE,
	WA_CustomScreen,  screen, // Link to screen
	TAG_END);

    if (! window) clean_exit("Can't open window\n");

    rp = window->RPort;
    
    /*
	Load the font and prepare the rastport
    */
    if ( ! (font = OpenDiskFont(&fontreq->fo_Attr)))
    {
	clean_exit("Can't open font\n");
    }

    SetFont(rp, font);
    ULONG styles = AskSoftStyle(rp);
    SetSoftStyle(rp, styles, fontreq->fo_Attr.ta_Style);
    SetAPen(rp, fontreq->fo_FrontPen);
    
    Move(rp, 5,15);
    
    load_file(filename);
    
    handle_events();
    
    clean_exit(NULL);
    return 0;
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

void load_file(CONST_STRPTR filename)
{
    if ( ! filename) return;
    
    BPTR infile = 0;
    WORD cp_x = rp->cp_x;
    WORD cp_y = rp->cp_y;
    ULONG len;
    
    if ( ! (infile = Open(filename, MODE_OLDFILE)))
    {
	goto cleanup;
    }
    
    while (FGets(infile, buffer, sizeof(buffer)))
    {
	len = strlen(buffer);
	// Remove newline
	if (buffer[len-1] == '\n')
	{
	    len--;
	}
	
	Move(rp, cp_x, cp_y);
	Text(rp, buffer, len);
	cp_y += rp->TxHeight;
    }
    
cleanup:
    PrintFault(IoErr(), "Error");
    if (infile) Close(infile);
}


static void clean_exit(CONST_STRPTR s)
{
    if (s) PutStr(s);

    // Give back allocated resourses
    if (filereq) FreeAslRequest(filereq);
    if (fontreq) FreeAslRequest(fontreq);
    if (screenreq) FreeAslRequest(screenreq);
    if (font) CloseFont(font);
    if (window) CloseWindow(window);
    if (screen) CloseScreen(screen);
    
    exit(0);
}

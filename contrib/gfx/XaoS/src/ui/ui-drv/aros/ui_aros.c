#include "aconfig.h"
#include "version.h"

const char VERsion[]="$VER: XaoS "XaoS_VERSION" ("__DATE__") $";

/*includes */
#include <ui.h>

#include <exec/memory.h>
#include <intuition/intuition.h>
#include <cybergraphx/cybergraphics.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>
#include <proto/keymap.h>
#include <string.h>

#define USE_GZZ     	TRUE

#if USE_GZZ
#define BORDERLEFT  	0
#define BORDERTOP   	0
#else
#define BORDERLEFT  	win->BorderLeft
#define BORDERTOP   	win->BorderTop
#endif

struct TextAttr     	topaz8 =
{
    "topaz.font" , 8
};

struct IntuitionBase 	*IntuitionBase;
struct GfxBase      	*GfxBase;
struct Library      	*CyberGfxBase;
struct Library	    	*KeymapBase;

struct ui_driver    	aros_driver;

struct Screen	    	*scr;
struct DrawInfo     	*dri;
struct Window	    	*win;
struct RastPort     	*rp;
struct TextFont     	*topazfont;
LONG	    	    	width = 320, height = 200;
ULONG	    	    	coltab[256];
char	    	    	*buffer[2];
int 	    	    	mousebuttons;
int 	    	    	actbuffer = 0;
static char 	    	*pubscreen = NULL;

static int
aros_set_color (int r, int g, int b, int init)
{
    static int val;

    if (init) val = 0;

    coltab[val] = (r << 16) + (g << 8) + b;

    return val++;
}

#if 0
static void
aros_setpalette (ui_palette pal, int start, int end)
{
}
#endif

static void
aros_print (int x, int y, char *text)
{
    SetABPenDrMd(rp, 2, 1, JAM2);
    Move(rp, BORDERLEFT + x, BORDERTOP + y + rp->TxBaseline);
    Text(rp, text, strlen(text));
}

static void
aros_display ()
{
    if (!buffer[0]) return;

#if 1
    WriteLUTPixelArray(buffer[actbuffer],
    	    	       0,
		       0,
		       width,
		       rp,
		       coltab,
		       BORDERLEFT,
		       BORDERTOP,
		       width,
		       height,
		       CTABFMT_XRGB8);
#else
    WriteChunkyPixels(rp, BORDERLEFT,
    	    	    	  BORDERTOP,
			  BORDERLEFT + width - 1,
			  BORDERTOP + height - 1,
			  buffer[actbuffer],
			  width);
#endif
}

static void
aros_flip_buffers ()
{
    actbuffer = 1 - actbuffer;
}

void
aros_free_buffers (char *b1, char *b2)
{
    if (buffer[0]) FreeVec(buffer[0]);
    buffer[0] = buffer[1] = NULL;
}

int
aros_alloc_buffers (char **b1, char **b2)
{
    *b1 = buffer[0] = AllocVec(width * height * 2, MEMF_CLEAR);
    if (buffer[0]) *b2 = buffer[1] = buffer[0] + width * height;

    actbuffer = 0;
    
    return width;			/* bytes per scanline */
}

static void
aros_getsize (int *w, int *h)
{
    *w = width;
    *h = height;
}

static void
aros_processevents (int wait, int *mx, int *my, int *mb, int *k)
{
    static int iflag = 0;
    
    struct IntuiMessage *imsg;
    
    if (wait) WaitPort(win->UserPort);
    
    while((imsg = (struct IntuiMessage *)GetMsg(win->UserPort)))
    {
    	switch(imsg->Class)
	{
	    case IDCMP_MOUSEBUTTONS:
	    	switch(imsg->Code)
		{
		    case SELECTDOWN:
		    	mousebuttons |= BUTTON1;
			break;
			
		    case SELECTUP:
		        mousebuttons &= ~BUTTON1;
			break;
			
	    	    case MENUDOWN:
		    	mousebuttons |= BUTTON3;
			break;
		
		    case MENUUP:
		    	mousebuttons &= ~BUTTON3;
			break;
			
		    case MIDDLEDOWN:
		    	mousebuttons |= BUTTON2;
			break;
			
		    case MIDDLEUP:
		    	mousebuttons &= ~BUTTON2;
			break;
		}
		break;
	
	    case IDCMP_RAWKEY:
	    	switch(imsg->Code)
		{
		    case CURSORLEFT:
		    	iflag |= 1;
			break;
			
		    case CURSORLEFT | IECODE_UP_PREFIX:
		    	iflag &= ~1;
			break;
			
		    case CURSORRIGHT:
		    	iflag |= 2;
			break;
			
		    case CURSORRIGHT | IECODE_UP_PREFIX:
		    	iflag &= ~2;
			break;
			
		    case CURSORUP:
		    	iflag |= 4;
			break;
			
		    case CURSORUP | IECODE_UP_PREFIX:
		    	iflag &= ~4;
			break;
		
		    case CURSORDOWN:
		    	iflag |= 8;
			break;
			
		    case CURSORDOWN | IECODE_UP_PREFIX:
		    	iflag &= ~8;
			break;
			
		    default:
		    	if (!(imsg->Code & IECODE_UP_PREFIX))
			{
			    UBYTE key = 0;
			    
			    if (imsg->Code == 0x45) /* ESC */
			    {
			       key = 'q';
			    }
			    else if ((imsg->Code == 0x50) || (imsg->Code == 0x5f)) /* F1 or HELP */
			    {
			       key = 'h';
			    }
			    else
			    {
			    	struct InputEvent ie;
			    	UBYTE buffer[10];
				
				memset(&ie, 0, sizeof(ie));

    	    	    	    	ie.ie_Class = IECLASS_RAWKEY;
				ie.ie_Code = imsg->Code;
				ie.ie_Qualifier = imsg->Qualifier;

				if (MapRawKey(&ie, buffer, 10, NULL) == 1)
				{
				    key = buffer[0];
				}
			    }
			    
			    if (key) ui_key(key);
			    
			} /* if (!(imsg->Code & IECODE_UP_PREFIX)) */
			break;
			
		} /* switch(imsg->Code) */
		break;
	
	    case IDCMP_NEWSIZE:
		if ((win->GZZWidth != width) ||
		    (win->GZZHeight != height))
		{
		    width = win->GZZWidth;
		    height = win->GZZHeight;
		    ui_resize();
		}
		break;
	
	    case IDCMP_CLOSEWINDOW: 
	    	ui_quit(0);
		break;
		
	} /* switch(imsg->Class) */
	
    	ReplyMsg((struct Message *)imsg);
	
    } /* while((imsg = (struct IntuiMessage *)GetMsg(win->UserPort))) */
    
    *mx = win->GZZMouseX;
    *my = win->GZZMouseY;
    *mb = mousebuttons;
    *k = iflag;
}

static int
aros_init ()
{
    if (!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 39)))
    {
    	printf("Can't open intuition.library V39!\n");
        return 0;
    }
    
    if (!(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 40)))
    {
    	printf("Can't open graphics.library V40!\n");
        return 0;
    }
    
    if (!(CyberGfxBase = OpenLibrary("cybergraphics.library", 0)))
    {
    	printf("Can't open cybergraphics.library\n");
        return 0;	
    }
    
    if (!(KeymapBase = OpenLibrary("keymap.library", 0)))
    {
    	printf("Can't open keymap.library!\n");
    	return 0;
    }
    
    topazfont = OpenFont(&topaz8);
    
    scr = LockPubScreen(pubscreen);
    if (!scr)
    {
    	printf("Can't lock pub screen!\n");
    	return 0;
    }
    
    if (!(dri = GetScreenDrawInfo(scr)))
    {
    	printf("Can't get screen drawinfo!\n");
    	return 0;
    }
    
    win = OpenWindowTags(0, WA_PubScreen    , (IPTR)scr     	,
    	    	    	    WA_Left 	    , 20    	    	,
			    WA_Top  	    , 20    	    	,
			    WA_InnerWidth   , width 	    	,
			    WA_InnerHeight  , height	    	,
			    WA_Title	    , (IPTR)"XaoS"  	,
			    WA_CloseGadget  , TRUE  	    	,
			    WA_DragBar	    , TRUE  	    	,
			    WA_DepthGadget  , TRUE  	    	,
			    WA_SizeGadget   , TRUE  	    	,
			    WA_SizeBBottom  , TRUE  	    	,
			    WA_RMBTrap	    , TRUE  	    	,
			    WA_MinWidth     , 50    	    	,
			    WA_MinHeight    , 50    	    	,
			    WA_MaxWidth     , scr->Width    	,
			    WA_MaxHeight    , scr->Height	,
			    WA_Activate     , TRUE  	    	,
			    WA_ReportMouse  , TRUE  	    	,
			    WA_NoCareRefresh, TRUE  	    	,
			    WA_GimmeZeroZero, USE_GZZ  	    	,
			    WA_IDCMP	    , IDCMP_MOUSEMOVE 	 |
			    	      	      IDCMP_MOUSEBUTTONS |
				      	      IDCMP_RAWKEY  	 |
				      	      IDCMP_NEWSIZE 	 |
				      	      IDCMP_CLOSEWINDOW ,
			    TAG_DONE);
    
    if (!win)
    {
    	printf("Can't open window!\n");
    	return 0;
    }	
    
    rp = win->RPort;
    if (topazfont) SetFont(rp, topazfont);
        	    
    return 1;
}

static void
aros_uninitialise ()
{
    if (win) CloseWindow(win); win = NULL;
    if (dri) FreeScreenDrawInfo(scr, dri); dri = NULL;
    if (scr) UnlockPubScreen(0, scr); scr = NULL;
    
    if (topazfont) CloseFont(topazfont);
    
    if (KeymapBase) CloseLibrary(KeymapBase); KeymapBase = NULL;
    if (CyberGfxBase) CloseLibrary((struct Library *)CyberGfxBase); CyberGfxBase = NULL;
    if (GfxBase) CloseLibrary((struct Library *)GfxBase); GfxBase = NULL;
    if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase); IntuitionBase = NULL;
}

static void
aros_getmouse (int *x, int *y, int *b)
{
    *x = win->GZZMouseX;
    *y = win->GZZMouseY;
    *b = mousebuttons;
}

static void
aros_clearscreen ()
{
    SetAPen(rp, dri->dri_Pens[SHADOWPEN]);
    RectFill(rp, BORDERLEFT,
    	    	 BORDERTOP,
		 BORDERLEFT + win->GZZWidth - 1,
		 BORDERTOP + win->GZZHeight - 1);
}

static void
aros_mousetype (int type)
{
}

static void
aros_flush (void)
{
}

static char *helptext[] =
{
  "AROS DRIVER VERSION 1.1                ",
  "===========================            ",
  "AROS team &                            ",
  "Stefan Haubenthal                      ",
  "                                       ",
};
#define UGLYTEXTSIZE (sizeof(helptext)/sizeof(char *))
static struct params params[] =
{
  {"-pubscreen", P_STRING, &pubscreen, "Public screen"},
  {NULL, 0, NULL, NULL}
};

struct ui_driver aros_driver =
{
    "aros",
    aros_init,
    aros_getsize,
    aros_processevents,
    aros_getmouse,
    aros_uninitialise,
    aros_set_color,		/*You should implement just one */
    NULL,		    	/*of these and add NULL as second */
    aros_print,
    aros_display,
    aros_alloc_buffers,
    aros_free_buffers,
    aros_flip_buffers,
    aros_clearscreen,
    aros_mousetype,		/*This should be NULL */
    NULL,			/*Driver depended action */
    aros_flush,			/*flush */
    8,				/*text width */
    8,				/*text height */
    helptext,
    UGLYTEXTSIZE,
    params,
    UPDATE_AFTER_RESIZE |
    PALETTE_ROTATION |
    UPDATE_AFTER_PALETTE,    	/*flags...see ui.h */
    0.0, 0.0,			/*width/height of screen in centimeters */
    0, 0,			/*resolution of screen for windowed systems */
    UI_C256,			/*Image type */
    0, 255, 255			/*start, end of palette and maximum allocatable */
				/*entries */
};


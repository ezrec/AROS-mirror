#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <cybergraphx/cybergraphics.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/intuition.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

/***********************************************************************************/

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Library *CyberGfxBase;
struct Screen *scr;
struct Window *win;
struct RastPort *rp;

ULONG cgfx_coltab[256];
UBYTE remaptable[256];
WORD winx = -1, winy = -1;
BYTE Keys[256];
BOOL forcescreen, forcewindow;
BOOL mustremap, truecolor, remapped, wbscreen = TRUE;

/***********************************************************************************/

UWORD pal=2;
ULONG a,c,d,x,y,z;
ULONG s[256];
UBYTE p[65536];
UBYTE *buf, *buf_remapped;

ULONG screenwidth  = 320;
ULONG screenheight = 200;

/***********************************************************************************/

void cleanup(char *msg)
{
    if (msg)
    {
        printf("parallax: %s\n",msg);
    }
    
    if (win) CloseWindow(win);
    
    if (buf) free(buf);
    if (buf_remapped) free(buf_remapped);
    
    if (remapped)
    {
    	WORD i;
	
	for(i = 0; i < 256; i++)
	{
	    ReleasePen(scr->ViewPort.ColorMap, remaptable[i]);
	}
    }

    if (scr)
    {
    	if (wbscreen)
	    UnlockPubScreen(0, scr);
	else
	    CloseScreen(scr);
    }
    
    if (CyberGfxBase) CloseLibrary(CyberGfxBase);
    if (GfxBase) CloseLibrary((struct Library *)GfxBase);
    if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
    
    exit(0);
}

/***********************************************************************************/

#define ARG_TEMPLATE "WINPOSX=X/N/K,WINPOSY=Y/N/K,W=WIDTH/N/K,H=HEIGHT/N/K,FORCESCREEN=SCR/S,FORCEWINDOW=WIN/S"

#define ARG_X 0
#define ARG_Y 1
#define ARG_WIDTH 2
#define ARG_HEIGHT 3
#define ARG_SCR 4
#define ARG_WIN 5
#define NUM_ARGS 6

static IPTR args[NUM_ARGS];

void getarguments(void)
{
    struct RDArgs *myargs;
    
    myargs = ReadArgs(ARG_TEMPLATE, args, NULL);
    if (!myargs)
    {
    	Fault(IoErr(), 0, p, 256);
	cleanup(p);
    }
    
    if (args[ARG_SCR])
    	forcescreen = TRUE;
    else if (args[ARG_WIN])
    	forcewindow = TRUE;

    if (args[ARG_X]) winx = *(IPTR *)args[ARG_X];
    if (args[ARG_Y]) winy = *(IPTR *)args[ARG_Y];
    
    if (args[ARG_WIDTH] || args[ARG_HEIGHT])
    {
    	if (args[ARG_WIDTH]) screenwidth = *(ULONG *)args[ARG_WIDTH];
    	if (args[ARG_HEIGHT]) screenheight = *(ULONG *)args[ARG_HEIGHT];
    }
    else
    {
    	puts("Tip: You can have different window size by using WIDTH and HEIGHT start arguments!");
    }
    
    FreeArgs(myargs);
}

/***********************************************************************************/

void openlibs(void)
{
    if (!(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 39)))
    {
        cleanup("Can't open intuition.library V39!");
    }
    
    if (!(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 39)))
    {
        cleanup("Can't open graphics.library V39!");
    }
    
    if (!(CyberGfxBase = OpenLibrary("cybergraphics.library",0)))
    {
        cleanup("Can't open cybergraphics.library!");
    }
}

/***********************************************************************************/

static void getvisual(void)
{
    if (forcescreen)
		wbscreen = FALSE;

    if (!wbscreen)
    {
        scr = OpenScreenTags(NULL, SA_Width	, W	,
				   SA_Height	, H	,
				   SA_Depth	, 8	,
				   TAG_DONE);
    	if (!scr) cleanup("Failed to open specified screen!");
    }
	else if (!(scr = LockPubScreen(NULL)))
    {
        cleanup("Failed to lock pub screen (workbench)!");
    }
    
    truecolor = (GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH) >= 15) ? TRUE : FALSE;

	if ((!truecolor) && (wbscreen))
		mustremap = TRUE;
}

/***********************************************************************************/

void setuppal(WORD pal)
{
    WORD a;
    
    if (remapped)
    {
    	for(a = 0; a < 256; a++)
	{
	    ReleasePen(scr->ViewPort.ColorMap, remaptable[a]);
	}
    }
    
    for(a = 0; a < 256; a ++)
    {
    	ULONG r, g, b;
	
	r = (pal == 0) * a;
	g = (pal == 1) * a;
	b = (pal == 2) * a;

	if (truecolor)
	{
	  cgfx_coltab[a] = (r<<16) + (g<<8) + (b);
	}
	else if (mustremap)
	{
	  ULONG red   = r * 0x01010101;
	  ULONG green = g * 0x01010101;
	  ULONG blue  = b * 0x01010101;

	  remaptable[a] = ObtainBestPen(scr->ViewPort.ColorMap,
      	    	    	    		red,
					green,
					blue,
					OBP_Precision, PRECISION_IMAGE,
					OBP_FailIfBad, FALSE,
					TAG_DONE);
	  remapped = TRUE;
	}
	else
	{

	  ULONG red   = r * 0x01010101;
	  ULONG green = g * 0x01010101;
	  ULONG blue  = b * 0x01010101;

	  SetRGB32(&scr->ViewPort, a, red, green, blue);
	}
	
    }
}

/***********************************************************************************/

void setupdemo(void)
{
    buf = malloc(screenwidth * screenheight);
    if (!buf) cleanup("Out of memory!");
    
    if (mustremap)
    {
	buf_remapped = malloc(screenwidth * screenheight);
	if (!buf) cleanup("Out of memory!");   
    }
    
    setuppal(pal);
    
    for(a = 0; a < 256; a++)
    {
    	s[a] = (WORD)(32 - sin(3.14159265 * a / 128) * 31);
    }
    
    d = 200;
}

/***********************************************************************************/

void render(void)
{
    static int firsttime = 1;
    static int oldmousex, oldmousey;
    
    if (firsttime)
    {
    	firsttime = 0;
    }
    else
    {
    	c += (win->MouseX - oldmousex);
	d += (win->MouseY - oldmousey);
	
	oldmousex = win->MouseX;
	oldmousey = win->MouseY;
    }
    a = 0;
    c -= 2;
    
    for(y = 0; y < screenheight; y++)
    {
    	for(x = 0; x < screenwidth; x++)
	{
	    buf[a++] = p[((UBYTE)((y - 100) & 255)) * 256 + ((UBYTE)(x & 255))];
	}
    }

    for(y = 0; y < 128; y++)
    {
    	for(x = 0; x < 128; x++)
	{
	    p[y * 256 + x] = p[y * 256 * 2 + x * 2] / 2 + 40;
	}
    }

    for(y = 0; y < 128; y++)
    {
    	memmove(p + y * 256 + 128, p + y * 256, 128);
    }
    
    memmove(p + 128 * 256, p, 32768);
    
    for(y = 0; y < 256; y++)
    {
    	for(x = 0; x < 256; x++)
	{
	    a = (UBYTE)(((s[x] ^ s[y]) & s[(UBYTE)((x ^ y) & 255)]) & 255);
	    if (a > 37) p[((UBYTE)((d + y) & 255)) * 256 + (UBYTE)((c + x) & 255)] = a * 8 - 30;
	}
    }
    
    if (truecolor)
    {
	WriteLUTPixelArray(buf,
			   0,
			   0,
			   screenwidth,
			   rp,
			   cgfx_coltab,
			   win->BorderLeft,
			   win->BorderTop,
			   screenwidth,
			   screenheight,
			   CTABFMT_XRGB8);
    }
    else if (mustremap)
    {
	LONG i;
	UBYTE *src = buf;
	UBYTE *dest = buf_remapped;

	for(i = 0; i < screenwidth * screenheight; i++)
	{
	    *dest++ = remaptable[*src++];
	}
	WriteChunkyPixels(rp,
	    	    	  win->BorderLeft,
			  win->BorderTop,
			  win->BorderLeft + screenwidth - 1,
			  win->BorderTop + screenheight - 1,
			  buf_remapped,
			  screenwidth);

    }
    else
    {
	WriteChunkyPixels(rp,
	    	    	  win->BorderLeft,
			  win->BorderTop,
			  win->BorderLeft + screenwidth - 1,
			  win->BorderTop + screenheight - 1,
			  buf,
			  screenwidth);
    }
    
}

/***********************************************************************************/

void makewin(void)
{
    struct TagItem winonwbtags[] =
    {
    	{WA_DragBar	, TRUE	    	    },
	{WA_DepthGadget	, TRUE	    	    },
	{WA_CloseGadget	, TRUE	    	    },
	{WA_Title	, (IPTR)"Parallax: Use LMB to change palette"},
	{TAG_DONE   	    	    	    }
    };
    
    struct TagItem winonscrtags[] =
    {
    	{WA_Borderless, TRUE },
	{TAG_DONE   	     }
    };

    if (winx == -1) winx = (scr->Width - screenwidth - scr->WBorLeft - scr->WBorRight) / 2;
    if (winy == -1) winy = (scr->Height - screenheight - scr->WBorTop - scr->WBorTop - scr->Font->ta_YSize - 1) / 2;
    
    win = OpenWindowTags(NULL, WA_CustomScreen	, (IPTR)scr,
    			       WA_Left		, winx,
			       WA_Top		, winy,
    			       WA_InnerWidth	, screenwidth,
    			       WA_InnerHeight	, screenheight,
			       WA_AutoAdjust	, TRUE,
	    	    	       WA_Activate	, TRUE,
			       WA_IDCMP		, IDCMP_CLOSEWINDOW |
			       			  IDCMP_RAWKEY |
						  IDCMP_MOUSEBUTTONS ,
			       TAG_MORE     	, wbscreen ? winonwbtags : winonscrtags);
			       
			       
   if (!win) cleanup("Can't open window");

   rp = win->RPort; 
}

/***********************************************************************************/

#define KC_LEFT         0x4F
#define KC_RIGHT     	0x4E
#define KC_UP        	0x4C
#define KC_DOWN      	0x4D
#define KC_ESC       	0x45

/***********************************************************************************/

void getevents(void)
{
    struct IntuiMessage *msg;
    
    while ((msg = (struct IntuiMessage *)GetMsg(win->UserPort)))
    {
        switch(msg->Class)
	{
	    case IDCMP_CLOSEWINDOW:
	        Keys[KC_ESC] = 1;
		break;
		
	    case IDCMP_RAWKEY:
	        {
		    WORD code = msg->Code & ~IECODE_UP_PREFIX;
		    
		    Keys[code] = (code == msg->Code) ? 1 : 0;

		}
	        break;

    	    case IDCMP_MOUSEBUTTONS:
	    	if (msg->Code == SELECTDOWN)
		{
	    	    pal = (pal + 1) % 3;
		    setuppal(pal);
		}
	    	break;
	}
        ReplyMsg((struct Message *)msg);
    }

}

/***********************************************************************************/

int main(int argc, char *argv[])
{
    BOOL done = FALSE;
    
    getarguments();
    openlibs();
    getvisual();
    setupdemo(); 
    makewin();

    while(!done)
    {
	getevents();
	render();	
	if (Keys[KC_ESC])
	{
	    done = TRUE;
	}
	WaitTOF();
    }

    cleanup(0);
    return 0;
}

/***********************************************************************************/


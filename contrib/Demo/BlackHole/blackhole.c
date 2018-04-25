
#include <dos/dos.h>
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
#include <string.h>
#include <memory.h>
#include <aros/debug.h>

/***********************************************************************************/

struct IntuitionBase 	*IntuitionBase;
struct GfxBase 		*GfxBase;
struct Library 		*CyberGfxBase;
struct Screen 		*scr;
struct Window 		*win;
struct RastPort 	*rp;
ULONG 			cgfx_coltab[256];
UBYTE	    	    	remaptable[256];
UBYTE 			Keys[128];
char			s[256];
WORD	    	    	winx = -1, winy = -1;
BOOL	    	    	forcescreen, forcewindow;
BOOL	    	    	mustremap, truecolor, remapped, wbscreen = TRUE;

static void cleanup(char *msg);

/***********************************************************************************/

UWORD x, xx, y;
BOOL retr;
UBYTE ch;
WORD YWert;
UBYTE plas[65536];
WORD XWerte[256];

#define W 320
#define H 200

static UBYTE chunkybuffer[65536];
static UBYTE chunkybuffer2[65536];
static UBYTE chunkybuffer3[65536];
static UBYTE *Zeiger, *Zeiger2;
static WORD offsetbuffer[65536];

static UBYTE chunkybuffer_remapped[W * H];

/***********************************************************************************/

static void Klecks(UWORD x, UWORD y, WORD farbe)
{
    WORD loopx, loopy = 10;
    UWORD offset = y * 256 + x;

    while(loopy--)
    {
    	loopx = 10;
    	while(loopx--)
	{
	    Zeiger[offset++] = farbe;
	}
	offset += 246;
    }
}

/***********************************************************************************/

static void Trash_It_256(void)
{
    UWORD x, y;
    
    if (Zeiger == chunkybuffer)
    {
    	Zeiger  = chunkybuffer2;
	Zeiger2 = chunkybuffer;
    }
    else
    {
    	Zeiger  = chunkybuffer;
	Zeiger2 = chunkybuffer2;
    }
    
    for(y = 1; y < 255; y++)
    {
	for(x = 1; x < 255; x++)
	{
	    WORD col;
	    
	    col =  Zeiger2[y * 256 + x + 1];
	    col += Zeiger2[y * 256 + x - 1];
	    col += Zeiger2[y * 256 + x - 256 - 1];
	    col += Zeiger2[y * 256 + x - 256];
	    col += Zeiger2[y * 256 + x - 256 + 1];
	    col += Zeiger2[y * 256 + x + 256 - 1];
	    col += Zeiger2[y * 256 + x + 256];
	    col += Zeiger2[y * 256 + x + 256 + 1];
	    col /= 8;

	    Zeiger[y * 256 + x] = col;
	}
    }
}


/***********************************************************************************/

static void CalcRain(UBYTE *plas, WORD intensity)
{
    UWORD i, a;
    
    Zeiger = Zeiger2 = chunkybuffer;
    
    for(i = 0; i <= 160; i++)
    {
    	for(a = 0; a <= 2; a++)
	{
	    Klecks(rand() % 246, rand() % 246, intensity); 
	}
	Trash_It_256();
    }
    Trash_It_256();
    memcpy(plas, chunkybuffer, 65536);
    
}

/***********************************************************************************/

#define pi 3.14159265

static void CalcWorm(UBYTE *seg3)
{
    UWORD x, x2, y2, col, peri;
    double r;
    
    r = 26.4;
    
    do
    {
    	r = r + 0.1 * sqrt(sqrt(sqrt(r)));
	peri = (UWORD)(869.0*(r/240.0));
	for (x = 0; x <= peri; x++)
	{
	    y2 = (UWORD)(0.6*(110.5+4000.0/r +
	    	    	 r * cos(x*pi/peri) ));
	    if ((y2 >= 0) && (y2 < 200))
	    {
	    	x2 = (UWORD)(159.5 +
		    	 r * sin(x*pi/peri) );
#if 0
		if ((x2 > 159) && (x2<320))
		{
		    col = ((UWORD)((double)x/peri*512.0)) & 255;
		    ((UWORD *)chunkybuffer2)[(160*y2+(319-x2))] = (256 * ( ((LONG)(2*r)) & 255))+257-col;
		    
		    ((UWORD *)seg3)[(160*y2+x2-160)] = (256 * ( ((LONG)(2*r)) & 255)) + col;
		}
#else
		if ((x2 > 159) && (x2<320))
		{
		    col = ((UWORD)((double)x/peri*512.0)) & 255;
		    offsetbuffer[(320*y2+(319-x2))] = (256 * ( ((LONG)(2*r)) & 255))+257-col;
		    offsetbuffer[(320*y2+x2)] = (256 * ( ((LONG)(2*r)) & 255)) + col;
		}
#endif
	    }
		    
	}

    } while (!(r >= 260.0));
}

/***********************************************************************************/

static void DoWorm(UBYTE *plas, UBYTE *seg3, WORD move)
{
    WORD loopy = 200;
    WORD loopx;
    
    UBYTE *dest = chunkybuffer;
    UWORD dx;
    UWORD off;
#if 0
    UBYTE *seg2 = chunkybuffer2;
    UWORD bx;
    
    bx = 
#endif
    dx = 0;
    
    while(loopy--)
    {
#if 0 
    	loopx = 160;
	while(loopx--)
	{
	    off = ((UWORD *)seg2)[bx++];
	    off += move;
	    *dest++ = plas[off];
	}
    	loopx = 160;
	while(loopx--)
	{
	    off = ((UWORD *)seg3)[dx++];
	    off += move;
	    *dest++ = plas[off];
	}	
#else
    	loopx = 320;
	while(loopx--)
	{
	    off = offsetbuffer[dx++];
	    off += move;
	    *dest++ = plas[off];
	}
#endif
    }
}

/***********************************************************************************/

static void refresh(void)
{
    if (truecolor)
    {
	WriteLUTPixelArray(chunkybuffer,
			   0,
			   0,
			   W,
			   rp,
			   cgfx_coltab,
			   win->BorderLeft,
			   win->BorderTop,
			   W,
			   H,
			   CTABFMT_XRGB8);
    }
    else if (mustremap)
    {
	LONG i;
	UBYTE *src = chunkybuffer;
	UBYTE *dest = chunkybuffer_remapped;

	for(i = 0; i < W * H; i++)
	{
	    *dest++ = remaptable[*src++];
	}
	WriteChunkyPixels(rp,
	    	    	  win->BorderLeft,
			  win->BorderTop,
			  win->BorderLeft + W - 1,
			  win->BorderTop + H - 1,
			  chunkybuffer_remapped,
			  W);

    }
    else
    {
	WriteChunkyPixels(rp,
	    	    	  win->BorderLeft,
			  win->BorderTop,
			  win->BorderLeft + W - 1,
			  win->BorderTop + H - 1,
			  chunkybuffer,
			  W);
    }
}

/***********************************************************************************/

static void initpalette(void)
{
    WORD palindex = 0;
    
    for(palindex = 0; palindex < 64; palindex++)
    {
    	ULONG red   = palindex * 4;
	ULONG green = 0;
	ULONG blue  = 0;

	cgfx_coltab[palindex] = (red << 16) + (green << 8) + blue;

    	red   = (63 - palindex) * 4;
	green = 0;
	blue  = 0;

	cgfx_coltab[palindex + 64] = (red << 16) + (green << 8) + blue;	

    	red   = palindex * 4;
	green = palindex * 4;
	blue  = palindex * 4;

	cgfx_coltab[palindex + 128] = (red << 16) + (green << 8) + blue;	

    	red   = (63 - palindex) * 4;
	green = (63 - palindex) * 4;
	blue  = (63 - palindex) * 4;

	cgfx_coltab[palindex + 192] = (red << 16) + (green << 8) + blue;	
    }

    if (!truecolor) for(palindex = 0; palindex < 256; palindex++)
    {
    	ULONG red   = (cgfx_coltab[palindex] >> 16) & 0xFF;
	ULONG green = (cgfx_coltab[palindex] >> 8) & 0xFF;
	ULONG blue  = cgfx_coltab[palindex] & 0xFF;
	
	if (mustremap)
	{
	    red   *= 0x01010101;
	    green *= 0x01010101;
	    blue  *= 0x01010101;

     	    remaptable[palindex] = ObtainBestPen(scr->ViewPort.ColorMap,
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
	    red   *= 0x01010101;
	    green *= 0x01010101;
	    blue  *= 0x01010101;
	    
      	    SetRGB32(&scr->ViewPort, palindex, red, green, blue);	    
	}
		
    }
}

/***********************************************************************************/

static void initstuff(void)
{    
#if 1
    CalcRain(plas, 255);
#else
{
    ULONG x;
    
    for(x= 0; x < 65535; x++)
    {
    	plas[x] = (x / 64);
    }
}
#endif
    CalcWorm(chunkybuffer3);
}

/***********************************************************************************/

static void ticker(void)
{
    x++;
    DoWorm(plas, chunkybuffer3, x * 510);
}

/***********************************************************************************/

static void cleanup(char *msg)
{
    
    if (msg)
    {
        printf("blackhole: %s\n",msg);
    }
    
    if (win) CloseWindow(win);
    
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

#define ARG_TEMPLATE "WINPOSX=X/N/K,WINPOSY=Y/N/K,FORCESCREEN=SCR/S,FORCEWINDOW=WIN/S"
#define ARG_X 0
#define ARG_Y 1
#define ARG_SCR 2
#define ARG_WIN 3
#define NUM_ARGS 4

static IPTR args[NUM_ARGS];

static void getarguments(void)
{
    struct RDArgs *myargs;
    
    if ((myargs = ReadArgs(ARG_TEMPLATE, args, NULL)))
    {
    	if (args[ARG_SCR])
	    forcescreen = TRUE;
	else if (args[ARG_WIN])
	    forcewindow = TRUE;
	    
	if (args[ARG_X]) winx = *(IPTR *)args[ARG_X];
	if (args[ARG_Y]) winy = *(IPTR *)args[ARG_Y];
	
    	FreeArgs(myargs);
    }
}

/***********************************************************************************/

static void openlibs(void)
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
    	UnlockPubScreen(NULL, scr);
        wbscreen = FALSE;
	
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

static void makewin(void)
{
    struct TagItem winonwbtags[] =
    {
    	{WA_DragBar	, TRUE	    	    },
	{WA_DepthGadget	, TRUE	    	    },
	{WA_CloseGadget	, TRUE	    	    },
	{WA_Title	, (IPTR)"BlackHole" },
	{TAG_DONE   	    	    	    }
    };
    
    struct TagItem winonscrtags[] =
    {
    	{WA_Borderless, TRUE },
	{TAG_DONE   	     }
    };
    
    if (winx == -1) winx = (scr->Width - W - scr->WBorLeft - scr->WBorRight) / 2;
    if (winy == -1) winy = (scr->Height - H - scr->WBorTop - scr->WBorTop - scr->Font->ta_YSize - 1) / 2;
    
    win = OpenWindowTags(NULL, WA_CustomScreen	, (IPTR)scr,
    			       WA_Left		, winx,
			       WA_Top		, winy,
    			       WA_InnerWidth	, W,
    			       WA_InnerHeight	, H,
			       WA_AutoAdjust	, TRUE,
			       WA_Activate	, TRUE,
			       WA_IDCMP		, IDCMP_CLOSEWINDOW |
			       			  IDCMP_RAWKEY,
			       TAG_MORE, wbscreen ? winonwbtags : winonscrtags);
			       
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

static void getevents(void)
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

	}
        ReplyMsg((struct Message *)msg);
    }

}

/***********************************************************************************/

static void action(void)
{
    initpalette();
    initstuff();
    
    while (!Keys[KC_ESC])
    {
        getevents();

    	ticker();
        refresh();
    	WaitTOF();
    }

}

/***********************************************************************************/

int main(void)
{
    getarguments();
    openlibs();
    getvisual();
    makewin();
    action();
    cleanup(0);
    
    return 0;
}

/***********************************************************************************/


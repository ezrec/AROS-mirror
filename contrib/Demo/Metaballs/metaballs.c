
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

#define balls 20

static WORD x, y, i;

static struct
{
    APTR p;
    WORD dia;
    BYTE rand;
} ball[balls];

#define W  320   // Full width
#define hW 160   // Half width
#define qW 80    // Quarter width
#define eW 40    // 1/8 width
#define H  200   // Full height
#define hH 100   // Half height
#define qH  50   // Quarter height
#define eH  25   // 1/8 height

static UBYTE chunkybuffer[W * H];
static UBYTE chunkybuffer_remapped[W * H];

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

#define SQR(x) ((x) * (x))
#define pi 3.14159265
#define deg 0.017453

static void Calc_Ball(WORD hohe, WORD breite, APTR po)
{
    WORD x, y, col;
    float e;
    
    for(y = 0; y < hohe; y++)
    {
    	for(x = 0; x < breite; x++)
	{
	    e = sqrt(SQR(x-(breite+1)/2) + 1.5*SQR(y-(breite+1)/2));
	    col = (WORD)(130.0+130.0*cos(e*pi/(hohe/2)));
	    if (col > 255) col=255;
	    if (e < (hohe / 2))
	    {
	    	((UBYTE *)po)[breite * y + x] = col;
	    }
	    else
	    {
	    	((UBYTE *)po)[breite * y + x] = 0;
	    }
	}
    }
    
}

/***********************************************************************************/

static void Do_Ball(WORD x, WORD y, WORD h, WORD b, APTR po)
{
    WORD loopy, loopx;
    UWORD destoffset = (y * W) + x;
    UWORD col;
    UBYTE *src = (UBYTE *)po;
    
    for(loopy = 0; loopy < h; loopy++)
    {
    	for(loopx = 0; loopx < b; loopx++)
	{
	    col = *src++;
	    col += chunkybuffer[destoffset];
	    if (col > 255) col = 255;
	    chunkybuffer[destoffset++] = col;
	}
	destoffset += (W - b);
    }
}

/***********************************************************************************/

static void initpalette(void)
{
    WORD palindex = 0;
    
    for(palindex = 0; palindex < 64; palindex++)
    {
    	ULONG red   = 0;
	ULONG green = 0;
	ULONG blue = palindex * 4;

	cgfx_coltab[palindex] = (red << 16) + (green << 8) + blue;

    	red = palindex * 4;
	green = palindex * 4;
	blue = 0xFF;

	cgfx_coltab[palindex + 64] = (red << 16) + (green << 8) + blue;	

    	red = 0xFF - palindex * 4;
	green = 0xFF - palindex * 4;
	blue = 0xFF - palindex * 4;

	cgfx_coltab[palindex + 128] = (red << 16) + (green << 8) + blue;	

    	red = palindex * 4;
	green = palindex * 4;
	blue = palindex * 4;

	cgfx_coltab[palindex + 192] = (red << 16) + (green << 8) + blue;	
    }
    
    cgfx_coltab[255] = 0xFFFFFF;    

    if (!truecolor)
    {
    	WORD i;
	
	for(i = 0; i < 256; i++)
	{
	    ULONG r = (cgfx_coltab[i] >> 16) & 0xFF;
	    ULONG g = (cgfx_coltab[i] >> 8) & 0xFF;
	    ULONG b = cgfx_coltab[i] & 0xFF;

    	    if (mustremap)
	    {
	      ULONG red   = r * 0x01010101;
	      ULONG green = g * 0x01010101;
	      ULONG blue  = b * 0x01010101;

	      remaptable[i] = ObtainBestPen(scr->ViewPort.ColorMap,
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

	      SetRGB32(&scr->ViewPort, i, red, green, blue);
	    }	    
	}
    }    
}

/***********************************************************************************/

static void initstuff(void)
{    
    int j=1;

    for(i = 1; i <= balls; i++)
    {
        if(i&0x1)
         j=i;
        else
         j=balls-i;

    	ball[i - 1].dia = 30 + abs(5 * (j-1));
	ball[i - 1].rand = (BYTE)(20.0+20.0*sin(i*pi/balls*13));
	ball[i - 1].p = malloc(SQR(ball[i - 1].dia));
	if (ball[i].p) cleanup("out of memory!");
	Calc_Ball(ball[i-1].dia, ball[i-1].dia, ball[i-1].p);
    }
}

/***********************************************************************************/

static void ticker(void)
{
    // Movement parameters
    // The chain consists of four sin/cos waves per axis
    // *dis* represents distance between blobs in the chain
    // *add* represents the movement speed of the chain/waves
    //
    // Implemented by using local floats to make it easier
    // to later implement CLI parameter values.

    float xdis1 = -45.0;
    float xdis2 =  65.0;
    float xdis3 =  12.0;
    float xdis4 = -25.0;

    float ydis1 =  35.0;
    float ydis2 = -55.0;
    float ydis3 = -15.0;
    float ydis4 = -05.0;

    float xadd1 =  2.0;
    float xadd2 =  1.25;
    float xadd3 = -0.55;
    float xadd4 =  0.93;

    float yadd1 =  1.55;
    float yadd2 =  0.63;
    float yadd3 =  0.23;
    float yadd4 = -0.94;

    float xpos1, ypos1;
    float xpos2, ypos2;
    float xpos3, ypos3;
    float xpos4, ypos4;

    float xnew = 0, ynew = 0;

    memset(chunkybuffer, 0, sizeof(chunkybuffer));

    // Precalculating position of each wave
    xpos1 = x * xadd1;
    xpos2 = x * xadd2;
    xpos3 = x * xadd3;
    xpos4 = x * xadd4;

    ypos1 = x * yadd1;
    ypos2 = x * yadd2;
    ypos3 = x * yadd3;
    ypos4 = x * yadd4;
    
    for(i = 0; i < balls; i++)
    {

        xnew = hW - ball[i].dia/2 +
               qW * sin ((xdis1 * i + xpos1) * deg)+
               qW * sin ((xdis2 * i + xpos2) * deg)+
               qW * sin ((xdis3 * i + xpos3) * deg)+
               qW * sin ((xdis4 * i + xpos4) * deg);

        ynew = hW - ball[i].dia/2 +
               qH * cos ((ydis1 * i + ypos1) * deg)+
               qH * cos ((ydis2 * i + ypos2) * deg)+
               qH * cos ((ydis3 * i + ypos3) * deg)+
               qH * cos ((ydis4 * i + ypos4) * deg);

        // replace qW/qH with eW/eH if you wish to keep
        // the blobs from moving entirely over the edges.

        Do_Ball((WORD)xnew,(WORD)ynew,
	 ball[i].dia, ball[i].dia, ball[i].p);    

    }
    x++;
}

/***********************************************************************************/

static void cleanup(char *msg)
{
    
    if (msg)
    {
        printf("metaballs: %s\n",msg);
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
    if (!(scr = LockPubScreen(NULL)))
    {
        cleanup("Can't lock pub screen!");
    }
    
    if (GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH) <= 8)
    {
    	if (!forcewindow)
	{
	    wbscreen = FALSE;
	}
	else
	{
	    mustremap = TRUE;
	}
    }
    
    if (forcescreen) wbscreen = FALSE;
    
    if (!wbscreen)
    {
    	UnlockPubScreen(NULL, scr);
        wbscreen = FALSE;
	
        scr = OpenScreenTags(NULL, SA_Width	, W	,
				   SA_Height	, H	,
				   SA_Depth	, 8	,
				   TAG_DONE);
    	if (!scr) cleanup("Can't open screen!");
    }
    
    truecolor = (GetBitMapAttr(scr->RastPort.BitMap, BMA_DEPTH) >= 15) ? TRUE : FALSE;
}

/***********************************************************************************/

static void makewin(void)
{
    struct TagItem winonwbtags[] =
    {
    	{WA_DragBar	, TRUE	    	    },
	{WA_DepthGadget	, TRUE	    	    },
	{WA_CloseGadget	, TRUE	    	    },
	{WA_Title	, (IPTR)"Metaballs" },
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


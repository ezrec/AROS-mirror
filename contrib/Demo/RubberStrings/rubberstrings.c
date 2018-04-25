
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

#define balls 2
#define radius 5
#define speed 3

static WORD x, i;

static struct
{
    APTR p;
    WORD dia;
    BYTE rand;
} ball[balls];

#define W  256   // Full width
#define hW 128   // Half width
#define qW  64   // Quarter width
#define eW  32   // 1/8 width
#define H  256   // Full height
#define hH 128   // Half height
#define qH  64   // Quarter height
#define eH  32   // 1/8 height

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

            col = 128 + 128*cos(radius*e*(1+(1.2*W-e)/(W))*deg);

	    if (col > 255) col=255;
	    ((UBYTE *)po)[breite * y + x] = col;

	}
    }
    
}

/***********************************************************************************/

static void Do_Ball(UWORD x, UWORD y, WORD h, WORD b, APTR po)
{
    UWORD loopy, loopx;
    ULONG destoffset = 0; // (y * W) + x;
    ULONG srcoffset = 0;
    UWORD col,col2;
    UWORD dx = 0, dy = 0;
    UWORD bW = b;
    UWORD bH = h;
    UWORD sx = x+W, sy = y+H;

    if(sx>bW) sx=bW;
    if(sy>bH) sy=bH;

#define src ((UBYTE *)po)

    for(loopy = y; loopy < sy; loopy ++)
    {
     dx=0;
     for(loopx = x; loopx < sx; loopx ++)
     {
      destoffset = (W*dy   ) + dx;
      srcoffset  = (bW*loopy) + loopx;

      col  = src[srcoffset];
      col2 = chunkybuffer[destoffset];

      //if(col2>col) col = col2; // eggplant

      //col += col2; // metaball

      col = (col+col2)>>1; // Glassy

      //col+=ctr; // colorcycle (add ctr++ below);

      //col = col & 0xff; // Xor-effect
      //if (col > 255) col = 255; // metaball max color
      chunkybuffer[destoffset] = col;
      dx++;
     }
     dy++;
    }

   // ctr++;

}

/***********************************************************************************/

static void initpalette(void)
{
    WORD palindex = 0;
    
    for(palindex = 0; palindex < 128; palindex++)
    {
    	ULONG re = palindex;
	ULONG gr = palindex;
	ULONG bl = palindex*2;
	cgfx_coltab[palindex] = (re << 16) + (gr << 8) + bl;	

    	re = 128-palindex;
	gr = 128-palindex;
	bl = 255-palindex*2;
	cgfx_coltab[palindex+128] = (re << 16) + (gr << 8) + bl;	
    }
    
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
    for(i = 1; i <= balls; i++)
    {
    	ball[i - 1].dia = 2*W;
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

    float xdis1 =  394   /balls;
    float xdis2 = -363   /balls;
    float xdis3 =  336   /balls;
    float xdis4 = -344   /balls;

    float ydis1 =  354   /balls;
    float ydis2 = -324   /balls;
    float ydis3 =  373   /balls;
    float ydis4 =  382   /balls;

    float xadd1 =  2.10  *speed;
    float xadd2 = -1.25  *speed;
    float xadd3 = -0.55  *speed;
    float xadd4 =  0.93  *speed;

    float yadd1 =  1.55  *speed;
    float yadd2 =  0.63  *speed;
    float yadd3 = -0.23  *speed;
    float yadd4 = -0.94  *speed;

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

        xnew = hW + // - ball[i].dia/2 +
               eW * sin ((xdis1 * i + xpos1) * deg)+
               eW * sin ((xdis2 * i + xpos2) * deg)+
               eW * sin ((xdis3 * i + xpos3) * deg)+
               eW * sin ((xdis4 * i + xpos4) * deg);

        ynew = hW + // - ball[i].dia/2 +
               eH * cos ((ydis1 * i + ypos1) * deg)+
               eH * cos ((ydis2 * i + ypos2) * deg)+
               eH * cos ((ydis3 * i + ypos3) * deg)+
               eH * cos ((ydis4 * i + ypos4) * deg);

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
        printf("rubberstrings: %s\n",msg);
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
	{WA_Title	, (IPTR)"Rubberstrings 1.0 - by Orgin" },
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


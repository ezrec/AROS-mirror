
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <cybergraphx/cybergraphics.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/intuition.h>
#include <aros/machine.h>

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
UBYTE 			Keys[128];
char			s[256];

static void cleanup(char *msg);

/***********************************************************************************/

#define RANDOM		((WORD)(rand() & 0xFFFF))

#define W            	320
#define H            	200
#define PARTICLES       4096

struct Particle
{
    WORD speed;
    WORD counter;
    WORD ypos;
    WORD xpos;
    UBYTE particlecolor;
};

#define yspeed speed
#define xspeed counter

static UBYTE chunkybufferarray[W * (H + 2)];
static UBYTE *chunkybuffer = &chunkybufferarray[W];

struct Particle particles[PARTICLES];

/***********************************************************************************/

static void refresh(void)
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

/***********************************************************************************/

static void initpalette(void)
{
    static UBYTE pal[256*3];
    
    ULONG ecx = 1024;
    ULONG eax = 0xA000;
    ULONG temp;
    ULONG palindex = 0;
    
    for(ecx = 1024; ecx; ecx--)
    {
    	eax = (eax >> 8) | (eax & 0xFF) << 24;
	if ((eax & 255) >= 63)
	{
	    eax &= ~0xFF; eax |= 63;
	    temp = ((eax & 0xFF00) + 0x400) & 0xFF00;
	    eax &= ~0xFF00; eax |= temp;
	}
	
	if (ecx & 3)
	{
	    pal[palindex++] = (UBYTE)eax;
	}	
    }
    
    for(palindex = 0; palindex < 256; palindex++)
    {
    	ULONG red   = pal[palindex*3] * 4;
	ULONG green = pal[palindex*3+1] * 4;
	ULONG blue  = pal[palindex*3+2] * 4;

	cgfx_coltab[palindex] = (red << 16) + (green << 8) + blue;

    }
}


/***********************************************************************************/

static void ticker(void)
{
    static ULONG tick;
    static ULONG ebx;
    static ULONG si = 0x100;
    
    WORD x, y;
    ULONG ecx;
    
    tick++;
    
    if (!(tick & 3))
    {
    	/* New explosion */
	
	x = RANDOM / 2;
	y = RANDOM / 2;
	
    	for(ecx = 256; ecx; ecx--)
	{
	    struct Particle *p;
	    float f;
	    WORD xspeed;
	    WORD yspeed;
	    
	    p = &particles[ebx];
	    ebx = (ebx + 1) & (PARTICLES - 1);
	    
	    p->speed = RANDOM >> 6;
	    p->counter = ecx;
	    
	    #warning: endianess
	    
	    f = (float)(*(ULONG *)(&p->speed));
	    
	    p->ypos = y;
	    
	    xspeed = sin(f) * p->speed;
	    yspeed = cos(f) * p->speed;
	    
	    p->xpos = x;

	    p->particlecolor = 58;
	    p->yspeed = ((UWORD)yspeed) * 2;
	    p->xspeed = xspeed;
	    	    	    
	}	
    }
    
    /* Draw/move all particles */
    
    for(ecx = PARTICLES; ecx; ecx--)
    {
    	struct Particle *p = &particles[si];
	WORD xp, yp;
	
	p->yspeed += 16;
    	p->ypos += p->yspeed; p->yspeed -= (p->yspeed / 16);
	p->xpos += p->xspeed; p->xspeed -= (p->xspeed / 16);
	
	yp = p->ypos / 256;
	xp = p->xpos / 128;

	if ((yp >= -99) && (yp <= 99) && (xp >= -159) && (xp <= 159))
	{
	    ULONG dest = (yp + 100) * W + xp + 160;
	    
	    UBYTE col = p->particlecolor--;
	    
	    if (col > chunkybuffer[dest])
	    {
	    	chunkybuffer[dest] = col;
	    }
	}
	
	si = (si + 1) & (PARTICLES - 1);
    }
    
    /* blur */
    
    for(ecx = 0; ecx < W * H; ecx++)
    {
    	UBYTE col = chunkybuffer[ecx];
	
	col += chunkybuffer[ecx + 1];
	col += chunkybuffer[ecx - W];
	col += chunkybuffer[ecx + W];
	col /= 4;
	
	if ((col > 0) && (tick & 1))
	{
	    col--;
	}
	
	chunkybuffer[ecx] = col;
    }
}

/***********************************************************************************/

static void cleanup(char *msg)
{
    
    if (msg)
    {
        printf("Firework: %s\n",msg);
    }
    
    if (win) CloseWindow(win);
    
    if (scr) UnlockPubScreen(0, scr);
    
    if (CyberGfxBase) CloseLibrary(CyberGfxBase);
    if (GfxBase) CloseLibrary((struct Library *)GfxBase);
    if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
    
    exit(0);
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
        cleanup("Need hi or true color screen!");
    }
}

/***********************************************************************************/

static void makewin(void)
{
    win = OpenWindowTags(NULL, WA_CustomScreen	, (IPTR)scr,
    			       WA_Left		, (scr->Width - W - scr->WBorLeft - scr->WBorRight) / 2,
			       WA_Top		, (scr->Height - H - scr->WBorTop - scr->WBorTop - scr->Font->ta_YSize - 1) / 2,
    			       WA_InnerWidth	, W,
    			       WA_InnerHeight	, H,
			       WA_AutoAdjust	, TRUE,
			       WA_Title		, (IPTR)"Firework",
			       WA_DragBar	, TRUE,
			       WA_DepthGadget	, TRUE,
			       WA_CloseGadget	, TRUE,
			       WA_Activate	, TRUE,
			       WA_IDCMP		, IDCMP_CLOSEWINDOW |
			       			  IDCMP_RAWKEY,
			       TAG_DONE);
			       
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
    openlibs();
    getvisual();
    makewin();
    action();
    cleanup(0);
    
    return 0;
}

/***********************************************************************************/



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

struct IntuitionBase    *IntuitionBase;
struct GfxBase 	        *GfxBase;
struct Library 	        *CyberGfxBase;
struct Screen           *scr;
struct Window           *win;
struct RastPort         *rp;

ULONG                   cgfx_coltab[256];
UBYTE                   remaptable[256];
UBYTE                   Keys[128];
char                    s[256];
WORD                    winx = -1, winy = -1;
BOOL                    forcescreen, forcewindow;
BOOL                    mustremap, truecolor, remapped, wbscreen = TRUE;

/***********************************************************************************/

#define RANDOM          ((WORD)(rand() & 0xFFFF))

#define W               320
#define H               200
#define PARTICLES       4096

struct Particle
{
    WORD speed;
    WORD counter;
    WORD ypos;
    WORD xpos;
    UBYTE particlecolor;
};

#define xspeed counter
#define yspeed speed

static UBYTE chunkybufferarray[W * (H + 2)];
static UBYTE *chunkybuffer = &chunkybufferarray[W];

static UBYTE chunkybuffer_remapped[W * H];

struct Particle particles[PARTICLES];

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

        if (truecolor)
        {
            cgfx_coltab[palindex] = (red << 16) + (green << 8) + blue;
        }
        else if (mustremap)
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

    	    //f= (float)(*(ULONG *)(&p->speed));
    	    f = (float)((ULONG)p->speed + (ULONG)p->counter * 65536);

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
        if (ecx >= W)
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
        {WA_DragBar, TRUE           },
        {WA_DepthGadget, TRUE       },
        {WA_CloseGadget, TRUE       },
        {WA_Title, (IPTR)"Firework" },
        {TAG_DONE                   }
    };

    struct TagItem winonscrtags[] =
    {
        {WA_Borderless, TRUE },
        {TAG_DONE            }
    };

    if (winx == -1) winx = (scr->Width - W - scr->WBorLeft - scr->WBorRight) / 2;
    if (winy == -1) winy = (scr->Height - H - scr->WBorTop - scr->WBorTop - scr->Font->ta_YSize - 1) / 2;

    win = OpenWindowTags(NULL, WA_CustomScreen, (IPTR)scr,
                        WA_Left, winx,
                        WA_Top, winy,
                        WA_InnerWidth, W,
                        WA_InnerHeight, H,
                        WA_AutoAdjust, TRUE,
                        WA_Activate, TRUE,
                        WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_RAWKEY,
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

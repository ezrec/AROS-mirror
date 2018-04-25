
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

#define random		rand

#define W            	320
#define H            	200

/***********************************************************************************/

unsigned char * buffy256, *buffy256_remapped;
unsigned int pal[256];

struct particle {
  int X0,Y0;
  int V0x,V0y;
  int power;
  int time;
  struct particle *next;
};

typedef struct particle particle;

particle * firstparticle;

void initdawafire(void)
{
  int i;

  /* 8bpp buffer */
  
  for ( i=0 ; i<256 ; i++ ) {  
    
    int r,g,b;

    r = g = b = 0 ;
    
    if ( (i > 7) && (i < 32) )
      r = 10 * ( i - 7 );
    if ( i > 31 )
      r = 255;
      
    if ( (i > 32 ) && (i < 57 ) )
      g = 10 * ( i - 32 );
    if ( i > 56 )
      g = 255;
    
    if ( i < 8 )
      b = 8 * i;
    if ( (i > 7) && (i < 17) )
      b = 8 * ( 16 - i );
    if ( (i > 57) && (i < 82) )
      b = 10 * ( i - 57 );
    if ( i > 81 )
      b = 255;

    if (truecolor)
    {
      cgfx_coltab[i] = (r<<16) + (g<<8) + (b);
    }
    else if (mustremap)
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

  buffy256 = (unsigned char *) calloc ( W*H , 1 );
  if (!buffy256) cleanup("Out of memory!");

  if (mustremap)
  {
    buffy256_remapped = (unsigned char *) calloc ( W*H , 1 );
    if (!buffy256_remapped) cleanup("Out of memory!");
  }
  
}

void refresh () {
  
  /* B G R X */

  static double k=0;
  int i;

  unsigned char * blur = buffy256;

  particle * precurrentparticle;
  particle * currentparticle;

  /* Add new particles */

  for (i=0; i<8; i++) {

    int ii;

    for (ii=0; ii<8; ii++) {
      
      precurrentparticle = (particle *) malloc (sizeof(particle));
      
      precurrentparticle->V0x = (random() & 31) - 16;
      precurrentparticle->V0y = (random() & 31);
      
      precurrentparticle->X0 = W/2+
	W/2.5*
	sin((20*sin(k/20))+i*70)*
	(sin(10+k/(10+i))+0.2)*
	cos((k+i*25)/10);
      precurrentparticle->Y0 = H/2+
	H/2.5*
	cos((20*sin(k/(20+i)))+i*70)*
	(sin(10+k/10)+0.2)*
	cos((k+i*25)/10);
      
      precurrentparticle->power = 110+50*sin(k/3);
      precurrentparticle->time = 0;
      precurrentparticle->next = firstparticle;
      
      firstparticle = precurrentparticle;
      
    }
    
    k += 0.02;
    
  }
  
  /*
   *  Physics:
   *
   *  Vx(t) = V0x
   *  Vy(t) = V0y - G.t
   *
   *  X(t) = X0 + V0x.t
   *  Y(t) = Y0 + V0y.t - G.t^2
   *
   */

  currentparticle = firstparticle->next;
  
  while (currentparticle != NULL) {
   
    int dawa;
    int tx,ty;
    int time;

    time = currentparticle->time / 16;

    tx =
      (currentparticle->X0) +
      time*(currentparticle->V0x);
    ty =
      (currentparticle->Y0) +
      time*(currentparticle->V0y) -
      time*time*2;
    
    ty = H-ty;

    if ((tx<W) && (ty>3) && (ty<H)) {
      dawa = buffy256 [ty*W+tx];
      dawa += currentparticle->power;
      if (dawa>255)
	dawa = 255;
      buffy256 [ty*W+tx] = dawa;
    }
    
    currentparticle->time += 1;

    currentparticle->power -= ((currentparticle->power >> 4) + 1);

    if (currentparticle->power <= 1) {
      precurrentparticle->next = currentparticle->next;
      free (currentparticle);
    } else
      precurrentparticle = currentparticle;
    
    currentparticle = precurrentparticle->next;

  }  

  /*
   * Blur :
   *
   *  &&&
   *  &@&
   *   &
   */

  blur += 2*W;
  
  for (i=0; i<W*(H-3); i++)
  {
    * (blur) =
        ( * (blur-W-1)     +
          * (blur-W)       +
          * (blur-W+1)     +
          * (blur-1)       +
          * (blur)         +
          ( (*blur) >> 1 ) +
          ( (*blur) >> 2 ) +
          * (blur+1)       +
          * (blur+W) ) >> 3;

     blur++;
  }
    if (truecolor)
    {
	WriteLUTPixelArray(buffy256,
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
	UBYTE *src = buffy256;
	UBYTE *dest = buffy256_remapped;

	for(i = 0; i < W * H; i++)
	{
	    *dest++ = remaptable[*src++];
	}
	WriteChunkyPixels(rp,
	    	    	  win->BorderLeft,
			  win->BorderTop,
			  win->BorderLeft + W - 1,
			  win->BorderTop + H - 1,
			  buffy256_remapped,
			  W);

    }
    else
    {
	WriteChunkyPixels(rp,
	    	    	  win->BorderLeft,
			  win->BorderTop,
			  win->BorderLeft + W - 1,
			  win->BorderTop + H - 1,
			  buffy256,
			  W);
    }
	
}

/***********************************************************************************/

static void cleanup(char *msg)
{
    
    if (msg)
    {
        printf("DawaFire: %s\n",msg);
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
	{WA_Title	, (IPTR)"DawaFire"  },
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
    initdawafire();
    
    /* Init Particle */

    firstparticle = NULL;

    /* Init FPS */

    //init_fps ();

    while (!Keys[KC_ESC])
    {
        getevents();

        refresh();
	
	WaitTOF();
	//next_fps();
    }

    /* Aff FPS */

    //aff_fps (NAME);

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


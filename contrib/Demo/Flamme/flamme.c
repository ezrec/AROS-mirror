
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

/***********************************************************************************/

#define W            	320
#define H            	200

/***********************************************************************************/

static unsigned char *scr1;
static unsigned char *scr2;


void refresh () {

  int bcl,tmp;
  unsigned char *src;
  unsigned char *dst;
  unsigned char *swp;

  for ( bcl=0 ; bcl<3*W ; bcl++ )
    *(scr2+W*H+bcl) = 56 + rand()%40;
  
  tmp = 30 + rand()%40;
  for ( bcl=0 ; bcl<tmp ; bcl++ )
    {
      dst = scr2 + W*(H+1) + rand()%(W-3);

      *dst =
	*(dst+1) =
	*(dst+2) =
	*(dst+W) =
	*(dst+W+1) =
	*(dst+W+2) =
	*(dst+2*W+1) =
	*(dst+2*W+2) =
	*(dst+2*W+3) = 149;
      }

  src = scr2 + 2*W;
  dst = scr1 + W;

  for ( bcl=0 ; bcl<W*(H+2)-2 ; bcl++ )
    {

      tmp =   *(src+W)
            + *(src+2*W-1)
	    + *(src+2*W)
	    + *(src+2*W+1);
     
      tmp >>= 2;
      
      if (tmp != 0)
	*dst++ = tmp-1;
      else
	*dst++ = 0;

      src++;
	
    }

  swp = scr1;
  scr1 = scr2;
  scr2 = swp;

    WriteLUTPixelArray(scr1,
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

void init_colormap() {

  int i;
  int r,g,b;

  for ( i=0 ; i<256 ; i++ )    
    {  
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

     cgfx_coltab[i] = (r<<16) + (g<<8) + (b);

    }

}

/***********************************************************************************/

static void cleanup(char *msg)
{
    
    if (msg)
    {
        printf("Flamme: %s\n",msg);
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
			       WA_Title		, (IPTR)"Flamme",
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
    /* Init Scr */

    scr1 = (unsigned char *) calloc ( W*(H+4) , 1 );
    scr2 = (unsigned char *) calloc ( W*(H+4) , 1 );

    if (!scr1 || !scr2) cleanup("Out of memory!");

    /* Init FPS */

    //init_fps ();

    /* Boucle */

    init_colormap();

    while (!Keys[KC_ESC])
    {
	getevents();

	refresh();
	//next_fps();

    }

    /* Aff FPS */

    //aff_fps (NAME);
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


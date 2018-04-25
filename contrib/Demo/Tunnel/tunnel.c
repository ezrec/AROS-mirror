
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <cybergraphx/cybergraphics.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/intuition.h>
#include <aros/macros.h>

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

#define W            	256
#define H            	256

#define RADIUS 64
#define DIST 256

void refresh ();
void init_tab ();

unsigned char angle [W*H];
unsigned char profondeur [W*H];

unsigned char *texture, *buffer, *buffer_remapped;

void refresh ();
void init_tab ();

/***********************************************************************************/

void inittunnel(void)
{
  int x,y;
  FILE * fichier;
  char * c;
  
  /* Chargement de la texture */

  c = (char *) malloc ( 128*129 );
  buffer = (char *)malloc (W*(H+1));
  if (!c || !buffer)
    cleanup("Out of memory!");

  if (mustremap)
  {
    buffer_remapped = malloc(W * H);
    if (!buffer_remapped) cleanup("Out of memory!");
  }
    
  fichier = fopen ( "pattern.data" , "rb" );
  if (!fichier)
    cleanup("Can't open pattern.data file!");
  
  if (fread ( buffer , 1, 128*128 , fichier ) != 128 * 128)
    cleanup("Error reading pattern.data file!");
  fclose ( fichier );
    
  /* Open palette */

  fichier = fopen ( "pattern.pal" , "rb" );
  if (!fichier)
  {
    fclose(fichier);
    cleanup("Can't open pattern.pal file!");
  }
  
  if (fread ( cgfx_coltab , 1, 256 * 4 , fichier ) != 256 * 4)
  {
    fclose(fichier);
    cleanup("Error reading pattern.pal file!");
  }
  
  fclose ( fichier );

  /* Fix palette endianess */
  
#if !AROS_BIG_ENDIAN
  {
      int i;
      
      for(i = 0; i < 256; i ++)
      {
          cgfx_coltab[i] = AROS_LONG2BE(cgfx_coltab[i]);
      }
  }
#endif

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
  
  /* Rotate texture */
  
  for(x = 0; x < 127; x++)
  {
    for(y = 0;y < 127; y++)
    {
      c[y * 128 + x] = buffer[x * 128 + y];
    }
  }
  
  texture = (unsigned char *) c;
  
  init_tab();

}

void refresh () {
  
  static double turn = 0;
  unsigned char depX,depY;
  unsigned char * a = angle;
  unsigned char * p = profondeur;
  unsigned char x,y;
  unsigned char * tmp = (unsigned char *)buffer;
  int i,j;

  depX = W/2 * ( 1 + sin ( turn ) );
  depY = W/2 * ( 1 + cos ( turn ) );
  turn += 3.14/100;

  for ( j=0 ; j<H ; j++ )
    for ( i=0 ; i<W ; i++ )
      {
	x = (( *(a++) + depX ) % 256) >> 1;
	y = (( *(p++) + depY ) % 256) >> 1;
	* ( tmp++ ) = * ( texture + (y<<7) + x );
      }

  if (truecolor)
  {
    WriteLUTPixelArray(buffer,
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
    UBYTE *src = buffer;
    UBYTE *dest = buffer_remapped;

    for(i = 0; i < W * H; i++)
    {
      *dest++ = remaptable[*src++];
    }
    WriteChunkyPixels(rp,
	    	    	  win->BorderLeft,
			  win->BorderTop,
			  win->BorderLeft + W - 1,
			  win->BorderTop + H - 1,
			  buffer_remapped,
			  W);

  }
  else
  {
    WriteChunkyPixels(rp,
	    	    	  win->BorderLeft,
			  win->BorderTop,
			  win->BorderLeft + W - 1,
			  win->BorderTop + H - 1,
			  buffer,
			  W);
  }

}

void init_tab () {

  int i,j;
  unsigned char * a = angle;
  unsigned char * p = profondeur;

  for ( j=0 ; j<H ; j++ )
    for ( i=0 ; i<W ; i++ )
      {
	* ( a++ ) = (int)( 128 + atan2(i-W/2,j-H/2)*255/6.24 );
	* ( p++ ) = (RADIUS*DIST) / sqrt((i-W/2)*(i-W/2)+(j-H/2)*(j-H/2)); 
      }
	
}

/***********************************************************************************/

static void cleanup(char *msg)
{
    
    if (msg)
    {
        printf("Tunnel: %s\n",msg);
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
	{WA_Title	, (IPTR)"Tunnel"    },
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
  inittunnel();
  
  /* Init FPS */

  //init_fps ();
  
  /* Boucle */

  while (!Keys[KC_ESC])
  {
      getevents();

    refresh ();
    //update_x ();
    //next_fps ();
    WaitTOF();
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


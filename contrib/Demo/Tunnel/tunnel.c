
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
UBYTE 			Keys[128];
char			s[256];

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

unsigned char *texture, *buffer;

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
			       WA_Title		, (IPTR)"Tunnel",
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


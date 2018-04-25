
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

#ifndef M_PI
#define	M_PI		3.14159265358979323846	/* pi */
#endif


#define RADIUS 64
#define DIST 256

void do_precalc(float Xd, float Z, float dalpha,
		float cx, float cy,  float cz,    /* vecteur de visee central */
		float vx1, float vy1, float vz1, /* vecteur selon X ecran */
		float vx2, float vy2, float vz2 /* vecteur selon Y ecran  */
		);
void refresh ();

float alpha [H/8+1][W/8+1]  ;
float zede [H/8+1][W/8+1] ;

unsigned char *texture;
unsigned char *buffer, *buffer_remapped;

/***********************************************************************************/

void initftunnel(void)
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
    	WORD palindex;
	
    	for(palindex = 0; palindex < 256; palindex++)
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

  /* Rotate texture */
  
  for(x = 0; x < 127; x++)
  {
    for(y = 0;y < 127; y++)
    {
      c[y * 128 + x] = buffer[x * 128 + (127 - y)];
    }
  }
  
  texture = (unsigned char *) c;

}



void do_precalc(float Xd, float Z, float dalpha,
		float cx, float cy,  float cz,    /* vecteur de visee central */
		float vx1, float vy1, float vz1, /* vecteur selon X ecran */
		float vx2, float vy2, float vz2 /* vecteur selon Y ecran  */
		)     
{
  int i,j;
  float t;
  float prec1,prec2;
  float prec3;
  float x,y,z;


  prec3 = (Xd*Xd -1);
  vx1/=W/8.0;
  vy1/=W/8.0;
  vz1/=W/8.0;


  for (j=0; j<=H/8; j++)
    {
      x = cx   - vx1*4/W+
	(j - H/16)*8*vx2/H;
      y = cy  -vy1*4/W+
	(j - H/16)*8*vy2/H;
      z = cz  - vz1*4/W+
	(j - H/16)*8*vz2/H;  
      
	




      for (i=0; i<=W/8; i++)
      {

	x+=vx1;
	y+=vy1;
	z+=vz1;
	

	prec1 = x*Xd;
	prec2 = (y*y + x*x);
	if (prec2 >= 0.00001)
	  {
	    
	    t= (-prec1 + sqrt( prec1*prec1   -   prec3 * prec2)) 
	      / prec2; 
	    /* remplacer sqrt par ident, sin, ... ca fait des
	       trucs marrants */
	    
	    alpha[j][i]= (atan2(t*y , Xd+t*x) +  dalpha) *128 / M_PI;
	    /* remplacer atan2 par "+" , "*" ou 1 des 2 termes...
	       trucs marrants... */

	    zede[j][i]= Z+ 8*t*z;
	    /* pkoi 8 ? 
	       paceke sinon ca marche pas bien */
	  }
	else   /* trop dans l'alignement du tunnel ... ~div by 0 */
	  {
	    alpha[j][i]= 0;
	    zede[j][i]= 1000;
	  }
	
      }
    }
  
}



void refresh () {
  int i,j;    /* macro-bloc */
  int ii,jj;  /* dans bloc (interpolation) */
  int a,ai, z,zi;      /* 16-16 (fixed-point = lsb) (/pixel)*/
  
  int a0,a0i, z0,z0i;  /*  (/ligne) (colone de gauche)*/
  int a1,a1i, z1,z1i;  /*  (/ligne) (c de droite )*/
  int a22;
  float  al0,al1,al2,al3;

  unsigned char *tmp,*tmp1;


  for (j=0; j<H/8; j++)
    for (i=0; i<W/8; i++)
      {
	
	al0=alpha[j][i];
	a0=( (int) 65536*al0); 
	z0=( (int) 65536*zede[j][i]) ;

	al1=alpha[j+1][i];
        if (abs(al1-al0) > 100)    /* detecter la jonction entre les bords */
	  al1=al0;     /* c'est du kludge pas efficace la...
			  il faudrait renormaliser tout ca et garder 
			  la valeur modulo 1 par exemple, plutot que d'egaliser
			  (ce qui produit des marches d'escalier) */
	
	a22=( (int) (65536*al1));

	a0i=( a22   - a0) /8; 
	z0i=(  ( (int) (65536*zede[j+1][i]))   -z0)  /8;

	al2=alpha[j][i+1];
	 if (abs(al2-al0) > 100)   
	  al2=al0;
	a1=( (int) 65536*al2); 
	z1=( (int) 65536*zede[j][i+1]) ;

	al3=alpha[j+1][i+1];
	if (abs(al3-al2) > 100)  
	  al3=al2;
	

	a1i=(  ( (int) (65536*al3))  - a1) /8; 
	z1i=(  ( (int) (65536*zede[j+1][i+1]))   -z1)  /8;
	


	
	tmp1= (unsigned char *)buffer + i*8 + (j*8)*W - W;
	for (jj=8; jj; jj--)
	  {
	    tmp=tmp1+=W;
	    
	    a=a0;
	    z=z0;
	    ai = (a1-a0) >>3;
	    zi = (z1-z0) >>3; 

	    for (ii=8; ii; ii--)
	      {
		* ( tmp++ ) = * ( texture 
				  + 
				  (
				   ((a>>17)&127)
				    | 
				   ((z>>8) & (127<<7))
				  )
				 ) ;
		a+=ai;
		z+=zi;
	      }


	    a0+=a0i;
	    z0+=z0i;

	    a1+=a1i;
	    z1+=z1i;
	    
	  }
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

/***********************************************************************************/

static void cleanup(char *msg)
{
    
    if (msg)
    {
        printf("FTunnel: %s\n",msg);
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
	{WA_Title	, (IPTR)"FTunnel"   },
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
  float Xd,Z,dalpha=0;
  float aa=0,thet=0;   /*alpha theta de la visee en spherique */

  float eaa=0;       /* ecran alpha : rotation de l'ecran au bout du vecteur visee */
  float exx,exy,exz;   /* vecteur ex normal */
  float eyx,eyy,eyz;   /* ey */

  initftunnel();


  /* Init FPS */

  //init_fps ();
  
  /* Boucle */
  
  while (!Keys[KC_ESC])
  {
      getevents();

    /*** pfff trigo 3d en coord spherique = caca ***/
    exx=-sin(aa);  exy= cos(aa); exz=0;
    eyx=-cos(aa)*sin(thet);  eyy= -sin(aa)*sin(thet); eyz=cos(thet);

    exx*=2; exy*=2; exz*=2;
    eyx*=2; eyy*=2; eyz*=2;
    
    Xd=sin(dalpha)*0.9;
    Z=sin(aa*0.1-thet*0.2+dalpha*0.12001)*700;
    
    aa+=0.004;
    /*aa=0; */
    thet+=0.006203;
    /*thet = M_PI/2; */
    eaa+=0.002;
    dalpha+=0.01;
    
    
    do_precalc(Xd,Z,dalpha,
	       4*cos(aa)*cos(thet), 4*sin(aa)*cos(thet),4* sin(thet),

	       cos(eaa)*exx + sin(eaa)*eyx, 
	       cos(eaa)*exy + sin(eaa)*eyy, 
	       cos(eaa)*exz + sin(eaa)*eyz, 

	       -sin(eaa)*exx + cos(eaa)*eyx, 
	       -sin(eaa)*exy + cos(eaa)*eyy, 
	       -sin(eaa)*exz + cos(eaa)*eyz
	       );

    refresh ();
    
    WaitTOF();
    //update_x ();
    //next_fps ();
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


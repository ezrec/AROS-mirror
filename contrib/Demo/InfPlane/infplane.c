
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

#define C_FUNCTION
/* comment/undefine this define to use the asm routine */

#define W            	320
#define H            	240

#ifndef M_PI
#define	M_PI		3.14159265358979323846	/* pi */
#endif


#define RADIUS 64
#define DIST 256

void do_precalc(float Hp2,                     /* altitude du 2eme plan (1er = 0) (positif...) */
		float Xd, float Yd, float Zd,
		float cx, float cy,  float cz,    /* vecteur de visee central */
		float vx1, float vy1, float vz1, /* vecteur selon X ecran */
		float vx2, float vy2, float vz2 /* vecteur selon Y ecran  */
		);
void refresh1 ();
extern void refresh_prout();   /* eh eh */

float alpha [H/8+1][W/8+1]  ;
float zede [H/8+1][W/8+1] ;

unsigned char *texture1, *buffer;

int ikse [H/8+1][W/8+1]  ;
int igrek [H/8+1][W/8+1] ;

/***********************************************************************************/

void rotate3d(float *xr, float *yr, float *zr,  /* le point a faire tourner */
	      float ax, float ay, float az)     /* les 3 angles (ordre ?..) */
{
  float xr2,yr2,zr2;

  
  xr2= (*xr*cos(az) + *yr*sin(az));
  yr2= (*xr*sin(az) - *yr*cos(az));
  *xr=xr2;
  *yr=yr2;

  xr2= (*xr*cos(ay) + *zr*sin(ay));
  zr2= (*xr*sin(ay) - *zr*cos(ay));
  *xr=xr2;
  *zr=zr2;

  zr2= (*zr*cos(ax) + *yr*sin(ax));
  yr2= (*zr*sin(ax) - *yr*cos(ax));
  *zr=zr2;
  *yr=yr2;
}

void initinfplane(void)
{
  int x,y;
  FILE * fichier;
  char * c;
  
  /* Chargement de la texture */

  c = (char *) malloc ( 256*257 );
  buffer = (char *)malloc (W*(H+1));
  if (!c || !buffer)
    cleanup("Out of memory!");
    
  fichier = fopen ( "pattern.data" , "rb" );
  if (!fichier)
    cleanup("Can't open pattern.data file!");
  
  if (fread ( c , 1, 256*256 , fichier ) != 256*256)
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
  
  texture1 = (unsigned char *) c;

}

void do_precalc(float Hp2,
		float Xd, float Yd, float Zd,
		float cx, float cy,  float cz,    /* vecteur de visee central */
		float vx1, float vy1, float vz1, /* vecteur selon X ecran */
		float vx2, float vy2, float vz2 /* vecteur selon Y ecran  */
		)     
{
  int i,j;
  float t;
  float x,y,z;


  /* euh hem, tjours le meme probleme qd X ou Y arrivent a 65536... */


  vx1/=W/8.0;
  vy1/=W/8.0;
  vz1/=W/8.0;

  vx2/=H/8.0;
  vy2/=H/8.0;
  vz2/=H/8.0;


  for (j=0; j<=H/8; j++)
    {
      x = cx   - vx1*W/16 +   (j - H/16)*vx2;
      y = cy   - vy1*W/16 +   (j - H/16)*vy2;
      z = cz   - vz1*W/16 +   (j - H/16)*vz2;  
      
      for (i=0; i<=W/8; i++)
      {

	x+=vx1;
	y+=vy1;
	z+=vz1;
	

	if (z <=0.001)        /* look downward ? */
	  {
	    
	    t= -Zd/z;
	    
	    ikse [j][i]= (int)256*(Xd+x*t); 
	    igrek[j][i]= (int)256*(Yd+y*t);

	  }
	else if (z >=0.001)  /* look at the sky => 2nd plane */
	  {
	    t= (Hp2-Zd)/z;
	    
	    ikse [j][i]= (int)256*(Xd+x*t); 
	    igrek[j][i]= (int)256*(Yd+y*t);
	    
	  }
	else             /* look infinite */
	  {
	    ikse [j][i]= -1;                    /* C'est une valeur completement idiote */
	    igrek[j][i]= -1;
	  }
	
      }
    }
  
}


void refresh1 () {
  int i,j;    /* macro-bloc */
  int ii,jj;  /* dans bloc (interpolation) */
  int a,ai, z,zi;      /* 8-8 (fixed-point = lsb) (/pixel)*/
  
  int a0,a0i, z0,z0i;
  int a1,a1i, z1,z1i;
  int a2,a3,z2,z3;

  unsigned char *tmp,*tmp1;


  for (j=0; j<H/8; j++)
    for (i=0; i<W/8; i++)
      {
	a0=ikse[j][i];
	a1=ikse[j][i+1];
	a2=ikse[j+1][i];
	a3=ikse[j+1][i+1];

	z0=igrek[j][i]     ;
	z1=igrek[j][i+1]   ;
	z2=igrek[j+1][i]   ;
	z3=igrek[j+1][i+1] ;

	a0i=( a2   - a0) /8; 
	z0i=( z2   - z0)  /8;


	a1i=(  a3  - a1) /8; 
	z1i=(  z3  - z1)  /8;
	


	
	tmp1= (unsigned char *)buffer + i*8 + (j*8)*W-W;
	for (jj=8; jj; jj--)
	  {
	    tmp=tmp1+=W;
	    
	    a=a0;
	    z=z0;
	    ai = (a1-a0) >>3;
	    zi = (z1-z0) >>3; 
	    
	    for (ii=8; ii; ii--)
	      {
		* ( tmp++ ) = * ( texture1  + 
				  ( ((a>>8)&255)   |
                                    ((z) & (255<<8))  
				  ) 
				) ;
		a+=ai;
		z+=zi;
	      }
	  /* unrolling loop... just to see if it improves things
	  (it does a bit) */

/*
	    * ( tmp++ ) = * ( texture1  + 
				  ((((a+=ai)>>8)&255)
				  | (((z+=zi)) & (255<<8))  )) ;
	    * ( tmp++ ) = * ( texture1  + 
				  ((((a+=ai)>>8)&255)
				  | (((z+=zi)) & (255<<8))  )) ;
	    * ( tmp++ ) = * ( texture1  + 
				  ((((a+=ai)>>8)&255)
				  | (((z+=zi)) & (255<<8))  )) ;
	    * ( tmp++ ) = * ( texture1  + 
				  ((((a+=ai)>>8)&255)
				  | (((z+=zi)) & (255<<8))  )) ;
	    * ( tmp++ ) = * ( texture1  + 
				  ((((a+=ai)>>8)&255)
				  | (((z+=zi)) & (255<<8))  )) ;
	    * ( tmp++ ) = * ( texture1  + 
				  ((((a+=ai)>>8)&255)
				  | (((z+=zi)) & (255<<8))  )) ;
	    * ( tmp++ ) = * ( texture1  + 
				  ((((a+=ai)>>8)&255)
				  | (((z+=zi)) & (255<<8))  )) ;
	    * ( tmp++ ) = * ( texture1  + 
				  ((((a+=ai)>>8)&255)
				  | (((z+=zi)) & (255<<8))  )) ;
	*/   

	    a0+=a0i;
	    z0+=z0i;

	    a1+=a1i;
	    z1+=z1i;
	    
	  }
      }
  
}

/***********************************************************************************/

static void cleanup(char *msg)
{
    
    if (msg)
    {
        printf("InfPlane: %s\n",msg);
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
			       WA_Title		, (IPTR)"InfPlane",
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
  int fichier;


  float Xd,Yd,Zd;
  float dzz=0;
  float aa=0,thet=0; 



  float ax,ay,az;

  float cx,cy,cz;      /* vecteur  visee */
  float exx,exy,exz;   /* vecteur ex normal */
  float eyx,eyy,eyz;   /* ey */

  initinfplane();

  /* Boucle */
  
  Xd=Yd=0;
  ax=ay=az=0;

  /* Init FPS */

  //init_fps ();
  
  /* Boucle */
  
  while (!Keys[KC_ESC])
  {
      getevents();

     aa+=0.006;
    thet+=0.008203;


    if (aa>1){             /* this is the "keyframing" code :)) */
      if (aa<2.45)
	ax+=0.0261;
      else
	{
	  ax+=0.0061;
	  ay+=0.0033;
	  az+=0.0018;
	}
    }

    /* rotate view */
    cx=0.3; cy=0; cz=0;
    exx=0; exy=1; exz=0;
    eyx=0; eyy=0; eyz=1;

    rotate3d(&cx, &cy, &cz,
	     ax,ay,az);
    rotate3d(&exx, &exy, &exz,
	     ax,ay,az);
    rotate3d(&eyx, &eyy, &eyz,
	     ax,ay,az);



    Zd=10*sin(dzz/70)*0.9+30;
    Yd=dzz+sin(aa*0.1-thet*0.2)*700;
    dzz+=1.5;
    Xd=dzz*2;


    /*Xd=Yd=0;
    //Zd=10; */
    do_precalc(240,
	       Xd,Yd,Zd,
	       cx,cy,cz,
	       exx,exy,exz,
	       eyx,eyy,eyz
	       );
    
  
#ifdef C_FUNCTION
    refresh1 ();                  /* la routine en C */
#else
    refresh_prout ();           /* ma routine asm */
#endif
      
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
    
    //update_x ();
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



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

#include "schedul_fx.h"

/***********************************************************************************/

#define ARG_TEMPLATE "WINPOSX=X/N/K,WINPOSY=Y/N/K,NOBLUR/S,FORCESCREEN=SCR/S,FORCEWINDOW=WIN/S"

#define ARG_X 0
#define ARG_Y 1
#define ARG_NOBLUR 	2
#define ARG_SCR     	3
#define ARG_WIN     	4
#define NUM_ARGS   	5

struct IntuitionBase 	*IntuitionBase;
struct GfxBase 		*GfxBase;
struct Library 		*CyberGfxBase;
struct Screen 		*scr;
struct Window 		*win;
struct RastPort 	*rp;
struct RDArgs		*myargs;
ULONG 			cgfx_coltab[256];
UBYTE	    	    	remaptable[256];
UBYTE 			Keys[128];
char			s[256];
WORD	    	    	winx = -1, winy = -1;
IPTR			args[NUM_ARGS];
BOOL	    	    	forcescreen, forcewindow;
BOOL	    	    	mustremap, truecolor, remapped, wbscreen = TRUE;

static void cleanup(char *msg);

/***********************************************************************************/

#ifndef M_PI
#define	M_PI		3.14159265358979323846	/* pi */
#endif

#define W 320     /* if you change it: dont forget to also change the W
		     in memshd.S ... */
#define H 240

int            motion_blur;
char *b8;   /* working 8bits buffer */
char *b8_remapped;

typedef struct
{
    int x,y,z,r;
} p3;

typedef float matrix[3][3];

#define GMAX 5000
p3 gal[GMAX];
float precos[512];
float presin[512];

typedef unsigned short word;

void memshset (char *dst, int center_shade,int fixed_shade, int length_2);

void init_plouf();
void drawgalaxy();

static inline float ocos(float a){
    return (precos[(int)(a*256/M_PI)&511]);
}

static inline float osin(float a){
    return (presin[(int)(a*256/M_PI)&511]);
}

/*#undef ocos
#undef osin

#define ocos cos
#define osin sin
*/


void mulmat(matrix *a, matrix *b, matrix *c){
    int i,j;

    for (i=0; i<3; i++)
	for (j=0; j<3; j++)
	    (*c)[i][j] = (*a)[i][0] * (*b)[0][j]
	        	+(*a)[i][1] * (*b)[1][j]
	        	+(*a)[i][2] * (*b)[2][j];
}

static inline void mulvec(matrix *a, float *x, float *y, float *z){
    float nx=*x,ny=*y,nz=*z;

    *x = nx*(*a)[0][0] + ny*(*a)[0][1] + nz*(*a)[0][2];
    *y = nx*(*a)[1][0] + ny*(*a)[1][1] + nz*(*a)[1][2];
    *z = nx*(*a)[2][0] + ny*(*a)[2][1] + nz*(*a)[2][2];

}


void setrmat(float a, float b, float c, matrix *m){
    int i,j;
    for (i=0; i<3; i++) for (j=0; j<3; j++) (*m)[i][j]=(float)(i==j);

    if (a!=0){
      (*m)[0][0] = cos(a);     (*m)[0][1] = sin(a);
      (*m)[1][0] = sin(a);     (*m)[1][1] = -cos(a);
      return;
    }
    if (b!=0){
      (*m)[0][0] = cos(b);     (*m)[0][2] = sin(b);
      (*m)[2][0] = sin(b);     (*m)[2][2] = -cos(b);
      return;
    }

      (*m)[1][1] = cos(c);     (*m)[1][2] = sin(c);
      (*m)[2][1] = sin(c);     (*m)[2][2] = -cos(c);
}
 

void rotate3d(float *xr, float *yr, float *zr,  /* point to rotate */
	      float ax, float ay, float az)     /* the 3 angles (order ?..) */
{
    float xr2,yr2,zr2;


    xr2= (*xr*ocos(az) + *yr*osin(az));
    yr2= (*xr*osin(az) - *yr*ocos(az));
    *xr=xr2;
    *yr=yr2;

    xr2= (*xr*ocos(ay) + *zr*osin(ay));
    zr2= (*xr*osin(ay) - *zr*ocos(ay));
    *xr=xr2;
    *zr=zr2;

    zr2= (*zr*ocos(ax) + *yr*osin(ax));
    yr2= (*zr*osin(ax) - *yr*ocos(ax));
    *zr=zr2;
    *yr=yr2;
}

void aff () {

    if (truecolor)
    {
	WriteLUTPixelArray(b8,
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
	UBYTE *src = b8;
	UBYTE *dest = b8_remapped;

	for(i = 0; i < W * H; i++)
	{
	    *dest++ = remaptable[*src++];
	}
	WriteChunkyPixels(rp,
	    	    	  win->BorderLeft,
			  win->BorderTop,
			  win->BorderLeft + W - 1,
			  win->BorderTop + H - 1,
			  b8_remapped,
			  W);

    }
    else
    {
	WriteChunkyPixels(rp,
	    	    	  win->BorderLeft,
			  win->BorderTop,
			  win->BorderLeft + W - 1,
			  win->BorderTop + H - 1,
			  b8,
			  W);
    }

}

void init_plouf(){
  int i;

  b8 = (unsigned char *)malloc(W*(H+1)); /* +1 : I dont trust my range checkings... */
  if (!b8) cleanup("Out of memory!");
  
  if (mustremap)
  {
    b8_remapped = (unsigned char *)malloc(W*H);
    if (!b8_remapped) cleanup("Out of memory!");
  }
  
  for (i=0; i<256; i++){
    unsigned int r,g,b;
    r = (int)(i*0.7);
    g = (int)(i*0.8);
    b = i;

    if (truecolor)
    {
    	cgfx_coltab[i]=((r<<16)|(g<<8)|(b));  
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

  for (i=0; i<512; i++){
    precos[i]=cos(i*M_PI/256);
    presin[i]=sin(i*M_PI/256);
  }


  /*
    uniforme cubique 
  for (i=0; i<GMAX; i++){
    gal[i].x = 1*((rand()&1023) - 512);
    gal[i].y = 1*((rand()&1023) - 512);
    gal[i].z = 1*((rand()&1023) - 512);
    gal[i].r = rand()&63;
  }
  */

  for (i=0; i<GMAX; i++){
    float r,th,h,dth;
    r = rand()*1.0 / RAND_MAX;
      r = (1-r)*(1-r)+0.05;
      if (r<0.12)
	th = rand()*M_PI*2/RAND_MAX;
      else {
	th = (rand()&3)*M_PI / 2+ r*r*2;
	dth =   rand()*1.0/RAND_MAX;
	dth = dth*dth*2;
	th+=dth;
      }
    

    gal[i].x = 512*r*cos(th);
    gal[i].z = 512*r*sin(th);
    h = (1+cos(r*M_PI))*150;
    dth =   rand()*1.0/RAND_MAX;
    gal[i].y =h*(dth-0.5);
    gal[i].r = (2-r)*60+31;
  }
  gal[0].x = gal[0].y = gal[0].z = 0;
  gal[0].r = 320;


}

void starsh(char *, char);

void drawshdisk(int x0, int y0, int r){
  int x=0;
  int y;
  int ly; /* last y */
  int delta;
  int c;  /* color at center */
  int d;  /* delta */

#define SLIMIT 17
#define SRANGE 15
    if (r<=SLIMIT)
    {
      /* range checking is already (more or less) done... */
      starsh(&b8[x0 + W*y0],10+r*5);
      return;
    }

    if (r < SLIMIT+SRANGE)
      r = ((r-SLIMIT)*SLIMIT)/SRANGE+1;
    
    y=ly=r;     /* AAaargh */

  delta = 3-2*r;
  do{ 
    
    if (y!=ly){          /* dont overlap these lines */
      c = ((r-y+1)<<13)/r;
      d = -c/(x+1);


      if (y==x+1)            /* this would overlap with the next x lines */
	goto TOTO;           /* WHY NOT */

      /*  note : for "normal" numbers (not too big) :
	  (unsigned int)(x) < M   <=>  0<=x<H
	  (because if x<0, then (unsigned)(x) = 2**32-|x| which is
	  BIG and thus >H )

	  This is clearly a stupid, unmaintanable, unreadable "optimization".
	  But i like it :)
      */
      if ((unsigned int)(y0-y-1)<H-3)   
	memshset(&b8[x0 + W*(y0-y+1)] ,c,d, x);
      if ((unsigned int)(y0+y-1)<H-3)
	memshset(&b8[x0 + W*(y0+y)] ,c,d, x);
    }
  TOTO:
    c = ((r-x+1)<<13)/r;
    d = -c/(y);

    if ((unsigned int)(y0-x-1)<H-3)
      memshset(&b8[x0 + W*(y0-x)] ,c,d, y);
    if ((unsigned int)(y0+x-1)<H-3)
      memshset(&b8[x0 + W*(y0+x+1)] ,c,d, y);
      
    ly=y;
    if (delta<0)
      delta += 4*x+6;
    else {
      delta += 4*(x-y)+10;
      y--;
    }
    
    x++;
  } while (x<y);

}


void mblur (char *src, int nbpixels);


void drawgalaxy(){
  int r;
  int x,y;
  float rx,ry,rz;
  int i;
  float oa,ob,oc;
  float t;

  float a, b, c;
  matrix ma,mb,mc,mr;

  /* t is the parametric coordinate for the animation;
   change the scale value to change the speed of anim
   (independant of processor speed)
  */
  t=opti_scale_time(0.418, &demo_elapsed_time);  

  a= 0.9*t;
  b=     t;
  c= 1.1*t;



  setrmat(a,0,0,&ma);
  setrmat(0,b,0,&mb);
  mulmat(&ma,&mb,&mc);
  setrmat(0,0,c,&ma);
  mulmat(&ma,&mc,&mr);

    oa=140*osin(a);
    ob=140*ocos(b);
    oc=240*osin(c);

    if (motion_blur){
  /*
    mblur does something like that:
    (or did, perhaps it's another version!..)

  for (i=0; i<W*H; i++)   
  b8[i]= (b8[i]>>3) + (b8[i]>>1) ;  
*/
      mblur (b8, W*H);
    }
    else
      memset(b8,0,W*H);


  for (i=0; i<GMAX; i++){
    
    rx=gal[i].x;
    ry=gal[i].y;
    rz=gal[i].z;

    mulvec(&mr, &rx, &ry, &rz);

    rx+=oa;
    ry+=ob;
    rz+=oc;
    rz+=300;

    if (rz>5){
      x = (int)(15*rx/(rz/5+1))+W/2;  /* tain jcomprend plus rien  */
      y = (int)(15*ry/(rz/5+1))+H/2;  /* a ces formules de daube !! */
      r = (int)(3*gal[i].r / (rz/4+3))+2;

     
      if ((unsigned int)x<W-1
	  &&
	  (unsigned int)y<H-1)
	
	drawshdisk(x,y, r);
    }       
  }


}

/***********************************************************************************/

static void cleanup(char *msg)
{
    
    if (msg)
    {
        printf("Galaxy: %s\n",msg);
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
    
    if (myargs) FreeArgs(myargs);
    
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

static void getargs(void)
{
    if (!(myargs = ReadArgs(ARG_TEMPLATE, args, 0)))
    {
        Fault(IoErr(), 0, s, 255);
	cleanup(s);
    }
    
    if (args[ARG_NOBLUR])
    {
        motion_blur = 0;
    } else {
        motion_blur = 1;
    }
    
    if (args[ARG_SCR])
	forcescreen = TRUE;
    else if (args[ARG_WIN])
	forcewindow = TRUE;

    if (args[ARG_X]) winx = *(IPTR *)args[ARG_X];
    if (args[ARG_Y]) winy = *(IPTR *)args[ARG_Y];
    
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
	{WA_Title	, (IPTR)"Galaxy"    },
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
    //init_fps();
    init_plouf();
    opti_sched_init();

    while (!Keys[KC_ESC])
    {
        getevents();

	drawgalaxy();

	aff();
	WaitTOF();
	opti_sched_nextframe();
	opti_sched_update();

    }

    //aff_fps(NAME);

}

/***********************************************************************************/

int main(void)
{
    getargs();
    openlibs();
    getvisual();
    makewin();
    action();
    cleanup(0);
    
    return 0;
}

/***********************************************************************************/


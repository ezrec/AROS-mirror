/*
   Landscape rendering
  
   Ok.. i know that voxel is something else... but a lot of people is using
   the name "voxel" to mean this kind of rendering tecnique.
   I wrote this to explain the basic idea behind the rendering of newvox4;
   newvox4 is very badly written (it's named 4 because is the fourth of
   a sequel of experiments) and is coded in pascal + asm.
   Since i got a few request of an explanation i decided to write the kernel
   of the rendering in C hoping that this will be easier to understand.
   This implements only the base landscape (no sky or floating ball) and
   with keyboard only support but i think you can get the idea of how I
   implemented those other things.
  
   I'm releasing this code to the public domain for free... and as it's
   probably really obvious there's no warranty of any kind on it.
   You can do whatever you want with this source; however a credit in any
   program that uses part of this code would be really appreciated :)
  
   Any comment is welcome :)
  
                                    Andrea "6502" Griffini, programmer
                                           agriff@ix.netcom.com
                                        http://vv.val.net/~agriffini
*/
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

#define SCREENWIDTH  320
#define SCREENHEIGHT 200
#define SCREENCY (SCREENHEIGHT / 2)

/***********************************************************************************/

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Library *CyberGfxBase;
struct Screen *scr;
struct Window *win;
struct RastPort *rp;

ULONG cgfx_coltab[256];
BYTE Keys[256];

/***********************************************************************************/

typedef unsigned char byte;

byte HMap[256*256];      /* Height field */
byte CMap[256*256];      /* Color map */
byte Video[SCREENWIDTH*SCREENHEIGHT];     /* Off-screen buffer */

/***********************************************************************************/

/* Reduces a value to 0..255 (used in height field computation) */
int Clamp(int x)
{
    return (x<0 ? 0 : (x>255 ? 255 : x));
}

/***********************************************************************************/

/* Heightfield and colormap computation */
void ComputeMap(void)
{
    int p,i,j,k,k2,p2;

    /* Start from a plasma clouds fractal */
    HMap[0]=128;
    for ( p=256; p>1; p=p2 )
    {
	p2=p>>1;
	k=p*8+20; k2=k>>1;
	for ( i=0; i<256; i+=p )
	{
	    for ( j=0; j<256; j+=p )
	    {
		int a,b,c,d;

		a=HMap[(i<<8)+j];
		b=HMap[(((i+p)&255)<<8)+j];
		c=HMap[(i<<8)+((j+p)&255)];
		d=HMap[(((i+p)&255)<<8)+((j+p)&255)];

		HMap[(i<<8)+((j+p2)&255)]=
		  Clamp(((a+c)>>1)+(rand()%k-k2));
		HMap[(((i+p2)&255)<<8)+((j+p2)&255)]=
		  Clamp(((a+b+c+d)>>2)+(rand()%k-k2));
		HMap[(((i+p2)&255)<<8)+j]=
		  Clamp(((a+b)>>1)+(rand()%k-k2));
	    }
	}
    }

    /* Smoothing */
    for ( k=0; k<3; k++ )
	for ( i=0; i<256*256; i+=256 )
	    for ( j=0; j<256; j++ )
	    {
		HMap[i+j]=(HMap[((i+256)&0xFF00)+j]+HMap[i+((j+1)&0xFF)]+
			   HMap[((i-256)&0xFF00)+j]+HMap[i+((j-1)&0xFF)])>>2;
	    }

    /* Color computation (derivative of the height field) */
    for ( i=0; i<256*256; i+=256 )
	for ( j=0; j<256; j++ )
	{
	    k=128+(HMap[((i+256)&0xFF00)+((j+1)&255)]-HMap[i+j])*4;
	    if ( k<0 ) k=0; if (k>255) k=255;
	    CMap[i+j]=k;
	}
}

/***********************************************************************************/

int lasty[SCREENWIDTH],         /* Last pixel drawn on a given column */
    lastc[SCREENWIDTH];         /* Color of last pixel on a column */

/*
   Draw a "section" of the landscape; x0,y0 and x1,y1 and the xy coordinates
   on the height field, hy is the viewpoint height, s is the scaling factor
   for the distance. x0,y0,x1,y1 are 16.16 fixed point numbers and the
   scaling factor is a 16.8 fixed point value.
 */

/***********************************************************************************/

void line(int x0,int y0,int x1,int y1,int hy,int s)
{
    int i,sx,sy;

    /* Compute xy speed */
    sx=(x1-x0)/SCREENWIDTH; sy=(y1-y0)/SCREENWIDTH;
    for ( i=0; i<SCREENWIDTH; i++ )
    {
	int c,y,h,u0,v0,u1,v1,a,b,h0,h1,h2,h3;

	/* Compute the xy coordinates; a and b will be the position inside the
	   single map cell (0..255).
	 */
	u0=(x0>>16)&0xFF;    a=(x0>>8)&255;
	v0=((y0>>8)&0xFF00); b=(y0>>8)&255;
	u1=(u0+1)&0xFF;
	v1=(v0+256)&0xFF00;

	/* Fetch the height at the four corners of the square the point is in */
	h0=HMap[u0+v0]; h2=HMap[u0+v1];
	h1=HMap[u1+v0]; h3=HMap[u1+v1];

	/* Compute the height using bilinear interpolation */
	h0=(h0<<8)+a*(h1-h0);
	h2=(h2<<8)+a*(h3-h2);
	h=((h0<<8)+b*(h2-h0))>>16;

	/* Fetch the color at the four corners of the square the point is in */
	h0=CMap[u0+v0]; h2=CMap[u0+v1];
	h1=CMap[u1+v0]; h3=CMap[u1+v1];

	/* Compute the color using bilinear interpolation (in 16.16) */
	h0=(h0<<8)+a*(h1-h0);
	h2=(h2<<8)+a*(h3-h2);
	c=((h0<<8)+b*(h2-h0));

	/* Compute screen height using the scaling factor */
	y=(((h-hy)*s)>>11)+100;

	/* Draw the column */
	if ( y<(a=lasty[i]) )
	{
	    unsigned char *b=Video+a*SCREENWIDTH+i;
	    int sc,cc;


	    if ( lastc[i]==-1 )
	        lastc[i]=c;

	    sc=(c-lastc[i])/(a-y);
	    cc=lastc[i];

	    if ( a>(SCREENHEIGHT-1) ) { b-=(a-(SCREENHEIGHT-1))*SCREENWIDTH; cc+=(a-(SCREENHEIGHT-1))*sc; a=SCREENHEIGHT-1; }
	    if ( y<0 ) y=0;
	    while ( y<a )
	    {
		*b=cc>>18; cc+=sc;
		b-=SCREENWIDTH; a--;
	    }
	    lasty[i]=y;

	}
	lastc[i]=c;

	/* Advance to next xy position */
	x0+=sx; y0+=sy;
    }
}

/***********************************************************************************/

float FOV=3.141592654/4;   /* half of the xy field of view */

/***********************************************************************************/

/*
// Draw the view from the point x0,y0 (16.16) looking at angle a
*/
void View(int x0,int y0,float aa)
{
    int d;
    int a,b,h,u0,v0,u1,v1,h0,h1,h2,h3;

#if 0
    /* Clear offscreen buffer */
    memset(Video,0,SCREENWIDTH*SCREENHEIGHT);
#else
    /* stegerg */
    
    for(b = 0; b < SCREENHEIGHT; b++)
    {
        memset(Video + b * SCREENWIDTH, 64 + ((256 - 64) * b / (SCREENHEIGHT-1)), SCREENWIDTH);
    }
#endif
    /* Initialize last-y and last-color arrays */
    for ( d=0; d<SCREENWIDTH; d++ )
    {
	lasty[d]=SCREENHEIGHT;
	lastc[d]=-1;
    }

    /* Compute viewpoint height value */

    /* Compute the xy coordinates; a and b will be the position inside the
       single map cell (0..255).
     */
    u0=(x0>>16)&0xFF;    a=(x0>>8)&255;
    v0=((y0>>8)&0xFF00); b=(y0>>8)&255;
    u1=(u0+1)&0xFF;
    v1=(v0+256)&0xFF00;

    /* Fetch the height at the four corners of the square the point is in */
    h0=HMap[u0+v0]; h2=HMap[u0+v1];
    h1=HMap[u1+v0]; h3=HMap[u1+v1];

    /* Compute the height using bilinear interpolation */
    h0=(h0<<8)+a*(h1-h0);
    h2=(h2<<8)+a*(h3-h2);
    h=((h0<<8)+b*(h2-h0))>>16;

    /* Draw the landscape from near to far without overdraw */
    for ( d=0; d<100; d+=1+(d>>6) )
    {
	line(x0+d*65536*cos(aa-FOV),y0+d*65536*sin(aa-FOV),
             x0+d*65536*cos(aa+FOV),y0+d*65536*sin(aa+FOV),
             h-30,SCREENCY*256/(d+1));
    }

    WriteLUTPixelArray(Video,
		       0,
		       0,
		       SCREENWIDTH,
		       rp,
		       cgfx_coltab,
		       win->BorderLeft,
		       win->BorderTop,
		       SCREENWIDTH,
		       SCREENHEIGHT,
		       CTABFMT_XRGB8);

}

/***********************************************************************************/

void cleanup(char *msg)
{
    if (msg)
    {
        printf("newvox: %s\n",msg);
    }
    
    if (win) CloseWindow(win);
    
    if (scr) UnlockPubScreen(0, scr);
    
    if (CyberGfxBase) CloseLibrary(CyberGfxBase);
    if (GfxBase) CloseLibrary((struct Library *)GfxBase);
    if (IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
    
    exit(0);
}

/***********************************************************************************/

void openlibs(void)
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

void getvisual(void)
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

void makewin(void)
{
    win = OpenWindowTags(NULL, WA_CustomScreen, (IPTR)scr, 
    			       WA_InnerWidth, SCREENWIDTH,
    			       WA_InnerHeight, SCREENHEIGHT,
			       WA_Title, (IPTR)"NewVox",
			       WA_DragBar, TRUE,
			       WA_DepthGadget, TRUE,
			       WA_CloseGadget, TRUE,
			       WA_Activate, TRUE,
			       WA_IDCMP, IDCMP_CLOSEWINDOW |
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

void getevents(void)
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

main(int argc, char *argv[])
{
    int done;
    int i,k;
    float ss,sa,a,s;
    int x0,y0;

    openlibs();
    getvisual();
   
    /* Set up the first 64 colors to a grayscale */
    for ( i=0; i<64; i++ )
    {
	ULONG red, green, blue;

	/* red = green = blue = i * 4; */
	green = i * 4; red = blue = 10;
	
	cgfx_coltab[i] = (red << 16) + (green << 8) + blue;
    }

    /* stegerg: add a sky */
    for (i = 0; i < (256 - 64); i++)
    {
        ULONG red, green, blue;
	
	blue = 0xFF;
	red = green = 255 - (i * 255 / (256 - 64 - 1));

	cgfx_coltab[64 + i] = (red << 16) + (green << 8) + blue;	
    }
    
    /* Compute the height map */
    ComputeMap();

    makewin();
    
    /* Main loop

	 a     = angle
	 x0,y0 = current position
	 s     = speed constant
	 ss    = current forward/backward speed
	 sa    = angular speed
     */
    done=0;
    a=0; k=x0=y0=0;
    s=1024; /*s=4096;*/
    ss=0; sa=0;
    while(!done)
    {
	/* Draw the frame */
	View(x0,y0,a);

	/* Update position/angle */
	x0+=ss*cos(a); y0+=ss*sin(a);
	a+=sa;

	/* Slowly reset the angle to 0 */
	if ( sa != 0 )
	{
	    if ( sa < 0 )
                sa += 0.001;
	    else
                sa -= 0.001;
	}

	/* User input */
	
	getevents();
		
	if (Keys[KC_ESC]) {
	    done = 1;
	}
	if (Keys[KC_UP]) {
	    ss+=s;
	}
	if (Keys[KC_DOWN]) {
	    ss-=s;
	}
	if (Keys[KC_RIGHT]) {
	    sa+=0.003;
	}
	if (Keys[KC_LEFT]) {
	    sa-=0.003;
	}
    }

    cleanup(0);
}

/***********************************************************************************/


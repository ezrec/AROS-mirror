#include "kdp.h"
#include "kdpgfx.h"
#include <math.h>

#include <proto/dos.h>

UBYTE *txt;
int light[41*26];
int blokx[41*26];
int bloky[41*26];

#define ARG_TEMPLATE "WINPOSX=X/N/K,WINPOSY=Y/N/K"
#define ARG_X 0
#define ARG_Y 1
#define NUM_ARGS 2

static IPTR args[NUM_ARGS];

static void getarguments(void)
{
    struct RDArgs *myargs;
    
    if ((myargs = ReadArgs(ARG_TEMPLATE, args, NULL)))
    {
	if (args[ARG_X])
	{
	    char s[10];
	    WORD winx = *(IPTR *)args[ARG_X];
	    
	    snprintf(s, sizeof(s), "%d", winx);
	    SetVar("WINPOSX", s, strlen(s), GVF_LOCAL_ONLY | LV_VAR);
	}
	    
	if (args[ARG_Y])
	{
	    char s[10];
	    WORD winy = *(IPTR *)args[ARG_Y];

	    snprintf(s, sizeof(s), "%d", winy);
	    SetVar("WINPOSY", s, strlen(s), GVF_LOCAL_ONLY | LV_VAR);
    	}
	
    	FreeArgs(myargs);
    }
}

void make_tuntab(float persp,float po,float rot,float ox,float oy,float mulx,float muly,float tox,float toy)
{
  float xx, yy;
  int x,y;
  float vx, vy;
  float pe;
  float mx;
  int l;
  int blk;
  for (y=0; y<26; y++)
    for (x=0; x<41; x++)
    {
      xx=(tox+x-20)*mulx;
      yy=(toy+y-13)*muly;
      vx=atan2(xx,yy)*(256/(2*PI));
      pe=900+(sin(rot+(2*PI*vx/(256/3)))*po);
      mx=sqrt((xx*xx)+(yy*yy));
      if (mx!=0)
        vy=pe/mx;
      l=vy/persp;
       if (l<0) l=0;
      if (l>255) l=255;
     
      blk=y*41+x;
      light[blk]=l*256;
      blokx[blk]=(vx+ox)*256;
      bloky[blk]=(vy+oy)*256;
    }
}


void stampblok(UBYTE *vmem)
{
    int   dx1, dy1, dx2, dy2, dx, dy;
    int   tx1, ty1, tx2, ty2, tx, ty;
    int   i, j, x, y, o;
    int x2,y2,blk;
    int lx1,lx2,lx3,ldx1,ldx2,ldx3;
    for(y2=0;y2<25;y2++)
      for(x2=0;x2<40;x2++)
        {

        blk=41*y2+x2;

        tx1 = blokx[blk]<<8;
        ty1 = bloky[blk]<<8;
        tx2 = blokx[blk+1]<<8;
        ty2 = bloky[blk+1]<<8;

        dx1 = ((blokx[blk+41]<<8)-tx1) >> 3;
        dy1 = ((bloky[blk+41]<<8)-ty1) >> 3;
        dx2 = ((blokx[blk+42]<<8)-tx2) >> 3;
        dy2 = ((bloky[blk+42]<<8)-ty2) >> 3;

        lx1=light[blk]<<8;
        lx2=light[blk+1]<<8;

        ldx1=((light[blk+41]<<8)-lx1)>>3;
        ldx2=((light[blk+42]<<8)-lx2)>>3;

        o=320*(y2<<3)+(x2<<3);
        for (y=0; y<=7; y++)
          {
          lx3=lx1;
          ldx3=(lx2-lx1)>>3;
          tx = tx1; ty = ty1;
          dx = ((tx2-tx1)) >> 3;
          dy = ((ty2-ty1)) >> 3;

          if (dx>16*65536) dx=-(32*65536-dx);
          if (dx<-(16*65536)) dx=32*65536-dx;
          for (x=0; x<=7; x++ )
            {
            //vmem[o] = txt[((ty & 0x0000FF00)+(tx >> 8)) & 0xFFFF];
	    //vmem[o] = txt[(((ty >>8) & 0x0000FF00)+(tx >> 16)) & 0xFFFF];
	    
	    //kprintf("vmem[%d] = %d  tx = %08x ty = %08x\n", o, vmem[o],tx,ty);
	    
            vmem[o]= KDPcoltab[ txt[(((ty >>8) & 0x0000FF00)+(tx >> 16)) & 0xFFFF]+(UBYTE)(lx3>>16)];
            tx += dx;
            ty += dy;
            lx3+=ldx3;
            o++;
            }
          tx1 += dx1;
          ty1 += dy1;
          lx1+=ldx1;
          tx2 += dx2;
          ty2 += dy2;
          lx2+=ldx2;
          o += 312;
          }
        }
}



main()
{
  KDPscreen screen;
  KDPmouse  mouse;
  int i;
  int x,y;
  float ox,oy;
  float mx,my;
  float mulx,muly;
  UBYTE *vmem;
  float rot = 0.0;
  float tox,toy;
  float doy;
  float qx = 0.0,qy = 0.0;
  int l;
  int ii;
  UBYTE pal[256*3];
  
  getarguments();
  
  txt=KDPloadFile("raw/tg.raw");
  if (KDPopen(&screen))
    {
    vmem=screen.vmem;
    KDPgetMouse(&mouse);
    mouse.button=0;
    mx=2;my=8;
    mulx=1;muly=1;
   
  for(i=0;i<512;i++)
   {
     ii=i-150;
     if(ii<0) ii=0;
     if(ii>255) ii=255;
    KDPcoltab[i]=ii;
  }
   
   KDPpalRange(0  ,0  ,0  ,
              62*2 ,34*2,255,
              0,100,pal);
  KDPpalRange(62*2, 34*2,255,
              255,255,255,
              100,255,pal);
   
    KDPsetPal(pal);
    while(mouse.button!=3)
      {
      if (mouse.button==1)
        {
        //mx+=(float)mouse.xspeed/100;
        my+=mouse.yspeed;
        }
      if (mouse.button==0)
        {
        ox+=(float)mouse.xspeed/10;
        doy+=(float)mouse.yspeed/50;
        }
      if (mouse.button==2)
        {
        //mulx+=(float)mouse.xspeed/10;
        //muly+=(float)mouse.yspeed/10;
        tox-=(float)mouse.xspeed/10;
        toy-=(float)mouse.yspeed/10;
        }
      else
        {
        tox=sin(qx)*20;
        toy=cos(qy)*13;
        }
      qx+=0.02;
      qy+=0.03;
      oy+=doy;
      rot+=0.1;
      ox=rot*4;
      mx=1.2+sin(rot)*1;
      //my+=(float)mouse.yspeed/50;
      /*for(x=0;x<41;x++)
        for(y=0;y<26;y++)
          {
          blokx[x][y]=(x-20)*mx;
          bloky[x][y]=(y-13)*my;
          } */
      make_tuntab(mx,my,rot,ox,oy,mulx,muly,tox,toy);
      stampblok(vmem);
      
     // for(i=0;i<100;i++)
     //   for(ii=0;ii<320;ii++)  
     //     vmem[((199-i)*320)+ii]=vmem[(i*320)+ii];
     // for(i=0;i<200;i++)
     //   for(ii=0;ii<160;ii++) 
     //     vmem[i*320+(320-ii)]=vmem[i*320+ii];
      
      KDPwaitVr();
      KDPshow(vmem);
      KDPgetMouse(&mouse);
      }
    }
  KDPclose(&screen);
}


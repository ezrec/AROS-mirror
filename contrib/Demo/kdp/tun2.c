#include "kdp.h"
#include "kdpgfx.h"
#include <math.h>

UBYTE *text1;
UBYTE *text2;
int light[41*26];
int blokx[41*26];
int bloky[41*26];
UBYTE ltab[512];
UBYTE vm1[64000];
UBYTE vm2[64000];
UBYTE pal[256*3];
//UWORD *color00=(UWORD *) 0xDFF180;



void make_tuntab(float persp,float po,float rot,float ox,float oy,float mulx,float muly,float tox,float toy)
{
  float xx, yy;
  int x,y;
  float vx, vy;
  float pe;
  float mx;
  float dvx=0;
  int l;
  int blk;
  for (y=0; y<26; y++)
    for (x=0; x<41; x++)
    {
      xx=(tox+x-20);
      yy=(toy+y-13);
      vx=atan2(xx,yy)*((256/(2*PI)));
      pe=mulx+(sin(rot+(2*PI*vx/(256/5)))*po);
      mx=sqrt((xx*xx)+(yy*yy));
      if (mx!=0)
        vy=pe/mx;
      
      blk=y*41+x;   
      blokx[blk]=(vx+ox+dvx)*256;
      bloky[blk]=(vy+oy)*256;
    }
}

void stampblok(UBYTE *vmem,UBYTE *txt)
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

       
        o=320*(y2<<3)+(x2<<3);
        for (y=0; y<=7; y++)
          {
        
          tx = tx1; ty = ty1;
          dx = ((tx2-tx1)) >> 3;
          dy = ((ty2-ty1)) >> 3;

          if (dx>16*65536) dx=-(32*65536-dx);
          if (dx<-(16*65536)) dx=32*65536-dx;
          for (x=0; x<=7; x++ )
            {
            vmem[o] = txt[(((ty>>8) & 0x0000FF00)+(tx >> 16)) & 0xFFFF]>>1;
            //vmem[o] = ltab[ txt[(((ty >>8) & 0x0000FF00)+(tx >> 16)) & 0xFFFF]+(UBYTE)(lx3>>16)];
            tx += dx;
            ty += dy;
           
            o++;
            }
          tx1 += dx1;
          ty1 += dy1;
      
          tx2 += dx2;
          ty2 += dy2;
      
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
  float mmx;
  float rot;
  float tox,toy;
  float doy;
  float qx,qy;
  float l;
  float sl;
  text1=KDPloadFile("raw/tg-1.raw");
  text2=KDPloadFile("raw/tg-2.raw");
  //pal=KDPloadFile("tun-l.pal");
  for(i=0;i<512;i++)
    {
    l=i/2;
    //sl=((PI/2)/255)*i;
    //l=(sin(sl)*512)-200;
    if (l<0) l=0;
    if (l>127) l=127;
    
    ltab[i]=l;
    //ltab[i+256]=255;
    }
  if (KDPopen(&screen))
    {
    vmem=screen.vmem;
    KDPgetMouse(&mouse);
    mouse.button=0;
    mx=2;my=8;
    mmx=1.8;
    mulx=900;muly=0;
    //KDPsetPal(&pal[0x30]);
    for(i=0;i<128;i++)
      {
      KDPsetColor(i,i,i/4,i);
      KDPsetColor(128+i,0,i,i*2);
      }
    KDPpalRange(0,0,0,
                50,128,100,
                0,128,pal);
    KDPpalRange(0,0,0,
                55,60,255,
                128,255,pal);
    KDPsetPal(pal);
    while(mouse.button!=3)
      {
      if (mouse.button==1)
        {
        mx+=(float)mouse.xspeed/10;
        my+=mouse.yspeed;
        //mx=0;
        }
      if (mouse.button==0)
        {
        ox+=(float)mouse.xspeed/10;
        doy+=(float)mouse.yspeed/50;
        }
      if (mouse.button==2)
        {
        //mulx+=(float)mouse.xspeed;
        //muly+=(float)mouse.yspeed/100;
        mmx+=(float)mouse.xspeed/100;
        /*tox-=(float)mouse.xspeed/10;
        toy-=(float)mouse.yspeed/10;*/
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
      ox+=0.5;
      //ox=rot*4;
      //mx=2+sin(rot*2)*1.8;
      //mx+=0.03;
      //my+=(float)mouse.yspeed/50;
      
      /*for(x=0;x<41;x++)
        for(y=0;y<26;y++)
          {
          blokx[x][y]=(x-20)*mx;
          bloky[x][y]=(y-13)*my;
          } */
      
      make_tuntab(mx,0,rot,ox*-2,oy,mulx,muly,tox,toy);
      stampblok(vmem,text1);
      make_tuntab(mx,my,rot,ox*5,oy,mulx*mmx,muly,tox,toy);
      stampblok(vm1,text2);
      make_tuntab(mx,my,rot,ox*-2,oy,mulx*1.3,muly,tox,toy);
      stampblok(vm2,text1);
      
      for(i=0;i<64000;i++)
        if (vm2[i]>mx) vm1[i]=128-ltab[80+vm1[i]];
      
      //for(i=0;i<64000;i++)
      //  if (vmem[i]<mx) vmem[i]=vm2[i];
      
      for(i=0;i<64000;i++)
        if (vmem[i]<mx) vmem[i]=vm1[i]+128;
          
      //KDPsetColor(0,0,0,0);
      KDPwaitVr();
      //KDPsetColor(0,255,0,0);
      KDPshow(vmem);
      KDPgetMouse(&mouse);
      }
    }
  KDPclose(&screen);
}


#include "kdp.h"
#include "kdpgfx.h"
//#include <stdio.h>
#include <math.h>

unsigned char sprite[256*256];
unsigned char light[256*256];
unsigned char picture[64000];
unsigned char palette[960];
unsigned char *vmem;
UBYTE pic[64000];

void make_sprite(float m)
{
  int x, y, val;

  for (y=-128; y<128; y++)
    for (x=-128; x<128; x++)
    {
      val=255-(sqrt((x*x)+(y*y))*m);
      if (val>0)
        sprite[256*(y+128)+(x+128)]=val;
      else
        sprite[256*(y+128)+(x+128)]=0;
    }
}

void load_pictures()
{
  KDPreadBMP("bmp/shade1.bmp",palette,picture);
  KDPreadBMP("bmp/shade2.bmp",palette,pic);
}

void make_lightpicture(int xpos, int ypos)
{
  int x, y;
  int xx,yy,sx,sy;
  memset(light, 0, 256*256);
  for (y=0; y<256; y++)
    for (x=0; x<256; x++)
      {
      light[(320*(ypos+y)+(xpos+x))& 0xffff]=sprite[256*y+x];
      }
}

void shade_picture()
{
  int x, y;
  int xdelta, ydelta;
  int o;

  UBYTE *vm=vmem;
  UBYTE *p=picture;
  UBYTE *pc=pic;

    vm+=320;
    p+=320;
    for(o=320;o<64000;o++)
    {
      //xdelta=picture[o-1]-picture[o];
      //ydelta=(picture[o-320]-picture[o])*320;
      //vmem[o]=KDPcgcoltab[(pic[o]<<8)+light[(o+xdelta+ydelta)&0xffff]];
      xdelta=(*(p-1))-(*p);
      ydelta=((*(p-320))-(*p))*320;
      p++;
      *(vm++)=KDPcgcoltab[((*(pc++))<<8)+light[(o+xdelta+ydelta)&0xffff]];
  }
    for(o=0;o<320;o++)
      vmem[o]=0;
}

main()
{
  KDPscreen screen;
  KDPmouse  mouse;
  int i=0;
  float m;
  float dm;
  float mx,my;
  int dz=1;
  float xx,yy;
  make_sprite(3);
  load_pictures();

  KDPmakecoltab1(palette);

if (KDPopen(&screen))
    {
    KDPsetPal(palette);
    vmem=screen.vmem;
    mouse.button=0;
    mx=160;my=100;
    while(mouse.button!=3)
      {
      make_lightpicture(mx, my);
      shade_picture();
      KDPwaitVr();
      KDPshow(vmem);
      KDPgetMouse(&mouse);
      
      if (mouse.button!=1)
        {
        mx+=sin(xx)*2;
        my+=sin(yy)*2;
        }
      else
        {
        mx+=mouse.xspeed;
        my+=mouse.yspeed;
        } 
      xx+=0.01;
      yy+=0.003;
      }
    }
  KDPclose(&screen);
}



#include "kdp.h"
#include "kdpgfx.h"
#include <math.h>
 #include <stdlib.h>

#define NUM 10000
float partx[NUM];
float party[NUM];
float partvx[NUM];
float partvy[NUM];
float grv[NUM];  
UBYTE pal[256*3];
main()
{
  KDPscreen screen;
  KDPmouse mouse;
  UBYTE *vmem;
  float grav=2;
  float ox,oy;
  float grx,gry;
  int n;
  int i,j;
  int x,y;
  int tel,dir;
  float xdif,ydif,len; 
  float pgrav,ograv;
  if (!(KDPopen(&screen)))
    {
    printf("kdp failed!\n");
    KDPclose(&screen);
    return;
    }
  
  vmem=screen.vmem;
  KDPpalRange(0,0,0,
              255,0,0,
              0,100,pal);
  KDPpalRange(255,0,0,
              255,255,0,
              100,200,pal);
  KDPpalRange(255,255,0,
              255,255,255,
              200,255,pal);
  KDPsetPal(pal);
               
  KDPgetMouse(&mouse);
  ox=0;oy=0; 
  i=0;
  dir=0;
  pgrav=20;
  ograv=20;
  for(i=0;i<NUM;i++)
  {
  partx[i]=rand()%320;
  party[i]=rand()%200;
  partvx[i]=0;partvy[i]=0;
  grv[i]=(rand()%1000)/150;
   }


  while(mouse.button!=3)
    {
    ox+=mouse.xspeed;
    oy+=mouse.yspeed;  
    if (mouse.button==0)
      ograv=20;
    if (mouse.button==1)
      for(i=0;i<NUM;i++)
        {
        partx[i]=rand()%320;
        party[i]=rand()%200;
        partvx[i]=0;partvy[i]=0;
        grv[i]=(rand()%1000)/150;
        }
    if (mouse.button==2)
      ograv=14;
    if (tel==1) dir=1-dir;
    
     
     for(i=0;i<NUM;i++)
        {
        xdif=ox-partx[i];
        ydif=oy-party[i];
        len=sqrt(xdif*xdif+ydif*ydif);
        len*=ograv;
        if (len!=0)
          {
          if (dir==1)
            {
            partvx[i]-=xdif/len;
            partvy[i]-=ydif/len;
            }
          if (dir==0)
            {
            partvx[i]+=xdif/len;
            partvy[i]+=ydif/len;
            }
          }
        }   
     for(i=0;i<NUM;i++)
       {
       partx[i]+=partvx[i];
       party[i]+=partvy[i];
       partvy[i]+=0.05;
       if (party[i]>199)
         {
         party[i]=199;
         partvy[i]=-(partvy[i]/2);
         }
     
       KDPaddpset(partx[i],party[i],50,vmem);
       }
    KDPcircle(ox,oy,4,4,255,vmem);      
    KDPblur(3,vmem);
    KDPwaitVr();
    KDPshow(vmem);
    KDPgetMouse(&mouse);
    }

  KDPclose(&screen);
}

    
    
    
        
    
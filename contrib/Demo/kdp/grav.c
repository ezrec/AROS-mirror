
#include "kdp.h"
#include "kdpgfx.h"
#include <math.h>
 
#include <stdlib.h>

#define NUM 64000
float partx[NUM];
float party[NUM];
float partvx[NUM];
float partvy[NUM];
float grv[NUM];  

main()
{
  KDPscreen screen;
  KDPmouse mouse;
  UBYTE *vmem;
  float grav=2;
  float ox,oy;
  float grx,gry;
  int n;
  int i,j,ii;
  UBYTE pal[256*3];
  int x,y;
  int tel,dir;
  UBYTE sp[256],sp2[256];
  float xdif,ydif,len; 
  float pgrav,ograv;
  if (!(KDPopen(&screen)))
    {
    printf("kdp failed!\n");
    KDPclose(&screen);
    return;
    }
  
  KDPpalRange(0  ,0  ,0  ,
              58 ,191,241,
              0,200,pal);
  KDPpalRange(58, 191,241,
              255,255,255,
              200,255,pal);                          
  for(ii=0;ii<16;ii++)
    for(i=0;i<16;i++)
      {
      len=8-sqrt(((ii-8)*(ii-8)+(i-8)*(i-8)));
      len*=8;
      if (len<0) len=0;
      if (len>255) len=255;
      sp[ii*16+i]=len;
      }
  for(ii=0;ii<16;ii++)
    for(i=0;i<16;i++)
      {
      len=8-sqrt(((ii-8)*(ii-8)+(i-8)*(i-8)));
      len*=32;
      if (len<0) len=0;
      if (len>255) len=255;
      sp2[ii*16+i]=len;
      } 
  KDPsetPal(pal); 
  
  
  vmem=screen.vmem;
   
  KDPgetMouse(&mouse);
  ox=0;oy=0; 
  i=0;
  n=500;
  dir=0;
  pgrav=20;
  ograv=50; 
  while(mouse.button!=3)
    {
    KDPcls(vmem);
    //KDPblur(2,vmem);
    ox+=mouse.xspeed;
    oy+=mouse.yspeed;  
    if (mouse.button==0)
      tel=0;
      //ograv=20;
    if (mouse.button==1)
      for(i=0;i<n;i++)
        {
        partx[i]=rand()%320;
        party[i]=rand()%200;
        partvx[i]=0;partvy[i]=0;
        grv[i]=(rand()%1000)/150;
        }
    if (mouse.button==2)
      tel++;
      //ograv=14;
    if (tel==1) dir=1-dir;
     
     for(i=0;i<n;i++)
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
          else
            {
            partvx[i]+=xdif/len;
            partvy[i]+=ydif/len;
            }
          }
        }   
     for(i=0;i<n;i++)
       {
       partx[i]+=partvx[i];
       party[i]+=partvy[i];
       KDPaddsp(partx[i],party[i],sp,vmem);
       //KDPpset(partx[i],party[i],255,vmem);
       }
    KDPsp(ox,oy,sp2,vmem);      
    KDPwaitVr();
    KDPshow(vmem);
    KDPgetMouse(&mouse);
    }

  KDPclose(&screen);
}

    
    
    
        
    
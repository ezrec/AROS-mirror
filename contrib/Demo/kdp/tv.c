
#include "kdp.h"
#include "kdpgfx.h"
#include <math.h>
 #include <stdlib.h>

UBYTE vm2[64000];
UBYTE tv[64000];

main()
{
  KDPscreen screen;
  KDPmouse mouse;
  UBYTE *vmem;
  UBYTE *vm; 
  UBYTE kl;
  int t;
  int ofs;
  int tt;
  int t2;
  int i;
  if (!(KDPopen(&screen)))
    {
    printf("kdp failed!\n");
    KDPclose(&screen);
    return;
    }
  
  vmem=screen.vmem;
      
  KDPgetMouse(&mouse);
  KDPsetColor(255,0,255,0); 
  KDPsetColor(254,0,255,0);
  ofs=3;
  KDPreadBMP("bmp/tv.bmp",0,tv);
  memcpy(vmem,tv,64000);
  tt=0;
  while(mouse.button!=3)
    {
   vm=vmem;
   if((rand()%30)==0)
    {
    kl=rand()%255;
    for(i=0;i<64000;i++)
     {
     if(*vm!=255) *vm=kl; 
     vm++;
     }
   }   
   else
   {
   for(i=0;i<64000;i++)
     {
     if(*vm!=255) *vm=rand()%254; 
     vm++;
     }
   }
   //KDPblur2(vmem);
   //KDPsetColor(0,rand()%255,rand()%255,rand()%255);
    t++;
    if(t>130)
      {
      t=0;
      for(i=0;i<3;i++)
        KDPline(ofs+i,130,ofs+i,146,255,vmem);
      ofs+=5;
      if(ofs>310)
        {
        ofs=3;
        memcpy(vmem,tv,64000);
        }
      } 
    t2++;
    if(t2>20) 
      {
      t2=0;
      tt=1-tt;
      }
    if(tt==1)
      {
      for(i=(160*329);i<64000;i++)
        if(tv[i]!=0) vmem[i]=254;
      }
    KDPwaitVr();
    KDPshow(vmem);
    KDPgetMouse(&mouse);
    }

  KDPclose(&screen);
}

    
    
    
        
    

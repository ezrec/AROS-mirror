#include <math.h>

#include "kdp.h"
#include "kdpgfx.h"
#include <stdlib.h>

float x[5000],y[5000],z[5000];
float xb[5000],yb[5000],zb[5000];
int sx[5000],sy[5000];
float size[5000];

UBYTE tex[256*256];

int nump,numf;

int load3d(char *filename)
{
  UBYTE *file;
  int pointo;
  int faceo;
  int i;
  float xofs,yofs,zofs;

  if (!(file=KDPloadFile(filename)))
    return 0;
  if (KDPgetMlong(&file[0])!=0x33445257)
    return 0;
  pointo=KDPgetMlong(&file[6]);
  numf=KDPgetMword(&file[10])+1;
  nump=KDPgetMword(&file[pointo])+1;
  faceo=12;
  
  pointo+=2;
  for(i=0;i<nump;i++)
    {
    x[i]=(float)KDPgetMword(&file[pointo])/1000;
    y[i]=(float)KDPgetMword(&file[pointo+2])/1000;
    z[i]=(float)KDPgetMword(&file[pointo+4])/1000;
    pointo+=6;
    }
  xofs=0;yofs=0;zofs=0;
  for(i=0;i<nump;i++)
    {
    xofs+=x[i];
    yofs+=y[i];
    zofs+=z[i];
    }
  xofs/=nump;
  yofs/=nump;
  zofs/=nump;
  for(i=0;i<nump;i++)
    {
    x[i]-=xofs;
    y[i]-=yofs;
    z[i]-=zofs;
    }
  return 1;
}





void project(float zx)
{
  int i;
  float zz;
  for(i=0;i<nump;i++)
    {
    zz=-(zb[i]-zx);
    if (zz!=0)
      {
      sx[i]=(int)((xb[i]*256)/zz)+160;
      sy[i]=(int)((yb[i]*256)/zz)+100;
      size[i]=256/zz;
      }
    }
}

void rotate(float rx,float ry,float rz)
{
  int i;
  float xx,xy,xz;
  float yx,yy,yz;
  float zx,zy,zz;
  float sx,sy,sz;
  float cx,cy,cz;
  sx=sin(rx);cx=cos(rx);
  sy=sin(ry);cy=cos(ry);
  sz=sin(rz);cz=cos(rz);
  xx=cy*cz;          xy=cy*sz;          xz=-sy;
  yx=sx*sy*cz-cx*sz; yy=sx*sy*sz+cx*cz; yz=sx*cy;
  zx=cx*sy*cz+sx*sz; zy=cx*sy*sz-sx*cz; zz=cx*cy;
  for(i=0;i<nump;i++)
    {
    xb[i]=x[i]*xx + y[i]*xy + z[i]*xz;
    yb[i]=x[i]*yx + y[i]*yy + z[i]*yz;
    zb[i]=x[i]*zx + y[i]*zy + z[i]*zz;
    }
}


int main(int argc,char **argv)
	{
	KDPscreen screen;
	KDPmouse mouse;
	UBYTE *vmem;
	int ofs;
	UBYTE pal[2][256*3];
   UBYTE *v2;
   
  float a=0,b=0,c=0;
  float da=0,db=0,dc=0;
  int i;
  float zx=20;
  float m=1;
  int t=0,fl=0;
  float speed=0;
  float xs=2,ys;
  float qx=0,qy=0;
  int tel;
  int delx=1;
  UBYTE *ra2;

   if (argc < 2)
   {
     printf("usage: scaletest <3dobj>\n");
     return 0;
   }

  if(load3d(argv[1])==0)
	{
	printf("hmm...\n");return 0;
	}
  
  KDPpalRange(  0,  0,  0,
						150,180,220,
						 0,200,pal[0]);
  KDPpalRange(150,180,220,
						255,255,255,
						200,255,pal[0]);
  for(i=0;i<256;i++)
    pal[1][i]=pal[0][i]/2;
  xs=100+rand()%100;
  ys=100+rand()%100;
  KDPreadBMP("bmp/cir.bmp",0,tex);
tel=0;
	xs=2000;ys=2000;
	if(KDPopen(&screen))
	{
	vmem=screen.vmem;
	  KDPgetMouse(&mouse);
	  KDPgetMouse(&mouse);
	  KDPsetPal(pal[0]);
		while(mouse.button!=3)
			{
			if(mouse.button==0)
				{
			  da+=(float)mouse.xspeed/500;
			  dc+=(float)mouse.yspeed/500;
			  }
	  if(mouse.button==1) {zx+=(float)mouse.yspeed/10;db+=(float)mouse.xspeed/500;}
	  if(mouse.button==2) {speed+=(float)mouse.yspeed/50;m+=(float)mouse.xspeed/50;da=0;db=0;dc=0;}
	  if(m<1) m=1;
	  a+=da;b+=db;c+=dc;
	  rotate(a,b,c);
	  project(zx);
	  qx+=0.03;
	  qy+=0.03;
	  
	  
	   
	  for(i=0;i<nump;i++)
		  KDPsp256(sx[i],sy[i],(int)(size[i]),tex,vmem);
	  
	  KDPwaitVr();
	  

	  //KDPblur2(vmem);
	  KDPshow(vmem);
			KDPcls(vmem);
			KDPgetMouse(&mouse);
			}
		}
	  KDPclose(&screen);
    return 0;
	}

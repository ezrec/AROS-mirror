#include "kdp.h"
#include "kdpgfx.h"
#include <time.h>
#include <stdio.h>
#include <math.h>

float x[5000],y[5000],z[5000];
float xb[5000],yb[5000],zb[5000];
int sx[5000],sy[5000];
int fac1[5000],fac2[5000],fac3[5000];
UBYTE col[5000];
int nump,numf;
UBYTE pal[256*3];
UBYTE ctab[512];
UBYTE ctab2[512];
clock_t time1,time2;


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
  for(i=0;i<numf;i++)
    {
    faceo+=2;
    fac1[i]=KDPgetMword(&file[faceo]);
    faceo+=2;
    fac2[i]=KDPgetMword(&file[faceo]);
    faceo+=4;
    fac3[i]=KDPgetMword(&file[faceo]);
    faceo+=6;
    }
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


void pset(int x,int y,UBYTE c,UBYTE *vmem)
	{
	int o;
	if((x<0) || (x>319) || (y<0) || (y>199)) return;
	o=y*320+x;
	vmem[o]=ctab[vmem[o]+c];
	}

void pset2(int x,int y,UBYTE c,UBYTE *vmem)
	{
	int o;
	if((x<0) || (x>318) || (y<0) || (y>198)) return;
	o=y*320+x;
	vmem[o]=ctab[vmem[o]+c];
	vmem[o+1]=ctab[vmem[o+1]+c];
	vmem[o+320]=ctab[vmem[o+320]+c];
	vmem[o+321]=ctab[vmem[o+321]+c];
	}

void sub(UBYTE *vmem)
	{
	int o;
	for(o=0;o<64000;o++)
	  vmem[o]--;;
	}

int main(int argc,char **argv)
	{
	KDPscreen screen;
	KDPmouse mouse;
	UBYTE *vmem;
	UBYTE *samp=(UBYTE *) 0xBFE101;
  float a=0,b=0,c=0;
  float da,db,dc;
  float zx=20;
  int i;
  int frame=0;
  float secs;
  int ia,ib,ic;
  int kl;
  int x1,x2,y1,y2;

   if (argc < 2)
   {
     printf("usage: wire <3dobj>\n");
     return 0;
   }

	for(i=0;i<512;i++)
		{
		if(i>255) ctab[i]=255;
		else      ctab[i]=i;
		}
	for(i=0;i<256;i++)
		ctab2[511-i]=i;
  for(i=0;i<256;i++)
    ctab2[i]=0;

	if(!(load3d(argv[1])))
    {
    printf("hmmm..!\n");
    return 0;
    }


	if(KDPopen(&screen))
		{

		KDPpalRange(0,0,0,
			    255,255,200,
			    0,255,pal);
		KDPsetPal(pal);
		vmem=screen.vmem;
	  KDPgetMouse(&mouse);
	  KDPgetMouse(&mouse);
		time1=clock();
		da=0;db=0;dc=0;
		KDPsetColor(0,30,30,50);
		while(mouse.button!=3)
			{
			if(mouse.button==0)
				{
				dc+=(float)mouse.yspeed/500;
				db+=(float)mouse.xspeed/500;
				}
			if(mouse.button==1)
				{
				da+=(float)mouse.xspeed/500;

				kl+=mouse.yspeed;
				if(kl<0) kl=0;
				if(kl>255) kl=255;
				}
		  if(mouse.button==2)
			  {
			  da=0;db=0;dc=0;
			  zx+=(float)mouse.yspeed/10;
			  }
      a+=da;
			b+=db;
			c+=dc;
			//sub(vmem);
			//KDPblur(2,vmem);
			KDPcls(vmem);
			rotate(a,b,c);
			project(zx);
            KDPpset2(kl,0,255,vmem);

			for(i=0;i<numf;i++)
				{
				ia=fac1[i];
				ib=fac2[i];
				ic=fac3[i];
				if ((zb[ia]<zx) && (zb[ib]<zx) && (zb[ic]<zx))
					{
				  KDPline2(sx[ia],sy[ia],sx[ib],sy[ib],kl,vmem);
				  KDPline2(sx[ib],sy[ib],sx[ic],sy[ic],kl,vmem);
			    KDPline2(sx[ic],sy[ic],sx[ia],sy[ia],kl,vmem);
			    }
			  }
			KDPwaitVr();
			KDPshow(vmem);
			KDPgetMouse(&mouse);
			frame++;
			}
		}
		time2=clock();
		secs=(float)(time2-time1)/(float)CLOCKS_PER_SEC;
		printf("frames: %d time:%f  fps:%f\n",frame,secs,(float)frame/secs);
		KDPclose(&screen);
	return 0;
	}

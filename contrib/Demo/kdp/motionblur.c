
#include "kdp.h"
#include "kdpgfx.h"
//#include "kdppoly.h"
#include <time.h>
#include <stdio.h>
#include <math.h>

float x[5000],y[5000],z[5000];
float xb[5000],yb[5000],zb[5000];
int size[5000];
int sx[5000],sy[5000];
int fac1[5000],fac2[5000],fac3[5000];
UBYTE col[5000];
int nump,numf;
UBYTE pal[256*3];
UBYTE ctab[512];
UBYTE ctab2[512];
clock_t time1,time2;
LONG rad1[5000],rad2[5000];
LONG zsrt1[5000],zsrt2[5000];
UBYTE vme[16][64000];
//KDPvertex vert[3];
UBYTE tex[256*256];




void radix(int byte, LONG N,LONG *source,LONG *dest,LONG *srts,LONG *srtd)
  {
  int i;
  int w;
  LONG count[256];
  LONG index[256];
  memset(count,0,sizeof(count));
  for(i=0;i<N;i++) count[(source[i]>>(byte*8))& 0xff]++;
  index[0]=0;
  for(i=1;i<256;i++) index[i]=index[i-1]+count[i-1];
  for(i=0;i<N;i++)
    {
    w=index[(source[i]>>(byte*8))& 0xff]++;
    dest[w]=source[i];
    srtd[w]=srts[i];
    }
  }

void radixsort(LONG *source,LONG *temp, LONG N, LONG *srts,LONG *srtd)
  {
  radix(0,N,source,temp,srts,srtd);
  radix(1,N,temp,source,srtd,srts);
  radix(2,N,source,temp,srts,srtd);
  radix(3,N,temp,source,srtd,srts);
  }




int load3d(char *filename)
{
  UBYTE *file;
  int pointo;
  int faceo;
  int i;
  float xofs,yofs,zofs;
  //nump=40;
  /*for(i=0;i<nump;i++)
  {
    x[i]=(10000-(float)(rand()%20000))/1000;
    y[i]=(10000-(float)(rand()%20000))/1000;
    z[i]=(10000-(float)(rand()%20000))/1000;
  } 
  return 1;*/
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
  printf("points:%d   faces:%d \n",nump,numf);
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
      size[i]=(256/zz);
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
	  vmem[o]--;
	}


void addbuffers(UBYTE *vmem)
	{
	int i;
	UBYTE *v0,*v1,*v2,*v3,*v4;
	v0=vmem;
	v1=vme[0];
	v2=vme[1];
	v3=vme[2];
	v4=vme[3];

	for(i=0;i<64000;i++)
		*(v0++)=(*(v1++)+*(v2++)+*(v3++)+*(v4++))>>2;
  }

void addbuffers8(UBYTE *vmem)
	{
	int i;
	UBYTE *v0,*v1,*v2,*v3,*v4,*v5,*v6,*v7,*v8;
	v0=vmem;
	v1=vme[0];
	v2=vme[1];
	v3=vme[2];
	v4=vme[3];
  v5=vme[4];
  v6=vme[5];
  v7=vme[6];
  v8=vme[7];


	for(i=0;i<64000;i++)
		*(v0++)=(*(v1++)+*(v2++)+*(v3++)+*(v4++)+*(v5++)+*(v6++)+*(v7++)+*(v8++))>>3;
  }
void addbuffers16(UBYTE *vmem)
	{
	int i;
	UBYTE *v0,*v1,*v2,*v3,*v4,*v5,*v6,*v7,*v8,*v9,*v10,*v11,*v12,*v13,*v14,*v15,*v16;
	v0=vmem;
	v1=vme[0];
	v2=vme[1];
	v3=vme[2];
	v4=vme[3];
    v5=vme[4];
    v6=vme[5];
    v7=vme[6];
    v8=vme[7];
    v9=vme[8];
	v10=vme[9];
	v11=vme[10];
	v12=vme[11];
    v13=vme[12];
    v14=vme[13];
    v15=vme[14];
    v16=vme[15];
             
              

	for(i=0;i<64000;i++)
		*(v0++)=(*(v1++)+*(v2++)+*(v3++)+*(v4++)+*(v5++)+*(v6++)+*(v7++)+*(v8++)+*(v9++)+*(v10++)+*(v11++)+*(v12++)+*(v13++)+*(v14++)+*(v15++)+*(v16++))>>4;
  }

int main(int argc,char **argv)
	{
	KDPscreen screen;
	KDPmouse mouse;
	UBYTE *vmem;
  float a=0,b=0,c=0;
  float da,db,dc;
  float zx=20;
  int i,ii;
  UBYTE sp1[16*16],sp2[16*16];
  int frame=0;
  float secs;
  float rzx;
  int blr;
  int ia,ib,ic;
  int frm;
  int tel=0,flip=0;
  //int ia,ib,ic;
  int kl;
  int col;
  float speed;
  float dz;

   if (argc < 2)
   {
     printf("usage: motionblur <3dobj>\n");
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


		KDPreadBMP("bmp/brush1.bmp",pal,sp1);

		KDPpalRange(0,0,0,
			    50,150,255,
			    0,200,pal);
		KDPpalRange(50,150,255,
			    255,255,255,
			    200,255,pal);

		KDPsetPal(pal);

        
		for(i=0;i<16*16;i++)
			sp2[i]=sp1[i]/5;

		vmem=screen.vmem;
	  KDPgetMouse(&mouse);
	  KDPgetMouse(&mouse);
		time1=clock();
		da=0;db=0;dc=0;
		KDPreadBMP("bmp/cir.bmp",0,tex);
		//for(i=0;i<256*256;i++)
        //tex[i]=KDPcoltab[tex[i]+10];
		while(mouse.button!=3)
			{
			if(mouse.button==0)
				{
				dc+=(float)mouse.yspeed/500;
				db+=(float)mouse.xspeed/500;
				blr=0;
				dz=0;
				}
			if(mouse.button==1)
				{
				da+=(float)mouse.xspeed/500;
	blr=1;
				kl+=mouse.yspeed;
				if(kl<0) kl=0;
				if(kl>255) kl=255;
				}
		  if(mouse.button==2)
			  {
			  tel++;
			  da=0;db=0;dc=0;
			  speed+=(float)mouse.xspeed/500;
			  dz=(float)mouse.yspeed/50;
			  }
			else
			  tel=0;

      if(tel==1) flip=1-flip;


      for(frm=0;frm<16;frm++)
	    {
	          a+=da;
			  b+=db;
			  c+=dc;
			  zx+=(dz);
			  rzx=zx+(frm*0.3);
			  //sub(vmem);
			  //KDPblur(2,vmem);
			  KDPcls(vme[frm]);
			  for(i=0;i<nump;i++)
			    {
			    x[i]+=speed;
			    if(x[i]<-10) x[i]=10;
			    if(x[i]>10) x[i]=-10;
			    }
			  rotate(a,b,c);
			  project(zx);//+((float)(frm-4)/2));

		
	          for(i=0;i<nump;i++)
	            if(zb[i]!=0)
	              KDPsp256(sx[i],sy[i],size[i],tex,vme[frm]);
	        }
			a-=da*7;
			b-=db*7;
			c-=dc*7;
			/*for(i=0;i<nump;i++)
			    {
			    x[i]-=(speed*7);
			    if(x[i]<-10) x[i]=10;
			    if(x[i]>10) x[i]=-10;
			    }
			*/
			addbuffers16(vmem);
			//KDPblur2(vmem);
			KDPwaitVr();
			KDPshow(vmem);
			KDPgetMouse(&mouse);
			KDPcls(vmem);
			frame++;
			}
		}
		time2=clock();
		KDPwriteBMP("bmp/test.bmp",pal,vmem,320,200);
		secs=(float)(time2-time1)/(float)CLOCKS_PER_SEC;
		printf("frames: %d time:%f  fps:%f\n",frame,secs,(float)frame/secs);
		KDPclose(&screen);
	return 0;
	}

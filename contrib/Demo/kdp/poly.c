#include "kdp.h"
#include "kdpgfx.h"
#include "kdppoly.h"
#include <time.h>
#include <stdio.h>
#include <math.h>

#include <proto/dos.h>


KDPvertex vert[5000];
KDPface fac[5000];
float x[5000],y[5000],z[5000];
float rx[5000],ry[5000],rz[5000];
float normx[5000],normy[5000],normz[5000];
float nx[5000],ny[5000],nz[5000];
float pnx[5000],pny[5000],pnz[5000];
float rnx[5000],rny[5000],rnz[5000];
int nump,numf;

UBYTE pal[256*3];
UBYTE tex[256*256];

LONG rad1[5000],rad2[5000];
LONG zsrt1[5000],zsrt2[5000];

clock_t time1,time2;

int flip, nomouserot;
float a=0.0,b=0.0,c=0.0;
float da=0.0,db=0.0,dc=0.0;

#define ARG_TEMPLATE "OBJ/A,WINPOSX=X/N/K,WINPOSY=Y/N/K,DRAWMODE=M/N/K," \
    	    	     "XANGLE=XA/N/K,YANGLE=YA/N/NK,ZANGLE=ZA/N/K,XROT=XR/N/K," \
		     "YROT=YR/N/K,ZROT=ZR/N/K,NOMOUSEROT=NMR/S"

#define ARG_OBJ      	0
#define ARG_X 	     	1
#define ARG_Y 	     	2
#define ARG_DRAWMODE 	3
#define ARG_XANGLE   	4
#define ARG_YANGLE   	5
#define ARG_ZANGLE   	6
#define ARG_XROT     	7
#define ARG_YROT     	8
#define ARG_ZROT     	9
#define ARG_NOMOUSEROT  10
#define NUM_ARGS     	11

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
	
	if (args[ARG_DRAWMODE])
	{
	    int i = (int)*(IPTR **)args[ARG_DRAWMODE];
	    
	    if ((i >= 0) && (i <= 8)) flip = i;
	}
	
	if (args[ARG_XANGLE])
	{
	    a = (float)(*(IPTR *)args[ARG_XANGLE]);
	    a = a * 3.14159265358979 / 180.0;
	}

	if (args[ARG_YANGLE])
	{
	    b = (float)(*(IPTR *)args[ARG_YANGLE]);
	    b = b * 3.14159265358979 / 180.0;
	}

	if (args[ARG_ZANGLE])
	{
	    c = (float)(*(IPTR *)args[ARG_ZANGLE]);
	    c = c * 3.14159265358979 / 1800.0;
	}

	if (args[ARG_XROT])
	{
	    da = (float)(*(IPTR *)args[ARG_XROT]);
	    da = da * 3.14159265358979 / 1800.0;
	}

	if (args[ARG_YROT])
	{
	    db = (float)(*(IPTR *)args[ARG_YROT]);
	    db = db * 3.14159265358979 / 1800.0;
	}

	if (args[ARG_ZROT])
	{
	    dc = (float)(*(IPTR *)args[ARG_ZROT]);
	    dc = dc * 3.14159265358979 / 1800.0;
	}
	
	if (args[ARG_NOMOUSEROT])
	{
	    nomouserot = 1;
	}
	
    	FreeArgs(myargs);
    }
}

void calcnorm(void)
{
  int i;
  int ia,ib,ic;
  float len;
  float relx1,rely1,relz1,relx2,rely2,relz2;
  for(i=0;i<numf;i++)
    {
    ia=fac[i].a;
    ib=fac[i].b;
    ic=fac[i].c;
    relx1=x[ib]-x[ia];
    rely1=y[ib]-y[ia];
    relz1=z[ib]-z[ia];
    relx2=x[ic]-x[ia];
    rely2=y[ic]-y[ia];
    relz2=z[ic]-z[ia];
    normx[i]=rely1*relz2-relz1*rely2;
    normy[i]=relz1*relx2-relx1*relz2;
    normz[i]=relx1*rely2-rely1*relx2;
    len=sqrt( (normx[i]*normx[i])+(normy[i]*normy[i])+(normz[i]*normz[i]) );
    normx[i]/=len;
    normy[i]/=len;
    normz[i]/=len;
    //if(normz[i]<0) normz[i]=-normz[i];
    }
}

void calcpointnorm(void)
{
  int i;
  int j;
  float px,py,pz;
  int tel;
  float len;
  float nx,ny,nz;
  for(i=0;i<nump;i++)
    {
    px=0;py=0;pz=0;
    tel=0;
    for(j=0;j<numf;j++)
      if ( (fac[j].a==i) | (fac[j].b==i) | (fac[j].c==i))
	{
	px+=normx[j];
	py+=normy[j];
	pz+=normz[j];
	tel++;
	}
    if (tel!=0)
      {
      nx=px/3;
      ny=py/3;
      nz=pz/3;
      len=sqrt( (nx*nx) + (ny*ny) + (nz*nz));
      pnx[i]=(nx/len);
      pny[i]=(ny/len);
      pnz[i]=(nz/len);
      }
    }
}


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
    w=index[(source[i]>>(byte<<3))& 0xff]++;
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




void calctexture(void)
	{
	int i;
	for(i=0;i<nump;i++)
		{
		vert[i].u=(x[i]+10)*12;
		vert[i].v=(z[i]+10)*12;
		}
  }

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
    fac[i].a=KDPgetMword(&file[faceo]);
    faceo+=2;
    fac[i].b=KDPgetMword(&file[faceo]);
    faceo+=4;
    fac[i].c=KDPgetMword(&file[faceo]);
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

  calctexture();
  calcnorm();
  calcpointnorm();
  printf("points:%d   faces:%d \n",nump,numf);
  return 1;
}





void project(float zx)
{
  int i;
  float zz;
  for(i=0;i<nump;i++)
    {
    zz=-(rz[i]-zx);
    if (zz!=0)
      {
      vert[i].x=(int)((rx[i]*256)/zz)+160;
      vert[i].y=(int)((ry[i]*256)/zz)+100;
      vert[i].z=rz[i];
      }
    }
}

void rotate(float tx,float ty,float tz)
{
  int i;
  float xx,xy,xz;
  float yx,yy,yz;
  float zx,zy,zz;
  float sx,sy,sz;
  float cx,cy,cz;
  sx=sin(tx);cx=cos(tx);
  sy=sin(ty);cy=cos(ty);
  sz=sin(tz);cz=cos(tz);
  xx=cy*cz;          xy=cy*sz;          xz=-sy;
  yx=sx*sy*cz-cx*sz; yy=sx*sy*sz+cx*cz; yz=sx*cy;
  zx=cx*sy*cz+sx*sz; zy=cx*sy*sz-sx*cz; zz=cx*cy;
  for(i=0;i<nump;i++)
    {
    rx[i]=x[i]*xx + y[i]*xy + z[i]*xz;
    ry[i]=x[i]*yx + y[i]*yy + z[i]*yz;
    rz[i]=x[i]*zx + y[i]*zy + z[i]*zz;
    }
}


void rotatenorm(float rx,float ry,float rz)
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
  for(i=0;i<numf;i++)
    {
    nx[i]=normx[i]*xx + normy[i]*xy + normz[i]*xz;
    ny[i]=normx[i]*yx + normy[i]*yy + normz[i]*yz;
    nz[i]=normx[i]*zx + normy[i]*zy + normz[i]*zz;
    }
}


void rotatepnorm(float rx,float ry,float rz)
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
    rnx[i]=pnx[i]*xx + pny[i]*xy + pnz[i]*xz;
    rny[i]=pnx[i]*yx + pny[i]*yy + pnz[i]*yz;
    rnz[i]=pnx[i]*zx + pny[i]*zy + pnz[i]*zz;
    vert[i].u=128+rnx[i]*128;
    vert[i].v=128+rny[i]*128;
    }
}


int main(int argc,char **argv)
	{
	KDPscreen screen;
	KDPmouse mouse;
	UBYTE *vmem;
  float zx=20;
  int i,ii;
  int frame=0;
  float secs;
  int blr;
  int col;
  int tel=0;
  //int ia,ib,ic;
  float dz;
  int rem=0;
  int rt=0;

   if (argc < 2)
   {
     printf("usage: poly <3dobj>\n");
     return 0;
   }

   getarguments();
   
	if(!(load3d(argv[1])))
    {
    printf("hmmm..!\n");
    return 0;
    }

  KDPreadBMP("bmp/env.bmp",pal,tex);

	KDPmakecoltab1(pal);


	if(KDPopen(&screen))
		{
		KDPsetPal(pal);

    KDPsetColor(0,20,40,80);

		vmem=screen.vmem;
	  KDPgetMouse(&mouse);
	  KDPgetMouse(&mouse);
		time1=clock();
		//da=0;db=0;dc=0;


    for(i=0;i<numf;i++)
	{
      fac[i].type=0;
      fac[i].texture=tex;
      fac[i].type=flip;

      }


		while(mouse.button!=3)
			{
			if(mouse.button==0)
				{
				if (!nomouserot)
				{
				    dc+=(float)mouse.yspeed/500;
				    db+=(float)mouse.xspeed/500;
			    	}
				tel=0;
				}
			if(mouse.button==1)
				{
				if (!nomouserot)
				{
				    da+=(float)mouse.xspeed/500;
				}
	blr=1;
				zx+=(float)mouse.yspeed/50;
				rt++;
				}
		  if(mouse.button==2)
			  {
			  tel++;
			  da=0;db=0;dc=0;
	}


      if(tel==1)
	{
	flip++;
	if(flip>8) flip=0;
	for(i=0;i<numf;i++)
	  fac[i].type=flip;
	}

      a+=da;
			b+=db;
			c+=dc;
			memset(vmem,1,3200);
			memset(vmem+3200,0,57600);
			memset(vmem+60800,1,3200);
			rotate(a,b,c);
			rotatenorm(a,b,c);
			rotatepnorm(a,b,c);
			project(zx);

	    for(i=0;i<numf;i++)
	{
	zsrt1[i]=i;
	zsrt2[i]=i;
	rad1[i]=(LONG)(10000*(((vert[fac[i].a].z+vert[fac[i].b].z+vert[fac[i].c].z)/3)+40 ));
	}
      radixsort(rad1,rad2,numf,zsrt1,zsrt2);

      for(i=0;i<nump;i++)
	{
	col=(int)(rnz[i]*255);
	if(col<0) col=-col;
	vert[i].c=col;
	}
      for(i=0;i<numf;i++)
	{
	col=(int)(nx[i]*255);
	if(col<0) col=-col;
	fac[i].color=255-col;
	}

      for(ii=0;ii<numf;ii++)
	{
	  KDPdrawPoly(&fac[zsrt1[ii]],vert,vmem);
	}
            KDPwaitVr();
			KDPshow(vmem);
			KDPgetMouse(&mouse);
			frame++;
			}
		}
		time2=clock();
		//KDPwriteBMP("bmp/test.bmp",pal,vmem,320,200);
		KDPclose(&screen);
		KDPwaitVr();
		secs=(float)(time2-time1)/(float)CLOCKS_PER_SEC;
		printf("frames: %d time:%f  fps:%f\n",frame,secs,(float)frame/secs);
		printf("clock/sec: %d\n",CLOCKS_PER_SEC);
		//KDPwriteBMP("ram:test.bmp",pal,vmem,320,200);
		
	return 0;
	}

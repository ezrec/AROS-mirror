/* KDPgfx v0.8 ... by wrecK/KnP YhdZ/KnP
  some graphic functions for KDP
  (hardcoded for 320x200x8bit)

*/
#include "kdpgfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

UBYTE KDPcgcoltab[256*256];
//color adding table
UBYTE  KDPcoltab[512]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
  22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
  48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,
  74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,
  100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,
  119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,
  138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,
  157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,
  195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,
  214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,
  233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,
  252,253,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255};
 
//draw pixel at(x,y) color(col) in (vmem)
void KDPpset(int x,int y,UBYTE col,UBYTE *vmem)
{
  if ((x>=0) & (x<320) & (y>=0) & (y<200))
    vmem[y*320+x]=col;
}

//get color value at (x,y) in (vmem)
UBYTE KDPpoint(int x,int y,UBYTE *vmem)
	{
	if ((x>=0) & (x<320) & (y>=0) & (y<200))
	  return vmem[y*320+x];
	else
	  return 0;
	}

//add (col) to pixel at (x,y) in (vmem)
void KDPaddpset(int x,int y,UBYTE col,UBYTE *vmem)
{
  int o=y*320+x;
  if ((x>=0) & (x<320) & (y>=0) & (y<200))
    vmem[o]=KDPcoltab[vmem[o]+col];
}

//add big pixel(2x2) at (x,y) in (vmem)
void KDPaddpset2(int x,int y,UBYTE col,UBYTE *vmem)
{
  int o=y*320+x;
  if ((x>=0) & (x<319) & (y>=0) & (y<199))
    {
    vmem[o]=KDPcoltab[vmem[o]+col];
    vmem[o+1]=KDPcoltab[vmem[o+1]+col];
    vmem[o+320]=KDPcoltab[vmem[o+320]+col];
    vmem[o+321]=KDPcoltab[vmem[o+321]+col];
    }
}

//add 16x16 sprite (sp) at (x,y) in (vmem)
void KDPaddsp(int x,int y,UBYTE *sp,UBYTE *vmem)
{
  int xx,yy;
  int o,so=0;
  UBYTE *spr,*vm;
  spr=sp;vm=vmem;
  if ((x<-8) | (x>327) | (y<-8) | (y>207)) return;
  if ((x<8) | (x>311) | (y<8) | (y>191))
	 {
    o=((y-8)*320)+(x-8);
    so=0;
    for(yy=y-8;yy<y+8;yy++)
      for(xx=x-8;xx<x+8;xx++)
	     {
        if((xx>=0) && (xx<320) && (yy>=0) && (yy<200))
		    {
	       o=yy*320+xx;
	       if (sp[so]!=0) vmem[o]=KDPcoltab[vmem[o]+sp[so]];
	       }
        so++;
        }
    }
  else
    {
    vm+=((y-8)*320)+(x-8);
    for(yy=0;yy<16;yy++)
      {
      for(xx=0;xx<16;xx++)
        {
	     if (*spr!=0) *vm=KDPcoltab[*vm+*spr];
	     vm++;
	     spr++;
	     }
      vm+=304;
      }
    }
}

//draw 16x16 sprite(sp) at (x,y) in (vmem)
void KDPsp(int x,int y,UBYTE *sp,UBYTE *vmem)
{
  int xx,yy;
  int o,so=0;
  UBYTE *spr,*vm;
  spr=sp;vm=vmem;
  if ((x<-8) | (x>327) | (y<-8) | (y>207)) return;
  if ((x<8) | (x>311) | (y<8) | (y>191))
	{
    o=((y-8)*320)+(x-8);
    so=0;
    for(yy=y-8;yy<y+8;yy++)
      for(xx=x-8;xx<x+8;xx++)
	{
	if((xx>=0) && (xx<320) && (yy>=0) && (yy<200))
		{
	  o=yy*320+xx;
	  if (sp[so]!=0) vmem[o]=sp[so];
	  }
	so++;
	}
    }
  else
    {
    vm+=((y-8)*320)+(x-8);
    for(yy=0;yy<16;yy++)
      {
      for(xx=0;xx<16;xx++)
	{
	if (*spr!=0) *vm=*spr;
	vm++;
	spr++;
	}
      vm+=304;
      }
    }
}

//add scalable texture(tex) at (x,y) sized(size) pixels to (vmem)
void KDPsp256(int x,int y,int size,UBYTE *tex,UBYTE *vmem)
	{
	int x1,y1,x2,y2;
	int u1=0,v1=0;
	int u,v;
	int du,dv;
	int xx,yy;
	int dx;

  if(size<=0) return;
	x1=x-(size/2);y1=y-(size/2);
	x2=x1+size;y2=y1+size;

	if (x1<0 && x2<0) return;
	if (x1>319 && x2>319) return;
	if (y1<0 && y2<0) return;
	if (y1>199 && y2>199) return;

  du=(256<<16)/((x2-x1));
  dv=(256<<16)/((y2-y1));

  //printf("%f %f \n",du,dv);
  if(x1<0) {u1+=-x1*du; x1=0;}
  if(y1<0) {v1+=-y1*dv; y1=0;}
  if(x2>319) x2=319;
  if(y2>199) y2=199;

  dx=320-(x2-x1);
  vmem+=320*y1+x1;
  v=v1;
	for(yy=y1;yy<y2;yy++)
		{
		u=u1;
	  for(xx=x1;xx<x2;xx++)
		{
		*vmem=KDPcoltab[*vmem+tex[((v>>8)&0xffffff00)+(u>>16)]];
		*vmem++;
		u+=du;
		}
	  vmem+=dx;
	  v+=dv;
	  }
  }

//draw big (2x2) pixel
void KDPpset2(int x,int y,UBYTE col,UBYTE *vmem)
{
  int v;
  if ((x>=0) & (x<319) & (y>=0) & (y<199))
    {
    v=y*320+x;
    vmem[v]=col;
    vmem[v+1]=col;
    vmem[v+320]=col;
    vmem[v+321]=col;
    }
}


//clear (vmem)
void KDPcls(UBYTE *vmem)
{
  memset(vmem,0,64000);
}

//blur(vmem) (fast)
void KDPblur2(UBYTE *vmem)
{
  int i;
  memset(vmem,0,319);
  memset(vmem+(64000-320),0,319);
  for(i=320;i<(64000-320);i++)
    vmem[i]=(vmem[i-320]+vmem[i+320]+vmem[i-1]+vmem[i+1])>>2;
}

UBYTE buf[64000];

//blur up
void KDPfire(UBYTE *vmem)
{
  int i;
  for(i=0;i<(64000-320);i++)
    buf[i]=(vmem[i]+vmem[i+1]+vmem[i-1]+vmem[i+320])>>2;
  memcpy(vmem,buf,64000);
}
void KDPblur3(UBYTE *vmem)
{
  int i;
  for(i=320;i<(64000-320);i++)
    buf[i]=(vmem[i-320]+vmem[i+1]+vmem[i-1]+vmem[i+320])>>2;
  memcpy(vmem,buf,64000);
}
//blur ,then sub (speed)
void KDPblur(int speed,UBYTE *vmem)
{
  int i;
  int c;
  memset(vmem,0,319);
  memset(vmem+(64000-320),0,319);
  for(i=320;i<(64000-320);i++)
    {
    c=((vmem[i-320]+vmem[i+320]+vmem[i-1]+vmem[i+1])>>2)-speed;
    if (c<0) c=0;
    vmem[i]=c;
    }
}

//draw circle
void KDPcircle(int xo,int yo,int a,int b,UBYTE col,UBYTE *vmem)
{
  int x,y,asq,bsq,a22,b22,xslope,yslope,fmid;
  if (a<0) a=-a;
  x=a;y=0;asq=a*a;bsq=b*b;
  a22=asq+asq;
  b22=bsq+bsq;
  xslope=b22*a;yslope=0;
  fmid=-bsq*a+asq;
  while(xslope>yslope)
    {
    KDPpset(xo+x,yo+y,col,vmem);
    KDPpset(xo-x,yo+y,col,vmem);
    KDPpset(xo+x,yo-y,col,vmem);
    KDPpset(xo-x,yo-y,col,vmem);
    y++;
    yslope+=a22;
    if (fmid>=0)
      {
      x--;
      xslope-=b22;
      fmid-=xslope;
      }
    fmid+=(yslope+asq);
    }
  fmid-=((yslope+xslope)/2+b22-a22);
  while(x>=0)
    {
    KDPpset(xo+x,yo+y,col,vmem);
    KDPpset(xo-x,yo+y,col,vmem);
    KDPpset(xo+x,yo-y,col,vmem);
    KDPpset(xo-x,yo-y,col,vmem);
    x-=1;
    xslope-=b22;
    if (fmid<=0)
      {
      y+=1;
      yslope+=a22;
      fmid+=yslope;
      }
    fmid-=(xslope+bsq);
    }
}

//draw circle (large pixels)
void KDPcircle2(int xo,int yo,int a,int b,UBYTE col,UBYTE *vmem)
{
  int x,y,asq,bsq,a22,b22,xslope,yslope,fmid;
  if (a<0) a=-a;
  x=a;y=0;asq=a*a;bsq=b*b;
  a22=asq+asq;
  b22=bsq+bsq;
  xslope=b22*a;yslope=0;
  fmid=-bsq*a+asq;
  while(xslope>yslope)
    {
    KDPpset2(xo+x,yo+y,col,vmem);
    KDPpset2(xo-x,yo+y,col,vmem);
    KDPpset2(xo+x,yo-y,col,vmem);
    KDPpset2(xo-x,yo-y,col,vmem);
    y++;
    yslope+=a22;
    if (fmid>=0)
      {
      x--;
      xslope-=b22;
      fmid-=xslope;
      }
    fmid+=(yslope+asq);
    }
  fmid-=((yslope+xslope)/2+b22-a22);
  while(x>=0)
    {
    KDPpset2(xo+x,yo+y,col,vmem);
    KDPpset2(xo-x,yo+y,col,vmem);
    KDPpset2(xo+x,yo-y,col,vmem);
    KDPpset2(xo-x,yo-y,col,vmem);
    x-=1;
    xslope-=b22;
    if (fmid<=0)
      {
      y+=1;
      yslope+=a22;
      fmid+=yslope;
      }
    fmid-=(xslope+bsq);
    }
}

//make range in (pal) from (begin) at(r1,g1,b1) to (end) at (r2,g2,b2)
void KDPpalRange(float r1,float g1,float b1,int r2,int g2,int b2, int begin, int end,UBYTE *pal)
{
  int i;
  int len=end-begin;
  float dr=(r2-r1)/len;
  float dg=(g2-g1)/len;
  float db=(b2-b1)/len;
  int col;
  for(i=begin;i<=end;i++)
    {
    col=(i*3);
    pal[col+0]=(BYTE)r1;
    pal[col+1]=(BYTE)g1;
    pal[col+2]=(BYTE)b1;
    r1+=dr;
    g1+=dg;
    b1+=db;
    }
   return;
}

//return best match for (red,greem,blue) in (pl)
int KDPfindCol(UBYTE *pl,int red,int green,int blue)
{
  int r,g,b;
  int col;
  int dif;
  int mindif;
  int i;
  mindif=0xffffff;
  for(i=0;i<256;i++)
    {
    r=pl[i*3+0];
    g=pl[i*3+1];
    b=pl[i*3+2];
    dif=abs(r-red)+abs(g-green)+abs(b-blue);
    if (dif<=mindif)
      {
      mindif=dif;
      col=i;
      }
    }
  return col;
}

//read bmp file, image data to(image), palette data to(pal)
int KDPreadBMP(char *filename,UBYTE *pal,UBYTE *image)
{
  UBYTE *file;
  WORD fileid;
  LONG filesize;
  LONG bitmapoffset;
  LONG bisize;
  LONG width;
  LONG height;
  WORD planes;
  WORD bitpix;
  LONG comp;
  LONG usize;
  LONG xpm;
  LONG ypm;
  LONG numcol;
  LONG impcol;
  int i;
  int x,y;
  file=KDPloadFile(filename);
  if(file==0) return 0;
  fileid=KDPgetMword(&file[0]);
  //printf("FileID: %x \n",fileid);
  if (fileid!=0x424d)
    {
    printf("No BMP file! \n");
    free(file);
    return 0;
    }
  filesize=KDPgetIlong(&file[2]);
  bitmapoffset=KDPgetIlong(&file[10]);
  bisize=KDPgetIlong(&file[14]);
  width= KDPgetIlong(&file[18]);
  height=KDPgetIlong(&file[22]);
  planes=KDPgetIword(&file[26]);
  bitpix=KDPgetIword(&file[28]);
  comp=  KDPgetIlong(&file[30]);
  usize= KDPgetIlong(&file[34]);
  xpm=   KDPgetIlong(&file[38]);
  ypm=   KDPgetIlong(&file[42]);
  numcol=KDPgetIlong(&file[46]);
  impcol=KDPgetIlong(&file[50]);
  //printf("Filesize: %d \nBitmapoffs:%d\nBitmapInfoHeader size:%d\nWidth:%d  Height:%d Planes:%d \nBits/Pixel:%d\nCompression:%d Uncompressed size:%d\nX-Pixels/meter:%d Y-Pixels/meter:%d \nColors used:%d Important colors:%d\n",
  //        filesize,bitmapoffset,bisize,width,height,planes,bitpix,comp,usize,xpm,ypm,numcol,impcol);

  if (pal!=0)
    for(i=0;i<256;i++)
      {
      pal[i*3+0]=file[i*4+2+54];
      pal[i*3+1]=file[i*4+1+54];
      pal[i*3+2]=file[i*4+0+54];
      }
  i=bitmapoffset;
  if (image!=0)
    for(y=0;y<height;y++)
      for(x=0;x<width;x++)
	{
	image[((height-1)-y)*width+x]=file[i];
	i++;
	}
  free(file);
  return 1;
}

//write (image) to bmp file
//needs some work (most programs wont read them :(  )
int KDPwriteBMP(char *filename,UBYTE *pal,UBYTE *image,int width,int height)
	{
	FILE *file;
	int offset,size,pad,i,x,y;
  file=fopen(filename,"wb");
  if(file==0)
    return 0;
  fputc('B',file);fputc('M',file); //bfType


  pad=4-(width % 4);
  if(pad==4) pad=0;
  offset=256*4+52+2;
  size=width*height+offset;
  KDPwriteIlong(size,file);        // bfSize
  KDPwriteIword(0,file);           // bfReserved1
  KDPwriteIword(0,file);           // bfReserved2
  KDPwriteIlong(offset,file);      // bfOffbits
  KDPwriteIlong(40,file);          // biSize
  KDPwriteIlong(width,file);       // biWidth
  KDPwriteIlong(height,file);      // bitHeight
  KDPwriteIword(1,file);           // bitPlanes
  KDPwriteIword(8,file);           // biBitCount
  KDPwriteIlong(0,file);           // biCompression
  KDPwriteIlong(0,file);// biSizeImage
  KDPwriteIlong(16,file);           // biXPelsPerMeter
  KDPwriteIlong(16,file);           // biYPelsPerMeter
  KDPwriteIlong(256,file);         // biClrUsed
  KDPwriteIlong(0,file);           // biClrImportant

  for(i=0;i<256;i++)
	{
	fputc(pal[i*3+2],file); //blue
	fputc(pal[i*3+1],file); //green
	fputc(pal[i*3+0],file); //red
	fputc(0,file);
    }

  for(y=(height-1);y>=0;y--)
    {
	for(x=0;x<=(width-1);x++)
	  fputc(image[y*width+x],file);
	if(pad!=0)
	  for(i=0;i<pad;i++)
	    fputc(0,file);
    }
  fclose(file);
  return 1;
  }

//for line drawing...
void oct03(int x0,int y0,int deltax,int deltay,int xdir,UBYTE col,UBYTE *vmem)
{
  int deltayx2,dyx2mdx2,ert;
  int ofs;
  deltayx2=deltay*2;
  dyx2mdx2=deltayx2-deltax*2;
  ert=deltayx2-deltax;
  ofs=y0*320+x0;
  vmem+=ofs;
  while(deltax--)
    {
    if (ert>=0)
      {
      vmem+=320;
      ert+=dyx2mdx2;
      }
    else
      {
      ert+=deltayx2;
      }
    vmem+=xdir;
    *vmem=KDPcoltab[*vmem+col];
    }
}

void oct13(int x0,int y0,int deltax,int deltay,int xdir,UBYTE col,UBYTE *vmem)
{
  int deltaxx2,dxx2mdy2,ert;
  int ofs;
  deltaxx2=deltax*2;
  dxx2mdy2=deltaxx2-deltay*2;
  ert=deltaxx2-deltay;
  ofs=y0*320+x0;
  vmem+=ofs;
  while(deltay--)
    {
    if (ert>=0)
      {
      vmem+=xdir;
      ert+=dxx2mdy2;
      }
    else
      {
      ert+=deltaxx2;
      }
    vmem+=320;
    *vmem=KDPcoltab[*vmem+col];
    }
}


void oct04(int x0,int y0,int deltax,int deltay,int xdir,UBYTE col,UBYTE *vmem)
{
  int deltayx2,dyx2mdx2,ert;
  int ofs;
  deltayx2=deltay*2;
  dyx2mdx2=deltayx2-deltax*2;
  ert=deltayx2-deltax;
  ofs=y0*320+x0;
  vmem+=ofs;
  while(deltax--)
    {
    if (ert>=0)
      {
      vmem+=320;
      ert+=dyx2mdx2;
      }
    else
      {
      ert+=deltayx2;
      }
    vmem+=xdir;
    *vmem=col;
    }
}

void oct14(int x0,int y0,int deltax,int deltay,int xdir,UBYTE col,UBYTE *vmem)
{
  int deltaxx2,dxx2mdy2,ert;
  int ofs;
  deltaxx2=deltax*2;
  dxx2mdy2=deltaxx2-deltay*2;
  ert=deltaxx2-deltay;
  ofs=y0*320+x0;
  vmem+=ofs;
  while(deltay--)
    {
    if (ert>=0)
      {
      vmem+=xdir;
      ert+=dxx2mdy2;
      }
    else
      {
      ert+=deltaxx2;
      }
    vmem+=320;
    *vmem=col;
    }
}

//draw line (2x2 pixels)
void KDPline2(int x0,int y0,int x1,int y1,UBYTE col,UBYTE *vmem)
{
  int deltax,deltay,temp;
  float dx,dy;
  dx=(float)(y1-y0)/(float)(x1-x0);
  dy=(float)(x1-x0)/(float)(y1-y0);
  if((x0<0) && (x1<0)) return;
  if((x0>319) && (x1>319)) return;
  if((y0<0) && (y1<0)) return;
  if((y0>199) && (y1>199)) return;

  if(y0<0)
    {
    x0+=(float)(-y0)*dy;
    y0=0;
    }
  if(y0>199)
    {
    x0+=(float)(199-y0)*dy;
    y0=199;
    }
  if(x0<0)
    {
    y0+=(float)(-x0)*dx;
    x0=0;
    }
  if(x0>319)
    {
    y0+=(float)(319-x0)*dx;
    x0=319;
    }

  if(y1<0)
    {
    x1+=(float)(-y1)*dy;
    y1=0;
    }
  if(y1>199)
    {
    x1+=(float)(199-y1)*dy;
    y1=199;
    }
  if(x1<0)
    {
    y1+=(float)(-x1)*dx;
    x1=0;
    }
  if(x1>319)
    {
    y1+=(float)(319-x1)*dx;
    x1=319;
    }


  if(x0<0) x0=0;
  if(x0>319) x0=319;
  if(y0<0) y0=0;
  if(y0>199) y0=199;
  if(x1<0) x1=0;
  if(x1>319) x1=319;
  if(y1<0) y1=0;
  if(y1>199) y1=199;


  if (y0>y1)
    {
    temp=y0;
    y0=y1;
    y1=temp;
    temp=x0;
    x0=x1;
    x1=temp;
    }
  deltax=x1-x0;
  deltay=y1-y0;
  if (deltax>0)
    {
    if(deltax>deltay)
      {
      oct03(x0,y0,deltax,deltay,1,col,vmem);
      }
    else
      {
      oct13(x0,y0,deltax,deltay,1,col,vmem);
      }
    }
  else
    {
    deltax=-deltax;
    if (deltax>deltay)
      {
      oct03(x0,y0,deltax,deltay,-1,col,vmem);
      }
    else
      {
      oct13(x0,y0,deltax,deltay,-1,col,vmem);
      }
    }
}



//draw line
void KDPline(int x0,int y0,int x1,int y1,UBYTE col,UBYTE *vmem)
{
  int deltax,deltay,temp;


  float dx,dy;

  dx=(float)(y1-y0)/(float)(x1-x0);
  dy=(float)(x1-x0)/(float)(y1-y0);


  if((x0<0) && (x1<0)) return;
  if((x0>319) && (x1>319)) return;
  if((y0<0) && (y0<0)) return;
  if((y0>199) && (y1>199)) return;

  if(y0<0)
	{
	x0+=(float)(-y0)*dy;
	y0=0;
	}
  if(y0>199)
	{
	x0+=(float)(199-y0)*dy;
	y0=199;
	}
  if(x0<0)
	{
	y0+=(float)(-x0)*dx;
	x0=0;
	}
  if(x0>319)
	{
	y0+=(float)(319-x0)*dx;
	x0=319;
	}



  if(y1<0)
	{
	x1+=(float)(-y1)*dy;
	y1=0;
	}
  if(y1>199)
	{
	x1+=(float)(199-y1)*dy;
	y1=199;
	}
  if(x1<0)
	{
	y1+=(float)(-x1)*dx;
	x1=0;
	}
  if(x1>319)
	{
	y1+=(float)(319-x1)*dx;
	x1=319;
	}

  if(x0<0) x0=0;
  if(x0>319) x0=319;
  if(y0<0) y0=0;
  if(y0>199) y0=199;
  if(x1<0) x1=0;
  if(x1>319) x1=319;
  if(y1<0) y1=0;
  if(y1>199) y1=199;

  if (y0>y1)
    {
    temp=y0;
    y0=y1;
    y1=temp;
    temp=x0;
    x0=x1;
    x1=temp;
    }
  deltax=x1-x0;
  deltay=y1-y0;
  if (deltax>0)
    {
    if(deltax>deltay)
      {
      oct04(x0,y0,deltax,deltay,1,col,vmem);
      }
    else
      {
      oct14(x0,y0,deltax,deltay,1,col,vmem);
      }
    }
  else
    {
    deltax=-deltax;
    if (deltax>deltay)
      {
      oct04(x0,y0,deltax,deltay,-1,col,vmem);
      }
    else
      {
      oct14(x0,y0,deltax,deltay,-1,col,vmem);
      }
    }
}

//draw box
void KDPbox(int x1,int y1,int x2,int y2,UBYTE col,UBYTE *vmem)
{
  if(x1<0) x1=0;
  if(x1>319) x1=319;
  if(x2<0) x2=0;
  if(x2>319) x2=319;
  if(y1<0) y1=0;
  if(y1>199) y1=199;
  if(y2<0) y2=0;
  if(y2>199) y2=199;
  KDPline(x1,y1,x2,y1,col,vmem);
  KDPline(x2,y1,x2,y2,col,vmem);
  KDPline(x2,y2,x1,y2,col,vmem);
  KDPline(x1,y2,x1,y1,col,vmem);
}

//draw filled box
void KDPbox2(int x1,int y1,int x2,int y2,UBYTE col,UBYTE *vmem)
{
  int s;
  int xd;
  int y;
  int x;
  int dx;
  int o=0;
  if(x1<0) x1=0;
  if(x1>319) x1=319;
  if(x2<0) x2=0;
  if(x2>319) x2=319;
  if(y1<0) y1=0;
  if(y1>199) y1=199;
  if(y2<0) y2=0;
  if(y2>199) y2=199;
  if(y2<y1)
    {
    s=y2;
    y2=y1;
    y1=s;
    }
  if(x2<x1)
    {
    s=x2;
    x2=x1;
    x1=s;
    }
  o+=(y1*320)+x1;
  xd=x2-x1;
  dx=(320-xd)-1;
  for(y=y1;y<=y2;y++)
    {
    for(x=0;x<=xd;x++)
      {
      vmem[o]=col;
      o++;
      }
    o+=dx;
    }
}

//create 256x256 color table from (pal) in  (KDPcgcoltab)
void KDPmakecoltab1(UBYTE *pal)
  {
  int i,ii;
  float r,g,b;
  float r1,g1,b1;
  float r2,g2,b2;
  float dr1,dg1,db1;
  float dr2,dg2,db2;
  int c1,c2;
  int nm=10;
  for(i=0;i<256;i++)
      {
      if ((i%25)==0)
	{
	printf("%d \n",nm);
	nm--;
	}
      r=pal[i*3+0];g=pal[i*3+1];b=pal[i*3+2];
      dr1=r/128;dg1=g/128;db1=b/128;
      dr2=(255-r)/128;dg2=(255-g)/128;db2=(255-b)/128;
      r1=r;r2=r;
      g1=g;g2=g;
      b1=b;b2=b;
      for(ii=0;ii<128;ii++)
	{
	c1=127-ii;c2=128+ii;
	KDPcgcoltab[i*256+c1]=KDPfindCol(pal,(int)r1,(int)g1,(int)b1);
	KDPcgcoltab[i*256+c2]=KDPfindCol(pal,(int)r2,(int)g2,(int)b2);
	r1-=dr1;g1-=dg1;b1-=db1;
	r2+=dr2;g2+=dg2;b2+=db2;
	if (r2>255) r2=255;
	if (g2>255) g2=255;
	if (b2>255) b2=255;
	}
      }
  return;
}

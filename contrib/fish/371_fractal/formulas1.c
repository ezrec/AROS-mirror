/* formulas.c */
#include <aros/oldprograms.h>
#include <stdio.h>
#include <math.h>
#include <exec/types.h>
#include <intuition/intuition.h>
#include "defs.h"


extern float str_a;
extern char str_abuf[];
extern float str_b;
extern char str_bbuf[];
extern float str_c;
extern char str_cbuf[];
extern float str_d;
extern char str_dbuf[];
extern float str_e;
extern char str_ebuf[];
extern float c_xmin;
extern char c_xminbuf[];
extern float c_xmax;
extern char c_xmaxbuf[];
extern float c_ymin;
extern char c_yminbuf[];
extern float c_ymax;
extern char c_ymaxbuf[];
extern float c_p;
extern char c_pbuf[];
extern float c_h;
extern char c_hbuf[];
extern short c_res;
extern char c_resbuf[];
extern short c_iter;
extern char c_iterbuf[];

extern struct Gadget gad1[];
extern struct Gadget gadb[];
extern struct Screen *scr,*openscreen();
extern struct Window *gwdw,*wdw,*openwindow();
extern struct Menu men[];

extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct RastPort *rport;
extern short tmpcol,mennum,itmnum,subnum;
extern short width;
extern short height;
extern short depth;

struct IntuiMessage *imsg;
struct ViewPort *WVP;
void ColorWindow(),scanmenu();
void PutBoolGadget(),PutGadget();
void runfractal();
void toggletitle();

static short cycle=CYCLE0;
static short fractal1=STRATT;
static short maxcol,counter;
extern short xc,yc,xoffset,yoffset;
static float resave,imsave,restep,imstep;
static float re,re2,im,im2,tmp,lambdahalva;


double pow(),sqrt(),atan(),log(),atof(),exp(),sin(),cos(),fabs(),tan();

void get1fractal(),get1values();
void stratt();
void cycle0();

void run1fractal();
void (*fracalg1)()=stratt;


void stratt()
{
float xx,yy,zz;
register float x,y,z;
float eee;
float xinc,yinc;
long xcrd,ycrd;
long color;

x=y=z=0;
eee=1+1.25*str_e;
xinc=width/2/eee;
yinc=height/2/eee;
for(xc=0,yc=0;!xc;)
  {
   xx=sin((double)(str_a*y))-z*cos((double)(str_b*x));
   yy=z*sin((double)(str_c*x))-cos((double)(str_d*y));
   zz=str_e*sin((double)(x));
   x=xx;y=yy;z=zz;
   xcrd=(long)((x+eee)*xinc);
   ycrd=(long)((y+eee)*yinc);
   color=(long)(ReadPixel(rport,xcrd,ycrd)+1);
   if(color<2)color=2L;
   if(color>maxcol)color=(long)maxcol;
   SetAPen(rport,color);
   WritePixel(rport,xcrd,ycrd);
   if(!(yc&63))scanmenu();
   yc++;
  };
}

void cycle0()
{
float xinc,yinc;
float x,y,xx,yy;
float h,p;
short res,itr;

long color,xcrd,ycrd;
short xstep,ystep;

xstep=width/c_res;
ystep=height/c_res;
xinc=width/(c_xmax-c_xmin);
yinc=height/(c_ymax-c_ymin);
for(xc=0;xc<width;xc+=xstep)
  {
   for(yc=0;yc<height;yc+=ystep)
     {
      x=xc/xinc+c_xmin;
      y=yc/yinc+c_ymin;
      for(counter=0;counter<c_iter;counter++)
        {
         switch(cycle)
            {
             case CYCLE0:{xx=-c_h*(sin((double)(y+sin((double)(c_p*y)))));
                          yy= c_h*(sin((double)(x+sin((double)(c_p*x)))));
                          break; };
             case CYCLE1:{xx=-c_h*(sin((double)(y*y+sin((double)(c_p*y)))));
                          yy= c_h*(sin((double)(x*x+sin((double)(c_p*x)))));
                          break; };
             case CYCLE2:{xx=-c_h*(sin((double)(y+tan((double)(c_p*y)))));
                          yy= c_h*(sin((double)(x+tan((double)(c_p*x)))));
                          break; };
             case CYCLE3:{xx=-c_h*(sin((double)(y+cos((double)(c_p*y)))));
                          yy= c_h*(sin((double)(x+cos((double)(c_p*x)))));
                          break; };
             case CYCLE4:{xx=-c_h*(sin((double)(y+sin((double)x)*sin((double)(c_p*y)))));
                          yy= c_h*(sin((double)(x+sin((double)x)*sin((double)(c_p*x)))));
                          break; };

             case CYCLE5:{if(fabs((double)y)<3.1415)xx=-c_h*sin((double)y);
      else if(fabs((double)y)<6.283)
      xx=-c_h*sin((double)y*y+sin((double)c_p*y));
      else
      xx=-c_h*sin((double)y+pow((double)sin((double)c_p*y),(double)2));

                          if(fabs((double)x)<3.1415)yy= c_h*sin((double)y);
      else if(fabs((double)x)<6.283)
      yy=-c_h*sin((double)x*x+sin((double)c_p*x));
      else
      yy=-c_h*sin((double)x+pow((double)sin((double)c_p*x),(double)2));
                           break; };

            };
         x+=xx;
         y+=yy;

         xcrd=(long)((x-c_xmin)*xinc);
         ycrd=(long)((y-c_xmin)*yinc);
         color=(long)(ReadPixel(rport,xcrd,ycrd)+1);
         if(color<2)color=2L;
         if(color>maxcol)color=(long)maxcol;
         SetAPen(rport,color);
         WritePixel(rport,xcrd,ycrd);
        };
      scanmenu();
     };
  };
}


void get1values()
{
gwdw=openwindow(scr,50,10,200,180,(ULONG)ACTIVATE,
                (USHORT)(GADGETUP),NULL);
rport=gwdw->RPort;
SetAPen(rport,1L);

Move(rport,10L,10L);
switch(fractal1)
   {
    case STRATT: { Text(rport,"Str.Attr",8L);
                   PutGadget("A",1,10,20,(gad1+0));
                   PutGadget("B",1,110,20,(gad1+1));
                   PutGadget("C",1,10,42,(gad1+2));
                   PutGadget("D",1,110,42,(gad1+3));
                   PutGadget("E",1,10,64,(gad1+4));
                   break; };
    case CYCLIC: { Text(rport,"Cyclic System",13L);
                   PutGadget("X-Min",5,10,20,(gad1+5));
                   PutGadget("X-Max",5,110,20,(gad1+6));
                   PutGadget("Y-Min",5,10,42,(gad1+7));
                   PutGadget("Y-Max",5,110,42,(gad1+8));
                   PutGadget("p",1,10,64,(gad1+9));
                   PutGadget("h",1,110,64,(gad1+10));
                   PutGadget("Res",3,10,86,(gad1+11));
                   PutGadget("Iter",4,110,86,(gad1+12));
                   break; };

   };

PutBoolGadget(110,160,(gadb+0));

Wait(1L << gwdw->UserPort->mp_SigBit);
while(imsg=(struct IntuiMessage *)GetMsg(gwdw->UserPort))
      ReplyMsg((struct Message *)imsg);
CloseWindow(gwdw);

str_a=atof(str_abuf);
str_b=atof(str_bbuf);
str_c=atof(str_cbuf);
str_d=atof(str_dbuf);
str_e=atof(str_ebuf);
c_xmin=atof(c_xminbuf);
c_xmax=atof(c_xmaxbuf);
c_ymin=atof(c_yminbuf);
c_ymax=atof(c_ymaxbuf);
c_p=atof(c_pbuf);
c_h=atof(c_hbuf);
c_res=(short)atof(c_resbuf);if(c_res<1)c_res=1;
c_iter=(short)atof(c_iterbuf);if(c_iter<10)c_iter=10;

}


void get1fractal()
{
  if((itmnum!=CYCLETYPE))fractal1=itmnum;
  switch(itmnum)
    {
     case STRATT:    { fracalg1=stratt;break; };
     case CYCLETYPE: { cycle=subnum;if(fractal1!=CYCLIC)break; };
     case CYCLIC:    { fracalg1=cycle0;break; };

    };
}


void run1fractal()
{
rport=wdw->RPort;
SetAPen(rport,0L);
RectFill(rport,0L,0L,(long)(width-1),(long)(height-1));
get1values();
rport=wdw->RPort;
maxcol=(1<<depth)-1;
SetMenuStrip(wdw,(men+3));
switch(fractal1)
  {
   case STRATT: { (*fracalg1)();break; };
   case CYCLIC: { (*fracalg1)();break; };
  };
ClearMenuStrip(wdw);
}


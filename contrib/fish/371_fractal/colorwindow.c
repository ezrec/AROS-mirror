/* colorwindow.c ranxerox v4.1 890820 891217 */
#include <aros/oldprograms.h>
#include <intuition/screens.h>
#include <intuition/intuition.h>
#include <exec/types.h>
#define Rp CW_cwdw->RPort
#define ABS(x) ((x)>0)?(x):-(x)
typedef enum { NOTHING,EXIT,REDUP,GREENUP,BLUEUP,REDDOWN,GREENDOWN,BLUEDOWN
             , RED,GREEN,BLUE,WAVE1,WAVE2,WAVE3,WAVE4,WAVE5,WAVE6,MOVE}
        VALUES;

#define MAXWAVES 2
#define REDLEFT 0x000020
#define GRELEFT 0x000040
#define BLULEFT 0x000080
#define COLLEFT REDLEFT|GRELEFT|BLULEFT
#define REDRIGHT 0x000100
#define GRERIGHT 0x000200
#define BLURIGHT 0x000400
#define COLRIGHT REDRIGHT|GRERIGHT|BLURIGHT

extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;

void _CWDrawWindow(),_CWSetColMap(),_CWRight(),_CWLeft();
void _CWDRGB(),_CWBox(),_CWWave(),_CWDown(),_CWUp();
long _CWGetChoice();

static struct Window *CW_cwdw;
static struct ViewPort *CW_WVP;
static char CW_red[32],CW_green[32],CW_blue[32];
static long CW_RGBflag,CW_WaveFlag;

static struct NewWindow CW_nwdw={150,50,150,150,0,1,MOUSEBUTTONS,
              ACTIVATE|WINDOWDRAG|WINDOWDEPTH,
              NULL,NULL,(UBYTE *)"COLORWINDOW 4.1",NULL,NULL,0,0,0,0,
              CUSTOMSCREEN
               };
static struct CW_1 { short Xmin,Ymin,Xmax,Ymax; };
static struct CW_1 CW_s1[]={ {10,70,140,120},
              {10,134,45,140},{15,53,135,60},{15,85,45,95},{60,85,90,95},
              {105,85,135,95},{15,105,45,115},{60,105,90,115},{105,105,135,115}
               };
static struct CW_2 { short X,Y; char *Txt; short Len; };
static struct CW_2 CW_s2[]={ {10,140,"EXIT",4},{23,18,"R",1},{73,18,"G",1},
              {123,18,"B",1},{2,59,"<",1},{140,59,">",1},{40,80,"WaveForms",9},
              {105,115,"NEXT",4}
               };

void ColorWindow(scr)
struct Screen *scr;
{
struct IntuiMessage *message;
long choice,xcrd,ycrd;
USHORT code;

CW_nwdw.Screen=scr;CW_cwdw=OpenWindow(&CW_nwdw);

CW_RGBflag=RED;CW_WaveFlag=1;
_CWDrawWindow();

choice=NOTHING;
while(choice!=EXIT)
{
 code=SELECTUP;
 Wait( 1L << CW_cwdw->UserPort->mp_SigBit );
 while((message=(struct IntuiMessage *)GetMsg(CW_cwdw->UserPort)))
  {
   if(message!=0L)
    {
     code=message->Code;xcrd=message->MouseX;ycrd=message->MouseY;
     ReplyMsg((struct Message *)message);
     if(code==SELECTDOWN)
      {
       switch(choice=_CWGetChoice(xcrd,ycrd))
        {
         case EXIT: break;
         case RED: { CW_RGBflag=RED;_CWDRGB();break;};
         case GREEN: { CW_RGBflag=GREEN;_CWDRGB();break;};
         case BLUE: { CW_RGBflag=BLUE;_CWDRGB();break;};
         case COLLEFT:
         case REDLEFT:
         case GRELEFT:
         case BLULEFT: { _CWLeft(choice);break;};
         case COLRIGHT:
         case REDRIGHT:
         case GRERIGHT:
         case BLURIGHT: { _CWRight(choice);break;};
         case REDUP:
         case GREENUP:
         case BLUEUP: { _CWUp(choice);break;};
         case REDDOWN:
         case GREENDOWN:
         case BLUEDOWN: { _CWDown(choice);break;};
         case WAVE1:
         case WAVE2:
         case WAVE3:
         case WAVE4:
         case WAVE5: { _CWWave(choice);break;};
         case WAVE6: { CW_WaveFlag=((CW_WaveFlag+1)>MAXWAVES)?1:CW_WaveFlag+1;
                       _CWDrawWindow();break;};
        };
      };
    };
  };
};
CloseWindow(CW_cwdw);
}

long _CWGetChoice(xcrd,ycrd)
long xcrd,ycrd;
{
long flag;
flag=NOTHING;
if(ycrd<20)
 {
  if((xcrd>5)&&(xcrd<12))flag=REDUP;
  if((xcrd>55)&&(xcrd<62))flag=GREENUP;
  if((xcrd>105)&&(xcrd<112))flag=BLUEUP;
 }
else
if(ycrd<40)
 {
  flag=BLURIGHT;
  if(xcrd<140)flag=BLUE;
  if(xcrd<110)flag=BLULEFT;
  if(xcrd<100)flag=GRERIGHT;
  if(xcrd<90)flag=GREEN;
  if(xcrd<60)flag=GRELEFT;
  if(xcrd<50)flag=REDRIGHT;
  if(xcrd<40)flag=RED;
  if(xcrd<10)flag=REDLEFT;
 }
else
if(ycrd<50)
 {
  if((xcrd>5)&&(xcrd<12))flag=REDDOWN;
  if((xcrd>55)&&(xcrd<62))flag=GREENDOWN;
  if((xcrd>105)&&(xcrd<112))flag=BLUEDOWN;
 }
else
if(ycrd<60)
 {
  if(xcrd<10)flag=COLLEFT;
  if(xcrd>140)flag=COLRIGHT;
 }
else
if(ycrd<85){ flag=NOTHING; } /* dummy */
else
if(ycrd<95)
 {
  if((xcrd>15)&&(xcrd<45))flag=WAVE1;
  if((xcrd>60)&&(xcrd<90))flag=WAVE2;
  if((xcrd>105)&&(xcrd<135))flag=WAVE3;
 }
else
if(ycrd<105){ flag=NOTHING; } /* dummy */
else
if(ycrd<115)
 {
  if((xcrd>15)&&(xcrd<45))flag=WAVE4;
  if((xcrd>60)&&(xcrd<90))flag=WAVE5;
  if((xcrd>105)&&(xcrd<135))flag=WAVE6;
 };

if((ycrd>132)&&(ycrd<142))
 {
  if((xcrd>9)&&(xcrd<48))flag=EXIT;
 };
return(flag);
}

void _CWDrawWindow()
{
long col,index;
SetDrMd(Rp,JAM1);
SetAPen(Rp,1L);
RectFill(Rp,1L,10L,149L,149L);
SetAPen(Rp,0L);

/* Draw Boxes */
for(index=0;index<9;index++)
_CWBox((long)(CW_s1[index].Xmin),(long)(CW_s1[index].Ymin),(long)(CW_s1[index].Xmax),(long)(CW_s1[index].Ymax));

/* Draw TEXT */
for(index=0;index<8;index++)
{ Move(Rp,(long)(CW_s2[index].X),(long)(CW_s2[index].Y));
  Text(Rp,(UBYTE *)(CW_s2[index].Txt),(long)(CW_s2[index].Len));    };
for(index=0;index<3;index++)
 {
   Move(Rp,index*50+2,33L);Text(Rp,"<",1L);
   Move(Rp,index*50+5,18L);Text(Rp,"+",1L);
   Move(Rp,index*50+5,47L);Text(Rp,"-",1L);
   Move(Rp,index*50+42,33L);Text(Rp,">",1L);
 };
/* Draw TEXT END */

/* Draw color 2 - 32 */
for(index=2;index<32;index++)
 {
   SetAPen(Rp,index);
   RectFill(Rp,(long)(7+index*4),52L,(long)(10+index*4),60L);
 };

/* Get RGB values for colors */
CW_WVP=(struct ViewPort *)ViewPortAddress(CW_cwdw);
for(index=0;index<32;index++)
 {
   col=GetRGB4(CW_WVP->ColorMap,index);
   CW_red[index]=(col & 0x0f00)>>8;
   CW_green[index]=(col & 0x00f0)>>4;
   CW_blue[index]=(col & 0x000f);
 };
_CWDRGB();

/* Draw Waves */
SetAPen(Rp,0L);
if(CW_WaveFlag==1)
{
Move(Rp,15L,94L);Draw(Rp,45L,84L);
Move(Rp,60L,94L);Draw(Rp,75L,84L);Draw(Rp,90L,94L);
Move(Rp,105L,84L);Draw(Rp,135L,94L);
/* WAVE 4 */
Move(Rp,15L,114L);Draw(Rp,25L,104L);Draw(Rp,25L,114L);Draw(Rp,35L,104L);
Draw(Rp,35L,114L);Draw(Rp,45L,104L);
Move(Rp,60L,114L);Draw(Rp,90L,114L);
};
if(CW_WaveFlag==2)
{
Move(Rp,15L,94L);Draw(Rp,30L,84L);Draw(Rp,45L,84L);
Move(Rp,60L,94L);Draw(Rp,70L,89L);Draw(Rp,80L,89L);Draw(Rp,90L,94L);
Move(Rp,105L,84L);Draw(Rp,120L,84L);Draw(Rp,135L,94L);
/* WAVE 4 */
Move(Rp,15L,114L);Draw(Rp,30L,104L);Draw(Rp,30L,114L);Draw(Rp,45L,104L);
Move(Rp,60L,104L);Draw(Rp,75L,114L);Draw(Rp,75L,104L);Draw(Rp,90L,114L);
};
}

void _CWBox(xmin,ymin,xmax,ymax)
long xmin,ymin,xmax,ymax;
{
  SetAPen(Rp,0L);
  RectFill(Rp,(long)(xmin-3),(long)(ymin-3),(long)(xmax+3),(long)(ymax+3));
  SetAPen(Rp,1L);
  RectFill(Rp,(long)(xmin-2),(long)(ymin-2),(long)(xmax+1),(long)(ymax+1));
  SetAPen(Rp,0L);
}

void _CWDRGB()
{
long index1,index2,tmp;
for(index1=0;index1<3;index1++)
 {
  SetAPen(Rp,1L);
  if((CW_RGBflag==RED)&&(index1==0))SetAPen(Rp,0L);
  if((CW_RGBflag==GREEN)&&(index1==1))SetAPen(Rp,0L);
  if((CW_RGBflag==BLUE)&&(index1==2))SetAPen(Rp,0L);
  Move(Rp,(long)(index1*50+12),18L);
  Text(Rp,"*  *",4L);

  SetAPen(Rp,0L);
  RectFill(Rp,(long)(index1*50+9),20L,(long)(index1*50+41),40L);

  SetAPen(Rp,1L);
  for(index2=2;index2<32;index2++)
   {
    switch(index1)
     {
      case 0: { tmp=CW_red[index2];break;};
      case 1: { tmp=CW_green[index2];break;};
      case 2: { tmp=CW_blue[index2];break;};
     };
    WritePixel(Rp,(long)(index1*50+8+index2),(long)(39-tmp));
   };
 };
}

void _CWLeft(mask)
long mask;
{
long index,tmpr,tmpg,tmpb;
tmpr=CW_red[2];tmpg=CW_green[2];tmpb=CW_blue[2];
for(index=2;index<31;index++)
 {
  if(mask&REDLEFT)CW_red[index]=CW_red[index+1];
  if(mask&GRELEFT)CW_green[index]=CW_green[index+1];
  if(mask&BLULEFT)CW_blue[index]=CW_blue[index+1];
 };
if(mask&REDLEFT)CW_red[31]=tmpr;
if(mask&GRELEFT)CW_green[31]=tmpg;
if(mask&BLULEFT)CW_blue[31]=tmpb;
_CWSetColMap();
_CWDRGB();
}

void _CWRight(mask)
long mask;
{
long index,tmpr,tmpg,tmpb;
tmpr=CW_red[31];tmpg=CW_green[31];tmpb=CW_blue[31];
for(index=31;index>2;index--)
 {
  if(mask&REDRIGHT)CW_red[index]=CW_red[index-1];
  if(mask&GRERIGHT)CW_green[index]=CW_green[index-1];
  if(mask&BLURIGHT)CW_blue[index]=CW_blue[index-1];
 };
if(mask&REDRIGHT)CW_red[2]=tmpr;
if(mask&GRERIGHT)CW_green[2]=tmpg;
if(mask&BLURIGHT)CW_blue[2]=tmpb;
_CWSetColMap();
_CWDRGB();
}

void _CWSetColMap()
{
long index;
for(index=2L;index<32L;index++)
 {
   SetRGB4(CW_WVP,index,(long)CW_red[index],(long)CW_green[index],(long)CW_blue[index]);
 };
}

void _CWUp(choice)
long choice;
{
long index;
for(index=2;index<32;index++)
 {
  if(choice==REDUP)CW_red[index]=((CW_red[index]+1)>15)?0:CW_red[index]+1;
  if(choice==GREENUP)CW_green[index]=((CW_green[index]+1)>15)?0:CW_green[index]+1;
  if(choice==BLUEUP)CW_blue[index]=((CW_blue[index]+1)>15)?0:CW_blue[index]+1;
  SetRGB4(CW_WVP,(long)index,(long)CW_red[index],(long)CW_green[index],
          (long)CW_blue[index]);
 };
_CWDRGB();
}

void _CWDown(choice)
long choice;
{
long index;
for(index=2;index<32;index++)
 {
  if(choice==REDDOWN)CW_red[index]=((CW_red[index]-1)<0)?15:CW_red[index]-1;
  if(choice==GREENDOWN)CW_green[index]=((CW_green[index]-1)<0)?15:CW_green[index]-1;
  if(choice==BLUEDOWN)CW_blue[index]=((CW_blue[index]-1)<0)?15:CW_blue[index]-1;
  SetRGB4(CW_WVP,(long)index,(long)CW_red[index],(long)CW_green[index],
          (long)CW_blue[index]);
 };
_CWDRGB();
}

void _CWWave(choice)
long choice;
{
long index,tmp;
for(index=0;index<30;index++)
 {
  tmp=0;
  switch(CW_WaveFlag)
     {
     case 1:
        {
        switch(choice)
           {  case WAVE1: { tmp=index / 2;break;};
              case WAVE2: { tmp=(index<15)?index:(30-index);break;};
              case WAVE3: { tmp=15-index/2;break;};
              case WAVE4: { tmp=index % 10;break;};
              case WAVE5: { tmp=0;break;};
           };
        break;};
     case 2:
        {
        switch(choice)
           {  case WAVE1: { tmp=(index>15)?15:index;break;};
              case WAVE2: { tmp=(index<15)?index:(30-index);
                            tmp=(tmp>10)?10:tmp;break;};
              case WAVE3: { tmp=((30-index)>15)?15:(30-index);break;};
              case WAVE4: { tmp=index % 15;break;};
              case WAVE5: { tmp=15-index % 15;break;};
           };
        break;};
     };
   switch(CW_RGBflag)
    { case RED: { CW_red[index+2]=tmp;break;};
      case GREEN: { CW_green[index+2]=tmp;break;};
      case BLUE: { CW_blue[index+2]=tmp;break;};
    };

 };
_CWSetColMap();_CWDRGB();
}





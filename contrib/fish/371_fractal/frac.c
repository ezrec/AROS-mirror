/* frac.c 2.1 9004?? 946 lines */
#define PAL_HEIGHT 256
#define NTSC_HEIGHT 200

#define TV_SYSTEM PAL_HEIGHT
/* This define tells the computer what TV_system you use, if you live in
   europe, you probably have PAL and this definition is ok.
   If you live in north america you will have to change the definition
   to NTSC_HEIGHT. ( unless of course you have an european amiga and
                     european tv set. )
*/

#include <aros/oldprograms.h>
#include <stdio.h>
#include <math.h>
#include <exec/types.h>
#include <intuition/intuition.h>
#include "defs.h"
#define SCREENTITLE "Fractals v 2.1  Ronnie J"
/* for Writeilbmfile */
#define WSAVE 0L
#define NOSAVE 1L
#define NOOPEN 2L

short width=320;
short height=TV_SYSTEM;
short depth=5;
static unsigned short viewmode=0;
long titelflag=1L;

extern struct Gadget gad[];
extern struct Gadget gadb[];
extern char namebuf[];
extern char nameundo[];

/* Menues */
extern struct MenuItem mit[];
extern struct Menu men[];
       struct Menu *menptr;
static short  fracmenu=0;
       short surface=D2RECT;
       short xc,yc,xoffset,yoffset;

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Screen *scr,*openscreen();
struct Window *gwdw,*wdw,*openwindow();
struct IntuiMessage *imsg;
struct ViewPort *WVP;

void ColorWindow(),scanmenu(),newscreen();
void ShutDown(),SavePic(),PutBoolGadget(),PutGadget();
void newmenu();
ULONG class;
USHORT code;
short tmpcol,mennum,itmnum,subnum;
long WriteILBMFile();
void about();
void runfractal(),run1fractal();
void getfractal(),get1fractal();
void switchrun(),switchfrac();

/* functions in file formulas2.c */
void stratt();

void toggletitle(),ShowTitle();
double sqrt(),atan(),log(),atof(),exp(),sin(),cos(),fabs();

struct RastPort *rport;


void main()
{

IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",0L);
if(IntuitionBase==NULL){printf("Error opening intuition!\n");ShutDown();};

GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0L);
if(GfxBase==NULL){printf("Error opening graphics!\n");ShutDown();};

scr=openscreen(width,height,5,0);
if(scr==NULL){printf("Error opening screen!\n");ShutDown();};

wdw=openwindow(scr,0,0,scr->Width,scr->Height,
       (ULONG)(ACTIVATE|SMART_REFRESH|NOCAREREFRESH|BACKDROP|BORDERLESS),
       (USHORT)MENUPICK);
if(wdw==NULL){printf("Error opening window!\n");ShutDown();};

WVP=(struct ViewPort *)ViewPortAddress(wdw);
SetRGB4(WVP,0L,0L,0L,0L);
SetRGB4(WVP,1L,15L,15L,15L);

menptr=(men+0);

do
{
SetMenuStrip(wdw,menptr);
Wait(1L << wdw->UserPort->mp_SigBit);
ClearMenuStrip(wdw);
   while(imsg=(struct IntuiMessage *)GetMsg(wdw->UserPort))
      {
       class=imsg->Class;
       code=imsg->Code;
       ReplyMsg((struct Message *)imsg);
       if(class==MENUPICK)
          {
           mennum=MENUNUM(code);itmnum=ITEMNUM(code);subnum=SUBNUM(code);
           if(mennum==MENU0)
              {
               if(itmnum==ABOUT)about();
               if(itmnum==TITLE)toggletitle();
               if(itmnum==START)switchrun();
               if(itmnum==COLOR)ColorWindow(scr);
               if(itmnum==SAVE)SavePic();
              };
           if(mennum==MENU1)
              {
               if(itmnum==NEXT)newmenu();
               if(itmnum!=NEXT)switchfrac();
              };
           if(mennum==MENU2)newscreen();
           if(mennum==MENU3)surface=itmnum;
          };
     };
}
while((mennum!=MENU0)||(itmnum!=QUIT));

if(wdw)          CloseWindow(wdw);
if(scr)          CloseScreen(scr);
if(GfxBase)      CloseLibrary(GfxBase);
if(IntuitionBase)CloseLibrary(IntuitionBase);
}

void newscreen()
{
if(itmnum==LO)width=320;
if(itmnum==HI)width=640;
if(itmnum==NO)height=TV_SYSTEM;
if(itmnum==LA)height=2*TV_SYSTEM;
depth=5;viewmode=0;
if(height>300)  viewmode |= LACE;
if(width==640){ viewmode |= HIRES; depth=4; };

if(wdw)CloseWindow(wdw);
if(scr)CloseScreen(scr);
scr=NULL;wdw=NULL;
scr=openscreen(width,height,depth,viewmode);
if(scr!=NULL)
  {
   wdw=openwindow(scr,0,0,scr->Width,scr->Height,
       (ULONG)(ACTIVATE|SMART_REFRESH|NOCAREREFRESH|BACKDROP|BORDERLESS),
       (USHORT)MENUPICK);
  };
if((scr==NULL)||(wdw==NULL))
  {
   if(scr)CloseScreen(scr);
   scr=NULL;wdw=NULL;
   mit[7].Flags=CHECKED|CHECKIT|ITEMTEXT|HIGHBOX|ITEMENABLED;
   mit[8].Flags=CHECKIT|ITEMTEXT|HIGHBOX|ITEMENABLED;
   mit[9].Flags=CHECKED|CHECKIT|ITEMTEXT|HIGHBOX|ITEMENABLED;
   mit[10].Flags=CHECKIT|ITEMTEXT|HIGHBOX|ITEMENABLED;
   viewmode=0;width=320;height=TV_SYSTEM;depth=5;

   scr=openscreen(width,height,depth,viewmode);
   if(scr==NULL){printf("Error opening screen!\n");ShutDown();};

   wdw=openwindow(scr,0,0,scr->Width,scr->Height,
       (ULONG)(ACTIVATE|SMART_REFRESH|NOCAREREFRESH|BACKDROP|BORDERLESS),
       (USHORT)MENUPICK);
   if(wdw==NULL){printf("Error opening window!\n");ShutDown();};

  };
WVP=(struct ViewPort *)ViewPortAddress(wdw);
SetRGB4(WVP,0L,0L,0L,0L);
SetRGB4(WVP,1L,15L,15L,15L);
}

struct Window *openwindow(scr,left,top,width,height,flags,idcmp)
struct Screen *scr;
SHORT left,top,width,height;
ULONG flags;
USHORT idcmp;
{
struct NewWindow nwdw;
nwdw.LeftEdge=left;
nwdw.TopEdge=top;
nwdw.Width=width;
nwdw.Height=height;
nwdw.DetailPen=0;
nwdw.BlockPen=1;
nwdw.Title=NULL;
nwdw.Flags=flags;
nwdw.IDCMPFlags=idcmp;
nwdw.Type=CUSTOMSCREEN;
nwdw.FirstGadget=NULL;
nwdw.CheckMark=NULL;
nwdw.Screen=scr;
nwdw.BitMap=NULL;
nwdw.MinWidth=0;
nwdw.MaxWidth=0;
nwdw.MinHeight=0;
nwdw.MaxHeight=0;
return(OpenWindow(&nwdw));
}


void toggletitle()
{
titelflag=1-titelflag;
ShowTitle(scr,titelflag);
}



void scanmenu()
{
while((imsg=(struct IntuiMessage *)GetMsg(wdw->UserPort)))
    {
     class=imsg->Class;
     code=imsg->Code;
     ReplyMsg((struct Message *)imsg);
     if(class==MENUPICK)
        {
         mennum=MENUNUM(code);itmnum=ITEMNUM(code);subnum=SUBNUM(code);
         if(mennum==MENU0)
             {
              switch(itmnum)
                 {
                  case 0: {xc=xoffset=width;yc=yoffset=height;break;};
                  case 1: {ClearMenuStrip(wdw);ColorWindow(scr);
                           SetMenuStrip(wdw,(men+3));break;};
                  case 2: {toggletitle();break;};
                 };
             };
        };
    };
}


void PutGadget(text,chrs,xpos,ypos,gadptr)
UBYTE *text;
short chrs,xpos,ypos;
struct Gadget *gadptr;
{
Move(rport,(long)xpos,(long)ypos);
ypos+=5;
Text(rport,text,(long)chrs);
RectFill(rport,(long)(xpos-1),(long)(ypos-1),
         (long)(xpos+(*gadptr).Width-9),(long)(ypos+(*gadptr).Height-1));
(*gadptr).LeftEdge=xpos;
(*gadptr).TopEdge=ypos;
AddGadget(gwdw,gadptr,0L);
}

void PutBoolGadget(xpos,ypos,gadptr)
short xpos,ypos;
struct Gadget *gadptr;
{
RectFill(rport,(long)(xpos),(long)(ypos),
         (long)(xpos+(*gadptr).Width),(long)(ypos+(*gadptr).Height));
(*gadptr).LeftEdge=xpos;
(*gadptr).TopEdge=ypos;
AddGadget(gwdw,gadptr,0L);
RefreshGadgets(gadptr,gwdw,NULL);
}

struct Screen *openscreen(width,height,depth,viewmodes)
short width,height,depth;
unsigned short viewmodes;
{
struct NewScreen nscr;
nscr.LeftEdge=0;
nscr.TopEdge=0;
nscr.Width=width;
nscr.Height=height;
nscr.Depth=depth;
nscr.DetailPen=0;
nscr.BlockPen=1;
nscr.ViewModes=viewmodes;
nscr.Type=CUSTOMSCREEN;
nscr.Font=NULL;
nscr.DefaultTitle=(UBYTE *)SCREENTITLE;
nscr.Gadgets=NULL;
nscr.CustomBitMap=NULL;
return(OpenScreen(&nscr));
}

void about()
{
gwdw=openwindow(scr,50,15,200,170,(ULONG)ACTIVATE,(USHORT)GADGETUP,NULL);
rport=gwdw->RPort;
SetAPen(rport,1L);
Move(rport,65L,10L);Text(rport,"FRACTALS",8L);
Move(rport,10L,30L);Text(rport,"By Ronnie Johansson",19L);
Move(rport,10L,40L);Text(rport,"student of Mathematics",22L);
Move(rport,10L,50L);Text(rport,"University of Linkoping",23L);
Move(rport,10L,70L);Text(rport,"If you want to contact",22L);
Move(rport,10L,80L);Text(rport,"me, my address is :",19L);
Move(rport,10L,90L);Text(rport,"Hasselkulleg. 70",16L);
Move(rport,10L,100L);Text(rport,"461 62 TROLLHATTAN",18L);
Move(rport,10L,110L);Text(rport,"            SWEDEN",18L);

PutBoolGadget(110,130,(gadb+0));

Wait(1L << gwdw->UserPort->mp_SigBit);
while((imsg=(struct IntuiMessage *)GetMsg(gwdw->UserPort)))
      ReplyMsg((struct Message *)imsg);
CloseWindow(gwdw);
}


void SavePic()
{
long success;
char saveflag;
saveflag=0;
gwdw=openwindow(scr,50,50,150,100,(ULONG)ACTIVATE,
                (USHORT)(GADGETUP),NULL);
rport=gwdw->RPort;
SetAPen(rport,1L);
Move(rport,10L,10L);Text(rport,"SAVE - Fractal",14L);
PutGadget("Name",4,10,30,(gad+15));

PutBoolGadget(10,70,(gadb+1));
PutBoolGadget(90,70,(gadb+2));

Wait(1L << gwdw->UserPort->mp_SigBit);
while((imsg=(struct IntuiMessage *)GetMsg(gwdw->UserPort)))
   {
      if((((struct Gadget *)(imsg->IAddress))->GadgetID)==2)saveflag=1;
      ReplyMsg((struct Message *)imsg);
   };
CloseWindow(gwdw);
if(saveflag==1)
  {
   ShowTitle(scr,titelflag=0L);
   success=WriteILBMFile(namebuf,scr);
   ShowTitle(scr,titelflag=1L);

   /* Print result window */
   gwdw=openwindow(scr,50,50,150,100,(ULONG)ACTIVATE,
                (USHORT)(GADGETUP),NULL);
   rport=gwdw->RPort;
   SetAPen(rport,1L);
   Move(rport,10L,10L);
   if(success==WSAVE)
        { Text(rport,"Picture saved.",14L);    };
   if(success==NOSAVE)
        { Text(rport,"Error :",7L);            Move(rport,10L,20L);
          Text(rport,"Saving file.",12L); Move(rport,10L,30L);
          Text(rport,"File removed...",15L);   };
   if(success==NOOPEN)
        { Text(rport,"Error :",7L);            Move(rport,10L,20L);
          Text(rport,"Opening file.",13L);     Move(rport,10L,30L);
          Text(rport,"File not saved.",15L);   };
   PutBoolGadget(90,70,(gadb+0));
   ScreenToFront(scr);
   Wait(1L << gwdw->UserPort->mp_SigBit);
   while((imsg=(struct IntuiMessage *)GetMsg(gwdw->UserPort)))
          ReplyMsg((struct Message *)imsg);
   CloseWindow(gwdw);
  };
}

void ShutDown()
{
if(wdw)          CloseWindow(wdw);
if(scr)          CloseScreen(scr);
if(GfxBase)      CloseLibrary((struct Library *)GfxBase);
if(IntuitionBase)CloseLibrary((struct Library *)IntuitionBase);
exit(10);
}


void newmenu()
{
switch(fracmenu)
  {
   case 0: { menptr=(men+5);
             break; };
   case 1: { menptr=(men+0);
             break; };
  };

fracmenu++;
if(fracmenu>1)fracmenu=0;
}


void switchrun()
{
/* All algorithms can not use the standard plotting routines
   ,therefore we use switch to call appropriate function.
*/
switch(fracmenu)
  {
   case 0: { runfractal();break; };
   case 1: { run1fractal();break; };
  };
}


void switchfrac()
{
/* All frac menues have their own getfrac menu
*/
switch(fracmenu)
  {
   case 0: { getfractal();break; };
   case 1: { get1fractal();break; };
  };
}






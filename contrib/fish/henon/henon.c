/*  Hénon v1.10
 *  A program for drawing Hénon pictures.
 *  Written by Stefan Zeiger in May '91
 *  Updated 2/1992
 *  © 1992 by ! Wizard Works !
 *  Compiler : SAS/C 5.10
*/


#include <exec/types.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include <intuition/intuitionbase.h>
#include <intuition/intuition.h>
#include <intuition/iobsolete.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/mathieeedoubtrans.h>
#include <stdio.h>
#include <math.h>

#ifndef TOPAZ_EIGHTY 
#define TOPAZ_EIGHTY 8
#endif

int CXBRK(void) { return(0); }    /* Disable Lattice CTRL-C handling */
int chkabort(void) { return(0); } /* really */


#define SIZE (80.0)
#define XSIZE (160.0)


struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase=NULL;
struct Library * MathIeeeDoubTransBase = NULL;


struct Screen *my_screen=NULL;
struct NewScreen my_new_screen=
{
  0,               /* LeftEdge  */
  0,               /* TopEdge   */
  640,             /* Width     */
  200,             /* Height    */
  2,               /* Depth     */
  0,               /* DetailPen */
  1,               /* BlockPen  */
  HIRES,           /* ViewModes */
  CUSTOMSCREEN,    /* Type      */
  NULL,            /* Font      */
  "Hénon v1.00",   /* Title     */
  NULL,            /* Gadget    */
  NULL             /* BitMap    */
};


struct Window *my_window=NULL;
struct NewWindow my_new_window=
{
  0,             /* LeftEdge    */
  11,            /* TopEdge     */
  640,           /* Width       */
  189,           /* Height      */
  0,             /* DetailPen   */
  1,             /* BlockPen    */
  CLOSEWINDOW,   /* IDCMPFlags  */
                 /*             */
  SMART_REFRESH| /* Flags       */
  WINDOWCLOSE|   /*             */
  ACTIVATE,      /*             */
  NULL,          /* FirstGadget */
  NULL,          /* CheckMark   */
  "The Hénon picture :",
  NULL,          /* Screen      */
  NULL,          /* BitMap      */
  NULL,          /* MinWidth    */
  NULL,          /* MinHeight   */
  NULL,          /* MaxWidth    */
  NULL,          /* MaxHeight   */
  CUSTOMSCREEN   /* Type        */
};


struct TextAttr my_font=
{
  "topaz.font",TOPAZ_EIGHTY,NULL,FPF_ROMFONT
};

struct IntuiText black_text=
{
  1,0,JAM2,
  0,0,
  &my_font,
  NULL, /* Text */
  NULL
};


void cleanexit(void);


void main(int argc,char **argv)
{
  unsigned char buf[20];
  double xx,x,y,xa,a,xymin,xymax,xystep,cosinus,sinus,precalc;
  ULONG i,class,itera;
  BOOL close_me=FALSE;
  struct IntuiMessage *my_message;
  BOOL os2=TRUE;
  UWORD PenData[]={~0};

  /****** INTERPRET ARGUMENTS ******/

  if(argc == 2)
  {
    if(strcmp(argv[1],"?")==0)
    {
      Write(Output(),"Hénon v1.00 by ! WIZARD WORKS !\nUsage : ",40);
      Write(Output(),argv[0],strlen(argv[0]));
      Write(Output()," <a> <xymin> <xymax> <xystep> <itera>\n",38);
      cleanexit();
      return;
    }
  }
  
  if(argc!=6)
  {
    puts("Bad args.");
    cleanexit();
    return;
  }

  sscanf(argv[1],"%lf",&a);
  sscanf(argv[2],"%lf",&xymin);
  sscanf(argv[3],"%lf",&xymax);
  sscanf(argv[4],"%lf",&xystep);
  sscanf(argv[5],"%ld",&itera);
  
  /****** PREPARE EVERYTHING ******/

  IntuitionBase=(struct IntuitionBase *) OpenLibrary("intuition.library",37);
  if(IntuitionBase==NULL)
  {
    IntuitionBase=(struct IntuitionBase *) OpenLibrary("intuition.library",0);
    os2=FALSE;
    if(IntuitionBase==NULL)
    {
      puts("Can't open Intuition !");
      cleanexit();
      return;
    }
  }

  GfxBase=(struct GfxBase *) OpenLibrary("graphics.library",0);
  if(GfxBase==NULL)
  {
    puts("Can't open Graphics !");
    cleanexit();
    return;
  }
  
  MathIeeeDoubTransBase=(struct Library *)
      OpenLibrary("mathieeedoubtrans.library",0);
  if(MathIeeeDoubTransBase==NULL)
  {
    puts("Can't open IEEE library !");
    cleanexit();
    return;
  }

  if(os2) 
    my_screen=(struct Screen*) OpenScreenTags(&my_new_screen,SA_Pens,PenData,TAG_DONE,0);
  else 
    my_screen=(struct Screen*) OpenScreen(&my_new_screen);

  if(my_screen==NULL)
  {
    puts("Can't open Screen");
    cleanexit();
    return;
  }

  my_new_window.Screen=my_screen;
  my_window=(struct Window *) OpenWindow(&my_new_window);
  if(my_window==NULL)
  {
    puts("Can't open Window");
    cleanexit();
    return;
  }

  SetAPen(my_window->RPort,1);
  black_text.IText=buf;

  //cosinus=IEEEDPCos(a);
  //sinus=IEEEDPSin(a);
  cosinus = cos(a);
  sinus=sin(a);

  /****** DRAW PICTURE ******/
SetAPen(my_window->RPort,1);

  for(xa=xymin;xa<=xymax;xa+=xystep)
  {
    x=xa;
    y=xa;

    for(i=0;i<=itera;i++)
    {
      precalc=y-(x*x);
      xx=x*cosinus-precalc*sinus;
      y=x*sinus+precalc*cosinus;
      x=xx;
      WritePixel(my_window->RPort,(ULONG)((XSIZE*x)+370.0),(ULONG)(105-(SIZE*y)));
    }
    my_message=(struct IntuiMessage *)GetMsg(my_window->UserPort);
    if(my_message)
    {
      class=my_message->Class;
      ReplyMsg((struct Message *)my_message);
      if(class==CLOSEWINDOW) close_me=TRUE;
    }
    if(close_me==TRUE) break;
  }

  /****** PRINT INFO ******/
  sprintf(buf,"A      : %lf",a);
  PrintIText(my_window->RPort,&black_text,8,15);
  sprintf(buf,"XYMin  : %lf",xymin);
  PrintIText(my_window->RPort,&black_text,8,23);
  sprintf(buf,"XYMax  : %lf",xymax);
  PrintIText(my_window->RPort,&black_text,8,31);
  sprintf(buf,"XYStep : %lf",xystep);
  PrintIText(my_window->RPort,&black_text,8,39);
  sprintf(buf,"Itera  : %ld",itera);
  PrintIText(my_window->RPort,&black_text,8,47);

  /****** TERMINATE PROGRAM ******/

  while(close_me==FALSE)
  {
    Wait(1<<my_window->UserPort->mp_SigBit);
    my_message=(struct IntuiMessage *)GetMsg(my_window->UserPort);
    if(my_message)
    {
      class=my_message->Class;
      ReplyMsg((struct Message *)my_message);
      if(class==CLOSEWINDOW)
        close_me=TRUE;
    }
  }

  cleanexit();
}

void cleanexit()
{
  if(my_window) CloseWindow(my_window);
  if(my_screen) CloseScreen(my_screen);
  if(GfxBase) CloseLibrary((struct Library *)GfxBase);
  if(IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
}

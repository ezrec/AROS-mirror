/*
    (C) 1995-97 AROS - The Amiga Replacement OS
    $Id$

    Desc: JumpEd Game
    Lang: german
*/

/*****************************************************************************

    NAME

        JumpEd

    SYNOPSIS

    LOCATION

        Workbench:Games

    BUGS

    SEE ALSO

        Jump Mine Quad Wumpus

    INTERNALS

    HISTORY

        24-Aug-1997     hkiel     Initial inclusion into the AROS tree

******************************************************************************/

static const char version[] = "$VER: JumpEd 0.1 (29.08.1997)\n";

#include "../prec.c"
#include "JumpEd.h"

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Window *Window;
struct RastPort *rp;
struct IntuiMessage *msg;

ULONG class,iflags;
USHORT code;

int field[33];

#define DA  1
#define WEG 0

#include "JumpEdDatei.h"

void open_lib()
{
  IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library",0L);
  GfxBase = (struct GfxBase *) OpenLibrary("graphics.library",0L);
}

void open_window()
{
  if((Window=(struct Window *)OpenWindow(NEWWINDOW))==NULL)
    exit(FALSE);
  rp =Window->RPort;
  iflags=Window->IDCMPFlags;
}

#define StopMsg() ModifyIDCMP(Window,NULL)
#define ContMsg() ModifyIDCMP(Window,iflags)

close_window()
{
  CloseWindow(Window);
}

close_lib()
{
  CloseLibrary((struct Library *)IntuitionBase);
  CloseLibrary((struct Library *)GfxBase);
}

wbmain()
{
  main();
}

main()
{
int nr,i;
BOOL ende=FALSE;

open_lib();
open_window();
open_datei();

while(ende!=TRUE)
{
 Wait(1L<<Window->UserPort->mp_SigBit);
 msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
 class=msg->Class;
 code=msg->Code;
 switch(class)
 {
   case IDCMP_CLOSEWINDOW:
         ende=TRUE;
         break;
   case IDCMP_RAWKEY:
         switch(code)
         {
           case   9:
                 ende=TRUE;
                 break;
           case  46:
                 open_datei();
                 break;
           case  39:
                 schreibedatei();
                 break;
           default:
                 break;
         }
         break; 
   case IDCMP_GADGETUP:
         switch(nr=((struct Gadget *)(msg->IAddress))->GadgetID)
         {
           case  0:
                 open_datei();
                 break;
           case 34:
                 schreibedatei();
                 break;
           case 35:
                 for(i=0;i<33;i++)
                   field[i]=DA;
                 field[16]=WEG;
                 break;
           default:
                 if(field[nr-1]==DA)
                   field[nr-1]=WEG;
                 else
                   field[nr-1]=DA;
                 break;
         }
         break;
   default:
         break;
 }
 set_buttons();
 ReplyMsg((struct Message *)msg);
}
close_window();
close_lib();
}

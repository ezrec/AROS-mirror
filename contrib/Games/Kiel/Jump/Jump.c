/*
    (C) 1995-97 AROS - The Amiga Replacement OS
    $Id$

    Desc: Jump Game
    Lang: german
*/

/*****************************************************************************

    NAME

        Jump

    SYNOPSIS

    LOCATION

        Workbench:Games

    BUGS

    SEE ALSO

        JumpEd Mine Quad Wumpus

    INTERNALS

    HISTORY

        24-Aug-1997     hkiel     Initial inclusion into the AROS tree

******************************************************************************/

static const char version[] = "$VER: Jump 0.1 (29.08.1997)\n";

#include "../prec.c"
#include "Jump.h"

#define FIRST 1
#define SECOND 2

int status=FIRST;

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Window *Window;
struct RastPort *rp;
struct IntuiMessage *msg;

ULONG class,iflags;
USHORT code;

int Feld[33];
int check[33][33]=
{
 {0,0,2,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {2,0,0,0,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,15,0,0,0,0,0,0,0,0,0,0,0},
 {4,0,0,0,0,0,8,0,0,0,10,0,0,0,0,0,0,0,0,0,0,0,16,0,0,0,0,0,0,0,0,0,0},
 {0,5,0,0,0,0,0,9,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,17,0,0,0,0,0,0,0,0,0},

 {0,0,6,0,0,0,0,0,10,0,0,0,12,0,0,0,0,0,0,0,0,0,0,0,18,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,12,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,9,0,0,0,0,0,0,0,0,0,15,0,0,0,17,0,0,0,0,0,0,0,0,0,23,0,0,0,0,0},
 {0,0,0,0,10,0,0,0,0,0,0,0,0,0,16,0,0,0,18,0,0,0,0,0,0,0,0,0,24,0,0,0,0},
 {0,0,0,0,0,11,0,0,0,0,0,0,0,0,0,17,0,0,0,19,0,0,0,0,0,0,0,0,0,25,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,19,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

 {0,0,0,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,23,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,16,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,24,0,0,0,0,0,28,0,0},
 {0,0,0,0,0,0,0,0,0,17,0,0,0,0,0,0,0,0,0,0,0,23,0,0,0,25,0,0,0,0,0,29,0},
 {0,0,0,0,0,0,0,0,0,0,18,0,0,0,0,0,0,0,0,0,0,0,24,0,0,0,26,0,0,0,0,0,30},
 {0,0,0,0,0,0,0,0,0,0,0,19,0,0,0,0,0,0,0,0,0,0,0,25,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,20,0,0,0,0,0,0,0,0,0,0,0,26,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,23,0,0,0,0,0,0,0,0,0,0,0,0,0,29,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,25,0,0,0,0,0,0,0,0,0,29,0,0,0,0,0},

 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,28,0,0,0,0,0,0,0,0,0,32},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,29,0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,32,0,0}
};

#define DA  1
#define WEG 2
int ist;

int history[32][3],movecount;

#include "JumpDatei.h"

 oeffnelib()
{
  IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library",0L);
  GfxBase = (struct GfxBase *) OpenLibrary("graphics.library",0L);
}

oeffnewindow()
{
  if((Window=(struct Window *)OpenWindow(NEWWINDOW))==NULL)
    exit(FALSE);
  rp =Window->RPort;
  iflags=Window->IDCMPFlags;
}

LoescheWin()
{
  SetAPen(rp,0);
  RectFill(rp,0,0,400,170);
}

StopMsg()
{
  ModifyIDCMP(Window,NULL);
}

ContMsg()
{
  ModifyIDCMP(Window,iflags);
}

schliessewindow()
{
  CloseWindow(Window);
}

schliesselib()
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
BOOL ende=FALSE;
int erster=0,nr,mitte;

oeffnelib();
oeffnewindow();
oeffnedatei();

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
           case  22:
                 if(movecount>0)
                 {
                   movecount--;
                   Feld[history[movecount][0]-1]=DA;
                   Feld[history[movecount][1]-1]=WEG;
                   Feld[history[movecount][2]-1]=DA;
                   status=FIRST;
                   setzen();
                 }
                 break;
           case   9:
                 ende=TRUE;
                 break;
           case  57:
                 status=FIRST;
                 oeffnedatei();
                 break;
           default:
                 break;
         }
         break; 
   case IDCMP_GADGETUP:
         switch(nr=((struct Gadget *)(msg->IAddress))->GadgetID)
         {
           case  0:
                 status=FIRST;
                 oeffnedatei();
                 break;
           case 34:
                 if(movecount>0)
                 {
                   movecount--;
                   Feld[history[movecount][0]-1]=DA;
                   Feld[history[movecount][1]-1]=WEG;
                   Feld[history[movecount][2]-1]=DA;
                   status=FIRST;
                   setzen();
                 }
                 break;
           default:
                 switch(status)
                 {
                   case FIRST:
                         if(Feld[nr-1]==DA)
                         {
                           Feld[nr-1]=WEG;
                           ist=nr;
                           status=SECOND;
                           erster=nr;
                           setzen();
                         }
                         break;
                   case SECOND:
                         if(Feld[nr-1]==WEG)
                           if(erster==nr)
                           {
                             Feld[nr-1]=DA;
                             status=FIRST;
                             setzen();
                           }
                           else
                             if((mitte=check[erster-1][nr-1])!=0&&Feld[mitte-1]==DA)
                             {
                               Feld[nr-1]=DA;
                               Feld[mitte-1]=WEG;
                               history[movecount][0]=erster;
                               history[movecount][1]=nr;
                               history[movecount][2]=mitte;
                               erster=0;
                               status=FIRST;
                               setzen();
                               movecount++;
                             }
                         break;
                 }
         }
         break;
   default:
         break;
 }
 ReplyMsg((struct Message *)msg);
}
schliessewindow();
schliesselib();
}

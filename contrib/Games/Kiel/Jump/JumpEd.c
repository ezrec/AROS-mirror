#include "../prec.c"
#include "JumpEd.h"

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Window *Window;
struct RastPort *rp;
struct IntuiMessage *msg;

ULONG class,iflags;
USHORT code;

int Feld[33];

#define DA  1
#define WEG 0

#include "JumpEdDatei.h"

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

StopMsg()
{
  ModifyIDCMP(Window,NULL);
}

ContMsg()
{
  ModifyIDCMP(Window,iflags);
}

LoescheWin()
{
  SetAPen(rp,0);
  RectFill(rp,0,0,400,170);
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
int nr,i;
BOOL ende=FALSE;

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
           case   9:
                 ende=TRUE;
                 break;
           case  46:
                 oeffnedatei();
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
                 oeffnedatei();
                 break;
           case 34:
                 schreibedatei();
                 break;
           case 35:
                 for(i=0;i<33;i++)
                   Feld[i]=DA;
                 Feld[16]=WEG;
                 setzen();
                 break;
           default:
                 if(Feld[nr-1]==DA)
                   Feld[nr-1]=WEG;
                 else
                   Feld[nr-1]=DA;
                 setzen();
                 break;
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

#include "../prec.c"
#include <graphics/gfx.h>
#include <time.h>

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Window *Window;
struct RastPort *rp;
struct IntuiMessage *msg;

ULONG iflags;
ULONG class;
USHORT code;

#define random(max) ((rand()%max)+1)

oeffnelib()
{
  IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library",0L);
  GfxBase = (struct GfxBase *) OpenLibrary("graphics.library",0L);
}

oeffnewindow(NeuesFenster)
struct NewWindow NeuesFenster;
{
  Window = (struct Window *) OpenWindow(&NeuesFenster);
  rp=Window->RPort;
  ActivateWindow(Window);
  iflags=Window->IDCMPFlags;
  SetBPen(rp,0);
}

schreibe(x, y, wort, farbe)
int x, y, farbe;
char *wort;
{
  SetAPen(rp,farbe);
  Move(rp,x,y);
  Text(rp,wort,strlen(wort));
}

maleFeld(x1,y1,x2,y2)
SHORT x1,y1,x2,y2;
{
  SetAPen(rp,2);
  Move(rp,x1,y2);
  Draw(rp,x1,y1);
  Draw(rp,x2,y1);
  SetAPen(rp,1);
  Move(rp,x2,y1);
  Draw(rp,x2,y2);
  Draw(rp,x1,y2);
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
  RectFill(rp,0,0,Window->Width-5,Window->Height-11);
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

#include "../prec.c"

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Window *Window;
struct RastPort *rp;
struct IntuiMessage *msg;

ULONG iflags;

#define StopMsg() ModifyIDCMP(Window,0L)
#define ContMsg() ModifyIDCMP(Window,iflags)
#define LoescheWin() EraseRect(rp,0,0,Window->Width-5,Window->Height-11)

void open_lib()
{
  IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library",0L);
  GfxBase = (struct GfxBase *) OpenLibrary("graphics.library",0L);
}

void open_window(NeuesFenster)
struct NewWindow NeuesFenster;
{
  Window = (struct Window *) OpenWindow(&NeuesFenster);
  rp=Window->RPort;
  iflags=Window->IDCMPFlags;
  ActivateWindow(Window);
}

void schreibe(x, y, wort, farbe)
int x, y, farbe;
char *wort;
{
SetAPen(rp,farbe);
  Move(rp, x, y);
  Text(rp, wort, strlen( wort));
}

void maleFeld(x1,y1,x2,y2)
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

/* AROS unimplemented
void copypic(x1,y1,dx,dy,x2,y2)
{
ClipBlit(rp,x1,y1,rp,x2,y2,dx,dy,192);
} */

void WinSize(Win,x,y)
struct Window *Win;
SHORT x,y;
{
SHORT altx,alty;
  altx=Win->Width;
  alty=Win->Height;
  Win->LeftEdge=10;
  Win->TopEdge=10;
  SizeWindow(Win,x-altx,y-alty);
}

void schliessewindow()
{
  CloseWindow(Window);
}

void schliesselib()
{
  CloseLibrary((struct Library *)IntuitionBase);
  CloseLibrary((struct Library *)GfxBase);
}

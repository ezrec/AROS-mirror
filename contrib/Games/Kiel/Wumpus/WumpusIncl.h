#include "../prec.c"

#define CATALOG_NAME "contrib/Games/Kiel/Wumpus.catalog"
#define CATALOG_VERSION 0
#define CATCOMP_ARRAY
#include "strings.h"
#include "../locale.c"

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Window *Window;
struct RastPort *rp;
struct IntuiMessage *msg;

ULONG iflags;
ULONG class;

#define random(max) ((rand()%max)+1)
#define StopMsg() ModifyIDCMP(Window,0L)
#define ContMsg() ModifyIDCMP(Window,iflags)
#define clear_win() EraseRect(rp,0,0,Window->Width-5,Window->Height-11)

void open_lib()
{
  IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library",0L);
  GfxBase = (struct GfxBase *) OpenLibrary("graphics.library",0L);
}

void open_window(struct NewWindow *newwindow)
{
  Window = (struct Window *) OpenWindow(newwindow);
  rp=Window->RPort;
  ActivateWindow(Window);
  iflags=Window->IDCMPFlags;
  SetBPen(rp,0);
}

void write_text(LONG x, LONG y, CONST_STRPTR text, ULONG color)
{
  SetAPen(rp,color);
  Move(rp,x,y);
  Text(rp,text,strlen(text));
}

void draw_rect(LONG x1, LONG y1, LONG x2, LONG y2)
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

void close_window()
{
  CloseWindow(Window);
}

void close_lib()
{
  CloseLibrary((struct Library *)IntuitionBase);
  CloseLibrary((struct Library *)GfxBase);
}

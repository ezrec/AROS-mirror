#include "../prec.c"

#define DEBUG 1
#include <aros/debug.h>

#define CATALOG_NAME "contrib/Games/Kiel/Mine.catalog"
#define CATALOG_VERSION 0
#define CATCOMP_ARRAY
#include "strings.h"
#include "../locale.c"

extern struct ExecBase *SysBase;
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Window *Window;
struct RastPort *rp;
struct IntuiMessage *msg;

#define clearwin() EraseRect(rp,0,0,Window->Width-5,Window->Height-11)
#define copypic(x1,y1,dx,dy,x2,y2) ClipBlit(rp,x1,y1,rp,x2,y2,dx,dy,192)

void close_lib();

void open_lib()
{
  IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library",0L);
  if(IntuitionBase==NULL)
  {
    fprintf(stderr,"Unable to open intuition.library.\n");
    exit(-1);
  }
  GfxBase = (struct GfxBase *) OpenLibrary("graphics.library",0L);
  if(GfxBase==NULL)
  {
    fprintf(stderr,"Unable to open graphics.library.\n");
    CloseLibrary((struct Library *)IntuitionBase);
    exit(-1);
  }
}

void open_window(struct NewWindow *newwindow)
{
  Window = (struct Window *) OpenWindow(newwindow);
  if(Window==NULL)
  {
    fprintf(stderr,"Unable to open window.\n");
    close_lib();
    exit(-1);
  }
  rp=Window->RPort;
}

void write_text(LONG x, LONG y, CONST_STRPTR text, ULONG color)
{
  SetAPen(rp,color);
  Move(rp, x, y);
  Text(rp, text, strlen(text));
}

void drawfield(LONG x1, LONG y1, LONG x2, LONG y2)
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

BOOL WinSize(struct Window *Win, LONG x, LONG y)
{
ULONG class;

  if(Win->Width!=x && Win->Height!=y)
  {
    ChangeWindowBox(Win, 10, 10, x, y);
    while(1)
    {
      WaitPort(Win->UserPort);
      msg=(struct IntuiMessage *)GetMsg(Win->UserPort);
      class=msg->Class;
      ReplyMsg((struct Message *)msg);
      if(class==IDCMP_CHANGEWINDOW)
        break;
    }
  }
return((Win->Width==x) && (Win->Height==y));
}

void close_window()
{
  CloseWindow(Window);
}

void close_lib()
{
  CloseLibrary((struct Library *)GfxBase);
  CloseLibrary((struct Library *)IntuitionBase);
}


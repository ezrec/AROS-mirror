#include "../prec.c"

extern struct ExecBase *SysBase;
extern struct Gadget *endreqglist, *gad;
extern struct NewGadget gt_endreqfalse, gt_endreqtrue;
struct IntuitionBase *IntuitionBase;
struct Library *GadToolsBase;
struct GfxBase *GfxBase;
struct Screen *scr;
APTR vi;
struct Window *Window;
struct RastPort *rp;
struct IntuiMessage *msg;

#define clearwin() EraseRect(rp,0,0,Window->Width-5,Window->Height-11)
#define copypic(x1,y1,dx,dy,x2,y2) ClipBlit(rp,x1,y1,rp,x2,y2,dx,dy,192)

void close_lib();

void open_lib()
{
struct TagItem ti1[] = {
  { GA_Immediate, TRUE },
  { TAG_DONE }
};

  IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library",0L);
  GfxBase = (struct GfxBase *) OpenLibrary("graphics.library",0L);
  GadToolsBase = OpenLibrary( "gadtools.library", 0L );
  if (GadToolsBase == NULL)
  {
    fprintf(stderr,"Could not open gadtools.library\n");
    exit(-1);
  }

  scr = LockPubScreen( NULL );
  vi = GetVisualInfoA( scr, NULL );

  gt_endreqtrue.ng_VisualInfo = vi;
  gt_endreqfalse.ng_VisualInfo = vi;

  gad = CreateContext( &endreqglist );
  if(gad==NULL)
    printf("CreateContext() failed\n");

  gad = CreateGadgetA( BUTTON_KIND, gad, &gt_endreqtrue, ti1 );
  gad = CreateGadgetA( BUTTON_KIND, gad, &gt_endreqfalse, ti1 );

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

void write_text(LONG x, LONG y, char *text, ULONG color)
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
      if(msg->Class==IDCMP_CHANGEWINDOW)
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
  FreeGadgets( endreqglist );
  FreeVisualInfo( vi );
  UnlockPubScreen( NULL, scr );
  CloseLibrary( (struct Library *)GadToolsBase );
  CloseLibrary((struct Library *)IntuitionBase);
  CloseLibrary((struct Library *)GfxBase);
}


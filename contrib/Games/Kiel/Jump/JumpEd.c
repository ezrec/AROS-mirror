/*
    (C) 1995-97 AROS - The Amiga Research OS
    $Id$

    Desc: JumpEd Game
    Lang: english
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
	16-Sep-1997     hkiel     Fixed all casts

******************************************************************************/

static const char version[] = "$VER: JumpEd 0.2 (16.09.1997)\n";

#include "../prec.c"
#include "JumpEd.h"

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Window *Window;
struct RastPort *rp;
struct IntuiMessage *msg;

ULONG class,iflags;
UWORD code;

int field[33];

#define THERE	1
#define AWAY	0

#include "JumpEdFile.h"

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

#define StopMsg() ModifyIDCMP(Window,0L)
#define ContMsg() ModifyIDCMP(Window,iflags)

void close_window()
{
  CloseWindow(Window);
}

void close_lib()
{
  CloseLibrary((struct Library *)IntuitionBase);
  CloseLibrary((struct Library *)GfxBase);
}

int main()
{
int number,i;
BOOL end_editor=FALSE;

/* Do some initting, including loading of existing defaults-file */
  open_lib();
  open_window();
  open_file();

/* The main loop of the editor */
  while(end_editor!=TRUE)
  {
    WaitPort(Window->UserPort);
    msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
    class=msg->Class;
    code=msg->Code;
    switch(class)
    {
      case IDCMP_CLOSEWINDOW:
            end_editor=TRUE;
            break;
      case IDCMP_RAWKEY:
            switch(code)
            {
              case   9: /* ESC */
                    end_editor=TRUE;
                    break;
              case  46: /* o */
                    open_file();
                    break;
              case  39: /* s */
                    write_file();
                    break;
              case  40: /* d */
                    for(i=0;i<33;i++)
                      field[i]=THERE;
                    field[16]=AWAY;
                    break;
              default:
                    break;
            }
            break; 
      case IDCMP_GADGETUP:
            switch(number=((struct Gadget *)(msg->IAddress))->GadgetID)
            {
              case  0:
                    open_file();
                    break;
              case 34:
                    write_file();
                    break;
              case 35:
                    for(i=0;i<33;i++)
                      field[i]=THERE;
                    field[16]=AWAY;
                    break;
              default:
                    if(field[number-1]==THERE)
                      field[number-1]=AWAY;
                    else
                      field[number-1]=THERE;
                    break;
            }
            break;
      default:
            break;
    }
    set_buttons();
    ReplyMsg((struct Message *)msg);
  }
/* Quit editor, do some clean up */
  close_window();
  close_lib();
  return(0);
}

/*
    (C) 1995-98 AROS - The Amiga Research OS
    $Id$

    Desc: Jump Game
    Lang: english
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
        16-Sep-1997     hkiel     Fixed all casts

******************************************************************************/

static const char version[] = "$VER: Jump 0.2 (16.09.1997)\n";

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
UWORD code;

int field[33];
/* This array contains the middle position(to be removed) for given
   first(row) and second(column) stone, returns 0 for impossible move */
BYTE check[33][33]=
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

#define THERE	1
#define AWAY	0
int marked;

int history[32][3],movecount;

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

#include "JumpFile.h"

int main()
{
BOOL end_game=FALSE;
int first=0,number,middle;

/* As usual some initting, including loading defaults-file or setting
   playfield to default if no defaults-file found */
  open_lib();
  open_window();
  open_file();

/* The main loop of the game */
  while(end_game!=TRUE)
  {
    WaitPort(Window->UserPort);
    msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
    class=msg->Class;
    code=msg->Code;
    switch(class)
    {
      case IDCMP_CLOSEWINDOW:
            end_game=TRUE;
            break;
      case IDCMP_RAWKEY:
            switch(code)
            {
/* Undo last move */
              case  22: /* Backspace */
                    if(movecount>0)
                    {
                      movecount--;
                      field[history[movecount][0]-1]=THERE;
                      field[history[movecount][1]-1]=AWAY;
                      field[history[movecount][2]-1]=THERE;
                      status=FIRST;
                    }
                    break;
/* Quit game */
              case   9: /* ESC */
                    end_game=TRUE;
                    break;
/* New game */
              case  57: /* n */
                    status=FIRST;
                    open_file();
                    break;
              default:
                    break;
            }
            break; 
      case IDCMP_GADGETUP:
            switch(number=((struct Gadget *)(msg->IAddress))->GadgetID)
            {
/* New game */
              case  0:
                    status=FIRST;
                    open_file();
                    break;
/* Undo last move */
              case 34:
                    if(movecount>0)
                    {
                      movecount--;
                      field[history[movecount][0]-1]=THERE;
                      field[history[movecount][1]-1]=AWAY;
                      field[history[movecount][2]-1]=THERE;
                      status=FIRST;
                    }
                    break;
              default:
/* Handle clicks on stones */
                    switch(status)
                    {
/* selected first stone, so mark it */
                      case FIRST:
                            if(field[number-1]==THERE)
                            {
                              field[number-1]=AWAY;
                              marked=number;
                              status=SECOND;
                              first=number;
                            }
                            break;
/* selected second stone */
                      case SECOND:
                            if(field[number-1]==AWAY)
                            {
                              if(first==number)
                              {
/* clicked on first stone again, so unmark it */
                                field[number-1]=THERE;
                                status=FIRST;
                              }
                              else
                              {
/* clicked on different stone than first, so check if move is OK
   and remove stones or do nothing */
                                if((middle=check[first-1][number-1])!=0)
                                if(field[middle-1]==THERE)
                                {
                                  field[number-1]=THERE;
                                  field[middle-1]=AWAY;
                                  history[movecount][0]=first;
                                  history[movecount][1]=number;
                                  history[movecount][2]=middle;
                                  first=0;
                                  status=FIRST;
                                  movecount++;
                                }
                              }
                            }
                            break;
                    }
            }
            break;
      default:
            break;
    }
/* Update playfield */
    set_buttons();
    ReplyMsg((struct Message *)msg);
  }
/* Terminated game, clean up */
  close_window();
  close_lib();
  return(0);
}

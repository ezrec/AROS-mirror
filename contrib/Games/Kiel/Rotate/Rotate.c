/*
    (C) 1999 AROS - The Amiga Research OS
    $Id$

    Desc: Rotate Game
    Lang: german
*/

/*****************************************************************************

    NAME

        Rotate

    SYNOPSIS

    LOCATION

        Workbench:Games

    BUGS

    SEE ALSO

        Jump JumpEd Mine Quad Wumpus

    INTERNALS

    HISTORY

        12-Jan-1999     hkiel     Initial inclusion into the AROS tree

******************************************************************************/

static const char version[] = "$VER: Rotate 0.1 (14.02.2000)\n";

#include "includes.h"
#include "defines.h"

struct Level {int b,h,e,c,t,e2;};

struct Level currlevel,newlevel={20,7,6,4,0,15};

BOOL error=FALSE,game=FALSE,replace,ende=FALSE;
int playfield[MAXX][MAXY],rotcount,remcount;

struct IntuiText p = {1,0,JAM1,10,30,NULL,(UBYTE *)"OK",NULL };
struct IntuiText n = {1,0,JAM1,70,30,NULL,(UBYTE *)"Cancel",NULL };
struct IntuiText w = {1,0,JAM1,70,30,NULL,(UBYTE *)"Next",NULL };
struct IntuiText i_body = {1,0,JAM1,10,10,NULL,(UBYTE *)"Rotate!\n Version 1.0\n Copyright by Henning Kiel in 1995.",NULL };
struct IntuiText q_body = {1,0,JAM1,10,10,NULL,(UBYTE *)"Do you really want to quit Rotate!  !?!\n",NULL };
struct IntuiText s_body = {1,0,JAM1,10,10,NULL,(UBYTE *)"Do you really want to abort current game ???",NULL };
struct IntuiText w_body = {1,0,JAM1,10,10,NULL,(UBYTE *)"You have won !",NULL };
struct IntuiText a_body = {1,0,JAM1,10,10,NULL,(UBYTE *)"Do you really want to give up ?",NULL };

#include "screen.h"
#include "functions.h"


void init()
{
  open_libs();
  srand((unsigned)time(NULL));
  open_screen();
}

void cleanup()
{
  close_screen();
  close_libs();
}

void menue()
{
int mx,my,ax=0,ay=0;

  while(!ende)
  {
    ContMsg();
    WaitPort(Window->UserPort);
    msg=(struct IntuiMessage *)GetMsg(Window->UserPort);
    class=msg->Class;
    code=msg->Code;
    mx=msg->MouseX;
    my=msg->MouseY;
    ReplyMsg((struct Message *)msg);
    StopMsg();
    switch(class)
    {
      case IDCMP_CLOSEWINDOW:
        if(!game)
          MenuOff();
        ende=AutoRequest(Window,&q_body,&p,&n,0L,0L,200,75);
        if(!game)
          MenuOn();
        break;

      case IDCMP_MENUPICK:
        getmenu();
        break;

      case IDCMP_MOUSEBUTTONS:
        if(code==SELECTUP&&game)
        {
          mx=(mx-bleft)/fwidth;
          my=(Window->Height-my-bbottom)/fheight;
          if(mx==ax&&my==ay)
          {
            rotcount++;
            rotate(mx,my);
            go();
          }
          else
            if((ax>0)&&(ax<(currlevel.b-1))&&(ay>0)&&(ay<(currlevel.h-1)))
              drawfield(ax,ay,playfield[ax][ay]);
        }
        if(code==SELECTDOWN&&game)
        {
          ax=(mx-bleft)/fwidth;
          ay=(Window->Height-my-bbottom)/fheight;
          if((ax>0)&&(ax<(currlevel.b-1))&&(ay>0)&&(ay<(currlevel.h-1)))
            push(ax,ay);
        }
        break;

      default:
        break;
    }
  }
}


/* ------------------------------------------------------------------------------ */

int main();

void wbmain()
{
  main();
}

int main()
{
  init();

  if(!error)
    menue();

  cleanup();
  return 0;
}

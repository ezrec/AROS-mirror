/*********************************************************************
** Bomberman-clone                                                  **
**   by Steven Don                                                  **
*********************************************************************/

#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include <cybergraphx/cybergraphics.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/exec.h>
#include <aros/debug.h>

#include <stdio.h>
#include <memory.h>
#include <time.h>

#define NO_MIDI 1
#define NO_SOUND 1
#define NO_JOY 1

#define WIDTH 272
#define HEIGHT 272

struct Window *win;
struct Screen *scr;

UBYTE *MainBitmapData,
      *BackBitmapData,
      *TargetData;

//Keyboard input
char            InBuffer [50];
volatile int    InPointer = 0, OutPointer = 0;

//Global indicator
int         Done;

int keys[128];

#define VK_RETURN 0x44

#define GetAsyncKeyState(x) keys[x]

static void cleanup(char *msg)
{
    if (msg) printf("Bomber: %s\n", msg);
    
    if (win) CloseWindow(win);
    if (scr) UnlockPubScreen(NULL, scr);
    
    exit(0);
}

#include "GraphicStuff.c"
#include "Game.c"
#include "WindowStuff.c"

static void HandleIntuiMessages(void)
{
  struct IntuiMessage *msg;
  
  while((msg = (struct IntuiMessage *)GetMsg(win->UserPort)))
  {
    switch(msg->Class)
    {
      case IDCMP_CLOSEWINDOW:
	Done = TRUE;
	break;

      case IDCMP_RAWKEY:
	if (msg->Code & IECODE_UP_PREFIX)
	{
	  keys[msg->Code & ~IECODE_UP_PREFIX] = 0;
	}
	else
	{
	  keys[msg->Code] = 1;
	  InBuffer[InPointer++]= msg->Code;
	  InPointer %= 50;
	}
	break;
    }
    ReplyMsg((struct Message *)msg);
  }
}

int main(void)
{
  InitWindow();
  InitGraph ();

  //Initialize game
  InitGame ();

  do
  {
    //Draw one game frame
    GameFrame ();

    WriteLUTPixelArray(MainBitmapData,
    	    	       0,
		       0,
		       WIDTH,
		       win->RPort,
		       Palette,
		       win->BorderLeft,
		       win->BorderTop,
		       WIDTH,
		       HEIGHT,
		       CTABFMT_XRGB8);
		       

    HandleIntuiMessages();
       
  } while (!Done);

  //Shut down the game
  CloseGame ();

  //Restore the graphic display
  CloseGraph ();

  cleanup(0);
  
  return 0;
}

/**************************************
*  TEST.C  08/04/90
*  Written by Timm Martin
*  This source code is public domain.
***************************************/

#include <aros/oldprograms.h>

#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <stdio.h>
#include "input.h"

/********************
*  SHARED VARIABLES
*********************/

struct IntuitionBase *IntuitionBase = NULL;
struct Window        *win           = NULL;

/************************
*  NEW WINDOW STRUCTURE
*************************/

struct NewWindow new_window =
{
  /* SHORT           LeftEdge    */ 0,
  /* SHORT           TopEdge     */ 0,
  /* SHORT           Width       */ 640,
  /* SHORT           Height      */ 40,
  /* UBYTE           DetailPen   */ 0,
  /* UBYTE           BlockPen    */ 1,
  /* ULONG           IDCMPFlags  */ CLOSEWINDOW|RAWKEY,
  /* ULONG           Flags       */ ACTIVATE|NOCAREREFRESH|SIMPLE_REFRESH|WINDOWCLOSE|WINDOWDEPTH|WINDOWDRAG|WINDOWSIZING,
  /* struct Gadget * FirstGadget */ NULL,
  /* struct Image *  CheckMark   */ NULL,
  /* UBYTE *         Title       */ (STRPTR)"Input Test Program",
  /* struct Screen * Screen      */ NULL,
  /* struct BitMap * BitMap      */ NULL,
  /* SHORT           MinWidth    */ 230,
  /* SHORT           MinHeight   */ 20,
  /* USHORT          MaxWidth    */ 640,
  /* USHORT          MaxHeight   */ 200,
  /* USHORT          Type        */ WBENCHSCREEN,
};

/*************
*  FUNCTIONS
**************/

int  main( int argc, char *argv[] );
void print_key( USHORT key );
void program_end( char *error );
void program_begin( void );
void window_input( void );

/***********
*  M A I N
************/

int main( int argc, char *argv[] )
{
  /* quit if not run from the CLI -- nowhere to send output */
  if (argc)
  {
    program_begin();
    window_input();
  }
  program_end( NULL );
  return 0;
}

/*****************
*  PROGRAM BEGIN
******************/

/*
This procedure opens the Intuition library, the window, and the console
device.  If any of these things fail, the program ends.
*/

void program_begin( void )
{
  if (!(IntuitionBase = (struct IntuitionBase *)
                        OpenLibrary( "intuition.library", 0L )))
    program_end( "intuition" );

  if (!(win = OpenWindow( &new_window )))
    program_end( "window" );

  if (!console_open())
    program_end( "console" );
}

/***************
*  PROGRAM END
****************/

/*
This procedure closes the console device, the window, and the Intuition
library.  It also prints an error message (if any) to the CLI.
*/

void program_end( char *error )
{
  if (error) printf( "FAILED: %s\n", error );

  console_close();

  if (win)           CloseWindow( win );
  if (IntuitionBase) CloseLibrary((struct Library*) IntuitionBase );
}

/****************
*  WINDOW INPUT
*****************/

/*
This procedure monitors for window input.  It returns when the user clicks on
the window close gadget.
*/

void window_input( void )
{
  BOOL finished;
  struct IntuiMessage *imessage;
  USHORT key;

  finished = FALSE;
  while (!finished)
  {
    /* wait for input from the window */
    Wait( 1L<<win->UserPort->mp_SigBit );

    /* for each input message */
    while ((imessage = (struct IntuiMessage *)GetMsg( win->UserPort )))
    {
      switch (imessage->Class)
      {
        case CLOSEWINDOW:
          finished = TRUE;                  /* end the program */
          break;
        case RAWKEY:
          if ((key = input_key( imessage )))  /* if a key was pressed */
            print_key( key );               /* print it */
          break;
      }
      /* reply to the message to free its memory */
      ReplyMsg( (struct Message *)imessage );
    }
  }
}

/*************
*  PRINT KEY
**************/

/*
This procedure prints an English description of the key that was pressed and
any qualifiers.
*/

char *command_keys[] =
{
  NULL, "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "TAB",
  "DELETE", "ESCAPE", "BACKSPACE", "HELP", "RETURN", "UP_ARROW",
  "DOWN_ARROW", "LEFT_ARROW", "RIGHT_ARROW",
};

void print_key( USHORT key )
{
  int c;

  /* check for qualifiers */
  if (key & QUAL_LSHIFT)
    printf( "LSHIFT " );
  if (key & QUAL_RSHIFT)
    printf( "RSHIFT " );
  if (key & QUAL_CONTROL)
    printf( "CONTROL " );
  if (key & QUAL_LALT)
    printf( "LALT " );
  if (key & QUAL_RALT)
    printf( "RALT " );
  if (key & QUAL_LAMIGA)
    printf( "LAMIGA " );
  if (key & QUAL_RAMIGA)
    printf( "RAMIGA " );

  /* which key was pressed? */
  c = KEY_VALUE( key );

  /* if this was a command key */
  if (KEY_COMMAND( key ))
    printf( "%s\n", command_keys[c] );
  /* else just normal ASCII character */
  else
    printf( "'%c'\n", c );
}

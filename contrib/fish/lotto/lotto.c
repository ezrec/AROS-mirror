/*********************************************************
*  FLORIDA LOTTO PROGRAM v1.0
*  © Copyright 1988 Timm Martin
*  All Rights Reserved
*
*  This program may be distributed freely as long as
*  this notice remains intact.  This program may not
*  be sold individually or as part of a group without
*  the express written consent of the author.
*
*  This program is for entertainment only and is sold
*  without any express or implied warranties whatsoever.
*  The user must assume the entire risk of using the
*  program (hey, you don't need my help to lose your
*  money).  Any liability of the author will be limited
*  to a refund of the purchase price (which is nothing).
*
*  Send all compliments, complaints, and monetary
*  expressions of gratitude to:
*
*    Timm Martin
*    11325 94th Street North
*    Largo, FL 34643
*
*  My only request is that if you win BIG using this
*  program, please mention to everyone that you used
*  this program to generate your winning numbers.
**********************************************************/

#define USE_SCREEN 1

#include <exec/types.h>
#include <graphics/gfxbase.h>
#include <graphics/text.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/screens.h>  /* Kickstart v1.2.1 or later */
#include <libraries/dos.h>
//#include <functions.h>          /* Manx C */

/********************
*  AMIGA STRUCTURES
*********************/

struct IntuitionBase *IntuitionBase = NULL;
struct GfxBase *GfxBase = NULL;
struct Screen *screen = NULL;
struct Window *window = NULL;

/************************
*  NEW SCREEN STRUCTURE
*************************/

#define COLORS 8L
#define PURPLE 0L
#define BLACK  1L
#define BLUE   2L
#define WHITE  3L
#define RED    6L
#define VIOLET 7L


/* AROS */
#define SHORT WORD
#define USHORT UWORD
#include <intuition/iobsolete.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#define LIBRARY_VERSION 0L

struct TextAttr screen_font = {
  (STRPTR)"topaz.font", 8, 0, 0
};

struct NewScreen new_screen = {
  0, 0, 320, 200, 3, PURPLE, PURPLE, 0, CUSTOMSCREEN,
  &screen_font, NULL, NULL, NULL
};

USHORT color_table[COLORS] = {
  0x748, 0x000, 0x00F, 0xFFF, 0x000, 0x0F0, 0xF00, 0x415
};

/*********************
*  GADGET STRUCTURES
**********************/

/*** TEXT ***/

struct IntuiText lotto_text = {
  WHITE, BLUE, JAM1, 6, 5, NULL, (STRPTR)"LOTTO", NULL
};
struct IntuiText quit_text = {
  WHITE, BLUE, JAM1, 10, 5, NULL, (STRPTR)"QUIT", NULL
};

/*** BORDERS ***/

SHORT gadget_pairs2[] = {
  4,21, 55,21, 55,4, 56,4, 56,22, 4,22, 4,23, 57,23, 57,4, 58,4, 58,24, 4,24
};
SHORT gadget_pairs1[] = {
  0,0, 0,20, 54,20, 54,0, 1,0, 1,19, 53,19, 53,1, 2,1
};

struct Border gadget_border2 = {
  -2, -2, VIOLET, PURPLE, JAM1, 12, gadget_pairs2, NULL
};
struct Border gadget_border1 = {
  -2, -2, BLACK, PURPLE, JAM1, 9, gadget_pairs1, &gadget_border2
};

#define LOTTO_GADGET 1
#define QUIT_GADGET  2

struct Gadget lotto_gadget = {
  NULL, 83, 106, 51, 17, GADGHCOMP, RELVERIFY, BOOLGADGET,
  (APTR)&gadget_border1, NULL, &lotto_text, 0, NULL, LOTTO_GADGET, NULL
};
struct Gadget quit_gadget = {
  &lotto_gadget, 186, 106, 51, 17, GADGHCOMP, RELVERIFY, BOOLGADGET,
  (APTR)&gadget_border1, NULL, &quit_text, 0, NULL, QUIT_GADGET, NULL
};

/************************
*  LOTTO BOX STRUCTURES
*************************/


SHORT box_pairs2[] = {
  4,23, 158,23, 158,4, 159,4, 159,24, 4,24,
  4,25, 160,25, 160,4, 161,4, 161,26, 4,26
};
SHORT box_pairs1[] = {
  0,0, 0,22, 157,22, 157,0, 1,0, 1,21, 156,21, 156,1, 2,1
};

struct Border box_border2 = {
  81, 74, VIOLET, PURPLE, JAM1, 12, box_pairs2, NULL
};
struct Border box_border1 = {
  81, 74, BLACK, PURPLE, JAM1, 9, box_pairs1, &box_border2
};

/************************
*  NEW WINDOW STRUCTURE
*************************/

#define LATER NULL
struct NewWindow new_window = {
  0, 0, 320, 200, PURPLE, PURPLE, GADGETUP,
  ACTIVATE | BORDERLESS | NOCAREREFRESH | SIMPLE_REFRESH, LATER,
  NULL, NULL, LATER, NULL, 320, 200, 320, 200,

#if USE_SCREEN  
  CUSTOMSCREEN
#else
  0
#endif
};

/********************
*  GLOBAL VARIABLES
*********************/

#define NUMBERS 6
#define UPPER  49L

int  numbers[NUMBERS];
long random_variable;

/*************
*  FUNCTIONS
**************/

void display_numbers();  /* displays the lotto numbers in the box */
void end_program();      /* closes the window, screen, and libraries */
void get_inputs();       /* receives and processes user input */
void get_numbers();      /* gets the lotto numbers */
void initialize();       /* draws the lotto box and renders the gadgets */
void open_all();         /* opens the libraries, screen, and window */
long random();           /* random number function */
long random_mult();      /* calculation used by random() function */
void randomize();        /* plants pseudo-random variable seed using clock */
void exit();

/**************************
*  M A I N  P R O G R A M
***************************/

int main(int argc, char **argv)
{
  open_all();
  initialize();
  get_inputs();
}

/*******************
*  DISPLAY NUMBERS
********************/

void display_numbers()
{
  char digits[2];
  int  i, j;
  long left, r;

  SetAPen( window->RPort, RED );
  RectFill( window->RPort, 83L, 76L, 236L, 94L );

  SetBPen( window->RPort, RED );
  for (i = 0; i < NUMBERS; i++) {
    left = 92 + i * 24;
    SetAPen( window->RPort, BLACK );
    for (j = 0; j < 150; j++) {
      r = random( UPPER ) + 1;
      digits[0] = r / 10 + '0';
      digits[1] = r % 10 + '0';
      Move( window->RPort, left, 88L );
      Text( window->RPort, digits, 2L );
    }
    SetAPen( window->RPort, WHITE );
    digits[0] = numbers[i] / 10 + '0';
    digits[1] = numbers[i] % 10 + '0';
    Move( window->RPort, left, 88L );
    Text( window->RPort, digits, 2L );
  }
}

/***************
*  END PROGRAM
****************/

void end_program( return_code )
  int return_code;
{
  if (window)        CloseWindow( window );
#if USE_SCREEN
  if (screen)        CloseScreen( screen );
#endif
  if (GfxBase)       CloseLibrary( (struct Library *)GfxBase );
  if (IntuitionBase) CloseLibrary( (struct Library *)IntuitionBase );

  exit( return_code );
}

/**************
*  GET INPUTS
***************/

#define FOREVER for(;;)
#define GADGET_ID ((struct Gadget *)(imessage->IAddress))->GadgetID
#define WAIT_FOR_INPUT Wait(1L<<window->UserPort->mp_SigBit)
#define WINDOW_INPUT (imessage=(struct IntuiMessage *)GetMsg(window->UserPort))

void get_inputs()
{
  struct IntuiMessage *imessage;
  BOOL terminated = FALSE;

  while (!terminated)
  {
    WAIT_FOR_INPUT;

    while (WINDOW_INPUT) {
      /* switch used here in case add more input methods later */
      switch (imessage->Class) {

        case GADGETUP:
          switch (GADGET_ID) {
            case LOTTO_GADGET: get_numbers(); break;
            case QUIT_GADGET: terminated = TRUE; break;
          } /* switch gadget */
          break;

      } /* switch input class */
      
      ReplyMsg((struct Message *)imessage);
    }   /* while window input */
    
 } /* while (!terminated) */
    
    end_program( 0 );
}

/***************
*  GET NUMBERS
****************/

void get_numbers()
{
  int  i, count, upper;
  int  list[UPPER];
  long r;

  for (i = 0; i < UPPER; i++)
    list[i] = i + 1;
  for (count = 0, upper = UPPER; count < NUMBERS; count++, upper--) {
    /* the random() function returns a long integer 0 <= r < upper */
    r = random( (long)upper );
    numbers[count] = list[r];
    for (i = r; i < upper; i++)
      list[i] = list[i+1];
  }
  display_numbers();
}

/**************
*  INITIALIZE
***************/

void initialize()
{
  SetAPen( window->RPort, RED );
  RectFill( window->RPort, 83L, 76L, 236L, 94L );
  DrawBorder( window->RPort, &box_border1, 0L, 0L );

  SetAPen( window->RPort, BLUE );
  RectFill( window->RPort, 83L, 106L, 133L, 122L );
  RectFill( window->RPort, 186L, 106L, 236L, 122L );
  AddGList( window, &quit_gadget, -1L, -1L, NULL );
  RefreshGList( &quit_gadget, window, NULL, -1L );
}

/************
*  OPEN ALL
*************/

void open_all()
{
  if (!(IntuitionBase = (struct IntuitionBase *)
        OpenLibrary( "intuition.library", LIBRARY_VERSION )))
    end_program( 1 );
  if (!(GfxBase = (struct GfxBase *)OpenLibrary( "graphics.library", 0L )))
    end_program( 2 );

#if USE_SCREEN
  if (!(screen = OpenScreen( &new_screen )))
    end_program( 3 );
  LoadRGB4( &screen->ViewPort, color_table, COLORS );

  new_window.Screen = screen;
#endif
  
  if (!(window = OpenWindow( &new_window )))
    end_program( 4 );

  randomize();
}

/**********
*  RANDOM
***********/

#define LARGE  100000000
#define SMALL  10000
#define MEDIUM 31415821

long random( upper )
  long upper;
{
  if (upper < 0)
    upper = -upper;  /* force positive */

  random_variable = (random_mult( random_variable, MEDIUM ) + 1) % LARGE;
  upper = ((random_variable / SMALL) * upper) / SMALL;

  return (upper);
}

/***************
*  RANDOM MULT
****************/

long random_mult( a, b )
  long a, b;
{
  long a0, a1, b0, b1, m;

  a0 = a % SMALL;
  a1 = a / SMALL;
  b0 = b % SMALL;
  b1 = b / SMALL;

  m = (((a0 * b1 + a1 * b0) % SMALL) * SMALL + a0 * b0) % LARGE;

  return (m);
}

/*************
*  RANDOMIZE
**************/

void randomize()
{
  struct DateStamp ds;

  DateStamp( &ds );
  random_variable = ds.ds_Tick;
}

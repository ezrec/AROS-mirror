/***********************************************************************
*
*                           Yachtc
*             Yes, Friends, it My First Amiga Program
*             Copyright 1985 by Sheldon Leemon
*             Feel free to copy and distribute the program and
*             source code, but don't try to sell, license, or
*             otherwise commercially exploit it.
*
*             If you have questions or comments, you may contact
*             the author through Delphi (username = DRX)
*             or Compuserve ID 72705,1355.  No late-night phone calls,
*             please. 
*
************************************************************************/

/* These dummy declarations of structs that are not used are
   here to keep the compiler from complaining.  Obviously, if your
   program uses the structs, don't include these dummy definitions--
   include the real ones */
/*
struct CopList { int dummy; };
struct UCopList { int dummy; };
struct cprlist { int dummy; };
struct Region { int dummy; };
struct VSprite { int dummy; };
struct collTable { int dummy; };
struct KeyMap { int dummy; };
struct Device { int dummy; };
struct Unit { int dummy; };
struct GfxBase { int dummy; };
*/

/* Include the definitions we need */

#include <aros/oldprograms.h>

#include <exec/types.h>
#include <intuition/intuition.h>
#include <exec/memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Structures needed for libraries */

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;

/* Structures required for graphics */

struct Screen *BdScr;
struct Window *BdWdw;
struct ViewPort *WVPort;

/* Prototypes */
void roll_dice(USHORT[]);
USHORT IMsg();
USHORT Shake(USHORT[]);
void score_turn(int[],USHORT[],USHORT);
USHORT Rollrep(USHORT);
void Deselect(USHORT);
unsigned int Evaluate(USHORT*,USHORT);
void ShowScore(USHORT,USHORT,USHORT);
void ShowDots(USHORT,USHORT);
void ClearRow(USHORT,USHORT);
void Name(USHORT);
void init_scr();
void init_pad();
int InitImages();
void FreeImages();
void Cleanup();

/***************** Program Constants ****************************/

#define BdRp   BdWdw->RPort

#define INTUITION_REV  30   /* Internal version number */
#define GRAPHICS_REV   30
#define BLANKW    56      /* Width of die blank */
#define BLANKH    23      /* Height of die blank */
#define BLANKL   530      /* Left edge of die blanks */
#define BLANKT    20      /* Top edge of die blanks */
#define BLANKS    31      /* Space between blanks */
#define DIEW      48      /* Width of each Die image */
#define DIEH      19      /* Height of each Die image */
#define DIEHOFF    4      /* Horizontal offset of die image from BlankL */
#define DIEVOFF    2      /* Vertical offset of die image from BlankT */
#define TEXTL      5      /* Left pixel of first text column */
#define TEXTT     20      /* Top pixel of first text row */
#define TEXTS      8      /* Space between each text line */
#define SPACEW     10      /* number of dots wide--space character */
#define SPACEH      9      /* number of dots high--space character */
#define DOTL     155      /* left position of first dot character */
#define DOTS      90      /* pixels between columns of dots=9*SPACEW */
#define DOTT      36      /* line of first dot text=TEXT+2*TEXTS */
#define HLINT     18      /* pixel of top horizontal line */
#define HLINL      1      /* Leftmost pixel of top horizontal line */
#define HLINR    489      /* Rightmost pixel of top horizontal line */
#define HLINS      8      /* Space between horizontal lines */
#define VLINT     13      /* Vertical line top pixel */
#define VLINB    199      /* Vertical line bottom pixel */
#define VLINL    128      /* Leftmost vertical line position */
#define VLINS     90      /* Space between vertical lines */
#define MAXLINES  23       /* Maximum number of screen lines */
#define DICE       5      /* number of dice on screen */
#define TURNS      5      /* the bones array member holding turns variable */
#define MAXPLAYERS   4      /* maximum number of players */
#define SCORECATS   21      /* number of scoring categories */
#define BONUS      62      /* number of points needed for upper bonus */
#define WHITE   0xFFF
#define RED      0xF00
#define GREEN   0x0F0
#define BLUE   0x00F
#define AQUA   0x0FF
#define PURPLE    0xF0F
#define YELLOW   0xFF0
#define BLACK   0x000


/***************** Image and other data structs ****************/

/* Image data for dice spots.  All 48 bits are used for the image.
   We move this data down to CHIP memory, just in case (later versions
   will lets us allocate structures in CHIP memory--so they say)*/

UBYTE SpotData [] [114] ={
 /* one spot */
 {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x07, 0xE0, 0x00, 0x00,
   0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00,
   0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00,
   0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00,
   0x00, 0x00, 0x07, 0xE0, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00
 },
 /* two spots*/
 {
   0x1F, 0x80, 0x00, 0x00, 0x00, 0x00,
   0x7F, 0xE0, 0x00, 0x00, 0x00, 0x00,
   0x7F, 0xE0, 0x00, 0x00, 0x00, 0x00,
   0x7F, 0xE0, 0x00, 0x00, 0x00, 0x00,
   0x1F, 0x80, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x01, 0xF8,
   0x00, 0x00, 0x00, 0x00, 0x07, 0xFE,
   0x00, 0x00, 0x00, 0x00, 0x07, 0xFE,
   0x00, 0x00, 0x00, 0x00, 0x07, 0xFE,
   0x00, 0x00, 0x00, 0x00, 0x01, 0xF8
 },
 /* three spots */
 {
   0x1F, 0x80, 0x00, 0x00, 0x00, 0x00,
   0x7F, 0xE0, 0x00, 0x00, 0x00, 0x00,
   0x7F, 0xE0, 0x00, 0x00, 0x00, 0x00,
   0x7F, 0xE0, 0x00, 0x00, 0x00, 0x00,
   0x1F, 0x80, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x07, 0xE0, 0x00, 0x00,
   0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00,
   0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00,
   0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00,
   0x00, 0x00, 0x07, 0xE0, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x01, 0xF8,
   0x00, 0x00, 0x00, 0x00, 0x07, 0xFE,
   0x00, 0x00, 0x00, 0x00, 0x07, 0xFE,
   0x00, 0x00, 0x00, 0x00, 0x07, 0xFE,
   0x00, 0x00, 0x00, 0x00, 0x01, 0xF8
 },
 /* four spots */
 {
   0x1F, 0x80, 0x00, 0x00, 0x01, 0xF8,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x1F, 0x80, 0x00, 0x00, 0x01, 0xF8,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x1F, 0x80, 0x00, 0x00, 0x01, 0xF8,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x1F, 0x80, 0x00, 0x00, 0x01, 0xF8
 },
 /* five spots */
 {
   0x1F, 0x80, 0x00, 0x00, 0x01, 0xF8,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x1F, 0x80, 0x00, 0x00, 0x01, 0xF8,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x07, 0xE0, 0x00, 0x00,
   0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00,
   0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00,
   0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00,
   0x00, 0x00, 0x07, 0xE0, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x1F, 0x80, 0x00, 0x00, 0x01, 0xF8,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x1F, 0x80, 0x00, 0x00, 0x01, 0xF8
 },
 /* six spots */
 {
   0x1F, 0x80, 0x00, 0x00, 0x01, 0xF8,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x1F, 0x80, 0x00, 0x00, 0x01, 0xF8,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x1F, 0x80, 0x00, 0x00, 0x01, 0xF8,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x1F, 0x80, 0x00, 0x00, 0x01, 0xF8,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x1F, 0x80, 0x00, 0x00, 0x01, 0xF8,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x7F, 0xE0, 0x00, 0x00, 0x07, 0xFE,
   0x1F, 0x80, 0x00, 0x00, 0x01, 0xF8
 }
};

UBYTE (*SpotData_chip)[114];
SHORT boardlines[] =
 {
   HLINR,VLINT,
   HLINR,VLINB,
   HLINL-1,VLINB,
   HLINL-1,VLINT,
   HLINL,VLINT,
   HLINL,VLINB,
   HLINR-1,VLINB,
   HLINR-1,VLINT
};               

   static USHORT colormap [8] =
      {
      WHITE,   /* background color */
#define BGRP     0
      RED,      /* color of window-close box */
#define REDP     1
      GREEN,   /* color of menu title */
#define GRNP     2
      YELLOW,   /* color of window-close dot */
#define YELP     3
      AQUA,   
#define AQUP     4
      PURPLE,   
#define PURP     5
      BLUE,
#define BLUP     6
      BLACK
#define BLKP     7
      };

   static char *textline [MAXLINES+4] = {
"Players",
" ",
"Aces       ",
"Twos       ",
"Threes     ",
"Fours      ",
"Fives      ",
"Sixes      ",
" ",
"Upper Total",
"Bonus      ",
" ",
"3 of a Kind",
"4 of a Kind",
"Full House ",
"Sm Straight",
"Lg Straight",
"Yacht      ",
"Yarboro    ",
" ",
"Lower Total",
" ",
"Grand Total",
"  One   ",
"  Two   ",
" Three  ",
"  Four  "};

/* ****************** Pre-initialized Intuition Structures************* */


struct TextAttr StdFont =
   {
   "topaz.font",              /* Font Name */
   TOPAZ_SIXTY,               /* Font Height */
   FS_NORMAL,                 /* Style */
   FPF_ROMFONT,               /* Preferences */
   };

/************* Images structures ***************/

struct Image Blank =
{
   0,0,         /* left, top */
   BLANKW,BLANKH,1,      /* width, height, depth */
   NULL,         /* image data */
   0x00,0x01,      /* PlanePick, PlaneOnOff */
   NULL,         /* NextImage */

};
/* An array of six image structs, holding the images of each die
   face, from 1-6 */

struct Image DieImage []={
{
   DIEHOFF,DIEVOFF,   /* left, top */
   DIEW,DIEH,1,      /* width, height, depth */
   NULL,            /* dummy image data--to be filled in after image data
                       is moved to chip memory */
   YELP-REDP,REDP,         /* PlanePick, PlaneOnOff */
   NULL            /* NextImage */

},


{DIEHOFF,DIEVOFF,DIEW,DIEH,1,NULL,YELP-REDP,REDP,NULL},   /*die images 1-5 */
{DIEHOFF,DIEVOFF,DIEW,DIEH,1,NULL,YELP-REDP,REDP,NULL},
{DIEHOFF,DIEVOFF,DIEW,DIEH,1,NULL,YELP-REDP,REDP,NULL},
{DIEHOFF,DIEVOFF,DIEW,DIEH,1,NULL,YELP-REDP,REDP,NULL},
{DIEHOFF,DIEVOFF,DIEW,DIEH,1,NULL,YELP-REDP,REDP,NULL},


};

/*******Here come da Gadgets ********/

struct Gadget DieGadg [6] ={
{
   NULL,                  /* Next Gadget */
   BLANKL,BLANKT,            /* Left Edge, Top Edge */   
   BLANKW,BLANKH,            /* Width, Height */
   GADGHIMAGE + GADGIMAGE,      /*Flags--highlight type, render type */
   NULL,                  /* Activation flags */
   BOOLGADGET,               /* Gadget type */
   (APTR)&DieImage[5],         /* Image structure for drawing gadget */
   (APTR)&Blank,            /* Alternate image for highlighting */
   NULL,                  /* Intuitext structure for gadget text */
   NULL,                  /* Mutual exclude, non-functional */
   NULL,                  /* Special info for proportional, string */
   NULL,         

         /* Gadget ID */
   NULL,                  /* User Data */
},

{&DieGadg[0],BLANKL,BLANKS+BLANKT,BLANKW,BLANKH,   /* DieGadg 1-4 */
GADGHIMAGE + GADGIMAGE,NULL,BOOLGADGET,
(APTR)&DieImage[5],(APTR)&Blank,NULL,NULL,NULL,NULL,NULL},

{&DieGadg[1],BLANKL,(2*BLANKS)+BLANKT,BLANKW,BLANKH,
GADGHIMAGE + GADGIMAGE,NULL,BOOLGADGET,
(APTR)&DieImage[5],(APTR)&Blank,NULL,NULL,NULL,NULL,NULL},

{&DieGadg[2],BLANKL,(3*BLANKS)+BLANKT,BLANKW,BLANKH,
GADGHIMAGE + GADGIMAGE,NULL,BOOLGADGET,
(APTR)&DieImage[5],(APTR)&Blank,NULL,NULL,NULL,NULL,NULL},

{&DieGadg[3],BLANKL,(4*BLANKS)+BLANKT,BLANKW,BLANKH,
GADGHIMAGE + GADGIMAGE,NULL,BOOLGADGET,
(APTR)&DieImage[5],(APTR)&Blank,NULL,NULL,NULL,NULL,NULL},

};

struct Gadget ButtonGadget =
{
   &DieGadg[4],                  /* Next Gadget */
   BLANKL,(5*BLANKS)+BLANKT,   /* Left Edge, Top Edge */   
   BLANKW,BLANKH,            /* Width, Height */
   GADGHCOMP + GADGIMAGE+ GADGDISABLED,       /*Flags--highlight type, render type */
   RELVERIFY,                /* Activation flags */
   BOOLGADGET,               /* Gadget type */
   (APTR)&Blank,            /* Image structure for drawing gadget */
   NULL,                  /* Alternate image for highlighting */
   NULL,                  /* Intuitext structure for gadget text */
   NULL,                  /* Mutual exclude, non-functional */
   NULL,                  /* Special info for proportional, string */
   NULL,                  /* Gadget ID */
   NULL,                  /* User Data */
};

struct Gadget ScoreGadget ={
   &ButtonGadget,            /* Next Gadget */
   VLINL+4,HLINT+(2*HLINS)-5,   /* Left Edge, Top Edge */   
   VLINS-6,(17*HLINS),         /* Width, Height */
   GADGHNONE,               /*Flags--highlight type, render type */
   RELVERIFY,               /* Activation flags */
   BOOLGADGET,               /* Gadget type */
   NULL,                  /* Image structure for drawing gadget */
   NULL,                  /* Alternate image for highlighting */
   NULL,                  /* Intuitext structure for gadget text */
   NULL,                  /* Mutual exclude, non-functional */
   NULL,                  /* Special info for proportional, string */
   NULL,                  /* Gadget ID */
   NULL                  /* User Data */
};
/************* finally, the Menus and their ITexts *************/

struct IntuiText AboutText [10] = {

   {BLUP,BGRP,JAM2,30,9,&StdFont,   "                                           ",NULL},
   {BLUP,BGRP,JAM2,30,18,&StdFont,
   "                   YachtC                  ",&AboutText[0]},

   {BLUP,BGRP,JAM2,30,27,&StdFont,
   "      Copyright 1985 by Sheldon Leemon     ",&AboutText[1]},

   {BLUP,BGRP,JAM2,30,36,&StdFont,
   "                                           ",&AboutText[2]},

   {BLUP,BGRP,JAM2,30,45,&StdFont,
   " You may copy and distribute this program  ",&AboutText[3]},

   {BLUP,BGRP,JAM2,30,54,&StdFont,
   " freely (i.e. for no money), but any kind  ",&AboutText[4]},

   {BLUP,BGRP,JAM2,30,63,&StdFont,
   " of commercial exploitation is a no-no.    ",&AboutText[5]},

   {BLUP,BGRP,JAM2,30,72,&StdFont,
   " Comments may be sent to the author via    ",&AboutText[6]},

   {BLUP,BGRP,JAM2,30,81,&StdFont,
   " Delphi (DRX) or CompuServe (72705,1355).  ",&AboutText[7]},

   {BLUP,BGRP,JAM2,30,90,&StdFont,   
   "                                           ",&AboutText[8]},

};

struct IntuiText InstructText [14] = {

   {BLUP,BGRP,JAM2,30,9,&StdFont,
   "                                           ",NULL},

   {BLUP,BGRP,JAM2,30,18,&StdFont,
   "                Instructions               ",&InstructText[0]},

   {BLUP,BGRP,JAM2,30,27,&StdFont,
   "                                           ",&InstructText[1]},

   {BLUP,BGRP,JAM2,30,36,&StdFont,
   " To start game, select 1-4 Player game from",&InstructText[2]},

   {BLUP,BGRP,JAM2,30,45,&StdFont,
   " the Project menu.  Each player gets up to ",&InstructText[3]},

   {BLUP,BGRP,JAM2,30,54,&StdFont,
   " 3 rolls of the dice to make the best hand ",&InstructText[4]},

   {BLUP,BGRP,JAM2,30,63,&StdFont,
   " possible.  Click on the dice you wish to  ",&InstructText[5]},

   {BLUP,BGRP,JAM2,30,72,&StdFont,
   " change, then click on the button below the",&InstructText[6]},

   {BLUP,BGRP,JAM2,30,81,&StdFont,
   " dice to roll.  If you are satisfied with  ",&InstructText[7]},

   {BLUP,BGRP,JAM2,30,90,&StdFont,
   " your hand, and wish to score, click on the",&InstructText[8]},

   {BLUP,BGRP,JAM2,30,99,&StdFont,
   " button without blanking any dice.  All    ",&InstructText[9]},

   {BLUP,BGRP,JAM2,30,108,&StdFont,
   " possible scores will appear in blue on    ",&InstructText[10]},

   {BLUP,BGRP,JAM2,30,117,&StdFont,
   " the scorepad.  Click on the one you want. ",&InstructText[11]},

   {BLUP,BGRP,JAM2,30,126,&StdFont,
   "                                           ",&InstructText[12]},

};

struct IntuiText OKText = 
   {BLUP,BGRP,JAM2,6,3,&StdFont," Proceed ",NULL};


struct IntuiText Menu0IText [] = {

   {
   BLUP,BGRP,JAM2,      /* Front Pen, Back Pen, Draw Mode */ 
   0,0,            /* Left Edge, Top Edge */
   &StdFont,         /* pointer to Text Font */
   " Start 1-Player Game ",   /* text of Menu Item */
   NULL            /* pointer to next IText */
   },

   {BLUP,BGRP,JAM2,0,0,&StdFont," Start 2-Player Game ",NULL },
   {BLUP,BGRP,JAM2,0,0,&StdFont," Start 3-Player Game ",NULL },
   {BLUP,BGRP,JAM2,0,0,&StdFont," Start 4-Player Game ",NULL },
};

struct IntuiText Menu1IText [] = {
   {BLUP,BGRP,JAM2,0,0,&StdFont," Instructions ",NULL },
   {BLUP,BGRP,JAM2,0,0,&StdFont," About YachtC ",NULL },
};

struct MenuItem Menu0Item[4] = {
   {
   &Menu0Item[1],         /* pointer to next Item */
   0,0,210,9,      /* Left, Top, Width, Height */
   ITEMTEXT | ITEMENABLED | HIGHCOMP,   /* Flags */
   0,            /* no mutual exclude */
   (APTR)&Menu0IText[0],   /* Render info */   
   NULL,NULL,NULL,         /*Alt Image, Command (amiga) char, subitem*/
   MENUNULL      /* next select */
   },

   {&Menu0Item[2],0,9,210,9,ITEMTEXT | ITEMENABLED | HIGHCOMP,
   0,(APTR)&Menu0IText[1],NULL,NULL,NULL,MENUNULL},

   {&Menu0Item[3],0,18,210,9,ITEMTEXT | ITEMENABLED | HIGHCOMP,
   0,(APTR)&Menu0IText[2],NULL,NULL,NULL,MENUNULL},

   {NULL,0,27,210,9,ITEMTEXT | ITEMENABLED | HIGHCOMP,
   0,(APTR)&Menu0IText[3],NULL,NULL,NULL,MENUNULL}
};

struct MenuItem Menu1Item[2] = {
   
   {&Menu1Item[1],0,0,140,9,ITEMTEXT | ITEMENABLED | HIGHCOMP,
   0,(APTR)&Menu1IText[0],NULL,NULL,NULL,MENUNULL},

   {NULL,0,9,140,9,ITEMTEXT | ITEMENABLED | HIGHCOMP,
   0,(APTR)&Menu1IText[1],NULL,NULL,NULL,MENUNULL}

};


struct Menu BdMenu [2] = {
   {
   &BdMenu[1],      /* ptr to next Menu */   
   40,0,90,0,   /* left,top,width,height--top and height ignored */
   MENUENABLED,   /* Flags */
   " Project ",   /* menu title */
   &Menu0Item[0]   /* First Item in list */
   },

{NULL,160,0,130,0,MENUENABLED," Information ",&Menu1Item[0]}

};
   
/* ********Pre-initialized NewScreen and NewWindow Structures*********** */

   struct NewScreen NewBdScr =
      {
      0,0,               /* LeftEdge (always=0),TopEdge */
      640,200,3,       

  /* Width, Height, Depth */
      PURP,GRNP,         /* DetailPen and BlockPen */
      HIRES,            /* special display modes */
      CUSTOMSCREEN,         /* Screen Type */
      &StdFont,            /* Pointer to Custom font structure*/
      NULL,               /* Pointer to title text */
      NULL,               /* Pointer to Screen Gadgets */
      NULL,               /* Pointer to CustomBitMap */
      };


struct NewWindow NewBoardWindow =
   {
   0,0,         /* Left Edge, Top Edge */
   640,200,      /* Width, Height */
   BLUP,YELP,      /* Block Pen, Detail Pen */
   GADGETUP + CLOSEWINDOW + MENUPICK,         /* IDCMP Flags */
   SMART_REFRESH + ACTIVATE +WINDOWDEPTH
   + BORDERLESS + WINDOWCLOSE,               /* Flags */
   &ScoreGadget,  /* Pointer to First Gadget */
   NULL,          /* Pointer to Check Mark Image */
   "                        YachtC",         /* Title */
   NULL,          /* Pointer to Screen structure */
   NULL,         /* Pointer to custom Bit Map */
   0,0,         /* Minimum Width, Height */
   0,0,         /* Maximum Width, Height */
   CUSTOMSCREEN   /* Type of Screen it resides on */
   };

/* *************************Program Begins Here******************* */

void main()
{
   USHORT bones [DICE];         /* The array for dice values */
   USHORT cats;
   int scores [MAXPLAYERS] [SCORECATS];
   /* USHORT IMsg(), flag; */
   USHORT flag;
   USHORT players, cur_player, turns;



init_scr();          /* do one-time initialization of screen */

players = 1;
while(players){


/* initialize the scores */

for(players=cur_player=0;players<MAXPLAYERS;players++){
   for(cats=0; cats<SCORECATS; scores[players][cats++] = -1)
      ;   /* blank the scoring columns to -1 to start with */

   for(turns=6; turns<10;

   scores[players][turns+11] = 0, scores[players][turns++] = 0)
      ;   /* except for the rows with graphics or totals (they = 0) */
   }

/* get number of players */

while( (flag=IMsg()) < 64);   /* wait til new game menu item selected */
players = flag - 63;      /* 1-4 player game selected */

init_pad();      /* draw the score pad */

SetDrMd (BdRp,JAM1);
for (turns=0;turns<players;turns++)
   Name(turns);

/* play a whole round */

for (turns=0;turns<13;turns++)
   for (cur_player=0; cur_player<players ;cur_player++)
      {



      SetDrMd(BdRp,JAM2+INVERSVID);   /*   highlight the player's name */
      Name(cur_player);

      roll_dice(bones);   /* get dice values */
      score_turn(scores[cur_player],bones,cur_player);

      SetDrMd(BdRp,JAM2);         /* unhighlight the player's name */
      Name(cur_player);

      }   /* for */
}   /* while */
}   /* end of main */

void roll_dice(bones)
USHORT bones [];

{
   USHORT changed,dieno,flag;
   /* USHORT Shake(),Rollrep(),IMsg(); */

for(dieno=0;dieno<DICE;dieno++)      /* do first roll */

   {
      DieGadg[dieno].Activation = TOGGLESELECT;
      bones[dieno]=Rollrep(dieno);  /* roll dice 5 times and store result */
   } /* for */

OnGadget(&ButtonGadget,BdWdw,NULL);
changed = bones[TURNS] = 1;

while(changed && bones[TURNS]<3)
  

 {
   while( ((flag=IMsg()) < 17) || (flag >24) );   /* wait til button pushed */
   changed = Shake(bones);            /* shake if any are changed */
   }

OffGadget(&ButtonGadget,BdWdw,NULL);
for(dieno = 0; dieno < DICE ; dieno++)      /* turn gadgets off */
   DieGadg[dieno].Activation = NULL;

}/* end of roll_dice */


void score_turn(scores,bones,cur_player)

int scores[];
USHORT bones[];
USHORT cur_player;

{
   USHORT IMsg();
   /* unsigned int Evaluate(),score; */
   unsigned int score;
   USHORT row;

   /* position score gadget */
 

     ScoreGadget.LeftEdge = VLINL + (VLINS*cur_player) + 4;
   /*   AddGadget(BdWdw,&ScoreGadget,-1); */

/* show possible scores to let player select */



for (row = 0; row <17; row ++)
   if (scores[row] == -1)
   {
      SetAPen (BdRp,BGRP);   /* erase dots with bg pen */
      ShowDots (row,cur_player);
      SetAPen (BdRp,BLUP);   /* possible scores in green */
      score = Evaluate(bones,row);
      ShowScore (score,row,cur_player);
   }


   /* wait  until user clicks on a score of an unused category */
   while( ( (row=IMsg()) > 17) || (scores[row] != -1 ) );


SetAPen (BdRp,BLKP);   /* selected scores in black */
score = Evaluate(bones,row);   /* evaluate row selected */
ShowScore (scores[row]=score,row,cur_player);   /* print in black */


if (row<7)
   {
   ClearRow(7,cur_player);
   ShowScore (scores[7]+=score,7,cur_player);   /* do upper sub-total */
   }
else
   {
   ClearRow(18,cur_player);
   ShowScore (scores[18]+=score,18,cur_player);   /* or else lower total */
   }
if (scores[7] > BONUS) 
   {
   ClearRow(8,cur_player);
   ShowScore (scores[8] = 35, 8, cur_player);   /* check for bonus */
   }

/*add sub-totals to total and display  */

ClearRow(20,cur_player);
ShowScore (scores[20]=scores[7]+scores[8]+scores[18],20,cur_player);
   


for (row = 0; row <17; row ++)
   if (scores[row] == -1)
   {
      SetAPen (BdRp,BGRP);   /* erase scores with bg pen */
      score = Evaluate(bones,row);
      ShowScore (score,row,cur_player);
      SetAPen (BdRp,BLKP);   /* draw dots in black pen */


      ShowDots (row,cur_player);
   }

}


USHORT IMsg()

{
   struct IntuiMessage *BdMsg;      /* Intuition message structure */
   ULONG Mclass;               /* Message class */
   USHORT Mcode,flag;            /* Message code */
   APTR Maddress;               /* Address of structure that caused message */
   SHORT Mmx, Mmy ;            /* Message mouse x and y */


flag = 35;
Wait (1 << BdWdw->UserPort->mp_SigBit);
while ((BdMsg = (struct IntuiMessage *)GetMsg(BdWdw->UserPort)))
   {
   Mclass = BdMsg->Class;
   Mcode = BdMsg->Code;
   Maddress = BdMsg->IAddress;
   Mmx = BdMsg->MouseX;
   Mmy = BdMsg->MouseY;
   ReplyMsg((struct Message *)BdMsg);
   switch (Mclass) {
      case CLOSEWINDOW:
         Cleanup();
         exit(TRUE);
         break;

      case GADGETUP:


         flag = (Mmy-29)/8;
         break;

      case MENUPICK:
         switch (MENUNUM(Mcode)){
            case 0:
               flag = 64 + ITEMNUM(Mcode);
               break;
            case 1:
               switch (ITEMNUM(Mcode)){
                  case 0:
                     AutoRequest
                     (BdWdw,&InstructText[13],NULL,&OKText,0,0,515,180);
                     flag = 32;
                     break;
                  case 1:
                     AutoRequest
                     (BdWdw,&AboutText[9],NULL,&OKText,0,0,515,140);
                     flag = 32;
                     break;
                  }   /* end of Item switch */
                  break;
            }   /* end of MenuNum switch */
            break;
   } 

  /* end of Class switch */      
            
   }/* while */

return(flag);
}

USHORT Shake(bones)
USHORT bones[];
{
   /* USHORT dieno,changed,Rollrep(); */
   USHORT dieno,changed;
   for(dieno=changed=0;dieno<DICE;dieno++)
   {
      OnGadget(&DieGadg[dieno],BdWdw,NULL);
      if(DieGadg[dieno].Flags & SELECTED)
      {
         bones[dieno]=Rollrep(dieno);  /* roll dice and store result */
         Deselect(dieno);
         changed++;

      } /* if */
   } /* for */
   bones[TURNS]++;
return(changed);
}

USHORT Rollrep(dieno)
USHORT dieno;
{
USHORT reps, throw;

for (reps=0;reps<9;reps++)
{
   throw = rand()%6;
   DrawImage(BdRp,&Blank,BLANKL, (BLANKS*dieno)+BLANKT);
   DrawImage(BdRp,&DieImage[throw],BLANKL, (BLANKS*dieno)+BLANKT);
}
DieGadg[dieno].GadgetRender=(APTR)&DieImage[throw];
return(throw);
}

void Deselect (dieno)
USHORT dieno;
{
USHORT Gadgetno ;

/* Since its not nice to change the SELECTED flag while the gadget is active */
/* (only the user is supposed to do that, by clicking on it), we'l be nice */
/* and first Remove the die gadget from the list, THEN change the flag */
/* and finally, Add it back to the list again. */

Gadgetno = RemoveGadget(BdWdw, &DieGadg[dieno]);
DieGadg[dieno].Flags ^= SELECTED;               /* toggle SELECTED flag */
AddGadget(BdWdw,&DieGadg[dieno],Gadgetno);
}


unsigned int Evaluate (bones,row)
USHORT *bones;
USHORT row;
{
unsigned int values[7];
unsigned int count, flag;

/* This routine evaluates the current dice values in the bones array */
/* in terms of the score they produce for the scoring category passed */
/* in row.  It returns the score. */

for(count=0;count<7;count++)
   values[count]=0;         /* zero out temporarily sort array */

for(count=0;count<5;count++){   /* sort dice by number of spots */
   values[bones[count]]++;
 

  values[6]+=(1+bones[count]);   /* add die to total */
   }
switch(row)
      {
/* spot dice */

      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:         return((row+1)*values[row]);   /* return total of desired spots */
   
/* 3 of a kind */

      case 10:
         for(count=flag=0;count<6;count++)
            if(values[count] > 2)
               flag = 1;
         return( (flag) ? values[6] : 0);

/* 4 of a kind */

      case 11:
         for(count=flag=0;count<6;count++)
            if(values[count] > 3)
               flag = 1;
         return( (flag) ? values[6] : 0);

/* full house */
      case 12:
         for(count=flag=0;count<6;count++)
            if(values[count] > 1)
               flag+=values[count];


         return((unsigned int) ((flag == 5) ? 25 : 0));

/* small straight */

      case 13:
         for(count=0; values[count] < 1 ; count++)
            ;   /* skip zeros */

         if ( (!count) && (!values[1]) )
            count = 2;      /* skip to 3 if there are 1's but no 2's */
         for( flag=count+4; count < flag ; count++)
            if (values[count] <1)
               return(0);

         return(30);    /* if four in a row are 1 or more, you got it */

/* large straight */

      case 14:
         for(count=0; values[count] < 1 ; count++)
            ;      /* skip zeros */

         for( flag=count+5; count < flag ; count++)
            if (values[count] !=1)
               return(0);

         return(40);      /* if five in a row, score */

/* yacht */

      case 15:
         for(count=flag=0;count<6;count++)
            if(values[count] == 5)
               flag = 1;
         return( (unsigned int) ((flag) ? 50 : 0));

/* yarboro */

      case 16:
         return(values[6]);

/* better not get here! */

      default:
         return(0);   
   }
   
}


void ShowScore(score,row,player)
USHORT score,row,player;
{
int length;
char *score_str = "0000";

   SetDrMd(BdRp,JAM2);
   Move (BdRp,DOTL + (DOTS*player), (row*TEXTS)+DOTT );
   Text (BdRp, "    ",4);
#ifdef LATTICE
   length = stcu_d (score_str,score,4);
#else
   /* score = strtol (score_str,NULL,10); */
   /* length=log10(score); */
   sprintf (score_str, "%4d", score);
   length = strlen (score_str);
#endif
   Move (BdRp,
      DOTL + (DOTS*player + ((4-length)*SPACEW) ), (row*TEXTS)+DOTT );
   Text (BdRp, score_str, length);

}


void ShowDots (row,player)
USHORT row,player;
{
   Move (BdRp,DOTL + (DOTS*player), (row*TEXTS)+DOTT );
   SetDrMd(BdRp,JAM2);
   Text (BdRp, "....",4);

}

void ClearRow (row,player)
USHORT row,player;
{

   Move (BdRp,DOTL + (DOTS*player), (row*TEXTS)+DOTT );
   SetDrMd(BdRp,JAM2);
   Text (BdRp, "    ",4);

}

void Name (player)
USHORT player;
{

   Move(BdRp, DOTL -(2*SPACEW) + (DOTS*player), TEXTT+2);
   Text(BdRp, textline[MAXLINES+player], 8);

}



void init_scr()
{
 
/* move image data to chip memory*/

if (InitImages() != TRUE) {
   printf("Not enough chip memory for images.\n");
 

  FreeImages();
   Exit(FALSE);
   }

/* Open the Intuition and Graphics libraries.
 * Get pointer to WCS routines, and if = 0, libraries aren't available.
 */
   IntuitionBase = (struct IntuitionBase *)
         OpenLibrary("intuition.library",INTUITION_REV);
   if (IntuitionBase == NULL) exit(FALSE);

   GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", GRAPHICS_REV);
   if (GfxBase == NULL) exit(FALSE);

/* Open the Screen and Windows.  If they = 0, they weren't opened. */

   if ((NewBoardWindow.Screen = BdScr =
 (struct Screen *)OpenScreen(&NewBdScr)) == NULL)
      exit(FALSE);

   if (( BdWdw = (struct Window *)OpenWindow(&NewBoardWindow)) == NULL)
      exit(FALSE);

SetMenuStrip(BdWdw,&BdMenu);

init_pad();      /* draw the score pad */



}   /* end of init_scr */

void init_pad()
{
/* Set up the board outline */

   ULONG Seconds,Micros;
   int count, column;

   WVPort = (struct ViewPort *)ViewPortAddress(BdWdw); /* find the viewport */
   LoadRGB4(WVPort,&colormap,8);   /* load our new set of colors */

   SetAPen (BdRp,BLKP);
   Move(BdRp,0,VLINT);
   SetDrMd (BdRp,JAM2);      /* JAM2 substituted */

   PolyDraw(BdRp,8,&boardlines);

/* Put in text and horizontal lines for board */

   Move(BdRp,TEXTL,TEXTT+2);
   Text(BdRp,textline[0],strlen(textline[0]) );


   for(count = 1; count<MAXLINES;count++) {
      if(*textline[count] == ' '){
         Move(BdRp,1,(count*HLINS)+HLINT);
         Draw(BdRp,HLINR,(count*HLINS)+HLINT);
         }

      else{
         Move(BdRp,TEXTL,(count*TEXTS)+TEXTT);
         Text(BdRp,textline[count],strlen(textline[count]) );

         for (column = 0; column < MAXPLAYERS; column ++)
            {
            ClearRow(count-2,column);
            ShowDots(count-2,column);
            }   /* for columns */
  

       }   /* else */

      };   /* for count */


/* Draw vertical lines for board */

   for(count = 0; count<MAXPLAYERS;count++) {   /* replace this with RectFill */
      Move(BdRp,(count*VLINS)+VLINL+1,VLINT);
      Draw(BdRp,(count*VLINS)+VLINL+1,VLINB);
      Move(BdRp,(count*VLINS)+VLINL,VLINT);
      Draw(BdRp,(count*VLINS)+VLINL,VLINB);
      }

/* Blank players names */ 
   SetDrMd(BdRp,JAM2);
   for (count=0;count<MAXPLAYERS;count++)
      {
      Move(BdRp, DOTL - (2*SPACEW) + (DOTS*count), TEXTT+2);
      Text(BdRp, "        ", 8);
      }

/* Draw initial die images */
   for(count = 0; count <5 ;count++){
   DrawImage (BdRp, &Blank, BLANKL, (BLANKS*count) + BLANKT);
   DrawImage (BdRp, &DieImage[5], BLANKL, (BLANKS*count) + BLANKT);
   }



/* use time to seed random number generator */
CurrentTime(&Seconds,&Micros);
srand(Micros);

}   /* end of init_pad */

int InitImages()
{
   extern UBYTE (*SpotData_chip)[114];
   int row,col;

   if ((SpotData_chip = (UBYTE (*)[114]) AllocMem(sizeof(SpotData),MEMF_CHIP))   == 0) return(FALSE);   /* allocate chip memory for images */

   row = 0;
   while (row < 6)
   {
      for (col=0 ; col < 114 ;col++){
            SpotData_chip[row][col] = SpotData[row][col];
            }
      DieImage[row].ImageData = SpotData_chip[row++];
   }   


   return(TRUE);

}

void FreeImages()
{
extern UBYTE (*SpotData_chip)[114];

   if (SpotData_chip != 0){
      FreeMem(SpotData_chip, sizeof(SpotData));
      }

}

void Cleanup()
{
   FreeImages();
   CloseWindow(BdWdw);
   CloseScreen(BdScr);
}

/* *********************** program ends here ****************** */

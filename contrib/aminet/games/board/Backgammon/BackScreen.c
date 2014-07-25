/************************************************************************/
/*  Hoser BackGammon version 1.0					*/
/*	Robert Pfister							*/
/*	Rfd#3 Box 2340		home:(207)-873-3520			*/
/*	Waterville, Maine 04901						*/
/*									*/
/*	Pfister_rob%dneast@dec.decwrl					*/
/*									*/
/*  Copyright  June,1987 all rights reserved.				*/
/*									*/
/*  This program will play a game of backgammon at the novice level	*/
/*									*/
/*  The code is in 4 parts...						*/
/*	 1) back.c     - main driver					*/
/*   /   2) eval.c     - evaluation of moves				*/
/* \/	 3) backscn.c  - screen stuff..					*/
/*	 4) backmenu.c - menu stuff, help text, and ``decoder''		*/
/*									*/
/* this was compiled under SASC 6.56					*/
/*									*/
/* This file contains:							*/
/*	gen_points	PutIntuiText	ITextLen	Gsetup		*/
/*	ShowDice	PutMoveNumber	PutPiece	decode		*/
/*	DoMenuStrip	UndoMenuStrip	requestor	finit		*/
/*	PutBarPiece	PutSpike	BlinkPiece	TextScreen	*/
/************************************************************************/
/* Most recent modification:	 3/ 8/93				*/
/* Most recent modification:	11/28/94				*/
/* Most recent modification:	 2/24/95				*/
/* Most recent modification:	 5/15/96				*/
/* Modifications & CleanUp:      5/16/96				*/


#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/diskfont.h>
#include <clib/alib_protos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "back.h"


#define max_vector	16		/* for AreaMove/Draw			*/
#define piece_width	32
#define sign(x)		((x) < 0 ? -1 : +1)
/* lbischoff: Added min(x, y) macro */
#define min(x, y)       ((x) < (y) ? x : y)

/* lbischoff: Added basepointers */
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct Library *DiskfontBase;

typedef  struct {
		SHORT	x,y;
	} coord;

/* order of points in box[] is upper left, lower right or	*/
/* left, top, right, bottom as individual values		*/
/* Spike entry is coordinates of spike point			*/
typedef  struct {
		coord	box[2];
		coord	spike;
	} point_rec;

typedef  struct {
		short	count,
			height;
		coord	point[13];
	} shape_rec;

static const shape_rec
  laced_piece =
    {12, 23,
	{{ 13,  2}, { 11,  6}, {  4,10}, { -4,10},
	 {-11,  6}, {-13,  2}, {-13,-2}, {-11,-6},
	 { -4,-10}, {  4,-10}, { 11,-6}, { 13,-2}}},
  norm_piece =
    {12, 11,
	{{ 13, 1}, { 11, 3}, {  3, 5}, { -3, 5},
	 {-11, 3}, {-13, 1}, {-13,-1}, {-11,-3},
	 { -3,-5}, {  3,-5}, { 11,-3}, { 13,-1}}};

static point_rec	point[26];
int			minY;			/* has to be global		*/
static int		xc, yc,			/* center of board		*/
			minX, maxX,		/* horizontal limits		*/
			/*minY,*/ maxY,		/* vertical limits		*/
			h_pitch,		/* width of spike		*/
			height,			/* spike height			*/
			move_num_left,		/* where move number starts	*/
			move_num_len,		/* how long it plots		*/
			bar_left, bar_right;	/* limits of bar		*/

static const shape_rec	*piece_ptr = &norm_piece;

UWORD			color_table[] = {0x000, 0xDDD, 0xAAA, 0xCCC, 0x444, 0x730, 0xFFD, 0x0AC};
struct ColorMap		color_map     = {0, 0, sizeof (color_table) / 2, (APTR)&color_table};
char			MyTitle []    = "BackGammon V2.3 (5/96)     Copyright June 1987     Robert Pfister";

struct RastPort		*rp;
static struct ViewPort	*vp;
extern struct Menu	MyMenu [];
struct Screen		*screen = NULL;
struct Window		*w      = NULL;
struct TextAttr		font[]  = {{"courier.font",11,0,0}, {"courier.font",13,0,0}};
struct TextFont		*tf     = NULL;

/* lbischoff: AROS does not handle Width/Height = -1 yet */
/* struct NewScreen	ns = {0, 0, -1, -1, 3, 0, 1, HIRES, CUSTOMSCREEN, font, MyTitle, NULL, NULL}; */
struct NewScreen	ns = {0, 0, 640, 480, 3, 0, 1, HIRES, CUSTOMSCREEN, font, MyTitle, NULL, NULL};
struct NewWindow	nw = {0, 10, -1, 190, 0, 1, 
				CLOSEWINDOW | MOUSEBUTTONS | MENUPICK,
				SMART_REFRESH | ACTIVATE | WINDOWDRAG | WINDOWDEPTH, NULL,NULL,
				"Backgammon anyone ?", NULL, NULL, 
				0, 0, 640, 200, CUSTOMSCREEN };

struct NewWindow	crnw = {20, 0, 405, 0, 0, 1,	/* top and height set when used */
				MOUSEBUTTONS, SMART_REFRESH | ACTIVATE | WINDOWDRAG,
				NULL, NULL, "Backgammon credits", NULL, NULL, 
				0, 0, 640, 200, CUSTOMSCREEN };

static short		AreaBuf [max_vector * 5 / sizeof (short)];
static APTR		TBuf;
static struct TmpRas	TRas;
static struct AreaInfo	AInfo;
BOOL			laced;
static int 		ErrSet = FALSE;
extern you_me_rec	you, me;
extern USHORT		total_games;


/*----------------------------------------------------------------------*/
void 	gen_points (struct Window *wp)
/*----------------------------------------------------------------------*/
/* adjust board for screen/window size					*/
{
  int		bar_width,
		d, n;
  point_rec	*prp;


  minX = 3;
  maxX = wp->Width - 3;
  bar_width = 8;
  h_pitch = (maxX - minX - bar_width) / 12;
  bar_width = maxX - minX - 12 * h_pitch;

  if (bar_width > 10) {
    minX += (bar_width - 10) / 2;
    maxX -= (bar_width - 10) / 2;
    bar_width = maxX - minX - 12 * h_pitch;
  }

  xc = (minX + maxX) / 2;
  bar_left = xc - bar_width / 2;
  bar_right = bar_left + bar_width;

  minY = screen->Font->ta_YSize * 3 + 1;
  maxY = wp->Height - 4;
  height = ((maxY - minY) * 3 / 7);
  yc = (minY + maxY) / 2;

  d = 0;
  prp = point + 1;

  for (n=0 ; n<12 ; n++) {
    prp->box [1].x = (prp->box [0].x = minX + n * h_pitch + d) + h_pitch - 1;
    prp->spike.y   =
    prp->box [0].y = (prp->box [1].y = maxY) - height;
    prp->spike.x   =  prp->box [0].x + h_pitch / 2;
    if (n == 5)
      d = bar_width;
    prp++;
  }

  for (n=11 ; n>=0 ; n--) {
    prp->box [1].x = (prp->box [0].x = minX + n * h_pitch + d) + h_pitch - 1;
    prp->box [0].y = minY;
    prp->spike.y   =
    prp->box [1].y = (prp->box [0].y = minY) + height;
    prp->spike.x   =  prp->box [0].x + h_pitch / 2;
    if (n == 6)
      d = 0;
    prp++;
  }

  prp = point;
  prp->box [0].x = xc - 10;
  prp->box [0].y = maxY - height;
  prp->box [1].x = xc + 10;
  prp->box [1].y = yc + 5;
  prp->spike.x   = xc;
  prp->spike.y   = yc + 9;

  prp = point + 25;
  prp->box [0].x = xc - 10;
  prp->box [0].y = maxY + height;
  prp->box [1].x = xc + 10;
  prp->box [1].y = yc - 5;
  prp->spike.x   = xc;
  prp->spike.y   = yc - 9;
}


/*----------------------------------------------------------------------*/
void 	PutIntuiText (	char 	*s, 
			int 	x, 
			int 	y, 
			int 	color)
/*----------------------------------------------------------------------*/
{
  struct IntuiText	it = {1, 0, JAM2, 0, 0, NULL, NULL, NULL};

  it.FrontPen = color;
  it.LeftEdge = x;
  it.TopEdge = y;
  it.IText = s;

  PrintIText (w->RPort, &it, 0, 0);
}


/*----------------------------------------------------------------------*/
int 	ITextLen (char *s)
/*----------------------------------------------------------------------*/
{
  struct IntuiText	it = {1, 0, JAM2, 0, 0, NULL, NULL, NULL};

  it.IText = s;
  return IntuiTextLength(&it);
}


/*----------------------------------------------------------------------*/
void 	Gsetup (void)
/*----------------------------------------------------------------------*/
{
  struct Preferences	prefs;


  GetPrefs (&prefs, sizeof prefs);

  if ((laced = (prefs.LaceWB & LACEWB))) {
    ns.ViewModes |= LACE;
    ns.Font = &font[1];
    tf = OpenDiskFont(&font[1]);
    piece_ptr = (shape_rec *) &laced_piece;
  }

  if (!(nw.Screen = screen = OpenScreen(&ns))) {
    finit();
    Alert(1);
    exit(1);
  }

  nw.Width   = screen->Width;
  /* lbischoff: this looks better in AROS */
  /* nw.TopEdge = screen->TopEdge + screen->Font->ta_YSize + 2; */
  nw.TopEdge = screen->TopEdge + screen->BarHeight + 1;
  nw.Height  = screen->Height - nw.TopEdge;

  if (!(w = OpenWindow (&nw))) {
    finit ();
    Alert (2);
    exit  (2);
  }

  vp = &screen->ViewPort;
  rp = w->RPort;

  /* intialize temporary area space */
  InitArea(&AInfo, AreaBuf, max_vector);
  rp->AreaInfo = &AInfo;

  if (!(TBuf = (APTR)AllocRaster (screen->Width, screen->Height))) {
    finit ();
    Alert (3);
    exit  (3);
  }

  gen_points (w);
  SetMenuStrip (w, MyMenu);
  rp->TmpRas = &TRas;
  InitTmpRas (&TRas, (char *)TBuf, RASSIZE (screen->Width, screen->Height));
  LoadRGB4 (vp, color_table, 8);
  SetDrMd  (rp, JAM1);

  SetAPen  (rp, back_color);
  RectFill (rp, minX, minY, maxX - 1, maxY);

  SetAPen  (rp, tm1_color);
  RectFill (rp, bar_left, minY, bar_right-1, maxY);

  /* write evaluation count at top of window	*/
  move_num_len  = ITextLen("Number of Moves Evaluated: ");
  move_num_left = minX + 2 + move_num_len;
  PutIntuiText ("Number of Moves Evaluated:", minX + 2,
	minY - screen->Font->ta_YSize - 2, dice_color);

  {
    struct DateStamp	time;

    DateStamp (&time);
    srand (time.ds_Tick);
  }
}


/*----------------------------------------------------------------------*/
void 	ShowDice ( BYTE  	d[4], 
		   int 		c)
/*----------------------------------------------------------------------*/
/* put dice on the screen						*/
{
  int			yu, yl, lace_offset;
  char			line [] = "      ";
  struct IntuiText	it = {1, 0, JAM1, 0, 0, NULL, NULL, NULL};


  lace_offset = (laced) ? 5 : 2;

  /* cover up dice with dice color */
  yu = yc - screen->Font->ta_YSize / 2;
  yl = yu + screen->Font->ta_YSize;

  SetAPen  (rp, dice_color);
  RectFill (rp, xc-27, yu-lace_offset , xc+27, yl+lace_offset);

  if (d[0] && d[1]) {
    /* put in the numbers for the left and right die */
    it.FrontPen = piece2_color;

    if (c == Uside) 
	it.FrontPen = piece1_color;

    line [0] = '0' + d[0];
    line [4] = '0' + d[1];

    it.LeftEdge = xc - piece_width / 2;
    it.TopEdge  = yu + 1;
    it.IText    = line;

    PrintIText (w->RPort, &it, 0, 0);
    SetAPen  (rp, tm1_color);
    RectFill (rp, bar_left, yu-lace_offset, bar_right - 1, yl+lace_offset);
  }
}


/*----------------------------------------------------------------------*/
void 	PutMoveNumber (int Number)
/*----------------------------------------------------------------------*/
/* only calls are from EVAL 						*/
{
  char 	Line [8];

  sprintf (Line, "%5d  ", Number);
  PutIntuiText (Line, move_num_left,
	minY - screen->Font->ta_YSize - 2, dice_color);
}


/*----------------------------------------------------------------------*/
void 	PutPiece ( long 	x, 
		   long 	y, 
		   long 	color)
/*----------------------------------------------------------------------*/
/* coords are for center of piece					*/
{
  int		i;
  const shape_rec	*sp = piece_ptr;


  SetAPen  (rp, color);
  AreaMove (rp, x + sp->point[sp->count - 1].x, y + sp->point[sp->count - 1].y);
  for (i=0 ; i<sp->count ; i++)
    AreaDraw (rp, x + sp->point [i].x, y + sp->point [i].y);
  AreaEnd  (rp);
}


/*----------------------------------------------------------------------*/
int 	decode ( int 	x, 
		 int 	y)
/*----------------------------------------------------------------------*/
/* return the 'spike' number given the screen				*/
/* point relative to upper left corner of the window			*/
{
  int	point,
	disp = 0,
	font_height = screen->Font->ta_YSize;

  if (x > xc)
    disp = bar_right - bar_left;

  /* check if dice 'clicked'...indicate unusual end of turn */
  if ((y > (yc - font_height / 2)) && (y < (yc + font_height / 2)) &&
	(x > xc - 27) && (x < xc + 27))
    return(-1);

  /* check if 'bar' selected */
  if ((x > (xc - piece_width / 2)) && (x < (xc + piece_width / 2))  &&
	( y > (font_height  - 2 - piece_ptr->height)) &&
	(y < (yc - font_height / 2 - 2)))
    point = 0;
  else
    point = 1 + (x + disp - minX) / h_pitch;

  if ( y < yc)
    point = 25 - point;

  return (point);
}


/*----------------------------------------------------------------------*/
void 	DoMenuStrip (char errmsg [])
/*----------------------------------------------------------------------*/
{
  ErrSet = TRUE;
  SetWindowTitles (w, (char *)-1, errmsg);
}


/*----------------------------------------------------------------------*/
void 	UnDoMenuStrip (void)
/*----------------------------------------------------------------------*/
{
  if (ErrSet == TRUE) {
    SetWindowTitles (w, (char *)-1, MyTitle);
    ErrSet = FALSE;
  }
}


/* put up a plain old requestor given three strings			*/
/*		name...	the question/statement of the requestor		*/
/*		yes...	the affirmative response			*/
/*		no...	the negative response				*/
/* this returns 'true' for the yes, 'false' for the no			*/

/*----------------------------------------------------------------------*/
BOOL 	requestor ( char 	name [], 
		    char 	yes [], 
		    char 	no [])
/*----------------------------------------------------------------------*/
{
  const static struct IntuiText
	Rt = {tm1_color,    tm2_color,    JAM2, 50, 10, NL, (char *)0, NL},
	Rr = {spike2_color, spike1_color, JAM1,  8,  3, NL, (char *)0, NL};

  struct IntuiText	Rbody, Rright, Rleft;


  Rbody = Rt;		Rright = Rr;		Rleft = Rr;
  Rleft.IText = no;	Rright.IText = yes;	Rbody.IText = name;
  return (AutoRequest (w, &Rbody, &Rleft, &Rright, 0, 0, 300, 90));
}


/*----------------------------------------------------------------------*/
void 	finit ( void )
/*----------------------------------------------------------------------*/
{
  char	*s[] = {
	" Running totals ",
	"                                     \n",
	"  Total games     xxxxxx             \n",
	"                                     \n",
	"                     You          Me \n",
	"   Dice total xxxxxxxxxx  xxxxxxxxxx \n",
	" Average roll       xx.x        xx.x \n",
	"   Incomplete xxxxxxxxxx  xxxxxxxxxx \n",
	"       Wasted xxxxxxxxxx  xxxxxxxxxx \n",
	"                                     \n",
	" [click in window to terminate]      \n"};

  if (total_games) {
    sprintf (&s[2][18], "%6d\n", (int)total_games);
    sprintf (&s[5][14], "%10d  %10d\n", (int)you.total.roll, (int)me.total.roll);
    sprintf (&s[6][20], "%4.1f        %4.1f\n",
	(float)you.total.average_roll / total_games,
	(float)me.total.average_roll / total_games);
    sprintf (&s[7][14], "%10d  %10d\n", (int)you.total.incomplete, (int)me.total.incomplete);
    sprintf (&s[8][14], "%10d  %10d\n", (int)you.total.wasted, (int)me.total.wasted);
    TextScreen (s, 11);
  }

  if (TBuf)	FreeRaster (TBuf, screen->Width, screen->Height);
  if (w)	CloseWindow (w);
  if (screen)	CloseScreen (screen);
  if (laced)	CloseFont (tf);
}


/*----------------------------------------------------------------------*/
void 	PutBarPiece ( int 	y, 
		      int 	pieces, 
		      int 	inc, 
		      int 	color)
/*----------------------------------------------------------------------*/
{
  char			line [4];
  struct IntuiText	it = {1, 0, JAM1, 0, 0, NULL, NULL, NULL};
  int			lace_offset;


  lace_offset = (laced) ? 5 : 0;

  SetAPen  (rp, back_color);
  RectFill (rp, xc - piece_width + 3, y, xc + piece_width - 3, y + inc);
  SetAPen  (rp, tm1_color);
  RectFill (rp, bar_left, y, bar_right - 1, y + inc);
  pieces = abs (pieces);

  if (pieces != 0) {
    PutPiece (xc, y + inc / 2, color);

    if (pieces > 1) {
      sprintf (line, "%d", pieces);
      it.FrontPen = dice_color;
      it.LeftEdge = bar_left + 2;
      it.TopEdge  = y + lace_offset;
      it.IText    = line;
      PrintIText (w->RPort, &it, 0, 0);
    }
  }
}


/*----------------------------------------------------------------------*/
void 	PutSpike ( int 	spk, 
		   int 	pieces)
/*----------------------------------------------------------------------*/
{
  int			i, lace_offset;
  long			color, inc, x, y;
  register UWORD	temp1x, temp1y, temp0x, temp0y;  /* for speedup */
  char			line [4];
  point_rec		*pp;


  lace_offset = (laced) ? 0 : 6;

  /* pick color of the pieces */
  if (pieces < 0)
    color = piece1_color;
  else
    color = piece2_color;

  inc = -piece_ptr->height;

  if (spk > 12)
      inc = -inc;

  if (spk == 0)				/* plot my ones on the bar	*/
    PutBarPiece (yc + screen->Font->ta_YSize, pieces, -inc, color);
  else if (spk == 25)			/* plot yours on the bar	*/
    PutBarPiece (yc - screen->Font->ta_YSize - inc, pieces, inc, color);
  else {				/* all the rest			*/
    pp = &point [spk];

    /* for speed improvements on slower machines */
    temp0x = pp->box [0].x;
    temp0y = pp->box [0].y;
    temp1x = pp->box [1].x;
    temp1y = pp->box [1].y;

    /* box in the area around the spike with background color...*/
    SetAPen  (rp, back_color);
    RectFill (rp, temp0x, temp0y, temp1x, temp1y);

    if (spk & 1)
      SetAPen (rp, spike1_color);
    else
      SetAPen (rp, spike2_color);

    if (spk < 13) { 				/* lower spikes */
      AreaMove (rp, temp0x, temp1y);			/* lower left corner	*/
      AreaDraw (rp, temp1x, temp1y);			/* lower right corner	*/
      AreaDraw (rp, pp->spike.x, pp->spike.y); 		/* point of spike	*/
      AreaDraw (rp, temp0x, temp1y);			/* lower left corner	*/
    AreaEnd (rp);
    }

    else { 					/* upper spikes */
      AreaMove (rp, temp0x, temp0y);			/* upper left corner	*/
      AreaDraw (rp, temp1x, temp0y);			/* upper right corner	*/
      AreaDraw (rp, pp->spike.x, pp->spike.y);		/* point of spike	*/
      AreaDraw (rp, temp0x, temp0y);			/* upper left corner	*/
    AreaEnd (rp);
    }

    if (pieces < 0)
      pieces = -pieces;

    /* go through number of pieces, up to "PerSpike" */
    x = pp->spike.x;
    y = (spk > 12) ? temp0y : temp1y;

    y += inc / 2;
    inc += 2 * sign (inc);

    for (i=0 ; (i < pieces) && (i < PerSpike) ; i++) {
      PutPiece (x, y, color);
      y += inc;
    }

    /* put some fancy numbers on it if > PerSpike pieces */
    if (pieces > PerSpike) {
      sprintf (line, "%2d", pieces);
      y = (spk > 12) ? (temp0y + 14 - lace_offset) : (temp1y - 8 + lace_offset);

      Move    (rp, x - screen->Font->ta_YSize +3, y);
      SetAPen (rp, dice_color);
      Text    (rp, line, 2);
    }
  }
}  /* end of PutSpike */


/*----------------------------------------------------------------------*/
void 	BlinkPiece ( BYTE 	board [],
		     int 	pos)
/*----------------------------------------------------------------------*/
{
  int		x, y, i, color, inc;
  point_rec	*pp;


  if (board [pos]) {		/* Don't bother if nothing there	*/
    pp = &point [pos];
    x  = pp->spike.x;
    inc = piece_ptr->height;

    if ((pos == 0) || (pos == 25))
      y = pp->spike.y;
    else {
      inc = -piece_ptr->height;
      y = pp->box [1].y;

      if (pos > 12) {
	inc = -inc;
	y = pp->box [0].y;
      }

      y   += inc / 2;
      inc += 2 * sign(inc);
    }

    if ((pos != 0) && (pos != 25))
      y += (min (abs (board [pos]), PerSpike) - 1) * inc;

    color = piece2_color;

    if (board [pos] < 0)
      color = piece1_color;

    for (i=2; i>=0; i--) {
      Delay (1);

      switch (pos) {

	case  0:
	  PutBarPiece (yc + screen->Font->ta_YSize, board [0], inc, color);
	  break;

	case 25:
	  PutBarPiece (yc - screen->Font->ta_YSize - inc, board [25], inc, color);
	  break;

	default:
	  PutPiece (x, y, blink_color);
      }

      Delay (1);

      switch (pos) {

	case  0:
	  PutBarPiece (yc + screen->Font->ta_YSize, board [0], inc, color);
	  break;

	case 25:
	  PutBarPiece (yc - screen->Font->ta_YSize - inc, board [25], inc, color);
	  break;

	default:
	  PutPiece (x, y, color);
      }
    }
    PutSpike (pos, board [pos]);
  }
}


/*----------------------------------------------------------------------*/
void 	TextScreen ( char 	*stuff [], 
		     int 	num)
/*----------------------------------------------------------------------*/
{
  int			i, n, longest = 0;
  unsigned long		Class;
  struct IOStdReq	*wr;
  struct MsgPort	*wrp;
  struct Window		*crw;
  struct IntuiMessage	*message;
  const  char		cons_init[] = "\2330 p";
  struct IntuiText	it = {1, 0, JAM1, 0, 0, NULL, NULL, NULL};


  for (i=1 ; i<num ; i++) {
    it.IText = stuff [i];
    if ((n = IntuiTextLength (&it)) > longest)
      longest = n;
  }

  crnw.Width    = min (longest +10, screen->Width);
  crnw.LeftEdge = (screen->Width - crnw.Width) / 2;
  crnw.Height   = (num + 2) * screen->Font->ta_YSize;
  crnw.TopEdge  = (maxY - crnw.Height) / 2;
  crnw.Screen   = screen;
  crnw.Title    = stuff [0];
  crw           = (struct Window *) OpenWindow (&crnw);

  if (crw != NULL) {
    wrp = CreatePort  ("my.con.write", 0);
    wr  = CreateStdIO (wrp);
    wr->io_Data = (APTR) crw;
    wr->io_Length = sizeof (struct Window);

    if (OpenDevice ("console.device", 0, (struct IORequest *)wr, 0) == 0) {
      wr->io_Command = CMD_WRITE;
      wr->io_Length  = -1;
      wr->io_Data    = (APTR) cons_init;
      DoIO ((struct IORequest *)wr);

      /* loop through all the stuff and put it to the open'd console */
      for (i=1 ; i<num ; i++) {
	wr->io_Length = -1;
	wr->io_Data   = (APTR) stuff [i];
	DoIO ((struct IORequest *)wr);
      }

      /* close the console */
      CloseDevice ((struct IORequest *)wr);
      DeleteStdIO (wr);
      DeletePort  (wrp);

    }/* end-if console created */

    for (;;) {
      Wait (1L << crw->UserPort->mp_SigBit);
      message = (struct IntuiMessage *) GetMsg (crw->UserPort);
      Class   = message->Class;
      ReplyMsg ((struct Message *)message);
      if (Class == MOUSEBUTTONS)
	break;
    }

    CloseWindow (crw);

  }  /* end-if window opened */
}

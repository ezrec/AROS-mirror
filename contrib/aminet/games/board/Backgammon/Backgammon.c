/************************************************************************/
/*									*/
/*  Hoser BackGammon version 1.0					*/
/*									*/
/*	Robert Pfister							*/
/*									*/
/*	Rfd#3 Box 2340		home:(207)-873-3520			*/
/*	Waterville, Maine 04901						*/
/*									*/
/*      Pfister_rob%dneast@dec.decwrl					*/
/*									*/
/*									*/
/*  Copyright  June,1987						*/
/*									*/
/*  This program will play a game of backgammon at the novice level	*/
/*									*/
/*  The code is in 4 parts...						*/
/*   /									*/
/* \/   1) back.c     - main driver					*/
/*	2) eval.c     - evaluation of moves				*/
/*	3) backscn.c  - screen stuff..					*/
/*	4) backmenu.c - menu stuff, help text, and "decoder" 		*/
/*									*/
/*	this was compiled under SASC 6.56				*/
/*									*/
/************************************************************************/
/* Contains:								*/
/*	ShowStandings	main		Restart		StoreMove	*/
/*	RecallMove	Won		UserMove	Roll		*/
/*	GameOver	stats		NewGame		valid		*/
/*	update								*/
/* most recent modification	11/ 8/93	E.M.Greene		*/
/* most recent modification	 2/14/94	E.M.Greene		*/
/* most recent modification	 3/ 9/95	E.M.Greene		*/
/* most recent modification	 7/ 1/95	E.M.Greene		*/
/*                                                                      */
/* Some corrections                                                     */
/* and modifications             5/16/96        F. Bernard              */
/*                                       (bernard@informatik.uni-kl.de) */
/*                                                                      */
/* You like Tetris ???   ==>>>  Try HEXAGONS !!!       (look at AMINET) */
/*                                                                      */
/************************************************************************/

/* Revisions, corrections, and improvements include:
1. Corrected errors in provided source code:
   a. InitArea() call in Gsetup had incorrect value for number of points
      thereby resulting in no pieces being displayed.
   b. TextScreen() failed to perform CloseDevice() upon completion
      thereby resulting in strange phenomena after being called
   c. stats() failed to display anything if program initiated from
      Workbench.  Revised to call TextScreen.
   d. Bug in eval() resulted in poor performance.  Correction made the
      computer a much more vigorous opponent -- it'll now eat your lunch
      a goodly percentage of the time.
   e. Numerous grammatical and spelling errors corrected, mainly "peice".
   f. Revised the process of displaying numbers on pieces when more than
      PerSpike pieces are on one point so that counts greater than nine
      are shown correctly.
   g. Corrected error in StoreMove so that "stack" overflow is properly
      detected.
   h. Added code to reset StackPos at start of new game.  Otherwise, if
      you play long enough, the stack would overflow and the bug of
      preceding item will result in a flameout.
   i. Eliminated Gerror from BackScreen and replaced with Alerts.  Gerror
      was trying to display error message about screen or window opening
      failures on a screen or window which hadn't opened!

2. Revisions:
   a. Eliminated dead code and variables.  Variable names were revised in
      a number of places to be more meaningful.  Reformatted source and
      added comments to make the source more readable.
   b. Merged IntuiMessage processing into one routine and revised to look
      for all messages currently queued.  Menu operations are now much
      more effective.
   c. Revised the dice "throw" generation to reduce the instances of the
      same values being repeatedly generated.
   d. Reduced declared sizes for AreaBuf and StackSize to something more
      reasonable.
   e. Replaced the calls to the lightly-documented function RangeRand
      with calls to Scott McMahan's implementation of the algorithm of
      Pierre L'Ecuyer [L'Ecuyer, P. Efficient and Portable Combined Random
      Number Generators. Commun. ACM 31, 6 (June, 1988) 742].
   f. Replaced the routine CopyBoard in BackEval with a macro invoking
      memcpy().

3. Enhancements:
   a. Array point[] was added to eliminate need to constantly compute
      board positions which are constants.
   b. Loop count in BlinkPiece() was reduced to expedite moves.
   c. Only one pair of dice are displayed thereby eliminating confusion
      about which pair of numbers human is to play.
   d. Revised color specification such that ColorRequestor can potentially
      be added.
   e. Added readout of relative standings of players.
   f. Recompiled with Lattice C resulting in more randomness in dice throws.
      [SAS Institute had ought to know how to produce a random number generator
      since they are in the statistical analysis business!]
   g. Revised game statistics to include number of points rolled but not
      used ("wasted").  Also, now show average roll to one decimal place.
   h. Revised as required to compile cleanly with SAS/C 6.3
   i. Added "cost" of coming off bar to "wasted" count	(11/16/94)
   j. Added counting of total rolls and display (2/14/94).
   k. Added total games and effective average roll to data display (12/27/95)

4. Modifications (by F.Bernard):
   a. Color definitons changed to get a more suitable board.
   b. Coordinates of displayed text corrected.
      (there where some lace/nolace problems)
   c. Some bugs in graphic functions removed.
   d. Sourcecode completely reformatted and cleaned up.
   e. Speed improvements for the creation of the playfield.
   f. Speedups in BackEval.c
   g. Random function replaced with the rand48 function.
   h. Enforcer hits removed !!
   i. Layout of menu-strips and requesters corrected.

*/

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <libraries/dos.h>
#include "back.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <math.h>
#include <time.h>
#include "intuition/intuition.h"


#define StackSize	100

/* lbischoff: Added basepointers */
struct IntuitionBase *IntuitionBase = NULL;
struct GfxBase *GfxBase = NULL;
struct Library *DiskfontBase = NULL;

extern int		AvgPos;
extern int		Moves;
extern int		minY;
extern struct RastPort	*rp;
extern struct Screen	*screen;
extern struct Window	*w;

you_me_rec		you, me;
USHORT			total_games = 0;

static signed char
	BoardPos [StackSize][26],/* stack of the previous positions	*/
	DicePos [StackSize][4],  /* stack of the dice rolls		*/
	StackPos,		 /* how many moves back are we at?	*/
	MoveList [4],		 /* stack of this turn's moves		*/
	CurMove = 0;		 /* place in the list			*/
signed char	Turn,		 /* whose turn is it?			*/
		board [26],	 /* describes the board position	*/
				 /* 0 and 25 are the bar		*/
		Dice [4];	 /* describes the 2 or 4 dice availible	*/
const char	version [] = "$VER: Backgammon 2.3 (96/05/16)";


/*----------------------------------------------------------------------*/
void 	ShowStandings ( void )
/*----------------------------------------------------------------------*/
{
  char	s[6];
  int	Msum=0,  Ysum=0, i;

  for (i=0 ; i<=24; i++)
    if (board [i] > 0)
      Msum += (25 - i) * board [i];

  for (i=1 ; i<=25; i++)
    if (board [i] < 0)
      Ysum -= i * board [i];

  /* write box scores at top of window	*/
  SetAPen  (rp, tm1_color);
  RectFill (rp, 400L, minY-3-screen->Font->ta_YSize, 520, minY-2);
  SetAPen  (rp, dice_color);
  sprintf  (s,"%4d", Msum);
  if (board [0] != 0)
    strcat (s,"+");
  Move (rp, 400L, (long)minY - 6);
  Text (rp,s,strlen(s));
  sprintf (s,"%4d", Ysum);
  if (board [25] != 0)
    strcat (s,"+");
  Move (rp, 460L, (long)minY - 6);
  Text (rp, s, strlen(s));
}


/*----------------------------------------------------------------------*/
unsigned 	short real_rand ( void )
/*----------------------------------------------------------------------*/
{
  return (unsigned short) (drand48 () * 6.0);
}


/*----------------------------------------------------------------------*/
int 	Roll (BYTE d[4])
/*----------------------------------------------------------------------*/
/* roll the dice  (4 results)						*/
{
  register int	 n;


  d[2] = d[3] = 0;

  for (n = (real_rand() >> 5) & 7; n >= 0; n--)
    d[0] += real_rand();

  d[1] = (abs (d[0]) % 6) + 1;		/* RangeRand(6L)+1; */

  for (n = (real_rand() >> 7) & 7; n >= 0; n--)
    d[0] += real_rand();

  d[0] = (abs (d[0]) % 6) + 1;		/* RangeRand(6L)+1; */

  if (d[0]==d[1]) {			/* if rolled doubles		*/
    d[2] = d[3] = d[1];
    return (1);
  }
  else
    return (0);
}


/*----------------------------------------------------------------------*/
int 	NewGame ( signed char 	board [26], 
		  signed char 	Dice  [4])
/*----------------------------------------------------------------------*/
{
  int i;


  StackPos = 0;
  memset (board, 0, sizeof_board);	/* initialize the board		*/

  board [ 1]= 2;		board [24]=-2;
  board [12]= 5;		board [ 6]=-5;
  board [17]= 3;		board [ 8]=-3;
  board [19]= 5;		board [13]=-5;

  memset (&you, 0, offsetof (you_me_rec, total.roll));
  memset (&me,  0, offsetof (you_me_rec, total.roll));
  AvgPos = 0;

  for (i=0 ; i<=25 ; i ++)
    PutSpike (i, board [i]);

  while (Roll (Dice));			/* until no doubles		*/
  if (Dice [0] > Dice [1])		/* see who goes first		*/
    return (Uside);
  else
    return (Cside);
}


/*----------------------------------------------------------------------*/
int 	UserMove ( BYTE 	board [26], 
		   BYTE 	Dice  [4], 
		   int 		side)
/*----------------------------------------------------------------------*/
{
  BYTE			Pick = 0, Valid = 0;  /* flags */
  int			i, x, y,
			from_point, to_point;
  struct IntuiMessage	*message;
  unsigned long		Class, msg_flag = 1L << w->UserPort->mp_SigBit;
  unsigned short	Code;


  while (Valid == 0) {
    Wait(msg_flag);

    while (message=(struct IntuiMessage *) GetMsg(w->UserPort)) {
      Code = message->Code;
      Class = message->Class;
      x = message->MouseX;
      y = message->MouseY;
      ReplyMsg((struct Message *)message);

      if (Class == MENUPICK) {
	if (Code != MENUNULL)
	  Valid = DoMenu ((MENUNUM (Code)*100) + (ITEMNUM (Code)*10) + SUBNUM (Code));
      }
      /* check if mouse pressed..... */
      else if ((Class == MOUSEBUTTONS) && (Code == SELECTDOWN)) {
	if ((to_point = decode(x,y)) == -1) {	/* clicked dice		*/
	  you.wasted += Dice [0] + Dice [1] + Dice [2] + Dice [3];
	  Dice [0] = Dice [1] = Dice [2] = Dice [3] = 0;
	  Valid = 1;
	  you.incomplete++;
	}
	else
	  if (Pick == 0) {		/* first click			*/
	    if (board [to_point] * side<=0)
	      DoMenuStrip ("No piece there!!");
	    else {
	      Pick = 1;
	      from_point = to_point;
	    }
	    break;
	  }

	  else if (Pick == 1) {		/* second click			*/
	    Pick = 0;
	    for (i=0 ; (Valid == 0) && (i<4) ; i++)
	      if ((Dice [i] != 0) &&
		 (valid(board, from_point, to_point, Dice [i]))) {

		if (from_point == 25)
		  you.wasted += 25 - to_point;

		if (from_point == to_point)
		  you.wasted += (Dice [i] - from_point);

		BlinkPiece (board, from_point);
		update (board, from_point, to_point, side);
		PutSpike (from_point , board [from_point]);
		BlinkPiece (board, to_point);
		PutSpike (0 , board [ 0]);
		PutSpike (25, board [25]);
		Valid = 1;
		Dice [i] = 0;
	      }
	    UnDoMenuStrip ();
	  }	/* end Pick == 1 */
		/* end else decode */
	if (Valid == 0)
	  DoMenuStrip ("Not Valid move");
      };	/* end if mousebutton */
    }		/* end while message */
  }		/* end while Valid */
  return (Valid != -1);
} /* end UserMove */


/*----------------------------------------------------------------------*/
void 	StoreMove ( void )
/*----------------------------------------------------------------------*/
{
  if (StackPos < StackSize - 1) {	/* can only store that many	*/
    memcpy (&BoardPos [StackPos][0], board, sizeof board);
    memcpy (&DicePos  [StackPos][0], Dice,  sizeof Dice);
    StackPos++;
  }
}


/*----------------------------------------------------------------------*/
void 	stats ( void )
/*----------------------------------------------------------------------*/
{
  char *stat_data [] = {
    " Game Statistics",
    "\n",
    " Number of games: xx\n  ",
    "\n",
    " Number of moves xx;  Avg positions evaluated xxxx \n",
    "\n",
    " Avg. roll   : User  xx.x Computer  xx.x \n  ",
    " Effect. avg.: User  xx.x Computer  xx.x \n  ",
    " Doubles     : User  xx   Computer  xx \n    ",
    " Inc. moves  : User  xx   Computer  xx \n    ",
    " Turns on bar: User  xx   Computer  xx \n    ",
    " Wasted      : User  xx   Computer  xx \n    ",
    "\n",
    "	[click in window to continue]\n         "};

  if ((you.moves != 0) && (me.moves != 0)) {
    sprintf (stat_data[2]," Number of games: %d\n", ++total_games);
    sprintf (stat_data[4]," Number of moves %d;"
	"  Avg positions evaluated %d\n", me.moves, AvgPos / me.moves);
    sprintf (stat_data[6]," Avg roll    : User %5.1f Computer %5.1f \n",
	(float)you.dice_total / you.moves, (float)me.dice_total / me.moves);
    sprintf (stat_data[7]," Effect. avg.: User %5.1f Computer %5.1f \n",
	(float)(you.dice_total - you.wasted) / you.moves,
	(float)(me.dice_total - me.wasted) / me.moves);
    sprintf (stat_data[8]," Doubles     : User  %2d   Computer  %2d \n",
	you.doubles, me.doubles);
    sprintf (stat_data[9]," Inc. moves  : User  %2d   Computer  %2d \n",
	you.incomplete, me.incomplete);
    sprintf (stat_data[10]," Turns on bar: User  %2d   Computer  %2d\n",
	you.on_bar, me.on_bar);
    sprintf (stat_data[11]," Wasted      : User %3d   Computer %3d\n",
	you.wasted, me.wasted);

    TextScreen (stat_data, 14);
    you.total.roll += you.dice_total;
    me.total.roll  += me.dice_total;
    you.total.average_roll += (float)you.dice_total / you.moves;
    me.total.average_roll  += (float)me.dice_total / me.moves;
    you.total.incomplete += you.incomplete;
    me.total.incomplete  += me.incomplete;
    you.total.wasted += you.wasted;
    me.total.wasted  += me.wasted;
  }
}


/*----------------------------------------------------------------------*/
void 	GameOver (BOOL  show_stats)
/*----------------------------------------------------------------------*/
{
  if (show_stats)  stats ();
  finit ();
  exit (0);
}


/*----------------------------------------------------------------------*/
int 	Won ( BYTE 	board [26], 
	      int 	side)
/*----------------------------------------------------------------------*/
{
  int i;


 if (side == 1) {
   for (i=0 ;i<=25;i++)
     if (board [i] > 0)
       return (0);
 }
 else
   for (i=0 ;i<=25; i++) {
     if (board [i] < 0)
       return (0);
 }
 return (1);
}


/*----------------------------------------------------------------------*/
int 	main ( void ) 	/* main procedure */
/*----------------------------------------------------------------------*/
{
  /* lbischoff: Added OpenLibrary calls */
  IntuitionBase = (struct IntuitionBase *) OpenLibrary ("intuition.library", 30);
  if (!IntuitionBase) goto cleanup;
  GfxBase = (struct GfxBase *) OpenLibrary ("graphics.library", 30);
  if (!GfxBase) goto cleanup;
  DiskfontBase = (struct Library *) OpenLibrary ("diskfont.library", 30);
  if (!DiskfontBase) goto cleanup;
  
    
  Gsetup ();
  memset (&you, 0, sizeof (you));
  memset (&me,  0, sizeof (me));

  srand48 (time (NULL));

  Turn = NewGame (board, Dice);

  for (;;) { /* go until game is over */
    if (Turn == Uside)
      DoMenuStrip ("Your turn");
    else
      DoMenuStrip ("My turn");

    ShowDice (Dice, Turn);
    ShowStandings ();

    if (Turn == Uside) {
      StoreMove(); /* store this board position */
       /* user stats */
      if (Dice [2] > 0)
	you.doubles++;

      you.dice_total += Dice [0] + Dice [1] + Dice [2] + Dice [3];
      you.moves++;

      if (board [25] < 0)
	you.on_bar++;
      /* do the Move */
      do
        if (UserMove (board, Dice, Turn) == 0)
	  GameOver (TRUE); /* escape */
      while ((Turn == Uside) && (Won (board, Turn) == 0) &&
	     (Dice [0] + Dice [1] + Dice [2] + Dice [3]!=0));

      you.wasted += (Dice [0] + Dice [1] + Dice [2] + Dice [3]);
    }

    else { /* turn == Cside */
      /* computer stats */
      if (Dice [2] > 0)
	me.doubles++;

      me.dice_total += Dice [0] + Dice [1] + Dice [2] + Dice [3];
      me.moves++;

      if (board [0] > 0)
	me.on_bar++;

      GenerateMoves (board, Dice);	/* do move			*/
      DoMove (board, Dice);
    }

    Turn = -Turn;
    Roll (Dice);

    if (Turn == 0)			/* signal to restart the game	*/
      Turn = NewGame (board, Dice);
    if (Won (board, Uside)) {		/* did user win yet?		*/
      ShowStandings ();
      stats ();

      if (requestor ("I lose.  Play again ?","Yes","No"))
	GameOver (FALSE);
      else
	Turn = NewGame (board, Dice);
    }

    if (Won (board, Cside)) {		/* did computer win yet?	*/
      ShowStandings ();
      stats ();
      if (requestor ("Yeah !  I Win.  Play Again ?","Yes","No"))
	GameOver (FALSE);
      else
	Turn = NewGame (board, Dice);
    }
  } /* end do forever */

cleanup:
  /* lbischoff: Added CloseLibrary calls */
  if (DiskfontBase) CloseLibrary (DiskfontBase);
  if (GfxBase) CloseLibrary (GfxBase);
  if (IntuitionBase) CloseLibrary (IntuitionBase);
  
  return 0;
} /* end of main */


/*----------------------------------------------------------------------*/
void 	Restart ( void )
/*----------------------------------------------------------------------*/
{
   Turn = 0;
}


/*----------------------------------------------------------------------*/
void 	RecallMove (int how)
/*----------------------------------------------------------------------*/
{
  register int 	i;


  StackPos = StackPos-1 + how;

  if (StackPos < 0)
    StackPos = 0;

  for (i=0 ; i < 26; i++)
    if (board [i] != BoardPos [StackPos][i])  { /* only refresh stuff changed */
      board [i] = BoardPos [StackPos][i];
      PutSpike (i, board [i]);
    }
  for (i=0 ; i<4 ; i++)
    Dice [i] = DicePos [StackPos][i];

  ShowDice (Dice, Uside);
  StackPos++;
}


/*----------------------------------------------------------------------*/
int 	valid ( BYTE 	board [26], 
		int 	from_point, 
		int 	to_point, 
		int 	die_value)
/*----------------------------------------------------------------------*/
/* is the move valid for the board and one die? 			*/
{
  register UWORD	i, temp = 25-die_value;
  	   BYTE		sign, 
			All_In,
			*bp;


  All_In = -1;

  if (Turn == Cside) {			/* computer's move			*/
    if ((board [0] != 0) && (from_point != 0))
      return (0);			/* man on bar and didn't select bar	*/
    sign = Cside;

    /* see if all pieces are in home base */
    bp = &board [1];

    for (i=1 ; i<=24; i++)
      if (*(bp++) > 0) {		/* find man fartherest from home	*/
	All_In = i;
        break;
      }

    /* check if can take it off */
    if (to_point > 24) {		/* if in home board			*/
      if ((All_In > 18) &&		/*   if all men in home board		*/
          ((from_point == temp) ||
	   (temp < All_In) && (from_point == All_In)))
        return (1);
      else
        return (0);
    }
  }

  else {				/* human's move				*/
    if ((board [25] != 0) && (from_point != 25))
      return (0);			/* man on bar and didn't select bar	*/

    sign = Uside;

    /* see if all pieces are in home base */
    bp = &board [25];
    for (i = 24; i >= 1; i--)
      if (*(--bp) < 0) {		/* find man fartherest from home	*/
	All_In = i;
        break;
      }

    /* see if can take off */
    if (to_point == from_point) {	/* double-clicked?			*/
      if ((All_In < 7) &&		/*   all pieces in home board		*/
	  ((from_point == die_value) ||
	   ((die_value > All_In) && (from_point == All_In))))
	return (1);
      else
	return (0);
    }
  }

  if ((to_point-from_point) * sign != die_value)
    return (0);				/* distance to move != die value	*/

  if (board [to_point] * sign < -1)
    return (0);				/* can't land at "to" point		*/

  return (1);				/* I suppose that's all there is..	*/
}


/*----------------------------------------------------------------------*/
int 	update ( BYTE 	*board, 
		 int 	from_point, 
		 int 	to_point, 
		 BYTE 	sign)
/*----------------------------------------------------------------------*/
/* put a given move onto the board              			*/
/* remove the piece from the source spike       			*/
{

  board [from_point] -= sign;

  /* if taking a piece off board or bar, don't go any further */
  if ((to_point == from_point) || (to_point >= 25))
    return (1);

  /* add a piece to the destination spike and determine if capturing	*/
  board [to_point] += sign;

  if (board [to_point] == 0) {		/* must have hit opponent	*/
    int bar = 0;

    if (sign < 0)
      bar = 25;
    board [to_point] = sign;		/* put us onto point		*/
    board [25-bar]  -= sign;		/* put opponent on bar		*/
  }
  return (0);
}

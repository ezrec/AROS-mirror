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
/* this was compiled under Manx 3.20a, using long integers		*/
/*									*/
/* This file contains global definitions for Hoser Backgammon		*/
/************************************************************************/
/* Last modification:		 1/ 3/92				*/
/* Most recent modification	 5/16/94	E.M.Greene		*/
/* Most recent modification	 2/14/95	E.M.Greene		*/
/* Most recent modification	 5/16/96	F. Bernard		*/

#define Uside		-1
#define Cside		1
#undef  NULL
#define NULL		((void *) 0)
#define NL		0

#define tm1_color	0
#define tm2_color	1
#define back_color	2
#define spike1_color	3
#define spike2_color	4
#define piece1_color	5
#define piece2_color	6
#define dice_color	7
#define blink_color	0
#define PerSpike	5L
#define sizeof_board	(26 * sizeof(BYTE))

#include <aros/oldprograms.h>
#include <exec/types.h>

typedef struct {
    USHORT	incomplete,		/*	YouInc,		MeInc,	*/
		dice_total,		/*	YouD,		MeD,	*/
		moves,			/*	YouM,		MeM,	*/
		on_bar,			/*	YouBar,		MeBar,	*/
		doubles,		/*	You2,		Me2,	*/
		wasted;			/*	YouWasted;	MeWasted*/
	    struct {
		      ULONG	roll;
		      float	average_roll;
		      ULONG	incomplete,
				wasted;
		} total;
    } you_me_rec;


/* globally-referenced routines' definitions				*/

void	DoMenuStrip   (char *text);
void	PutMoveNumber (int count);
void	BlinkPiece    (BYTE *board, int piece);
int	update        (BYTE *board, int from_point, int to_point, BYTE sign);
void	PutSpike      (int side, int board_pos);
int	valid         (BYTE *board, int i1, int i2, int die2);
int	DoMenu        (int menu_item);
void	DoMenuStrip   (char *s);
void	UnDoMenuStrip (void);
int	decode        (int x, int y);
void	Gsetup        (void);
void	ShowDice      (BYTE *dice, int turn);
void	finit         (void);
void	DoMove        (BYTE *board, BYTE *dice);
void	GenerateMoves (BYTE *board, BYTE *dice);
void	TextScreen    (char **text, int lines);
BOOL	requestor     (char name[], char yes[], char no[]);

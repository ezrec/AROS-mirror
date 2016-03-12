/********************************************************************/
/*                                                                  */
/*  Hoser BackGammon version 1.0                                    */
/*                                                                  */
/*      Robert Pfister                                              */
/*  Copyright  June,1987 all rights reserved.                       */
/********************************************************************/
/*                                                                  */
/* Code speedups for V2.3 done by F.Bernard (16-May-96)             */
/*                                                                  */
/********************************************************************/

/****************************************************************/
/* These routines  do all the thinking for the computer side    */
/* (read: the magic lives here)                                 */
/****************************************************************/

/* This file contains:
	Eval		vulnerable	doom		[CopyBoard]
	DoMove		FindMove	GenerateMoves	Find4
	Find3		Find2		Find1
*/

#include "back.h"
#include <string.h>

#define Running  1
#define Blocking 2
#define Pegging  3
#define BackGame 4

#define CopyBoard(b1, b2)   memcpy (b2, b1, sizeof_board)

extern you_me_rec	me;

int	Moves    = 0,	
	AvgPos   = 0,
	LastVun  = 0,
	count    = 0,
	max      = 0,
	GameStat = 0,
	MaxStat  = 0,
	list  [4][2];

const BYTE	block[26] = {-1,1,2,3,4,5,6,      8,9,10,11,12,13,
			     14,15,17,21,25,27,   25,21,19,8,3,1,0},

		run[26]   = {-1,1,2,3,4,5,6,      15,16,17,18,19,20,  
			     30,31,32,33,34,35,   50,51,52,53,54,55},

		peg[26]   = {-20,0,0,0,0,0,0,     1,2,3,4,5,6, 
			     30,31,32,33,34,35,   50,49,48,47,46,45,0},

		back[26]  = {20,40,30,20,10,5,0,  1,2,3,4,5,6,
			     30,31,32,30,40,50,   50,49,48,30,20,10,0};



/*----------------------------------------------------------------------*/
int 	doom (BYTE Board [26])
/*----------------------------------------------------------------------*/
/* find the "doom" factor..the possibility of getting outa being captured */
{
  register	int	i; 
  register	int	value = 0;


  if (LastVun == 0)
    return (0);

  for (i=1 ; i<=10 ; i++)
    if (Board [i] < -1)
      value++;

  return (5 * value);
}


/*----------------------------------------------------------------------*/
int 	vulnerable ( BYTE 	Board [26], 
		     int 	pos)
/*----------------------------------------------------------------------*/
{
  register	int	i, j, k;
  register	int	value = 0;
  		int	HitTable [7][7];


  if ( (Board [pos]<0) || (pos == 0) || (Board [pos] >= 2) )
    return (0);

  memset (&HitTable, 0, sizeof (HitTable));	/*  intitialize table */

  for (i=1 ; i<=12 ; i++)			/* look at all dice within 12 places	*/
    if (pos-i >= 0)				/* within the board			*/
      if (Board [pos-i] < 0)			/* there exists a piece at that place	*/
	if (valid (Board, pos, pos-i, i)) {	/* can capture legally			*/
	  if (i <= 6)				/* find hits for one die		*/
	    for (j=1 ; j<=6 ; j++) {
	      HitTable [i][j] = 1;
	      HitTable [j][i] = 1;
	    }
	  for (j=1 ; j<i ; j++) { 		/* figure out combinations		*/
	    k = i-j;				/* first die is j, second is k		*/
						/* check legality of the intermediate place*/
	    if (valid (Board, pos, pos-j, j)) {
	      HitTable [k][j] = 1;
	      HitTable [j][k] = 1;
	    }
	    else if (valid (Board, pos, pos-k, k)) {
	      HitTable [k][j] = 1;
	      HitTable [j][k] = 1;
	    }
	  }
	} /* end if valid (board, position) */

  for (i=1 ; i<=6 ; i++)			/* find how many hits in that table	*/
    for (j=1 ; j<=6; j++)
      if (HitTable [i][j] == 1)
	value++;

  LastVun = value;
  return (value);

}  /* end of vulnerable */


/*----------------------------------------------------------------------*/
int 	Eval (BYTE   Board [26])
/*----------------------------------------------------------------------*/
{
  register	int	i, value = 0, temp;
  		int	num  = 0, me, you;
  		int	Msum = 0, Ysum = 0;

  /* see who is ahead */
  for (i=0 ; i<=24 ; i++)
    if (Board [i] > 0)
      Msum += (25 - i) * Board [i];

  for (i=1 ; i<=25 ; i++)
    if ((temp = Board [i]) < 0)
      Ysum -= i * temp;

  for (i=0 ; (i <= 24) && (Board[i] <= 0) ; i++); /* first occurence of me	*/
  me = i;

  for (i=25; (i >= 1) && (Board[i] >= 0) ; i--); /* first occurence of you	*/
  you = i;

  GameStat = BackGame;	/* default case				*/
  if (Ysum > Msum+10) 	GameStat = Running;	/* if closer to winning than them	*/
  if (you > 18)		GameStat = Blocking;	/* if an opponent is within the bounds	*/
  if (me > you)		GameStat = Pegging;	/* if complete separation		*/

  for (i=0 ; i<=24 ; i++) {
    if ((temp = Board [i]) > 0) {
      num += temp;

      switch (GameStat) {

	case BackGame:
	  value += back [i] * temp;
	  if (i < 18)
	    value += 3 * vulnerable (Board, i);
	  break;

	case Pegging:
	  value += peg [i] * temp;
	  break;

	case Blocking:
	  value += block [i] * temp;
	  value -= 4 * vulnerable (Board, i);
	  value -= 6 * doom (Board);
	  break;

	case Running:
	  value += run [i] * temp;
	  value -= 6 * vulnerable (Board, i);
	  value -= 6 * doom (Board);
	  break;
	}	/* end cases	*/
      }		/* end if	*/
    }		/* end for	*/

    /* add points for taking men off...more if no chance of being taken */
    if (GameStat == Pegging)
      value += (15-num) * 300;
    else
      value += (15-num) * 75;

    switch (GameStat) {

      case BackGame:

	num = 0;
	for (i=0 ; i <= 6 ; i++)
	  if ((Board [i] > 0) && (you > i))
	    num++;

	value += 100 * num;
	num = 0;

	for (i=15 ; i <= 21 ; i++)
	  if ((temp = Board [i]) > 0)
	    num += temp;

	value += 30  * num;
	value -= 100 * Board [25];
	num = 0;

	for (i=17 ; i <= 23 ; i++)
	  if (Board [i] >= 2)
	    num++;

	value += (num * 300);
	break;

      /* if blocking game, check the block length...add points for more */
      case Blocking:

	num = 0;
	for (i=14 ; i <= 22 ; i++) {
	  if ((temp = Board [i]) >= 2)
	    num++;
	  if ((you > i) && (temp >= 2))
	    num++;
	}

	value += (num * 400);
	num = 0;

	for (i=0 ; i <= 17 ; i++)
	  if ((temp = Board [i]) > 0)
	     num += (temp * (18-i) * (18-i));

	value -= num * 2/3;
	break;

      case Pegging:
      case Running:

	num = 0;
	for (i=0 ; i <= 17 ; i++)
	  if (Board [i] > 0)
	    num++;

	value -= 10 * num;
	num = 0;

	for (i=0 ; i <= 17 ; i++)
	  if ((temp = Board [i]) > 0)
	    num += (temp * (26-i) * (26-i));

	value -= num;
	break;
    }
  return (value);
}


/*----------------------------------------------------------------------*/
void 	DoMove ( BYTE 	Board [26], 
		 BYTE 	Dice  [4])
/*----------------------------------------------------------------------*/
{
  register int	j;


  AvgPos += count;

  /* don't do anything if nothing to be done  */
  if (count == 0) {
    DoMenuStrip ("Can't move from this position !!");
    me.wasted += Dice [0] + Dice [1] + Dice [2] + Dice [3];
  }
  else {
    PutMoveNumber (count);

    /* show move */
    for (j=0 ; j<=3 ; j++)
      if (list [j][0] != -1) {
	BlinkPiece (Board, list [j][0]);
	update (Board, list [j][0], list [j][1], 1);
	PutSpike (list [j][0], Board [list [j][0]]);

	if ((list [j][1] > 0) && (list [j][1] < 25))
	  BlinkPiece (Board, list [j][1]);

	if (list [j][0] == 0)
	  me.wasted += list [j][1];
      }
      else
	me.wasted += Dice [j];

    /* touch up the bars */
    PutSpike (0 , Board [ 0]);
    PutSpike (25, Board [25]);
  }
}


/*----------------------------------------------------------------------------*/
void 	FindMove ( BYTE 	Board [26], 
		   BYTE 	Dice  [4], 
		   int 		i1, 
		   int 		i2, 
		   int 		i3,
		   int 		i4)
/*----------------------------------------------------------------------------*/
{
  int j;


  if ((count<50) || (10 *(count/10) == count))
    PutMoveNumber (count);

  j = Eval (Board);

  if ((count == 1) || (j > max)) {
    max = j;
    MaxStat = GameStat;
    list [0][0] = i1; list [0][1] = i1 + Dice [0];
    list [1][0] = i2; list [1][1] = i2 + Dice [1];
    list [2][0] = i3; list [2][1] = i3 + Dice [2];
    list [3][0] = i4; list [3][1] = i4 + Dice [3];
  }
}


/*----------------------------------------------------------------------*/
void 	Find1 ( BYTE 	Board [26], 
		BYTE 	Dice  [4])
/*----------------------------------------------------------------------*/
{
  register int	i1,i2;
  BYTE		Board1 [26], 
		Dice2  [4];


  for (i2=0 ; i2<=1 ; i2++) {
    Dice2 [0] = Dice [i2];
    Dice2 [1] = 0;
    Dice2 [2] = 0;
    Dice2 [3] = 0;

    for (i1=0 ; i1<=24 ; i1++)
      if (Board [i1] > 0)
	if (valid (Board, i1, i1 + Dice [i2], Dice [i2])) {
	  count++;
	  CopyBoard (Board, Board1);
	  update (Board1, i1, i1+Dice [i2], 1);
	  FindMove (Board1, Dice2, i1, -1, -1, -1);
	}
  }
} /* end of find1 */


/*----------------------------------------------------------------------*/
void 	Find2 ( BYTE 	Board [26], 
		BYTE 	Dice  [4])
/*----------------------------------------------------------------------*/
{
  register int	i1, i2;
  BYTE		Board1 [26], 
		Board2 [26];


  for (i1=0 ; i1<=24 ; i1++) {
    if (Board [i1] > 0)
      if (valid (Board, i1, i1+Dice [0], Dice [0])) {
	CopyBoard (Board, Board1);
	update (Board1, i1, i1+Dice [0], 1);

	for (i2=0 ; i2<=24 ; i2++) {
	  if (Board1 [i2] > 0)
	    if (valid (Board1, i2, i2+Dice [1], Dice [1])) {
	      count++;
	      CopyBoard (Board1, Board2);
	      update (Board2, i2, i2+Dice [1], 1);
	      FindMove (Board2, Dice, i1, i2, -1, -1);
	    }	/* end valid Dice [1]	*/
	}	/* end for i2		*/
      }		/* end if Valid Dice[0]	*/
  }		/* end for i1		*/
}/* end find2 */


/*----------------------------------------------------------------------*/
void 	Find3 ( BYTE 	Board [26], 
		BYTE 	Dice  [4])
/*----------------------------------------------------------------------*/
{
  register int	i1, i2, i3;
  BYTE		Board1 [26], 
		Board2 [26], 
		Board3 [26];


  for (i1=0 ; i1<=24 ; i1++) {
    if (Board [i1] > 0)
      if (valid (Board, i1, i1+Dice [0], Dice [0])) {
	CopyBoard (Board, Board1);
	update (Board1, i1, i1+Dice [0], 1);

	for (i2=0 ; i2<=24 ; i2++) {
	  if (Board1 [i2] > 0)
	    if (valid (Board1, i2, i2+Dice [1], Dice [1])) {
	      CopyBoard (Board1, Board2);
	      update (Board2, i2, i2+Dice [1], 1);

	      for (i3=0 ; i3<=24 ; i3++) {
		if (Board2 [i3] > 0)
		  if (valid (Board2, i3, i3+Dice [2], Dice [2])) {
		    count++;
		    CopyBoard (Board2, Board3);
		    update (Board3, i3, i3+Dice [2], 1);
		    FindMove (Board3, Dice, i1, i2, i3, -1);
		  }	/* end valid Dice [2]	*/
	      }		/* end for i3		*/
	    }		/* end valid Dice [1]	*/
	}		/* end for i2		*/
      }			/* end if Valid Dice[0]	*/
  }			/* end for i1		*/
} /* end of Find3 */


/*----------------------------------------------------------------------*/
void 	Find4 ( BYTE 	Board [26], 
		BYTE 	Dice  [4])
/*----------------------------------------------------------------------*/
{
  register int	i1, i2, i3, i4;
  BYTE		Board1 [26], 
		Board2 [26], 
		Board3 [26], 
		Board4 [26];


  for (i1=0 ; i1 <= 24 ; i1++) {
    if (Board [i1] > 0)
      if (valid (Board, i1, i1+Dice [0], Dice [0])) {
	CopyBoard (Board, Board1);
	update (Board1, i1, i1+Dice [0], 1);

	for (i2=0 ; i2<=24 ; i2++) {
	  if (Board1 [i2] > 0)
	    if (valid (Board1, i2, i2+Dice [1],Dice [1])) {
	      CopyBoard (Board1, Board2);
	      update (Board2, i2, i2+Dice [1], 1);

	      for (i3=0 ; i3<=24 ; i3++) {
		if (Board2 [i3] > 0)
		  if (valid (Board2, i3, i3+Dice [2],Dice [2])) {
		    CopyBoard (Board2, Board3);
		    update (Board3, i3, i3+Dice [2], 1);

		    for (i4=0 ; i4<=24 ; i4++) {
		      if (Board3 [i4] > 0)
		        if (valid (Board3, i4, i4+Dice [3], Dice [3])) {
			  count++;
			  CopyBoard (Board3, Board4);
			  update (Board4, i4, i4+Dice [3], 1);
			  FindMove (Board4, Dice, i1, i2, i3, i4);
			}
		    }	/* end for i4		*/
		  }	/* end valid Dice[2]	*/
	      }		/* end for i3		*/
	    }		/* end valid Dice[1]	*/
        }		/* end for i2		*/
      }			/* end if Valid Dice[0]	*/
  }			/* end for i1		*/
} /* end of Find4 */


/*----------------------------------------------------------------------*/
void 	GenerateMoves ( BYTE 	Board [26], 
			BYTE 	Dice  [4])
/*----------------------------------------------------------------------*/
{
  BYTE 	Dice2 [4];

  count = 0;
  PutMoveNumber (count);

  /* check for doubles rolled */
  if (Dice [2] != 0) {
    Find4 (Board, Dice);

    if (count != 0)
      return;

    me.incomplete++;
    Find3 (Board, Dice);

    if (count != 0)
      return;

    Find2 (Board, Dice);

    if (count != 0)
      return;
  }
  else {
    Find2 (Board, Dice);
    Dice2 [0] = Dice [1];
    Dice2 [1] = Dice [0];
    Dice2 [2] = Dice [2];
    Dice2 [3] = Dice [3];
    Find2 (Board, Dice2);

    if (count != 0)
      return;
    me.incomplete++;
  }

  Find1 (Board, Dice);

} /* end of generate */

/*
 *  
 mosaic.c
 *  X version by kirk johnson  october 1990
 *  Amiga version by Loren J. Rittle  Sun Feb 24 06:17:30 1991
 *  Aris, this ones for you...
 *        ...when do I get my pizza :-)
 *
 *  It only took 4-5 hours to port from X, I did it on a dare!
 *  No Copyright N© 1991 Loren J. Rittle.  No rights reserved.
 *  Some code generated via PowerWindows 2.5.
 *  I also used their PD event shell as a starting point.
 *  
 *  Original UseNet post header:
 *  Path: news.larc.nasa.gov!elroy.jpl.nasa.gov!usc!cs.utexas.edu!sun-barr!newstop!exodus!kanchenjunga.LCS.MIT.EDU
 *  From: tuna@kanchenjunga.LCS.MIT.EDU (Kirk 'UhOh' Johnson)
 *  Newsgroups: comp.sources.x
 *  Subject: v11i083: mosaic, Part01/01
 *  Message-ID: <8101@exodus.Eng.Sun.COM>
 *  Date: 17 Feb 91 19:12:47 GMT
 *  Sender: news@exodus.Eng.Sun.COM
 *  Lines: 2409
 *  Approved: argv@sun.com
 *  
 *  Submitted-by: tuna@kanchenjunga.LCS.MIT.EDU (Kirk 'UhOh' Johnson)
 *  Posting-number: Volume 11, Issue 83
 *  Archive-name: mosaic/part01
 *
 *  Thanks to Kirk for releasing such a nice simple game program with source...
 *  Hard Drive Installable and Multitasks, thus better than any psygnosis game...
 */

#include <aros/oldprograms.h>
#include <exec/types.h>
#include <exec/io.h>
#include <exec/memory.h>
#include <libraries/dos.h>
#include <intuition/intuition.h>
#include <graphics/gfxmacros.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "mosaic.h"

struct IntuitionBase *IntuitionBase = NULL;
struct GfxBase *GfxBase = NULL;

SHORT BorderVectors1[] =
{
  0, 0,
  65, 0,
  65, 25,
  0, 25,
  0, 0
};

struct Border Border1 =
{
  -1, -1,
  3, 0, JAM1,
  5,
  BorderVectors1,
  NULL
};

struct IntuiText IText1 =
{
  3, 0, JAM2,
  5, 7,
  NULL,
  "Restart",
  NULL
};

struct Gadget Gadget1 =
{
  NULL,
  366, 107,
  64, 24,
  NULL,
  RELVERIFY,
  BOOLGADGET,
  (APTR) & Border1,
  NULL,
  &IText1,
  NULL,
  NULL,
  NULL,
  NULL
};

struct IntuiText IText2 =
{
  3, 0, JAM2,
  366, 138,
  NULL,
  "Next:",
  NULL
};

struct NewWindow NewWindowStructure1 =
{
  20, 10,
  440, 188,
  0, 1,
  MOUSEBUTTONS + MOUSEMOVE + GADGETUP + CLOSEWINDOW + RAWKEY,
  WINDOWDRAG + WINDOWDEPTH + REPORTMOUSE + WINDOWCLOSE + ACTIVATE + NOCAREREFRESH,
  &Gadget1,
  NULL,
  "Mosaic, Amiga Version by Loren J. Rittle\0Sun Feb 24 04:36:18 1991",
  NULL,
  NULL,
  450, 188,
  450, 188,
  WBENCHSCREEN
};

struct NewWindow NewWindowStructure2 =
{
  465, 10,
  170, 120,
  0, 1,
  NULL,
  WINDOWDRAG + WINDOWDEPTH + NOCAREREFRESH,
  NULL,
  NULL,
  "Mosaic Score",
  NULL,
  NULL,
  450, 188,
  450, 188,
  WBENCHSCREEN
};

char *ScoreFile = "mosaic.scores";

Word tile[NTiles];		/* the board */
Word piece[NPieces];		/* the "deck" of pieces */
Word nextpiece;			/* index into the deck */

Word size[NTiles];		/* score data structures */
Word parent[NTiles];

Word tscore[3];			/* total score */
Word pscore[3];			/* last piece score */
Word remain[3];			/* tiles remaining */

static char buf[256];

NameAndScore highscore[NHighScores];

int highlitX = 0;
int highlitY = 0;

int highlitXonDown;
int highlitYonDown;

BPTR SpeakFH = NULL;

struct Window *wG = NULL;
struct RastPort *rpG;

struct Window *wS = NULL;
struct RastPort *rpS;

#define BoundVarVal(var, min, max) \
{                                  \
  if ((var) < (min))               \
    (var) = (min);                 \
  else if ((var) > (max))          \
    (var) = (max);                 \
}

UWORD  backfill[8] =
{0x1111, 0x4444, 0x1111, 0x4444, 0x1111, 0x4444, 0x1111, 0x4444};

UWORD  solid[8] =
{0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};

void
main (void)
{
  UWORD code;
  ULONG class;
  APTR object;
  int MouseX, MouseY;
  int x, y;

  struct IntuiMessage *message;

  IntuitionBase = (struct IntuitionBase *)OpenLibrary ("intuition.library", 0);
  GfxBase = (struct GfxBase *)OpenLibrary ("graphics.library", 0);
  if (!IntuitionBase || !GfxBase)
    fatal ("Can't open Intuition and/or Graphics Library.");

  wG = OpenWindow (&NewWindowStructure1);
  if (wG == NULL)
    fatal ("Can't open a window.");
  rpG = wG->RPort;

  wS = OpenWindow (&NewWindowStructure2);
  if (wS == NULL)
    fatal ("Can't open a window.");
  rpS = wS->RPort;

  {
    struct Process* proc = (struct Process *)FindTask(0L);
    /* APTR temp = proc->pr_WindowPtr; */

    /* proc->pr_WindowPtr = (APTR)-1L; */
    SpeakFH = Open ("speak:", MODE_NEWFILE);
    /* proc->pr_WindowPtr = temp; */
  }


  PrintIText (rpG, &IText2, 0, 0);

  ReadHighScores ();

restart:
  InitGame ();
  drawScore ();
  SetAPen (rpG, 2);
  SetAfPt (rpG, (void *) backfill, 3);
  RectFill (rpG, 20, 14, 24 * 14 + 20, 24 * 7 + 14);

  SetAPen (rpG, 3);
  for (x = 0; x < 25; x++)
    {
      Move (rpG, x * 14 + 20, 14);
      Draw (rpG, x * 14 + 20, 24 * 7 + 14);
    }

  for (y = 0; y < 25; y++)
    {
      Move (rpG, 20, y * 7 + 14);
      Draw (rpG, 24 * 14 + 20, y * 7 + 14);
    }

  while (1)
    {
      WaitPort (wG->UserPort);
      while ((message = (struct IntuiMessage *) GetMsg (wG->UserPort)) != NULL)
	{
	  code = message->Code;
	  object = message->IAddress;
	  class = message->Class;
	  MouseX = message->MouseX;
	  MouseY = message->MouseY;
	  ReplyMsg ((struct Message *) message);
	  if (class == CLOSEWINDOW)
	    QuitGame (0);
	  /*
           * This is very kludgy, don't follow this example of how to read
           * keys... This is a quick hack to allow the 'a' for auto play
           * feature to work.  Try it, you may hate it! Because, the
           * AutoPlay() function is not too smart.  LJR
           */
	  if ((class == RAWKEY) && (code == 32))
	    AutoPlay ();
	  /* Here again, quick hack to support open/closing of
	   * score window via 's'.  LJR
	   */
	  if ((class == RAWKEY) && (code == 33))
	    {
	      if (wS == NULL)
		{
	          wS = OpenWindow (&NewWindowStructure2);
	          if (wS == NULL)
		    fatal ("Can't open a window.");
	          rpS = wS->RPort;
		  drawScore ();
		}
	      else
		{
		  CloseWindow(wS);
		  wS = NULL;
		}
	    }
	  if (class == MOUSEMOVE)
	    MoveBox (MouseX, MouseY);
	  if (class == MOUSEBUTTONS)
	    if (code & 0x0080)
	      {
		if ((highlitXonDown == highlitX) &&
		    (highlitYonDown == highlitY))
		  {
		    if ((nextpiece < NPieces) &&
			(DropPiece (highlitY, highlitX, piece[nextpiece])))
		      {
			nextpiece += 1;
			drawNext ();

			if (nextpiece == NPieces)
			  {
			    MoveBox (-1, -1);
			    CheckHighScore ();
			  }
		      }
		    else
		      if (SpeakFH)
			Write (SpeakFH, "No Way!", 7);
		      else
			/* DisplayBeep (NULL) */;
		  }
		else
		  if (SpeakFH)
		    Write (SpeakFH, "Un-do!", 7);
		  else
		    /* DisplayBeep (NULL) */;
	      }
	    else
	      {
		highlitXonDown = highlitX;
		highlitYonDown = highlitY;
	      }
	  if ((class == GADGETUP) || (class == GADGETDOWN))
	    if (object == (void *) &Gadget1)
	      goto restart;
	}
    }

}

void
MoveBox (int MouseX, int MouseY)
{
  int x, y;
  int newX = (MouseX - 27) / 14;
  int newY = (MouseY - 17) / 7;

  BoundVarVal (newX, 0, 22);
  BoundVarVal (newY, 0, 22);

  if ((newX == highlitX) && (newY == highlitY))
    return;

  SetAPen (rpG, 3);
  for (x = highlitX; x < highlitX + 3; x++)
    {
      Move (rpG, x * 14 + 20, highlitY * 7 + 14);
      Draw (rpG, x * 14 + 20, highlitY * 7 + 14 + 14);
    }

  for (y = highlitY; y < highlitY + 3; y++)
    {
      Move (rpG, highlitX * 14 + 20, y * 7 + 14);
      Draw (rpG, highlitX * 14 + 20 + 28, y * 7 + 14);
    }

  SetAfPt (rpG, (void *) backfill, 3);
  SetAPen (rpG, 2);
  if (!tile[highlitY * BoardSize + highlitX])
    RectFill (rpG, highlitX * 14 + 21, highlitY * 7 + 15, (highlitX+1) * 14 + 19, (highlitY+1) * 7 + 13);
  if (!tile[highlitY * BoardSize + highlitX + 1])
    RectFill (rpG, (highlitX+1) * 14 + 21, highlitY * 7 + 15, (highlitX+2) * 14 + 19, (highlitY+1) * 7 + 13);
  if (!tile[(highlitY + 1) * BoardSize + highlitX])
    RectFill (rpG, highlitX * 14 + 21, (highlitY+1) * 7 + 15, (highlitX+1) * 14 + 19, (highlitY+2) * 7 + 13);
  if (!tile[(highlitY + 1) * BoardSize + highlitX + 1])
    RectFill (rpG, (highlitX+1) * 14 + 21, (highlitY+1) * 7 + 15, (highlitX+2) * 14 + 19, (highlitY+2) * 7 + 13);

  if (nextpiece == NPieces)
    {
      highlitX = highlitY = 0;
      return;
    }
  highlitX = newX;
  highlitY = newY;

  SetAPen (rpG, 1);
  for (x = highlitX; x < highlitX + 3; x++)
    {
      Move (rpG, x * 14 + 20, highlitY * 7 + 14);
      Draw (rpG, x * 14 + 20, highlitY * 7 + 14 + 14);
    }

  for (y = highlitY; y < highlitY + 3; y++)
    {
      Move (rpG, highlitX * 14 + 20, y * 7 + 14);
      Draw (rpG, highlitX * 14 + 20 + 28, y * 7 + 14);
    }

  SetAfPt (rpG, (void *) solid, 3);
  if (!tile[highlitY * BoardSize + highlitX])
    {
      SetAPen (rpG, piece[nextpiece] & 0x0003);
      RectFill (rpG, highlitX * 14 + 21, highlitY * 7 + 15, (highlitX+1) * 14 + 19, (highlitY+1) * 7 + 13);
    }
  if (!tile[highlitY * BoardSize + highlitX + 1])
    {
      SetAPen (rpG, (piece[nextpiece] & 0x000C)>>2);
      RectFill (rpG, (highlitX+1) * 14 + 21, highlitY * 7 + 15, (highlitX+2) * 14 + 19, (highlitY+1) * 7 + 13);
    }
  if (!tile[(highlitY + 1) * BoardSize + highlitX])
    {
      SetAPen (rpG, (piece[nextpiece] & 0x0030)>>4);
      RectFill (rpG, highlitX * 14 + 21, (highlitY+1) * 7 + 15, (highlitX+1) * 14 + 19, (highlitY+2) * 7 + 13);
    }
  if (!tile[(highlitY + 1) * BoardSize + highlitX + 1])
    {
      SetAPen (rpG, (piece[nextpiece] & 0x00C0)>>6);
      RectFill (rpG, (highlitX+1) * 14 + 21, (highlitY+1) * 7 + 15, (highlitX+2) * 14 + 19, (highlitY+2) * 7 + 13);
    }
}

void
QuitGame (int rc)
{
  if (wG)
    CloseWindow (wG);
  if (wS)
    CloseWindow (wS);
  if (SpeakFH)
    Close (SpeakFH);
  if (GfxBase != NULL)
    CloseLibrary ((struct Library *)GfxBase);
  if (IntuitionBase != NULL)
    CloseLibrary ((struct Library *)IntuitionBase);
  exit (0);
}

void
warning (char *msg)
{
  fflush (stdout);
  fprintf (stderr, "%s: warning! %s\n", AppName, msg);
  fflush (stderr);
}

void
fatal (char *msg)
{
  fflush (stdout);
  fprintf (stderr, "%s: %s\n", AppName, msg);
  QuitGame (1);
}

void
ReadHighScores (void)
{
  int i;
  FILE *s;

  s = fopen (ScoreFile, "r");
  if (s == NULL)
    {
      warning ("unable to open score file; creating new one");

      for (i = 0; i < NHighScores; i++)
	{
	  strcpy (highscore[i].uname, ".");
	  highscore[i].score = -1;
	}

      WriteHighScores ();
    }
  else
    {
      for (i = 0; i < NHighScores; i++)
	{
	  fgets (buf, 30, s);
	  if (sscanf (buf, "%s %d",
		      highscore[i].uname, &highscore[i].score) != 2)
	    fatal ("incomplete score file read");
	}
      fclose (s);
    }
}


void
WriteHighScores (void)
{
  int i;
  FILE *s;

  s = fopen (ScoreFile, "w");
  if (s == NULL)
    fatal ("unable to open score file");

  for (i = 0; i < NHighScores; i++)
    fprintf (s, "%s %d\n", highscore[i].uname, highscore[i].score);

  fclose (s);
}

void
CheckHighScore (void)
{
  int i;
  int score;
  char *uname;

  uname = NULL /* getenv ("USERNAME") */;
  if (uname == NULL)
    uname = "Unknown_Puzzle_Solver";
  score = tscore[0] + tscore[1] + tscore[2];

  /*
   * note that we don't actually try to do any locking of the high score
   * file during this critical section ...
   */

  ReadHighScores ();

  for (i = 0; i < NHighScores; i++)
    if (strcmp (highscore[i].uname, uname) == 0)
      break;

  if (i == NHighScores)
    i = NHighScores - 1;

  if (score > highscore[i].score)
    {
      while ((i > 0) && (score > highscore[i - 1].score))
	{
	  strcpy (highscore[i].uname, highscore[i - 1].uname);
	  highscore[i].score = highscore[i - 1].score;
	  i -= 1;
	}
      strcpy (highscore[i].uname, uname);
      highscore[i].score = score;

      WriteHighScores ();

      if (SpeakFH)
	{
	  Write (SpeakFH, "Congratulations!", 16);
	  Write (SpeakFH, "You got a hi score.", 19);
	}
      else
	/* DisplayBeep (NULL) */;
    }
  drawHighScores ();
}

void
AutoPlay (void)
{
  int r, c;

  while (nextpiece < NPieces)
    {
      do
	{
	  r = rand () % (BoardSize - 1);
	  c = rand () % (BoardSize - 1);
	}
      while (!DropPiece (r, c, piece[nextpiece]));

      nextpiece += 1;
      drawNext ();

      if (nextpiece == NPieces)
	{
	  MoveBox (-1, -1);
	  CheckHighScore ();
	}
    }
}

void
UpdateAndScore (int r, int c, Word score[])
{
  int i;

  i = r * BoardSize + c;

  PossiblyMerge (i, i + 1);
  PossiblyMerge (i + BoardSize, i + BoardSize + 1);

  PossiblyMerge (i, i + BoardSize);
  PossiblyMerge (i + 1, i + BoardSize + 1);

  if (c >= 1)
    {
      PossiblyMerge (i, i - 1);
      PossiblyMerge (i + BoardSize, i + BoardSize - 1);
    }
  if (r >= 1)
    {
      PossiblyMerge (i, i - BoardSize);
      PossiblyMerge (i + 1, i - BoardSize + 1);
    }
  if (c <= (BoardSize - 3))
    {
      PossiblyMerge (i + 1, i + 2);
      PossiblyMerge (i + BoardSize + 1, i + BoardSize + 2);
    }
  if (r <= (BoardSize - 3))
    {
      PossiblyMerge (i + BoardSize, i + (2 * BoardSize));
      PossiblyMerge (i + BoardSize + 1, i + (2 * BoardSize) + 1);
    }
  /* compute the new score */
  for (i = 0; i < 3; i++)
    score[i] = 0;
  for (i = 0; i < NTiles; i++)
    if ((tile[i] != 0) && (parent[i] == i))
      score[tile[i] - 1] += size[i] * size[i];
}

void
PossiblyMerge (int i, int j)
{
  Word irep;
  Word jrep;
  Word scan;

  /* tiles are not the same color */
  if (tile[i] != tile[j])
    return;

  /* find i's rep */
  irep = i;
  while (parent[irep] != irep)
    irep = parent[irep];

  /* compress path from i to irep */
  scan = i;
  while (parent[scan] != scan)
    {
      scan = parent[scan];
      parent[scan] = irep;
    }

  /* find j's rep */
  jrep = j;
  while (parent[jrep] != jrep)
    jrep = parent[jrep];

  /* compress path from j to jrep */
  scan = j;
  while (parent[scan] != scan)
    {
      scan = parent[scan];
      parent[scan] = jrep;
    }

  /* tiles are already in the same set */
  if (irep == jrep)
    return;

  /* merge the sets */
  if (size[irep] > size[jrep])
    {
      parent[jrep] = irep;
      size[irep] += size[jrep];
    }
  else
    {
      parent[irep] = jrep;
      size[jrep] += size[irep];
    }
}

int
DropPiece (int r, int c, Word p)
{
  int idx;
  Word type;
  Word nscore[3];

  idx = r * BoardSize + c;

  /* check for illegal move */
  if ((tile[idx] != 0) ||
      (tile[idx + 1] != 0) ||
      (tile[idx + BoardSize] != 0) ||
      (tile[idx + BoardSize + 1] != 0))
    return 0;

  /* place the piece */
  type = p & 0x03;
  tile[idx] = type;
  remain[type - 1] -= 1;
  p >>= 2;

  type = p & 0x03;
  tile[idx + 1] = type;
  remain[type - 1] -= 1;
  p >>= 2;

  type = p & 0x03;
  tile[idx + BoardSize] = type;
  remain[type - 1] -= 1;
  p >>= 2;

  type = p & 0x03;
  tile[idx + BoardSize + 1] = p & 0x03;
  remain[type - 1] -= 1;

  /* update the score */
  UpdateAndScore (r, c, nscore);
  for (idx = 0; idx < 3; idx++)
    {
      pscore[idx] = nscore[idx] - tscore[idx];
      tscore[idx] = nscore[idx];
    }

  /* redraw */
  drawTile (r++, c);
  drawTile (r, c++);
  drawTile (r--, c);
  drawTile (r, c);
  drawScore ();

  return 1;
}

void
InitGame (void)
{
  int i, j, k, l;
  int idx, swap;

  /* randomize */
  srand ((int) time (NULL));

  /* clear the board */
  for (i = 0; i < NTiles; i++)
    tile[i] = 0;

  /* set up deck */
  idx = 0;
  for (i = 1; i <= 3; i++)
    for (j = 1; j <= 3; j++)
      for (k = 1; k <= 3; k++)
	for (l = 1; l <= 3; l++)
	  piece[idx++] = (i << 6) | (j << 4) | (k << 2) | (l << 0);

  /* shuffle */
  for (i = 0; i < 1000; i++)
    {
      idx = rand () % NPieces;
      swap = piece[idx];
      piece[idx] = piece[0];
      piece[0] = swap;
    }
  nextpiece = 0;

  /* clear score data structures */
  for (i = 0; i < NTiles; i++)
    {
      size[i] = 1;
      parent[i] = i;
    }

  for (i = 0; i < 3; i++)
    {
      tscore[i] = 0;
      pscore[i] = 0;
      remain[i] = (NPieces * 4) / 3;
    }
  drawNext ();
}

void
drawTile (int r, int c)
{
  int x, y;

  x = c * 14 + 20;
  y = r * 7 + 14;

  SetAfPt (rpG, (void *) solid, 3);
  SetAPen (rpG, tile[r * BoardSize + c]);
  RectFill (rpG, x + 1, y + 1, x + 13, y + 6);
}

void
drawHighScores (void)
{
  int i;

  SetAPen (rpG, 1);
  for (i = 0; i < NHighScores; i++)
    if (highscore[i].score > 0)
      {
	sprintf (buf, "%-24s %5d", highscore[i].uname, highscore[i].score);
	Move (rpG, 50, 30 + i * 14);
	Text (rpG, buf, strlen (buf));
      }
}

void
drawNext (void)
{
  int p = (nextpiece < NPieces) ? piece[nextpiece] : 0;
  int x = 26 * 14 + 20;
  int y = 20 * 7 + 14;

  SetAfPt (rpG, (void *) solid, 3);
  SetAPen (rpG, p & 0x03);
  RectFill (rpG, x + 1, y + 1, x + 13, y + 6);
  p >>= 2;
  SetAPen (rpG, p & 0x03);
  RectFill (rpG, x + 15, y + 1, x + 27, y + 6);
  p >>= 2;
  SetAPen (rpG, p & 0x03);
  RectFill (rpG, x + 1, y + 8, x + 13, y + 13);
  p >>= 2;
  SetAPen (rpG, p & 0x03);
  RectFill (rpG, x + 15, y + 8, x + 27, y + 13);
}

void
drawScore (void)
{
  int i, total;

  if (!wS)
    return;

  SetAPen(rpS, 3);
  Move (rpS, 10, 20);
  Text (rpS, "to play:", 8);

  Move (rpS, 10, 30);
  for (i = 0; i < 3; i++)
    {
      SetAPen(rpS, i+1);
      sprintf (buf, "%5d ", remain[i]);
      Text (rpS, buf, strlen (buf));
    }

  SetAPen(rpS, 3);
  Move (rpS, 10, 50);
  Text (rpS, "total:", 6);

  total = 0;
  Move (rpS, 10, 60);
  for (i = 0; i < 3; i++)
    {
      SetAPen(rpS, i+1);
      total += tscore[i];
      sprintf (buf, "%5d ", tscore[i]);
      Text (rpS, buf, strlen (buf));
    }

  SetAPen(rpS, 3);
  sprintf (buf, "%5d", total);
  Move (rpS, 10, 70);
  Text (rpS, buf, strlen (buf));

  Move (rpS, 10, 90);
  Text (rpS, "piece:", 6);

  total = 0;
  Move (rpS, 10, 100);
  for (i = 0; i < 3; i++)
    {
      SetAPen(rpS, i+1);
      total += pscore[i];
      sprintf (buf, "%5d ", pscore[i]);
      Text (rpS, buf, strlen (buf));
    }

  SetAPen(rpS, 3);
  sprintf (buf, "%5d", total);
  Move (rpS, 10, 110);
  Text (rpS, buf, strlen (buf));
}

/*
 * mosaic.h
 * kirk johnson
 * october 1990
 */

#ifndef _MOSAIC_H_
#define _MOSAIC_H_

#include <stdio.h>

/* #define DEBUG */

#define AppName   "mosaic"

#define BoardSize (24)		/* width, in tiles */
#define NPieces   (81)		/* # pieces per game */

#define NTiles (BoardSize*BoardSize)

#define MaxUnameLen  (30)	/* eight chars per uname, max */
#define MaxScoreLen  (5)	/* five chars per score, max */
#define NHighScores  (10)	/* # of high scores kept */
#define NHelpLines   (10)	/* # of lines in help window */

#define MaxPossibleScore  "34992"


typedef unsigned short Word;

typedef struct
{
  char uname[MaxUnameLen+1];	/* username */
  int  score;			/* score */
} NameAndScore;

void MoveBox (int MouseX, int MouseY);
void main (void);
void QuitGame(int rc);
void warning(char *msg);
void fatal(char *msg);
void ReadHighScores(void);
void WriteHighScores(void);
void CheckHighScore(void);
void AutoPlay(void);
void UpdateAndScore(int r, int c, Word score[]);
void PossiblyMerge(int i, int j);
int DropPiece(int r, int c, Word p);
void InitGame(void);
void drawTile(int r, int c);
void drawHighScores(void);
void drawNext(void);
void drawScore(void);

extern Word tile[NTiles];	/* the board */
extern Word piece[NPieces];	/* the "deck" of pieces */
extern Word nextpiece;		/* index into the deck */

extern Word tscore[3];		/* total score */
extern Word pscore[3];		/* last piece score */
extern Word remain[3];		/* tiles remaining */

extern NameAndScore highscore[NHighScores];

#endif /* _MOSAIC_H_ */

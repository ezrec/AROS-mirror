#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "book.h"

#define PLAYERFILE "players.dat"

int totalplayers = 0;

#define MAXPLAYERS 500

typedef struct {
  char player[100];
  int wins;
  int losses;
  int draws;
} playerentry;

playerentry playerdb[MAXPLAYERS];
static char lname[100];

static int rscorecompare(const void *aa, const void *bb)
{
    const playerentry *a = (const playerentry *)aa;
    const playerentry *b = (const playerentry *)bb;   
    float ascore, bscore;
    ascore = (a->wins+(a->draws/2))/(a->wins+a->draws+a->losses);
    bscore = (b->wins+(b->draws/2))/(b->wins+b->draws+b->losses);
    if (ascore > bscore) return(-1);
    else if (bscore > ascore) return(1);
    else return(0);
}

static int scorecompare(const void *aa, const void *bb)
{
    const playerentry *a = (const playerentry *)aa;
    const playerentry *b = (const playerentry *)bb;   
    int ascore, bscore;
    ascore = 100*(a->wins+(a->draws/2))/(a->wins+a->draws+a->losses);
    bscore = 100*(b->wins+(b->draws/2))/(b->wins+b->draws+b->losses);
    if (bscore > ascore) return(1);
    else if (bscore < ascore) return(-1);
    else return(0);
}

static int namecompare(const void *aa, const void *bb)
{
    const playerentry *a = (const playerentry *)aa;
    const playerentry *b = (const playerentry *)bb;   
    if (strcmp(a->player,b->player) > 0) return(1);
    else if (strcmp(a->player,b->player) < 0) return(-1);
    else return(0);
}

void DBSortPlayer (const char *style)
{
  if (strncmp(style,"score",5) == 0) {
    qsort(&playerdb,totalplayers,sizeof(playerentry),scorecompare);
  } else if (strncmp(style,"name",4) == 0) {
    qsort(&playerdb,totalplayers,sizeof(playerentry),namecompare);
  } else if (strncmp(style,"reverse",7) == 0) {
    qsort(&playerdb,totalplayers,sizeof(playerentry),rscorecompare);
  }
}

void DBListPlayer (const char *style)
{
  int i;
	
  DBReadPlayer ();
  DBSortPlayer (style);
  for (i = 0; i < totalplayers; i++) {
    printf("%s %2.0f%% %d %d %d\n",
	playerdb[i].player,
	100.0*(playerdb[i].wins+((float)playerdb[i].draws/2))/
	 (playerdb[i].wins+playerdb[i].draws+playerdb[i].losses),
	playerdb[i].wins,
	playerdb[i].losses,
	playerdb[i].draws);
    if ((i+1) % 10 == 0) {printf("[Type a character to continue.]\n"); getchar();}
  }
}

void DBWritePlayer (void)
{
   int i;
   FILE *wfp;
   DBSortPlayer ("reverse");
   if ((wfp = fopen(PLAYERFILE,"w")) != NULL) {
     for (i = 0; i < totalplayers; i++) {
        fprintf(wfp,"%s %d %d %d\n",
	  playerdb[i].player,
          playerdb[i].wins,
          playerdb[i].losses,
	  playerdb[i].draws);
     }
   }
   fclose(wfp);
}

void DBReadPlayer (void)
{
   FILE *rfp;
   int n;
   totalplayers = 0;
   if ((rfp = fopen(PLAYERFILE,"r")) != NULL) {
    while (!feof(rfp)) {
     n = fscanf(rfp,"%s %d %d %d\n",
	playerdb[totalplayers].player,
        &playerdb[totalplayers].wins,
        &playerdb[totalplayers].losses,
        &playerdb[totalplayers].draws);
     if (n == 4) totalplayers++;
    }
    fclose(rfp);
   }
}

int DBSearchPlayer (const char *player)
{
  int index = -1;
  int i;

  for (i = 0; i < totalplayers; i++)
    if (strncmp(playerdb[i].player,player,strlen(playerdb[i].player)) == 0)
    {
      index = i;
      break;
    }
  return (index);
}

void DBUpdatePlayer (const char *player, const char *resultstr)
{
  const char *p;
  char *x;
  int index;
  int result = R_NORESULT;

  memset(lname,0,sizeof(lname));
  p = player;
  x = lname;
  strcpy(lname,player);
  do {
    if (*p != ' ') 
      *x++ = *p++;
    else
	p++;
  } while (*p != '\0');
  *x = '\000';
  memset(playerdb,0,sizeof(playerdb[MAXPLAYERS]));
  DBReadPlayer ();
  index = DBSearchPlayer (lname);
  if (index == -1) {
    strcpy(playerdb[totalplayers].player,lname);
    playerdb[totalplayers].wins = 0;
    playerdb[totalplayers].losses = 0;
    playerdb[totalplayers].draws = 0;
    index = totalplayers;
    totalplayers++;
  }
  if (strncmp(resultstr,"1-0",3) == 0)
     result = R_WHITE_WINS;
  else if (strncmp(resultstr,"0-1",3) == 0)
     result = R_BLACK_WINS;
  else if (strncmp(resultstr,"1/2-1/2",7) == 0)
     result = R_DRAW;

  if ((computerplays == white && result == R_WHITE_WINS)||
      (computerplays == black && result == R_BLACK_WINS))
    playerdb[index].wins++;
  else if ((computerplays == white && result == R_BLACK_WINS)||
      (computerplays == black && result == R_WHITE_WINS))
    playerdb[index].losses++;
  else
    /* Shouln't one check for draw here? Broken PGN files surely exist */
    playerdb[index].draws++;
  DBWritePlayer ();
}

/********************************************************************/
/*                                                                  */
/*  Hoser BackGammon version 1.0                                    */
/*                                                                  */
/*      Robert Pfister                                              */
/*                                                                  */
/*      Rfd#3 Box 2340                home:(207)-873-3520           */
/*      Waterville, Maine 04901                                     */
/*                                                                  */
/*      Pfister_rob%dneast@dec.decwrl                               */
/*                                                                  */
/*                                                                  */
/*  Copyright  June,1987 all rights reserved.                       */
/*                                                                  */
/*  This program will play a game of backgammon at the novice level */
/*                                                                  */
/*  The code is in 4 parts...                                       */
/*                                                                  */
/*       1) back.c     - main driver                                */
/*       2) eval.c     - evaluation of moves                        */
/*   /   3) backscn.c  - screen stuff..                             */
/* \/    4) backmenu.c - menu stuff, help text, and ``decoder''     */
/*                                                                  */
/*       this was compiled under SASC 6.56                          */
/*                                                                  */
/********************************************************************/

#include "exec/types.h"
#include "proto/intuition.h"

char *Help[] =
       {
       " Help \0",
       "                                                             \n",
       " To move  a piece, click on  it with the left  mouse button, \n",
       " then click on the  spike that  you want to move  the piece. \n",
       "                                                             \n",
       " The `bar' is the center strip of the board.                 \n",
       "                                                             \n",
       " If your move is invalid, an error message will be displayed \n",
       " in the menu strip.                                          \n",
       "                                                             \n",
       " To `cast off' pieces, double click on the piece desired.    \n",
       "                                                             \n",      
       " If you cannot move, click on your dice.                     \n",
       "                                                             \n",
       " The number  that appears below  the menu bar is  the number \n",
       " of the  position that's  being evaluated.  For doubles this \n",
       " number can reach around 5000, so be patient.                \n",
       "                                                             \n",
       " `Redo Move' returns board to the start of the move.         \n",
       " `Take Back' will back up one move.                          \n",
       "                                                             \n"
       };

int NumHelp=20;


char *credits[] = {
  " Credits \0",
  "                                                                  \n",
  " Hoser BackGammon           Copyright June 1987 by Robert Pfister \n",
  "                                                                  \n",
  " Written in Aztec 'C' 3.20 as an A.I. course project.             \n",
  " Uses home-brewed heuristics to play like I would.                \n",
  "                                                                  \n",
  " Snail:       {donations accepted..I suppose}                     \n",
  "              Robert Pfister                                      \n",
  "              Rfd#3 box 2340                                      \n",
  "              Waterville, Maine 04901   phone: 207-873-3520       \n",
  " E-Mail:      pfister_rob%dneast.dec@decwrl.dec.com               \n",
  "                                                                  \n",
  " Freely Distributable (if I'm credited with the original version) \n",
  "                                                                  \n",
  " Extensively revised by E.M.Greene      11-Apr-92                 \n",
  " Converted to SASC 6.2                  12-Feb-93                 \n",
  " Upgrades  w/ SASC 6.5                  18-Nov-94 & 1-Jul-95      \n",
  " Modifications by F.Bernard             16-May-96                 \n",
  "                                                                  \n"
  };

#define NumCredits	19


struct IntuiText IText[]=
 {
  {0,1,JAM1,CHECKWIDTH,0,NULL,"New Game "},	/*  0 */
  {0,1,JAM1,CHECKWIDTH,0,NULL,"About    "},	/*  1 */
  {0,1,JAM1,CHECKWIDTH,0,NULL,"Quit     "},	/*  2 */
  {0,1,JAM1,CHECKWIDTH,0,NULL,"Help     "},	/*  3 */
  {0,1,JAM1,CHECKWIDTH,0,NULL,"Back up  "},	/*  4 */
  {0,1,JAM1,CHECKWIDTH,0,NULL,"Redo Move"}	/*  5 */
  };

struct MenuItem menu_item[] = {
  { &menu_item[1],			  /* newgame 0: */
    0,0,133,12,(ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ),
    NULL, (APTR) &IText[0], NULL,'N',NULL},
  { &menu_item[2],			  /* about  1: */
    0,15,133,12,(ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ),
    NULL,(APTR) &IText[1],NULL,'I',NULL},
  { NULL,				  /* quit  2: */
    0,30,133,12,(ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ),
    NULL,(APTR) &IText[2],NULL,'Q',NULL},
  { &menu_item[4],			  /* help  3: */
    0,0,133,12,(ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ),
    NULL,(APTR) &IText[3],NULL,'H',NULL},
  { &menu_item[5],			  /* Takeback 4:  */
    0,15,133,12,(ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ),
    NULL,(APTR) &IText[4],NULL,'X',NULL},
  { NULL,				  /* move hint  5: */
    0,30,133,12,(ITEMTEXT|ITEMENABLED|HIGHCOMP|COMMSEQ),
    NULL,(APTR) &IText[5],NULL,'B',NULL}
};


struct Menu MyMenu[] = {
  { &MyMenu [1], 0, 0, 83, 0, MENUENABLED, "Project",  &menu_item [0]},
  { NULL,       80, 0, 80, 0, MENUENABLED, "Cheating", &menu_item [3]}};


/*----------------------------------------------------------------------*/
void	Restart    (void);
void	TextScreen (char **text, int lines);
void	RecallMove (int n);
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
int 	DoMenu (int x)
/*----------------------------------------------------------------------*/
{
  switch (x) {
    case  31: /* new game */
      Restart ();
      return (1);
      break;
    case  41: /* put up a window with the credits in it */
      TextScreen (credits, NumCredits);
      break;
    case  51: /* quit */
      return (-1);
      break;
    case 131: /* do it for help as well */
      TextScreen (Help, NumHelp);
      break;
    case 141: /* take back a full move */
      RecallMove (-1); 
      break;
    case 151:
      RecallMove (0);
      break;            
    default: break;
  }
  return (0);
}

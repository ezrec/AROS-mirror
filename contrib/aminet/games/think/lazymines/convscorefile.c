/*
 * convscorefile.c
 * ===============
 * Converts pre 3.0 to 3.0 highscorefile.
 *
 * Copyright (C) 1995-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <proto/dos.h>


char  *version = "$VER: convscorefile 1.0 (19.5.95)";


#define HSMAGIC   "LZM#30HS"
#define HSFNAME   "LazyMines.hiscore"
#define DEFNAME   "Lorens Younes"
#define NUM_SCORES   10


struct highscore {
   char    name[31];
   UWORD   score;
};

static struct highscore   hiscores[2][3][NUM_SCORES];
static char   fullname[256];
static BOOL   need_save = FALSE;


static void
default_scorers (void)
{
   int   i, j, k;
   
   
   for (i = 0; i < 2; ++i)
   {
      for (j = 0; j < 3; ++j)
      {
         for (k = 0; k < NUM_SCORES; ++k)
         {
            strcpy (hiscores[i][j][k].name, DEFNAME);
            hiscores[i][j][k].score = 999;
         }
      }
   }
}


void
load_highscores (void)
{
   register UWORD   i = 0, j = 0, k = 0, m;
   FILE  *score_file;
   int   ch;
   char   num_str[4];
   UBYTE   status = 0;
   
   
   if (score_file = fopen (fullname, "r"))
   {
      need_save = TRUE;
      while ((ch = fgetc (score_file)) != EOF && k < NUM_SCORES)
      {
         if (status == 0)
         {
            if (ch == '\n')
            {
               hiscores[0][j][k].name[i] = '\0';
               ++status;
               i = 0;
            }
            else
            {
               hiscores[0][j][k].name[i] = ch;
               ++i;
            }
         }
         else
         {
            if (ch == '\n')
            {
               num_str[i] = '\0';
               hiscores[0][j][k].score = atoi (num_str);
               ++status;
               if (status > 1)
               {
                  status = 0;
                  ++j;
                  if (j > 2)
                  {
                     j = 0;
                     ++k;
                  }
               }
               i = 0;
            }
            else
            {
               num_str[i] = ch;
               ++i;
            }
         }
      }
      fclose (score_file);
   }
   
   for (m = k; m < NUM_SCORES; ++m)
   {
      for (i = j; i < 3; ++i)
      {
         switch (status)
         {
         case 0:
            strncpy (hiscores[0][i][m].name, DEFNAME, 30);
         case 1:
            hiscores[0][i][m].score = 999;
            status = 0;
            break;
         }
      }
   }
}


void
save_highscores (void)
{
   BPTR   fh;
   int    i, j, k;
   
   
   if (need_save)
   {
      if (fh = Open (fullname, MODE_NEWFILE))
      {
         Write (fh, HSMAGIC, sizeof (HSMAGIC));
         for (i = 0; i < 2; ++i)
            for (j = 0; j < 3; ++j)
               for (k = 0; k < NUM_SCORES; ++k)
                  Write (fh, &hiscores[i][j][k], sizeof (hiscores[i][j][k]));
      
         Close (fh);
         SetProtection (fullname, 2);
      }
   }
}


int
main (
   int    argc,
   char  *argv[])
{
   if (argc < 2)
      strcpy (fullname, HSFNAME);
   else if (argc == 2)
   {
      strcpy (fullname, argv[1]);
      AddPart (fullname, HSFNAME, 255);
   }
   else
   {
      fprintf (stderr, "Too many arguments!\n");
      
      exit (1);
   }
   
   default_scorers ();
   
   load_highscores ();
   save_highscores ();
   
   return 0;
}

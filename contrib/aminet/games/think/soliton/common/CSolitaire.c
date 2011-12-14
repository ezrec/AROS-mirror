/* Soliton cardgames for Amiga computers
 * Copyright (C) 1997-2002 Kai Nickel and other authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <clib/alib_protos.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include "Cardgame.h"
#include "CButton.h"
#include "CSolitaire.h"
#include "Locales.h"
#include "Settings.h"
#include "Soliton.h"

struct CSolitaire_Data;

/* local functions */
static void Undo_RemHead(struct CSolitaire_Data *data);
static int Top(struct CSolitaire_Data *data, int p);
static void DisplayStats(struct CSolitaire_Data *data);
static BOOL dragStart(struct CSolitaire_Data *data, LONG source, LONG* anz);
static BOOL dragAccept(struct CSolitaire_Data *data, int source, int dest, int size, int* cards);
static void dragDone(struct CSolitaire_Data *data, int source, int dest, int size);
static void NewGame(struct CSolitaire_Data *data);
static void UpdateIndicator(struct CSolitaire_Data *data);

/****************************************************************************************
  Hilfsfunktionen
****************************************************************************************/

static int family(int nr)
{
  if (nr >= 2  && nr <= 14) return 0;
  if (nr >= 15 && nr <= 27) return 1;
  if (nr >= 28 && nr <= 40) return 2;
  if (nr >= 41 && nr <= 53) return 3;
  return 0;
}

static int isRed(int nr)
{
  return (nr >= 15 && nr <= 40); 
}

static int value(int nr)
{
  if (nr >= 2  && nr <= 14) return nr;
  if (nr >= 15 && nr <= 27) return nr - 13;
  if (nr >= 28 && nr <= 40) return nr - 26;
  if (nr >= 41 && nr <= 53) return nr - 39;
  return 0;
}

static int fits_temp(int drauf, int drunter)
{
  if (drunter == -1)
  {
    return value(drauf) == 13;
  }
  else
  {
    if (isRed(drauf) == isRed(drunter)) return FALSE;
    if (value(drunter) <= 13 && value(drunter) - 1 == value(drauf)) return TRUE;
    if (value(drunter) == 2 && value(drauf) == 14) return TRUE;
    return FALSE;
  }
}

static int fits_final(int drauf, int drunter)
{
  if(drunter == -1)
  {
    return (value(drauf) == 14);
  }
  else
  {
    if (family(drauf) != family(drunter)) return FALSE;
    return     drauf - 1 == drunter 
            || (value(drauf) == 2 && value(drunter) == 14);
  }
}

#define Cover(c)   if((c) < 100) (c) += 100;
#define UnCover(c) if((c) > 100) (c) -= 100;
#define isTemp(p)  ((p) >= 6 && (p) <= 12)
#define isFinal(p) ((p) >= 2 && (p) <= 5)
#define isTempFC(p) ((p) >= 8 && (p) <= 15)
#define isGoalFC(p) ((p) >= 4 && (p) <= 7)

/****************************************************************************************
  Stats
****************************************************************************************/

static void AddScore(struct Stats *stats, int p)
{
  if(-p > stats->game_score)
    p = -stats->game_score;
  stats->game_score += p;
  stats->all_score  += p;
}

static void ClearAll(struct Stats *stats)
{
  stats->game_score   = 0;
  stats->game_turns   = 0;
  stats->game_seconds = 0;
  stats->all_won      = 0;
  stats->all_lost     = 0;
  stats->all_score    = 0;
  stats->all_seconds  = 0;
}

/****************************************************************************************
  Data
****************************************************************************************/

struct UndoNode
{
  struct UndoNode *next;                  // Listenzeiger
  int       from, to;                     // die beiden Stapel
  int       size;                         // # transferierter Karten
  BOOL      uncover;     /* Klondike: card uncovered */
  int       flag;                         // für beliebige Info
};

struct CSolitaire_Data
{
  Object *         obj;                     //  MUI Object
  Object *         timer;
  struct Stats     stats;
  struct UndoNode *undohead;                // Zugliste

  /* Freecell */
  BOOL             sweeping;                // gerade am sweepen? (um rekursion zu vermeiden)
  BOOL             gamemode;

  /* Klondike */
  Object *         movebutton;
  Object *         score;
  long             last_malus;              // letzter Zeitpkt timerbedingten Pkteabzugs
  int              opencards[53];           //  Nicht sichtbare Karten auf
  int              opencardsSize;           //  Open-Stack
  BOOL             gameonline;
};

/****************************************************************************************
  Finished / TryMove
****************************************************************************************/

static BOOL Finished(struct CSolitaire_Data *data)
{
  int i, top;

  if(data->gamemode == GAMEMODE_FREECELL)
  {
    if(!data->stats.finished)
    {
      STRPTR a, b;

      for(i = 4; i < 8; i++)
      {
        top = Top(data, i);
        if(top == -1 || value(top) != 13)
          return FALSE;
      }
  
      data->stats.finished = TRUE;

      data->stats.all_won++;
      DisplayStats(data);

      while(xget(data->obj, MUIA_Cardgame_TimerRunning))
        setatt(data->obj, MUIA_Cardgame_TimerRunning, FALSE);

      /* outside of MUI_RequestA to fix SAS-C optimizer bug */
      a = GetStr(MSG_REQ_GADGETS);
      b = GetStr(MSG_REQ_NEWGAME);
      if(MUI_RequestA(_app(data->obj), _win(data->obj), 0, "Soliton",
      a, b, NULL) == 1)
      {
        NewGame(data);

        while(xget(data->obj, MUIA_Cardgame_TimerRunning))
          setatt(data->obj, MUIA_Cardgame_TimerRunning, FALSE);
      }
    }
  }
  else
  {
    if(!data->stats.finished)
    {
      for(i = 2; i <= 5; i++)
      {
        top = Top(data, i);
        if(top == -1 || value(top) != 13)
          return FALSE;
      }
    }

    data->stats.finished = TRUE;

    data->stats.all_won++;
    AddScore(&data->stats, 600 - data->stats.game_seconds); // Bonus
    DisplayStats(data);

    setatt(data->obj, MUIA_Cardgame_TimerRunning, FALSE);
    DoMethod((Object*)xget(data->obj, MUIA_ApplicationObject), MUIM_Soliton_Statistics, TRUE);
  }
  return TRUE;
}

static BOOL tryMove(struct CSolitaire_Data *data, int from, int to, int anz, BOOL execute)
{
  LONG a = anz;

  if(data->gamemode == GAMEMODE_FREECELL && from == to)
    return FALSE;
  if(dragStart(data, from, &a))
  {
    // nur wenn ohne Block-Änderung akzptiert bzw. auf final
    if(a == anz || (data->gamemode == GAMEMODE_KLONDIKE && isFinal(to)))
    {
      int size, cards[53];
      DoMethod(data->obj, MUIM_Cardgame_GetCards, from, cards, &size);
      if(dragAccept(data, from, to, anz, &cards[size - anz]))
      {
        if(execute)
        {
          DoMethod(data->obj, MUIM_Cardgame_MoveCards, from, to, anz);
          dragDone(data, from, to, anz);
        }
        return TRUE;
      }
    }
  }

  return FALSE;
}

/****************************************************************************************
  Top/ topFace
****************************************************************************************/

static int Top(struct CSolitaire_Data *data, int p)
{
  int size, cards[53];

  DoMethod(data->obj, MUIM_Cardgame_GetCards, p, cards, &size);
  return (size ? cards[size-1] : -1);
}

static BOOL topFace(struct CSolitaire_Data *data, int p, BOOL face)
{
  int size, cards[53];

  DoMethod(data->obj, MUIM_Cardgame_GetCards, p, cards, &size);
  if(size)
  {
    if(face && cards[size - 1] > 100)
    {
      UnCover(cards[size - 1]);
      DoMethod(data->obj, MUIM_Cardgame_SetCards, p, cards, size);
      return TRUE;
    }
    if(!face && cards[size - 1] < 100)
    {
      Cover(cards[size - 1]);
      DoMethod(data->obj, MUIM_Cardgame_SetCards, p, cards, size);
      return TRUE;
    }
  }
  return FALSE;
}

/****************************************************************************************
  NewGame
****************************************************************************************/

static void NewGame(struct CSolitaire_Data *data)
{
  int cards[53], i;
  struct Settings * s = (struct Settings*) xget((Object*)xget(data->obj, MUIA_ApplicationObject),
  MUIA_Soliton_Settings);

  if(data->gamemode == GAMEMODE_FREECELL)
  {
    int last;
    /* fill cards stacks and make them random ordered */

    for(i = 0; i < 52; i++)
      cards[i] = i + 2;

    srand(time(0));
    for(i = 1; i < 100; i++)
    {
      int z1,z2,tmp;

      z1  = rand()/(RAND_MAX/52);
      z2  = rand()/(RAND_MAX/52);
      tmp = cards[z1];
      cards[z1] = cards[z2];
      cards[z2] = tmp;
    }

    last = 52;
    for(i = 0; i < 8; i++)
    {
      if(s->stack > i) --last;
      DoMethod(data->obj, MUIM_Cardgame_SetCards, i, &cards[last], s->stack > i ? 1 : 0);
    }
    for(i = 8; i < 12-s->stack; i++)
    {
      last -= 7;
      DoMethod(data->obj, MUIM_Cardgame_SetCards, i, &cards[last], 7);
    }
    for(; i < 16; i++)
    {
      last -= 6;
      DoMethod(data->obj, MUIM_Cardgame_SetCards, i, &cards[last], 6);
    }
    data->sweeping = FALSE;
  }
  else
  {
    /* fill cards stacks and make them random ordered */
    for(i = 0; i < 52; i++)
      cards[i] = i + 102;

    srand(time(0));
    for(i = 1; i < 100; i++)
    {
      int z1,z2,tmp;

      z1  = rand()/(RAND_MAX/52);
      z2  = rand()/(RAND_MAX/52);
      tmp = cards[z1];
      cards[z1] = cards[z2];
      cards[z2] = tmp;
    }

    DoMethod(data->obj, MUIM_Cardgame_SetEmptyImage, 1, 0);
    DoMethod(data->obj, MUIM_Cardgame_SetCards,  0, cards, 24);
    DoMethod(data->obj, MUIM_Cardgame_SetCards,  1, cards, 0);
    DoMethod(data->obj, MUIM_Cardgame_SetCards,  2, cards, 0);
    DoMethod(data->obj, MUIM_Cardgame_SetCards,  3, cards, 0);
    DoMethod(data->obj, MUIM_Cardgame_SetCards,  4, cards, 0);
    DoMethod(data->obj, MUIM_Cardgame_SetCards,  5, cards, 0);

    cards[51] -= 100;
    DoMethod(data->obj, MUIM_Cardgame_SetCards,  6, &cards[51], 1);
    cards[50] -= 100;
    DoMethod(data->obj, MUIM_Cardgame_SetCards,  7, &cards[49], 2);
    cards[48] -= 100;
    DoMethod(data->obj, MUIM_Cardgame_SetCards,  8, &cards[46], 3);
    cards[45] -= 100;
    DoMethod(data->obj, MUIM_Cardgame_SetCards,  9, &cards[42], 4);
    cards[41] -= 100;
    DoMethod(data->obj, MUIM_Cardgame_SetCards, 10, &cards[37], 5);
    cards[36] -= 100;
    DoMethod(data->obj, MUIM_Cardgame_SetCards, 11, &cards[31], 6);
    cards[30] -= 100;
    DoMethod(data->obj, MUIM_Cardgame_SetCards, 12, &cards[24], 7);

    data->opencardsSize = 0;

    data->last_malus = 0;               // Zeitpkt des letzten Zeit-Punkteabzugs
  }

  data->stats.game_score   = 0;             // Statistik rücksetzen
  data->stats.game_seconds = 0;             // ...
  data->stats.game_turns   = 0;             // ...
  
  if(!data->stats.finished)
    data->stats.all_lost++;
  data->stats.finished = FALSE;

  while(data->undohead)
    Undo_RemHead(data);               // Undo-Liste leeren
  if(data->gamemode != GAMEMODE_FREECELL)
    UpdateIndicator(data);              // Move-Indicator setzen

  DisplayStats(data);

  if(xget(data->obj, MUIA_Cardgame_TimerRunning))
    setatt(data->obj, MUIA_Cardgame_TimerRunning, FALSE);
  DoMethod(data->obj, MUIM_Cardgame_TimerReset);

  setatt(data->obj, MUIA_Cardgame_NoREKOBack, s->rekoback ? 0 : 1);
  data->gameonline = TRUE;
}


/****************************************************************************************
  Timer
****************************************************************************************/

static void timerTick(struct CSolitaire_Data *data, long secs)
{
  long diff = secs - data->stats.game_seconds;
  data->stats.game_seconds = secs;
  data->stats.all_seconds += diff;

  if(secs - 10 > data->last_malus) // Punkteabzug wegen Zeit
  {
    data->last_malus += 10;
    AddScore(&data->stats, -2);
  }

  DisplayStats(data);
}

/****************************************************************************************
  Suggest / Sweep / Move
****************************************************************************************/

static BOOL Suggest(struct CSolitaire_Data *data, BOOL execute)
{
  int from, to, anz;
  //
  //  Karte von temp umdrehen !!!
  //

  //
  //  1 Karte von open auf final oder temp
  //
  anz  = 1;
  from = 1;
  for(to = 2; to <= 12; to++)
  {
    if(tryMove(data, from, to, anz, execute))
      return TRUE;
  }

  //
  //  1 Karte von temp auf final
  //
  anz = 1;
  for(from = 6; from <= 12; from++)
  {
    for(to = 2; to <= 5 ; to++)
    {
      if(tryMove(data, from, to, anz, execute))
        return TRUE;
    }
  }

  //
  //  max. Anz. Karten von temp auf temp
  //
  for(from = 6; from <= 12; from++)
  {
    int size, cards[53];
    DoMethod(data->obj, MUIM_Cardgame_GetCards, from, cards, &size);

    anz = 0;
    while(anz < size && cards[size-anz-1] < 100)
      anz++;
    if(anz)
    {
      for(to = 6; to <= 12; to++)
      {
        // Nicht König von einem freien auf einen anderen freien Platz verschieben
        if(anz != size || value(cards[size - anz]) != 13)
        {
          if(tryMove(data, from, to, anz, execute))
            return TRUE;
        }
      }
    }
  }

  return FALSE;
}

static void Sweep(struct CSolitaire_Data *data)
{
  BOOL sweep;
  int dest, source;

  if(data->gamemode == GAMEMODE_FREECELL)
  {
    do
    {
      sweep = FALSE;
      for(dest = 4; dest <= 7; dest++)
      {
        for(source = 0; source <= 15; source++)
        {
          if(source <= 3 || source >= 8)
          {
            if(tryMove(data, source, dest, 1, TRUE))
            {
              if(xget(data->obj, MUIA_Cardgame_TimerRunning)) // Nicht etwa neues Spiel?
              {
                sweep = TRUE;
                break;
              }
              else  // bei neuem Spiel nicht sofort wieder sweepen
              {
                return;
              }
            }
          }
        }
      }
    } while(sweep);
  }
  else
  {
    do
    {
      sweep = FALSE;
      for(dest = 2; dest <= 5; dest++)
      {
        for(source = 1; source <= 12; source++)
        {
          if(source == 1 || source >= 6)
          {
            if(tryMove(data, source, dest, 1, TRUE))
            {
              if(xget(data->obj, MUIA_Cardgame_TimerRunning)) // Nicht etwa neues Spiel?
              {
                sweep = TRUE;
                break;
              }
              else  // bei neuem Spiel nicht sofort wieder sweepen
              {
                return;
              }
            }
          }
        }
      }
    } while(sweep);
  }
}

/****************************************************************************************
  UpdateIndicator / DisplayStats
****************************************************************************************/

static void UpdateIndicator(struct CSolitaire_Data *data)
{
  if(data->movebutton)
    setatt(data->movebutton, MUIA_CButton_Visible, Suggest(data, FALSE));
}

static void DisplayStats(struct CSolitaire_Data *data)
{
  static char str[20];
  LONG s;
  Object *app;

  s = data->stats.game_seconds;
  sprintf(str, "%d:%02d ", (int)(s / 60), (int)(s % 60));
  setatt(data->timer, MUIA_Text_Contents, str);

  sprintf(str, "%d", (int)data->stats.game_score);
  setatt(data->score, MUIA_Text_Contents, str);

  if((app = (Object*) xget(data->obj, MUIA_ApplicationObject)))
    setatt(app, MUIA_Soliton_Stats, &data->stats);
}

/****************************************************************************************
  Undo
****************************************************************************************/

static void Undo_Store(struct CSolitaire_Data *data, int from, int to, int size,
BOOL uncover, int flag)
{
  struct UndoNode *n;
  
  if((n = AllocVec(sizeof(struct UndoNode), MEMF_PUBLIC)))
  {
    n->next    = data->undohead;
    n->from    = from;
    n->to      = to;
    n->size    = size;
    n->uncover = uncover;
    n->flag    = flag;
    data->undohead = n;
  }
  else /* clear all undo in case we failed */
  {
    while(data->undohead)
      Undo_RemHead(data);
  }
}

static void Undo_RemHead(struct CSolitaire_Data *data)
{
  struct UndoNode *n;

  n = data->undohead->next;
  FreeVec(data->undohead);
  data->undohead = n;
}

static void Undo(struct CSolitaire_Data *data)
{
  if(!data->undohead || data->stats.finished)
    return;
  if(data->gamemode == GAMEMODE_FREECELL)
  {
    DoMethod(data->obj, MUIM_Cardgame_MoveCards, data->undohead->to, data->undohead->from, data->undohead->size);

    Undo_RemHead(data);
  }
  else
  {
    //
    // D'n'D-Zug rückgängig machen
    //
    if(data->undohead->to != -1)
    {
      if(data->undohead->from == 1)
      {
        // Zug von open
        if(data->undohead->uncover) // Karte wieder auf Schattenstapel verdrängen
        {
          int size, cards[5];
          DoMethod(data->obj, MUIM_Cardgame_GetCards, 1, cards, &size);
          data->opencards[data->opencardsSize] = cards[0];
          data->opencardsSize++;
          DoMethod(data->obj, MUIM_Cardgame_SetCards, 1, cards, 0);
          DoMethod(data->obj, MUIM_Cardgame_SetEmptyImage, 1, 1);
        }
      }
      else
      {
        // normaler Zug
        if(data->undohead->uncover) // Aufdecken rückgängig
          topFace(data, data->undohead->from, FALSE);
      }
      DoMethod(data->obj, MUIM_Cardgame_MoveCards, data->undohead->to, data->undohead->from, data->undohead->size);
    }
    else
    {
      //
      // Einfaches aufdecken rückgängig machen
      //
      if(data->undohead->uncover)
      {
        topFace(data, data->undohead->from, FALSE);
      }
      else
      {
        int i;
        //
        // size Karten von open nach game zurück
        //
        if(data->undohead->from == 0)
        {
          // n Karten zurück
          for(i = 0; i < data->undohead->size; i++)
          {
            topFace(data, 1, FALSE);
            DoMethod(data->obj, MUIM_Cardgame_MoveCards, 1, 0, 1);
          }

          // <flag> Karten wieder auffächern 
          data->opencardsSize -= data->undohead->flag;
          DoMethod(data->obj, MUIM_Cardgame_SetCards, 1, &data->opencards[data->opencardsSize], data->undohead->flag);

          if(!data->opencardsSize)
            DoMethod(data->obj, MUIM_Cardgame_SetEmptyImage, 1, 0);
        }

        //
        // game auf open zurück
        //
        if(data->undohead->from == 1)  
        {
          int size, cards[53];
          DoMethod(data->obj, MUIM_Cardgame_GetCards, 0, cards, &size);
          DoMethod(data->obj, MUIM_Cardgame_SetCards, 0, cards, 0);

          data->opencardsSize = 0;
          for(i = size-1; i >= 0; i--) // Umgekehrt zurück auf Schattenstapel
          {
            UnCover(cards[i]); // dabei manche aufdecken
            data->opencards[data->opencardsSize] = cards[i];
            data->opencardsSize++;
          }

          data->opencardsSize -= data->undohead->size;
          if(data->opencardsSize < 0)
            data->opencardsSize = 0;
          DoMethod(data->obj, MUIM_Cardgame_SetCards, 1, &data->opencards[data->opencardsSize], data->undohead->size);
          DoMethod(data->obj, MUIM_Cardgame_SetEmptyImage, 1, 1);
        }
      }
    }

    AddScore(&data->stats, -15);              // Strafe
    DisplayStats(data);

    Undo_RemHead(data);                   // das eigentliche entfernen
    UpdateIndicator(data);
  }
}

/****************************************************************************************
  DragStart/-Accept/-Done / clickCard
****************************************************************************************/

static BOOL dragStart(struct CSolitaire_Data *data, LONG source, LONG* anz)
{
  LONG size = *anz;
  int csize, cards[53], i;

  if(!size)
    return FALSE; /* Paranoia */

  if(data->gamemode == GAMEMODE_FREECELL)
  {
    if(isGoalFC(source))
      return FALSE; // keine Karten vom Zielstapel

    DoMethod(data->obj, MUIM_Cardgame_GetCards, source, cards, &csize);
    if(!csize)
      return FALSE; // von leerem Stapel

    for(i = csize - size + 1; i < csize; i++)
    {
      int c1 = cards[i-1], c2 = cards[i];
      struct Settings * s = (struct Settings*) xget((Object*)xget(data->obj, MUIA_ApplicationObject),
      MUIA_Soliton_Settings);
      if(s->equalcolor)
      {
        if(family(c1) != family(c2))
          return FALSE;
      }
      else if(isRed(c1) == isRed(c2))
        return FALSE;
      if(value(c1) != value(c2) + 1)
        return FALSE;
    }

    if(size > 1)
      *anz = -size; // Drag nur simulieren, Karten werden selbst umgelegt
  }
  else
  {
    if(!source)
      return FALSE;
    DoMethod(data->obj, MUIM_Cardgame_GetCards, source, cards, &csize);
    if(!csize)
      return FALSE; /* empty stack */

    for(i = 0; i < size; i++)
      if(cards[csize - i - 1] > 100)
        return FALSE; /* hidden cards included */

    if(source == 1 && size != 1)
      return FALSE; /* more than 1 from open */

    /* block-moves ? */

    if(isTemp(source))
    {
      Object* app = (Object*)xget(data->obj, MUIA_ApplicationObject);

      if(app)
      {
        struct Settings * s = (struct Settings*) xget(app, MUIA_Soliton_Settings);
        if(s->block)
        {
          while(size < csize && cards[csize - size - 1] < 100)
            size++;
          *anz = size;
        }
      }
    }
  }
  return TRUE;
}

static BOOL dragAccept(struct CSolitaire_Data *data, int source, int dest, int size, int* cards)
{
  if(data->gamemode == GAMEMODE_FREECELL)
  {
    int down = Top(data, dest), up = cards[0];

    if(!size)
      return FALSE;      // man weiss ja nie

    if(isGoalFC(dest))
    {
      if(size > 1)
        return FALSE; /* only one card allowed */
      if(down == -1)
        return (BOOL) (value(up) == 14); /* only Ace allowed */
      if(family(down) != family(up))
        return FALSE;
      if(value(down) == 14)
        return (BOOL) (value(up) == 2); /* only throw 2 ony Ace */
      if(value(down) != value(up) -1)
        return FALSE;
    }
    else if(isTempFC(dest))
    {
      int freec = 1, i;

      if(down != -1)
      {
        struct Settings * s = (struct Settings*) xget((Object*)xget(data->obj, MUIA_ApplicationObject),
        MUIA_Soliton_Settings);
        if(s->equalcolor)
        {
          if(family(up) != family(down))
            return FALSE;
        }
        else if(isRed(up) == isRed(down))
          return FALSE;
        if(value(down) != value(up) + 1)
          return FALSE;
      }

      for(i = 0; i < 4; i++)
      {
        if(Top(data, i) == -1)
          freec++;
      }
      for(i = 8; i <= 15; i++)
      {
        if(Top(data, i) == -1 && i != source && i != dest)
          freec *= 2;
      }
    
      if(size > freec)
        return FALSE;
    }
    else  // Cell
    {
       if(size > 1 || Top(data, dest) != -1)
        return FALSE;
    }
  }
  else
  {
    int totop, frombottom;

    if(!size)
      return FALSE;      // man weiss ja nie
    if(dest == 1)
      return FALSE; // open ist kein erlaubtes Ziel

    totop = Top(data, dest);
    if(totop > 100)
      return FALSE; // nicht auf verdeckte Karte
    frombottom = cards[0];

    if(isTemp(dest)) /* temp */
    {
      if(!fits_temp(frombottom, totop))
        return FALSE;
    }
    else /* final */
    {
      if(size != 1 || !fits_final(frombottom, totop))
        return FALSE;
    }
  }
  return TRUE;
}

static BOOL laydown(struct CSolitaire_Data *data, int from)
{
  int min = 100, i;
  int size, cards[20];

  for(i = 4; i < 8; i++)
  {
    DoMethod(data->obj, MUIM_Cardgame_GetCards, i, cards, &size);
    if(size < min)
      min = size;
  }
  if(min < 1)
    min = 1;   /* 2 == laydown in every case */

  for(i = 4; i < 8; i++)
  {
    DoMethod(data->obj, MUIM_Cardgame_GetCards, i, cards, &size);
    if(size <= min && tryMove(data, from, i, 1, TRUE))
      return TRUE;
  }

  return FALSE;
}

static void moveOverCell(struct CSolitaire_Data *data, int from, int to, int size)
{
  int freec = 0, temp  = -1, i, j, buffer[4];

  for(i = 0; i < 4; i++)
  {
    if(Top(data, i) == -1)
      freec++;
  }
  
  if(size <= freec + 1)
  {
    // hochlegen
    for(j = 0; j < size - 1; j++)
    {
      for(i = 0; i < 4; i++)
      {
        if(Top(data, i) == -1)
        {
          buffer[j] = i;
          DoMethod(data->obj, MUIM_Cardgame_MoveCards, from, i, 1);
          break;
        }
      }
    }

    // rüberlegen
    DoMethod(data->obj, MUIM_Cardgame_MoveCards, from, to, 1);

    // runterlegen
    for(j = size - 2; j >= 0; j--)
    {
      DoMethod(data->obj, MUIM_Cardgame_MoveCards, buffer[j], to, 1);
    }
  }
  else
  {
    // freie Stelle finden

    for(i = 8; i <= 15; i++)
    {
      if(Top(data, i) == -1 && i != to)
      {
        temp = i; 
        break;
      }
    }

    // über freie Stelle gehen
    moveOverCell(data, from, temp, freec + 1          );
    moveOverCell(data, from, to  , size - (freec + 1) );
    moveOverCell(data, temp, to  , freec + 1          );
  }
}

static void dragDone(struct CSolitaire_Data *data, int source, int dest, int size)
{
  if(data->gamemode == GAMEMODE_FREECELL)
  {
    if(source != dest)
    {
      Undo_Store(data, source, dest, size, 0, 0);

      if(size > 1) // Karten über Zellen umlegen
      {
        moveOverCell(data, source, dest, size);
      }
    }

    if(!data->sweeping)  // Rekursionen vermeiden (sonst Stack overflow)
    {
      BOOL move;
      int i;

      data->sweeping = TRUE;
      do
      {
        move = FALSE;
        for(i = 0; i < 4; i++)
        {
          if(laydown(data, i))
            move = TRUE;
        }
        for(i = 8; i < 16; i++)
        {
          if(laydown(data, i))
            move = TRUE;
        }
      } while(move);
      data->sweeping = FALSE;

      if(!xget(data->obj, MUIA_Cardgame_TimerRunning))
      {
        setatt(data->obj, MUIA_Cardgame_TimerRunning, TRUE);
      }

      DisplayStats(data);
      Finished(data);
    }
  }
  else
  {
    BOOL autoturn = FALSE, unc = FALSE;
    Object* app = (Object*)xget(data->obj, MUIA_ApplicationObject);

    if(app)
      autoturn = ((struct Settings*)xget(app, MUIA_Soliton_Settings))->autoopen;

    //  From aufdecken
    if(autoturn && source != 1)
    {
      if(topFace(data, source, TRUE))
      {
        unc = TRUE;
        if(source != 1)
          AddScore(&data->stats, 5);
      }
    }

    //  Open nachfüllen
    if(source == 1)
    {
      int size, cards[5];
      DoMethod(data->obj, MUIM_Cardgame_GetCards, 1, cards, &size);
      if(!size && data->opencardsSize)
      {
        unc = TRUE;
        data->opencardsSize--;
        cards[0] = data->opencards[data->opencardsSize];
        DoMethod(data->obj, MUIM_Cardgame_SetCards, 1, cards, 1);
        if(!data->opencardsSize)
          DoMethod(data->obj, MUIM_Cardgame_SetEmptyImage, 1, 0);
      }
    }

    //  Undo und Statistiken
    if(source != dest)
      Undo_Store(data, source, dest, size, unc, 0);

    if(isFinal(dest) && !isFinal(source))
      AddScore(&data->stats, 10);
    else if(!isFinal(dest) && isFinal(source))
      AddScore(&data->stats, -15);
    else if(source == 1 && dest != 1)
      AddScore(&data->stats, 5);

    if(!xget(data->obj, MUIA_Cardgame_TimerRunning))
      setatt(data->obj, MUIA_Cardgame_TimerRunning, TRUE);

    DisplayStats(data);
    UpdateIndicator(data);
    Finished(data);
  }
}

static BOOL clickCard(struct CSolitaire_Data * data, int p, int nr, BOOL dblclck)
{
  if(data->gamemode == GAMEMODE_FREECELL)
  {
    if(dblclck && nr == 1 && !isGoalFC(p))
    {
      int i;

      for(i = 4; i < 8; i++)
      {
        if(tryMove(data, p, i, 1, TRUE))
          return TRUE;
      }
      if(isTempFC(p))
      {
        for(i = 0; i < 4; i++)
        {
          if(tryMove(data, p, i, 1, TRUE))
            return TRUE;
        }
      }
    }
  }
  else
  {
    int size,cards[53], dest, i;

    /* double click */
    if(dblclck) 
    {
      if(isTemp(p) || p == 1)
      {
        for(dest = 2; dest <= 12; dest++)
        {
          if(tryMove(data, p, dest, nr, TRUE))
            return TRUE;
        }
      }
    }

    //
    //  Karte aufdecken
    //
    if(isTemp(p) && nr == 1)
    {
      if(topFace(data, p, TRUE))
      {
        Undo_Store(data, p, -1, 0, TRUE, 0);
        UpdateIndicator(data);

        if(!xget(data->obj, MUIA_Cardgame_TimerRunning))
          setatt(data->obj, MUIA_Cardgame_TimerRunning, TRUE);

        AddScore(&data->stats, 5);
        DisplayStats(data);
      }
      return TRUE;
    }

    //
    //  Game Pile
    //
    if(p == 0)
    {
      // aufdeck besorgen
      ULONG aufdeck = 1;
      Object* app = (Object*)xget(data->obj, MUIA_ApplicationObject);
      if(app)
        aufdeck = ((struct Settings*)xget(app, MUIA_Soliton_Settings))->open;

      DoMethod(data->obj, MUIM_Cardgame_GetCards, 0, cards, &size);
      //
      //  Open auf Game umdrehen
      //
      if(!size)
      {
        // sichtbare Karten in Hintergrundbuffer und leeren

        DoMethod(data->obj, MUIM_Cardgame_GetCards, 1, cards, &size);
        for(i = 0; i < size; i++)
        {
          data->opencards[data->opencardsSize] = cards[i];
          data->opencardsSize++;
        }
        DoMethod(data->obj, MUIM_Cardgame_SetEmptyImage, 1, 0);
        DoMethod(data->obj, MUIM_Cardgame_SetCards     , 1, cards, 0);

        Undo_Store(data, 1, -1, size, FALSE, 0);

        // Reihenfolge umkehren und auf Game setzen

        for(i = 0; i < data->opencardsSize; i++)
        {
          Cover(data->opencards[i]);
          cards[data->opencardsSize - 1 - i] = data->opencards[i];
        }
        DoMethod(data->obj, MUIM_Cardgame_SetCards, 0, cards, data->opencardsSize);

        data->opencardsSize = 0;

        // Statistik mitführen
        data->stats.game_turns++;
        switch(aufdeck)
        {
        case 3:
          if(data->stats.game_turns > 2)
            AddScore(&data->stats, -20);
          break;
        case 2:
          if(data->stats.game_turns > 1)
            AddScore(&data->stats, -50);
          break;
        case 1:
          AddScore(&data->stats, -100);
          break;
        }
        DisplayStats(data);
      }
      //
      // n Karten von game auf open aufdecken
      //
      else 
      {
        int anz = aufdeck;
        if(anz > size)
          anz = size;

        // sichtbaren Karten in Hintergrundbuffer und leeren

        DoMethod(data->obj, MUIM_Cardgame_GetCards, 1, cards, &size);
        if(size)
        {
          for(i = 0; i < size; i++)
          {
            data->opencards[data->opencardsSize] = cards[i];
            data->opencardsSize++;
          }
          DoMethod(data->obj, MUIM_Cardgame_SetEmptyImage, 1, 1);
          DoMethod(data->obj, MUIM_Cardgame_SetCards     , 1, cards, 0);
        }

        // anz Karten rüberschaufeln und umdrehen
        for (i = 0; i < anz; i++)
        {
          DoMethod(data->obj, MUIM_Cardgame_MoveCards, 0, 1, 1);
          topFace(data, 1, TRUE);
        }

        Undo_Store(data, 0, -1, anz, FALSE, size);
      }

      if(!xget(data->obj, MUIA_Cardgame_TimerRunning))
        setatt(data->obj, MUIA_Cardgame_TimerRunning, TRUE);

      UpdateIndicator(data);
      return TRUE;
    }
  }
  return FALSE;
}

static IPTR SetGameMode(struct IClass* cl, Object* obj, enum GameMode mode)
{
  Object *app;
  struct CSolitaire_Data * data;
  struct TagItem ti[5];

  ti[0].ti_Tag = MUIA_Cardgame_RasterX;
  ti[1].ti_Tag = MUIA_Cardgame_RasterY;
  ti[2].ti_Tag = MUIA_Cardgame_Piles;
  ti[4].ti_Tag = TAG_DONE;

  data = (struct CSolitaire_Data *) INST_DATA(cl,obj);

  if((app = (Object*)xget(obj, MUIA_ApplicationObject)))
  {
    struct Settings *s;
    s = (struct Settings*)xget(app, MUIA_Soliton_Settings);
    ti[3].ti_Tag = MUIA_Cardgame_NoREKOBack;
    ti[3].ti_Data = s->rekoback ? 0 : 1;
  } else ti[3].ti_Tag = TAG_DONE;

  switch(mode)
  {
  case GAMEMODE_FREECELL:
    ti[0].ti_Data = 16;
    ti[1].ti_Data = 2;
    ti[2].ti_Data = (IPTR) "T,0,0,2;T,16,0,2;T,32,0,2;T,48,0,2;A,71,0,20;"
        "B,87,0,20;C,103,0,20;D,119,0,20;V,0,2,18;V,17,2,18;V,34,2,18;"
        "V,51,2,18;V,68,2,18;V,85,2,18;V,102,2,18;V,119,2,18;";
    break;
  case GAMEMODE_KLONDIKE:
    ti[0].ti_Data = 7;
    ti[1].ti_Data = 7;
    ti[2].ti_Data = (IPTR) "T,0,0,60;H,7,0,3;A,21,0,13;B,28,0,13;C,35,0,13;"
        "D,42,0,13;V,0,7,18;V,7,7,18;V,14,7,18;V,21,7,18;V,28,7,18;V,35,7,18;"
        "V,42,7,18;";
    break;
  }
  SetAttrsA(obj, ti);

  data->gamemode = mode;
  if(data->movebutton)
    setatt(data->movebutton, MUIA_CButton_Visible, FALSE);
  data->gameonline = FALSE;
  data->undohead  = NULL;
  ClearAll(&data->stats);
  data->stats.finished = TRUE;
  return (IPTR)obj;
}

/****************************************************************************************
  New / Dispose
****************************************************************************************/

static IPTR CSolitaire_New(struct IClass* cl, Object* obj, struct opSet* msg)
{
  struct CSolitaire_Data * data;

  if((obj = (Object*) DoSuperMethod(cl, obj, OM_NEW, 0, NULL)))
  {
    data = (struct CSolitaire_Data *) INST_DATA(cl,obj);
    data->obj        = obj;
    data->timer      = (Object*)GetTagData(MUIA_CSolitaire_Timer     , 0, msg->ops_AttrList);
    data->movebutton = (Object*)GetTagData(MUIA_CSolitaire_MoveButton, 0, msg->ops_AttrList);
    data->score      = (Object*)GetTagData(MUIA_CSolitaire_Score     , 0, msg->ops_AttrList);
    SetGameMode(cl,obj,GAMEMODE_KLONDIKE);
    return (IPTR) obj;
  }
  else
    return 0;
}

static IPTR CSolitaire_Dispose(struct IClass* cl, Object* obj, Msg msg)
{
  struct CSolitaire_Data * data = (struct CSolitaire_Data *) INST_DATA(cl,obj);

  while(data->undohead)
    Undo_RemHead(data);

  return DoSuperMethodA(cl, obj, msg);
}


/****************************************************************************************
  Dispatcher / Init / Exit
****************************************************************************************/

DISPATCHER(CSolitaire_Dispatcher)
{
  struct CSolitaire_Data* data = (struct CSolitaire_Data *) INST_DATA(cl, obj);

  switch(msg->MethodID)
  {
    case OM_NEW                   : return CSolitaire_New        (cl, obj, (struct opSet *) msg);
    case OM_DISPOSE               : return CSolitaire_Dispose    (cl, obj, msg);

    case MUIM_Cardgame_DragStart  : return (data->gameonline ?
                                    dragStart(data, ((struct MUIP_Cardgame_DragStart*)msg)->source,
                                    ((struct MUIP_Cardgame_DragStart*)msg)->size) : 0UL);

    case MUIM_Cardgame_DragAccept : return (data->gameonline ? 
                                    dragAccept(data, ((struct MUIP_Cardgame_DragAccept*)msg)->source,
                                    ((struct MUIP_Cardgame_DragAccept*)msg)->dest,
                                    ((struct MUIP_Cardgame_DragAccept*)msg)->size,
                                    ((struct MUIP_Cardgame_DragAccept*)msg)->cards) : 0UL);

    case MUIM_Cardgame_DragDone   : if(data->gameonline)
                                      dragDone(data, ((struct MUIP_Cardgame_DragDone*)msg)->source,
                                                   ((struct MUIP_Cardgame_DragDone*)msg)->dest,
                                                   ((struct MUIP_Cardgame_DragDone*)msg)->size);
                                    return 0;

    case MUIM_Cardgame_ClickCard  : return (data->gameonline ? 
                                    clickCard(data, ((struct MUIP_Cardgame_ClickCard*)msg)->pile,
                                    ((struct MUIP_Cardgame_ClickCard*)msg)->nr,
                                    ((struct MUIP_Cardgame_ClickCard*)msg)->dblclck) : 0UL);
  
    case MUIM_Cardgame_TimerTick  : if (data->gameonline)
                                      timerTick(data, ((struct MUIP_Cardgame_TimerTick*)msg)->sec);
                                    return 0;

    case MUIM_CSolitaire_NewGame  : NewGame(data);       return 0;
    case MUIM_CSolitaire_Undo     : Undo(data);          return 0;
    case MUIM_CSolitaire_Sweep    : Sweep(data);         return 0;
    case MUIM_CSolitaire_Move     : Suggest(data, TRUE); return 0;
    case MUIM_CSolitaire_GameMode : return SetGameMode(cl, obj, ((struct MUIP_Settings_GameMode *)msg)->mode);
  }
  return DoSuperMethodA(cl, obj, msg);
}

struct MUI_CustomClass *CL_CSolitaire = NULL;

BOOL CSolitaire_Init(void)
{
  if(!(CL_CSolitaire = MUI_CreateCustomClass(NULL, NULL, CL_Cardgame,
  sizeof(struct CSolitaire_Data), CSolitaire_Dispatcher)))
  {
    ErrorReq(MSG_CREATE_CSOLITAIRECLASS);
    return FALSE;
  }
  return TRUE;
}

void CSolitaire_Exit(void)
{
  if(CL_CSolitaire)
    MUI_DeleteCustomClass(CL_CSolitaire);
}


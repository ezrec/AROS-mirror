/*
 * field.c
 * =======
 * Implements the minefield.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dos/dos.h>
#include <exec/memory.h>
#include <exec/types.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <intuition/imageclass.h>
#include <proto/intuition.h>

#include "localize.h"
#include "display_globals.h"
#include "layout.h"
#include "images.h"
#include "counter.h"
#include "game.h"
#include "field.h"

extern struct GfxBase * GfxBase;

struct field {
   struct RastPort  *rp;
   WORD              left;
   WORD              top;
   UBYTE             rows;
   UBYTE             columns;
   UWORD             mines;
   UBYTE            *data;
   UWORD            *zeros;
   UWORD             swept;
};


#define MINE     0x09
#define LOCKED   0x10
#define WARNED   0x20
#define SWEPT    0x40
#define PATH     0x80

#define FIELD(f, r, c)          ((f)->data[(r) * (f)->columns + (c)])
#define SET_FIELD(f, r, c, v)   (FIELD ((f), (r), (c)) = (v))

#define CELL_VAL(f, r, c)       (FIELD ((f), (r), (c)) & 0x0F)
#define IS_ZERO(f, r, c)        (CELL_VAL ((f), (r), (c)) == 0)
#define IS_MINE(f, r, c)        (CELL_VAL ((f), (r), (c)) == MINE)
#define IS_LOCKED(f, r, c)      (FIELD ((f), (r), (c)) & LOCKED)
#define IS_WARNED(f, r, c)      (FIELD ((f), (r), (c)) & WARNED)
#define IS_SWEPT(f, r, c)       (FIELD ((f), (r), (c)) & SWEPT)
#define IS_PATH(f, r, c)        (FIELD ((f), (r), (c)) & PATH)
#define IS_SWEEPABLE(f, r, c)   (!(FIELD ((f), (r), (c)) & (LOCKED | SWEPT)))

#define LOCK_CELL(f, r, c)      (FIELD ((f), (r), (c)) |= LOCKED)
#define WARN_CELL(f, r, c)      (FIELD ((f), (r), (c)) |= WARNED)
#define UNLOCK_CELL(f, r, c)    (FIELD ((f), (r), (c)) &= ~(LOCKED | WARNED))
#define SWEEP_CELL(f, r, c)     (FIELD ((f), (r), (c)) |= SWEPT)
#define PATH_CELL(f, r, c)      (FIELD ((f), (r), (c)) |= PATH)

#define POSINFIELD(f, r, c)     ((c) + (r) * (f)->columns)
#define POS2ROW(f, p)           ((p) / (f)->columns)
#define POS2COL(f, p)           ((p) % (f)->columns)

#define FIELDWIDTH(f)           ((f)->columns * cell_w + 2 * LINEWIDTH)
#define FIELDHEIGHT(f)          ((f)->rows * cell_h + 2 * LINEHEIGHT)


#define RAISEDBOX     0
#define RECESSEDBOX   1
#define THINBOX       2


static void
draw_box(
   struct RastPort  *rp,
   WORD              left,
   WORD              top,
   UWORD             width,
   UWORD             height,
   UBYTE             type)
{
   if (type == THINBOX)
   {
      SetAPen (rp, gui_pens[SHADOWPEN]);
      Move (rp, left, top);
      Draw (rp, left + width - 1, top);
      Draw (rp, left + width - 1, top + height - 1);
      Draw (rp, left, top + height - 1);
      Draw (rp, left, top);
      SetAPen (rp, gui_pens[BACKGROUNDPEN]);
      Move (rp, left + 1, top + 1);
      Draw (rp, left + 1, top + height - 2);
      Move (rp, left + width - 2, top + 1);
      Draw (rp, left + width - 2, top + height - 2);
   }
   else
   {
      SetAPen(rp, gui_pens[((type == RAISEDBOX) ? SHINEPEN : SHADOWPEN)]);
      Move (rp, left + width - 2, top);
      Draw (rp, left, top);
      Draw (rp, left, top + height - 1);
      Move (rp, left + 1, top + height - 2);
      Draw (rp, left + 1, top + 1);
      SetAPen(rp, gui_pens[((type == RAISEDBOX) ? SHADOWPEN: SHINEPEN)]);
      Move (rp, left + 1, top + height - 1);
      Draw (rp, left + width - 1, top + height - 1);
      Draw (rp, left + width - 1, top);
      Move (rp, left + width - 2, top + 1);
      Draw (rp, left + width - 2, top + height - 2);
   }
}


static void
draw_cell (
   struct RastPort  *rp,
   WORD              left,
   WORD              top,
   UBYTE             value)
{
   draw_box (rp, left, top, cell_w, cell_h,
             ((value & SWEPT) ? THINBOX : RAISEDBOX));
   SetAPen (rp, gui_pens[BACKGROUNDPEN]);
   RectFill (rp, left + LINEWIDTH, top + LINEHEIGHT,
             left + cell_w - LINEWIDTH - 1, top + cell_h - LINEHEIGHT - 1);
   if (value & SWEPT)
   {
      value &= 0x0F;
      if (value == MINE)
         DrawImageState (rp, mine_image, left, top, IDS_NORMAL, NULL);
      else
      {
         if (value > 0)
         {
            char   ch = value + '0';
            
            SetAPen (rp, (game_pens[value - 1] != -1 && display_colors) ?
                     game_pens[value - 1] : gui_pens[TEXTPEN]);
            Move (rp, left + (cell_w - rp->TxWidth) / 2,
                  top + (cell_h - rp->TxHeight) / 2 + rp->TxBaseline);
            Text (rp, &ch, 1);
         }
      }
   }
   else if (value & LOCKED)
   {
      if (value & WARNED)
      {
         SetAPen (rp, gui_pens[TEXTPEN]);
         Move (rp, left + (cell_w - rp->TxWidth) / 2,
               top + (cell_h - rp->TxHeight) / 2 + rp->TxBaseline);
         Text (rp, "?", 1);
      }
      else
         DrawImageState (rp, flag_image, left, top, IDS_NORMAL, NULL);
   }
}


static UBYTE
count_neighbors (
   field_ptr   field,
   WORD        row,
   WORD        col,
   UBYTE       filter)
{
   register WORD    r, c;
   register UBYTE   count = 0;
   
   
   for (r = row - 1; r <= row + 1; ++r)
   {
      for (c = col - 1; c <= col + 1; ++c)
      {
         if (field_inside (field, r, c) && !(r == row && c == col) &&
             (FIELD (field, r, c) & filter) == filter)
         {
            ++count;
         }
      }
   }
   
   return count;
}


static void
avoid_trivial (
   field_ptr   field)
{
   register WORD   r, c, rr, cc;
   UBYTE           row0, col0;
   UWORD           pos, next;
   BOOL            trivial = FALSE;
   
   
   if (CELL_VAL (field, 0, 0) == 0)
   {
      SWEEP_CELL (field, 0, 0);
      pos = next = 0;
      field->zeros[next++] = POSINFIELD (field, 0, 0);
      while (pos < next && !trivial)
      {
         row0 = POS2ROW (field, field->zeros[pos]);
         col0 = POS2COL (field, field->zeros[pos]);
         for (r = row0 - 1; r <= row0 + 1; ++r)
         {
            for (c = col0 - 1; c <= col0 + 1; ++c)
            {
               if (field_inside (field, r, c) &&
                   !IS_SWEPT (field, r, c))
               {
                  SWEEP_CELL (field, r, c);
                  if (r == field->rows - 1 && c == field->columns - 1)
                     trivial = TRUE;
                  
                  if (IS_ZERO (field, r, c))
                      field->zeros[next++] = POSINFIELD (field, r, c);
               }
            }
         }
         ++pos;
      }
               
      for (r = 0; r < field->rows; ++r)
         for (c = 0; c < field->columns; ++c)
            if (IS_SWEPT (field, r, c))
               FIELD (field, r, c) &= ~SWEPT;
   }
   
   if (trivial)
   {
      row0 = col0 = r = 0;
      if (!IS_PATH (field, 0, 1))
         r |= 1;
      if (!IS_PATH (field, 1, 0))
         r |= 2;
      if (!IS_PATH (field, 1, 1))
         r |= 4;
      if (r == 0)
      {
#ifdef __AROS__
         if ((random() & 1) == 0)
#else
         if ((int)(2 * drand48 ()) == 0)
#endif
            r |= 1;
         else
            r |= 2;
      }
      
      while (row0 == 0 && col0 == 0)
      {
#ifdef __AROS__
	 rr = random() % 3;
#else
         rr = 3 * drand48 ();
#endif
         if (r & (1 << rr))
         {
            row0 = (rr == 0) ? 0 : 1;
            col0 = (rr == 1) ? 0 : 1;
         }
      }
      
      for (r = 0; r < field->rows && trivial; ++r)
      {
         for (c = 0; c < field->columns && trivial; ++c)
         {
            if (IS_MINE (field, r, c))
            {
               trivial = FALSE;
               
               SET_FIELD (field, r, c, 0);
               for (rr = r - 1; rr <= r + 1; ++rr)
               {
                  for (cc = c - 1; cc <= c + 1; ++cc)
                  {
                     if (field_inside (field, rr, cc) &&
                         !IS_MINE (field, rr, cc))
                     {
                        SET_FIELD (field, rr, cc,
                                   count_neighbors (field, rr, cc, MINE));
                     }
                  }
               }
            }
         }
      }
               
      SET_FIELD (field, row0, col0, MINE);
      for (rr = row0 - 1; rr <= row0 + 1; ++rr)
      {
         for (cc = col0 - 1; cc <= col0 + 1; ++cc)
         {
            if (field_inside (field, rr, cc) && !IS_MINE (field, rr, cc))
            {
               SET_FIELD (field, rr, cc,
                          count_neighbors (field, rr, cc, MINE));
            }
         }
      }
   }
}


static BOOL
generate_path (
   field_ptr   field)
{
   register WORD       r, c;
   UBYTE               row, col;
   UWORD               path_count, next_pos, possible_pos[8];
   UBYTE               count, try_count = 0;
   BOOL                done, failure = TRUE;
   
   char                msg[81];
   ULONG               char_count;
   struct TextExtent   te;
   
   
   SetAPen (field->rp, gui_pens[BACKGROUNDPEN]);
   RectFill (field->rp, field->left + LINEWIDTH, field->top + LINEHEIGHT,
             field->left + FIELDWIDTH (field) - LINEWIDTH - 1,
             field->top + FIELDHEIGHT (field) - LINEHEIGHT - 1);
   sprintf (msg, localized_string (MSG_GENERATE_PATH), try_count + 1);
   char_count = TextFit (field->rp, msg, strlen (msg), &te, NULL, 1,
                         FIELDWIDTH (field) - 2 * LINEWIDTH,
                         FIELDHEIGHT (field) - 2 * LINEHEIGHT);
   SetAPen (field->rp, gui_pens[TEXTPEN]);
   
   while (failure)
   {
      Move (field->rp,
            field->left + (FIELDWIDTH (field) - te.te_Width) / 2,
            field->top + field->rp->TxBaseline +
            (FIELDHEIGHT (field) - te.te_Height) / 2);
      Text (field->rp, msg, char_count);
      
      done = FALSE;
      row = col = 0;
      path_count = 1;
      PATH_CELL (field, row, col);
      while (!done)
      {
         count = 0;
         for (r = row - 1; r <= row + 1; ++r)
         {
            for (c = col - 1; c <= col + 1; ++c)
            {
               if (field_inside (field, r, c) && !IS_PATH (field, r, c) &&
                   !(col == 0 && r < row) && !(row == 0 && c < col) &&
                   !(col == field->columns - 1 && r < row) &&
                   !(row == field->rows - 1 && c < col))
               {
                  possible_pos[count++] = POSINFIELD (field, r, c);
               }
            }
         }
         
         if (count > 0)
         {
#ifdef __AROS__
            next_pos = possible_pos[random() % count];
#else
            next_pos = possible_pos[(int)(count * drand48 ())];
#endif
            row = POS2ROW (field, next_pos);
            col = POS2COL (field, next_pos);
            PATH_CELL (field, row, col);
            ++path_count;
         }
         
         if (count == 0 ||
             (row == field->rows - 1 && col == field->columns - 1))
            done = TRUE;
      }
      
      if (count == 0 ||
          field->rows * field->columns - path_count < field->mines)
      {
         ++try_count;
         sprintf (msg, localized_string (MSG_GENERATE_PATH), try_count + 1);
         
         if (current_level == OPTIONAL_LEVEL && try_count == 100)
            return FALSE;
         
         memset (field->data, 0,
                 field->rows * field->columns * sizeof (*field->data));
      }
      else
         failure = FALSE;
   }
   
   return TRUE;
}


static void
generate_opening (
   field_ptr   field)
{
   register UBYTE   row, col, row0, col0;
   register WORD    r, c;
   UBYTE            best_row = 0, best_col = 0;
   UWORD            best_count = 0xFFFF, sweep_count, wanted_count;
   UWORD            pos, next;
   
   
   wanted_count = field->rows * field->columns * (auto_opening / 100.0);
   if (wanted_count == 0)
      wanted_count = 1;
   
   for (row = 0; row < field->rows; ++row)
   {
      for (col = 0; col < field->columns; ++col)
      {
         if (!(IS_MINE (field, row, col) || IS_PATH (field, row, col)))
         {
            sweep_count = 1;
            if (CELL_VAL (field, row, col) == 0)
            {
               PATH_CELL (field, row, col);
               pos = next = 0;
               field->zeros[next++] = POSINFIELD (field, row, col);
               while (pos < next)
               {
                  row0 = POS2ROW (field, field->zeros[pos]);
                  col0 = POS2COL (field, field->zeros[pos]);
                  for (r = row0 - 1; r <= row0 + 1; ++r)
                  {
                     for (c = col0 - 1; c <= col0 + 1; ++c)
                     {
                        if (field_inside (field, r, c) &&
                            !IS_PATH (field, r, c))
                        {
                           PATH_CELL (field, r, c);
                           ++sweep_count;
                           
                           if (IS_ZERO (field, r, c))
                              field->zeros[next++] = POSINFIELD (field, r, c);
                        }
                     }
                  }
                  ++pos;
               }
               
               for (r = 0; r < field->rows; ++r)
                  for (c = 0; c < field->columns; ++c)
                     if (IS_PATH (field, r, c) && !IS_ZERO (field, r, c))
                        FIELD (field, r, c) &= ~PATH;
            }
            
            if (abs (sweep_count - wanted_count) <
                abs (best_count - wanted_count) &&
                sweep_count < field->rows * field->columns - field->mines)
            {
               best_count = sweep_count;
               best_row = row;
               best_col = col;
            }
         }
      }
   }
   
   reveal_this (field, best_row, best_col);
   counter_update (time_counter, best_count / 4);
   time_on = TRUE;
   timer_start (timer_obj, 0L, 1000000L);
}


static void
mutate_neighbors (
   field_ptr   field,
   WORD        row,
   WORD        col,
   UBYTE       filter,
   UBYTE       mask)
{
   register WORD   r, c;
   
   
   for (r = row - 1; r <= row + 1; ++r)
   {
      for (c = col - 1; c <= col + 1; ++c)
      {
         if (field_inside (field, r, c) && (r != row || r != col) &&
             FIELD (field, r, c) & filter)
         {
            FIELD (field, r, c) &= ~mask;
         }
      }
   }
}


static void
sweep_zero (
   field_ptr   field,
   UBYTE       row,
   UBYTE       col)
{
   register WORD   r, c;
   UWORD           pos = 0, next = 0;
   
   
   field->zeros[next++] = POSINFIELD (field, row, col);
   while (pos < next)
   {
      row = POS2ROW (field, field->zeros[pos]);
      col = POS2COL (field, field->zeros[pos]);
      for (r = row - 1; r <= row + 1; ++r)
      {
         for (c = col - 1; c <= col + 1; ++c)
         {
            if (field_inside (field, r, c) && IS_SWEEPABLE (field, r, c))
            {
               SWEEP_CELL (field, r, c);
               ++field->swept;
               draw_cell (field->rp, field->left + LINEWIDTH + c * cell_w,
                          field->top + LINEHEIGHT + r * cell_h,
                          FIELD (field, r, c));
               
               if (IS_ZERO (field, r, c))
                  field->zeros[next++] = POSINFIELD (field, r, c);
            }
         }
      }
      ++pos;
   }
}


static BOOL
reveal_around (
   field_ptr   field,
   UBYTE       row,
   UBYTE       col)
{
   register WORD   r, c;
   BOOL            success = TRUE;
   
   
   for (r = row - 1; r <= row + 1 && success; ++r)
      for (c = col - 1; c <= col + 1 && success; ++c)
         if (field_inside (field, r, c) && (r != row || c != col))
            success = reveal_this (field, r, c);
   
   return success;
}


field_ptr
field_init (
   struct RastPort  *rp,
   WORD              left,
   WORD              top,
   UBYTE             rows,
   UBYTE             columns,
   UWORD             mines)
{
   field_ptr   field;
   
   
   if ((field = AllocVec (sizeof (*field), MEMF_PUBLIC)))
   {
      field->rp = rp;
      field->left = left;
      field->top = top;
      field->rows = rows;
      field->columns = columns;
      field->mines = mines;
      if ((field->data = AllocVec (rows * columns *
                                   sizeof (*field->data), MEMF_PUBLIC)) &&
          (field->zeros = AllocVec (rows * columns *
                                    sizeof (*field->zeros), MEMF_PUBLIC)))
      {
         return field;
      }
   }
   
   field_free (field);
   return NULL;
}


void
field_free (
   field_ptr   field)
{
   if (field != NULL)
   {
      if (field->data != NULL)
         FreeVec (field->data);
      if (field->zeros != NULL)
         FreeVec (field->zeros);
      
      FreeVec (field);
   }
}


WORD
field_left (
   field_ptr   field)
{
   return field->left;
}


WORD
field_top (
   field_ptr   field)
{
   return field->top;
}


BOOL
field_size (
   field_ptr   field,
   UBYTE       rows,
   UBYTE       columns,
   UWORD       mines)
{
   field->mines = mines;
   if (rows != field->rows || columns != field->columns)
   {
      FreeVec (field->data);
      FreeVec (field->zeros);
      if ((field->data = AllocVec (rows * columns *
                                   sizeof (*field->data), MEMF_PUBLIC)) &&
          (field->zeros = AllocVec (rows * columns *
                                    sizeof (*field->zeros), MEMF_PUBLIC)))
      {
         field->rows = rows;
         field->columns = columns;
         
         return TRUE;
      }
      
      return FALSE;
   }
   
   return TRUE;
}


BOOL
field_inside (
   field_ptr   field,
   WORD        row,
   WORD        column)
{
   return (BOOL)(row >= 0 && row < field->rows &&
                 column >= 0 && column < field->columns);
}


BOOL
field_reset (
   field_ptr   field)
{
   STRPTR              msg;
   ULONG               char_count;
   struct TextExtent   te;
   
   UWORD            n = field->mines;
   register UBYTE   r, c;
   
   
   draw_box (field->rp, field_left (field), field_top (field),
             FIELDWIDTH (field), FIELDHEIGHT (field), RECESSEDBOX);

   field->swept = 0;
   memset (field->data, 0,
           field->rows * field->columns * sizeof (*field->data));
   
   if (chosen_task == SWEEP_PATH && !generate_path (field))
      return FALSE;
   
   SetAPen (field->rp, gui_pens[BACKGROUNDPEN]);
   RectFill (field->rp, field->left + LINEWIDTH, field->top + LINEHEIGHT,
             field->left + FIELDWIDTH (field) - LINEWIDTH - 1,
             field->top + FIELDHEIGHT (field) - LINEHEIGHT - 1);
   msg = localized_string (MSG_INITIALIZE_FIELD);
   char_count = TextFit (field->rp, msg, strlen (msg), &te, NULL, 1,
                         FIELDWIDTH (field) - 2 * LINEWIDTH,
                         FIELDHEIGHT (field) - 2 * LINEHEIGHT);
   SetAPen (field->rp, gui_pens[TEXTPEN]);
   Move (field->rp,
         field->left + (FIELDWIDTH (field) - te.te_Width) / 2,
         field->top + field->rp->TxBaseline +
         (FIELDHEIGHT (field) - te.te_Height) / 2);
   Text (field->rp, msg, char_count);
   
   while (n > 0)
   {
#ifdef __AROS__
      r = random() % field->rows;
      c = random() % field->columns;
#else
      r = drand48() * field->rows;
      c = drand48() * field->columns;
#endif


      if (!(IS_MINE (field, r, c) ||
            (IS_PATH (field, r, c) && chosen_task == SWEEP_PATH)))
      {
         SET_FIELD (field, r, c, MINE);
         --n;
      }
   }

   for (r = 0; r < field->rows; ++r)
      for (c = 0; c < field->columns; ++c)
         if (!IS_MINE (field, r, c))
            SET_FIELD (field, r, c, count_neighbors (field, r, c, MINE));
   
   SetAPen (field->rp, gui_pens[BACKGROUNDPEN]);
   RectFill (field->rp, field->left + LINEWIDTH, field->top + LINEHEIGHT,
             field->left + LINEWIDTH + cell_w - 1,
             field->top + LINEHEIGHT + cell_h - 1);
   draw_box (field->rp, field->left + LINEWIDTH, field->top + LINEHEIGHT,
             cell_w, cell_h, RAISEDBOX);
   for (r = 0; r < field->rows; ++r)
   {
      for (c = 0; c < field->columns; ++c)
      {
         if (r || c)
         {
            ClipBlit (field->rp,
                      field->left + LINEWIDTH, field->top + LINEHEIGHT,
                      field->rp, field->left + LINEWIDTH + c * cell_w,
                      field->top + LINEHEIGHT + r * cell_h,
                      cell_w, cell_h, 0xC0);
         }
      }
   }
   
   if (auto_opening > 0 && chosen_task == SWEEP_ALL)
      generate_opening (field);
   else if (chosen_task == SWEEP_PATH)
   {
      avoid_trivial (field);
      chosen_task = SWEEP_ALL;
      reveal_this (field, 0, 0);
      chosen_task = SWEEP_PATH;
      time_on = TRUE;
      timer_start (timer_obj, 0L, 1000000L);
   }
   return TRUE;
}


void
field_draw (
   field_ptr   field)
{
   register UBYTE   r, c;
   
   
   draw_box (field->rp, field_left (field), field_top (field),
             FIELDWIDTH (field), FIELDHEIGHT (field), RECESSEDBOX);
   for (r = 0; r < field->rows; ++r)
   {
      for (c = 0; c < field->columns; ++c)
      {
         draw_cell (field->rp, field->left + LINEWIDTH + c * cell_w,
                    field->top + LINEHEIGHT + r * cell_h,
                    FIELD (field, r, c));
      }
   }
}


void
field_delete (
   field_ptr   field)
{
   SetAPen (field->rp, gui_pens[BACKGROUNDPEN]);
   RectFill (field->rp, field->left, field->top,
             field->left + FIELDWIDTH (field) - 1,
             field->top + FIELDHEIGHT (field) - 1);
}


BOOL
reveal_this (
   field_ptr   field,
   UBYTE       row,
   UBYTE       col)
{
   if (IS_SWEEPABLE (field, row, col) &&
       (chosen_task == SWEEP_ALL || count_neighbors (field, row, col, SWEPT)))
   {
      SWEEP_CELL (field, row, col);
      draw_cell (field->rp, field->left + LINEWIDTH + col * cell_w,
                 field->top + LINEHEIGHT + row * cell_h,
                 FIELD (field, row, col));
      
      if (IS_MINE (field, row, col))
         return FALSE;
      
      ++field->swept;

      if (CELL_VAL (field, row, col) == 0)
         sweep_zero (field, row, col);
   }
   
   return TRUE;
}


BOOL
sweep_this (
   field_ptr   field,
   UBYTE       row,
   UBYTE       col)
{
   register WORD    r, c;
   register UBYTE   lock_count = 0;
   register UBYTE   warn_count = 0;
   register UBYTE   free_count = 0;
   register BOOL    success = TRUE;
   
   
   if (IS_SWEPT (field, row, col))
   {
      for (r = row - 1; r <= row + 1; ++r)
      {
         for (c = col - 1; c <= col + 1; ++c)
         {
            if (field_inside (field, r, c))
            {
               if (IS_WARNED (field, r, c))
                  ++warn_count;
               else if (IS_LOCKED (field, r, c))
                  ++lock_count;
               else if (!IS_SWEPT (field, r, c))
                  ++free_count;
            }
         }
      }
      
      if (lock_count >= CELL_VAL (field, row, col))
      {
         if (warn_count)
            mutate_neighbors (field, row, col, WARNED, WARNED | LOCKED);
         success = reveal_around (field, row, col);
      }
      else if (free_count + warn_count + lock_count ==
               CELL_VAL (field, row, col))
      {
         if (warn_count)
            mutate_neighbors (field, row, col, WARNED, WARNED | LOCKED);
         for (r = row - 1; r <= row + 1; ++r)
         {
            for (c = col - 1; c <= col + 1; ++c)
            {
               if (field_inside (field, r, c) && IS_SWEEPABLE (field, r, c))
               {
                  toggle_lock (field, r, c);
               }
            }
         }
      }
   }
   release_around (field, row, col);
   
   return success;
}


void
toggle_lock (
   field_ptr   field,
   UBYTE       row,
   UBYTE       col)
{
   if (!IS_SWEPT (field, row, col))
   {
      if (IS_LOCKED (field, row, col))
      {
         if (IS_WARNED (field, row, col))
            UNLOCK_CELL (field, row, col);
         else
         {
            if (place_warnings)
               WARN_CELL (field, row, col);
            else
               UNLOCK_CELL (field, row, col);
            counter_update (flag_counter, counter_value (flag_counter) + 1);
         }
         draw_cell (field->rp, field->left + LINEWIDTH + col * cell_w,
                    field->top + LINEHEIGHT + row * cell_h,
                    FIELD (field, row, col));
      }
      else if (counter_value (flag_counter) > 0)
      {
         LOCK_CELL (field, row, col);
         draw_cell (field->rp, field->left + LINEWIDTH + col * cell_w,
                    field->top + LINEHEIGHT + row * cell_h,
                    FIELD (field, row, col));
         counter_update (flag_counter, counter_value (flag_counter) - 1);
      }
   }
}


void
press_this (
   field_ptr   field,
   UBYTE       row,
   UBYTE       col)
{
   if (IS_SWEEPABLE (field, row, col) &&
       (chosen_task == SWEEP_ALL || count_neighbors (field, row, col, SWEPT)))
   {
      draw_box (field->rp, field->left + LINEWIDTH + col * cell_w,
                field->top + LINEHEIGHT + row * cell_h,
                cell_w, cell_h, THINBOX);
   }
}


void
press_around (
   field_ptr   field,
   UBYTE       row,
   UBYTE       col)
{
   register WORD   r, c;
   
   
   if (IS_SWEPT (field, row, col))
      for (r = row - 1; r <= row + 1; ++r)
         for (c = col - 1; c <= col + 1; ++c)
            if (field_inside (field, r, c) && (r != row || c != col))
               press_this (field, r, c);
}


void
release_this (
   field_ptr   field,
   UBYTE       row,
   UBYTE       col)
{
   if (IS_SWEEPABLE (field, row, col))
   {
      draw_box (field->rp, field->left + LINEWIDTH + col * cell_w,
                field->top + LINEHEIGHT + row * cell_h,
                cell_w, cell_h, RAISEDBOX);
   }
}


void
release_around (
   field_ptr   field,
   UBYTE       row,
   UBYTE       col)
{
   register WORD   r, c;
   
   
   if (IS_SWEPT (field, row, col))
      for (r = row - 1; r <= row + 1; ++r)
         for (c = col - 1; c <= col + 1; ++c)
            if (field_inside (field, r, c) && (r != row || c != col))
               release_this (field, r, c);
}


BOOL
field_swept (
   field_ptr   field)
{
   return (BOOL)((chosen_task == SWEEP_ALL &&
                  field->rows * field->columns - field->mines ==
                  field->swept) ||
                 (chosen_task == SWEEP_PATH &&
                  IS_SWEPT (field, field->rows - 1, field->columns - 1)));
}


void
field_win (
   field_ptr   field)
{
   register UBYTE   r, c;
   
   
   for (r = 0; r < field->rows; ++r)
   {
      for (c = 0; c < field->columns; ++c)
      {
         if (IS_MINE (field, r, c) &&
             (IS_SWEEPABLE (field, r, c) || IS_WARNED (field, r, c)))
         {
            LOCK_CELL (field, r, c);
            draw_cell (field->rp, field->left + LINEWIDTH + c * cell_w,
                       field->top + LINEHEIGHT + r * cell_h,
                       FIELD (field, r, c));
         }
      }
   }
   counter_update (flag_counter, 0);
}


void
field_lose (
   field_ptr   field)
{
   register UBYTE   r, c;
   
   
   for (r = 0; r < field->rows; ++r)
   {
      for (c = 0; c < field->columns; ++c)
      {
         if (IS_MINE (field, r, c) && !IS_SWEPT (field, r, c))
         {
            SWEEP_CELL (field, r, c);
            draw_cell (field->rp, field->left + LINEWIDTH + c * cell_w,
                       field->top + LINEHEIGHT + r * cell_h,
                       FIELD (field, r, c));
         }
      }
   }
}

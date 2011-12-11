/*
 * counter.c
 * =========
 * Implements digital counters.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <stdio.h>

#include <exec/memory.h>
#include <exec/types.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <intuition/imageclass.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <proto/intuition.h>
#include <proto/alib.h>

#include "display_globals.h"
#include "images.h"
#include "counter.h"

extern struct GfxBase * GfxBase;

/* Data for counter object */
struct counter {
   struct Window  *win;
   WORD            left;
   WORD            top;
   UWORD           value;
   BOOL            digital;
};


/* Calculates the width that a digital counter will get */
UWORD         /* calculated width */
counter_width (void)
{
   return 3 * DIGITWIDTH + 6 * LINEWIDTH;
}


/* Calculates the height that a digital counter will get */
UWORD          /* calculated height */
counter_height (void)
{
   return DIGITHEIGHT + 4 * LINEHEIGHT;
}


/* Creates a new counter object */
counter_ptr                   /* created counter */
counter_init (
   struct Window  *win,       /* window to use counter in */
   WORD            left,      /* left offset */
   WORD            top,       /* top offset */
   UWORD           value,     /* initial value */
   BOOL            digital)   /* digital counter? */
{
   counter_ptr   counter;
   
   
   if (NULL != (counter = AllocVec (sizeof (*counter), MEMF_PUBLIC)))
   {
      counter->win = win;
      counter->left = left;
      counter->top = top;
      counter->value = value;
      counter->digital = digital;
   }
   
   return counter;
}


/* Frees a counter object */
void
counter_free (
   counter_ptr   counter)   /* counter to free */
{
   if (counter)
      FreeVec (counter);
}


/* Reads the value of a counter */
UWORD              /* the value */
counter_value (
   counter_ptr   counter)   /* counter to read value from */
{
   return counter->value;
}


/* Changes a counter's position */
void
counter_move (
   counter_ptr   counter,   /* counter to move */
   WORD          left,      /* new left offset */
   WORD          top)       /* new top offset */
{
   counter->left = left;
   counter->top = top;
}


/* Draws a counter */
void
counter_draw (
   counter_ptr   counter,   /* counter to draw */
   APTR          vi)        /* visual info */
{
   if (counter->digital)
   {
      ULONG   bgpen;
      
      
      DrawBevelBox (counter->win->RPort,
                    counter->left, counter->top,
                    counter_width (), counter_height (),
                    GT_VisualInfo, vi,
                    GTBB_Recessed, TRUE,
                    TAG_DONE);
      
      GetAttr (IA_BGPen, (Object *)digit_images[0], &bgpen);
      SetAPen (counter->win->RPort, bgpen);
      RectFill (counter->win->RPort,
                counter->left + LINEWIDTH, counter->top + LINEHEIGHT,
                counter->left + counter_width () - LINEWIDTH - 1,
                counter->top + counter_height () - LINEHEIGHT - 1);
   }
   
   counter_update (counter, counter->value);
}


/* Deletes a counter */
void
counter_delete (
   counter_ptr   counter)   /* counter to delete */
{
   if (counter->digital)
   {
      SetAPen (counter->win->RPort, gui_pens[BACKGROUNDPEN]);
      RectFill (counter->win->RPort,
                counter->left, counter->top,
                counter->left + counter_width () - 1,
                counter->top + counter_height () - 1);
   }
}


void
counter_update (
   counter_ptr   counter,
   UWORD         value)
{
   register UBYTE   i;
   char             value_str[4];
   
   
   counter->value = value;
   if (counter->value > 999)
      sprintf (value_str, "%03d", counter->value % 1000);
   else
      sprintf (value_str, "%3d", counter->value);
   
   for (i = 0; i < 3; ++i)
   {
      if (counter->digital)
      {
         DrawImageState (counter->win->RPort,
                         digit_images[((value_str[i] == ' ') ?
                                       EMPTYDIGIT : value_str[i] - '0')],
                         counter->left + 2 * LINEWIDTH +
                         i * (DIGITWIDTH + LINEWIDTH),
                         counter->top + 2 * LINEHEIGHT,
                         IDS_NORMAL, NULL);
      }
      else
      {
         counter->win->Title[counter->left + i] = value_str[i];
         SetWindowTitles (counter->win, counter->win->Title, (UBYTE *)~0);
      }
   }
}

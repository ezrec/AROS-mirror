/*
 * counter.h
 * =========
 * Interface to digital counters.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#ifndef COUNTER_H
#define COUNTER_H

#include <exec/types.h>
#include <intuition/intuition.h>


/* Pointer to counter object */
typedef struct counter  *counter_ptr;


/* Calculates the width that a digital counter will get */
UWORD                   /* calculated width */
counter_width (void);


/* Calculates the height that a digital counter will get */
UWORD                    /* calculated height */
counter_height (void);


/* Creates a new counter object */
counter_ptr                    /* created counter */
counter_init (
   struct Window  *win,        /* window to use counter in */
   WORD            left,       /* left offset */
   WORD            top,        /* top offset */
   UWORD           value,      /* initial value */
   BOOL            digital);   /* digital counter? */


/* Frees a counter object */
void
counter_free (
   counter_ptr   counter);   /* counter to free */


/* Reads the value of a counter */
UWORD                        /* the value */
counter_value (
   counter_ptr   counter);   /* counter to read value from */


/* Changes a counter's position */
void
counter_move (
   counter_ptr   counter,   /* counter to move */
   WORD          left,      /* new left offset */
   WORD          top);      /* new top offset */


/* Draws a counter */
void
counter_draw (
   counter_ptr   counter,   /* counter to draw */
   APTR          vi);       /* visual info */


/* Deletes a counter */
void
counter_delete (
   counter_ptr   counter);   /* counter to delete */


/* Sets the value of a counter */
void
counter_update (
   counter_ptr   counter,   /* counter to update */
   UWORD         value);    /* new value */

#endif /* COUNTER_H */

/*
 * button.h
 * ========
 * The smiley button.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <exec/types.h>
#include <graphics/rastport.h>
#include <intuition/intuition.h>


void
button_init (
   struct RastPort  *rp,
   WORD              x,
   WORD              y,
   UWORD             w,
   UWORD             h,
   struct Image     *img);

void
button_changebox (
   WORD    x,
   WORD    y,
   UWORD   w,
   UWORD   h);

void
button_render (void);

BOOL
button_hit (
   WORD   x,
   WORD   y);

void
button_pressed (
   WORD   x,
   WORD   y);

BOOL
button_release (
   WORD   x,
   WORD   y);

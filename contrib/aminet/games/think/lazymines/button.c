/*
 * button.c
 * ========
 * The smiley button.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <clib/alib_protos.h>
#include <proto/graphics.h>
#include <intuition/imageclass.h>
#include <proto/intuition.h>

#include "display_globals.h"
#include "button.h"


static struct RastPort  *btn_rp;
static WORD             left, top;
static UWORD            width, height;
static struct Image    *image;
static BOOL             highlighted, used = FALSE;

extern struct GfxBase * GfxBase;

static void
button_box (
   struct RastPort  *rp,
   WORD              left,
   WORD              top,
   UWORD             width,
   UWORD             height,
   BOOL              recessed)
{
   SetAPen (rp, gui_pens[((recessed) ? FILLPEN : BACKGROUNDPEN)]);
   RectFill (rp, left + 2, top + 1, left + width - 3, top + height - 2);
   SetAPen(rp, gui_pens[((recessed) ? SHADOWPEN : SHINEPEN)]);
   Move (rp, left + width - 2, top);
   Draw (rp, left, top);
   Draw (rp, left, top + height - 1);
   Move (rp, left + 1, top + height - 2);
   Draw (rp, left + 1, top + 1);
   SetAPen(rp, gui_pens[((recessed) ? SHINEPEN : SHADOWPEN)]);
   Move (rp, left + 1, top + height - 1);
   Draw (rp, left + width - 1, top + height - 1);
   Draw (rp, left + width - 1, top);
   Move (rp, left + width - 2, top + 1);
   Draw (rp, left + width - 2, top + height - 2);
}


__inline static BOOL
button_inside (
   WORD   x,
   WORD   y)
{
   return (BOOL)(x >= left && x < left + width &&
                 y >= top && y < top + height);
}


void
button_init (
   struct RastPort  *rp,
   WORD              x,
   WORD              y,
   UWORD             w,
   UWORD             h,
   struct Image     *img)
{
   btn_rp = rp;
   left = x;
   top = y;
   width = w;
   height = h;
   image = img;
   SetAttrs (image, IA_Left, LINEWIDTH + INTERWIDTH / 2,
                    IA_Top, LINEHEIGHT + INTERHEIGHT / 2, TAG_DONE); 
   highlighted = FALSE;
   used = TRUE;
}


void
button_changebox (
   WORD    x,
   WORD    y,
   UWORD   w,
   UWORD   h)
{
   if (used)
   {
      SetAPen (btn_rp, gui_pens[BACKGROUNDPEN]);
      RectFill (btn_rp, left, top, left + width - 1, top + height - 1);
      
      left = x;
      top = y;
      width = w;
      height = h;
      button_render ();
   }
}


void
button_render (void)
{
   if (used)
   {
      button_box (btn_rp,
                  left, top, width, height,
                  highlighted);
      
      DoMethod ((Object *)image, IM_DRAWFRAME, btn_rp, (left << 16) + top,
                ((highlighted) ? IDS_SELECTED : IDS_NORMAL), NULL,
                (width - 2 * LINEWIDTH - INTERWIDTH << 16) +
                height - 2 * LINEHEIGHT - INTERHEIGHT);
   }
}


BOOL
button_hit (
   WORD   x,
   WORD   y)
{
   if (used && button_inside (x, y))
   {
      highlighted = TRUE;
      button_render ();
      
      return TRUE;
   }
   
   return FALSE;
}


void
button_pressed (
   WORD   x,
   WORD   y)
{
   if (used)
   {
      if (button_inside (x, y))
      {
         if (!highlighted)
         {
            highlighted = TRUE;
            button_render ();
         }
      }
      else if (highlighted)
      {
         highlighted = FALSE;
         button_render ();
      }
   }
}


BOOL
button_release (
   WORD   x,
   WORD   y)
{
   BOOL   retval = FALSE;
   
   
   if (used)
   {
      button_pressed (x, y);
      if (highlighted)
      {
         retval = TRUE;
         highlighted = FALSE;
         button_render ();
      }
   }
   
   return retval;
}

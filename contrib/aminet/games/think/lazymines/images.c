/*
 * images.c
 * ========
 * Definition and initialization of game images.
 *
 * Copyright (C) 1994-1998 Håkan L. Younes (lorens@hem.passagen.se)
 */

#include <exec/types.h>
#include <graphics/gfxbase.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include "chunkyiclass.h"

#include "display_globals.h"
#include "layout.h"
#include "images.h"

extern struct GfxBase * GfxBase;

/* Various constants */
#define NUM_DIGITS        11   /* number of digit images */
#define NUM_IMAGECOLORS   8    /* number of image colors */
#define NUM_IMAGEPENS     9    /* number of image pens */


/* 
 * Definition of images
 * --------------------
 * 1 = black (TEXTPEN)
 * 2 = white (SHINEPEN)
 * 3 = red (FILLPEN)
 * 4 = red (SHINEPEN) == 1 => BACKGROUNDPEN
 * 5 = brown (SHADOWPEN) == 3 || 4 => TEXTPEN
 * 6 = yellow (SHINEPEN) == 1 => BACKGROUNDPEN
 * 7 = lightgray (BACKGROUNDPEN)
 * 8 = gray (SHADOWPEN)
 * 9 = blue (FILLPEN)
 */
static UBYTE   flag_data[] = {
   0,8, 0,8,
   0,0,2,0,0,0,0,0,
   0,0,2,3,3,0,0,0,
   0,0,2,3,3,3,3,0,
   0,0,2,3,3,3,3,3,
   0,0,2,0,0,0,0,0,
   0,0,2,1,0,0,0,0,
   0,5,5,5,5,1,0,0,
   5,5,5,5,5,5,5,0
};

static UBYTE   mine_data[] = {
   0,8, 0,8,
   0,0,0,0,0,0,0,0,
   0,0,0,1,0,0,0,0,
   0,0,1,1,1,1,0,0,
   0,1,1,0,1,1,1,1,
   1,1,1,1,1,1,1,0,
   0,0,1,1,1,1,0,0,
   0,0,0,0,1,0,0,0,
   0,0,0,0,0,0,0,0
};

UBYTE   digit_data[NUM_DIGITS][DIGITWIDTH * DIGITHEIGHT + 4 + 1] = {
   { 0,DIGITWIDTH, 0,DIGITHEIGHT,
     0,0,4,4,4,4,4,4,4,4,4,0,0,
     4,0,0,4,4,4,4,4,4,4,0,0,4,
     4,4,0,0,4,4,4,4,4,0,0,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,0,0,0,0,0,0,0,0,0,4,4,
     4,0,0,5,5,5,5,5,5,5,0,0,4,
     0,0,5,5,5,5,5,5,5,5,5,0,0,
     4,0,0,5,5,5,5,5,5,5,0,0,4,
     4,4,0,0,0,0,0,0,0,0,0,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,0,0,4,4,4,4,4,0,0,4,4,
     4,0,0,4,4,4,4,4,4,4,0,0,4,
     0,0,4,4,4,4,4,4,4,4,4,0,0, 0
   },
   { 0,DIGITWIDTH, 0,DIGITHEIGHT,
     0,0,5,5,5,5,5,5,5,5,5,0,0,
     5,0,0,5,5,5,5,5,5,5,0,0,4,
     5,5,0,0,5,5,5,5,5,0,0,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,0,0,0,0,0,0,0,0,0,4,4,
     5,0,0,5,5,5,5,5,5,5,0,0,4,
     0,0,5,5,5,5,5,5,5,5,5,0,0,
     5,0,0,5,5,5,5,5,5,5,0,0,4,
     5,5,0,0,0,0,0,0,0,0,0,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,0,0,5,5,5,5,5,0,0,4,4,
     5,0,0,5,5,5,5,5,5,5,0,0,4,
     0,0,5,5,5,5,5,5,5,5,5,0,0, 0
   },
   { 0,DIGITWIDTH, 0,DIGITHEIGHT,
     0,0,4,4,4,4,4,4,4,4,4,0,0,
     5,0,0,4,4,4,4,4,4,4,0,0,4,
     5,5,0,0,4,4,4,4,4,0,0,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,0,0,0,0,0,0,0,0,0,4,4,
     5,0,0,4,4,4,4,4,4,4,0,0,4,
     0,0,4,4,4,4,4,4,4,4,4,0,0,
     4,0,0,4,4,4,4,4,4,4,0,0,5,
     4,4,0,0,0,0,0,0,0,0,0,5,5,
     4,4,4,0,0,0,0,0,0,0,5,5,5,
     4,4,4,0,0,0,0,0,0,0,5,5,5,
     4,4,4,0,0,0,0,0,0,0,5,5,5,
     4,4,4,0,0,0,0,0,0,0,5,5,5,
     4,4,4,0,0,0,0,0,0,0,5,5,5,
     4,4,0,0,4,4,4,4,4,0,0,5,5,
     4,0,0,4,4,4,4,4,4,4,0,0,5,
     0,0,4,4,4,4,4,4,4,4,4,0,0, 0
   },
   { 0,DIGITWIDTH, 0,DIGITHEIGHT,
     0,0,4,4,4,4,4,4,4,4,4,0,0,
     5,0,0,4,4,4,4,4,4,4,0,0,4,
     5,5,0,0,4,4,4,4,4,0,0,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,0,0,0,0,0,0,0,0,0,4,4,
     5,0,0,4,4,4,4,4,4,4,0,0,4,
     0,0,4,4,4,4,4,4,4,4,4,0,0,
     5,0,0,4,4,4,4,4,4,4,0,0,4,
     5,5,0,0,0,0,0,0,0,0,0,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,0,0,4,4,4,4,4,0,0,4,4,
     5,0,0,4,4,4,4,4,4,4,0,0,4,
     0,0,4,4,4,4,4,4,4,4,4,0,0, 0
   },
   { 0,DIGITWIDTH, 0,DIGITHEIGHT,
     0,0,5,5,5,5,5,5,5,5,5,0,0,
     4,0,0,5,5,5,5,5,5,5,0,0,4,
     4,4,0,0,5,5,5,5,5,0,0,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,0,0,0,0,0,0,0,0,0,4,4,
     4,0,0,4,4,4,4,4,4,4,0,0,4,
     0,0,4,4,4,4,4,4,4,4,4,0,0,
     5,0,0,4,4,4,4,4,4,4,0,0,4,
     5,5,0,0,0,0,0,0,0,0,0,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,0,0,5,5,5,5,5,0,0,4,4,
     5,0,0,5,5,5,5,5,5,5,0,0,4,
     0,0,5,5,5,5,5,5,5,5,5,0,0, 0
   },
   { 0,DIGITWIDTH, 0,DIGITHEIGHT,
     0,0,4,4,4,4,4,4,4,4,4,0,0,
     4,0,0,4,4,4,4,4,4,4,0,0,5,
     4,4,0,0,4,4,4,4,4,0,0,5,5,
     4,4,4,0,0,0,0,0,0,0,5,5,5,
     4,4,4,0,0,0,0,0,0,0,5,5,5,
     4,4,4,0,0,0,0,0,0,0,5,5,5,
     4,4,4,0,0,0,0,0,0,0,5,5,5,
     4,4,4,0,0,0,0,0,0,0,5,5,5,
     4,4,0,0,0,0,0,0,0,0,0,5,5,
     4,0,0,4,4,4,4,4,4,4,0,0,5,
     0,0,4,4,4,4,4,4,4,4,4,0,0,
     5,0,0,4,4,4,4,4,4,4,0,0,4,
     5,5,0,0,0,0,0,0,0,0,0,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,0,0,4,4,4,4,4,0,0,4,4,
     5,0,0,4,4,4,4,4,4,4,0,0,4,
     0,0,4,4,4,4,4,4,4,4,4,0,0, 0
   },
   { 0,DIGITWIDTH, 0,DIGITHEIGHT,
     0,0,4,4,4,4,4,4,4,4,4,0,0,
     4,0,0,4,4,4,4,4,4,4,0,0,5,
     4,4,0,0,4,4,4,4,4,0,0,5,5,
     4,4,4,0,0,0,0,0,0,0,5,5,5,
     4,4,4,0,0,0,0,0,0,0,5,5,5,
     4,4,4,0,0,0,0,0,0,0,5,5,5,
     4,4,4,0,0,0,0,0,0,0,5,5,5,
     4,4,4,0,0,0,0,0,0,0,5,5,5,
     4,4,0,0,0,0,0,0,0,0,0,5,5,
     4,0,0,4,4,4,4,4,4,4,0,0,5,
     0,0,4,4,4,4,4,4,4,4,4,0,0,
     4,0,0,4,4,4,4,4,4,4,0,0,4,
     4,4,0,0,0,0,0,0,0,0,0,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,0,0,4,4,4,4,4,0,0,4,4,
     4,0,0,4,4,4,4,4,4,4,0,0,4,
     0,0,4,4,4,4,4,4,4,4,4,0,0, 0
   },
   { 0,DIGITWIDTH, 0,DIGITHEIGHT,
     0,0,4,4,4,4,4,4,4,4,4,0,0,
     5,0,0,4,4,4,4,4,4,4,0,0,4,
     5,5,0,0,4,4,4,4,4,0,0,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,0,0,0,0,0,0,0,0,0,4,4,
     5,0,0,5,5,5,5,5,5,5,0,0,4,
     0,0,5,5,5,5,5,5,5,5,5,0,0,
     5,0,0,5,5,5,5,5,5,5,0,0,4,
     5,5,0,0,0,0,0,0,0,0,0,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,0,0,5,5,5,5,5,0,0,4,4,
     5,0,0,5,5,5,5,5,5,5,0,0,4,
     0,0,5,5,5,5,5,5,5,5,5,0,0, 0
   },
   { 0,DIGITWIDTH, 0,DIGITHEIGHT,
     0,0,4,4,4,4,4,4,4,4,4,0,0,
     4,0,0,4,4,4,4,4,4,4,0,0,4,
     4,4,0,0,4,4,4,4,4,0,0,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,0,0,0,0,0,0,0,0,0,4,4,
     4,0,0,4,4,4,4,4,4,4,0,0,4,
     0,0,4,4,4,4,4,4,4,4,4,0,0,
     4,0,0,4,4,4,4,4,4,4,0,0,4,
     4,4,0,0,0,0,0,0,0,0,0,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,0,0,4,4,4,4,4,0,0,4,4,
     4,0,0,4,4,4,4,4,4,4,0,0,4,
     0,0,4,4,4,4,4,4,4,4,4,0,0, 0
   },
   { 0,DIGITWIDTH, 0,DIGITHEIGHT,
     0,0,4,4,4,4,4,4,4,4,4,0,0,
     4,0,0,4,4,4,4,4,4,4,0,0,4,
     4,4,0,0,4,4,4,4,4,0,0,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,4,0,0,0,0,0,0,0,4,4,4,
     4,4,0,0,0,0,0,0,0,0,0,4,4,
     4,0,0,4,4,4,4,4,4,4,0,0,4,
     0,0,4,4,4,4,4,4,4,4,4,0,0,
     5,0,0,4,4,4,4,4,4,4,0,0,4,
     5,5,0,0,0,0,0,0,0,0,0,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,5,0,0,0,0,0,0,0,4,4,4,
     5,5,0,0,4,4,4,4,4,0,0,4,4,
     5,0,0,4,4,4,4,4,4,4,0,0,4,
     0,0,4,4,4,4,4,4,4,4,4,0,0, 0
   },
   { 0,DIGITWIDTH, 0,DIGITHEIGHT,
     0,0,5,5,5,5,5,5,5,5,5,0,0,
     5,0,0,5,5,5,5,5,5,5,0,0,5,
     5,5,0,0,5,5,5,5,5,0,0,5,5,
     5,5,5,0,0,0,0,0,0,0,5,5,5,
     5,5,5,0,0,0,0,0,0,0,5,5,5,
     5,5,5,0,0,0,0,0,0,0,5,5,5,
     5,5,5,0,0,0,0,0,0,0,5,5,5,
     5,5,5,0,0,0,0,0,0,0,5,5,5,
     5,5,0,0,0,0,0,0,0,0,0,5,5,
     5,0,0,5,5,5,5,5,5,5,0,0,5,
     0,0,5,5,5,5,5,5,5,5,5,0,0,
     5,0,0,5,5,5,5,5,5,5,0,0,5,
     5,5,0,0,0,0,0,0,0,0,0,5,5,
     5,5,5,0,0,0,0,0,0,0,5,5,5,
     5,5,5,0,0,0,0,0,0,0,5,5,5,
     5,5,5,0,0,0,0,0,0,0,5,5,5,
     5,5,5,0,0,0,0,0,0,0,5,5,5,
     5,5,5,0,0,0,0,0,0,0,5,5,5,
     5,5,0,0,5,5,5,5,5,0,0,5,5,
     5,0,0,5,5,5,5,5,5,5,0,0,5,
     0,0,5,5,5,5,5,5,5,5,5,0,0, 0
   }
};

UBYTE   calm_data[] = {
   0,16, 0,14,
   0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,
   0,0,0,1,1,6,6,6,6,6,6,1,1,0,0,0,
   0,0,1,6,6,6,6,6,6,6,6,6,6,1,0,0,
   0,1,6,6,6,6,6,6,6,6,6,6,6,6,1,0,
   0,1,6,6,6,1,1,6,6,1,1,6,6,6,1,0,
   1,6,6,6,6,1,1,6,6,1,1,6,6,6,6,1,
   1,6,6,6,6,1,1,6,6,1,1,6,6,6,6,1,
   1,6,6,6,6,6,6,6,6,6,6,6,6,6,6,1,
   1,6,6,6,6,6,6,6,6,6,6,6,6,6,6,1,
   0,1,6,6,6,1,6,6,6,6,1,6,6,6,1,0,
   0,1,6,6,6,6,1,1,1,1,6,6,6,6,1,0,
   0,0,1,6,6,6,6,6,6,6,6,6,6,1,0,0,
   0,0,0,1,1,6,6,6,6,6,6,1,1,0,0,0,
   0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0
};

UBYTE   afraid_data[] = {
   0,16, 0,14,
   0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,
   0,0,0,1,1,6,6,6,6,6,6,1,1,0,0,0,
   0,0,1,6,6,6,6,6,6,6,6,6,6,1,0,0,
   0,1,6,6,6,1,1,6,6,1,1,6,6,6,1,0,
   0,1,6,6,1,6,6,6,6,6,6,1,6,6,1,0,
   1,6,6,6,6,1,1,6,6,1,1,6,6,6,6,1,
   1,6,6,6,6,1,1,6,6,1,1,6,6,6,6,1,
   1,6,6,6,6,6,6,6,6,6,6,6,6,6,6,1,
   1,6,6,6,6,6,6,6,6,6,6,6,6,6,6,1,
   0,1,6,6,6,6,1,1,1,1,6,6,6,6,1,0,
   0,1,6,6,6,1,1,1,1,1,1,6,6,6,1,0,
   0,0,1,6,6,6,6,6,6,6,6,6,6,1,0,0,
   0,0,0,1,1,6,6,6,6,6,6,1,1,0,0,0,
   0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0
};

UBYTE    happy_data[] = {
   0,16, 0,14,
   0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,
   0,0,0,1,1,6,6,6,6,6,6,1,1,0,0,0,
   0,0,1,6,6,6,6,6,6,6,6,6,6,1,0,0,
   0,1,6,6,6,6,6,6,6,6,6,6,6,6,1,0,
   0,1,6,1,1,1,1,1,1,1,1,1,1,6,1,0,
   1,6,6,6,1,1,1,6,6,1,1,1,6,6,6,1,
   1,6,6,6,1,1,1,6,6,1,1,1,6,6,6,1,
   1,6,6,6,6,6,6,6,6,6,6,6,6,6,6,1,
   1,6,6,6,1,6,6,6,6,6,6,1,6,6,6,1,
   0,1,6,6,6,1,1,1,1,1,1,6,6,6,1,0,
   0,1,6,6,6,6,1,1,1,1,6,6,6,6,1,0,
   0,0,1,6,6,6,6,6,6,6,6,6,6,1,0,0,
   0,0,0,1,1,6,6,6,6,6,6,1,1,0,0,0,
   0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0
};

UBYTE   dead_data[] = {
   0,16, 0,14,
   0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,
   0,0,0,1,1,6,6,6,6,6,6,1,1,0,0,0,
   0,0,1,6,6,6,6,6,6,6,6,6,6,1,0,0,
   0,1,6,6,6,6,6,6,6,6,6,6,6,6,1,0,
   0,1,6,6,1,6,1,6,6,1,6,1,6,6,1,0,
   1,6,6,6,6,1,6,6,6,6,1,6,6,6,6,1,
   1,6,6,6,1,6,1,6,6,1,6,1,6,6,6,1,
   1,6,6,6,6,6,6,6,6,6,6,6,6,6,6,1,
   1,6,6,6,6,6,6,6,6,6,6,6,6,6,6,1,
   0,1,6,6,6,6,6,1,1,6,6,6,6,6,1,0,
   0,1,6,6,6,6,1,6,6,1,6,6,6,6,1,0,
   0,0,1,6,6,6,6,6,6,6,6,6,6,1,0,0,
   0,0,0,1,1,6,6,6,6,6,6,1,1,0,0,0,
   0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0
};

static UBYTE   bigmine_data[] = {
   0,15, 0,15,
   0,0,0,0,9,1,1,1,1,1,0,0,0,0,0,
   1,1,8,1,1,1,1,1,1,1,1,1,8,1,1,
   1,8,1,1,1,1,1,1,1,1,1,1,1,8,1,
   8,1,1,8,7,8,1,1,1,1,1,1,1,1,8,
   9,1,8,7,2,7,8,1,1,1,1,1,1,1,0,
   1,1,1,8,7,8,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,1,1,1,9,1,
   1,1,8,8,1,1,1,1,1,1,1,7,8,9,1,
   0,1,1,1,7,1,1,1,1,1,8,1,1,1,0,
   0,1,1,1,8,1,1,1,1,1,8,1,1,1,8,
   1,1,1,1,1,1,1,1,1,9,9,9,1,1,1,
   1,1,8,1,1,1,1,9,9,1,1,1,8,1,1,
   0,0,0,0,0,1,1,1,1,1,0,0,0,0,0
};

/* Palette for images */
ULONG   image_palette[NUM_IMAGECOLORS][3] = {
   { 0x00000000, 0x00000000, 0x00000000 },   /* black */
   { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },   /* white */
   { 0xE1E1E1E1, 0x00000000, 0x53535353 },   /* red */
   { 0x83838383, 0x00000000, 0x30303030 },   /* brown */
   { 0xEEEEEEEE, 0xDDDDDDDD, 0x00000000 },   /* yellow */
   { 0xD2D2D2D2, 0xD2D2D2D2, 0xD2D2D2D2 },   /* lightgray */
   { 0x82828282, 0x82828282, 0x82828282 },   /* gray */
   { 0x48484848, 0x43434343, 0xA7A7A7A7 }    /* blue */
};

/* Pens for images */
static LONG    palette_pens[NUM_IMAGECOLORS];   /* obtained pens */
static UWORD   real_pens[NUM_IMAGEPENS + 1];    /* pens to use for real */
static UWORD   def_pens[NUM_IMAGEPENS + 1];     /* default image pens */

/* The image class */
static Class  *chunkyiclass;

/* The image objects */
struct Image  *flag_image;
struct Image  *mine_image;
struct Image  *digit_images[NUM_DIGITS];
struct Image  *bigmine_image;
struct Image  *face_image;


/* Initialization of game images */
BOOL                                 /* success? */
init_images (
   struct Screen  *scr,              /* screen to use images on */
   BOOL            create_digits,    /* create digit images? */
   BOOL            display_colors)   /* colorfull images? */
{
   register UBYTE   n;
   BOOL             success = TRUE;
   
   
   /* Initialize the pens */
   for (n = 0; n < NUM_IMAGECOLORS; ++n)
   {
      if (GfxBase->LibNode.lib_Version >= 39L)
      {
         palette_pens[n] = ObtainBestPen (scr->ViewPort.ColorMap,
                                          image_palette[n][0],
                                          image_palette[n][1],
                                          image_palette[n][2],
                                          OBP_Precision, PRECISION_IMAGE,
                                          TAG_DONE);
      }
      else
         palette_pens[n] = -1;
   }
   
   real_pens[0] = (palette_pens[0] == -1) ?
                  gui_pens[TEXTPEN] : palette_pens[0];
   def_pens[0] = gui_pens[TEXTPEN];
   real_pens[1] = (palette_pens[1] == -1) ?
                  gui_pens[SHINEPEN] : palette_pens[1];
   def_pens[1] = gui_pens[SHINEPEN];
   real_pens[2] = (palette_pens[2] == -1) ?
                  gui_pens[FILLPEN] : palette_pens[2];
   def_pens[2] = gui_pens[FILLPEN];
   real_pens[3] = (palette_pens[2] == -1) ?
                  gui_pens[SHINEPEN] : palette_pens[2];
   if (real_pens[3] == real_pens[0])
      real_pens[3] = gui_pens[BACKGROUNDPEN];
   def_pens[3] = gui_pens[SHINEPEN];
   if (def_pens[3] == def_pens[0])
      def_pens[3] = gui_pens[BACKGROUNDPEN];
   real_pens[4] = (palette_pens[3] == -1) ?
                  gui_pens[SHADOWPEN] : palette_pens[3];
   if (real_pens[4] == real_pens[2] || real_pens[4] == real_pens[3])
   {
      if (palette_pens[3] != -1)
         ReleasePen (scr->ViewPort.ColorMap, palette_pens[3]);
      palette_pens[3] = -1;
      real_pens[4] = gui_pens[TEXTPEN];
   }
   def_pens[4] = gui_pens[SHADOWPEN];
   if (def_pens[4] == def_pens[2] || def_pens[4] == def_pens[3])
      def_pens[4] = gui_pens[TEXTPEN];
   real_pens[5] = (palette_pens[4] == -1) ?
                  gui_pens[SHINEPEN] : palette_pens[4];
   if (real_pens[5] == real_pens[0])
   {
      if (palette_pens[4] != -1)
         ReleasePen (scr->ViewPort.ColorMap, palette_pens[4]);
      palette_pens[4] = -1;
      real_pens[5] = gui_pens[BACKGROUNDPEN];
   }
   def_pens[5] = gui_pens[SHINEPEN];
   if (def_pens[5] == def_pens[0])
      def_pens[5] = gui_pens[BACKGROUNDPEN];
   real_pens[6] = (palette_pens[5] == -1) ?
                  gui_pens[BACKGROUNDPEN] : palette_pens[5];
   def_pens[6] = gui_pens[BACKGROUNDPEN];
   real_pens[7] = (palette_pens[6] == -1) ?
                  gui_pens[SHADOWPEN] : palette_pens[6];
   def_pens[7] = gui_pens[SHADOWPEN];
   real_pens[8] = (palette_pens[7] == -1) ?
                  gui_pens[FILLPEN] : palette_pens[7];
   def_pens[8] = gui_pens[FILLPEN];
   def_pens[9] = real_pens[9] = (UWORD)~0;
   
   /* Create image objects */
   if (chunkyiclass = init_chunkyiclass ())
   {
      flag_image = NewObject (chunkyiclass, NULL,
                              IA_Left, LINEWIDTH *
                                       ((cell_space == NORMAL_SPACE) ? 2 : 1),
                              IA_Top, LINEHEIGHT *
                                      ((cell_space == NORMAL_SPACE) ? 2 : 1),
                              IA_Width, cell_w - LINEWIDTH *
                                        ((cell_space == NORMAL_SPACE) ? 4 : 2),
                              IA_Height, cell_h - LINEHEIGHT *
                                        ((cell_space == NORMAL_SPACE) ? 4 : 2),
                              IA_Data, flag_data,
                              IA_Pens, (display_colors) ?
                                       real_pens : def_pens,
                              CHUNKYIA_Screen, scr,
                              TAG_DONE);
      if (flag_image == NULL)
         success = FALSE;
      
      mine_image = NewObject (chunkyiclass, NULL,
                              IA_Left, LINEWIDTH *
                                       ((cell_space == NORMAL_SPACE) ? 2 : 1),
                              IA_Top, LINEHEIGHT *
                                      ((cell_space == NORMAL_SPACE) ? 2 : 1),
                              IA_Width, cell_w - LINEWIDTH *
                                        ((cell_space == NORMAL_SPACE) ? 4 : 2),
                              IA_Height, cell_h - LINEHEIGHT *
                                        ((cell_space == NORMAL_SPACE) ? 4 : 2),
                              IA_Data, mine_data,
                              IA_Pens, (display_colors) ?
                                       real_pens : def_pens,
                              CHUNKYIA_Screen, scr,
                              TAG_DONE);
      if (mine_image == NULL)
         success = FALSE;
      
      for (n = 0; n < NUM_DIGITS; ++n)
      {
         if (create_digits)
         {
            digit_images[n] = NewObject (chunkyiclass, NULL,
                                         IA_Data, digit_data[n],
                                         IA_BGPen, real_pens[0],
                                         IA_Pens, (display_colors) ?
                                                  real_pens : def_pens,
                                         CHUNKYIA_Screen, scr,
                                         TAG_DONE);
            if (digit_images[n] == NULL)
               success = FALSE;
         }
         else
            digit_images[n] = NULL;
      }
      bigmine_image = NewObject (chunkyiclass, NULL,
                                 IA_Data, bigmine_data,
                                 IA_Pens, (display_colors) ?
                                          real_pens : def_pens,
                                 CHUNKYIA_Screen, scr,
                                 TAG_DONE);
      if (bigmine_image == NULL)
         success = FALSE;
      
      if (create_digits)
      {
         face_image = NewObject (chunkyiclass, NULL,
                                 IA_Data, calm_data,
                                 CHUNKYIA_SelectedData, afraid_data,
                                 IA_Pens, (display_colors) ?
                                          real_pens : def_pens,
                                 CHUNKYIA_Screen, scr,
                                 TAG_DONE);
         if (face_image == NULL)
            success = FALSE;
      }
      else
         face_image = NULL;
   }
   else
      success = FALSE;
   
   return success;
}


/* Change image colors */
void
update_images (
   BOOL   display_colors)   /* colorfull images? */
{
   SetAttrs (flag_image,
             IA_Pens, (display_colors) ? real_pens : def_pens, TAG_DONE);
   SetAttrs (mine_image,
             IA_Pens, (display_colors) ? real_pens : def_pens, TAG_DONE);
   if (digit_images[0])
   {
      register UBYTE   n;
      
      for (n = 0; n < NUM_DIGITS; ++n)
      {
         SetAttrs (digit_images[n],
                   IA_Pens, (display_colors) ? real_pens : def_pens,
                   TAG_DONE);
      }

      SetAttrs (face_image,
                IA_Pens, (display_colors) ? real_pens : def_pens,
                TAG_DONE);
   }
   SetAttrs (bigmine_image,
             IA_Pens, (display_colors) ? real_pens : def_pens, TAG_DONE);
}


/* Finalization of game images */
void
free_images (
   struct Screen  *scr)   /* screen that images were used on */
{
   register UBYTE   n;
   
   
   /* Free image objects */
   if (chunkyiclass != NULL)
   {
      if (flag_image != NULL)
         DisposeObject (flag_image);
      if (mine_image != NULL)
         DisposeObject (mine_image);
      for (n = 0; n < NUM_DIGITS; ++n)
      {
         if (digit_images[n] != NULL)
            DisposeObject (digit_images[n]);
      }
      if (bigmine_image != NULL)
         DisposeObject (bigmine_image);
      if (face_image != NULL)
         DisposeObject (face_image);
      
      free_chunkyiclass (chunkyiclass);
   }
   
   
   /* Release pens */
   for (n = 0; n < NUM_IMAGECOLORS; ++n)
   {
      if (palette_pens[n] != -1)
         ReleasePen (scr->ViewPort.ColorMap, palette_pens[n]);
   }
}

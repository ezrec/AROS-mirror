// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log$
// Revision 1.1  2000/02/29 18:21:05  stegerg
// Doom port based on ADoomPPC. Read README.AROS!
//
//
// DESCRIPTION:
//	Gamma correction LUT stuff.
//	Functions to draw patches (by post) directly to screen.
//	Functions to blit a block to the screen.
//
//-----------------------------------------------------------------------------

static const char
rcsid[] = "$Id$";

#include <exec/exec.h>

#ifdef AROS 
 #define MEMF_NOCACHESYNCPPC    	0
 #define MEMF_NOCACHESYNCM68K   	0
 #define PPCAllocVec(size,flags) 	I_AllocLow(size)
#else
  #include <powerup/clib/ppc_protos.h>
  #include <powerup/ppclib/memory.h>
#endif

#include "i_system.h"
#include "i_video.h"
#include "r_local.h"

#include "doomdef.h"
#include "doomstat.h"
#include "doomdata.h"

#include "m_bbox.h"
#include "m_swap.h"
#include "m_argv.h"
#include "am_map.h"
#include "r_draw.h"

#include "v_video.h"

#include "st_stuff.h"

extern int weirdaspect;

// Screen buffer memory
byte *vid_mem;

// Each screen is [SCREENWIDTH*SCREENHEIGHT]; 
byte* screens[5];	
 
int dirtybox[4]; 



// Now where did these came from?
byte gammatable[5][256] =
{
    {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
     17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,
     33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,
     49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,
     65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,
     81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,
     97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,
     113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,
     128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
     144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
     160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
     176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
     192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
     208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
     224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
     240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255},

    {2,4,5,7,8,10,11,12,14,15,16,18,19,20,21,23,24,25,26,27,29,30,31,
     32,33,34,36,37,38,39,40,41,42,44,45,46,47,48,49,50,51,52,54,55,
     56,57,58,59,60,61,62,63,64,65,66,67,69,70,71,72,73,74,75,76,77,
     78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,
     99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,
     115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,129,
     130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,
     146,147,148,148,149,150,151,152,153,154,155,156,157,158,159,160,
     161,162,163,163,164,165,166,167,168,169,170,171,172,173,174,175,
     175,176,177,178,179,180,181,182,183,184,185,186,186,187,188,189,
     190,191,192,193,194,195,196,196,197,198,199,200,201,202,203,204,
     205,205,206,207,208,209,210,211,212,213,214,214,215,216,217,218,
     219,220,221,222,222,223,224,225,226,227,228,229,230,230,231,232,
     233,234,235,236,237,237,238,239,240,241,242,243,244,245,245,246,
     247,248,249,250,251,252,252,253,254,255},

    {4,7,9,11,13,15,17,19,21,22,24,26,27,29,30,32,33,35,36,38,39,40,42,
     43,45,46,47,48,50,51,52,54,55,56,57,59,60,61,62,63,65,66,67,68,69,
     70,72,73,74,75,76,77,78,79,80,82,83,84,85,86,87,88,89,90,91,92,93,
     94,95,96,97,98,100,101,102,103,104,105,106,107,108,109,110,111,112,
     113,114,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,
     129,130,131,132,133,133,134,135,136,137,138,139,140,141,142,143,144,
     144,145,146,147,148,149,150,151,152,153,153,154,155,156,157,158,159,
     160,160,161,162,163,164,165,166,166,167,168,169,170,171,172,172,173,
     174,175,176,177,178,178,179,180,181,182,183,183,184,185,186,187,188,
     188,189,190,191,192,193,193,194,195,196,197,197,198,199,200,201,201,
     202,203,204,205,206,206,207,208,209,210,210,211,212,213,213,214,215,
     216,217,217,218,219,220,221,221,222,223,224,224,225,226,227,228,228,
     229,230,231,231,232,233,234,235,235,236,237,238,238,239,240,241,241,
     242,243,244,244,245,246,247,247,248,249,250,251,251,252,253,254,254,
     255},

    {8,12,16,19,22,24,27,29,31,34,36,38,40,41,43,45,47,49,50,52,53,55,
     57,58,60,61,63,64,65,67,68,70,71,72,74,75,76,77,79,80,81,82,84,85,
     86,87,88,90,91,92,93,94,95,96,98,99,100,101,102,103,104,105,106,107,
     108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,
     125,126,127,128,129,130,131,132,133,134,135,135,136,137,138,139,140,
     141,142,143,143,144,145,146,147,148,149,150,150,151,152,153,154,155,
     155,156,157,158,159,160,160,161,162,163,164,165,165,166,167,168,169,
     169,170,171,172,173,173,174,175,176,176,177,178,179,180,180,181,182,
     183,183,184,185,186,186,187,188,189,189,190,191,192,192,193,194,195,
     195,196,197,197,198,199,200,200,201,202,202,203,204,205,205,206,207,
     207,208,209,210,210,211,212,212,213,214,214,215,216,216,217,218,219,
     219,220,221,221,222,223,223,224,225,225,226,227,227,228,229,229,230,
     231,231,232,233,233,234,235,235,236,237,237,238,238,239,240,240,241,
     242,242,243,244,244,245,246,246,247,247,248,249,249,250,251,251,252,
     253,253,254,254,255},

    {16,23,28,32,36,39,42,45,48,50,53,55,57,60,62,64,66,68,69,71,73,75,76,
     78,80,81,83,84,86,87,89,90,92,93,94,96,97,98,100,101,102,103,105,106,
     107,108,109,110,112,113,114,115,116,117,118,119,120,121,122,123,124,
     125,126,128,128,129,130,131,132,133,134,135,136,137,138,139,140,141,
     142,143,143,144,145,146,147,148,149,150,150,151,152,153,154,155,155,
     156,157,158,159,159,160,161,162,163,163,164,165,166,166,167,168,169,
     169,170,171,172,172,173,174,175,175,176,177,177,178,179,180,180,181,
     182,182,183,184,184,185,186,187,187,188,189,189,190,191,191,192,193,
     193,194,195,195,196,196,197,198,198,199,200,200,201,202,202,203,203,
     204,205,205,206,207,207,208,208,209,210,210,211,211,212,213,213,214,
     214,215,216,216,217,217,218,219,219,220,220,221,221,222,223,223,224,
     224,225,225,226,227,227,228,228,229,229,230,230,231,232,232,233,233,
     234,234,235,235,236,236,237,237,238,239,239,240,240,241,241,242,242,
     243,243,244,244,245,245,246,246,247,247,248,248,249,249,250,250,251,
     251,252,252,253,254,254,255,255}
};



int	usegamma;
			 
//
// V_MarkRect 
// 
/*
void
V_MarkRect
( int		x,
  int		y,
  int		width,
  int		height ) 
{ 
    M_AddToBox (dirtybox, x, y); 
    M_AddToBox (dirtybox, x+width-1, y+height-1); 
} 
*/


//
// V_CopyRect 
// 
void
V_CopyRect
( int		srcx,
  int		srcy,
  int		srcscrn,
  int		width,
  int		height,
  int		destx,
  int		desty,
  int		destscrn ) 
{ 
    byte*	src;
    byte*	dest; 
	 
#ifdef RANGECHECK 
    if (srcx<0
	||srcx+width >SCREENWIDTH
	|| srcy<0
	|| srcy+height>SCREENHEIGHT 
	||destx<0||destx+width >SCREENWIDTH
	|| desty<0
	|| desty+height>SCREENHEIGHT 
	|| (unsigned)srcscrn>4
	|| (unsigned)destscrn>4)
    {
	I_Error ("Bad V_CopyRect");
    }
#endif 
    src = screens[srcscrn]+SCREENWIDTH*srcy+srcx; 
    dest = screens[destscrn]+SCREENWIDTH*desty+destx; 

    for ( ; height>0 ; height--) 
    { 
	memcpy (dest, src, width); 
	src += SCREENWIDTH; 
	dest += SCREENWIDTH; 
    } 

    if (destscrn == 0)
        I_MarkRect (destx, desty, width, height);
} 
 

//
// V_DrawPatch
// Masks a column based masked pic to the screen. 
//
void
V_DrawPatch
( int		x,
  int		y,
  int		scrn,
  patch_t*	patch ) 
{ 

    int		count;
    int		col; 
    column_t*	column; 
    byte*	desttop;
    byte*	dest;
    byte*	source; 
    int		w; 
    int		x0;
	 
    y -= SWAPSHORT(patch->topoffset); 
    x -= SWAPSHORT(patch->leftoffset); 
#ifdef RANGECHECK 
    if (x<0
	||x+SWAPSHORT(patch->width) >SCREENWIDTH
	|| y<0
	|| y+SWAPSHORT(patch->height)>SCREENHEIGHT 
	|| (unsigned)scrn>4)
    {
      fprintf( stderr, "Patch at %d,%d exceeds LFB\n", x,y );
      // No I_Error abort - what is up with TNT.WAD?
      fprintf( stderr, "V_DrawPatch: bad patch (ignored)\n");
      return;
    }
#endif 
 
    x0 = x;
    col = 0; 
    desttop = screens[scrn]+y*SCREENWIDTH+x; 
	 
    w = SWAPSHORT(patch->width); 

    for ( ; col<w ; x++, col++, desttop++)
    { 
	column = (column_t *)((byte *)patch + SWAPLONG(patch->columnofs[col])); 
 
	// step through the posts in a column 
	while (column->topdelta != 0xff ) 
	{ 
	    source = (byte *)column + 3; 
	    dest = desttop + column->topdelta*SCREENWIDTH; 
	    count = column->length; 
			 
	    while (count--) 
	    { 
		*dest = *source++; 
		dest += SCREENWIDTH; 
	    } 
	    column = (column_t *)(  (byte *)column + column->length 
				    + 4 ); 
	} 
    }			 
    if (scrn == 0)
	I_MarkRect (x0, y, SWAPSHORT(patch->width), SWAPSHORT(patch->height));
} 
 
//
// V_DrawPatchFlipped 
// Masks a column based masked pic to the screen.
// Flips horizontally, e.g. to mirror face.
//
void
V_DrawPatchFlipped
( int		x,
  int		y,
  int		scrn,
  patch_t*	patch ) 
{ 

    int		count;
    int		col; 
    column_t*	column; 
    byte*	desttop;
    byte*	dest;
    byte*	source; 
    int		w; 
    int		x0;
	 
    y -= SWAPSHORT(patch->topoffset); 
    x -= SWAPSHORT(patch->leftoffset); 
#ifdef RANGECHECK 
    if (x<0
	||x+SWAPSHORT(patch->width) >SCREENWIDTH
	|| y<0
	|| y+SWAPSHORT(patch->height)>SCREENHEIGHT 
	|| (unsigned)scrn>4)
    {
      fprintf( stderr, "Patch origin %d,%d exceeds LFB\n", x,y );
      I_Error ("Bad V_DrawPatch in V_DrawPatchFlipped");
    }
#endif 
 
    x0 = x;
    col = 0; 
    desttop = screens[scrn]+y*SCREENWIDTH+x; 
	 
    w = SWAPSHORT(patch->width); 

    for ( ; col<w ; x++, col++, desttop++) 
    { 
	column = (column_t *)((byte *)patch + SWAPLONG(patch->columnofs[w-1-col])); 
 
	// step through the posts in a column 
	while (column->topdelta != 0xff ) 
	{ 
	    source = (byte *)column + 3; 
	    dest = desttop + column->topdelta*SCREENWIDTH; 
	    count = column->length; 
			 
	    while (count--) 
	    { 
		*dest = *source++; 
		dest += SCREENWIDTH; 
	    } 
	    column = (column_t *)(  (byte *)column + column->length 
				    + 4 ); 
	} 
    }			 
    if (scrn == 0)
	I_MarkRect (x0, y, SWAPSHORT(patch->width), SWAPSHORT(patch->height));
} 
 


//
// V_DrawPatchDirect
// Draws directly to the screen on the pc. 
//
void
V_DrawPatchDirect
( int		x,
  int		y,
  int		scrn,
  patch_t*	patch ) 
{
    V_DrawPatch (x,y,scrn, patch); 
}

void	//stretches bitmap to fill screen
V_DrawPatchInDirect
( int		x,
  int		y,
  int		scrn,
  patch_t*	patch	)
{
    int		count;
    int		col; 
    column_t*	column; 
    byte*	desttop;
    byte*	dest;
    byte*	source; 
    int		w; 
    int		x0;

  int deltax,deltay,deltaxi,deltayi,stretchx,stretchy;
  int srccol,collen;
	 
  y -= SWAPSHORT(patch->topoffset); 
  x -= SWAPSHORT(patch->leftoffset); 

#ifdef RANGECHECK 
  if (x<0
      ||x+SWAPSHORT(patch->width)>320
      || y<0
      || y+SWAPSHORT(patch->height)>200
      || (unsigned)scrn>4)
    {
	fprintf(	stderr, "Patch at %d,%d exceeds LFB\n", x,y );
	// No I_Error abort - what is up with TNT.WAD?
	fprintf(	stderr, "V_DrawPatch: bad patch (ignored)\n");
	return;
    }
#endif 

  deltax=(SCREENWIDTH<<16)/320;
  deltaxi=(320<<16)/SCREENWIDTH;
  deltay=(SCREENHEIGHT<<16)/200;
  deltayi=(200<<16)/SCREENHEIGHT;
  stretchx=(x*deltax)>>16;
  stretchy=(y*deltay)>>16;

  x0 = x;
  col = 0;
  desttop = screens[scrn]+stretchy*SCREENWIDTH+stretchx;
	 
  w = SWAPSHORT(patch->width)<<16;

  for ( ; col < w; x++, col += deltaxi, desttop++) {
    column = (column_t *)((byte *)patch + SWAPLONG(patch->columnofs[col>>16]));

    // step through the posts in a column
    while (column->topdelta != 0xff ) {
      source = (byte *)column +	3;
      dest = desttop+((column->topdelta*deltay)>>16)*SCREENWIDTH;
      collen = count = (column->length*deltay)>>16;
      srccol = 0;
      while (count--) {
        *dest = source[srccol>>16];
        dest += SCREENWIDTH;
        srccol += deltayi;
      }
      column = (column_t *)((byte *)column + (column->length)+4);
    }
  }
  if (scrn == 0)
      I_MarkRect (x0, y, SWAPSHORT(patch->width), SWAPSHORT(patch->height)); 
}


void	//stretches bitmap to fill screen
V_DrawPatchInDirectFlipped
( int		x,
  int		y,
  int		scrn,
  patch_t*	patch	)
  {
  int		count;
  int		col;
  column_t*	column;
  byte*	desttop;
  byte*	dest;
  byte*	source;
  int		w;

  int deltax,deltay,deltaxi,deltayi,stretchx,stretchy;
  int srccol,collen;
	 
  y	-=	SWAPSHORT(patch->topoffset);
  x	-=	SWAPSHORT(patch->leftoffset);
#ifdef RANGECHECK
  if (x<0
      ||x+SWAPSHORT(patch->width)	>320
      ||	y<0
      ||	y+SWAPSHORT(patch->height)>200
      ||	(unsigned)scrn>4)
  {
    fprintf(	stderr, "Patch at %d,%d exceeds LFB\n", x,y );
    // No I_Error abort - what is up with TNT.WAD?
    fprintf(	stderr, "V_DrawPatch: bad patch (ignored)\n");
    return;
  }
#endif 

  deltax=(SCREENWIDTH<<16)/320;
  deltaxi=(320<<16)/SCREENWIDTH;
  deltay=(SCREENHEIGHT<<16)/200;
  deltayi=(200<<16)/SCREENHEIGHT;
  stretchx=(x*deltax)>>16;
  stretchy=(y*deltay)>>16;

  col = 0;
  desttop	= screens[scrn]+stretchy*SCREENWIDTH+stretchx;
 
  w = SWAPSHORT(patch->width)<<16;

  for ( ; col < w; x++, col += deltaxi, desttop++) {
    column = (column_t *)((byte *)patch	+
               SWAPLONG(patch->columnofs[SWAPSHORT(patch->width)-1-(col>>16)]));

    // step through the posts in a column
    while (column->topdelta != 0xff) {
      source = (byte *)column +	3;
      dest = desttop+((column->topdelta*deltay)>>16)*SCREENWIDTH;
      collen = count = (column->length*deltay)>>16;
      srccol=0;
      while (count--) {
        *dest=source[srccol>>16];
        dest += SCREENWIDTH;
        srccol+=deltayi;
      }
      column = (column_t *)((byte *)column+(column->length)+4);
    }
  }
  if (scrn == 0)
      I_MarkRect (stretchx,stretchy,(SWAPSHORT(patch->width)*deltax)>>16,
                  (SWAPSHORT(patch->height)*deltax)>>16);
}



//
// V_DrawBlock
// Draw a linear block of pixels into the view buffer.
//
void
V_DrawBlock
( int		x,
  int		y,
  int		scrn,
  int		width,
  int		height,
  byte*		src ) 
{ 
    byte*	dest; 
    int		height0;
	 
#ifdef RANGECHECK 
    if (x<0
	||x+width >SCREENWIDTH
	|| y<0
	|| y+height>SCREENHEIGHT 
	|| (unsigned)scrn>4 )
    {
	I_Error ("Bad V_DrawBlock");
    }
#endif 
 
    height0 = height;
    dest = screens[scrn] + y*SCREENWIDTH+x; 

    while (height--) 
    { 
	memcpy (dest, src, width); 
	src += width; 
	dest += SCREENWIDTH; 
    } 
    if (scrn == 0)
        I_MarkRect (x, y, width, height0);
}
 


//
// V_GetBlock
// Gets a linear block of pixels from the view buffer.
//
void
V_GetBlock
( int		x,
  int		y,
  int		scrn,
  int		width,
  int		height,
  byte*		dest ) 
{ 
    byte*	src; 
	 
#ifdef RANGECHECK 
    if (x<0
	||x+width >SCREENWIDTH
	|| y<0
	|| y+height>SCREENHEIGHT 
	|| (unsigned)scrn>4 )
    {
	I_Error ("Bad V_DrawBlock");
    }
#endif 
 
    src = screens[scrn] + y*SCREENWIDTH+x; 

    while (height--) 
    { 
	memcpy (dest, src, width); 
	src += SCREENWIDTH; 
	dest += width; 
    } 
} 


#ifdef GRAFFITI
extern int video_graffiti;
#endif


//
// V_Init
// 
void V_Init (void) 
{ 
    int i;
    int p;
    int vid_flags;
    int vid_size;

#ifdef GRAFFITI
    video_graffiti = 0;
    if (M_CheckParm ("-graffitilo")) {
      video_graffiti = 1;
      SCREENWIDTH = 320;
      SCREENHEIGHT = 200;
    } else if (M_CheckParm ("-graffitihi")) {
      video_graffiti = 2;
      SCREENWIDTH = 640;
      SCREENHEIGHT = 400;
    } else {
#endif

     p = M_CheckParm ("-width");
      if (p && p < myargc-1)
        SCREENWIDTH = atoi(myargv[p+1]);
      else
        SCREENWIDTH = 320;

      p = M_CheckParm ("-height");
      if (p && p < myargc-1)
        SCREENHEIGHT = atoi(myargv[p+1]);
      else
        SCREENHEIGHT = 200;

#ifdef GRAFFITI
    }
#endif

    printf ("Resolution: %d x %d\n", SCREENWIDTH, SCREENHEIGHT);

    if (SCREENWIDTH < 320 || SCREENWIDTH > MAXSCREENWIDTH ||
        SCREENWIDTH % 32 != 0 ||
        SCREENHEIGHT < 200 || SCREENHEIGHT > MAXSCREENHEIGHT)
      I_Error ("Resolution out of range or width is not a multiple of 32");

    if ((SCREENHEIGHT*8) == (SCREENWIDTH*5))	  // 640x400, 320x200
      weirdaspect = 1;
    else if ((SCREENHEIGHT*4) == (SCREENWIDTH*3)) // 640x480, 320x240
      weirdaspect = 0;
    else {
      weirdaspect = 0;
      printf("Warning: Bad aspect ratio, things might look screwy\n");
    }


    //allocate all the res-dependant vars
    resinit_r_plane ();
    resinit_am_map ();
    resinit_r_draw ();
    xtoviewangle = (angle_t *)I_calloc (SCREENWIDTH+1, sizeof(angle_t));
    negonearray = (short *)I_calloc (SCREENWIDTH, sizeof(short));
    screenheightarray = (short *)I_calloc (SCREENWIDTH, sizeof(short));

    // stick these in low dos memory on PCs

    vid_flags = MEMF_CLEAR+MEMF_FAST+MEMF_PUBLIC;
    if (M_CheckParm ("-mmu"))
      vid_flags += MEMF_NOCACHESYNCPPC+MEMF_NOCACHESYNCM68K;
    vid_size = SCREENWIDTH*SCREENHEIGHT*4+SCREENWIDTH*ST_HEIGHT;
    vid_mem = PPCAllocVec(vid_size + 100, vid_flags);
    if (vid_mem == NULL)
      I_Error ("Couldn't allocate memory for screens\n");
    printf ("Screen memory located at $%08x\n", vid_mem);

    for (i=0 ; i<4 ; i++)
	screens[i] = vid_mem + 100 + i*SCREENWIDTH*SCREENHEIGHT;
    screens[4] = vid_mem + SCREENWIDTH*SCREENHEIGHT*4;
}

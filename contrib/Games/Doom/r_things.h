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
// DESCRIPTION:
//	Rendering of moving objects, sprites.
//
//-----------------------------------------------------------------------------


#ifndef __R_THINGS__
#define __R_THINGS__

#include "doomdef.h"


#ifdef __GNUG__
#pragma interface
#endif

//#define MAXVISSPRITES  	128
#define MAXVISSPRITES  	256

extern FAR vissprite_t	vissprites[MAXVISSPRITES];
extern vissprite_t*	vissprite_p;
extern vissprite_t	vsprsortedhead;

// Constant arrays used for psprite clipping
//  and initializing clipping.
//extern short		negonearray[SCREENWIDTH];
//extern short		screenheightarray[SCREENWIDTH];
extern short		*negonearray;
extern short		*screenheightarray;

// vars for R_DrawMaskedColumn

//#ifdef AMIGA
//#include "amiga_macros.h"
//
//#define TYPE_NORMAL 0
//#define TYPE_FUZZ   1
//#define TYPE_TRANSL 2
//
//void REGARGS R_DrawMaskedColumnAmi (REG(a0, column_t* column), REG(d0, type));
//#endif

extern short*		mfloorclip;
extern short*		mceilingclip;
extern fixed_t		spryscale;
extern fixed_t		sprtopscreen;

extern fixed_t		pspritescale;
extern fixed_t		pspriteiscale;
extern fixed_t		pspritescale2;
extern fixed_t		pspriteiscale2;


void R_DrawMaskedColumn (column_t* column);


void R_SortVisSprites (void);

void R_AddSprites (sector_t* sec);
void R_AddPSprites (void);
void R_DrawSprites (void);
void R_InitSprites (char** namelist);
void R_ClearSprites (void);
void R_DrawMasked (void);

void
R_ClipVisSprite
( vissprite_t*		vis,
  int			xl,
  int			xh );


#endif
//-----------------------------------------------------------------------------
//
// $Log$
// Revision 1.1  2000/02/29 18:21:06  stegerg
// Doom port based on ADoomPPC. Read README.AROS!
//
//
//-----------------------------------------------------------------------------

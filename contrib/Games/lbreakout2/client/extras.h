/***************************************************************************
                          extras.h  -  description
                             -------------------
    begin                : Sun Sep 9 2001
    copyright            : (C) 2001 by Michael Speck
    email                : kulkanie@gmx.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

typedef struct {
    float       x, y;
    float       alpha;
    int         type; /* defined in lbreakout.h */
    int         offset; /* offset in extra pic */
    int         dir; /* extra will move to this direction */
    SDL_Rect    update_rect; /* screen update region */
} Extra;

/*
====================================================================
Load and delete extra graphics
====================================================================
*/
void extras_load();
void extras_delete();
/*
====================================================================
Reset extras
====================================================================
*/
void extras_reset();
/*
====================================================================
Create new extra at position
====================================================================
*/
void extra_create( int type, int x, int y, int dir );
/*
====================================================================
Use extra when paddle collected it
====================================================================
*/
void extra_use( Paddle *paddle, int type );
/*
====================================================================
Show, hide extras
====================================================================
*/
void extras_hide();
void extras_show_shadow();
void extras_show();
void extras_alphashow( int alpha );
/*
====================================================================
Update extras
====================================================================
*/
void extras_update( int ms );
/*
====================================================================
Wall
====================================================================
*/
void wall_hide();
void wall_show();
void wall_alphashow( int alpha );
void wall_update( int ms );

/***************************************************************************
                          shots.h  -  description
                             -------------------
    begin                : Sat Sep 8 2001
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
    float   x, y;
    float   cur_fr;
    Target  target;
    int     next_too; /* destroys right brick, too */
    Paddle *paddle; /* paddle that initiated the shot */
    int     dir; /* direction of shot depending on paddle */
    int     get_target; /* if this is true shots_update() will compute the target
                           and clear this flag */
    SDL_Rect update_rect; /* screen update region */
} Shot;

/*
====================================================================
Load, delete shot graphics
====================================================================
*/
void shot_load();
void shot_delete();
/*
====================================================================
Create new shot at position (centered).
'signum' of direction determines into which direction the shot 
vertically goes.
====================================================================
*/
void shot_create( Paddle *paddle );
/*
====================================================================
Delete all shots
====================================================================
*/
void shots_reset();
/*
====================================================================
Hide and show shots
====================================================================
*/
void shots_hide();
void shots_show();
void shots_alphashow( int alpha );
/*
====================================================================
Update position of shots and check if bricks get destroyed.
A list of all destroyed bricks is returned (at maximum 
PADDLE_WEAPON_AMMO * 4)
====================================================================
*/
typedef struct {
    int x, y;
} Brick_Pos;
void shots_update( int ms, Brick_Pos *bricks, int *count );
/*
====================================================================
Set 'get_target' flag so target is updated next time 
'shots_update' is called. -1 means to update all shots.
====================================================================
*/
void shots_check_targets( int mx, int my );

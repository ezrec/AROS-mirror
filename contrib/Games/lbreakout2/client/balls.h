/***************************************************************************
                          balls.h  -  description
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

/* the ball struct is located in paddle.h */

/*
====================================================================
Load/delete ball graphics
====================================================================
*/
void ball_load();
void ball_delete();
/*
====================================================================
Create ball at position
====================================================================
*/
Ball* ball_create( int x, int y );
/*
====================================================================
Set a special ball property like metal ball.
====================================================================
*/
enum {
    BALL_NORMAL = 0,
    BALL_METAL,
    BALL_EXPL,
    BALL_WEAK
};
void balls_set_type( int type );
/*
====================================================================
Set chaotic behaviour (random relfection)
====================================================================
*/
void balls_set_chaos( int chaos );
/*
====================================================================
Clear ball list and attach one ball to paddle
====================================================================
*/
void balls_reset();
/*
====================================================================
Initate velocity values (for all balls one value)
====================================================================
*/
void balls_set_vel( float v_start, float v_change, float v_max );
/*
====================================================================
Show/hide all balls
====================================================================
*/
void balls_hide();
void balls_show_shadow();
void balls_show();
void balls_alphashow( int alpha );
/*
====================================================================
Update balls and detach attached balls if fire was pressed.
Return True if at least one ball was lost and return the number
of lost balls in 'top' and 'bottom'.
====================================================================
*/
int balls_update( int ms, int *top, int *bottom );
/*
====================================================================
All balls with target mx,my will have there 'get_target' flag
set True so they compute a new target next time balls_update()
is called. If 'mx' is -1 all balls will set their flag.
====================================================================
*/
void balls_check_targets(int mx, int my);
/*
====================================================================
Adjust velocity of ball to spare out any illegal values.
Add a little entropy to the vector if 'entropy' is True.
====================================================================
*/
void ball_mask_vel(Ball *b, float old_vx, int entropy );
/*
====================================================================
Get target for a ball.
input :
    Ball* b
function :
    -check if ball b hits a brick and if so:
    -compute the hitten brick in lev_map (int mx, my)
    -the reset position of the ball after destroying the brick (float x, y)
    -the time in milliseconds it takes the ball to hit the brick from its current position
    by using ball_v as velocity (int time)
    -the side at which the ball hits; might be LEFT, RIGHT, TOP, BOTTOM (int side)
    -the reflection vector (float a); if reflecting at an horizontal wall it's a = {0, 1} else a = {1, 0}
====================================================================
*/
void ball_get_target( Ball *b );
/*
====================================================================
Increase velocity acording to vel_change
====================================================================
*/
void balls_inc_vel( int ms );
/*
====================================================================
Return all balls that have ball->return_allowed True to the paddle
(if they touched this one as last).
====================================================================
*/
void balls_return( Paddle *paddle );

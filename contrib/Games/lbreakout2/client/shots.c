/***************************************************************************
                          shots.c  -  description
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

#include "lbreakout.h"
#include "config.h"
#include "levels.h"
#include "shrapnells.h"
#include "player.h"
#include "display.h"
#include "paddle.h"
#include "bricks.h"
#include "shots.h"

extern SDL_Surface *shot_pic;
extern SDL_Surface *shot_shadow;
List *shots; /* list of all shots */
int shot_w = 10;
int shot_h = 10;
int shot_fr_num = 4;
float shot_fpms = 0.01;
float shot_v_y = 0.2;
int shot_alpha = 128;
extern Brick bricks[MAP_WIDTH][MAP_HEIGHT];
extern SDL_Surface *offscreen;
extern SDL_Surface *stk_display;
extern SDL_Rect stk_drect;
extern int shadow_size;
extern Config config;
#ifdef AUDIO_ENABLED
extern StkSound *wav_shot;
#endif
extern int game_type, game_host;
extern Paddle *paddles[4];
extern int paddle_count;
extern int net_shot_fired;

/*
====================================================================
Locals
====================================================================
*/

/*
====================================================================
Compute target of shot.
====================================================================
*/
void shot_get_target( Shot *shot ) {
    int mx = (int)(shot->x + 3) / BRICK_WIDTH;
    int my = (int)(shot->y + 3 + ((shot->dir==1)?5:0) ) / BRICK_HEIGHT;
    memset( &shot->target, 0, sizeof(Target) );
    while ( bricks[mx][my].id == -1 ) 
        my += shot->dir;
    shot->target.mx = mx; shot->target.my = my;
    mx = (int)(shot->x + 6) / BRICK_WIDTH;
    if (mx != shot->target.mx) {
        my = (int)(shot->y + 3 + ((shot->dir==1)?5:0) ) / BRICK_HEIGHT;
        while(bricks[mx][my].id == -1) 
            my += shot->dir;
        if (my == shot->target.my)
            shot->next_too = 1;
        else
            if ( ( shot->dir == -1 && my > shot->target.my ) ||
                 ( shot->dir ==  1 && my < shot->target.my ) ) {
                shot->target.mx = mx;
                shot->target.my = my;
                shot->next_too = 0;
            }
    }
    shot->target.cur_tm = 0;
    shot->target.time = abs( (int)((shot->y + 3 + ((shot->dir==1)?5:0) - (shot->target.my * BRICK_HEIGHT + ((shot->dir==-1)?(BRICK_HEIGHT - 1):0) )) / fabs(shot_v_y)) );
}

/*
====================================================================
Publics
====================================================================
*/

/*
====================================================================
Load, delete shot graphics
====================================================================
*/
void shot_load()
{
    shots = list_create( LIST_AUTO_DELETE, LIST_NO_CALLBACK );
}
void shot_delete()
{
    if ( shots ) list_delete( shots ); shots = 0;
}
/*
====================================================================
Create new shot at position (centered).
'signum' of direction determines into which direction the shot 
vertically goes.
====================================================================
*/
void shot_create( Paddle *paddle )
{
    Shot *shot = calloc( 1, sizeof( Shot ) );
    shot->cur_fr = 0;
    shot->paddle = paddle;
    shot->dir = (paddle->type == PADDLE_TOP) ? 1 : -1;
    shot->x = paddle->x + ( paddle->w >> 1 ) - (shot_w >> 1);
    shot->y = paddle->y + ( paddle->h >> 1 ) - (shot_h >> 1);
    shot->get_target = 1;
    list_add( shots, shot );
#ifdef AUDIO_ENABLED
    stk_sound_play( wav_shot );
    net_shot_fired = 1;
#endif
}
/*
====================================================================
Delete all shots
====================================================================
*/
void shots_reset()
{
    list_clear( shots );
}
/*
====================================================================
Hide and show shots
====================================================================
*/
void shots_hide()
{
    ListEntry  *entry = shots->head->next;
    Shot        *shot;
    while ( entry != shots->tail ) {
        shot = entry->item;
        entry = entry->next;
        stk_surface_blit( offscreen, 
            (int)shot->x,(int)shot->y,
            shot_w + shadow_size,shot_h + shadow_size,
            stk_display, (int)shot->x,(int)shot->y );
        shot->update_rect = stk_drect;
    }
}
void shots_show()
{
    ListEntry  *entry = shots->head->next;
    Shot       *shot;
    int        x, y;
    stk_surface_clip( stk_display, 0, 0, 
        stk_display->w - BRICK_WIDTH, stk_display->h );
    while ( entry != shots->tail ) {
        shot = entry->item;
        entry = entry->next;
        x = (int)shot->x; y = (int)shot->y;
        stk_surface_alpha_blit( shot_shadow, 
            0, 0, shot_w, shot_h, 
            stk_display, x + shadow_size, y + shadow_size,
            SHADOW_ALPHA );
        stk_surface_alpha_blit( shot_pic, (int)shot->cur_fr * shot_w, 0,
            shot_w, shot_h, stk_display, x, y, shot_alpha );
        if ( shot->dir == -1 ) {
            /* movement up */
            shot->update_rect.h += shot->update_rect.y - y;
            shot->update_rect.y = y;
            if ( shot->update_rect.y < 0 ) {
                shot->update_rect.h += shot->update_rect.y;
                shot->update_rect.y = 0;
            }
        }
        else {
            /* movement down */
            shot->update_rect.h += y - shot->update_rect.y;
            if ( shot->update_rect.y + shot->update_rect.h >= stk_display->h )
                shot->update_rect.h = stk_display->h - shot->update_rect.y;
        }
        stk_display_store_rect( &shot->update_rect );
    }
    stk_surface_clip( stk_display, 0, 0, 0, 0 );
}
void shots_alphashow( int alpha )
{
    ListEntry  *entry = shots->head->next;
    Shot        *shot;
    while ( entry != shots->tail ) {
        shot = entry->item;
        stk_surface_alpha_blit( shot_pic, (int)shot->cur_fr * shot_w, 0,
            shot_w, shot_h, stk_display, (int)shot->x, (int)shot->y,
            alpha );
        stk_display_store_rect( &shot->update_rect );
        entry = entry->next;
    }
}
/*
====================================================================
Update position of shots and check if bricks get destroyed.
A list of all destroyed bricks is returned (at maximum 
PADDLE_WEAPON_AMMO * 4)
====================================================================
*/
void shots_update( int ms, Brick_Pos *bricks, int *count )
{
    int i;
    ListEntry  *entry = shots->head->next;
    Shot        *shot;
    *count = 0;
    while ( entry != shots->tail ) {
        shot = entry->item;
        if ( shot->get_target ) { /* new target? */
            shot_get_target(shot);
            shot->get_target = 0;
        }
        shot->cur_fr += ms * shot_fpms;
        if (shot->cur_fr >= shot_fr_num) shot->cur_fr -= shot_fr_num;
        shot->y += shot->dir * ms * shot_v_y;
        shot->target.cur_tm += ms;
        entry = entry->next;
        /* kill 'out of screen' shots */
        if ( shot->y + shot_h < 0 || shot->y > stk_display->h ) {
            shot->paddle->weapon_ammo++; /* give back used shot */
            stk_display_store_rect( &shot->update_rect );
            list_delete_entry( shots, entry->prev );
            continue;
        }
        /* check hits */
        if (shot->target.cur_tm > shot->target.time) {
            if ( brick_hit(shot->target.mx, shot->target.my, 0, SHR_BY_SHOT, vector_get( 0, shot->dir ), shot->paddle ) ) {
                shots_check_targets( shot->target.mx, shot->target.my );
                bricks[*count].x = shot->target.mx;
                bricks[(*count)++].y = shot->target.my;
            }
            if (shot->next_too) {
                if ( brick_hit(shot->target.mx + 1, shot->target.my, 0, SHR_BY_SHOT, vector_get( 0, shot->dir ), shot->paddle ) ) {
                    shots_check_targets(shot->target.mx + 1, shot->target.my);
                    bricks[*count].x = shot->target.mx + 1;
                    bricks[(*count)++].y = shot->target.my;
                }
            }
            shot->paddle->weapon_ammo++; /* give back used shot */
            stk_display_store_rect( &shot->update_rect );
            list_delete_entry( shots, entry->prev );
            continue;
        }
        /* in multiplayer we check if we hit the opponent if so we steal
           him a 1000 points */
        if ( game_type == GT_NETWORK )
            for ( i = 0; i < paddle_count; i++ )
                if ( paddles[i] != shot->paddle )
                if ( shot->x + shot_w > paddles[i]->x )
                if ( shot->x < paddles[i]->x + paddles[i]->w )
                if ( shot->y + shot_h > paddles[i]->y )
                if ( shot->y < paddles[i]->y + paddles[i]->h ) {
                    player_add_score( paddles[i]->player, -1000 );
                    player_add_score( shot->paddle->player, 1000 );
                    shot->paddle->weapon_ammo++;
                    stk_display_store_rect( &shot->update_rect );
                    list_delete_entry( shots, entry->prev );
                }
    }
}
/*
====================================================================
Set 'get_target' flag so target is updated next time 
'shots_update' is called. -1 means to update all shots.
====================================================================
*/
void shots_check_targets( int mx, int my )
{
    Shot        *shot;
    list_reset( shots );
    while ( ( shot = list_next( shots ) ) )
        if ( mx == -1 || (shot->target.mx == mx && shot->target.my == my) )
            shot->get_target = 1;
}

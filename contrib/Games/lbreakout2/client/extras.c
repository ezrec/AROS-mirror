/***************************************************************************
                          extras.c  -  description
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

#include "lbreakout.h"
#include "config.h"
#include "event.h"
#include "shrapnells.h"
#include "difficulty.h"
#include "levels.h"
#include "player.h"
#include "display.h"
#include "paddle.h"
#include "bricks.h"
#include "shots.h"
#include "balls.h"
#include "extras.h"
#include "frame.h"

extern Diff *game_diff;
extern Player *player;
extern Config config;
extern SDL_Surface *stk_display;
extern SDL_Rect stk_drect;
extern SDL_Surface *offscreen;
extern SDL_Surface *bkgnd; /* background + frame */
extern SDL_Surface *offscreen_backup;
extern int ball_w, ball_dia;
extern float ball_v, ball_vm, ball_old_v;
extern List *balls;
extern SDL_Surface *brick_pic;
extern Brick bricks[MAP_WIDTH][MAP_HEIGHT];
extern SDL_Surface *extra_pic; /* graphics */
extern SDL_Surface *extra_shadow;
extern SDL_Surface *paddle_pic;
extern SDL_Surface *ball_pic;
extern SDL_Surface *shot_pic;
extern StkFont *display_font;
List *extras = 0; /* extras */
int active[EX_NUMBER]; /* true if extra activated */
int extra_time[EX_NUMBER]; /* time in milliseconds when extra expires: 0 unlimited */
/* wall */
extern SDL_Surface *wall_pic;
extern int shadow_size;
#ifdef AUDIO_ENABLED
extern StkSound *wav_score, *wav_metal, *wav_std, *wav_wall, *wav_joker;
extern StkSound *wav_goldshower, *wav_life_up;
extern StkSound *wav_speedup, *wav_speeddown;
extern StkSound *wav_chaos, *wav_darkness, *wav_ghost;
extern StkSound *wav_timeadd, *wav_expl_ball, *wav_weak_ball;
extern StkSound *wav_bonus_magnet, *wav_malus_magnet, *wav_disable;
extern StkSound *wav_attach; /* handled by balls.c */
#endif
extern int game_type;
extern Paddle *paddles[4];
extern int paddle_count;
extern int game_host;
int net_bonuses[4][EX_NUMBER]; /* reference counter for extras of all paddles 
                                  as only one of as kind is sent per message -
                                  host only */
extern int extra_count;

/*
====================================================================
Extra times
====================================================================
*/
enum {
    TIME_WALL   = 20000,
    TIME_METAL  = 10000,
    TIME_WEAPON = 10000,
    TIME_FROZEN =  1000,
    TIME_SLIME  = 20000,
    TIME_FAST   = 20000,
    TIME_SLOW   = 20000,
    TIME_GOLDSHOWER   = 20000,
	TIME_DARKNESS     = 20000,
	TIME_CHAOS        = 20000,
	TIME_GHOST_PADDLE = 20000,
    TIME_SPIN_RIGHT   = 20000,
    TIME_SPIN_LEFT    = 20000, 
    TIME_EXPL_BALL    = 10000,
    TIME_BONUS_MAGNET = 20000,
    TIME_MALUS_MAGNET = 20000,
    TIME_WEAK_BALL    = 20000
};

/*
====================================================================
Locals
====================================================================
*/

/*
====================================================================
Public
====================================================================
*/

/*
====================================================================
Load and delete extra graphics
====================================================================
*/
void extras_load()
{
    extras = list_create( LIST_AUTO_DELETE, LIST_NO_CALLBACK );
}
void extras_delete()
{
    if ( extras ) list_delete( extras ); extras = 0;
}
/*
====================================================================
Reset extras
====================================================================
*/
void extras_reset()
{
    list_clear( extras );
    memset( active, 0, sizeof( int ) * EX_NUMBER );
    memset( extra_time, 0, sizeof( int ) * EX_NUMBER );
    extra_count = 0;
    /* set alpha keys to OPAQUE */
    SDL_SetAlpha( paddle_pic, 0,0 );
    SDL_SetAlpha( extra_pic, 0,0 );
    SDL_SetAlpha( ball_pic, 0,0 );
    SDL_SetAlpha( shot_pic, 0,0 );
    SDL_SetAlpha( display_font->surface, 0,0 );
}
/*
====================================================================
Create new extra at position
====================================================================
*/
void extra_create( int type, int x, int y, int dir )
{
    Extra *e = calloc( 1, sizeof( Extra ) );
    e->type = type;
    e->offset = type * BRICK_WIDTH;
    e->x = x; e->y = y;
    e->dir = dir;
    e->alpha = 0;
    list_add( extras, e );
}
/*
====================================================================
Use extra when paddle collected it
====================================================================
*/
void extra_use( Paddle *paddle, int extra_type )
{
    Ball    *b;
    int i, j;
    float dist;
    
    while( extra_type == EX_RANDOM ) extra_type = rand() % (EX_NUMBER);

    if ( game_type == GT_NETWORK && game_host ) {
        /* net update */
        paddle->net_bonuses[extra_type]++;
        /* statistics */
        paddle->player->extras_collected++;
    }
    
    switch (extra_type) {
        case EX_SCORE200:
            player_add_score( paddle->player, 200 );
            if ( active[EX_JOKER] )
                player_add_score( paddle->player, 200 );
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_score );
#endif
            break;
        case EX_SCORE500:
            player_add_score( paddle->player, 500 );
            if ( active[EX_JOKER] )
                player_add_score( paddle->player, 500 );
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_score );
#endif
            break;
        case EX_SCORE1000:
            player_add_score( paddle->player, 1000 );
            if ( active[EX_JOKER] )
                player_add_score( paddle->player, 1000 );
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_score );
#endif
            break;
        case EX_SCORE2000:
            player_add_score( paddle->player, 2000 );
            if ( active[EX_JOKER] )
                player_add_score( paddle->player, 2000 );
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_score );
#endif
            break;
        case EX_SCORE5000:
            player_add_score( paddle->player, 5000 );
            if ( active[EX_JOKER] )
                player_add_score( paddle->player, 5000 );
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_score );
#endif
            break;
        case EX_SCORE10000:
            player_add_score( paddle->player, 10000 );
            if ( active[EX_JOKER] )
                player_add_score( paddle->player, 10000 );
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_score );
#endif
            break;
        case EX_GOLDSHOWER:
            paddle->extra_time[EX_GOLDSHOWER] += TIME_GOLDSHOWER;
            paddle->active[extra_type] = 1;
            if ( paddle->active[EX_JOKER] ) paddle->extra_time[EX_GOLDSHOWER] += TIME_GOLDSHOWER;
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_goldshower );
#endif
            break;
        case EX_LIFE:
            if ( game_type == GT_NETWORK ) {
#ifdef AUDIO_ENABLED
                stk_sound_play( wav_score );
#endif
                player_add_score( paddle->player, 10000 );
            }
            else {
                if (paddle->player->lives < game_diff->max_lives) {
                    paddle->player->lives++;
                    frame_add_life();
                }
                else {
                    player_add_score( paddle->player, 10000 );
#ifdef AUDIO_ENABLED
                    stk_sound_play( wav_score );
#endif
                }
            }
            break;
        case EX_SHORTEN:
            paddle_init_resize( paddle, -1);
            break;
        case EX_LENGTHEN:
            paddle_init_resize( paddle, 1);
            break;
        case EX_BALL:
            b = ball_create(
                paddle->x + (paddle->w - ball_w) / 2, 
                paddle->y + ((paddle->type == PADDLE_TOP)?paddle->h:-ball_dia) );
            b->paddle = paddle;
            if ( paddle->type == PADDLE_TOP )
                b->vel.y = 1.0;
            else
                b->vel.y = -1.0;
            b->vel.x = (float)((rand() % 95) + 6);
            if ( rand() % 2 )
                b->vel.x /= -100.0;
            else
                b->vel.x /= 100.0;
            vector_set_length( &b->vel, ball_v );
            b->get_target = 1;
            list_add( balls, b );
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_std );
#endif
            break;
        case EX_WALL:
            paddle->extra_time[EX_WALL] += TIME_WALL;
            if ( paddle->active[EX_JOKER] ) paddle->extra_time[EX_WALL] += TIME_WALL;
            if ( paddle->active[EX_WALL] ) {
#ifdef AUDIO_ENABLED
                stk_sound_play( wav_std );
#endif
                break;
            }
            paddle->active[extra_type] = 1;
            if ( paddle->wall_y == 0 ) {
                for (i = 1; i < MAP_WIDTH - 1; i++) {
                    bricks[i][0].type = MAP_WALL;
                    bricks[i][0].id = 0;
                }
            }
            else
                for (i = 1; i < MAP_WIDTH - 1; i++) {
                    bricks[i][MAP_HEIGHT - 1].type = MAP_WALL;
                    bricks[i][MAP_HEIGHT - 1].id = 0;
                }
            paddle->wall_alpha = 0;
            balls_check_targets( -1, 0 );
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_wall );
#endif
            break;
        case EX_METAL:
            extra_time[EX_METAL] += TIME_METAL;
            active[extra_type] = 1;
            if ( active[EX_JOKER] ) extra_time[EX_METAL] += TIME_METAL;
            balls_set_type( BALL_METAL );
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_metal );
#endif
            /* other ball extras are disabled */
            if ( active[EX_EXPL_BALL] ) {
                active[EX_EXPL_BALL] = 0;
                extra_time[EX_EXPL_BALL] = 0;
            }
            if ( active[EX_WEAK_BALL] ) {
                active[EX_WEAK_BALL] = 0;
                extra_time[EX_WEAK_BALL] = 0;
            }
            break;
        case EX_FROZEN:
            paddle->extra_time[EX_FROZEN] = TIME_FROZEN;
            paddle->active[extra_type] = 1;
            paddle_freeze( paddle, 1 );
#ifdef AUDIO_ENABLED
#endif
            break;
        case EX_WEAPON:
            paddle->extra_time[EX_WEAPON] += TIME_WEAPON;
            paddle->active[extra_type] = 1;
            if ( paddle->active[EX_JOKER] ) paddle->extra_time[EX_WEAPON] += TIME_WEAPON;
            weapon_install( paddle, 1 );
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_std );
#endif
            break;
        case EX_SLIME:
            paddle->extra_time[EX_SLIME] += TIME_SLIME;
            paddle->active[extra_type] = 1;
            if ( paddle->active[EX_JOKER] ) paddle->extra_time[EX_SLIME] += TIME_SLIME;
            paddle_set_slime( paddle, 1 );
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_attach );
#endif
            break;
        case EX_FAST:
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_speedup );
#endif
            if ( active[EX_SLOW] || active[EX_FAST] )
                ball_v = ball_old_v;
            if ( active[EX_SLOW] ) {
                extra_time[EX_SLOW] = 0;
                active[EX_SLOW] = 0;
            }
            extra_time[EX_FAST] += TIME_FAST;
            active[extra_type] = 1;
            if ( active[EX_JOKER] ) extra_time[EX_FAST] += TIME_FAST;
            ball_old_v = ball_v;
            ball_v = ball_vm;
            list_reset( balls );
            while ( ( b = list_next( balls ) ) ) {
                vector_set_length( &b->vel, ball_v );
                if ( b->target.exists ) {
                    dist = sqrt( SQUARE(b->cur.x - b->target.x) + 
                                 SQUARE(b->cur.y - b->target.y) );
                    b->target.time = (int)floor(dist / ball_v);
                    b->target.cur_tm = 0;
                }
            }
            break;
        case EX_SLOW:
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_speeddown );
#endif
            if ( active[EX_SLOW] || active[EX_FAST] )
                ball_v = ball_old_v;
            if ( active[EX_FAST] ) {
                extra_time[EX_FAST] = 0;
                active[EX_FAST] = 0;
            }
            extra_time[EX_SLOW] += TIME_SLOW;
            active[extra_type] = 1;
            if ( active[EX_JOKER] ) extra_time[EX_SLOW] += TIME_SLOW;
            ball_old_v = ball_v;
            ball_v = game_diff->v_start;
            list_reset( balls );
            while ( ( b = list_next( balls ) ) ) {
                vector_set_length( &b->vel, ball_v );
                if ( b->target.exists ) {
                    dist = sqrt( SQUARE(b->cur.x - b->target.x) + 
                                 SQUARE(b->cur.y - b->target.y) );
                    b->target.time = (int)floor(dist / ball_v);
                    b->target.cur_tm = 0;
                }
            }
            break;
        case EX_CHAOS:
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_chaos );
#endif
            extra_time[EX_CHAOS] += TIME_CHAOS;
            active[extra_type] = 1;
			balls_set_chaos( 1 );
			break;
        case EX_DARKNESS:
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_darkness );
#endif
            extra_time[EX_DARKNESS] += TIME_DARKNESS;
			if ( active[EX_DARKNESS] ) break;
			/* backup offscreen and turn it black */
            stk_surface_fill( offscreen, 0,0,-1,-1, 0x0 );
            stk_surface_fill( stk_display, 0,0,-1,-1, 0x0 );
            stk_display_store_rect( 0 );
            /* set alpha keys to 128 */
            SDL_SetAlpha( paddle_pic, SDL_SRCALPHA, 128 );
            SDL_SetAlpha( extra_pic, SDL_SRCALPHA, 128 );
            SDL_SetAlpha( ball_pic, SDL_SRCALPHA, 128 );
            SDL_SetAlpha( shot_pic, SDL_SRCALPHA, 128 );
            SDL_SetAlpha( display_font->surface, SDL_SRCALPHA, 128 );
            /* use dark explosions */
            exps_set_dark( 1 );
            active[extra_type] = 1;
			break;
        case EX_GHOST_PADDLE:
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_ghost );
#endif
            paddle->extra_time[EX_GHOST_PADDLE] += TIME_GHOST_PADDLE;
            paddle->active[extra_type] = 1;
			paddle_set_invis( paddle, 1 );
			break;
        case EX_TIME_ADD:
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_timeadd );
#endif
            for ( i = 0; i < EX_NUMBER; i++ )
                if ( extra_time[i] )
                    extra_time[i] += 7000;
            for ( i = 0; i < EX_NUMBER; i++ ) {
                for ( j = 0; j < paddle_count; j++ )
                    if ( paddles[j]->extra_time[i] )
                        paddles[j]->extra_time[i] += 7000;
            }
            break;
        case EX_EXPL_BALL:
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_expl_ball );
#endif
            balls_set_type( BALL_EXPL );
            extra_time[EX_EXPL_BALL] += TIME_EXPL_BALL;
            active[extra_type] = 1;
            if ( active[EX_JOKER] ) extra_time[EX_EXPL_BALL] += TIME_EXPL_BALL;
            /* other ball extras are disabled */
            if ( active[EX_METAL] ) {
                active[EX_METAL] = 0;
                extra_time[EX_METAL] = 0;
            }
            if ( active[EX_WEAK_BALL] ) {
                active[EX_WEAK_BALL] = 0;
                extra_time[EX_WEAK_BALL] = 0;
            }
            break;
        case EX_WEAK_BALL:
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_weak_ball );
#endif
            balls_set_type( BALL_WEAK );
            extra_time[EX_WEAK_BALL] += TIME_WEAK_BALL;
            active[extra_type] = 1;
            /* other ball extras are disabled */
            if ( active[EX_METAL] ) {
                active[EX_METAL] = 0;
                extra_time[EX_METAL] = 0;
            }
            if ( active[EX_EXPL_BALL] ) {
                active[EX_EXPL_BALL] = 0;
                extra_time[EX_EXPL_BALL] = 0;
            }
            break;
        case EX_BONUS_MAGNET:
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_bonus_magnet );
#endif
            paddle_set_attract( paddle, ATTRACT_BONUS );
            paddle->extra_time[EX_BONUS_MAGNET] += TIME_BONUS_MAGNET;
            paddle->active[extra_type] = 1;
            if ( paddle->active[EX_JOKER] ) paddle->extra_time[EX_BONUS_MAGNET] += TIME_BONUS_MAGNET;
            if ( paddle->active[EX_MALUS_MAGNET] ) {
                paddle->active[EX_MALUS_MAGNET] = 0;
                paddle->extra_time[EX_MALUS_MAGNET] = 0;
            }
            break;
        case EX_MALUS_MAGNET:
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_malus_magnet );
#endif
            paddle_set_attract( paddle, ATTRACT_MALUS );
            paddle->extra_time[EX_MALUS_MAGNET] += TIME_MALUS_MAGNET;
            paddle->active[extra_type] = 1;
            if ( paddle->active[EX_BONUS_MAGNET] ) {
                paddle->active[EX_BONUS_MAGNET] = 0;
                paddle->extra_time[EX_BONUS_MAGNET] = 0;
            }
            break;
        case EX_DISABLE:
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_disable );
#endif
            /* set all active extra times to 1 so they will expire next
               prog cycle */ 
            for ( i = 0; i < EX_NUMBER; i++ )
                if ( extra_time[i] )
                    extra_time[i] = 1;
            for ( i = 0; i < EX_NUMBER; i++ ) {
                for ( j = 0; j < paddle_count; j++ )
                    if ( paddles[j]->extra_time[i] )
                        paddles[j]->extra_time[i] = 1;
            }
            break;
    }
}
/*
====================================================================
Show, hide extras
====================================================================
*/
void extras_hide()
{
    ListEntry  *entry = extras->head->next;
    Extra       *ex;
    while ( entry != extras->tail ) {
        ex = entry->item;
        entry = entry->next;
        stk_surface_blit( offscreen, 
            (int)ex->x, (int)ex->y,
            BRICK_WIDTH + shadow_size, BRICK_HEIGHT + shadow_size, 
            stk_display, (int)ex->x, (int)ex->y );
        ex->update_rect = stk_drect;
    }
}
void extras_show_shadow()
{
    ListEntry  *entry = extras->head->next;
    Extra       *extra;
    while ( entry != extras->tail ) {
        extra = entry->item;
        stk_surface_clip( stk_display, 0,0,
            stk_display->w - BRICK_WIDTH, stk_display->h );
        stk_surface_alpha_blit( 
            extra_shadow, extra->type * BRICK_WIDTH, 0,
            BRICK_WIDTH, BRICK_HEIGHT,
            stk_display, (int)extra->x + shadow_size, (int)extra->y + shadow_size,
            ((int)extra->alpha)>>1 );
        stk_surface_clip( stk_display, 0,0,-1,-1 );
        entry = entry->next;
    }
}
void extras_show()
{
    ListEntry  *entry = extras->head->next;
    Extra       *ex;
    int         x, y;
    while ( entry != extras->tail ) {
        ex = entry->item;
        entry = entry->next;
        x = (int)ex->x; y = (int)ex->y;
        stk_surface_alpha_blit( extra_pic, ex->offset, 0,
            BRICK_WIDTH, BRICK_HEIGHT, stk_display, x, y, ex->alpha );
        if ( x < ex->update_rect.x ) {
            /* movement to left */
            ex->update_rect.w += ex->update_rect.x - x;
            ex->update_rect.x = x;
        }
        else
            /* movement to right */
            ex->update_rect.w += x - ex->update_rect.x;
        if ( ex->dir == -1 ) {
            /* movement up */
            ex->update_rect.h += ex->update_rect.y - y;
            ex->update_rect.y = y;
            if ( ex->update_rect.y < 0 ) {
                ex->update_rect.h += ex->update_rect.y;
                ex->update_rect.y = 0;
            }
        }
        else {
            /* movement down */
            ex->update_rect.h += y - ex->update_rect.y;
            if ( ex->update_rect.y + ex->update_rect.h >= stk_display->h )
                ex->update_rect.h = stk_display->h - ex->update_rect.y;
        }
        stk_display_store_rect( &ex->update_rect );
    }
}
void extras_alphashow( int alpha )
{
    ListEntry  *entry = extras->head->next;
    Extra       *ex;
    int         x, y;
    while ( entry != extras->tail ) {
        ex = entry->item;
        x = (int)ex->x;
        y = (int)ex->y;
        stk_surface_alpha_blit( extra_pic, ex->type * BRICK_WIDTH, 0,
            BRICK_WIDTH, BRICK_HEIGHT, stk_display, x, y, alpha );
        stk_display_store_rect( &ex->update_rect );
        entry = entry->next;
    }
}
/*
====================================================================
Update extras
====================================================================
*/
void extras_update( int ms )
{
    Extra       *ex;
    Ball *ball;
    int i, j;
    int type, px, py, dir;
    int magnets;
    Paddle *magnet;
    float dist;
    
    /* release new extras */
    while ( bricks_pop_extra( &type, &px, &py, &dir ) )
        extra_create( type, px, py, dir );
    /* check extra_time of limited extras */
    /* general extras */
    for ( i = 0; i < EX_NUMBER; i++ )
        if ( extra_time[i] ) {
            extra_time[i] -= ms;
            if ( extra_time[i] <= 0 ) {
                extra_time[i] = 0;
                /* expired */
                switch ( i ) {
                    case EX_EXPL_BALL:
                    case EX_WEAK_BALL:
                    case EX_METAL: 
                        balls_set_type( BALL_NORMAL ); 
                        break;
                    case EX_SLOW:
                    case EX_FAST:
                        ball_v = ball_old_v;
                        list_reset( balls );
                        while ( ( ball = list_next( balls ) ) ) {
                            vector_set_length( &ball->vel, ball_v );
                            if ( ball->target.exists ) {
                                dist = sqrt( SQUARE(ball->cur.x - ball->target.x) + 
                                             SQUARE(ball->cur.y - ball->target.y) );
                                ball->target.time = (int)floor(dist / ball_v);
                                ball->target.cur_tm = 0;
                            }
                        }
                        break;
					case EX_DARKNESS:
						/* restore offscreen */
                        stk_surface_blit( bkgnd, 0,0,-1,-1, offscreen, 0,0 );
						bricks_draw();
                        if ( game_type == GT_LOCAL )
                            frame_draw_lives( player->lives, game_diff->max_lives );
						/* back to screen */
                        stk_surface_blit( offscreen, 0,0,-1,-1, stk_display, 0,0 );
                        stk_display_store_rect( 0 );
                        /* set alpha keys to OPAQUE */
                        SDL_SetAlpha( paddle_pic, 0,0 );
                        SDL_SetAlpha( extra_pic, 0,0 );
                        SDL_SetAlpha( ball_pic, 0,0 );
                        SDL_SetAlpha( shot_pic, 0,0 );
                        SDL_SetAlpha( display_font->surface, 0,0 );
                        /* use bright explosions */
                        exps_set_dark( 0 );
						break;
					case EX_CHAOS:
						balls_set_chaos( 0 );
						break;
                }
                /* set deactivated */
                active[i] = 0; 
            }
        }
    /* paddlized extras */
    for ( j = 0; j < paddle_count; j++ )
        for ( i = 0; i < EX_NUMBER; i++ )
            /* extra_time of wall is updated in wall_update() */
            if ( paddles[j]->extra_time[i] && i != EX_WALL ) {
                paddles[j]->extra_time[i] -= ms;
                if ( paddles[j]->extra_time[i] <= 0 ) {
                    paddles[j]->extra_time[i] = 0;
                    /* expired */
                    switch ( i ) {
                        case EX_SLIME: paddle_set_slime( paddles[j], 0 ); break;
                        case EX_WEAPON: weapon_install( paddles[j], 0 ); break;
                        case EX_FROZEN:
                            paddle_freeze( paddles[j], 0 );
                            break;
                        case EX_GHOST_PADDLE:
                            paddle_set_invis( paddles[j], 0 );
                            break;
                        case EX_BONUS_MAGNET:
                        case EX_MALUS_MAGNET:
                            paddle_set_attract( paddles[j], ATTRACT_NONE );
                            break;
                    }
                    /* set deactivated */
                    paddles[j]->active[i] = 0; /* wall is handled in wall_...() */
                }
            }
    /* move extras and check if paddle was hit */
    list_reset( extras );
    while ( ( ex = list_next( extras ) ) ) {
        /* if only one paddle has a magnet active all extras will 
         * be attracted by this paddle else the extras 'dir' is used 
         */
        magnets = 0; magnet = 0;
        for ( i = 0; i < paddle_count; i++ )
            if ( paddle_check_attract( paddles[i], ex->type ) ) {
                magnets++;
                magnet = paddles[i]; /* last magnet */
            }
        if ( magnets != 1 ) {
            /* either no or more than one magnet so use default */
            if ( ex->dir > 0 )
                ex->y += 0.05 * ms;
            else
                ex->y -= 0.05 * ms;
        }
        else {
            /* 'magnet' is the paddle that will attract this extra */
            if ( magnet->type == PADDLE_TOP )
                ex->y -= 0.05 * ms;
            else
                ex->y += 0.05 * ms;
            if ( ex->x + ( BRICK_WIDTH >> 1 ) < magnet->x + ( magnet->w >> 1 ) ) {
                ex->x += 0.05 * ms;
                if ( ex->x + ( BRICK_WIDTH >> 1 ) > magnet->x + ( magnet->w >> 1 ) )
                    ex->x = magnet->x + ( magnet->w >> 1 ) - ( BRICK_WIDTH >> 1 );
            }
            else {
                ex->x -= 0.05 * ms;
                if ( ex->x + ( BRICK_WIDTH >> 1 ) < magnet->x + ( magnet->w >> 1 ) )
                    ex->x = magnet->x + ( magnet->w >> 1 ) - ( BRICK_WIDTH >> 1 );
            }
        }
		if ( !active[EX_DARKNESS] ) {
        	if ( ex->alpha < 255 ) {
            	ex->alpha += 0.25 * ms;
                if (ex->alpha > 255)
                    ex->alpha = 255;
            }
		}
		else {
			if ( ex->alpha < 128 ) {
            	ex->alpha += 0.25 * ms;
                if (ex->alpha > 128)
                    ex->alpha = 128;
            }
		}
        /* if out of screen forget this extra */
        if ( ex->y >= stk_display->h || ex->y + BRICK_HEIGHT < 0 ) {
            stk_display_store_rect( &ex->update_rect );
            list_delete_current( extras );
            continue;
        }
        for ( j = 0; j < paddle_count; j++ ) {
            /* contact with paddle core ? */
            if ( paddle_solid( paddles[j] ) )
            if ( ex->x + BRICK_WIDTH > paddles[j]->x )
            if ( ex->x < paddles[j]->x + paddles[j]->w - 1 )
            if ( ex->y + BRICK_HEIGHT > paddles[j]->y )
            if ( ex->y < paddles[j]->y + paddles[j]->h ) {
                /* any extra except EX_JOKER is simply used */
                if ( ex->type != EX_JOKER ) {
                    if ( game_type == GT_LOCAL || game_host )
                        extra_use( paddles[j], ex->type );
                    stk_display_store_rect( &ex->update_rect );
                    list_delete_current( extras );
                    break;
                }
                /* use EX_JOKER and work through all active extras */
#ifdef AUDIO_ENABLED
                stk_sound_play( wav_joker );
#endif
                paddles[j]->active[EX_JOKER] = 1;
                list_reset( extras );
                while ( ( ex = list_next( extras ) ) ) {
                    if ( ex->type != EX_JOKER )
                    if ( ex->type != EX_SHORTEN )
                    if ( ex->type != EX_FROZEN )
                    if ( ex->type != EX_FAST )
                    if ( ex->type != EX_RANDOM )
                    if ( ex->type != EX_DARKNESS )
                    if ( ex->type != EX_GHOST_PADDLE )
                    if ( ex->type != EX_CHAOS )
                    if ( ex->type != EX_DISABLE )
                    if ( ex->type != EX_MALUS_MAGNET )
                    if ( ex->type != EX_WEAK_BALL )
                    if ( game_type == GT_LOCAL || game_host )
                        extra_use( paddles[j], ex->type );
                    stk_display_store_rect( &ex->update_rect );
                    list_delete_current( extras );
                }
                paddles[j]->active[EX_JOKER] = 0;
                break;
            }
        }
    }
}
/* wall */
void wall_hide()
{
    int j;
    for ( j = 0; j < paddle_count; j++ )
        if ( paddles[j]->active[EX_WALL] ) {
            stk_surface_blit( offscreen, BRICK_WIDTH, paddles[j]->wall_y,
                wall_pic->w, wall_pic->h,
                stk_display, BRICK_WIDTH, paddles[j]->wall_y );
            stk_display_store_drect();
        }
}
void wall_show()
{
    int j;
    for ( j = 0; j < paddle_count; j++ )
        if ( paddles[j]->active[EX_WALL] )
            stk_surface_alpha_blit( wall_pic, 0,0,
                wall_pic->w, wall_pic->h,
                stk_display, BRICK_WIDTH, paddles[j]->wall_y,
                (int)paddles[j]->wall_alpha );
}
void wall_alphashow( int alpha )
{
    int j;
    for ( j = 0; j < paddle_count; j++ )
        if ( paddles[j]->active[EX_WALL] )
            stk_surface_alpha_blit( wall_pic, 0,0,
                wall_pic->w, wall_pic->h,
                stk_display, BRICK_WIDTH, paddles[j]->wall_y,
                alpha );
}
void wall_update( int ms )
{
    int i, j;

    for ( j = 0; j < paddle_count; j++ )
        if ( paddles[j]->active[EX_WALL] ) {
            if ( paddles[j]->extra_time[EX_WALL] > 0 ) {
                paddles[j]->extra_time[EX_WALL] -= ms;
                if ( paddles[j]->extra_time[EX_WALL] < 0 ) 
                    paddles[j]->extra_time[EX_WALL] = 0;
                /* still appearing? */
                if (paddles[j]->wall_alpha < 255) {
                    paddles[j]->wall_alpha += 0.25 * ms;
                    if (paddles[j]->wall_alpha > 255) 
                        paddles[j]->wall_alpha = 255;
                }				
            }
            else {
                paddles[j]->wall_alpha -= 0.25 * ms;
                if ( paddles[j]->wall_alpha < 0 ) {
                    paddles[j]->wall_alpha = 0;
                    /* disappear */
                    wall_hide();
                    paddles[j]->active[EX_WALL] = 0;
                    if ( paddles[j]->wall_y == 0 )
                        for (i = 1; i < MAP_WIDTH - 1; i++) 
                            bricks[i][0].type = MAP_EMPTY;
                    else
                        for (i = 1; i < MAP_WIDTH - 1; i++) 
                            bricks[i][MAP_HEIGHT - 1].type = MAP_EMPTY;
                    balls_check_targets( -1, 0 );
                }
            }
        }
}

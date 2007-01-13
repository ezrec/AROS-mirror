/***************************************************************************
                          paddle.c  -  description
                             -------------------
    begin                : Fri Sep 7 2001
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
#include "difficulty.h"
#include "levels.h"
#include "player.h"
#include "display.h"
#include "paddle.h"
#include "shots.h"
#include "bricks.h"

/*
====================================================================
Paddle & plasma weapon
====================================================================
*/
extern SDL_Surface *paddle_pic; /* paddle tiles: left, middle, right for each paddle type */
extern SDL_Surface *paddle_shadow;
int paddle_cw = 18, paddle_ch = 18; /* size of a graphical paddle component */
enum { PADDLE_INVIS_DELAY = 200 };
extern SDL_Surface *weapon_pic;
extern SDL_Surface *weapon_shadow;
int weapon_w = 14;
int weapon_h = 18;
int weapon_fr_num = 4;
float weapon_fpms = 0.006;

/*
====================================================================
Other stuff
====================================================================
*/
extern Diff *game_diff; /* current difficulty */
extern SDL_Surface *stk_display;
extern SDL_Rect stk_drect;
extern SDL_Surface *offscreen;
extern int motion;
extern int motion_x;
extern int motion_rel_x;
extern int keystate[SDLK_LAST];
extern Config config;
extern int shadow_size;
#ifdef AUDIO_ENABLED
extern StkSound *wav_expand, *wav_shrink, *wav_frozen;
#endif
extern int active[EX_NUMBER];
extern List *balls;
extern float ball_v;
extern int ball_dia;
extern int keystate[SDLK_LAST];
extern int buttonstate[STK_BUTTON_COUNT];
extern int game_type, game_host;
extern Net_Paddle net_paddle;

/*
====================================================================
Create/delete a paddle
  'y' is the y coordinate ('x' is simply centered)
  'ammo' is the number of balls the paddle may bring to game
         additionally
====================================================================
*/
Paddle* paddle_create( int y, int ammo )
{
    Paddle *paddle = calloc( 1, sizeof( Paddle ) );
    if ( paddle == 0 ) return 0;
	delay_set( &paddle->resize_delay, 40 );
    paddle->len = game_diff->paddle_size;
    paddle->max_len = game_diff->paddle_max_size;
    paddle->w = (paddle->len + 2) * paddle_cw;
	paddle->wanted_w = paddle->w;
    paddle->h = paddle_ch;
    paddle->y = y;
    if ( y < stk_display->h / 2 ) {
        paddle->wall_y = 0;
        paddle->type = PADDLE_TOP;
        paddle->pic_x_offset = paddle_cw * 3;
    }
    else {
        paddle->wall_y = stk_display->h - BRICK_HEIGHT;
        paddle->type = PADDLE_BOTTOM;
    }
    paddle->wall_alpha = 0;
    paddle->x = ((MAP_WIDTH * BRICK_WIDTH) - paddle->w) / 2; /* centered */
    paddle->cur_x = paddle->x;
    paddle->friction = 0.3;
    paddle->time = 0;
    /* reset weapon delay&weapon */
    delay_set( &paddle->weapon_delay, 100 );
    paddle->weapon_inst = paddle->weapon_fire = 0;
    paddle->weapon_ammo = PADDLE_WEAPON_AMMO;
    /* reset slime&frozen */
    paddle->slime = 0;
    paddle->frozen = 0;
    paddle->pic_y_offset = 0;
    /* attract */
    paddle->attract = ATTRACT_NONE;
	/* no ivisiblivty */
	paddle->invis = 0;
    /* ammo */
    paddle->ammo = ammo;
    return paddle;
}
void paddle_delete( Paddle *paddle )
{
    if ( paddle )
        free( paddle );
}
/*
====================================================================
Freeze paddle movement or set slime.
====================================================================
*/
void paddle_freeze( Paddle *paddle, int freeze )
{
#ifdef AUDIO_ENABLED
    if ( !paddle->frozen && freeze )
        stk_sound_play( wav_frozen );
#endif
    paddle->frozen = freeze;
    if ( freeze )
        paddle->pic_y_offset = paddle_ch << 1;
    else
        if ( paddle->slime )
            paddle->pic_y_offset = paddle_ch;
        else
            paddle->pic_y_offset = 0;
}
void paddle_set_slime( Paddle *paddle, int slime )
{
    paddle->slime = slime;
    if ( paddle->frozen )
        paddle->pic_y_offset = paddle_ch << 1;
    else
        if ( slime )
            paddle->pic_y_offset = paddle_ch;
        else
            paddle->pic_y_offset = 0;
}
int paddle_slimy( Paddle *paddle )
{
    return paddle->slime;
}
/*
====================================================================
Set if paddle attracts boni/mali or none.
====================================================================
*/
void paddle_set_attract( Paddle *paddle, int attract )
{
    paddle->attract = attract;
    /* no graphical change yet */
}
int paddle_attract_malus( Paddle *paddle )
{
    return ( paddle->attract == ATTRACT_MALUS );
}
int paddle_attract_bonus( Paddle *paddle )
{
    return ( paddle->attract == ATTRACT_BONUS );
}
int paddle_check_attract( Paddle *paddle, int type )
{
    switch ( type ) {
        /* bonus */
        case EX_SCORE200:
        case EX_SCORE500:
        case EX_SCORE1000:
        case EX_SCORE2000:
        case EX_SCORE5000:
        case EX_SCORE10000:
        case EX_GOLDSHOWER:
        case EX_LENGTHEN:
        case EX_LIFE:
        case EX_SLIME:
        case EX_METAL:
        case EX_BALL:
        case EX_WALL:
        case EX_WEAPON:
        case EX_SLOW:
        case EX_JOKER:
        case EX_EXPL_BALL:
        case EX_BONUS_MAGNET:
            if ( paddle_attract_bonus( paddle ) )
                return 1;
            return 0;
        /* malus */
        case EX_SHORTEN:
        case EX_FAST:
        /*
        case EX_SPIN_RIGHT:
        case EX_SPIN_LEFT:
        */
        case EX_MALUS_MAGNET:
        case EX_WEAK_BALL:
	    case EX_DARKNESS:
	    case EX_GHOST_PADDLE:
        case EX_FROZEN:
            if ( paddle_attract_malus( paddle ) )
                return 1;
            return 0;
        /* neutral */
        case EX_DISABLE:
	    case EX_CHAOS:
        case EX_TIME_ADD:
        case EX_RANDOM:
            return 0;
    }
    return 0;
}
/*
====================================================================
(De)activate ghostly behaviour: paddle is only seen when moved at
maximum 250ms before.
====================================================================
*/
void paddle_set_invis( Paddle *paddle, int invis )
{
	if ( invis ) paddle->invis_delay = PADDLE_INVIS_DELAY;
	paddle->invis = invis;
}
int  paddle_solid( Paddle *paddle )
{
	/* does paddle is visible? */
	if ( !paddle->invis ) return 1;
	if ( paddle->invis_delay ) return 1;
	return 0;
}
/*
====================================================================
Init paddle resize (the change between wanted_w and w MUST be
even (else the paddle shrinks til eternity)
====================================================================
*/
int paddle_init_resize( Paddle *paddle, int c )
{
    /* resize possbile */
    if ( paddle->len + c > paddle->max_len || paddle->len + c < 1 ) return 0;
#ifdef AUDIO_ENABLED
    if ( c < 0 )
        stk_sound_play( wav_shrink );
    else
        stk_sound_play( wav_expand );
#endif
    /* get wanted width */
    paddle->len += c;
	paddle->wanted_w = (paddle->len + 2) * paddle_cw;
	/* reset delay */
	delay_reset( &paddle->resize_delay );
    return 1;
}
/*
====================================================================
Actually resize paddle and return the -1 for shrinkage and 1 
for expansion.
====================================================================
*/
int paddle_resize( Paddle *paddle, int ms )
{
	if ( paddle->w == paddle->wanted_w ) return 0;
	if ( !delay_timed_out( &paddle->resize_delay, ms ) ) return 0;
	/* change size and position */
	if ( paddle->w < paddle->wanted_w ) {
		paddle->w += 2;
		paddle->cur_x -= 1;
		paddle->x = (int)paddle->cur_x;
		/* check range */
    	if (paddle->x < BRICK_WIDTH)
        	paddle->x = BRICK_WIDTH;
	    if (paddle->x + paddle->w > stk_display->w - BRICK_WIDTH)
    	    paddle->x = stk_display->w - BRICK_WIDTH - paddle->w;
        return 1;
	}	
	else {
		paddle->w -= 2;
		paddle->cur_x += 1;
		paddle->x = (int)paddle->cur_x;
        return -1;
    }
}
/*
====================================================================
Detach all balls to the passed direction (-1 or 1) and return True
if there were any balls detached.
====================================================================
*/
int paddle_detach_balls( Paddle *paddle, int dir )
{
    Ball *ball;
    int fired = 0;
    list_reset( balls );
    while ( ( ball = list_next( balls ) ) )
        if ( ball->attached && ball->paddle == paddle )
        /* if not in wall */
        if (ball->x + paddle->x >= BRICK_WIDTH && ball->x + ball_dia + paddle->x < stk_display->w - BRICK_WIDTH) {
            /* release ball */
            ball->attached = 0;
            ball->paddle->attached_ball_count--;
            ball->moving_back = ball->idle_time = ball->return_allowed = 0;
            ball->x += paddle->x;
            ball->y += paddle->y;
            ball->cur.x = ball->x;
            ball->cur.y = ball->y;
            ball->vel.x = (float)dir;
            if ( ball->paddle->type == PADDLE_TOP )
                ball->vel.y = 1.2;
            else
                ball->vel.y = -1.2;
            vector_set_length( &ball->vel, ball_v );
            ball->get_target = 1;
            fired = 1;
        }
    return fired;
}
/*
====================================================================
Hide and show paddle on screen.
====================================================================
*/
void paddle_hide( Paddle *paddle )
{
    paddle->update_rect.x = paddle->x;
    paddle->update_rect.y = paddle->y;
    paddle->update_rect.w = paddle->w + shadow_size;
    paddle->update_rect.h = paddle->h + shadow_size;
    stk_surface_blit( offscreen, paddle->x,  paddle->y,
        paddle->update_rect.w, paddle->update_rect.h, 
        stk_display, paddle->x, paddle->y );
    /* store rect if it won't be drawn */
	if ( paddle->invis )
		if ( paddle->invis_delay == 0 ) 
            stk_display_store_rect( &paddle->update_rect );
}
void paddle_show_shadow( Paddle *paddle )
{
	int aux_w = 0, i;
	int rem_middle_w; /* remaining points to draw in the middle */
    if ( paddle->invis ) return;
    if ( active[EX_DARKNESS] ) return;
    stk_surface_clip( stk_display, 0, 0, stk_display->w - BRICK_WIDTH, stk_display->h );
	/* left end */
    stk_surface_alpha_blit( paddle_shadow, paddle->pic_x_offset,0,paddle_cw, paddle_ch,
        stk_display, paddle->x + shadow_size, paddle->y + shadow_size,
        SHADOW_ALPHA );
	/* middle part */
    for ( i = paddle_cw, rem_middle_w = paddle->w - ( paddle_cw << 1 ); rem_middle_w > 0; i += paddle_cw, rem_middle_w -= paddle_cw ) {
		if ( rem_middle_w > paddle_cw )
			aux_w = paddle_cw;
		else
        aux_w = rem_middle_w;
        stk_surface_alpha_blit( paddle_shadow, paddle->pic_x_offset+paddle_cw,0,aux_w, paddle_ch,
            stk_display, paddle->x + shadow_size + i, paddle->y + shadow_size,
            SHADOW_ALPHA );
    }
	/* right end */
	i = paddle->w - paddle_cw;	
    stk_surface_alpha_blit( paddle_shadow, paddle->pic_x_offset+(paddle_cw<<1),0,paddle_cw, paddle_ch,
        stk_display, paddle->x + shadow_size + i, paddle->y + shadow_size,
        SHADOW_ALPHA );
    stk_surface_clip( stk_display, 0, 0, 0, 0 );
}
void paddle_show( Paddle *paddle )
{
	int i, aux_w = 0;
	int rem_middle_w; /* remaining points to draw in the middle */
	/* if invisible return or show alpha */
	if ( paddle->invis ) {
		if ( paddle->invis_delay != 0 ) 
			paddle_alphashow( paddle, 128 );
		return;
	}
    /* paddle */
	/* left end */
    stk_surface_blit( paddle_pic, paddle->pic_x_offset,paddle->pic_y_offset,
        paddle_cw, paddle_ch, stk_display, paddle->x, paddle->y );
	/* middle part */
    for ( i = paddle_cw, rem_middle_w = paddle->w - ( paddle_cw << 1 ); rem_middle_w > 0; i += paddle_cw, rem_middle_w -= paddle_cw ) {
		if ( rem_middle_w > paddle_cw )
			aux_w = paddle_cw;
		else
			aux_w = rem_middle_w;
        stk_surface_blit( paddle_pic, 
            paddle->pic_x_offset+paddle_cw,paddle->pic_y_offset,
            aux_w, paddle_ch, stk_display, paddle->x + i, paddle->y );
    }
	i = paddle->w - paddle_cw;	
	/* right end */
    stk_surface_blit( paddle_pic, 
        paddle->pic_x_offset+(paddle_cw<<1),paddle->pic_y_offset,
        paddle_cw, paddle_ch, stk_display, paddle->x + i, paddle->y );
    /* weapon */
    if ( paddle->weapon_inst )
        stk_surface_blit( weapon_pic, 
            (int)paddle->weapon_cur * weapon_w, 0, weapon_w, weapon_h,
            stk_display, paddle->x + ((paddle->w - weapon_w)>>1), paddle->y );
    /* update */
    if ( paddle->x < paddle->update_rect.x ) {
        /* movement to left */
        paddle->update_rect.w += paddle->update_rect.x - paddle->x;
        paddle->update_rect.x = paddle->x;
    }
    else {
        /* movement to right */
        paddle->update_rect.w += paddle->x - paddle->update_rect.x;
    }
    stk_display_store_rect( &paddle->update_rect );
}
void paddle_alphashow( Paddle *paddle, int a )
{
    int i, aux_w;
	int rem_middle_w;
	/* left end */
    stk_surface_alpha_blit( paddle_pic, paddle->pic_x_offset,paddle->pic_y_offset,
        paddle_cw, paddle_ch, stk_display, paddle->x, paddle->y, a );
	/* middle part */
    for ( i = paddle_cw, rem_middle_w = paddle->w - ( paddle_cw << 1 ); rem_middle_w > 0; i += paddle_cw, rem_middle_w -= paddle_cw ) {
		if ( rem_middle_w > paddle_cw )
			aux_w = paddle_cw;
		else
			aux_w = rem_middle_w;
        stk_surface_alpha_blit( paddle_pic, paddle->pic_x_offset+paddle_cw,paddle->pic_y_offset,
            aux_w, paddle_ch, stk_display, paddle->x + i, paddle->y, a );
    }
	i = paddle->w - paddle_cw;	
	/* right end */
    stk_surface_alpha_blit( paddle_pic, paddle->pic_x_offset+(paddle_cw<<1),paddle->pic_y_offset,
        paddle_cw, paddle_ch, stk_display, paddle->x + i, paddle->y, a );
    if ( paddle->weapon_inst )
        stk_surface_alpha_blit( weapon_pic, 
            (int)paddle->weapon_cur * weapon_w, 0, weapon_w, weapon_h,
            stk_display, paddle->x + ((paddle->w - weapon_w)>>1), paddle->y, a );
    /* update */
    if ( paddle->x < paddle->update_rect.x ) {
        /* movement to left */
        paddle->update_rect.w += paddle->update_rect.x - paddle->x;
        paddle->update_rect.x = paddle->x;
    }
    else {
        /* movement to right */
        paddle->update_rect.w += paddle->x - paddle->update_rect.x;
    }
    stk_display_store_rect( &paddle->update_rect );
}
/*
====================================================================
Update paddle animation (weapon)
====================================================================
*/
void paddle_update( Paddle *paddle, int ms )
{
    Ball *ball;
    int result;
	/* invisiblity */
	if ( paddle->invis && paddle->invis_delay > 0 ) {
		paddle->invis_delay -= ms;
		if ( paddle->invis_delay < 0 ) {
            paddle->invis_delay = 0;
            net_paddle.invis = 1;
        }
	}
    /* weapon animation */
    if ( paddle->weapon_inst ) {
        paddle->weapon_cur += ms * weapon_fpms;
        if (paddle->weapon_cur >= weapon_fr_num) paddle->weapon_cur -= weapon_fr_num;
    }
    /* the host manages firing for all paddles */
    if ( game_type == GT_LOCAL || game_host ) 
        if ( paddle->weapon_inst && paddle->weapon_fire )
            if ( delay_timed_out( &paddle->weapon_delay, ms ) )
                if ( paddle->weapon_ammo > 0 ) {
                    shot_create( paddle );
                    paddle->weapon_ammo--;
                }
	/* check for resize */
	if ( ( result = paddle_resize( paddle, ms ) ) ) {
        list_reset( balls );
        while ( ( ball = list_next( balls ) ) )
            /* adjust attached balls */
            if ( ball->attached && ball->paddle == paddle ) {
                if ( result < 0 ) {
                    /* shrinked */
                    if ( ball->cur.x > ( paddle->w >> 1 ) ) {
                        ball->cur.x -= 2;
                        ball->x = (int)ball->cur.x;
                    }	
                }
                else {
                    /* expanded */
                    if ( ball->cur.x > ( paddle->w >> 1 ) ) {
                        ball->cur.x += 2;
                        ball->x = (int)ball->cur.x;
                    }	
                }
            }
    }
}
/*
====================================================================
Handle events (used for local paddle)
====================================================================
*/
void paddle_handle_events( Paddle *paddle, int ms )
{
    int fired_attached;
#ifdef PADDLE_FRICTION
    int old_x = paddle->x;
    int off_x;
#endif
    int moved = 0;

    if ( paddle->frozen ) {
#ifdef PADDLE_FRICTION
        paddle->v_x = 0;
#endif
        return;
    }
    /* mouse motion */
    if ( motion ) {
        if (config.rel_motion)
            paddle->cur_x += motion_rel_x;
        else
            paddle->cur_x = motion_x - ( paddle->w >> 1 );
#ifdef PADDLE_FRICTION
        paddle->time = 200;
#endif
        moved = 1;
    }
    /* keys */
    if (keystate[config.k_left] ) {
        paddle->cur_x -= config.key_speed * (ms << keystate[config.k_turbo]);
        paddle->time = 0;
        moved = 1;
    }
    if (keystate[config.k_right] ) {
        paddle->cur_x += config.key_speed * (ms << keystate[config.k_turbo]);
        paddle->time = 0;
        moved = 1;
    }
#ifdef PADDLE_FRICTION
    /* paddle should have friction for some time */
    if (paddle->time > 0) {
        paddle->time -= ms;
        if (paddle->time < 0) {
            paddle->time = 0;
            paddle->v_x = 0.0;
        }
    }
    else
        paddle->v_x = 0.0;
#endif
    /* check paddle when moved */
    if ( moved ) {
        /* check range */
        if (paddle->cur_x < BRICK_WIDTH)
            paddle->cur_x = BRICK_WIDTH;
        if (paddle->cur_x + paddle->w >= stk_display->w - BRICK_WIDTH)
            paddle->cur_x = stk_display->w - BRICK_WIDTH - paddle->w;
        /* absolute position */
        paddle->x = (int)paddle->cur_x;
        net_paddle.x = paddle->x;
        net_paddle.invis = 0;
#ifdef PADDLE_FRICTION
        /* offset */
        off_x = paddle->x - old_x;
        /* speed */
        paddle->v_x = (float)(off_x) / ms;
        if ( motion ) {
            /* limit mouse speed */
            if (paddle->v_x > 5.0) paddle->v_x = 5.0;
            if (paddle->v_x < -5.0) paddle->v_x = -5.0;
            paddle->v_x /= 5;
        }
		/* visible for some time */
		paddle->invis_delay = PADDLE_INVIS_DELAY;
#endif
    }
    /* fire balls */
    if ( buttonstate[STK_BUTTON_LEFT] || buttonstate[STK_BUTTON_RIGHT] || keystate[config.k_lfire] || keystate[config.k_rfire] ) {
        fired_attached = 
            paddle_detach_balls( paddle, 
                                 ((buttonstate[STK_BUTTON_LEFT] || keystate[config.k_lfire])?-1:1) );
        if ( !fired_attached && !paddle->block_new_ball && ( paddle->ammo > 0 || ( game_type == GT_NETWORK && !game_host ) ) )
            /* if there is ammo left create a new ball */
            paddle->fire_new_ball = (buttonstate[STK_BUTTON_LEFT] || keystate[config.k_lfire])?-1:1;
        else
            paddle->fire_new_ball = 0;
        /* check if this was first click since last net package and update */
        if ( fired_attached || paddle->fire_new_ball )
            if ( !net_paddle.left_fire && !net_paddle.right_fire ) {
                if ( buttonstate[STK_BUTTON_LEFT] || keystate[config.k_lfire] )
                    net_paddle.left_fire = 1;
                else
                    net_paddle.right_fire = 1;
            }
        paddle->block_new_ball = 1;
    }
}
/*
====================================================================
De/Activate weapon
====================================================================
*/
void weapon_install( Paddle *paddle, int install )
{
    if ( !install ) weapon_stop_fire( paddle );
    paddle->weapon_inst = install;
}
/*
====================================================================
Check if weapon's installed
====================================================================
*/
int weapon_installed( Paddle *paddle )
{
    return paddle->weapon_inst;
}
/*
====================================================================
Start firing at maxium rate until weapon_stop() is called.
====================================================================
*/
void weapon_start_fire( Paddle *paddle )
{
    delay_reset( &paddle->weapon_delay );
    if ( game_type == GT_LOCAL || game_host )
        if ( paddle->weapon_ammo > 0 ) {
            shot_create( paddle );
            paddle->weapon_ammo--;
        }
    paddle->weapon_fire = 1;
}
void weapon_stop_fire( Paddle *paddle )
{
    paddle->weapon_fire = 0;
}
/*
====================================================================
weapon_firing() returns true if weapon is firing
====================================================================
*/
int weapon_firing( Paddle *paddle )
{
    return paddle->weapon_fire;
}

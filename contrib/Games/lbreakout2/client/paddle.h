/***************************************************************************
                          paddle.h  -  description
                             -------------------
    begin                : Thu Sep 6 2001
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

/* if this is not defined paddle has no velocity balls are reflected by convex surface */
#define PADDLE_FRICTION

enum {
    PADDLE_TOP = 0,
    PADDLE_BOTTOM,
    PADDLE_WEAPON_AMMO = 10
};
typedef struct {
    Player *player; /* controlling player */
    int     type; /* either top or bottom */
    float   cur_x;
    int     x, y;
    int     w, h; /* actual geometry */
#ifdef PADDLE_FRICTION
    float   v_x;
#endif
	int		wanted_w; /* resize until w == wanted_w */
    int     len; /* how many middle components ? */
    int     max_len; /* limit */
    float   friction; /* how much relative speed is given to balls ? */
    int     time;
    int     pic_y_offset; /* use paddle tile set at this offset */
    int     pic_x_offset; /* 0 for bottom paddle or 3*tilesize for
        top paddle (if any) */
    int     frozen; /* paddle frozen? */
    int     slime; /* paddle covered with sticky slime */
    int     attract; /* true if paddle attracts balls */
    Delay   resize_delay; /* every delay ms the paddle size is adjusted about two pixels */
    int     invis; /* is paddle invisible? */
    int     invis_delay; /* as long as this isn't timed out the paddle is seen. will be reset by movement */
    SDL_Rect update_rect; /* screen update region */
    /* WEAPON */
    float   weapon_cur;
    int     weapon_inst; /* is weapon installed? */
    int     weapon_fire; /* fire weapon! */
    Delay   weapon_delay; /* delay between shots */
    int     weapon_ammo; /* number of shots available */
    /* BALLS */
    int     ammo; /* number of balls paddle may additional fire to game */
    int     fire_new_ball; /* if this is true balls_update() will fire a new ball
                              and clear this flag */
    int     block_new_ball; /* as long as this is true no new balls
                                    may be fired (is cleared by releasing button or key) */
    int     attached_ball_count; /* number of attached balls */
    /* EXTRAS */
    int     active[EX_NUMBER];
    int     extra_time[EX_NUMBER]; /* paddle specific extras */
    int     wall_y; /* position where wall is drawn */
    float   wall_alpha;
    int     net_bonuses[EX_NUMBER]; /* reference counter for extras collected since 
                                       last message was sent - host only */
} Paddle;

/* LET'S HACK!!! */
typedef struct {
    Coord   cur; /* current position */
    int     x, y;
    Vector  vel; /* velocity components */
    int     attached; /* attached to paddle ? */
    Paddle  *paddle; /* last paddle the ball had contact with */
    int     moving_back; /* ball moves back to paddle as there was no 
                            brick/paddle contact within the last
                            20 secs */
    int     idle_time; /* time passed since last brick/paddle contact */
    int     return_allowed; /* if this is set returning by click is allowed */
    Target  target; /* target in map */
    int     get_target; /* if this is true balls_update() will compute the target
                           and clear this flag */
} Ball;

/* network data struct for paddle */
typedef struct {
    int x; /* x position, y is fix */
    int invis; /* invisible? */
    int left_fire, right_fire; /* the first that was clicked is 
                                  set true until being sent */
} Net_Paddle;

/*
====================================================================
Create/delete a paddle
  'y' is the y coordinate ('x' is simply centered)
  'ammo' is the number of balls the paddle may bring to game
         additionally
====================================================================
*/
Paddle* paddle_create( int y, int ammo );
void paddle_delete( Paddle *paddle );
/*
====================================================================
Freeze paddle movement or set slime.
====================================================================
*/
void paddle_freeze( Paddle *paddle, int freeze );
void paddle_set_slime( Paddle *paddle, int slime );
int paddle_slimy( Paddle *paddle );
/*
====================================================================
Set/check if paddle attracts boni/mali or none.
====================================================================
*/
enum {
    ATTRACT_NONE = 0,
    ATTRACT_BONUS,
    ATTRACT_MALUS
};
void paddle_set_attract( Paddle *paddle, int attract );
int paddle_attract_malus( Paddle *paddle );
int paddle_attract_bonus( Paddle *paddle );
int paddle_check_attract( Paddle *paddle, int extra );
/*
====================================================================
(De)activate ghostly behaviour: paddle is only seen when moved at
maximum 250ms before.
====================================================================
*/
void paddle_set_invis( Paddle *paddle, int invis );
int  paddle_solid( Paddle *paddle );
/*
====================================================================
Resize paddle
====================================================================
*/
int paddle_init_resize( Paddle *paddle, int c );
/*
====================================================================
Detach all balls to the passed direction (-1 or 1) and return True
if there were any balls detached.
====================================================================
*/
int paddle_detach_balls( Paddle *paddle, int dir );
/*
====================================================================
Hide and show paddle on screen.
====================================================================
*/
void paddle_hide( Paddle *paddle );
void paddle_show_shadow( Paddle *paddle );
void paddle_show( Paddle *paddle );
void paddle_alphashow( Paddle *paddle, int a );
/*
====================================================================
Update paddle animation (weapon)
====================================================================
*/
void paddle_update( Paddle *paddle, int ms );
/*
====================================================================
Handle events (used for local paddle)
====================================================================
*/
void paddle_handle_events( Paddle *paddle, int ms );
/*
====================================================================
De/Activate weapon
====================================================================
*/
void weapon_install( Paddle *paddle, int install );
/*
====================================================================
Check if weapon's installed
====================================================================
*/
int weapon_installed( Paddle *paddle );
/*
====================================================================
Start firing at maxium rate until weapon_stop() is called.
====================================================================
*/
void weapon_start_fire( Paddle *paddle );
void weapon_stop_fire( Paddle *paddle );
/*
====================================================================
weapon_firing() returns true if weapon is firing
====================================================================
*/
int weapon_firing( Paddle *paddle );

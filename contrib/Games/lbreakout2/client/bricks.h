/***************************************************************************
                          bricks.h  -  description
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

/* various defines needed by special bricks */
enum { 
    INVIS_BRICK_ID = 6, /* id of invisible brick */
	BRICK_EXP_TIME = 150, /* delay until explosion */
	BRICK_COUNT = 20, /* various brick types */
    BRICK_GROW_FIRST = 10, /* only bricks within this range may grow randomly */
    BRICK_GROW_LAST = 17,
    GROW_BRICK_ID = 19,
    BRICK_HEAL_TIME = 4000 /* after this time duration is increased */
}; 
/* the brick types are not appropiate but in order to use the old
lbreakout functions we need them */
enum {
    MAP_EMPTY = 0,  /* no brick */
    MAP_WALL,       /* can't be destroyed at all */
    MAP_BRICK,      /* may be destroyed if duration is not -1 */
	MAP_BRICK_EXP,  /* explodes neighbors if destroyed */ 
    MAP_BRICK_GROW, /* grow neighbors if destroyed */
    MAP_BRICK_HEAL, /* heals itself when hit */
    MAP_BRICK_CHAOS,/* chaotic reflection by this brick */
};
enum {
    BRICK_WIDTH = 40,
    BRICK_HEIGHT = 20,
    BRICK_SCORE = 100
};
typedef struct {
    int type; /* any of the old MAP_XXX types above */
    int id; /* picture id -- if -1 no brick -- if 0 it's indestructible */
    int dur; /* durability - if -1 & type != MAP_WALL it's only destructible by energy ball */
    int extra; /* extra released when destroyed */
    int score; /* score you get when desctroying this brick */
	int exp_time; /* -1 means inexplosive; value is set by a nearby explosion brick */
    Paddle *exp_paddle; /* paddle that initiated the explosion */
    int heal_time; /* if not -1 counted down and when 0 brick heals one duration (until fully healed) */
	int mx, my; /* a bit hacky but who cares? this conatins the position of an explosion/healing brick */
} Brick;

/* extra conversion item */
typedef struct {
    int  type; /* extra of type */
    char c;   /* is assigned to this character */
} Extra_Conv;
/* brick conversion item */
typedef struct {
    char c;   /* is assigned to this character */
    int  type; /* extra of type */
    int  id; /* pic id */
	int  dur;
	int  score; 
} Brick_Conv;

/* network data struct used to save the brick changes */
enum {
    /* modification types */
    BRICK_HIT = 0,
    BRICK_REMOVE,
    BRICK_HEAL,
    BRICK_GROW
};
typedef struct {
    int used; /* if true this is some valid data */
    int mod; /* destruction, hit, regeneration, growth */
    int hits; /* either number of regen or hit points */
    int dest_type; /* shot, normal, energy, explosion --
                      defined in shrapnells.h */
    int paddle; /* 0 bottom, 1 top */
    int gold_shower;
    int degrees; /* 0 to 360 degrees, direction normal animation moves to */
} Net_Brick;

/*
====================================================================
Load picture containing brick graphics
====================================================================
*/
void bricks_load();
/*
====================================================================
Delete bricks picture
====================================================================
*/
void bricks_delete();
/*
====================================================================
Create bricks from level info.
====================================================================
*/
void bricks_create( Level *level );
/*
====================================================================
Draw all bricks to offscreen surface.
====================================================================
*/
void bricks_draw();
/*
====================================================================
Add brick with clipped shadow to offscreen. To draw a brick without 
shadow check use brick_draw().
====================================================================
*/
void brick_draw_complex( int mx, int my, int px, int py );
/*
====================================================================
Draw brick to passed surface
====================================================================
*/
void brick_draw( SDL_Surface *surf, int map_x, int map_y, int shadow );
/*
====================================================================
Hit brick and remove if destroyed. 'metal' means the ball
destroys any brick with the first try.
type and imp are used for shrapnell creation.
A released extra will vertically move with the signum of extra_dir.
'paddle' is the paddle that initiated hit either by shot or ball.
Return true on destruction
====================================================================
*/
int brick_hit( int map_x, int map_y, int metal, int type, Vector imp, Paddle *paddle );
/*
====================================================================
The actual remove function. If it is called directly there is no
duration check (done by brick_hit) but the brick is destroyed
in any case.
'paddle' is the paddle that initiated hit either by shot or ball.
====================================================================
*/
void brick_remove( int map_x, int map_y, int type, Vector imp, Paddle *paddle );
/*
====================================================================
Push pop extra on/off stack
====================================================================
*/
void bricks_push_extra( int type, int px, int py, int dir );
int bricks_pop_extra( int *type, int *px, int *py, int *dir );
/*
====================================================================
Update a net brick info.
  'mx,my'  map position
  'mod'    BRICK_REMOVE, BRICK_HIT, BRICK_HEAL, BRICK_GROW
  'type'   SHR_BY_NORMAL_BALL | ENERGY_BALL | SHOT | EXPL
  'imp'    the impact vector of SHR_BY_NORMAL_BALL
  *paddle' responsible paddle
====================================================================
*/
void net_brick_set( int mx, int my, int mod, int type, Vector imp, Paddle *paddle );
/*
====================================================================
Make brick at mx,my loose 'points' duration. It must have been
previously checked that this operation is completely valid.
It does not update net_bricks or the player's duration reference.
====================================================================
*/
void brick_loose_dur( int mx, int my, int points );
/*
====================================================================
Initiate a brick explosion.
====================================================================
*/
void brick_start_expl( int x, int y, Paddle *paddle );


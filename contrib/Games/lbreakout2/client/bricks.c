/***************************************************************************
                          bricks.c  -  description
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

#include <math.h>
#include "lbreakout.h"
#include "config.h"
#include "levels.h"
#include "player.h"
#include "display.h"
#include "paddle.h"
#include "shrapnells.h"
#include "bricks.h"

extern SDL_Surface *stk_display;
extern SDL_Surface *offscreen; /* offscreen with bricks, background, frame */
extern SDL_Surface *bkgnd; /* background picture (includes frame) */
Brick bricks[MAP_WIDTH][MAP_HEIGHT];
int brick_count; /* how many bricks out there? used to determine when level is cleared */
int brick_extra_count; /* how many extras? used for network game statistics */
int warp_limit; /* if brick count drops below this count the player may proceed to next level */
int warp_ok; /* true if warping is allowed */
extern int warp_blinks; /* number of warp sign blinks left before it gets static */
extern int warp_x, warp_y;
extern int warp_blink;
extern SDL_Surface *warp_pic;
extern SDL_Surface *brick_pic; /* brick graphics in a horizontal order */
enum { EXTRA_STACK_LIMIT = 20 };
typedef struct { 
    int type; 
    int px, py; 
    int dir; 
} Stack_Extra; /* pushed extra */
Stack_Extra extra_stack[EXTRA_STACK_LIMIT]; /* stack'ed extras released in brick_hit */
int extra_count;
extern int shadow_size;
extern SDL_Surface *brick_shadow; /* shadow mask */
extern Config config;
extern int shine_x, shine_y, shine_recreate;
extern int active[EX_NUMBER];
#ifdef AUDIO_ENABLED
extern StkSound *wav_boom;
#endif
List *exp_bricks = 0; /* list of exploding bricks */
List *heal_bricks = 0; /* list of regenerating bricks */
int grow = 0;
int grow_mask[MAP_WIDTH][MAP_HEIGHT]; /* if grow is set this contains the indices of the 
                                         growing bricks in brick_conv_table. anything out of 0-BRICK_COUNT 
                                         means no brick */
extern int game_type;
extern int game_host;
extern Net_Brick net_bricks[EDIT_WIDTH][EDIT_HEIGHT]; /* used to save brick 
                                                         modifications for net talk -
                                                         host only */

/* extras conversion table */
Extra_Conv extra_conv_table[EX_NUMBER] = {
    { EX_SCORE200,   '0' },
    { EX_SCORE500,   '1' },
    { EX_SCORE1000,  '2' },
    { EX_SCORE2000,  '3' },
    { EX_SCORE5000,  '4' },
    { EX_SCORE10000, '5' },
    { EX_GOLDSHOWER, 'g' },
    { EX_LENGTHEN,   '+' },
    { EX_SHORTEN,    '-' },
    { EX_LIFE,       'l' },
    { EX_SLIME,      's' },
    { EX_METAL,      'm' },
    { EX_BALL,       'b' },
    { EX_WALL,       'w' },
    { EX_FROZEN,     'f' },
    { EX_WEAPON,     'p' },
    { EX_RANDOM,     '?' },
    { EX_FAST,       '>' },
    { EX_SLOW,       '<' },
    { EX_JOKER,      'j' },
	{ EX_DARKNESS,   'd' },
	{ EX_CHAOS,      'c' },
	{ EX_GHOST_PADDLE, '~' },
    { EX_DISABLE,      '!' },
    { EX_TIME_ADD,     '&' },
/*    
    { EX_SPIN_RIGHT,   ')' },
    { EX_SPIN_LEFT,    '(' },
*/    
    { EX_EXPL_BALL,    '*' },
    { EX_BONUS_MAGNET, '}' },
    { EX_MALUS_MAGNET, '{' },
    { EX_WEAK_BALL,    'W' }
};
/* brick conversion table: brick id, char */
Brick_Conv brick_conv_table[BRICK_COUNT] = {
    { 'E', MAP_WALL,        0, -1, 0 },
	{ '#', MAP_BRICK,       1, -1, 1000 },
    { '@', MAP_BRICK_CHAOS, 2, -1, 1000 },
	{ 'a', MAP_BRICK,       3,  1, BRICK_SCORE * 1 },
	{ 'b', MAP_BRICK,       4,  2, BRICK_SCORE * 2 },
	{ 'c', MAP_BRICK,       5,  3, BRICK_SCORE * 3 },
	{ 'v', MAP_BRICK,       6,  4, BRICK_SCORE * 4 },
    { 'x', MAP_BRICK_HEAL,  7,  1, BRICK_SCORE * 2},
    { 'y', MAP_BRICK_HEAL,  8,  2, BRICK_SCORE * 4},
    { 'z', MAP_BRICK_HEAL,  9,  3, BRICK_SCORE * 6},
	{ 'd', MAP_BRICK,      10,  1, BRICK_SCORE },
	{ 'e', MAP_BRICK,      11,  1, BRICK_SCORE },
	{ 'f', MAP_BRICK,      12,  1, BRICK_SCORE },
	{ 'g', MAP_BRICK,      13,  1, BRICK_SCORE },
	{ 'h', MAP_BRICK,      14,  1, BRICK_SCORE },
	{ 'i', MAP_BRICK,      15,  1, BRICK_SCORE },
	{ 'j', MAP_BRICK,      16,  1, BRICK_SCORE },
	{ 'k', MAP_BRICK,      17,  1, BRICK_SCORE },
	{ '*', MAP_BRICK_EXP,  18,  1, BRICK_SCORE * 2 },
	{ '!', MAP_BRICK_GROW, GROW_BRICK_ID,  1, BRICK_SCORE * 2 },
};

/*
====================================================================
Locals
====================================================================
*/

/*
====================================================================
Remove brick from offscreen and screen.
Create shrapnells by type and impulse.
'paddle' is the paddle that initiated hit either by shot or ball.
====================================================================
*/
void brick_remove( int mx, int my, int type, Vector imp, Paddle *paddle )
{
    int px, py, w, h, i, j, shadow;
    int expl_brick = 0; /* explosive bricks have their explosion as animation */
    int dir;
    
	/* if explosive set exp_time of surrounding bricks */
	if ( bricks[mx][my].type == MAP_BRICK_EXP ) {
        expl_brick = 1;
        if ( game_type == GT_LOCAL || game_host ) {
            /* host inits explosions and client receives them
               as normal brick explosions */
            for ( i = mx - 1; i <= mx + 1; i++ )
                for ( j = my - 1; j <= my + 1; j++ )
                    if ( i != mx || j != my ) 
                        if ( bricks[i][j].type != MAP_EMPTY && bricks[i][j].dur > 0 )
                            if ( bricks[i][j].exp_time == -1 )
                                brick_start_expl( i,j,paddle );
        }
        /* add explosion animation */
        exp_create( mx * BRICK_WIDTH + ( BRICK_WIDTH >> 1 ), my * BRICK_HEIGHT + ( BRICK_HEIGHT >> 1 ) );
#ifdef AUDIO_ENABLED
        stk_sound_play( wav_boom );
#endif
	}
    if ( bricks[mx][my].type == MAP_BRICK_GROW ) {
        if ( game_type == GT_LOCAL || game_host ) {
            /* host inits growth and client receives the 
               resulting grow_mask */
            if ( !grow ) memset( grow_mask, 0xff, sizeof( grow_mask ) );
            grow = 1;
            for ( i = mx - 1; i <= mx + 1; i++ )
                for ( j = my - 1; j <= my + 1; j++ )
                    if ( bricks[i][j].type == MAP_EMPTY ) 
                        grow_mask[i][j] = RANDOM( BRICK_GROW_FIRST, BRICK_GROW_LAST );
        }
    }
    
    /* decrease brick count if no indestructible brick was destroyed */
    if ( game_type == GT_LOCAL || game_host )
        if ( bricks[mx][my].dur != -1 ) {
            --brick_count;
            /* if this drops the brick count below the warp limit let's
               activate warping */
            if ( game_type == GT_LOCAL ) {
                if ( !warp_ok ) {
                    if ( brick_count <= warp_limit ) {
                        warp_ok = 1;
                        warp_blinks = 4;
                    }
                }
                else 
                    if ( brick_count > warp_limit ) {
                        warp_ok = 0;
                        warp_blink = 0;
                    }
            }
        }
    /* update stats */
    if ( game_type == GT_NETWORK && game_host )
        paddle->player->bricks_removed++;
	/* remove brick from map */
    bricks[mx][my].id = -1;
    bricks[mx][my].dur = -1;
    bricks[mx][my].exp_time = -1;
    bricks[mx][my].heal_time = -1;
    bricks[mx][my].type = MAP_EMPTY;
    /* mark as removed in players brick map */
    paddle->player->bricks[mx][my] = 0; 
    paddle->player->grown_bricks[mx][my] = 0;
    /* if networked game update net_bricks */
    if ( game_type == GT_NETWORK && game_host )
        net_brick_set( mx, my, BRICK_REMOVE, type, imp, paddle );
    
    /* get screen position */
    px = mx * BRICK_WIDTH;
    py = my * BRICK_HEIGHT;
    
    /* release extra if one exists */
    dir = ( paddle->type == PADDLE_TOP ) ? -1 : 1;
    /* if the host send the gold_shower flag for this the client
       silenty sets a (fake) 1000P bonus and does not rely 
       on the 'active' flags */
    if ( bricks[mx][my].extra != EX_NONE ) {
        bricks_push_extra( bricks[mx][my].extra, px, py, dir );
    }
    else {
        if ( game_type == GT_LOCAL || game_host )
            if ( paddle->active[EX_GOLDSHOWER] )
                bricks_push_extra( EX_SCORE1000, px, py, dir );
    }
    bricks[mx][my].extra = EX_NONE;
    /* get score */
    if ( game_type == GT_LOCAL || game_host )
        player_add_score( paddle->player, bricks[mx][my].score );

	/* in case of darkness no (graphical) remove nescessary */
	if ( active[EX_DARKNESS] ) {
#ifdef AUDIO_ENABLED
    stk_sound_play( wav_boom );
#endif
		return;
	}		

    /* add shrapnells */
    if ( !expl_brick )
        shrapnells_create( px, py, BRICK_WIDTH, BRICK_HEIGHT, type, imp );
    /* recreate shine if needed */
    if (px == shine_x && py == shine_y) shine_recreate = 1;
    /* clear offscreen */
    w = BRICK_WIDTH + shadow_size; h = BRICK_HEIGHT + shadow_size;
    if ( px + w > stk_display->w - BRICK_WIDTH ) 
        w = stk_display->w - BRICK_WIDTH - px;
    stk_surface_clip( offscreen, px, py, w, h );
    stk_surface_blit( bkgnd, px, py, w, h, offscreen, px, py );
    /* if shadow redraw close bricks */
    for ( i = mx - 1; i <= mx + 1; i++ )
        for ( j = my - 1; j <= my + 1; j++ ) {
            if ( i > 0 && j > 0 && i < MAP_WIDTH - 1 ) {
                if ( bricks[i][j].type != MAP_EMPTY ) {
                    if ( i <= mx && j <= my ) shadow = 1; else shadow = 0;
                    brick_draw( offscreen, i, j, shadow );
                }
            }
        }
    stk_surface_clip( offscreen, 0, 0, 0, 0 );
    /* update screen */
    stk_surface_blit( offscreen, px, py, w, h, stk_display, px, py );
    stk_display_store_drect();
#ifdef AUDIO_ENABLED
    stk_sound_play( wav_boom );
#endif
}

/*
====================================================================
Publics
====================================================================
*/

/*
====================================================================
Load picture containing brick graphics
====================================================================
*/
void bricks_load()
{
	exp_bricks = list_create( LIST_NO_AUTO_DELETE, LIST_NO_CALLBACK );
	heal_bricks = list_create( LIST_NO_AUTO_DELETE, LIST_NO_CALLBACK );
}
/*
====================================================================
Delete bricks picture
====================================================================
*/
void bricks_delete()
{
	if ( exp_bricks ) list_delete( exp_bricks ); exp_bricks = 0;
	if ( heal_bricks ) list_delete( heal_bricks ); heal_bricks = 0;
}
/*
====================================================================
Create bricks from level info.
====================================================================
*/
void bricks_create( Level *level )
{
    int i, j, k;
    int y_off;
    
    /* clear explosion/healing list */
    list_clear( exp_bricks );
    list_clear( heal_bricks );
    
    /* clear everything */
    for (i = 0; i < MAP_WIDTH; i++)
        for (j = 0; j < MAP_HEIGHT; j++) {
            bricks[i][j].id = -1;
            bricks[i][j].dur = -1;
            bricks[i][j].type = MAP_EMPTY;
        }
    
    /* build walls */
    for (i = 0; i < MAP_WIDTH; i++)
        if ( game_type == GT_LOCAL ) {
            /* in multiplayer this is open */
            bricks[i][0].id = 0;
            bricks[i][0].dur = -1;
            bricks[i][0].type = MAP_WALL; /* this means - indestructible */
        }
    for (j = 0; j < MAP_HEIGHT; j++) {
        bricks[0][j].id = 0;
        bricks[0][j].dur = -1;
        bricks[0][j].type = MAP_WALL; /* this means - indestructible */
        bricks[MAP_WIDTH - 1][j].id = 0;
        bricks[MAP_WIDTH - 1][j].dur = -1;
        bricks[MAP_WIDTH - 1][j].type = MAP_WALL;
    }

    /* load map -- center if multiplayer */
    if ( game_type == GT_NETWORK ) 
        y_off = ( MAP_HEIGHT - EDIT_HEIGHT ) / 2;
    else
        y_off = 1;
    for (i = 0; i < EDIT_WIDTH; i++)
        for (j = 0; j < EDIT_HEIGHT; j++) {
            /* create bricks */
			bricks[i + 1][j + y_off].exp_time = -1;
			bricks[i + 1][j + y_off].heal_time = -1;
			for ( k = 0; k < BRICK_COUNT; k++ )
				if ( level->bricks[i][j] == brick_conv_table[k].c ) {
					bricks[i + 1][j + y_off].type = brick_conv_table[k].type;
					bricks[i + 1][j + y_off].id = brick_conv_table[k].id;
					bricks[i + 1][j + y_off].dur = brick_conv_table[k].dur;
					bricks[i + 1][j + y_off].score = brick_conv_table[k].score;
					break;
				}
            /* create extras */
			bricks[i + 1][j + y_off].extra = EX_NONE;
			for ( k = 0; k < EX_NUMBER; k++ )
				if ( level->extras[i][j] == extra_conv_table[k].c ) {
					bricks[i + 1][j + y_off].extra = extra_conv_table[k].type;
					break;
				}
        }

    /* count bricks & extras */
    brick_count = 0; brick_extra_count = 0;
    for (i = 1; i < MAP_WIDTH - 1; i++)
        for (j = 1; j < MAP_HEIGHT - 1; j++) {
            if ( bricks[i][j].dur > 0 ) {
                brick_count++;
                if ( bricks[i][j].extra != EX_NONE )
                    brick_extra_count++;
            }
        }
    warp_limit = ( 100 - config.rel_warp_limit ) * brick_count / 100;
    warp_ok = 0;
}
/*
====================================================================
Draw all bricks to offscreen surface.
====================================================================
*/
void bricks_draw()
{
    int i, j;
    if ( offscreen == 0 ) return;
    stk_surface_clip( offscreen, 0, 0, stk_display->w - BRICK_WIDTH, stk_display->h );
    for ( j = 1; j < MAP_HEIGHT - 1; j++ )
        for ( i = 1; i < MAP_WIDTH - 1; i++ )
            if ( bricks[i][j].id >= 0 )
                brick_draw( offscreen, i, j, 1 );
    stk_surface_clip( offscreen, 0,0,0,0 );
}
/*
====================================================================
Draw brick to passed surface
====================================================================
*/
void brick_draw( SDL_Surface *surf, int map_x, int map_y, int shadow )
{
    int x = map_x * BRICK_WIDTH, y = map_y * BRICK_HEIGHT;
    /* dont draw invisible bricks */
    if ( bricks[map_x][map_y].id == INVIS_BRICK_ID ) return;
    /* add shadow */
    if ( shadow ) {
        stk_surface_alpha_blit( brick_shadow, 
            bricks[map_x][map_y].id * BRICK_WIDTH, 0, 
            BRICK_WIDTH, BRICK_HEIGHT,
            surf, x + shadow_size, y + shadow_size, SHADOW_ALPHA );
    }
    /* brick if not frame brick */
    if ( map_x == 0 || map_y == 0 || map_x == MAP_WIDTH - 1 ) return;
    stk_surface_blit( brick_pic, 
        bricks[map_x][map_y].id * BRICK_WIDTH, 0, 
        BRICK_WIDTH, BRICK_HEIGHT, surf, x,y );
}
/*
====================================================================
Add brick with clipped shadow to offscreen. To draw a brick without 
shadow check use brick_draw().
====================================================================
*/
void brick_draw_complex( int mx, int my, int px, int py )
{
    brick_draw( offscreen, mx, my, 1 );
    /* redraw surrounding bricks */
    stk_surface_clip( offscreen, px + shadow_size, py + shadow_size, BRICK_WIDTH, BRICK_HEIGHT );
    if ( mx + 1 == MAP_WIDTH - 1 ) {
        /* right frame part */
        stk_surface_blit( bkgnd, px + BRICK_WIDTH, py,
            BRICK_WIDTH, ( BRICK_HEIGHT << 1 ),
            offscreen, px + BRICK_WIDTH, py );
    }
    else {
        brick_draw( offscreen, mx + 1, my, 0 );
        brick_draw( offscreen, mx + 1, my + 1, 0 );
    }
    if ( bricks[mx][my + 1].type != MAP_EMPTY )
        brick_draw( offscreen, mx, my + 1, 0 );
    stk_surface_clip( offscreen, 0, 0, 0, 0 );
}
/*
====================================================================
Hit brick and remove if destroyed. 'metal' means the ball
destroys any brick with the first try.
type and imp are used for shrapnell creation.
'extra' contains the pushed extra if one was released.
'paddle' is the paddle that initiated hit either by shot or ball.
Return true on destruction
====================================================================
*/
int brick_hit( int mx, int my, int metal, int type, Vector imp, Paddle *paddle )
{
    int remove = 0;
    int loose_dur = 0;
    
    /* a map wall can't be touched */
    if ( bricks[mx][my].type == MAP_WALL ) return 0;

    /* if metal ball resistance is futile */
    if ( metal )
        remove = 1;
    else {
        if ( bricks[mx][my].dur == -1 ) return 0; /* duration of -1 means only breakable by engery ball (metal ball) */
        if ( bricks[mx][my].dur <= 1 )
            remove = 1;
        else
            loose_dur = 1;
    }

    if ( remove ) 
        brick_remove( mx, my, type, imp, paddle );
    else
        if ( loose_dur )  {
            /* if networked game update net_bricks */
            if ( game_type == GT_NETWORK && game_host )
                net_brick_set( mx, my, BRICK_HIT, SHR_BY_NORMAL_BALL, imp, paddle );
            brick_loose_dur( mx, my, 1 );
            paddle->player->bricks[mx][my] = bricks[mx][my].dur;
        }
    return remove;
}
/*
====================================================================
Get released extra from queue if any.
====================================================================
*/
void bricks_push_extra( int type, int px, int py, int dir )
{
    if ( extra_count == EXTRA_STACK_LIMIT ) return;
    extra_stack[extra_count].type = type;
    extra_stack[extra_count].px = px;
    extra_stack[extra_count].py = py;
    extra_stack[extra_count].dir = dir;
    extra_count++;
}
int bricks_pop_extra( int *type, int *px, int *py, int *dir )
{
    if ( extra_count == 0 ) return 0;
    extra_count--;
    *type = extra_stack[extra_count].type;
    *px = extra_stack[extra_count].px;
    *py = extra_stack[extra_count].py;
    *dir = extra_stack[extra_count].dir;
    return 1;
}
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
void net_brick_set( int mx, int my, int mod, int type, Vector imp, Paddle *paddle )
{
    Net_Brick *brick = &net_bricks[mx - 1][my - (MAP_HEIGHT - EDIT_HEIGHT) / 2];
    /* update hits, regen points */
    switch ( mod ) {
        case BRICK_REMOVE: 
        case BRICK_GROW:
            brick->hits = 0; 
            break;
        case BRICK_HIT:
            if ( brick->used && brick->mod == BRICK_HEAL )
                brick->hits--;
            else
                brick->hits++;
            break;
        case BRICK_HEAL:
            if ( brick->used && brick->mod == BRICK_HIT )
                brick->hits--;
            else
                brick->hits++;
            break;
    }
    /* set used */
    brick->used = 1;
    /* modification and destruction type */
    brick->mod = mod;
    brick->dest_type = type;
    /* paddle */
    if ( mod == BRICK_REMOVE )
        brick->paddle = (paddle->type == PADDLE_TOP);
    /* if this brick offers no extra and goldshower is 
       set we'll have to release a 1000P -- this flag is not
       reset when this isn't the case as a previous update may
       have stated True */
    if ( mod == BRICK_REMOVE )
        if ( bricks[mx][my].extra == EX_NONE && paddle->active[EX_GOLDSHOWER] )
            brick->gold_shower = 1;
    if ( mod == BRICK_REMOVE && type == SHR_BY_NORMAL_BALL ) {
        /* translate impact vector into degrees (0°: horizontally right, clockwise going) */
        if ( imp.x == 0 )
            brick->degrees = 90; /* basically impossible as we mask this out */
        else
            brick->degrees = (int)(360 * atan( (double)(fabs(imp.y))/fabs(imp.x) ) / 6.28);
        /* get the proper quartal */
        if ( imp.x > 0 ) {
            if ( imp.y < 0 )
                brick->degrees = 360 - brick->degrees;
        }
        else {
            if ( imp.y >= 0 )
                brick->degrees = 180 - brick->degrees;
            else
                brick->degrees = 180 + brick->degrees;
        }
    }
    else
        brick->degrees = 0;
}
/*
====================================================================
Make brick at mx,my loose 'points' duration. It must have been
previously checked that this operation is completely valid.
It does not update net_bricks or the player's duration reference.
====================================================================
*/
void brick_loose_dur( int mx, int my, int points )
{
    int px, py;
    int refresh_h, refresh_w;
    while ( points-- > 0 ) {
        bricks[mx][my].dur--;
        bricks[mx][my].id--;
        /* set regeneration time if it's a healing brick */
        if ( game_type == GT_LOCAL || game_host ) {
            if ( bricks[mx][my].type == MAP_BRICK_HEAL ) {
                /* if this brick is already healing just reset the time
                  but don't add to the list again */
                if ( bricks[mx][my].heal_time != -1 )
                    bricks[mx][my].heal_time = BRICK_HEAL_TIME;
                else {
                    bricks[mx][my].mx = mx;
                    bricks[mx][my].my = my;
                    bricks[mx][my].heal_time = BRICK_HEAL_TIME;
                    list_add( heal_bricks, &bricks[mx][my] );
                }
            }
        }
        if ( !active[EX_DARKNESS]) {
            px = mx * BRICK_WIDTH;
            py = my * BRICK_HEIGHT;
            refresh_w = BRICK_WIDTH+shadow_size;
            refresh_h = BRICK_HEIGHT+shadow_size;
            /* copy background as old brick may have different transparency 
               do this in three parts to save computation */
            stk_surface_blit( bkgnd, px, py, 
                shadow_size, BRICK_HEIGHT,
                offscreen, px, py );
            stk_surface_blit( bkgnd, px + shadow_size, py, 
                BRICK_WIDTH - shadow_size, BRICK_HEIGHT + shadow_size,
                offscreen, px + shadow_size, py );
            stk_surface_blit( bkgnd, px + BRICK_WIDTH, py + shadow_size,
                shadow_size, BRICK_HEIGHT,
                offscreen, px + BRICK_WIDTH, py + shadow_size );
            /* draw brick + surrounding */
            brick_draw_complex( mx, my, px, py );
            /* udpate screen */
            stk_surface_blit( offscreen, px, py, 
                refresh_w, refresh_h, stk_display, px, py );
            stk_display_store_drect();
        }
    }
}
/*
====================================================================
Initiate a brick explosion.
====================================================================
*/
void brick_start_expl( int x, int y, Paddle *paddle )
{
    bricks[x][y].exp_time = BRICK_EXP_TIME;
    bricks[x][y].exp_paddle = paddle;
    bricks[x][y].mx = x; bricks[x][y].my = y;
    list_add( exp_bricks, &bricks[x][y] );
}


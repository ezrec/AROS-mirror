/***************************************************************************
                          balls.c  -  description
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
#include "difficulty.h"
#include "levels.h"
#include "shrapnells.h"
#include "player.h"
#include "display.h"
#include "paddle.h"
#include "shots.h"
#include "balls.h"
#include "bricks.h"

#define BALLS_IDLE_LIMIT 10000

#define TOARC(d) (((float)d/180)*M_PI)
#define TODEG(a) (((float)a/M_PI)*180)
#define VLEN(x, y) ( sqrt( (x)*(x) + (y)*(y) ) )
#define REC_SQRT_2 (0.707106781)

extern SDL_Surface *ball_pic; /* ball pictures */
extern SDL_Surface *ball_shadow;
int ball_pic_x_offset = 0; /* display ball at this position */
float ball_vhmask = 0.363970234; /* twenty degrees */
float ball_vvmask = 5.67128182; /* ten degrees */
int ball_rad = 6;
int ball_dia = 12;
int ball_w = 12;
int ball_h = 12;
List *balls = 0;
float ball_vm; /* v max */
float ball_v; /* ball's speed */
float ball_old_v; /* old speed (before extra used) */
float ball_v_add;
int ball_metal = 0; /* ball gets through metal */
int ball_weak = 0; /* 30% that a ball is unable to damage a brick */
int ball_expl = 0; /* like an explosibve brick a ball causes explosion */
float ball_metal_alpha_change = 1.2; /* pulse factor */
float ball_metal_alpha = 0; /* alpha of ball when blue */
int ball_chaos = 0; /* chaotic ball reflection */
extern SDL_Surface *offscreen;
extern SDL_Surface *stk_display;
extern Config config;
extern Brick bricks[MAP_WIDTH][MAP_HEIGHT];
extern int keystate[SDLK_LAST];
extern int buttonstate[STK_BUTTON_COUNT];
extern int shadow_size;
#ifdef AUDIO_ENABLED
extern StkSound *wav_reflect;
extern StkSound *wav_attach;
#endif
extern int active[EX_NUMBER];
extern char circle_msg[256];
extern StkFont *font;
extern int game_type;
extern Paddle *paddles[4];
extern Paddle *l_paddle, *r_paddle;
extern int paddle_count;
extern int game_type, level_type;
extern int game_host;
extern int net_ball_reflected, net_ball_attached;

/*
====================================================================
Locals
====================================================================
*/

/*
====================================================================
Add a little bit of entropy or not when masking the ball
velocity.
====================================================================
*/
enum {
    NO_ENTROPY = 0,
    ADD_ENTROPY
};

#ifdef WITH_BUG_REPORT
/*
====================================================================
Display info about a ball's target.
====================================================================
*/
static void ball_print_target_info( Ball *ball )
{
    Coord  center = { 
        ball->cur.x + ball_rad, 
        ball->cur.y + ball_rad }; /* ball center */
    printf( "Target exists: %i\n", ball->target.exists );
    printf("Ball: %4.2f,%4.2f (%i,%i) -> %4.2f,%4.2f (%4.2f)\n", 
        center.x, center.y, (int)center.x/BRICK_WIDTH, (int)center.y/BRICK_HEIGHT,
        ball->vel.x, ball->vel.y, ball->vel.y/ball->vel.x );
    printf("Brick %i,%i: Side %i (%4.2f,%4.2f)\n", ball->target.mx, ball->target.my, ball->target.side, ball->target.x, ball->target.y );
    printf("Perp Vector: %4.2f,%4.2f\n", ball->target.perp_vector.x, ball->target.perp_vector.y);
    printf("Takes %i ms\n", ball->target.time);
}
#endif

/*
====================================================================
Clear contents of target.
====================================================================
*/
void ball_clear_target( Target *t )
{
    memset(t, 0, sizeof(Target));
    t->side = SIDE_UNDEFINED;
}
/*
====================================================================
Attach ball to paddle.
====================================================================
*/
void ball_attach( Ball *ball, Paddle *paddle )
{
    /* relative position */
    ball->attached = 1;
    ball->paddle = paddle;
    ball->paddle->attached_ball_count++;
    ball->idle_time = ball->moving_back = ball->return_allowed = 0;
    ball->cur.x -= paddle->x;
    ball->cur.y -= paddle->y;
    ball->x = (int)ball->cur.x;
    ball->y = (int)ball->cur.y;
#ifdef AUDIO_ENABLED
    stk_sound_play( wav_attach );
    if ( game_type == GT_NETWORK && game_host )
        net_ball_attached = 1; /* used to let client play this sound */
#endif
}
/*
====================================================================
Reflect ball at brick assume normed perp_vector.
====================================================================
*/
void ball_check_brick_reflection( Ball *b )
{
    float old_vx;
    Vector n;
    int reflect;
    int chaos_reflect;
    
	/* time left? */
    if (b->target.cur_tm < b->target.time) return;

    /* if the brick is destructible (thus it'll take damage)
     * we must reset the idle time
     */
    if ( bricks[b->target.mx][b->target.my].dur != -1 || ( ball_metal && bricks[b->target.mx][b->target.my].type != MAP_WALL ) )
        b->idle_time = 0;
    /* or if it is within the last four rows and no chaotic malus is active it will
       hit the paddle soon so reset here too */
    if ( b->target.my >= MAP_HEIGHT - 4 && b->vel.y > 0 && !active[EX_CHAOS] )
        b->idle_time = 0;
    
    /* will reflect? */
    reflect = 1;
    if ( ball_metal && bricks[b->target.mx][b->target.my].type != MAP_WALL )
        reflect = 0;
    
    /* will reflect chaotic? */
    chaos_reflect = 0;
    if ( active[EX_CHAOS] || bricks[b->target.mx][b->target.my].type == MAP_BRICK_CHAOS )
        chaos_reflect = 1;
    
    /* remove brick -- if weak ball there is a 40% chance that no damage is done to the brick */
    if ( !ball_weak || rand() % 10 < 6 ) {
        /* if explosive ball und brick is destructible by normal means set as explosive */
        if ( ball_expl && bricks[b->target.mx][b->target.my].dur > 0 ) {
            bricks[b->target.mx][b->target.my].type = MAP_BRICK_EXP;
            bricks[b->target.mx][b->target.my].dur = 1;
        }
        /* hit brick */
        if ( ball_metal )
            brick_hit( b->target.mx, b->target.my, ball_metal, SHR_BY_ENERGY_BALL, b->vel, b->paddle );
        else
            brick_hit( b->target.mx, b->target.my, ball_metal, SHR_BY_NORMAL_BALL, b->vel, b->paddle );
    }

	/* we have a target and so we have a reset position and even if the ball's
	   not reflected the position must be reset */
    b->cur.x = b->target.x; b->x = (int)b->cur.x;
    b->cur.y = b->target.y; b->y = (int)b->cur.y;
	
    if ( reflect ) {
#ifdef AUDIO_ENABLED
        stk_sound_play( wav_reflect );
        if ( game_type == GT_NETWORK && game_host )
            net_ball_reflected = 1; /* used to let client play this sound */
#endif
        old_vx = b->vel.x;
		if ( !chaos_reflect ) {
        	/* normal reflection */
	        n.x = (1 - 2 * b->target.perp_vector.x * b->target.perp_vector.x) * b->vel.x + (  - 2 * b->target.perp_vector.x * b->target.perp_vector.y) * b->vel.y;
    	    n.y = (  - 2 * b->target.perp_vector.x * b->target.perp_vector.y) * b->vel.x + (1 - 2 * b->target.perp_vector.y * b->target.perp_vector.y) * b->vel.y;
        	b->vel.x = n.x;
        	b->vel.y = n.y;
		}
		else {
			b->vel.x = ((float)RANDOM( -10000, 10000 )) / 10000;
			b->vel.y = (float)(RANDOM( -10000, 10000 )) / 10000;
		}	
        if ( b->target.side >= CORNER_UPPER_LEFT && !chaos_reflect )
            ball_mask_vel( b, old_vx, ADD_ENTROPY );
        else
            ball_mask_vel( b, old_vx, NO_ENTROPY );
    }
    /* mark target as disabled so it won't get stuck at the 
       bottom of the screen but keep the target position so 
       that we know what needs an update. */
    b->target.exists = 0;
    /* check targets */
   	balls_check_targets( b->target.mx, b->target.my );
   	shots_check_targets( b->target.mx, b->target.my );
}
/*
====================================================================
Handle ball's contact with paddle: reflect at perpendicular (normed)
or attach.
====================================================================
*/
void ball_handle_paddle_contact( Ball *ball, Paddle *paddle, Vector perp_vector )
{
    float  old_vx = ball->vel.x;
    Vector c; /* A(perp_vector) = c; */

    ball->paddle = paddle;
    /* attach ball or reflect */
    if ( paddle_slimy( paddle ) ) {
        ball_attach( ball, paddle );
    }
    else {
        /* assume that perpendicular is normed */
        /* reflect */
        if ( perp_vector.x || perp_vector.y ) {
            /* a simple 2x2 matrix does this for us */
            c.x = (1 - 2 * perp_vector.x * perp_vector.x) * ball->vel.x + (-2 * perp_vector.x * perp_vector.y) * ball->vel.y;
            c.y = (-2 * perp_vector.x * perp_vector.y) * ball->vel.x + (1 - 2 * perp_vector.y * perp_vector.y) * ball->vel.y;
            /* if this new velocity vector does not bring back the ball to the playing field
               thus the lower hemispherical parts of the paddle were hit we consider this
               to be no reflection at all to prevent balls from getting stuck when 'bonus floor'
               is active */
            if ( (paddle->type == PADDLE_TOP && c.y < 0) ||
                 (paddle->type == PADDLE_BOTTOM && c.y > 0) )
                return;
            /* set new speed vector */
            ball->vel.x = c.x; ball->vel.y = c.y;
#ifdef PADDLE_FRICTION
            /* transfer friction to ball's velocity if not convex */
            if ( !config.convex )
                ball->vel.x += paddle->v_x * paddle->friction;
#endif
            /* adjust velocity */
            ball_mask_vel( ball, old_vx, NO_ENTROPY );
            /* or reset position if in wall */
            if ( ball->x < BRICK_WIDTH ) {
                ball->cur.x = BRICK_WIDTH;
                ball->x = (int)ball->cur.x;
            }
            else
                if ( ball->x + ball_dia >= stk_display->w - BRICK_WIDTH ) {
                    ball->cur.x = stk_display->w - BRICK_WIDTH - ball_dia;
                    ball->x = (int)ball->cur.x;
                }
            if ( paddle->active[EX_WALL] ) {
                if ( paddle->type == PADDLE_BOTTOM ) {
                    if ( ball->cur.y + ball_dia > stk_display->h - BRICK_HEIGHT - 1 ) {
                        ball->cur.y = stk_display->h - BRICK_HEIGHT - 1 - ball_dia;
                        ball->y = (int)ball->cur.y;
                    }
                }
                else {
                    if ( ball->cur.y < BRICK_HEIGHT ) {
                        ball->cur.y = BRICK_HEIGHT;
                        ball->y = (int)ball->cur.y;
                    }
                }
            }
            /* get new target */
            ball->get_target = 1;
#ifdef AUDIO_ENABLED
            stk_sound_play( wav_reflect );
            if ( game_type == GT_NETWORK && game_host )
                net_ball_reflected = 1; /* used to let client play this sound */
#endif
        }
		else
			fprintf( stderr, "ERROR: Can't reflect at perpendicular (0,0)!\n" );
    }
    /* count hits for statistics */
    if ( game_type == GT_NETWORK && level_type == LT_DEATHMATCH )
        ball->paddle->player->hits++;
}
/*
====================================================================
Berechnung der Schnittpunkte der Geraden, die orthogonal zur
Geraden der Ballgeschwindigkeit durch den Ballmittelpunkt verläuft,
also der tangentialen Geschwindigkeitspunkte.
Der Geschwindigkeitsvektor wird auf 1 genormt. Ausgehend vom
Mittelpunkt wird der dazu orthogonale Vektor jeweils mit geändertem
Vorzeichen addiert und ergibt so die Tangentialpunkte.

If you're able and willing to translate this please send me your
result. ;-)
====================================================================
*/
void ball_get_tangents( Ball *ball, Coord *left, Coord *right )
{
    Vector norm_vel = ball->vel;
    float center_x = ball->cur.x + ball_rad, center_y = ball->cur.y + ball_rad;
    vector_norm( &norm_vel );
    left->x = center_x + norm_vel.y * ball_rad;
    left->y = center_y - norm_vel.x * ball_rad;
    right->x = center_x - norm_vel.y * ball_rad;
    right->y = center_y + norm_vel.x * ball_rad;
}
/*
====================================================================
Check if the ball is on paddle's level and an reflect is
possible.
====================================================================
*/
int ball_paddle_contact_possible( Ball *ball, Paddle *paddle, Vector old )
{
	if ( !paddle_solid( paddle ) ) return 0;
    if ( paddle->type == PADDLE_TOP ) {
        if ( ball->vel.y > 0 ) return 0;
        if ( ball->y > paddle->y + paddle->h - 1 ) return 0; /* below paddle */
        if ( ball->y + ball_dia <= paddle->y + ( paddle->h >> 1 ) )
            if ( old.y + ball_dia <= paddle->y + ( paddle->h >> 1 ) ) 
                return 0; /* already behind paddle */
    }
    else {
        if ( ball->vel.y < 0 ) return 0; /* ball moves up so no contact possible because if it was below the paddle it has been 
                                            reflected by the bonus floor and MUST ignore the paddle */
        if ( ball->y + ball_dia < paddle->y ) return 0; /* above paddle */
        if ( ball->y >= paddle->y + ( paddle->h >> 1 ) )
            if ( old.y >= paddle->y + ( paddle->h >> 1 ) ) return 0; /* already behind paddle */
    }
	return 1;			
}
/*
====================================================================
Check reflection of ball at paddle. 'old' is the position of
the ball before update. Used to compute direction.
====================================================================
*/
enum { CONTACT_LEFT = 1, CONTACT_MIDDLE, CONTACT_RIGHT };
void ball_check_paddle_reflection( Ball *ball, Paddle *paddle )
{
    Line    ball_line; /* balls velocity line */
    Line    paddle_line; /* paddle line */
    Coord   pt, pt2; /* auxiliary point (result of intersection) */
    int     contact = 0; /* paddle contact */
    Vector  perp_vector; /* perpendicular of ball's direction change */
    Coord   center = { ball->cur.x + ball_rad, ball->cur.y + ball_rad }; /* center of ball */
    Vector  norm_vel; /* normed ball velocity vector */
    /* paddle is constructed as two hemispheres at the side and a cylinder in the middle */
    Coord   right_hemi_center = { paddle->x + paddle->w - ( paddle->h >> 1 ), paddle->y + ( paddle->h >> 1 ) };
    Coord   left_hemi_center = { paddle->x + ( paddle->h >> 1 ), paddle->y + ( paddle->h >> 1 ) };
    /* radius of hemispheres */
    int     hemi_r = ( ball_rad ) + ( paddle->h >> 1 );
    /* if paddle's treated as convex these are the perpendiculars through the hemisphere centers */
    Vector  left_convex_perp = { 1, (paddle->type == PADDLE_TOP)?-1:1 };
    /* paddle center */
    Coord   paddle_center = { paddle->x + ( paddle->w >> 1 ), paddle->y + ( paddle->h >> 1 ) };
    /*  center of the convex behaviour -- computed when reflecting by using
        left/right_convex_perp and paddle_center */
    Coord   convex_center;
    /* perpendicular line used for convex behaviour */
    Line    convex_line;

	/* the simple check for the y-position of ball and paddle is done 
     * in ball_paddle_contact_possible() so if we got here it's possible 
     * by velocity and position of ball that it hits the paddle 
     */

    /*  basic idea:
        The paddle is constructed of a middle rectangle and two hemispheres.
        We check the center line of the ball with the imaginary paddle that's size
        is paddle_size + ball_rad. The intersection with this paddle is the reset point
        for the ball at the same time (if sticky).
        The perpendicular is computed as convex thing. (overwrites the perpendicular
        set by the reflection)
    */
    /* ball line */
    line_set( &ball_line, center.x, center.y, ball->vel.y / ball->vel.x );
    /* imaginary paddle upper/lower line 
     * -- we'll decide at intersection which hemipshere to check 
     */
    if ( paddle->type == PADDLE_TOP )
        line_set_hori( &paddle_line, paddle->y + paddle->h - 1 + ball_rad );
    else
        line_set_hori( &paddle_line, paddle->y - ball_rad );
    line_intersect( &paddle_line, &ball_line, &pt );
    if ( pt.x < left_hemi_center.x ) {
        /* intersect left */
        norm_vel = ball->vel; vector_norm( &norm_vel );
        if ( circle_intersect( left_hemi_center, hemi_r,
                               center, norm_vel,
                               &pt, &pt2 ) ) {
            if ( VEC_DIST( center, left_hemi_center ) <= hemi_r ) {
                if ( paddle->type == PADDLE_TOP ) {
                    /* use lower point as intersection */
                    if ( pt.y < pt2.y ) pt = pt2;
                }
                else    
                    /* use the higher point as this is the upper intersection */
                    if ( pt.y > pt2.y ) pt = pt2;
                /* use vector between hemi_sphere center and ball center as reflection */
                perp_vector = vector_get( center.x - left_hemi_center.x,
                                          center.y - left_hemi_center.y );
                vector_norm( &perp_vector );
                /* had contact */
                contact = CONTACT_LEFT;
            }
        }
    }
    else
        if ( pt.x > right_hemi_center.x ) {
            /* intersect right */
            norm_vel = ball->vel; vector_norm( &norm_vel );
            if ( circle_intersect( right_hemi_center, hemi_r,
                                   center, norm_vel,
                                   &pt, &pt2 ) ) {
                if ( VEC_DIST( center, right_hemi_center ) <= hemi_r ) {
                    if ( paddle->type == PADDLE_TOP ) {
                        /* use lower point as intersection */
                        if ( pt.y < pt2.y ) pt = pt2;
                    }
                    else    
                        /* use the higher point as this is the upper intersection */
                        if ( pt.y > pt2.y ) pt = pt2;
                    /* use vector between hemi_sphere center and ball center as reflection */
                    perp_vector = vector_get( center.x - right_hemi_center.x,
                                              center.y - right_hemi_center.y );
                    vector_norm( &perp_vector );
                    /* had contact */
                    contact = CONTACT_RIGHT;
                }
            }
        }
        else {
            contact = CONTACT_MIDDLE; /* contact with middle part */
            perp_vector = vector_get( 0, 1 ); /* reflect at horizontal line */
        }
    /* if we got here 'pt' contains the intersection with the imaginary paddle so reset ball
    to this position */
    if ( contact ) {
        /* reset idle time */
        ball->idle_time = 0;
        /* reset position if ball will be attached */
        if ( paddle_slimy( paddle ) ) {
            ball->cur.x = pt.x - ( ball_rad );
            ball->cur.y = pt.y - ( ball_rad );
            ball->x = (int)ball->cur.x; ball->y = (int)ball->cur.y;
        }
        /* convex perpendicular */
        if ( config.convex ) {
            line_set_vert( &paddle_line, paddle_center.x );
            line_set( &convex_line, left_hemi_center.x, left_hemi_center.y,
                      vector_monotony( left_convex_perp ) );
            line_intersect( &paddle_line, &convex_line, &convex_center );
            /* get actual perp_vector */
            perp_vector.x = convex_center.x - pt.x;
            perp_vector.y = convex_center.y - pt.y;
            //vector_norm( &perp_vector );
            /* this vector is not normed but for whatever reason... 
               the reflection behaviour is much nicer this way */
        }
        /* handle contact: attach, reflect, sound... */
        ball_handle_paddle_contact( ball, paddle, perp_vector );
    }
}
/*
====================================================================
Intersect ball line with imaginary brick line.
Use target's map position and 
set reset position (centered) and perp_vector of target.
ball_rad is substracted later in ball_get_target()
====================================================================
*/
enum { LINE_HORI = 0, LINE_VERT };
int check_line( Line *ball_line, int type, int anchor, int range_start, int range_end, Coord *pt ) {
    Line line;
    if ( type == LINE_HORI )
        line_set_hori( &line, anchor );
    else
        line_set_vert( &line, anchor );
    line_intersect( &line, ball_line, pt );
    if ( type == LINE_HORI ) {
        if ( pt->x >= range_start && pt->x <= range_end ) return 1;
        return 0;
    }
    else {
        if ( pt->y >= range_start && pt->y <= range_end ) return 1;
        return 0;
    }
}
void ball_intersect_brick( Ball *ball, Target *target )
{
    Line  ball_line;
    Coord pt; /* auxiliary point */
    int   x = target->mx * BRICK_WIDTH, y = target->my * BRICK_HEIGHT; /* left upper corner of brick */
    int   intersect = 0; /* intersected? */

    /* ball_line */
    line_set( &ball_line, ball->cur.x + ball_rad, ball->cur.y + ball_rad, ball->vel.y / ball->vel.x );

    if ( ball->vel.x > 0 ) {
        /* left */
        if ( check_line( &ball_line,
                         LINE_VERT,
                         x - ball_rad,
                         y - ball_rad, y + BRICK_HEIGHT + ball_rad,
                         &pt ) ) {
            intersect = 1;
            target->perp_vector = vector_get( 1, 0 );
        }
    }
    else {
        /* right */
        if ( check_line( &ball_line,
                         LINE_VERT,
                         x + BRICK_WIDTH + ball_rad,
                         y - ball_rad, y + BRICK_HEIGHT + ball_rad,
                         &pt ) ) {
            intersect = 1;
            target->perp_vector = vector_get( 1, 0 );
        }
    }
    if ( !intersect ) {
    if ( ball->vel.y > 0 ) {
        /* top */
        if ( check_line( &ball_line,
                         LINE_HORI,
                         y - ball_rad,
                         x - ball_rad, x + BRICK_WIDTH + ball_rad,
                         &pt ) ) {
            intersect = 1;
            target->perp_vector = vector_get( 0, 1 );
        }
    }
    else {
        /* bottom */
        if ( check_line( &ball_line,
                         LINE_HORI,
                         y + BRICK_HEIGHT + ball_rad,
                         x - ball_rad, x + BRICK_WIDTH + ball_rad,
                         &pt ) ) {
            intersect = 1;
            target->perp_vector = vector_get( 0, 1 );
        }
    }
    }
    /* intersected */
    if ( intersect ) {
        target->x = pt.x;
        target->y = pt.y;
        /* perp_vector is set */
    }
}
/*
====================================================================
Reflect ball at target at target->side and set perp_vector
and reset position x,y of the target. Does not update the ball.
====================================================================
*/
void ball_reflect_at_side( Ball *ball, Target *target )
{
    float old_vx;
    int   compute_vel, start;
	Line  ball_line;
	Line  brick_line;
	Coord pt;
	/* ball line */
    line_set( &ball_line, ball->cur.x + ball_rad, ball->cur.y + ball_rad, ball->vel.y / ball->vel.x );
	/* brick line and perp vector */
	switch ( target->side ) {
		case SIDE_LEFT:
			line_set_vert( &brick_line, target->mx * BRICK_WIDTH - ball_rad );
			target->perp_vector = vector_get( 1, 0 );
			break;
		case SIDE_RIGHT:
			line_set_vert( &brick_line, target->mx * BRICK_WIDTH + BRICK_WIDTH + ball_rad );
			target->perp_vector = vector_get( 1, 0 );
			break;
		case SIDE_TOP:
			line_set_hori( &brick_line, target->my * BRICK_HEIGHT - ball_rad );
			target->perp_vector = vector_get( 0, 1 );
			break;
		case SIDE_BOTTOM:
			line_set_hori( &brick_line, target->my * BRICK_HEIGHT + BRICK_HEIGHT + ball_rad );
			target->perp_vector = vector_get( 0, 1 );
			break;
		default:
			fprintf( stderr, "Unknown side: %i\n", target->side );
			break;
	}
	/* intersect, it's already assured that we hit this brick so just get the reset position */
	line_intersect( &brick_line, &ball_line, &pt );
	target->x = pt.x;
	target->y = pt.y;
    /* check if ball slid into next brick because of high angle when
       reflect at side (not corner) */
    compute_vel = 0;
    switch ( target->side ) {
        case SIDE_BOTTOM:
        case SIDE_TOP:
            if ( ball->vel.x > 0 )
                pt.x = target->x + ball_rad;
            else
                pt.x = target->x - ball_rad;
            start = (int)pt.x / BRICK_WIDTH;
            if ( bricks[start][(int)target->y/BRICK_HEIGHT].type != MAP_EMPTY ) {
                if ( ball->vel.x > 0 )
                    target->x = start * BRICK_WIDTH - ball_rad - 1;
                else
                    target->x = (start+1) * BRICK_WIDTH + ball_rad;
                compute_vel = 1;
            }
            break;
        case SIDE_LEFT:
        case SIDE_RIGHT:
            if ( ball->vel.y > 0 )
                pt.y = target->y + ball_rad;
            else
                pt.y = target->y - ball_rad;
            start = (int)pt.y / BRICK_HEIGHT;
            if ( bricks[(int)target->x/BRICK_WIDTH][start].type != MAP_EMPTY ) {
                if ( ball->vel.y > 0 )
                    target->y = start * BRICK_HEIGHT - ball_rad - 1;
                else
                    target->y = (start+1) * BRICK_HEIGHT + ball_rad;
                compute_vel = 1;
            }
            break;
    }
    if ( compute_vel ) {
        old_vx = ball->vel.x;
        ball->vel.x = target->x - (ball->cur.x + ball_rad);
        ball->vel.y = target->y - (ball->cur.y + ball_rad);
        ball_mask_vel( ball, old_vx, NO_ENTROPY );
    }
}
/*
====================================================================
Reflect ball at target but ignore target::side and reflect at 
corner instead. Does not update the ball.
====================================================================
*/
void ball_reflect_at_corner( Ball *ball, Target *target, int corner )
{
	Coord corner_center; /* center of corner circle */
	Coord ball_center = { ball->cur.x + ball_rad, ball->cur.y + ball_rad };
	Vector norm_vel = ball->vel;
	Coord pt, pt2; /* intersection points */
	/* norm velocity */
	vector_norm( &norm_vel );
	/* set up center of corner */
	switch ( corner ) {
		case CORNER_UPPER_LEFT:
			corner_center = vector_get( target->mx * BRICK_WIDTH, target->my * BRICK_HEIGHT );
			break;
		case CORNER_UPPER_RIGHT:
			corner_center = vector_get( target->mx * BRICK_WIDTH + BRICK_WIDTH - 1, target->my * BRICK_HEIGHT );
			break;
		case CORNER_LOWER_LEFT:
			corner_center = vector_get( target->mx * BRICK_WIDTH, target->my * BRICK_HEIGHT + BRICK_HEIGHT - 1);
			break;
		case CORNER_LOWER_RIGHT:
			corner_center = vector_get( target->mx * BRICK_WIDTH + BRICK_WIDTH - 1, target->my * BRICK_HEIGHT + BRICK_HEIGHT - 1);
			break;
	}
	/* intersect */
	circle_intersect( corner_center, ball_rad + 2, ball_center, norm_vel, &pt, &pt2 );
	/* use nearest point for reset and perp vector */
	if ( VEC_DIST( ball_center, pt ) < VEC_DIST( ball_center, pt2 ) ) {
		target->x = pt.x;
		target->y = pt.y;
	}
	else {
		target->x = pt2.x;
		target->y = pt2.y;
	}
    /* compute the spherical perp vector 
       (corner center - intersection point) */
    target->perp_vector = 
        vector_get( corner_center.x - target->x, 
                    corner_center.y - target->y );
    vector_norm( &target->perp_vector );
    /* this vector must operate within a 90° region depending on the corner.
       if it doesn't we have a side reflection unnoticed by the previous
       checks as we enclosed a corner. this is the only position to check
       this as the reset position is different when a corner is enclosed.
       doing this anywhere else would lead to errors. */
	switch ( corner ) {
		case CORNER_UPPER_LEFT:
            if ( target->perp_vector.x * target->perp_vector.y >= 0 ) {
                /* we needed the spherical perp to determine if it is 
                   really a corner however we might have set the
                   config option linear_corner */
                if ( config.linear_corner )
                    target->perp_vector = vector_get( REC_SQRT_2, REC_SQRT_2 );
                break;
            }
            if ( target->y < corner_center.y || target->x >= corner_center.x )
                target->perp_vector = vector_get( 0, 1 ); /* top */
            else
                target->perp_vector = vector_get( 1, 0 ); /* left */
            break;
		case CORNER_LOWER_RIGHT:
            if ( target->perp_vector.x * target->perp_vector.y >= 0 ) {
                if ( config.linear_corner )
                    target->perp_vector = vector_get( REC_SQRT_2, REC_SQRT_2 );
                break;
            }
            if ( target->y > corner_center.y || target->x <= corner_center.x )
                target->perp_vector = vector_get( 0, 1 ); /* bottom */
            else
                target->perp_vector = vector_get( 1, 0 ); /* right */
            break;
		case CORNER_UPPER_RIGHT:
            if ( target->perp_vector.x * target->perp_vector.y <= 0 ) {
                if ( config.linear_corner )
                    target->perp_vector = vector_get( REC_SQRT_2, -REC_SQRT_2 );
                break;
            }
            if ( target->y < corner_center.y || target->x <= corner_center.x )
                target->perp_vector = vector_get( 0, 1 ); /* top */
            else
                target->perp_vector = vector_get( 1, 0 ); /* right */
            break;
		case CORNER_LOWER_LEFT:
            if ( target->perp_vector.x * target->perp_vector.y <= 0 ) {
                if ( config.linear_corner )
                    target->perp_vector = vector_get( REC_SQRT_2, -REC_SQRT_2 );
                break;
            }
            if ( target->y > corner_center.y || target->x >= corner_center.x )
                target->perp_vector = vector_get( 0, 1 ); /* bottom */
            else
                target->perp_vector = vector_get( 1, 0 ); /* left */
			break;
	}
}
/*
====================================================================
Reflect ball at target ball:t and decide by ball::t::side wether
to use reflect_at_side or reflect_at_corner.
====================================================================
*/
void ball_reflect( Ball *ball )
{
	if ( !ball->target.exists ) return;
	if ( ball->target.side <= SIDE_LEFT ) 
		ball_reflect_at_side( ball, &ball->target );
	else
		ball_reflect_at_corner( ball, &ball->target, ball->target.side );
}

/*
====================================================================
Check if ball's tangents enclose a corner and update target's side.
====================================================================
*/
void ball_corner_check( Ball *ball, Target *target_left_tang, Target *target_right_tang, Target *target ) {
	/* balls moving ... */
	if ( ball->vel.y > 0 ) {
		if ( ball->vel.x < 0 ) {
			/* ... down left */
			if ( target == target_right_tang && target->side == SIDE_TOP   ) target->side = CORNER_UPPER_RIGHT;
			if ( target == target_left_tang  && target->side == SIDE_RIGHT ) target->side = CORNER_UPPER_RIGHT;
		}	
		else {
			/* ... down right */
			if ( target == target_left_tang  && target->side == SIDE_TOP   ) target->side = CORNER_UPPER_LEFT;
			if ( target == target_right_tang && target->side == SIDE_LEFT  ) target->side = CORNER_UPPER_LEFT;
		}
	}
	else {
		if ( ball->vel.x < 0 ) {
			/* ... up left */
			if ( target == target_right_tang && target->side == SIDE_RIGHT  ) target->side = CORNER_LOWER_RIGHT;
			if ( target == target_left_tang  && target->side == SIDE_BOTTOM ) target->side = CORNER_LOWER_RIGHT;
		}
		else {
			/* ... up right */
			if ( target == target_left_tang  && target->side == SIDE_LEFT   ) target->side = CORNER_LOWER_LEFT;
			if ( target == target_right_tang && target->side == SIDE_BOTTOM ) target->side = CORNER_LOWER_LEFT;
		}
	}
}

/*
====================================================================
Public
====================================================================
*/

/*
====================================================================
Load/delete ball graphics
====================================================================
*/
void ball_load()
{
    balls = list_create( LIST_AUTO_DELETE, LIST_NO_CALLBACK );
}
void ball_delete()
{
    if ( balls ) list_delete( balls ); balls = 0;
}
/*
====================================================================
Create ball at position
====================================================================
*/
Ball* ball_create( int x, int y )
{
    Ball *ball = calloc( 1, sizeof( Ball ) );
    ball->cur.x = x;
    ball->x = x;
    ball->cur.y = y;
    ball->y = y;
    ball->attached = 0;
    ball->idle_time = 0;
    ball->moving_back = 0;
    ball->return_allowed = 0;
    ball_clear_target(&ball->target);
    return ball;
}
/*
====================================================================
Set a special ball property like metal ball.
====================================================================
*/
void balls_set_type( int type )
{
    ball_metal = 0;
    ball_expl  = 0;
    ball_weak  = 0;
    switch ( type ) {
        case BALL_NORMAL:
            ball_pic_x_offset = 0;
            break;
        case BALL_METAL:
            ball_metal = 1;
            ball_pic_x_offset = ball_w;
            ball_metal_alpha = 0;
            if ( ball_metal_alpha_change < 0 ) 
                ball_metal_alpha_change = -ball_metal_alpha_change;
            break;
       case BALL_EXPL:
           ball_expl = 1;
           ball_pic_x_offset = ball_w + ball_w;
           break;
       case BALL_WEAK:
           ball_weak = 1;
           ball_pic_x_offset = ball_w + ball_w + ball_w;
           break;
	}		
}
/*
====================================================================
Set chaotic behaviour (random relfection)
====================================================================
*/
void balls_set_chaos( int chaos )
{
    ball_chaos = chaos;
}
/*
====================================================================
Clear ball list and attach one ball to paddle
====================================================================
*/
void balls_reset()
{
    int i;
    Ball *ball;
    list_clear( balls );
    for ( i = 0; i < paddle_count; i++ ) {
        if ( paddles[i]->type == PADDLE_BOTTOM )
            ball = ball_create((paddles[i]->w - ball_w) / 2, -ball_dia );
        else
            ball = ball_create((paddles[i]->w - ball_w) / 2, paddles[i]->h );
        ball->attached = 1;
        ball->paddle = paddles[i];
        ball->paddle->attached_ball_count = 1;
        list_add( balls, ball );
    }
    balls_set_type( BALL_NORMAL );
}
/*
====================================================================
Initate velocity values (for all balls one value )
====================================================================
*/
void balls_set_vel( float v_start, float v_change, float v_max )
{
    ball_v = v_start;
    ball_vm = v_max;
    ball_v_add = v_change;
}
/*
====================================================================
Show/hide all balls
====================================================================
*/
void balls_hide()
{
    ListEntry *entry = balls->head->next;
    Ball *ball;
    int bx, by, bw, bh;
    while ( entry != balls->tail ) {
        ball = (Ball*)entry->item;
        entry = entry->next;
        /* balls position; add paddle pos if attached */
        bx = ball->x; by = ball->y;
        bw = ball_w + shadow_size;
        bh = ball_h + shadow_size;
        if (ball->attached) {
            bx += ball->paddle->x;
            by += ball->paddle->y;
        }
        /* blit background */
        stk_surface_blit( offscreen, 
            bx, by, bw,bh,
            stk_display, bx, by );
        stk_display_store_drect();
    }
}
void balls_show_shadow()
{
    ListEntry *entry = balls->head->next;
    int bx, by;
    int mx, my;
    Ball *ball;
    if ( active[EX_DARKNESS] ) return;
    while ( entry != balls->tail ) {
        ball = entry->item;
        entry = entry->next;
        /* balls position; add paddle pos if attached */
        bx = ball->x;
        by = ball->y;
        if (ball->attached) {
            bx += ball->paddle->x;
            by += ball->paddle->y;
        }
        /* show ball -- no shadow if darkness -- no shadow if going back home */
        if ( !ball->moving_back ) {
            stk_surface_clip( stk_display, 0, 0, stk_display->w - BRICK_WIDTH, stk_display->h );
            stk_surface_alpha_blit( ball_shadow, 0, 0, ball_w, ball_h,
                stk_display, bx + shadow_size, by + shadow_size,
                (ball_metal)?(((int)ball_metal_alpha)>>1):SHADOW_ALPHA );
            stk_display_store_drect();
            /* redraw nearby bricks */
            stk_surface_clip( stk_display, bx + shadow_size, by + shadow_size, ball_w, ball_h );
            /* check the three outer ocrners of the shadow if there's a brick */
            mx = ( bx + shadow_size + ball_w ) / BRICK_WIDTH;
            my = ( by + shadow_size ) / BRICK_HEIGHT;
            if ( my < MAP_HEIGHT - 1 )
                if ( mx < MAP_WIDTH - 1 && bricks[mx][my].type != MAP_EMPTY )
                    brick_draw( stk_display, mx, my, 0 );
            mx = ( bx + shadow_size + ball_w ) / BRICK_WIDTH;
            my = ( by + shadow_size + ball_h ) / BRICK_HEIGHT;
            if ( my < MAP_HEIGHT - 1 )
                if ( mx < MAP_WIDTH - 1 && bricks[mx][my].type != MAP_EMPTY )
                    brick_draw( stk_display, mx, my, 0 );
            mx = ( bx + shadow_size ) / BRICK_WIDTH;
            my = ( by + shadow_size + ball_h ) / BRICK_HEIGHT;
            if ( my < MAP_HEIGHT - 1 )
                if ( mx < MAP_WIDTH - 1 && bricks[mx][my].type != MAP_EMPTY )
                    brick_draw( stk_display, mx, my, 0 );
            stk_surface_clip( stk_display, 0, 0, 0, 0 );
        }
    }
}
void balls_show()
{
    ListEntry *entry = balls->head->next;
    Ball *ball;
    int bx, by;
    while ( entry != balls->tail ) {
        ball = entry->item;
        entry = entry->next;
        /* balls position; add paddle pos if attached */
        bx = ball->x;
        by = ball->y;
        if (ball->attached) {
            bx += ball->paddle->x;
            by += ball->paddle->y;
        }
        if ( ball_metal ) 
            stk_surface_alpha_blit( ball_pic, ball_pic_x_offset, 0,
                ball_w, ball_h, stk_display, bx, by,
                ball_metal_alpha );
        else
            stk_surface_blit( ball_pic, ball_pic_x_offset, 0,
                ball_w, ball_h, stk_display, bx, by );
        stk_display_store_drect();
    }
}
void balls_alphashow( int alpha )
{
    ListEntry *entry = balls->head->next;
    Ball *b;
    int bx, by;
    while ( entry != balls->tail ) {
        b = entry->item;
        /* balls position; add paddle pos if attached */
        bx = b->x;
        by = b->y;
        if (b->attached) {
            bx += b->paddle->x;
            by += b->paddle->y;
        }
        /* show ball */
        stk_surface_alpha_blit( ball_pic, ball_pic_x_offset, 0,
            ball_w, ball_h, stk_display, bx, by, alpha );
        entry = entry->next;
    }
}
/*
====================================================================
Update balls and detach attached balls if fire was pressed.
Return True if at least one ball was lost and return the number
of lost balls in 'top' and 'bottom'.
====================================================================
*/
int balls_update( int ms, int *top, int *bottom )
{
    int i;
    ListEntry *entry = balls->head->next;
    Ball *ball;
    Vector old; /* old position of ball before update */
    *top = *bottom = 0;
	/* modify alpha when metal */
	if ( ball_metal ) {
		ball_metal_alpha += ball_metal_alpha_change * ms;
		if ( ball_metal_alpha >= 255 || ball_metal_alpha <= 0 ) {
			ball_metal_alpha_change = -ball_metal_alpha_change;
			if ( ball_metal_alpha < 0 ) ball_metal_alpha = 0;
			if ( ball_metal_alpha > 255 ) ball_metal_alpha = 255;
		}	
	}
    /* fire new balls */
    for ( i = 0; i < paddle_count; i++ )
        if ( paddles[i]->fire_new_ball ) {
            if ( paddles[i]->type == PADDLE_BOTTOM )
                ball = ball_create( paddles[i]->x + (paddles[i]->w - ball_w) / 2, 
                                    paddles[i]->y - ball_dia );
            else
                ball = ball_create( paddles[i]->x + (paddles[i]->w - ball_w) / 2, 
                                    paddles[i]->y + paddles[i]->h );
            ball->paddle = paddles[i];
            ball->vel.x = 1.0 * paddles[i]->fire_new_ball;
            if ( ball->paddle->type == PADDLE_TOP )
                ball->vel.y = 1.2;
            else
                ball->vel.y = -1.2;
            vector_set_length( &ball->vel, ball_v );
            ball->get_target = 1;
            paddles[i]->ammo--;
            paddles[i]->fire_new_ball = 0;
            list_add( balls, ball );
        }
    /* move balls */
    while ( entry != balls->tail ) {
        ball = entry->item;
        old.x = ball->cur.x;
        old.y = ball->cur.y;
        /* update ball when moving back */
        if ( ball->moving_back ) {
            /* update velocity */
            ball->vel.x = ( ball->paddle->x + ( ball->paddle->w >> 1 ) ) - ( ball->cur.x + ball_rad );
            ball->vel.y = ( ball->paddle->y - ball_rad + 2 ) - ( ball->cur.y + ball_rad );
            vector_set_length( &ball->vel, ball_vm );
            /* new position */
            ball->cur.x += ball->vel.x * ms;
            ball->cur.y += ball->vel.y * ms;
            ball->x = (int)ball->cur.x;
            ball->y = (int)ball->cur.y;
            /* check if paddle is reached and attach the ball */
            if ( ball->x + ball_rad >= ball->paddle->x && ball->x + ball_rad < ball->paddle->x + ball->paddle->w )
                if ( ball->y + ball_dia >= ball->paddle->y && ball->y + ball_dia < ball->paddle->y + ball->paddle->h ) {
                    ball->cur.x = ball->paddle->x + ( ball->paddle->w >> 1 ) - ball_rad;
                    if ( ball->paddle->type == PADDLE_TOP )
                        ball->cur.y = ball->paddle->y + ball->paddle->h;
                    else
                        ball->cur.y = ball->paddle->y - ball_dia;
                    ball->x = (int)ball->cur.x;
                    ball->y = (int)ball->cur.y;
                    ball_attach( ball, ball->paddle );
                }
        }
        /* update ball if not attached and not moving back */
        if ( !ball->attached && !ball->moving_back ) {
            /* increase idle time -- paddle and brick_check will reset this value */
            if ( !ball->return_allowed )
                ball->idle_time += ms;
            /* check if reflected by any paddle */
            for ( i = 0; i < paddle_count; i++ )
                if ( ball_paddle_contact_possible( ball, paddles[i], old ) )
                    ball_check_paddle_reflection( ball, paddles[i] );
            /* update target? */
            if ( ball->get_target ) {
                ball_get_target( ball );
                ball->get_target = 0;
            }
            /* new position */
            ball->cur.x += ball->vel.x * ms;
            ball->cur.y += ball->vel.y * ms;
            ball->x = (int)ball->cur.x;
            ball->y = (int)ball->cur.y;
            /* or by brick */
            /* quick hack to handle the case when the ball was just attached but touches the wall and the 
               slimy paddle in the same instant. - Patrick Hohmeyer 19.12.01 */
            if ( ball->target.exists && !ball->attached ) {
                ball->target.cur_tm += ms;
                ball_check_brick_reflection( ball );
            }
            /* check if idle time is above limit and the ball has a target because if there
             * is no target the ball moves out of the window and should not go back to the 
             * paddle as it's moving into this direction by itself
             */
            if ( ball->idle_time >= BALLS_IDLE_LIMIT && !ball->return_allowed && ball->target.exists ) {
                /* okay send this ball back home or allow to do so by click */
                if ( !config.return_on_click ) {
                    ball->idle_time = 0;
                    ball->moving_back = 1;
                    ball->target.exists = 0; /* no target */
                }
                else {
                    ball->idle_time = 0;
                    ball->return_allowed = 1;
                }
            }
        }
        entry = entry->next;
        /* delete ball if outside of window */
        if (!ball->attached && (ball->x >= stk_display->w || ball->x + ball_dia < 0 || ball->y >= stk_display->h || ball->y + ball_dia < 0 )) {
            if ( ball->y + ball_rad <= stk_display->h >> 1 )
                (*top)++;
            else
                (*bottom)++;
            list_delete_entry( balls, entry->prev );
        }
    }
    /* any balls lost?*/
    return (*top || *bottom);
}
/*
====================================================================
All balls with target mx,my will have there 'get_target' flag
set True so they compute a new target next time balls_update()
is called. If 'mx' is -1 all balls will set their flag.
====================================================================
*/
void balls_check_targets(int mx, int my) {
    Ball *ball;
    int reset = 0;
    list_reset( balls );
    while ( ( ball = list_next( balls ) ) ) 
        if ( !ball->attached && !ball->moving_back )
        if ( mx == -1 || ( ball->target.mx == mx && ball->target.my == my ) ) {
            /*
             * As we don't have a constant velocity but assume one it is possible that
             * the ball is within a wall when this function is called because it actually
             * passed it's reset position without time expiration because of the velocity
             * change. So we have to check here if it is already behind this position and if so
             * simply reset here. This doesn't hurt as this would happen before reflection, too.
             */
            if ( ball->target.exists ) {
                if ( ball->vel.y > 0 ) {
                    if ( ball->cur.y > ball->target.y ) 
                        reset = 1;
                }
                else {
                    if ( ball->cur.y < ball->target.y ) 
                        reset = 1;
                }
                if ( ball->vel.x > 0 ) {
                    if ( ball->cur.x > ball->target.x ) 
                        reset = 1;
                }
                else {
                    if ( ball->cur.x < ball->target.x ) 
                        reset = 1;
                }
                if ( reset ) {
                    ball->cur.x = ball->target.x;
                    ball->cur.y = ball->target.y;
                    ball->x = (int)ball->cur.x;
                    ball->y = (int)ball->cur.y;
                }
            }
            ball->get_target = 1;
        }
}
/*
====================================================================
Adjust velocity of ball to spare out any illegal values.
Add a little entropy to the vector if 'entropy' is True.
====================================================================
*/
void ball_mask_vel(Ball *b, float old_vx, int entropy )
{
    float m, entropy_level = 0;

    /* b->vel.x == 0 would cause seg faults */
    if (b->vel.x == 0) {
        if (old_vx < 0)
            b->vel.x = 0.01;
        else
            b->vel.x = -0.01;
    }

    if ( entropy == ADD_ENTROPY )
        entropy_level = (float)((rand() % 81)+40)/1000.0;
    
    m = b->vel.y / b->vel.x;
    if (fabs(m) < ball_vhmask) {
        /* mask angles from 70 to 110 and -110 to -70 */
        if (b->vel.y < 0)
            b->vel.y = -fabs(ball_vhmask * b->vel.x);
        else
            b->vel.y = fabs(ball_vhmask * b->vel.x);
        if ( entropy == ADD_ENTROPY )
            b->vel.x -= b->vel.x * entropy_level;
    }
    else
    if (fabs(m) > ball_vvmask) {
        /* mask angles from -10 to 10 and 170 to 190 */
        if (b->vel.x < 0)
            b->vel.x = -fabs(b->vel.y / ball_vvmask);
        else
            b->vel.x = fabs(b->vel.y / ball_vvmask);
        if ( entropy == ADD_ENTROPY )
            b->vel.x += b->vel.x * entropy_level;
    }
    else 
    if ( entropy == ADD_ENTROPY ) {
        if ( rand() % 2 )
            entropy_level = -entropy_level;
        b->vel.x += b->vel.x * entropy_level;
    }
    
    /* avoid 45° angles */
    if (b->vel.x == b->vel.y)
        b->vel.x *= 0.98;
    
    /* adjust speed */
    vector_set_length( &b->vel, ball_v );
}
/*
====================================================================
Get target for a ball.
====================================================================
*/
enum { TANG_LEFT = 0, TANG_RIGHT };
enum { DIR_UP = 0, DIR_DOWN, DIR_LEFT, DIR_RIGHT };
void ball_get_target( Ball *ball )
{
    int    cur_tang;
    float  mono; /* monotony */
    Coord  tang_pts[2]; /* tangential points */
    Line   tang; /* current tangent */
    Coord  center = { 
        ball->cur.x + ball_rad, 
        ball->cur.y + ball_rad }; /* ball center */
    int    start, end, dir, line_pos, change; /* used to intersect the brick grid */
    Line   cur_line; /* dito */
    Coord  pt; /* auxiliary point. used for this 'n' that */
    Target targets[2]; /* targets hit by the tangents: nearest is the actual target */
    Target hori_target[2], vert_target[2]; /* used to get target of tangent */
    float  dist; /* distance between two points */
    Vector norm_vel; /* normed ball velocity */
#ifdef WITH_BUG_REPORT
	int	    corner = -1; /* id of corner used for corner reflection */
	char	tang_target_chosen_str[2][128]; /* either hori or vert target chosen */
	char	side_str[128];
    Coord   test_pts[2];
#endif
	Target	*prim, *sec; /* primary, secondary target */
	int     maybe_corner;
        
#ifdef WITH_BUG_REPORT
	side_str[0] = 0;
#endif
	
    /* remotely handled balls don't need a target */
    if ( game_type == GT_NETWORK && !game_host )
        return;
    
    /* balls moving back to paddle must not be reflected */
    if ( ball->moving_back ) return;
	/* attached balls MUST NOT be reflected!!!! */
	if ( ball->attached ) return;
	/* balls already out of the screen though still visible don't need new reflection, too */
	if ( ball->cur.y + ball_dia >= stk_display->h - 1 ) return;
	
    /* clear ball targets */
    ball_clear_target( &ball->target );
    ball_clear_target( &targets[TANG_LEFT] );
    ball_clear_target( &targets[TANG_RIGHT] );
    /* monotony */
    mono = ball->vel.y / ball->vel.x;
    /* normed velocity */
    norm_vel = ball->vel; vector_norm( &norm_vel );
    /* tangential points */
    ball_get_tangents( ball, &tang_pts[TANG_LEFT], &tang_pts[TANG_RIGHT] );
    /* get all map bricks the tangents intersect and check target */
    for ( cur_tang = 0; cur_tang < 2; cur_tang++ ) {
        /* clear targets */
        ball_clear_target( &hori_target[cur_tang] );
        ball_clear_target( &vert_target[cur_tang] );
        /* current tangent */
        line_set( &tang, tang_pts[cur_tang].x, tang_pts[cur_tang].y, mono );
        /* intersect horizontal lines */
        /* get direction */
        dir = DIR_DOWN;
        if ( ball->vel.y < 0 ) dir = DIR_UP;
        /* get starting line */
        start = ((int)( tang_pts[cur_tang].y / BRICK_HEIGHT )) * BRICK_HEIGHT;
        /* get end line */
        if ( dir == DIR_UP )
            end = 0;
        else
            end = ( MAP_HEIGHT - 1 ) * BRICK_HEIGHT;
        /* adjust lines if ball moves up */
        if ( dir == DIR_UP ) {
            start += BRICK_HEIGHT - 1;
            end += BRICK_HEIGHT - 1;
        }
        /* get position change */
        change = BRICK_HEIGHT;
        if ( dir == DIR_UP ) change = -change;
        /* we're at this brick so we can't reflect here */
        start += change;
        /* intersect */
        line_pos = start;
        /*  end specifies the last line to be checked to we have to add
            another line to state the break condition.
            this last line is not checked */
        end += change;
        while ( line_pos != end ) {
            line_set_hori( &cur_line, line_pos );
            if ( line_intersect( &cur_line, &tang, &pt ) && ( pt.x >= 0 && pt.x < stk_display->w ) )
                if ( bricks[(int)pt.x / BRICK_WIDTH][(int)pt.y / BRICK_HEIGHT].type != MAP_EMPTY ) {
                    /* we got our horizontal target */
                    hori_target[cur_tang].exists = 1;
                    hori_target[cur_tang].x = pt.x;
                    hori_target[cur_tang].y = pt.y;
                    hori_target[cur_tang].mx = (int)pt.x / BRICK_WIDTH;
                    hori_target[cur_tang].my = (int)pt.y / BRICK_HEIGHT;
                    if ( ball->vel.y < 0 )
                        hori_target[cur_tang].side = SIDE_BOTTOM;
                    else
                        hori_target[cur_tang].side = SIDE_TOP;
                    break; /* we got our target for this tangent */
                }
            line_pos += change;
        }
        /* intersect vertical lines */
        /* get direction */
        dir = DIR_RIGHT;
        if ( ball->vel.x < 0 ) dir = DIR_LEFT;
        /* get starting line */
        start = ((int)( tang_pts[cur_tang].x / BRICK_WIDTH )) * BRICK_WIDTH;
        /* get end line */
        if ( dir == DIR_LEFT )
            end = 0;
        else
            end = ( MAP_WIDTH - 1 ) * BRICK_WIDTH;
        /* adjust lines if ball moves up */
        if ( dir == DIR_LEFT ) {
            start += BRICK_WIDTH - 1;
            end += BRICK_WIDTH - 1;
        }
        /* get position change */
        change = BRICK_WIDTH;
        if ( dir == DIR_LEFT ) change = -change;
        /* we're at this brick so we can't reflect here */
        start += change;
        /* intersect */
        line_pos = start;
        /*  end specifies the last line to be checked too we have to add
            another line to state the break condition.
            this last line is not checked */
        end += change;
        while ( line_pos != end ) {
            line_set_vert( &cur_line, line_pos );
            if ( line_intersect( &cur_line, &tang, &pt ) && ( pt.y >= 0 && pt.y < stk_display->h ) )
                if ( bricks[(int)pt.x / BRICK_WIDTH][(int)pt.y / BRICK_HEIGHT].type != MAP_EMPTY ) {
                    /* we got our vertical target */
                    vert_target[cur_tang].exists = 1;
                    vert_target[cur_tang].x = pt.x;
                    vert_target[cur_tang].y = pt.y;
                    vert_target[cur_tang].mx = (int)pt.x / BRICK_WIDTH;
                    vert_target[cur_tang].my = (int)pt.y / BRICK_HEIGHT;
                    if ( ball->vel.x < 0 )
                        vert_target[cur_tang].side = SIDE_RIGHT;
                    else
                        vert_target[cur_tang].side = SIDE_LEFT;
                    break; /* we got our target for this tangent */
                }
            line_pos += change;
        }
        /* get closest target */
        if ( !hori_target[cur_tang].exists ) {
            targets[cur_tang] = vert_target[cur_tang];
#ifdef WITH_BUG_REPORT
			if ( !vert_target[cur_tang].exists )
				sprintf( tang_target_chosen_str[cur_tang], "No target chosen." );
			else
				sprintf( tang_target_chosen_str[cur_tang], "Vertical target chosen." );
#endif
		}	
        else
            if ( !vert_target[cur_tang].exists ) {
                targets[cur_tang] = hori_target[cur_tang];
#ifdef WITH_BUG_REPORT
				sprintf( tang_target_chosen_str[cur_tang], "Horizontal target chosen." );
#endif
			}
            else {
				/* check the relation and choose the correct target */
				/* if vertical and hori hit the same brick we have hit the corner */
				if ( hori_target[cur_tang].mx == vert_target[cur_tang].mx && hori_target[cur_tang].my == vert_target[cur_tang].my ) {
					/* congrats! we hit the exact corner pixel! now we have to decide by corner and 
					   tangent which target to use */
					if ( cur_tang == TANG_LEFT ) {
						/* left tangent */
						if ( ball->vel.y > 0 ) { 
							if ( ball->vel.x > 0 ) /* upper, right */
								targets[cur_tang] = vert_target[cur_tang];
							else
								targets[cur_tang] = hori_target[cur_tang];
						}
						else {
							if ( ball->vel.x > 0 ) /* lower, right */
								targets[cur_tang] = hori_target[cur_tang];
							else
								targets[cur_tang] = vert_target[cur_tang];
						}
					}
					else {
						/* right tangent */
						if ( ball->vel.y > 0 ) { 
							if ( ball->vel.x > 0 ) /* upper, right */
								targets[cur_tang] = hori_target[cur_tang];
							else
								targets[cur_tang] = vert_target[cur_tang];
						}
						else {
							if ( ball->vel.x > 0 ) /* lower, right */
								targets[cur_tang] = vert_target[cur_tang];
							else
								targets[cur_tang] = hori_target[cur_tang];
						}
					}
#ifdef WITH_BUG_REPORT
					if ( targets[cur_tang].x == hori_target[cur_tang].x && targets[cur_tang].y == hori_target[cur_tang].y )
						sprintf( tang_target_chosen_str[cur_tang], "(TRICKY) Horizontal target chosen." );
					else
						sprintf( tang_target_chosen_str[cur_tang], "(TRICKY) Vertical target chosen." );
#endif
				}
				else {
					if ( VEC_DIST( tang_pts[cur_tang], vector_get( hori_target[cur_tang].x, hori_target[cur_tang].y ) ) < VEC_DIST( tang_pts[cur_tang], vector_get( vert_target[cur_tang].x, vert_target[cur_tang].y ) ) ) {
                    	targets[cur_tang] = hori_target[cur_tang];
#ifdef WITH_BUG_REPORT					
						sprintf( tang_target_chosen_str[cur_tang], "Horizontal target chosen." );
#endif					
					}
                	else {
                    	targets[cur_tang] = vert_target[cur_tang];
#ifdef WITH_BUG_REPORT					
						sprintf( tang_target_chosen_str[cur_tang], "Vertical target chosen." );
#endif					
					}	
				}	
            }
    } /* now we have the two targets hit by the tangents */
	/* whatever's up the nearest brick is hit */
    if ( targets[TANG_LEFT].exists || targets[TANG_RIGHT].exists ) {
		prim = sec = 0;
        if ( !targets[TANG_LEFT].exists || !targets[TANG_RIGHT].exists ) {
			if ( targets[TANG_LEFT].exists )
				prim = &targets[TANG_LEFT];
			else 
				prim = &targets[TANG_RIGHT];
		}	
		else {
			if ( VEC_DIST( center, vector_get( targets[TANG_RIGHT].x, targets[TANG_RIGHT].y ) ) < VEC_DIST( center, vector_get( targets[TANG_LEFT].x, targets[TANG_LEFT].y ) ) )  {
				prim = &targets[TANG_RIGHT];
				sec  = &targets[TANG_LEFT];
			}
			else {
				prim = &targets[TANG_LEFT];
				sec  = &targets[TANG_RIGHT];
			}
		}	
		/* however, the primary target maybe be blocked by another brick or may be a corner */
		/* check if side of prim  target isn't blocked by a brick */
		switch ( prim->side ) {
			case SIDE_TOP: 
				if ( bricks[prim->mx][prim->my - 1].type != MAP_EMPTY ) {
					if ( ball->vel.x > 0 )
						prim->side = SIDE_LEFT;
					else
						prim->side = SIDE_RIGHT;
#ifdef WITH_BUG_REPORT
					sprintf( side_str, "Had to change side as TOP wasn't appropiate!" );
#endif							
				}
				break;
			case SIDE_BOTTOM: 
				if ( bricks[prim->mx][prim->my + 1].type != MAP_EMPTY ) {
					if ( ball->vel.x > 0 )
						prim->side = SIDE_LEFT;
					else
						prim->side = SIDE_RIGHT;
#ifdef WITH_BUG_REPORT
					sprintf( side_str, "Had to change side as BOTTOM wasn't appropiate!" );
#endif							
				}
				break;
			case SIDE_LEFT: 
				if ( bricks[prim->mx - 1][prim->my].type != MAP_EMPTY ) {
					if ( ball->vel.y > 0 )
						prim->side = SIDE_TOP;
					else
						prim->side = SIDE_BOTTOM;
#ifdef WITH_BUG_REPORT
					sprintf( side_str, "Had to change side as LEFT wasn't appropiate!" );
#endif							
				}
				break;
			case SIDE_RIGHT: 
				if ( bricks[prim->mx + 1][prim->my].type != MAP_EMPTY ) {
					if ( ball->vel.y > 0 )
						prim->side = SIDE_TOP;
					else
						prim->side = SIDE_BOTTOM;
#ifdef WITH_BUG_REPORT
					sprintf( side_str, "Had to change side as RIGHT wasn't appropiate!" );
#endif							
				}
				break;
		}
		/* now it still may be a corner */
        if ( sec == 0 || prim->mx != sec->mx || prim->my != sec->my || prim->side != sec->side ) {
            maybe_corner = 1;
            if ( ball->vel.y > 0 ) {
                if ( ball->vel.x > 0 ) {
                    /* upper left corner */
                    if ( bricks[prim->mx][prim->my - 1].type != MAP_EMPTY ) maybe_corner = 0;
                    if ( bricks[prim->mx - 1][prim->my].type != MAP_EMPTY ) maybe_corner = 0;
                }
                else {
                    /* upper right corner */
                    if ( bricks[prim->mx][prim->my - 1].type != MAP_EMPTY ) maybe_corner = 0;
                    if ( bricks[prim->mx + 1][prim->my].type != MAP_EMPTY ) maybe_corner = 0;
                }
            }
            else {
                if ( ball->vel.x > 0 ) {
                    /* lower left corner */
                    if ( bricks[prim->mx][prim->my + 1].type != MAP_EMPTY ) maybe_corner = 0;
                    if ( bricks[prim->mx - 1][prim->my].type != MAP_EMPTY ) maybe_corner = 0;
                }
                else {
                    /* lower right corner */
                    if ( bricks[prim->mx][prim->my + 1].type != MAP_EMPTY ) maybe_corner = 0;
                    if ( bricks[prim->mx + 1][prim->my].type != MAP_EMPTY ) maybe_corner = 0;
                }
            }
            if ( maybe_corner )
                ball_corner_check( ball, &targets[TANG_LEFT], &targets[TANG_RIGHT], prim );
        }
		/* we updated primary's side info correctly and may reflect now */
		ball->target = *prim;
		ball_reflect( ball );
        /* we got the reset position and the perpvector so finalize target */
        /* compute time: assume constant velocity: velocity change must not be too high! */
        dist = sqrt( SQUARE(center.x - ball->target.x) + SQUARE(center.y - ball->target.y) );
        ball->target.time = (int)floor(dist / ball_v);
        /* target's reset position is center position right now but
           we need the upper left corner of the ball */
        ball->target.x -= ball_rad; ball->target.y -= ball_rad;
        /* some error information */
#ifdef WITH_BUG_REPORT
        pt.x = ball->cur.x; pt.y = ball->cur.y;
        ball->cur.x = ball->target.x; ball->cur.y = ball->target.y;
        ball_get_tangents( ball, &test_pts[TANG_LEFT], &test_pts[TANG_RIGHT] );
        ball->cur.x = pt.x; ball->cur.y = pt.y;
		if ( bricks[(int)test_pts[0].x/BRICK_WIDTH][(int)test_pts[0].y/BRICK_HEIGHT].type != MAP_EMPTY ||
             bricks[(int)test_pts[1].x/BRICK_WIDTH][(int)test_pts[1].y/BRICK_HEIGHT].type != MAP_EMPTY ) { 
			printf( "*****\n" );
            printf( "Test Failed: %4.2f,%4.2f (%i,%i):\n",
                ball->target.x+ball_rad, ball->target.y+ball_rad,
                (int)(ball->target.x+ball_rad)/BRICK_WIDTH, 
                (int)(ball->target.y+ball_rad)/BRICK_HEIGHT );
            printf( "Left Tangent %4.2f,%4.2f (%i,%i) or Right Tangent %4.2f,%4.2f (%i,%i)\n",
                test_pts[0].x,test_pts[0].y,
                (int)test_pts[0].x/BRICK_WIDTH,(int)test_pts[0].y/BRICK_HEIGHT,
                test_pts[1].x,test_pts[1].y,
                (int)test_pts[1].x/BRICK_WIDTH,(int)test_pts[1].y/BRICK_HEIGHT);
			printf( "*****\n" );
            printf( "2.4: Balls: %i\n", balls->count );
        	if ( targets[TANG_LEFT].exists ) {
                printf( "Left Tangential Point: %4.2f,%4.2f\n",
                    tang_pts[TANG_LEFT].x, tang_pts[TANG_LEFT].y );
                printf( "Left Tangent: Horizontal: %i,%i, %i (%4.2f,%4.2f)\n", 
						hori_target[TANG_LEFT].mx, hori_target[TANG_LEFT].my, hori_target[TANG_LEFT].side, 
						hori_target[TANG_LEFT].x, hori_target[TANG_LEFT].y );
                printf( "Left Tangent:   Vertical: %i,%i, %i (%4.2f,%4.2f)\n", 
						vert_target[TANG_LEFT].mx, vert_target[TANG_LEFT].my, vert_target[TANG_LEFT].side, 
						vert_target[TANG_LEFT].x, vert_target[TANG_LEFT].y );
				printf( "%s\n", tang_target_chosen_str[TANG_LEFT] );
			}
        	if ( targets[TANG_RIGHT].exists ) {
                printf( "Right Tangential Point: %4.2f,%4.2f\n",
                    tang_pts[TANG_RIGHT].x, tang_pts[TANG_RIGHT].y );
                printf( "Right Tangent: Horizontal: %i,%i, %i (%4.2f,%4.2f)\n",
						hori_target[TANG_RIGHT].mx, hori_target[TANG_RIGHT].my, hori_target[TANG_RIGHT].side, 
						hori_target[TANG_RIGHT].x, hori_target[TANG_RIGHT].y );
                printf( "Right Tangent:   Vertical: %i,%i, %i (%4.2f,%4.2f)\n",
						vert_target[TANG_RIGHT].mx, vert_target[TANG_RIGHT].my, vert_target[TANG_RIGHT].side, 
						vert_target[TANG_RIGHT].x, vert_target[TANG_RIGHT].y );
				printf( "%s\n", tang_target_chosen_str[TANG_RIGHT] );
			}
			if ( side_str[0] != 0 ) printf( "BTW: %s\n", side_str );
			printf( "-----\n" );
            ball_print_target_info( ball );
			if ( corner != -1 ) printf( "%s\n", circle_msg );
        	printf("*****\n");
			printf( "\nYou encountered a bug! Please send this output to kulkanie@gmx.net. Thanks!\n" );
            exit(1);
            /* move ball back to paddle as the current target is nonsense */
/*            ball->target.exists = 0;
            ball->idle_time = 0;
			ball->moving_back = 1;
            ball->return_allowed = 0;*/
		}	
#endif
    }
}
/*
====================================================================
Increase velocity acording to vel_change
====================================================================
*/
void balls_inc_vel( int ms )
{
    ListEntry *entry;
    if (ball_v < ball_vm && !(balls->count == 1 && ((Ball*)(balls->head->next->item))->attached)) {
        ball_v += ball_v_add * ms;
        entry = balls->head->next;
        while ( entry != balls->tail ) {
            vector_set_length( &((Ball*)entry->item)->vel, ball_v );
            entry = entry->next;
        }
    }
}
/*
====================================================================
Return all balls that have ball->return_allowed True.
====================================================================
*/
void balls_return( Paddle *paddle )
{
	Ball *ball;
    if ( game_type == GT_NETWORK && !game_host )
        return; /* only the host may initiate this */
    list_reset( balls );
    while ( ( ball = list_next( balls ) ) != 0 ) 
    if ( ball->return_allowed && ball->paddle == paddle ) {
        ball->moving_back = 1;
        ball->target.exists = 0;
        ball->return_allowed = 0;
    }
}


/***************************************************************************
                          comm.c  -  description
                             -------------------
    begin                : Fri Aug 2 2002
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
#include "event.h"
#include "config.h"
#include "difficulty.h"
#include "shrapnells.h"
#include "levels.h"
#include "player.h"
#include "display.h"
#include "paddle.h"
#include "bricks.h"
#include "shots.h"
#include "balls.h"
#include "extras.h"
#include "manager.h"
#include "comm.h"
#include "client_data.h"
#include "../gui/gui.h"

/*
====================================================================
Externals
====================================================================
*/
extern int stk_quit_request;
extern SDL_Surface *stk_display;
extern SDL_Surface *offscreen, *cr_bkgnd;
extern Paddle *l_paddle, *r_paddle;
extern Paddle *paddles[2];
#ifdef NETWORK_ENABLED
extern Net_Socket *game_peer;
#endif
extern int game_host;
extern int level_type;
extern char client_error[128];
extern List *balls;
extern List *shots;
extern List *levels;
extern int active[EX_NUMBER];
extern int grow;
extern int grow_mask[MAP_WIDTH][MAP_HEIGHT];
extern Vector impact_vectors[180];
extern Brick bricks[MAP_WIDTH][MAP_HEIGHT];
#ifdef AUDIO_ENABLED
extern StkSound *wav_reflect;
extern StkSound *wav_attach;
extern StkSound *wav_click;
extern StkSound *wav_shot;
#endif
extern char client_name[16];
extern Display *display_score[2]; 
extern char chatter[CHAT_LINE_COUNT][CHAT_LINE_WIDTH];
extern GuiWidget *gui_key_widget;
extern GuiWidget *gui_clicked_widget;
extern GuiWidget *gui_focused_widget;
extern GuiWidget *dlg_pauseroom;
extern int game_match_won;

Net_Brick net_bricks[EDIT_WIDTH][EDIT_HEIGHT]; /* used to save brick 
                                                  modifications for net talk -
                                                  host only */
Net_Paddle net_paddle; /* local paddle information for net talk - host&client */
int net_ball_reflected, net_ball_attached; /* used to let client play sound */
int net_shot_fired; /* used to play client sound */
/* net_bonuses is added to struct Paddle as host need it for both paddles */
Delay sync_timeout; /* if this timeout expired a pair of messages is send
                       without waiting for receiving opposite messages:
                       this either initiates or recovers connection */
enum { COMM_RAW_BUFFER_LIMIT = 256 * 3 };
Uint8 comm_raw_buffer[COMM_RAW_BUFFER_LIMIT]; /* enough to handle up to 20 
                                                 free/attached balls and loads of bricks */
int comm_raw_length = 0;
                 
/*
====================================================================
Locals
====================================================================
*/

#define WITH_COMM_STATS

#ifdef WITH_COMM_STATS
/* statistics */
typedef struct {
    int last_call; /* last time pkg_count was updated */
    int start_time; /* time in milliseconds when first call */
    int pack_count; /* number of packages transferred */
    int byte_count; /* number of bytes transferred */
    int low_lat, high_lat; /* lowest,highest latency */
    int kickoffs; /* number of asynchron connection restarts */
} Comm_Stats;
Comm_Stats stats_in, stats_out;
#endif

/*
====================================================================
Check bits
====================================================================
*/
#define SETBIT( data, bit ) data |= (1L << bit )
#define GETBIT( data, bit ) ((data >> bit) & 1)

#ifdef NETWORK_ENABLED
/*
====================================================================
Built paddle information (16bit) from net_paddle struct.
 0 - 9: x position
 10: left
 11: or right fire (only one flag is set)
 12: weapon fire
 13: invisibility
 14: host sets this bit if any paddle has fired a shot
     (used to play client sound)
====================================================================
*/
static Uint16 comm_pack_local_paddle()
{
    Uint16 data;
    /* 0 - 9: paddle position */
    data = (Uint16)net_paddle.x;
    /* 10: left fire */
    if ( net_paddle.left_fire )
        SETBIT( data, 10 );
    /* 11: right fire */
    if ( net_paddle.right_fire )
        SETBIT( data, 11 );
    /* 12: weapon fire */
    if ( l_paddle->weapon_fire )
        SETBIT( data, 12 );
    /* 13: invisibility */
    if ( net_paddle.invis )
        SETBIT( data, 13 );
    /* 14: shot fired in host game */
    if ( game_host && net_shot_fired ) {
        SETBIT( data, 14 );
        net_shot_fired = 0;
    }
    /* reset the fire flags */
    net_paddle.left_fire = net_paddle.right_fire = 0;
    return data;
}

/*
====================================================================
Parse the packed paddle info and update the remote paddle 
with it.
====================================================================
*/
static void comm_update_remote_paddle( Uint16 data )
{
    int dir = 0;
    /* position */
    r_paddle->x = data & 1023;
    r_paddle->cur_x = r_paddle->x;
    if ( game_host ) {
        /* attached/new balls -
           as the host manages the balls the client does not
           need to detach/fire any. */
        if ( GETBIT( data, 10 ) )
            dir = -1;
        else
            if ( GETBIT( data, 11 ) )
                dir = 1;
        if ( dir ) {
            if ( !paddle_detach_balls( r_paddle, dir ) ) {
                /* bring a new ball to game */
                if ( r_paddle->ammo > 0 )
                    r_paddle->fire_new_ball = dir;
            }
        }
        /* handle weapon */
        if ( !weapon_firing( r_paddle ) && GETBIT( data, 12 ) )
            weapon_start_fire( r_paddle );
        else
            if ( weapon_firing( r_paddle ) && !GETBIT( data, 12 ) )
                weapon_stop_fire( r_paddle );
    }
    /* invisibility */
    if ( GETBIT( data, 13 ) )
        r_paddle->invis_delay = 0;
    else
        r_paddle->invis_delay = 2000;
    /* fire sound */
#ifdef AUDIO_ENABLED    
    if ( !game_host && GETBIT( data, 14 ) )
        stk_sound_play( wav_shot );
#endif
}

/*
====================================================================
Write ball data to comm_raw_buffer.
  2 Byte: 
    0-13: moving balls count
    14:   play reflect sound
    15:   play attach sound
  3 Byte each: moving ball positions 
    0-7:  lower x
    8-15: lower y
    16:   8th bit of x
    17:   9th bit of x
    18:   8th bit of y
  2 Byte each: attached ball positions 
    0-7:  x + 20
    8-14: y + 20
    15:   paddle (top or bottom)
====================================================================
*/
void comm_pack_balls() {
    Uint8 byte;
    Uint16 count = 0;
    Ball *ball;
    comm_raw_length = 2; /* keep the first two byte free for the counter */
    /* moving balls */
    list_reset( balls );
    while ( ( ball = list_next( balls ) ) )
        if ( !ball->attached ) {
            byte = ball->x & 255; /* lower 8 bits of x */
            comm_raw_buffer[comm_raw_length++] = byte;
            byte = ball->y & 255; /* lower 8 bits of y */
            comm_raw_buffer[comm_raw_length++] = byte;
            /* higher bits */
            byte = 0;
            if ( GETBIT( ball->x, 8 ) )
                SETBIT( byte, 0 );
            if ( GETBIT( ball->x, 9 ) )
                SETBIT( byte, 1 );
            if ( GETBIT( ball->y, 8 ) )
                SETBIT( byte, 2 );
            comm_raw_buffer[comm_raw_length++] = byte;
            /* counter */
            count++;
        }
    comm_raw_buffer[0] = (Uint8)(count & 255);
    comm_raw_buffer[1] = (Uint8)(count >> 8);
    /* sounds */
    if ( net_ball_reflected ) {
        net_ball_reflected = 0;
        SETBIT( comm_raw_buffer[1], 6 );
    }
    if ( net_ball_attached ) {
        net_ball_attached = 0;
        SETBIT( comm_raw_buffer[1], 7 );
    }
    /* attached balls */
    list_reset( balls );
    while ( ( ball = list_next( balls ) ) )
        if ( ball->attached ) {
            byte = ball->x + 20;
            comm_raw_buffer[comm_raw_length++] = byte;
            byte = ball->y + 20;
            /* the highest bit of y is used to indicate
               the paddle: 0 bottom, 1 top */
            if ( ball->paddle->type == PADDLE_TOP )
                SETBIT( byte, 7 );
            comm_raw_buffer[comm_raw_length++] = byte;
        }
}
/*
====================================================================
Unpack the ball data and apply it to the client. All moving balls
are only drawn statically but not moved. The attached balls are 
moved normally with the paddle. (and can be fired by the client but
will be replaced in the next message by host)
====================================================================
*/
void comm_unpack_balls()
{
    Ball *ball;
    Uint8 count = comm_raw_buffer[0] + ((comm_raw_buffer[1] & 63) << 8), i, pos = 2;
    Uint8 byte;
    list_clear( balls );
    /* get the moving balls */
    for ( i = 0; i < count; i++ ) {
        if ( ( ball = calloc( 1, sizeof( Ball ) ) ) ) {
            ball->x = comm_raw_buffer[pos++];
            ball->y = comm_raw_buffer[pos++];
            byte = comm_raw_buffer[pos++];
            if ( GETBIT( byte, 0 ) )
                ball->x += (1L << 8);
            if ( GETBIT( byte, 1 ) )
                ball->x += (1L << 9);
            if ( GETBIT( byte, 2 ) )
                ball->y += (1L << 8);
            list_add( balls, ball );
        }
    }
    /* get the attached balls */
    while ( pos < comm_raw_length ) {
        if ( ( ball = calloc( 1, sizeof( Ball ) ) ) ) {
            ball->x = comm_raw_buffer[pos++] - 20;
            byte = comm_raw_buffer[pos++];
            ball->y = ( byte & 127 ) - 20;
            ball->cur.x = ball->x; ball->cur.y = ball->y;
            ball->attached = 1;
            /* which paddle? */
            if ( GETBIT( byte, 7 ) )
                ball->paddle = paddles[1];
            else
                ball->paddle = paddles[0];
            list_add( balls, ball );
        }
    }
#ifdef AUDIO_ENABLED    
    /* play the sounds */
    if ( GETBIT( comm_raw_buffer[1], 6 ) )
        stk_sound_play( wav_reflect );
    if ( GETBIT( comm_raw_buffer[1], 7 ) )
        stk_sound_play( wav_attach );
#endif
}

/*
====================================================================
Pack shots
  3 Bytes each: fired shots
    0-7:  lower x
    8-15: lower y
    16:   8th bit of x
    17:   9th bit of x
    18:   8th bit of y
====================================================================
*/
static void comm_pack_shots()
{
    Uint8 byte;
    Shot *shot;
    comm_raw_length = 0;
    list_reset( shots );
    while ( ( shot = list_next( shots ) ) ) {
        byte = ((int)(shot->x)) & 255; /* lower 8 bits of x */
        comm_raw_buffer[comm_raw_length++] = byte;
        byte = ((int)(shot->y)) & 255; /* lower 8 bits of y */
        comm_raw_buffer[comm_raw_length++] = byte;
        /* higher bits */
        byte = 0;
        if ( GETBIT( ((int)(shot->x)), 8 ) )
            SETBIT( byte, 0 );
        if ( GETBIT( ((int)(shot->x)), 9 ) )
            SETBIT( byte, 1 );
        if ( GETBIT( ((int)(shot->y)), 8 ) )
            SETBIT( byte, 2 );
        comm_raw_buffer[comm_raw_length++] = byte;
    }
}
/*
====================================================================
Unpack shots and display them on the client.
====================================================================
*/
static void comm_unpack_shots()
{
    Uint8 byte;
    Shot *shot = 0;
    int pos = 0, i = comm_raw_length / 3;
    list_clear( shots );
    while ( i-- > 0 ) {
        if ( ( shot = calloc( 1, sizeof( Shot ) ) ) ) {
            shot->x = comm_raw_buffer[pos++];
            shot->y = comm_raw_buffer[pos++];
            byte = comm_raw_buffer[pos++];
            if ( GETBIT( byte, 0 ) )
                shot->x += (1L << 8);
            if ( GETBIT( byte, 1 ) )
                shot->x += (1L << 9);
            if ( GETBIT( byte, 2 ) )
                shot->y += (1L << 8);
            list_add( shots, shot );
        }
    }
}

/*
====================================================================
Pack bricks data into comm_raw_buffer. 
  3 Bytes each: modified bricks
    0-7:   id in the editable window
    8-9:   modification type 
      [00 hit, 01 destroy, 10 regenerate, 11 grow]
    10-11: number of hit/regen points or destroy type (according to
           the modification type)
      [00 normal, 01 energy, 10 shot, 11 expl]
    12:    responsible paddle used for shot animation 
           and extra direction (0 bottom, 1 top)
    13:    if true and brick has no extra release 1000P (goldshower)
    14-15: unused
    16-23: clockwise impact position (0 to 360°) for normal 
           animation
====================================================================
*/
static void comm_pack_bricks()
{
    int i, j;
    Net_Brick *brick;
    Uint8 temp;
    comm_raw_length = 0;
    for ( j = 0; j < EDIT_HEIGHT; j++)
        for ( i = 0; i < EDIT_WIDTH; i++ )
            if ( net_bricks[i][j].used ) {
                brick = &net_bricks[i][j];
                temp = 0;
                /* id */
                comm_raw_buffer[comm_raw_length++] = (Uint8)(j * EDIT_WIDTH + i);
                /* modification type */
                if ( brick->mod == BRICK_REMOVE || brick->mod == BRICK_GROW )
                    SETBIT( temp, 0 );
                if ( brick->mod == BRICK_HEAL || brick->mod == BRICK_GROW )
                    SETBIT( temp, 1 );
                /* points or destroy type */
                if ( brick->mod == BRICK_REMOVE ) {
                    if ( brick->dest_type == SHR_BY_ENERGY_BALL || brick->dest_type == SHR_BY_EXPL )
                        SETBIT( temp, 2 );
                    if ( brick->dest_type == SHR_BY_SHOT || brick->dest_type == SHR_BY_EXPL )
                        SETBIT( temp, 3 );
                }
                else
                    if ( brick->mod != BRICK_GROW ) {
                        if ( GETBIT( brick->hits, 0 ) )
                            SETBIT( temp, 2 );
                        if ( GETBIT( brick->hits, 1 ) )
                            SETBIT( temp, 3 );
                    }
                /* paddle */
                if ( brick->paddle )
                    SETBIT( temp, 4 );
                /* goldshower */
                if ( brick->gold_shower )
                    SETBIT( temp, 5 );
                comm_raw_buffer[comm_raw_length++] = temp;
                /* impact position */
                temp = 0;
                if ( brick->mod == BRICK_REMOVE && brick->dest_type == SHR_BY_NORMAL_BALL )
                    temp = brick->degrees / 2;
                comm_raw_buffer[comm_raw_length++] = temp;
                /* clear for new updates */
                memset( brick, 0, sizeof( Net_Brick ) );
            }
}

/*
====================================================================
Unpack the brick data and apply the changes to the client. For 
growing bricks only the grow_mask is set and later handled by 
grow_bricks().
====================================================================
*/
static void comm_unpack_bricks()
{
    int x, y, i = comm_raw_length / 3, pos = 0;
    Uint8 temp;
    Net_Brick brick;
    grow = 0; memset( grow_mask, 0xff, sizeof( grow_mask ) );
    while ( i-- > 0 ) {
        /* unpack data */
        memset( &brick, 0, sizeof( Net_Brick ) ); 
        x = comm_raw_buffer[pos] % EDIT_WIDTH; 
        x += 1;
        y = comm_raw_buffer[pos++] / EDIT_WIDTH; 
        y += (MAP_HEIGHT - EDIT_HEIGHT) / 2;
        temp = comm_raw_buffer[pos++];
        brick.mod = temp & 3;
        if ( brick.mod == BRICK_REMOVE )
            brick.dest_type = (temp >> 2) & 3;
        else
            brick.hits = (temp >> 2) & 3;
        brick.paddle = GETBIT( temp, 4 );
        brick.gold_shower = GETBIT( temp, 5 );
        brick.degrees = comm_raw_buffer[pos++]; /* degree cut in half */
        /* parse data */
        if ( brick.gold_shower ) {
            /* the brick destruction might have been overwritten
               so directly push this extra */
            bricks_push_extra( EX_SCORE1000, x * BRICK_WIDTH, y * BRICK_HEIGHT, 
                               (brick.paddle==PADDLE_BOTTOM)?1:-1 );
        }
        switch ( brick.mod ) {
            case BRICK_REMOVE:
                brick_remove( x, y, brick.dest_type, 
                              ((brick.dest_type==SHR_BY_SHOT)?vector_get( 0, (brick.paddle==0)?-1:1 ):impact_vectors[brick.degrees]), 
                              paddles[brick.paddle] );
                break;
            case BRICK_GROW:
                grow = 1;
                grow_mask[x][y] = RANDOM( BRICK_GROW_FIRST, BRICK_GROW_LAST );
                break;
            case BRICK_HIT:
                brick_loose_dur( x, y, brick.hits );
                break;
            case BRICK_HEAL:
                bricks[x][y].dur++;
                bricks[x][y].id++;
                /* redraw */
                if ( !active[EX_DARKNESS] ) {
                    brick_draw( offscreen, x, y, 0 );
                    brick_draw( stk_display, x, y, 0 );
                }
                break;
        }
    }
}

/*
====================================================================
The host sends different messages depending on the level type:
LT_DEATHMATCH:
  8      bit host frags
  8      bit client frags
  16     bit host paddle
  16 +?? bit packed ball data 
LT_NORMAL:
  32     bit host score
  32     bit client score
  16     bit host paddle
  32     bit new extras of paddle bottom
  32     bit new extras of paddle top
    Each bits represents an extra. Only one one-of-a-kind
    is send per message.
  16 +?? bit packed ball data
  ??     bit shot data
  ??     bit brick data
====================================================================
*/
static int comm_send_host_msg()
{
    Net_Msg msg;
    Uint32 temp, i;
#ifdef WITH_COMM_STATS
    stats_out.pack_count++;
#endif
    if ( level_type == LT_DEATHMATCH ) {
        comm_pack_balls();
        net_init_msg( &msg, MSG_DM_HOSTDATA );
        if ( net_pack_int8( &msg, l_paddle->player->frags ) )
        if ( net_pack_int8( &msg, r_paddle->player->frags ) )
        if ( net_pack_int16( &msg, comm_pack_local_paddle() ) )
        if ( net_pack_raw( &msg, comm_raw_buffer, comm_raw_length ) ) 
        if ( net_write_msg( game_peer, &msg ) ) {
#ifdef WITH_COMM_STATS
            stats_out.byte_count += 1 + 2 + 2 + comm_raw_length + 2;
#endif
            return 1;
        }
    }
    else {
        net_init_msg( &msg, MSG_MP_HOSTDATA );
        if ( net_pack_int32( &msg, 
                              (Uint32)display_score[(l_paddle==paddles[1])]->cur_value ) )
        if ( net_pack_int32( &msg, 
                              (Uint32)display_score[(r_paddle==paddles[1])]->cur_value ) )
        if ( net_pack_int16( &msg, comm_pack_local_paddle() ) ) {
            /* pack host extras */
            temp = 0;
            for ( i = 0; i < EX_NUMBER; i++ )
                if ( l_paddle->net_bonuses[i] ) {
                    l_paddle->net_bonuses[i]--;
                    SETBIT( temp, i );
                }
            if ( !net_pack_int32( &msg, temp ) )
                return 0;
            /* pack client extras */
            temp = 0;
            for ( i = 0; i < EX_NUMBER; i++ )
                if ( r_paddle->net_bonuses[i] ) {
                    r_paddle->net_bonuses[i]--;
                    SETBIT( temp, i );
                }
            if ( !net_pack_int32( &msg, temp ) )
                return 0;
#ifdef WITH_COMM_STATS
            stats_out.byte_count += 1 + 8 + 2 + 8;
#endif
            /* pack balls */
            comm_pack_balls();
            if ( !net_pack_raw( &msg, comm_raw_buffer, comm_raw_length ) )
                return 0;
#ifdef WITH_COMM_STATS
            stats_out.byte_count += comm_raw_length + 2;
#endif
            /* pack shots */
            comm_pack_shots();
            if ( !net_pack_raw( &msg, comm_raw_buffer, comm_raw_length ) )
                return 0;
#ifdef WITH_COMM_STATS
            stats_out.byte_count += comm_raw_length + 2;
#endif
            /* pack bricks */
            comm_pack_bricks();
            if ( !net_pack_raw( &msg, comm_raw_buffer, comm_raw_length ) )
                return 0;
#ifdef WITH_COMM_STATS
            stats_out.byte_count += comm_raw_length + 2;
#endif
            /* send message */
            if ( !net_write_msg( game_peer, &msg ) )
                return 0;
            return 1;
        }
    }
    return 0;
}
/*
====================================================================
The client does only send it's paddle info. Anything else is the
hosts task.
====================================================================
*/
static int comm_send_client_msg()
{
    Net_Msg msg;
#ifdef WITH_COMM_STATS
    stats_out.pack_count++;
    stats_out.byte_count += 2 + 1;
#endif
    /* send message */
    net_init_msg( &msg, (level_type==LT_DEATHMATCH)?MSG_DM_CLIENTDATA:MSG_MP_CLIENTDATA );
    if ( net_pack_int16( &msg, comm_pack_local_paddle() ) )
        if ( net_write_msg( game_peer, &msg ) )
            return 1;
    return 0;
}
/*
====================================================================
Parse and apply the information send by host. In opposite to
the client's data the host data differs for the level types.
====================================================================
*/
static void comm_parse_host_msg( Net_Msg *msg )
{
    int temp, i;
    if ( level_type == LT_DEATHMATCH ) {
        /* host frags */
        if ( net_unpack_int8( msg, &temp ) )
            r_paddle->player->frags = temp;
        /* client frags */
        if ( net_unpack_int8( msg, &temp ) )
            l_paddle->player->frags = temp;
        /* host paddle */
        if ( net_unpack_int16( msg, &temp ) )
            comm_update_remote_paddle( temp );
        /* ball positions */
        if ( net_unpack_raw( msg, &comm_raw_buffer, &comm_raw_length, COMM_RAW_BUFFER_LIMIT - 1 ) )
            comm_unpack_balls();
    }
    else {
        /* host & client scores */
        if ( net_unpack_int32( msg, &temp ) )
            r_paddle->player->level_score = temp;
        if ( net_unpack_int32( msg, &temp ) )
            l_paddle->player->level_score = temp;
        /* host paddle */
        if ( net_unpack_int16( msg, &temp ) )
            comm_update_remote_paddle( temp );
        /* host extras */
        if ( net_unpack_int32( msg, &temp ) ) {
            for ( i = 0; i < EX_NUMBER; i++ )
                if ( GETBIT( temp, i ) )
                    extra_use( r_paddle, i );
        }
        /* client extras */
        if ( net_unpack_int32( msg, &temp ) ) {
            for ( i = 0; i < EX_NUMBER; i++ )
                if ( GETBIT( temp, i ) )
                    extra_use( l_paddle, i );
        }
        /* ball positions */
        if ( net_unpack_raw( msg, &comm_raw_buffer, &comm_raw_length, COMM_RAW_BUFFER_LIMIT - 1 ) )
            comm_unpack_balls();
        /* shot positions */
        if ( net_unpack_raw( msg, &comm_raw_buffer, &comm_raw_length, COMM_RAW_BUFFER_LIMIT - 1 ) )
            comm_unpack_shots();
        /* brick data */
        if ( net_unpack_raw( msg, &comm_raw_buffer, &comm_raw_length, COMM_RAW_BUFFER_LIMIT - 1 ) )
            comm_unpack_bricks();
    }
}
/*
====================================================================
Parse and apply the paddle information the client has sent.
====================================================================
*/
static void comm_parse_client_msg( Net_Msg *msg )
{
    int data;
    if ( net_unpack_int16( msg, &data ) )
        comm_update_remote_paddle( data );
}
#endif

/*
====================================================================
Publics
====================================================================
*/

/*
====================================================================
Reset the communicator. (data passed between client/host, 
asynchronous timeout)
====================================================================
*/
void comm_reset()
{
    memset( net_bricks, 0, sizeof( net_bricks ) );
    memset( l_paddle->net_bonuses, 0, sizeof( l_paddle->net_bonuses ) );
    memset( r_paddle->net_bonuses, 0, sizeof( r_paddle->net_bonuses ) );
    memset( &net_paddle, 0, sizeof( net_paddle ) );
    net_paddle.x = l_paddle->x;
    net_ball_reflected = net_ball_attached = 0;
    net_shot_fired = 0;
    /* timeout before asynchronous send */
    delay_set( &sync_timeout, 1000 );
    sync_timeout.cur = sync_timeout.limit; /* initiate connection */
}

/*
====================================================================
Classify next send operation as asynchronous.
====================================================================
*/
void comm_reinit_connection()
{
    sync_timeout.cur = sync_timeout.limit;
}

/*
====================================================================
Handle communication
====================================================================
*/
int comm_handle( int ms, int *result )
{
#ifdef NETWORK_ENABLED
#ifdef WITH_COMM_STATS
    int current_time = SDL_GetTicks();
#endif
    int async = delay_timed_out( &sync_timeout, ms ), i;
    Net_Msg msg;
#ifdef WITH_COMM_STATS
    if ( async )
        stats_out.kickoffs++;
#endif
    if ( game_host ) {
        if ( net_read_msg( game_peer, &msg, 0 ) ) {
#ifdef WITH_COMM_STATS
            stats_in.pack_count++;
            stats_in.low_lat = MINIMUM( stats_in.low_lat, current_time - stats_in.last_call );
            stats_in.high_lat = MAXIMUM( stats_in.high_lat, current_time - stats_in.last_call );
            stats_in.last_call = current_time;
            stats_in.byte_count += msg.length + 1;
#endif
            switch ( msg.type ) {
                case MSG_PAUSE:
                    *result = COMM_PAUSE;
                    return 0;
                case MSG_GAME_EXITED:
                    *result = COMM_QUIT;
                    return 0;
                case MSG_ERROR:
                    net_unpack_string( &msg, client_error, 127 );
                    *result = COMM_ERROR;
                    return 0;
                case MSG_DM_CLIENTDATA:
                case MSG_MP_CLIENTDATA:
                    delay_reset( &sync_timeout );
                    comm_parse_client_msg( &msg );
                    if ( !async )
                        if ( !comm_send_host_msg() )
                            *result = COMM_ERROR;
                    break;
            }
        }
        if ( async )
            if ( !comm_send_host_msg() )
                *result = COMM_ERROR;
    }
    else {
        if ( net_read_msg( game_peer, &msg, 0 ) ) {
#ifdef WITH_COMM_STATS
            stats_in.pack_count++;
            stats_in.low_lat = MINIMUM( stats_in.low_lat, current_time - stats_in.last_call );
            stats_in.high_lat = MAXIMUM( stats_in.high_lat, current_time - stats_in.last_call );
            stats_in.last_call = current_time;
            stats_in.byte_count += msg.length + 1;
#endif
            switch ( msg.type ) {
                case MSG_ROUND_OVER:
                    *result = COMM_NEXT_ROUND;
                    if ( net_unpack_int8( &msg, &i ) && i )
                        *result = COMM_GAME_OVER;
                    net_unpack_int8( &msg, &game_match_won );
                    return 0;
                case MSG_PAUSE:
                    *result = COMM_PAUSE;
                    return 0;
                case MSG_GAME_EXITED:
                    *result = COMM_QUIT;
                    return 0;
                case MSG_ERROR:
                    net_unpack_string( &msg, client_error, 127 );
                    *result = COMM_ERROR;
                    return 0;
                case MSG_DM_HOSTDATA:
                case MSG_MP_HOSTDATA:
                    delay_reset( &sync_timeout );
                    comm_parse_host_msg( &msg );
                    if ( !async )
                        if ( !comm_send_client_msg() )
                            *result = COMM_ERROR;
                    break;
            }
        }
        if ( async )
            if ( !comm_send_client_msg() )
                *result = COMM_ERROR;
    }
    return 1;
#else
    return 1;
#endif
}

/*
====================================================================
Send ROUND_OVER to client so he can initiate next round or end
a game. If 'last' is true the game is over. If 'client_wins'
is True client was the winner.
====================================================================
*/
int comm_send_round_over( int last, int client_wins )
{
#ifdef NETWORK_ENABLED
    Net_Msg msg;
    /* send client the final results */
    if ( net_build_msg( &msg, MSG_ROUND_OVER, last, client_wins ) )
    if ( net_write_msg( game_peer, &msg ) )
        return 1;
    return 0;
#else
    return 1;
#endif
}

/*
====================================================================
Run the remote pause in case the other player send a 
MSG_PAUSE. Wait for the MSG_UNPAUSE or send it yourself.
====================================================================
*/
int comm_remote_pause( StkFont *font, char *str, Net_Socket *peer )
{
#ifdef NETWORK_ENABLED
    SDL_Surface *buffer = 
        stk_surface_create( SDL_SWSURFACE, stk_display->w, stk_display->h );
    char buf[CHAT_MSG_LIMIT + 16];
    int leave = 0, ms = 0, ret = 0;
    SDL_Event event;
    Net_Msg msg;
    
    /* save screen */
    stk_surface_blit( stk_display, 0,0,-1,-1, buffer, 0,0 );
    /* clear chatter */
    memset( chatter, 0, sizeof( chatter ) );
    /* clear global gui widgets */
    gui_focused_widget = 0;
    gui_clicked_widget = 0;
    gui_key_widget = 0;
    /* use 'str' as initial chatter */
    client_add_pausechatter( str, 1 );
    /* gray screen */
    stk_surface_gray( stk_display, 0,0,-1,-1, 1 );
    /* show pauseroom */
    gui_widget_show( dlg_pauseroom );
    stk_display_update( STK_UPDATE_ALL );
    /* disable event filter */
    SDL_SetEventFilter( 0 );
    /* click it, baby */
#ifdef AUDIO_ENABLED
    stk_sound_play( wav_click );
#endif
    /* main loop */
    stk_timer_reset();
    while ( !stk_quit_request && !leave ) {
        /* events */
        if ( SDL_PollEvent( &event ) )
            gui_dispatch_event( &event, ms );
        else
            gui_dispatch_event( 0, ms );
        ms = stk_timer_get_time();
        /* net talk */
        if ( net_read_msg( peer, &msg, 0 ) )
            switch ( msg.type ) {
                case MSG_UNPAUSE:
                    ret = 1; leave = 1;
                    /* close pauseroom */
                    gui_widget_hide( dlg_pauseroom );
                    break;
                case MSG_CHAT:
                    /* add chatter to chat window */
                    if ( net_unpack_string( &msg, buf, CHAT_MSG_LIMIT + 16 ) )
                        client_add_pausechatter( buf, 0 );
                    break;
            }
        /* window closed by callback? */
        if ( !dlg_pauseroom->visible ) {
            leave = 1;
            if ( !net_write_empty_msg( peer, MSG_UNPAUSE ) )
                 ret = 0;
            else ret = 1;
        }
        /* update */
        stk_display_update( STK_UPDATE_RECTS );
        SDL_Delay( 10 );
    }
    /* enable event filter */
    SDL_SetEventFilter( event_filter );
    /* restore screen */
    stk_surface_blit( buffer, 0,0,-1,-1, stk_display, 0,0 );
    stk_display_update( STK_UPDATE_ALL );
    SDL_FreeSurface(buffer);
    /* clear chatter */
    memset( chatter, 0, sizeof( chatter ) );
    return ret;
#else
    return 1;
#endif
}

/*
====================================================================
Clear print stats which are updated by comm_handle().
====================================================================
*/
void comm_clear_stats()
{
#ifdef WITH_COMM_STATS
    memset( &stats_in, 0, sizeof( Comm_Stats ) );
    memset( &stats_out, 0, sizeof( Comm_Stats ) );
    stats_in.low_lat = stats_out.low_lat = 10000;
    stats_in.last_call = stats_out.last_call =
    stats_in.start_time = stats_out.start_time = SDL_GetTicks();
#endif
}
void comm_print_stats()
{
#ifdef WITH_COMM_STATS
    int total_time = stats_in.last_call - stats_in.start_time;
    printf( "---- Statistics ----\n" );
    printf( "Time: %i ms\n", total_time );
    printf( "Packages (in/out): %i / %i\n", stats_in.pack_count, stats_out.pack_count );
    printf( "  Bytes (in/out): %i / %i\n", stats_in.byte_count, stats_out.byte_count );
    printf( "  Per Package (in/out): %i / %i\n", 
            stats_in.byte_count / ((stats_in.pack_count)?stats_in.pack_count:1),
            stats_out.byte_count / ((stats_out.pack_count)?stats_out.pack_count:1) );
    printf( "  Bytes Per Second (in/out): %i Bps / %i Bps\n",
            1000 * stats_in.byte_count / ((total_time)?total_time:1),
            1000 * stats_out.byte_count / ((total_time)?total_time:1) );
    printf( "Latency: %i ms\n", 
            total_time / ((stats_in.pack_count)?stats_in.pack_count:1) );
    printf( "  Lowest: %i ms\n",
            stats_in.low_lat );
    printf( "  Highest: %i ms\n",
            stats_in.high_lat );
    printf( "Asynchronous Kick-Offs: %i\n\n", stats_out.kickoffs );
#endif
}

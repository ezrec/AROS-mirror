/***************************************************************************
                          game.c  -  description
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

#include "lbreakout.h"
#include "event.h"
#include "config.h"
#include "difficulty.h"
#include "shrapnells.h"
#include "levels.h"
#include "player.h"
#include "display.h"
#include "paddle.h"
#include "chart.h"
#include "shine.h"
#include "credit.h"
#include "bricks.h"
#include "shots.h"
#include "frame.h"
#include "balls.h"
#include "extras.h"
#include "comm.h"
#include "help.h"
#include "game.h"

SDL_Surface *bkgnd = 0; /* current background picture */
extern SDL_Surface *nuke_bkgnd; /* nuke background */
SDL_Surface *offscreen = 0; /* buffer with frame, background and bricks */
extern StkFont *font; /* standard font */
Player *player = 0; /* current player */
extern Config config; /* lbreakout config struct */
extern int stk_quit_request; /* terminate game */
extern SDL_Surface *stk_display; 
extern Brick bricks[MAP_WIDTH][MAP_HEIGHT];
extern SDL_Surface *brick_pic;
extern int keystate[SDLK_LAST];
extern int buttonstate[STK_BUTTON_COUNT];
extern int active[EX_NUMBER];
extern int brick_count, brick_extra_count, warp_limit, warp_ok;
extern char **levelset_names; /* names of all available levelsets */
char *levelset_name = 0; /* pointer (may not be deleted) to the current levelset's name */
extern int player_count;
extern Player players[MAX_PLAYERS]; /* player infos */
#ifdef AUDIO_ENABLED
extern StkSound *wav_click;
extern StkSound *wav_damn, *wav_dammit, *wav_wontgiveup, *wav_excellent, *wav_verygood;
#endif
extern List *exp_bricks;
extern List *heal_bricks;
extern List *levels;

/* shots_update returns a list of destroyed bricks */
Brick_Pos shot_bricks[PADDLE_WEAPON_AMMO * 4];
int shot_bricks_count = 0;

/* grow stuff put here because of interaction between balls and bricks */
extern int grow;
extern int grow_mask[MAP_WIDTH][MAP_HEIGHT]; /* indicates which tiles are blocked by a ball */
extern Brick_Conv brick_conv_table[BRICK_COUNT];
extern int brick_count;
extern List *balls;
extern int ball_dia;
extern int shadow_size;

/*
====================================================================
Game settings (in singleplayer this equals the config settings
but in multiplayer these are the remote hosts settings)
====================================================================
*/
#ifdef NETWORK_ENABLED
extern Net_Socket *client; /* client socket to the game server */
Net_Socket *game_peer = 0; /* connection to remote player */
#endif
extern char client_error[128];
int game_type = GT_LOCAL;
int game_host = 0 ; /* true if you host the game */
int game_challenger = 0 ; /* true if you challenged */
Diff *game_diff = 0; /* game's difficulty */
int game_rounds, game_frags, game_balls; /* multiplayer information */
int level_type; /* type of current level */
int game_cur_round; /* current round in level (up to game_rounds) */
int game_match_won = 0; /* comm_handle sets this global so our client knows if he won */
extern char mp_peer_name[16]; /* opponents name for multiplayer */
extern char client_name[16]; /* our name as given by the server */
Paddle *paddles[2] = { 0 }; /* paddle 0 is on bottom and paddle 1 is on top */
Paddle *l_paddle = 0; /* that's the local paddle (handled by local events) */
Paddle *r_paddle = 0; /* remote paddle (handled by net events) */
int paddle_count = 0; /* all paddles */
Display *display_score[2]; /* either score or frags */
Display *display_player[2]; /* player name and lifes or wins */
Vector impact_vectors[180]; /* impact vectors for normal brick remove animation for
                               client to save computation time */

/*
====================================================================
Locals
====================================================================
*/

/* return values of breakout_run() */
enum {
    REQUEST_NONE = 0,
    REQUEST_RESTART,
    REQUEST_NUKE,
    REQUEST_QUIT,
    REQUEST_NEXT_LEVEL,
    REQUEST_NEXT_PLAYER,
    REQUEST_PAUSE,
    REQUEST_NEXT_ROUND,
    REQUEST_HELP,
    REQUEST_WARP,
    REQUEST_GAME_OVER,
    REMOTE_QUIT,
    REMOTE_PAUSE,
    NET_ERROR
};

/*
====================================================================
Create all bricks by ids stored to grow_mask.
====================================================================
*/
static void game_grow_bricks()
{
    int i, j;
    int px, py, refresh_w, refresh_h;
	Ball *list_ball;

    /* clear occupied grow parts -
       the client doesn't need this as his grow mask
       was send by the host */
    if ( game_type == GT_LOCAL || game_host ) {
        list_reset( balls );
        while ( ( list_ball = list_next( balls ) ) != 0 )  {
            grow_mask[(list_ball->x) / BRICK_WIDTH][(list_ball->y) / BRICK_HEIGHT] = -1;
            grow_mask[(list_ball->x + ball_dia) / BRICK_WIDTH][(list_ball->y) / BRICK_HEIGHT] = -1;
            grow_mask[(list_ball->x) / BRICK_WIDTH][(list_ball->y + ball_dia) / BRICK_HEIGHT] = -1;
            grow_mask[(list_ball->x + ball_dia) / BRICK_WIDTH][(list_ball->y + ball_dia) / BRICK_HEIGHT] = -1;
        }
    }
    /* refresh tile size */
    refresh_w = BRICK_WIDTH + shadow_size;
    refresh_h = BRICK_HEIGHT + shadow_size;
    /* add bricks */
    px = py = 0;
    for ( j = 0; j < MAP_HEIGHT; j++ ) {
        for ( i = 0; i < MAP_WIDTH; i++ ) {
            if ( grow_mask[i][j] >= BRICK_GROW_FIRST && grow_mask[i][j] <= BRICK_GROW_LAST ) {
                /* update net brick */
                if ( game_type == GT_NETWORK && game_host )
                    net_brick_set( i, j, BRICK_GROW, 0, vector_get(0,0), 0 );
                /* add brick */
                bricks[i][j].id = brick_conv_table[grow_mask[i][j]].id;
                bricks[i][j].type = brick_conv_table[grow_mask[i][j]].type;
                bricks[i][j].score = brick_conv_table[grow_mask[i][j]].score;
                bricks[i][j].dur = brick_conv_table[grow_mask[i][j]].dur;
                /* keep the extra that is already assigned to this position */
                bricks[i][j].exp_time = -1;
                bricks[i][j].heal_time = -1;
                if ( !active[EX_DARKNESS] )
                    brick_draw_complex( i, j, px, py );
                /* add id to grown_brick mask */
                player->grown_bricks[i][j] = brick_conv_table[grow_mask[i][j]].id;
                /* adjust brick count */
                if ( game_type == GT_LOCAL || game_host )
                    brick_count++;
                /* refresh */
                if ( !active[EX_DARKNESS] ) {
                    stk_surface_blit( offscreen, px, py, 
                        refresh_w, refresh_h,
                        stk_display, px, py );
                    stk_display_store_drect();
                }
            }	
            px += BRICK_WIDTH;
        }            
        py += BRICK_HEIGHT;
        px  = 0;
    }
    grow = 0;
    /* get new targets */
    if ( game_type == GT_LOCAL || game_host )
        balls_check_targets( -1, 0 );
}

/*
====================================================================
Enter nuke mode and allow player to disintegrate single bricks
by spending 5% of his/her score.
====================================================================
*/
static void game_nuke()
{
    char buf[128];
    SDL_Event event;
    int x,y,i,j,leave = 0;
    SDL_Surface *buffer = 
        stk_surface_create( SDL_SWSURFACE, stk_display->w, stk_display->h );
    SDL_Surface *red_mask = 
        stk_surface_create( SDL_SWSURFACE, BRICK_WIDTH, BRICK_HEIGHT );
    stk_surface_fill( red_mask, 0,0,-1,-1, 0xFF0000 );
    SDL_SetAlpha( red_mask, SDL_SRCALPHA, 128 );
    SDL_SetColorKey(buffer, 0, 0);

#ifdef AUDIO_ENABLED
    stk_sound_play( wav_click );
#endif
    SDL_SetEventFilter(0);
    event_clear_sdl_queue();
    /* backup screen contents */
    stk_surface_blit( stk_display, 0,0,-1,-1, buffer, 0,0 );
    /* display bricks darkened */
    stk_surface_blit( nuke_bkgnd, 0,0,-1,-1, 
        stk_display, 0,0 );
    for ( i = 1; i < MAP_WIDTH - 1; i++ )
        for ( j = 1; j < MAP_HEIGHT - 1; j++ )
            if ( bricks[i][j].id >= 0 )
                stk_surface_alpha_blit( brick_pic,
                    bricks[i][j].id * BRICK_WIDTH, 0,
                    BRICK_WIDTH, BRICK_HEIGHT,
                    stk_display,
                    i*BRICK_WIDTH, j*BRICK_HEIGHT, 128 );
    /* info */
    font->align = STK_FONT_ALIGN_LEFT;
    sprintf( buf, "Plane Of Inner Stability entered (Score: %i)",
        player->score );
    stk_font_write( font, stk_display,
        BRICK_WIDTH, (MAP_HEIGHT-1)*BRICK_HEIGHT, 
        128, buf );
    /* show score of player */
    stk_display_update( STK_UPDATE_ALL );
            
    x = y = -1;
    while (!leave && !stk_quit_request) {
        SDL_WaitEvent(&event);
        switch ( event.type ) {
            case SDL_QUIT: stk_quit_request = 1; break;
            case SDL_MOUSEBUTTONUP:
                if ( x != -1 ) {
                    if ( confirm( font, 
                        "Disintegrate Brick? (Costs 5% of your score.) y/n", 
                        CONFIRM_YES_NO ) ) {
                        /* implant a bomb to this brick and return */
                        brick_start_expl( x,y, l_paddle );
                        /* this brick won't give you score */
                        bricks[x][y].score = 0;
                        player->score -= (int)(0.05 * player->score);
                        leave = 1;
                    }
                }
                break;
            case SDL_MOUSEMOTION:
                if ( x != -1 ) {
                    /* clear old selection */
                    stk_surface_blit( nuke_bkgnd,
                        x*BRICK_WIDTH, y*BRICK_HEIGHT,
                        BRICK_WIDTH, BRICK_HEIGHT,
                        stk_display, 
                        x*BRICK_WIDTH, y*BRICK_HEIGHT );
                    stk_surface_alpha_blit( brick_pic,
                        bricks[x][y].id * BRICK_WIDTH, 0,
                        BRICK_WIDTH, BRICK_HEIGHT,
                        stk_display,
                        x*BRICK_WIDTH, y*BRICK_HEIGHT, 128 );
                    stk_display_store_drect();
                    x = y = -1;
                }
                /* make new selection if brick */
                i = event.motion.x / BRICK_WIDTH;
                j = event.motion.y / BRICK_HEIGHT;
                if ( i >= 1 && i <= MAP_WIDTH -2 )
                if ( j >= 1 && j <= MAP_HEIGHT - 2 )
                if ( bricks[i][j].id >= 0 ) {
                    x = i; y = j;
                    stk_surface_blit( red_mask, 0,0,-1,-1,
                        stk_display,x*BRICK_WIDTH, y*BRICK_HEIGHT );
                    stk_display_store_drect();
                }
                break;
            case SDL_KEYDOWN:
                if ( event.key.keysym.sym == SDLK_ESCAPE )
                    leave = 1;
                break;
        }
        stk_display_update( STK_UPDATE_RECTS );
    }
            
    stk_surface_blit( buffer, 0,0,-1,-1, stk_display, 0,0 );
    stk_display_update( STK_UPDATE_ALL );
    SDL_FreeSurface(red_mask);
    SDL_FreeSurface(buffer);
    SDL_SetEventFilter(event_filter);
}

/*
====================================================================
Initate current level from player's level resource pointer.
Recreates offscreen (frame, background, bricks) and resets paddle,
balls, extras. Do not add the bricks already removed:
player->bricks[x][y] == 0.
Return Value: True if successful
====================================================================
*/
enum { WITH_CREDITS = 1 };
static int game_init_level( Player *player, int with_credits )
{
    int ammo;
    int i, j, k, length;
#ifdef NETWORK_ENABLED    
    Net_Msg msg;
#endif
    char str[32];
    
#ifdef NETWORK_ENABLED
    /* the leveldata is provided by the challenger (which doesn't have
       to equal the game_host) */
    if ( game_type == GT_NETWORK ) {
        if ( game_challenger ) {
            net_init_msg( &msg, MSG_LEVEL_DATA );
            if ( net_pack_string( &msg, player->level->author ) )
            if ( net_pack_string( &msg, player->level->name ) )
            if ( net_pack_raw( &msg, player->level->bricks, sizeof( player->level->bricks ) ) )
            if ( net_pack_raw( &msg, player->level->extras, sizeof( player->level->extras ) ) )
            if ( net_write_msg( game_peer, &msg ) ) {
#ifdef GAME_DEBUG
    printf( "level data sent to peer - awaiting READY...\n" );
#endif
                msg.type = MSG_NONE;
                while ( 1 ) {
                    if ( net_read_msg( game_peer, &msg, 10000 ) ) {
                        if ( msg.type == MSG_DM_CLIENTDATA || msg.type == MSG_MP_CLIENTDATA )
                            continue;
                        if ( msg.type == MSG_DM_HOSTDATA || msg.type == MSG_MP_HOSTDATA )
                            continue;
                        if ( msg.type == MSG_GAME_EXITED ) {
                            sprintf( client_error, "remote player has left the game" );
                            break;
                        }
                        if ( msg.type != MSG_READY ) {
                            strcpy( client_error, net_get_error() );
                            if ( client_error[0] == 0 )
                                sprintf( client_error, "remote player did not send a READY" );
                        }
                        break;
                    }
                    else {
                        if ( client_error[0] == 0 )
                            sprintf( client_error, "connection timed out" );
                        break;
                    }
                }
            }
            if ( msg.type != MSG_READY )
                return 0;
#ifdef GAME_DEBUG
    printf( "received!\n" );
#endif
        }
        else {
            msg.type = MSG_NONE;
            /* receive stuff */
#ifdef GAME_DEBUG
    printf( "waiting for level data...\n" );
#endif
            while ( 1 ) {
                if ( net_read_msg( game_peer, &msg, 10000 ) ) {
                    if ( msg.type == MSG_DM_CLIENTDATA || msg.type == MSG_MP_CLIENTDATA )
                        continue;
                    if ( msg.type == MSG_DM_HOSTDATA || msg.type == MSG_MP_HOSTDATA )
                        continue;
                    if ( msg.type == MSG_GAME_EXITED ) {
                        sprintf( client_error, "remote player has left the game" );
                        break;
                    }
                    if ( msg.type != MSG_LEVEL_DATA ) {
                        strcpy( client_error, net_get_error() );
                        if ( client_error[0] == 0 )
                            sprintf( client_error, "challenger did not send level data" );
                    }
                    break;
                }
                else {
                    if ( client_error[0] == 0 )
                        sprintf( client_error, "connection timed out" );
                    break;
                }
            }
            if ( msg.type != MSG_LEVEL_DATA )
                return 0;
#ifdef GAME_DEBUG
    printf( "received!\n" );
#endif
            net_unpack_string( &msg, player->level->author, 31 );
            net_unpack_string( &msg, player->level->name, 31 );
            net_unpack_raw( &msg, player->level->bricks, &length, sizeof( player->level->bricks ) );
            net_unpack_raw( &msg, player->level->extras, &length, sizeof( player->level->extras ) );
            if ( length != sizeof( player->level->bricks ) ) {
                sprintf( client_error, "Incompatible level size" );
                return 0;
            }
#ifdef GAME_DEBUG
    printf( "sending ready\n" );
#endif
            if ( !net_write_empty_msg( game_peer, MSG_READY ) )
                return 0;
        }
    }
#endif
    
    /* load bricks&extras from level data */
    bricks_create( player->level );
    
    /* check if this is a deathmatch level */
    if ( game_type == GT_NETWORK && brick_count == 0 )
        level_type = LT_DEATHMATCH;
    else
        level_type = LT_NORMAL;

    /* remove cleared bricks (only need for singe player) */
    if ( game_type == GT_LOCAL ) {
        for ( i = 1; i < MAP_WIDTH - 1; i++ )
            for ( j = 1; j < MAP_HEIGHT - 1; j++ ) {
                if ( player->bricks[i][j] == -99 ) {
                    /* initiate duration -- 0 means this brick was removed */
                    if ( bricks[i][j].type == MAP_EMPTY )
                        player->bricks[i][j] = 0;
                    else
                        player->bricks[i][j] = bricks[i][j].dur;
                }
                else
                    if ( player->bricks[i][j] == 0 ) {
                        if ( bricks[i][j].dur > 0 ) brick_count--;
                        bricks[i][j].id = -1;
                        bricks[i][j].dur = -1;
                        bricks[i][j].type = MAP_EMPTY;
                    }
                    else 
                        if ( bricks[i][j].type != MAP_BRICK_HEAL ) /* generative bricks are kept reset */
                            if ( bricks[i][j].dur > 1 ) { 
                                /*if multiple duration we need to use the player->bricks value */
                                for ( k = 0; k < bricks[i][j].dur - player->bricks[i][j]; k++ )
                                    bricks[i][j].id--;
                                bricks[i][j].dur = player->bricks[i][j];
                            }
                /* regnerative bricks with a durability of less than three shall be added to the regen list */
                if ( bricks[i][j].type == MAP_BRICK_HEAL && bricks[i][j].dur < 3 ) {
                    bricks[i][j].mx = i;
                    bricks[i][j].my = j;
                    bricks[i][j].heal_time = BRICK_HEAL_TIME;
                    list_add( heal_bricks, &bricks[i][j] );
                }
                if ( player->grown_bricks[i][j] ) {
                    if ( bricks[i][j].type != MAP_EMPTY ) 
                        fprintf( stderr, "WARNING: Grown brick is blocked by another brick at %i,%i.\n", i, j );
                    else {
                        /* add grown brick */
                        bricks[i][j].id = player->grown_bricks[i][j];
                        bricks[i][j].dur = 1;
                        bricks[i][j].type = MAP_BRICK;
                        brick_count++;
                    }
                }
            }
        /* check warp limit */
        if ( brick_count <= warp_limit )
            warp_ok = 1;
    }
    
    /* initiate frame for game type */
    frame_init();
    
    /* recreate offscreen */
    if ( offscreen ) SDL_FreeSurface( offscreen );
    offscreen = stk_surface_create( SDL_SWSURFACE, stk_display->w, stk_display->h );
    SDL_SetColorKey( offscreen, 0, 0 );

    /* add&create background */
    if ( bkgnd ) SDL_FreeSurface( bkgnd );
    bkgnd = bkgnd_draw( player->level->bkgnd_id );
    /* add frame */
    frame_draw();
    /* add bricks */
    bricks_draw();
    /* draw lives */
    if ( game_type == GT_LOCAL )
        frame_draw_lives( player->lives, game_diff->max_lives );
    
    /* create paddles */
    for ( i = 0; i < paddle_count; i++ )
        paddle_delete( paddles[i] );
    paddle_count = 0;
    if ( game_type == GT_LOCAL ) {
        paddles[0] = paddle_create( ( MAP_HEIGHT - 2 ) * BRICK_HEIGHT, 0 );
        paddles[0]->player = player;
        paddle_count = 1;
        l_paddle = paddles[0];
        r_paddle = 0;
    }
    else {
        ammo = ( level_type == LT_DEATHMATCH ) ? game_balls : 0;
        paddles[0] = paddle_create( ( MAP_HEIGHT - 2 ) * BRICK_HEIGHT, ammo );
        paddles[1] = paddle_create( BRICK_HEIGHT + 2, ammo );
        if ( game_challenger ) {
            paddles[0]->player = &players[0];
            paddles[1]->player = &players[1];
            l_paddle = paddles[0];
            r_paddle = paddles[1];
        }
        else {
            paddles[0]->player = &players[1];
            paddles[1]->player = &players[0];
            l_paddle = paddles[1];
            r_paddle = paddles[0];
        }
        paddle_count = 2;
    }
    
    /* initiate special multiplayer vars */
    if ( game_type == GT_NETWORK )
        for ( i = 0; i < paddle_count; i++ ) {
            paddles[i]->player->frags = 0;
            paddles[i]->player->level_score = 0;
        }
    
    /* statistics */
    if ( game_type == GT_NETWORK && game_host )
        for ( i = 0; i < paddle_count; i++ ) {
            paddles[i]->player->bricks_total += brick_count;
            paddles[i]->player->extras_total += brick_extra_count;
        }
        
    /* reset the net data */
    if ( game_type == GT_NETWORK )
        comm_reset();
    
    /* displays */
    displays_clear();
    if ( game_type == GT_LOCAL ) {
        /* we put these displays to the old positions 
           at the top of the frame */
        sprintf( str, "%s", player->name );
        length = ( strlen( player->name ) ) * 8;
        display_player[0] = displays_add( 402, 0,
                                          length + 4, 16, str, player->lives, 0 );
        display_score[0] = displays_add( stk_display->w - 52 - 76, 0,
                                         76, 16, "", player->score, 9 );
        display_player[0]->use_alpha = 0;
        display_score[0]->use_alpha = 0;
    }
    else {
        sprintf( str, "%s ~x%i", paddles[0]->player->name, paddles[0]->player->wins );
        length = strlen( str ) * 8;
        display_player[0] = displays_add( BRICK_WIDTH + 20, ( MAP_HEIGHT - 1 ) * BRICK_HEIGHT + 2,
                                          length + 4, 16, str, 0, 0 );
        sprintf( str, "%s ~x%i", paddles[1]->player->name, paddles[1]->player->wins );
        length = strlen( str ) * 8;
        display_player[1] = displays_add( BRICK_WIDTH + 20, 2,
                                          length + 4, 16, str, 0, 0 );
        if ( level_type == LT_NORMAL ) {
            display_score[0] = displays_add( stk_display->w - BRICK_WIDTH - 20 - 76,
                                             ( MAP_HEIGHT - 1 ) * BRICK_HEIGHT + 2,
                                             76, 16, "", paddles[0]->player->level_score, 9 );
            display_score[1] = displays_add( stk_display->w - BRICK_WIDTH - 20 - 76, 2,
                                             76, 16, "", paddles[1]->player->level_score, 9 );
        }
        else {
            display_score[0] = displays_add( stk_display->w - BRICK_WIDTH - 20 - 76, 
                                             ( MAP_HEIGHT - 1 ) * BRICK_HEIGHT + 2,
                                             76, 16, "Frags: ", paddles[0]->player->frags, 2 );
            display_score[1] = displays_add( stk_display->w - BRICK_WIDTH - 20 - 76, 2,
                                             76, 16, "Frags: ", paddles[1]->player->frags, 2 );
        }
    }
    
    /* event reset */
    event_reset();
    /* shots reset */
    shots_reset();
    /* reset balls */
    balls_set_vel( game_diff->v_start, game_diff->v_change, game_diff->v_max );
    balls_reset();
    /* reset extras */
    extras_reset();
    /* reset shrapnells */
    shrapnells_reset();
    /* reset shine */
    shine_reset();
    /* reset explosions */
    exps_clear();
    exps_set_dark( 0 );
    /* initiate credit */
    if ( with_credits )
        credit_init( player->level->name, player->level->author, levels_get_id( player->level ) );

    /* show offscreen */
    stk_surface_blit( offscreen, 0,0,-1,-1, stk_display, 0,0 );
    /* no refresh rect as we want to use dim effect */
    return 1;
}

/*
====================================================================
Display info about player.
====================================================================
*/
static void player_info()
{
    int i;
    char info[256], buf[32];
    /* short info about actual player */
    /* no info if this is the last active player */
    if ( players_count() > 1 ) {
        sprintf( info, "Next Player: %s##", player->name );
        for ( i = 0; i < player_count; i++ ) {
            /* add player and score */
            sprintf( buf, "#%12s %10i", " ", 
                     players[i].score );
            strcpy( buf + 1, players[i].name );
            buf[strlen(players[i].name)+1] = 32;
            strcat( info, buf );
        }
        confirm( font, info, CONFIRM_ANY_KEY );
    }
    else
        stk_display_update( STK_UPDATE_ALL );
}
/*
====================================================================
Display final message.
====================================================================
*/
static void final_info()
{
    int i;
    char info[256], buf[32];
    /* short info about actual player */
    sprintf( info, "Game Over!##" );
    for ( i = 0; i < player_count; i++ ) {
        /* add player and score */
        sprintf( buf, "#%12s %10i", " ", 
                 players[i].score );
        strcpy( buf + 1, players[i].name );
        buf[strlen(players[i].name)+1] = 32;
        strcat( info, buf );
    }
    confirm( font, info, CONFIRM_ANY_KEY );
}

/*
====================================================================
Runs Lbreakout until player looses ball or explicitly wishes
to quit, restart. (just request! confirmation not done here)
Assumes that the current offscreen was already displayed on screen
by _level.
====================================================================
*/
static int breakout_run() {
    SDL_Event event;
    int result = REQUEST_NONE;
    int ms, i;
    int min_time; /* a frame must last this number of milliseconds at minimum */
    Brick *brick;
    int top, bottom;
    
    /* clear communicator statistics */
    if ( game_type == GT_NETWORK )
        comm_clear_stats();
    /* frame delay */
    min_time = ( config.fps ) ? 10 : 0;
    /* grab input if wanted */
    event_grab_input();
    /* run main loop */
    event_reset();
    stk_timer_reset(); ms = 1;
    while( result == REQUEST_NONE ) {
        /* events */
        if ( event_poll( &event ) ) {
            switch ( event.type ) {
                case SDL_QUIT: stk_quit_request = 1; result = REQUEST_QUIT; break;
                case SDL_KEYDOWN:
                    if ( keystate[config.k_lfire] || keystate[config.k_rfire] )
                        if ( weapon_installed( l_paddle ) )
                            if ( !weapon_firing( l_paddle ) ) 
                                weapon_start_fire( l_paddle );
                    /* various requests */
                    switch ( event.key.keysym.sym ) {
                        case SDLK_F1:
                        case SDLK_h:
                            if ( game_type == GT_NETWORK ) break; /* only for single player */
                            result = REQUEST_HELP;
                            break;
						case SDLK_q:
                        case SDLK_ESCAPE: result = REQUEST_QUIT; break;
                        case SDLK_r:
                            if ( game_type == GT_NETWORK ) break; /* only for single player */
                            result = REQUEST_RESTART;
                            break;
                        case SDLK_d:
                            if ( game_type == GT_NETWORK ) break; /* only for single player */
                            result = REQUEST_NUKE;
                            break;
                        case SDLK_f:
                            event_ungrab_input();
                            config.fullscreen = !config.fullscreen;
                            stk_display_apply_fullscreen( config.fullscreen );
                            /* redraw offscreen to get background */
                            stk_surface_blit( offscreen, 0,0,-1,-1, 
                                stk_display, 0,0 );
                            stk_display_update( STK_UPDATE_ALL);
                            event_grab_input();
                            break;
                        case SDLK_s:
#ifdef AUDIO_ENABLED
                            config.sound = !config.sound;
                            stk_audio_enable_sound( config.sound );
#endif
                            break;
                        case SDLK_a:
                            config.anim++;
                            if ( config.anim >= 4 ) config.anim = 0;
                            break;
                        case SDLK_TAB:
                            stk_display_take_screenshot();
                            break;
                        case SDLK_t:
                            break;
                        case SDLK_p:
                            result = REQUEST_PAUSE;
                            break;
                        default: 
                            if ( game_type == GT_LOCAL )
                                if ( event.key.keysym.sym == config.k_warp )
                                    result = REQUEST_WARP;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    /* stop firing shots */
                    if ( weapon_firing( l_paddle ) )
                        if ( !keystate[config.k_lfire] && !keystate[config.k_rfire] ) 
                            weapon_stop_fire( l_paddle );
                    /* may bring new balls to game */
                    l_paddle->block_new_ball = 0;
                    /* return balls that may do so */
                    if ( event.key.keysym.sym == config.k_return )
                        balls_return( l_paddle );
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if ( buttonstate[STK_BUTTON_LEFT] && weapon_installed( l_paddle ) )
                        if ( !weapon_firing( l_paddle ) ) 
                            weapon_start_fire( l_paddle );
                    break;
                case SDL_MOUSEBUTTONUP:
                    if ( weapon_firing( l_paddle ) )
                        if ( !buttonstate[STK_BUTTON_LEFT] ) 
                            weapon_stop_fire( l_paddle );
                    if ( event.button.button == STK_BUTTON_MIDDLE )
                        balls_return( l_paddle );
                    l_paddle->block_new_ball = 0;
                    break;
            }
        }
        /* hide */
        displays_hide();
        frame_info_hide();
        extras_hide();
        for ( i = 0; i < paddle_count; i++ )
            paddle_hide( paddles[i] );
        balls_hide();
        shots_hide();
        shrapnells_hide();
        wall_hide();
        frame_warp_icon_hide();
        shine_hide();
        exps_hide();
        credit_hide();
        /* handle sdl events for local paddle */
        paddle_handle_events( l_paddle, ms );
        /* handle remote events */
        if ( game_type == GT_NETWORK )
            if ( !comm_handle( ms, &result ) ) {
                /* translate the comm result into breakout result */
                switch ( result ) {
                    case COMM_PAUSE: result = REMOTE_PAUSE; break;
                    case COMM_QUIT: result = REMOTE_QUIT; break;
                    case COMM_ERROR: result = NET_ERROR; break;
                    case COMM_NEXT_ROUND: result = REQUEST_NEXT_ROUND; break;
                    case COMM_GAME_OVER: result = REQUEST_GAME_OVER; break;
                }
                break;
            }
        /* update paddles */
        for ( i = 0; i < paddle_count; i++ )
            paddle_update( paddles[i], ms );
        /* balls are only updated by host */
        if ( game_type == GT_LOCAL || game_host ) {
            if ( balls_update( ms, &top, &bottom ) ) {
                /* balls were lost: 'top' balls at top and 'bottom' balls at bottom */
                if ( game_type == GT_LOCAL && balls->count == 0 ) {
                    /* all balls where lost */
                    result = REQUEST_NEXT_PLAYER;
                    player->lives--;
                }
                else
                    if ( game_type == GT_NETWORK ) {
                        if (level_type == LT_DEATHMATCH ) {
                            /* adjust frags */
                            paddles[0]->player->frags += top;
                            paddles[1]->player->frags += bottom;
                            paddles[0]->player->frags_total += top;
                            paddles[1]->player->frags_total += bottom;
                            /* limit reached? */
                            if ( paddles[0]->player->frags >= game_frags || paddles[1]->player->frags >= game_frags ) {
                                result = REQUEST_NEXT_ROUND;
                            }
                            else {
                                /* allow firing these balls */
                                paddles[0]->ammo += bottom;
                                paddles[1]->ammo += top;
                            }
                        }
                        else {
                            /* in normal game we loose 10% score for each ball */
                            /* only one ball may be fired back */
                            if ( top ) {
                                player_add_score( paddles[1]->player, -top * ( paddles[1]->player->level_score / 10 ) );
                                if ( paddles[1]->ammo == 0 ) 
                                    paddles[1]->ammo = 1;
                            }
                            if ( bottom ) {
                                player_add_score( paddles[0]->player, -bottom * ( paddles[0]->player->level_score / 10 ) );
                                if ( paddles[0]->ammo == 0 )
                                    paddles[0]->ammo = 1;
                            }
                        }
                    }
            }
        }
        /* the shots are only handled by host. the client tells that
           he is firing and receives the positions of all active shots */
        if ( game_type == GT_LOCAL || game_host )
            shots_update( ms, shot_bricks, &shot_bricks_count );
        /* explosions and animations are drawn by client and host 
           but for the client there is no effect in collecting an extra
           or hitting a brick with a shot (the host has sent relevant 
           results by comm_handle() */
        extras_update( ms );
        wall_update( ms );
        shrapnells_update( ms );
        frame_warp_icon_update( ms );
        shine_update( ms );
        exps_update( ms );
        /* the display value is sent to the client so only
           the host needs to update */
        if ( game_type == GT_LOCAL || game_host )
            displays_update( ms );
        /* credits are unimportant and run for both */
        credit_update( ms );
        /* brick modifications - host only */
        if ( game_type == GT_LOCAL || game_host ) {
            /* bricks destroyed by shots */
            for ( i = 0; i < shot_bricks_count; i++ )
                balls_check_targets( shot_bricks[i].x, shot_bricks[i].y );
            /* check if bricks were destroyed by explosion */
            if ( exp_bricks->count > 0 ) {
                list_reset( exp_bricks );
                while ( ( brick = list_next( exp_bricks ) ) != 0 ) {
                    brick->exp_time -= ms;
                    if ( brick->exp_time <= 0 ) {
                        brick->exp_time = -1;
                        brick_remove( brick->mx, brick->my, SHR_BY_EXPL, 
                                      vector_get( 0, 0 ), brick->exp_paddle );
                        balls_check_targets( brick->mx, brick->my );
                        list_delete_current( exp_bricks );
                    }
                }
            }
            /* check if bricks regenerate */
            if ( heal_bricks->count > 0 ) {
                list_reset( heal_bricks );
                while ( ( brick = list_next( heal_bricks ) ) != 0 ) {
                    /* skip brick if destroyed meanwhile */
                    if ( brick->type != MAP_BRICK_HEAL ) {
                        list_delete_current( heal_bricks );
                        continue;
                    }
                    brick->heal_time -= ms;
                    if ( brick->heal_time < 0 ) {
                        brick->dur++;
                        brick->id++;
                        if ( game_type == GT_NETWORK && game_host )
                            net_brick_set( brick->mx, brick->my, BRICK_HEAL, 
                                           0, vector_get(0,0), 0 );
                        /* redraw */
                        if ( !active[EX_DARKNESS] ) {
                            brick_draw( offscreen, brick->mx, brick->my, 0 );
                            brick_draw( stk_display, brick->mx, brick->my, 0 );
                            stk_display_store_drect();
                        }
                        if ( brick->dur < 3 ) {
                            /* initate next healing step */
                            brick->heal_time = BRICK_HEAL_TIME;
                        }
                        else {
                            brick->heal_time = -1;
                            list_delete_current( heal_bricks );
                        }
                    }
                }
            }
            /* update ball speed */
            if ( !active[EX_SLOW] && !active[EX_FAST] ) balls_inc_vel( ms );
            /* check if all bricks where destroyed */
            if ( brick_count == 0 && level_type == LT_NORMAL ) {
                if ( game_type == GT_LOCAL )
                    result = REQUEST_NEXT_LEVEL;
                else
                    if ( game_host )
                        result = REQUEST_NEXT_ROUND;
            }
        }
        /* check if there are growing bricks - the client
           was sent the grow mask by the communicator */
        if ( grow ) game_grow_bricks();
        /* update score - host & client */
        if ( game_type == GT_LOCAL ) {
            display_set_value( display_score[0], paddles[0]->player->score );
            display_set_value( display_player[0], paddles[0]->player->lives );
        }
        else {
            if ( level_type == LT_NORMAL ) {
                display_set_value( display_score[0], paddles[0]->player->level_score );
                display_set_value( display_score[1], paddles[1]->player->level_score );
            }
            else {
                display_set_value( display_score[0], paddles[0]->player->frags );
                display_set_value( display_score[1], paddles[1]->player->frags );
            }
        }
        /* client has no display animation */
        if ( game_type == GT_NETWORK && !game_host ) {
            display_score[0]->cur_value = display_score[0]->value;
            display_score[1]->cur_value = display_score[1]->value;
        }
        /* show -- some things will be missing if darkness is enabled */
        balls_show_shadow();
        extras_show_shadow();
        for ( i = 0; i < paddle_count; i++ )
            paddle_show_shadow( paddles[i] );
       	shots_show();
        if ( config.debris_level == DEBRIS_BELOW_BALL ) {
            exps_show();
            if ( !active[EX_DARKNESS] ) {
                shrapnells_show();
                frame_info_show();
            }
        }	
		if ( config.ball_level == BALL_ABOVE_BONUS )
            extras_show();
        balls_show();
		if ( config.ball_level == BALL_BELOW_BONUS )
            extras_show();
        for ( i = 0; i < paddle_count; i++ )
            paddle_show( paddles[i] );
        if ( !active[EX_DARKNESS] ) wall_show();
        shine_show();
        if ( config.debris_level == DEBRIS_ABOVE_BALL ) {
            exps_show();
            if ( !active[EX_DARKNESS] ) {
                shrapnells_show();
                frame_info_show();
            }
        }	
        frame_warp_icon_show();
        displays_show();
        credit_show();
        /* update anything that was changed */
        stk_display_update( STK_UPDATE_RECTS );
        /* determine how long it took to draw the frame */
        ms = stk_timer_get_time();
        /* if we're below min_time we'll have to wait */
        if ( ms < min_time ) {
            SDL_Delay( min_time - ms );
            ms += stk_timer_get_time();
        }
    }
    /* release input */
    event_ungrab_input();
    /* print stats */
    if ( game_type == GT_NETWORK )
        comm_print_stats();
    return result;
}
/*
====================================================================
Fade all animations until they disappear
====================================================================
*/
static void fade_anims()
{
    float alpha = 255.0;
    int ms, i;
    stk_timer_reset();
    if ( game_type == GT_LOCAL )
        frame_remove_life();
    while ( alpha > 0 ) {
        displays_hide();
        for ( i = 0; i < paddle_count; i++ )
            paddle_hide( paddles[i] );
        balls_hide();
        extras_hide();
        shrapnells_hide();
        shots_hide();
        wall_hide();
        credit_hide();
        ms = stk_timer_get_time();
        alpha -= 0.5 * ms;
        if ( alpha < 0 ) alpha = 0;
        shrapnells_update( ms );
        for ( i = 0; i < paddle_count; i++ )
            paddle_alphashow( paddles[i], alpha );
        balls_alphashow( alpha );
        extras_alphashow( alpha );
        shots_alphashow( alpha );
        shrapnells_show();
        wall_alphashow( alpha );
        displays_show();
        credit_alphashow( alpha );
        stk_display_update( STK_UPDATE_RECTS );
    }
}

/*
====================================================================
Publics
====================================================================
*/

/*
====================================================================
Load all static resources like frame, bricks, balls, extras...
====================================================================
*/
void game_create()
{
    int i;
    bricks_load();
    ball_load();
    shot_load();
    extras_load();
    frame_create();
    shrapnells_init();
    shine_load();
    /* load names of all valid levelsets */
    levelsets_load_names();
    /* create vectors for all degrees in 2° steps */
    for ( i = 0; i < 180; i++ ) {
        impact_vectors[i].x = cos( 6.28 * i / 180 );
        impact_vectors[i].y = sin( 6.28 * i / 180 );
    }
}
/*
====================================================================
Delete anything created by game_create();
====================================================================
*/
void game_delete()
{
    int i;
    bricks_delete();
    for ( i = 0; i < paddle_count; i++ )
        paddle_delete( paddles[i] );
    paddle_count = 0;
    displays_clear();
    ball_delete();
    shot_delete();
    extras_delete();
    frame_delete();
    shrapnells_delete();
    shine_delete();
    /* free levelset names */
    levelsets_delete_names();
}
/*
====================================================================
Initiates player and the first level. If channel is set the 
multiplayer values are handled.
Return Value: True if successful
====================================================================
*/
int game_init( char *setname, int diff_level, 
               Net_Socket *channel, int challenger, int host, int rounds, int frags, int balls )
{
    int i;
    /* game type */
    if ( channel == 0 )
        game_type = GT_LOCAL;
    else
        game_type = GT_NETWORK;
    /* load levels:
     * multiplayer levels are only loaded by host,
     * the client starts with an all empty set and receives the current level data
     * over the connection
     */
    if ( game_type == GT_LOCAL || challenger ) {
        if ( !levels_load( setname ) ) 
            return 0;
    }
    else {
        /* client needs just one level which is filled
           with challengers leveldata in game_init_level */
        levels_create_empty_set( 1 );
    }
	/* set global pointer to set name */
	levelset_name = setname;
    /* set difficulty */
    game_diff = diff_get( diff_level );
    /* initiate players */
    players_clear();
    if ( channel == 0 ) {
        for ( i = 0; i < config.player_count; i++ )
            player_add( config.player_names[i], game_diff->lives, levels_get_first() );
    }
    else {
        player_add( client_name, game_diff->lives, levels_get_first() );
        player_add( mp_peer_name, game_diff->lives, levels_get_first() );
    }
    player = players_get_first();
    /* special multiplayer settings */
#ifdef NETWORK_ENABLED
    if ( game_type == GT_NETWORK ) {
        game_peer = channel;
        game_challenger = challenger;
        game_host = host;
        game_cur_round = 0;
        game_rounds = rounds;
        game_frags = frags;
        game_balls = balls - 1; /* the player can always fire the initial ball */
    }
#endif
    /* initiate level */
    return game_init_level( player, WITH_CREDITS );
}

/*
====================================================================
Free all memory allocated by game_init()
====================================================================
*/
void game_clear()
{
    stk_surface_free( &offscreen );
    stk_surface_free( &bkgnd );
    credit_clear();
}
/*
====================================================================
Run game after first level was initiated. Initiates next levels,
too.
====================================================================
*/
void game_run()
{
    int leave = 0;
    int next_player = 0;
    int result;
    char str[64];
    int i;
#ifdef NETWORK_ENABLED
    int win;
    Net_Msg msg;
    Player *winner;
#endif
    
    stk_display_fade( STK_FADE_IN, STK_FADE_DEFAULT_TIME );
    if ( game_type == GT_LOCAL )
        player_info();
    else {
#ifdef NETWORK_ENABLED
        sprintf( str, "Round %i: Get Ready!", game_cur_round + 1 );
        if ( !display_info( font, str, game_peer ) )
            return;
#endif
    }
    while ( !leave && !stk_quit_request ) {
        /* run game until player wants to quit or restart or lost the ball or reached next level */
        result = breakout_run();
        if ( game_type == GT_NETWORK ) {
#ifdef NETWORK_ENABLED
            /* separate handling for multiplayer */
            switch ( result ) {
                case REQUEST_GAME_OVER:
                case REQUEST_NEXT_ROUND:
                    if ( game_host ) {
                        /* check the result */
                        if ( level_type == LT_NORMAL ) {
                            /* scores are updated automatically
                               by player_add_score() */
                            /* player with the highest levelscore wins */
                            if ( players[0].level_score > players[1].level_score ) {
                                winner = &players[0];
                                players[0].wins++;
                                players[1].losses++;
                            }
                            else {
                                winner = &players[1];
                                players[1].wins++;
                                players[0].losses++;
                            }
                        }
                        else {
                            /* player who hit the frag limit wins */
                            if ( players[0].frags >= game_frags ) {
                                winner = &players[0];
                                players[0].wins++;
                                players[1].losses++;
                            }
                            else {
                                winner = &players[1];
                                players[1].wins++;
                                players[0].losses++;
                            }
                        }
                        /* send update to game server */
                        if ( game_challenger )
                            win = (winner == player);
                        else 
                            win = !(winner == player);
                        if ( net_build_msg( &msg, MSG_WINNER, win ) )
                            net_write_msg( client, &msg );
                        /* next round */
                        game_cur_round++;
                        if ( game_cur_round == game_rounds ) {
                            /* next level */
                            player->level = levels_get_next( player->level );
                            if ( !player->level ) {
                                /* the game is over */
                                leave = 1;
                                client_error[0] = 0;
                            }
                            else
                            if ( !game_init_level( player, 1 ) )
                                leave = 1;
                            game_cur_round = 0;
                        }
                        else {
                            /* restart level */
                            if ( !game_init_level( player, 1 ) )
                                leave = 1;
                        }
                        /* inform client that round is over.
                           if 'leave' is True the game is finished.
                           pass if client won the round. */
                        if ( !comm_send_round_over( leave, (winner!=player) ) )
                            leave = 1;
                    }
                    else {
                        /* game over? */
                        if ( result == REQUEST_GAME_OVER ) {
                            leave = 1;
                            break;
                        }
                        /* update round index */
                        game_cur_round++;
                        if ( game_cur_round == game_rounds )
                            game_cur_round = 0;
                        /* check if we won (received by comm_handle) */
                        winner = (game_match_won)?player:0;
                        /* initiate level */
                        if ( !game_init_level( player, 1 ) )
                            leave = 1;
                    }
                    if ( !leave ) {
                        /* get ready to kick ass! */
                        if ( winner == player )
                            sprintf( str, "You Won The Round!#Round %i: Get Ready!", game_cur_round + 1 );
                        else
                            sprintf( str, "You Lost The Round!#Round %i: Get Ready!", game_cur_round + 1 );
                        if ( !display_info( font, str, game_peer ) )
                            leave = 1;
                    }
                    break;
                case REQUEST_QUIT:
                    net_write_empty_msg( game_peer, MSG_GAME_EXITED );
                    leave = 1;
                    client_error[0] = 0;
                    break;
                case REMOTE_QUIT:
                    confirm( font, "Remote player has left the game", CONFIRM_ANY_KEY );
                    leave = 1;
                    client_error[0] = 0;
                    break;
                case NET_ERROR:
                    confirm( font, client_error, CONFIRM_ANY_KEY );
                    leave = 1;
                    break;
                case REQUEST_PAUSE:
                    if ( net_write_empty_msg( game_peer, MSG_PAUSE ) ) {
                        if ( !comm_remote_pause( font, "You paused the game", game_peer ) ) {
                            confirm( font, "Connection problems", CONFIRM_ANY_KEY );
                            leave = 1;
                        }
                        comm_reinit_connection();
                    }
                    break;
                case REMOTE_PAUSE:
                    if ( !comm_remote_pause( font, "Remote player has paused the game", game_peer ) ) {
                        confirm( font, "Connection problems", CONFIRM_ANY_KEY );
                        leave = 1;
                    }
                    comm_reinit_connection();
                    break;
            }
#endif
        }
        else
        switch ( result ) {
            case REQUEST_HELP:
                help_run();
                break;
            case REQUEST_PAUSE:
                confirm( font, "Pause", CONFIRM_PAUSE );
                break;
            case REQUEST_NUKE:
                game_nuke();
                break;
            case REQUEST_QUIT:
                if ( stk_quit_request || confirm( font, "Exit Game? y/n", CONFIRM_YES_NO ) )
                    leave = 1;
                break;
            case REQUEST_WARP:
                if ( brick_count > warp_limit )
                    break;
                if ( !confirm( font, "Warp To Next Level? y/n", CONFIRM_YES_NO ) )
                    break;
            case REQUEST_NEXT_LEVEL:
#ifdef AUDIO_ENABLED
                if ( config.speech ) {
                    if ( rand() % 2 )
                        stk_sound_play( wav_excellent );
                    else
                        stk_sound_play( wav_verygood );
                }
#endif
                player->level = levels_get_next( player->level );
                if ( player->level ) player->level_id++;
                player_reset_bricks( player );
                if ( !player->level ) {
                    confirm( font, "Congratulations! No more levels left!", CONFIRM_ANY_KEY );
                    /* player finished all levels so ''deactivate'' him */
                    player->lives = 0;
                    /* if this was the last player show congrats */
                    if ( players_count() == 0 ) {
                        /* congrats */
                        final_info();
                        leave = 1;
                        break;
                    }
                    /*
                    else {
                        player = players_get_next();
                        next_player = 1;
                    }
                    */
                }
                /* switch player in any case after level was finished so the other 
                   players don't have to wait too long */
                if ( player_count > 1 ) {
                    player = players_get_next();
                    next_player = 1;
                }
                if ( next_player ) {
                    fade_anims();
                    game_init_level( player, WITH_CREDITS );
                    player_info();
                    next_player = 0;
                }
                else {
                    stk_display_fade( STK_FADE_OUT, STK_FADE_DEFAULT_TIME );
                    game_init_level( player, WITH_CREDITS );
                    stk_display_fade( STK_FADE_IN, STK_FADE_DEFAULT_TIME );
                }
                break;
            case REQUEST_RESTART:
                /* if there is only one life left this is a continue request */
                if ( player->lives == 1 ) {
                    if ( confirm( font , "Buy a continue? (Costs 50% score!) y/n", CONFIRM_YES_NO ) ) {
                        player->score /= 2;
                        player->lives += game_diff->lives;
                    }
                    else
                        break;
                }
                else
                    if ( !confirm( font, "Restart Level? y/n", CONFIRM_YES_NO ) ) 
                        break;
                player_reset_bricks( player );
                player->lives--;
            case REQUEST_NEXT_PLAYER:
                /* if we got here the current player lost a life */
#ifdef AUDIO_ENABLED
                if ( result == REQUEST_NEXT_PLAYER && config.speech ) {
                    if ( rand() % 2 )
                        stk_sound_play( wav_damn );
                    else
                        stk_sound_play( wav_dammit );
                }
#endif
                /* if he has no lives left but enough score to buy one he may do so
                else he will be kicked out of the game as lives == 0 */
                if ( player->lives == 0 ) {
                    if ( confirm( font, "Buy a continue? (Costs 50% score!) y/n", CONFIRM_YES_NO ) ) {
                        player->score /= 2;
                        player->lives += game_diff->lives;
#ifdef AUDIO_ENABLED
                        if ( config.speech )
                            stk_sound_play( wav_wontgiveup );
#endif
                    }
                }
                if ( players_count() >= 1 )
                    player = players_get_next();
                else {
                    /* all players died - quit game */
                    leave = 1;
                    confirm( font, "Game Over!", CONFIRM_ANY_KEY );
                    break;
                }
                fade_anims();
                game_init_level( player, WITH_CREDITS );
                player_info();
                break;
        }
    }
    stk_display_fade( STK_FADE_OUT, STK_FADE_DEFAULT_TIME );
    /* add players to chart -- only local game */
    if ( game_type == GT_LOCAL ) {
        chart_clear_new_entries();
        for ( i = 0; i < config.player_count; i++ )
            chart_add( chart_set_query( levelset_name), players[i].name, players[i].level_id + 1, players[i].score );
        /* and save... maybe it crashes and we don't want to loose highscore results, right? */
        chart_save();
    }
}

/*
====================================================================
Test this level until ball gets lost.
====================================================================
*/
void game_test_level( Level *level )
{
    Player test_player;
    /* initate testplayer */
    strcpy( test_player.name, "Player" );
    test_player.score = 0;
    test_player.lives = 1;
    test_player.level = level;
    test_player.level->bkgnd_id = 1;
    player_reset_bricks( &test_player );
    player = &test_player;
    /* difficulty */
    game_diff = diff_get( config.diff );
    /* init game */
    game_init_level( player, 0 /* no credits */ );
    /* run */
    stk_display_fade( STK_FADE_IN, STK_FADE_DEFAULT_TIME );
    breakout_run();
    stk_display_fade( STK_FADE_OUT, STK_FADE_DEFAULT_TIME );
}

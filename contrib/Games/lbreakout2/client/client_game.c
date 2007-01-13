/***************************************************************************
                          client_game.c  -  description
                             -------------------
    begin                : Sat Oct 26 12:02:57 CEST 2002
    copyright            : (C) 2002 by Michael Speck
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

#include <time.h>
#include "lbreakout.h"
#include "config.h"
#include "event.h"
#include "../gui/gui.h"
#include "client_data.h"
#include "levels.h"
#include "player.h"
#include "paddle.h"
#include "game.h"

/*
====================================================================
Externals
====================================================================
*/
extern Config config;
extern char client_error[128];
extern SDL_Surface *stk_display;
#ifdef NETWORK_ENABLED
extern Net_Socket *client;
#endif
extern GuiWidget *dlg_chatroom;
extern GuiWidget *dlg_stats;
extern GuiWidget *label_winner;
extern GuiWidget *label_stats;
extern StkFont *chat_font_error;
extern StkFont *chat_font_normal;
extern char client_ip[NET_IP_SIZE];
extern char client_name[16];
extern int client_state;
/* CHALLENGE */
extern char *mp_diff_names[];
extern char mp_levelset[16];
extern int mp_peer_id;
extern char mp_peer_name[16];
extern int mp_levelset_version;
extern int mp_levelset_update;
extern int mp_level_count;
extern int mp_diff, mp_rounds, mp_frags, mp_balls;
extern Paddle *l_paddle, *r_paddle;

#ifdef NETWORK_ENABLED
/*
====================================================================
Update statistics dialogue as host and send client the 
stat results.
====================================================================
*/
static void client_update_stats( void )
{
    Net_Msg msg;
    int l_kept = 0, r_kept = 0;
    int l_bricks = 0, r_bricks = 0;
    int l_extras = 0, r_extras = 0;
    int result;
    /* result 0:draw 1:host 2:guest */
    if ( l_paddle->player->wins == r_paddle->player->wins ) {
        gui_label_set_text( label_winner, "Result: DRAW" );
        result = 0;
    }
    else
        if ( l_paddle->player->wins > r_paddle->player->wins ) {
            gui_label_set_text( label_winner, "Result: VICTORY" );
            result = 1;
        }
        else {
            gui_label_set_text( label_winner, "Result: DEFEAT" );
            result = 2;
        }
    /* stats */
    if ( l_paddle->player->hits + r_paddle->player->frags_total > 0 )
        l_kept = 100 * l_paddle->player->hits / (l_paddle->player->hits + r_paddle->player->frags_total);
    if ( r_paddle->player->hits + l_paddle->player->frags_total > 0 )
        r_kept = 100 * r_paddle->player->hits / (r_paddle->player->hits + l_paddle->player->frags_total);
    if ( l_paddle->player->bricks_total > 0 )
        l_bricks = 100 * l_paddle->player->bricks_removed / l_paddle->player->bricks_total;
    if ( r_paddle->player->bricks_total > 0 )
        r_bricks = 100 * r_paddle->player->bricks_removed / r_paddle->player->bricks_total;
    if ( l_paddle->player->extras_total > 0 )
        l_extras = 100 * l_paddle->player->extras_collected / l_paddle->player->extras_total;
    if ( r_paddle->player->extras_total > 0 )
        r_extras = 100 * r_paddle->player->extras_collected / r_paddle->player->extras_total;
    /* we are host and the other player needs these stats too */
    net_init_msg( &msg, MSG_GAME_STATS );
    if ( net_pack_int16( &msg, mp_peer_id ) )
    if ( net_pack_int8( &msg, result ) )
    if ( net_pack_int8( &msg,l_paddle->player->wins ) )
    if ( net_pack_int8( &msg,r_paddle->player->wins ) )
    if ( net_pack_int8( &msg,l_paddle->player->losses ) )
    if ( net_pack_int8( &msg,r_paddle->player->losses ) )
    if ( net_pack_int16( &msg,l_paddle->player->frags_total ) )
    if ( net_pack_int16( &msg,r_paddle->player->frags_total ) )
    if ( net_pack_int8( &msg,l_kept ) )
    if ( net_pack_int8( &msg,r_kept ) )
    if ( net_pack_int32( &msg,l_paddle->player->score ) )
    if ( net_pack_int32( &msg,r_paddle->player->score ) )
    if ( net_pack_int8( &msg,l_bricks ) )
    if ( net_pack_int8( &msg,r_bricks ) )
    if ( net_pack_int8( &msg,l_extras ) )
    if ( net_pack_int8( &msg,r_extras ) )
        net_write_msg( client, &msg );
    /* string */
    gui_label_set_text( label_stats,
        "                  %12s %12s##" \
        "Wins:             %12i %12i#" \
        "Losses:           %12i %12i#" \
        "--- DEATHMATCH ---#" \
        "Frags:            %12i %12i#" \
        "Balls Kept:       %11i%% %11i%%#" \
        "--- NORMAL ---#" \
        "Total Score:      %12i %12i#" \
        "Bricks Cleared:   %11i%% %11i%%#" \
        "Extras Collected: %11i%% %11i%%",
        client_name, mp_peer_name,
        l_paddle->player->wins, r_paddle->player->wins,
        l_paddle->player->losses, r_paddle->player->losses,
        l_paddle->player->frags_total, r_paddle->player->frags_total,
        l_kept, r_kept,
        l_paddle->player->score, r_paddle->player->score,
        l_bricks, r_bricks, l_extras, r_extras );
}

/*
====================================================================
Run some tests to benchmark the speed of your machine.
Return the time in milliseconds the benchmark took.
====================================================================
*/
static int client_benchmark( void )
{
    SDL_Surface *surf = 
        stk_surface_create( SDL_SWSURFACE, 640, 480 );
    int i, start;
    double var = 15364.32498745;
    double var2 = 278834.3121398243;
    start = SDL_GetTicks();
    /* some floating point operations */
    for ( i = 0; i < 100000; i++ )
        var = log(  exp( var ) );
    for ( i = 0; i < 100000; i++ ) {
        var *= var2;
        var /= var2;
    }
    /* blitting stuff */
    for ( i = 0; i < 10; i++ ) {
        stk_surface_blit( stk_display, 0,0,-1,-1, surf, 0,0 );
        stk_surface_alpha_blit( surf, 0,0,-1,-1, stk_display, 0,0, 192 );
        stk_surface_blit( surf, 0,0,-1,-1, stk_display, 0,0 );
    }
    /* stop! */
    SDL_FreeSurface( surf );
    return SDL_GetTicks() - start;
}
#endif

/*
====================================================================
Establish a direct connection and run the game.
Return False if there was an error.
====================================================================
*/
void client_run_game( int challenger )
{
#ifdef NETWORK_ENABLED    
    char buf[128], ip_name[NET_IP_SIZE];
    int port, ip_recv = 0;
    Net_Msg msg;
    Net_Socket *gserver = 0, *endpoint = 0;
    int start_time;
    int host = 0; /* the host is the faster machine that
                     also builds the frames for the client */
    int my_result, remote_result; /* benchmark results */
    int bench_recv = 0, dummy;
    int game_port = config.client_game_port;
    stk_display_fade( STK_FADE_OUT, STK_FADE_DEFAULT_TIME );
    /* clear error */
    client_error[0] = 0;
    /* info */
    if ( challenger )
        sprintf( buf, "Waiting for remote player..." );
    else
        sprintf( buf, "Connecting to remote player..." );
    chat_font_normal->align = STK_FONT_ALIGN_CENTER_X | STK_FONT_ALIGN_CENTER_Y;
    stk_font_write( chat_font_normal, stk_display, 
        stk_display->w/2, stk_display->h/2, STK_OPAQUE, buf );
    stk_display_update( STK_UPDATE_ALL );
    /* make the connection */
    if ( challenger ) {
        /* the challenger opens a game server */
        for ( port = game_port; port < game_port + 10; port++ )
            if ( ( gserver = net_open( port, 0 ) ) )
                break;
        if ( gserver == 0 )
            sprintf( client_error, "%s", net_get_error() );
        else {
            /* send port and wait for answer */
            net_init_msg( &msg, MSG_IP_ADDRESS );
            if ( !net_pack_int16( &msg, mp_peer_id ) ||
                 !net_pack_string( &msg, client_ip ) ||
                 !net_pack_int32( &msg, port ) ||
                 !net_write_msg( client, &msg ) )
                sprintf( client_error, "%s", net_get_error() );
            else {
                start_time = time( 0 );
                while ( time( 0 ) <= start_time + 8 )
                    if ( ( endpoint = net_accept( gserver ) ) )
                        break;
                if ( endpoint == 0 )
                    sprintf( client_error, "Remote player did not respond" );
            }
        }
    }
    else {
        /* the client receives the port and connects to it */
        if ( net_read_msg( client, &msg, 8000 ) )
            if ( msg.type == MSG_IP_ADDRESS ) {
                net_unpack_int16( &msg, &dummy ); /* this user's id */
                net_unpack_string( &msg, ip_name, NET_IP_SIZE );
                net_unpack_int32( &msg, &port );
#ifdef NET_INFO
                printf( "IP received: %s\n", ip_name );
                //strcpy( ip_name, "80.6.55.46" );
                printf( "IP forced: %s:%i\n", ip_name, port );
#endif
                ip_recv = 1;
            }
        if ( ip_recv ) {
            if ( ( endpoint = net_connect( ip_name, port ) ) == 0 )
                sprintf( client_error, net_get_error() );
        }
        else
            if ( client_error[0] == 0 )
                sprintf( client_error, "Challenger didn't send ip address (timeout)" );
    }
    /* check who's faster */
    if ( client_error[0] == 0 ) {
        my_result = client_benchmark();
        net_init_msg( &msg, MSG_BENCHMARK );
        if ( !net_pack_int16( &msg, mp_peer_id ) ||
             !net_pack_int16( &msg, my_result ) ||
             !net_write_msg( endpoint, &msg ) )
            fprintf( stderr, "%s\n", net_get_error() );
        if ( net_read_msg( endpoint, &msg, 8000 ) )
            if ( msg.type == MSG_BENCHMARK )
                if ( net_unpack_int16( &msg, &dummy ) )
                if ( net_unpack_int16( &msg, &remote_result ) )
                    bench_recv = 1;
        if ( bench_recv ) {
            /* the one with less time wins: equal time decides 
               for the challenger */
            if ( my_result == remote_result )
                host = challenger;
            else
                if ( my_result < remote_result )
                    host = 1;
                else
                    host = 0;
            printf( "Local: %i, Remote: %i, Host: %i\n", 
                    my_result, remote_result, host );
        }
        else
            if ( client_error[0] == 0 )
                sprintf( client_error, "Remote player didn't send benchmark result (timeout)" );
    }
    /* run game if no error */
    if ( client_error[0] == 0 ) {
        SDL_SetEventFilter( event_filter );
        if ( game_init( mp_levelset, mp_diff, endpoint, challenger, host, mp_rounds, mp_frags, mp_balls ) ) {
            game_run();
            /* before we clear the game data 
               we set the statistics label and 
               send this data to our opponent */
            if ( host ) client_update_stats();
        }
        SDL_SetEventFilter( 0 );
        game_clear();
        net_close( endpoint );
        if ( challenger )
            net_close( gserver );
    }
    if ( client_error[0] != 0 ) {
        /* show error */
        SDL_FillRect( stk_display, 0, 0x0 );
        chat_font_error->align = STK_FONT_ALIGN_CENTER_X | STK_FONT_ALIGN_CENTER_Y;
        stk_font_write( chat_font_error, stk_display, 
            stk_display->w/2, stk_display->h/2, STK_OPAQUE, client_error );
        stk_display_update( STK_UPDATE_ALL );
        event_clear_sdl_queue();
        stk_wait_for_input();
    }
    /* update client */
    net_write_empty_msg( client, MSG_GAME_EXITED );
    client_data_clear();
    gui_widget_draw( dlg_chatroom );
    if ( client_error[0] == 0 ) {
        /* everything was okay so display stats */
        gui_widget_show( dlg_stats );
        client_state = CLIENT_STATS;
    }
    else
        client_state = CLIENT_NONE;
    stk_display_fade( STK_FADE_IN, STK_FADE_DEFAULT_TIME );
#endif
}

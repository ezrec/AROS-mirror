/***************************************************************************
                          client_recv.c  -  description
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
 
#include <stdarg.h>
#include "lbreakout.h"
#include "levels.h"
#include "client_data.h"
#include "../gui/gui.h"

/*
====================================================================
Externals
====================================================================
*/
#ifdef NETWORK_ENABLED
extern Net_Socket *client;
#endif
extern GuiWidget *dlg_info;
extern GuiWidget *dlg_confirm;
extern GuiWidget *dlg_chatroom;
extern GuiWidget *label_channel;
extern GuiWidget *label_info;
extern GuiWidget *label_confirm;
extern GuiWidget *label_stats;
extern GuiWidget *label_winner;
extern GuiWidget *list_chatter;
extern GuiWidget *list_levels;
extern GuiWidget *list_users;
extern GuiWidget *list_games;
extern GuiWidget *list_channels;
extern List *client_users;
extern List *client_games;
extern List *client_channels;
extern int client_state;
extern List *levels;
/* CHALLENGE */
extern char client_name[16];
extern char *mp_diff_names[];
extern char mp_levelset[16];
extern int mp_peer_id;
extern char mp_peer_name[16];
extern int mp_levelset_version;
extern int mp_levelset_update;
extern int mp_level_count;
extern int levelset_version, levelset_update;
extern int mp_diff, mp_rounds, mp_frags, mp_balls;
extern ClientUser *client_user;
extern ClientChannel *client_channel;
extern void client_run_game( int challenger );

/*
====================================================================
Popup info dialogue and set status to INFO.
====================================================================
*/
void client_popup_info( char *format, ... )
{
    char buffer[256];
    va_list args;
    va_start( args, format );
    vsnprintf( buffer, 256, format, args );
    va_end( args );
    gui_label_set_text( label_info, buffer );
    gui_widget_show( dlg_info );
    client_state = CLIENT_INFO;
}
/*
====================================================================
Popup confirm dialogue and _keep_ status.
====================================================================
*/
void client_popup_confirm( char *format, ... )
{
    char buffer[256];
    va_list args;
    va_start( args, format );
    vsnprintf( buffer, 256, format, args );
    va_end( args );
    gui_label_set_text( label_confirm, buffer );
    gui_widget_show( dlg_confirm );
}
    
/*
====================================================================
Receive messages from server.
====================================================================
*/
void client_recv( GuiWidget *widget, GuiEvent *event )
{
#ifdef NETWORK_ENABLED
    static char buf[NET_MSG_DATA_LIMIT + 1];
    static Net_Msg msg;
    int num, length, win;
    int stats[2][7];
    char name[16], host[16], guest[16];
    ClientUser *user;
    Level *level;
    ClientChannel *channel;
    
    if ( event->type != GUI_TIME_PASSED ) return;
        
    if ( net_read_msg( client, &msg, 0 ) )
    switch ( msg.type ) {
        case MSG_PREPARE_UPDATE:
            client_data_clear();
            break;
        case MSG_ERROR:
            if ( net_unpack_string( 
                     &msg, buf, NET_MSG_DATA_LIMIT + 1 ) ) {
                gui_label_set_text( label_info, buf );
                gui_widget_show( dlg_info );
            }
            break;
        case MSG_BUSY:
            if ( client_state == CLIENT_AWAIT_ANSWER ||
                 client_state == CLIENT_AWAIT_TRANSFER_CONFIRMATION )
                client_popup_info( "%s is busy at the moment.", 
                    mp_peer_name );
            break;
        case MSG_DISCONNECT:
            gui_widget_hide( dlg_chatroom );
            break;
        /* chatter */
        case MSG_SERVER_INFO:
            if ( net_unpack_string( 
                     &msg, buf, NET_MSG_DATA_LIMIT + 1 ) )
                client_add_chatter( buf, 1 );
            break;
        case MSG_CHAT:
            if ( net_unpack_string( 
                     &msg, buf, CHAT_MSG_LIMIT + 16 ) )
                client_add_chatter( buf, 0 );
            break;
        /* users */
        case MSG_ADD_USER:
            if ( net_unpack_int16( &msg, &num ) )
            if ( net_unpack_string( &msg, name, 16 ) ) {
                client_add_user( num, name );
                gui_list_update( list_users, 
                    client_users->count );
                /* re-select current entry */
                if ( client_user ) {
                    num = list_check( client_users, client_user );
                    if ( num != -1 )
                        gui_list_select( list_users, 0, num, 1 );
                }
            }
            break;
        case MSG_REMOVE_USER:
            if ( net_unpack_int16( &msg, &num ) ) {
                client_remove_user( num );
                gui_list_update( list_users, 
                    client_users->count );
                /* re-select current entry */
                if ( client_user ) {
                    num = list_check( client_users, client_user );
                    if ( num != -1 )
                        gui_list_select( list_users, 0, num, 1 );
                }
            }
            break;
        /* games */
        case MSG_ADD_GAME:
            if ( net_unpack_int16( &msg, &num ) )
            if ( net_unpack_string( &msg, name, 16 ) )
            if ( net_unpack_string( &msg, host, 16 ) )
            if ( net_unpack_string( &msg, guest, 16 ) ) {
                client_add_game( num, name, host, guest );
                gui_list_update( list_games, client_games->count );
            }
            break;
        case MSG_REMOVE_GAME:
            if ( net_unpack_int16( &msg, &num ) ) {
                client_remove_game( num );
                gui_list_update( list_games, client_games->count );
            }
            break;
        case MSG_UPDATE_GAME:
            if ( net_unpack_int16( &msg, &num ) )
            if ( net_unpack_int8( &msg, &win ) ) {
                client_update_game( num, win  );
                gui_list_update( list_games, client_games->count );
            }
            break;
        /* channels */
        case MSG_ADD_CHANNEL:
            if ( net_unpack_int16( &msg, &num ) )
            if ( net_unpack_string( &msg, name, 16 ) ) {
                client_add_channel( num, name );
                if ( client_state == CLIENT_SELECT_CHANNEL ) {
                    gui_list_update( list_channels,
                        client_channels->count );
                    /* re-select current entry */
                    if ( client_channel ) {
                        num = list_check( client_channels, 
                            client_channel );
                        if ( num != -1 )
                            gui_list_select( 
                                list_channels, 0, num, 1 );
                    }
                }
            }
            break;
        case MSG_REMOVE_CHANNEL:
            if ( net_unpack_int16( &msg, &num ) ) {
                client_remove_channel( num );
                if ( client_state == CLIENT_SELECT_CHANNEL ) {
                    gui_list_update( list_channels,
                        client_channels->count );
                    /* re-select current entry */
                    if ( client_channel ) {
                        num = list_check( client_channels, 
                            client_channel );
                        if ( num != -1 )
                            gui_list_select( 
                                list_channels, 0, num, 1 );
                    }
                }
            }
            break;
        case MSG_SET_CHANNEL:
            /* we only need to update the name */
            if ( net_unpack_int16( &msg, &num ) )
            if ( ( channel = client_find_channel( num ) ) )
                gui_label_set_text( label_channel, channel->name );
            break;
        /* challenge */
        case MSG_CHALLENGE:
            /* the user may only be challenged if client state is NONE
               because otherwise he is doing something that shouldn't be
               interrupted */
            if ( client_state != CLIENT_NONE ) {
                if ( net_unpack_int16( &msg, &num ) ) /* challenger */
                if ( net_build_msg( &msg, MSG_BUSY, num ) )
                    net_write_msg( client, &msg );
                break;
            }
            if ( net_unpack_int16( &msg, &mp_peer_id ) ) /* challenger */
            if ( net_unpack_int16( &msg, &num ) ) /* this client (challenged) */
            if ( net_unpack_string( &msg, mp_levelset, 16 ) )
            if ( net_unpack_int8( &msg, &mp_diff ) )
            if ( net_unpack_int8( &msg, &mp_rounds ) )
            if ( net_unpack_int8( &msg, &mp_frags ) )
            if ( net_unpack_int8( &msg, &mp_balls ) )
            if ( ( user = client_find_user( mp_peer_id ) ) ) {
                strcpy_lt( mp_peer_name, user->name, 15 );
                client_popup_confirm( "    You have been challenged!##"\
                    "    Challenger: %13s#"\
                    "    Levelset:   %13s#"\
                    "    Difficulty: %13s#"\
                    "    Rounds:     %13i#"\
                    "    Frag Limit: %13i#"\
                    "    Balls:      %13i",
                    mp_peer_name, mp_levelset, mp_diff_names[mp_diff],
                    mp_rounds, mp_frags, mp_balls );
                client_state = CLIENT_ANSWER;
            }
            break;
        case MSG_REJECT_CHALLENGE:
            if ( client_state == CLIENT_AWAIT_ANSWER ) {
                client_popup_info( 
                    "%s is too scared to accept your challenge.", 
                    mp_peer_name );
            }
            break;
        case MSG_CANCEL_CHALLENGE:
            if ( client_state == CLIENT_ANSWER ) {
                gui_widget_hide( dlg_confirm );
                client_popup_info( "%s got cold feet.", 
                    mp_peer_name );
            }
            break;
        case MSG_ACCEPT_CHALLENGE:
            if ( client_state == CLIENT_AWAIT_ANSWER ) {
                gui_widget_hide( dlg_info );
                client_state = CLIENT_PLAY;
                client_run_game( 1 );
            }
            break;
        /* transfer */
       case MSG_INIT_TRANSFER:
            /* the user may only be challenged if client state is LISTEN
               because otherwise he is doing something that shouldn't be
               interrupted */
            if ( client_state != CLIENT_LISTEN ) {
                if ( net_unpack_int16( &msg, &num ) ) /* sender */
                if ( net_build_msg( &msg, MSG_BUSY, num ) )
                    net_write_msg( client, &msg );
                break;
            }
           if ( net_unpack_int16( &msg, &mp_peer_id ) ) /* sender */
           if ( net_unpack_string( &msg, mp_levelset, 16 ) ) /* set name */
           if ( net_unpack_int8( &msg, &mp_levelset_version ) )
           if ( net_unpack_int8( &msg, &mp_levelset_update ) ) /* version */
           if ( net_unpack_int8( &msg, &mp_level_count ) )
           if ( ( user = client_find_user( mp_peer_id ) ) ) { /* level count */
               strcpy_lt( mp_peer_name, user->name, 15 );
               /* test if we got a newer version of this set */
               sprintf( name, "~%s", mp_levelset );
               if ( levels_load( name ) ) {
                   if ( levelset_version > mp_levelset_version ||
                        ( levelset_version == mp_levelset_version &&
                          levelset_update >= mp_levelset_update ) ) {
                       if ( !net_build_msg( &msg, MSG_OLDER_SET, mp_peer_id ) ||
                            !net_write_msg( client, &msg ) )
                           fprintf( stderr, "%s\n", net_get_error() );
                       break;
                   }
               }
               gui_widget_hide( dlg_info );
               if ( !net_write_empty_msg( client, MSG_CLOSE_TRANSFER ) )
                   fprintf( stderr, "%s\n", net_get_error() );
               client_popup_confirm( 
                   "%s wants to send you the levelset %s "\
                   "v%i.%02i (%i levels). Do you accept?", 
                   mp_peer_name, mp_levelset, mp_levelset_version, 
                   mp_levelset_update, mp_level_count );
               client_state = CLIENT_CONFIRM_TRANSFER;
           }
           break;
       case MSG_OLDER_SET:
            if ( client_state == CLIENT_AWAIT_TRANSFER_CONFIRMATION ) {
                client_popup_info( 
                    "%s's levelset is up-to-date.", mp_peer_name );
            }
           break;
       case MSG_CANCEL_TRANSFER:
            if ( client_state == CLIENT_CONFIRM_TRANSFER ) {
                gui_widget_hide( dlg_confirm );
                client_popup_info( 
                    "%s cancelled the transfer.", mp_peer_name );
            }
           break;
       case MSG_REJECT_SET:
            if ( client_state == CLIENT_AWAIT_TRANSFER_CONFIRMATION ) {
                client_popup_info( 
                    "%s rejected your levelset.", mp_peer_name );
            }
           break;
       case MSG_ACCEPT_SET:
            if ( client_state == CLIENT_AWAIT_TRANSFER_CONFIRMATION ) {
                /* send all levels */
                list_reset( levels );
                while ( ( level = list_next( levels ) ) ) {
                    net_init_msg( &msg, MSG_TRANSFER_DATA );
                    if ( !net_pack_int16( &msg, mp_peer_id ) ||
                         !net_pack_string( &msg, level->author ) ||
                         !net_pack_string( &msg, level->name ) ||
                         !net_pack_raw( &msg, level->bricks, sizeof( level->bricks ) ) ||
                         !net_pack_raw( &msg, level->extras, sizeof( level->extras ) ) ||
                         !net_write_msg( client, &msg ) ) {
                        fprintf( stderr, "%s\n", net_get_error() );
                        break;
                    }
                }
                client_popup_info( "Levelset sent to %s.", mp_peer_name );
            }
            break;
        case MSG_TRANSFER_DATA:
            if ( client_state == CLIENT_RECEIVE ) {
                if ( ( level = list_current( levels ) ) == 0 ) {
                    client_popup_info( "Levelset incomplete!" );
                    break;
                }
                if ( !net_unpack_int16( &msg, &num ) /* this user */ ||
                     !net_unpack_string( &msg, level->author, 31 ) ||
                     !net_unpack_string( &msg, level->name, 31 ) ||
                     !net_unpack_raw( &msg, level->bricks, &length, sizeof( level->bricks ) ) ||
                     !net_unpack_raw( &msg, level->extras, &length, sizeof( level->extras ) ) ) {
                    client_popup_info( "%s\n", net_get_error() );
                    break;
                }
                else {
                    if ( ( level = list_next( levels ) ) == 0 ) {
                        /* save to disk */
                        levelset_version = mp_levelset_version;
                        levelset_update = mp_levelset_update;
                        levels_save( mp_levelset );
                        client_popup_info( "Levelset saved as ~%s", 
                            mp_levelset );
                        levelsets_load_names();
                        client_update_levelsets(); 
                    }
                }
            }
            break;
        /* game stats */
        case MSG_GAME_STATS:
            memset( stats, 0, sizeof( stats ) );
            /* stats[0] is remote host and stats[1] is us */
            net_unpack_int16( &msg, &num ); /* this users id */
            net_unpack_int8( &msg, &win );
            net_unpack_int8( &msg, &stats[0][0] );
            net_unpack_int8( &msg, &stats[1][0] );
            net_unpack_int8( &msg, &stats[0][1] );
            net_unpack_int8( &msg, &stats[1][1] );
            net_unpack_int16( &msg, &stats[0][2] );
            net_unpack_int16( &msg, &stats[1][2] );
            net_unpack_int8( &msg, &stats[0][3] );
            net_unpack_int8( &msg, &stats[1][3] );
            net_unpack_int32( &msg, &stats[0][4] );
            net_unpack_int32( &msg, &stats[1][4] );
            net_unpack_int8( &msg, &stats[0][5] );
            net_unpack_int8( &msg, &stats[1][5] );
            net_unpack_int8( &msg, &stats[0][6] );
            net_unpack_int8( &msg, &stats[1][6] );
            /* build winner string */
            if ( win == 0 )
                gui_label_set_text( label_winner,
                    "Result: DRAW" );
            else
                if ( win == 1 ) 
                    gui_label_set_text( label_winner,
                        "Result: DEFEAT" );
                else
                    gui_label_set_text( label_winner,
                        "Result: VICTORY" );
            /* build stats string */
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
                stats[1][0], stats[0][0],
                stats[1][1], stats[0][1],
                stats[1][2], stats[0][2],
                stats[1][3], stats[0][3],
                stats[1][4], stats[0][4],
                stats[1][5], stats[0][5],
                stats[1][6], stats[0][6] );
            break;
    }
#endif
}

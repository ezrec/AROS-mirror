/***************************************************************************
                          main.c  -  description
                             -------------------
    begin                : Sat Apr 27 12:02:57 CEST 2001
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

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include "../common/net.h"
#include "../common/messages.h"
#include "../common/list.h"
#include "../common/tools.h"
#include "../common/parser.h"
#include "server.h"

#ifdef NETWORK_ENABLED

/*
====================================================================
This server does not handle the actual game between two partners
but handles user login, chat and grouping of users.
====================================================================
*/

/*
====================================================================
Server data.
====================================================================
*/
Net_Socket *server = 0; /* server socket */
Uint16 user_id = 1; /* unique user id assigned to a user on connection */
Uint16 game_id = 1; /* unique game id assigned every challenge and
                    used when MSG_ACCEPT is received */
Uint16 channel_id = 1; /* unique id for channels */
List *users = 0; /* all users logged in. just pointers to the users handled
                    by each channel. */
List *channels = 0; /* channels containing users and games */
int server_halt_start = 0; /* time when halt command came in to delay
                              halt for 5 seconds */
int server_halting = 0; /* if True server will go down after some seconds */

/* config stuff that may overwritten by command line arguments */
/* port the server is running at */
int server_port = 2002;
/* max number of users */
int user_limit = 30; 
/* after sending no messages for this amount of seconds a user is kicked */
int user_idle_limit = 1200; 
/* welcome message send to user when logging in */
char welcome_msg[512] = "Welcome to LBreakout2 online, enjoy the game!";
/* a user with this login name will become admin */
char admin_pwd[16] = "";

/*
====================================================================
Forwarded declarations.
====================================================================
*/
void server_send_update( SUser *target );
void remove_user_callback( void *ptr );

/*
====================================================================
Error messages.
====================================================================
*/
void server_error_bad_pack( SUser *user )
{
    fprintf( stderr, "package received from '%s' corrupted\n", user->name );
}
void server_error_empty_pack( SUser *user )
{
    fprintf( stderr, "package received from '%s' is empty?\n", user->name );
}
void server_send_error( Net_Socket *socket, char *error )
{
    Net_Msg msg;
    if ( net_build_msg( &msg, MSG_ERROR, msg ) )
        net_write_msg( socket, &msg );
}

/*
====================================================================
Broadcast a package to all not playing users. 
Do not use the list_next() function without backup as the internal 
pointer may be in use by server_handle().
====================================================================
*/
void server_broadcast_all( Net_Msg *msg )
{
    ListEntry *cur_entry = users->cur_entry;
    SUser* user = 0;
    list_reset( users );
    while ( ( user = list_next( users ) ) )
        if ( !user->playing && user->ready )
            net_write_msg( user->socket, msg );
    users->cur_entry = cur_entry;
}

/*
====================================================================
Send a server info.
====================================================================
*/
void server_send_info( char *text )
{
    Net_Msg msg;
    if ( net_build_msg( &msg, MSG_SERVER_INFO, text ) )
        server_broadcast_all( &msg );
}

/*
====================================================================
Channels by that name are default channels and not deleted when
empty.
====================================================================
*/
int server_is_default_channel( SChannel *channel )
{
    int ret = 0;
    if ( STRCMP( channel->name, "MAIN" ) ) ret = 1;
    return ret;
}

/*
====================================================================
If not found add a new channel by that name.
Return pointer to channel.
Send nescessary updates.
====================================================================
*/
SChannel* server_add_channel( char *name )
{
    Net_Msg msg;
    SChannel *channel = 0;
    list_reset( channels );
    while ( ( channel = list_next( channels ) ) )
        if ( STRCMP( channel->name, name ) )
            return channel;
    if ( ( channel = calloc( 1, sizeof( SChannel ) ) ) ) {
        channel->id = channel_id++;
        strcpy_lt( channel->name, name, 15 );
        printf( "channel '%s' (%i) added\n", channel->name, channel->id );
        channel->users = list_create( LIST_AUTO_DELETE, remove_user_callback );
        channel->games = list_create( LIST_AUTO_DELETE, LIST_NO_CALLBACK );
        list_add( channels, channel );
        /* a new channel is posted to all not-playing users */
        if ( net_build_msg( &msg, MSG_ADD_CHANNEL, channel->id, channel->name ) )
            server_broadcast_all( &msg );
        return channel;
    }
    return 0;
}

/*
====================================================================
Remove a channel.
Send nescessary updates.
====================================================================
*/
void remove_channel_callback( void *ptr )
{
    SChannel *channel = (SChannel*)ptr;
    if ( channel ) {
        if ( channel->users )
            list_delete( channel->users );
        if ( channel->games )
            list_delete( channel->games );
        free( channel );
    }
}
void server_remove_channel( SChannel *channel )
{
    Net_Msg msg;
    if ( server_is_default_channel( channel ) )
        return;
    if ( net_build_msg( &msg, MSG_REMOVE_CHANNEL, channel->id ) )
        server_broadcast_all( &msg );
    printf( "channel '%s' (%i) removed\n", channel->name, channel->id );
    list_delete_item( channels, channel );
}

/*
====================================================================
Search for a channel by id.
====================================================================
*/
SChannel *server_find_channel( int id )
{
    SChannel *channel = 0;
    list_reset( channels );
    while ( ( channel = list_next( channels ) ) )
        if ( channel->id == id )
            return channel;
    return 0;
}

/*
====================================================================
Search for a channel by name.
====================================================================
*/
SChannel *server_find_channel_by_name( char *name )
{
    SChannel *channel = 0;
    list_reset( channels );
    while ( ( channel = list_next( channels ) ) )
        if ( STRCMP( channel->name, name ) )
            return channel;
    return 0;
}

/*
====================================================================
Broadcast a message to users in the same channel.
====================================================================
*/
void server_broadcast_channel( int cid, Net_Msg *msg )
{
    SUser* user = 0;
    SChannel *channel = server_find_channel( cid );
    if ( channel ) {
        list_reset( channel->users );
        while ( ( user = list_next( channel->users ) ) )
            if ( !user->playing && user->ready )
                net_write_msg( user->socket, msg );
    }
}

/*
====================================================================
Check if the user name contains at least one non-whitespace.
====================================================================
*/
int server_validate_user_name( char *name )
{
    int i;
    for ( i = 0; i < strlen( name ); i++)
        if ( name[i] > 32 )
            return 1;
    return 0;
}

/*
====================================================================
Add a new user to channel MAIN but do not update anything as the 
user is hidden.
====================================================================
*/
SUser* server_add_hidden_user( Net_Socket *socket )
{
    SChannel *channel = server_add_channel( "MAIN" );
    SUser *user = 0;
    if ( channel ) {
        user = calloc( 1, sizeof( SUser ) );
        if ( user ) {
            user->id = user_id++;
            strcpy( user->name, "" );
            user->socket = socket;
            user->idle_start_time = time( 0 );
            user->channel = channel->id;
            user->transfer_id = -1;
            list_add( channel->users, user );
            list_add( users, user );
#ifdef NET_DEBUG
            printf( "channel '%s' (%i) counts %i users\n", 
                    channel->name, channel->id, channel->users->count );
#endif
            return user;
        }
    }
    fprintf( stderr, "out of memory\n" );
    return 0;
}

/*
====================================================================
Activate the user and send all nescessary updates.
====================================================================
*/
void server_activate_user( SUser *user )
{
    Net_Msg msg;
    char text[CHAT_MSG_LIMIT + 16];
    if ( net_build_msg( &msg, MSG_LOGIN_OKAY, user->id, 
                   user->name, user->socket->remote_ip ) )
        net_write_msg( user->socket, &msg );
    printf( "player '%s' (%i) has connected from %s\n", user->name, user->id, user->socket->remote_ip );
    sprintf( text, "%s#You are logged in as: %s.", welcome_msg, user->name );
    if ( net_build_msg( &msg, MSG_SERVER_INFO, text ) )
        net_write_msg( user->socket, &msg );
    if ( net_build_msg( &msg, MSG_SERVER_INFO, 
        "Hit the help button at the lower right-hand side for information "\
        "about console commands, options, levelset transfer etc." ) )
        net_write_msg( user->socket, &msg );
    /* add user to channel */
    if ( net_build_msg( &msg, MSG_ADD_USER, user->id, user->name ) )
        server_broadcast_channel( user->channel, &msg );
    /* activate user */
    user->ready = 1;
    /* send full update about channels, users, games */
    server_send_update( user );
}

/*
====================================================================
Delete a user and send all nescessary updates
====================================================================
*/
void remove_user_callback( void *ptr )
{
    SUser *user = ptr;
    if ( user ) {
        net_close( user->socket );
        free( user );
    }
}
void server_remove_user( SUser *user )
{
    SChannel *channel = server_find_channel( user->channel );
    Net_Msg msg;
    if ( channel ) {
        if ( user->ready && !user->playing ) {
            if ( net_build_msg( &msg, MSG_REMOVE_USER, user->id ) )
                server_broadcast_channel( user->channel, &msg );
        }
        list_delete_item( users, user );
        list_delete_item( channel->users, user );
#ifdef NET_DEBUG
        printf( "channel '%s' (%i) counts %i users\n", 
                channel->name, channel->id, channel->users->count );
#endif
        if ( channel->users->count == 0 ) /* empty? */
            server_remove_channel( channel );
    }
    else {
        printf( "ERROR: can't delete user '%s' (%i): invalid channel\n", user->name, user->id );
        list_delete_item( users, user );
    }
}

/*
====================================================================
Find a user by id.
====================================================================
*/
SUser* server_find_user( id )
{
    ListEntry *cur_entry = users->cur_entry;
    SUser* user = 0;
    list_reset( users );
    while ( ( user = list_next( users ) ) )
        if ( user->id == id )
            break;
    users->cur_entry = cur_entry;
    return user;
}

/*
====================================================================
Find a user by id.
====================================================================
*/
SUser* server_find_user_by_name( char *name )
{
    ListEntry *cur_entry = users->cur_entry;
    SUser* user = 0;
    list_reset( users );
    while ( ( user = list_next( users ) ) )
        if ( STRCMP( user->name, name ) )
            break;
    users->cur_entry = cur_entry;
    return user;
}

/*
====================================================================
Kick user and send nescessary updates.
====================================================================
*/
void server_kick_user( SUser *user, char *reason )
{
    Net_Msg msg;
    char buf[CHAT_MSG_LIMIT];
    sprintf( buf, "You have been kicked! (%s)", reason );
    server_send_error( user->socket, buf );
    net_write_empty_msg( user->socket, MSG_DISCONNECT );
    printf( "player '%s' (%i) has been kicked (%s)\n", user->name, user->id, reason );
    if ( STRCMP( reason, "admin" ) ) {
        sprintf( buf, "'%s' has been kicked by admin.", user->name );
        if ( net_build_msg( &msg, MSG_SERVER_INFO, buf ) )
            server_broadcast_all( &msg );
    }
    server_remove_user( user );
}

/*
====================================================================
Switch user to new channel and remove empty channel.
Send nescessary updates.
====================================================================
*/
void server_transfer_user( SUser *user, char *name )
{
    char buf[64];
    Net_Msg msg;
    SChannel *channel, *old;
    /* get old channel of user */
    if ( ( old = server_find_channel( user->channel ) ) == 0 )
        return;
    /* same channel? */
    if ( STRCMP( old->name, name ) )
        return;
    if ( ( channel = server_add_channel( name ) ) ) {
        /* mute user as he will receive a complete update
           after the transfer */
        user->ready = 0;
        /* transfer */
        if ( net_build_msg( &msg, MSG_REMOVE_USER, user->id ) )
            server_broadcast_channel( user->channel, &msg );
        list_transfer( old->users, channel->users, user );
        if ( old->users->count == 0 ) /* empty */
            server_remove_channel( old );
        if ( net_build_msg( &msg, MSG_ADD_USER, user->id, user->name ) )
            server_broadcast_channel( channel->id, &msg );
        user->channel = channel->id;
        /* update */
        user->ready = 1;
        sprintf( buf, "You have entered channel '%s'.", name );
        if ( net_build_msg( &msg, MSG_SERVER_INFO, buf ) )
            net_write_msg( user->socket, &msg );
        server_send_update( user );
    }
}
            
/*
====================================================================
Add a game with these users as players but keep the status 'not
running'. No updates.
====================================================================
*/
void server_add_hidden_game( int id, SUser *host, SUser *guest, char *set )
{
    SChannel *channel = server_find_channel( host->channel );
    SGame *game = calloc( 1, sizeof( SGame ) );
    if ( channel && game ) {
        game->id = id;
        game->host = host;
        game->guest = guest;
        strcpy_lt( game->set, set, 15 );
        list_add( channel->games, game );
    }
}

/*
====================================================================
Find the game in which the user is either host or guest.
====================================================================
*/
SGame *server_find_game( SUser *user )
{
    SChannel *channel = server_find_channel( user->channel );
    SGame *game = 0;
    if ( channel ) {
        list_reset( channel->games );
        while ( ( game = list_next( channel->games ) ) )
            if ( game->host == user || game->guest == user )
                return game;
    }
    return 0;
}

/*
====================================================================
Remove game by checking host or guest. 
Send an update if this game was 'running'
====================================================================
*/
void server_remove_game( SUser *user )
{
    Net_Msg msg;
    SChannel *channel = server_find_channel( user->channel );
    SGame *game = server_find_game( user );
    if ( game && channel ) {
        if ( game->running ) {
            if ( net_build_msg( &msg, MSG_REMOVE_GAME, game->id ) )
                server_broadcast_channel( user->channel, &msg );
        }
        list_delete_current( channel->games );
    }
    if ( user->playing ) {
        user->playing = 0;
        if ( net_build_msg( &msg, MSG_ADD_USER, user->id, user->name ) )
            server_broadcast_channel( user->channel, &msg );
    }
}

/*
====================================================================
Set game status to 'running' and send updates.
====================================================================
*/
void server_activate_game( SUser *user )
{
    Net_Msg msg;
    SGame *game = server_find_game( user );
    if ( game ) {
        game->running = 1;
        game->host->playing = 1;
        game->guest->playing = 1;
        if ( net_build_msg( &msg, MSG_REMOVE_USER, game->host->id ) )
            server_broadcast_channel( user->channel, &msg );
        if ( net_build_msg( &msg, MSG_REMOVE_USER, game->guest->id ) )
            server_broadcast_channel( user->channel, &msg );
        if ( net_build_msg( &msg, MSG_ADD_GAME, game->id, game->set,
                 game->host->name, game->guest->name ) )
            server_broadcast_channel( user->channel, &msg );
    }
}

/*
====================================================================
Send a full update to user.
====================================================================
*/
void server_send_update( SUser *target )
{
    Net_Msg msg;
    SUser *user;
    SGame *game;
    SChannel *channel;
    net_write_empty_msg( target->socket, MSG_PREPARE_UPDATE );
    /* channels */
    list_reset( channels );
    while ( ( channel = list_next( channels ) ) ) {
        if ( net_build_msg( &msg, MSG_ADD_CHANNEL, channel->id, channel->name ) )
            net_write_msg( target->socket, &msg );
    }
    if ( ( channel = server_find_channel( target->channel ) ) ) {
        /* update channel id */
        if ( net_build_msg( &msg, MSG_SET_CHANNEL, target->channel ) )
            net_write_msg( target->socket, &msg );
        /* users in this channel */
        list_reset( channel->users );
        while ( ( user = list_next( channel->users ) ) )
            if ( user->ready && !user->playing ) {
                if ( net_build_msg( &msg, MSG_ADD_USER, user->id, user->name ) )
                    net_write_msg( target->socket, &msg );
            }
        /* games in this channel */
        list_reset( channel->games );
        while ( ( game = list_next( channel->games ) ) ) {
            if ( net_build_msg( &msg, MSG_ADD_GAME, game->id, 
                     game->set, game->host->name, game->guest->name ) )
                net_write_msg( target->socket, &msg );
        }
    }
}

/*
====================================================================
Parse and handle a command send by a user except help which is
handled by user itself.
====================================================================
*/
void server_handle_command( SUser *user, char *cmd_line )
{
    Net_Msg msg;
    ListEntry *backup;
    SUser *entry;
    SChannel *channel;
    char buf[CHAT_MSG_LIMIT + 16];
    char *ptr;
    List *args = parser_explode_string( cmd_line, ' ' );
    char *cmd = list_first( args );
    if ( STRCMP( cmd, "/search" ) ) {
        if ( ( ptr = list_next( args ) ) ) {
            backup = users->cur_entry;
            list_reset( users );
            while ( ( entry = list_next( users ) ) )
                if ( STRCMP( entry->name, ptr ) ) {
                    channel = server_find_channel( entry->channel );
                    sprintf( buf, "'%s' is %s in channel '%s'",
                                   ptr,
                                   (!entry->ready)?"logging in":(entry->playing)?"playing":"chatting",
                                   (channel)?channel->name:"???" );
                    if ( net_build_msg( &msg, MSG_SERVER_INFO, buf ) )
                        net_write_msg( user->socket, &msg );
                    break;
                }
            if ( entry == 0 ) {
                sprintf( buf, "'%s' is not online.", ptr );
                if ( net_build_msg( &msg, MSG_SERVER_INFO, buf ) )
                    net_write_msg( user->socket, &msg );
            }
            users->cur_entry = backup;
        }
    }
    else
    if ( STRCMP( cmd, "/version" ) ) {
        sprintf( buf, "Version: %s, Protocol: %i\n", VERSION, PROTOCOL );
        if ( net_build_msg( &msg, MSG_SERVER_INFO, buf ) )
            net_write_msg( user->socket, &msg );
    }
    else
    if ( STRCMP( cmd, "/userlimit" ) && STRCMP( user->name, "admin" ) ) {
        if ( ( ptr = list_next( args ) ) )
            user_limit = atoi( ptr );
        sprintf( buf, "User limit set to %i.\n", user_limit );
        if ( net_build_msg( &msg, MSG_SERVER_INFO, buf ) )
            net_write_msg( user->socket, &msg );
    }
    else
    if ( STRCMP( cmd, "/idletime" ) && STRCMP( user->name, "admin" ) ) {
        if ( ( ptr = list_next( args ) ) )
            user_idle_limit = atoi( ptr );
        sprintf( buf, "User idle limit set to %i.\n", user_idle_limit );
        if ( net_build_msg( &msg, MSG_SERVER_INFO, buf ) )
            net_write_msg( user->socket, &msg );
    }
    else
    if ( STRCMP( cmd, "/kick" ) && STRCMP( user->name, "admin" ) ) {
        if ( ( ptr = list_next( args ) ) && ( entry = server_find_user_by_name( ptr ) ) )
            server_kick_user( entry, "admin" );
    }
    else
    if ( STRCMP( cmd, "/info" ) && STRCMP( user->name, "admin" ) ) {
        sprintf( buf, "Admin: %s", cmd_line + 6 );
        if ( net_build_msg( &msg, MSG_SERVER_INFO, buf ) )
            server_broadcast_all( &msg );
    }
    else
    if ( STRCMP( cmd, "/halt" ) && STRCMP( user->name, "admin" ) ) {
        if ( net_build_msg( &msg, MSG_SERVER_INFO, "SERVER IS GOING DOWN IN 5 SECONDS!" ) )
            server_broadcast_all( &msg );
        server_halt_start = time( 0 );
        server_halting = 1;
    }
    else {
        sprintf( buf, "unknown command: %s\n", cmd );
        if ( net_build_msg( &msg, MSG_SERVER_INFO, buf ) )
            net_write_msg( user->socket, &msg );
    }
    list_delete( args );
}

/*
====================================================================
Initiate/finalize server
====================================================================
*/
int server_init()
{
    /* initialize network functions */
    net_init();
    /* open socket */
    if ( ( server = net_open( server_port, 0 ) ) == 0 ) {
        fprintf( stderr, "%s\n", net_get_error() );
        return 0;
    }
    /* info */
    printf( "LBreakout2 server %s -- by Michael Speck\n\n", VERSION );
    printf( "Bound to port %i\n", server_port );
    printf( "Server protocol: %i\n", PROTOCOL );
    printf( "Maximum number of users: %i\n", user_limit );
    printf( "User idle time: %i secs\n", user_idle_limit );
    printf( "\n" );
    /* stuff */
    users = list_create( LIST_NO_AUTO_DELETE, LIST_NO_CALLBACK );
    channels = list_create( LIST_AUTO_DELETE, remove_channel_callback );
    /* create messages */
    net_init_msgs();
    
    return 1;
}
void server_finalize( void )
{
    Net_Msg msg;
    if ( net_build_msg( &msg, MSG_ERROR, "Server halted!" ) )
        server_broadcast_all( &msg );
    if ( net_build_msg( &msg, MSG_DISCONNECT ) )
        server_broadcast_all( &msg );
    /* lists */
    if ( users ) list_delete( users ); users = 0;
    if ( channels ) list_delete( channels ); channels = 0;
    /* close server socket */
    if ( server ) net_close( server ); server = 0;
    printf( "server halted\n" );
}

/*
====================================================================
Poll and handle a message or connection request.
====================================================================
*/
void server_handle( int *leave )
{
    char name[16], set[16];
    char text[CHAT_MSG_LIMIT + 16], buf[CHAT_MSG_LIMIT];
    int proto;
    int accept;
    SUser *new_user = 0;
    SGame *game = 0;
    Net_Socket *new_socket = 0;
    Net_Msg msg;
    SUser *user, *r_user;
    int i, version, update, levelcount, id;
    /* new connection */
    net_clear_error();
    if ( ( new_socket = net_accept( server ) ) ) {
        /* the user is added to the list but not ready yet
           if the identification fails he will be removed else
           ready is set True */
        new_user = server_add_hidden_user( new_socket );
    }
    else
        if ( net_get_error() )
            fprintf( stderr, "%s\n", net_get_error() );
    /* active users */
    list_reset( users );
    while ( ( user = list_next( users ) ) ) {
        if ( !net_read_msg( user->socket, &msg, 0 ) ) {
            if ( user->socket->lost ) {
                printf( "connection to player '%s' (%i) lost\n", user->name, user->id );
                server_remove_user( user );
            }
            else
            if ( user_idle_limit > 0 ) {
                if ( user->idle_start_time + user_idle_limit <= time( 0 ) )
                    server_kick_user( user, "timeout" );
            }
        }
        else {
            user->idle_start_time = time( 0 );
            /* handle messages */
            switch ( msg.type ) {
                case MSG_IDENTIFY: 
                    /* user has sent his identification */
                    if ( !net_unpack_int8( &msg, &proto ) ||
                         !net_unpack_string( &msg, name, 16 ) ) {
                        server_error_bad_pack( user );
                        break;
                    }
                    accept = 1;
                    if ( !server_validate_user_name( name ) ) {
                        server_send_error( user->socket,
                                           "Invalid user name#(must contain non-whitespaces)" );
                        accept = 0;
                    }
                    else
                    if ( users->count == user_limit + 1 ) {
                        if ( admin_pwd[0] == 0 || !STRCMP( name, admin_pwd ) ) {
                            accept = 0;
                            server_send_error( user->socket, "Server is full" );
                        }
                    }
                    else
                    if ( PROTOCOL != proto ) {
                        accept = 0;
                        sprintf( text, "Invalid client protocol '%i': '%i' is required", proto, PROTOCOL );
                        server_send_error( user->socket, text );
                    }
                    else {
                        if ( strstr( name, "admin" ) ) {
                            accept = 0;
                            sprintf( text, "Your user name must not contain the term 'admin'." );
                            server_send_error( user->socket, text );
                        }
                        else {
                            i = 2;
                            strcpy_lt( text, name, 15 );
                            while ( server_find_user_by_name( text ) ) {
                                sprintf( text, "%s(%i)", name, i );
                                i++;
                                if ( i == 10 ) break;
                            }
                            if ( i == 10 ) {
                                accept = 0;
                                sprintf( text, "The name '%s' is already in use.", name );
                                server_send_error( user->socket, text );
                            }
                            else
                                strcpy_lt( name, text, 15 );
                        }
                    }
                    if ( accept ) {
                        if ( admin_pwd[0] != 0 && STRCMP( admin_pwd, name ) )
                            strcpy( user->name, "admin" );
                        else
                            strcpy( user->name, name );
                        server_activate_user( user );
                    }
                    else
                        server_remove_user( user );
                    break;
                case MSG_DISCONNECT:
                    /* user has disconnected */
                    printf( "player '%s' (%i) has left the server\n", user->name, user->id );
                    if ( user->playing )
                        server_remove_game( user );
                    server_remove_user( user );
                    break;
                case MSG_CHAT:
                    /* check if chatter starts with an '/'. if so it's a command and
                       will be interpreted. else it's normal chatter which is broadcasted
                       to all unbound users or the partner you're linked to */
                    if ( net_unpack_string( &msg, buf, CHAT_MSG_LIMIT ) ) {
                        buf[CHAT_MSG_LIMIT - 1] = 0;
                        if ( buf[0] == 0 )
                            break; /* nothing to say? */
                        if ( buf[0] == '/' )
                            server_handle_command( user, buf );
                        else {
                            sprintf( text, "<%s> %s", user->name, buf );
                            if ( net_build_msg( &msg, MSG_CHAT, text ) )
                                server_broadcast_channel( user->channel, &msg );
                        }
                    }
                    break;
                case MSG_WHISPER:
                    if ( net_unpack_int16( &msg, &i ) )
                    if ( net_unpack_string( 
                             &msg, buf, CHAT_MSG_LIMIT ) ) {
                        buf[CHAT_MSG_LIMIT - 1] = 0;
                        if ( buf[0] == 0 )
                            break; /* nothing to say? */
                        if ( ( r_user = server_find_user( i ) ) ) {
                            sprintf( text, "<%s> %s", 
                                user->name, buf );
                            if ( net_build_msg( &msg, MSG_CHAT, text ) )
                                net_write_msg( r_user->socket, &msg );
                            if ( user != r_user )
                                net_write_msg( user->socket, &msg );
                        }
                    }
                    break;
                case MSG_GAME_STATS:
                case MSG_IP_ADDRESS:
                case MSG_BENCHMARK:
                case MSG_BUSY:
                case MSG_OLDER_SET:
                case MSG_CANCEL_TRANSFER:
                case MSG_REJECT_SET:
                case MSG_ACCEPT_SET:
                case MSG_TRANSFER_DATA:
                case MSG_CANCEL_CHALLENGE:
                case MSG_REJECT_CHALLENGE:
                case MSG_ACCEPT_CHALLENGE:
                    /* if challenge is skipped remove hidden game */
                    if ( msg.type == MSG_CANCEL_CHALLENGE ||
                         msg.type == MSG_REJECT_CHALLENGE )
                        server_remove_game( user );
                    /* if challenge is accepted activate game */
                    if ( msg.type == MSG_ACCEPT_CHALLENGE )
                        server_activate_game( user );
                    /* pass message */
                    if ( net_unpack_int16( &msg, &i ) )
                    if ( ( r_user = server_find_user( i ) ) )
                        net_write_msg( r_user->socket, &msg );
                    break;
                case MSG_CHALLENGE:
                    if ( net_unpack_int16( &msg, &i ) ) /* challenger (this user) */
                    if ( net_unpack_int16( &msg, &i ) ) /* challenged */
                    if ( net_unpack_string( &msg, set, 16 ) ) /* levelset */
                    if ( ( r_user = server_find_user( i ) ) ) {
                        if ( r_user == user ) break;
                        /* if user is playing send a busy back */
                        if ( r_user->playing ) {
                            if ( net_build_msg( &msg, MSG_BUSY, i ) )
                            if ( net_write_msg( user->socket, &msg ) )
                                break;
                        }
                        net_write_msg( r_user->socket, &msg );
                        server_add_hidden_game( game_id++, user, r_user, set );
                    }
                    break;
                case MSG_GAME_EXITED:
                    /* send an update to user */
                    server_remove_game( user );
                    server_send_update( user );
                    break;
                case MSG_WINNER:
                    /* if true the challenger won a round else the guest */
                    if ( net_unpack_int8( &msg, &i ) ) {
                        game = server_find_game( user );
                        if ( i ) game->host_wins++;
                        else     game->guest_wins++;
                        if ( net_build_msg( &msg, MSG_UPDATE_GAME, game->id, i ) )
                            server_broadcast_channel( user->channel, &msg );
                    }
                    break;
                case MSG_ENTER_CHANNEL:
                    /* user wants to switch to channel by this name
                       (either existing or new) */
                    if ( net_unpack_string( &msg, name, 16 ) )
                        server_transfer_user( user, name );
                    break;
                case MSG_OPEN_TRANSFER:
                    net_unpack_int16( &msg, &user->transfer_id );
                    break;
                case MSG_CLOSE_TRANSFER:
                    user->transfer_id = -1;
                    break;
                case MSG_INIT_TRANSFER:
                    if ( net_unpack_int16( &msg, &id ) )
                    if ( net_unpack_string( &msg, set, 16 ) )
                    if ( net_unpack_int8( &msg, &version ) )
                    if ( net_unpack_int8( &msg, &update ) )
                    if ( net_unpack_int8( &msg, &levelcount ) )
                    if ( ( r_user = server_find_user( id ) ) ) {
                        if ( r_user == user ) break;
                        /* if user is playing send a busy back */
                        if ( r_user->playing ) {
                            if ( net_build_msg( &msg, MSG_BUSY, i ) )
                            if ( net_write_msg( user->socket, &msg ) )
                                break;
                        }
                        /* check transfer id */
                        if ( r_user->transfer_id != user->id || 
                             levelcount == 0 /* no levels? */ ) {
                            if ( net_build_msg( &msg, MSG_REJECT_SET, r_user->id ) )
                                net_write_msg( user->socket, &msg );
                            if ( levelcount > 0 )
                                sprintf( buf, "'%s' is not open to your transfers.", r_user->name );
                            else
                                sprintf( buf, "Your set contains no levels???" );
                            if ( net_build_msg( &msg, MSG_SERVER_INFO, buf ) )
                                net_write_msg( user->socket, &msg );
                        }
                        else {
                            /* transfer ok but rebuild message to replace the
                               receivers name with the sender's one */
                            if ( net_build_msg( &msg, MSG_INIT_TRANSFER, 
                                     user->id, set, version, update, levelcount ) )
                                net_write_msg( r_user->socket, &msg );
                        }
                    }
                    break;
            }
        }
    }
}

/*
====================================================================
Display help text.
====================================================================
*/
void display_help()
{
    printf( "Usage:\n  lbreakout2server\n" );
    printf( "    [-p <SERVER_PORT>]      The server will run at this port.\n" );
    printf( "    [-l <USER_LIMIT>]       Maximum number of users that can login to server.\n" );
    printf( "    [-i <USER_IDLE_TIME>]   A user is kicked after <USER_IDLE_TIME> seconds\n" );
    printf( "                            of inactivity. (no communication)\n" );
    printf( "    [-m <WELCOME_MESSAGE>]  This message is send to a user on login.\n" );
    printf( "    [-a <ADMIN_PWD>]        The user logging in as <ADMIN_PWD> will become\n" );
    printf( "                            the administrator named 'admin'.\n" );
    exit( 0 );
}

/*
====================================================================
Parse the command line.
====================================================================
*/
void parse_args( int argc, char **argv )
{
    int i;
    for ( i = 0; i < argc; i++ ) {
        if ( STRCMP( "-p", argv[i] ) )
            if ( argv[i + 1] )
                server_port = atoi( argv[i + 1] );
        if ( STRCMP( "-l", argv[i] ) )
            if ( argv[i + 1] )
                user_limit = atoi( argv[i + 1] );
        if ( STRCMP( "-i", argv[i] ) )
            if ( argv[i + 1] )
                user_idle_limit = atoi( argv[i + 1] );
        if ( STRCMP( "-h", argv[i] ) || STRCMP( "--help", argv[i] ) )
            display_help();
        if ( STRCMP( "-m", argv[i] ) )
            if ( argv[i + 1] )
                strcpy_lt( welcome_msg, argv[i + 1], 511 );
        if ( STRCMP( "-a", argv[i] ) )
            if ( argv[i + 1] )
                strcpy_lt( admin_pwd, argv[i + 1], 15 );
    }
}

/*
====================================================================
Shutdown server cleanly if CTRL-C was involved.
====================================================================
*/
void signal_handler( int signal )
{
    switch ( signal ) {
        case SIGINT:
            server_finalize();
            exit( 0 );
    }
}

#endif

int main( int argc, char *argv[])
{
#ifdef NETWORK_ENABLED
    int leave = 0;
    SDL_Init( SDL_INIT_TIMER );
    parse_args( argc, argv );
    signal( SIGINT, signal_handler );
    if ( server_init() ) {
        while ( !leave ) {
            server_handle( &leave );
            if ( server_halting && time( 0 ) > server_halt_start + 5 )
                leave = 1;
            SDL_Delay( 5 );
        }
        server_finalize();
    }
    return 0;
#else
    fprintf( stderr, "LBreakout2 has been compiled without network support.\n" );
#endif
}

/***************************************************************************
                          client_handlers.c  -  description
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
 
#include "lbreakout.h"
#include "config.h"
#include "levels.h"
#include "../gui/gui.h"
#include "client_data.h"
#include "client_handlers.h"

/*
====================================================================
Externals
====================================================================
*/
extern Config config;
extern List *client_users;
extern List *client_games;
extern List *client_channels;
extern List *client_levelsets;
extern char *client_levelset;
extern ClientUser *client_user;
extern ClientChannel *client_channel;
extern char chatter[CHAT_LINE_COUNT][CHAT_LINE_WIDTH];
extern GuiWidget *dlg_connect;
extern GuiWidget *dlg_info;
extern GuiWidget *dlg_confirm;
extern GuiWidget *dlg_chatroom;
extern GuiWidget *dlg_channels;
extern GuiWidget *dlg_stats;
extern GuiWidget *dlg_pauseroom;
extern GuiWidget *dlg_help;
extern GuiWidget *label_info;
extern GuiWidget *label_stats;
extern GuiWidget *label_winner;
extern GuiWidget *edit_server;
extern GuiWidget *edit_username;
extern GuiWidget *list_chatter;
extern GuiWidget *edit_chatter;
extern GuiWidget *list_levels;
extern GuiWidget *list_users;
extern GuiWidget *list_channels;
extern GuiWidget *edit_channel;
extern GuiWidget *edit_pausechatter;
extern GuiWidget *list_help;
extern int levelset_version, levelset_update;
extern List *levels;
extern void client_popup_info( char *format, ... );
extern void client_run_game( int challenger );
#ifdef NETWORK_ENABLED
extern Net_Socket *game_peer;
#endif
extern int client_topic_count;
extern char *client_helps[];
extern Text *client_help_text;

/*
====================================================================
Client
====================================================================
*/
char client_error[128]; /* error message */
#ifdef NETWORK_ENABLED
Net_Socket *client = 0; /* client socket to the game server */
#endif
int client_id; /* id assigned by server */
char client_name[16]; /* our local username */
char client_ip[NET_IP_SIZE]; /* local ip send when logging in to server */
int client_state = CLIENT_NONE;

/*
====================================================================
Challenge data 
====================================================================
*/
char *mp_diff_names[] = { "Easy", "Medium", "Hard" };
char mp_levelset[16]; /* name of levelset we play */
int mp_peer_id;
char mp_peer_name[16]; /* remote player we want to play with */
int mp_levelset_version;
int mp_levelset_update; /* version of levelset */
int mp_level_count; /* number of levels in set */
int mp_diff, mp_rounds, mp_frags, mp_balls; /* game configuration */

/*
====================================================================
Connect to specified game server and open chatroom on success.
====================================================================
*/
void client_connect( GuiWidget *widget, GuiEvent *event )
{
    char *ptr;
    char server[24];
#ifdef NETWORK_ENABLED
    int connected = 0;
    Net_Msg msg;
#endif
    if ( event->type != GUI_CLICKED ) return;
    /* extract ip and port */
    gui_edit_get_text( edit_server, server, 24, 0, -1 );
    ptr = strchr( server, ':' );
    if ( !ptr )
        snprintf( config.host, 16, server );
    else {
        ptr[0] = 0;
        snprintf( config.host, 16, server );
        ptr++;
        config.port = atoi( ptr );
    }
    /* set user name */
    gui_edit_get_text( edit_username, 
        config.username, 16, 0,-1 );
    /* open info window */
    gui_label_set_text( label_info, 
        "Connecting to %s at port %i...", config.host, config.port );
    gui_widget_show( dlg_info );
#ifdef NETWORK_ENABLED
    /* try to connect */
    client_error[0] = 0;
    if ( ( client = net_connect( config.host, config.port ) ) ) {
        /* send identification */
        net_init_msg( &msg, MSG_IDENTIFY );
        if ( !net_pack_int8( &msg, PROTOCOL ) ||
             !net_pack_string( &msg, config.username ) || 
             !net_write_msg( client, &msg ) )
            sprintf( client_error, "%s\n", net_get_error() );
        else
            if ( net_read_msg( client, &msg, 8000 ) ) {
                switch ( msg.type ) {
                    case MSG_LOGIN_OKAY:
                        net_unpack_int16( &msg, &client_id );
                        net_unpack_string( &msg, client_name, 16 );
                        net_unpack_string( &msg, client_ip, 
                            NET_IP_SIZE );
                        connected = 1;
                        break;
                    case MSG_ERROR:
                        net_unpack_string( &msg, client_error, 127 );
                        break;
                    case MSG_DISCONNECT:
                        strcpy( client_error, 
                            "Server has disconnected" );
                        break;
                    default:
                        sprintf( client_error, 
                            "Unexpected message %i received", 
                            msg.type );
                        break;
                }
                
            }
            else
                strcpy( client_error, "No response from server" );
    }
    else
        strcpy( client_error, net_get_error() );
    /* either display error or go to chatroom */
    if ( !connected )
        gui_label_set_text( label_info, "ERROR: %s", client_error );
    else {
        gui_widget_hide( dlg_info );
        client_data_clear();
        gui_widget_show( dlg_chatroom );
    }
#endif
}

/*
====================================================================
Close client chatroom and return to connect dialogue.
====================================================================
*/
void client_disconnect( 
    GuiWidget *widget, GuiEvent *event )
{
    if ( event->type == GUI_CLICKED ) {
        gui_widget_hide( dlg_chatroom );
#ifdef NETWORK_ENABLED
        /* disconnect */
        if ( client ) {
            net_write_empty_msg( client, MSG_DISCONNECT );
            net_close( client );
            client = 0;
        }
#endif
    }
}
    
/*
====================================================================
Close connect dialogue and return to LBreakout's menu.
====================================================================
*/
void client_quit( GuiWidget *widget, GuiEvent *event )
{
    if ( event->type == GUI_CLICKED )
        gui_widget_hide( dlg_connect );
}

/*
====================================================================
Close the info window and clear state.
====================================================================
*/
void client_close_info( GuiWidget *widget, GuiEvent *event )
{
#ifdef NETWORK_ENABLED
    Net_Msg msg;
    if ( event->type == GUI_CLICKED ) {
        gui_widget_hide( dlg_info );
        switch ( client_state ) {
            case CLIENT_AWAIT_TRANSFER_CONFIRMATION:
                if ( net_build_msg( &msg, 
                         MSG_CANCEL_TRANSFER, mp_peer_id ) )
                    net_write_msg( client, &msg );
                break;
            case CLIENT_LISTEN:
                net_write_empty_msg( client, MSG_CLOSE_TRANSFER );
                break;
            case CLIENT_AWAIT_ANSWER:
                if ( net_build_msg( &msg, 
                         MSG_CANCEL_CHALLENGE, mp_peer_id ) )
                    net_write_msg( client, &msg );
                break;
        }
        client_state = CLIENT_NONE;
    }
#endif
}

/*
====================================================================
Send chatter this function is either called by the send button
or by the edit.
====================================================================
*/
void client_send_chatter( 
    GuiWidget *widget, GuiEvent *event )
{
#ifdef NETWORK_ENABLED
    char buf[CHAT_MSG_LIMIT + 1];
    Net_Msg msg;
    if ( ( widget->type == GUI_EDIT && 
         event->type == GUI_KEY_RELEASED &&
         event->key.keysym == SDLK_RETURN ) ||
         ( widget->type == GUI_BUTTON &&
         event->type == GUI_CLICKED ) ) {
        /* get message */
        gui_edit_get_text( edit_chatter, 
             buf, CHAT_MSG_LIMIT + 1, 0,-1 );
        /* clear chat edit */
        gui_edit_set_text( edit_chatter, "" );
        /* check for server-sided commands */
        /* deliver message to all users ... */
        if ( !net_build_msg( &msg, MSG_CHAT, buf ) ||
             !net_write_msg( client, &msg ) ) 
            fprintf( stderr, "%s\n", net_get_error() );
    }
#endif
}
/*
====================================================================
Whisper chatter if a user is selected.
====================================================================
*/
void client_whisper_chatter( 
    GuiWidget *widget, GuiEvent *event )
{
#ifdef NETWORK_ENABLED
    char buf[CHAT_MSG_LIMIT + 1];
    Net_Msg msg;
    if ( event->type != GUI_CLICKED ) return;
    /* get message */
    gui_edit_get_text( edit_chatter, 
         buf, CHAT_MSG_LIMIT + 1, 0,-1 );
    /* send to selected user */
    if ( client_user ) {
        /* deliver message */
        if ( !net_build_msg( &msg, MSG_WHISPER, 
                             client_user->id, buf ) ||
             !net_write_msg( client, &msg ) ) 
            fprintf( stderr, "%s\n", net_get_error() );
        /* clear chat edit */
        gui_edit_set_text( edit_chatter, "" );
    }
    else
        client_add_chatter( 
            "You must selected a user to whisper!", 1 );
#endif
}

/*
====================================================================
Update selected peer and levelset.
====================================================================
*/
void client_handle_user_list( 
    GuiWidget *widget, GuiEvent *event )
{
    if ( event->type == GUI_ITEM_SELECTED )
        client_user = list_get( client_users, event->item.y );
    else
        if ( event->type == GUI_ITEM_UNSELECTED )
            client_user = 0;
}
void client_handle_levelset_list( 
    GuiWidget *widget, GuiEvent *event )
{
    if ( event->type == GUI_ITEM_SELECTED )
        client_levelset = list_get( client_levelsets, 
            event->item.y );
    else
        if ( event->type == GUI_ITEM_UNSELECTED )
            client_levelset = 0;
}

/*
====================================================================
Handle confirmation/cancelling of confirmation dialogue.
====================================================================
*/
void client_confirm( GuiWidget *widget, GuiEvent *event )
{
#ifdef NETWORK_ENABLED
    Net_Msg msg;
    if ( event->type != GUI_CLICKED ) return;
    gui_widget_hide( dlg_confirm );
    switch ( client_state ) {
        case CLIENT_CONFIRM_TRANSFER:
            if ( net_build_msg( &msg, 
                     MSG_ACCEPT_SET, mp_peer_id ) )
                net_write_msg( client, &msg );
            /* prepare to receive levelcount levels */
            levels_create_empty_set( mp_level_count );
            list_first( levels ); /* set internal pointer to
                first level */
            client_popup_info( "Receiving levels..." );
            client_state = CLIENT_RECEIVE;
            break;
        case CLIENT_ANSWER:
            if ( net_build_msg( &msg, 
                     MSG_ACCEPT_CHALLENGE, mp_peer_id ) ) {
                net_write_msg( client, &msg );
                client_state = CLIENT_PLAY;
                client_run_game( 0 );
            }
            break;
    }
#endif
}
void client_cancel( GuiWidget *widget, GuiEvent *event )
{
#ifdef NETWORK_ENABLED
    Net_Msg msg;
    if ( event->type != GUI_CLICKED ) return;
    gui_widget_hide( dlg_confirm );
    switch ( client_state ) {
        case CLIENT_CONFIRM_TRANSFER:
            if ( net_build_msg( &msg,
                     MSG_REJECT_SET, mp_peer_id ) )
                net_write_msg( client, &msg );
            break;
        case CLIENT_ANSWER:
            if ( net_build_msg( &msg, 
                     MSG_REJECT_CHALLENGE, mp_peer_id ) )
                net_write_msg( client, &msg );
            break;
    }
    client_state = CLIENT_NONE;
#endif
}

/*
====================================================================
Challenge selected user.
====================================================================
*/
void client_challenge( GuiWidget *widget, GuiEvent *event )
{
#ifdef NETWORK_ENABLED
    Net_Msg msg;
    if ( event->type != GUI_CLICKED ) return;
    if ( client_user == 0 ) {
        client_popup_info( 
            "You must select a user for a challenge." );
        return;
    }
    if ( client_levelset == 0 ) {
        client_popup_info( 
            "You must select a levelset for a challenge." );
        return;
    }
    if ( client_user->id == client_id ) {
        client_popup_info( 
            "You can't challenge yourself." );
        return;
    }
    strcpy( mp_peer_name, client_user->name );
    mp_peer_id = client_user->id;
    strcpy( mp_levelset, client_levelset );
    mp_diff = config.mp_diff;
    mp_rounds = config.mp_rounds;
    mp_balls = config.mp_balls;
    mp_frags = config.mp_frags;
    /* challenger, challenged, levelset, diff, rounds, frags, balls */
    net_init_msg( &msg, MSG_CHALLENGE );
    if ( !net_pack_int16( &msg, client_id ) ||
         !net_pack_int16( &msg, mp_peer_id ) ||
         !net_pack_string( &msg, mp_levelset ) ||
         !net_pack_int8( &msg, mp_diff ) ||
         !net_pack_int8( &msg, mp_rounds ) ||
         !net_pack_int8( &msg, mp_frags ) ||
         !net_pack_int8( &msg, mp_balls ) ||
         !net_write_msg( client, &msg ) )
        fprintf( stderr, "%s\n", net_get_error() );
    else {
        client_popup_info( 
            "You have challenged %s. Let's see what (s)he says...",
            mp_peer_name );
        client_state = CLIENT_AWAIT_ANSWER;
    }
#endif
}

/*
====================================================================
Update multiplayer network configuration.
====================================================================
*/
void client_update_difficulty( GuiWidget *widget, GuiEvent *event )
{
    if ( event->type != GUI_CHANGED ) return;
    gui_spinbutton_get_value( widget, &config.mp_diff );
    config.mp_diff--;
}
void client_update_rounds( GuiWidget *widget, GuiEvent *event )
{
    if ( event->type != GUI_CHANGED ) return;
    gui_spinbutton_get_value( widget, &config.mp_rounds );
}
void client_update_frags( GuiWidget *widget, GuiEvent *event )
{
    if ( event->type != GUI_CHANGED ) return;
    gui_spinbutton_get_value( widget, &config.mp_frags );
}
void client_update_balls( GuiWidget *widget, GuiEvent *event )
{
    if ( event->type != GUI_CHANGED ) return;
    gui_spinbutton_get_value( widget, &config.mp_balls );
}
void client_update_port( GuiWidget *widget, GuiEvent *event )
{
    char aux[8];
    if ( event->type != GUI_CHANGED ) return;
    gui_edit_get_text( widget, aux, 8, 0, -1 );
    config.client_game_port = atoi( aux );
}

/*
====================================================================
Allow user to transfer a levelset.
====================================================================
*/
void client_listen( GuiWidget *widget, GuiEvent *event )
{
#ifdef NETWORK_ENABLED
    Net_Msg msg;
    if ( event->type != GUI_CLICKED ) return;
    if ( client_user == 0 ) {
        client_popup_info( 
            "You must selected the user you want to listen to." );
        return;
    }
    if ( net_build_msg( &msg, MSG_OPEN_TRANSFER, client_user->id ) &&
         net_write_msg( client, &msg ) ) {
        client_popup_info( 
            "You're now listening to transfers from %s.", 
            client_user->name );
        client_state = CLIENT_LISTEN;
    }
    else
        client_popup_info( "ERROR: %s", net_get_error() );
#endif
}

/*
====================================================================
Initiate levelset transfer.
====================================================================
*/
void client_transfer( GuiWidget *widget, GuiEvent *event )
{
#ifdef NETWORK_ENABLED
    Net_Msg msg;
    if ( event->type != GUI_CLICKED ) return;
    if ( client_user == 0 ) {
        client_popup_info( 
            "You must selected the user to whom you "\
            "want to transfer your levelset." );
        return;
    }
    if ( client_levelset == 0 ) {
        client_popup_info( 
            "You must select the levelset you want to send." );
        return;
    }
    if ( client_user->id == client_id ) {
        client_popup_info( "You can't transfer to yourself." );
        return;
    }
    if ( !levels_load( client_levelset ) ) {
        client_add_chatter( "Can't find levelset?", 1 );
        return;
    }
    /* initiate transfer */
    net_init_msg( &msg, MSG_INIT_TRANSFER );
    if ( !net_pack_int16( &msg, client_user->id ) ||
         !net_pack_string( &msg, 
              (client_levelset[0]=='~')?client_levelset+1:
                                        client_levelset ) ||
         !net_pack_int8( &msg, levelset_version ) ||
         !net_pack_int8( &msg, levelset_update ) ||
         !net_pack_int8( &msg, levels->count ) ||
         !net_write_msg( client, &msg ) )
        fprintf( stderr, "%s\n", net_get_error() );
    else {
        strcpy_lt( mp_peer_name, client_user->name, 15 );
        mp_peer_id = client_user->id;
        strcpy_lt( mp_levelset, client_levelset, 15 );
        client_popup_info(
            "You've offered the levelset %s to %s.#"\
            "Awaiting confirmation...",  
            mp_levelset, mp_peer_name );
        client_state = CLIENT_AWAIT_TRANSFER_CONFIRMATION;
    }
#endif
}

/*
====================================================================
Open channel selector
====================================================================
*/
void client_select_channel( GuiWidget *widget, GuiEvent *event )
{
    if ( event->type != GUI_CLICKED ) return;
    /* select first channel (we always have MAIN) */
    gui_list_update( list_channels, client_channels->count );
    if ( client_channels->count > 0 ) {
        client_channel = list_first( client_channels );
        gui_edit_set_text( edit_channel, client_channel->name );
        gui_list_select( list_channels, 0,0, 1 );
    }
    gui_widget_show( dlg_channels );
    client_state = CLIENT_SELECT_CHANNEL;
}
/*
====================================================================
Handle channel (un)selection.
====================================================================
*/
void client_handle_channel_list( 
    GuiWidget *widget, GuiEvent *event )
{
    /* if a channel is selected the name is copied into the edit.
       unselecting does not change anything. the channel by the 
       caption in the edit is opened on enter_channel() */
    if ( event->type == GUI_ITEM_SELECTED ) {
        client_channel = list_get( client_channels, event->item.y );
        if ( client_channel ) 
            gui_edit_set_text( edit_channel, client_channel->name );
    }
}
/*
====================================================================
Close channel selector or enter new channel.
====================================================================
*/
void client_enter_channel( GuiWidget *widget, GuiEvent *event )
{
#ifdef NETWORK_ENABLED
    Net_Msg msg;
    char buf[16];
    if ( event->type != GUI_CLICKED ) return;
    gui_widget_hide( dlg_channels );
    client_state = CLIENT_NONE;
    /* send name of channel we want to enter */
    buf[0] = 0;
    gui_edit_get_text( edit_channel, buf, 16, 0,-1 );
    if ( net_build_msg( &msg, MSG_ENTER_CHANNEL, buf ) )
        net_write_msg( client, &msg );
#endif
}
void client_cancel_channel( GuiWidget *widget, GuiEvent *event )
{
    if ( event->type != GUI_CLICKED ) return;
    gui_widget_hide( dlg_channels );
    client_channel = 0;
    client_state = CLIENT_NONE;
}

/*
====================================================================
Close statistics
====================================================================
*/
void client_close_stats( GuiWidget *widget, GuiEvent *event )
{
    if ( event->type != GUI_CLICKED ) return;
    gui_widget_hide( dlg_stats );
    client_state = CLIENT_NONE;
    gui_label_set_text( label_stats, "Awaiting stats..." );
    gui_label_set_text( label_winner, "..." );
}

/*
====================================================================
Send chatter to gamepeer in pauseroom when ENTER was pressed.
====================================================================
*/
void client_send_pausechatter( GuiWidget *widget, GuiEvent *event )
{
#ifdef NETWORK_ENABLED
    char buf[CHAT_MSG_LIMIT + 16];
    Net_Msg msg;
    if ( widget->type == GUI_EDIT && 
         event->type == GUI_KEY_RELEASED &&
         event->key.keysym == SDLK_RETURN ) {
        /* get message */
        sprintf( buf, "<%s>", client_name );
        gui_edit_get_text( edit_pausechatter, 
             buf+strlen(buf), CHAT_MSG_LIMIT + 1, 0,-1 );
        /* clear chat edit */
        gui_edit_set_text( edit_pausechatter, "" );
        /* deliver message to remote ... */
        if ( !net_build_msg( &msg, MSG_CHAT, buf ) ||
             !net_write_msg( game_peer, &msg ) ) 
            fprintf( stderr, "%s\n", net_get_error() );
        else
            client_add_pausechatter( buf, 0 );
    }
#endif
}

/*
====================================================================
Close pauseroom.
====================================================================
*/
void client_close_pauseroom( GuiWidget *widget, GuiEvent *event )
{
    if ( event->type != GUI_CLICKED ) return;
    gui_widget_hide( dlg_pauseroom );
}

/*
====================================================================
Popup help dialogue.
====================================================================
*/
void client_popup_help( GuiWidget *widget, GuiEvent *event )
{
    if ( event->type != GUI_CLICKED ) return;
    gui_widget_show( dlg_help );
    client_state = CLIENT_HELP;
}
/*
====================================================================
Close help dialogue.
====================================================================
*/
void client_close_help( GuiWidget *widget, GuiEvent *event )
{
    if ( event->type != GUI_CLICKED ) return;
    gui_widget_hide( dlg_help );
}
/*
====================================================================
Select topic and display help text.
====================================================================
*/
void client_handle_topic_list( GuiWidget *widget, GuiEvent *event )
{
    if ( event->type != GUI_ITEM_SELECTED ) return;
    if ( event->item.y >= client_topic_count ) return;
    if ( client_help_text ) delete_text( client_help_text );
    client_help_text = 
        create_text( client_helps[event->item.y], 41 );
    gui_list_update( list_help, client_help_text->count );
}

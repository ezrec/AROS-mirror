/***************************************************************************
                          client_data.c  -  description
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
 
#include "client_data.h"
#include "lbreakout.h"
#include "levels.h"
#include "../gui/gui.h"
 
/*
====================================================================
Externals
====================================================================
*/
extern GuiWidget *list_levels;
extern List *levelset_names;
extern GuiWidget *list_chatter;
extern GuiWidget *list_games;
extern GuiWidget *list_users;
extern GuiWidget *list_channels;
extern GuiWidget *list_pausechatter;

List *client_users = 0; /* users of channel known to client */
List *client_games = 0; /* running games */
List *client_channels = 0; /* list of known channels */
List *client_levelsets = 0; /* list of MP set names */

ClientChannel *client_channel = 0; /* selected channel */
ClientUser *client_user = 0; /* selected user */
char       *client_levelset = 0; /* selected levelset */
char chatter[CHAT_LINE_COUNT][CHAT_LINE_WIDTH];

int client_topic_count = 5;
char *client_topics[] = {
    "Challenges",
    "Channels",
    "Console Commands",
    "Game Rules",
    "Transfers"
};
char *client_helps[] = {
  "To challenge a user you first have to select him/her in the "\
  "upper right list and the levelset you want to play "\
  "from the list below. Then you adjust the game settings to your "\
  "likings and hit the 'Challenge' button which will send your "\
  "challenge to the other user who will either accept or decline. "\
  "If (s)he accepts a direct connection will be established and the "\
  "game will start.##"\
  "DIFFICULTY: This influences ball speed, paddle size and score as for "\
  "single player modus. (1 = Easy, 2 = Medium, 3 = Hard)##"\
  "ROUNDS: This is the number of rounds played per level. Winning a "\
  "round scores one point and the player with the most points wins "\
  "the match.##"\
  "FRAGS: In a deathmatch level (no bricks) a player gains a frag everytime "\
  "the opponent looses a ball. The player who hits this limit first "\
  "wins the round.#In normal levels (with bricks) this option is ignored "\
  "and the level is over after all bricks were cleared.##"\
  "BALLS: In a deathmatch level (no bricks) each player may fire multiple "\
  "balls up to this number. If a player looses a ball he may fire it again.#"\
  "In normal levels (with bricks) this option is ignored and a player may bring "\
  "back only one ball.##"\
  "GAME PORT: The actual game is running via a direct connection and the "\
  "challenger opens the local gameserver at this port.",
  "To switch a channel hit the 'C' button above the user list. "\
  "You can enter either an existing or a new channel. You "\
  "can only talk to and challenge users in the same channel.",
  "User Console Commands:#" \
  " /search <USER>   search for a user#" \
  " /version         display server#"\
  "                  version##" \
  "Admin Console Commands:#" \
  " /halt            halt server#"
  " /idletime        user idle time before#" \
  "                  timeout kick#" \
  " /info <MSG>      broadcast server#"\
  "                  message#" \
  " /kick <USER>     kick user#" \
  " /userlimit       number of users that#" \
  "                  may login",
  "Basically you play the game as in single player mode but their "\
  "are some special things you might find useful to know.##"\
  "DEATHMATCH:#"\
  "The point here is to play it fast and to use all of your balls. "\
  "Use the right and left mouse button to fire the balls to "\
  "different directions while moving the paddle. Your opponent "\
  "will have trouble to reflect all balls if you do it right. "\
  "If you're not sure wether you got balls left click anyway.##"\
  "NORMAL:#"\
  "Your goal here is too gain more score than your opponent to win a "\
  "round. Basically you do this by clearing bricks and collecting extras "\
  "but there are some other ways as well:#"\
  "1) To loose a ball means to loose 10% score.#"\
  "2) Hitting your opponents paddle with the plasma weapon will give you "\
  "1000 points while stealing him/her the same amount.#"\
  "3) Bonus/malus magnet will attract _all_ bonuses/maluses even those "\
  "released by your opponent.",
  "You cannot offer someone your levelset unless this user "\
  "wants to receive your set which requires the following steps:#"\
  "1) select the user to whom you want to listen#"\
  "2) press the 'L' button above the levelset list#"\
  "3) wait for offer or cancel 'listening'#"\
  "The receiver is now ready and the sender has to:#"\
  "1) select the listening user#"\
  "2) select the levelset (s)he wants to transfer#"\
  "3) hit the 'T' button above the levelset list#"\
  "If the receiver has this levelset already located in "\
  "~/.lgames/lbreakout2-levels and it is up-to-date the transfer "\
  "does not take place otherwise the receiver is asked to confirm "\
  "and if (s)he does so the set is transferred to the home "\
  "directory and directly available if intended for network. (starts with 'MP_')#"\
  "NOTE: Only the challenger is required to have the levelset so you "\
  "don't have to transfer sets to play with someone. However if your opponent "\
  "likes your set and wants to challenge others with it both can engage into "\
  "a transfer."
};
Text *client_help_text = 0;

/*
====================================================================
LOCALS
====================================================================
*/

static ClientGame *client_find_game( int id )
{
    ClientGame *entry = 0;
    list_reset( client_games );
    while ( ( entry = list_next( client_games ) ) )
        if ( entry->id == id )
            return entry;
    return 0;
}

static void client_add_chatter_intern( 
    GuiWidget *list, char *string, int info )
{
    int i, j;
    static Text *text;
    /* build new text */
    text = create_text( string, 52 );
    /* move old lines up */
    for ( i = 0; i < CHAT_LINE_COUNT - text->count; i++ )
        strcpy( chatter[i], chatter[i + text->count] );
    /* copy new lines */
    for ( j = 0, i = CHAT_LINE_COUNT - text->count; 
          i < CHAT_LINE_COUNT; i++, j++ ) {
        if ( info ) {
            strcpy( chatter[i] + 1, text->lines[j] );
            chatter[i][0] = '!';
        }
        else if ( j > 0 ) {
            strcpy( chatter[i] + 3 + info, text->lines[j] );
            chatter[i][0] = ' '; 
            chatter[i][1] = ' '; 
            chatter[i][2] = ' ';
        }
        else
            strcpy( chatter[i], text->lines[j] );
    }
    /* free memory */
    delete_text( text );
    /* update gui */
    gui_list_update( list, CHAT_LINE_COUNT );
}

/*
====================================================================
PUBLICS
====================================================================
*/

/*
====================================================================
Create/delete client's data structs.
====================================================================
*/
void client_data_create( void )
{
    client_channels = list_create( LIST_AUTO_DELETE, 0 );
    client_users = list_create( LIST_AUTO_DELETE, 0 );
    client_games = list_create( LIST_AUTO_DELETE, 0 );
    client_levelsets = list_create( LIST_AUTO_DELETE, 0 );
}
void client_data_delete( void )
{
    if ( client_channels ) list_delete( client_channels ); 
    client_channels = 0;
    if ( client_users ) list_delete( client_users ); 
    client_users = 0;
    if ( client_games ) list_delete( client_games ); 
    client_games = 0;
    if ( client_levelsets ) list_delete( client_levelsets ); 
    client_levelsets = 0;
    if ( client_help_text ) delete_text( client_help_text );
    client_help_text = 0;
}

/*
====================================================================
Clear all data structs
====================================================================
*/
void client_data_clear( void )
{
    list_clear( client_channels );
    list_clear( client_users );
    list_clear( client_games );
    client_user = 0;
    client_channel = 0;
    gui_list_update( list_users, 0 );
    gui_list_update( list_games, 0 );
    gui_list_update( list_channels, 0 );
}

/*
====================================================================
Add/remove/find users/games/channels. Do not update the GUI.
====================================================================
*/
void client_add_user( int id, char *name )
{
    ClientUser *user;
    if ( client_find_user( id ) ) return;
    user = calloc( 1, sizeof( ClientUser ) );
    if ( user ) {
        strcpy_lt( user->name, name, 12 );
        user->id = id;
        list_add( client_users, user );
    }
}
void client_remove_user( int id )
{
    ClientUser *entry = 0;
    if ( ( entry  = client_find_user( id ) ) ) {
        if ( entry == client_user )
            client_user = 0;
        list_delete_item( client_users, entry );
    }
}
ClientUser* client_find_user( int id )
{
    ClientUser *entry;
    list_reset( client_users );
    while ( ( entry = list_next( client_users ) ) )
        if ( entry->id == id )
            return entry;
    return 0;
}
void client_add_game( int id, char *set, char *host, char *guest )
{
    ClientGame *game;
    if ( client_find_game( id ) ) return;
    game = calloc( 1, sizeof( ClientGame ) );
    if ( game ) {
        game->id = id;
        strcpy_lt( game->host, host, 15 );
        strcpy_lt( game->guest, guest, 15 );
        strcpy_lt( game->levelset, set, 15 );
        list_add( client_games, game );
    }
}
void client_remove_game( int id )
{
    ClientGame *game = 0;
    if ( ( game = client_find_game( id ) ) )
        list_delete_item( client_games, game );
}
void client_add_channel( int id, char *name )
{
    ClientChannel *channel;
    if ( client_find_channel( id ) ) return;
    channel = calloc( 1, sizeof( ClientChannel ) );
    if ( channel ) {
        channel->id = id;
        strcpy_lt( channel->name, name, 15 );
        list_add( client_channels, channel );
    }
}
void client_remove_channel( int id )
{
    ClientChannel *channel = 0;
    if ( ( channel = client_find_channel( id ) ) ) {
        if ( client_channel == channel )
            client_channel = 0;
        list_delete_item( client_channels, channel );
    }
}
ClientChannel* client_find_channel( int id )
{
    ClientChannel *channel;
    list_reset( client_channels );
    while ( ( channel = list_next( client_channels ) ) ) {
        if ( channel->id == id )
            return channel;
    }
    return 0;
}

/*
====================================================================
Update game and GUI.
====================================================================
*/
void client_update_game( int id, int host_win )
{
    ClientGame *game = client_find_game( id );
    if ( game ) {
        game->host_wins += host_win;
        game->guest_wins += !host_win;
        gui_list_update( list_games, client_games->count );
    }
}

/*
====================================================================
Rebuild client's levelset list (mp levels) from global 
levelset list. Update the GUI.
====================================================================
*/
void client_update_levelsets( void )
{
    char *name;
    list_clear( client_levelsets ); client_levelset = 0;
    list_reset( levelset_names );
    while ( ( name = list_next( levelset_names ) ) )
        if ( levelset_is_network( name ) )
            list_add( client_levelsets, 
                strdup( name ) );
    gui_list_update( list_levels, client_levelsets->count );
}

/*
====================================================================
Add chatter to chat window. If 'info' is true the text is
displayed red and without indention.
====================================================================
*/
void client_add_chatter( char *string, int info )
{
    client_add_chatter_intern( list_chatter, string, info );
}

/*
====================================================================
Add chatter to pause chat window. If 'info' is true the text is
displayed red and without indention.
====================================================================
*/
void client_add_pausechatter( char *string, int info )
{
    client_add_chatter_intern( list_pausechatter, string, info );
}

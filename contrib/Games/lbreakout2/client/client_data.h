/***************************************************************************
                          client_data.h  -  description
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

#ifndef __CLIENT_DATA_H
#define __CLIENT_DATA_H

/*
====================================================================
Client states
====================================================================
*/
enum {
    CLIENT_NONE = 0,
    CLIENT_INFO, /* not open to any challenges/transfers */
    CLIENT_AWAIT_ANSWER, /* wait for answer to a challenge */
    CLIENT_ANSWER, /* answer to a challenge */
    CLIENT_CONFIRM_TRANSFER, /* say yes or no to transfer */
    CLIENT_AWAIT_TRANSFER_CONFIRMATION, /* wait for answer on 
                                           transfer offer */
    CLIENT_RECEIVE, /* receive level data */
    CLIENT_LISTEN, /* listen to user for a levelset */
    CLIENT_SELECT_CHANNEL, /* selecting a channel */
    CLIENT_STATS, /* looking at game stats */
    CLIENT_PLAY, /* playing game */
    CLIENT_HELP /* looking at help */
};

/*
====================================================================
Chatter definitions.
====================================================================
*/
enum { 
    CHAT_LINE_COUNT = 200,
    CHAT_LINE_WIDTH = 64 /* includes \0 */
};

/*
====================================================================
Client data structs
====================================================================
*/
typedef struct {
    int  id;
    char name[16];
} ClientUser;
typedef struct {
    int  id;
    char name[16];
} ClientChannel;
typedef struct {
    int id;
    char host[16], guest[16];
    char levelset[16];
    int  host_wins, guest_wins;
} ClientGame;

/*
====================================================================
Create/delete client's data structs.
====================================================================
*/
void client_data_create( void );
void client_data_delete( void );

/*
====================================================================
Clear all data structs
====================================================================
*/
void client_data_clear( void );

/*
====================================================================
Add/remove/find users/games/channels. Do not update the GUI.
====================================================================
*/
void client_add_user( int id, char *name );
void client_remove_user( int id );
ClientUser* client_find_user( int id );
void client_add_game( int id, char *set, char *host, char *guest );
void client_remove_game( int id );
void client_add_channel( int id, char *name );
void client_remove_channel( int id );
ClientChannel* client_find_channel( int id );

/*
====================================================================
Update game and GUI.
====================================================================
*/
void client_update_game( int id, int host_win );

/*
====================================================================
Rebuild client's levelset list (mp levels) from global 
levelset list.
====================================================================
*/
void client_update_levelsets( void );

/*
====================================================================
Add chatter to chat window. If 'info' is true the text is
displayed red and without indention.
====================================================================
*/
void client_add_chatter( char *string, int info );

/*
====================================================================
Add chatter to pause chat window. If 'info' is true the text is
displayed red and without indention.
====================================================================
*/
void client_add_pausechatter( char *string, int info );

#endif

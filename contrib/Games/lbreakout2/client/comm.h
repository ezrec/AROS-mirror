/***************************************************************************
                          comm.h  -  description
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

/*
====================================================================
This module is responsible for the network in-game communication.
It does not handle connecting but upkeeping connection.
====================================================================
*/

#ifndef __COMM_H
#define __COMM_H

/*
====================================================================
Reset the communicator. (data passed between client/host, 
asynchronous timeout)
====================================================================
*/
void comm_reset();

/*
====================================================================
Classify next send operation as asynchronous.
====================================================================
*/
void comm_reinit_connection();

/*
====================================================================
Handle communication
====================================================================
*/
enum {
    COMM_PAUSE = 0,
    COMM_QUIT,
    COMM_ERROR,
    COMM_NEXT_ROUND,
    COMM_GAME_OVER
};
int comm_handle( int ms, int *result );

/*
====================================================================
Send ROUND_OVER to client so he can initiate next round or end
a game. If 'last' is true the game is over. If 'client_wins'
is True client was the winner.
====================================================================
*/
int comm_send_round_over( int last, int client_wins );

/*
====================================================================
Run the remote pause in case the other player send a 
MSG_PAUSE. Wait for the MSG_UNPAUSE. Only the remote player
can end the pause.
====================================================================
*/
int comm_remote_pause( StkFont *font, char *str, Net_Socket *game_peer );

/*
====================================================================
Clear print stats which are updated by comm_handle().
====================================================================
*/
void comm_clear_stats();
void comm_print_stats();

#endif

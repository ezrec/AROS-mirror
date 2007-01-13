/***************************************************************************
                          msg.h  -  description
                             -------------------
    begin                : Mon Oct 21 12:02:57 CEST 2002
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

#ifndef __MESSAGES_H
#define __MESSAGES_H

/*
====================================================================
Protocol used by server and client (must be equal)
====================================================================
*/
#define PROTOCOL 2

/*
====================================================================
Server/client messages.
====================================================================
*/
enum {
    MSG_NONE = 0,
    MSG_ERROR,          /* transfer an error message */
    MSG_SERVER_INFO,    /* server message written to chat */
    MSG_IDENTIFY,       /* request nickname and lbreakout2 version */
    MSG_LOGIN_OKAY,     /* accept login */
    MSG_CHAT,           /* a chat message */
    MSG_PREPARE_UPDATE, /* tell client to prepare for full update */
    MSG_ADD_USER,       /* add a user to client's challengable list */
    MSG_REMOVE_USER,    /* remove a user from client's challengable list */
    MSG_ADD_USERS,      /* packed channels to speed up full updates */
    MSG_ADD_GAME,       /* add a game analoguely */
    MSG_REMOVE_GAME,    /* dito */
    MSG_UPDATE_GAME,    /* send update of a specific game */
    MSG_ADD_GAMES,      /* packed channels to speed up full updates */
    MSG_ADD_CHANNEL,
    MSG_REMOVE_CHANNEL, /* chat channels */
    MSG_ADD_CHANNELS,   /* packed channels to speed up full updates */
    MSG_CHALLENGE,      /* challenge another user */
    MSG_ACCEPT_CHALLENGE, /* accept challenge */
    MSG_REJECT_CHALLENGE, /* reject challenge */
    MSG_BUSY,           /* user can't accept challenges at the moment */
    MSG_CANCEL_CHALLENGE, /* cancel your challenge */
    MSG_GAME_EXITED,    /* returned if user leaves a game (either normal or abnormal) */
    MSG_DISCONNECT,     /* received when player leaves server */
    MSG_IP_ADDRESS,     /* ip address of the game host */
    MSG_LEVEL_DATA,     /* sent raw leveldata of current level to client */
    MSG_READY,          /* send if player is ready */
    MSG_PAUSE,          /* pause game */
    MSG_UNPAUSE,        /* guess what, eh? */
    MSG_BENCHMARK,      /* result of speed test (faster machine becomes host) */
    MSG_DM_HOSTDATA,    /* ingame package send by host (deathmatch) */
    MSG_DM_CLIENTDATA,  /* ingame message send by client */
    MSG_MP_HOSTDATA,    /* ingame package send by host (multiplayer) */
    MSG_MP_CLIENTDATA,  /* ingame message send by client */
    MSG_ROUND_OVER,     /* result of a network round */
    MSG_WINNER,         /* game host sends winner of round to server */
    MSG_ENTER_CHANNEL,  /* enter this channel */
    MSG_SET_CHANNEL,    /* update clients channel id */
    MSG_INIT_TRANSFER,  /* send an transfer offer to remote user */
    MSG_CANCEL_TRANSFER,/* stop the transfer */
    MSG_ACCEPT_SET,     
    MSG_REJECT_SET,     /* accept/refuse set offer */
    MSG_OLDER_SET,      /* send if you want to update a newer set */
    MSG_WHISPER,        /* private talk to another user */
    MSG_OPEN_TRANSFER,
    MSG_CLOSE_TRANSFER, /* allow user a transfer? */
    MSG_TRANSFER_DATA,  /* a single level */
    MSG_GAME_STATS      /* game statistics */
};

enum { CHAT_MSG_LIMIT = 112 };

/* Define messages */
void net_init_msgs( void );

#endif

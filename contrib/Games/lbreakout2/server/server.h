/***************************************************************************
                          server.h  -  description
                             -------------------
    begin                : Sun Apr 28 12:02:57 CEST 2002
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

#ifdef NETWORK_ENABLED

/*
====================================================================
User
====================================================================
*/
typedef struct _SUser{
    int id;             
    char name[16];      /* nickname */
    Net_Socket *socket; /* connection endpoint to server */
    int playing;        /* true if in game */
    int ready;          /* true if user is completely accepted by server */
    int idle_start_time; /* time when last package was received */
    int transfer_id;    /* this user may send levelsets */
    int channel;        /* id of the channel the user is currently in */
} SUser;

/*
====================================================================
Game
====================================================================
*/
typedef struct {
    int   id;     
    int   running; /* if actually running the game will be posted  
                      (challenges create an empty SGame that's why we
                       need this flag) */
    SUser *host;  /* player who made the challenge (pointer to 'users')*/
    SUser *guest; /* player that was challenged (pointer to 'users')*/
    char  set[16];  /* levelset that is played */
    int   host_wins, guest_wins; /* won rounds */
} SGame;

/*
====================================================================
Channel
====================================================================
*/
typedef struct {
    int id;
    char name[16];  /* name of the channel */
    List *users;    /* list of all users in this channel */
    List *games;    /* list of all games in this channel */
} SChannel;

#endif

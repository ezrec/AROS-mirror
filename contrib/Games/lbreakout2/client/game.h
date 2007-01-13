/***************************************************************************
                          game.h  -  description
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

/*
====================================================================
Load all static resources like frame, bricks, balls, extras...
====================================================================
*/
void game_create();
/*
====================================================================
Delete anything created by game_create();
====================================================================
*/
void game_delete();
/*
====================================================================
Initiates player and the first level. If channel is set the 
multiplayer values are handled.
Return Value: True if successful
====================================================================
*/
int game_init( char *setname, int diff_level, 
               Net_Socket *channel, int challenger, int host, int rounds, int frags, int balls );

/*
====================================================================
Free all memory allocated by game_init()
====================================================================
*/
void game_clear();

/*
====================================================================
Run game after first level was initiated. Initiates next levels,
too.
====================================================================
*/
void game_run();

/*
====================================================================
Test this level until ball gets lost.
====================================================================
*/
void game_test_level( Level *level );

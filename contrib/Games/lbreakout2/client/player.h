/***************************************************************************
                          player.h  -  description
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
Player information. Names are saved in the config.
====================================================================
*/
enum { MAX_PLAYERS = 4 };
typedef struct {
    char name[32]; /* name */
    int level_score; /* score gained in the current level */
    int score; /* current score */
    int lives; /* lives remaining (single player) */
    int frags; /* frags gained (multiplayer only) */
    int wins, losses; /* number of rounds won or lost */
    Level *level; /* current level */
    int level_id;
    int bricks[MAP_WIDTH][MAP_HEIGHT]; /* duration of level bricks */
    int grown_bricks[MAP_WIDTH][MAP_HEIGHT]; /* if not NULL the id of the brick to grow */
    /* statistics */
    int frags_total; /* total number of frags */
    int hits; /* number of successfull ball returns */
    int bricks_removed, bricks_total;
    int extras_collected, extras_total;
} Player;

/*
====================================================================
Add this player to the list and increase the counter until
MAX_PLAYERS is reached.
Return Value: True if successful
====================================================================
*/
int player_add( char *name, int lives, Level *level );
/*
====================================================================
Reset bricks array of this player.
====================================================================
*/
void player_reset_bricks( Player *player );
/*
====================================================================
Get first player.
Return Value: first player in list
====================================================================
*/
Player* players_get_first();
/*
====================================================================
Get next player in list (cycle: return first player after
last player).
Return Value: current player
====================================================================
*/
Player* players_get_next();
/*
====================================================================
Reset player counter.
====================================================================
*/
void players_clear();
/*
====================================================================
Return number of players still in game (lives > 0)
====================================================================
*/
int players_count();
/*
====================================================================
Add score + difficulty bonus.
====================================================================
*/
void player_add_score( Player *player, int score );

/***************************************************************************
                          player.c  -  description
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

#include "levels.h"
#include "player.h"
#include "difficulty.h"

int current_player = 0;
int player_count = 0;
Player players[MAX_PLAYERS];
extern Diff *game_diff;

/*
====================================================================
Add this player to the list and increase the counter until
MAX_PLAYERS is reached.
Return Value: True if successful
====================================================================
*/
int player_add( char *name, int lives, Level *level )
{
    if ( player_count == MAX_PLAYERS ) return 0;
    memset( &players[player_count], 0, sizeof( Player ) );
    strcpy( players[player_count].name, name );
    players[player_count].lives = lives;
    players[player_count].level = level;
    player_reset_bricks( &players[player_count] );
    player_count++;
    return 1;
}
/*
====================================================================
Reset bricks array of this player.
====================================================================
*/
void player_reset_bricks( Player *player )
{
    int i, j;
    for ( i = 0; i < MAP_WIDTH; i++ )
        for ( j = 0; j < MAP_HEIGHT; j++ ) {
            player->bricks[i][j] = -99; /* undefined duration which means it is initalized by game_init_level */
            player->grown_bricks[i][j] = 0; /* would be a wall that can't be grown so this means no brick there */
        }
}
/*
====================================================================
Get first player.
Return Value: first player in list
====================================================================
*/
Player* players_get_first()
{
    current_player = -1;
    return players_get_next();
}
/*
====================================================================
Get next player in list (cycle: return first player after
last player).
Return Value: current player
====================================================================
*/
Player* players_get_next()
{
    if ( players_count() == 0 ) return 0;
    do {
        current_player++;
        if ( current_player == player_count ) current_player = 0;
    }
    while ( players[current_player].lives == 0 );
    return &players[current_player];
}
/*
====================================================================
Reset player counter.
====================================================================
*/
void players_clear()
{
    player_count = 0;
}
/*
====================================================================
Return number of players still in game (lives > 0)
====================================================================
*/
int players_count()
{
    int i;
    int count = 0;
    for ( i = 0; i < player_count; i++ )
        if ( players[i].lives > 0 )
            count++;
    return count;
}
/*
====================================================================
Add score + difficulty bonus.
====================================================================
*/
void player_add_score( Player *player, int score )
{
    int add = game_diff->score_mod * score / 10;
    player->score += add;
    player->level_score += add;
    if ( add < 0 ) {
        if ( player->score < 0 )
            player->score = 0;
        if ( player->level_score < 0 )
            player->level_score = 0;
    }
}

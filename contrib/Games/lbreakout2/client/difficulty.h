/***************************************************************************
                          difficulty.h  -  description
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
Difficulty level specific information.
====================================================================
*/
typedef struct {
    int     lives, max_lives;
    int     paddle_size; /* paddle's starting size */
    int     paddle_max_size; /* max size */
    int     score_mod; /* 10: 100% ±1: ±10% */
    float   v_start, v_change, v_max; /* velocity for this difficulty */
} Diff;

/*
====================================================================
Get difficulty level information (easy, normal, hard)
====================================================================
*/
Diff* diff_get( int id );

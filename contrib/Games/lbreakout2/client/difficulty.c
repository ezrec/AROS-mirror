/***************************************************************************
                          difficulty.c  -  description
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

#include "lbreakout.h"
#include "difficulty.h"

int diff_count = 3;
Diff diffs[3] = {
    { 6, 9, 2, 8, 8,  0.24, 0.0000010, 0.40 },
    { 5, 7, 2, 6, 10, 0.27, 0.0000012, 0.43 },
    { 4, 5, 1, 4, 13, 0.30, 0.0000014, 0.45 }
};

/*
====================================================================
Get difficulty level information (easy, normal, hard)
====================================================================
*/
Diff* diff_get( int id )
{
    if ( id < 0 ) id = 0;
    if ( id > diff_count ) id = diff_count - 1;
    return &diffs[id];
}

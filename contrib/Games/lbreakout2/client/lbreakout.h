/***************************************************************************
                          lbreakout.h  -  description
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

#ifndef __LBREAKOUT_H
#define __LBREAKOUT_H

#define WITH_BUG_REPORT
#define GAME_DEBUG

/*
====================================================================
Global definitions for LBreakout and general system includes.
====================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../gui/stk.h"
#include "../common/tools.h"
#include "../common/list.h"
#include "../common/net.h"
#include "../common/messages.h"
#include "misc.h"

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795f
#endif

/*
====================================================================
Size of playing field and editable playing field.
====================================================================
*/
enum {
    MAP_WIDTH = 16,
    MAP_HEIGHT = 24,
    EDIT_WIDTH = 14,
    EDIT_HEIGHT = 18
};

/*
====================================================================
Extras
====================================================================
*/
enum {
    EX_NONE = -1,
    EX_SCORE200 = 0,
    EX_SCORE500,
    EX_SCORE1000,
    EX_SCORE2000,
    EX_SCORE5000,
    EX_SCORE10000,
    EX_GOLDSHOWER,
    EX_SHORTEN,
    EX_LENGTHEN,
    EX_LIFE,
    EX_SLIME,
    EX_METAL,
    EX_BALL,
    EX_WALL,
    EX_FROZEN,
    EX_WEAPON,
    EX_RANDOM,
    EX_FAST,
    EX_SLOW,
    EX_JOKER,
	EX_DARKNESS,
	EX_CHAOS,
	EX_GHOST_PADDLE,
    EX_DISABLE,
    EX_TIME_ADD,
    EX_EXPL_BALL,
    EX_BONUS_MAGNET,
    EX_MALUS_MAGNET,
    EX_WEAK_BALL,
    /*
    EX_SPIN_RIGHT,
    EX_SPIN_LEFT,
    */
	EX_NUMBER
};
/*
====================================================================
Alpha of shadow
====================================================================
*/
enum { SHADOW_ALPHA = 128 };
/*
====================================================================
Number of original backgrounds.
====================================================================
*/
enum { ORIG_BACK_COUNT = 6 };

/*
====================================================================
Game/level types
====================================================================
*/
enum { GT_LOCAL = 0, GT_NETWORK };
enum { LT_NORMAL = 0, LT_DEATHMATCH };

#endif

/**
 * Scout - The Amiga System Monitor
 *
 *------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * You must not use this source code to gain profit of any kind!
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 */


/* Prototypes for functions defined in
objects/scout_timer.c
 */

struct TimerEntry {
    struct MinNode te_Node;
    struct TimeRequest *te_Addr;
    TEXT te_Address[ADDRESS_LENGTH];
    TEXT te_Name[NODENAME_LENGTH];
    TEXT te_ReplyPort[ADDRESS_LENGTH];
    TEXT te_Unit[NODETYPE_LENGTH];
    TEXT te_Timeout[NUMBER_LENGTH];
};

void PrintTimer( STRPTR );

void SendTimerList( STRPTR );

#define TimersWindowObject                   NewObject(TimersWinClass->mcc_Class, NULL

APTR MakeTimersWinClass( void );


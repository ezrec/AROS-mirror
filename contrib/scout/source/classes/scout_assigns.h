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
 * @author Richard K�rber <rkoerber@gmx.de>
 */


/* Prototypes for functions defined in
objects/scout_assigns.c
 */

struct AssignEntry {
    struct MinNode ae_Node;
    APTR ae_Addr;
    TEXT ae_Address[ADDRESS_LENGTH];
    TEXT ae_Name[FILENAME_LENGTH];
    TEXT ae_Path[PATH_LENGTH];
    TEXT ae_RealPath[PATH_LENGTH];
    TEXT ae_Type[NODETYPE_LENGTH];
};

void PrintAssigns( STRPTR );

void SendAssList( STRPTR );

#define AssignsWindowObject                  NewObject(AssignsWinClass->mcc_Class, NULL

APTR MakeAssignsWinClass( void );


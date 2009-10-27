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
objects/scout_resethandlers.c
 */

struct ResetHandlerEntry {
    struct MinNode rhe_Node;
    struct Interrupt *rhe_Addr;
    TEXT rhe_Address[ADDRESS_LENGTH];
    TEXT rhe_Name[NODENAME_LENGTH];
    TEXT rhe_Type[NODETYPE_LENGTH];
    TEXT rhe_Pri[NUMBER_LENGTH];
    TEXT rhe_Data[ADDRESS_LENGTH];
    TEXT rhe_Code[ADDRESS_LENGTH];
};

void PrintResetHandlers( STRPTR );

void SendResetHandlersList( STRPTR );

#define ResetHandlersWindowObject            NewObject(ResetHandlersWinClass->mcc_Class, NULL

APTR MakeResetHandlersWinClass( void );


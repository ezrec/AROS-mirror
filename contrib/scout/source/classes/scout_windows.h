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
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 */


/* Prototypes for functions defined in
objects/scout_windows.c
 */

struct WindowEntry {
   struct MinNode we_Node;
   APTR we_Addr;
   TEXT we_Address[ADDRESS_LENGTH];
   TEXT we_Position[NUMBER_LENGTH * 2];
   TEXT we_Size[NUMBER_LENGTH * 2];
   TEXT we_Flags[NUMBER_LENGTH];
   TEXT we_IDCMP[NUMBER_LENGTH];
   TEXT we_Title[TEXT_LENGTH]; // ohne diese 4 zusätzlichen Bytes gibt es einen MungWall-Hit!
   TEXT we_ScreenAddress[ADDRESS_LENGTH];
   TEXT we_Type[NODETYPE_LENGTH];
};

void PrintWindows( STRPTR );

void SendWinList( STRPTR );

BOOL CloseScreenAll( struct Screen *screen );

#define WindowsWindowObject                  NewObject(WindowsWinClass->mcc_Class, NULL

APTR MakeWindowsWinClass( void );


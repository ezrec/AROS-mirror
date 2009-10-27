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
objects/scout_devices.c
 */

struct DeviceEntry {
    struct MinNode de_Node;
    struct Device *de_Addr;
    TEXT de_Address[ADDRESS_LENGTH];
    TEXT de_Name[FILENAME_LENGTH];
    TEXT de_Pri[NUMBER_LENGTH];
    TEXT de_Version[VERSION_LENGTH];
    TEXT de_OpenCount[NUMBER_LENGTH];
    TEXT de_RAMPtrCount[NUMBER_LENGTH];
    TEXT de_Type[NODETYPE_LENGTH];
    TEXT de_CodeType[FILENAME_LENGTH];
};

void PrintDevices( STRPTR );

void FlushDevices( void );

void SendDevList( STRPTR );

#define DevicesWindowObject                  NewObject(DevicesWinClass->mcc_Class, NULL

APTR MakeDevicesWinClass( void );


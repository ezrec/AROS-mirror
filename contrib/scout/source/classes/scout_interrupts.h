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
objects/scout_interrupts.c
 */

struct InterruptEntry {
    struct MinNode ie_Node;
    struct Interrupt *ie_Addr;
    TEXT ie_Address[ADDRESS_LENGTH];
    TEXT ie_Name[NODENAME_LENGTH];
    TEXT ie_Pri[NUMBER_LENGTH];
    TEXT ie_Data[ADDRESS_LENGTH];
    TEXT ie_Code[ADDRESS_LENGTH];
    TEXT ie_Number[32];
    TEXT ie_RealNumber[32];
    TEXT ie_Kind[NUMBER_LENGTH];
    TEXT ie_Type[NUMBER_LENGTH];
    ULONG ie_IntNumber;
};

void PrintInterrupts( STRPTR );

void SendIntList( STRPTR );

struct Interrupt *FindInterrupt( STRPTR );

struct Interrupt *RemoveInterrupt( STRPTR );

#define InterruptsWindowObject               NewObject(InterruptsWinClass->mcc_Class, NULL

APTR MakeInterruptsWinClass( void );


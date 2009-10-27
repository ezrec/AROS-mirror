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
objects/scout_patches.c
 */

struct PatchEntry {
    struct MinNode pe_Node;
    APTR pe_Addr;
    TEXT pe_Address[ADDRESS_LENGTH];
    struct Library *pe_LibraryAddr;
    TEXT pe_Library[NODENAME_LENGTH];
    ULONG pe_Offset;
    TEXT pe_OffsetDec[NUMBER_LENGTH];
    TEXT pe_OffsetHex[NUMBER_LENGTH];
    TEXT pe_Function[NODENAME_LENGTH];
    TEXT pe_State[NODETYPE_LENGTH];
    TEXT pe_Patcher[TEXT_LENGTH];
    TEXT pe_Type[NODETYPE_LENGTH];
};

void PrintPatches( STRPTR );

void SendPatchList( STRPTR );

#define PatchesWindowObject                  NewObject(PatchesWinClass->mcc_Class, NULL

APTR MakePatchesWinClass( void );


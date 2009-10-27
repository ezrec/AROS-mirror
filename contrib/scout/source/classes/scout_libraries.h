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
objects/scout_libraries.c
 */

void PrintLibraries( STRPTR );

void FlushLibraries( void );

void SendLibList( STRPTR );

struct LibraryEntry {
    struct MinNode le_Node;
    struct Library *le_Addr;
    TEXT le_Address[ADDRESS_LENGTH];
    TEXT le_Name[FILENAME_LENGTH];
    TEXT le_Pri[NUMBER_LENGTH];
    TEXT le_Version[VERSION_LENGTH];
    TEXT le_OpenCount[NUMBER_LENGTH];
    TEXT le_RAMPtrCount[NUMBER_LENGTH];
    TEXT le_Type[NODETYPE_LENGTH];
    TEXT le_CodeType[FILENAME_LENGTH];
};

#define LibrariesWindowObject                NewObject(LibrariesWinClass->mcc_Class, NULL

APTR MakeLibrariesWinClass( void );

#if defined(__amigaos4__)
struct InterfaceEntry {
    struct Interface *ife_Addr;
    TEXT ife_Address[ADDRESS_LENGTH];
    TEXT ife_Name[FILENAME_LENGTH];
    TEXT ife_RefCount[NUMBER_LENGTH];
    TEXT ife_Version[VERSION_LENGTH];
};
#endif


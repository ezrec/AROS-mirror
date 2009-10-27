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
objects/scout_expansions.c
 */

struct ExpansionEntry {
    struct ConfigDev *ee_Addr;
    TEXT ee_Address[ADDRESS_LENGTH];
    TEXT ee_Flags[NUMBER_LENGTH];
    TEXT ee_BoardAddr[ADDRESS_LENGTH];
    TEXT ee_BoardSize[NUMBER_LENGTH];
    TEXT ee_Type[NUMBER_LENGTH];
    TEXT ee_Product[NUMBER_LENGTH];
    TEXT ee_Manufacturer[NUMBER_LENGTH];
    TEXT ee_SerialNumber[NUMBER_LENGTH];
    TEXT ee_ManufName[FILENAME_LENGTH];
    TEXT ee_ProdName[FILENAME_LENGTH];
    TEXT ee_ProdClass[FILENAME_LENGTH];
    TEXT ee_HardwareType[ADDRESS_LENGTH];
};

void PrintExpansions( STRPTR );

void SendExpList( STRPTR );

#define ExpansionsWindowObject               NewObject(ExpansionsWinClass->mcc_Class, NULL

APTR MakeExpansionsWinClass( void );


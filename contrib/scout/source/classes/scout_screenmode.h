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
objects/scout_screenmode.c
 */

struct ScreenModeEntry {
    struct MinNode sme_Node;
    ULONG sme_ModeID;
    TEXT sme_Id[ADDRESS_LENGTH];
    TEXT sme_Name[NODENAME_LENGTH];
    TEXT sme_Width[NUMBER_LENGTH];
    TEXT sme_Height[NUMBER_LENGTH];
    TEXT sme_Depth[NUMBER_LENGTH];
};

struct ScreenModeData {
    struct NameInfo name;
    struct DimensionInfo dimension;
    struct MonitorInfo monitor;
    struct DisplayInfo display;
};

void PrintSMode( STRPTR );

void SendSModeList( STRPTR );

#define ScreenModesWindowObject              NewObject(ScreenModesWinClass->mcc_Class, NULL

APTR MakeScreenModesWinClass( void );


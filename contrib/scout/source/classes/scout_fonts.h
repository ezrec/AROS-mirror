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
objects/scout_fonts.c
 */

struct FontEntry {
    struct MinNode fe_Node;
    struct TextFont *fe_Addr;
    TEXT fe_Address[ADDRESS_LENGTH];
    TEXT fe_Name[FILENAME_LENGTH];
    TEXT fe_XSize[NUMBER_LENGTH];
    TEXT fe_YSize[NUMBER_LENGTH];
    TEXT fe_Style[NUMBER_LENGTH];
    TEXT fe_Flags[NUMBER_LENGTH];
    TEXT fe_Accessors[NUMBER_LENGTH];
    TEXT fe_LoChar[NUMBER_LENGTH];
    TEXT fe_HiChar[NUMBER_LENGTH];
    TEXT fe_Place[NUMBER_LENGTH];
};

void FlushFonts( void );

void PrintFonts( STRPTR );

void SendFontList( STRPTR );

#define FontsWindowObject                    NewObject(FontsWinClass->mcc_Class, NULL

APTR MakeFontsWinClass( void );


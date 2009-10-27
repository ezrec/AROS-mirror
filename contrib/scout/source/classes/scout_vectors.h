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
objects/scout_vectors.c
 */

struct VectorEntry {
    TEXT ve_ResetVectors[6][ADDRESS_LENGTH];
    TEXT ve_AutoVectors[7][ADDRESS_LENGTH];
    TEXT ve_IntVectors[16][ADDRESS_LENGTH];
};

void ClearResetVectors( void );

void PrintVectors( STRPTR );

void SendVectorList( STRPTR );

#define VectorsWindowObject                  NewObject(VectorsWinClass->mcc_Class, NULL

APTR MakeVectorsWinClass( void );


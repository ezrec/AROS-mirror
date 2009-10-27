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
objects/scout_mount.c
 */

struct MountEntry {
    struct MinNode me_Node;
    struct DosList *me_Device;
    struct DosList me_dol;
    BOOL me_ValidFSSM;
    ULONG me_fssmUnit;
    struct DosEnvec me_fssmEnviron;
    ULONG me_fssmFlags;
    TEXT me_Address[ADDRESS_LENGTH];
    TEXT me_Name[FILENAME_LENGTH];
    TEXT me_Unit[NUMBER_LENGTH];
    TEXT me_Heads[NUMBER_LENGTH];
    TEXT me_Cylinders[NUMBER_LENGTH];
    TEXT me_DiskState[FILENAME_LENGTH];
    TEXT me_Filesystem[NUMBER_LENGTH];
    TEXT me_DeviceName[FILENAME_LENGTH];
    TEXT me_Handler[FILENAME_LENGTH];
};

STRPTR GetDiskState( LONG );

STRPTR GetDiskType( LONG );

BOOL IsValidFSSM( struct FileSysStartupMsg *fssm );

void PrintMounts( STRPTR );

void SendMountList( STRPTR );

#define MountsWindowObject                   NewObject(MountsWinClass->mcc_Class, NULL

APTR MakeMountsWinClass( void );


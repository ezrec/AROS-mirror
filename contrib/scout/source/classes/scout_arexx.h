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
objects/scout_arexx.c
 */

extern struct MUI_Command arexx_list[56];

STRPTR FindMyARexxPort( CONST_STRPTR );

ULONG SafePutToPort( struct Message * , STRPTR );

short SendStartupMsg( STRPTR, STRPTR, BOOL );

struct Task * MyFindTask( STRPTR );

BOOL MyRemoveTask( STRPTR, BOOL );

BOOL MyFreezeTask( STRPTR );

BOOL MyActivateTask( STRPTR );

BOOL MySignalTask( STRPTR, STRPTR );

struct List * MyGetList( STRPTR );

struct Node * MyFindName( STRPTR , STRPTR );

struct Screen * MyFindScreen( STRPTR );

struct Window * MyFindWindow( STRPTR );


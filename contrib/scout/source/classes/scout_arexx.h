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

extern const struct MUI_Command arexx_list[56];

STRPTR FindMyARexxPort( CONST_STRPTR );

IPTR SafePutToPort( struct Message * , CONST_STRPTR );

short SendStartupMsg( CONST_STRPTR, CONST_STRPTR, BOOL );

struct Task * MyFindTask( CONST_STRPTR );

BOOL MyRemoveTask( CONST_STRPTR, BOOL );

BOOL MyFreezeTask( CONST_STRPTR );

BOOL MyActivateTask( CONST_STRPTR );

BOOL MySignalTask( CONST_STRPTR, CONST_STRPTR );

struct List * MyGetList( CONST_STRPTR );

struct Node * MyFindName( CONST_STRPTR , CONST_STRPTR );

struct Screen * MyFindScreen( CONST_STRPTR );

struct Window * MyFindWindow( CONST_STRPTR );


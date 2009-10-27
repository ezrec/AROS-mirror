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
objects/scout_tasks_detail.c
 */

#if defined(__AROS__)
    #define TasksDetailWindowObject  BOOPSIOBJMACRO_START(TasksDetailWinClass->mcc_Class)
#else
    #define TasksDetailWindowObject  NewObject(TasksDetailWinClass->mcc_Class, NULL
#endif

struct SegListEntry {
    struct MinNode sle_Node;
    TEXT sle_Lower[ADDRESS_LENGTH];
    TEXT sle_Upper[ADDRESS_LENGTH];
    TEXT sle_Size[ADDRESS_LENGTH];
};

APTR MakeTasksDetailWinClass( void );


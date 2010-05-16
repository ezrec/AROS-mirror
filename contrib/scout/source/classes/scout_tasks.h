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
objects/scout_tasks.c
 */

struct TaskEntry {
    struct MinNode te_Node;
    struct Task *te_Addr;
    char te_Address[ADDRESS_LENGTH];
    char te_Name[NODENAME_LENGTH];
    char te_Pri[NUMBER_LENGTH];
    char te_Type[NODETYPE_LENGTH];
    char te_Num[NUMBER_LENGTH];
    char te_State[TASK_STATE_LENGTH];
    char te_CPU[NUMBER_LENGTH];
    char te_SigWait[NUMBER_LENGTH];
    char te_FreeStack[NUMBER_LENGTH];
#ifdef __MORPHOS__
    char te_FreeStackPPC[NUMBER_LENGTH];
#endif
};

struct Task *TaskExists( struct Task *tasktofind );

CONST_STRPTR GetTaskState( UBYTE state, ULONG waitmask );

CONST_STRPTR GetNodeType( UBYTE );

STRPTR GetTaskName( struct Task *, STRPTR , ULONG );

CONST_STRPTR GetSigBitName( LONG );

void GetTaskEntry( struct Task *task, struct TaskEntry *te, BOOL cpuflag );

extern TEXT updatetimetext[8];

void PrintTasks( STRPTR );

void SendTaskList( STRPTR );

#define TasksWindowObject                    NewObject(TasksWinClass->mcc_Class, NULL

APTR MakeTasksWinClass( void );


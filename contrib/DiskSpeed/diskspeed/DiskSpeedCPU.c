/*
    Copyright © 1995-2003, The AROS Development Team. All rights reserved.
    $Id$

    Desc: CPU Load Monitoring code.
    Lang: English.

    Based on DiskSpeed v4.2 by Michael Sinz
*/

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>
#include <proto/utility.h>
#include <exec/resident.h>
#include <exec/execbase.h>

#include <exec/io.h>
#include	<exec/ports.h>
#include	<devices/timer.h>
#include	<dos/dos.h>
#include	<dos/dosextens.h>
#include	<devices/trackdisk.h>

#include <clib/exec_protos.h>
#include	<clib/dos_protos.h>
#include	<clib/timer_protos.h>

#define DEBUG 1
#   include <aros/debug.h>

#include <limits.h>
#include <string.h>
#include <stdio.h>

struct  Task    *   SPEED_Task;
APTR    stack;

struct CPU_AVAIL
{
    ULONG   _CPU_Use_Base;              /* Set to TRUE in order to use CPU...   */
    ULONG   _CPU_State_Flag;            /* Used to control this routine...      */
    ULONG   _CPU_Count_Low;             /* The low-order word for CPU count     */
    ULONG   _CPU_Count_High;            /* The high-order word for CPU count    */
    ULONG   CPU_Task;                   /* The task pointer...                  */
};

static const char Task_Name[] = "DiskSpeed CPU Load Monitor";

extern void CPU_Available();

void Free_CPU_Task(struct Task * SPEED_Task)
{
    if (SPEED_Task != 0) {
        if (SPEED_Task->tc_SPLower != 0) FreeMem(SPEED_Task->tc_SPLower, AROS_STACKSIZE);
        FreeMem(SPEED_Task, sizeof(struct Task));
    }

}
/*
    *****************************************************************************
        The CPU load init routine. Install the CPU Usage monitor task...
    *****************************************************************************
*/

struct Task *Init_CPU_Available(struct CPU_AVAIL *CPU_AVAIL_struct)
{
    CPU_AVAIL_struct->_CPU_State_Flag = 0;
    CPU_AVAIL_struct->_CPU_Count_Low = 0;
    CPU_AVAIL_struct->_CPU_Count_High = 0;
    
    if ((CPU_AVAIL_struct->CPU_Task!=0)||(CPU_AVAIL_struct->_CPU_Use_Base==0))  return CPU_AVAIL_struct->CPU_Task;
    
    /* OK we arent running and its OK to go .. so create our task ... */

    SPEED_Task = (struct Task *)AllocMem(sizeof(struct Task),
					       MEMF_PUBLIC | MEMF_CLEAR);

	if ( SPEED_Task != NULL)
	{
        NewList(&SPEED_Task->tc_MemEntry);
		SPEED_Task->tc_Node.ln_Type = NT_TASK;
		SPEED_Task->tc_Node.ln_Name = Task_Name;
		SPEED_Task->tc_Node.ln_Pri = -127;

		stack = AllocMem(AROS_STACKSIZE, MEMF_PUBLIC);

		if (stack != NULL)
		{
            struct TagItem tasktags[] =
		    {
                {TASKTAG_ARG1, (IPTR)CPU_AVAIL_struct},
			    {TAG_DONE	    	    	}
		    };
		
		    SPEED_Task->tc_SPLower = stack;
		    SPEED_Task->tc_SPUpper = (BYTE *)stack + AROS_STACKSIZE;
#if AROS_STACK_GROWS_DOWNWARDS
		    SPEED_Task->tc_SPReg = (BYTE *)SPEED_Task->tc_SPUpper - SP_OFFSET;
#else
		    SPEED_Task->tc_SPReg = (BYTE *)SPEED_Task->tc_SPLower + SP_OFFSET;
#endif

            if (NewAddTask(SPEED_Task, CPU_Available, NULL, tasktags) != NULL)
            {
                CPU_AVAIL_struct->CPU_Task = SPEED_Task;
                return SPEED_Task;
            }
        }

        Free_CPU_Task(SPEED_Task);
    }
    return NULL;
}

void Free_CPU_Available(struct CPU_AVAIL *CPU_AVAIL_struct)
{
    if (CPU_AVAIL_struct->CPU_Task)
    {
        RemTask( CPU_AVAIL_struct->CPU_Task );
        Free_CPU_Task(CPU_AVAIL_struct->CPU_Task);
        CPU_AVAIL_struct->CPU_Task = 0;
    }
}

void CPU_Calibrate(struct CPU_AVAIL *CPU_AVAIL_struct)
{
    while ( CPU_AVAIL_struct->_CPU_Count_Low != 0 ) CPU_AVAIL_struct->_CPU_Count_Low -= 1;
}

void CPU_Available(struct CPU_AVAIL *CPU_AVAIL_struct)
{
    while (0==0) /* RUN FOREVER!! */
    {
        if ( CPU_AVAIL_struct->_CPU_State_Flag == 0 )
        {
            CPU_AVAIL_struct->_CPU_Count_Low +=1;
            if ( CPU_AVAIL_struct->_CPU_Count_Low == 0 ) CPU_AVAIL_struct->_CPU_Count_High += 1;

        }
    }
}

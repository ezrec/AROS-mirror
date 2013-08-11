/*
    Copyright © 1995-2001, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Search a task by name.
    Lang: english
*/
#include <exec/execbase.h>
#include <aros/libcall.h>
#include <proto/exec.h>

#include "exec_intern.h"

/*****************************************************************************

    NAME */

	AROS_LH1(struct Task *, FindTask,

/*  SYNOPSIS */
	AROS_LHA(CONST_STRPTR, name, A1),

/*  LOCATION */
	struct ExecBase *, SysBase, 49, Exec)

/*  FUNCTION
	Find a task with a given name or get the address of the current task.
	Finding the address of the current task is a very quick function
	call, but finding a special task is a very CPU intensive instruction.
	Note that generally a task may already be gone when this function
	returns.

    INPUTS
	name - Pointer to name or NULL for current task.

    RESULT
	Address of task structure found.

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

******************************************************************************/
{
    AROS_LIBFUNC_INIT

    /* Quick return for a quick argument */
    if (name==NULL)
        return THISCPU ? THISCPU->ThisTask : NULL;

    return (struct Task *)ScanTasks(SCANTAG_FILTER_NAME, name, TAG_END);

    AROS_LIBFUNC_EXIT
} /* FindTask */


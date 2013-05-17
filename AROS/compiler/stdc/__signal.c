/*
    Copyright © 2012-2013, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>
#include <libraries/stdc.h>

/* We include the signal.h of posixc.library for getting min/max signal number
   TODO: Implement more elegant way to get maximum and minimum signal number
*/
#include <aros/posixc/signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "__stdc_intbase.h"
#include "__signal.h"

#define DEBUG 0
#include <aros/debug.h>

struct signal_func_data *__sig_getfuncdata(int signum)
{
    struct StdCIntBase *StdCBase =
        (struct StdCIntBase *)__aros_getbase_StdCBase();
    int i;

    if (signum < SIGHUP || signum > _SIGMAX)
    {
        errno = EINVAL;
        return NULL;
    }

    if (StdCBase->sigfunc_array == NULL)
    {
        StdCBase->sigfunc_array = malloc(_SIGMAX*sizeof(struct signal_func_data));

        if (!StdCBase->sigfunc_array)
        {
            errno = ENOMEM;
            return NULL;
        }

        for (i = 0; i < _SIGMAX; i++)
        {
            StdCBase->sigfunc_array[i].sigfunc = SIG_DFL;
            StdCBase->sigfunc_array[i].flags = 0;
        }
    }

    return &StdCBase->sigfunc_array[signum-1];
}

/* Handler for SIG_DFL */
void __sig_default(int signum)
{
    /* FIXME: Can we use something more user oriented than kprintf here ?
       f.ex. a requester or DOS IO functions */
    switch (signum)
    {
    case SIGABRT:
        kprintf("Program aborted.\n");
        break;

    case SIGTERM:
        kprintf("Program terminated.\n");
        break;

    default:
        kprintf("Caught signal %d; aborting...\n", signum);
        break;
    }

    __stdc_jmp2exit(0, 20);

    assert(0); /* Should not be reached */
}

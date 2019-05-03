/*
    Copyright � 2019, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <exec/types.h>
#include <aros/symbolsets.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

DEFINESET(INIT_ARRAY);
DEFINESET(INIT);

int main();
void (*__kick_start)(void *, void *);
void (*__set_runtime_env)(void);


//FIXME: isn't there a set function already available for this?
//FIXME: how this will work with applications not compiled through build system?
static void __run_program_sets(struct ExecBase *SysBase)
{
    /* This function is called back from startup code in library */
    /* By being in linklib it has access to application sets */
    if (!set_open_libraries()) asm("int3");
    if (!set_call_funcs(SETNAME(INIT), 1, 1)) asm("int3");
    set_call_funcs(SETNAME(INIT_ARRAY), 1, 0);
}

#define RUNTIMESTARTUP      "runtimestartup.so"
#define FUNC_SETRUNTIMEENV  "__set_runtime_env"
#define FUNC_KICKSTART      "__kick_start"

static void __bye()
{
    printf("Exiting...\n");
    exit(100);
}

void __runtimestartup()
{
    /* This thread is not an AROS Process/Task. Restrictions apply. */

    /* This is executed before main() via INIT_ARRAY section. The host thread
     * have not yet reached main(). It will never reach it, __kick_start has to
     * guarantee this.
     */

    void *__so_handle = NULL;

    __so_handle = dlopen("./"RUNTIMESTARTUP, RTLD_LAZY);
    if (__so_handle == NULL)
        __so_handle = dlopen(RUNTIMESTARTUP, RTLD_LAZY);

    if (__so_handle == NULL)
    {
        printf("<<ERROR>>Loader "RUNTIMESTARTUP" not fount at either ./ or /usr/lib\n");
        __bye();
    }

    __set_runtime_env = (void (*)())dlsym(__so_handle, FUNC_SETRUNTIMEENV);
    if (__set_runtime_env == NULL)
    {
        printf("<<ERROR>> Function "FUNC_SETRUNTIMEENV" not found in "RUNTIMESTARTUP"\n");
        dlclose(__so_handle);
        __bye();
    }

    __kick_start = (void (*)(void *, void *))dlsym(__so_handle, FUNC_KICKSTART);
    if (__kick_start == NULL)
    {
        printf("<<ERROR>> Function "FUNC_KICKSTART" not found in "RUNTIMESTARTUP"\n");
        dlclose(__so_handle);
        __bye();
    }

    __set_runtime_env();

    // TODO: error handling

    __kick_start(main, __run_program_sets);

    // TODO: error handling
}

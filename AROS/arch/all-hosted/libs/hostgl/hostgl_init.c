#include <aros/symbolsets.h>
#include <proto/exec.h>

#include LC_LIBDEFS_FILE

struct SignalSemaphore GLOBAL_GLX_CONTEXT_SEM;

static int HostGL_Init(LIBBASETYPEPTR LIBBASE)
{
    InitSemaphore(&GLOBAL_GLX_CONTEXT_SEM);
    return 1;
}

static int HostGL_Expunge(LIBBASETYPEPTR LIBBASE)
{
    return 1;
}

ADD2INITLIB(HostGL_Init, 0)
ADD2EXPUNGELIB(HostGL_Expunge, 0)

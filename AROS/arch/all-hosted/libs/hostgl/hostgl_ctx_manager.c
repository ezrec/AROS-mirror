/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/symbolsets.h>
#include <proto/exec.h>
#include "hostgl_ctx_manager.h"
#include "tls.h"
#include <aros/debug.h>

#include LC_LIBDEFS_FILE

static struct TaskLocalStorage * ctxtls;

#include "glx_hostlib.h"

/* AROS is just one process to host, thus it can only have
   one current GLX context. On the other hand, there can
   be many GL apps running under AROS at the same time, each
   with it's own GL context. To solve this, calls to HostGL
   functions are serialized and at each call, it is checked
   if current HostGL GLX context matches current AROS task
   GL context. If no, change is made before executing the call.
   Constant switching of GLX contexts is bad for performance,
   but solves the problem. */

static struct SignalSemaphore GLOBAL_GLX_CONTEXT_SEM;
static volatile GLXContext GLOBAL_GLX_CONTEXT;

AROSMesaContext HostGL_GetCurrentContext()
{
    return GetFromTLS(ctxtls);
}

VOID HostGL_SetCurrentContext(AROSMesaContext ctx)
{
    InsertIntoTLS(ctxtls, ctx);
}

VOID HostGL_Lock()
{
    ObtainSemaphore(&GLOBAL_GLX_CONTEXT_SEM);
}

VOID HostGL_UnLock()
{
    ReleaseSemaphore(&GLOBAL_GLX_CONTEXT_SEM);
}

/* This funtion needs to be called while holding the semaphore */
VOID HostGL_SetGlobalGLXContext()
{
    AROSMesaContext cur_ctx = HostGL_GetCurrentContext();
    if (cur_ctx)
    {
        if (cur_ctx->glXctx != GLOBAL_GLX_CONTEXT)
        {
            GLOBAL_GLX_CONTEXT = cur_ctx->glXctx;
            D(bug("TASK: 0x%x, GLX: 0x%x\n",FindTask(NULL), GLOBAL_GLX_CONTEXT));
#if defined(RENDERER_SEPARATE_X_WINDOW)
            GLXCALL(glXMakeContextCurrent, cur_ctx->XDisplay, cur_ctx->glXWindow, cur_ctx->glXWindow, cur_ctx->glXctx);
#endif
#if defined(RENDERER_PBUFFER_WPA)
            GLXCALL(glXMakeContextCurrent, cur_ctx->XDisplay, cur_ctx->glXPbuffer, cur_ctx->glXPbuffer, cur_ctx->glXctx);
#endif  
        }
    }
    else
    {
        GLOBAL_GLX_CONTEXT = NULL;
        D(bug("TASK: 0x%x, GLX: 0x%x\n",FindTask(NULL), GLOBAL_GLX_CONTEXT));
    }
}

static int HostGL_Ctx_Manager_Init(LIBBASETYPEPTR LIBBASE)
{
    InitSemaphore(&GLOBAL_GLX_CONTEXT_SEM);
    GLOBAL_GLX_CONTEXT = NULL;
    ctxtls = CreateTLS();
    return 1;
}

static int HostGL_Ctx_Manager_Expunge(LIBBASETYPEPTR LIBBASE)
{
    DestroyTLS(ctxtls);
    return 1;
}

ADD2INITLIB(HostGL_Ctx_Manager_Init, 0)
ADD2EXPUNGELIB(HostGL_Ctx_Manager_Expunge, 0)

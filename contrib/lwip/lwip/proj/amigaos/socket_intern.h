/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id: socket_intern.h,v 1.1 2002/07/11 17:59:24 sebauer Exp $
*/

#ifndef MUIMASTER_INTERN_H
#define MUIMASTER_INTERN_H

#ifndef EXEC_TYPES_H
#   include <exec/types.h>
#endif
#ifndef EXEC_LIBRARIES_H
#   include <exec/libraries.h>
#endif
#ifndef EXEC_MEMORY_H
#   include <exec/memory.h>
#endif
#ifndef INTUITION_CLASSES_H
#   include <intuition/classes.h>
#endif
#ifndef INTUITION_INTUITIONBASE_H
#   include <intuition/intuitionbase.h>
#endif
#ifndef GRAPHICS_GFXBASE_H
#   include <graphics/gfxbase.h>
#endif

#ifdef _AROS
#ifndef AROS_ASMCALL_H
#   include <aros/asmcall.h>
#endif
#ifndef CLIB_BOOPSISTUBS_H
#   include <clib/boopsistubs.h>
#endif

#else

#include <dos.h>

#define AROS_LIBFUNC_INIT
#define AROS_LIBBASE_EXT_DECL(a,b) a b = (struct Library*)getreg(REG_A6);
#define AROS_LIBFUNC_EXIT

#endif

#ifndef DOS_DOS_H
#   include <dos/dos.h>
#endif
#ifndef UTILITY_UTILITY_H
#   include <utility/utility.h>
#endif
#ifndef EXEC_SEMAPHORES_H
#include <exec/semaphores.h>
#endif


/* Sometype defs in AROS */
#ifndef _AROS
#ifndef _AROS_TYPES_DEFINED
typedef unsigned long IPTR;
typedef long STACKLONG;
typedef unsigned long STACKULONG;
#define _AROS_TYPES_DEFINED
#endif
#endif

/****************************************************************************************/

struct SocketBase_intern
{
    struct Library		library;
    struct ExecBase		*sysbase;
    BPTR			seglist;

    struct MsgPort *stack_port;
};

/****************************************************************************************/

#undef SOCKB
#define SOCKB(b)	((struct SocketBase_intern *)b)

#ifdef _AROS

#undef SysBase
#define SysBase     	(SOCKB(SocketBase)->sysbase)

#else

#undef SysBase
#define SysBase     	(((struct SocketBase_intern *)SocketBase)->sysbase)

#ifndef _COMPILER_H
#include "compiler.h"
#endif

#endif

/****************************************************************************************/

#endif /* MUIMASTER_INTERN_H */

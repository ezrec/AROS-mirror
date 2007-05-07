/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id: socket_intern.h,v 1.3 2002/07/20 14:42:57 sebauer Exp $
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

#ifdef __AROS__
#ifndef AROS_ASMCALL_H
#   include <aros/asmcall.h>
#endif
#ifndef CLIB_BOOPSISTUBS_H
#   include <clib/boopsistubs.h>
#endif

#else

#include <dos.h>

#define AROS_LIBFUNC_INIT
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
#ifndef __AROS__
#ifndef _AROS_TYPES_DEFINED
typedef unsigned long IPTR;
typedef long STACKLONG;
typedef unsigned long STACKULONG;
#define _AROS_TYPES_DEFINED
#endif
#endif

#ifndef _NETDB_H
#include <netdb.h>
#endif

#ifndef _GLOBALDATA_H
#include "globaldata.h"
#endif

#ifndef _LOCALDATA_H
#include "localdata.h"
#endif

/****************************************************************************************/

struct SocketBase_intern
{
    struct Library		library;
    struct ExecBase		*sysbase;

    struct GlobalData *gldata;
    struct LocalData *locdata;

    struct SocketBase_intern *orgbase;
    struct ThreadData *data;
};

/****************************************************************************************/

#undef SOCKB
#define SOCKB(b)	((struct SocketBase_intern *)b)

#ifdef __AROS__

#undef SysBase
#define SysBase     	(SOCKB(SocketBase)->sysbase)

#else

#undef SysBase
#define SysBase     	(((struct SocketBase_intern *)SocketBase)->sysbase)

#ifndef _COMPILER_H
#include "compiler.h"
#endif

#endif

#define GLDATA(b) (((struct SocketBase_intern*)b)->gldata)
#define LOCDATA(b) (((struct SocketBase_intern*)b)->locdata)

/****************************************************************************************/

#endif /* MUIMASTER_INTERN_H */

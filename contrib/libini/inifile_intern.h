/*
    Copyright © 2002-2004, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef INIFILE_INTERN_H
#define	INIFILE_INTERN_H

#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/memory.h>
#include <exec/semaphores.h>
#include <exec/execbase.h>
#include <aros/asmcall.h>
#include <dos/dos.h>
#include <utility/utility.h>
#include <libcore/base.h>

extern struct ExecBase *SysBase;

struct inifileBase_intern
{
    struct Library    library;
};

/****************************************************************************************/

#undef INB
#define INB(b)	((struct inifileBase_intern *)(b))

/****************************************************************************************/

#endif /* INIFILE_INTERN_H */

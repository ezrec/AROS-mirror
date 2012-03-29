/*
 * $Id$
 */

#include <aros/asmcall.h>
#include <exec/execbase.h>

struct ExecBase *SysBase;

extern void _start(void);

AROS_UFH1(__used void, startup,
	  AROS_UFHA(struct ExecBase *, sBase, A6))
{
    AROS_USERFUNC_INIT
    SysBase = sBase;
    _start();
    AROS_USERFUNC_EXIT
}

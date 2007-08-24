#ifndef _VBCCINLINE_FEELINCLASS_H
#define _VBCCINLINE_FEELINCLASS_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef FEELIN_TYPES_H
#include <feelin/types.h>
#endif

struct TagItem * __F_Query(__reg("a6") struct Library *, __reg("d0") ULONG What, __reg("a0") struct FeelinBase *)="\tjsr\t-30(a6)";
#define F_Query(What,Feelin) __F_Query(FeelinBase, (What), (Feelin))

#endif /*  _VBCCINLINE_FEELINCLASS_H  */

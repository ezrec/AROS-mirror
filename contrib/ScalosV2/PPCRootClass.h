// :ts=4 (Tabsize)

#ifndef SCALOS_H
#include "Scalos.h"
#endif /* SCALOS_H */

#ifndef POWERPC_SEMAPHORESPPC_H
#include <powerpc/semaphoresPPC.h>
#endif /* POWERPC_SEMAPHORESPPC_H */

// not real instance data. This struct is located before the object data.
struct RootInst
{
	struct SignalSemaphorePPC	objectlistsem;
	struct MinList			objectlist;
	struct SignalSemaphorePPC	notilistsem;
	struct MinList			notilist;
	struct MinNode			node;
	struct SC_Class			*oclass;
};

#define myRootInst(obj) (((struct RootInst *)(obj)) - 1)

// refer to SCCP_Notify also
struct NotifyNode				// node of notilist
{
	struct	MinNode node;
	ULONG	TriggerAttr;
	ULONG	TriggerVal;
	Object	*DestObject;
	ULONG	numargs;
	ULONG	arg_method;
	ULONG	arg_attr;
	ULONG	arg_value;
	// args follow here
};

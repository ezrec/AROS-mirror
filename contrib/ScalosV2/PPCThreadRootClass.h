// :ts=4 (Tabsize)

#ifndef SCALOS_H
#include "Scalos.h"
#endif /* SCALOS_H */

#ifndef POWERPC_SEMAPHORESPPC_H
#include <powerpc/semaphoresPPC.h>
#endif /* POWERPC_SEMAPHORESPPC_H */

#ifndef POWERPC_PORTSPPC_H
#include <powerpc/portsPPC.h>
#endif /* POWERPC_PORTSPPC_H */

// not real instance data. This struct is located before the object data.
struct ThreadRootInst
{
	struct SignalSemaphorePPC	objectlistsem;
	struct MinList			objectlist;
	struct SignalSemaphorePPC	notilistsem;
	struct MinList			notilist;
	Object					*destobject;
	struct MsgPortPPC		*msgport;

	struct MinNode			node;
	struct SC_Class			*oclass;
};

#define myThreadRootInst(obj) (((struct ThreadRootInst *)(obj)) - 1)

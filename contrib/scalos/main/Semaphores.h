// Semaphores.h
// $Date$
// $Revision$


#ifndef	SEMAPHORES_H
#define	SEMAPHORES_H

/* ------------------------------------------------- */

#ifdef SCALOSSEMAPHORE
#undef SCALOSSEMAPHORE
#endif
#define SCALOSSEMAPHORE		struct ScalosSemaphore

#define DEBUG_SEMAPHORE_MAGIC	0x55aabbcd

/* ------------------------------------------------- */

#if defined(DEBUG_SEMAPHORES)

#define	OWNMODE_SHARED		0
#define	OWNMODE_EXCLUSIVE	1

#undef SignalSemaphore

struct DebugSemaOwner
	{
	struct Node node;
	ULONG OwnMode;
	CONST_STRPTR FileName;
	CONST_STRPTR Func;
	ULONG Line;
	struct Process *Proc;
	};

struct ScalosSemaphore
	{
	struct SignalSemaphore Sema;

	ULONG MagicNumber;
	struct SignalSemaphore *SafetyPtr;

	struct List OwnerList;
	struct List BidderList;
	};

#else /* DEBUG_SEMAPHORES */

struct ScalosSemaphore
	{
	struct SignalSemaphore Sema;
	};

#endif /* DEBUG_SEMAPHORES */

/* ------------------------------------------------- */

#endif //SEMAPHORES_H

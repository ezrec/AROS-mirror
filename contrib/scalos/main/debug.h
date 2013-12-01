// debug.h
// $Date$
// $Revision$


#ifndef DEBUG_H
#define DEBUG_H

// Debugging Macros

#include <exec/semaphores.h>
#include <dos/dosextens.h>

/* ------------------------------------------------- */
#define       DEBUG_SEMAPHORES
//#define       DEBUG_LOCKS
//#define       DEBUG_MEMORY
/* ------------------------------------------------- */

#include "Semaphores.h"

#define	d1(x)	;
#define	d2(x)	{ Forbid(); x; Permit(); }

#define	debugLock_d1(LockName) ;
#define	debugLock_d2(LockName) \
	{\
	char xxName[200];\
	strcpy(xxName, "");\
	NameFromLock((LockName), xxName, sizeof(xxName));\
	KPrintF("%s/%s/%ld: " #LockName "=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, LockName, xxName);\
	}

/* ------------------------------------------------- */

#define	TIMESTAMPCOUNT_RESET_d1(iwt)
#define	TIMESTAMPCOUNT_START_d1(iwt,cnt)
#define	TIMESTAMPCOUNT_END_d1(iwt,cnt)
#define	TIMESTATS_d1(iwt)

#define	TIMESTAMPCOUNT_RESET_d2(iwt) \
	{ \
	memset((iwt)->iwt_TimeStamps, 0, sizeof((iwt)->iwt_TimeStamps)); \
	}
#define	TIMESTAMPCOUNT_START_d2(iwt,cnt) \
	{			\
	struct EClockVal ev;	\
	ReadEClock(&ev); \
	(iwt)->iwt_TimeStamps[cnt].ev_hi = ev.ev_lo; \
	}
#define	TIMESTAMPCOUNT_END_d2(iwt,cnt) \
	{			\
	struct EClockVal ev;	\
	ReadEClock(&ev); 	\
	(iwt)->iwt_TimeStamps[cnt].ev_lo += ev.ev_lo - (iwt)->iwt_TimeStamps[cnt].ev_hi; \
	}
#define	TIMESTATS_d2(iwt)	   \
	{			\
	struct EClockVal ev;	\
	ULONG n;	    \
	ULONG ticks = ReadEClock(&ev); \
	KPrintF("%s/%s/%ld: =====================================\n", __FILE__, __FUNC__, __LINE__);   \
	for (n = 0; n < sizeof((iwt)->iwt_TimeStamps) / sizeof((iwt)->iwt_TimeStamps[0]); n++) \
		{ \
		ULONG micro = ((ULONG64) 1000000 * (iwt)->iwt_TimeStamps[n].ev_lo) / ticks; \
		KPrintF("%s/%s/%ld: Counter[%ld]=%lu us\n", __FILE__, __FUNC__, __LINE__, n, micro);   \
		} \
	KPrintF("%s/%s/%ld: =====================================\n", __FILE__, __FUNC__, __LINE__);   \
	}

#define	TIMESTAMP_d1()		;
#define	TIMESTAMP_d2()		\
	{			\
	struct EClockVal ev;	\
	ULONG ticks = ReadEClock(&ev); \
	KPrintF("%s/%s/%ld: ticks=%lu  hi=%8lu  lo=%8lu\n", __FILE__, __FUNC__, __LINE__, ticks, ev.ev_hi, ev.ev_lo);	\
	}

#ifdef	AMITHLON
#define	TRACE_AMITHLON(n)	(*((char *) 0xf0000000) = (n))
#else
#define	TRACE_AMITHLON(n)	((void) 0)
#endif

/* ------------------------------------------------- */

#ifdef	DEBUG_SEMAPHORES

#undef	SignalSemaphore

// replace all semaphore-related functions by debug versions

//#undef  SCA_LockWindowList

//#define SCA_LockWindowList(accessmode)	DebugSCA_LockWindowList(accessmode, __FILE__, __FUNC__, __LINE__)
#define ScalosCreateSemaphore()         	DebugScalosCreateSemaphore(__FILE__, __FUNC__, __LINE__)
#define ScalosDeleteSemaphore(xsema)    	DebugScalosDeleteSemaphore(xsema, __FILE__, __FUNC__, __LINE__)
#define ScalosInitSemaphore(xsema)    		DebugScalosInitSemaphore(xsema, __FILE__, __FUNC__, __LINE__)
#define ScalosObtainSemaphore(xsema)    	DebugScalosObtainSemaphore(xsema, __FILE__, __FUNC__, __LINE__)
#define ScalosObtainSemaphoreShared(xsema)    	DebugScalosObtainSemaphoreShared(xsema, __FILE__, __FUNC__, __LINE__)
#define ScalosReleaseSemaphore(xsema)   	DebugScalosReleaseSemaphore(xsema, __FILE__, __FUNC__, __LINE__)
#define ScalosAttemptSemaphore(xsema)    	DebugScalosAttemptSemaphore(xsema, __FILE__, __FUNC__, __LINE__)
#define ScalosAttemptSemaphoreShared(xsema)    	DebugScalosAttemptSemaphoreShared(xsema, __FILE__, __FUNC__, __LINE__)

#define	ScalosLockIconListExclusive(xiwt)	DebugScalosLockIconListExclusive(xiwt, __FILE__, __FUNC__, __LINE__)
#define	ScalosLockIconListShared(xiwt)		DebugScalosLockIconListShared(xiwt, __FILE__, __FUNC__, __LINE__)
#endif /* DEBUG_SEMAPHORES */


/* ------------------------------------------------- */

#ifdef	DEBUG_LOCKS

BPTR Lock_Debug(CONST_STRPTR name, LONG mode,
	CONST_STRPTR CallingArg, CONST_STRPTR CallingFile, 
	CONST_STRPTR CallingFunc, ULONG CallingLine);
void UnLock_Debug(BPTR lock,
	CONST_STRPTR CallingArg, CONST_STRPTR CallingFile, 
	CONST_STRPTR CallingFunc, ULONG CallingLine);
BPTR DupLock_Debug(BPTR lock,
	CONST_STRPTR CallingArg, CONST_STRPTR CallingFile, 
	CONST_STRPTR CallingFunc, ULONG CallingLine);
BPTR ParentDir_Debug(BPTR lock,
	CONST_STRPTR CallingArg, CONST_STRPTR CallingFile, 
	CONST_STRPTR CallingFunc, ULONG CallingLine);
BPTR ParentOfFH_Debug(BPTR fh,
	CONST_STRPTR CallingArg, CONST_STRPTR CallingFile, 
	CONST_STRPTR CallingFunc, ULONG CallingLine);
BPTR DupLockFromFH_Debug(BPTR fh,
	CONST_STRPTR CallingArg, CONST_STRPTR CallingFile, 
	CONST_STRPTR CallingFunc, ULONG CallingLine);

#undef Lock
#undef UnLock
#undef DupLock
#undef ParentDir
#undef ParentOfFH
#undef DupLockFromFH

#define	Lock(name, mode) Lock_Debug(name, mode, #mode, __FILE__, __FUNC__, __LINE__)
#define	UnLock(lock) UnLock_Debug(lock, #lock, __FILE__, __FUNC__, __LINE__)
#define	DupLock(lock) DupLock_Debug(lock, #lock, __FILE__, __FUNC__, __LINE__)
#define	ParentDir(lock) ParentDir_Debug(lock, #lock, __FILE__, __FUNC__, __LINE__)
#define	ParentOfFH(fh) ParentOfFH_Debug(fh, #fh, __FILE__, __FUNC__, __LINE__)
#define	DupLockFromFH(fh) DupLockFromFH_Debug(fh, #fh, __FILE__, __FUNC__, __LINE__)

#endif /* DEBUG_LOCKS */

/* ------------------------------------------------- */

#ifdef DEBUG_MEMORY

APTR ScalosAlloc_Debug(ULONG Size, CONST_STRPTR CallingFile, 
	CONST_STRPTR CallingFunc, ULONG CallingLine);
void ScalosFree_Debug(APTR mem, CONST_STRPTR CallingFile,
	CONST_STRPTR CallingFunc, ULONG CallingLine);

#define	ScalosAlloc(size) ScalosAlloc_Debug(size, __FILE__, __FUNC__, __LINE__)
#define	ScalosFree(mem) ScalosFree_Debug(mem, __FILE__, __FUNC__, __LINE__)

#endif /* DEBUG_MEMORY */

/* ------------------------------------------------- */

#if 0

#define	UnLock(fh) \
	do { KPrintF("%s/%s/%ld: UnLock(%08lx)\n", __FILE__, __FUNC__, __LINE__, (fh)); \
	UnLock(fh); } while (0)

#endif

/* ------------------------------------------------- */

// from debug.lib
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);


#endif	/* DEBUG_H */

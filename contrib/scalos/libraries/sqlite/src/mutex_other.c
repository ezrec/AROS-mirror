/*
** 2007 August 28
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains the C functions that implement mutexes for OS/2
**
** $Id$
*/

/*
** The code in this file is only used if SQLITE_MUTEX_OTHER is defined.
** See the mutex.h file for details.
*/
#ifdef SQLITE_OS_OTHER
#ifndef SQLITE_MUTEX_OMIT


#include <proto/exec.h>
#include <proto/dos.h>

#undef GLOBAL

#include "os_other.h"
#include "sqliteInt.h"

//#define SQLITE_DEBUG

#include "sqlite3_base.h"
#include "LibSQLite3.h"

#if defined(SQLITE_DEBUG)
#undef  d2
#define d2(x)	          x
#endif /* SQLITE_DEBUG */


/********************** Amiga Mutex Implementation **********************
**
** This implementation of mutexes is built using the AMigaOS API.
*/


static int otherMutexInit(void);
static int otherMutexEnd(void);
static sqlite3_mutex *otherMutexAlloc(int iType);
static void otherMutexFree(sqlite3_mutex *p);
static void otherMutexEnter(sqlite3_mutex *p);
static int otherMutexTry(sqlite3_mutex *p);
static void otherMutexLeave(sqlite3_mutex *p);
#ifdef SQLITE_DEBUG
static int otherMutexHeld(sqlite3_mutex *p);
static int otherMutexNotheld(sqlite3_mutex *p);
#endif /* SQLITE_DEBUG */


/*
** The mutex object
** Each recursive mutex is an instance of the following structure.
*/
struct sqlite3_mutex
{
  struct SignalSemaphore sema;	// Semaphore controlling the lock
  int  id;          		/* Mutex type */
  int  nRef;        		/* Number of references */
  struct Task *owner;		// Thread holding this mutex
};

/*
** Initialize and deinitialize the mutex subsystem.
*/
static int otherMutexInit(void)
{
	return SQLITE_OK;
}


static int otherMutexEnd(void)
{
	return SQLITE_OK;
}


/*
** The sqlite3_mutex_alloc() routine allocates a new
** mutex and returns a pointer to it.  If it returns NULL
** that means that a mutex could not be allocated. 
** SQLite will unwind its stack and return an error.  The argument
** to sqlite3_mutex_alloc() is one of these integer constants:
**
** <ul>
** <li>  SQLITE_MUTEX_FAST               0
** <li>  SQLITE_MUTEX_RECURSIVE          1
** <li>  SQLITE_MUTEX_STATIC_MASTER      2
** <li>  SQLITE_MUTEX_STATIC_MEM         3
** <li>  SQLITE_MUTEX_STATIC_PRNG        4
** <li>  SQLITE_MUTEX_STATIC_LRU
** </ul>
**
** The first two constants cause sqlite3_mutex_alloc() to create
** a new mutex.  The new mutex is recursive when SQLITE_MUTEX_RECURSIVE
** is used but not necessarily so when SQLITE_MUTEX_FAST is used.
** The mutex implementation does not need to make a distinction
** between SQLITE_MUTEX_RECURSIVE and SQLITE_MUTEX_FAST if it does
** not want to.  But SQLite will only request a recursive mutex in
** cases where it really needs one.  If a faster non-recursive mutex
** implementation is available on the host platform, the mutex subsystem
** might return such a mutex in response to SQLITE_MUTEX_FAST.
**
** The other allowed parameters to sqlite3_mutex_alloc() each return
** a pointer to a static preexisting mutex.  Three static mutexes are
** used by the current version of SQLite.  Future versions of SQLite
** may add additional static mutexes.  Static mutexes are for internal
** use by SQLite only.  Applications that use SQLite mutexes should
** use only the dynamic mutexes returned by SQLITE_MUTEX_FAST or
** SQLITE_MUTEX_RECURSIVE.
**
** Note that if one of the dynamic mutex parameters (SQLITE_MUTEX_FAST
** or SQLITE_MUTEX_RECURSIVE) is used then sqlite3_mutex_alloc()
** returns a different mutex on every call.  But for the static
** mutex types, the same mutex is returned on every call that has
** the same type number.
*/
static sqlite3_mutex *otherMutexAlloc(int iType)
{
	sqlite3_mutex *p;

	d1(KPrintF(__FILE__ "/%s/%ld: iType=%ld\n", __FUNC__, __LINE__, iType));

	switch( iType )
		{
	case SQLITE_MUTEX_FAST:
	case SQLITE_MUTEX_RECURSIVE:
		p = sqlite3MallocZero( sizeof(*p) );
		d1(KPrintF(__FILE__ "/%s/%ld: p=%08lx\n", __FUNC__, __LINE__, p));
		if( p )
			{
			p->id = iType;
			InitSemaphore(&p->sema);
			}
		break;

	default:
		{
		static sqlite3_mutex staticMutexes[6];
		static int isInit = 0;

		while( !isInit )
			{
			static long lock = 0;

			Forbid();
			lock++;
			if( lock == 1 )
				{
		                int i;

				Permit();

				for(i = 0; i < sizeof(staticMutexes)/sizeof(staticMutexes[0]); i++)
					{
					InitSemaphore(&staticMutexes[i].sema);
					}
		                isInit = 1;
				}
			else
				{
				Permit();
				Delay(1);
				}
			}
		assert( iType-2 >= 0 );
		assert( iType-2 < sizeof(staticMutexes)/sizeof(staticMutexes[0]) );
		p = &staticMutexes[iType-2];
		p->id = iType;
		d1(KPrintF(__FILE__ "/%s/%ld: p=%08lx\n", __FUNC__, __LINE__, p));
		break;
		}
		}

	d1(KPrintF(__FILE__ "/%s/%ld: p=%08lx\n", __FUNC__, __LINE__, p));

	return p;
}


/*
** This routine deallocates a previously allocated mutex.
** SQLite is careful to deallocate every mutex that it allocates.
*/
static void otherMutexFree(sqlite3_mutex *p)
{
	assert( p );
	assert( p->nRef==0 );
	assert( p->id==SQLITE_MUTEX_FAST || p->id==SQLITE_MUTEX_RECURSIVE );

	d1(KPrintF(__FILE__ "/%s/%ld: p=%08lx  id=%ld\n", __FUNC__, __LINE__, p, p->id));
	sqlite3_free(p);
	d1(KPrintF(__FILE__ "/%s/%ld: p=%08lx\n", __FUNC__, __LINE__, p));
}

/*
** The sqlite3_mutex_enter() and sqlite3_mutex_try() routines attempt
** to enter a mutex.  If another thread is already within the mutex,
** sqlite3_mutex_enter() will block and sqlite3_mutex_try() will return
** SQLITE_BUSY.  The sqlite3_mutex_try() interface returns SQLITE_OK
** upon successful entry.  Mutexes created using SQLITE_MUTEX_RECURSIVE can
** be entered multiple times by the same thread.  In such cases the,
** mutex must be exited an equal number of times before another thread
** can enter.  If the same thread tries to enter any other kind of mutex
** more than once, the behavior is undefined.
*/
static void otherMutexEnter(sqlite3_mutex *p)
{
	assert( p );
	d1(KPrintF(__FILE__ "/%s/%ld: p=%08lx\n", __FUNC__, __LINE__, p));
	assert( p->id==SQLITE_MUTEX_RECURSIVE || sqlite3_mutex_notheld(p) );

	ObtainSemaphore(&p->sema);

	p->owner = FindTask(NULL);
	p->nRef++;
	d1(KPrintF(__FILE__ "/%s/%ld: p=%08lx\n", __FUNC__, __LINE__, p));
}


static int otherMutexTry(sqlite3_mutex *p)
{
	int rc;

	assert( p );
	assert( p->id==SQLITE_MUTEX_RECURSIVE || sqlite3_mutex_notheld(p) );

	d1(KPrintF(__FILE__ "/%s/%ld: p=%08lx\n", __FUNC__, __LINE__, p));
	if (AttemptSemaphore(&p->sema))
		{
		p->owner = FindTask(NULL);
		p->nRef++;
		rc = SQLITE_OK;
		}
	else
		{
		rc = SQLITE_BUSY;
		}

	d1(KPrintF(__FILE__ "/%s/%ld: rc=%ld\n", __FUNC__, __LINE__, rc));

	return rc;
}

/*
** The sqlite3_mutex_leave() routine exits a mutex that was
** previously entered by the same thread.  The behavior
** is undefined if the mutex is not currently entered or
** is not currently allocated.  SQLite will never do either.
*/
static void otherMutexLeave(sqlite3_mutex *p)
{
	struct Task *tid;

	d1(KPrintF(__FILE__ "/%s/%ld: p=%08lx\n", __FUNC__, __LINE__, p));
	assert( p );
	assert( p->nRef > 0 );
	tid = FindTask(NULL);
	assert( p->owner==tid );
	p->nRef--;
	assert( p->nRef==0 || p->id==SQLITE_MUTEX_RECURSIVE );
	ReleaseSemaphore(&p->sema);
	d1(KPrintF(__FILE__ "/%s/%ld: p=%08lx\n", __FUNC__, __LINE__, p));
}

#ifdef SQLITE_DEBUG
/*
** The sqlite3_mutex_held() and sqlite3_mutex_notheld() routine are
** intended for use inside assert() statements.
*/
static int otherMutexHeld(sqlite3_mutex *p)
{
	struct Task *tid;

	tid = FindTask(NULL);
	return p==NULL || (p->nRef!=0 && p->owner==tid);
}

static int otherMutexNotheld(sqlite3_mutex *p)
{
	struct Task *tid;

	tid = FindTask(NULL);

	return p==NULL || p->nRef==0 || p->owner!=tid;
}
#endif /* SQLITE_DEBUG */

sqlite3_mutex_methods *sqlite3OtherMutex(void)
{
	static sqlite3_mutex_methods sMutex =
	{
	otherMutexInit,
	otherMutexEnd,
	otherMutexAlloc,
	otherMutexFree,
	otherMutexEnter,
	otherMutexTry,
	otherMutexLeave,
#ifdef SQLITE_DEBUG
	otherMutexHeld,
	otherMutexNotheld
#endif /* SQLITE_DEBUG */
	};

	d1(KPrintF(__FILE__ "/%s/%ld: \n", __FUNC__, __LINE__));

	return &sMutex;
}

#endif /* SQLITE_MUTEX_OMIT */
#endif /* SQLITE_OS_OTHER */

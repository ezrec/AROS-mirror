#ifndef _PTHREAD_H_
#define _PTHREAD_H_

/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id: $
*/

#include <semaphore.h>
#include <sys/cdefs.h>
#include <time.h>

struct timespec
{
    time_t tv_sec;  /* seconds */
    long   tv_nsec; /* nanoseconds */
};

struct sched_param {
    int sched_priority;
};

typedef IPTR pthread_t;

typedef struct
{
} pthread_attr_t;

typedef struct
{
} pthread_cond_t;

typedef struct
{
} pthread_condattr_t;

#define PTHREAD_MUTEX_ALREADY_RECURSIVE 1

typedef struct
{
	BOOL Initialized;
	struct SignalSemaphore sigSem;
} pthread_mutex_t;

typedef struct
{
} pthread_mutexattr_t;

enum
{
    PTHREAD_CANCEL_ENABLE,
    PTHREAD_CANCEL_DISABLE,
    PTHREAD_CANCEL_DEFERRED,
    PTHREAD_CANCEL_ASYNCHRONOUS,
    PTHREAD_MUTEX_RECURSIVE,
};

/*
__BEGIN_DECLS

extern int pthread_create(
    pthread_t*            thread,
    const pthread_attr_t* attr,
    void*                 (*func) (void*),
    void*                 arg);

extern int pthread_cancel(pthread_t thread);

extern int pthread_equal(pthread_t t1, pthread_t t2);

extern int pthread_join(pthread_t thread, void** exit_value);

extern pthread_t pthread_self(void);

extern int pthread_suspend_np(pthread_t thread);

extern int pthread_continue_np(pthread_t thread);

extern int pthread_setcancelstate(int newstate, int* oldstate);

extern int pthread_setcanceltype(int newtype, int* oldtype);

extern int pthread_attr_init(pthread_attr_t* attr);

extern int pthread_attr_destroy(pthread_attr_t* attr);

extern int pthread_attr_setstacksize(pthread_attr_t* attr, size_t stacksize);

extern int pthread_cond_destroy(pthread_cond_t* cond);

extern int pthread_cond_init(
    pthread_cond_t*           cond,
    const pthread_condattr_t* attr);

extern int pthread_cond_signal(pthread_cond_t* cond);

extern int pthread_cond_timedwait(
    pthread_cond_t*         cond,
    pthread_mutex_t*        mutex,
    const struct timespec* abstime);

extern int pthread_mutex_init(
    pthread_mutex_t*           mutex,
    const pthread_mutexattr_t* attr);

extern int pthread_mutex_destroy(pthread_mutex_t* mutex);

extern int pthread_mutex_lock(pthread_mutex_t* mutex);

extern int pthread_mutex_unlock(pthread_mutex_t* mutex);

extern int pthread_mutexattr_destroy(pthread_mutexattr_t* attr);
    
extern int pthread_mutexattr_init(pthread_mutexattr_t* attr);
    
extern int pthread_mutexattr_settype(pthread_mutexattr_t* attr, int type);

__END_DECLS
*/
#endif /* _PTHREAD_H_ */

 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Thread support using AmigaOS processes
  *
  * Copyright 2003-2004 Richard Drummond
  */

/*
 * Simple emulation of POSIX sempahores use exec semaphores.
 * amd message passing.
 */
#include <exec/semaphores.h>
#include <exec/lists.h>

struct PSemaphore {
   struct SignalSemaphore mutex;
   struct MinList         wait_queue;
   int                    value;
   int                    live;
};

typedef struct PSemaphore uae_sem_t;

int uae_sem_init (uae_sem_t *sem, int pshared, unsigned int value);

int uae_sem_destroy (uae_sem_t *sem);

int uae_sem_post (uae_sem_t *sem);

int uae_sem_wait (uae_sem_t *sem);

int uae_sem_trywait (uae_sem_t *sem);

int uae_sem_get_value (uae_sem_t *sem);


#include "commpipe.h"

/*
 * Threading using AmigaOS processes
 */
typedef struct Task *uae_thread_id;
#define BAD_THREAD NULL

#define uae_set_thread_priority(pri)

extern int uae_start_thread (void *(*f) (void *), void *arg, uae_thread_id *foo);

/* Do nothing; thread exits if thread function returns.  */
#define UAE_THREAD_EXIT do {} while (0)

#define uae_wait_thread (id)

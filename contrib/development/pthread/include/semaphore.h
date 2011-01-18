#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <sys/cdefs.h>

typedef struct
{
    int value;
} sem_t;

/*
__BEGIN_DECLS

int sem_destroy(sem_t* sem);
int sem_init(sem_t* sem, int shared, unsigned int value);
int sem_post(sem_t* sem);
int sem_wait(sem_t* sem);

__END_DECLS*/

#endif /* _SEMAPHORE_H_ */

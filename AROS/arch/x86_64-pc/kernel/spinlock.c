#include "spinlock.h"

void spinlock_init(spinlock_t *lock)
{
    *lock = 0;
}

void spinlock_release(spinlock_t *lock)
{
    *lock = 0;
}

int spinlock_is_locked(spinlock_t *lock)
{
    return (*lock & 1) == 1;
}

void spinlock_acquire(spinlock_t *lock)
{
    asm volatile ("lock btsl $0, %1\n\t"
	"jnc 1f\n\t"
	"\n2: pause\n\t"
	"testl $1, %1\n\t"
	"je 2b\n\t"
	"lock btsl $0, %1\n\t"
	"jc 2b\n\t"
	"\n1:"
	:"+m"(*lock)::"memory");
}
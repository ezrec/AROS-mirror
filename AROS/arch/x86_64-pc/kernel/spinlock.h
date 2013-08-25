#ifndef ASM_SPINLOCK_H
#define ASM_SPINLOCK_H

#include <stdint.h>

/* According to Intel documentation, it's best to align spinlocks on 128-byte boundary */
typedef uint32_t __attribute__((__aligned__(128)))	spinlock_t;

void spinlock_init(spinlock_t *lock);
void spinlock_acquire(spinlock_t *lock);
void spinlock_release(spinlock_t *lock);
int  spinlock_is_locked(spinlock_t *lock);

#endif /* ASM_SPINLOCK_H */

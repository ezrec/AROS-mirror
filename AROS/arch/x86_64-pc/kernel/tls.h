#ifndef ASM_TLS_H
#define ASM_TLS_H

typedef struct tls
{
    struct ExecBase     *SysBase;
    void *              KernelBase;    /* Base of kernel.resource */
    void **             CPUStorage;    /* Per-CPU private data */
} tls_t;

#define TLS_OFFSET(name) ((char *)&(((tls_t *)0)->name)-(char *)0)

#define TLS_GET(name) \
    ({ \
        tls_t *__tls = (tls_t *)0; \
        typeof(__tls -> name) __ret; \
        switch (sizeof(__tls -> name)) { \
case 1: asm volatile("movb %%gs:%P1,%b0":"=q"(__ret):"0"(0),"i"(TLS_OFFSET(name)):"memory"); break; \
case 4: asm volatile("movl %%gs:%P1,%0":"=r"(__ret):"i"(TLS_OFFSET(name)):"memory"); break; \
case 8: asm volatile("movq %%gs:%P1,%q0":"=r"(__ret):"i"(TLS_OFFSET(name)):"memory"); break; \
        } \
        __ret;  \
    })

#define TLS_SET(name, val) \
    do { \
        tls_t *__tls = (tls_t *)0; \
        typeof(__tls -> name) __set = val; \
        switch (sizeof(__tls -> name)) { \
case 1: asm volatile("movb %b0,%%gs:%P1"::"iq"(__set),"i"(TLS_OFFSET(name)):"memory"); break; \
case 4: asm volatile("movl %0,%%gs:%P1"::"ir"(__set),"i"(TLS_OFFSET(name)):"memory"); break; \
case 8: asm volatile("movq %q0,%%gs:%P1"::"ir"(__set),"i"(TLS_OFFSET(name)):"memory"); break; \
        } \
    } while(0);

#endif

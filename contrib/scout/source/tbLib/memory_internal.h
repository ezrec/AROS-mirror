#ifndef TBLIB_MEMORY_INTERNAL_H
#define TBLIB_MEMORY_INTERNAL_H 1

void * ASM AsmCreatePool( REG(d0, ULONG),
                          REG(d1, ULONG),
                          REG(d2, ULONG),
                          REG(a6, struct Library *) );
APTR ASM AsmAllocPooled( REG(a0, APTR poolHeader),
                         REG(d0, ULONG memSize),
                         REG(a6, struct Library *) );
void ASM AsmFreePooled( REG(a0, APTR poolHeader),
                        REG(a1, APTR memory),
                        REG(d0, ULONG memSize),
                        REG(a6, struct Library *) );
void ASM AsmDeletePool( REG(a0, void *),
                        REG(a6, struct Library *) );

#define TB_POOL_MAGIC   0xa0b18c9e

#endif /* TBLIB_MEMORY_INTERNAL_H */


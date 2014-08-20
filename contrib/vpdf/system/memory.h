#ifndef	SYS_MEMORY_H
#define SYS_MEMORY_H

void *mmalloc(int size);
void *mcalloc(int size, int num);
void mfree(void *mem);

void MemoryInit(void);

#endif

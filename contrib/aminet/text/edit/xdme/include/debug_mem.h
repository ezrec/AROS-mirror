
#ifndef DEBUG_MEM_H
#define DEBUG_MEM_H

#ifndef Prototype
#define Prototype extern
#endif

Prototype void * _debug_AllocMem (long, long, const char *);
Prototype void _debug_FreeMem (void *, long, const char *);
Prototype void _debug_FreeAllMem (void);

#ifndef DEBUG_C
#ifdef AllocMem
#undef AllocMem
#endif
#define AllocMem(s,r)    _debug_AllocMem(s,r,"AllocMem() File: " __FILE__ " Line: " #__LINE__)

#ifdef FreeMem
#undef FreeMem
#endif
#define FreeMem(p,s)    _debug_FreeMem(p,s,"FreeMem() File: " __FILE__ " Line: " #__LINE__)
#endif

#endif

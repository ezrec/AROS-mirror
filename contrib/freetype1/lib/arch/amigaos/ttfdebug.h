#ifndef _ARCH_AMIGAOS_TTFDEBUG_H
#define _ARCH_AMIGAOS_TTFDEBUG_H

#ifdef __AROS__
#include <aros/debug.h>
#else
#   define SHOWDEBUG 1
#   undef SHOWDEBUG

#   ifdef SHOWDEBUG
void kprintf(UBYTE *fmt,...);
#       define D(x) (x)
#       define bug kprintf
#   else
#       define D(x) ;
#   endif
#   define D()
#endif

#endif /* _ARCH_AMIGAOS_TTFDEBUG_H */


#ifndef _LIBRARY_DEBUG_H
#define _LIBRARY_DEBUG_H

#ifdef DEBUG
#define DB(x)	x
#else
#define DB(x)
#endif

#ifdef __MORPHOS__
	void dprintf(char *, ... );
	#undef kprintf
	#define kprintf dprintf
#elif defined(__AROS__)
    	#include <aros/debug.h>
#else
	VOID kprintf(STRPTR,...);
#endif

#endif

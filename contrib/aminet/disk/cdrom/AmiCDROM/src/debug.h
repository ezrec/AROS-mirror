#ifndef ACDR_DEBUG_H
#define ACDR_DEBUG_H

#define NDEBUG
//#define DEBUG_SECTORS

#ifdef __AROS__
#	undef DEBUG
#	define DEBUG 0
#	include <aros/debug.h>
#	if DEBUG>0
#		define BUG(x) x
#		define dbprintf kprintf
#	else
#		define BUG(x)
#	endif
#	define BUG2(x)
#else
#	ifdef NDEBUG
#		define BUG(x) /* nothing */
#	else
#		define BUG(x) x
#	endif
#	if !defined(NDEBUG) || defined(DEBUG_SECTORS)
#		define BUG2(x) x
#	else
#		define BUG2(x) /* nothing */
#	endif
#endif

#endif /* ACDR_DEBUG_H */

#ifndef ACDR_DEBUG_H
#define ACDR_DEBUG_H

#define NDEBUG
//#define DEBUG_SECTORS

#ifdef NDEBUG
#	define BUG(x) /* nothing */
#else
#	define BUG(x) x
#endif

#if !defined(NDEBUG) || defined(DEBUG_SECTORS)
#	define BUG2(x) x
#else
#	define BUG2(x) /* nothing */
#endif

#endif /* ACDR_DEBUG_H */

/*
 * $Header$
 * $Log$
 * Revision 1.2  2001/04/05 16:57:00  stegerg
 * don't include <malloc.h> as this include does not exist in AROS
 *
 * Revision 1.1  2001/04/04 05:43:36  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 * Revision 1.1  1998/07/02 17:35:50  bnv
 * Initial revision
 *
 */

#ifndef __BMEM_H__
#define __BMEM_H__

#include <stdlib.h>
#ifndef AROS
#include <malloc.h>
#endif

#ifdef __DEBUG__
#	define	MALLOC(s,d)	mem_malloc(s,d)
#	define	REALLOC(p,s)	mem_realloc(p,s)
#	define	FREE		mem_free
#else
#	if defined(__BORLANDC__) && (defined(__HUGE__) || defined(__LARGE__))
#		define	MALLOC(s,d)	farmalloc(s)
#		define	REALLOC(p,s)	farrealloc(p,s)
#		define	FREE		farfree
#	else
#		define	MALLOC(s,d)	malloc(s)
#		define	REALLOC(p,s)	realloc(p,s)
#		define	FREE		free
#	endif
#endif

/* ------ function prototypes --------- */
#ifdef __DEBUG__
void	*mem_malloc(size_t size, char *desc);
void	*mem_realloc(void *ptr, size_t size);
void	mem_free(void *ptr);
void	mem_list(void);
long	mem_allocated(void);
#endif

#endif

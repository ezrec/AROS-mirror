/*
 * $Header$
 * $Log$
 * Revision 1.1  2001/04/04 05:43:37  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 * Revision 1.1  1998/07/02 17:35:50  bnv
 * Initial revision
 *
 */

#ifndef __STACK_H__
#define __STACK_H__

#include <lstring.h>
#include <dqueue.h>

/* ---- function prototypes ---- */
void	CreateStack( void );
void	DeleteStack( void );
void	Queue2Stack( PLstr str );
void	Push2Stack( PLstr str );
PLstr	PullFromStack( void );
long	StackQueued( void );

#endif

/*
 * $Header$
 * $Log$
 * Revision 1.1  2001/04/04 05:43:36  wang
 * First commit: compiles on Linux, Amiga, Windows, Windows CE, generic gcc
 *
 * Revision 1.2  1999/11/29 14:58:00  bnv
 * Changed: Some defines
 *
 * Revision 1.1  1998/07/02 17:35:50  bnv
 * Initial revision
 *
 */

#ifndef __INTERPRET_H__
#define __INTERPRET_H__

#include <lstring.h>
#include <bintree.h>

#ifdef  __INTERPRET_C__
#	define EXTERN
#else
#	define EXTERN extern
#endif

#ifndef ALIGN
#	define CIPTYPE	byte
#else
#	define CIPTYPE	dword
#endif

/* -------- variables definitions ------ */
EXTERN	int	Rx_proc;		/* current procedure	*/
EXTERN	int	Rx_id;			/* current program id	*/
EXTERN	CIPTYPE	*Rxcodestart;		/* actual code          */
EXTERN	CIPTYPE	*Rxcip;			/* instruction pointer	*/

/* -------- function prototypes ------- */
void	RxInitInterpret( void );
void	RxDoneInterpret( void );
int	RxInterpret( void );

#undef EXTERN
#endif

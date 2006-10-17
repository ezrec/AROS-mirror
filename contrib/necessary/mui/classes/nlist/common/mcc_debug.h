/*
** debug functions header
*/

#ifndef MCC_DEBUG_H
#define MCC_DEBUG_H

#ifdef WITH_DEBUG

#include <clib/exec_protos.h>
#include <clib/dos_protos.h>

void kprintf( const char *fmt, ... );

#define	DEBTIME		0
#define D(x)		do { x; if ( DEBTIME > 0 ) Delay( DEBTIME ); } while( 0 )

#ifdef __GNUC__
 #define	bugn		kprintf
 #define	bug			kprintf( "%.20s() ==> ", __FUNCTION__ ); kprintf
#else
 #define	bugn		kprintf
 #define	bug			kprintf( "%.10s - %.20s() - %5ld ==> ", __FILE__, __FUNC__, __LINE__ ); kprintf
#endif

#else
#define D(x)

#endif	/* WITH_DEBUG */

#endif /* MCC_DEBUG_H */


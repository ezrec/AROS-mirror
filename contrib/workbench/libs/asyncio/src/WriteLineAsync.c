#include "async.h"


_CALL LONG
WriteLineAsync( _REG( a0 ) AsyncFile *file, _REG( a1 ) STRPTR line )
{
	/* Since SAS/C have an inlined strlen... */
#if defined( NOEXTERNALS ) && !defined( __SAS )
	LONG	i = 0;
	STRPTR	s = line;

	while( *s )
	{
		++i, ++s;
	}

	return( WriteAsync( file, line, i ) );
#else
	return( WriteAsync( file, line, strlen( line ) ) );
#endif
}

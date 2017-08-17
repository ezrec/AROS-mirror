#include "async.h"


_CALL LONG
ReadCharAsync( _REG( a0 ) AsyncFile *file )
{
	UBYTE	ch;

	if( file->af_BytesLeft )
	{
		/* if there is at least a byte left in the current buffer, get it
		 * directly. Also update all counters
		 */

		ch = *file->af_Offset;
		--file->af_BytesLeft;
		++file->af_Offset;

		return( ( LONG ) ch );
	}

	/* there were no characters in the current buffer, so call the main read
	 * routine. This has the effect of sending a request to the file system to
	 * have the current buffer refilled. After that request is done, the
	 * character is extracted for the alternate buffer, which at that point
	 * becomes the "current" buffer
	 */

	if( ReadAsync( file, &ch, 1 ) > 0 )
	{
		return( ( LONG ) ch );
	}

	/* We couldn't read above, so fail */

	return( -1 );
}

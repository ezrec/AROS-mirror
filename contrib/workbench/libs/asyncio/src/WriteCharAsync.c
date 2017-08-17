#include "async.h"


_CALL LONG
WriteCharAsync( _REG( a0 ) AsyncFile *file, _REG( d0 ) UBYTE ch )
{
	if( file->af_BytesLeft )
	{
		/* if there's any room left in the current buffer, directly write
		 * the byte into it, updating counters and stuff.
		 */

		*file->af_Offset = ch;
		--file->af_BytesLeft;
		++file->af_Offset;

		/* one byte written */
		return( 1 );
	}

	/* there was no room in the current buffer, so call the main write
	 * routine. This will effectively send the current buffer out to disk,
	 * wait for the other buffer to come back, and then put the byte into
	 * it.
	 */

	{
		TEXT	c;

		c = ch;		/* SAS/C workaround... */

		return( WriteAsync( file, &c, 1 ) );
	}
}

#include "async.h"


_LIBCALL LONG
PeekAsync( _REG( a0 ) AsyncFile *file, _REG( a1 ) APTR buffer, _REG( d0 ) LONG numBytes )
{
#ifdef ASIO_NOEXTERNALS
	struct ExecBase	*SysBase = file->af_SysBase;
#endif

	/* Try to fill a new buffer, if needed */
	if( !file->af_BytesLeft )
	{
		LONG	bytes;

		if( ( bytes = ReadAsync( file, &bytes, 1 ) ) <= 0 )
		{
			return( bytes );
		}

		/* Unread byte */
		--file->af_Offset;
		++file->af_BytesLeft;
	}

	/* Copy what we can */
	numBytes = MIN( numBytes, file->af_BytesLeft );
	CopyMem( file->af_Offset, buffer, numBytes );
	return( numBytes );
}

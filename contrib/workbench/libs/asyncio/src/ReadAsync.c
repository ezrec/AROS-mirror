#include "async.h"


_LIBCALL LONG
ReadAsync( _REG( a0 ) AsyncFile *file, _REG( a1 ) APTR buffer, _REG( d0 ) LONG numBytes )
{
#ifdef ASIO_NOEXTERNALS
	struct ExecBase	*SysBase = file->af_SysBase;
#endif
	LONG totalBytes = 0;
	LONG bytesArrived;

	/* if we need more bytes than there are in the current buffer, enter the
	 * read loop
	 */

	while( numBytes > file->af_BytesLeft )
	{
		/* drain buffer */
		CopyMem( file->af_Offset, buffer, file->af_BytesLeft );

		numBytes		-= file->af_BytesLeft;
		buffer			=  ( APTR ) ( ( ULONG ) buffer + file->af_BytesLeft );
		totalBytes		+= file->af_BytesLeft;
		file->af_BytesLeft	=  0;

		bytesArrived = AS_WaitPacket( file );

		if( bytesArrived <= 0 )
		{
			if( bytesArrived == 0 )
			{
				return( totalBytes );
			}

			return( -1 );
		}

		/* ask that the buffer be filled */
		AS_SendPacket( file, file->af_Buffers[ 1 - file->af_CurrentBuf ] );

		/* in case we tried to seek past EOF */
		if( file->af_SeekOffset > bytesArrived )
		{
			file->af_SeekOffset = bytesArrived;
		}

		file->af_Offset		= file->af_Buffers[ file->af_CurrentBuf ] + file->af_SeekOffset;
		file->af_CurrentBuf	= 1 - file->af_CurrentBuf;
		file->af_BytesLeft	= bytesArrived - file->af_SeekOffset;
		file->af_SeekOffset	= 0;
	}

	CopyMem( file->af_Offset, buffer, numBytes );
	file->af_BytesLeft	-= numBytes;
	file->af_Offset		+= numBytes;

	return( totalBytes + numBytes );
}

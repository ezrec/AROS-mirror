#include "async.h"


_LIBCALL LONG
WriteAsync( _REG( a0 ) AsyncFile *file, _REG( a1 ) APTR buffer, _REG( d0 ) LONG numBytes )
{
#ifdef ASIO_NOEXTERNALS
	struct ExecBase	*SysBase = file->af_SysBase;
#endif
	LONG totalBytes = 0;

	/* this takes care of NIL: */
	if( !file->af_Handler )
	{
		file->af_Offset		= file->af_Buffers[ 0 ];
		file->af_BytesLeft	= file->af_BufferSize;
		return( numBytes );
	}

	while( numBytes > file->af_BytesLeft )
	{
		if( file->af_BytesLeft )
		{
			CopyMem( buffer, file->af_Offset, file->af_BytesLeft );

			numBytes	-= file->af_BytesLeft;
			buffer		=  ( APTR ) ( ( ULONG ) buffer + file->af_BytesLeft );
			totalBytes	+= file->af_BytesLeft;
		}

		if( AS_WaitPacket( file ) < 0 )
		{
			return( -1 );
		}

		/* send the current buffer out to disk */
		AS_SendPacket( file, file->af_Buffers[ file->af_CurrentBuf ] );

		file->af_CurrentBuf	= 1 - file->af_CurrentBuf;
		file->af_Offset		= file->af_Buffers[ file->af_CurrentBuf ];
		file->af_BytesLeft	= file->af_BufferSize;
	}

	CopyMem( buffer, file->af_Offset, numBytes );
	file->af_BytesLeft	-= numBytes;
	file->af_Offset		+= numBytes;

	return ( totalBytes + numBytes );
}

#include "async.h"


_LIBCALL LONG
CloseAsync( _REG( a0 ) AsyncFile *file )
{
	LONG	result;

	if( file )
	{
#ifdef ASIO_NOEXTERNALS
		struct ExecBase		*SysBase = file->af_SysBase;
		struct DosLibrary	*DOSBase = file->af_DOSBase;
#endif
		result = AS_WaitPacket( file );

		if( result >= 0 )
		{
			if( !file->af_ReadMode )
			{
				/* this will flush out any pending data in the write buffer */
				if( file->af_BufferSize > file->af_BytesLeft )
				{
					result = Write(
						file->af_File,
						file->af_Buffers[ file->af_CurrentBuf ],
						file->af_BufferSize - file->af_BytesLeft );
				}
			}
		}

		if( file->af_CloseFH )
		{
			Close( file->af_File );
		}

		FreeVec(file);
	}
	else
	{
#ifndef ASIO_NOEXTERNALS
		SetIoErr( ERROR_INVALID_LOCK );
#endif
		result = -1;
	}

	return( result );
}

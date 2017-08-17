#include "async.h"


#ifdef ASIO_NOEXTERNALS
_LIBCALL AsyncFile *
OpenAsync(
	_REG( a0 ) const STRPTR fileName,
	_REG( d0 ) OpenModes mode,
	_REG( d1 ) LONG bufferSize,
	_REG( a1 ) struct ExecBase *SysBase,
	_REG( a2 ) struct DosLibrary *DOSBase )
#else
_LIBCALL AsyncFile *
OpenAsync(
	_REG( a0 ) const STRPTR fileName,
	_REG( d0 ) OpenModes mode,
	_REG( d1 ) LONG bufferSize )
#endif
{
	static const WORD PrivateOpenModes[] =
	{
		MODE_OLDFILE, MODE_NEWFILE, MODE_READWRITE
	};
	BPTR		handle;
	AsyncFile	*file = NULL;

	if( handle = Open( fileName, PrivateOpenModes[ mode ] ) )
	{
#ifdef ASIO_NOEXTERNALS
		file = AS_OpenAsyncFH( handle, mode, bufferSize, TRUE, SysBase, DOSBase );
#else
		file = AS_OpenAsyncFH( handle, mode, bufferSize, TRUE );
#endif

		if( !file )
		{
			Close( handle );
		}
	}

	return( file );
}

#include "async.h"


#ifdef ASIO_NOEXTERNALS
_CALL AsyncFile *
OpenAsyncFromFH(
	_REG( a0 ) BPTR handle,
	_REG( d0 ) OpenModes mode,
	_REG( d1 ) LONG bufferSize,
	_REG( a1 ) struct ExecBase *SysBase,
	_REG( a2 ) struct DosLibrary *DOSBase )
{
	return( AS_OpenAsyncFH( handle, mode, bufferSize, FALSE, SysBase, DOSBase ) );
}
#else
_CALL AsyncFile *
OpenAsyncFromFH( _REG( a0 ) BPTR handle, _REG( d0 ) OpenModes mode, _REG( d1 ) LONG bufferSize )
{
	return( AS_OpenAsyncFH( handle, mode, bufferSize, FALSE ) );
}
#endif

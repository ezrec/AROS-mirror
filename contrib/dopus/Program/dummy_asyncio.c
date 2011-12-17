/* AROS: This is dummy implementation for the asyncio library just to prevent
linker errors */
// FIXME: We should port the real asyncio.lib, especially since
//        ABIv1 has full DOS Packet support


#include "dopus.h"
#include "asyncio.h"


struct AsyncFile *OpenAsync(const STRPTR fileName, UBYTE accessMode, LONG bufferSize)
{
	return NULL;
}

LONG ReadAsync(struct AsyncFile *file, APTR buffer, LONG numBytes)
{
	return 0;
}

LONG CloseAsync(struct AsyncFile *file)
{
	return 0;
}

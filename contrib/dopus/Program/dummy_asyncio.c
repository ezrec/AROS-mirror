/* AROS: This is dummy implementation for the asyncio library just to prevent
linker errors */
#warning We should port the real asyncio.lib


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

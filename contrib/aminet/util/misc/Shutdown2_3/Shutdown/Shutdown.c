#include "shutdownbase.h"

struct Library *ShutdownBase;

void
main()
{
	if(ShutdownBase = (struct Library *)OpenLibrary("shutdown.library",2))
	{
		Shutdown(SHUTDOWN_NORMAL);

		CloseLibrary(ShutdownBase);
	}
}

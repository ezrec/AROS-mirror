
#include "lib_init.h"
#include "lib_debug.h"
#include <proto/exec.h>

struct Library *UtilityBase = NULL;

LIBAPI BOOL Library_Init(LIB_BASE_T *LibBase)
{
	UtilityBase = OpenLibrary("utility.library", 0);
	if (UtilityBase)
	{
		return TRUE;
	}
	return FALSE;
}

LIBAPI void Library_Exit(LIB_BASE_T *LibBase)
{
	CloseLibrary(UtilityBase);	
}

#include "libnixstuff.h"

void libnixopen(void)
{
	SysBase = *(struct Library **)4;
	__DOSBase = OpenLibrary("dos.library", 0);
	__UtilityBase = OpenLibrary("utility.library", 0);
}

void libnixclose(void)
{
	if(__DOSBase) CloseLibrary(__DOSBase);
	if(__UtilityBase) CloseLibrary(__UtilityBase);
}

void exit(void)
{
	// bit wary of putting anything here... shouldn't be called anyway
}

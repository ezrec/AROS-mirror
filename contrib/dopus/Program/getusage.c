#include "dopus.h"


ULONG oldidle;
ULONG olddisp;

int getusage()
{
	LONG idlediff, dispdiff;
	
	idlediff = SysBase->IdleCount - oldidle;
	dispdiff = SysBase->DispCount - olddisp;
	
	oldidle = SysBase->IdleCount;
	olddisp = SysBase->DispCount;
	
	return (idlediff / (idlediff + dispdiff)) * 100;
	
}

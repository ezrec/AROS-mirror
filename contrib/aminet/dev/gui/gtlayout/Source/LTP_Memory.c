/*
**	GadTools layout toolkit
**
**	Copyright © 1993-1998 by Olaf `Olsen' Barthel
**		Freely distributable.
**
**	:ts=4
*/

#ifndef _GTLAYOUT_GLOBAL_H
#include "gtlayout_global.h"
#endif

#include "Assert.h"


/*****************************************************************************/


APTR
LTP_Alloc(LayoutHandle *handle,ULONG amount)
{
	APTR result;

	if(amount > 0)
		result = AsmAllocPooled(handle->Pool,amount,SysBase);
	else
		result = NULL;

	if(!result)
		handle->Failed = TRUE;

	return(result);
}


/*****************************************************************************/


VOID
LTP_Free(LayoutHandle *handle,APTR mem,ULONG memsize)
{
	if(mem && memsize > 0)
		AsmFreePooled(handle->Pool,mem,memsize,SysBase);
}

#include <OSIncludes.h>

#ifndef _AROS
#pragma header
#endif

#include <string.h>

#include "pui.h"
#include "classmacros.h"
#include "labelclass.h"

ULONG LabelInstanceSize(void)
{
	return sizeof(struct labeldata);
}

ULONG LabelDispatcher(struct IClass *cl,Object *obj,Msg msg)
{
	ULONG rc;
	
	rc = 0;
	
	switch(msg->MethodID)
	{
		case GM_HITTEST:
			rc = 0;
			break;
		
		default:
			rc = DoSuperMethodA(cl,obj,msg);
			break;

	} /* switch(msg->MethodID) */
	
	return rc;
}



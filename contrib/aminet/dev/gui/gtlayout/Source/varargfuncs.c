#ifndef _GTLAYOUT_GLOBAL_H
#include "gtlayout_global.h"
#endif

#include <aros/system.h>
#include "aros/arosspecific.h"
#include "aros/gtlayout_defines_aros.h"

/*****************************************************************************/

#include <stdarg.h>

/*****************************************************************************/

#include "Assert.h"

/*****************************************************************************/

#undef AROS_TAGRETURNTYPE
#define AROS_TAGRETURNTYPE  LayoutHandle *
LayoutHandle *
LT_CreateHandleTags(struct Screen *Screen, unsigned long tag1,...)
{
	AROS_SLOWSTACKTAGS_PRE(tag1)
	retval = LT_CreateHandleTagList(Screen,AROS_SLOWSTACKTAGS_ARG(tag1));
	AROS_SLOWSTACKTAGS_POST
}
#undef AROS_TAGRETURNTYPE

#define AROS_TAGRETURNTYPE VOID
VOID
LT_New(LayoutHandle *handle, unsigned long tag1,...)	/* link library only */
{
//	AROS_SLOWSTACKTAGS_PRE(tag1)
	LT_NewA(handle,AROS_SLOWSTACKTAGS_ARG(tag1));
//	AROS_SLOWSTACKTAGS_POST
}
#undef AROS_TAGRETURNTYPE

#define AROS_TAGRETURNTYPE struct Window *
struct Window *
LT_Layout(LayoutHandle *handle,STRPTR title,struct IBox *bounds,LONG extraWidth,LONG extraHeight,ULONG IDCMP,LONG align, unsigned long tag1, ...)
{
	AROS_SLOWSTACKTAGS_PRE(tag1)
	
	retval = LT_Build(handle,
		LAWN_Title,			title,
		LAWN_Bounds,		bounds,
		LAWN_ExtraWidth,	extraWidth,
		LAWN_ExtraHeight,	extraHeight,
		LAWN_IDCMP,			IDCMP,
		LAWN_AlignWindow,	align,
		TAG_MORE,AROS_SLOWSTACKTAGS_ARG(tag1));
	AROS_SLOWSTACKTAGS_POST

}
#undef AROS_TAGRETURNTYPE

#define AROS_TAGRETURNTYPE struct Window *
struct Window *
LT_Build(LayoutHandle *Handle, unsigned long tag1, ...)
{
	AROS_SLOWSTACKTAGS_PRE(tag1)
	retval = LT_BuildA(Handle,AROS_SLOWSTACKTAGS_ARG(tag1));
	AROS_SLOWSTACKTAGS_POST
}
#undef AROS_TAGRETURNTYPE

#define AROS_TAGRETURNTYPE VOID
VOID
LT_SetAttributes(LayoutHandle *handle,LONG id, unsigned long tag1,...)
{
//	AROS_SLOWSTACKTAGS_PRE(tag1)
	LT_SetAttributesA(handle,id,AROS_SLOWSTACKTAGS_ARG(tag1));
//	AROS_SLOWSTACKTAGS_POST
}
#undef AROS_TAGRETURNTYPE

#define AROS_TAGRETURNTYPE LONG
LONG
LT_GetAttributes(LayoutHandle *Handle,LONG ID, unsigned long tag1,...)
{
	AROS_SLOWSTACKTAGS_PRE(tag1)
	retval = LT_GetAttributesA(Handle,ID,AROS_SLOWSTACKTAGS_ARG(tag1));
	AROS_SLOWSTACKTAGS_POST
}
#undef AROS_TAGRETURNTYPE

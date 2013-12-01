// SeparatorGadgetClass.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/gfxmacros.h>
#include <graphics/gfxbase.h>
#include <graphics/text.h>
#include <graphics/rpattr.h>
#include <libraries/dos.h>
#include <workbench/startup.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <intuition/gadgetclass.h>
#include <intuition/screens.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/iconobject.h>
#include "debug.h"
#include <proto/scalos.h>

#include <clib/alib_protos.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdarg.h>

#include <defs.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// The functions in this module

static SAVEDS(ULONG) INTERRUPT dispatchSeparatorGadgetClass(Class *cl, Object *o, Msg msg);
static ULONG SeparatorGadget_Set(Class *cl, Object *o, Msg msg);
static ULONG SeparatorGadget_Get(Class *cl, Object *o, Msg msg);

//----------------------------------------------------------------------------

/***********************************************************/
/**	Make the class and set up the dispatcher's hook	**/
/***********************************************************/
struct ScalosClass *initSeparatorGadgetClass(const struct PluginClass *plug)
{
	struct ScalosClass *SeparatorGadgetClass;

	SeparatorGadgetClass =  SCA_MakeScalosClass(plug->plug_classname,
			plug->plug_superclassname,
			0,
			0 );
	d1(KPrintF("%s/%s/%ld  SeparatorGadgetClass=%08lx  plug_classname=<%s> plug_superclassname=<%s>\n",
		__FILE__, __FUNC__, __LINE__, SeparatorGadgetClass, plug->plug_classname, plug->plug_superclassname));

	if (SeparatorGadgetClass)
		{
		// initialize the cl_Dispatcher Hook
		SETHOOKFUNC(SeparatorGadgetClass->sccl_class->cl_Dispatcher, dispatchSeparatorGadgetClass);
		}

	return SeparatorGadgetClass;
}

//----------------------------------------------------------------------------

/**************************************************************************/
/**********	   The SeparatorGadgetCLASS class dispatcher	     *********/
/**************************************************************************/
static SAVEDS(ULONG) INTERRUPT dispatchSeparatorGadgetClass(Class *cl, Object *o, Msg msg)
{
	ULONG Result;

	d1(KPrintF("%s/%s/%ld  Class=%l08x  SuperClass=%08lx  Method=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, cl->cl_Super, msg->MethodID));

	switch (msg->MethodID)
		{
	case OM_SET:
		Result = SeparatorGadget_Set(cl, o, msg);
		break;
	case OM_GET:
		Result = SeparatorGadget_Get(cl, o, msg);
		break;
	default:
		Result = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------

static ULONG SeparatorGadget_Get(Class *cl, Object *o, Msg msg)
{
	struct opGet *opg = (struct opGet *) msg;
	struct Gadget *gg = (struct Gadget *) o;
	ULONG Success = TRUE;

	if (NULL == opg->opg_Storage)
		return 0;

	*(opg->opg_Storage) = 0;

	switch (opg->opg_AttrID)
		{
	case GA_Left:	// required since gadgetclass attribute is [IS] - no support for [G]
		*(opg->opg_Storage) = (ULONG) gg->LeftEdge;
		break;
	case GA_Top:	// required since gadgetclass attribute is [IS] - no support for [G]
		*(opg->opg_Storage) = (ULONG) gg->TopEdge;
		break;
	case GA_Width:	// required since gadgetclass attribute is [IS] - no support for [G]
		*(opg->opg_Storage) = (ULONG) gg->Width;
		break;
	case GA_Height:	// required since gadgetclass attribute is [IS] - no support for [G]
		*(opg->opg_Storage) = (ULONG) gg->Height;
		break;
	default:
		Success = DoSuperMethodA(cl, o, msg);
		break;
		}

	return Success;
}

//----------------------------------------------------------------------------

static ULONG SeparatorGadget_Set(Class *cl, Object *o, Msg msg)
{
//	struct CycleGadgetInstance *inst = INST_DATA(cl, o);
	struct opSet *ops = (struct opSet *) msg;
	struct TagItem *TagList = ops->ops_AttrList;
	struct TagItem *tag;
	struct Gadget *gg = (struct Gadget *) o;
	BOOL NeedRerender = FALSE;
	ULONG Result;

	while ((tag = NextTagItem(&TagList)))
		{
		switch (tag->ti_Tag)
			{
		case GA_Disabled:
			if ((tag->ti_Data && !(gg->Flags & GFLG_DISABLED)) ||
				(!tag->ti_Data && (gg->Flags & GFLG_DISABLED)))
				{
				d1(KPrintF("%s/%s/%ld  o=%l08x  Flags=%08lx\n", __FILE__, __FUNC__, __LINE__, o, gg->Flags));
				NeedRerender = TRUE;
				}
			break;
		default:
			break;
			}
		}

	Result = DoSuperMethodA(cl, o, msg);

	if (NeedRerender)
		{
		struct RastPort *rp;

		rp = ObtainGIRPort(ops->ops_GInfo);
		d1(KPrintF("%s/%s/%ld:  o=%08lx  rp=%08lx\n", __FILE__, __FUNC__, __LINE__, o, rp));
		if (rp)
			{
			DoMethod(o,
				GM_RENDER,
				ops->ops_GInfo,
				rp,
				GREDRAW_UPDATE);
			ReleaseGIRPort(rp);
			}
		}

	return Result;
}

//----------------------------------------------------------------------------



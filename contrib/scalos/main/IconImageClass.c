// IconImageClass.c
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
#include <intuition/imageclass.h>
#include <intuition/icclass.h>
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
#include <datatypes/iconobject.h>

#include "scalos_structures.h"
#include "functions.h"
#include "Variables.h"

#include "IconImageClass.h"

//----------------------------------------------------------------------------

struct IconImageINST
	{
	Object *sii_IconObject;
	};

//----------------------------------------------------------------------------

// The functions in this module
static SAVEDS(ULONG) INTERRUPT dispatchIconImageClass(Class *cl, Object *o, Msg msg);
static BOOL InitIconImage(struct IconImageINST *inst, const struct opSet *ops, const struct Image *NewImage);
static void DisposeIconImage(struct IconImageINST *inst);
static void DrawIconImage(struct IconImageINST *inst, struct impDraw *Msg, struct Image *myImage);

//----------------------------------------------------------------------------


Class *IconImageClass;


/***********************************************************/
/**	Make the class and set up the dispatcher's hook	**/
/***********************************************************/
Class *initIconImageClass(void)
{
	if (NULL == IconImageClass)
		{
		IconImageClass =  MakeClass( NULL,
				(STRPTR) IMAGECLASS, NULL,
				sizeof(struct IconImageINST),
				0 );

		if (IconImageClass)
			{
			// initialize the cl_Dispatcher Hook
			SETHOOKFUNC(IconImageClass->cl_Dispatcher, dispatchIconImageClass);
			}
		}

	return IconImageClass;
}

/***********************************************************/
/******************	 Free the class	  ****************/
/***********************************************************/
BOOL freeIconImageClass( Class *cl )
{
	BOOL Result = TRUE;

	if (IconImageClass)
		{
		Result = FreeClass(IconImageClass);
		IconImageClass = NULL;
		}

	return Result;
}

/**************************************************************************/
/**********	   The IconImageCLASS class dispatcher	  *********/
/**************************************************************************/
static SAVEDS(ULONG) INTERRUPT dispatchIconImageClass(Class *cl, Object *o, Msg msg)
{
	struct IconImageINST *inst;
	ULONG retval = 0l;
	Object *object;

	d1(kprintf("%s/%s/%ld:  Class=%l08x  SuperClass=%08lx  Method=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, cl->cl_Super, msg->MethodID));

	switch (msg->MethodID)
		{
	case OM_NEW:	   /* First, pass up to superclass */
		if ((object = (Object *) DoSuperMethodA(cl, o, msg)))
			{
			struct opSet *ops = (struct opSet *) msg;

			/* Initial local instance data */
			inst = INST_DATA(cl, object);

			d1(kprintf("%s/%s/%ld: Left=%ld  Top=%ld\n", __FILE__, __FUNC__, __LINE__, \
				((struct Image *) object)->LeftEdge, ((struct Image *) object)->TopEdge));

			if (!InitIconImage(inst, ops, (struct Image *) object))
				{
				DisposeObject(object);
				object = NULL;
				}

			retval = (ULONG) object;
			d1(kprintf("OM_NEW: object=%08lx  inst=%08lx\n", object, inst));
			}
		break;

	case OM_DISPOSE:
		d1(kprintf("OM_DISPOSE: %08lx\n", inst));

		inst = INST_DATA(cl, o);

		DisposeIconImage(inst);

		DoSuperMethodA(cl, o, msg);
		break;

	case IM_DRAW:
		d1(kprintf("%s/%s/%ld: IM_DRAW  State=%ld\n", __FILE__, __FUNC__, __LINE__, ((struct impDraw *) msg)->imp_State));

		inst = INST_DATA(cl, o);

		DrawIconImage(inst, (struct impDraw *) msg, (struct Image *) o);
		break;

	default:	/* IconImageCLASS does not recognize the methodID, let the superclass's */
			/* dispatcher take a look at it. */
		d1(kprintf("%s/%s/%ld: Class=%08lx  Obj=%08lx  msg=%08lx  Method=%08lx\n", __FILE__, __FUNC__, __LINE__, cl, o, msg, msg->MethodID));
		retval = DoSuperMethodA(cl, o, msg);
		d1(kprintf("%s/%s/%ld:\n", __FILE__, __FUNC__, __LINE__));
		break;
		}

	d1(kprintf("%s/%s/%ld:  retval=%l08x\n", __FILE__, __FUNC__, __LINE__, retval));

	return(retval);
}


static BOOL InitIconImage(struct IconImageINST *inst, const struct opSet *ops, const struct Image *NewImage)
{
	memset(inst, 0, sizeof(inst));

	inst->sii_IconObject = (APTR) GetTagData(ICI_IconObj, (ULONG) NULL, ops->ops_AttrList);
	if (NULL == inst->sii_IconObject)
		return FALSE;

	return TRUE;
}


static void DisposeIconImage(struct IconImageINST *inst)
{
	if (inst->sii_IconObject)
		{
		DisposeIconObject(inst->sii_IconObject);
		inst->sii_IconObject = NULL;
		}
}


static void DrawIconImage(struct IconImageINST *inst, struct impDraw *Msg, struct Image *myImage)
{
	struct ExtGadget *gg = (struct ExtGadget *) inst->sii_IconObject;
	struct RastPort *rp = Msg->imp_RPort;
	WORD x, y;

	if (NULL == rp)
		return;

	x = myImage->LeftEdge + Msg->imp_Offset.X;
	y = myImage->TopEdge + Msg->imp_Offset.Y;

	d1(kprintf("%s/%s/%ld: State=%ld  x=%ld  y=%ld\n", __FILE__, __FUNC__, __LINE__, Msg->imp_State, x, y));
	d1(kprintf("%s/%s/%ld: rp=%08lx  win=%08lx\n", __FILE__, __FUNC__, __LINE__, rp, rp->Layer->Window));

	switch (Msg->imp_State)
		{
	case IDS_NORMAL:
		if (NULL == gg->GadgetRender)
			{
			DoMethod(inst->sii_IconObject, IDTM_Layout, 
				iInfos.xii_iinfos.ii_Screen,
				rp->Layer->Window,
				rp,
				Msg->imp_DrInfo,
				IOLAYOUTF_NormalImage);
			}

		DoMethod(inst->sii_IconObject, IDTM_Draw, 
			iInfos.xii_iinfos.ii_Screen,
			rp->Layer->Window,
			rp,
			Msg->imp_DrInfo,
			x, y,
			IODRAWF_NoText | IODRAWF_AbsolutePos);
		break;

	case IDS_SELECTED:
		if (NULL == gg->GadgetRender)
			{
			DoMethod(inst->sii_IconObject, IDTM_Layout, 
				iInfos.xii_iinfos.ii_Screen,
				rp->Layer->Window,
				rp,
				Msg->imp_DrInfo,
				IOLAYOUTF_SelectedImage);
			}

		DoMethod(inst->sii_IconObject, IDTM_Draw, 
			iInfos.xii_iinfos.ii_Screen,
			rp->Layer->Window,
			rp,
			Msg->imp_DrInfo,
			x, y,
			IODRAWF_NoText | IODRAWF_AbsolutePos);
		break;

	case IDS_INACTIVEDISABLED:
	case IDS_SELECTEDDISABLED:
	case IDS_DISABLED:
		{
		// jetzt "DISABLED" Markierung zu Fuß nachholen!
		static UWORD myPattern[] =
			{ 0x8888, 0x2222 };

		SetABPenDrMd(rp, 1, 0, JAM1);
		SetAfPt(rp, myPattern, 1);
		RectFill(rp, x, y,
			x + myImage->Width - 1,
			y + myImage->Height - 1);
		}
		break;
		}
}



// AppIcons.c
// $Date$
// $Revision$


#include <stdlib.h>
#include <string.h>
#include <exec/types.h>
#include <intuition/classes.h>
#include <intuition/intuitionbase.h>
#include <datatypes/iconobject.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <scalos/scalos.h>
#include <workbench/workbench.h>
#include <exec/nodes.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/iconobject.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/scalos.h>
#include <proto/intuition.h>
#include <proto/layers.h>

#include <defs.h>

#include "wb39.h"
#include "wb39proto.h"


// aus wb39.c
extern struct Library *WorkbenchBase;
extern struct ScalosBase *ScalosBase;

#if 0
static SAVEDS(struct AppIcon *) INTERRUPT ASM myAddAppIcon(
		REG(d0, ULONG id), REG(d1, ULONG userdata),
		REG(a0, char *text), REG(a1, struct MsgPort *msgport),
		REG(a2, BPTR lock), REG(a3, struct DiskObject *diskobj),
		REG(a4, struct TagItem *taglist));
static SAVEDS(BOOL) INTERRUPT ASM myRemoveAppIcon(REG(a0, struct myAppIcon *ai));
static SAVEDS(struct myAppIcon *) INTERRUPT ASM myNewAddAppIcon(
		REG(d0, ULONG ID), REG(d1, ULONG UserData),
		REG(a0, Object *iObj), REG(a1, struct MsgPort *MessagePort),
		REG(a2, struct TagItem *TagList));
#endif


#ifdef __AROS__
static APTR origNewAddAppIcon;
static APTR origRemoveAppIcon;
static APTR origAddAppIcon;
#else
static LIBFUNC_P5_DPROTO(struct ScaAppObjNode *, (*origNewAddAppIcon),
	D0, ULONG, ID,
	D1, ULONG, UserData,
	A0, Object *, iObj,
	A1, struct MsgPort *, MessagePort,
	A2, struct TagItem *, TagList);
static LIBFUNC_P1_DPROTO(BOOL, (*origRemoveAppIcon),
	A0, struct ScaAppObjNode *, ai);
static LIBFUNC_P7_DPROTO(struct AppIcon *, (*origAddAppIcon),
	D0, ULONG, id,
	D1, ULONG, userdata,
	A0, char *, text,
	A1, struct MsgPort *, msgport,
	A2, BPTR, lock,
	A3, struct DiskObject *, diskobj,
	A4, struct TagItem *, taglist);
#endif


BOOL AppIconsInit(void)
{
#if 0
#if defined(__MORPHOS__)
	STATIC_PATCHFUNC(myRemoveAppIcon);
	STATIC_PATCHFUNC(myAddAppIcon);
	STATIC_PATCHFUNC(myNewAddAppIcon);
#endif /* __MORPHOS__ */

	origRemoveAppIcon = (BOOL (* ASM) ()) SetFunction(WorkbenchBase,
		-66, PATCH_NEWFUNC(myRemoveAppIcon));
	origAddAppIcon = (struct AppIcon * (* ASM) ()) SetFunction(WorkbenchBase,
		-60, PATCH_NEWFUNC(myAddAppIcon));

	origNewAddAppIcon = (struct ScaAppObjNode * (* ASM) ()) SetFunction(ScalosBase->scb_LibNode,
		-42, PATCH_NEWFUNC(myNewAddAppIcon));
#endif
	return TRUE;
}


void AppIconsCleanup(void)
{
	if (origRemoveAppIcon)
		{
		SetFunction(WorkbenchBase, -66 , (ULONG (* const )()) origRemoveAppIcon);
		origRemoveAppIcon = NULL;
		}
	if (origAddAppIcon)
		{
		SetFunction(WorkbenchBase, -60, (ULONG (* const )()) origAddAppIcon);
		origAddAppIcon = NULL;
		}
	if (origNewAddAppIcon)
		{
		SetFunction(&ScalosBase->scb_LibNode, -42, (ULONG (* const )()) origNewAddAppIcon);
		origNewAddAppIcon = NULL;
		}
}

#if 0
// AddAppIconA
static SAVEDS(struct AppIcon *) INTERRUPT ASM myAddAppIcon(
		REG(d0, ULONG id), REG(d1, ULONG userdata),
		REG(a0, char *text), REG(a1, struct MsgPort *msgport),
		REG(a2, BPTR lock), REG(a3, struct DiskObject *diskobj),
		REG(a4, struct TagItem *taglist))
{
	struct myAppIcon *ai;

	d1(kprintf(__FUNC__ "/%ld: diskobj=%08lx\n", __LINE__, diskobj));
	if (diskobj)
		{
		d1(kprintf(__FUNC__ "/%ld: x=%ld  y=%ld  w=%ld  h=%ld\n", __LINE__, \
			diskobj->do_Gadget.LeftEdge, diskobj->do_Gadget.TopEdge, \
			diskobj->do_Gadget.Width, diskobj->do_Gadget.Height));
		}

	ai = (struct myAppIcon *) origAddAppIcon(id, userdata, text, msgport, lock, diskobj, taglist);
	if (ai)
		{
		struct Hook *RenderHook;
		Object *iObj = (Object *) ai->mai_AppIconClone.an_object;

		RenderHook = (struct Hook *) GetTagData(WBAPPICONA_RenderHook, NULL, taglist);
		d1(kprintf(__FUNC__ "/%ld: RenderHook=%08lx  iObj=%08lx\n", __LINE__, RenderHook, iObj));
		if (RenderHook && iObj)
			{
			SetAttrs(iObj,
				IDTA_RenderHook, RenderHook,
				TAG_END);
			}

		d(kprintf(__FUNC__ "/%ld:\n", __LINE__));
		}

	return (struct AppIcon *) ai;
}


// RemoveAppIcon
static SAVEDS(BOOL) INTERRUPT ASM myRemoveAppIcon(REG(a0, struct myAppIcon *mai))
{
	struct ScaAppObjNode *sNode;
	struct DiskObject *diskObj;

	d(kprintf(__FUNC__ "/%ld: mai=%08lx\n", __LINE__, mai));

	if (NULL == mai)
		return FALSE;

	sNode = mai->mai_OrigAppIcon;

	d(kprintf(__FUNC__ "/%ld: sNode=%08lx\n", __LINE__, sNode));

	if (0x5343 != sNode->an_Kennug)	// "SC"
		return origRemoveAppIcon(sNode);

	if (0 != sNode->an_type)
		return origRemoveAppIcon(sNode);

	diskObj = (struct DiskObject *) mai->mai_AppIconClone.an_userdata2;

	d(kprintf(__FUNC__ "/%ld: DiskObj=%08lx\n", __LINE__, diskObj));

	SCA_RemoveAppObject((struct AppObject *) sNode);

	if (mai->mai_TagListClone)
		FreeTagItems(mai->mai_TagListClone);
	if (diskObj)
		FreeDiskObject(diskObj);

	free(mai);

	return TRUE;
}


// SCA_NewAddAppIcon
static SAVEDS(struct myAppIcon *) INTERRUPT ASM myNewAddAppIcon(
		REG(d0, ULONG ID), REG(d1, ULONG UserData),
		REG(a0, Object *iObj), REG(a1, struct MsgPort *MessagePort),
		REG(a2, struct TagItem *TagList))
{
	struct ScaAppObjNode *appObj = origNewAddAppIcon(ID, UserData, iObj, MessagePort, TagList);
	struct myAppIcon *mai;

	if (NULL == appObj)
		return NULL;

	d(kprintf(__FUNC__ "/%ld: appObj=%08lx\n", __LINE__, appObj));

	mai = malloc(sizeof(struct myAppIcon));
	if (NULL == mai)
		{
		SCA_RemoveAppObject((struct AppObject *) appObj);
		}

	mai->mai_AppIconClone = *appObj;
	mai->mai_OrigAppIcon = appObj;
	mai->mai_TagListClone = CloneTagItems(TagList);

	d(kprintf(__FUNC__ "/%ld: myAppIcon=%08lx  TagList=%08lx\n", __LINE__, mai, mai->mai_TagListClone));

	return mai;
}

#endif

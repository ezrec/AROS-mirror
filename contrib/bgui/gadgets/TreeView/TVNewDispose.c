/*
 * @(#) $Header$
 *
 * BGUI Tree List View class
 *
 * (C) Copyright 1999 Manuel Lemos.
 * (C) Copyright 1996-1999 Nick Christie.
 * All Rights Reserved.
 *
 * $Log$
 * Revision 42.3  2004/06/16 20:16:49  verhaegs
 * Use METHODPROTO, METHOD_END and REGFUNCPROTOn where needed.
 *
 * Revision 42.2  2004/03/24 12:40:28  stegerg
 * Real bug fix (not AROS related): in dispose method
 * do LVM_CLEAR method on listview, because otherwise
 * the superclass (listclass) might try to access
 * the resource hook, although treeview class already
 * has freed that hook, before making the DoSuperMethodA
 * call.
 *
 * Revision 42.1  2000/05/15 19:29:08  stegerg
 * replacements for REG macro
 *
 * Revision 42.0  2000/05/09 22:21:55  mlemos
 * Bumped to revision 42.0 before handing BGUI to AROS team
 *
 * Revision 41.11  2000/05/09 20:35:42  mlemos
 * Bumped to revision 41.11
 *
 * Revision 1.2  2000/05/09 20:00:40  mlemos
 * Merged with the branch Manuel_Lemos_fixes.
 *
 * Revision 1.1.2.3  1999/05/31 01:17:13  mlemos
 * Made the method functions take the arguments in the apropriate registers.
 * Ensured that the bgui.library only remains opened while there are any
 * outstanding instances.
 *
 * Revision 1.1.2.2  1999/05/24 23:59:43  mlemos
 * Corrected the declaration of SysBase.
 *
 * Revision 1.1.2.1  1999/02/21 04:07:55  mlemos
 * Nick Christie sources.
 *
 *
 *
 */

/************************************************************************
*********************  TREEVIEW CLASS: NEW/DISPOSE  *********************
************************************************************************/

/************************************************************************
******************************  INCLUDES  *******************************
************************************************************************/

#include "TreeViewPrivate.h"
#include "TVUtil.h"

/************************************************************************
**************************  LOCAL DEFINITIONS  **************************
************************************************************************/


/************************************************************************
*************************  EXTERNAL REFERENCES  *************************
************************************************************************/

/*
 * Functions from TVLVHandlers:
 */

//extern ASM SAVEDS ULONG TV_LVRsrcHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct lvResource *lvr);
extern ASM SAVEDS REGFUNCPROTO3(ULONG, TV_LVRsrcHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvResource *, lvr));

//extern ASM SAVEDS ULONG TV_LVDispHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct lvRender *lvr);
extern ASM SAVEDS REGFUNCPROTO3(ULONG, TV_LVDispHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvRender *, lvr));

//extern ASM SAVEDS ULONG TV_LVCompHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct lvCompare *lvc);
extern ASM SAVEDS REGFUNCPROTO3(ULONG, TV_LVCompHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct lvCompare *, lvc));

//extern ASM SAVEDS ULONG TV_LVNotifyHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct opUpdate *opu);
extern ASM SAVEDS REGFUNCPROTO3(ULONG, TV_LVNotifyHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opUpdate *, opu));

/*
 * Functions from TVUtil are listed in TVUtil.h
 */

/*
 * Data from startup module
 */

extern struct ExecBase *SysBase;

/************************************************************************
*****************************  PROTOTYPES  ******************************
************************************************************************/

//ASM ULONG TV_New(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct opSet *ops);
ASM REGFUNCPROTO3(ULONG, TV_New,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opSet *, ops));

//ASM ULONG TV_Dispose(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) Msg msg);
ASM REGFUNCPROTO3(ULONG, TV_Dispose,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, Msg, msg));

//LOCAL ASM SAVEDS ULONG TV_TVRsrcHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct tvResource *tvr);
LOCAL ASM SAVEDS REGFUNCPROTO3(ULONG, TV_TVRsrcHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvResource *, tvr));

//LOCAL ASM SAVEDS ULONG TV_TVDispHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct tvRender *tvr);
LOCAL ASM SAVEDS REGFUNCPROTO3(ULONG, TV_TVDispHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvRender *, tvr));

//LOCAL ASM SAVEDS ULONG TV_TVCompHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct tvCompare *tvc);
LOCAL ASM SAVEDS REGFUNCPROTO3(ULONG, TV_TVCompHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCompare *, tvc));

//LOCAL ASM SAVEDS ULONG TV_TVExpandHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct tvExpand *tve);
LOCAL ASM SAVEDS REGFUNCPROTO3(ULONG, TV_TVExpandHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvExpand *, tve));

/************************************************************************
*****************************  LOCAL DATA  ******************************
************************************************************************/

/*
 * Vector item data for the built-in expanded and contracted node images
 */

LOCAL struct VectorItem BoxPlusVector[] = {
	9,9,VIF_SCALE,	0,0,VIF_MOVE,	8,0,VIF_DRAW,	8,8,VIF_DRAW,
	0,8,VIF_DRAW,	0,0,VIF_DRAW,	4,2,VIF_MOVE,	4,6,VIF_DRAW,
	2,4,VIF_MOVE,	6,4,VIF_DRAW|VIF_LASTITEM
};

LOCAL struct VectorItem BoxMinusVector[] = {
	9,9,VIF_SCALE,	0,0,VIF_MOVE,	8,0,VIF_DRAW,	8,8,VIF_DRAW,
	0,8,VIF_DRAW,	0,0,VIF_DRAW,	2,4,VIF_MOVE,	6,4,VIF_DRAW|VIF_LASTITEM
};

LOCAL struct VectorItem ArrowRightVector[] = {
	9,9,VIF_SCALE,	2,0,VIF_MOVE|VIF_AREASTART,		2,8,VIF_DRAW,
	6,4,VIF_DRAW,	2,0,VIF_DRAW|VIF_AREAEND|VIF_LASTITEM
};

LOCAL struct VectorItem ArrowDownVector[] = {
	9,9,VIF_SCALE,	0,2,VIF_MOVE|VIF_AREASTART,		8,2,VIF_DRAW,
	4,6,VIF_DRAW,	0,2,VIF_DRAW|VIF_AREAEND|VIF_LASTITEM
};

static ULONG instances=0;

/************************************************************************
******************************  TV_NEW()  *******************************
*************************************************************************
* Handle OM_NEW: create and return a new object. Supply our class ptr,
* object ptr and initial attributes. Returns the new object or NULL
* on failure.
*
*************************************************************************/

//ASM ULONG TV_New(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) struct opSet *ops)
ASM REGFUNC3(ULONG, TV_New,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct opSet *, ops))
{
struct TagItem		*tags;
struct VectorItem	*expvi,*convi;
Object				*lvobj;
HOOKPTR				lvrsrchk,lvdisphk,lvcomphk,lvnotihk;
HOOKPTR				tvrsrchk,tvdisphk,tvcomphk,tvexpahk;
ULONG				rc;

if(BGUIBase==NULL
&& (BGUIBase=OpenLibrary("bgui.library",41))==NULL)
	return(NULL);

lvrsrchk = lvdisphk = lvcomphk = lvnotihk = NULL;
tvrsrchk = tvdisphk = tvcomphk = tvexpahk = NULL;
tags = ops->ops_AttrList;
rc = 0;

/*
 * Allocate and initialise a bunch of hook structures for
 * the embedded listview object as well as ourselves.
 */

if (	(lvrsrchk = TV_AllocHook((HOOKFUNC) TV_LVRsrcHandler,NULL)) &&
		(lvdisphk = TV_AllocHook((HOOKFUNC) TV_LVDispHandler,NULL)) &&
		(lvcomphk = TV_AllocHook((HOOKFUNC) TV_LVCompHandler,NULL)) &&
		(lvnotihk = TV_AllocHook((HOOKFUNC) TV_LVNotifyHandler,NULL)) &&
		(tvrsrchk = TV_AllocHook((HOOKFUNC) TV_TVRsrcHandler,NULL)) &&
		(tvdisphk = TV_AllocHook((HOOKFUNC) TV_TVDispHandler,NULL)) &&
		(tvcomphk = TV_AllocHook((HOOKFUNC) TV_TVCompHandler,NULL)) &&
		(tvexpahk = TV_AllocHook((HOOKFUNC) TV_TVExpandHandler,NULL))
		)
	{
	/*
	 * Create a group containing just the listview. Let some
	 * acceptable listview tags through to the embedded object.
	 */

	if (rc = TV_DoSuperNew(cl,obj,
			StartMember,
				lvobj = ListviewObject,
					LISTV_ResourceHook,		lvrsrchk,
					LISTV_DisplayHook,		lvdisphk,
					LISTV_CompareHook,		lvcomphk,
					LISTV_ListFont,			GetTagData(LISTV_ListFont,NULL,tags),
					LISTV_MinEntriesShown,	GetTagData(LISTV_MinEntriesShown,3,tags),
					LISTV_MultiSelect,		GetTagData(LISTV_MultiSelect,FALSE,tags),
					LISTV_MultiSelectNoShift,GetTagData(LISTV_MultiSelectNoShift,FALSE,tags),
					LISTV_ReadOnly,			GetTagData(LISTV_ReadOnly,FALSE,tags),
					LISTV_ThinFrames,		GetTagData(LISTV_ThinFrames,FALSE,tags),
					PGA_NewLook,			GetTagData(PGA_NewLook,FALSE,tags),
					GA_ID,					GetTagData(GA_ID,NULL,tags),
				EndObject,
			EndMember,
			TAG_DONE)
			)
		{
		TVData			*tv;

      instances++;

		/*
		 * Get a ptr to our instance data and clear it
		 */

		tv = (TVData *) INST_DATA(cl,rc);
		memset(tv,0,sizeof(TVData));

		/*
		 * Initialise the root node.
		 */

		NewList(RootList(tv));
		tv->tv_RootNode.tn_Flags = TNF_EXPANDED;

		/*
		 * If the OS is >= V39, create a private
		 * memory pool for allocating treenodes in
		 */

		if (((struct Library *) SysBase)->lib_Version >= 39)
			tv->tv_MemPool = CreatePool(MEMF_STD,2048,768);

		/*
		 * Useful in hook functions to have reverse references
		 * from the instance data back to the class/object:
		 */

		tv->tv_TreeView = (Object *) rc;
		tv->tv_TreeViewClass = cl;

		/*
		 * Transfer the ptrs to allocated listview hooks into the
		 * object's instance data, then clear the original pointers.
		 * Also place a ptr to the object instance data in the hooks.
		 */

		tv->tv_LVRsrcHook = lvrsrchk;
		tv->tv_LVRsrcHook->h_Data = (APTR) tv;
		tv->tv_LVDispHook = lvdisphk;
		tv->tv_LVDispHook->h_Data = (APTR) tv;
		tv->tv_LVCompHook = lvcomphk;
		tv->tv_LVCompHook->h_Data = (APTR) tv;
		tv->tv_LVNotifyHook = lvnotihk;
		tv->tv_LVNotifyHook->h_Data = (APTR) tv;
		lvrsrchk = lvdisphk = lvcomphk = lvnotihk = NULL;

		/*
		 * Place a ptr to the embedded listview
		 * object in our instance data
		 */

		tv->tv_Listview = lvobj;

		/*
		 * Add our notification hook to the listview
		 */

		AddHook(lvobj,tv->tv_LVNotifyHook);

		/*
		 * Put ptrs to our instance data in the hook data
		 * of our default treeview hook functions
		 */

		tvrsrchk->h_Data = (APTR) tv;
		tvdisphk->h_Data = (APTR) tv;
		tvcomphk->h_Data = (APTR) tv;
		tvexpahk->h_Data = (APTR) tv;

		/*
		 * Collect user's treeview hook functions,
		 * set defaults if omitted.
		 */

		tv->tv_ResourceHook = (HOOKPTR) GetTagData(TVA_ResourceHook,(ULONG) tvrsrchk,tags);
		tv->tv_DisplayHook = (HOOKPTR) GetTagData(TVA_DisplayHook,(ULONG) tvdisphk,tags);
		tv->tv_CompareHook = (HOOKPTR) GetTagData(TVA_CompareHook,(ULONG) tvcomphk,tags);
		tv->tv_ExpandHook = (HOOKPTR) GetTagData(TVA_ExpandHook,(ULONG) tvexpahk,tags);

		if (tv->tv_ResourceHook == tvrsrchk)
			tvrsrchk = NULL;

		if (tv->tv_DisplayHook == tvdisphk)
			tvdisphk = NULL;

		if (tv->tv_CompareHook == tvcomphk)
			tvcomphk = NULL;

		if (tv->tv_ExpandHook == tvexpahk)
			tvexpahk = NULL;

		/*
		 * Collect other attributes specified by user,
		 * or set defaults.
		 */

		tv->tv_Indentation = GetTagData(TVA_Indentation,8,tags);
		tv->tv_CopyEntries = GetTagData(TVA_CopyEntries,FALSE,tags);
		tv->tv_LineStyle = GetTagData(TVA_LineStyle,TVLS_NONE,tags);
		tv->tv_LeftAlignImage = GetTagData(TVA_LeftAlignImage,FALSE,tags);
		tv->tv_NoLeafImage = GetTagData(TVA_NoLeafImage,FALSE,tags);

		tv->tv_Indentation = max(tv->tv_Indentation,8);

		/*
		 * Collect user supplied expanded/contracted images.
		 */

		tv->tv_ExpandedImage = (Object *) GetTagData(TVA_ExpandedImage,NULL,tags);
		tv->tv_ContractedImage = (Object *) GetTagData(TVA_ContractedImage,NULL,tags);

		/*
		 * For either image not supplied explicity by user, determine
		 * which built-in image to use instead and create the objects.
		 */

		expvi = BoxMinusVector;
		convi = BoxPlusVector;

		if (GetTagData(TVA_ImageStyle,0,tags) == TVIS_ARROW)
			{
			expvi = ArrowDownVector;
			convi = ArrowRightVector;
			}

		if (!tv->tv_ExpandedImage)
			{
			tv->tv_ExpandedImage = VectorObject,VIT_VectorArray,expvi,
					IA_Width,9,IA_Height,9,EndObject;
			tv->tv_DefExpImage = TRUE;
			}

		if (!tv->tv_ContractedImage)
			{
			tv->tv_ContractedImage = VectorObject,VIT_VectorArray,convi,
					IA_Width,9,IA_Height,9,EndObject;
			tv->tv_DefConImage = TRUE;
			}

		if (!(tv->tv_ExpandedImage && tv->tv_ContractedImage))
			{
			/*
			 * Failed to obtain one or both images: commit suicide.
			 */

			CoerceMethod(cl,(Object *) rc,OM_DISPOSE);
			rc = NULL;
			}

		} /* endif create object */

	} /* endif alloc hooks */

/*
 * Free any unused default treeview hooks
 */

TV_FreeHook(tvrsrchk);
TV_FreeHook(tvdisphk);
TV_FreeHook(tvcomphk);
TV_FreeHook(tvexpahk);

/*
 * If we failed to create the object,
 * free the allocated listview hooks:
 */

if (!rc)
	{
	TV_FreeHook(lvnotihk);
	TV_FreeHook(lvcomphk);
	TV_FreeHook(lvdisphk);
	TV_FreeHook(lvrsrchk);
  if(instances==0
  && BGUIBase)
	{
		CloseLibrary(BGUIBase);
		BGUIBase=NULL;
	}
	}

return(rc);
}
REGFUNC_END

/************************************************************************
****************************  TV_DISPOSE()  *****************************
*************************************************************************
* Dispose of treeview object. We need to free our hook structures, and
* the node images, before asking our superclass to dispose of us.
*
*************************************************************************/

//ASM ULONG TV_Dispose(REG(a0) Class *cl,REG(a2) Object *obj,REG(a1) Msg msg)
ASM REGFUNC3(ULONG, TV_Dispose,
	REGPARAM(A0, Class *, cl),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, Msg, msg))
{
TVData	*tv;
ULONG rc;

tv = (TVData *) INST_DATA(cl,obj);

/*
 * Free all the treenodes
 */

TV_FreeTreeNodeList(tv,RootList(tv));

/* stegerg: CHECKME: Added following line, because
   of real bug. superclass might access the resource
   hook, which was killed before calling superclass,
   otherwise */
DoMethod(tv->tv_Listview,LVM_CLEAR,0);

/*
 * Free the images, if default ones were created
 */

if (tv->tv_DefExpImage)
	DisposeObject(tv->tv_ExpandedImage);
if (tv->tv_DefConImage)
	DisposeObject(tv->tv_ContractedImage);

/*
 * Free all the internal listview hook structures
 */

TV_FreeHook(tv->tv_LVNotifyHook);
TV_FreeHook(tv->tv_LVCompHook);
TV_FreeHook(tv->tv_LVDispHook);
TV_FreeHook(tv->tv_LVRsrcHook);

/*
 * Free default treeview hook structures, if any
 */

if (tv->tv_ResourceHook->h_Entry == TV_TVRsrcHandler)
	TV_FreeHook(tv->tv_ResourceHook);
if (tv->tv_DisplayHook->h_Entry == TV_TVDispHandler)
	TV_FreeHook(tv->tv_DisplayHook);
if (tv->tv_CompareHook->h_Entry == TV_TVCompHandler)
	TV_FreeHook(tv->tv_CompareHook);
if (tv->tv_ExpandHook->h_Entry == TV_TVExpandHandler)
	TV_FreeHook(tv->tv_ExpandHook);

/*
 * Free memory pool that treenodes were allocated in
 */

if (tv->tv_MemPool)
	DeletePool(tv->tv_MemPool);

rc=DoSuperMethodA(cl,obj,msg);
if(--instances==0)
{
	CloseLibrary(BGUIBase);
	BGUIBase=NULL;
}
return rc;
}
REGFUNC_END

/************************************************************************
**************************  TV_TV???HANDLER()  **************************
*************************************************************************
* Default internal hook functions for resource, display, compare and
* expand.
*
*************************************************************************/

//LOCAL ASM SAVEDS ULONG TV_TVRsrcHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct tvResource *tvr)
LOCAL ASM SAVEDS REGFUNC3(ULONG, TV_TVRsrcHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvResource *, tvr))
{
TVData	*tv;
ULONG	rc;

rc = 0;
tv = (TVData *) hook->h_Data;

if (tv->tv_CopyEntries)
	{
	if (tvr->tvr_Command == TVRC_MAKE)
		rc = (ULONG) TV_AllocStrCpy(tv,tvr->tvr_Entry,0);
	else
		TV_FreeVec(tv,tvr->tvr_Entry);
	}
else
	{
	if (tvr->tvr_Command == TVRC_MAKE)
		rc = (ULONG) tvr->tvr_Entry;
	}

return(rc);
}
REGFUNC_END


//LOCAL ASM SAVEDS ULONG TV_TVDispHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct tvRender *tvr)
LOCAL ASM SAVEDS REGFUNC3(ULONG, TV_TVDispHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvRender *, tvr))
{
return((ULONG) tvr->tvr_Entry);
}
REGFUNC_END


//LOCAL ASM SAVEDS ULONG TV_TVCompHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct tvCompare *tvc)
LOCAL ASM SAVEDS REGFUNC3(ULONG, TV_TVCompHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvCompare *, tvc))
{
return((ULONG) Stricmp((STRPTR) tvc->tvc_EntryA,(STRPTR) tvc->tvc_EntryB));
}
REGFUNC_END


//LOCAL ASM SAVEDS ULONG TV_TVExpandHandler(REG(a0) struct Hook *hook,
//	REG(a2) Object *obj, REG(a1) struct tvExpand *tve)
LOCAL ASM SAVEDS REGFUNC3(ULONG, TV_TVExpandHandler,
	REGPARAM(A0, struct Hook *, hook),
	REGPARAM(A2, Object *, obj),
	REGPARAM(A1, struct tvExpand *, tve))
{
return(1L);
}
REGFUNC_END


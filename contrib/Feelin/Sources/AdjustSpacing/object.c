#include "Private.h"

struct FS_UpdateSpec                            { struct FeelinClass *Class; };

///code_update_spec
STATIC F_HOOKM(void,code_update_spec,FS_UpdateSpec)
{
	struct FeelinClass *Class = Msg -> Class;
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	ULONG id_Value = F_DynamicFindID("FA_Numeric_Value");

	LOD -> spacing_spec.l = F_Get(LOD -> LSpacing,id_Value);
	LOD -> spacing_spec.t = F_Get(LOD -> TSpacing,id_Value);
	LOD -> spacing_spec.r = F_Get(LOD -> RSpacing,id_Value);
	LOD -> spacing_spec.b = F_Get(LOD -> BSpacing,id_Value);

//   F_Log(0,"spec 0x%08lx",*((ULONG *)(&LOD -> spacing_spec)));

	F_SuperDo(Class,Obj,FM_Set,F_IDA(FA_AdjustSpacing_Spec),&LOD -> spacing_spec,TAG_DONE);
}
//+
///update_objects
STATIC void update_objects(FClass *Class,FObject Obj)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	ULONG id_Value = F_DynamicFindID("FA_Numeric_Value");

	F_Do(LOD -> LSpacing,FM_Set,id_Value,LOD -> spacing_spec.l,FA_NoNotify,TRUE,TAG_DONE);
	F_Do(LOD -> TSpacing,FM_Set,id_Value,LOD -> spacing_spec.t,FA_NoNotify,TRUE,TAG_DONE);
	F_Do(LOD -> RSpacing,FM_Set,id_Value,LOD -> spacing_spec.r,FA_NoNotify,TRUE,TAG_DONE);
	F_Do(LOD -> BSpacing,FM_Set,id_Value,LOD -> spacing_spec.b,FA_NoNotify,TRUE,TAG_DONE);
}
//+

/***************************************************************************/
/*** Methods ***************************************************************/
/***************************************************************************/

///AdjustSpacing_New
F_METHOD(FObject,AdjustSpacing_New)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	if (F_SuperDo(Class,Obj,Method,

		FA_ChainToCycle,  FALSE,
		FA_Group_Rows,    2,
		FA_Frame,         "$group-frame",
		FA_Frame_Title,   F_CAT(SPACING),
		FA_ContextHelp,   F_CAT(HELP),

		Child, HLabel(F_CAT(LEFT)),   Child, LOD -> LSpacing = F_MakeObj(FV_MakeObj_Slider,TRUE,0,20,0, FA_SetMax,FV_SetHeight,TAG_DONE),
		Child, HLabel(F_CAT(TOP)),    Child, LOD -> TSpacing = F_MakeObj(FV_MakeObj_Slider,TRUE,0,20,0, FA_SetMax,FV_SetHeight,TAG_DONE),
		Child, HLabel(F_CAT(RIGHT)),  Child, LOD -> RSpacing = F_MakeObj(FV_MakeObj_Slider,TRUE,0,20,0, FA_SetMax,FV_SetHeight,TAG_DONE),
		Child, HLabel(F_CAT(BOTTOM)), Child, LOD -> BSpacing = F_MakeObj(FV_MakeObj_Slider,TRUE,0,20,0, FA_SetMax,FV_SetHeight,TAG_DONE),

		TAG_MORE,Msg))
	{
		ULONG id_Value = F_DynamicFindID("FA_Numeric_Value");

		F_Do(LOD -> RSpacing,FM_Notify,id_Value,FV_Notify_Always,Obj,FM_CallHookEntry,2,F_FUNCTION_GATE(code_update_spec),Class);
		F_Do(LOD -> BSpacing,FM_Notify,id_Value,FV_Notify_Always,Obj,FM_CallHookEntry,2,F_FUNCTION_GATE(code_update_spec),Class);
		F_Do(LOD -> LSpacing,FM_Notify,id_Value,FV_Notify_Always,LOD -> RSpacing,FM_Set,2,id_Value,FV_Notify_Value);
		F_Do(LOD -> TSpacing,FM_Notify,id_Value,FV_Notify_Always,LOD -> BSpacing,FM_Set,2,id_Value,FV_Notify_Value);

		update_objects(Class,Obj);

		return Obj;
	}
	return NULL;
}
//+
///AdjustSpacing_Set
F_METHOD(void,AdjustSpacing_Set)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	struct TagItem         *Tags = Msg,item;

	while  (F_DynamicNTI(&Tags,&item,Class))
	switch (item.ti_Tag)
	{
		case FA_AdjustSpacing_Spec:
		{
			if (item.ti_Data)
			{
				CopyMem((FInner *)(item.ti_Data),&LOD -> spacing_spec,sizeof (FInner));
			}
			else
			{
				LOD -> spacing_spec.l = 0;
				LOD -> spacing_spec.r = 0;
				LOD -> spacing_spec.t = 0;
				LOD -> spacing_spec.b = 0;
			}

			update_objects(Class,Obj);
		}
		break;
	}

	F_SUPERDO();
}
//+
///AdjustSpacing_Get
F_METHOD(void,AdjustSpacing_Get)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	struct TagItem         *Tags = Msg,item;

	BOOL up = FALSE;

	while  (F_DynamicNTI(&Tags,&item,Class))
	switch (item.ti_Tag)
	{
		case FA_AdjustSpacing_Spec:   F_STORE(&LOD -> spacing_spec); break;

		default: up = TRUE;
	}

	if (up) F_SUPERDO();
}
//+


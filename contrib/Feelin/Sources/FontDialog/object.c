#include "Private.h"

/*** Methods ***************************************************************/

///FontDialog_New
F_METHOD(APTR,FontDialog_New)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	
	if ((LOD -> Chooser = FontChooserObject, End) != NULL)
	{
		F_Do(LOD -> Chooser,FM_Notify,"FA_FontChooser_Choosed",TRUE,Obj,F_IDM(FM_FontDialog_Choosed),0);
 
		if (F_SuperDo(Class,Obj,Method,
	 
				FA_Window_Title,     F_CAT(TITLE),
				FA_Window_Open,      TRUE,
				FA_Width,            "40%",
				FA_Height,           "60%",

			  "FA_Dialog_Buttons",     FV_Dialog_Buttons_Boolean,
			  "FA_Dialog_Separator",   TRUE,

		TAG_MORE,Msg))
		{
			if (F_Do((FObject) F_Get(Obj,FA_Child),FM_AddMember,LOD -> Chooser,FV_AddMember_Head))
			{
				return Obj;
			}
			return NULL;
		}
		F_DisposeObj(LOD -> Chooser); LOD -> Chooser = NULL;
	}
	return NULL;
}
//+
///FontDialog_Set
F_METHOD(void,FontDialog_Set)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	struct TagItem *Tags = Msg,item;

	while  (F_DynamicNTI(&Tags,&item,Class))
	switch (item.ti_Tag)
	{
		case FA_FontDialog_Spec:
		{
			F_Set(LOD -> Chooser,(ULONG) "FA_FontChooser_Spec",item.ti_Data);
		}
		break;
	}

	F_SUPERDO();
}
//+
///FontDialog_Get
F_METHOD(void,FontDialog_Get)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	struct TagItem *Tags = Msg,item;

	while  (F_DynamicNTI(&Tags,&item,Class))
	switch (item.ti_Tag)
	{
		case FA_FontDialog_Spec:
		{
			F_STORE(F_Get(LOD -> Chooser,(ULONG) "FA_FontChooser_Spec"));
		}
		break;
	}

	F_SUPERDO();
}
//+
///FontDialog_Chooser
F_METHOD(void,FontDialog_Choosed)
{
	F_Set(Obj,FA_Window_Open,FALSE);
	F_Do((FObject) F_Get(Obj,FA_Application),FM_Application_PushMethod,Obj,FM_Set,2,"FA_Dialog_Response",FV_Dialog_Response_Ok);
}
//+


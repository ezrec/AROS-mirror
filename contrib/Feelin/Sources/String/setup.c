#include "Private.h"

/*** Methods ***************************************************************/

///String_Setup
F_METHOD(int32,String_Setup)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	if (F_SUPERDO())
	{
		uint32 data;
		struct FeelinPalette *ref = (struct FeelinPalette *) F_Get(Obj,FA_ColorScheme);

		F_Do(Obj,FM_ModifyHandler,FF_EVENT_BUTTON,0);

		data = F_Do(_app,FM_Application_Resolve,LOD -> p_ASpec,DEF_STRING_TEXT_ACTIVE);    LOD -> APen = (FColor *) F_Do(_display,FM_CreateColor,data,ref);
		data = F_Do(_app,FM_Application_Resolve,LOD -> p_ISpec,DEF_STRING_TEXT_INACTIVE);  LOD -> IPen = (FColor *) F_Do(_display,FM_CreateColor,data,ref);
		data = F_Do(_app,FM_Application_Resolve,LOD -> p_BSpec,DEF_STRING_TEXT_BLOCK);     LOD -> BPen = (FColor *) F_Do(_display,FM_CreateColor,data,ref);
		
		data = F_Do(_app,FM_Application_Resolve,LOD -> p_Cursor,DEF_STRING_CURSOR);

		if (data)
		{
			LOD -> Cursor = ImageDisplayObject, FA_ImageDisplay_Spec,data, End;
 
			if (LOD -> Cursor)
			{
				F_DoA(LOD -> Cursor,FM_ImageDisplay_Setup,Msg);
			}
		}
	
		LOD -> BlinkSpeed = F_Do(_app,FM_Application_ResolveInt,LOD -> p_BlinkSpeed,DEF_STRING_BLINK_SPEED);

		if (LOD -> BlinkSpeed)
		{
			data = F_Do(_app,FM_Application_Resolve,LOD -> p_Blink,DEF_STRING_BLINK);
 
			if (data)
			{
				LOD -> Blink = ImageDisplayObject, FA_ImageDisplay_Spec,data, End;
 
				if (LOD -> Blink)
				{
					F_DoA(LOD -> Blink,FM_ImageDisplay_Setup,Msg);
				}
			}
		}

		return TRUE;
	}
	return FALSE;
}
//+
///String_Cleanup
F_METHOD(void,String_Cleanup)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	if (_render)
	{
		if (LOD -> Blink)  F_Do(LOD -> Blink, FM_ImageDisplay_Cleanup,_render);
		if (LOD -> Cursor) F_Do(LOD -> Cursor,FM_ImageDisplay_Cleanup,_render);

		if (LOD -> BPen) { F_Do(_display,FM_RemColor,LOD -> BPen); LOD -> BPen = NULL; }
		if (LOD -> IPen) { F_Do(_display,FM_RemColor,LOD -> IPen); LOD -> IPen = NULL; }
		if (LOD -> APen) { F_Do(_display,FM_RemColor,LOD -> APen); LOD -> APen = NULL; }
	}

	F_Do(Obj,FM_ModifyHandler,0,FF_EVENT_BUTTON);

	F_DisposeObj(LOD -> Blink);    LOD -> Blink  = NULL;
	F_DisposeObj(LOD -> Cursor);   LOD -> Cursor = NULL;

	F_SUPERDO();
}
//+

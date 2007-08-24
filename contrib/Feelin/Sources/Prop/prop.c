#include "Private.h"

///Prop_Decrease
F_METHODM(uint32,Prop_Decrease,FS_Prop_Decrease)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	if (LOD -> First > 0)
	{
		F_Set(Obj,F_IDA(FA_Prop_First),LOD -> First - Msg -> Value);
	}

	return LOD -> First;
}
//+
///Prop_Increase
F_METHODM(uint32,Prop_Increase,FS_Prop_Increase)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	if (LOD -> First + LOD -> Visible < LOD -> Entries)
	{
		F_Set(Obj,F_IDA(FA_Prop_First),LOD -> First + Msg -> Value);
	}

	return LOD -> First;
}
//+

#include "Private.h"

///DS_New
F_METHODM(APTR,DS_New,TagItem)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	LOD -> Pool = (APTR)(F_DynamicGTD(F_IDA(FA_Dataspace_Pool),NULL,Msg));

	if (!LOD -> Pool)
	{
		if ((LOD -> Pool = F_CreatePool(1024,
												 FA_Pool_Items,   4,
												 FA_Pool_Name,    "Dataspace / Chunks",
												 TAG_DONE)) != NULL)
		{
			LOD -> Flags |= FF_Dataspace_Allocated;
		}
	}

	if (!LOD -> Pool)
	{
		F_Log(FV_LOG_USER,"Unable to Create/Access Pool");

		return NULL;
	}
	return Obj;
}
//+
///DS_Dispose
F_METHOD(void,DS_Dispose)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	if (LOD -> Pool && FF_Dataspace_Allocated & LOD -> Flags)
	{
		F_DeletePool(LOD -> Pool); LOD -> Pool = NULL;
	}
}
//+

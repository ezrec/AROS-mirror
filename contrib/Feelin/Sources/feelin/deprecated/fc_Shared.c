/*
**    $VER: fc_Shared.c 1.00 (2004/12/10)
**
**    Shared class.
**
**    © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
*****************************************************************************

*/

#include "Private.h"

struct LocalObjectData
{
	 FObject                         Object;
};

/****************************************************************************
*** Methods *****************************************************************
****************************************************************************/

///Shared_New
F_METHOD_NEW(Shared_New)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	struct TagItem *Tags = Msg,item;

	if (F_SUPERDO())
	{
		while  (F_DynamicNTI(&Tags,&item,Class))
		switch (item.ti_Tag)
		{
			case FA_Shared_Object: LOD -> Object = (FObject)(item.ti_Data); break;
		}

		if (LOD -> Object)
		{
			return Obj;
		}
		else
		{
			F_Log(FV_LOG_DEV,"FA_Shared_Object is NULL");
		}
	}
	return NULL;
}
//+
///Shared_Dispose
F_METHOD_DISPOSE(Shared_Dispose)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);
	FObject real = LOD -> Object;

	LOD -> Object = NULL;

	/* We need to clear  object  here,  because  FC_Object.Dispose()  invoke
	FM_Get  to  obtain  the  parent  of  the object, the problem is that all
	methods are differed to the real object... */

	F_DisposeObj(real);

	F_SUPERDO();
}
//+
///Shared_Super
F_METHOD(uint32,Shared_Super)
{
	return F_SUPERDO();
}
//+
///Shared_Dispatcher
F_METHOD(uint32,Shared_Dispatcher)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	return F_OBJDO(LOD -> Object);
}
//+

/****************************************************************************
*** Constructor *************************************************************
****************************************************************************/

STATIC F_METHODS_ARRAY =
{
	F_METHODS_ADD_STATIC(Shared_New,      FM_New),
	F_METHODS_ADD_STATIC(Shared_Dispose,  FM_Dispose),
	F_METHODS_ADD_STATIC(Shared_Super,    FM_Lock),
	F_METHODS_ADD_STATIC(Shared_Super,    FM_Unlock),
	
	F_ARRAY_END
};

STATIC F_TAGS_ARRAY =
{
	F_TAGS_ADD_SUPER(Object),
	F_TAGS_ADD_LOD,
	F_TAGS_ADD_METHODS,
	F_TAGS_ADD_DISPATCHER(Shared_Dispatcher),
	
	F_ARRAY_END
};

///fc_shared_create
FClass * fc_shared_create(struct in_FeelinBase *FeelinBase)
{
	return F_CreateClassA(FC_Shared,F_TAGS);
}
//+


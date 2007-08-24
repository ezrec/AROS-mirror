#include "Private.h"

/*** Methods ***************************************************************/

///XMLApplication_New
F_METHOD(FObject,XMLApplication_New)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	if (F_SUPERDO())
	{
		if (LOD -> Application)
		{
			return Obj;
		}
	}
	return NULL;
}
//+
///XMLApplication_Dispose
F_METHOD(void,XMLApplication_Dispose)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	F_DisposeObj(LOD -> Application);

	F_SUPERDO();
}
//+
///XMLApplication_Build
F_METHOD(LONG,XMLApplication_Build)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	APTR pool=NULL;
	FXMLMarkup *markup=NULL;
	FXMLMarkup *markup_application=NULL;
	FXMLMarkup *markup_notify=NULL;
	
	STATIC FDOCID xml_application_ids[] =
	{
		{ "Application",          11, FV_XMLAPPLICATION_ID_APPLICATION },
		{ "attribute",             9, FV_XMLAPPLICATION_ID_ATTRIBUTE   },
		{ "feelin:application",   18, FV_XMLAPPLICATION_ID_ROOT        },
		{ "message",               7, FV_XMLAPPLICATION_ID_MESSAGE     },
		{ "method",                6, FV_XMLAPPLICATION_ID_METHOD      },
		{ "notify",                6, FV_XMLAPPLICATION_ID_NOTIFY      },
		{ "obj",                   3, FV_XMLAPPLICATION_ID_OBJ         },
		{ "target",                6, FV_XMLAPPLICATION_ID_TARGET      },
		{ "value",                 5, FV_XMLAPPLICATION_ID_VALUE       },

		F_ARRAY_END
	};

	if (LOD -> Application)
	{
		F_Log(0,"Application already built (0x%08lx)",LOD -> Application); return FALSE;
	}

	F_Do(Obj,FM_Get,
			  "FA_Document_Pool",         &pool,
			  "FA_XMLDocument_Markups",   &markup,
				TAG_DONE);

	F_Do(Obj,(ULONG) "FM_Document_AddIDs",xml_application_ids);

/*** searching for markup 'feelin:application' *****************************/

	for ( ; markup ; markup = markup -> Next)
	{
		if (markup -> Name -> ID == FV_XMLAPPLICATION_ID_ROOT)
		{
			break;
		}
	}

	if (markup)
	{
		for (markup = (FXMLMarkup *)(markup -> ChildrenList.Head) ; markup ; markup = markup -> Next)
		{
			switch (markup -> Name -> ID)
			{
				case FV_XMLAPPLICATION_ID_APPLICATION: markup_application = markup; break;
				case FV_XMLAPPLICATION_ID_NOTIFY: markup_notify = markup; break;
			}
		}
	
		if (markup_application)
		{
			if ((LOD -> Application = (FObject) F_Do(Obj,(ULONG) "FM_XMLObject_Create",markup_application,pool)) != NULL)
			{
				if (markup_notify)
				{
					if (F_Do(Obj,F_IDM(FM_XMLApplication_CreateNotify),markup_notify,pool))
					{
						return TRUE;
					}
					return FALSE;
				}
				return TRUE;
			}
		}
	}
	else
	{
		F_Do(Obj,F_IDR(FM_Document_Log),0,NULL,"Unable to locate <%s> markup","feelin:application");
	}
	return FALSE;
}
//+
///XMLApplication_Run
F_METHOD(ULONG,XMLApplication_Run)
{
	struct LocalObjectData *LOD = F_LOD(Class,Obj);

	return F_Do(LOD -> Application,FM_Application_Run);
}
//+


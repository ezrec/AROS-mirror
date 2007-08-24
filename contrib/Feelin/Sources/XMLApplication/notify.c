#include "Private.h"

/*

	How to build notification message ?

	example: "message='FA_

	Names begining with upper case are taken as attributes relative  to  the
	target  object.  If  the attribute doesn't exists, I try it as a method.
	The best way to avoid confusion is to provide full length attributes and
	methods e.g. FA_Numeric_Value or FM_Numeric_Increase.


*/

///f_separe_words
STRPTR * f_separe_words(STRPTR Source,APTR Pool,ULONG *Words)
{
	if (Words)
	{
		*Words = 0;
	}

	if (Source)
	{
		STRPTR buf = Source;
		ULONG n=0;
		ULONG w=0;

		while (*buf++)
		{
			if (*buf == ',') n++;
		}

		if (n)
		{
			STRPTR *array = F_NewP(Pool,sizeof (STRPTR *) * (n + 3));

//         F_Log(0,"%ld comas",n);

			if (array)
			{
				STRPTR *pos = array;

				buf = Source;

				while (*buf)
				{
					STRPTR item_s = buf;

					if (*buf == ',')
					{
						F_Log(0,"ERROR MISSING PARAMETER: (%s)",buf); break;
					}

					while (*buf && *buf != ',') buf++;

					if ((*pos = F_NewP(Pool,buf - item_s + 1)) != NULL)
					{
						CopyMem(item_s,*pos,buf - item_s);

						pos++; w++;
					}
					else break;

					if (!*buf)
					{
						break;
					}
					else if (*buf == ',')
					{
						buf++;
					}
					else
					{
						F_Log(0,"MISSING COMA: (%s)",buf); break;
					}
				}

				if (!*buf)
				{
					if (Words)
					{
						*Words = w;
					}

					return array;
				}
			}
		}
	}
	return NULL;
}
//+
///f_built_message
LONG f_built_message(FClass *Class,FObject Obj,ULONG *Msg,STRPTR *Strings,FObject Target)
{
	FClassAttribute *ca=NULL;

	for ( ; *Strings ; Strings++, Msg++)
	{
		STRPTR str = *Strings;

		if (*str >= 'a' && *str <= 'z')
		{
			if (F_StrCmp(str,"value",ALL) == 0)
			{
				*Msg = FV_Notify_Value;
			}
			else if (F_StrCmp(str,"toggle",ALL) == 0)
			{
				*Msg = FV_Notify_Toggle;
			}
			else
			{
				LONG ok=FALSE;
				ULONG rc;
				
				rc = F_Do(Obj,F_IDR(FM_Document_Resolve),str,ca -> Type,ca -> Values,&ok);
				
				if (ok)
				{
					*Msg = rc;
				}
			}
		}
		else if (*str >= 'A' && *str <= 'Z')
		{
			FClassMethod *cm;

			if ((ca = F_DynamicFindAttribute(str,_class(Target),NULL)) != NULL)
			{
				*Msg = ca -> ID;
			}
			else if ((cm = F_DynamicFindMethod(str,_class(Target),NULL)) != NULL)
			{
				*Msg = cm -> ID;
			}
			else
			{
				F_Log(0,"Unable to resolve method or attribute (%s)",str);

				return FALSE;
			}
		}
		else
		{
			F_Log(0,"check ERROR (%s)",str); return FALSE;
		}

		if (!*Msg)
		{
			F_Log(0,"Unable to resolve message item (%s)",str);

			return FALSE;
		}
	}
	return TRUE;
}
//+

///XMLApplication_CreateNotify
F_METHODM(LONG,XMLApplication_CreateNotify,FS_XMLApplication_CreateNotify)
{
	FXMLMarkup *markup;

	for (markup = (FXMLMarkup *)(Msg -> Markup -> ChildrenList.Head) ; markup ; markup = markup -> Next)
	{
		FXMLAttribute *attribute;

		STRPTR data_obj=markup -> Name -> Key;
		STRPTR data_attribute=NULL;
		STRPTR data_value=NULL;
		STRPTR data_target=NULL;
		STRPTR data_method=NULL;
		STRPTR data_message=NULL;

		FObject notify_obj;

		for (attribute = (FXMLAttribute *)(markup -> AttributesList.Head) ; attribute ; attribute = attribute -> Next)
		{
			switch (attribute -> Name -> ID)
			{
				case FV_XMLAPPLICATION_ID_ATTRIBUTE:  data_attribute = attribute -> Data; break;
				case FV_XMLAPPLICATION_ID_VALUE:      data_value     = attribute -> Data; break;
				case FV_XMLAPPLICATION_ID_TARGET:     data_target    = attribute -> Data; break;
				case FV_XMLAPPLICATION_ID_METHOD:     data_method    = attribute -> Data; break;
				case FV_XMLAPPLICATION_ID_MESSAGE:    data_message   = attribute -> Data; break;
			}
		}

		if (!data_attribute) { F_Do(Obj,F_IDR(FM_Document_Log),attribute -> Line,NULL,"Attribute not defined for object '%s'",data_obj); continue; }
		if (!data_value)     { F_Do(Obj,F_IDR(FM_Document_Log),attribute -> Line,NULL,"Value not defined for attribute '%s'",data_attribute); continue; }
		if (!data_target)    { F_Do(Obj,F_IDR(FM_Document_Log),attribute -> Line,NULL,"Target not defined"); continue; }
		if (!data_method)    { F_Do(Obj,F_IDR(FM_Document_Log),attribute -> Line,NULL,"Method not defined for target '%s'",data_target); continue; }

		notify_obj = (FObject) F_Do(Obj,F_IDR(FM_XMLObject_Find),data_obj);

		if (notify_obj)
		{
			FObject notify_target;

			if (F_StrCmp("_self",data_target,ALL) == 0)              notify_target = (FObject) notify_obj;
			else if (F_StrCmp("_parent",data_target,ALL) == 0)       notify_target = (FObject) F_Get(notify_obj,FA_Parent);
			else if (F_StrCmp("_window",data_target,ALL) == 0)       notify_target = (FObject) F_Get(notify_obj,FA_Window);
			else if (F_StrCmp("_application",data_target,ALL) == 0)  notify_target = (FObject) F_Get(notify_obj,FA_Application);
			else
			{
				notify_target = (FObject) F_Do(Obj,F_IDR(FM_XMLObject_Find),data_target);
			}

			if (notify_target)
			{
				struct FS_Notify *notify;
				ULONG notify_nargs=0;

				if (data_message)
				{
					STRPTR *strings = f_separe_words(data_message,Msg -> Pool,&notify_nargs);

					if (strings)
					{
						if ((notify = F_NewP(Msg -> Pool,sizeof (struct FS_Notify) + (sizeof (ULONG) * (notify_nargs + 1)))) != NULL)
						{
							if (!f_built_message(Class,Obj,(ULONG *)((ULONG)(notify) + sizeof (struct FS_Notify)),strings,notify_target))
							{
								F_Do(Obj,F_IDR(FM_Document_Log),markup -> Line,NULL,"Unable to built message (%s)",data_message);

								return FALSE;
							}
						}
					}
					else return FALSE;
				}
				else
				{
					if (!(notify = F_NewP(Msg -> Pool,sizeof (struct FS_Notify) + sizeof (ULONG))))
					{
						F_Do(Obj,F_IDR(FM_Document_Log),markup -> Line,NULL,"Out of memory"); return FALSE;
					}
				}

				if (notify)
				{
					FClassAttribute *notify_attribute = F_DynamicFindAttribute(data_attribute,_class(notify_obj),NULL);

					if (notify_attribute)
					{
						LONG notify_value_ok = FALSE;

						if (F_StrCmp("always",data_value,ALL) == 0)
						{
							notify -> Value = FV_Notify_Always; notify_value_ok = TRUE;
						}
						else
						{
							notify -> Value = F_Do(Obj,F_IDR(FM_Document_Resolve),data_value,notify_attribute -> Type,notify_attribute -> Values,&notify_value_ok);
						}

						if (notify_value_ok)
						{
							FClassMethod *notify_method = F_DynamicFindMethod(data_method,_class(notify_target),NULL);

							if (notify_method)
							{
								notify -> Attribute  = notify_attribute -> ID;
								notify -> Target     = notify_target;
								notify -> Method     = notify_method -> ID;
								notify -> Count      = notify_nargs;

								F_DoA(notify_obj,FM_Notify,notify);
							}
						}
						else return FALSE;
					}
					else
					{
						F_Do(Obj,F_IDR(FM_Document_Log),markup -> Line,NULL,"Unable to resolve attribute (%s) for object (%s)",data_attribute,data_obj);
					}
				}
			}
			else
			{
				F_Do(Obj,F_IDR(FM_Document_Log),markup -> Line,NULL,"Unable to resolve target (%s)",data_target);

				return FALSE;
			}
		}
		else return FALSE;
	}
	return TRUE;
}
//+


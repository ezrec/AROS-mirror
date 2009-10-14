/*
**    $VER: lib_Dynamic.c 08.02 (2005/05/30)
**
**    Heart of the Dynamic ID system
**
**    © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
*****************************************************************************

 * 08.02 ** 2005/05/30 *

	Dynamic ID space is no longer wasted  if  a  class  defined  methods  or
	attributes but no new dynamic items.

 * 08.01 ** 2005/04/04 *

	Removed a little bug in F_DynamicNTI().  When  an  attribute  failed  to
	resolve (because the developer used a non existing one), it was replaced
	with TAG_DONE. But, instead of continuing the parsing of the  tag  list,
	the  function  was returning TAG_IGNORE (== 1), which corresponds to the
	second attribute defined by any  class,  if  the  argument  'Class'  was
	supplied.

*/

#include "Private.h"

#define FV_ID_TODO                              -1
#define FV_ID_DONE                              0

/* FV_ID_TODO & FV_ID_DONE are used by Dynamic table resolvers.  FV_ID_TODO
is  used  to  mark  entries  to resolve. FV_ID_DONE is used to mark invalid
entries. This may happen is a Dynamic ID has not been found. */

/****************************************************************************
*** Private *****************************************************************
****************************************************************************/

///f_dynamic_log_defined
STATIC void f_dynamic_log_defined
(
	STRPTR Name,
	UBYTE Type,
	FClass *Class,
	struct in_FeelinBase *FeelinBase
)
{
	F_Log(FV_LOG_DEV,"F_Dynamic() - '%s' (0x%08.lx) does not exists in FC_%s.",Name,Name,Class -> Name);
	FPrintf(FeelinBase -> Public.Console,"Defined %s: ",(Type == 'A') ? "Attributes" : "Methods");

	if (Type == 'A')
	{
		FClassAttribute *en;

		for (en = Class -> Attributes ; en -> Name ; en++)
		{
			FPrintf(FeelinBase -> Public.Console,"%s ",en -> Name);
		}
	}
	else
	{
		FClassMethod *en;

		for (en = Class -> Methods ; en -> Function ; en++)
		{
			if (en -> Name)
			{
				FPrintf(FeelinBase -> Public.Console,"%s ",en -> Name);
			}
		}
	}
	FPrintf(FeelinBase -> Public.Console,"\n");
	Flush(FeelinBase -> Public.Console);
}
//+
///f_dynamic_str_cmp
STATIC LONG f_dynamic_str_cmp
(
	STRPTR Str1,
	STRPTR Str2
)
/*

	Compares two strings. The underscore '_' is used as a delimiter,  it  is
	used like a NULL char e.g. "Prop" == "Prop" == "Prop_Entries"

*/
{
	if (!Str1 && !Str2)
	{
		return 0;
	}
	else if (!Str1)
	{
		return -1;
	}
	else if (!Str2)
	{
		return 1;
	}
	else
	{
		UBYTE c1,c2;

		for (c1 = *Str1++, c2 = *Str2++ ; c1 && c2 ; c1 = *Str1++, c2 = *Str2++)
		{
			if (c1 == '_' && c2 == '_')
			{
				return 0;
			}
			else if (c1 != c2)
			{
				break;
			}
		}
		return (LONG)(c1) - (LONG)(c2);
	}
}
//+

/****************************************************************************
*** Shared ******************************************************************
****************************************************************************/

///f_dynamic_find_name
struct FeelinClass * f_dynamic_find_name
(
	STRPTR String,
	struct in_FeelinBase *FeelinBase
)
{
	FHashLink *link;
	STRPTR str = String;

/*
	Ca chie avec les semaphores
*/

	while (*str && *str != '_') str++;

	if ((link = F_HashFind(FeelinBase -> HashClasses,String,str - String,NULL)) != NULL)
	{
		return (struct FeelinClass *)(link -> Data);
	}
	return NULL;
}
//+

/****************************************************************************
*** Functions ***************************************************************
****************************************************************************/

///f_dynamic_create
F_LIB_DYNAMIC_CREATE
/*

	If a class defines either attributes or methods, a Dynamic ID  is  given
	to  the  class  as  a basis for its attributes and methods IDs. Since v8
	this is the case of almost all  classes,  because  dynamic  methods  are
	defined  in  the  same  structure as method handlers. Thus, only classes
	using a dispatcher but defining no attributes nor methods are considered
	static.

*/
{
	BOOL rc = TRUE;
	BOOL dynamic_id_used = FALSE;

	if (!Class) return FALSE;

	F_OPool(FeelinBase -> DynamicPool);

	if (Class -> Public.Attributes || Class -> Public.Methods)
	{
		struct in_FeelinClass *node;

		for (node = (struct in_FeelinClass *) FeelinBase -> Classes.Head ; node ; node = (struct in_FeelinClass *) node -> Public.Next)
		{
			if (node != Class && node -> DynamicID == Class -> DynamicID)
			{
				Class -> DynamicID += FV_Class_DynamicSpace;
			}
		}
	}

/*** resolve attributes ****************************************************/

	if (Class -> Public.Attributes)
	{
		FClassAttribute *en;
		uint32 id = DYNA_ATTR | Class -> DynamicID;

		for (en = Class -> Public.Attributes ; en -> Name ; en++)
		{
			/* Attributes  that  use  a  static  ID  e.g.  FA_Right  are  left
			untouched.  Thus,  an  attribute  can  have  a Dynamic name, but a
			static numerical representation */

			if (en -> Name[0] >= 'A' && en -> Name[0] <= 'Z')
			{
				if ((0xFF000000 & en -> ID) != ATTR_BASE)
				{
					en -> ID = id++; dynamic_id_used = TRUE;
				}
			}
			else
			{
				F_Log(FV_LOG_CORE,"Invalid attribute name (0x%08lx)(%s)",en -> Name,en -> Name);

				F_RPool(FeelinBase -> DynamicPool);

				return FALSE;
			}
		}
	}

/*** resolve methods *******************************************************/

	if (Class -> Public.Methods)
	{
		FClassMethod *en;
		uint32 id = DYNA_MTHD | Class -> DynamicID;

		for (en = Class -> Public.Methods ; en -> Function ; en++)
		{
			/* Methods that use a static ID e.g. FM_New  are  left  untouched.
			Thus,  a  method  can  have a Dynamic name, but a static numerical
			representation */

			if (en -> Name && ((en -> ID & 0xFF000000) != MTHD_BASE))
			{
				/* A method can be inherited by simply suppling its whole  e.g.
				"FM_Object_New".  Otherwise the string supplied here is used as
				the name of the method. */

				if (en -> Name[0] >= 'A' && en -> Name[0] <= 'Z')
				{
					if (en -> Name[0] == 'F' && en -> Name[1] == 'M' && en -> Name[2] == '_')
					{
						FClassMethod *m = F_DynamicFindMethod(en -> Name,Class -> Public.Super,NULL);

						if (m)
						{
							en -> ID = m -> ID;
						}
					}
					else
					{
						en -> ID = id++; dynamic_id_used = TRUE; // method definition
					}
				}
				else
				{
					F_Log(FV_LOG_CORE,"Invalid method name (0x%08lx)(%s)",en -> Name,en -> Name);

					F_RPool(FeelinBase -> DynamicPool);

					return FALSE;
				}
			}
		}
	}

	/* if the class dynamic id has not be used, we reset it to  0  to  avoid
	wasting dynamic space */

	if (!dynamic_id_used)
	{
		Class -> DynamicID = 0;
	}

	F_DynamicResolveTable(Class -> Public.ResolvedIDs);

	f_dynamic_auto_add_class((FClass *) Class,FeelinBase);

	if (Class -> Public.AutoResolvedIDs)
	{
		rc = F_DynamicAddAutoTable(Class -> Public.AutoResolvedIDs);
	}

	F_RPool(FeelinBase -> DynamicPool);

	return rc;
}
//+
///f_dynamic_delete
F_LIB_DYNAMIC_DELETE
{
	if (Class)
	{
		F_OPool(FeelinBase -> ClassesPool);
		F_OPool(FeelinBase -> DynamicPool);

		if (Class -> AutoResolvedIDs)
		{
			F_DynamicRemAutoTable(Class -> AutoResolvedIDs);
		}

		F_RPool(FeelinBase -> DynamicPool);
		F_RPool(FeelinBase -> ClassesPool);
	}
}
//+
///f_dynamic_find_attribute
F_LIB_DYNAMIC_FIND_ATTRIBUTE
{
	if (Name)
	{
		STRPTR back=Name;

		if (Name[0] == 'F' && Name[1] == 'A' && Name[2] == '_')
		{

			/* if a complete name is supplied, 'FromClass'  is  replaced  with
			the  correct  class e.g. for "FA_Area_Left", 'FromClass' will be a
			pointer to the class "Area". Then 'Name' adjusted to the last part
			e.g. "Left". */

			Name += 3;

			FromClass = f_dynamic_find_name(Name,FeelinBase);

			if (!FromClass)
			{
				F_Log(0,"F_DynamicFindAttribute() (0x%08lx)(%s) not found",back,back); return NULL;
			}

			while (*Name != '_') Name++; Name++;

			if (FromClass -> Attributes)
			{
				FClassAttribute *en;

				for (en = FromClass -> Attributes ; en -> Name ; en++)
				{
					if (F_StrCmp(Name,en -> Name,ALL) == 0)
					{
						if (RealClass)
						{
							*RealClass = FromClass;
						}

						return en;
					}
				}
			}
		}
		else
		{
			while (FromClass)
			{
				if (FromClass -> Attributes)
				{
					FClassAttribute *en;

					for (en = FromClass -> Attributes ; en -> Name ; en++)
					{
						if (F_StrCmp(Name,en -> Name,ALL) == 0)
						{
							if (RealClass)
							{
								*RealClass = FromClass;
							}

							return en;
						}
					}
				}
				FromClass = FromClass -> Super;
			}
		}
		F_Log(FV_LOG_DEV,"F_DynamicFindAttribute() (0x%08lx)(%s) not found",back,back);
	}
	return NULL;
}
//+
///f_dynamic_find_method
F_LIB_DYNAMIC_FIND_METHOD
{
	if (Name)
	{
		STRPTR back = Name;

		if (Name[0] == 'F' && Name[1] == 'M' && Name[2] == '_')
		{

			/* if a complete name is supplied, 'FromClass'  is  replaced  with
			the  correct class e.g. for "FM_Area_Setup", 'FromClass' will be a
			pointer to the class "Area". Then 'Name' adjusted to the last part
			e.g. "Setup". */

			Name += 3;

			FromClass = f_dynamic_find_name(Name,FeelinBase);

			if (!FromClass)
			{
				F_Log(0,"F_DynamicFindMethod() '%s' not found",back); return NULL;
			}

			while (*Name != '_') Name++; Name++;

			if (FromClass -> Methods)
			{
				FClassMethod *en;

				for (en = FromClass -> Methods ; en -> Function ; en++)
				{
					if (F_StrCmp(Name,en -> Name,ALL) == 0)
					{
						if (RealClass)
						{
							*RealClass = FromClass;
						}

						return en;
					}
				}
			}
		}
		else
		{
			while (FromClass)
			{
				if (FromClass -> Methods)
				{
					FClassMethod *en;

					for (en = FromClass -> Methods ; en -> Function ; en++)
					{
						if (F_StrCmp(Name,en -> Name,ALL) == 0)
						{
							if (RealClass)
							{
								*RealClass = FromClass;
							}

							return en;
						}
					}
				}
				FromClass = FromClass -> Super;
			}
		}
		F_Log(FV_LOG_DEV,"F_DynamicFindMethod() '%s' not found",back);
	}
	return NULL;
}
//+
///f_dynamic_find_id
F_LIB_DYNAMIC_FIND_ID
{
	struct FeelinClass *cl;
	UBYTE type;
	STRPTR name;

//   F_Log("F_DynamicFindID() - Name '%s' (0x%lx)\n",Name,Name);

	if (!Name) return 0;

	switch ((uint32)(Name) & 0xFF000000)
	{
		case TAG_USER:
		case MTHD_BASE:
		case ATTR_BASE:
		case DYNA_MTHD:
		case DYNA_ATTR: return (uint32)(Name);
	}

	type = Name[1];

	if (Name[0] != 'F' || (type != 'A' && type != 'M') || Name[2] != '_')
	{
		F_Log(FV_LOG_DEV,"F_DynamicFindID() - Incorrect name '%s' (0x%08.lx)",Name,Name);

		return -1;
	}

	if ((cl = f_dynamic_find_name(name = Name + 3,FeelinBase)) != NULL)
	{
//      F_DebugOut("FindDynamicID - Dynamic 0x%lx - Base '%s' ('%s')",cl,cl -> DynamicName,Name + 3);

		while (*name != '_') name++; name++;
//      F_DebugOut("Entries 0x%lx (0x%lx - 0x%lx)",entry,cl -> Methods,cl -> Attributes);

		if (type == 'A')
		{
			FClassAttribute *en;

			for (en = cl -> Attributes ; en -> Name ; en++)
			{
				if (F_StrCmp(name,en -> Name,ALL) == 0)
				{
					return en -> ID;
				}
			}
		}
		else
		{
			FClassMethod *en;

			for (en = cl -> Methods ; en -> Function ; en++)
			{
				if (F_StrCmp(name,en -> Name,ALL) == 0)
				{
					return en -> ID;
				}
			}
		}

		f_dynamic_log_defined(Name,type,cl,FeelinBase);
	}
	else
	{
		F_Log(FV_LOG_DEV,"F_DynamicFindID() - Unable to find '%s' (0x%08.lx)",Name,Name);
	}
	return 0;
}

#if 0
//+
///f_dynamic_resolve_table
F_LIB_DYNAMIC_RESOLVE_TABLE
{
	FDynamicEntry *entry;
	FClass *cl;
	STRPTR name;

	if (!Entries) return;

	/* First clear them all */

	for (entry = Entries ; entry -> Name ; entry++) entry -> ID = FV_ID_TODO;

	for (;;)
	{
		for (entry = Entries ; entry -> ID != FV_ID_TODO ; entry++)
			if (!entry -> Name)
				return;

		if ((name = entry -> Name) != NULL)
		{
			UBYTE type = name[1];

			if (name[0] != 'F' || (type != 'A' && type != 'M') || name[2] != '_')
			{
				F_Log(FV_LOG_DEV,"F_Dynamic() - Incorrect name '%s' (0x%08.lx)",name,name);

				entry -> ID = FV_ID_DONE;
			}
			else if ((cl = f_dynamic_find_name(name += 3,FeelinBase)))
			{
				BOOL fromstart=TRUE;

				while (*name != '_') name++; name++;

				if (type == 'A')
				{
					FClassAttribute *attr = cl -> Attributes;

					while (attr -> Name)
					{
						if (F_StrCmp(name,attr -> Name,ALL) == 0)
						{
							fromstart = FALSE;
							entry -> ID = attr -> ID;
							entry++;

							/* Instead of searching the whole table again, I check
							if the next item is compatible (same type & same class
							name). If table is ordonned efficiently resolving  the
							table will be very fast. */

							if (!(name = entry -> Name))              break;
							if (type != name[1])                      break;
							if (f_dynamic_str_cmp(name += 3,cl -> Name) != 0)  break;
							while (*name != '_') name++; name++;
						}
						attr++;
					}
				}
				else
				{
					FClassMethod *mthd = cl -> Methods;

					while (mthd -> Function)
					{
						if (mthd -> Name)
						{
							if (F_StrCmp(name,mthd -> Name,ALL) == 0)
							{
								fromstart = FALSE;
								entry -> ID = mthd -> ID;
								entry++;

								/* Instead of searching the whole  table  again,  I
								check  if  the next item is compatible (same type &
								same class name). If table is  ordered  efficiently
								resolving the table will be very fast. */

								if (!(name = entry -> Name))              break;
								if (type != name[1])                      break;
								if (f_dynamic_str_cmp(name += 3,cl -> Name) != 0)  break;
								while (*name != '_') name++; name++;
							}
						}
						mthd++;
					}
				}

				/* name not defined in the class */

				if (entry -> ID == FV_ID_TODO && fromstart)
				{
					f_dynamic_log_defined(entry -> Name,type,cl,FeelinBase);

					entry -> ID = FV_ID_DONE;
				}
			}
			else
			{
				F_Log(FV_LOG_DEV,"F_Dynamic() - Unable to find '%s' (0x%08.lx)",entry -> Name,entry -> Name);

				entry -> ID = FV_ID_DONE;
			}
		}
	}
}
//+
*/
///f_dynamic_resolve_table
#endif

enum  {

		FV_RESOLVE_TYPE_INVALID,
		FV_RESOLVE_TYPE_ATTRIBUTE,
		FV_RESOLVE_TYPE_METHOD

		};

F_LIB_DYNAMIC_RESOLVE_TABLE
{
	uint32 n=0;

	if (Entries)
	{
		FClass *cl = NULL;
		FDynamicEntry *en;

		for (en = Entries ; en -> Name ; en++)
		{
			uint32 type = FV_RESOLVE_TYPE_INVALID;

			if (en -> Name[0] == 'F' && en -> Name[2] == '_')
			{
				switch (en -> Name[1])
				{
					case 'A':   type = FV_RESOLVE_TYPE_ATTRIBUTE; break;
					case 'M':   type = FV_RESOLVE_TYPE_METHOD; break;
				}
			}

			if (type)
			{
				if (cl)
				{
					if (F_StrCmp(en -> Name + 3,cl -> Name,ALL) != '_')
					{
						cl = NULL;
					}
				}

				if (!cl)
				{
					cl = f_dynamic_find_name(en -> Name + 3,FeelinBase);
				}

				if (cl)
				{
				   STRPTR name = en -> Name + 3;

	            while (*name != '_') name++; name++;

//					  F_Log(0,"Class (%s) Entry (%s)(%s)",cl -> Name,en -> Name,name);

					if (type == FV_RESOLVE_TYPE_ATTRIBUTE && cl -> Attributes)
					{
						FClassAttribute *attribute;

						for (attribute = cl -> Attributes ; attribute -> Name ; attribute++)
						{
							if (F_StrCmp(attribute -> Name,name,ALL) == 0)
							{
								en -> ID = attribute -> ID; n++; break;
							}
						}

						if (!attribute -> Name)
						{
							F_Log(FV_LOG_CLASS,"Attribute (%s) not defined by Class (%s)",en -> Name,cl -> Name);

							en -> ID = 0;
	               }
					}
					else if (type == FV_RESOLVE_TYPE_METHOD && cl -> Methods)
					{
						FClassMethod *method;

						for (method = cl -> Methods ; method -> Function ; method++)
						{
							if (F_StrCmp(method -> Name,name,ALL) == 0)
							{
								en -> ID = method -> ID; n++; break;
							}
						}

						if (!method -> Name)
	               {
							F_Log(FV_LOG_CLASS,"Method (%s) not defined by Class (%s)",en -> Name,cl -> Name);

							en -> ID = 0;
	               }
					}
					else
					{
						F_Log(FV_LOG_CLASS,"Unable to resolve (%s), Class (%s) defines none",en -> Name,cl -> Name);
					}
				}
				else
				{
					F_Log(FV_LOG_CLASS,"Unable to find class to resolve (%s)",en -> Name);
				}
			}
			else F_Log(FV_LOG_CLASS,"Invalid name (0x%08lx)(%s)",en -> Name,en -> Name);
		}
	}
	return n;
}
//+


///f_dynamic_nti
F_LIB_DYNAMIC_NTI
{
	if(!(*TLP)) return NULL;

	item -> ti_Tag  = 0;
	item -> ti_Data = 0;

	for (;;)
	{
		switch ((*TLP)->ti_Tag)
		{
			case TAG_MORE: if (!((*TLP) = (struct TagItem *)(*TLP)->ti_Data)) return NULL; continue;
			case TAG_IGNORE: break;
			case TAG_END: (*TLP) = NULL; return NULL;
			case TAG_SKIP: (*TLP) += (*TLP)->ti_Data + 1; continue;

			default:
			{
				/* Use post-increment (return will return the current value and
				then tagListPtr will be incremented) */

				item -> ti_Tag  = (*TLP) -> ti_Tag;
				item -> ti_Data = (*TLP) -> ti_Data;

				if (!(TAG_USER & item -> ti_Tag))
				{
					FClass *real_class=NULL;
					FClassAttribute *attr = F_DynamicFindAttribute((STRPTR)(item -> ti_Tag),(FClass *) Class,&real_class);

					if (attr)
					{
						item -> ti_Tag = attr -> ID;

						if (attr -> Values && TypeOfMem((APTR)(item -> ti_Data)))
						{
							FClassAttributeValue *val;

							for (val = attr -> Values ; val -> Name ; val++)
							{
								if (F_StrCmp((STRPTR)(item -> ti_Data),val -> Name,ALL) == 0)
								{
//                           F_Log(0,"found value (%s)(%ld)",val -> Name,val -> Value);

									item -> ti_Data = val -> Value;

									break;
								}
							}
						}
					}
					else
					{
						item -> ti_Tag = TAG_IGNORE;
					}
				}

				/* save values in the real  tag,  so  that  we  don't  have  to
				resolve everything again */

				(*TLP) -> ti_Tag  = item -> ti_Tag;
				(*TLP) -> ti_Data = item -> ti_Data;

				if (item -> ti_Tag != TAG_IGNORE)
				{
					if (Class)
					{
						if (item -> ti_Tag >=  (DYNA_ATTR | Class -> DynamicID) &&
							 item -> ti_Tag <  ((DYNA_ATTR | Class -> DynamicID) + FV_Class_DynamicSpace))
						{
							item -> ti_Tag = (0x00FFFFFF & item -> ti_Tag) - Class -> DynamicID;
						}
					}
					return (struct TagItem *)(*TLP)++;
				}
			}
		}
		(*TLP)++;
	}
}
//+
///f_dynamic_fti
F_LIB_DYNAMIC_FTI
{
	if(!(Tags)) return NULL;

	Attribute = F_DynamicFindID((STRPTR)(Attribute));

	for (;;)
	{
		switch (Tags -> ti_Tag)
		{
			case TAG_MORE:
			{
				if (!(Tags = (struct TagItem *)(Tags->ti_Data)))
				{
					return NULL;
				}
			}
			continue;

			case TAG_IGNORE: break;
			case TAG_END: return Tags = NULL;

			case TAG_SKIP:
			{
				Tags += Tags->ti_Data + 1;
			}
			continue;

			default:
			{
				uint32 id = F_DynamicFindID((STRPTR)(Tags -> ti_Tag));

				if (!id)
				{
					id = Tags -> ti_Tag;
				}

				if (id == Attribute) return Tags;
			}
		}
		Tags++;
	}
}
//+
///f_dynamic_gtd
F_LIB_DYNAMIC_GTD
{
	struct TagItem *item = F_DynamicFTI(Attribute,Tags);

	if (item)
	{
		return item -> ti_Data;
	}
	else
	{
		return Default;
	}
}
//+


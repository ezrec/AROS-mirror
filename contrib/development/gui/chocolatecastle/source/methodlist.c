/*------------------------------------------*/
/* Code generated with ChocolateCastle 0.1  */
/* written by Grzegorz "Krashan" Kraszewski */
/* <krashan@teleinfo.pb.bialystok.pl>       */
/*------------------------------------------*/


/* MethodListClass code. */

#include <libraries/mui.h>
#include <libvstring.h>

#include "methodlist.h"
#include "support.h"
#include "locale.h"

struct MUI_CustomClass *MethodListClass;


#ifdef __AROS__
AROS_UFP3(IPTR, MethodListGate,
AROS_UFPA(Class  *, cl,  A0),
AROS_UFPA(Object *, obj, A2),
AROS_UFPA(Msg     , msg, A1));
#else
LONG MethodListDispatcher(void);
const struct EmulLibEntry MethodListGate = {TRAP_LIB, 0, (void(*)(void))MethodListDispatcher};
#endif

/// MethodListData

struct MethodListData
{
	char IdBuf[10];
};


///
/// CreateMethodListClass()

struct MUI_CustomClass *CreateMethodListClass(void)
{
	struct MUI_CustomClass *cl;

	cl = MUI_CreateCustomClass(NULL, MUIC_List, NULL, sizeof(struct MethodListData), (APTR)&MethodListGate);
	MethodListClass = cl;
	return cl;
}


///
/// DeleteMethodListClass()

void DeleteMethodListClass(void)
{
	MUI_DeleteCustomClass(MethodListClass);
}


///

/// MethodListNew()

IPTR MethodListNew(Class *cl, Object *obj, struct opSet *msg)
{
	obj = DoSuperNewM(cl, obj,
		MUIA_Frame, MUIV_Frame_InputList,
		MUIA_List_Format, "BAR,BAR,BAR,BAR",
		MUIA_List_Title, TRUE,
	TAG_MORE, msg->ops_AttrList);

	return (IPTR)obj;
}


///
/// MethodListListConstruct()

IPTR MethodListListConstruct(UNUSED Class *cl, UNUSED Object *obj, struct MUIP_List_Construct *msg)
{
	struct MethodEntry *ent, *ine;

	ine = (struct MethodEntry*)msg->entry;

	if (ent = AllocPooled(msg->pool, sizeof(struct MethodEntry)))
	{
		if (ent->Name = AllocVecPooled(msg->pool, strln(ine->Name) + 1))
		{
			if (ent->Structure = AllocVecPooled(msg->pool, strln(ine->Structure) + 1))
			{
				if (ent->Function = AllocVecPooled(msg->pool, strln(ine->Function) + 1))
				{
					if (ent->Identifier = AllocVecPooled(msg->pool, strln(ine->Identifier) + 1))
					{
						StrCopy(ine->Name, ent->Name);
						StrCopy(ine->Structure, ent->Structure);
						StrCopy(ine->Function, ent->Function);
						StrCopy(ine->Identifier, ent->Identifier);
						ent->Standard = ine->Standard;
						return (IPTR)ent;
					}
					FreeVecPooled(msg->pool, ent->Identifier);
				}
				FreeVecPooled(msg->pool, ent->Structure);
			}
			FreeVecPooled(msg->pool, ent->Name);
		}
		FreePooled(msg->pool, ent, sizeof(struct MethodEntry));
	}
	return 0;
}


///
/// MethodListListDestruct()

IPTR MethodListListDestruct(UNUSED Class *cl, UNUSED Object *obj, struct MUIP_List_Destruct *msg)
{
	struct MethodEntry *ent = (struct MethodEntry*)msg->entry;

	if (ent)
	{
		if (ent->Function) FreeVecPooled(msg->pool, ent->Function);
		if (ent->Name) FreeVecPooled(msg->pool, ent->Name);
		if (ent->Structure) FreeVecPooled(msg->pool, ent->Structure);
		if (ent->Identifier) FreeVecPooled(msg->pool, ent->Identifier);
		FreePooled(msg->pool, ent, sizeof(struct MethodEntry));
	}
	return 0;
}


///
/// MethodListListDisplay()

IPTR MethodListListDisplay(UNUSED Class *cl, UNUSED Object *obj, struct MUIP_List_Display *msg)
{
	struct MethodEntry *ent = (struct MethodEntry*)msg->entry;

	if (!ent)
	{
		msg->array[0] = (STRPTR)LS(MSG_METHOD_LIST_HEADER_NAME, "\33cMethod Name");
		msg->array[1] = (STRPTR)LS(MSG_METHOD_LIST_HEADER_IDENTIFIER, "\33cIdentifier");
		msg->array[2] = (STRPTR)LS(MSG_METHOD_LIST_HEADER_FUNCTION, "\33cFunction");
		msg->array[3] = (STRPTR)LS(MSG_METHOD_LIST_HEADER_STRUCTURE, "\33cMessage Structure");
	}
	else
	{
		msg->array[0] = (STRPTR)ent->Name;
		msg->array[1] = (STRPTR)ent->Identifier;
		msg->array[2] = (STRPTR)ent->Function;
		msg->array[3] = (STRPTR)ent->Structure;
	}

	return 0;
}


///
/// MethodListInsertMethodTable()

IPTR MethodListInsertMethodTable(UNUSED Class *cl, Object *obj, struct MTLP_InsertMethodTable *msg)
{
	struct MethodEntry *me = msg->Table;

	xset(obj, MUIA_List_Quiet, TRUE);

	while (me->Name)
	{
		DoMethod(obj, MUIM_List_InsertSingle, (IPTR)me++, MUIV_List_Insert_Bottom);
	}

	xset(obj, MUIA_List_Quiet, FALSE);
	return 0;
}


///
/// MethodListDispatcher()

#ifdef __AROS__
BOOPSI_DISPATCHER(IPTR, MethodListGate, cl, obj, msg)
{
#else
LONG MethodListDispatcher(void)
{
	Class *cl = (Class*)REG_A0;
	Object *obj = (Object*)REG_A2;
	Msg msg = (Msg)REG_A1;
#endif

	switch (msg->MethodID)
	{
		case OM_NEW:                  return MethodListNew(cl, obj, (struct opSet*)msg);
		case MUIM_List_Construct:     return MethodListListConstruct(cl, obj, (struct MUIP_List_Construct*)msg);
		case MUIM_List_Destruct:      return MethodListListDestruct(cl, obj, (struct MUIP_List_Destruct*)msg);
		case MUIM_List_Display:       return MethodListListDisplay(cl, obj, (struct MUIP_List_Display*)msg);
		case MTLM_InsertMethodTable:  return MethodListInsertMethodTable(cl, obj, (struct MTLP_InsertMethodTable*)msg);
		default:                      return (DoSuperMethodA(cl, obj, msg));
	}
}
#ifdef __AROS__
BOOPSI_DISPATCHER_END
#endif

///


/*------------------------------------------*/
/* Code generated with ChocolateCastle 0.5  */
/* written by Grzegorz "Krashan" Kraszewski */
/* <krashan@teleinfo.pb.edu.pl>             */
/*------------------------------------------*/

/* MethodEditorClass code. The class generates a group of gadgets for editing */
/* a BOOPSI method. */

#include <libraries/mui.h>
#include <libvstring.h>

#include "methodeditor.h"
#include "methodlist.h"
#include "support.h"
#include "generator.h"
#include "locale.h"


struct MUI_CustomClass *MethodEditorClass;

/// dispatcher prototype

#ifdef __AROS__
AROS_UFP3(IPTR, MethodEditorGate,
AROS_UFPA(Class  *, cl,  A0),
AROS_UFPA(Object *, obj, A2),
AROS_UFPA(Msg     , msg, A1));
#else
IPTR MethodEditorDispatcher(void);
const struct EmulLibEntry MethodEditorGate = {TRAP_LIB, 0, (void(*)(void))MethodEditorDispatcher};
#endif

///
/// MethodEditorData

struct MethodEditorData
{
	Object *NamePages;           // pagegroup containing read-only and writable gadget for method name
	Object *Name;                // method name (writable)
	Object *NameReadOnly;        // method name (read only)
	Object *Function;            // function name in the code (writable always)
	Object *StructPages;         // pagegroup containing read-only and writable gadget for message structure name
	Object *Struct;              // message structure name (writable)
	Object *StructReadOnly;      // message structure name (read-only)
	Object *IdentifierPages;     // pagegroup containing read-only and writable gadget for method id
	Object *Identifier;          // method id (writable)
	Object *IdentifierReadOnly;  // method id (read-only)
	Object *Generator;           // parent generator
	BOOL    Standard;            // standard methods are only partially editable
	UBYTE   WinTitle[144];       // space for window title
};

///
/// CreateMethodEditorClass()

struct MUI_CustomClass *CreateMethodEditorClass(void)
{
	struct MUI_CustomClass *cl;

	cl = MUI_CreateCustomClass(NULL, NULL, EditorClass, sizeof(struct MethodEditorData), (APTR)&MethodEditorGate);
	MethodEditorClass = cl;
	return cl;
}

///
/// DeleteMethodEditorClass()

void DeleteMethodEditorClass(void)
{
	if (MethodEditorClass) MUI_DeleteCustomClass(MethodEditorClass);
}

///

/// MethodEditorNew()

IPTR MethodEditorNew(Class *cl, Object *obj, struct opSet *msg)
{
	if (obj = (Object*)DoSuperMethodA(cl, obj, (Msg)msg))
	{
		struct MethodEditorData *d = INST_DATA(cl, obj);

		d->Generator = (Object*)GetTagData(EDLA_Generator, 0, msg->ops_AttrList);
	}

	return (IPTR)obj;
}


///
/// MethodEditorGet()

IPTR MethodEditorGet(Class *cl, Object *obj, struct opGet *msg)
{
	switch (msg->opg_AttrID)
	{
		case EDLA_WindowID:
			*msg->opg_Storage = 0x4D455445;  // "METE"
		return TRUE;

		case EDLA_WindowTitle:
			*(STRPTR*)msg->opg_Storage = (STRPTR)"?";
		return TRUE;
	}
	return DoSuperMethodA(cl, obj, (Msg)msg);
}

///
/// MethodEditorBuildEditor()

// This class builds the group of gadgets needed to edit a BOOPSI method
// parameters.

IPTR MethodEditorBuildEditor(Class *cl, Object *obj, UNUSED Msg msg)
{
	struct MethodEditorData *d = INST_DATA(cl, obj);

	Object *obj2;

	obj2 = MUI_NewObjectM(MUIC_Group,
		MUIA_Group_Child, MUI_NewObjectM(MUIC_Group,
			MUIA_Group_Columns, 2,
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Text,
				MUIA_Text_Contents, LS(MSG_METHOD_EDITOR_METHOD_NAME_LABEL, "Method Name:"),
				MUIA_Text_PreParse, "\33r",
				MUIA_HorizWeight, 0,
			TAG_END),
			MUIA_Group_Child, d->NamePages = MUI_NewObjectM(MUIC_Group,
				MUIA_Group_PageMode, TRUE,
				MUIA_Group_Child, d->Name = MUI_NewObjectM(MUIC_String,
					MUIA_Frame, MUIV_Frame_String,
					MUIA_String_Accept, IdAllowed,
					MUIA_CycleChain, TRUE,
					MUIA_String_AdvanceOnCR, TRUE,
					MUIA_String_MaxLen, 80,
				TAG_END),
				MUIA_Group_Child, d->NameReadOnly = MUI_NewObjectM(MUIC_Text,
					MUIA_Frame, MUIV_Frame_Text,
					MUIA_Background, MUII_TextBack,
				TAG_END),
			TAG_END),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Text,
				MUIA_Text_Contents, LS(MSG_METHOD_EDITOR_METHOD_FUNCTION_LABEL, "Function Name:"),
				MUIA_Text_PreParse, "\33r",
				MUIA_HorizWeight, 0,
			TAG_END),
			MUIA_Group_Child, d->Function = MUI_NewObjectM(MUIC_String,
				MUIA_Frame, MUIV_Frame_String,
				MUIA_String_Accept, IdAllowed,
				MUIA_CycleChain, TRUE,
				MUIA_String_AdvanceOnCR, TRUE,
				MUIA_String_MaxLen, 80,
			TAG_END),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Text,
				MUIA_Text_Contents, LS(MSG_METHOD_EDITOR_METHOD_STRUCTURE_LABEL, "Message Structure Type:"),
				MUIA_Text_PreParse, "\33r",
				MUIA_HorizWeight, 0,
			TAG_END),
			MUIA_Group_Child, d->StructPages = MUI_NewObjectM(MUIC_Group,
				MUIA_Group_PageMode, TRUE,
				MUIA_Group_Child, d->Struct = MUI_NewObjectM(MUIC_String,
					MUIA_Frame, MUIV_Frame_String,
					MUIA_String_Accept, IdAllowed,
					MUIA_CycleChain, TRUE,
					MUIA_String_AdvanceOnCR, TRUE,
					MUIA_String_MaxLen, 80,
				TAG_END),
				MUIA_Group_Child, d->StructReadOnly = MUI_NewObjectM(MUIC_Text,
					MUIA_Frame, MUIV_Frame_Text,
					MUIA_Background, MUII_TextBack,
				TAG_END),
			TAG_END),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Text,
				MUIA_Text_Contents, LS(MSG_METHOD_EDITOR_METHOD_IDENTIFIER_LABEL, "Method Identifier:"),
				MUIA_Text_PreParse, "\33r",
				MUIA_HorizWeight, 0,
			TAG_END),
			MUIA_Group_Child, d->IdentifierPages = MUI_NewObjectM(MUIC_Group,
				MUIA_Group_PageMode, TRUE,
				MUIA_Group_Child, d->Identifier = MUI_NewObjectM(MUIC_String,
					MUIA_Frame, MUIV_Frame_String,
					MUIA_String_Accept, HexAllowed,
					MUIA_CycleChain, TRUE,
					MUIA_String_AdvanceOnCR, TRUE,
					MUIA_String_MaxLen, 9,
				TAG_END),
				MUIA_Group_Child, d->IdentifierReadOnly = MUI_NewObjectM(MUIC_Text,
					MUIA_Frame, MUIV_Frame_Text,
					MUIA_Background, MUII_TextBack,
				TAG_END),
			TAG_END),
		TAG_END),
	TAG_END);

	return (IPTR)obj2;
}

///
/// MethodEditorListToWindow()

// This class gets method parameters from a MethodEntry structure and puts them
// into gadgets.

IPTR MethodEditorListToWindow(Class *cl, Object *obj, struct EDLP_ListToWindow *msg)
{
	struct MethodEditorData *d = INST_DATA(cl, obj);
	Object *list, *window;
	struct MethodEntry *me;

	if (list = (Object*)xget(obj, EDLA_List))
	{
		DoMethod(list, MUIM_List_GetEntry, msg->Position, (IPTR)&me);

		if (me)
		{
			STRPTR module_name;

			window = (Object*)xget(obj, EDLA_Window);
			module_name = (STRPTR)xget(d->Generator, GENA_UnitName);
			xset(d->Function, MUIA_String_Contents, me->Function);
			FmtNPut(d->WinTitle, (STRPTR)"%s/%s()", 144, module_name, me->Name);
			xset(window, MUIA_Window_Title, d->WinTitle);

			if (me->Standard)      // One of default methods.
			{
				xset(d->NamePages, MUIA_Group_ActivePage, 1);
				xset(d->NameReadOnly, MUIA_Text_Contents, me->Name);
				xset(d->StructPages, MUIA_Group_ActivePage, 1);
				xset(d->StructReadOnly, MUIA_Text_Contents, me->Structure);
				xset(d->IdentifierPages, MUIA_Group_ActivePage, 1);
				xset(d->IdentifierReadOnly, MUIA_Text_Contents, me->Identifier);
				d->Standard = TRUE;
			}
			else                   // Some custom method.
			{
				xset(d->NamePages, MUIA_Group_ActivePage, 0);
				xset(d->Name, MUIA_String_Contents, me->Name);
				xset(d->StructPages, MUIA_Group_ActivePage, 0);
				xset(d->Struct, MUIA_String_Contents, me->Structure);
				xset(d->IdentifierPages, MUIA_Group_ActivePage, 0);
				xset(d->Identifier, MUIA_String_Contents, me->Identifier);
				d->Standard = FALSE;
			}
		}
	}

	return 0;
}

///
/// MethodEditorWindowToList()

// This class gets method parameters from gadgets and puts them into a
// MethodEntry structure. Then verification of parameters is performed, if
// paremeters are OK, TRUE is returned, FALSE otherwise.

IPTR MethodEditorWindowToList(Class *cl, Object *obj, struct EDLP_WindowToList *msg)
{
	struct MethodEditorData *d = INST_DATA(cl, obj);
	struct MethodEntry me;
	Object *list, *window;

	me.Standard = d->Standard;
	window = (Object*)xget(obj, EDLA_Window);

	if (list = (Object*)xget(obj, EDLA_List))
	{
		if (me.Standard)
		{
			me.Name = (STRPTR)xget(d->NameReadOnly, MUIA_Text_Contents);
			me.Structure = (STRPTR)xget(d->StructReadOnly, MUIA_Text_Contents);
			me.Identifier = (STRPTR)xget(d->IdentifierReadOnly, MUIA_Text_Contents);
		}
		else
		{
			me.Name = (STRPTR)xget(d->Name, MUIA_String_Contents);
			me.Structure = (STRPTR)xget(d->Struct, MUIA_String_Contents);
			me.Identifier = (STRPTR)xget(d->Identifier, MUIA_String_Contents);
		}

		me.Function = (STRPTR)xget(d->Function, MUIA_String_Contents);

		if (StrLen(me.Name) == 0)
		{
			MUI_Request(_app(obj), window, 0, "ChocolateCastle",
			 (char*)LS(MSG_ERROR_REQUESTER_OK_BUTTON, "*_OK"),
			 (char*)LS(MSG_METHOD_EDITOR_ERROR_MISSING_NAME, "Missing name for this method."));
			xset(window, MUIA_Window_ActiveObject, d->Name);
			return FALSE;
		}

		if ((me.Name[0] != '_') && (me.Name[0] < 'A'))
		{
			MUI_Request(_app(obj), window, 0, "ChocolateCastle",
             (char*)LS(MSG_ERROR_REQUESTER_OK_BUTTON, "*_OK"),
			 (char*)LS(MSG_METHOD_EDITOR_ERROR_NAME_DIGIT, "Method name can't start from a digit."));
			xset(window, MUIA_Window_ActiveObject, d->Name);
			return FALSE;
		}

		if (StrLen(me.Function) == 0)
		{
			MUI_Request(_app(obj), window, 0, "ChocolateCastle",
             (char*)LS(MSG_ERROR_REQUESTER_OK_BUTTON, "*_OK"),
			 (char*)LS(MSG_METHOD_EDITOR_ERROR_MISSING_FUNCTION, "Method %s() has no function name."),
			 (IPTR)me.Name);
			xset(window, MUIA_Window_ActiveObject, d->Function);
			return FALSE;
		}

		if ((me.Function[0] != '_') && (me.Function[0] < 'A'))
		{
			MUI_Request(_app(obj), window, 0, "ChocolateCastle",
             (char*)LS(MSG_ERROR_REQUESTER_OK_BUTTON, "*_OK"),
			 (char*)LS(MSG_METHOD_EDITOR_ERROR_FUNCTION_DIGIT, "Function name for method %s() can't start from a digit."),
			 (IPTR)me.Name);
			xset(window, MUIA_Window_ActiveObject, d->Function);
			return FALSE;
		}

		if ((me.Structure[0]) && (me.Structure[0] != '_') && (me.Structure[0] < 'A'))
		{
			MUI_Request(_app(obj), window, 0, "ChocolateCastle",
			 (char*)LS(MSG_ERROR_REQUESTER_OK_BUTTON, "*_OK"),
			 (char*)LS(MSG_METHOD_EDITOR_ERROR_STRUCTURE_DIGIT, "Structure name for method %s() can't start from a digit."),
			 (IPTR)me.Name);
			xset(window, MUIA_Window_ActiveObject, d->Struct);
			return FALSE;
		}

		if (StrLen(me.Identifier) == 0)
		{
			MUI_Request(_app(obj), window, 0, "ChocolateCastle",
			 (char*)LS(MSG_ERROR_REQUESTER_OK_BUTTON, "*_OK"),
			 (char*)LS(MSG_METHOD_EDITOR_ERROR_MISSING_IDENTIFIER, "Method %s() has no identifier value."),
			 (IPTR)me.Name);
			xset(window, MUIA_Window_ActiveObject, d->Identifier);
			return FALSE;
		}

		DoMethod(list, MUIM_List_InsertSingle, (IPTR)&me, msg->Position);
		return TRUE;
	}

	return FALSE;
}

///
/// MethodEditorClear()

// This method clears editor gadgets (it is done when a new method is added).

IPTR MethodEditorClear(Class *cl, Object *obj, UNUSED Msg msg)
{
	struct MethodEditorData *d = INST_DATA(cl, obj);
	Object *window;
	STRPTR module_name;

	d->Standard = FALSE;
	xset(d->NamePages, MUIA_Group_ActivePage, 0);
	xset(d->Name, MUIA_String_Contents, "");
	xset(d->Function, MUIA_String_Contents, "");
	xset(d->StructPages, MUIA_Group_ActivePage, 0);
	xset(d->Struct, MUIA_String_Contents, "");
	xset(d->IdentifierPages, MUIA_Group_ActivePage, 0);
	xset(d->Identifier, MUIA_String_Contents, "");
	window = (Object*)xget(obj, EDLA_Window);
	module_name = (STRPTR)xget(d->Generator, GENA_UnitName);
	FmtNPut(d->WinTitle, (STRPTR)"%s/%s", 144, module_name,
     LS(MSG_METHOD_EDITOR_WINDOW_TITLE_NEW_METHOD, "new method"));
	xset(window, MUIA_Window_Title, d->WinTitle);
    return 0;
}


///
/// MethodEditorDispatcher()

#ifdef __AROS__
BOOPSI_DISPATCHER(IPTR, MethodEditorGate, cl, obj, msg)
{
#else
IPTR MethodEditorDispatcher(void)
{
	Class *cl = (Class*)REG_A0;
	Object *obj = (Object*)REG_A2;
	Msg msg = (Msg)REG_A1;
#endif

	switch (msg->MethodID)
	{
		case OM_NEW:              return MethodEditorNew(cl, obj, (struct opSet*)msg);
		case OM_GET:              return MethodEditorGet(cl, obj, (struct opGet*)msg);
		case EDLM_BuildEditor:    return MethodEditorBuildEditor(cl, obj, msg);
		case EDLM_ListToWindow:   return MethodEditorListToWindow(cl, obj, (struct EDLP_ListToWindow*)msg);
		case EDLM_WindowToList:   return MethodEditorWindowToList(cl, obj, (struct EDLP_WindowToList*)msg);
		case EDLM_Clear:          return MethodEditorClear(cl, obj, msg);
		default:                  return DoSuperMethodA(cl, obj, msg);
	}
}
#ifdef __AROS__
BOOPSI_DISPATCHER_END
#endif

///

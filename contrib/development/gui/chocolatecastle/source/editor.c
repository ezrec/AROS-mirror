/*------------------------------------------*/
/* Code generated with ChocolateCastle 0.4  */
/* written by Grzegorz "Krashan" Kraszewski */
/* <krashan@teleinfo.pb.edu.pl>             */
/*------------------------------------------*/

/* EditorClass code. */

#include <libraries/mui.h>

#include "support.h"
#include "editor.h"
#include "locale.h"

/// definitions

struct MUI_CustomClass *EditorClass;

/* INTERNAL OBJECT IDENTIFIERS */

#define EDLIST_ADD       0x6EDA2911ul
#define EDLIST_EDIT      0x6EDA8E13ul
#define EDLIST_DELETE    0x6EDAB229ul
#define EDWIN_OK         0x6EDA6FA3ul

/* INTERNAL METHODS */

#define EDLM_Finalize    0x6EDAA722ul
#define EDLM_Buttons     0x6EDA7E36ul

struct EDLP_Finalize
{
	STACKED ULONG MethodID;
	STACKED IPTR  Accept;
};

struct EDLP_Buttons
{
	STACKED ULONG MethodID;
	STACKED IPTR  ActEntry;
};


///
/// dispatcher prototype

#ifdef __AROS__
AROS_UFP3(IPTR, EditorDispatcher,
AROS_UFPA(Class  *, cl,  A0),
AROS_UFPA(Object *, obj, A2),
AROS_UFPA(Msg     , msg, A1));
#else
intptr_t EditorDispatcher(void);
const struct EmulLibEntry EditorGate = {TRAP_LIB, 0, (void(*)(void))EditorDispatcher};
#endif

///
/// EditorData

struct EditorData
{
	Object *List;
	Object *Listview;
	Object *EdWindow;
	Object *AddButton;
	Object *EditButton;
	Object *DeleteButton;
	ULONG   InsertPos;
	ULONG   ActionType;    // edit or add
};

///
/// CreateEditorClass()

struct MUI_CustomClass *CreateEditorClass(void)
{
	struct MUI_CustomClass *cl;

	#ifdef __MORPHOS__
	cl = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct EditorData), (APTR)&EditorGate);
	#else
	cl = MUI_CreateCustomClass(NULL, MUIC_Group, NULL, sizeof(struct EditorData), (APTR)EditorDispatcher);
	#endif
	EditorClass = cl;
	return cl;
}

///
/// DeleteEditorClass()

void DeleteEditorClass(void)
{
	if (EditorClass) MUI_DeleteCustomClass(EditorClass);
}

///

/// create_listview()

Object *create_listview(Object *list)
{
	Object *obj;

	obj = MUI_NewObjectM(MUIC_Listview,
		MUIA_Listview_List, list,
	TAG_END);

	return obj;
}

///
/// create_buttons_row()

Object *create_buttons_row(void)
{
	Object *obj;

	obj = MUI_NewObjectM(MUIC_Group,
		MUIA_Group_Horiz, TRUE,
		MUIA_Group_HorizSpacing, 0,
		MUIA_Group_Child, MUI_NewObjectM(MUIC_Text,
			MUIA_Text_Contents, LS(MSG_EDITOR_BUTTON_ADD, "Add"),
			MUIA_Text_PreParse, "\33c",
			MUIA_Text_HiChar, LS(MSG_EDITOR_BUTTON_ADD_HOTKEY, "a")[0],
			MUIA_ControlChar, LS(MSG_EDITOR_BUTTON_ADD_HOTKEY, "a")[0],
			MUIA_Frame, MUIV_Frame_Button,
			MUIA_Background, MUII_ButtonBack,
			MUIA_Font, MUIV_Font_Button,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			MUIA_CycleChain, TRUE,
			MUIA_UserData, EDLIST_ADD,
		TAG_END),
		MUIA_Group_Child, MUI_NewObjectM(MUIC_Text,
			MUIA_Text_Contents, LS(MSG_EDITOR_BUTTON_EDIT, "Edit"),
			MUIA_Text_PreParse, "\33c",
			MUIA_Text_HiChar, LS(MSG_EDITOR_BUTTON_EDIT_HOTKEY, "e")[0],
			MUIA_ControlChar, LS(MSG_EDITOR_BUTTON_EDIT_HOTKEY, "e")[0],
			MUIA_Frame, MUIV_Frame_Button,
			MUIA_Background, MUII_ButtonBack,
			MUIA_Font, MUIV_Font_Button,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			MUIA_CycleChain, TRUE,
			MUIA_UserData, EDLIST_EDIT,
			MUIA_Disabled, TRUE,
		TAG_END),
		MUIA_Group_Child, MUI_NewObjectM(MUIC_Text,
			MUIA_Text_Contents, LS(MSG_EDITOR_BUTTON_DELETE, "Delete"),
			MUIA_Text_PreParse, "\33c",
			MUIA_Text_HiChar, LS(MSG_EDITOR_BUTTON_DELETE_HOTKEY, "d")[0],
			MUIA_ControlChar, LS(MSG_EDITOR_BUTTON_DELETE_HOTKEY, "d")[0],
			MUIA_Frame, MUIV_Frame_Button,
			MUIA_Background, MUII_ButtonBack,
			MUIA_Font, MUIV_Font_Button,
			MUIA_InputMode, MUIV_InputMode_RelVerify,
			MUIA_CycleChain, TRUE,
			MUIA_UserData, EDLIST_DELETE,
			MUIA_Disabled, TRUE,
		TAG_END),
	TAG_END);

	return obj;
}

///
/// CreateEditWindow()

Object *CreateEditWindow(Object *obj)
{
	Object *obj2;

	obj2 = MUI_NewObjectM(MUIC_Window,
		MUIA_Window_ID, xget(obj, EDLA_WindowID),
		MUIA_Window_Title, xget(obj, EDLA_WindowTitle),
		MUIA_Window_ScreenTitle, ScreenTitle,
		MUIA_Window_RootObject, MUI_NewObjectM(MUIC_Group,
			MUIA_Group_Child, DoMethod(obj, EDLM_BuildEditor),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Group,
				MUIA_Group_Horiz, TRUE,
				MUIA_Group_Child, MUI_NewObjectM(MUIC_Rectangle,
				TAG_END),
				MUIA_Group_Child, MUI_NewObjectM(MUIC_Text,
					MUIA_Frame, MUIV_Frame_Button,
					MUIA_Background, MUII_ButtonBack,
					MUIA_Font, MUIV_Font_Button,
					MUIA_CycleChain, TRUE,
					MUIA_UserData, EDWIN_OK,
					MUIA_InputMode, MUIV_InputMode_RelVerify,
					MUIA_ControlChar, LS(MSG_EDITOR_BUTTON_OK_HOTKEY, "o")[0],
					MUIA_Text_HiChar, LS(MSG_EDITOR_BUTTON_OK_HOTKEY, "o")[0],
					MUIA_Text_Contents, LS(MSG_EDITOR_BUTTON_OK, "OK"),
					MUIA_Text_PreParse, "\33c",
				TAG_END),
				MUIA_Group_Child, MUI_NewObjectM(MUIC_Rectangle,
				TAG_END),
			TAG_END),
		TAG_END),
	TAG_END);

	return obj2;
}

///
/// EditWindowNotifications()

void EditWindowNotifications(Object *editor, Object *edwin)
{
	DoMethod(edwin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, (IPTR)editor, 2, EDLM_Finalize, FALSE);
	DoMethod(findobj(edwin, EDWIN_OK), MUIM_Notify, MUIA_Pressed, FALSE, (IPTR)editor, 2, EDLM_Finalize, TRUE);
}

///
/// edl_notifications()

void edl_notifications(Object *obj, struct EditorData *d)
{
	DoMethod(d->AddButton, MUIM_Notify, MUIA_Pressed, FALSE, (intptr_t)obj, 2, EDLM_Action, EDLV_Action_Add);
	DoMethod(d->EditButton, MUIM_Notify, MUIA_Pressed, FALSE, (intptr_t)obj, 2, EDLM_Action, EDLV_Action_Edit);
	DoMethod(d->DeleteButton, MUIM_Notify, MUIA_Pressed, FALSE, (intptr_t)d->List, 2, MUIM_List_Remove, MUIV_List_Remove_Active);
	DoMethod(d->List, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime, (intptr_t)obj, 2, EDLM_Buttons, MUIV_TriggerValue);
	DoMethod(d->Listview, MUIM_Notify, MUIA_Listview_DoubleClick, MUIV_EveryTime, (intptr_t)obj, 2, EDLM_Action, EDLV_Action_Edit);
}

///

/// EditorNew()

IPTR EditorNew(Class *cl, Object *obj, struct opSet *msg)
{
	Object *newobj = 0;
	Object *list, *listview, *brow;

	list = (Object*)GetTagData(EDLA_List, 0, msg->ops_AttrList);

	obj = DoSuperNewM(cl, obj,
		MUIA_Group_VertSpacing, 0, 
		MUIA_Group_Child, listview = create_listview(list),
		MUIA_Group_Child, brow = create_buttons_row(),
	TAG_MORE, msg->ops_AttrList);

	if (obj)
	{
		struct EditorData *d = INST_DATA(cl, obj);

		if (d->EdWindow = CreateEditWindow(obj))
		{
			DoMethod(App, OM_ADDMEMBER, (intptr_t)d->EdWindow);
			EditWindowNotifications(obj, d->EdWindow);
			d->List = list;
			d->Listview = listview;
			d->AddButton = findobj(brow, EDLIST_ADD);
			d->EditButton = findobj(brow, EDLIST_EDIT);
			d->DeleteButton = findobj(brow, EDLIST_DELETE);
			edl_notifications(obj, d);

			newobj = obj;
		}
	}

	if (!newobj) CoerceMethod(cl, obj, OM_DISPOSE);

	return (IPTR)newobj;
}

///
/// EditorGet()

intptr_t EditorGet(Class *cl, Object *obj, struct opGet *msg)
{
	struct EditorData *d = (struct EditorData*)INST_DATA(cl, obj);

	switch (msg->opg_AttrID)
	{
		case EDLA_WindowID:
			*msg->opg_Storage = 0;
		return TRUE;
		
		case EDLA_WindowTitle:
			*(STRPTR*)msg->opg_Storage = (STRPTR)"?";
		return TRUE;

		case EDLA_List:
			*(Object**)msg->opg_Storage = d->List;
		break;

		case EDLA_Window:
			*(Object**)msg->opg_Storage = d->EdWindow;
		break;
	}
	return DoSuperMethodA(cl, obj, (Msg)msg);
}

///
/// EditorAction()

IPTR EditorAction(Class *cl, Object *obj, struct EDLP_Action *msg)
{
	struct EditorData *d = INST_DATA(cl, obj);

	d->ActionType = msg->ActionType;

	if (d->ActionType == EDLV_Action_Edit)
	{
		LONG aentry;

		aentry = xget(d->List, MUIA_List_Active);

		if (aentry != MUIV_List_Active_Off)
		{
			DoMethod(obj, EDLM_ListToWindow, aentry);
			d->InsertPos = aentry;
		}
	}
	else
	{	
		DoMethod(obj, EDLM_Clear);
		d->InsertPos = MUIV_List_Insert_Bottom;
	}

	xset(_win(obj), MUIA_Window_Sleep, TRUE);
	xset(d->EdWindow, MUIA_Window_Open, TRUE);

	return 0;
}

///
/// EditorFinalize()
/*----------------------------------------------------------------------------*/
/* The method is called when entry edit window is closed either with window   */
/* closegadget (negative), or 'OK' button (positive). Entry edit window is    */
/* closed, then main editor window is waken up. If it is a positive closing,  */
/* and action was 'edit', the edited entry is removed from the list, and new  */
/* one is created in the same place. For 'add' action, a new entry is just    */
/* added at the bottom of the list. The new entry is created and inserted     */
/* by a subclass in EDLM_WindowToList() method.                               */
/*                                                                            */
/* Added, 22.01.2008: EDLM_WindowToList() called on positive edit window      */
/* closing verifies integrity of data entered by an user. In case some data   */
/* are missing or malformed, EDLM_WindowToList() displays a requester         */
/* informing user of the problem, then does not insert an entry and returns   */
/* FALSE (it returns a boolean value now). Then this method does not close    */
/* edit window, and does not wake main editor window up, so user can fix the  */
/* problem and press 'OK' again.                                              */
/*----------------------------------------------------------------------------*/

IPTR EditorFinalize(Class *cl, Object *obj, struct EDLP_Finalize *msg)
{
	struct EditorData *d = INST_DATA(cl, obj);

	if (msg->Accept)
	{
		if (d->ActionType == EDLV_Action_Edit)
		{
			if (DoMethod(obj, EDLM_WindowToList, d->InsertPos))
			{
				DoMethod(d->List, MUIM_List_Remove, d->InsertPos + 1);
				xset(d->List, MUIA_List_Active, d->InsertPos);
				xset(d->EdWindow, MUIA_Window_Open, FALSE);
				xset(_win(obj), MUIA_Window_Sleep, FALSE);
			}
		}
		else     // EDLV_Action_Add
		{
			if (DoMethod(obj, EDLM_WindowToList, d->InsertPos))
			{
				xset(d->EdWindow, MUIA_Window_Open, FALSE);
				xset(_win(obj), MUIA_Window_Sleep, FALSE);
			}
		}
	}
	else    // window cancelled
	{
		xset(d->EdWindow, MUIA_Window_Open, FALSE);
		xset(_win(obj), MUIA_Window_Sleep, FALSE);
	}

	return 0;
}

///
/// EditorButtons()

IPTR EditorButtons(Class *cl, Object *obj, struct EDLP_Buttons *msg)
{
	struct EditorData *d = INST_DATA(cl, obj);

	if ((LONG)msg->ActEntry == MUIV_List_Active_Off)
	{
		xset(d->DeleteButton, MUIA_Disabled, TRUE);
		xset(d->EditButton, MUIA_Disabled, TRUE);
	}
	else
	{
		xset(d->DeleteButton, MUIA_Disabled, FALSE);
		xset(d->EditButton, MUIA_Disabled, FALSE);
	}

	return 0;
}

///
/// EditorBuildEditor()

IPTR EditorBuildEditor(UNUSED Class *cl, UNUSED Object *obj)
{
	Object *obj2;

	obj2 = MUI_NewObjectM(MUIC_Text,
		MUIA_Text_Contents, "???",
	TAG_END);

	return (IPTR)obj2;
}

///
/// EditorDispatcher()

#ifdef __AROS__
BOOPSI_DISPATCHER(IPTR, EditorDispatcher, cl, obj, msg)
{
#else
intptr_t EditorDispatcher(void)
{
	Class *cl = (Class*)REG_A0;
	Object *obj = (Object*)REG_A2;
	Msg msg = (Msg)REG_A1;
#endif

	switch (msg->MethodID)
	{
		case OM_NEW:             return (EditorNew(cl, obj, (struct opSet*)msg));
		case OM_GET:             return (EditorGet(cl, obj, (struct opGet*)msg));
		case EDLM_Action:        return (EditorAction(cl, obj, (struct EDLP_Action*)msg));
		case EDLM_Finalize:      return (EditorFinalize(cl, obj, (struct EDLP_Finalize*)msg));
		case EDLM_Buttons:       return (EditorButtons(cl, obj, (struct EDLP_Buttons*)msg));
		case EDLM_BuildEditor:   return (EditorBuildEditor(cl, obj));
		default:                 return (DoSuperMethodA(cl, obj, msg));
	}
}
#ifdef __AROS__
BOOPSI_DISPATCHER_END
#endif

///

/* Generator class body */

/*------------------------------------------*/
/* Code generated with ChocolateCastle 0.1  */
/* written by Grzegorz "Krashan" Kraszewski */
/* <krashan@teleinfo.pb.bialystok.pl>       */
/*------------------------------------------*/

#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <proto/dos.h>
#include <clib/debug_protos.h>
#include <clib/alib_protos.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <libvstring.h>

#include "generator.h"
#include "support.h"
#include "locale.h"

#define OBJ_DIR             0x6EDA4894ul   // directory string gadget
#define OBJ_CLASS           0x6EDA9373ul   // class name string gadget
#define OBJ_GENBUT          0x6EDA9002ul   // generate text button
#define OBJ_EGROUP          0x6EDA9483ul   // space for subclasses
#define OBJ_REMARKS         0x6EDA839Aul   // radio button for remarks
#define OBJ_SAVBUT          0x6EDA839Dul   // save text button
#define OBJ_LODBUT          0x6EDA839Eul   // load text button
#define OBJ_DOCCHECK        0x6EDA839Ful   // autodoc template generation checkmark

#define REMARKS_WARNINGS    0
#define REMARKS_COMMENTS    1

extern struct Library *IntuitionBase, *MUIMasterBase;

/// GeneratorData

struct GeneratorData
{
	Object *ModuleName;
	Object *DestDir;
	Object *GenerateButton;
	Object *SaveButton;
	Object *LoadButton;
	Object *EmptyGroup;
	Object *TodoRemarksRadio;
	Object *DocCheck;
	Object *LibGroup;           // optional MUI group for library parameters
	BPTR    FileHandle;
	int     Indent;
	STRPTR  ClassName;
	STRPTR  ClassNameSmall;
	int     Condition;
	UBYTE   LineBuf[INPUT_LINE_MAX_LEN];
};


///

struct MUI_CustomClass *GeneratorClass;

#ifdef __AROS__
AROS_UFP3(IPTR, g_Generator,
AROS_UFPA(Class  *, cl,  A0),
AROS_UFPA(Object *, obj, A2),
AROS_UFPA(Msg     , msg, A1));
#else
IPTR d_Generator(void);

static struct EmulLibEntry g_Generator = {TRAP_LIB, 0, (void(*)(void))d_Generator};
#endif

/// CreateGeneratorClass()

struct MUI_CustomClass *CreateGeneratorClass(void)
{
	struct MUI_CustomClass *cl;

	cl = MUI_CreateCustomClass(NULL, MUIC_Window, NULL, sizeof(struct GeneratorData), &g_Generator);

	GeneratorClass = cl;
	return cl;
}


///
/// DeleteGeneratorClass()

void DeleteGeneratorClass(void)
{
	 MUI_DeleteCustomClass(GeneratorClass);
}


///

/// create_root()

Object *create_root(void)
{
	Object *o;

	o = MUI_NewObjectM(MUIC_Group,
		MUIA_Group_Child, MUI_NewObjectM(MUIC_Group,
			MUIA_Group_Columns, 2,
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Text,
				MUIA_Text_Contents, LS(MSG_PROJECT_SOURCE_DIR_LABEL, "Source Drawer:"),
				MUIA_Text_PreParse, "\33r",
				MUIA_Frame, MUIV_Frame_String,
				MUIA_FramePhantomHoriz, TRUE,
				MUIA_HorizWeight, 0,
			TAG_END),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Popasl,
				MUIA_ShortHelp, LS(MSG_PROJECT_SOURCE_DIR_HELP,
					"Path to a drawer where generated files will be stored. "
                    " A subdirectory \33bwill not\33n be created."),
				MUIA_CycleChain, TRUE,
				MUIA_Popasl_Type, ASL_FileRequest,
				ASLFR_TitleText, LS(MSG_PROJECT_SOURCE_DIR_REQUEST_TITLE, "Select Source Drawer"),
				ASLFR_DrawersOnly, TRUE,
				MUIA_Popstring_String, MUI_NewObjectM(MUIC_String,
					MUIA_Frame, MUIV_Frame_String,
					MUIA_String_Reject, "#?~()|*",
					MUIA_UserData, OBJ_DIR,
					MUIA_String_AdvanceOnCR, TRUE,
				TAG_END),
				MUIA_Popstring_Button, MUI_NewObjectM(MUIC_Image,
					MUIA_Image_Spec, "6:20",
					MUIA_InputMode, MUIV_InputMode_RelVerify,
					MUIA_Frame, MUIV_Frame_ImageButton,
					MUIA_Image_FreeVert, TRUE,
				TAG_END),
			TAG_END),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Text,
				MUIA_Text_Contents, LS(MSG_PROJECT_MODULE_NAME_LABEL, "Module Name:"),
				MUIA_Text_PreParse, "\33r",
				MUIA_Frame, MUIV_Frame_String,
				MUIA_FramePhantomHoriz, TRUE,
				MUIA_HorizWeight, 0,
			TAG_END),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_String,
				MUIA_ShortHelp, LS(MSG_PROJECT_MODULE_NAME_HELP,
					"This name is used in functions or methods names, class pointer or library base"
					" name and, after lowercasing, in file names."),
				MUIA_CycleChain, TRUE,
				MUIA_Frame, MUIV_Frame_String,
				MUIA_String_MaxLen, 40,
				MUIA_String_Accept, IdAllowed,
				MUIA_UserData, OBJ_CLASS,
				MUIA_String_AdvanceOnCR, TRUE,
			TAG_END),
		TAG_END),
		MUIA_Group_Child, MUI_NewObjectM(MUIC_Group,
			MUIA_Group_Horiz, TRUE,
			MUIA_ShortHelp, LS(MSG_GENERATOR_AUTODOC_CHECKMARK_HELP,
				"Generates partially filled autodoc template for every library function or class method."),
			MUIA_Group_Child, labelled_checkmark(LS(MSG_GENERATOR_AUTODOC_CHECKMARK_LABEL,
			 "Generate autodoc templates."), OBJ_DOCCHECK),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Rectangle,
			TAG_END),
		TAG_END),
		MUIA_Group_Child, MUI_NewObjectM(MUIC_Group,
			MUIA_UserData, OBJ_EGROUP,
		TAG_END),
		MUIA_Group_Child, MUI_NewObjectM(MUIC_Group,
			MUIA_ShortHelp, LS(MSG_PROJECT_TODO_REMARKS_HELP,
				"ChocolateCastle inserts remarks in places of code, which must be completed"
				" manually. These remarks may be just plain comments, or compiler warnings"
				" generated with #warning preprocessor command. The later will be printed out"
				" during compilation."),
			MUIA_Frame, MUIV_Frame_Group,
			MUIA_FrameTitle, LS(MSG_PROJECT_TODO_REMARKS_GROUP_TITLE, "TODO Remarks"),
			MUIA_Background, MUII_GroupBack,
			MUIA_Group_Horiz, TRUE,
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Rectangle, TAG_END),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Radio,
				MUIA_CycleChain, TRUE,
				MUIA_Radio_Entries, TodoRadioEntries,
				MUIA_UserData, OBJ_REMARKS,
			TAG_END),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Rectangle, TAG_END),
		TAG_END),

		MUIA_Group_Child, MUI_NewObjectM(MUIC_Group,
			MUIA_Group_Horiz, TRUE,
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Text,
				MUIA_CycleChain, TRUE,
				MUIA_Text_Contents, LS(MSG_PROJECT_GENERATE_BUTTON, "Generate"),
				MUIA_Text_PreParse, "\33c",
				MUIA_Frame, MUIV_Frame_Button,
				MUIA_Background, MUII_ButtonBack,
				MUIA_Font, MUIV_Font_Button,
				MUIA_InputMode, MUIV_InputMode_RelVerify,
				MUIA_ControlChar, LS(MSG_PROJECT_GENERATE_BUTTON_HOTKEY, "g")[0],
				MUIA_Text_HiChar, LS(MSG_PROJECT_GENERATE_BUTTON_HOTKEY, "g")[0],
				MUIA_UserData, OBJ_GENBUT,
			TAG_END),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Text,
				MUIA_CycleChain, TRUE,
				MUIA_Text_Contents, LS(MSG_PROJECT_SAVE_BUTTON, "Save"),
				MUIA_Text_PreParse, "\33c",
				MUIA_Frame, MUIV_Frame_Button,
				MUIA_Background, MUII_ButtonBack,
				MUIA_Font, MUIV_Font_Button,
				MUIA_InputMode, MUIV_InputMode_RelVerify,
				MUIA_ControlChar, LS(MSG_PROJECT_SAVE_BUTTON_HOTKEY, "s")[0],
				MUIA_Text_HiChar, LS(MSG_PROJECT_SAVE_BUTTON_HOTKEY, "s")[0],
				MUIA_UserData, OBJ_SAVBUT,
			TAG_END),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Text,
				MUIA_CycleChain, TRUE,
				MUIA_Text_Contents, LS(MSG_PROJECT_LOAD_BUTTON, "Load"),
				MUIA_Text_PreParse, "\33c",
				MUIA_Frame, MUIV_Frame_Button,
				MUIA_Background, MUII_ButtonBack,
				MUIA_Font, MUIV_Font_Button,
				MUIA_InputMode, MUIV_InputMode_RelVerify,
				MUIA_ControlChar, LS(MSG_PROJECT_LOAD_BUTTON_HOTKEY, "l")[0],
				MUIA_Text_HiChar, LS(MSG_PROJECT_LOAD_BUTTON_HOTKEY, "l")[0],
				MUIA_UserData, OBJ_LODBUT,
			TAG_END),
		TAG_END),
	End;

	return o;
}

///
/// main_window_notifications()

void main_window_notifications(Object *obj, struct GeneratorData *d)
{
	DoMethod(d->GenerateButton, MUIM_Notify, MUIA_Pressed, FALSE, (IPTR)obj, 1, GENM_GenerateAction);
	DoMethod(d->SaveButton, MUIM_Notify, MUIA_Pressed, FALSE, (IPTR)obj, 1, GENM_SaveAction);
	DoMethod(d->LoadButton, MUIM_Notify, MUIA_Pressed, FALSE, (IPTR)obj, 1, GENM_LoadAction);

	/* Trigger notifications on GENA_UnitName. */

	DoMethod(d->ModuleName, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime, (IPTR)obj, 3, MUIM_Set, GENA_UnitName, TRUE);
}

///
/// generate_library_bases_definitions()

static void generate_library_bases_definitions(Object *obj)
{
	T("struct Library\n");
	II; I; T("*SysBase,\n");
	I; T("*IntuitionBase,\n");
	I; T("*UtilityBase,\n");
	I; T("*MultimediaBase;\n");
	IO;
}


///
/// generate_init_reggae_class()

static void generate_init_reggae_class(Object *obj)
{
	LEAD("init_class()");
	T("Class *init_class(struct LibBase *lb)\n{\n");
	II; I; T("Class *cl = NULL;\n\n");
	I; TC("if ((cl = Create%sClass()))\n");
	I; T("{\n"); II; I; T("cl->cl_UserData = (ULONG)lb;\n"); I; T("AddClass(cl);\n"); IO;
	I; T("}\n\n"); I; T("lb->MyClass = cl;\n"); I; T("return cl;\n"); IO; T("}\n");
	TRAIL("init_class()");
}


///
/// generate_init_resources()

void generate_init_resources(Object *obj, LONG project_type)
{
	LEAD("InitResources()");
	T("BOOL InitResources(struct LibBase *lb)\n{\n"); II;
	I; T("if (!(IntuitionBase = OpenLibrary((STRPTR)\"intuition.library\", 50))) return FALSE;\n");
	I; T("if (!(UtilityBase = OpenLibrary((STRPTR)\"utility.library\", 50))) return FALSE;\n");

	if (project_type == PROJECT_TYPE_REGGAE)
	{
		I; T("if (!(MultimediaBase = OpenLibrary((STRPTR)\"multimedia/multimedia.class\", 52))) return FALSE;\n");
		I; T("if (!(init_class(lb))) return FALSE;\n");
	}

	I; T("return TRUE;\n"); IO;
	T("}\n");
	TRAIL("InitResources()");
}


///
/// generate_free_resources()

void generate_free_resources(Object *obj, LONG project_type)
{
	LEAD("FreeResources()");
	T("void FreeResources()\n{\n"); II;
	I; T("if (UtilityBase) CloseLibrary(UtilityBase);\n");
	I; T("if (IntuitionBase) CloseLibrary(IntuitionBase);\n");

	if (project_type == PROJECT_TYPE_REGGAE)
	{
		I; T("if (MultimediaBase) CloseLibrary(MultimediaBase);\n");
	}

	I; T("return;\n"); IO;
	T("}\n");
	TRAIL("FreeResources()");
}


///

/// GeneratorNew()

IPTR GeneratorNew(Class *cl, Object *obj, struct opSet *msg)
{
	Object *newobj = NULL;

	if (obj = DoSuperNewM(cl, obj,
		MUIA_Window_RootObject, create_root(),
		MUIA_Window_ID, 0x47454E45,
		MUIA_Window_ScreenTitle, ScreenTitle,
	TAG_MORE, msg->ops_AttrList))
	{
		struct GeneratorData *d = INST_DATA(cl, obj);

		d->DestDir = findobj(obj, OBJ_DIR);
		d->ModuleName = findobj(obj, OBJ_CLASS);
		d->GenerateButton = findobj(obj, OBJ_GENBUT);
		d->SaveButton = findobj(obj, OBJ_SAVBUT);
		d->LoadButton = findobj(obj, OBJ_LODBUT);
		d->EmptyGroup = findobj(obj, OBJ_EGROUP);
		d->TodoRemarksRadio = findobj(obj, OBJ_REMARKS);
		d->DocCheck = findobj(obj, OBJ_DOCCHECK);

		main_window_notifications(obj, d);

		newobj = obj;

		if (!newobj) CoerceMethod(cl, obj, OM_DISPOSE);
	}
	return (IPTR)newobj;
}

///
/// GeneratorGet()

IPTR GeneratorGet(Class *cl, Object *obj, struct opGet *msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);

	switch (msg->opg_AttrID)
	{
		case GENA_SubclassSpace:
		 *msg->opg_Storage = (ULONG)d->EmptyGroup;
		return TRUE;

		case GENA_UnitName:
		 *msg->opg_Storage = (ULONG)xget(d->ModuleName, MUIA_String_Contents);
		return TRUE;

		case GENA_ProjectType:
		 *(STRPTR*)msg->opg_Storage = (STRPTR)"Unknown";
		return TRUE;
	}
	return DoSuperMethodA(cl, obj, (Msg)msg);
}

///
/// GeneratorSetup()

int GeneratorSetup(Class *cl, Object *obj, struct GENP_Setup *msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);
	int rv = FALSE;
	STRPTR filename, lowercase_name, dest_dir;

	GetAttr(MUIA_String_Contents, d->DestDir, (IPTR *)&dest_dir);
	GetAttr(MUIA_String_Contents, d->ModuleName, (IPTR *)&filename);

	if (lowercase_name = FmtNew(msg->NamePattern, filename))
	{
		BPTR dir_lock;

		strlow(lowercase_name);

		if (dir_lock = Lock((STRPTR)dest_dir, SHARED_LOCK))
		{
			BPTR olddir;

			olddir = CurrentDir(dir_lock);
			if (d->FileHandle = Open((STRPTR)lowercase_name, MODE_NEWFILE))
			{
				if (d->ClassName = FmtNew((STRPTR)"%s", filename))
				{
					if (d->ClassNameSmall = FmtNew((STRPTR)"%s", filename))
					{
						strlow(d->ClassNameSmall);
						rv = TRUE;
					}
				}
			}
			CurrentDir(olddir);
			UnLock(dir_lock);
		}
		FreeVecPooled(MPool, lowercase_name);
	}
	return rv;
}

///
/// GeneratorCleanup()

IPTR GeneratorCleanup(Class *cl, Object *obj, Msg msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);

	msg = msg;
	if (d->FileHandle) Close(d->FileHandle);
	d->FileHandle = BNULL;
	FreeVecPooled(MPool, d->ClassName);
	d->ClassName = NULL;
	FreeVecPooled(MPool, d->ClassNameSmall);
	d->ClassNameSmall = NULL;

	return 0;
}

///
/// GeneratorSignature()

IPTR GeneratorSignature(Class *cl, Object *obj, Msg msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);

	msg = msg;

#ifdef __AROS__

	FPuts(d->FileHandle, (STRPTR)"/*\n");
	FPuts(d->FileHandle, (STRPTR)"    Copyright © 2012, The AROS Development Team. All rights reserved.\n");
	FPuts(d->FileHandle, (STRPTR)"    $Id");
	FPuts(d->FileHandle, (STRPTR)"$\n*/\n\n");

#else

	FPuts(d->FileHandle, (STRPTR)"/*------------------------------------------*/\n");
	FPuts(d->FileHandle, (STRPTR)"/* Code generated with ChocolateCastle " CHC_VERSION "  */\n");
	FPuts(d->FileHandle, (STRPTR)"/* written by Grzegorz \"Krashan\" Kraszewski */\n");
	FPuts(d->FileHandle, (STRPTR)"/* <krashan@teleinfo.pb.edu.pl>             */\n");
	FPuts(d->FileHandle, (STRPTR)"/*------------------------------------------*/\n\n");

#endif

	return 0;
}

///
/// GeneratorInsertRemark()

IPTR GeneratorInsertRemark(Class *cl, Object *obj, struct GENP_InsertRemark *msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);
	IPTR remark_mode;

	msg = msg;
	DoMethod(obj, GENM_DoIndent);
	GetAttr(MUIA_Radio_Active, d->TodoRemarksRadio, &remark_mode);

	switch (remark_mode)
	{
		case REMARKS_WARNINGS: FPuts(d->FileHandle, (STRPTR)"#warning "); break;
		case REMARKS_COMMENTS: FPuts(d->FileHandle, (STRPTR)"// "); break;
	}

	FPrintf(d->FileHandle, (STRPTR)"TODO: %s\n", (IPTR)msg->Text);

	return 0;
}

///
/// GeneratorIndentIn()

IPTR GeneratorIndentIn(Class *cl, Object *obj, Msg msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);

	msg = msg;
	d->Indent++;

	return 0;
}

///
/// GeneratorIndentOut()

IPTR GeneratorIndentOut(Class *cl, Object *obj, Msg msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);

	msg = msg;
	if (d->Indent > 0) d->Indent--;
	else Printf((STRPTR)"Indentation error!\n");

	return 0;
}

///
/// GeneratorDoIndent()

IPTR GeneratorDoIndent(Class *cl, Object *obj, Msg msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);
	ULONG i = d->Indent;

	msg = msg;

#ifdef __AROS__
	while (i--) FPuts(d->FileHandle, "    ");
#else
	while (i--) FPutC(d->FileHandle, '\t');
#endif

	return 0;
}

///
/// GeneratorText()

IPTR GeneratorText(Class *cl, Object *obj, struct GENP_Text *msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);

	msg = msg;
	FPuts(d->FileHandle, (STRPTR)msg->Text);

	return 0;
}

///
/// GeneratorTextParam()

IPTR GeneratorTextParam(Class *cl, Object *obj, struct GENP_TextParam *msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);
	IPTR param = (IPTR)"";

	msg = msg;

	switch (msg->Param)
	{
		case PARAM_CLASSNAME:
			param = (IPTR)d->ClassName;
		break;
		
		case PARAM_CLASSNAMELC:
			param = (IPTR)d->ClassNameSmall;
		break;

		case PARAM_LIBNAME:
			param = xget(findobj(d->LibGroup, OBJ_LIBG_NAME), MUIA_String_Contents);
		break;

		case PARAM_VERSION:
			param = xget(findobj(d->LibGroup, OBJ_LIBG_VERSION), MUIA_String_Contents);
		break;

		case PARAM_REVISION:
			param = xget(findobj(d->LibGroup, OBJ_LIBG_REVISION), MUIA_String_Contents);
		break;

		case PARAM_COPYRIGHT:
			param = xget(findobj(d->LibGroup, OBJ_LIBG_COPYRIGHT), MUIA_String_Contents);
		break;

		default:
			param = msg->Param;
		break;
	}

	FPrintf(d->FileHandle, (STRPTR)msg->Text, param);
	return 0;
}

///
/// GeneratorSaveAction()

IPTR GeneratorSaveAction(Class *cl, Object *obj, UNUSED Msg msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);
	int success = FALSE;
	STRPTR dirname;
	struct FileRequester *freq;

	if (!DoMethod(obj, GENM_Verify)) return 0;

	dirname = (STRPTR)xget(d->DestDir, MUIA_String_Contents);

	if (freq = MUI_AllocAslRequest(ASL_FileRequest, TAG_END))
	{
		STRPTR filename;

		if (filename = FmtNew((STRPTR)"%s.chc", xget(d->ModuleName, MUIA_String_Contents)))
		{
			strlow(filename);

			if (MUI_AslRequestTags(freq,
				ASLFR_Window, (IPTR)_window(obj),
				ASLFR_TitleText, (IPTR)LS(MSG_PROJECT_SAVE_REQUESTER_TITLE, "Save Project"),
				ASLFR_PositiveText, (IPTR)LS(MSG_PROJECT_SAVE_REQUESTER_BUTTON, "Save"),
				ASLFR_InitialPattern, (IPTR)"#?.chc",
				ASLFR_DoPatterns, TRUE,
				ASLFR_DoSaveMode, TRUE,
				ASLFR_InitialDrawer, (IPTR)dirname,
				ASLFR_InitialFile, (IPTR)filename,
			TAG_END))
			{
				BPTR lock, olddir, handle;

				dirname = freq->fr_Drawer;

				if (lock = Lock((STRPTR)dirname, ACCESS_READ))
				{
					olddir = CurrentDir(lock);

					if (handle = Open(freq->fr_File, MODE_NEWFILE))
					{
						struct GENP_Save genps;

						genps.MethodID = GENM_Save;
						genps.Handle = (IPTR)handle;
						success = DoMethodA(obj, &genps);
						Close(handle);
					}
					else MUI_Request(App, obj, 0, "ChocolateCastle", (char*)LS(MSG_ERROR_REQUESTER_OK_BUTTON, "*_OK"),
					 (char*)LS(MSG_PROJECT_SAVE_OPEN_ERROR, "Can't open file \"%s\", DOS error %ld."),
					 (IPTR)freq->fr_File, IoErr());

					CurrentDir(olddir);
					UnLock(lock);
				}
				else MUI_Request(App, obj, 0, "ChocolateCastle", (char*)LS(MSG_ERROR_REQUESTER_OK_BUTTON, "*_OK"),
				 (char*)LS(MSG_PROJECT_SAVE_DIRLOCK_ERROR, "Problem with directory \"%s\", DOS error %ld."),
				 (IPTR)freq->fr_Drawer, IoErr());
			}
			FreeVecPooled(MPool, filename);
		}
		MUI_FreeAslRequest(freq);
	}

	if (success) MUI_Request(App, obj, 0, "ChocolateCastle", (char*)LS(MSG_INFO_REQUESTER_OK_BUTTON, "*_OK"),
	 (char*)LS(MSG_PROJECT_INFO_PROJECT_SAVED, "Project description has been saved in \"%s\"."),  (IPTR)dirname);

	return 0;
}

///
/// GeneratorLoadAction()

IPTR GeneratorLoadAction(Class *cl, Object *obj, Msg msg)
{
	struct FileRequester *freq;
	struct GeneratorData *d = INST_DATA(cl, obj);

	cl = cl;
	msg = msg;

	if (freq = MUI_AllocAslRequest(ASL_FileRequest, TAG_END))
	{
		if (MUI_AslRequestTags(freq,
			ASLFR_Window, (IPTR)_window(obj),
			ASLFR_TitleText, (IPTR)LS(MSG_PROJECT_LOAD_REQUESTER_TITLE, "Load Project"),
			ASLFR_PositiveText, (IPTR)LS(MSG_PROJECT_LOAD_REQUESTER_BUTTON, "Load"),
			ASLFR_InitialPattern, (IPTR)"#?.chc",
			ASLFR_DoPatterns, TRUE,
		TAG_END))
		{
			BPTR lock;

			if (lock = Lock(freq->fr_Drawer, SHARED_LOCK))
			{
				BPTR olddir, handle;

				olddir = CurrentDir(lock);

				if (handle = Open(freq->fr_File, MODE_OLDFILE))
				{
					struct GENP_Load genpl;
					struct Parser parser;

					if (parser.args = AllocDosObject(DOS_RDARGS, NULL))
					{
						parser.line = 1;
						genpl.MethodID = GENM_Load;
						genpl.Handle = (IPTR)handle;
						genpl.Parser = &parser;
						genpl.LineBuf = d->LineBuf;
						DoMethodA(obj, &genpl);
						FreeDosObject(DOS_RDARGS, parser.args);
					}
					Close(handle);
				}
				else MUI_Request(App, obj, 0, "ChocolateCastle", (char*)LS(MSG_ERROR_REQUESTER_OK_BUTTON, "*_OK"),
				 (char*)LS(MSG_PROJECT_LOAD_OPEN_ERROR, "Can't open file \"%s\", DOS error %ld."),
				 (IPTR)freq->fr_File, IoErr());

				CurrentDir(olddir);
				UnLock(lock);
			}
			else MUI_Request(App, obj, 0, "ChocolateCastle", (char*)LS(MSG_ERROR_REQUESTER_OK_BUTTON, "*_OK"),
			 (char*)LS(MSG_PROJECT_LOAD_DIRLOCK_ERROR, "Problem with directory \"%s\", DOS error %ld."),
			 (IPTR)freq->fr_Drawer, IoErr());
		}
		MUI_FreeAslRequest(freq);
	}

	return 0;
}

///
/// GeneratorSave()

IPTR GeneratorSave(Class *cl, Object *obj, struct GENP_Save *msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);
	BOOL rv = FALSE;

	FPrintf((BPTR)msg->Handle, (STRPTR)"TYPE=\"%s\"\nMODULE=%s DRAWER=\"%s\" AUTODOCS=%ld TODO=%ld\n",
	 xget(obj, GENA_ProjectType), xget(d->ModuleName, MUIA_String_Contents),
	 xget(d->DestDir, MUIA_String_Contents), xget(d->DocCheck, MUIA_Selected),
	 xget(d->TodoRemarksRadio, MUIA_Radio_Active));
	rv = TRUE;

	return rv;
}

///
/// GeneratorLoad()

IPTR GeneratorLoad(Class *cl, Object *obj, struct GENP_Load *msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);
	LONG externity = 0, remarks = 0;
	IPTR rv = FALSE;

	msg->Parser->params[0] = (LONG)LS(MSG_MODULE_UNNAMED, "Unnamed");
	msg->Parser->params[1] = (LONG)"";
	msg->Parser->params[2] = (LONG)&externity;
	msg->Parser->params[3] = (LONG)&remarks;

	//FIXME: SKIPS FOR NOW UNUSED PROJECT TYPE

	FGets((BPTR)msg->Handle, msg->LineBuf, INPUT_LINE_MAX_LEN);


	if (FGets((BPTR)msg->Handle, msg->LineBuf, INPUT_LINE_MAX_LEN))
	{
		msg->Parser->line++;

		if (check_pattern(msg->LineBuf, (STRPTR)"MODULE/K,DRAWER/K,AUTODOCS/K/N,TODO/K/N", msg->Parser))
		{
			xset(d->ModuleName, MUIA_String_Contents, msg->Parser->params[0]);
			xset(d->DestDir, MUIA_String_Contents, msg->Parser->params[1]);
			xset(d->DocCheck, MUIA_Selected, *((LONG*)msg->Parser->params[2]));
			xset(d->TodoRemarksRadio, MUIA_Radio_Active, *((LONG*)msg->Parser->params[3]));
			rv = TRUE;
		}
		else syntax_error(obj, msg->Parser);
	}
	else unexpected_eof(obj, msg->Parser);

	return rv;
}

///
/// GeneratorGenerate()

IPTR GeneratorGenerate(Class *cl, Object *obj, Msg msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);

	msg = msg;
	//FIXME: NO RETURN VALUE IS PASSED HERE, NO ERROR HANDLING!
	MUI_Request(App, obj, 0, "ChocolateCastle", (char*)LS(MSG_INFO_REQUESTER_OK_BUTTON, "*_OK"),
	 (char*)LS(MSG_PROJECT_INFO_CODE_GENERATED, "Code has been generated to \"%s\"."),
	 xget(d->DestDir, MUIA_String_Contents));

	return 0;
}

///
/// GeneratorMethodHeader()

IPTR GeneratorMethodHeader(UNUSED Class *cl, Object *obj, struct GENP_MethodHeader *msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);

#ifdef __AROS__

	TC("/*** %s__"); T(msg->FuncName); T(" ***/\n\n");

	if (xget(d->DocCheck, MUIA_Selected))
	{
		UBYTE line[79];
		STRPTR unit_name;

		unit_name = (STRPTR)xget(obj, GENA_UnitName);
		if (msg->ExtClass) unit_name = (STRPTR)xget(findobj(d->LibGroup, OBJ_LIBG_NAME), MUIA_String_Contents);

		T("/*****************************************************************************");
		T("\n    NAME\n");
		FmtNPut(line, (STRPTR)"        %s\n", 79, msg->MethodName);
		T(line);
		FmtNPut(line, "\n    LOCATION\n\n        %s\n\n    FUNCTION\n\n\n    NOTES\n\n\n    SEE ALSO\n\n",
		79, unit_name);
		T(line);
		T("\n******************************************************************************/\n\n\n");
	}

	TC("IPTR %s__"); T(msg->FuncName); T("(Class *cl, Object *obj, ");
	if (!(*msg->StructName)) T("Msg msg");
	else { T("struct "); T(msg->StructName); T(" *msg"); }
	T(")\n{\n");
	II;

	if (msg->Instance)
	{
		I; TC("struct %s_DATA *data = INST_DATA(cl, obj);\n");
	}

#else

	TC("/// %s"); T(msg->FuncName); T("()\n\n");

	if (xget(d->DocCheck, MUIA_Selected))
	{
		UBYTE line[79];
		STRPTR unit_name;

		unit_name = (STRPTR)xget(obj, GENA_UnitName);
		if (msg->ExtClass) unit_name = (STRPTR)xget(findobj(d->LibGroup, OBJ_LIBG_NAME), MUIA_String_Contents);

		FmtNPut(line, (STRPTR)"/****** %s/%s ***********************************************************************",
		 79, unit_name, msg->FuncName);
		T(line);
		T("\n* NAME\n");
		FmtNPut(line, (STRPTR)"*   %s (v0) 0x%s\n", 79, msg->MethodName, msg->Identifier);
		T(line);
		T("*\n* FUNCTION\n*\n*\n* NOTES\n*\n*\n* SEE ALSO\n*\n");
		T("*\n******************************************************************************\n*\n*/\n\n");
	}
	
    TC("IPTR %s"); T(msg->FuncName); T("(Class *cl, Object *obj, ");
	if (!(*msg->StructName)) T("Msg msg");
	else { T("struct "); T(msg->StructName); T(" *msg"); }
	T(")\n{\n");
	II;
	
	if (msg->Instance)
	{
		I; TC("struct %sData *d = INST_DATA(cl, obj);\n");
	}

#endif

	return 0;
}

///
/// GeneratorMethodFooter()

IPTR GeneratorMethodFooter(UNUSED Class *cl, Object *obj, UNUSED struct GENP_MethodFooter *msg)
{
	if (msg->ReturnZero) { I; T("return 0;\n"); }
	IO;

#ifdef __AROS__
	T("}\n\n\n");
#else
	T("}\n\n\n///\n");
#endif

	return 0;
}

///
/// GeneratorVerify()

IPTR GeneratorVerify(Class *cl, Object *obj)
{
	struct GeneratorData *d = INST_DATA(cl, obj);
	STRPTR module_name;

	module_name = (STRPTR)xget(d->ModuleName, MUIA_String_Contents);

	if (module_name[0] == 0x00)
	{
		MUI_Request(_app(obj), obj, 0, "ChocolateCastle",
		 (char*)LS(MSG_ERROR_REQUESTER_OK_BUTTON, "*_OK"),
		 (char*)LS(MSG_GENERATE_ERROR_NO_MODULE_NAME, "Module name is missing."));
		xset(obj, MUIA_Window_ActiveObject, d->ModuleName);
		return FALSE;
	}

	if ((module_name[0] != '_') && (module_name[0] < 'A'))
	{
		MUI_Request(_app(obj), obj, 0, "ChocolateCastle",
		 (char*)LS(MSG_ERROR_REQUESTER_OK_BUTTON, "*_OK"),
		 (char*)LS(MSG_GENERATE_ERROR_MODULE_NAME_DIGIT, "Module name can't start from a digit."));
		xset(obj, MUIA_Window_ActiveObject, d->ModuleName);
		return FALSE;
	}

	return TRUE;
}

///
/// GeneratorGeneateAction()

IPTR GeneratorGenerateAction(UNUSED Class *cl, Object *obj)
{
	if (DoMethod(obj, GENM_Verify)) DoMethod(obj, GENM_Generate);

	return 0;
}


///
/// GeneratorMakefileSignature()

IPTR GeneratorMakefileSignature(Class *cl, Object *obj)
{
	struct GeneratorData *d = INST_DATA(cl, obj);

	FPuts(d->FileHandle, (STRPTR)"##############################################\n");
	TLN((STRPTR)"## Makefile for %s\n");
	FPuts(d->FileHandle, (STRPTR)"## generated with ChocolateCastle " CHC_VERSION "\n");
	FPuts(d->FileHandle, (STRPTR)"## written by Grzegorz \"Krashan\" Kraszewski\n");
	FPuts(d->FileHandle, (STRPTR)"## <krashan@teleinfo.pb.edu.pl>\n");
	FPuts(d->FileHandle, (STRPTR)"##############################################\n\n");
	return 0;
}

///
/// GeneratorCreateLibGroup()

static Object* create_lib_group_label(CONST_STRPTR label)
{
	Object *obj;

	obj = MUI_NewObjectM(MUIC_Text,
		MUIA_HorizWeight, 0,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_FramePhantomHoriz, TRUE,
		MUIA_Text_Contents, label,
		MUIA_Text_PreParse, "\33r",
	TAG_END);

	return obj;
}


IPTR GeneratorCreateLibGroup(Class *cl, Object *obj)
{
	struct GeneratorData *d = INST_DATA(cl, obj);

	d->LibGroup = MUI_NewObjectM(MUIC_Group,
		MUIA_Group_Child, MUI_NewObjectM(MUIC_Group,
			MUIA_Group_Columns, 5,
			MUIA_Group_Child, create_lib_group_label(LS(MSG_LIBRARY_GROUP_NAME_LABEL, "Library Name:")),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_String,
				MUIA_Frame, MUIV_Frame_String,
				MUIA_String_Accept, "abcdefghijklmnopqrstuvwxyz.-0123456789",
				MUIA_String_MaxLen, 80,
				MUIA_UserData, OBJ_LIBG_NAME,
				MUIA_Disabled, TRUE,
				MUIA_ShortHelp, LS(MSG_LIBRARY_GROUP_NAME_HELP,
					"This name is used as a name of library executable file and library "
					"identificator in the system."),
			TAG_END),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Rectangle,
				MUIA_FixWidthTxt, "a",
			TAG_END),
			MUIA_Group_Child, create_lib_group_label(LS(MSG_LIBRARY_GROUP_VERSION_LABEL, "Version:")),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_String,
				MUIA_Frame, MUIV_Frame_String,
				MUIA_String_Accept, "0123456789",
				MUIA_FixWidthTxt, "000000",
				MUIA_String_MaxLen, 5,
				MUIA_String_Integer, 1,
				MUIA_String_Format, MUIV_String_Format_Right,
				MUIA_UserData, OBJ_LIBG_VERSION,
				MUIA_Disabled, TRUE,
				MUIA_ShortHelp, LS(MSG_LIBRARY_GROUP_VERSION_HELP,
					"Major part of library version number."),
			TAG_END),
			MUIA_Group_Child, create_lib_group_label(LS(MSG_LIBRARY_GROUP_COPYRIGHT_LABEL, "Copyright String:")),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_String,
				MUIA_Frame, MUIV_Frame_String,
				MUIA_String_MaxLen, 128,
				MUIA_UserData, OBJ_LIBG_COPYRIGHT,
				MUIA_Disabled, TRUE,
				MUIA_ShortHelp, LS(MSG_LIBRARY_GROUP_COPYRIGHT_HELP,
					"The last part of library identification string. Used for "
					"copyright notice usually."),
			TAG_END),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Rectangle,
				MUIA_FixWidthTxt, "a",
			TAG_END),
			MUIA_Group_Child, create_lib_group_label(LS(MSG_LIBRARY_GROUP_REVISION_LABEL, "Revision:")),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_String,
				MUIA_Frame, MUIV_Frame_String,
				MUIA_String_Accept, "0123456789",
				MUIA_FixWidthTxt, "000000",
				MUIA_String_MaxLen, 5,
				MUIA_String_Contents, "0",
				MUIA_String_Format, MUIV_String_Format_Right,
				MUIA_UserData, OBJ_LIBG_REVISION,
				MUIA_Disabled, TRUE,
				MUIA_ShortHelp, LS(MSG_LIBRARY_GROUP_REVISION_HELP,
					"Minor part of library version number (note that it is not a decimal fraction)."),
			TAG_END),
		TAG_END),
	TAG_END);

	return (IPTR)d->LibGroup;
}

///
/// GeneratorDisableLibGroup()

IPTR GeneratorDisableLibGroup(Class *cl, Object *obj, struct GENP_DisableLibGroup *msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);

	xset(findobj(d->LibGroup, OBJ_LIBG_NAME), MUIA_Disabled, msg->Disabled);
	xset(findobj(d->LibGroup, OBJ_LIBG_VERSION), MUIA_Disabled, msg->Disabled);
	xset(findobj(d->LibGroup, OBJ_LIBG_REVISION), MUIA_Disabled, msg->Disabled);
	xset(findobj(d->LibGroup, OBJ_LIBG_COPYRIGHT), MUIA_Disabled, msg->Disabled);
	return 0;
}

///
/// GeneratorLibVersionH()

IPTR GeneratorLibVersionH(Class *cl, Object *obj, struct GENP_LibVersionH *msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);

	if (DoMethod(obj, GENM_Setup, (IPTR)msg->FileName))
	{
		LONG v, r;

		v = xget(findobj(d->LibGroup, OBJ_LIBG_VERSION), MUIA_String_Integer);
		r = xget(findobj(d->LibGroup, OBJ_LIBG_REVISION), MUIA_String_Integer);
		DoMethod(obj, GENM_Signature);
		FPrintf(d->FileHandle, (STRPTR)"#define LIBNAME \"%s\"\n", xget(findobj(d->LibGroup, OBJ_LIBG_NAME), MUIA_String_Contents));
		FPrintf(d->FileHandle, (STRPTR)"#define VERSION %lu\n", v);
		FPrintf(d->FileHandle, (STRPTR)"#define REVISION %lu\n", r);
		FPuts(d->FileHandle, (STRPTR)"#define DATE \"xx.xx.xxxx\"\n");
		FPrintf(d->FileHandle, (STRPTR)"#define VERS LIBNAME \" %ld.%ld\"\n", v, r);
		FPrintf(d->FileHandle, (STRPTR)"#define VSTRING VERS \" (\" DATE \") %s\"\n", xget(findobj(d->LibGroup,
		 OBJ_LIBG_COPYRIGHT), MUIA_String_Contents));
		FPuts(d->FileHandle, (STRPTR)"#define VERSTAG \"\\0$VER: \" VSTRING\n");
	}

	DoMethod(obj, GENM_Cleanup);
	return 0;
}

///
/// GeneratorLibraryC()

IPTR GeneratorLibraryC(Class *cl, Object *obj, struct GENP_LibraryC *msg)
{
	// struct GeneratorData *d = INST_DATA(cl, obj);

	if (DoMethod(obj, GENM_Setup, (IPTR)"dummy.c"))
	{
		DoMethod(obj, GENM_Signature);
		T("#include <exec/types.h>\n\n");
		T("#include \"lib_version.h\"\n\n");
		LEAD("dummy()");
		T("LONG dummy(void)\n{\n");
		II; I; T("return -1;\n");
		IO; T("}\n\n\n");
		T("__attribute__ ((section (\".text\"))) const UBYTE VTag[] = VERSTAG;\n");
		TRAIL("dummy()");
	}

	DoMethod(obj, GENM_Cleanup);

	if (DoMethod(obj, GENM_Setup, (IPTR)"library.c"))
	{
		// LONG v, r; // unused

		// v = xget(findobj(d->LibGroup, OBJ_LIBG_VERSION), MUIA_String_Integer);
		// r = xget(findobj(d->LibGroup, OBJ_LIBG_REVISION), MUIA_String_Integer);
		DoMethod(obj, GENM_Signature);
		
        LEAD("includes");
		T("#define __NOLIBBASE__\n\n");
		T("#include <proto/exec.h>\n");
		T("#include <proto/dos.h>\n");
		T("#include <proto/intuition.h>\n");
		T("#include <proto/utility.h>\n");
		T("#include <proto/multimedia.h>\n\n");
		T("#include <exec/resident.h>\n");
		T("#include <exec/libraries.h>\n\n");
//		  T("#include \"library.h\"\n");
		T("#include \"lib_version.h\"\n");
		if (msg->ProjectType == PROJECT_TYPE_REGGAE) TCS("#include \"%s.h\"\n");
		TRAIL("includes");

		LEAD("basic definitions");
		T("#define SUPERCLASS \"multimedia.class\"\n\n");
		T("struct LibBase\n{\n");
		II; I; T("struct Library           LibNode;\n");
		I; T("struct SignalSemaphore   BaseLock;\n");
		if (msg->ProjectType == PROJECT_TYPE_REGGAE) { I; T("Class                   *MyClass;\n"); }
		I; T("APTR                     SegList;\n");
		I; T("BOOL                     InitFlag;\n");
		if (msg->ProjectType == PROJECT_TYPE_REGGAE) { I; T("const struct TagItem    *Attributes;\n"); }
		IO; T("};\n\n\n");
		T("#define UNUSED __attribute__((unused))\n#define OBJNAME(obj) xget(obj, MMA_ObjectName)\n\n\n");
		generate_library_bases_definitions(obj);
		TRAIL("basic_definitions");

		LEAD("prototypes of basic library functions");
		T("struct Library *LibInit(struct Library *unused, APTR seglist, struct Library *sysb);\n");
		T("struct Library *LibOpen(void);\n");
		T("ULONG LibClose(void);\n");
		T("APTR LibExpunge(void);\n");
		T("ULONG LibReserved(void);\n");
		T("Class *GetClass(void);\n");
		T("APTR LibExpunge2(struct LibBase *lb);\n");
		TRAIL("prototypes of basic library functions");
		
		LEAD("resident and jumptable");
		T("extern const UBYTE VTag[];\n");
		TLN("char LibName[] = \"%s\";\n\n\n");
        T("struct Resident ROMTag =\n{\n");
		II; I; T("RTC_MATCHWORD,\n"); I; T("&ROMTag,\n"); I; T("&ROMTag + 1,\n");
		I; T("RTF_EXTENDED | RTF_PPC,\n"); I; T("VERSION,\n"); I; T("NT_LIBRARY,\n");
		I; T("0,\n"); I; T("LibName,\n"); I; T("VSTRING,\n");
		I; T("(APTR)LibInit,\n"); I; T("REVISION,\n"); I; T("NULL\n");
		IO; T("};\n\n\n");
		T("APTR JumpTable[] =\n{\n");
		II; I; T("(APTR)FUNCARRAY_BEGIN,\n"); I; T("(APTR)FUNCARRAY_32BIT_NATIVE,\n");
		I; T("(APTR)LibOpen,\n"); I; T("(APTR)LibClose,\n"); I; T("(APTR)LibExpunge,\n");
		I; T("(APTR)LibReserved,\n"); I; T("(APTR)GetClass,\n");
		I; T("(APTR)0xFFFFFFFF,\n"); I; T("(APTR)FUNCARRAY_END\n"); IO; T("};\n");
		TRAIL("resident and jumptable");

		if (msg->ProjectType == PROJECT_TYPE_REGGAE) generate_init_reggae_class(obj);
		generate_init_resources(obj, msg->ProjectType);
		generate_free_resources(obj, msg->ProjectType);

		LEAD("LibInit()");
		T("static struct LibBase *LibInit2(struct LibBase *lb, APTR seglist, UNUSED struct Library *SysBase)\n{\n");
		II; I; T("lb->SegList = seglist;\n");
		if (msg->ProjectType == PROJECT_TYPE_REGGAE) { I; T("lb->Attributes = 0;\n"); }
		I; T("InitSemaphore(&lb->BaseLock);\n\n");
		I; T("return lb;\n");
		IO; T("}\n\n\nstruct TagItem LibTags[] = {\n");
		II;
		I; T("{ LIBTAG_FUNCTIONINIT, (IPTR)JumpTable },\n");
		I; T("{ LIBTAG_LIBRARYINIT,  (IPTR)LibInit2 },\n");
		I; T("{ LIBTAG_MACHINE,      MACHINE_PPC },\n");
		I; T("{ LIBTAG_BASESIZE,     sizeof(struct LibBase) },\n");
		I; T("{ LIBTAG_SEGLIST,      0 },\n");
		I; T("{ LIBTAG_TYPE,         NT_LIBRARY },\n");
		I; T("{ LIBTAG_NAME,         0 },\n");
		I; T("{ LIBTAG_IDSTRING,     0 },\n");
		I; T("{ LIBTAG_FLAGS,        LIBF_CHANGED | LIBF_SUMUSED },\n");
		I; T("{ LIBTAG_VERSION,      VERSION },\n");
		I; T("{ LIBTAG_REVISION,     REVISION },\n");
		I; T("{ LIBTAG_PUBLIC,       TRUE },\n");
		I; T("{ TAG_END,             0 }\n");
		IO; T("};\n\n\n");
		T("struct Library *LibInit(UNUSED struct Library *unused, APTR seglist, struct Library *sysbase)\n{\n");
		II; I; T("SysBase = sysbase;\n");
		I; T("LibTags[4].ti_Data = (IPTR)seglist;\n");
		I; T("LibTags[6].ti_Data = (IPTR)ROMTag.rt_Name;\n");
		I; T("LibTags[7].ti_Data = (IPTR)ROMTag.rt_IdString;\n\n");
		I; T("return NewCreateLibrary(LibTags);\n");
		IO; T("}\n");
		TRAIL("LibInit()");

		LEAD("LibOpen()");
		T("struct Library *LibOpen(void)\n{\n");
		II; I; T("struct LibBase *lb = (struct LibBase*)REG_A6;\n");
		I; T("struct Library *lib = (struct Library*)lb;\n\n");
		I; T("ObtainSemaphore(&lb->BaseLock);\n\n");
		I; T("if (!lb->InitFlag)\n");
		I; T("{\n");
		II; I; T("if (InitResources(lb)) lb->InitFlag = TRUE;\n");
		I; T("else\n");
		I; T("{\n");
		II; I; T("lib = NULL;\n");
		I; T("FreeResources();\n");
		IO; I; T("}\n");
		IO; I; T("}\n\n");
		I; T("if (lib)\n");
		I; T("{\n");
		II; I; T("lb->LibNode.lib_Flags &= ~LIBF_DELEXP;\n");
		I; T("lb->LibNode.lib_OpenCnt++;\n");
		IO; I; T("}\n\n");
		I; T("ReleaseSemaphore(&lb->BaseLock);\n");
		I; T("return lib;\n");
		IO; I; T("}\n");
        TRAIL("LibOpen()");

		LEAD("LibClose()");
		T("ULONG LibClose(void)\n{\n");
		II; I; T("struct LibBase *lb = (struct LibBase*)REG_A6;\n");
		I; T("ULONG ret = 0;\n\n");
		I; T("ObtainSemaphore(&lb->BaseLock);\n\n");
		I; T("if (--lb->LibNode.lib_OpenCnt == 0)\n");
		I; T("{\n");
		II; I; T("if (lb->LibNode.lib_Flags & LIBF_DELEXP) ret = (ULONG)LibExpunge2(lb);\n");
		IO; I; T("}\n\n");
		I; T("ReleaseSemaphore(&lb->BaseLock);\n");
		I; T("return ret;\n");
		IO; T("}\n");
		TRAIL("LibClose()");

		LEAD("LibExpunge()");
		T("APTR LibExpunge(void)\n{\n");
		II; I; T("struct LibBase *lb = (struct LibBase*)REG_A6;\n\n");
		I; T("return LibExpunge2(lb);\n");
		IO; T("}\n\n\n");
		T("APTR LibExpunge2(struct LibBase *lb)\n{\n");
		II; I; T("APTR seglist = NULL;\n");
		I; T("LONG base_size = 0;\n\n");
		I; T("ObtainSemaphore(&lb->BaseLock);\n\n");
		I; T("if (lb->LibNode.lib_OpenCnt == 0)\n");
		I; T("{\n");
		II;

		if (msg->ProjectType == PROJECT_TYPE_REGGAE)
		{
			I; T("if (!lb->MyClass || FreeClass(lb->MyClass))\n");
			I; T("{\n");
			II; I; T("lb->MyClass = NULL;\n");
		}

		I; T("Forbid();\n");
		I; T("Remove((struct Node*)lb);\n");
		I; T("Permit();\n");
		I; T("FreeResources();\n");
		I; T("seglist = lb->SegList;\n");
		I; T("base_size = lb->LibNode.lib_NegSize + lb->LibNode.lib_PosSize;\n");
		I; T("FreeMem((UBYTE*)lb - lb->LibNode.lib_NegSize, base_size);\n");
		I; T("lb = NULL;\n");
		IO; I; T("}\n");

		if (msg->ProjectType == PROJECT_TYPE_REGGAE)
		{
			T("\n");
			I; T("if (lb && lb->MyClass) AddClass(lb->MyClass);\n");
			IO; I; T("}\n");
		}

		I; T("else lb->LibNode.lib_Flags |= LIBF_DELEXP;\n\n");
		I; T("if (lb) ReleaseSemaphore(&lb->BaseLock);\n");
		I; T("return seglist;\n");
		IO; T("}\n");
		TRAIL("LibExpunge()");

		LEAD("LibReserved()");
		T("ULONG LibReserved(void)\n{\n");
		II; I; T("return 0;\n");
		IO; T("}\n");
        TRAIL("LibReserved()");

		if (msg->ProjectType == PROJECT_TYPE_REGGAE)
		{
			LEAD("GetClass()");
			T("Class *GetClass(void)\n{\n");
			II; I; T("struct LibBase *lb = (struct LibBase*)REG_A6;\n\n");
			I; T("return lb->MyClass;\n");
			IO; T("}\n");
			TRAIL("GetClass()");
		}
	}

	DoMethod(obj, GENM_Cleanup);
	return 0;
}

///
/// GeneratorLeadingComment()

IPTR GeneratorLeadingComment(Class *cl, Object *obj, struct GENP_LeadingComment *msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);

	FPrintf(d->FileHandle, (STRPTR)"/// %s\n\n", (IPTR)msg->VarText);
	return 0;
}

///
/// GeneratorTrailingComment()

IPTR GeneratorTrailingComment(Class *cl, Object *obj, UNUSED struct GENP_TrailingComment *msg)
{
	struct GeneratorData *d = INST_DATA(cl, obj);

	FPrintf(d->FileHandle, (STRPTR)"\n\n///\n");
	return 0;
}

///

/// d_Generator()

#ifdef __AROS__
BOOPSI_DISPATCHER(IPTR, g_Generator, cl, obj, msg)
{
#else
IPTR d_Generator(void)
{
	Class *cl = (Class*)REG_A0;
	Object *obj = (Object*)REG_A2;
	Msg msg = (Msg)REG_A1;
#endif

	switch (msg->MethodID)
	{
		case OM_NEW:                  return GeneratorNew(cl, obj, (struct opSet*)msg);
		case OM_GET:                  return GeneratorGet(cl, obj, (struct opGet*)msg);
		case GENM_Setup:              return GeneratorSetup(cl, obj, (struct GENP_Setup*)msg);
		case GENM_Cleanup:            return GeneratorCleanup(cl, obj, msg);
		case GENM_InsertRemark:       return GeneratorInsertRemark(cl, obj, (struct GENP_InsertRemark*)msg);
		case GENM_IndentIn:           return GeneratorIndentIn(cl, obj, msg);
		case GENM_IndentOut:          return GeneratorIndentOut(cl, obj, msg);
		case GENM_DoIndent:           return GeneratorDoIndent(cl, obj, msg);
		case GENM_Signature:          return GeneratorSignature(cl, obj, msg);
		case GENM_Text:               return GeneratorText(cl, obj, (struct GENP_Text*)msg);
		case GENM_TextParam:          return GeneratorTextParam(cl, obj, (struct GENP_TextParam*)msg);
		case GENM_Save:               return GeneratorSave(cl, obj, (struct GENP_Save*)msg);
		case GENM_Load:               return GeneratorLoad(cl, obj, (struct GENP_Load*)msg);
		case GENM_SaveAction:         return GeneratorSaveAction(cl, obj, msg);
		case GENM_LoadAction:         return GeneratorLoadAction(cl, obj, msg);
		case GENM_Generate:           return GeneratorGenerate(cl, obj, msg);
		case GENM_MethodHeader:       return GeneratorMethodHeader(cl, obj, (struct GENP_MethodHeader*)msg);
		case GENM_MethodFooter:       return GeneratorMethodFooter(cl, obj, (struct GENP_MethodFooter*)msg);
		case GENM_Verify:             return GeneratorVerify(cl, obj);
		case GENM_GenerateAction:     return GeneratorGenerateAction(cl, obj);
		case GENM_MakefileSignature:  return GeneratorMakefileSignature(cl, obj);
		case GENM_CreateLibGroup:     return GeneratorCreateLibGroup(cl, obj);
		case GENM_DisableLibGroup:    return GeneratorDisableLibGroup(cl, obj, (struct GENP_DisableLibGroup*)msg);
		case GENM_LibVersionH:        return GeneratorLibVersionH(cl, obj, (struct GENP_LibVersionH*)msg);
		case GENM_LibraryC:           return GeneratorLibraryC(cl, obj, (struct GENP_LibraryC*)msg);
		case GENM_LeadingComment:     return GeneratorLeadingComment(cl, obj, (struct GENP_LeadingComment*)msg);
		case GENM_TrailingComment:    return GeneratorTrailingComment(cl, obj, (struct GENP_TrailingComment*)msg);
		default:                      return DoSuperMethodA(cl, obj, msg);
	}
}
#ifdef __AROS__
BOOPSI_DISPATCHER_END
#endif

///

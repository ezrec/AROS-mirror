/* MuiGenerator class body */

/* Code generated with ChocolateCastle 0.1 */
/* written by Grzegorz "Krashan" Kraszewski <krashan@teleinfo.pb.bialystok.pl> */

/// includes

#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <clib/alib_protos.h>
#include <libraries/asl.h>
#include <string.h>

#include "muigenerator.h"
#include "generator.h"
#include "support.h"
#include "methodlist.h"
#include "methodeditor.h"
#include "locale.h"

///
/// instance data

struct MuiGeneratorData
{
	Object *SuperclassTypeRadio;
	Object *SuperclassNameString;
	Object *SuperclassPointerString;
	Object *SuperclassPageGroup;
	Object *MethodList;
	Object *MethodDelete;
	Object *MethodEdit;
	Object *MethodAdd;
};


///
/// tables

const char* PopularMuiSuperclasses[] =
{
	"Application",
	"Area",
	"Dataspace",
	"Family",
	"Gauge",	
	"Group",
	"List",
	"Notify",
	"Numeric",
	"Numericbutton",
	"Poplist",
	"Popobject",
	"Popstring",
	"Process",
	"Prop",
	"Register",
	"Scrollgroup",
	"Semaphore",
	"Slider",
	"String",
	"Text",
	"Virtgroup",
	"Window",
	NULL
};

const struct MethodEntry DefaultMethods[] = {
	{ (STRPTR)"OM_NEW", (STRPTR)"New", (STRPTR)"opSet", (STRPTR)"00000101", TRUE },
	{ (STRPTR)"OM_DISPOSE", (STRPTR)"Dispose", (STRPTR)"", (STRPTR)"00000102", TRUE },
	{ (STRPTR)"OM_SET", (STRPTR)"Set", (STRPTR)"opSet", (STRPTR)"00000103", TRUE },
	{ (STRPTR)"OM_GET", (STRPTR)"Get", (STRPTR)"opGet", (STRPTR)"00000104", TRUE },
	{ NULL, NULL, NULL, NULL, 0 }
};

#define SUPERCLASS_TYPE_PRIVATE  0
#define SUPERCLASS_TYPE_PUBLIC   1


///
/// global vars, prototypes

struct MUI_CustomClass *MuiGeneratorClass;

IPTR d_MuiGenerator(void);

static struct EmulLibEntry g_MuiGenerator = {TRAP_LIB, 0, (void(*)(void))d_MuiGenerator};


///
/// CreateMuiGeneratorClass()

struct MUI_CustomClass *CreateMuiGeneratorClass(void)
{
	struct MUI_CustomClass *cl;

	cl = MUI_CreateCustomClass(NULL, NULL, GeneratorClass,  sizeof(struct MuiGeneratorData), &g_MuiGenerator);

	MuiGeneratorClass = cl;
	return cl;
}


///
/// DeleteMuiGeneratorClass()

void DeleteMuiGeneratorClass(void)
{
	 MUI_DeleteCustomClass(MuiGeneratorClass);
}


///

/// create_my_group()

static Object *create_my_group(Object *obj, struct MuiGeneratorData *d)
{
	Object *o;

	o = MUI_NewObjectM(MUIC_Group,
		MUIA_Group_Child, MUI_NewObjectM(MUIC_Group,
			MUIA_Frame, MUIV_Frame_Group,
			MUIA_FrameTitle, LS(MSG_MUI_GENERATOR_SUPERCLASS_LABEL, "Superclass"),
			MUIA_Background, MUII_GroupBack,
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Group,
				MUIA_Group_Horiz, TRUE,
				MUIA_Group_Child, MUI_NewObjectM(MUIC_Rectangle,
				TAG_END),
				MUIA_Group_Child, d->SuperclassTypeRadio = MUI_NewObjectM(MUIC_Radio,
					MUIA_Radio_Entries, SuperclassRadioEntries,
					MUIA_CycleChain, TRUE,
				TAG_END),
				MUIA_Group_Child, RectangleObject,
				TAG_END),
			TAG_END),
			MUIA_Group_Child, d->SuperclassPageGroup = MUI_NewObjectM(MUIC_Group,
				MUIA_Group_PageMode, TRUE,
				MUIA_Group_Child, MUI_NewObjectM(MUIC_Group,
					MUIA_Group_Horiz, TRUE,
					MUIA_Group_Child, MUI_NewObjectM(MUIC_Text,
						MUIA_Text_Contents, LS(MSG_MUI_GENERATOR_SUPERCLASS_PTRNAME_LABEL, "Pointer Name:"),
						MUIA_Frame, MUIV_Frame_String,
						MUIA_FramePhantomHoriz, TRUE,
						MUIA_HorizWeight, 0,
					TAG_END),
					MUIA_Group_Child, d->SuperclassPointerString = MUI_NewObjectM(MUIC_String,
						MUIA_Frame, MUIV_Frame_String,
						MUIA_String_MaxLen, 50,
						MUIA_String_Accept, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_",
						MUIA_CycleChain, TRUE,
					TAG_END),
				TAG_END),
				MUIA_Group_Child, MUI_NewObjectM(MUIC_Group,
					MUIA_Group_Horiz, TRUE,
					MUIA_Group_Child, MUI_NewObjectM(MUIC_Text,
						MUIA_Text_Contents, LS(MSG_MUI_GENERATOR_SUPERCLASS_CNAME_LABEL, "Superclass Name:"),
						MUIA_Frame, MUIV_Frame_String,
						MUIA_FramePhantomHoriz, TRUE,
						MUIA_HorizWeight, 0,
					TAG_END),
					MUIA_Group_Child, d->SuperclassNameString = MUI_NewObjectM(MUIC_Poplist,
						MUIA_Popstring_String, MUI_NewObjectM(MUIC_String,
							MUIA_Frame, MUIV_Frame_String,
							MUIA_String_MaxLen, 50,
							MUIA_String_Accept, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_",
						TAG_END),
						MUIA_Popstring_Button, MUI_NewObjectM(MUIC_Image,
							MUIA_Image_Spec, "6:18",
							MUIA_InputMode, MUIV_InputMode_RelVerify,
							MUIA_Frame, MUIV_Frame_ImageButton,
							MUIA_Image_FreeVert, TRUE,
						TAG_END),
						MUIA_Poplist_Array, PopularMuiSuperclasses,
						MUIA_CycleChain, TRUE,
					TAG_END),
				TAG_END),
			TAG_END),
		TAG_END),
		MUIA_Group_Child, MUI_NewObjectM(MUIC_Group,
			MUIA_Frame, MUIV_Frame_Group,
			MUIA_FrameTitle, LS(MSG_MUI_GENERATOR_METHODS_FRAME_TITLE, "Methods"),
			MUIA_Background, MUII_GroupBack,
			MUIA_Group_Child, NewObjectM(MethodEditorClass->mcc_Class, NULL,
				EDLA_List, d->MethodList = NewObjectM(MethodListClass->mcc_Class, NULL,
				TAG_END),
				EDLA_Generator, obj,
			TAG_END),
		TAG_END),

		/*
		MUIA_Group_Child, MUI_NewObjectM(MUIC_Group,
			MUIA_Frame, MUIV_Frame_Group,
			MUIA_FrameTitle, "Options",
			MUIA_Background, MUII_GroupBack,
		TAG_END),
		*/
	TAG_END);

	return o;
}


///
/// load_super_data()

int load_super_data(Class *cl, Object *obj, struct GENP_Load *msg)
{
	UBYTE line[512];
	int result = FALSE;
	struct MuiGeneratorData *d = INST_DATA(cl, obj);

	if (FGets(msg->Handle, (STRPTR)line, 512))
	{
		msg->Parser->line++;

		if (check_pattern(line, (STRPTR)"SUPERTYPE/K/N/A,SUPERNAME/K/A", msg->Parser))
		{
			ULONG type = *(ULONG*)msg->Parser->params[0];

			if (type < 2) xset(d->SuperclassTypeRadio, MUIA_Radio_Active, type);

			switch (type)
			{
				case 0:
					xset(d->SuperclassPointerString, MUIA_String_Contents, msg->Parser->params[1]);
					result = TRUE;
				break;

				case 1:
					xset(d->SuperclassNameString, MUIA_String_Contents, msg->Parser->params[1]);
					result = TRUE;
				break;

				default: syntax_error(obj, msg->Parser);
			}
		}
		else syntax_error(obj, msg->Parser);
	}
	else unexpected_eof(obj, msg->Parser);

 return result;
}


///
/// load_method()

static BOOL load_method(Class *cl, Object *obj, struct GENP_Load *msg)
{
	UBYTE line[512];
	int result = FALSE;
	struct MuiGeneratorData *d = INST_DATA(cl, obj);

	if (FGets(msg->Handle, (STRPTR)line, 512))
	{
		msg->Parser->line++;

		if (check_pattern(line, (STRPTR)"METHOD/K/A,FUNCTION/K/A,STRUCTURE/K/A,ID/K/A,STD/K/N/A", msg->Parser))
		{
			struct MethodEntry me;

			me.Name = (STRPTR)msg->Parser->params[0];
			me.Function = (STRPTR)msg->Parser->params[1];
			me.Structure = (STRPTR)msg->Parser->params[2];
			me.Identifier = (STRPTR)msg->Parser->params[3];
			me.Standard = *(LONG*)msg->Parser->params[4];
			DoMethod(d->MethodList, MUIM_List_InsertSingle, (intptr_t)&me, MUIV_List_Insert_Bottom);
			result = TRUE;
		}
		else syntax_error(obj, msg->Parser);
	}
	return result;
}


///
/// generate_set_contents()

static void generate_set_contents(Object *obj)
{
	I; T("int tagcount = 0;\n");
	I; T("struct TagItem *tag, *tagptr = msg->ops_AttrList;\n\n");
	I; T("while (tag = NextTagItem(&tagptr))\n");
	I; T("{\n");
	II; TODO("attributes in OM_SET.");
	IO; I; T("}\n\n");
	I; T("tagcount += DoSuperMethodA(cl, obj, (Msg)msg);\n");
	I; T("return tagcount;\n");
}

///
/// generate_get_contents()

static void generate_get_contents(Object *obj)
{
	I; T("int rv = FALSE;\n\n");
	I; T("switch (msg->opg_AttrID)\n"); I; T("{\n"); II;
	TODO("attributes in OM_GET.");
	I; T("default: rv = (DoSuperMethodA(cl, obj, (Msg)msg));\n");
	IO; I; T("}\n\n");
	I; T("return rv;\n");
}

///

/// MuiGeneratorNew()

IPTR MuiGeneratorNew(Class *cl, Object *obj, struct opSet *msg)
{
	Object *newobj = NULL;

	if (obj = (Object*)DoSuperMethodA(cl, obj, (Msg)msg))
	{
		struct MuiGeneratorData *d = INST_DATA(cl, obj);
		Object *my_group;

		if (my_group = create_my_group(obj, d))
		{
			Object *parent;

			GetAttr(GENA_SubclassSpace, obj, (ULONG*)&parent);

			if (parent)
			{
				DoMethod(parent, OM_ADDMEMBER, (intptr_t)my_group);
				SetAttrs(obj, MUIA_Window_Title,
				 (IPTR)LS(MSG_MUI_GENERATOR_WINDOW_TITLE, "MUI Custom Class"), TAG_END);
				DoMethod(d->SuperclassTypeRadio, MUIM_Notify, MUIA_Radio_Active, MUIV_EveryTime,
					(intptr_t)d->SuperclassPageGroup, 3, MUIM_Set, MUIA_Group_ActivePage, MUIV_TriggerValue);
				DoMethod(d->MethodList, MTLM_InsertMethodTable, (IPTR)DefaultMethods);

				newobj = obj;
			}
			else MUI_DisposeObject(my_group);
		}

		if (!newobj) CoerceMethod(cl, obj, OM_DISPOSE);
	}
	return (IPTR)newobj;
}


///
/// MuiGeneratorDispose()

IPTR MuiGeneratorDispose(Class *cl, Object *obj, Msg msg)
{
	return DoSuperMethodA(cl, obj, msg);
}


///
/// MuiGeneratorGet()

IPTR MuiGeneratorGet(Class *cl, Object *obj, struct opGet *msg)
{
	switch (msg->opg_AttrID)
	{
		case GENA_ProjectType:
		 *(STRPTR*)msg->opg_Storage = (STRPTR)"MUI Class";
		return TRUE;
	}
	return DoSuperMethodA(cl, obj, (Msg)msg);
}


///
/// MuiGeneratorGenerate()

intptr_t MuiGeneratorGenerate(Class *cl, Object *obj, Msg msg)
{
	struct MuiGeneratorData *d = INST_DATA(cl, obj);
	int entry_number;

	msg = msg;

	/* === Class code. === */

	if (DoMethod(obj, GENM_Setup, (intptr_t)"%s.c"))
	{
		char *s;
		int32_t superclass_type;

		DoMethod(obj, GENM_Signature);
		TC("/* %sClass code. */\n\n");
		TCS("#include \"%s.h\"\n\n");
		TC("struct MUI_CustomClass *%sClass;\n\n");

		T("/// dispatcher prototype\n\n");
		TC("IPTR %sDispatcher(void);\n");
		TC("const struct EmulLibEntry %sGate = ");
		TC("{TRAP_LIB, 0, (void(*)(void))%sDispatcher};\n");
		T("\n\n///\n");

		TC("/// %sData\n\n");
		TC("struct %sData\n{\n"); II; I; T("// Insert object instance data here.\n"); IO; T("};\n\n\n");
		T("///\n");

		/*------------------------------------------------------------------------*/
		/* create class                                                           */
		/*------------------------------------------------------------------------*/

		TC("/// Create%sClass()\n\n");
		TC("struct MUI_CustomClass *Create%sClass(void)\n{\n");
		II; I; T("struct MUI_CustomClass *cl;\n\n");
		I; T("cl = MUI_CreateCustomClass(NULL, ");
		superclass_type = xget(d->SuperclassTypeRadio, MUIA_Radio_Active);
		
		if (superclass_type == SUPERCLASS_TYPE_PRIVATE)
		{
			s = (char*)xget(d->SuperclassPointerString, MUIA_String_Contents);
			T("NULL, "); T(s); T(", ");
		}
		else
		{
			s = (char*)xget(d->SuperclassNameString, MUIA_String_Contents);
			T("MUIC_"); T(s); T(", NULL, ");
		}

		TC("sizeof(struct %sData), ");
		TC("(APTR)&%sGate);\n");
		I; TC("%sClass = cl;\n"); I; T("return cl;\n"); IO; T("}\n\n\n");
		T("///\n");

		/*------------------------------------------------------------------------*/
		/* delete class                                                           */
		/*------------------------------------------------------------------------*/

		TC("/// Delete%sClass()\n\n");
		TC("void Delete%sClass(void)\n{\n");
		II; I; TC("if (%sClass) "); TC("MUI_DeleteCustomClass(%sClass);\n");
		IO; T("}\n\n\n///\n");

		/*------------------------------------------------------------------------*/
		/* methods                                                                */
		/*------------------------------------------------------------------------*/

		for (entry_number = 0;; entry_number++)
		{
			struct MethodEntry *me;
			BOOL return_zero = TRUE;

			DoMethod(d->MethodList, MUIM_List_GetEntry, entry_number, (intptr_t)&me);
			if (!me) break;
			DoMethod(obj, GENM_MethodHeader, (IPTR)me->Name, (IPTR)me->Function,
			 (IPTR)me->Structure, (IPTR)me->Identifier, TRUE, FALSE);

			if (strcmp((char*)me->Name, "OM_GET") == 0)
			{
				generate_get_contents(obj);
				return_zero = FALSE;
			}
			else if (strcmp((char*)me->Name, "OM_SET") == 0)
			{
				generate_set_contents(obj);
				return_zero = FALSE;
			}

			DoMethod(obj, GENM_MethodFooter, return_zero);
		}

		generate_dispatcher(obj, d->MethodList);
	}
	DoMethod(obj, GENM_Cleanup);

	/* === Class header. === */

	if (DoMethod(obj, GENM_Setup, (intptr_t)"%s.h"))
	{
		DoMethod(obj, GENM_Signature);
		TC("/* %sClass header. */\n\n");
		T("#include <proto/intuition.h>\n");
		T("#include <proto/utility.h>\n");
		T("#include <proto/muimaster.h>\n");
		T("#include <clib/alib_protos.h>\n\n");
		TC("extern struct MUI_CustomClass *%sClass;\n\n");
		TC("struct MUI_CustomClass *Create%sClass(void);\n");
		TC("void Delete%sClass(void);\n\n\n");

		/*----------------*/
		/* custom methods */
		/*----------------*/

		for (entry_number = 0;; entry_number++)
		{
			struct MethodEntry *me;

			DoMethod(d->MethodList, MUIM_List_GetEntry, entry_number, (intptr_t)&me);
			if (!me) break;

			if (!me->Standard)
			{
				T("#define "); T(me->Name); T(" 0x"); T(me->Identifier); T("\n");
			}
		}
	}
	DoMethod(obj, GENM_Cleanup);
	DoSuperMethodA(cl, obj, msg);   // displays info requester
	return 0;
}


///
/// MuiGeneratorSave()

IPTR MuiGeneratorSave(Class *cl, Object *obj, struct GENP_Save *msg)
{
	struct MuiGeneratorData *d = INST_DATA(cl, obj);

	if (DoSuperMethodA(cl, obj, msg))
	{
		ULONG supertype, e;

		supertype = xget(d->SuperclassTypeRadio, MUIA_Radio_Active);

		if (supertype == 0) FPrintf(msg->Handle, (STRPTR)"SUPERTYPE=0 SUPERNAME=%s\n", xget(d->SuperclassPointerString, MUIA_String_Contents));
		else FPrintf(msg->Handle, (STRPTR)"SUPERTYPE=1 SUPERNAME=%s\n", xget(d->SuperclassNameString, MUIA_String_Contents));

		// dumping methods

		for (e = 0; ; e++)
		{
			struct MethodEntry *me;
			STRPTR s;

			DoMethod(d->MethodList, MUIM_List_GetEntry, e, (intptr_t)&me);
			if (!me) break;
			if (*me->Structure == 0x00) s = (STRPTR)"\"\"";
			else s = me->Structure;
			FPrintf(msg->Handle, (STRPTR)"METHOD=%s FUNCTION=%s STRUCTURE=%s ID=%s STD=%ld\n", (intptr_t)me->Name,
			 (intptr_t)me->Function, (intptr_t)s, (intptr_t)me->Identifier, me->Standard);
		}
	}

	return TRUE;
}


///
/// MuiGeneratorLoad()

IPTR MuiGeneratorLoad(Class *cl, Object *obj, struct GENP_Load *msg)
{
	struct MuiGeneratorData *d = INST_DATA(cl, obj);
	BOOL rv = FALSE;

	if (DoSuperMethodA(cl, obj, (Msg)msg))
	{
		if (load_super_data(cl, obj, msg))
		{
			DoMethod(d->MethodList, MUIM_List_Clear);
			while (load_method(cl, obj, msg));
			rv = TRUE;
		}
	}

	return rv;
}


///
/// d_MuiGenerator()

IPTR d_MuiGenerator(void)
{
	Class *cl = (Class*)REG_A0;
	Object *obj = (Object*)REG_A2;
	Msg msg = (Msg)REG_A1;

	switch (msg->MethodID)
	{
		case OM_NEW:  return (MuiGeneratorNew(cl, obj, (struct opSet*)msg));
		case OM_DISPOSE:  return (MuiGeneratorDispose(cl, obj, msg));
		case OM_GET:  return (MuiGeneratorGet(cl, obj, (struct opGet*)msg));
		case GENM_Generate:  return (MuiGeneratorGenerate(cl, obj, msg));
		case GENM_Save:  return (MuiGeneratorSave(cl, obj, (struct GENP_Save*)msg));
		case GENM_Load:  return (MuiGeneratorLoad(cl, obj, (struct GENP_Load*)msg));
		default:  return (DoSuperMethodA(cl, obj, msg));
	}
}


///


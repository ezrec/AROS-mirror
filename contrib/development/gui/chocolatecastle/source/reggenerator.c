/* RegGenerator class body */

/* Code generated with ChocolateCastle 0.1 */
/* written by Grzegorz "Krashan" Kraszewski <krashan@teleinfo.pb.edu.pl> */

/// includes

#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <clib/alib_protos.h>
#include <libraries/asl.h>
#include <string.h>

#include "reggenerator.h"
#include "generator.h"
#include "support.h"
#include "methodeditor.h"
#include "methodlist.h"
#include "locale.h"

///
/// class data

struct RegGeneratorData
{
	Object *MethodList;
	Object *ExtClassCheck;
	Object *ClassTypeCycle;
};

struct MUI_CustomClass *RegGeneratorClass;

IPTR d_RegGenerator(void);

static struct EmulLibEntry g_RegGenerator = {TRAP_LIB, 0, (void(*)(void))d_RegGenerator};

#define OBJ_EXTCHECK     0x6EDA6F36   // external class checkmark
#define OBJ_LIBSETTINGS  0x6EDA6F37   // library settings button
#define OBJ_CLASSTYPE    0x6EDA6F38   // class type cycle

// table of Reggae class type constants

STRPTR ClassTypeConstants[] = {
	(STRPTR)"MMCLASS_STREAM",
	(STRPTR)"MMCLASS_DEMUXER",
	(STRPTR)"MMCLASS_DECODER",
	(STRPTR)"MMCLASS_FILTER",
	(STRPTR)"MMCLASS_ENCODER",
	(STRPTR)"MMCLASS_MULTIPLEXER",
	(STRPTR)"MMCLASS_OUTPUT",
	(STRPTR)"MMCLASS_BASIC"
};

///
/// CreateRegGeneratorClass()

struct MUI_CustomClass *CreateRegGeneratorClass(void)
{
	struct MUI_CustomClass *cl;

	cl = MUI_CreateCustomClass(NULL, NULL, GeneratorClass,
	 sizeof(struct RegGeneratorData), &g_RegGenerator);

	RegGeneratorClass = cl;
	return cl;
}


///
/// DeleteRegGeneratorClass()

void DeleteRegGeneratorClass(void)
{
	 MUI_DeleteCustomClass(RegGeneratorClass);
}


///

/// create_reggae_class_group()

static Object* create_reggae_class_group(void)
{
	return MUI_NewObjectM(MUIC_Group,
		MUIA_Group_Horiz, TRUE,
		MUIA_Group_Child, MUI_NewObjectM(MUIC_Text,
			MUIA_Text_SetMax, TRUE,
			MUIA_Text_Contents, LS(MSG_REGGAE_GENERATOR_CLASS_TYPE_LABEL, "Class Type:"),
			MUIA_Frame, MUIV_Frame_Button,
			MUIA_FramePhantomHoriz, TRUE,
		TAG_END),
		MUIA_Group_Child, MUI_NewObjectM(MUIC_Cycle,
			MUIA_Cycle_Entries, ReggaeClassTypeEntries,
			MUIA_UserData, OBJ_CLASSTYPE,
		TAG_END),
	TAG_END);
}


///
/// create_my_group()

static Object* create_my_group(Object *obj, struct RegGeneratorData *d)
{
	Object *o;

	o = MUI_NewObjectM(MUIC_Group,
		MUIA_Group_Child, MUI_NewObjectM(MUIC_Group,
			MUIA_Group_Horiz, TRUE,
			MUIA_ShortHelp, LS(MSG_REGGAE_GENERATOR_EXTERNAL_CLASS_HELP,
				"Generates public Reggae class as a shared library. Complete skeleton "
				"is generated, including class descriptor and makefile. When unchecked, "
				"statically linked private class is generated."),
            MUIA_Group_Child, labelled_checkmark(LS(MSG_REGGAE_GENERATOR_EXTERNAL_CLASS,
			 "Generate public class (shared library)."), OBJ_EXTCHECK),
			MUIA_Group_Child, MUI_NewObjectM(MUIC_Rectangle,
			TAG_END),
		TAG_END),
		MUIA_Group_Child, DoMethod(obj, GENM_CreateLibGroup),
		MUIA_Group_Child, create_reggae_class_group(),
		MUIA_Group_Child, MUI_NewObjectM(MUIC_Group,
			MUIA_Frame, MUIV_Frame_Group,
			MUIA_FrameTitle, LS(MSG_REGGAE_EDITOR_METHODS_FRAME_TITLE, "Methods"),
			MUIA_Background, MUII_GroupBack,
			MUIA_Group_Child, NewObjectM(MethodEditorClass->mcc_Class, NULL,
				EDLA_List, d->MethodList = NewObjectM(MethodListClass->mcc_Class, NULL,
				TAG_END),
				EDLA_Generator, obj,
			TAG_END),
		TAG_END),
	TAG_END);

	return o;
}


///
/// load_method()

static BOOL load_method(Class *cl, Object *obj, struct GENP_Load *msg)
{
	BOOL result = FALSE;
	struct RegGeneratorData *d = INST_DATA(cl, obj);

	if (FGets(msg->Handle, msg->LineBuf, INPUT_LINE_MAX_LEN))
	{
		msg->Parser->line++;

		if (check_pattern(msg->LineBuf, (STRPTR)"METHOD/K/A,FUNCTION/K/A,STRUCTURE/K/A,ID/K/A,STD/K/N/A", msg->Parser))
		{
			struct MethodEntry me;

			me.Name = (STRPTR)msg->Parser->params[0];
			me.Function = (STRPTR)msg->Parser->params[1];
			me.Structure = (STRPTR)msg->Parser->params[2];
			me.Identifier = (STRPTR)msg->Parser->params[3];
			me.Standard = *(LONG*)msg->Parser->params[4];
			DoMethod(d->MethodList, MUIM_List_InsertSingle, (IPTR)&me, MUIV_List_Insert_Bottom);
			result = TRUE;
		}
		else syntax_error(obj, msg->Parser);
	}
	return result;
}


///
/// load_externity()

static BOOL load_externity(Class *cl, Object *obj, struct GENP_Load *msg)
{
	BOOL result = FALSE;
	struct RegGeneratorData *d = INST_DATA(cl, obj);

	if (FGets(msg->Handle, msg->LineBuf, INPUT_LINE_MAX_LEN))
	{
		msg->Parser->line++;

		if (check_pattern(msg->LineBuf, (STRPTR)"EXTERNAL/K/N/A", msg->Parser))
		{
			xset(d->ExtClassCheck, MUIA_Selected, *((LONG*)msg->Parser->params[0]));
			result = TRUE;
		}
		else syntax_error(obj, msg->Parser);
	}
	return result;
}


///
/// load_lib_settings()

static BOOL load_lib_settings(UNUSED Class *cl, Object *obj, struct GENP_Load *msg)
{
	BOOL result = FALSE;

	if (FGets(msg->Handle, msg->LineBuf, INPUT_LINE_MAX_LEN))
	{
		msg->Parser->line++;

		if (check_pattern(msg->LineBuf, (STRPTR)"LIBNAME/K/A,VERSION/K/N/A,REVISION/K/N/A,COPYRIGHT/K/A", msg->Parser))
		{
			xset(findobj(obj, OBJ_LIBG_NAME), MUIA_String_Contents, (STRPTR)msg->Parser->params[0]);
			xset(findobj(obj, OBJ_LIBG_VERSION), MUIA_String_Integer, *((LONG*)msg->Parser->params[1]));
			xset(findobj(obj, OBJ_LIBG_REVISION), MUIA_String_Integer, *((LONG*)msg->Parser->params[2]));
			xset(findobj(obj, OBJ_LIBG_COPYRIGHT), MUIA_String_Contents, (STRPTR)msg->Parser->params[3]);
			result = TRUE;
		}
		else syntax_error(obj, msg->Parser);
	}
	return result;
}


///
/// generate_new_contents()

static void generate_new_contents(Object *obj)
{
	I; T("Object *newobj = NULL;\n\n");
	I; T("if ((obj = (Object*)DoSuperMethodA(cl, obj, (Msg)msg)))\n");
	I; T("{\n"); II;
	I; TC("struct %sData *d = "); TC("(struct %sData*)INST_DATA(cl, obj);\n\n");
	TODO("Add object initialization. Set 'newobj' only if succesfull.");
	I; T("\n");
	I; T("newobj = obj;\n"); IO;
	I; T("}\n\n");
	I; T("if (!newobj) CoerceMethod(cl, obj, OM_DISPOSE);\n");
	I; T("return (IPTR)newobj;\n");
}


///
/// generate_dispose_contents()

static void generate_dispose_contents(Object *obj)
{
	I; T("\n");
	I; T("DoMethod(obj, MMM_LockObject);\n\n");
	TODO("Free resources allocated in OM_NEW() and MMM_Setup().");
	I; T("\n");
	I; T("DoMethod(obj, MMM_UnlockObject);\n");
	I; T("return DoSuperMethodA(cl, obj, msg);\n");
}


///
/// generate_set_contents()

static void generate_set_contents(Object *obj)
{
	I; T("int tagcount = 0;\n");
	I; T("struct TagItem *tag, *tagptr = msg->ops_AttrList;\n\n");
	I; T("DoMethod(obj, MMM_LockObject);\n\n");
	I; T("while ((tag = NextTagItem(&tagptr)))\n");
	I; T("{\n");
	II; TODO("attributes in OM_SET.");
	IO; I; T("}\n\n");
	I; T("tagcount += DoSuperMethodA(cl, obj, (Msg)msg);\n");
	I; T("DoMethod(obj, MMM_UnlockObject);\n");
	I; T("return tagcount;\n");
}

///
/// generate_get_contents()

void generate_get_contents(Object *obj)
{
	I; T("int rv = FALSE;\n\n");
	I; T("DoMethod(obj, MMM_LockObject);\n\n");
	I; T("switch (msg->opg_AttrID)\n"); I; T("{\n"); II;
	TODO("attributes in OM_GET.");
	T("\n");
	I; T("default: rv = (DoSuperMethodA(cl, obj, (Msg)msg));\n");
	IO; I; T("}\n\n");
	I; T("DoMethod(obj, MMM_UnlockObject);\n");
	I; T("return rv;\n");
}

///
/// generate_pull_contents()

static void generate_pull_contents(Object *obj)
{
	I; T("LONG bytes;\n\n");
	I; T("bytes = DoMethod(obj, MMM_PrePull, msg->Port, (ULONG)msg->Buffer, msg->Length);\n\n");
	I; T("if (bytes == MMM_PREPULL_NOT_DONE)\n");
	I; T("{\n");
	II; TODO("Implement pulling on output port here.");
	IO; I; T("}\n\n");
	I; T("DoMethod(obj, MMM_PostPull, msg->Port, (ULONG)msg->Buffer, bytes);\n\n");
	I; T("return bytes;\n");
}


///
/// generate_seek_contents()

static void generate_seek_contents(Object *obj)
{
	I; T("LONG preseek;\n\n");
	I; T("preseek = DoMethod(obj, MMM_PreSeek, msg->Port, msg->Type, (ULONG)msg->Position, 0 /* insert seek mask */);\n\n");
	I; T("if (preseek == MMM_PRESEEK_NOT_DONE)\n");
	I; T("{\n"); II;
	TODO("Implement seeking on output port here."); IO;
	I; T("}\n\n");
	I; T("return DoMethod(obj, MMM_PostSeek, msg->Port, msg->Type, (ULONG)msg->Position, preseek);\n");
}


///
/// generate_makefile()

static void generate_makefile(Object *obj)
{
	if (DoMethod(obj, GENM_Setup, (IPTR)"makefile"))
	{
		DoMethod(obj, GENM_MakefileSignature);
		T("CC = ppc-morphos-gcc\n");
		T("LD = ppc-morphos-gcc\n");
		T("CFLAGS = -noixemul -O2 -mno-prototype -mcpu=604 -W -Wall\n");
		T("LDFLAGS = -nostartfiles -nostdlib -noixemul\n");
		T("LIBS = -labox\n\n");
		TLN("all: %s\n\n");
		TLN("debug: %s.db\n\n");
		TLN("clean:\n\trm -f %s "); TLN("%s.db *.o\n\n");
		TLN("%s: "); TCS("dummy.o library.o %s.o data\n");
		TCS("\t$(LD) -s $(LDFLAGS) dummy.o library.o %s.o $(LIBS) -o "); TLN("%s\n");
		TLN("\tppc-morphos-objcopy --add-section .dtcode=data %s\n");
		TLN("\tppc-morphos-objcopy --set-section-flags .dtcode=readonly %s\n");
		TLN("\tppc-morphos-strip --strip-unneeded --remove-section .comment %s\n\n");
		TLN("%s.db: "); TCS("dummy.o library.o %s.o data\n");
		TCS("\t$(LD) -g $(LDFLAGS) dummy.o library.o %s.o $(LIBS) -o "); TLN("%s.db\n");
		TLN("\tppc-morphos-objcopy --add-section .dtcode=data %s.db\n");
		TLN("\tppc-morphos-objcopy --set-section-flags .dtcode=readonly %s.db\n\n");
		TCS("%s.o: "); TCS("%s.c "); TCS("%s.h\n");
		T("\t$(CC) $(CFLAGS) -c -o $@ $<\n\n");
		T("dummy.o: dummy.c\n");
		T("\t$(CC) $(CFLAGS) -c -o $@ $<\n\n");
		T("library.o: library.c "); TCS("%s.h\n");
		T("\t$(CC) $(CFLAGS) -c -o $@ $<\n\n");
		T("data: data.c\n\t$(CC) $(CFLAGS) -nostartfiles -nostdlib -o data data.c\n");
		T("\t ppc-morphos-strip --strip-unneeded --remove-section .comment data\n");
	}

	DoMethod(obj, GENM_Cleanup);
}


///
/// table of methods having contents

struct FilledMethod
{
	STRPTR Name;
	void(*Function)(Object*);
};

const struct FilledMethod FilledMethods[] = {
	{ (STRPTR)"OM_NEW", generate_new_contents },
	{ (STRPTR)"OM_DISPOSE", generate_dispose_contents },
	{ (STRPTR)"OM_SET", generate_set_contents },
	{ (STRPTR)"OM_GET", generate_get_contents },
	{ (STRPTR)"MMM_Pull", generate_pull_contents },
	{ (STRPTR)"MMM_Seek", generate_seek_contents },
	{ NULL, NULL }
};


///
/// generate_method_contents()

BOOL generate_method_contents(STRPTR name, Object *obj)
{
	const struct FilledMethod *fm = FilledMethods;

	while (fm->Name)
	{
		if (strcmp((char*)name, (char*)fm->Name) == 0)
		{
			(*fm->Function)(obj);
			return FALSE;
		}

		fm++;
	}

	T("\n");
	I; T("DoMethod(obj, MMM_LockObject);\n\n");
	I; T("DoMethod(obj, MMM_UnlockObject);\n");
	return TRUE;
}


///
/// generate_data_section()

static void generate_data_section(Object *obj, struct RegGeneratorData *d)
{
	if (DoMethod(obj, GENM_Setup, (IPTR)"data.c"))
	{
		DoMethod(obj, GENM_Signature);

		T("#include <utility/tagitem.h>\n");
		T("#include <classes/multimedia/multimedia.h>\n\n");
		T("const struct TagItem ClassTags[] = {\n");
		II;
		I; T("{ MMA_ClassType        , "); T(ClassTypeConstants[xget(d->ClassTypeCycle, MUIA_Cycle_Active)]); T(" },\n");
		I; T("{ TAG_END              , 0 }\n");
		IO;
		T("};\n\n");
		T("const struct TagItem* ClassAtributes(void)\n");
		T("{\n");
		II;
		I; T("return ClassTags;\n");
		IO;
		T("}\n\n");
	}

	DoMethod(obj, GENM_Cleanup);
}


///

/// RegGeneratorNew()

IPTR RegGeneratorNew(Class *cl, Object *obj, struct opSet *msg)
{
	Object *newobj = NULL;

	if (obj = (Object*)DoSuperMethodA(cl, obj, (Msg)msg))
	{
		struct RegGeneratorData *d = INST_DATA(cl, obj);
		Object *my_group;

		if (my_group = create_my_group(obj, d))
		{
			Object *parent;

			GetAttr(GENA_SubclassSpace, obj, (ULONG*)&parent);

			if (parent)
			{
				DoMethod(parent, OM_ADDMEMBER, (IPTR)my_group);
				SetAttrs(obj, MUIA_Window_Title, (IPTR)LS(MSG_REGGAE_GENERATOR_WINDOW_TITLE,
				 "Reggae Custom Class"), TAG_END);
				DoMethod(d->MethodList, MTLM_InsertMethodTable, (IPTR)ReggaeDefMethods);
				d->ExtClassCheck = findobj(my_group, OBJ_EXTCHECK);
				d->ClassTypeCycle = findobj(my_group, OBJ_CLASSTYPE);
				DoMethod(d->ExtClassCheck, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
					(IPTR)obj, 2, GENM_DisableLibGroup, MUIV_NotTriggerValue);
				newobj = obj;
			}
			else MUI_DisposeObject(my_group);
		}

		if (!newobj) CoerceMethod(cl, obj, OM_DISPOSE);
	}
	return (IPTR)newobj;
}


///
/// RegGeneratorDispose()

IPTR RegGeneratorDispose(Class *cl, Object *obj, Msg msg)
{
//	struct RegGeneratorData *d = INST_DATA(cl, obj);

	return DoSuperMethodA(cl, obj, msg);
}


///
/// RegGeneratorGet()

IPTR RegGeneratorGet(Class *cl, Object *obj, struct opGet *msg)
{
	switch (msg->opg_AttrID)
	{
		case GENA_ProjectType:
		 *(STRPTR*)msg->opg_Storage = (STRPTR)"Reggae Class";
		return TRUE;
	}
	return DoSuperMethodA(cl, obj, (Msg)msg);
}


///
/// RegGeneratorGenerate()

IPTR RegGeneratorGenerate(Class *cl, Object *obj, UNUSED Msg msg)
{
	struct RegGeneratorData *d = INST_DATA(cl, obj);
	BOOL external;

	external = xget(d->ExtClassCheck, MUIA_Selected);

	if (external)
	{
		generate_makefile(obj);
		generate_data_section(obj, d);
		DoMethod(obj, GENM_LibVersionH, (IPTR)"lib_version.h");
		DoMethod(obj, GENM_LibraryC, PROJECT_TYPE_REGGAE);
	}

	/* === Class code. === */

	if (DoMethod(obj, GENM_Setup, (intptr_t)"%s.c"))
	{
		LONG entry_number;

		DoMethod(obj, GENM_Signature);

		TC("/* %s code. */\n\n");

		LEAD("includes");
		T("#define __NOLIBBASE__\n\n");
		T("#include <proto/intuition.h>\n");
		T("#include <proto/utility.h>\n");
		T("#include <proto/multimedia.h>\n\n");
		TCS("#include \"%s.h\"\n");
		TRAIL("includes");

		T("\nextern struct Library *IntuitionBase, *UtilityBase, *MultimediaBase;\n\n");
		TC("Class *%sClass;\n\n");

		LEAD("dispatcher prototype");
		TC("IPTR %sDispatcher(void);\n");
		TC("const struct EmulLibEntry %sGate = ");
		TC("{TRAP_LIB, 0, (void(*)(void))%sDispatcher};\n");
		TRAIL("dispatcher prototype");

		LEAD("instance data");
		TC("struct %sData\n{\n"); II; I; T("// Insert object instance data here.\n"); IO; T("};\n");
		TRAIL("instance data");

		/*------------------------------------------------------------------------*/
		/* create class                                                           */
		/*------------------------------------------------------------------------*/

		TC("/// Create%sClass()\n\n");
		TC("Class *Create%sClass(void)\n{\n");
		II; I; T("Class *cl;\n\n");
		I; TC("if ((cl = MakeClass(NULL, (STRPTR)\"multimedia.class\", NULL, sizeof(struct %sData), 0)))\n");
		I; T("{\n");
		II; I; TC("cl->cl_Dispatcher.h_Entry = (HOOKFUNC)&%sGate;\n");
		I; T("cl->cl_UserData = NULL;\n");
		IO; I; T("}\n\n");
		I; TC("%sClass = cl;\n"); I; T("return cl;\n"); IO; T("}\n");
		T("\n\n///\n");

		/*------------------------------------------------------------------------*/
		/* delete class                                                           */
		/*------------------------------------------------------------------------*/

		TC("/// Delete%sClass()\n\n");
		TC("void Delete%sClass(void)\n{\n");
		II; I; TC("if (%sClass) "); TC("FreeClass(%sClass);\n");
		IO; T("}\n\n\n///\n\n");

		/*------------------------------------------------------------------------*/
		/* methods                                                                */
		/*------------------------------------------------------------------------*/

		for (entry_number = 0;; entry_number++)
		{
			struct MethodEntry *me;
			BOOL return_zero, instance_data = TRUE, external;

			DoMethod(d->MethodList, MUIM_List_GetEntry, entry_number, (intptr_t)&me);
			if (!me) break;

			if (strcmp((char*)me->Name, "OM_NEW") == 0) instance_data = FALSE;
			external = xget(d->ExtClassCheck, MUIA_Selected);
			DoMethod(obj, GENM_MethodHeader, (IPTR)me->Name, (IPTR)me->Function,
			 (IPTR)me->Structure, (IPTR)me->Identifier, instance_data, external);
			return_zero = generate_method_contents(me->Name, obj);
			DoMethod(obj, GENM_MethodFooter, return_zero);
		}

		generate_dispatcher(obj, d->MethodList);
	}

	DoMethod(obj, GENM_Cleanup);

	/* === Class header. === */

	if (DoMethod(obj, GENM_Setup, (intptr_t)"%s.h"))
	{
		LONG entry_number;

		DoMethod(obj, GENM_Signature);

        TC("/* %sClass header. */\n\n");
		T("#include <intuition/classes.h>\n\n");
		TC("extern Class *%sClass;\n\n");
		TC("Class *Create%sClass(void);\n");
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
/// RegGeneratorSave()

IPTR RegGeneratorSave(Class *cl, Object *obj, struct GENP_Save *msg)
{
	struct RegGeneratorData *d = INST_DATA(cl, obj);
	LONG e;

	if (DoSuperMethodA(cl, obj, msg))
	{
		LONG external;

		external = xget(d->ExtClassCheck, MUIA_Selected);
		FPrintf(msg->Handle, (STRPTR)"EXTERNAL=%ld\n", external);

		if (external)
		{
			FPrintf(msg->Handle, (STRPTR)"LIBNAME=%s VERSION=%ld REVISION=%ld COPYRIGHT=\"%s\"\n",
			 xget(findobj(obj, OBJ_LIBG_NAME), MUIA_String_Contents),
			 xget(findobj(obj, OBJ_LIBG_VERSION), MUIA_String_Integer),
			 xget(findobj(obj, OBJ_LIBG_REVISION), MUIA_String_Integer),
			 xget(findobj(obj, OBJ_LIBG_COPYRIGHT), MUIA_String_Contents));
		}

		for (e = 0; ; e++)
		{
			struct MethodEntry *me;
			STRPTR s;

			DoMethod(d->MethodList, MUIM_List_GetEntry, e, (intptr_t)&me);
			if (!me) break;
			if (*me->Structure == 0x00) s = (STRPTR)"\"\"";
			else s = me->Structure;
			FPrintf(msg->Handle, (STRPTR)"METHOD=%s FUNCTION=%s STRUCTURE=%s ID=%s STD=%ld\n",
			 (IPTR)me->Name, (IPTR)me->Function, (IPTR)s, (IPTR)me->Identifier, me->Standard);
		}
	}

	return TRUE;
}


///
/// RegGeneratorLoad()

IPTR RegGeneratorLoad(Class *cl, Object *obj, struct GENP_Load *msg)
{
	struct RegGeneratorData *d = INST_DATA(cl, obj);
	BOOL rv = FALSE;

	if (DoSuperMethodA(cl, obj, (Msg)msg))
	{
		DoMethod(d->MethodList, MUIM_List_Clear);

		if (rv = load_externity(cl, obj, msg))
		{
			if ((xget(d->ExtClassCheck, MUIA_Selected) == FALSE) || (rv = load_lib_settings(cl, obj, msg)))
			{
				while (load_method(cl, obj, msg));
				rv = TRUE;
			}
		}
	}

	return rv;
}


///
/// d_RegGenerator()

IPTR d_RegGenerator(void)
{
	Class *cl = (Class*)REG_A0;
	Object *obj = (Object*)REG_A2;
	Msg msg = (Msg)REG_A1;

	switch (msg->MethodID)
	{
		case OM_NEW:  return (RegGeneratorNew(cl, obj, (struct opSet*)msg));
		case OM_DISPOSE:  return (RegGeneratorDispose(cl, obj, msg));
		case OM_GET:  return (RegGeneratorGet(cl, obj, (struct opGet*)msg));
		case GENM_Generate:  return (RegGeneratorGenerate(cl, obj, msg));
		case GENM_Save:  return (RegGeneratorSave(cl, obj, (struct GENP_Save*)msg));
		case GENM_Load:  return (RegGeneratorLoad(cl, obj, (struct GENP_Load*)msg));
		default:  return (DoSuperMethodA(cl, obj, msg));
	}
}


///


// DefIconsPrefs.c
// $Date$
// $Revision$


#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <dos/filehandler.h>
#include <libraries/iffparse.h>
#include <libraries/locale.h>
#include <libraries/mui.h>
#include <libraries/asl.h>
#include <utility/tagitem.h>
#include <datatypes/iconobject.h>

#define	__USE_SYSBASE

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/iconobject.h>
#include <proto/intuition.h>
#include <proto/wb.h>
#define	NO_INLINE_STDARG
#include <proto/muimaster.h>

#include <clib/alib_protos.h>

#include <mui/NListview_mcc.h>
#include <mui/NListtree_mcc.h>
#include <defs.h>

#define	ScalosFileTypes_NUMBERS
#include STR(SCALOSLOCALE)

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <DefIcons.h>

#include "debug.h"
#include "FileTypesPrefs.h"
#include "FileTypesPrefs_proto.h"
#include "IconobjectMCC.h"

//----------------------------------------------------------------------------

// local data structures

#define IOBUFFERLEN		480

#define	DEFICON_THEME_PREFIX	"THEME:DefIcons/def_"
#define	DEFICON_PATH_PREFIX	"ENV:Sys/def_"

#define	IS_CASE_SENSITIVE(magic)	((magic)->arg2 >= 0)

enum ProtectionMode
	{
	PROT_Set,
	PROT_Unset,
	PROT_Ignore,
	};

//----------------------------------------------------------------------------

// defined in FileTypesPrefs.c
extern STRPTR AddFileTypeActionStrings[];
extern STRPTR AddFileTypeActionStringsProject[];
extern STRPTR AddFileTypeActionStringsDisk[];

//----------------------------------------------------------------------------

static void FillFileTypesActions(struct FileTypesPrefsInst *inst,
	struct FileTypesEntry *fte, BOOL IsRootEntry);
static void TranslateMatchStringToPrintable(STRPTR Dest, size_t DestLength,
	CONST_STRPTR Src, size_t SrcLength);
static void GetProtectionString(STRPTR Buffer, size_t BuffSize,
	ULONG AndMask, ULONG CmpMask);
static STRPTR GetProtectionBitString(ULONG BitMask, ULONG AndMask, ULONG CmpMask,
	ULONG MsgIdSet, ULONG MsgIdCleared,
	STRPTR Buffer, size_t *BuffSize);
static void SetProtectionCycle(Object *o, ULONG BitMask,
	ULONG AndMask, ULONG CmpMask);
static void SetMagicString(struct FileTypesActionEntry *fae, CONST_STRPTR String, size_t Length);
static size_t TranslateFromPrintable(STRPTR Buffer, size_t BuffLength,
	CONST_STRPTR InputString);
static UBYTE HexValue(char ch);
static void SetProtectionBits(struct FileTypesActionEntry *fae,
	enum ProtectionMode Protect, ULONG Mask);
static struct FileTypesActionListEntry *AddFileTypesAction(struct FileTypesEntry *fte,
	const struct Magic *curr, struct Node *PrevNode);
static void CleanupFileTypesActions(struct FileTypesEntry *fte);
static ULONG GetActionFromString(CONST_STRPTR ActionString);
static void DefIconFullName(struct FileTypesPrefsInst *inst,
	STRPTR Path, size_t MaxLen, CONST_STRPTR FileTypeName);
static void InvokeWBInfo(struct FileTypesPrefsInst *inst,
	BPTR DirLock, STRPTR DefIconName);
static void SelectFileType(struct FileTypesPrefsInst *inst, const struct TypeNode *tn);
static struct MUI_NListtree_TreeNode *GetRootEntry(struct FileTypesPrefsInst *inst,
	struct MUI_NListtree_TreeNode *ln);

//----------------------------------------------------------------------------

static void FillFileTypesActions(struct FileTypesPrefsInst *inst,
	struct FileTypesEntry *fte, BOOL IsRootEntry)
{
	d1(KPrintF(__FILE__ "/%s/%ld: fte=%08lx  <%s>  IsRootEntry=%ld\n", __FUNC__, __LINE__, fte, fte->fte_TypeNode.tn_Name, IsRootEntry));

	if (fte)
		{
		struct FileTypesActionListEntry *ftale;

		// OBJNDX_String_FileTypes_Name is read-only for root entries
		set(inst->fpb_Objects[OBJNDX_String_FileTypes_Name],
			MUIA_Disabled, IsRootEntry);

		set(inst->fpb_Objects[OBJNDX_String_FileTypes_Name], MUIA_String_Contents,
			fte->fte_TypeNode.tn_Name);

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_Clear);
		set(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIA_NList_Quiet, MUIV_NList_Quiet_Full);


		for (ftale = (struct FileTypesActionListEntry *) fte->fte_MagicList.lh_Head;
			ftale != (struct FileTypesActionListEntry *) &fte->fte_MagicList.lh_Tail;
			ftale = (struct FileTypesActionListEntry *) ftale->ftale_Node.ln_Succ)
			{
			d1(KPrintF(__FILE__ "/%s/%ld: ftale=%08lx  action=%ld\n", \
				__FUNC__, __LINE__, ftale, ftale->ftale_Magic.action));

			DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
				MUIM_NList_InsertSingle,
				ftale,
				MUIV_NList_Insert_Bottom);
			}

		set(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIA_NList_Quiet, MUIV_NList_Quiet_None);
		}
}

//----------------------------------------------------------------------------

SAVEDS(APTR) INTERRUPT FileTypesActionConstructFunc(struct Hook *hook, APTR obj, struct NList_ConstructMessage *ltcm)
{
	struct FileTypesActionListEntry *ftale = ltcm->entry;
	struct FileTypesActionEntry *fae = (struct FileTypesActionEntry *) AllocPooled(ltcm->pool, sizeof(struct FileTypesActionEntry));

	d1(KPrintF(__FILE__ "/%s/%ld: obj=%08lx  ltcm=%08lx  memPool=%08lx  UserData=%08lx\n", \
		__FUNC__, __LINE__, obj, ltcm, ltcm->pool, ltcm->entry));
	d1(KPrintF(__FILE__ "/%s/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae)
		{
		fae->fae_ftale = ftale;
		}

	return fae;
}

SAVEDS(void) INTERRUPT FileTypesActionDestructFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct FileTypesActionEntry *fae = ltdm->entry;

	d1(KPrintF(__FILE__ "/%s/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));
	if (fae)
		{
		FreePooled(ltdm->pool, fae, sizeof(struct FileTypesActionEntry));
		ltdm->entry = NULL;
		}
}

SAVEDS(ULONG) INTERRUPT FileTypesActionDisplayFunc(struct Hook *hook, APTR obj, struct NList_DisplayMessage *ltdm)
{
	struct FileTypesActionEntry *fae = ltdm->entry;

	d1(KPrintF(__FILE__ "/%s/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae)
		{
		char TempString[80];

		d1(KPrintF(__FILE__ "/%s/%ld: action=%d\n", __FUNC__, __LINE__, fae->fae_ftale->ftale_Magic.action));

		ltdm->preparses[0] = "";

		strcpy(fae->fae_ActionAttrs, "");

		switch (fae->fae_ftale->ftale_Magic.action)
			{
		case ACT_MATCH:
			ltdm->strings[0] = AddFileTypeActionStrings[0];
			TranslateMatchStringToPrintable(TempString, sizeof(TempString),
				fae->fae_ftale->ftale_Magic.str, abs(fae->fae_ftale->ftale_Magic.arg2));
			sprintf(fae->fae_ActionAttrs, "%lu \"%s\"%s",
				(unsigned long) fae->fae_ftale->ftale_Magic.arg1,
				TempString,
				IS_CASE_SENSITIVE(&fae->fae_ftale->ftale_Magic) ? GetLocString(MSGID_FILETYPESACTION_CASE_SENSITIVE) : (STRPTR) "");
			break;
		case ACT_SEARCH:
			ltdm->strings[0] = AddFileTypeActionStrings[1];
			TranslateMatchStringToPrintable(TempString, sizeof(TempString),
				fae->fae_ftale->ftale_Magic.str, abs(fae->fae_ftale->ftale_Magic.arg2));
			sprintf(fae->fae_ActionAttrs, "\"%s\"%s",
				TempString,
				IS_CASE_SENSITIVE(&fae->fae_ftale->ftale_Magic) ? GetLocString(MSGID_FILETYPESACTION_CASE_SENSITIVE) : (STRPTR) "");
			break;
		case ACT_SEARCHSKIPSPACES:
			ltdm->strings[0] = AddFileTypeActionStrings[1];
			TranslateMatchStringToPrintable(TempString, sizeof(TempString),
				fae->fae_ftale->ftale_Magic.str, abs(fae->fae_ftale->ftale_Magic.arg2));
			sprintf(fae->fae_ActionAttrs, "\"%s\"%s%s",
				TempString,
				GetLocString(MSGID_FILETYPESACTION_SKIP_SPACES),
				IS_CASE_SENSITIVE(&fae->fae_ftale->ftale_Magic) ? GetLocString(MSGID_FILETYPESACTION_CASE_SENSITIVE) : (STRPTR) "");
			break;
		case ACT_DOSDEVICE:
			ltdm->strings[0] = AddFileTypeActionStrings[7];
			sprintf(fae->fae_ActionAttrs, "\"%s\"", fae->fae_ftale->ftale_Magic.str);
			break;
		case ACT_DEVICENAME:
			ltdm->strings[0] = AddFileTypeActionStrings[8];
			sprintf(fae->fae_ActionAttrs, "\"%s\"", fae->fae_ftale->ftale_Magic.str);
			break;
		case ACT_CONTENTS:
			ltdm->strings[0] = AddFileTypeActionStrings[9];
			sprintf(fae->fae_ActionAttrs, "\"%s\"", fae->fae_ftale->ftale_Magic.str);
			break;
		case ACT_DOSTYPE:
			ltdm->strings[0] = AddFileTypeActionStrings[10];
			TranslateMatchStringToPrintable(TempString, sizeof(TempString),
				fae->fae_ftale->ftale_Magic.str, abs(fae->fae_ftale->ftale_Magic.arg2));
			sprintf(fae->fae_ActionAttrs, "\"%s\"", TempString);
			break;
		case ACT_MINSIZEMB:
			ltdm->strings[0] = AddFileTypeActionStrings[11];
			sprintf(fae->fae_ActionAttrs,
				GetLocString(MSGID_FILETYPESACTION_MINSIZEMB),
                                fae->fae_ftale->ftale_Magic.arg2);
			break;
		case ACT_FILESIZE:
			ltdm->strings[0] = AddFileTypeActionStrings[2];
			sprintf(fae->fae_ActionAttrs,
				GetLocString(MSGID_FILETYPESACTION_FILESIZE),
                                fae->fae_ftale->ftale_Magic.arg2);
			break;
		case ACT_NAMEPATTERN:
			ltdm->strings[0] = AddFileTypeActionStrings[3];
			sprintf(fae->fae_ActionAttrs, "\"%s\"", fae->fae_ftale->ftale_Magic.str);
			break;
		case ACT_PROTECTION:
			ltdm->strings[0] = AddFileTypeActionStrings[4];
			GetProtectionString(fae->fae_ActionAttrs, sizeof(fae->fae_ActionAttrs),
				fae->fae_ftale->ftale_Magic.arg1, fae->fae_ftale->ftale_Magic.arg2);
			break;
		case ACT_OR:
			strcpy(fae->fae_ActionAttrs, "\033C");
			ltdm->strings[0] = AddFileTypeActionStrings[5];
			break;
		case ACT_ISASCII:
			ltdm->strings[0] = AddFileTypeActionStrings[6];
			break;
		case ACT_MACROCLASS:
			ltdm->strings[0] = AddFileTypeActionStrings[12];
			break;
		case ACT_END:
		default:
			ltdm->strings[0] = "???";
			break;
			}

		ltdm->strings[1] = fae->fae_ActionAttrs;
		}
	else
		{
		// display titles
		ltdm->preparses[0] = "";

		ltdm->strings[0] = "";
		}

	return 0;
}

//----------------------------------------------------------------------------

static void TranslateMatchStringToPrintable(STRPTR Dest, size_t DestLength,
	CONST_STRPTR Src, size_t SrcLength)
{
	while (SrcLength && DestLength > 1)
		{
		if (isprint(*Src) && isascii(*Src))
			{
			*Dest++ = *Src++;
			SrcLength--;
			DestLength--;
			}
		else
			{
			if (DestLength < 4 + 1)
				break;
			sprintf(Dest, "\\x%02x", *Src);
			Src++;
			SrcLength--;
			DestLength -= strlen(Dest);
			Dest += strlen(Dest);
			}
		}
	*Dest = '\0';
}

//----------------------------------------------------------------------------

static void GetProtectionString(STRPTR Buffer, size_t BuffSize,
	ULONG AndMask, ULONG CmpMask)
{
	Buffer = GetProtectionBitString(FIBF_HIDDEN, AndMask, CmpMask,
		MSGID_FILETYPESACTION_PROTECTION_H_SET,
		MSGID_FILETYPESACTION_PROTECTION_H_CLEARED,
		Buffer, &BuffSize);
	Buffer = GetProtectionBitString(FIBF_SCRIPT, AndMask, CmpMask,
		MSGID_FILETYPESACTION_PROTECTION_S_SET,
		MSGID_FILETYPESACTION_PROTECTION_S_CLEARED,
		Buffer, &BuffSize);
	Buffer = GetProtectionBitString(FIBF_PURE, AndMask, CmpMask,
		MSGID_FILETYPESACTION_PROTECTION_P_SET,
		MSGID_FILETYPESACTION_PROTECTION_P_CLEARED,
		Buffer, &BuffSize);
	Buffer = GetProtectionBitString(FIBF_ARCHIVE, AndMask, CmpMask,
		MSGID_FILETYPESACTION_PROTECTION_A_SET,
		MSGID_FILETYPESACTION_PROTECTION_A_CLEARED,
		Buffer, &BuffSize);

	Buffer = GetProtectionBitString(FIBF_READ, AndMask, CmpMask,
		MSGID_FILETYPESACTION_PROTECTION_R_SET,
		MSGID_FILETYPESACTION_PROTECTION_R_CLEARED,
		Buffer, &BuffSize);
	Buffer = GetProtectionBitString(FIBF_WRITE, AndMask, CmpMask,
		MSGID_FILETYPESACTION_PROTECTION_W_SET,
		MSGID_FILETYPESACTION_PROTECTION_W_CLEARED,
		Buffer, &BuffSize);
	Buffer = GetProtectionBitString(FIBF_EXECUTE, AndMask, CmpMask,
		MSGID_FILETYPESACTION_PROTECTION_E_SET,
		MSGID_FILETYPESACTION_PROTECTION_E_CLEARED,
		Buffer, &BuffSize);
	/* Buffer = */ GetProtectionBitString(FIBF_DELETE, AndMask, CmpMask,
		MSGID_FILETYPESACTION_PROTECTION_D_SET,
		MSGID_FILETYPESACTION_PROTECTION_D_CLEARED,
		Buffer, &BuffSize);

}

//----------------------------------------------------------------------------

static STRPTR GetProtectionBitString(ULONG BitMask, ULONG AndMask, ULONG CmpMask,
	ULONG MsgIdSet, ULONG MsgIdCleared,
	STRPTR Buffer, size_t *BuffSize)
{
	if (BitMask & AndMask)
		{
		if (BitMask & CmpMask)
			stccpy(Buffer, GetLocString(MsgIdSet), *BuffSize);
		else
			stccpy(Buffer, GetLocString(MsgIdCleared), *BuffSize);
		}
	else
		stccpy(Buffer, GetLocString(MSGID_FILETYPESACTION_PROTECTION_IGNORE), *BuffSize);

	*BuffSize -= strlen(Buffer);

	return Buffer + strlen(Buffer);
}

//----------------------------------------------------------------------------

static void SetProtectionCycle(Object *o, ULONG BitMask,
	ULONG AndMask, ULONG CmpMask)
{
	if (BitMask & AndMask)
		{
		if (BitMask & CmpMask)
			set(o, MUIA_Cycle_Active, 0);	// set
		else
			set(o, MUIA_Cycle_Active, 1);	// unset
		}
	else
		{
		set(o, MUIA_Cycle_Active, 2);	// ignore
		}
}

//----------------------------------------------------------------------------

void SetFileTypesIcon(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *ln)
{
	BOOL FoundNative = TRUE;

	while (ln)
		{
		struct FileTypesEntry *fte = (struct FileTypesEntry *) ln->tn_User;

		d1(KPrintF(__FILE__ "/%s/%ld: fte=%08lx\n", __FUNC__, __LINE__, fte));
		if (fte)
			{
			d1(KPrintF(__FILE__ "/%s/%ld: fte=%08lx  <%s>\n", __FUNC__, __LINE__, fte, fte->fte_TypeNode.tn_Name));

			if (DisplayFileTypesIcon(inst, fte->fte_TypeNode.tn_Name))
				break;
			}

		FoundNative = FALSE;

		ln = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
			MUIM_NListtree_GetEntry,
			ln,
			MUIV_NListtree_GetEntry_Position_Parent, 0);
		}

	set(inst->fpb_Objects[OBJNDX_Button_FileTypes_CreateIcon],
		MUIA_Disabled, FoundNative);
	set(inst->fpb_Objects[OBJNDX_Button_FileTypes_EditIcon],
		MUIA_Disabled, !FoundNative);
}

//----------------------------------------------------------------------------

SAVEDS(ULONG) INTERRUPT SelectFileTypesEntryHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *ln = NULL;

	// Make sure Popobject is closed
	DoMethod(inst->fpb_Objects[OBJNDX_Popobject_FileTypes_Methods_Add],
		MUIM_Popstring_Close, FALSE);

	get(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIA_NListtree_Active, &ln);

	d1(KPrintF("%s/%s/%ld: inst=%08lx  ln=%08lx\n", __FILE__, __FUNC__, __LINE__, inst, ln));

	if (ln)
		{
		struct FileTypesEntry *fte = (struct FileTypesEntry *) ln->tn_User;
		struct MUI_NListtree_TreeNode *lnRoot;
		struct MUI_NListtree_TreeNode *pln;
		ULONG Level;

		for (pln = ln, Level = 0; pln && Level < 5; Level++)
			{
			pln = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
				MUIM_NListtree_GetEntry, pln,
				MUIV_NListtree_GetEntry_Position_Parent, 0);
			d1(KPrintF("%s/%s/%ld: pln=%08lx <%s>  Level=%lu\n", __FILE__, __FUNC__, __LINE__, \
				pln, pln ? pln->tn_Name : (STRPTR) "", Level));
			}

		d1(KPrintF("%s/%s/%ld: pln=%08lx  pln=%08lx\n", __FILE__, __FUNC__, __LINE__, pln + 0, pln + 1));
		d1(KPrintF("%s/%s/%ld: Level=%lu\n", __FILE__, __FUNC__, __LINE__, Level));

		set(inst->fpb_Objects[OBJNDX_String_FileTypes_Name], MUIA_Disabled, FALSE);
		set(inst->fpb_Objects[OBJNDX_Button_FileTypes_Remove], MUIA_Disabled, FALSE);

		set(inst->fpb_Objects[OBJNDX_Button_FileTypes_Actions_Learn], MUIA_Disabled, FALSE);
		set(inst->fpb_Objects[OBJNDX_Popobject_FileTypes_Methods_Add], MUIA_Disabled, FALSE);

		lnRoot = GetRootEntry(inst, ln);

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods_Add],
			MUIM_NList_Clear);
		if (0 == Stricmp(lnRoot->tn_Name, "Project"))
			{
			d1(KPrintF("%s/%s/%ld: Add Project\n", __FILE__, __FUNC__, __LINE__));

			DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods_Add],
				MUIM_NList_Insert, AddFileTypeActionStringsProject,
				-1, MUIV_NList_Insert_Sorted, 0);
			}
		else if (0 == Stricmp(lnRoot->tn_Name, "Disk"))
			{
			d1(KPrintF("%s/%s/%ld: Add Disk\n", __FILE__, __FUNC__, __LINE__));

			DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods_Add],
				MUIM_NList_Insert, AddFileTypeActionStringsDisk,
				-1, MUIV_NList_Insert_Sorted, 0);
			}
		set(inst->fpb_Objects[OBJNDX_Menu_ExpandFileTypes], MUIA_Menuitem_Enabled, TRUE);
		set(inst->fpb_Objects[OBJNDX_Menu_CollapseFileTypes], MUIA_Menuitem_Enabled, TRUE);

		set(inst->fpb_Objects[OBJNDX_Text_FileTypes_FileTypesName],
			MUIA_Text_Contents, fte->fte_TypeNode.tn_Name);

		FillFileTypesActions(inst, fte, Level <= 2);

		SetFileTypesIcon(inst, ln);

		// Select corresponding entry in OBJNDX_MainListTree
		SelectFileType(inst, &fte->fte_TypeNode);
		}
	else
		{
		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_Clear);

		set(inst->fpb_Objects[OBJNDX_Text_FileTypes_FileTypesName],
			MUIA_Text_Contents, "");

		set(inst->fpb_Objects[OBJNDX_Menu_ExpandFileTypes], MUIA_Menuitem_Enabled, TRUE);
		set(inst->fpb_Objects[OBJNDX_Menu_CollapseFileTypes], MUIA_Menuitem_Enabled, TRUE);

		set(inst->fpb_Objects[OBJNDX_Button_FileTypes_Actions_Remove], MUIA_Disabled, TRUE);
		set(inst->fpb_Objects[OBJNDX_Button_FileTypes_Actions_Learn], MUIA_Disabled, TRUE);
		set(inst->fpb_Objects[OBJNDX_Popobject_FileTypes_Methods_Add], MUIA_Disabled, TRUE);

		set(inst->fpb_Objects[OBJNDX_String_FileTypes_Name], MUIA_Disabled, TRUE);
		set(inst->fpb_Objects[OBJNDX_Button_FileTypes_Remove], MUIA_Disabled, TRUE);

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods_Add],
			MUIM_NList_Clear);
		}

	return 0;
}

//----------------------------------------------------------------------------

SAVEDS(ULONG) INTERRUPT SelectFileTypesActionEntryHookFunc(struct Hook *hook, Object *o, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;
	ULONG n;
	ULONG visibleNdx = 0;

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active, &fae);

	d1(KPrintF(__FUNC__ "/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae)
		{
		char TempString[80];

		d1(KPrintF(__FUNC__ "/%ld: fae=%08lx  action=%ld\n", __FUNC__, __LINE__, fae, fae->fae_ftale->ftale_Magic.action));

		set(inst->fpb_Objects[OBJNDX_Button_FileTypes_Actions_Remove], MUIA_Disabled, FALSE);

		switch (fae->fae_ftale->ftale_Magic.action)
			{
		case ACT_MATCH:
			set(inst->fpb_Objects[OBJNDX_String_Filetypes_Actions_Match_Offset], MUIA_String_Integer, fae->fae_ftale->ftale_Magic.arg1);
			set(inst->fpb_Objects[OBJNDX_Checkmark_FileTypes_Action_Match_Case], MUIA_Selected, IS_CASE_SENSITIVE(&fae->fae_ftale->ftale_Magic));
			TranslateMatchStringToPrintable(TempString, sizeof(TempString),
				fae->fae_ftale->ftale_Magic.str, abs(fae->fae_ftale->ftale_Magic.arg2));
			set(inst->fpb_Objects[OBJNDX_String_Filetypes_Actions_Match_Match], MUIA_String_Contents, TempString);

			visibleNdx = OBJNDX_Group_Filetypes_Actions_Match;
			break;

		case ACT_SEARCH:
			set(inst->fpb_Objects[OBJNDX_Checkmark_FileTypes_Action_Search_SkipSpaces], MUIA_Selected, FALSE);
			set(inst->fpb_Objects[OBJNDX_Checkmark_FileTypes_Action_Search_Case], MUIA_Selected, IS_CASE_SENSITIVE(&fae->fae_ftale->ftale_Magic));
			TranslateMatchStringToPrintable(TempString, sizeof(TempString),
				fae->fae_ftale->ftale_Magic.str, abs(fae->fae_ftale->ftale_Magic.arg2));
			set(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Search_Search], MUIA_String_Contents, TempString);

			visibleNdx =OBJNDX_Group_Filetypes_Actions_Search;
			break;

		case ACT_SEARCHSKIPSPACES:
			set(inst->fpb_Objects[OBJNDX_Checkmark_FileTypes_Action_Search_SkipSpaces], MUIA_Selected, TRUE);
			set(inst->fpb_Objects[OBJNDX_Checkmark_FileTypes_Action_Search_Case], MUIA_Selected, IS_CASE_SENSITIVE(&fae->fae_ftale->ftale_Magic));
			TranslateMatchStringToPrintable(TempString, sizeof(TempString),
				fae->fae_ftale->ftale_Magic.str, abs(fae->fae_ftale->ftale_Magic.arg2));
			set(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Search_Search], MUIA_String_Contents, TempString);

			visibleNdx = OBJNDX_Group_Filetypes_Actions_Search;
			break;

		case ACT_FILESIZE:
			set(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Filesize_Filesize], MUIA_String_Integer, fae->fae_ftale->ftale_Magic.arg2);
			visibleNdx = OBJNDX_Group_Filetypes_Actions_Filesize;
			break;

		case ACT_MINSIZEMB:
			set(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_MinSizeMB_MinSize], MUIA_String_Integer, fae->fae_ftale->ftale_Magic.arg2);
			visibleNdx =OBJNDX_Group_FileTypes_Action_MinSizeMB;
			break;

		case ACT_NAMEPATTERN:
			set(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Pattern_Pattern], MUIA_String_Contents, fae->fae_ftale->ftale_Magic.str);
			visibleNdx =OBJNDX_Group_Filetypes_Actions_Pattern;
			break;

		case ACT_DOSDEVICE:
			set(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_DosDevice_Pattern], MUIA_String_Contents, fae->fae_ftale->ftale_Magic.str);
			visibleNdx = OBJNDX_Group_Filetypes_Actions_DosDevice;
			break;

		case ACT_DEVICENAME:
			set(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_DeviceName_Pattern], MUIA_String_Contents, fae->fae_ftale->ftale_Magic.str);
			visibleNdx = OBJNDX_Group_Filetypes_Actions_DeviceName;
			break;

		case ACT_CONTENTS:
			set(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Contents_Pattern], MUIA_String_Contents, fae->fae_ftale->ftale_Magic.str);
			visibleNdx = OBJNDX_Group_Filetypes_Actions_Contents;
			break;

		case ACT_DOSTYPE:
			set(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_DosType_Pattern], MUIA_String_Contents, fae->fae_ftale->ftale_Magic.str);
			visibleNdx = OBJNDX_Group_Filetypes_Actions_DosType;
			break;

		case ACT_PROTECTION:
			SetProtectionCycle(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_R],
				FIBF_READ, fae->fae_ftale->ftale_Magic.arg1, fae->fae_ftale->ftale_Magic.arg2);
			SetProtectionCycle(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_W],
				FIBF_WRITE, fae->fae_ftale->ftale_Magic.arg1, fae->fae_ftale->ftale_Magic.arg2);
			SetProtectionCycle(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_E],
				FIBF_EXECUTE, fae->fae_ftale->ftale_Magic.arg1, fae->fae_ftale->ftale_Magic.arg2);
			SetProtectionCycle(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_D],
				FIBF_DELETE, fae->fae_ftale->ftale_Magic.arg1, fae->fae_ftale->ftale_Magic.arg2);
			SetProtectionCycle(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_H],
				FIBF_HIDDEN, fae->fae_ftale->ftale_Magic.arg1, fae->fae_ftale->ftale_Magic.arg2);
			SetProtectionCycle(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_S],
				FIBF_SCRIPT, fae->fae_ftale->ftale_Magic.arg1, fae->fae_ftale->ftale_Magic.arg2);
			SetProtectionCycle(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_P],
				FIBF_PURE, fae->fae_ftale->ftale_Magic.arg1, fae->fae_ftale->ftale_Magic.arg2);
			SetProtectionCycle(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_A],
				FIBF_ARCHIVE, fae->fae_ftale->ftale_Magic.arg1, fae->fae_ftale->ftale_Magic.arg2);

			visibleNdx = OBJNDX_Group_Filetypes_Actions_Protection;
			break;

		case ACT_OR:
		case ACT_ISASCII:
		case ACT_MACROCLASS:
		case ACT_END:
			visibleNdx = OBJNDX_Group_Filetypes_Actions_Empty;
			break;
			}
		}
	else
		{
		set(inst->fpb_Objects[OBJNDX_Button_FileTypes_Actions_Remove], MUIA_Disabled, TRUE);
		visibleNdx = OBJNDX_Group_Filetypes_Actions_Empty;
		}

	for (n = OBJNDX_Group_Filetypes_Actions_Protection;
		n <= OBJNDX_Group_Filetypes_Actions_DosType; n++)
		{
		set(inst->fpb_Objects[n], MUIA_ShowMe, n == visibleNdx);
		}

	return 0;
}

//----------------------------------------------------------------------------

SAVEDS(void) INTERRUPT ChangedFileTypesActionMatchMatchHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active, &fae);

	d1(KPrintF(__FUNC__ "/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae && ACT_MATCH == fae->fae_ftale->ftale_Magic.action)
		{
		char TempString[255];
		CONST_STRPTR MatchString = NULL;
		size_t Length;

		get(inst->fpb_Objects[OBJNDX_String_Filetypes_Actions_Match_Match], MUIA_String_Contents, &MatchString);

		Length = TranslateFromPrintable(TempString, sizeof(TempString), MatchString);

		SetMagicString(fae, TempString, Length);

		if (IS_CASE_SENSITIVE(&fae->fae_ftale->ftale_Magic))
			fae->fae_ftale->ftale_Magic.arg2 = Length;
		else
			fae->fae_ftale->ftale_Magic.arg2 = -Length;

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_RedrawEntry, fae);
                }
}

SAVEDS(void) INTERRUPT ChangedFileTypesActionMatchCaseHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active, &fae);

	d1(KPrintF(__FUNC__ "/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae && ACT_MATCH == fae->fae_ftale->ftale_Magic.action)
		{
		size_t Length = abs(fae->fae_ftale->ftale_Magic.arg2);
		ULONG MatchCase = 0;

		get(inst->fpb_Objects[OBJNDX_Checkmark_FileTypes_Action_Match_Case], MUIA_Selected, &MatchCase);

		if (MatchCase)
			fae->fae_ftale->ftale_Magic.arg2 = Length;
		else
			fae->fae_ftale->ftale_Magic.arg2 = -Length;

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_RedrawEntry, fae);
                }
}

SAVEDS(void) INTERRUPT ChangedFileTypesActionMatchOffsetHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active, &fae);

	d1(KPrintF(__FUNC__ "/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae && ACT_MATCH == fae->fae_ftale->ftale_Magic.action)
		{
		ULONG MatchOffset = 0;

		get(inst->fpb_Objects[OBJNDX_String_Filetypes_Actions_Match_Offset], MUIA_String_Integer, &MatchOffset);

		fae->fae_ftale->ftale_Magic.arg1 = MatchOffset;

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_RedrawEntry, fae);
                }
}

SAVEDS(void) INTERRUPT ChangedFileTypesActionSearchSearchHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active, &fae);

	d1(KPrintF(__FUNC__ "/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae && (ACT_SEARCH == fae->fae_ftale->ftale_Magic.action || ACT_SEARCHSKIPSPACES == fae->fae_ftale->ftale_Magic.action))
		{
		char TempString[255];
		CONST_STRPTR SearchString = NULL;
		size_t Length;

		get(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Search_Search], MUIA_String_Contents, &SearchString);

		Length = TranslateFromPrintable(TempString, sizeof(TempString), SearchString);

		if (IS_CASE_SENSITIVE(&fae->fae_ftale->ftale_Magic))
			fae->fae_ftale->ftale_Magic.arg2 = Length;
		else
			fae->fae_ftale->ftale_Magic.arg2 = -Length;

		SetMagicString(fae, TempString, Length);

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_RedrawEntry, fae);
                }
}

SAVEDS(void) INTERRUPT ChangedFileTypesActionSearchCaseHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active, &fae);

	d1(KPrintF(__FUNC__ "/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae && (ACT_SEARCH == fae->fae_ftale->ftale_Magic.action || ACT_SEARCHSKIPSPACES == fae->fae_ftale->ftale_Magic.action))
		{
		ULONG SearchCaseSensitive = 0;
		size_t Length = abs(fae->fae_ftale->ftale_Magic.arg2);

		get(inst->fpb_Objects[OBJNDX_Checkmark_FileTypes_Action_Search_Case], MUIA_Selected, &SearchCaseSensitive);

		if (SearchCaseSensitive)
			fae->fae_ftale->ftale_Magic.arg2 = Length;
		else
			fae->fae_ftale->ftale_Magic.arg2 = -Length;

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_RedrawEntry, fae);
                }
}

SAVEDS(void) INTERRUPT ChangedFileTypesActionSearchSkipSpacesHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active, &fae);

	d1(KPrintF(__FUNC__ "/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae && (ACT_SEARCH == fae->fae_ftale->ftale_Magic.action || ACT_SEARCHSKIPSPACES == fae->fae_ftale->ftale_Magic.action))
		{
		ULONG SkipSpaces = 0;

		get(inst->fpb_Objects[OBJNDX_Checkmark_FileTypes_Action_Search_SkipSpaces], MUIA_Selected, &SkipSpaces);

		if (SkipSpaces)
			fae->fae_ftale->ftale_Magic.action = ACT_SEARCHSKIPSPACES;
		else
			fae->fae_ftale->ftale_Magic.action = ACT_SEARCH;

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_RedrawEntry, fae);
                }
}

SAVEDS(void) INTERRUPT ChangedFileTypesActionFilesizeHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active, &fae);

	d1(KPrintF(__FUNC__ "/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae && ACT_FILESIZE == fae->fae_ftale->ftale_Magic.action)
		{
		ULONG FileSize = 0;

		get(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Filesize_Filesize], MUIA_String_Integer, &FileSize);

		fae->fae_ftale->ftale_Magic.arg2 = FileSize;
		
		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_RedrawEntry, fae);
                }
}

SAVEDS(void) INTERRUPT ChangedFileTypesActionMinSizeHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active, &fae);

	d1(KPrintF(__FUNC__ "/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae && ACT_MINSIZEMB == fae->fae_ftale->ftale_Magic.action)
		{
		ULONG FileSize = 0;

		get(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_MinSizeMB_MinSize], MUIA_String_Integer, &FileSize);

		fae->fae_ftale->ftale_Magic.arg2 = FileSize;

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_RedrawEntry, fae);
                }
}
SAVEDS(void) INTERRUPT ChangedFileTypesActionPatternHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active, &fae);

	d1(KPrintF(__FUNC__ "/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae && ACT_NAMEPATTERN == fae->fae_ftale->ftale_Magic.action)
		{
		CONST_STRPTR Pattern = NULL;

		get(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Pattern_Pattern], MUIA_String_Contents, &Pattern);

		SetMagicString(fae, Pattern, 1 + strlen(Pattern));
		
		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_RedrawEntry, fae);
                }
}


SAVEDS(void) INTERRUPT ChangedFileTypesActionDosDeviceHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active, &fae);

	d1(KPrintF(__FUNC__ "/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae && ACT_DOSDEVICE == fae->fae_ftale->ftale_Magic.action)
		{
		CONST_STRPTR Pattern = NULL;

		get(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_DosDevice_Pattern], MUIA_String_Contents, &Pattern);

		SetMagicString(fae, Pattern, 1 + strlen(Pattern));

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_RedrawEntry, fae);
                }
}


SAVEDS(void) INTERRUPT ChangedFileTypesActionDeviceNameHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active, &fae);

	d1(KPrintF(__FUNC__ "/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae && ACT_DEVICENAME == fae->fae_ftale->ftale_Magic.action)
		{
		CONST_STRPTR Pattern = NULL;

		get(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_DeviceName_Pattern], MUIA_String_Contents, &Pattern);

		SetMagicString(fae, Pattern, 1 + strlen(Pattern));

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_RedrawEntry, fae);
                }
}


SAVEDS(void) INTERRUPT ChangedFileTypesActionContentsHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active, &fae);

	d1(KPrintF(__FUNC__ "/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae && ACT_CONTENTS == fae->fae_ftale->ftale_Magic.action)
		{
		CONST_STRPTR Pattern = NULL;

		get(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_Contents_Pattern], MUIA_String_Contents, &Pattern);

		SetMagicString(fae, Pattern, 1 + strlen(Pattern));

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_RedrawEntry, fae);
                }
}


SAVEDS(void) INTERRUPT ChangedFileTypesActionDosTypeHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active, &fae);

	d1(KPrintF(__FUNC__ "/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae && ACT_DOSTYPE == fae->fae_ftale->ftale_Magic.action)
		{
		char TempString[255];
		size_t Length;
		CONST_STRPTR MatchString = NULL;

		get(inst->fpb_Objects[OBJNDX_String_FileTypes_Action_DosType_Pattern], MUIA_String_Contents, &MatchString);

		Length = TranslateFromPrintable(TempString, sizeof(TempString), MatchString);

		SetMagicString(fae, TempString, Length);
		fae->fae_ftale->ftale_Magic.arg2 = Length;

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_RedrawEntry, fae);
                }
}


SAVEDS(void) INTERRUPT ChangedFileTypesActionProtectionHookFunc(struct Hook *hook, APTR obj, struct NList_DestructMessage *ltdm)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active, &fae);

	d1(KPrintF(__FUNC__ "/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae && ACT_PROTECTION == fae->fae_ftale->ftale_Magic.action)
		{
		ULONG Protect = 0;

		if (inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_R] == obj)
			{
			get(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_R], MUIA_Cycle_Active, &Protect);
			SetProtectionBits(fae, Protect, FIBF_READ);
			}
		else if (inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_W] == obj)
			{
			get(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_W], MUIA_Cycle_Active, &Protect);
			SetProtectionBits(fae, Protect, FIBF_WRITE);
			}
		else if (inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_E] == obj)
			{
			get(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_E], MUIA_Cycle_Active, &Protect);
			SetProtectionBits(fae, Protect, FIBF_EXECUTE);
			}
		else if (inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_D] == obj)
			{
			get(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_D], MUIA_Cycle_Active, &Protect);
			SetProtectionBits(fae, Protect, FIBF_DELETE);
			}
		else if (inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_S] == obj)
			{
			get(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_S], MUIA_Cycle_Active, &Protect);
			SetProtectionBits(fae, Protect, FIBF_SCRIPT);
			}
		else if (inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_P] == obj)
			{
			get(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_P], MUIA_Cycle_Active, &Protect);
			SetProtectionBits(fae, Protect, FIBF_PURE);
			}
		else if (inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_A] == obj)
			{
			get(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_A], MUIA_Cycle_Active, &Protect);
			SetProtectionBits(fae, Protect, FIBF_ARCHIVE);
			}
		else if (inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_H] == obj)
			{
			get(inst->fpb_Objects[OBJNDX_Cycle_Filetypes_Protection_H], MUIA_Cycle_Active, &Protect);
			SetProtectionBits(fae, Protect, FIBF_HIDDEN);
			}

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_RedrawEntry, fae);
                }
}

//----------------------------------------------------------------------------

static void SetMagicString(struct FileTypesActionEntry *fae, CONST_STRPTR String, size_t Length)
{
	if (fae->fae_ftale->ftale_AllocatedString)
		free(fae->fae_ftale->ftale_AllocatedString);

	fae->fae_ftale->ftale_AllocatedString = fae->fae_ftale->ftale_Magic.str = malloc(Length);
	if (fae->fae_ftale->ftale_Magic.str)
		memcpy(fae->fae_ftale->ftale_Magic.str, String, Length);
}

//----------------------------------------------------------------------------

static size_t TranslateFromPrintable(STRPTR Buffer, size_t BuffLength,
	CONST_STRPTR InputString)
{
	size_t DestLength = 0;

	d1(KPrintF(__FUNC__ "/%ld: InputString=<%s>\n", __FUNC__, __LINE__, InputString));

	while (BuffLength && *InputString)
		{
		if ('\\' == InputString[0]
			&& strlen(InputString) >= 4
			&& 'x' == tolower(InputString[1])
			&& isxdigit(InputString[2])
			&& isxdigit(InputString[3]))
			{
			*Buffer++ = (HexValue(InputString[2]) << 4)
				+ HexValue(InputString[3]);
			InputString += 4;	// skip "0x99"
			}
		else
			{
			*Buffer++ = *InputString++;
			}

		DestLength++;
		BuffLength--;
		}

	d1(KPrintF(__FUNC__ "/%ld: DestLength=%d\n", __FUNC__, __LINE__, DestLength));

	return DestLength;
}

//----------------------------------------------------------------------------

static UBYTE HexValue(char ch)
{
	if (isdigit(ch))
		return (UBYTE) (ch - '0');
	else
		return (UBYTE) (tolower(ch) + 10 - 'a');
}

//----------------------------------------------------------------------------

static void SetProtectionBits(struct FileTypesActionEntry *fae,
	enum ProtectionMode Protect, ULONG Mask)
{
	switch (Protect)
		{
	case PROT_Set:
		fae->fae_ftale->ftale_Magic.arg1 |= Mask;
		fae->fae_ftale->ftale_Magic.arg2 |= Mask;
		break;
	case PROT_Unset:
		fae->fae_ftale->ftale_Magic.arg1 |= Mask;
		fae->fae_ftale->ftale_Magic.arg2 &= ~Mask;
		break;
	case PROT_Ignore:
	default:
		fae->fae_ftale->ftale_Magic.arg1 &= ~Mask;
		fae->fae_ftale->ftale_Magic.arg2 &= ~Mask;
		}
}

//----------------------------------------------------------------------------

SAVEDS(void) INTERRUPT RenameFileTypeHookFunc(struct Hook *hook, APTR obj, Msg *msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *ln = NULL;

	get(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIA_NListtree_Active, &ln);

	if (ln)
		{
		struct FileTypesEntry *fte = (struct FileTypesEntry *) ln->tn_User;
		CONST_STRPTR NewName = NULL;

		CleanupFoundNodes(inst);

		get(inst->fpb_Objects[OBJNDX_String_FileTypes_Name], MUIA_String_Contents, &NewName);

		if (fte->fte_AllocatedName)
			free(fte->fte_AllocatedName);
		fte->fte_AllocatedName = fte->fte_TypeNode.tn_Name = strdup(NewName);

		DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
			MUIM_NListtree_Redraw,
			ln);

		set(inst->fpb_Objects[OBJNDX_Text_FileTypes_FileTypesName],
			MUIA_Text_Contents, fte->fte_TypeNode.tn_Name);

		SetFileTypesIcon(inst, ln);
		}
}


SAVEDS(void) INTERRUPT AddFileTypeHookFunc(struct Hook *hook, APTR obj, Msg *msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct TypeNode NewTn;
	struct MUI_NListtree_TreeNode *ln;

	memset(&NewTn, 0, sizeof(NewTn));

	CleanupFoundNodes(inst);

	NewTn.tn_Name = GetLocString(MSGID_NEW_FILETYPE);
	NewTn.tn_Description[0].action = ACT_END;

	ln = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
		MUIM_NListtree_Insert,
		NewTn.tn_Name,
		&NewTn,
		MUIV_NListtree_Insert_ListNode_Active,
		MUIV_NListtree_Insert_PrevNode_Active,
		TNF_LIST);

	if (ln)
		{
		// Make the new filetype the active one
		set(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIA_NListtree_Active, ln);

		// try to activate filetype name string gadget
		set(inst->fpb_Objects[OBJNDX_WIN_Main], MUIA_Window_ActiveObject, inst->fpb_Objects[OBJNDX_String_FileTypes_Name]);
		}
}


SAVEDS(void) INTERRUPT RemoveFileTypeHookFunc(struct Hook *hook, APTR obj, Msg *msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct MUI_NListtree_TreeNode *ln = NULL;

	CleanupFoundNodes(inst);
	get(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIA_NListtree_Active, &ln);

	if (ln)
		{
		DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
			MUIM_NListtree_Remove,
			MUIV_NListtree_Remove_ListNode_Active,
			ln);
		}
}


// Close "Add method" popup
SAVEDS(void) INTERRUPT AddFileTypesMethodHookFunc(struct Hook *hook, APTR obj, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;
	struct MUI_NListtree_TreeNode *ln = NULL;

	d1(KPrintF("%s/%s//%ld: inst=%08lx\n", __FILE__, __FUNC__, __LINE__, inst));

	get(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIA_NListtree_Active, &ln);
	d1(KPrintF("%s/%s//%ld: ln=%08lx\n", __FILE__, __FUNC__, __LINE__, ln));
	if (ln)
		{
		struct FileTypesEntry *fte = (struct FileTypesEntry *) ln->tn_User;
		CONST_STRPTR NewActionString;

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_GetEntry,
			MUIV_NList_GetEntry_Active, &fae);

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods_Add],
			MUIM_NList_GetEntry,
			MUIV_NList_GetEntry_Active,
			&NewActionString);
		d1(KPrintF("%s/%s//%ld: NewAction=<%s>\n", __FILE__, __FUNC__, __LINE__, NewActionString));

		if (NewActionString && strlen(NewActionString))
			{
			struct Magic Newmg;
			struct FileTypesActionListEntry *ftaleNew;

			memset(&Newmg, 0, sizeof(Newmg));
			Newmg.action = GetActionFromString(NewActionString);
			Newmg.str = (STRPTR) "";
			d1(KPrintF("%s/%s//%ld: NewAction=<%s>  %ld\n", __FILE__, __FUNC__, __LINE__, NewActionString, Newmg.action));

			ftaleNew = AddFileTypesAction(fte, &Newmg, fae ? &fae->fae_ftale->ftale_Node : NULL);

			if (ftaleNew)
				{
				ULONG InsertPos;

				if (fae)
					{
					InsertPos = MUIV_NList_GetPos_Start;

					DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
						MUIM_NList_GetPos, fae, &InsertPos);

					d1(KPrintF("%s/%s/%ld: InsertPos=%ld\n", __FILE__, __FUNC__, __LINE__, InsertPos));

					InsertPos++;
					}
				else
					InsertPos = MUIV_NList_Insert_Bottom;

				DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
					MUIM_NList_InsertSingle,
					ftaleNew,
					InsertPos);

				// Make the new action the active one
				get(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods], MUIA_NList_InsertPosition, &InsertPos);
				set(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods], MUIA_NList_Active, InsertPos);
				}
			}
		}

	DoMethod(inst->fpb_Objects[OBJNDX_Popobject_FileTypes_Methods_Add], MUIM_Popstring_Close, TRUE);
}


SAVEDS(void) INTERRUPT RemoveFileTypesActionHookFunc(struct Hook *hook, APTR obj, Msg *msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
		MUIM_NList_GetEntry,
		MUIV_NList_GetEntry_Active, &fae);

	d1(KPrintF(__FUNC__ "/%ld: fae=%08lx\n", __FUNC__, __LINE__, fae));

	if (fae)
		{
		Remove(&fae->fae_ftale->ftale_Node);

		DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
			MUIM_NList_Remove,
			MUIV_NList_Remove_Active);
		}
}

//----------------------------------------------------------------------------

SAVEDS(APTR) INTERRUPT FileTypesConstructFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_ConstructMessage *ltcm)
{
	struct FileTypesEntry *fte;
	BOOL Success = FALSE;

	d1(kprintf(__FILE__ "/%s/%ld: obj=%08lx  ltcm=%08lx  memPool=%08lx  UserData=%08lx\n", \
		__FUNC__, __LINE__, obj, ltcm, ltcm->MemPool, ltcm->UserData));

	do	{
		const struct TypeNode *TnOrig = ltcm->UserData;
		const struct Magic *curr;

		fte = AllocPooled(ltcm->MemPool, sizeof(struct FileTypesEntry));
		d1(KPrintF(__FILE__ "/%s/%ld: fte=%08lx\n", __FUNC__, __LINE__, fte));
		if (NULL == fte)
			break;

		NewList(&fte->fte_MagicList);
		fte->fte_TypeNode = *TnOrig;

		fte->fte_FindStart = 0;
		fte->fte_FindLength = 0;

		fte->fte_AllocatedName2 = NULL;

		fte->fte_AllocatedName = fte->fte_TypeNode.tn_Name = strdup(TnOrig->tn_Name);
		if (NULL == fte->fte_AllocatedName)
			break;

		// add all ACT_*** entries to fte_MagicList
		for (curr = TnOrig->tn_Description; ACT_END != curr->action; curr++)
			{
			d1(KPrintF(__FILE__ "/%s/%ld: curr=%08lx  action=%ld\n", __FUNC__, __LINE__, curr, curr->action));

			if (!AddFileTypesAction(fte, curr, NULL))
				break;
			}

		Success = TRUE;
		} while (0);

	if (!Success && fte)
		{
		CleanupFileTypesActions(fte);
		if (fte->fte_AllocatedName)
			{
			free(fte->fte_AllocatedName);
			fte->fte_AllocatedName = NULL;
			}
		FreePooled(ltcm->MemPool, fte, sizeof(struct FileTypesEntry));
		fte = NULL;
		}

	return fte;
}

SAVEDS(void) INTERRUPT FileTypesDestructFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_DestructMessage *ltdm)
{
	struct FileTypesEntry *fte = (struct FileTypesEntry *) ltdm->UserData;

	d1(KPrintF(__FILE__ "/%s/%ld: fte=%08lx\n", __FUNC__, __LINE__, fte));

	CleanupFileTypesActions(fte);

	if (fte->fte_AllocatedName)
		free(fte->fte_AllocatedName);
	if (fte->fte_AllocatedName2)
		free(fte->fte_AllocatedName2);

	FreePooled(ltdm->MemPool, fte, sizeof(struct FileTypesEntry));
	ltdm->UserData = NULL;
}

SAVEDS(ULONG) INTERRUPT FileTypesDisplayFunc(struct Hook *hook, APTR obj, struct MUIP_NListtree_DisplayMessage *ltdm)
{
	struct FileTypesEntry *fte = (struct FileTypesEntry *) ltdm->TreeNode->tn_User;

	d1(KPrintF(__FILE__ "/%s/%ld: fte=%08lx\n", __FUNC__, __LINE__, fte));

	if (fte)
		{
		if (fte->fte_FindLength > 0)
			{
			if (fte->fte_AllocatedName2)
				free(fte->fte_AllocatedName2);

			fte->fte_AllocatedName2 = malloc(1 + strlen(fte->fte_TypeNode.tn_Name) + 2 * 6);
			if (fte->fte_AllocatedName2)
				{
				if (fte->fte_FindStart)
					stccpy(fte->fte_AllocatedName2, fte->fte_TypeNode.tn_Name, fte->fte_FindStart);
				else
					strcpy(fte->fte_AllocatedName2, "");

				strcat(fte->fte_AllocatedName2, "\0335" );	// FILLPEN
				strncat(fte->fte_AllocatedName2, fte->fte_TypeNode.tn_Name + fte->fte_FindStart, fte->fte_FindLength);
				strcat(fte->fte_AllocatedName2, MUIX_PT);
				strcat(fte->fte_AllocatedName2, fte->fte_TypeNode.tn_Name + fte->fte_FindStart + fte->fte_FindLength);

				ltdm->Array[0] = fte->fte_AllocatedName2;
				}
			else
				{
				ltdm->Array[0] = fte->fte_TypeNode.tn_Name;
				}
			}
		else
			{
			ltdm->Array[0] = fte->fte_TypeNode.tn_Name;
			}
		}

	return 0;
}

//----------------------------------------------------------------------------

static struct FileTypesActionListEntry *AddFileTypesAction(struct FileTypesEntry *fte,
	const struct Magic *curr, struct Node *PrevNode)
{
	struct FileTypesActionListEntry *ftale;
	BOOL Success = FALSE;

	ftale = calloc(1, sizeof(struct FileTypesActionListEntry));
	if (ftale)
		{
		ftale->ftale_AllocatedString = NULL;
		ftale->ftale_Magic = *curr;

		switch (curr->action)
			{
		case ACT_SEARCHSKIPSPACES:
		case ACT_MATCH:
			ftale->ftale_AllocatedString = ftale->ftale_Magic.str = malloc(1 + abs(ftale->ftale_Magic.arg2));
			if (ftale->ftale_Magic.str)
				{
				memcpy(ftale->ftale_Magic.str, curr->str, abs(ftale->ftale_Magic.arg2));
				Success = TRUE;
				}
			break;
		case ACT_NAMEPATTERN:
			ftale->ftale_AllocatedString = ftale->ftale_Magic.str = malloc(1 + strlen(ftale->ftale_Magic.str));
			if (ftale->ftale_Magic.str)
				{
				strcpy(ftale->ftale_Magic.str, curr->str);
				Success = TRUE;
				}
			break;
		default:
			Success = TRUE;
			break;
			}
		}

	if (Success)
		{
		if (PrevNode)
			Insert(&fte->fte_MagicList, &ftale->ftale_Node, PrevNode->ln_Pred);
		else
			AddTail(&fte->fte_MagicList, &ftale->ftale_Node);
		}
	else
		{
		if (ftale)
			free(ftale);
		ftale = NULL;
		}

	return ftale;
}

//----------------------------------------------------------------------------

static void CleanupFileTypesActions(struct FileTypesEntry *fte)
{
	struct FileTypesActionListEntry *ftale;

	while ((ftale = (struct FileTypesActionListEntry *) RemTail(&fte->fte_MagicList)))
		{
		if (ftale->ftale_AllocatedString)
			free(ftale->ftale_AllocatedString);
		free(ftale);
		}
}

//----------------------------------------------------------------------------

static ULONG GetActionFromString(CONST_STRPTR ActionString)
{
	static const ULONG Actions[] =
		{
		ACT_MATCH,
		ACT_SEARCH,
		ACT_FILESIZE,
		ACT_NAMEPATTERN,
		ACT_PROTECTION,
		ACT_OR,
		ACT_ISASCII,
		ACT_DOSDEVICE,
		ACT_DEVICENAME,
		ACT_CONTENTS,
		ACT_DOSTYPE,
		ACT_MINSIZEMB,
		ACT_MACROCLASS,
		};
	ULONG n;

	for (n = 0; AddFileTypeActionStrings[n]; n++)
		{
		if (n < Sizeof(Actions) &&
			0 == strcmp(AddFileTypeActionStrings[n], ActionString))
			{
			return Actions[n];
			}
		}

	// not found -- this should never happen
	return ACT_END;
}

//----------------------------------------------------------------------------

SAVEDS(void) INTERRUPT FileTypesActionDragDropSortHookFunc(struct Hook *hook, APTR obj, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	struct FileTypesActionEntry *fae;
	struct MUI_NListtree_TreeNode *ln = NULL;
	ULONG InsertPosition = 0;

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst));

	get(obj, MUIA_NList_DragSortInsert, &InsertPosition);
	DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
		MUIM_NList_GetEntry,
		InsertPosition, &fae);

	get(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIA_NListtree_Active, &ln);

	if (ln && fae)
		{
		struct FileTypesEntry *fte = (struct FileTypesEntry *) ln->tn_User;
		struct FileTypesActionEntry *faeParent = NULL;

		d1(KPrintF(__FUNC__ "/%ld: InsertPosition=%lu  fae=%08lx\n", __FUNC__, __LINE__, InsertPosition, fae));

		// Remove from parent list at former position
		Remove(&fae->fae_ftale->ftale_Node);

		if (InsertPosition > 0)
			{
			// Get new parent
			DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
				MUIM_NList_GetEntry,
				InsertPosition - 1, &faeParent);
			}

		d1(KPrintF(__FUNC__ "/%ld: faeParent=%08lx\n", __FUNC__, __LINE__));

		// insert into parent list at new position
		if (faeParent)
			Insert(&fte->fte_MagicList, &fae->fae_ftale->ftale_Node, &faeParent->fae_ftale->ftale_Node);
		else
			AddHead(&fte->fte_MagicList, &fae->fae_ftale->ftale_Node);
		}
}

//----------------------------------------------------------------------------

SAVEDS(void) INTERRUPT LearnFileTypeHookFunc(struct Hook *hook, APTR obj, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	BPTR OldDir = (BPTR)NULL;
	BPTR fh = (BPTR)NULL;
	STRPTR MergedDataBuffer = NULL;
	STRPTR ReadDataBuffer = NULL;
	UBYTE *CompareFlags = NULL;
	size_t BuffLength = 488;

	struct MUI_NListtree_TreeNode *ln = NULL;

	get(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIA_NListtree_Active, &ln);

	d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst, ln));

	do     {
		BOOL Result;
		struct Window *win = NULL;
		ULONG n;
		ULONG StartOffset, Count;
		size_t MergedLength = BuffLength;
		struct MUI_NListtree_TreeNode *ln = NULL;
		struct FileTypesEntry *fte;

		get(inst->fpb_Objects[OBJNDX_NListtree_FileTypes], MUIA_NListtree_Active, &ln);

		d1(KPrintF(__FUNC__ "/%ld: inst=%08lx\n", __FUNC__, __LINE__, inst, ln));

		if (NULL == ln)
			break;

		fte = (struct FileTypesEntry *) ln->tn_User;

		get(inst->fpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Window, &win);
		if (NULL == win)
			break;

		CompareFlags = malloc(BuffLength);
		if (NULL == CompareFlags)
			break;

		ReadDataBuffer = malloc(BuffLength);
		if (NULL == ReadDataBuffer)
			break;

		MergedDataBuffer = malloc(BuffLength);
		if (NULL == MergedDataBuffer)
			break;

		if (NULL == inst->fpb_LearnReq)
			{
			inst->fpb_LearnReq = MUI_AllocAslRequestTags(ASL_FileRequest,
				ASLFR_Flags1, FRF_DOPATTERNS,
				ASLFR_UserData, inst,
				ASLFR_DoMultiSelect, TRUE,
				ASLFR_IntuiMsgFunc, &inst->fpb_Hooks[HOOKNDX_AslIntuiMsg],
				TAG_END);
			}

		if (NULL == inst->fpb_LearnReq)
			break;

		//AslRequest(
		Result = MUI_AslRequestTags(inst->fpb_LearnReq,
			ASLFR_Window, win,
			ASLFR_SleepWindow, TRUE,
			ASLFR_TitleText, GetLocString(MSGID_LEARN_OPEN),
			TAG_END);

		if (!Result)
			break;

		if (inst->fpb_LearnReq->fr_NumArgs < 2)
			{
			MUI_Request(inst->fpb_Objects[OBJNDX_APP_Main], inst->fpb_Objects[OBJNDX_WIN_Main], 0, NULL,
				GetLocString(MSGID_SELECT_TWO_REQ_OK),
				GetLocString(MSGID_SELECT_TWO_REQ_CONTENTS));
			break;
			}

		// read all specified sample files,
		// and collect equal areas
		for (n = 0; n < inst->fpb_LearnReq->fr_NumArgs; n++)
			{
			OldDir = CurrentDir(inst->fpb_LearnReq->fr_ArgList[n].wa_Lock);

			// open next file
			fh = Open(inst->fpb_LearnReq->fr_ArgList[n].wa_Name, MODE_OLDFILE);
			if ((BPTR)NULL == fh)
				break;

			// Read one block
			MergedLength = FRead(fh, ReadDataBuffer, 1, MergedLength);
			if (MergedLength < 1)
				break;

			if (0 == n)
				{
				// if first file, then initialize MergedDataBuffer and CompareFlags
				memcpy(MergedDataBuffer, ReadDataBuffer, MergedLength);
				memset(CompareFlags, 1, MergedLength);
				}
			else
				{
				// mask out all different bytes
				ULONG k;

				for (k = 0; k < MergedLength; k++)
					{
					if (MergedDataBuffer[k] != ReadDataBuffer[k])
						CompareFlags[k] = 0;
					}
				}

			Close(fh);
			fh = (BPTR)NULL;

			CurrentDir(OldDir);
			OldDir = (BPTR)NULL;
			}

		// now we have the common data in MergedDataBuffer,
		// and a 1 in CompareFlags for each byte that was
		// equal for all sample files
		for (n = Count = StartOffset= 0; n < MergedLength; n++)
			{
			if (CompareFlags[n])
				{
				if (0 == Count++)
					StartOffset = n;
				}
			else
				{
				// ignore all equal sections of less than 2 bytes
				if (Count >= 2)
					{
					struct Magic Newmg;
					struct FileTypesActionListEntry *ftaleNew;

					memset(&Newmg, 0, sizeof(Newmg));
					Newmg.action = ACT_MATCH;
					Newmg.arg1 = StartOffset;
					Newmg.arg2 = Count;
					Newmg.str = MergedDataBuffer + StartOffset;

					ftaleNew = AddFileTypesAction(fte, &Newmg, NULL);

					if (ftaleNew)
						{
						DoMethod(inst->fpb_Objects[OBJNDX_NListview_FileTypes_Methods],
							MUIM_NList_InsertSingle,
							ftaleNew,
							MUIV_NList_Insert_Bottom);
						}
					}
				Count = 0;
				}
			}
		} while (0);

	if (fh)
		Close(fh);
	if (OldDir)
		CurrentDir(OldDir);
	if (MergedDataBuffer)
		free(MergedDataBuffer);
	if (ReadDataBuffer)
		free(ReadDataBuffer);
	if (CompareFlags)
		free(CompareFlags);
}

//----------------------------------------------------------------------------

SAVEDS(void) INTERRUPT CreateNewFileTypesIconHookFunc(struct Hook *hook, APTR obj, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;

	if (inst->fpb_IconObject)
		{
		struct MUI_NListtree_TreeNode *ln;

		ln = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
			MUIM_NListtree_GetEntry,
			MUIV_NListtree_GetEntry_ListNode_Active,
			MUIV_NListtree_GetEntry_Position_Active, 0);
		if (ln)
			{
			struct FileTypesEntry *fte = (struct FileTypesEntry *) ln->tn_User;
			char IconPath[MAX_FILENAME];
			struct TagItem ti[] = {
				{ICONA_NoPosition, TRUE},
				{TAG_END}};

			DefIconFullName(inst, IconPath, sizeof(IconPath),
				fte->fte_TypeNode.tn_Name);

			DoMethod(inst->fpb_IconObject,
				IDTM_Write,
				IconPath,
				NULL,
				ti);

			// Toggle selection of current entry to enforce re-evaluation of icon via HOOKNDX_SelectFileTypesEntry
			set(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
				MUIA_NListtree_Active, MUIV_NListtree_Active_Off);
			set(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
				MUIA_NListtree_Active, ln);
			}
		}
}

//----------------------------------------------------------------------------

SAVEDS(void) INTERRUPT EditFileTypesIconHookFunc(struct Hook *hook, APTR obj, Msg msg)
{
	struct FileTypesPrefsInst *inst = (struct FileTypesPrefsInst *) hook->h_Data;
	BPTR DirLock = (BPTR)NULL;

	do	{
		struct MUI_NListtree_TreeNode *ln;
		char IconPath[MAX_FILENAME];
		STRPTR DefIconName = "";

		if (NULL == inst->fpb_IconObject)
			break;

		ln = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
			MUIM_NListtree_GetEntry,
			MUIV_NListtree_GetEntry_ListNode_Active,
			MUIV_NListtree_GetEntry_Position_Active, 0);
		if (NULL == ln)
			break;

		get(inst->fpb_Objects[OBJNDX_Text_FileTypes_DefIconName], MUIA_Text_Contents, &DefIconName);

		stccpy(IconPath, inst->fpb_DefIconPath, sizeof(IconPath));
		AddPart(IconPath, DefIconName, sizeof(IconPath));

		DirLock = Lock(inst->fpb_DefIconPath, ACCESS_READ);
		if ((BPTR)NULL == DirLock)
			break;

		DefIconName = FilePart(IconPath);
		if (NULL == DefIconName)
			break;

		set(inst->fpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, TRUE);
		InvokeWBInfo(inst, DirLock, DefIconName);
		SetFileTypesIcon(inst, ln);
		set(inst->fpb_Objects[OBJNDX_WIN_Main], MUIA_Window_Sleep, FALSE);
		} while (0);

	if (DirLock)
		UnLock(DirLock);
}

//----------------------------------------------------------------------------

BOOL DisplayFileTypesIcon(struct FileTypesPrefsInst *inst, CONST_STRPTR FileTypesName)
{
	char IconPath[MAX_FILENAME];
	BOOL Success = FALSE;

	if (inst->fpb_IconObject)
		{
		DisposeIconObject(inst->fpb_IconObject);
		inst->fpb_IconObject = NULL;
		}

	DefIconFullName(inst, IconPath, sizeof(IconPath), FileTypesName);

	inst->fpb_IconObject = NewIconObject(IconPath, NULL);
	if (inst->fpb_IconObject)
		{
		// display name of filetype
		set(inst->fpb_Objects[OBJNDX_Text_FileTypes_DefIconName],
			MUIA_Text_Contents, FilePart(IconPath));

		// display icon for filetype
		set(inst->fpb_Objects[OBJNDX_IconObjectMCC_FileTypes_Icon],
			MUIA_Iconobj_Object, inst->fpb_IconObject);

		Success = TRUE;
		}

	return Success;
}

//----------------------------------------------------------------------------

static void DefIconFullName(struct FileTypesPrefsInst *inst,
	STRPTR Path, size_t MaxLen, CONST_STRPTR FileTypesName)
{
	stccpy(Path, inst->fpb_DefIconPath, MaxLen);
	AddPart(Path, "def_", MaxLen);
	strcat(Path, FileTypesName);
}

//----------------------------------------------------------------------------

static void InvokeWBInfo(struct FileTypesPrefsInst *inst,
	BPTR DirLock, STRPTR DefIconName)
{
	d1(KPrintF(__FILE__ "/%s/%ld: START DefIconName=<%s>\n", __FUNC__, __LINE__, DefIconName));

	WBInfo(DirLock, DefIconName, inst->fpb_WBScreen);

	d1(KPrintF(__FILE__ "/%s/%ld: END\n", __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static void SelectFileType(struct FileTypesPrefsInst *inst, const struct TypeNode *tn)
{
	d1(KPrintF(__FILE__ "/%s/%ld: START  tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn->tn_Name));

	// first, deselect current entry
	set(inst->fpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Active, MUIV_NListtree_Active_Off);

	while (tn)
		{
		struct MUI_NListtree_TreeNode *lttn;

		d1(KPrintF(__FILE__ "/%s/%ld: tn=%08lx  <%s>\n", __FUNC__, __LINE__, tn, tn->tn_Name));

		lttn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_FindName,
			MUIV_NListtree_FindName_ListNode_Root,
			tn->tn_Name,
			MUIV_NListtree_FindName_Flag_SameLevel);
		d1(KPrintF(__FILE__ "/%s/%ld: lttn=%08lx  <%s>\n", __FUNC__, __LINE__, lttn, lttn ? lttn->tn_Name : ""));
		if (lttn)
			{
			// Select correcponding entry in OBJNDX_MainListTree
			set(inst->fpb_Objects[OBJNDX_MainListTree], MUIA_NListtree_Active, lttn);
			break;
			}

		tn = tn->tn_Parent;
		}

	d1(KPrintF(__FILE__ "/%s/%ld: END\n", __FUNC__, __LINE__));
}

//----------------------------------------------------------------------------

static struct MUI_NListtree_TreeNode *GetRootEntry(struct FileTypesPrefsInst *inst,
	struct MUI_NListtree_TreeNode *ln)
{
	struct MUI_NListtree_TreeNode *lnp;
	struct MUI_NListtree_TreeNode *lnRoot = ln;

	do	{
		lnp = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_NListtree_FileTypes],
			MUIM_NListtree_GetEntry,
			ln,
			MUIV_NListtree_GetEntry_Position_Parent,
			0);
		if (lnp)
			{
			lnRoot = ln;
			ln = lnp;
			}
		} while (lnp);

	d1(KPrintF(__FILE__ "/%s/%ld: lnRoot=%08lx <%s>\n", __FUNC__, __LINE__, lnRoot, lnRoot ? lnRoot->tn_Name : (STRPTR) ""));

	return lnRoot;
}

//----------------------------------------------------------------------------



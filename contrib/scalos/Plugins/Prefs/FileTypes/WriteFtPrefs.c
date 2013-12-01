// WriteFtPrefs.c
// $Date$


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <utility/utility.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <libraries/iffparse.h>
#include <devices/clipboard.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <intuition/intuition.h>
#include <intuition/classusr.h>
#include <graphics/gfxmacros.h>
#include <prefs/prefhdr.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/utility.h>
#include <proto/asl.h>
#include <proto/locale.h>
#define	NO_INLINE_STDARG
#include <proto/muimaster.h>

#include <mui/NListview_mcc.h>
#include <mui/NListtree_mcc.h>

#include <defs.h>

#include "debug.h"
#include "FileTypesPrefs.h"
#include "FileTypesPrefs_proto.h"

//----------------------------------------------------------------------------

static LONG WriteFileTypeDef(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, ULONG Flags);
static LONG WriteChild(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, 
	ULONG Level, ULONG Flags, BPTR fh);
static LONG WriteChildEntry(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, 
	ULONG Level, ULONG Flags,
	BPTR fh, CONST_STRPTR NameStart, CONST_STRPTR NameEnd);
static LONG SaveIcon(struct FileTypesPrefsInst *inst, CONST_STRPTR IconName);

//----------------------------------------------------------------------------

LONG WriteFileTypes(struct FileTypesPrefsInst *inst, CONST_STRPTR SaveName, ULONG Flags)
{
	BPTR OldDir;
	struct MUI_NListtree_TreeNode *tn;
	LONG Result = RETURN_OK;

	inst->fpb_FileTypesDirLock = Lock(SaveName, ACCESS_READ);
	if ((BPTR)NULL == inst->fpb_FileTypesDirLock)
		return IoErr();

	OldDir = CurrentDir(inst->fpb_FileTypesDirLock);

	tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		MUIV_NListtree_GetEntry_ListNode_Root,
		MUIV_NListtree_GetEntry_Position_Head,
		0);
	d1(KPrintF(__FILE__ "/%s/%ld: tn=%08lx\n", __FUNC__, __LINE__, tn));

	while (tn && RETURN_OK == Result)
		{
		d1(kprintf(__FILE__ "/%s/%ld: tn=%08lx\n", __FUNC__, __LINE__, tn));

		Result = WriteFileTypeDef(inst, tn, Flags);

		tn = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry, 
			tn, 
			MUIV_NListtree_GetEntry_Position_Next,
			MUIV_NListtree_GetEntry_Flag_SameLevel);
		}

	CurrentDir(OldDir);

	return Result;
}

//----------------------------------------------------------------------------

static LONG WriteFileTypeDef(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, ULONG Flags)
{
	LONG Result = RETURN_OK;

	do	{
		struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;
		struct MUI_NListtree_TreeNode *tnChild;

		d1(kprintf(__FILE__ "/%s/%ld: fte=%08lx\n", __FUNC__, __LINE__, fte));

		tnChild = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry, 
			tn,
			MUIV_NListtree_GetEntry_Position_Head,
			0);

		d1(kprintf(__FILE__ "/%s/%ld: tnChild=%08lx\n", __FUNC__, __LINE__, tnChild));

		// Only save FileType descriptor if either children or attributes are present
		if (tnChild || !IsListEmpty(&fte->ftle_AttributesList))
			{
			if (!(Flags & FTWRITEFLAG_ONLY_SAVE_CHANGED) || fte->ftle_Changed)
				{
				char DateStr[80];
				struct tm *tm;
				const struct FtAttribute *fta;
				time_t now;
				BPTR fLock;

				fLock = Open(tn->tn_Name, MODE_NEWFILE);

				if ((BPTR)NULL == fLock)
					{
					Result = IoErr();
					break;
					}


				if (EOF == SetFileSize(fLock, 0, OFFSET_BEGINNING))
					{
					Result = IoErr();
					break;
					}

				Seek(fLock, 0, OFFSET_BEGINNING);

				if (EOF == FPuts(fLock, "# Scalos filetype description\n"))
					{
					Result = IoErr();
					break;
					}

				time(&now);
				tm = localtime(&now);
				strftime(DateStr, sizeof(DateStr), "# written %c\n", tm);

				if (EOF == FPuts(fLock, DateStr))
					{
					Result = IoErr();
					break;
					}

				if (EOF == FPuts(fLock, "\n"))
					{
					Result = IoErr();
					break;
					}

				// Write Attributes
				for (fta = (const struct FtAttribute *) fte->ftle_AttributesList.lh_Head;
					RETURN_OK == Result && fta != (const struct FtAttribute *) &fte->ftle_AttributesList.lh_Tail;
					fta = (const struct FtAttribute *) fta->fta_Node.ln_Succ)
					{
					char Line[120];
					char ValueString[120];

					d1(kprintf(__FILE__ "/%s/%ld: fta=%08lx  pred=%08lx  succ=%08lx\n", __FUNC__, __LINE__, \
						fta, fta->fta_Node.ln_Pred, fta->fta_Node.ln_Succ));

					sprintf(Line, "%s \"%s\"\n", GetAttributeName(fta->fta_Type), 
						GetAttributeValueString(fta, ValueString, sizeof(ValueString)));

					if (EOF == FPuts(fLock, Line))
						{
						Result = IoErr();
						break;
						}
					}

				if (EOF == FPuts(fLock, "\n"))
					{
					Result = IoErr();
					break;
					}

				// Write Children
				while (tnChild && RETURN_OK == Result)
					{
					d1(kprintf(__FILE__ "/%s/%ld: tnChild=%08lx <%s>\n", __FUNC__, __LINE__, tnChild, tnChild->tn_Name));

					Result = WriteChild(inst, tnChild, 0, Flags, fLock);

					tnChild = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
						MUIM_NListtree_GetEntry, 
						tnChild,
						MUIV_NListtree_GetEntry_Position_Next,
						0);
					}

				if (!Close(fLock))
					{
					Result = IoErr();
					break;
					}

				if (inst->fpb_fCreateIcons)
					Result = SaveIcon(inst, tn->tn_Name);
				}
			}
		else
			{
			// Filetype description is empty -> try to delete it!
			if (fte->ftle_FileFound)
				{
				char FileName[MAX_FILENAME];

				DeleteFile(tn->tn_Name);

				// Also delete associated icon
				strcpy(FileName, tn->tn_Name);
				strcat(FileName, ".info");
				DeleteFile(FileName);
				}
			}
		} while (0);

	return Result;
}

//----------------------------------------------------------------------------

static LONG WriteChild(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn, 
	ULONG Level, ULONG Flags, BPTR fh)
{
	LONG Result = RETURN_OK;
	const struct FileTypesListEntry *fte = (const struct FileTypesListEntry *) tn->tn_User;

	d1(kprintf(__FILE__ "/%s/%ld: ftle_EntryType=%ld\n", __FUNC__, __LINE__, fte->ftle_EntryType));

	switch (fte->ftle_EntryType)
		{
	case ENTRYTYPE_PopupMenu:
		Result = WriteChildEntry(inst, tn, Level, Flags, fh, "POPUPMENU", "ENDPOPUPMENU\n");
		break;
	case ENTRYTYPE_PopupMenu_SubMenu:
		Result = WriteChildEntry(inst, tn, Level, Flags, fh, "SUBMENU", "ENDSUBMENU");
		break;
	case ENTRYTYPE_PopupMenu_MenuItem:
		Result = WriteChildEntry(inst, tn, Level, Flags, fh, "MENUENTRY", "ENDMENUENTRY");
		break;
	case ENTRYTYPE_PopupMenu_InternalCmd:
		Result = WriteChildEntry(inst, tn, Level, Flags, fh, "INTERNALCMD", NULL);
		break;
	case ENTRYTYPE_PopupMenu_WbCmd:
		Result = WriteChildEntry(inst, tn, Level, Flags, fh, "WBCMD", NULL);
		break;
	case ENTRYTYPE_PopupMenu_ARexxCmd:
		Result = WriteChildEntry(inst, tn, Level, Flags, fh, "AREXXCMD", NULL);
		break;
	case ENTRYTYPE_PopupMenu_CliCmd:
		Result = WriteChildEntry(inst, tn, Level, Flags, fh, "CLICMD", NULL);
		break;
	case ENTRYTYPE_PopupMenu_PluginCmd:
		Result = WriteChildEntry(inst, tn, Level, Flags, fh, "PLUGINCMD", NULL);
		break;
	case ENTRYTYPE_PopupMenu_IconWindowCmd:
		Result = WriteChildEntry(inst, tn, Level, Flags, fh, "ICONWINDOWCMD", NULL);
		break;
	case ENTRYTYPE_PopupMenu_MenuSeparator:
		Result = WriteChildEntry(inst, tn, Level, Flags, fh, "MENUSEPARATOR", NULL);
		break;
	case ENTRYTYPE_ToolTip:
		Result = WriteChildEntry(inst, tn, Level, Flags, fh, "TOOLTIP", "ENDTOOLTIP\n");
		break;
	case ENTRYTYPE_ToolTip_Group:
		Result = WriteChildEntry(inst, tn, Level, Flags, fh, "GROUP", "ENDGROUP");
		break;
	case ENTRYTYPE_ToolTip_Member:
		Result = WriteChildEntry(inst, tn, Level, Flags, fh, "MEMBER", "ENDMEMBER");
		break;
	case ENTRYTYPE_ToolTip_HBar:
		Result = WriteChildEntry(inst, tn, Level, Flags, fh, "HBAR", NULL);
		break;
	case ENTRYTYPE_ToolTip_String:
		Result = WriteChildEntry(inst, tn, Level, Flags, fh, "STRING", NULL);
		break;
	case ENTRYTYPE_ToolTip_Space:
		Result = WriteChildEntry(inst, tn, Level, Flags, fh, "SPACE", NULL);
		break;
	case ENTRYTYPE_ToolTip_DtImage:
		Result = WriteChildEntry(inst, tn, Level, Flags, fh, "DTIMAGE", NULL);
		break;
	default:
		break;
		}

	return Result;
}

//----------------------------------------------------------------------------

static LONG WriteChildEntry(struct FileTypesPrefsInst *inst, struct MUI_NListtree_TreeNode *tn,
	ULONG Level, ULONG Flags,
	BPTR fh, CONST_STRPTR NameStart, CONST_STRPTR NameEnd)
{
	struct FileTypesListEntry *fte = (struct FileTypesListEntry *) tn->tn_User;
	const struct FtAttribute *fta;
	struct MUI_NListtree_TreeNode *tnChild;
	LONG Result = RETURN_OK;
	ULONG n;
	char Line[80];

	for (n = 0; n < Level; n++)
		{
		if (EOF == FPuts(fh, "\t"))
			{
			Result = IoErr();
			break;
			}
		}

	sprintf(Line, "%s", NameStart);
	if (EOF == FPuts(fh, Line))
		Result = IoErr();

	// Write Attributes
	for (fta = (const struct FtAttribute *) fte->ftle_AttributesList.lh_Head;
		RETURN_OK == Result && fta != (const struct FtAttribute *) &fte->ftle_AttributesList.lh_Tail;
		fta = (const struct FtAttribute *) fta->fta_Node.ln_Succ)
		{
		char Line[120];
		char ValueString[120];


		GetAttributeValueString(fta, ValueString, sizeof(ValueString));

		sprintf(Line, " %s", GetAttributeName(fta->fta_Type));
		
		if (EOF == FPuts(fh, Line))
			{
			Result = IoErr();
			break;
			}

		if (strlen(ValueString) > 0)
			{
			sprintf(Line, " \"%s\"", ValueString);
			if (EOF == FPuts(fh, Line))
				{
				Result = IoErr();
				break;
				}
			}
		}
	// End Attribute list
	if (EOF == FPuts(fh, "\n"))
		Result = IoErr();

	tnChild = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
		MUIM_NListtree_GetEntry, 
		tn,
		MUIV_NListtree_GetEntry_Position_Head,
		0);

	// Write Children
	while (tnChild && RETURN_OK == Result)
		{
		d1(kprintf(__FILE__ "/%s/%ld: tnChild=%08lx <%s>\n", __FUNC__, __LINE__, tnChild, tnChild->tn_Name));

		Result = WriteChild(inst, tnChild, Level + 1, Flags, fh);

		tnChild = (struct MUI_NListtree_TreeNode *) DoMethod(inst->fpb_Objects[OBJNDX_MainListTree],
			MUIM_NListtree_GetEntry, 
			tnChild,
			MUIV_NListtree_GetEntry_Position_Next,
			0);
		}

	if (NameEnd)
		{
		for (n = 0; n < Level; n++)
			{
			if (EOF == FPuts(fh, "\t"))
				{
				Result = IoErr();
				break;
				}
			}

		sprintf(Line, "%s\n", NameEnd);

		if (EOF == FPuts(fh, Line))
			Result = IoErr();
		}

	if (RETURN_OK == Result && (Flags & FTWRITEFLAG_CLEAR_CHANGE_FLAG))
		fte->ftle_Changed = FALSE;

	return Result;
}

//----------------------------------------------------------------------------

static LONG SaveIcon(struct FileTypesPrefsInst *inst, CONST_STRPTR IconName)
{
	struct DiskObject *icon, *allocIcon;

	static UBYTE ImageData[] =
		{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
		0x00, 0x00, 0x07, 0xFF, 0x80, 0x00, 0x40, 0x00, 0x00, 0x00, 0x18, 0x00, 0x60, 0x00, 0x10, 0x00,
		0x00, 0x00, 0x20, 0xFC, 0x10, 0x00, 0x08, 0x00, 0x00, 0x00, 0x41, 0x02, 0x08, 0x00, 0x0C, 0x00, 
		0x00, 0x00, 0x40, 0x82, 0x08, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x40, 0x82, 0x08, 0x00, 0x0C, 0x00, 
		0x00, 0x00, 0x21, 0x04, 0x08, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x1E, 0x18, 0x10, 0x00, 0x0C, 0x00,
		0x00, 0x00, 0x00, 0x60, 0x20, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0x00, 0x0C, 0x00, 
		0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x02, 0x1C, 0x00, 0x00, 0x0C, 0x00, 
		0x00, 0x00, 0x01, 0x08, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x0C, 0x00, 
		0x00, 0x00, 0x01, 0x08, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x01, 0x08, 0x00, 0x00, 0x0C, 0x00, 
		0x40, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x0C, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00,
		0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 
		0xD5, 0x55, 0x55, 0x55, 0x55, 0x56, 0x00, 0x00, 0xD5, 0x55, 0x50, 0x00, 0x55, 0x55, 0x80, 0x00, 
		0xD5, 0x55, 0x47, 0xFF, 0x95, 0x55, 0x60, 0x00, 0xD5, 0x55, 0x5F, 0x03, 0xE5, 0x55, 0x50, 0x00, 
		0xD5, 0x55, 0x3E, 0x55, 0xF5, 0x55, 0x50, 0x00, 0xD5, 0x55, 0x3F, 0x55, 0xF5, 0x55, 0x50, 0x00,
		0xD5, 0x55, 0x3F, 0x55, 0xF5, 0x55, 0x50, 0x00, 0xD5, 0x55, 0x5E, 0x53, 0xF5, 0x55, 0x50, 0x00, 
		0xD5, 0x55, 0x41, 0x47, 0xE5, 0x55, 0x50, 0x00, 0xD5, 0x55, 0x55, 0x1F, 0xD5, 0x55, 0x50, 0x00, 
		0xD5, 0x55, 0x55, 0x7F, 0x15, 0x55, 0x50, 0x00, 0xD5, 0x55, 0x54, 0xFC, 0x55, 0x55, 0x50, 0x00, 
		0xD5, 0x55, 0x55, 0xE1, 0x55, 0x55, 0x50, 0x00, 0xD5, 0x55, 0x54, 0xF5, 0x55, 0x55, 0x50, 0x00, 
		0xD5, 0x55, 0x55, 0x05, 0x55, 0x55, 0x50, 0x00, 0xD5, 0x55, 0x54, 0xF5, 0x55, 0x55, 0x50, 0x00,
		0xD5, 0x55, 0x54, 0xF5, 0x55, 0x55, 0x50, 0x00, 0x35, 0x55, 0x55, 0x05, 0x55, 0x55, 0x50, 0x00, 
		0x0D, 0x55, 0x55, 0x55, 0x55, 0x55, 0x50, 0x00, 0x03, 0x55, 0x55, 0x55, 0x55, 0x55, 0x50, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		};

	static struct Image NormalImage =
		{
		0, 0, 54, 23,
		2,
		(UWORD *)ImageData,
		3, 0,
		NULL
		};

	static STRPTR defToolTypes[] =
		{
		"ACTION=USE",
		NULL
		};

	static struct DiskObject DefaultIcon =
		{
		WB_DISKMAGIC, WB_DISKVERSION,

		{
		NULL, 
		0, 0, 54, 24,
		GFLG_GADGIMAGE | GFLG_GADGHBOX,
		GACT_RELVERIFY | GACT_IMMEDIATE,
		GTYP_BOOLGADGET,
		&NormalImage, NULL,
		NULL,
		0,
		NULL,
		0,
		NULL
		},

		WBPROJECT,
		NULL,
		defToolTypes,
		NO_ICON_POSITION, NO_ICON_POSITION,
		NULL,
		NULL,
		8192
		};

	icon = allocIcon = GetDiskObject("ENV:sys/def_ScaFileType");
	if (NULL == icon)
		icon = allocIcon = GetDiskObject("ENV:sys/def_pref");
	if (NULL == icon)
		icon = &DefaultIcon;

	if (icon)
		{
		STRPTR oldDefaultTool = icon->do_DefaultTool;

		icon->do_DefaultTool = (STRPTR) inst->fpb_ProgramName;

		PutDiskObject((STRPTR) IconName, icon);

		icon->do_DefaultTool = oldDefaultTool;

		if (allocIcon)
			FreeDiskObject(allocIcon);
		}

	return RETURN_OK;
}

//----------------------------------------------------------------------------

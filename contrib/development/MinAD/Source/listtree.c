/*--------------------------------------------------------------------------

 minad - MINimalistic AutoDoc

 Copyright (C) 2005-2012 Rupert Hausberger <naTmeg@gmx.net>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

--------------------------------------------------------------------------*/

#include "include.h"
#include "ndk.h"

/* THIS APP IS A MESS! */

/****************************************************************************/

struct AutoDocs autodocs;

static struct AutoDocFunc *AddFunc(struct AutoDocFile *file, UBYTE *name, UBYTE *data, LONG size);

/****************************************************************************/

static LONG seek_nl(UBYTE *s)
{
	UBYTE *p = s;

	while (*p && (*p == '\t' || *p == '\n' || *p == ' ')) p++;
	while (*p && *p != '\n') p++;
	return (p - s);
}

static LONG seek_lf(UBYTE *s)
{
	UBYTE *p = s;

	while (*p && *p != LINEFEED) p++;
	return (p - s);
}

static LONG fix_funcname(UBYTE *funcname, LONG size)
{
	UBYTE *p;
	BOOL again = TRUE;

	/* Trim left */
	while (again) {
		p = funcname;

		again = FALSE;
		if	(*p == '\t' || *p == '\n' || *p == ' ') {
			UBYTE *q = p+1;

			while (*q) *p++ = *q++;
			*p = '\0';
			size--;
			again = TRUE;
		}
	}
	if (size == 0) return 0;

	/* Trim right */
	p = funcname + size - 1;
	while (*p == '\t' || *p == ' ') *p-- = '\0';

	/* Cut at the first ' ' char */
	p = funcname;
	while (*p) {
		if (*p == '\t' || *p == ' ') {
			*p = '\0'; break;
		}
		p++;
	}
	if ((size = p - funcname) <= 38)
		return size;

	/* Workaround for broken files with overlapping names */
	_strcpy(funcname + 38, "(...)");
	return (38 + 5);
}

static BOOL CheckFile(struct AutoDocFile *file)
{
	UBYTE file_name[SIZE_PATH];
	UBYTE *file_data;
	LONG file_size;
	BOOL success = FALSE;

	_strcpy(file_name, autodocs.ad_Path);
	if (!AddPart(file_name, file->adf_FileName, SIZE_PATH))
		return success;

	if ((file_data = ReadFileMagic(file_name, &file_size, "TABLE OF CONTENTS", 17)))
	{
		UBYTE *ptr = file_data;
		LONG ofs = 0;

		while (ptr - file_data < file_size)
		{
			if (*ptr == LINEFEED && *(ptr+1))
			{
				UBYTE func_name[SIZE_NAME];
				LONG rest[2];

				ptr++; ofs++;
				rest[0] = seek_nl(ptr);
				rest[1] = seek_lf(ptr);

				if (rest[0] > 1 && rest[0] < SIZE_NAME && rest[1] > 1 && rest[1] > rest[0])
				{
					UBYTE *func_data;
					LONG func_size = rest[1] - rest[0];

					if ((func_data = (UBYTE *)myAlloc(func_size + 1)))
					{
						_strncpy(func_name, ptr, rest[0]); *(func_name + rest[0]) = '\0';
						fix_funcname(func_name, rest[0]);

						CopyMem(file_data + ofs + rest[0], func_data, func_size); *(func_data + func_size) = '\0';
						AddFunc(file, func_name, func_data, func_size);
					}
				}
				ofs += rest[1];
				ptr += rest[1];
				continue;
			}
			ptr++;
			ofs++;
		}
		if (ofs == file_size)
			success = TRUE;

		myFree(file_data);
	}
	return success;
}

/****************************************************************************/

static void CheckGroup(struct AutoDocFile *file)
{
	UBYTE *ndkname, filenamelower[SIZE_NAME];
	LONG i = 0l;

	LowerString(file->adf_FileNameShort, filenamelower);

	if (!_stricmp(filenamelower, "muimaster")) {
		file->adf_Group = GRP_MUI;
		setbit(autodocs.ad_Flags, GRP_MUI);
		return;
	}
	else if (!_strncmp(filenamelower, "mui_", 4)) {
		file->adf_Group = GRP_MUI;
		setbit(autodocs.ad_Flags, GRP_MUI);
		return;
	}
	else if (!_strncmp(filenamelower, "mcc_", 4)) {
		file->adf_Group = GRP_MCC;
		setbit(autodocs.ad_Flags, GRP_MCC);
		return;
	}
	while ((ndkname = ndk39[i++]))
	{
		if (!_stricmp(filenamelower, ndkname)) {
			file->adf_Group = GRP_NDK;
			setbit(autodocs.ad_Flags, GRP_NDK);
			return;
		}
	}
}

/****************************************************************************/

static void FreeFile(struct AutoDocFile *file)
{
	struct AutoDocFunc *func, *tmp_func;

	ForeachNodeSafe(&file->adf_Funcs, func, tmp_func) {
		if (func->adf_Data) myFree(func->adf_Data);
		if (func->adf_High) myFree(func->adf_High);
		myFree(func);
	}
	myFree(file);
}

static void FreeListtree(void)
{
	struct AutoDocFile *file, *tmp_file;

	ForeachNodeSafe(&autodocs.ad_Files, file, tmp_file)
		FreeFile(file);

	myNewList(&autodocs.ad_Files);
	autodocs.ad_TotalFiles = 0l;
	autodocs.ad_TotalFuncs = 0l;
	autodocs.ad_Flags = 0;
}

/****************************************************************************/

static struct AutoDocFunc *AddFunc(struct AutoDocFile *file, UBYTE *name, UBYTE *data, LONG size)
{
	struct AutoDocFunc *func;

	if ((func = (struct AutoDocFunc *)myAlloc(sizeof(struct AutoDocFunc))))
	{
		_strcpy(func->adf_FuncName, name);
		_strcpy(func->adf_FuncNameShort, name);
		func->adf_Data = data;
		func->adf_High = NULL;
		func->adf_Size = size;
		func->adf_Type = TYP_FUNC;

		myAddTail(&file->adf_Funcs, &func->adf_Node);
		autodocs.ad_TotalFuncs++;
		return func;
	}
	return NULL;
}

/****************************************************************************/

static struct AutoDocFile *AddFile(UBYTE *filename)
{
	struct AutoDocFile *file;
	
	/* If suffix not is '.doc', handle it as trash */
	if (!_strstr(filename, ".doc"))
		return NULL;

	if ((file = (struct AutoDocFile *)myAlloc(sizeof(struct AutoDocFile))))
	{
		int offset = 0;

		while (filename[offset++] != '.'); //cutoff '.doc'

		myNewList(&file->adf_Funcs);
		_strcpy(file->adf_FileName, filename);
		_strncpy(file->adf_FileNameShort, filename, offset - 1); file->adf_FileNameShort[offset - 1] = 0;
		file->adf_ListFlags = TNF_LIST;
		file->adf_Type = TYP_FILE;
		file->adf_Group = GRP_OTHER;

		if (CheckFile(file)) {
			myAddTail(&autodocs.ad_Files, &file->adf_Node);
			autodocs.ad_TotalFiles++;
			return file;
		}
		FreeFile(file);
	}
	return NULL;
}

/****************************************************************************/

static struct AutoDocFile *AddDrawer(UBYTE *drawername)
{
	struct AutoDocFile *file;

	if ((file = (struct AutoDocFile *)myAlloc(sizeof(struct AutoDocFile))))
	{
		myNewList(&file->adf_Funcs); /* init for safe interate at FreeListtree () */
		_strcpy(file->adf_FileName, drawername);
		_strcpy(file->adf_FileNameShort, drawername);
		file->adf_ListFlags = 0;
		file->adf_Type = TYP_DRAWER;

		myAddTail(&autodocs.ad_Files, &file->adf_Node);
		return file;
	}
	return NULL;
}

/****************************************************************************/

static void AddRoot(void)
{
	struct AutoDocFile *file;

	if ((file = (struct AutoDocFile *)myAlloc(sizeof(struct AutoDocFile))))
	{
		myNewList(&file->adf_Funcs); /* init for safe interate at FreeListtree () */
		file->adf_FileName[0] = 0;
		file->adf_FileNameShort[0] = 0;
		file->adf_ListFlags = TNF_LIST | TNF_OPEN;
		file->adf_Type = TYP_ROOT;

		myAddHead(&autodocs.ad_Files, &file->adf_Node);
	} else
		return;

	if ((file = (struct AutoDocFile *)myAlloc(sizeof(struct AutoDocFile))))
	{
		myNewList(&file->adf_Funcs); /* init for safe interate at FreeListtree () */
		file->adf_FileName[0] = 0;
		file->adf_FileNameShort[0] = 0;
		file->adf_ListFlags = 0;
		file->adf_Type = TYP_PARENT;

		myAddTail(&autodocs.ad_Files, &file->adf_Node);
	}
}

/****************************************************************************/

UBYTE *BuildOverview_Group(enum Groups group)
{
	struct AutoDocFile *file;
	UBYTE *buffer;

	if (!(buffer = myAlloc(SIZE_OVERVIEW)))
		return NULL;

	if (group == GRP_MCC)
		_strcpy(buffer, "\n\t\033bGROUP MCC 'Mui Custom Classes'");
	else if (group == GRP_MUI)
		_strcpy(buffer, "\n\t\033bGROUP MUI 'Magic User Interface'");
	else if (group == GRP_NDK)
		_strcpy(buffer, "\n\t\033bGROUP NDK 'Native Developement Kit 3.9'");

	if (isset(autodocs.ad_Flags, GRP_FILTER))
		_strcat(buffer, " [FILTER ENABLED]\n");
	else
		_strcat(buffer, "\n");

	ForeachNode(&autodocs.ad_Files, file)
	{
		if (isset(autodocs.ad_Flags, GRP_FILTER) && isclr(file->adf_Flags, GRP_FILTER))
			continue;

		if ((group == GRP_MCC) && (file->adf_Group != GRP_MCC))
			continue;
		else if ((group == GRP_MUI) && (file->adf_Group != GRP_MUI))
			continue;
		else if ((group == GRP_NDK) && (file->adf_Group != GRP_NDK))
			continue;
		else if (*file->adf_FileNameShort == 0)
			continue;

		_strcat(buffer, "\t\t");
		_strcat(buffer, file->adf_FileNameShort);
		_strcat(buffer, "\n");
	}
	return buffer;
}

UBYTE *BuildOverview_File(struct AutoDocFile *file)
{
	struct AutoDocFunc *func;
	UBYTE *buffer;

	if (!(buffer = myAlloc(SIZE_OVERVIEW)))
		return NULL;

	if (isset(autodocs.ad_Flags, GRP_FILTER))
		_strcpy(buffer, "\n\t\033bTABLE OF CONTENTS [FILTER ENABLED]\n");
	else
		_strcpy(buffer, "\n\t\033bTABLE OF CONTENTS\n");

	ForeachNode(&file->adf_Funcs, func)
	{
		if (isset(autodocs.ad_Flags, GRP_FILTER) && isclr(func->adf_Flags, GRP_FILTER))
			continue;

		_strcat(buffer, "\t\t");
		_strcat(buffer, func->adf_FuncNameShort);
		_strcat(buffer, "\n");
	}
	return buffer;
}

/****************************************************************************/

static void BuildListtree(void)
{
	struct ExAllData *buffer;

	AddRoot();

	if ((buffer = (struct ExAllData *)myAlloc(SIZE_EXALL)))
	{
		BPTR lock;

		if ((lock = Lock(autodocs.ad_Path, ACCESS_READ)))
		{
			struct ExAllControl *eac;

			if ((eac = AllocDosObject(DOS_EXALLCONTROL, NULL)))
			{
				BOOL more = TRUE;

				eac->eac_LastKey = 0;
				while (more)
				{
					struct ExAllData *ead;

					more = ExAll(lock, buffer, SIZE_EXALL, ED_TYPE, eac);
					if ((!more) && (IoErr() != ERROR_NO_MORE_ENTRIES))
						break;

		  			if (eac->eac_Entries == 0)
						continue;

					ead = buffer;
					while (ead)
					{
						if (ead->ed_Type == ST_USERDIR)
							AddDrawer(ead->ed_Name);
						else if (ead->ed_Type == ST_FILE)
							AddFile(ead->ed_Name);
						
						ead = ead->ed_Next;
					}
				}
				FreeDosObject(DOS_EXALLCONTROL, eac);
			}
			UnLock(lock);
		}
		myFree(buffer);
	}

	{
		struct AutoDocFile *file;

		clrbit(autodocs.ad_Flags, GRP_MCC);
		clrbit(autodocs.ad_Flags, GRP_MUI);
		clrbit(autodocs.ad_Flags, GRP_NDK);

		ForeachNode(&autodocs.ad_Files, file)
		{
			if (file->adf_Type != TYP_FILE)
				continue;
			else if (isset(autodocs.ad_Flags, GRP_FILTER) && isclr(file->adf_Flags, GRP_FILTER))
				continue;

			CheckGroup(file);
		}
	}
}

/****************************************************************************/

/* Sort a 'struct List *' */
static void SortByType(enum Types type)
{
	struct List *list = (struct List *)&autodocs.ad_Files;
	struct AutoDocFile *cur, *tmp;
	BOOL again = TRUE;

	while (again) {
		again = FALSE;
		ForeachNodeSafe(list, cur, tmp) {
			struct AutoDocFile *succ;

			if (!(succ = (struct AutoDocFile *)GetSucc(cur)))
				break;

			if ((cur->adf_Type != type) || (succ->adf_Type != type))
				continue;

			if (_stricmp(cur->adf_FileName, succ->adf_FileName) > 0) {
				myRemove(&cur->adf_Node);
				myInsert(list, &cur->adf_Node, &succ->adf_Node);
				again = TRUE;
			}
		}
	}
}

static void SortListtree(void)
{
	struct List *list = (struct List *)&autodocs.ad_Files;

	/* sort if more than (root, parent, one entry) */
	if (ListSize(list) > 3) {
		SortByType(TYP_DRAWER);	/* Sort Drawers */
		SortByType(TYP_FILE);	/* Sort Files */
	}
}

/****************************************************************************/

void ShowListtree(void)
{
	struct MUI_NListtree_TreeNode *root = NULL, *froot = NULL;
	struct MUI_NListtree_TreeNode *ndk = NULL;
	struct MUI_NListtree_TreeNode *mui = NULL;
	struct MUI_NListtree_TreeNode *mcc = NULL;
	struct AutoDocFile *file;

	set(MUI_Obj[MAIN][M_Files], MUIA_NListtree_Quiet, TRUE);
	set(MUI_Obj[MAIN][M_Docs], MUIA_NListtree_Quiet, TRUE);
	DoMethod(MUI_Obj[MAIN][M_Files], MUIM_NListtree_Clear);
	DoMethod(MUI_Obj[MAIN][M_Docs], MUIM_NListtree_Clear);

	/* Add Roots (Drawer Index & AutoDocs Index) */
	file = (struct AutoDocFile *)((struct List *)(&autodocs.ad_Files))->lh_Head;
	file->adf_ListMsg.adm_Type = TYP_ROOT;
	froot = (struct MUI_NListtree_TreeNode *)DoMethod(MUI_Obj[MAIN][M_Files], MUIM_NListtree_Insert, "\033bDrawer Index", &file->adf_ListMsg, MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail, file->adf_ListFlags);
	root = (struct MUI_NListtree_TreeNode *)DoMethod(MUI_Obj[MAIN][M_Docs], MUIM_NListtree_Insert, "\033bAutoDoc Index", &file->adf_ListMsg, MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail, file->adf_ListFlags);

	/* Add Parent */
	if (autodocs.ad_Path[strlen(autodocs.ad_Path) - 1] != ':')
	{
		file = (struct AutoDocFile *)((struct Node *)file)->ln_Succ;
		file->adf_ListMsg.adm_Type = TYP_PARENT;
		DoMethod(MUI_Obj[MAIN][M_Files], MUIM_NListtree_Insert, "..", &file->adf_ListMsg, froot, MUIV_NListtree_Insert_PrevNode_Tail, 0);
	}
	/* Add Drawers */
	ForeachNode(&autodocs.ad_Files, file)
	{
		if (file->adf_Type == TYP_DRAWER) {
			file->adf_ListMsg.adm_File = file;
			file->adf_ListMsg.adm_Type = TYP_DRAWER;
			DoMethod(MUI_Obj[MAIN][M_Files], MUIM_NListtree_Insert, file->adf_FileName, &file->adf_ListMsg, froot, MUIV_NListtree_Insert_PrevNode_Tail, 0);
		}
	}

	/* Add Groups */
	if (autodocs.ad_Mode == MODE_AUTOGROUP)
	{
		if (isset(autodocs.ad_Flags, GRP_MCC)) {
			static struct AutoDocMessage msg;
			msg.adm_Type = TYP_GROUP;
			msg.adm_Group = GRP_MCC;
			mcc = (struct MUI_NListtree_TreeNode *)DoMethod(MUI_Obj[MAIN][M_Docs], MUIM_NListtree_Insert, "\033bMCC", &msg, root, MUIV_NListtree_Insert_PrevNode_Tail, TNF_LIST);
		}
		if (isset(autodocs.ad_Flags, GRP_MUI)) {
			static struct AutoDocMessage msg;
			msg.adm_Type = TYP_GROUP;
			msg.adm_Group = GRP_MUI;
			mui = (struct MUI_NListtree_TreeNode *)DoMethod(MUI_Obj[MAIN][M_Docs], MUIM_NListtree_Insert, "\033bMUI", &msg, root, MUIV_NListtree_Insert_PrevNode_Tail, TNF_LIST);
		}
		if (isset(autodocs.ad_Flags, GRP_NDK)) {
			static struct AutoDocMessage msg;
			msg.adm_Type = TYP_GROUP;
			msg.adm_Group = GRP_NDK;
			ndk = (struct MUI_NListtree_TreeNode *)DoMethod(MUI_Obj[MAIN][M_Docs], MUIM_NListtree_Insert, "\033bNDK", &msg, root, MUIV_NListtree_Insert_PrevNode_Tail, TNF_LIST);
		}
	}

	/* Add Files */
	ForeachNode(&autodocs.ad_Files, file)
	{
		if (file->adf_Type == TYP_FILE)
		{
			struct MUI_NListtree_TreeNode *group = root;
			struct MUI_NListtree_TreeNode *node1, *node2;

			if (autodocs.ad_Mode == MODE_AUTOGROUP)
			{
				if (file->adf_Group == GRP_NDK)
					group = ndk;
				else if (file->adf_Group == GRP_MUI)
					group = mui;
				else if (file->adf_Group == GRP_MCC)
					group = mcc;
			}
			if (isset(autodocs.ad_Flags, GRP_FILTER) && isclr(file->adf_Flags, GRP_FILTER))
				continue;

			file->adf_ListMsg.adm_File = file;
			file->adf_ListMsg.adm_Type = TYP_FILE;
			node1 = (struct MUI_NListtree_TreeNode *)DoMethod(MUI_Obj[MAIN][M_Docs], MUIM_NListtree_Insert, file->adf_FileNameShort, &file->adf_ListMsg, group, MUIV_NListtree_Insert_PrevNode_Tail, file->adf_ListFlags);
			file->adf_ListMsg.adm_Node = node1;

			if (node1)
			{
				struct AutoDocFunc *func;

				/* Add Funcs */
				ForeachNode(&file->adf_Funcs, func)
				{
					if (isset(autodocs.ad_Flags, GRP_FILTER) && isclr(func->adf_Flags, GRP_FILTER))
						continue;
					
					func->adf_ListMsg.adm_Func = func;
					func->adf_ListMsg.adm_Type = TYP_FUNC;
					node2 = (struct MUI_NListtree_TreeNode *)DoMethod(MUI_Obj[MAIN][M_Docs], MUIM_NListtree_Insert, func->adf_FuncNameShort, &func->adf_ListMsg, node1, MUIV_NListtree_Insert_PrevNode_Tail, 0);
					func->adf_ListMsg.adm_Node = node2;
				}
			}
		}
	}

	{
		static UBYTE buffer[128];
		
		SPrintf(buffer,	"\n"
								"\t\033bCURRENT PATH\n"
								"\n"
								"\t\t%ld Functions in %ld Files", autodocs.ad_TotalFuncs, autodocs.ad_TotalFiles);
		PrintfText(buffer);
	}

	set(MUI_Obj[MAIN][M_Files], MUIA_NListtree_Quiet, FALSE);
	set(MUI_Obj[MAIN][M_Docs], MUIA_NListtree_Quiet, FALSE);
}

/****************************************************************************/

#ifdef __MORPHOS__
	#pragma pack(2)
#endif

static struct {
	UBYTE *name;
	ULONG len;
} keywords[] = { //31 Keywords, descending order
	{ "WARNING" },
	{ "TAGS" },
	{ "SYNOPSIS" },
	{ "SUPERCLASS" },
	{ "SPECIAL INPUTS" },
	{ "SEE ALSO" },
	{ "RETURNS" },
	{ "RESULT" },
	{ "REQUIRES" },
	{ "PURPOSE" },
	{ "OVERVIEW" },
	{ "OUTPUTS" },
	{ "NOTES" },
	{ "NOTE" },
	{ "NAME" },
	{ "METHODS" },
	{ "IO REQUEST RESULT" },
	{ "IO REQUEST OUTPUT" },
	{ "IO REQUEST INPUT" },
	{ "IO REQUEST" },
	{ "IMPLEMENTATION" },
	{ "INPUTS" },
	{ "INPUT" },
	{ "HOOK ENVIRONMENT" },
	{ "FUNCTION" },
	{ "FILES" },
	{ "EXAMPLE" },
	{ "DESCRIPTION" },
	{ "CAVEATS" },
	{ "BUGS" },
	{ "ATTRIBUTES" },
	{ NULL }
};
static BOOL keywords_init = FALSE;

#ifdef __MORPHOS__
	#pragma pack()
#endif

static LONG Highlight(UBYTE *data, LONG size, UBYTE *high)
{
	UBYTE *d = data;
	UBYTE *h = high;

	if (!keywords_init) {
		LONG i = 0l;

		while (keywords[i].name) {
			keywords[i].len = _strlen(keywords[i].name);
			i++;
		}
		keywords_init = TRUE;
	}

	while (d - data < size)
	{
		LONG i = 0l;
		BOOL found = FALSE;

		while (keywords[i].name)
		{
			ULONG len = keywords[i].len;

			if (_memcmp(d, keywords[i].name, len) == 0) {
				CopyMem("\033b", h, 2); h += 2;
				CopyMem(keywords[i].name, h, len); h += len;
				CopyMem("\033n", h, 2); h += 2;
				d += len;
				found = TRUE;
				break;
			}
			i++;
		}
		if (found)
			continue;

		*h++ = *d++;
	}
	*h = '\0';

	return (h - high);
}

/****************************************************************************/

void DirRide(void)
{
	struct MUI_NListtree_TreeNode *node = (struct MUI_NListtree_TreeNode *)
	DoMethod(MUI_Obj[MAIN][M_Files], MUIM_NListtree_GetEntry, node, MUIV_NListtree_GetEntry_ListNode_Active, 0);

	if (node)
	{
		struct AutoDocMessage *msg = (struct AutoDocMessage *)node->tn_User;

		if (msg)
		{
			switch (msg->adm_Type)
			{
				case TYP_ROOT:
				{
					static UBYTE buffer[128];

					SPrintf(buffer,	"\n"
											"\t\033bCURRENT PATH\n"
											"\n"
											"\t\t%ld Functions in %ld Files", autodocs.ad_TotalFuncs, autodocs.ad_TotalFiles);
					PrintfText(buffer);
					break;
				}
				case TYP_PARENT:
				{
					RemPart(autodocs.ad_Path);

					set(MUI_Obj[MAIN][M_Path], MUIA_String_Contents, autodocs.ad_Path);
					UpdateListtree();
					break;
				}
				case TYP_DRAWER:
				{
					struct AutoDocFile *file = msg->adm_File;

					if (AddPart(autodocs.ad_Path, file->adf_FileNameShort, SIZE_PATH))
					{
						set(MUI_Obj[MAIN][M_Path], MUIA_String_Contents, autodocs.ad_Path);
						UpdateListtree();
					}
					break;
				}
				default:
				{
					if (ListSize((struct List *)&autodocs.ad_Files) > 2)
						PrintfText(NULL);
					break;
				}
			}
		} else
			PrintfText(NULL);
	} else
		PrintfText(NULL);
}

/****************************************************************************/

void ShowFunc(void)
{
	struct MUI_NListtree_TreeNode *node = (struct MUI_NListtree_TreeNode *)
	DoMethod(MUI_Obj[MAIN][M_Docs], MUIM_NListtree_GetEntry, node, MUIV_NListtree_GetEntry_ListNode_Active, 0);

	if (node)
	{
		struct AutoDocMessage *msg = (struct AutoDocMessage *)node->tn_User;

		if (msg)
		{
			switch (msg->adm_Type)
			{
				case TYP_ROOT:
				{
					static UBYTE buffer[128];

					SPrintf(buffer,	"\n"
											"\t\033bCURRENT PATH\n"
											"\n"
											"\t\t%ld Functions in %ld Files", autodocs.ad_TotalFuncs, autodocs.ad_TotalFiles);
					PrintfText(buffer);
					break;
				}
				case TYP_GROUP:
				{
					UBYTE *buffer;

					if ((buffer = BuildOverview_Group(msg->adm_Group))) {
						PrintfText(buffer);
						myFree(buffer);
					} else
						PrintfText("ERROR: not enough memory");

					break;
				}
				case TYP_FILE:
				{
					UBYTE *buffer;

					if ((buffer = BuildOverview_File(msg->adm_File))) {
						PrintfText(buffer);
						myFree(buffer);
					} else
						PrintfText("ERROR: not enough memory");

					break;
				}
				case TYP_FUNC:
				{
					struct AutoDocFunc *func = msg->adm_Func;

					/* Highlight keywords */
					if (!func->adf_High) {
						if ((func->adf_High = (UBYTE *)myAlloc(func->adf_Size + FUNCSTRFIX))) {
							Highlight(func->adf_Data, func->adf_Size, func->adf_High);
							myFree(func->adf_Data); func->adf_Data = NULL;
						}
					}
					if (func->adf_High)
						PrintfText(func->adf_High);
					else
						PrintfText("ERROR: not enough memory");

					break;
				}
				default:
				{
					if (ListSize((struct List *)&autodocs.ad_Files) > 2)
						PrintfText(NULL);
					break;
				}
			}
		} else
			PrintfText(NULL);
	} else
		PrintfText(NULL);
}

/****************************************************************************/

void InitListtree(void)
{
	struct AutoDocsPrefs	prefs;
	BPTR fh;

	/* Load prefs */
	if ((fh = Open(PREFSFILE, MODE_OLDFILE)))
	{
		Read(fh, &prefs, sizeof(struct AutoDocsPrefs));
		_strcpy(autodocs.ad_Path, prefs.adp_Path);
		//autodocs.ad_Mode = prefs.adp_Mode;
		Close(fh);
	}
	/* else Init defaults */
	else
	{
		UBYTE realpath[1024];

		/* Try "AutoDocs:" assign */
		if (AssignAvail(DEFAULTPATH1, realpath)) {
			_strcpy(autodocs.ad_Path, realpath);
		} else {
			_strcpy(autodocs.ad_Path, DEFAULTPATH2);
		}

		//autodocs.ad_Mode = MODE_AUTOGROUP;
	}
	set(MUI_Obj[MAIN][M_Path], MUIA_String_Contents, autodocs.ad_Path);
	//set(MUI_Obj[MAIN][M_Mode], MUIA_Cycle_Active, (LONG)autodocs.ad_Mode);
	autodocs.ad_Mode = MODE_AUTOGROUP;

	myNewList(&autodocs.ad_Files);
	BuildListtree();
	SortListtree();
	ShowListtree();
}

void ExitListtree(void)
{
	struct AutoDocsPrefs	prefs;
	BPTR fh;

	/* Save prefs */
	if ((fh = Open(PREFSFILE, MODE_NEWFILE)))
	{
		_strcpy(prefs.adp_Path, autodocs.ad_Path);
		prefs.adp_Mode = autodocs.ad_Mode;
		Write(fh, &prefs, sizeof(struct AutoDocsPrefs));
		Close(fh);
	}
}

/* Update after ASL close, (mainly called from ASL CloseHook) */
void UpdateListtree(void)
{
	FreeListtree();
	BuildListtree();
	SortListtree();
	ShowListtree();
	Filter();
}

/****************************************************************************/

void Search(void)
{
	struct AutoDocFile *file;
	UBYTE *pattern;

	DoMethod(MUI_Obj[SEARCH][S_List], MUIM_NList_Clear);
	get(MUI_Obj[SEARCH][S_String], MUIA_String_Contents, &pattern);
	if (*pattern == 0) return;

	set(MUI_Obj[SEARCH][S_List], MUIA_NList_Quiet, TRUE);
	ForeachNode(&autodocs.ad_Files, file)
	{
		if (file->adf_Type == TYP_FILE)
		{
			struct AutoDocFunc *func;

			ForeachNode(&file->adf_Funcs, func)
			{
				UBYTE lc_func[SIZE_NAME], lc_patt[SIZE_NAME];
				
				if (_strstr(LowerString(func->adf_FuncNameShort, lc_func),
								LowerString(pattern, lc_patt)))
				{
					func->adf_ListMsg.adm_File = file;
					func->adf_ListMsg.adm_Func = func;
					DoMethod(MUI_Obj[SEARCH][S_List], MUIM_NList_InsertSingle, &func->adf_ListMsg, MUIV_NList_Insert_Bottom);
				}
			}
		}
	}
	set(MUI_Obj[SEARCH][S_List], MUIA_NList_Quiet, FALSE);
}

/****************************************************************************/

void SelectFromSearch(void)
{
	struct AutoDocMessage *msg;

	set(MUI_Obj[SEARCH][S_List], MUIA_NList_Quiet, TRUE);
	DoMethod(MUI_Obj[SEARCH][S_List], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &msg);
	
	if (msg && msg->adm_Node) {
		DoMethod(MUI_Obj[MAIN][M_Docs], MUIM_NListtree_Open, MUIV_NListtree_Open_ListNode_Parent, msg->adm_Node, 0);
		set(MUI_Obj[MAIN][M_Docs], MUIA_NListtree_Active, msg->adm_Node);
	}
	set(MUI_Obj[SEARCH][S_List], MUIA_NList_Quiet, FALSE);
}

/****************************************************************************/

void Filter(void)
{
	struct AutoDocFile *file;
	LONG files = 0, funcs = 0l;
	UBYTE *filter;

	get(MUI_Obj[MAIN][M_Filter], MUIA_String_Contents, &filter);
	if (*filter == 0) {
		set(MUI_Win[MAIN], MUIA_Window_Title, APP_PROGNAME" � Main");
		clrbit(autodocs.ad_Flags, GRP_FILTER);
		ShowListtree(); return;
	}

	ForeachNode(&autodocs.ad_Files, file)
	{
		if (file->adf_Type == TYP_FILE)
		{
			struct AutoDocFunc *func;

			clrbit(file->adf_Flags, GRP_FILTER);

			ForeachNode(&file->adf_Funcs, func)
			{
				UBYTE lc_func[SIZE_NAME], lc_patt[SIZE_NAME];

				if (_strstr(LowerString(func->adf_FuncNameShort, lc_func),
								LowerString(filter, lc_patt)))
				{
					setbit(file->adf_Flags, GRP_FILTER);
					setbit(func->adf_Flags, GRP_FILTER);
					funcs++;
				} else
					clrbit(func->adf_Flags, GRP_FILTER);
			}
			if (isset(file->adf_Flags, GRP_FILTER))
				files++;
		}
	}

	{
		static UBYTE title[128];

		SPrintf(title, APP_PROGNAME" � Main � [Filter showing %ld/%ld Funcs in %ld/%ld Files]",
			funcs, autodocs.ad_TotalFuncs, files, autodocs.ad_TotalFiles);
		set(MUI_Win[MAIN], MUIA_Window_Title, title);
	}
	setbit(autodocs.ad_Flags, GRP_FILTER);
	ShowListtree();
}

void FilterClear(void)
{
	set(MUI_Obj[MAIN][M_Filter], MUIA_String_Contents, NULL);

	set(MUI_Win[MAIN], MUIA_Window_Title, APP_PROGNAME" � Main");
	clrbit(autodocs.ad_Flags, GRP_FILTER);
	ShowListtree();
}

/****************************************************************************/

void OpenCloseAll(BOOL mode)
{
	set(MUI_Obj[MAIN][M_Docs], MUIA_NListtree_Quiet, TRUE);
	if (mode)
		DoMethod(MUI_Obj[MAIN][M_Docs], MUIM_NListtree_Open, MUIV_NListtree_Open_ListNode_Root, MUIV_NListtree_Open_TreeNode_All, 0);
	else {
		DoMethod(MUI_Obj[MAIN][M_Docs], MUIM_NListtree_Close, MUIV_NListtree_Close_ListNode_Root, MUIV_NListtree_Close_TreeNode_All, 0);
		set(MUI_Obj[MAIN][M_Docs], MUIA_NListtree_Active, MUIV_NListtree_Active_First);
		DoMethod(MUI_Obj[MAIN][M_Docs], MUIM_NListtree_Open, MUIV_NListtree_Open_ListNode_Active, MUIV_NListtree_Open_TreeNode_Active, 0);
		set(MUI_Obj[MAIN][M_Docs], MUIA_NListtree_Active, MUIV_NListtree_Active_Off);
	}
	set(MUI_Obj[MAIN][M_Docs], MUIA_NListtree_Quiet, FALSE);
}

/****************************************************************************/

void mTools_MakeGuide(void)
{
	struct MUI_NListtree_TreeNode *node = (struct MUI_NListtree_TreeNode *)
	DoMethod(MUI_Obj[MAIN][M_Docs], MUIM_NListtree_GetEntry, node, MUIV_NListtree_GetEntry_ListNode_Active, 0);

	if (node)
	{
		struct AutoDocMessage *msg = (struct AutoDocMessage *)node->tn_User;

		if (msg)
		{
			ErrorMsg("- not implemented -");
		}
	}
}

/****************************************************************************/


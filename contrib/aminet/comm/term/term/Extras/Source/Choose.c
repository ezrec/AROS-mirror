/*
**	Choose.c
**
**	Routines that call the application support library (ASL).
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

STATIC LONG
GetDirEntryType(STRPTR Name)
{
	APTR OldWindowPtr;
	LONG Result = 0;
	BPTR FileLock;

	DisableDOSRequesters(&OldWindowPtr);

	if(FileLock = Lock(Name,SHARED_LOCK))
	{
		D_S(struct FileInfoBlock,FileInfo);

		if(Examine(FileLock,FileInfo))
			Result = FileInfo->fib_DirEntryType;

		UnLock(FileLock);
	}

	EnableDOSRequesters(OldWindowPtr);

	return(Result);
}

STATIC struct FileRequester *
SelectDrawerCommon(struct Window *Parent,CONST_STRPTR TitleText,CONST_STRPTR PositiveText,STRPTR DrawerName,LONG DrawerNameSize,BOOL SaveMode)
{
	struct FileRequester *DrawerRequester;
	struct TagItem LocalTags[4+1+1],*TagPtr;
	UBYTE LocalDrawerName[MAX_FILENAME_LENGTH];

	if(DrawerName[0])
		LimitedStrcpy(sizeof(LocalDrawerName),LocalDrawerName,DrawerName);
	else
	{
		if(!LocalGetCurrentDirName(LocalDrawerName,sizeof(LocalDrawerName)))
			LocalDrawerName[0] = 0;
	}

	GetDimensionTags(Parent,LocalTags);

	TagPtr = &LocalTags[4];

	if(TitleText)
	{
		TagPtr->ti_Tag	= ASLFR_TitleText;
		TagPtr->ti_Data	= (IPTR)TitleText;

		TagPtr++;
	}

	TagPtr->ti_Tag = TAG_DONE;

	if(!PositiveText)
		PositiveText = LocaleString(MSG_GLOBAL_OPEN_TXT);

	if(DrawerRequester = (struct FileRequester *)AllocAslRequestTags(ASL_FileRequest,
		ASLFR_Window,			Parent,
		ASLFR_InitialDrawer,	LocalDrawerName,
		ASLFR_PositiveText,		PositiveText,
		ASLFR_Flags1,			SaveMode ? (FILF_SAVE | FILF_NEWIDCMP) : FILF_NEWIDCMP,
		ASLFR_Flags2,			FIL1F_NOFILES,
	TAG_MORE,LocalTags))
	{
		BOOL GotIt = FALSE;

		LT_LockWindow(Parent);

		if(AslRequest(DrawerRequester,NULL))
		{
			PutDimensionTags(NULL,DrawerRequester->fr_LeftEdge,DrawerRequester->fr_TopEdge,DrawerRequester->fr_Width,DrawerRequester->fr_Height);

			if(DrawerRequester->fr_Drawer[0])
			{
				LimitedStrcpy(DrawerNameSize,DrawerName,DrawerRequester->fr_Drawer);
				GotIt = TRUE;
			}
		}

		if(!GotIt)
		{
			FreeAslRequest(DrawerRequester);
			DrawerRequester = NULL;
		}

		LT_UnlockWindow(Parent);
	}

	return(DrawerRequester);
}

struct FileRequester *
SaveDrawer(struct Window *Parent,CONST_STRPTR TitleText,CONST_STRPTR PositiveText,STRPTR DrawerName,LONG DrawerNameSize)
{
	return(SelectDrawerCommon(Parent,TitleText,PositiveText,DrawerName,DrawerNameSize,TRUE));
}

struct FileRequester *
OpenDrawer(struct Window *Parent,CONST_STRPTR TitleText,CONST_STRPTR PositiveText,STRPTR DrawerName,LONG DrawerNameSize)
{
	return(SelectDrawerCommon(Parent,TitleText,PositiveText,DrawerName,DrawerNameSize,FALSE));
}

struct FileRequester *
SaveFile(struct Window *Parent,CONST_STRPTR TitleText,CONST_STRPTR PositiveText,STRPTR Pattern,STRPTR FullName,LONG FullNameSize)
{
	UBYTE LocalDrawerName[MAX_FILENAME_LENGTH];
	struct TagItem LocalTags[4+2+1],*TagPtr;
	struct FileRequester *FileRequester;
	STRPTR DrawerName,FileName;
	UBYTE Separator;
	ULONG Flags;

	Flags = FILF_SAVE | FILF_NEWIDCMP;

	GetDimensionTags(Parent,LocalTags);

	TagPtr = &LocalTags[4];

	if(Pattern)
	{
		Flags |= FRF_DOPATTERNS;

		TagPtr->ti_Tag	= ASLFR_InitialPattern;
		TagPtr->ti_Data	= (IPTR)Pattern;

		TagPtr++;
	}

	if(TitleText)
	{
		TagPtr->ti_Tag	= ASLFR_TitleText;
		TagPtr->ti_Data	= (IPTR)TitleText;

		TagPtr++;
	}

	TagPtr->ti_Tag = TAG_DONE;

	if(!PositiveText)
		PositiveText = LocaleString(MSG_GLOBAL_SAVE_TXT);

	if(GetDirEntryType(FullName) > 0)
	{
		FileName	= "";
		DrawerName	= FullName;
		Separator	= 0;
	}
	else
	{
		FileName	= FilePart(FullName);
		DrawerName	= PathPart(FullName);
		Separator	= *DrawerName;
		*DrawerName	= 0;
	}

	if(FullName[0])
		LimitedStrcpy(sizeof(LocalDrawerName),LocalDrawerName,FullName);
	else
	{
		if(!LocalGetCurrentDirName(LocalDrawerName,sizeof(LocalDrawerName)))
			LocalDrawerName[0] = 0;
	}

	if(Separator)
		*DrawerName = Separator;

	FileRequester = (struct FileRequester *)AllocAslRequestTags(ASL_FileRequest,
		ASLFR_Window,			Parent,
		ASLFR_InitialDrawer,	LocalDrawerName,
		ASLFR_InitialFile,		FileName,
		ASLFR_PositiveText,		PositiveText,
		ASLFR_Flags1,			Flags,
	TAG_MORE,LocalTags);

	if(FileRequester)
	{
		BOOL GotIt = FALSE;

		LT_LockWindow(Parent);

		if(AslRequest(FileRequester,NULL))
		{
			PutDimensionTags(NULL,FileRequester->fr_LeftEdge,FileRequester->fr_TopEdge,FileRequester->fr_Width,FileRequester->fr_Height);

			if(FileRequester->fr_File[0] && strlen(FileRequester->fr_Drawer) < FullNameSize)
			{
				LimitedStrcpy(FullNameSize,FullName,FileRequester->fr_Drawer);

				if(AddPart(FullName,FileRequester->fr_File,FullNameSize))
					GotIt = TRUE;
			}
		}

		if(GotIt && Config->MiscConfig->ProtectiveMode)
		{
			BPTR FileLock;
			BOOL DoesExist;

			if(FileLock = Lock(FullName,SHARED_LOCK))
			{
				DoesExist = TRUE;

				UnLock(FileLock);
			}
			else
				DoesExist = (BOOL)(IoErr() == ERROR_OBJECT_IN_USE);

			if(DoesExist)
			{
				if(!ShowRequest(Parent,LocaleString(MSG_GLOBAL_FILE_ALREADY_EXISTS_OVERWRITE_TXT),LocaleString(MSG_GLOBAL_REPLACE_CANCEL_TXT),FilePart(FullName)))
					GotIt = FALSE;
			}
		}

		if(!GotIt)
		{
			FreeAslRequest(FileRequester);
			FileRequester = NULL;
		}

		LT_UnlockWindow(Parent);
	}

	return(FileRequester);
}

struct FileRequester *
OpenSingleFile(struct Window *Parent,CONST_STRPTR TitleText,CONST_STRPTR PositiveText,CONST_STRPTR Pattern,STRPTR FullName,LONG FullNameSize)
{
	UBYTE LocalDrawerName[MAX_FILENAME_LENGTH];
	struct TagItem LocalTags[4+2+1],*TagPtr;
	struct FileRequester *FileRequester;
	STRPTR DrawerName,FileName;
	UBYTE Separator;
	ULONG Flags;

	Flags = FILF_NEWIDCMP;

	GetDimensionTags(Parent,LocalTags);

	TagPtr = &LocalTags[4];

	if(Pattern)
	{
		Flags |= FRF_DOPATTERNS;

		TagPtr->ti_Tag	= ASLFR_InitialPattern;
		TagPtr->ti_Data	= (IPTR)Pattern;

		TagPtr++;
	}

	if(TitleText)
	{
		TagPtr->ti_Tag	= ASLFR_TitleText;
		TagPtr->ti_Data	= (IPTR)TitleText;

		TagPtr++;
	}

	TagPtr->ti_Tag = TAG_DONE;

	if(!PositiveText)
		PositiveText = LocaleString(MSG_GLOBAL_OPEN_TXT);

	if(GetDirEntryType(FullName) > 0)
	{
		FileName	= "";
		DrawerName	= FullName;
		Separator	= 0;
	}
	else
	{
		FileName	= FilePart(FullName);
		DrawerName	= PathPart(FullName);
		Separator	= *DrawerName;
		*DrawerName	= 0;
	}

	if(FullName[0])
		LimitedStrcpy(sizeof(LocalDrawerName),LocalDrawerName,FullName);
	else
	{
		if(!LocalGetCurrentDirName(LocalDrawerName,sizeof(LocalDrawerName)))
			LocalDrawerName[0] = 0;
	}

	if(Separator)
		*DrawerName = Separator;

	FileRequester = (struct FileRequester *)AllocAslRequestTags(ASL_FileRequest,
		ASLFR_Window,			Parent,
		ASLFR_InitialDrawer,	LocalDrawerName,
		ASLFR_InitialFile,		FileName,
		ASLFR_PositiveText,		PositiveText,
		ASLFR_Flags1,			Flags,
	TAG_MORE,LocalTags);

	if(FileRequester)
	{
		BOOL GotIt = FALSE;

		LT_LockWindow(Parent);

		if(AslRequest(FileRequester,NULL))
		{
			PutDimensionTags(NULL,FileRequester->fr_LeftEdge,FileRequester->fr_TopEdge,FileRequester->fr_Width,FileRequester->fr_Height);

			if(FileRequester->fr_File[0] && strlen(FileRequester->fr_Drawer) < FullNameSize)
			{
				LimitedStrcpy(FullNameSize,FullName,FileRequester->fr_Drawer);

				if(AddPart(FullName,FileRequester->fr_File,FullNameSize))
					GotIt = TRUE;
			}
		}

		if(!GotIt)
		{
			FreeAslRequest(FileRequester);
			FileRequester = NULL;
		}

		LT_UnlockWindow(Parent);
	}

	return(FileRequester);
}

struct FileRequester *
OpenSeveralFiles(struct Window *Parent,CONST_STRPTR TitleText,CONST_STRPTR PositiveText,CONST_STRPTR Pattern,STRPTR FullName,LONG FullNameSize)
{
	UBYTE LocalDrawerName[MAX_FILENAME_LENGTH];
	struct TagItem LocalTags[4+3+1],*TagPtr;
	struct FileRequester *FileRequester;
	STRPTR DrawerName,FileName;
	UBYTE Separator;
	ULONG Flags;

	Flags = FILF_MULTISELECT | FILF_NEWIDCMP;

	GetDimensionTags(Parent,LocalTags);

	TagPtr = &LocalTags[4];

	if(Pattern)
	{
		Flags |= FRF_DOPATTERNS;

		TagPtr->ti_Tag	= ASLFR_InitialPattern;
		TagPtr->ti_Data	= (IPTR)Pattern;

		TagPtr++;
	}

	if(TitleText)
	{
		TagPtr->ti_Tag	= ASLFR_TitleText;
		TagPtr->ti_Data	= (IPTR)TitleText;

		TagPtr++;
	}

	if(PositiveText)
	{
		TagPtr->ti_Tag	= ASLFR_PositiveText;
		TagPtr->ti_Data	= (IPTR)PositiveText;

		TagPtr++;
	}

	TagPtr->ti_Tag = TAG_DONE;

	if(GetDirEntryType(FullName) > 0)
	{
		FileName	= "";
		DrawerName	= FullName;
		Separator	= 0;
	}
	else
	{
		FileName	= FilePart(FullName);
		DrawerName	= PathPart(FullName);
		Separator	= *DrawerName;
		*DrawerName	= 0;
	}

	if(FullName[0])
		LimitedStrcpy(sizeof(LocalDrawerName),LocalDrawerName,FullName);
	else
	{
		if(!LocalGetCurrentDirName(LocalDrawerName,sizeof(LocalDrawerName)))
			LocalDrawerName[0] = 0;
	}

	if(Separator)
		*DrawerName = Separator;

	FileRequester = (struct FileRequester *)AllocAslRequestTags(ASL_FileRequest,
		ASLFR_Window,			Parent,
		ASLFR_InitialDrawer,	LocalDrawerName,
		ASLFR_InitialFile,		FileName,
		ASLFR_Flags1,			Flags,
	TAG_MORE,LocalTags);

	if(FileRequester)
	{
		BOOL GotIt = FALSE;

		LT_LockWindow(Parent);

		if(AslRequest(FileRequester,NULL))
		{
			PutDimensionTags(NULL,FileRequester->fr_LeftEdge,FileRequester->fr_TopEdge,FileRequester->fr_Width,FileRequester->fr_Height);

			if(strlen(FileRequester->fr_Drawer) < FullNameSize)
			{
				STRPTR LocalFileName;

				LimitedStrcpy(FullNameSize,FullName,FileRequester->fr_Drawer);

				if(FileRequester->fr_NumArgs > 1 && FileRequester->fr_ArgList != NULL)
					LocalFileName = FileRequester->fr_ArgList->wa_Name;
				else
					LocalFileName = FileRequester->fr_File;

				if(LocalFileName[0])
				{
					if(AddPart(FullName,LocalFileName,FullNameSize))
						GotIt = TRUE;
				}
			}
		}

		if(!GotIt)
		{
			FreeAslRequest(FileRequester);
			FileRequester = NULL;
		}

		LT_UnlockWindow(Parent);
	}

	return(FileRequester);
}

STATIC BOOL
OpenFontCommon(struct Window *Parent,STRPTR FontName,WORD *FontSize,BOOL MonoSpaced)
{
	struct TagItem			 DimensionTags[5];
	struct FontRequester	*FontRequester;
	BOOL					 Result = FALSE;

	if(FontRequester = (struct FontRequester *)AllocAslRequestTags(ASL_FontRequest,
		ASLFO_Window,			Parent,
		ASLFO_InitialName,		FontName,
		ASLFO_InitialSize,		*FontSize,
		ASLFO_InitialFrontPen,	Pens[TEXTPEN],
		ASLFO_InitialBackPen,	Pens[BACKGROUNDPEN],
		ASLFO_PrivateIDCMP,		TRUE,
		ASLFO_MaxHeight,		255,
		ASL_FuncFlags,			MonoSpaced ? (FONF_NEWIDCMP|FONF_FIXEDWIDTH) : FONF_NEWIDCMP,
	TAG_MORE,GetDimensionTags(NULL,DimensionTags)))
	{
		LT_LockWindow(Parent);

		if(AslRequest(FontRequester,NULL))
		{
			PutDimensionTags(NULL,FontRequester->fo_LeftEdge,FontRequester->fo_TopEdge,FontRequester->fo_Width,FontRequester->fo_Height);

			strcpy(FontName,FontRequester->fo_Attr.ta_Name);

			*FontSize = FontRequester->fo_Attr.ta_YSize;

			Result = TRUE;
		}

		LT_UnlockWindow(Parent);

		FreeAslRequest(FontRequester);
	}

	return(Result);
}

BOOL
OpenAnyFont(struct Window *Parent,STRPTR FontName,WORD *FontSize)
{
	return(OpenFontCommon(Parent,FontName,FontSize,FALSE));
}

BOOL
OpenFixedFont(struct Window *Parent,STRPTR FontName,WORD *FontSize)
{
	return(OpenFontCommon(Parent,FontName,FontSize,TRUE));
}

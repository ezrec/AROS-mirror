/*
**	PickFile.c
**
**	Simplified file selection routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_LIST=1,GAD_USE,GAD_SELECT,GAD_CANCEL };

BOOL
ValidateFile(STRPTR FileName,LONG Type,STRPTR RealName)
{
	BPTR Segment;
	BOOL Valid;

	Valid = FALSE;

	if(Segment = LoadSeg(FileName))
	{
		BPTR *SegmentData;
		UWORD *Match;
		BPTR SegPtr;
		BOOL GotIt;
		LONG Size;

		SegPtr = Segment;
		GotIt = FALSE;

		do
		{
			SegmentData = (BPTR *)BADDR(SegPtr);

			SegPtr	= SegmentData[0];
			Size	= (IPTR)SegmentData[-1] - 2 * sizeof(ULONG) - sizeof(struct Resident);
			Match	= (UWORD *)(SegmentData + 1);

			while(!GotIt && Size > 0)
			{
				if(*Match == RTC_MATCHWORD)
				{
					struct Resident *Resident = (struct Resident *)Match;

					if(Resident->rt_MatchTag == Resident)
					{
						if(Resident->rt_Type == Type && Resident->rt_Name)
						{
							if(!Stricmp(FilePart(FileName),Resident->rt_Name))
							{
								if(RealName)
									strcpy(RealName,Resident->rt_Name);

								Valid = TRUE;
							}
						}

						GotIt = TRUE;
					}
				}

				Match	+= 2;
				Size	-= sizeof(ULONG);
			}
		}
		while(!GotIt && SegPtr);

		UnLoadSeg(Segment);
	}

	return(Valid);
}

	/* AddFile(struct List *List,STRPTR Name):
	 *
	 *	Add another file to the list:
	 */

STATIC VOID
AddFile(struct List *List,STRPTR Name)
{
	struct Node *NewNode;

	if(NewNode = CreateNode(Name))
	{
		struct Node *Node;
		LONG Result;

		for(Node = List->lh_Head ; Node->ln_Succ ; Node = Node->ln_Succ)
		{
			Result = Stricmp(Name,Node->ln_Name);

			if(Result < 0)
			{
				Insert(List,NewNode,Node->ln_Pred);
				return;
			}

			if(Result == 0)
			{
				FreeVecPooled(NewNode);
				return;
			}
		}

		AddTail(List,NewNode);
	}
}

	/* FileMultiScan(struct List *FileList,STRPTR Directory,STRPTR Pattern):
	 *
	 *	Scan an assignment, also handles multipath assignments.
	 */

STATIC VOID
FileMultiScan(struct List *FileList,CONST_STRPTR Directory,CONST_STRPTR Pattern,LONG Type)
{
	UBYTE MatchBuffer[MAX_FILENAME_LENGTH];
	struct MsgPort *FileSysTask;
	struct DevProc *DevProc;
	D_S(struct FileInfoBlock,FileInfo);

	DevProc		= NULL;
	FileSysTask	= GetFileSysTask();

	if(ParsePatternNoCase(Pattern,MatchBuffer,sizeof(MatchBuffer)) != -1)
	{
			/* Loop until all assignments are
			 * processed.
			 */

		do
		{
				/* Get the default filesystem task
				 * in case we stumble upon NULL
				 * directory locks.
				 */

			if(DevProc = GetDeviceProc(Directory,DevProc))
			{
					/* Set the default filesystem task. */

				SetFileSysTask(DevProc->dvp_Port);

					/* Check the object type. */

				if(Examine(DevProc->dvp_Lock,FileInfo))
				{
						/* Is it really a directory? */

					if(FileInfo->fib_DirEntryType > 0)
					{
							/* Scan the directory... */

						while(ExNext(DevProc->dvp_Lock,FileInfo))
						{
								/* Did we find a file? */

							if(FileInfo->fib_DirEntryType < 0)
							{
									/* Does the name match the template? */

								if(MatchPatternNoCase(MatchBuffer,FileInfo->fib_FileName))
								{
									if(Type == -1)
										AddFile(FileList,FileInfo->fib_FileName);
									else
									{
										UBYTE LocalBuffer[MAX_FILENAME_LENGTH],RealName[MAX_FILENAME_LENGTH];

										strcpy(LocalBuffer,Directory);

										if(AddPart(LocalBuffer,FileInfo->fib_FileName,sizeof(LocalBuffer)))
										{
											if(ValidateFile(LocalBuffer,Type,RealName))
												AddFile(FileList,RealName);
										}
									}
								}
							}
						}
					}
				}
			}
			else
				break;
		}
		while(DevProc && (DevProc->dvp_Flags & DVPF_ASSIGN));
	}

		/* Reset the default filesystem task. */

	SetFileSysTask(FileSysTask);

		/* Free device process data. */

	if(DevProc)
		FreeDeviceProc(DevProc);
}

	/* BuildFileList(STRPTR Directory,STRPTR Pattern):
	 *
	 *	Build a list of files in an assigned directory
	 *	matching a certain pattern.
	 */

STATIC struct List *
BuildFileList(CONST_STRPTR Directory,CONST_STRPTR Pattern,LONG Type)
{
	struct List	*FileList;
	APTR OldPtr;
	BPTR NewDir;

	FileList = NULL;

		/* No DOS requesters, please! */

	DisableDOSRequesters(&OldPtr);

		/* Is the assignment present? */

	if(NewDir = Lock(Directory,ACCESS_READ))
	{
			/* Allocate space for the new list. */

		if(FileList = CreateList())
		{
				/* Will we have to deal with
				 * an assignment or a volume?
				 */

			if(IsAssign(Directory))
				FileMultiScan(FileList,Directory,Pattern,Type);
			else
			{
				UBYTE MatchBuffer[MAX_FILENAME_LENGTH];
				D_S(struct FileInfoBlock,FileInfo);

				if(ParsePatternNoCase(Pattern,MatchBuffer,sizeof(MatchBuffer)) != -1)
				{
						/* Take a look at the assignment. */

					if(Examine(NewDir,FileInfo))
					{
							/* Does it really refer to a directory? */

						if(FileInfo->fib_DirEntryType > 0)
						{
								/* Examine the whole directory. */

							while(ExNext(NewDir,FileInfo))
							{
									/* Is it a file? */

								if(FileInfo->fib_DirEntryType < 0)
								{
									if(MatchPatternNoCase(MatchBuffer,FileInfo->fib_FileName))
									{
										if(Type == -1)
											AddFile(FileList,FileInfo->fib_FileName);
										else
										{
											UBYTE LocalBuffer[MAX_FILENAME_LENGTH],RealName[MAX_FILENAME_LENGTH];

											strcpy(LocalBuffer,Directory);

											if(AddPart(LocalBuffer,FileInfo->fib_FileName,sizeof(LocalBuffer)))
											{
												if(ValidateFile(LocalBuffer,Type,RealName))
													AddFile(FileList,RealName);
											}
										}
									}
								}
							}
						}
					}
				}
			}

			if(Type == NT_DEVICE)
			{
				UBYTE MatchBuffer[MAX_FILENAME_LENGTH];

				if(ParsePatternNoCase(Pattern,MatchBuffer,sizeof(MatchBuffer)) != -1)
				{
					struct Node *Node;

					Forbid();

					for(Node = SysBase->DeviceList.lh_Head ; Node->ln_Succ ; Node = Node->ln_Succ)
					{
						if(MatchPatternNoCase(MatchBuffer,Node->ln_Name))
							AddFile(FileList,Node->ln_Name);
					}

					Permit();
				}
			}

			if(Type == NT_LIBRARY)
			{
				UBYTE MatchBuffer[MAX_FILENAME_LENGTH];

				if(ParsePatternNoCase(Pattern,MatchBuffer,sizeof(MatchBuffer)) != -1)
				{
					struct Node *Node;

					Forbid();

					for(Node = SysBase->LibList.lh_Head ; Node->ln_Succ ; Node = Node->ln_Succ)
					{
						if(MatchPatternNoCase(MatchBuffer,Node->ln_Name))
							AddFile(FileList,Node->ln_Name);
					}

					Permit();
				}
			}

				/* Does the list contain any entries? */

			if(IsListEmpty(FileList))
			{
				FreeVecPooled(FileList);

				FileList = NULL;
			}
		}

			/* Release the lock on the directory. */

		UnLock(NewDir);
	}

		/* Enable DOS requesters again. */

	EnableDOSRequesters(OldPtr);

		/* Return the file name list. */

	return(FileList);
}

	/* PickFile(STRPTR Directory,STRPTR Pattern,STRPTR Prompt,STRPTR Name):
	 *
	 *	Your nice file selection routine. No need to hunt for
	 *	a library/device by checking all assignments by hand.
	 */

BOOL
PickFile(struct Window *Window,CONST_STRPTR Directory,CONST_STRPTR Pattern,CONST_STRPTR Prompt,STRPTR Name,LONG Type)
{
	UBYTE DummyBuffer[MAX_FILENAME_LENGTH];
	struct FileRequester *FileRequest;
	struct List *FileList;
	BOOL Result;

	Result = FALSE;

	if(FileList = BuildFileList(Directory,Pattern,Type))
	{
		struct LayoutHandle *Handle;

		if(Handle = LT_CreateHandleTags(Window->WScreen,
			LAHN_LocaleHook,	&LocaleHook,
		TAG_DONE))
		{
			struct Window *PanelWindow;
			struct Node *Node;
			ULONG Index,i;

			Index = (ULONG)~0;

			for(Node = FileList->lh_Head, i = 0 ; Node->ln_Succ ; Node = Node->ln_Succ, i++)
			{
				if(!Stricmp(Node->ln_Name,Name))
				{
					Index = i;

					break;
				}
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LONG MaxWidth,MaxHeight;

					MaxWidth	= 0;
					MaxHeight	= 0;

					if(FileList)
					{
						struct Node *Node;
						LONG Len;

						for(Node = FileList->lh_Head ; Node->ln_Succ ; Node = Node->ln_Succ)
						{
							Len = strlen(Node->ln_Name);

							if(Len > MaxWidth)
								MaxWidth = Len;

							MaxHeight++;
						}
					}

					if(MaxWidth < 30)
						MaxWidth = 30;

					if(MaxHeight < 10)
						MaxHeight = 10;
					else
					{
						if(MaxHeight > 20)
							MaxHeight = 20;
					}

					LT_New(Handle,
						LA_Type,		LISTVIEW_KIND,
						LA_Chars,		30,
						LA_ID,			GAD_LIST,
						LALV_Lines,		10,
						LALV_MaxGrowX,	MaxWidth,
						LALV_MaxGrowY,	MaxHeight,
						LALV_ResizeY,	TRUE,
						GTLV_Labels,	FileList,
						GTLV_Selected,	Index,
						LALV_Link,		NIL_LINK,
						LALV_CursorKey,	TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		XBAR_KIND,
						LAXB_FullSize,	TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,LA_Type,HORIZONTAL_KIND,
					LAGR_SameSize,	TRUE,
					LAGR_Spread,	TRUE,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_GLOBAL_USE_GAD,
						LA_ID,			GAD_USE,
						LABT_ReturnKey,	TRUE,
						LABT_ExtraFat,	TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_TERMPICKFILE_SELECT_GAD,
						LA_ID,			GAD_SELECT,
						LABT_ExtraFat,	TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_GLOBAL_CANCEL_GAD,
						LA_ID,			GAD_CANCEL,
						LABT_EscKey,	TRUE,
						LABT_ExtraFat,	TRUE,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_EndGroup(Handle);
			}

			if(PanelWindow = LT_Build(Handle,
				LAWN_TitleText,		Prompt,
				LAWN_HelpHook,		&GuideHook,
				LAWN_Parent,		Window,
				WA_DepthGadget,		TRUE,
				WA_DragBar,			TRUE,
				WA_RMBTrap,			TRUE,
				WA_Activate,		TRUE,
				WA_SimpleRefresh,	TRUE,
			TAG_DONE))
			{
				struct IntuiMessage	*Message;
				struct Gadget *MsgGadget;
				ULONG MsgClass;
				UWORD MsgCode;
				BOOL Done;

				LT_ShowWindow(Handle,TRUE);

				PushWindow(PanelWindow);

				Done = FALSE;

				do
				{
					if(Wait(PORTMASK(PanelWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
						break;

					while(Message = (struct IntuiMessage *)LT_GetIMsg(Handle))
					{
						MsgClass	= Message->Class;
						MsgCode		= Message->Code;
						MsgGadget	= (struct Gadget *)Message->IAddress;

						LT_ReplyIMsg(Message);

						if(MsgClass == IDCMP_GADGETUP)
						{
							switch(MsgGadget->GadgetID)
							{
								case GAD_USE:

									if(Index != (ULONG)~0)
									{
										struct Node *Node = GetListNode(Index,FileList);

										if(Node)
										{
											strcpy(Name,Node->ln_Name);

											Result = TRUE;
										}
									}

									Done = TRUE;
									break;

								case GAD_CANCEL:

									Done = TRUE;
									break;

								case GAD_LIST:

									Index = MsgCode;
									break;

								case GAD_SELECT:

									strcpy(DummyBuffer,Name);

									if(FileRequest = OpenSingleFile(PanelWindow,Prompt,LocaleString(MSG_GLOBAL_SELECT_TXT),Pattern,DummyBuffer,sizeof(DummyBuffer)))
									{
										FreeAslRequest(FileRequest);

										strcpy(Name,DummyBuffer);

										if(Type != -1)
										{
											UBYTE LocalBuffer[MAX_FILENAME_LENGTH];

											if(ValidateFile(DummyBuffer,Type,LocalBuffer))
											{
												*PathPart(Name) = 0;

												AddPart(Name,LocalBuffer,sizeof(LocalBuffer));
											}
										}

										Done = Result = TRUE;
									}

									break;
							}
						}

						if(MsgClass == IDCMP_IDCMPUPDATE)
						{
							struct Node *Node;

							if(Node = GetListNode(Index = MsgCode,FileList))
							{
								strcpy(Name,Node->ln_Name);

								Done = Result = TRUE;

								LT_PressButton(Handle,GAD_USE);
							}
						}
					}
				}
				while(!Done);

				PopWindow();
			}

			LT_DeleteHandle(Handle);
		}

		DeleteList(FileList);
	}
	else
	{
		strcpy(DummyBuffer,Name);

		if(FileRequest = OpenSingleFile(Window,Prompt,LocaleString(MSG_GLOBAL_SELECT_TXT),Pattern,DummyBuffer,sizeof(DummyBuffer)))
		{
			FreeAslRequest(FileRequest);

			strcpy(Name,DummyBuffer);

			if(Type != -1)
			{
				UBYTE LocalBuffer[MAX_FILENAME_LENGTH];

				if(ValidateFile(DummyBuffer,Type,LocalBuffer))
				{
					*PathPart(Name) = 0;

					AddPart(Name,LocalBuffer,sizeof(LocalBuffer));
				}
			}

			return(TRUE);
		}
	}

	return(Result);
}

/*
**	UploadQueue.c
**
**	Upload queue user interface.
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_LIST=1000,GAD_NAME,
			GAD_ADD,GAD_REMOVE,GAD_CLEAR,GAD_HIDE,
			GAD_ADD_FILES
		};

enum	{	APP_ICON=1,APP_WINDOW=2 };

enum	{	TRANSFERMSG_TOGGLE_ICON=1,
			TRANSFERMSG_DISPLAYLIST
		};

STATIC struct GenericList	*TransferList;
STATIC LONG					 TransferSignal;
STATIC struct MsgQueue		*TransferQueue;

STATIC VOID
LocalMsgDestructor(struct DataMsg *Item)
{
	Signal(Item->Client,Item->Mask);
}

VOID
NotifyUploadQueue(struct GenericList *UnusedList,WORD Login)
{
	if(TransferProcess)
	{
		LONG SigBit;

		if((SigBit = AllocSignal(-1)) != -1)
		{
			struct DataMsg Msg;

			InitMsgItem(&Msg,(DESTRUCTOR)LocalMsgDestructor);

			Msg.Type	= TRANSFERMSG_DISPLAYLIST;
			Msg.Size	= Login;
			Msg.Client	= FindTask(NULL);
			Msg.Mask	= 1L << SigBit;

			ClrSignal(Msg.Mask);

			PutMsgItem(TransferQueue,(struct MsgItem *)&Msg);

			Wait(Msg.Mask);

			FreeSignal(SigBit);
		}
	}
}

	/* QueuePanel(struct Window *Window):
	 *
	 *	Create the user interface for the queue panel.
	 */

STATIC LayoutHandle *
QueuePanel(struct Window *Parent)
{
	LayoutHandle *Handle;

	LockGenericList(TransferList);

	if(Handle = LT_CreateHandleTags(Parent ? Parent->WScreen : NULL,
		LAHN_LocaleHook,	&LocaleHook,
	TAG_DONE))
	{
		LONG Left,Top,Width,Height;
		BOOL KeepPosition;

		Left = Top = Width = Height = 0;

		if(Handle->Screen == Screen)
		{
			KeepPosition = TRUE;

			GetWindowInfo(WINDOW_QUEUE,&Left,&Top,&Width,&Height,0,0);
		}
		else
			KeepPosition = FALSE;

		LT_New(Handle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		LISTVIEW_KIND,
					LA_LabelID,		MSG_UPLOADQUEUE_FILES_TO_UPLOAD_TXT,
					LA_LabelPlace,	PLACE_Above,
					LA_Chars,		30,
					LA_ID,			GAD_LIST,
					LALV_Lines,		10,
					LALV_CursorKey,	TRUE,
					LALV_MaxGrowY,	20,
					LALV_MaxGrowX,	50,
					LALV_ResizeX,	TRUE,
					LALV_ResizeY,	TRUE,
					GTLV_Labels,	TransferList,

					Kick30 ? LALV_Link : TAG_IGNORE,NIL_LINK,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_ID,			GAD_NAME,
					GTST_MaxChars,	MAX_FILENAME_LENGTH - 1,
					LAST_Picker,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,		HORIZONTAL_KIND,
				LA_LabelID,		MSG_V36_0043,
				LAGR_SameSize,	TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_ADD_SEVERAL_FILES_TXT,
					LA_ID,			GAD_ADD_FILES,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_UPLOADQUEUE_ADD_TXT,
					LA_ID,			GAD_ADD,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_UPLOADQUEUE_REMOVE_TXT,
					LA_ID,			GAD_REMOVE,
					GA_Disabled,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_UPLOADQUEUE_CLEAR_TXT,
					LA_ID,			GAD_CLEAR,
					GA_Disabled,	!TransferList->ListCount,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_New(Handle,
				LA_Type,	VERTICAL_KIND,
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
					LA_LabelID,		MSG_UPLOADQUEUE_BINARY_UPLOAD_TXT,
					LA_ID,			UPLOAD_BINARY,
					LABT_ExtraFat,	TRUE,
					LABT_ReturnKey,	TRUE,
					GA_Disabled,	!TransferList->ListCount,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_UPLOADQUEUE_TEXT_UPLOAD_TXT,
					LA_ID,			UPLOAD_TEXT,
					GA_Disabled,	!TransferList->ListCount,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_UPLOADQUEUE_HIDE_TXT,
					LA_ID,			GAD_HIDE,
					LABT_EscKey,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}

		if(LT_Build(Handle,
			LAWN_TitleID,		MSG_UPLOADQUEUE_FILE_UPLOAD_LIST_TXT,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_Activate,		TRUE,
			WA_SimpleRefresh,	TRUE,

			KeepPosition ? LAWN_Left : TAG_IGNORE,			Left,
			KeepPosition ? LAWN_Top : TAG_IGNORE,			Top,
			KeepPosition ? TAG_IGNORE : LAWN_BelowMouse,	TRUE,
		TAG_DONE))
		{
			GuideContext(CONTEXT_UPLOAD_QUEUE);

			UnlockGenericList(TransferList);

			return(Handle);
		}

		LT_DeleteHandle(Handle);
	}

	UnlockGenericList(TransferList);

	return(NULL);
}

	/* HandleQueueWindow(LayoutHandle *Handle):
	 *
	 *	Process queue panel messages.
	 */

STATIC LONG
HandleQueueWindow(LayoutHandle *Handle)
{
	struct IntuiMessage		*Message;
	ULONG					 MsgClass;
	UWORD					 MsgCode;
	struct Gadget			*MsgGadget;

	struct Node				*Node;

	struct Window			*Window = Handle->Window;
	LONG					 Result = 0;

	UBYTE		 			 DummyBuffer[MAX_FILENAME_LENGTH];
	struct FileRequester	*FileRequest;
	struct TagItem			 DimensionTags[5];

	while(Message = (struct IntuiMessage *)LT_GetIMsg(Handle))
	{
		MsgClass	= Message->Class;
		MsgCode		= Message->Code;
		MsgGadget	= (struct Gadget *)Message->IAddress;

		LT_ReplyIMsg(Message);

		if(MsgClass == IDCMP_IDCMPUPDATE && MsgGadget->GadgetID == GAD_NAME)
		{
			strcpy(DummyBuffer,LT_GetString(Handle,GAD_NAME));

			if(FileRequest = OpenSingleFile(Window,LocaleString(MSG_UPLOADQUEUE_SELECT_FILE_TXT),LocaleString(MSG_GLOBAL_SELECT_TXT),NULL,DummyBuffer,sizeof(DummyBuffer)))
			{
				LT_SetAttributes(Handle,GAD_NAME,
					GTST_String,	DummyBuffer,
				TAG_DONE);

				FreeAslRequest(FileRequest);
			}

			LT_Activate(Handle,GAD_NAME);
		}

		if(MsgClass == IDCMP_GADGETUP)
		{
			switch(MsgGadget->GadgetID)
			{
				case GAD_ADD_FILES:

					LT_LockWindow(Window);

					if(!LocalGetCurrentDirName(DummyBuffer,sizeof(DummyBuffer)))
						DummyBuffer[0] = 0;

					if(FileRequest = AllocAslRequestTags(ASL_FileRequest,
						ASLFR_Window,			Window,
						ASLFR_TitleText,		LocaleString(MSG_ADD_FILES_TITLE_TXT),
						ASLFR_Flags1,			FILF_MULTISELECT | FILF_NEWIDCMP,
						ASLFR_PositiveText,		LocaleString(MSG_ADD_GAD),
						ASLFR_NegativeText,		LocaleString(MSG_DONE_GAD),
						ASLFR_InitialDrawer,	DummyBuffer,
						ASLFR_TextAttr,			&UserFont,
						ASLFR_PrivateIDCMP,		TRUE,
					TAG_MORE,GetDimensionTags(Window,DimensionTags)))
					{
						LONG i;

						LockGenericList(TransferList);

						LT_SetAttributes(Handle,GAD_LIST,
							GTLV_Labels,	~0,
						TAG_DONE);

						while(AslRequest(FileRequest,NULL))
						{
							for(i = 0 ; i < FileRequest->fr_NumArgs ; i++)
							{
								if(FileRequest->fr_ArgList[i].wa_Lock)
								{
									if(!NameFromLock(FileRequest->fr_ArgList[i].wa_Lock,DummyBuffer,sizeof(DummyBuffer)))
										DummyBuffer[0] = 0;
								}
								else
									strcpy(DummyBuffer,FileRequest->fr_Drawer);

								if(FileRequest->fr_ArgList[i].wa_Name)
								{
									if(!AddPart(DummyBuffer,FileRequest->fr_ArgList[i].wa_Name,sizeof(DummyBuffer)))
										DummyBuffer[0] = 0;
								}

								if(DummyBuffer[0])
								{
									if(Node = (struct Node *)AllocVecPooled(sizeof(struct Node) + 256,MEMF_ANY))
									{
										Node->ln_Name = (STRPTR)(Node + 1);

										strcpy(Node->ln_Name,DummyBuffer);

										AddGenericListNode(TransferList,Node,ADD_GLIST_BOTTOM,FALSE);
									}
								}
							}

							LT_SetAttributes(Handle,GAD_LIST,
								GTLV_Labels,	TransferList,
								LALV_Selected,	TransferList->ListOffset,
							TAG_DONE);
						}

						LT_SetAttributes(Handle,GAD_LIST,
							GTLV_Labels,	TransferList,
							LALV_Selected,	TransferList->ListOffset,
						TAG_DONE);

						if(TransferList->ListCount)
						{
							LT_SetAttributes(Handle,GAD_NAME,
								GTST_String,	"",
							TAG_DONE);

							LT_SetAttributes(Handle,GAD_REMOVE,
								GA_Disabled,	FALSE,
							TAG_DONE);

							LT_SetAttributes(Handle,UPLOAD_BINARY,
								GA_Disabled,	FALSE,
							TAG_DONE);

							LT_SetAttributes(Handle,UPLOAD_TEXT,
								GA_Disabled,	FALSE,
							TAG_DONE);

							LT_SetAttributes(Handle,GAD_CLEAR,
								GA_Disabled,	FALSE,
							TAG_DONE);
						}

						PutDimensionTags(Window,FileRequest->fr_LeftEdge,FileRequest->fr_TopEdge,FileRequest->fr_Width,FileRequest->fr_Height);

						FreeAslRequest(FileRequest);
					}

					LT_UnlockWindow(Window);
					break;

				case GAD_ADD:

					if(Node = (struct Node *)AllocVecPooled(sizeof(struct Node) + 256,MEMF_ANY))
					{
						Node->ln_Name = (STRPTR)(Node + 1);

						Node->ln_Name[0] = 0;

						LockGenericList(TransferList);

						LT_SetAttributes(Handle,GAD_LIST,
							GTLV_Labels,	~0,
						TAG_DONE);

						AddGenericListNode(TransferList,Node,ADD_GLIST_BOTTOM,FALSE);

						LT_SetAttributes(Handle,GAD_LIST,
							GTLV_Labels,	TransferList,
							LALV_Selected,	TransferList->ListOffset,
						TAG_DONE);

						UnlockGenericList(TransferList);

						LT_SetAttributes(Handle,GAD_NAME,
							GTST_String,	"",
						TAG_DONE);

						LT_SetAttributes(Handle,GAD_REMOVE,
							GA_Disabled,	FALSE,
						TAG_DONE);

						LT_SetAttributes(Handle,UPLOAD_BINARY,
							GA_Disabled,	FALSE,
						TAG_DONE);

						LT_SetAttributes(Handle,UPLOAD_TEXT,
							GA_Disabled,	FALSE,
						TAG_DONE);

						LT_SetAttributes(Handle,GAD_CLEAR,
							GA_Disabled,	FALSE,
						TAG_DONE);

						LT_Activate(Handle,GAD_NAME);
					}
					else
						DisplayBeep(Window->WScreen);

					break;

				case GAD_REMOVE:

					if(TransferList->ListNode)
					{
						LockGenericList(TransferList);

						LT_SetAttributes(Handle,GAD_LIST,
							GTLV_Labels,	~0,
						TAG_DONE);

						DeleteGenericListNode(TransferList,NULL,FALSE);

						LT_SetAttributes(Handle,GAD_LIST,
							GTLV_Labels,	TransferList,
							LALV_Selected,	TransferList->ListOffset,
						TAG_DONE);

						if(!TransferList->ListCount)
						{
							LT_SetAttributes(Handle,GAD_REMOVE,
								GA_Disabled,	TRUE,
							TAG_DONE);

							LT_SetAttributes(Handle,GAD_NAME,
								GTST_String,	"",
							TAG_DONE);

							LT_SetAttributes(Handle,GAD_CLEAR,
								GA_Disabled,	TRUE,
							TAG_DONE);

							LT_SetAttributes(Handle,UPLOAD_BINARY,
								GA_Disabled,	TRUE,
							TAG_DONE);

							LT_SetAttributes(Handle,UPLOAD_TEXT,
								GA_Disabled,	TRUE,
							TAG_DONE);
						}
						else
						{
							LT_SetAttributes(Handle,GAD_NAME,
								GTST_String,	TransferList->ListNode->ln_Name,
							TAG_DONE);
						}

						UnlockGenericList(TransferList);
					}

					break;

				case GAD_LIST:

					SelectGenericListNode(TransferList,NULL,MsgCode);

					LockGenericList(TransferList);

					if(TransferList->ListNode)
					{
						LT_SetAttributes(Handle,GAD_NAME,
							GTST_String,	TransferList->ListNode->ln_Name,
						TAG_DONE);

						LT_SetAttributes(Handle,GAD_REMOVE,
							GA_Disabled,	FALSE,
						TAG_DONE);
					}

					UnlockGenericList(TransferList);

					break;

				case GAD_NAME:

					LockGenericList(TransferList);

					LT_SetAttributes(Handle,GAD_LIST,
						GTLV_Labels,	~0,
					TAG_DONE);

					if(TransferList->ListNode)
						strcpy(TransferList->ListNode->ln_Name,LT_GetString(Handle,GAD_NAME));
					else
					{
						if(Node = (struct Node *)AllocVecPooled(sizeof(struct Node) + 256,MEMF_ANY))
						{
							Node->ln_Name = (STRPTR)(Node + 1);

							strcpy(Node->ln_Name,LT_GetString(Handle,GAD_NAME));

							AddGenericListNode(TransferList,Node,ADD_GLIST_BOTTOM,FALSE);

							LT_SetAttributes(Handle,GAD_NAME,
								GTST_String,	"",
							TAG_DONE);

							LT_SetAttributes(Handle,GAD_REMOVE,
								GA_Disabled,	FALSE,
							TAG_DONE);

							LT_SetAttributes(Handle,UPLOAD_BINARY,
								GA_Disabled,	FALSE,
							TAG_DONE);

							LT_SetAttributes(Handle,UPLOAD_TEXT,
								GA_Disabled,	FALSE,
							TAG_DONE);

							LT_SetAttributes(Handle,GAD_CLEAR,
								GA_Disabled,	FALSE,
							TAG_DONE);
						}
					}

					LT_SetAttributes(Handle,GAD_LIST,
						GTLV_Labels,	TransferList,
						LALV_Selected,	~0,
					TAG_DONE);

					UnlockGenericList(TransferList);

					LT_SetAttributes(Handle,GAD_REMOVE,
						GA_Disabled,	TRUE,
					TAG_DONE);

					LT_SetAttributes(Handle,GAD_NAME,
						GTST_String,	"",
					TAG_DONE);

					SelectGenericListNode(TransferList,NULL,-1);

					break;

				case GAD_CLEAR:

					LockGenericList(TransferList);

					LT_SetAttributes(Handle,GAD_LIST,
						GTLV_Labels,	~0,
					TAG_DONE);

					ClearGenericList(TransferList,FALSE);

					LT_SetAttributes(Handle,GAD_LIST,
						GTLV_Labels,	TransferList,
						LALV_Selected,	~0,
					TAG_DONE);

					UnlockGenericList(TransferList);

					LT_SetAttributes(Handle,GAD_REMOVE,
						GA_Disabled,	TRUE,
					TAG_DONE);

					LT_SetAttributes(Handle,GAD_NAME,
						GTST_String,	"",
					TAG_DONE);

					LT_SetAttributes(Handle,GAD_CLEAR,
						GA_Disabled,	TRUE,
					TAG_DONE);

					LT_SetAttributes(Handle,UPLOAD_BINARY,
						GA_Disabled,	TRUE,
					TAG_DONE);

					LT_SetAttributes(Handle,UPLOAD_TEXT,
						GA_Disabled,	TRUE,
					TAG_DONE);

					break;

				case UPLOAD_BINARY:
				case UPLOAD_TEXT:
				case GAD_HIDE:

					Result = MsgGadget->GadgetID;
					break;
			}
		}
	}

	return(Result);
}

	/* QueueEntry(VOID):
	 *
	 *	The entry point for the queue process.
	 */

STATIC VOID SAVE_DS
QueueEntry(VOID)
{
	if(TransferQueue = CreateMsgQueue(0,0))
	{
		struct MsgPort		*AppPort;
		struct AppIcon		*AppIcon;
		struct AppWindow	*AppWindow;
		APTR				 OldPtr;

		if(AppPort = CreateMsgPort())
		{
			if((TransferSignal = AllocSignal(-1)) != -1)
			{
				ULONG				 Signals;
				BOOL				 Done = FALSE;
				LayoutHandle		*Handle;
				struct Window		*LocalWindow;
				ULONG				 WindowMask,
									 TransferMask,
									 ClientMask;
				struct AppMessage	*AppMessage;
				struct DiskObject	*LoadedIcon = NULL;

				if(WorkbenchBase && !Config->TransferConfig->HideUploadIcon)
				{
					if(IconBase)
					{
						UBYTE LocalBuffer[MAX_FILENAME_LENGTH];

						strcpy(LocalBuffer,Config->PathConfig->DefaultStorage);

						if(AddPart(LocalBuffer,"term_DropIcon",sizeof(LocalBuffer)))
							LoadedIcon = GetDiskObject(LocalBuffer);

						if(!LoadedIcon)
							LoadedIcon = GetDiskObject("PROGDIR:term_DropIcon");
					}

					AppIcon = AddAppIconA(APP_ICON,0,LocaleString(MSG_UPLOADQUEUE_TERM_UPLOAD_LIST_TXT),AppPort,BNULL,LoadedIcon ? LoadedIcon : &DropIcon,NULL);
				}
				else
					AppIcon = NULL;

				AppWindow		= NULL;
				WindowMask		= 0;
				Handle			= NULL;
				LocalWindow		= NULL;
				TransferMask	= TransferQueue->SigMask;
				ClientMask		= (1L << TransferSignal);

				TransferProcess = (struct Process *)FindTask(NULL);

				OldPtr = NULL;

				Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);

				do
				{
					Signals = Wait(SIG_KILL | SIG_SHOW | SIG_HIDE | SIG_GOAWAY | WindowMask | TransferMask | PORTMASK(AppPort));

					if(Signals & TransferMask)
					{
						struct DataMsg *Item;

						while(Item = GetMsgItem(TransferQueue))
						{
							switch(Item->Type)
							{
									/* Show or hide the icon? */

								case TRANSFERMSG_TOGGLE_ICON:

										/* Hide the icon */

									if(Item->Size)
									{
										if(AppIcon)
										{
											RemoveAppIcon(AppIcon);

											AppIcon = NULL;
										}

										if(LoadedIcon)
										{
											FreeDiskObject(LoadedIcon);

											LoadedIcon = NULL;
										}
									}
									else
									{
										if(!AppIcon)
										{
											if(WorkbenchBase)
											{
												if(IconBase)
												{
													UBYTE LocalBuffer[MAX_FILENAME_LENGTH];

													strcpy(LocalBuffer,Config->PathConfig->DefaultStorage);

													if(AddPart(LocalBuffer,"term_DropIcon",sizeof(LocalBuffer)))
														LoadedIcon = GetDiskObject(LocalBuffer);

													if(!LoadedIcon)
														LoadedIcon = GetDiskObject("PROGDIR:term_DropIcon");
												}

												AppIcon = AddAppIconA(APP_ICON,0,LocaleString(MSG_UPLOADQUEUE_TERM_UPLOAD_LIST_TXT),AppPort,BNULL,LoadedIcon ? LoadedIcon : &DropIcon,NULL);
											}
											else
												AppIcon = NULL;
										}
									}

									break;

								case TRANSFERMSG_DISPLAYLIST:

									if(Item->Size)
									{
										LT_SetAttributes(Handle,GAD_LIST,
											GTLV_Labels,	TransferList,
											LALV_Selected,	TransferList->ListOffset,
										TAG_DONE);

										if(!TransferList->ListCount)
										{
											LT_SetAttributes(Handle,GAD_REMOVE,
												GA_Disabled,	TRUE,
											TAG_DONE);

											LT_SetAttributes(Handle,GAD_NAME,
												GTST_String,	"",
											TAG_DONE);

											LT_SetAttributes(Handle,GAD_CLEAR,
												GA_Disabled,	TRUE,
											TAG_DONE);

											LT_SetAttributes(Handle,UPLOAD_BINARY,
												GA_Disabled,	TRUE,
											TAG_DONE);

											LT_SetAttributes(Handle,UPLOAD_TEXT,
												GA_Disabled,	TRUE,
											TAG_DONE);
										}
										else
										{
											LT_SetAttributes(Handle,GAD_NAME,
												GTST_String,	TransferList->ListNode->ln_Name,
											TAG_DONE);
										}
									}
									else
									{
										LT_SetAttributes(Handle,GAD_LIST,
											GTLV_Labels,~0,
										TAG_DONE);
									}

									break;
							}

							DeleteMsgItem((struct MsgItem *)Item);
						}
					}

					if(Signals & SIG_HIDE)
					{
							if(AppWindow)
							{
								RemoveAppWindow(AppWindow);

								AppWindow = NULL;
							}

							if(Handle)
							{
								if(Handle->Window->WScreen == Screen)
									PutWindowInfo(WINDOW_QUEUE,Handle->Window->LeftEdge,Handle->Window->TopEdge,Handle->Window->Width,Handle->Window->Height);

								LT_DeleteHandle(Handle);

								Handle = NULL;
							}

							WindowMask	= 0;
							LocalWindow	= NULL;

							RestoreWindowPtr(OldPtr);

							Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);
					}

					if(Signals & SIG_GOAWAY)
					{
							if(Handle)
							{
								if(LocalWindow->WScreen == Screen)
								{
									if(AppWindow)
									{
										RemoveAppWindow(AppWindow);

										AppWindow = NULL;
									}

									if(Handle->Window->WScreen == Screen)
										PutWindowInfo(WINDOW_QUEUE,Handle->Window->LeftEdge,Handle->Window->TopEdge,Handle->Window->Width,Handle->Window->Height);

									LT_DeleteHandle(Handle);

									Handle		= NULL;
									WindowMask	= 0;
									LocalWindow	= NULL;

									RestoreWindowPtr(OldPtr);
								}
							}

							Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);
					}

					if(Signals & SIG_KILL)
						break;

					if(Signals & SIG_SHOW)
					{
							if(!Handle)
							{
								Forbid();

								if(Handle = QueuePanel(Window))
								{
									Permit();

									if(WorkbenchBase)
										AppWindow = AddAppWindowA(APP_WINDOW,0,Handle->Window,AppPort,NULL);
									else
										AppWindow = NULL;

									LocalWindow	= Handle->Window;
									WindowMask	= PORTMASK(LocalWindow->UserPort);

									ChangeWindowPtr(&OldPtr,(APTR)LocalWindow);
								}
								else
									Permit();
							}

							if(Handle)
								LT_ShowWindow(Handle,TRUE);
					}

					if(Signals & WindowMask)
					{
						struct DataMsg	Msg;
						LONG			Type;

						switch(Type = HandleQueueWindow(Handle))
						{
							case GAD_HIDE:

								if(AppWindow)
								{
									RemoveAppWindow(AppWindow);

									AppWindow = NULL;
								}

								if(Handle)
								{
									if(Handle->Window->WScreen == Screen)
										PutWindowInfo(WINDOW_QUEUE,Handle->Window->LeftEdge,Handle->Window->TopEdge,Handle->Window->Width,Handle->Window->Height);

									LT_DeleteHandle(Handle);

									Handle = NULL;
								}

								RestoreWindowPtr(OldPtr);

								WindowMask	= 0;
								LocalWindow	= NULL;

								break;

							case UPLOAD_BINARY:
							case UPLOAD_TEXT:

								InitMsgItem(&Msg,(DESTRUCTOR)LocalMsgDestructor);

								Msg.Type	= DATAMSGTYPE_UPLOAD;
								Msg.Data	= NULL;
								Msg.Size	= Type;
								Msg.Client	= (struct Task *)TransferProcess;
								Msg.Mask	= ClientMask;

								Forbid();

								ClrSignal(ClientMask);

								PutMsgItem(SpecialQueue,(struct MsgItem *)&Msg);

								Wait(ClientMask);

								Permit();

								break;
						}
					}

					if(Signals & PORTMASK(AppPort))
					{
						while(AppMessage = (struct AppMessage *)GetMsg(AppPort))
						{
								if(AppMessage->am_Type == AMTYPE_APPWINDOW || AppMessage->am_Type == AMTYPE_APPICON)
								{
									if(AppMessage->am_Type == AMTYPE_APPICON && !AppMessage->am_NumArgs)
									{
										if(Handle)
											LT_ShowWindow(Handle,TRUE);
										else
										{
											if(Handle = QueuePanel(NULL))
											{
												AppWindow = AddAppWindowA(APP_WINDOW,0,Handle->Window,AppPort,NULL);

												LocalWindow = Handle->Window;

												WindowMask = PORTMASK(LocalWindow->UserPort);

												ChangeWindowPtr(&OldPtr,(APTR)LocalWindow);
											}
										}
									}
									else
									{
										UBYTE	DummyBuffer[MAX_FILENAME_LENGTH];
										BPTR	OldDir,FileLock;
										APTR	OldPtr;
										LONG	i;

										DisableDOSRequesters(&OldPtr);

										LockGenericList(TransferList);

										LT_SetAttributes(Handle,GAD_LIST,
											GTLV_Labels,	~0,
										TAG_DONE);

										for(i = 0 ; i < AppMessage->am_NumArgs ; i++)
										{
											if(AppMessage->am_ArgList[i].wa_Name)
											{
												OldDir = CurrentDir(AppMessage->am_ArgList[i].wa_Lock);

												if(GetFileSize(AppMessage->am_ArgList[i].wa_Name))
												{
													if(FileLock = Lock(AppMessage->am_ArgList[i].wa_Name,ACCESS_READ))
													{
														if(NameFromLock(FileLock,DummyBuffer,sizeof(DummyBuffer)))
														{
															struct Node *Node;

															if(Node = (struct Node *)AllocVecPooled(sizeof(struct Node) + 256,MEMF_ANY))
															{
																Node->ln_Name = (STRPTR)(Node + 1);

																strcpy(Node->ln_Name,DummyBuffer);

																AddGenericListNode(TransferList,Node,ADD_GLIST_BOTTOM,FALSE);
															}
														}

														UnLock(FileLock);
													}
												}

												CurrentDir(OldDir);
											}
										}

										UnlockGenericList(TransferList);

										EnableDOSRequesters(OldPtr);

										LT_SetAttributes(Handle,GAD_LIST,
											GTLV_Labels,	TransferList,
										TAG_DONE);

										LT_SetAttributes(Handle,GAD_CLEAR,
											GA_Disabled,	!TransferList->ListCount,
										TAG_DONE);

										LT_SetAttributes(Handle,UPLOAD_BINARY,
											GA_Disabled,	!TransferList->ListCount,
										TAG_DONE);

										LT_SetAttributes(Handle,UPLOAD_TEXT,
											GA_Disabled,	!TransferList->ListCount,
										TAG_DONE);
									}
								}

								ReplyMsg((struct Message *)AppMessage);
						}
					}
				}
				while(!Done);

				if(AppIcon)
					RemoveAppIcon(AppIcon);

				if(AppWindow)
					RemoveAppWindow(AppWindow);

				if(LoadedIcon)
					FreeDiskObject(LoadedIcon);

				if(Handle)
				{
					if(Handle->Window->WScreen == Screen)
						PutWindowInfo(WINDOW_QUEUE,Handle->Window->LeftEdge,Handle->Window->TopEdge,Handle->Window->Width,Handle->Window->Height);

					LT_DeleteHandle(Handle);
				}

				while(AppMessage = (struct AppMessage *)GetMsg(AppPort))
					ReplyMsg((struct Message *)AppMessage);

				FreeSignal(TransferSignal);
			}
		}

		DeleteMsgQueue(TransferQueue);
	}

	Forbid();

	TransferProcess = NULL;

	Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);
}

	/* CloseQueueWindow():
	 *
	 *	Close the queue window, wherever it may be.
	 */

VOID
CloseQueueWindow()
{
	ShakeHands((struct Task *)TransferProcess,SIG_GOAWAY);
}

	/* DeleteQueueProcess():
	 *
	 *	Terminate the queue handler process.
	 */

VOID
DeleteQueueProcess()
{
	ShakeHands((struct Task *)TransferProcess,SIG_KILL);
}

	/* CreateQueueProcess():
	 *
	 *	Create the queue handler process, if it's still around
	 *	bring it to the front.
	 */

BOOL
CreateQueueProcess()
{
	if(!TransferProcess)
	{
		BPTR UploadDrawer = Lock(Config->PathConfig->BinaryUploadPath,SHARED_LOCK);

		TransferList = GenericListTable[GLIST_UPLOAD];

		StartProcessWaitForHandshake("term Upload process",(TASKENTRY)QueueEntry,
			NP_WindowPtr,	NULL,
			NP_CurrentDir,	UploadDrawer,
		TAG_DONE);

		if(!TransferProcess)
			UnLock(UploadDrawer);
	}
	else
		Signal((struct Task *)TransferProcess,SIG_SHOW);

	return((BOOL)(TransferProcess != NULL));
}

	/* ToggleUploadQueueIcon():
	 *
	 *	Turn the upload queue appicon on or off.
	 */

VOID
ToggleUploadQueueIcon(BOOL Mode)
{
	if(TransferProcess)
	{
		LONG SigBit;

		if((SigBit = AllocSignal(-1)) != -1)
		{
			struct DataMsg Msg;

			InitMsgItem(&Msg,(DESTRUCTOR)LocalMsgDestructor);

			Msg.Type	= TRANSFERMSG_TOGGLE_ICON;
			Msg.Size	= Mode;
			Msg.Client	= FindTask(NULL);
			Msg.Mask	= 1L << SigBit;

			ClrSignal(Msg.Mask);

			PutMsgItem(TransferQueue,(struct MsgItem *)&Msg);

			Wait(Msg.Mask);

			FreeSignal(SigBit);
		}
	}
}

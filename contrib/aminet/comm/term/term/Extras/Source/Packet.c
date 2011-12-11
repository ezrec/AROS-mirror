/*
**	Packet.c
**
**	Support routines for the `packet window'
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* Maximum size of the packet line buffer. */

#define MAX_PACKET_LINE_LENGTH	1024

	/* Some private data required to handle both the window,
	 * the editing functions and the command history buffer.
	 */

STATIC STRPTR					PacketContents;
STATIC STRPTR					PacketTempBuffer;

STATIC struct Process *			PacketProcess;
STATIC struct SignalSemaphore	PacketSemaphore;

STATIC LONG						PacketLine;
STATIC LONG						PacketCount;

STATIC BOOL						HasList	= FALSE;
STATIC BOOL						Echo	= -1;

STATIC BYTE						PacketSignal;

	/* Gadget IDs */

enum	{	GAD_STRING, GAD_LIST };

	/* The menu to attach to the packet window. */

enum	{	MEN_LOADHISTORY=1,MEN_SAVEHISTORY,MEN_CLEARHISTORY,
			MEN_OTHERWINDOW,MEN_TOGGLE_ECHO,MEN_QUITPANEL
		};

STATIC VOID
PacketDestructor(struct MsgItem *UnusedItem)
{
	Signal((struct Task *)PacketProcess,1L << PacketSignal);
}

STATIC VOID
PacketOutput(APTR Data,LONG Size,LONG Type)
{
	struct DataMsg Msg;

	InitMsgItem(&Msg,PacketDestructor);

	Msg.Type = Type;
	Msg.Data = Data;
	Msg.Size = Size;

	Forbid();

	ClrSignal(1L << PacketSignal);

	PutMsgItem(SpecialQueue,(struct MsgItem *)&Msg);

	Wait(1L << PacketSignal);

	Permit();
}

STATIC VOID
PacketWriteString(APTR Data,LONG Size)
{
	LONG Type;

	if(Echo == FALSE)
		Type = DATAMSGTYPE_WRITESTRING_NOECHO;
	else
		Type = DATAMSGTYPE_WRITESTRING;

	PacketOutput(Data,Size,Type);
}

STATIC VOID
PacketSerialCommand(APTR Data,LONG Size)
{
	LONG Type;

	if(Echo == FALSE)
		Type = DATAMSGTYPE_SERIALCOMMANDNOECHO;
	else
		Type = DATAMSGTYPE_SERIALCOMMAND;

	PacketOutput(Data,Size,Type);
}

	/* CreateAllGadgets():
	 *
	 *	Create the packet string gadget.
	 */

STATIC struct Gadget *
CreateAllGadgets(LONG Width,LONG Height,LONG *MinHeight,struct Gadget **GadgetArray,struct Gadget **GadgetList,APTR LocalVisualInfo)
{
	STATIC struct Hook PacketHook;

	struct Gadget		*Gadget;
	struct NewGadget	 NewGadget;

	InitHook(&PacketHook,(HOOKFUNC)CommonEditRoutine,NULL);

	ObtainSemaphore(&PacketSemaphore);

	memset(&NewGadget,0,sizeof(struct NewGadget));

	if(Gadget = CreateContext(GadgetList))
	{
		LONG Rest,StringHeight,ProtoHeight;

		StringHeight	= 3 + TextAttr.tta_YSize + 3;
		ProtoHeight		= Window->WScreen->WBorTop + Window->WScreen->Font->ta_YSize + 1 + 1 + StringHeight + 1 + Window->WScreen->WBorBottom;
		Rest			= (Height - 2 - ProtoHeight - 2) / TextAttr.tta_YSize;

		if(MinHeight)
			*MinHeight = ProtoHeight;

		NewGadget.ng_Width		= Width - 26;
		NewGadget.ng_Height		= StringHeight;
		NewGadget.ng_GadgetText	= NULL;
		NewGadget.ng_TextAttr	= (struct TextAttr *)&TextAttr;
		NewGadget.ng_VisualInfo	= LocalVisualInfo;
		NewGadget.ng_GadgetID	= GAD_STRING;
		NewGadget.ng_Flags		= 0;
		NewGadget.ng_LeftEdge	= 6;
		NewGadget.ng_TopEdge	= Window->WScreen->WBorTop + Window->WScreen->Font->ta_YSize + 2;

		GadgetArray[GAD_STRING] = Gadget = CreateGadget(STRING_KIND,Gadget,&NewGadget,
			GTST_MaxChars,	MAX_PACKET_LINE_LENGTH - 1,
			GTST_EditHook,	&PacketHook,
			GA_TabCycle,	FALSE,
		TAG_DONE);

			/* If the window is large enough to display the
			 * list, create the list view gadget.
			 */

		if(Rest > 0)
		{
			NewGadget.ng_Height		= 2 + Rest * TextAttr.tta_YSize + 2 + StringHeight;
			NewGadget.ng_GadgetID	= GAD_LIST;

			GadgetArray[GAD_LIST] = Gadget = CreateGadget(LISTVIEW_KIND,Gadget,&NewGadget,
				GTLV_ShowSelected,	GadgetArray[GAD_STRING],
				GTLV_Labels,		&PacketHistoryList,
				GTLV_Selected,		PacketLine,
			TAG_DONE);

			if(Gadget)
				HasList = TRUE;
			else
				HasList = FALSE;
		}
		else
			HasList = FALSE;
	}

	ReleaseSemaphore(&PacketSemaphore);

	return(Gadget);
}

STATIC VOID
HandlePacketGadgetEvent(UWORD QualifierCode)
{
	BOOL Activate,NeedChange,AddIt;
	UWORD Qualifier,Code;
	struct Node *Node;

	Qualifier	= QualifierCode >> 8;
	Code		= QualifierCode & 0xFF;
	Activate	= TRUE;
	NeedChange	= FALSE;

	switch(Qualifier)
	{
		case 0xFF:

			switch(Code)
			{
				case CONTROL_('S'):
				case CONTROL_('Q'):

					PacketTempBuffer[0] = Code;
					PacketWriteString(PacketTempBuffer,1);
					break;

				case '\r':

					LimitedStrcpy(MAX_PACKET_LINE_LENGTH,PacketTempBuffer,PacketContents);

						/* Add the line unless it is already in the history list. */

					AddIt = TRUE;

					if(PacketLine != -1)
					{
						if(Node = GetListNode(PacketLine,&PacketHistoryList))
						{
							if(!Stricmp(Node->ln_Name,PacketTempBuffer))
								AddIt = FALSE;
						}
					}

					if(AddIt)
						AddPacketHistory(PacketTempBuffer);

					PacketLine = -1;
					NeedChange = TRUE;

					LimitedStrcat(MAX_PACKET_LINE_LENGTH,PacketTempBuffer,"\\r");
					PacketSerialCommand(PacketTempBuffer,strlen(PacketTempBuffer));
					break;

				case '\n':

					Activate = FALSE;
					break;
			}

			break;

		default:

				/* Clear the history list when pressing Amiga+Del/Amiga+Backspace */

			if((Qualifier & (AMIGARIGHT | AMIGALEFT)) && (Code == DEL_CODE || Code == BACKSPACE_CODE))
			{
				ClearPacketHistory();

				PacketCount = 0;
				PacketLine = -1;

				NeedChange = TRUE;

				break;
			}

				/* This looks like a function key. Send the corresponding macro. */

			if(Code >= F01_CODE && Code <= F10_CODE)
			{
				STRPTR String;
				LONG Index;

				Index = Code - F01_CODE;

					/* Pick the right macro */

				if(Qualifier & CONTROL_KEY)
					String = MacroKeys->Keys[3][Index];
				else
				{
					if(Qualifier & ALT_KEY)
						String = MacroKeys->Keys[2][Index];
					else
					{
						if(Qualifier & SHIFT_KEY)
							String = MacroKeys->Keys[1][Index];
						else
							String = MacroKeys->Keys[0][Index];
					}
				}

					/* Anything to do? */

				if(String[0])
				{
						/* This should really insert the macro at the current
						 * cursor position, but due to some strange reasons
						 * we can't get the position reliably.
						 */

					LimitedStrcpy(MAX_PACKET_LINE_LENGTH,PacketTempBuffer,PacketContents);
					LimitedStrcat(MAX_PACKET_LINE_LENGTH,PacketTempBuffer,String);

					GT_SetGadgetAttrs(PacketGadgetArray[GAD_STRING],PacketWindow,NULL,
						GTST_String,		PacketTempBuffer,
					TAG_DONE);

						/* Now, that's really nasty... */

					((struct StringInfo *)PacketGadgetArray[GAD_STRING]->SpecialInfo)->BufferPos = strlen(PacketTempBuffer);
				}

				break;
			}

				/* The user pressed the cursor-up key to
				 * scroll through the command history.
				 */

			if(Code == CURSORUP)
			{
					/* Shift key: jump to first command
					 * history entry.
					 */

				if(Qualifier & SHIFT_KEY)
				{
					if(IsListEmpty(&PacketHistoryList))
						PacketLine = -1;
					else
					{
						NeedChange = TRUE;
						PacketLine = 0;
					}
				}
				else
				{
					if(PacketLine == -1)
					{
						NeedChange = TRUE;
						PacketLine = PacketCount - 1;
					}
					else
					{
						if(PacketLine > 0)
						{
							NeedChange = TRUE;
							PacketLine--;
						}
					}
				}
			}

				/* The user pressed the cursor-down key to
				 * scroll through the command history.
				 */

			if(Code == CURSORDOWN)
			{
				if(IsListEmpty(&PacketHistoryList))
					PacketLine = -1;
				else
				{
						/* Shift key: jump to last command
						 * history entry.
						 */

					if(Qualifier & SHIFT_KEY)
					{
						NeedChange = TRUE;
						PacketLine = PacketCount - 1;
					}
					else
					{
						if(PacketLine != -1 && PacketLine < PacketCount - 1)
						{
							NeedChange = TRUE;
							PacketLine++;
						}
						else
						{
							NeedChange = TRUE;
							PacketLine = -1;
						}
					}
				}
			}

			break;
	}

		/* Check if we have to update the string gadget contents. */

	if(NeedChange)
	{
		STRPTR String;

		String = "";

		if(PacketLine != -1)
		{
			if(Node = GetListNode(PacketLine,&PacketHistoryList))
			{
				if(HasList)
				{
					String = NULL;

					GT_SetGadgetAttrs(PacketGadgetArray[GAD_LIST],PacketWindow,NULL,
						GTLV_Selected,PacketLine,
					TAG_DONE);
				}
				else
					String = Node->ln_Name;
			}
		}

		if(String)
		{
			GT_SetGadgetAttrs(PacketGadgetArray[GAD_STRING],PacketWindow,NULL,
				GTST_String,String,
			TAG_DONE);
		}
	}

	if(Activate)
		ActivateGadget(PacketGadgetArray[GAD_STRING],PacketWindow,NULL);
}

	/* HandlePacket():
	 *
	 *	Process the input coming through the packet window.
	 */

STATIC BOOL
HandlePacket(VOID)
{
	ULONG Signals;

	while(TRUE)
	{
		Signals = Wait(PORTMASK(PacketWindow->UserPort) | SIG_KILL | SIGBREAKF_CTRL_F);

		if(Signals & SIG_KILL)
			return(TRUE);

		if(Signals & SIGBREAKF_CTRL_F)
		{
			WindowToFront(PacketWindow);
			ActivateWindow(PacketWindow);
		}

		if(Signals & PORTMASK(PacketWindow->UserPort))
		{
			struct IntuiMessage		*Message;
			ULONG					 MsgClass,MsgCode;
			struct Gadget			*MsgGadget;
			struct FileRequester	*FileRequest;
			UBYTE					 DummyBuffer[MAX_FILENAME_LENGTH];
			BOOL					 SwapWindow;

			SwapWindow = FALSE;

			while(Message = (struct IntuiMessage *)GT_GetIMsg(PacketWindow->UserPort))
			{
				MsgClass	= Message->Class;
				MsgCode		= Message->Code;
				MsgGadget	= (struct Gadget *)Message->IAddress;

				GT_ReplyIMsg(Message);

					/* Re-enable the string gadget if necessary. */

				if(MsgClass == IDCMP_RAWKEY)
				{
					if(MsgCode == (IECODE_UP_PREFIX|RAMIGA_CODE) && CommandWindow == PacketWindow)
						ActivateGadget(CommandGadget,PacketWindow,NULL);

					if(MsgCode == HELP_CODE)
						GuideDisplay(CONTEXT_PACKETWINDOW);
				}

					/* Menu help is required. */

				if(MsgClass == IDCMP_MENUHELP)
					GuideDisplay(CONTEXT_PACKET_MENU);

					/* Imagery may need refreshing */

				if(MsgClass == IDCMP_REFRESHWINDOW)
				{
					GT_BeginRefresh(PacketWindow);
					GT_EndRefresh(PacketWindow,TRUE);
				}

					/* Handle the menu. */

				if(MsgClass == IDCMP_MENUPICK)
				{
					struct MenuItem *MenuItem;

					while(MsgCode != MENUNULL)
					{
						MenuItem = ItemAddress(PacketMenu,MsgCode);

						switch((IPTR)GTMENUITEM_USERDATA(MenuItem))
						{
							case MEN_TOGGLE_ECHO:

								if(MenuItem->Flags & CHECKED)
									Echo = TRUE;
								else
									Echo = FALSE;

								break;

							case MEN_QUITPANEL:

								MsgClass = IDCMP_CLOSEWINDOW;
								break;

							case MEN_LOADHISTORY:

								LT_LockWindow(PacketWindow);

								DummyBuffer[0] = 0;

								if(FileRequest = OpenSingleFile(PacketWindow,LocaleString(MSG_PACKET_LOAD_HISTORY_TXT),NULL,NULL,DummyBuffer,sizeof(DummyBuffer)))
								{
									FreeAslRequest(FileRequest);

									if(GetFileSize(DummyBuffer))
									{
										BPTR SomeFile;

										if(SomeFile = Open(DummyBuffer,MODE_OLDFILE))
										{
											if(PacketLine)
											{
												switch(ShowRequest(PacketWindow,LocaleString(MSG_PACKET_PACKET_WINDOW_STILL_HOLDS_LINES_TXT),LocaleString(MSG_PACKET_DISCARD_APPEND_CANCEL_TXT),PacketLine))
												{
													case 1:

														ClearPacketHistory();
														break;

													case 0:

														Close(SomeFile);
														SomeFile = BNULL;
														break;
												}
											}

											if(SomeFile)
											{
												LONG Len;

												LineRead(BNULL,NULL,0);

												while(Len = LineRead(SomeFile,DummyBuffer,sizeof(DummyBuffer) - 1))
												{
													DummyBuffer[Len - 1] = 0;

													AddPacketHistory(DummyBuffer);
												}

												Close(SomeFile);
											}
										}
										else
											ShowError(PacketWindow,ERR_LOAD_ERROR,IoErr(),DummyBuffer);
									}
								}

								LT_UnlockWindow(PacketWindow);
								break;

							case MEN_SAVEHISTORY:

								LT_LockWindow(PacketWindow);

								if(!PacketLine)
									ShowRequest(PacketWindow,LocaleString(MSG_PACKET_NOTHING_IN_THE_BUFFER_TXT),LocaleString(MSG_GLOBAL_CONTINUE_TXT));
								else
								{
									DummyBuffer[0] = 0;

									if(FileRequest = SaveFile(PacketWindow,LocaleString(MSG_PACKET_SAVE_HISTORY_TXT),NULL,NULL,DummyBuffer,sizeof(DummyBuffer)))
									{
										BPTR SomeFile = BNULL;
										LONG Error = 0;

										FreeAslRequest(FileRequest);

											/* If the file we are about
											 * to create already exists,
											 * ask the user whether we are
											 * to create, append or skip
											 * the file.
											 */

										if(GetFileSize(DummyBuffer))
										{
											switch(ShowRequest(PacketWindow,LocaleString(MSG_GLOBAL_FILE_ALREADY_EXISTS_TXT),LocaleString(MSG_GLOBAL_CREATE_APPEND_CANCEL_TXT),DummyBuffer))
											{
												case 1:

													SomeFile = Open(DummyBuffer,MODE_NEWFILE);
													break;

												case 2:

													SomeFile = OpenToAppend(DummyBuffer,NULL);
													break;
											}
										}
										else
											SomeFile = Open(DummyBuffer,MODE_NEWFILE);

										if(!SomeFile)
											Error = IoErr();
										else
										{
											struct Node *SomeNode;

											ObtainSemaphore(&PacketSemaphore);

											for(SomeNode = PacketHistoryList.lh_Head ; SomeNode->ln_Succ ; SomeNode = SomeNode->ln_Succ)
											{
												SetIoErr(0);

												if(FPrintf(SomeFile,"%s\n",SomeNode->ln_Name) < 1)
												{
													Error = IoErr();

													break;
												}
											}

											Close(SomeFile);

											if(GetFileSize(DummyBuffer))
											{
												AddProtection(DummyBuffer,FIBF_EXECUTE);

												if(Config->MiscConfig->CreateIcons)
													AddIcon(DummyBuffer,FILETYPE_TEXT,TRUE);
											}
											else
												DeleteFile(DummyBuffer);

											ReleaseSemaphore(&PacketSemaphore);
										}

										if(Error)
											ShowError(PacketWindow,ERR_SAVE_ERROR,Error,DummyBuffer);
									}
								}

								LT_UnlockWindow(PacketWindow);
								break;

							case MEN_CLEARHISTORY:

								LT_LockWindow(PacketWindow);

								ClearPacketHistory();

								LT_UnlockWindow(PacketWindow);
								break;

							case MEN_OTHERWINDOW:

								SwapWindow = TRUE;
								break;
						}

						MsgCode = MenuItem->NextSelect;
					}

					ActivateGadget(PacketGadgetArray[GAD_STRING],PacketWindow,NULL);
				}

					/* Shut down. */

				if(MsgClass == IDCMP_CLOSEWINDOW)
				{
					Forbid();

					if(!(SetSignal(0,SIG_KILL) & SIG_KILL))
						return(FALSE);
					else
						return(TRUE);
				}

					/* Activate the string gadget as well. */

				if(MsgClass == IDCMP_ACTIVEWINDOW)
					ActivateGadget(PacketGadgetArray[GAD_STRING],PacketWindow,NULL);

				if(MsgClass == IDCMP_NEWSIZE)
				{
					PacketWindow->Flags |= WFLG_RMBTRAP;

					strcpy(PacketTempBuffer,PacketContents);

					RemoveGList(PacketWindow,PacketGadgetList,(UWORD)-1);

					FreeGadgets(PacketGadgetList);
					PacketGadgetList = NULL;

					EraseWindow(PacketWindow,NULL);
					RefreshWindowFrame(PacketWindow);

					ObtainSemaphore(&PacketSemaphore);

					if(CreateAllGadgets(PacketWindow->Width,PacketWindow->Height,NULL,PacketGadgetArray,&PacketGadgetList,VisualInfo))
					{
						PacketContents = ((struct StringInfo *)PacketGadgetArray[GAD_STRING]->SpecialInfo)->Buffer;

						AddGList(PacketWindow,PacketGadgetList,(UWORD)-1,(UWORD)-1,NULL);
						RefreshGList(PacketGadgetList,PacketWindow,NULL,(UWORD)-1);
						GT_RefreshWindow(PacketWindow,NULL);

						ReleaseSemaphore(&PacketSemaphore);

						PacketWindow->Flags &= ~WFLG_RMBTRAP;

						GT_SetGadgetAttrs(PacketGadgetArray[GAD_STRING],PacketWindow,NULL,
							GTST_String,	PacketTempBuffer,
						TAG_DONE);

						ActivateGadget(PacketGadgetArray[GAD_STRING],PacketWindow,NULL);
					}
					else
					{
						DisplayBeep(PacketWindow->WScreen);

						ReleaseSemaphore(&PacketSemaphore);

						if(!(SetSignal(0,SIG_KILL) & SIG_KILL))
							return(FALSE);
						else
							return(TRUE);
					}
				}

				if(MsgClass == IDCMP_MOUSEBUTTONS)
					ActivateGadget(PacketGadgetArray[GAD_STRING],PacketWindow,NULL);

					/* User has entered a string. */

				if(MsgClass == IDCMP_GADGETUP)
				{
					switch(MsgGadget->GadgetID)
					{
						case GAD_STRING:

							HandlePacketGadgetEvent(MsgCode);
							break;

						case GAD_LIST:

							ActivateGadget(PacketGadgetArray[GAD_STRING],PacketWindow,NULL);
							break;
					}
				}

				if(SwapWindow)
				{
					BumpWindow(Window);

					SwapWindow = FALSE;
				}
			}
		}
	}
}

STATIC VOID SAVE_DS
PacketProcessEntry(VOID)
{
	BOOL RingBack = TRUE;

	if(PacketTempBuffer = (STRPTR)AllocVecPooled(MAX_PACKET_LINE_LENGTH,MEMF_ANY))
	{
		if((PacketSignal = AllocSignal(-1)) != -1)
		{
			STATIC IPTR MenuTags[] =
			{
				LAMN_TitleID,			MSG_PACKET_PROJECT_MEN,
					LAMN_ItemID,		MSG_PACKET_LOAD_HISTORY_MEN,
						LAMN_UserData,	MEN_LOADHISTORY,
					LAMN_ItemID,		MSG_PACKET_SAVE_HISTORY_MEN,
						LAMN_UserData,	MEN_SAVEHISTORY,
					LAMN_ItemID,		MSG_PACKET_CLEAR_HISTORY_MEN,
						LAMN_UserData,	MEN_CLEARHISTORY,

					LAMN_ItemText,		(IPTR)NM_BARLABEL,

					LAMN_ItemID,		MSG_PACKET_OTHER_WINDOW_MEN,
						LAMN_UserData,	MEN_OTHERWINDOW,

					LAMN_ItemText,		(IPTR)NM_BARLABEL,

					LAMN_ItemID,		MSG_PACKET_TOGGLE_ECHO_MEN,
						LAMN_UserData,	MEN_TOGGLE_ECHO,
						LAMN_Toggle,	TRUE,

					LAMN_ItemText,		(IPTR)NM_BARLABEL,

					LAMN_ItemID,		MSG_PACKET_QUIT_MEN,
						LAMN_UserData,	MEN_QUITPANEL,

				TAG_DONE
			};

			if(PacketMenu = LT_NewMenuTags(
				LAHN_LocaleHook,		&LocaleHook,
				LAMN_Screen,			Window->WScreen,
				LAMN_TextAttr,			Window->WScreen->Font,
				LAMN_AmigaGlyph,		AmigaGlyph,
				LAMN_CheckmarkGlyph,	CheckGlyph,
			TAG_MORE,MenuTags))
			{
				LONG PacketLeft,PacketTop,PacketWidth,PacketHeight,MinHeight;

				PacketLeft		= Window->LeftEdge;
				PacketTop		= Window->TopEdge + Window->Height;
				PacketWidth		= Window->Width;
				PacketHeight	= Window->WScreen->WBorTop + Window->WScreen->Font->ta_YSize + 1 + 1 + 3 + TextAttr.tta_YSize + 3 + 1 + Window->WScreen->WBorBottom;

				GetWindowInfo(WINDOW_PACKET,&PacketLeft,&PacketTop,&PacketWidth,&PacketHeight,PacketWidth,PacketHeight);

				if(CreateAllGadgets(PacketWidth,PacketHeight,&MinHeight,PacketGadgetArray,&PacketGadgetList,VisualInfo))
				{
					if(PacketWindow = OpenWindowTags(NULL,
						WA_Width,			PacketWidth,
						WA_Height,			PacketHeight,

						WA_Left,			PacketLeft,
						WA_Top,				PacketTop,

						WA_Activate,		TRUE,
						WA_DragBar,			TRUE,
						WA_DepthGadget,		TRUE,
						WA_CloseGadget,		TRUE,
						WA_RMBTrap,			TRUE,
						WA_SizeGadget,		TRUE,
						WA_MinWidth,		Window->BorderLeft+80+Window->BorderRight,
						WA_MinHeight,		MinHeight,
						WA_MaxWidth,		Window->WScreen->Width,
						WA_MaxHeight,		Window->WScreen->Height,
						OpenWindowTag,		Window->WScreen,
						WA_NewLookMenus,	TRUE,
						WA_SimpleRefresh,	TRUE,
						BackfillTag,		&BackfillHook,

						WA_IDCMP,			STRINGIDCMP | LISTVIEWIDCMP | IDCMP_NEWSIZE | IDCMP_SIZEVERIFY | IDCMP_CLOSEWINDOW | IDCMP_MENUPICK | IDCMP_ACTIVEWINDOW | IDCMP_RAWKEY | IDCMP_MOUSEBUTTONS | IDCMP_MENUHELP | IDCMP_REFRESHWINDOW,
						WA_MenuHelp,		TRUE,

						WA_Title,			LocaleString(MSG_GLOBAL_PACKET_WINDOW_TXT),

						AmigaGlyph ? WA_AmigaKey  : TAG_IGNORE, AmigaGlyph,
						CheckGlyph ? WA_Checkmark : TAG_IGNORE, CheckGlyph,
					TAG_DONE))
					{
						APTR OldPtr;

						PacketContents = ((struct StringInfo *)PacketGadgetArray[GAD_STRING]->SpecialInfo)->Buffer;

						AddGList(PacketWindow,PacketGadgetList,(UWORD)-1,(UWORD)-1,NULL);
						RefreshGList(PacketGadgetList,PacketWindow,NULL,(UWORD)-1);
						GT_RefreshWindow(PacketWindow,NULL);

						SetMenuStrip(PacketWindow,PacketMenu);

						PacketWindow->Flags &= ~WFLG_RMBTRAP;

						ActivateGadget(PacketGadgetArray[GAD_STRING],PacketWindow,NULL);

						PacketProcess = (struct Process *)FindTask(NULL);

						ChangeWindowPtr(&OldPtr,(APTR)PacketWindow);

						Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);

						RingBack = HandlePacket();

						RestoreWindowPtr(OldPtr);

						PacketWindow->Flags |= WFLG_RMBTRAP;

						ClearMenuStrip(PacketWindow);

						RemoveGList(PacketWindow,PacketGadgetList,(UWORD)-1);

						PutWindowInfo(WINDOW_PACKET,PacketWindow->LeftEdge,PacketWindow->TopEdge,PacketWindow->Width,PacketWindow->Height);

						LT_DeleteWindowLock(PacketWindow);

						CloseWindow(PacketWindow);
						PacketWindow = NULL;
					}

					FreeGadgets(PacketGadgetList);
					PacketGadgetList = NULL;
				}

				LT_DisposeMenu(PacketMenu);
				PacketMenu = NULL;
			}

			FreeSignal(PacketSignal);
			PacketSignal = -1;
		}

		FreeVecPooled(PacketTempBuffer);
	}

	Forbid();

	PacketProcess = NULL;

	if(RingBack)
		Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);
	else
		CheckItem(MEN_PACKET_WINDOW,FALSE);
}

	/* ClearPacketHistory():
	 *
	 *	Release the command history.
	 */

VOID
ClearPacketHistory()
{
	if(PacketProcess)
	{
		ObtainSemaphore(&PacketSemaphore);

		if(HasList)
		{
			GT_SetGadgetAttrs(PacketGadgetArray[GAD_LIST],PacketWindow,NULL,
				GTLV_Labels,~0,
			TAG_DONE);
		}

		FreeList(&PacketHistoryList);

		PacketCount	= 0;
		PacketLine	= -1;

		if(HasList)
		{
			GT_SetGadgetAttrs(PacketGadgetArray[GAD_LIST],PacketWindow,NULL,
				GTLV_Labels,&PacketHistoryList,
			TAG_DONE);
		}

		ReleaseSemaphore(&PacketSemaphore);
	}
}

	/* AddPacketHistory(STRPTR Buffer):
	 *
	 *	Add a line to the packet window command history. This
	 *	works very much the same as the AddLine()-routine.
	 */

VOID
AddPacketHistory(STRPTR Buffer)
{
	if(PacketProcess)
	{
		struct Node *Node;

		ObtainSemaphore(&PacketSemaphore);

		if(HasList)
		{
			GT_SetGadgetAttrs(PacketGadgetArray[GAD_LIST],PacketWindow,NULL,
				GTLV_Labels, ~0,
			TAG_DONE);
		}

		if(Node = CreateNode(Buffer))
		{
			AddTail(&PacketHistoryList,Node);

			if(HasList)
			{
				GT_SetGadgetAttrs(PacketGadgetArray[GAD_LIST],PacketWindow,NULL,
					GTLV_Top,		PacketCount,
					GTLV_Labels,	&PacketHistoryList,
				TAG_DONE);
			}

			PacketCount++;
		}
		else
		{
			if(HasList)
			{
				GT_SetGadgetAttrs(PacketGadgetArray[GAD_LIST],PacketWindow,NULL,
					GTLV_Labels, &PacketHistoryList,
				TAG_DONE);
			}
		}

		ReleaseSemaphore(&PacketSemaphore);
	}
}

	/* DeletePacketWindow():
	 *
	 *	Delete the packet window and release the command
	 *	history.
	 */

VOID
DeletePacketWindow(BOOL WindowOnly)
{
	CheckItem(MEN_PACKET_WINDOW,FALSE);

	if(PacketProcess)
	{
		ShakeHands((struct Task *)PacketProcess,SIG_KILL);

		if(!WindowOnly)
		{
			HasList = FALSE;

			FreeList(&PacketHistoryList);

			PacketCount	= -1;
			PacketLine	= 0;
		}
	}
}

	/* CreatePacketWindow():
	 *
	 *	Open the packet window and allocate the command history
	 *	buffer.
	 */

BOOL
CreatePacketWindow()
{
	BOOL Result;

	if(Echo == -1)
		Echo = Config->SerialConfig->Duplex;

	if(!PacketProcess)
	{
		InitSemaphore(&PacketSemaphore);

		StartProcessWaitForHandshake("term Packet Process",(TASKENTRY)PacketProcessEntry,TAG_DONE);
	}

	Forbid();

	if(PacketProcess)
	{
		Signal((struct Task *)PacketProcess,SIGBREAKF_CTRL_F);

		Result = TRUE;
	}
	else
		Result = FALSE;

	Permit();

	CheckItem(MEN_PACKET_WINDOW,Result);
	return(Result);
}

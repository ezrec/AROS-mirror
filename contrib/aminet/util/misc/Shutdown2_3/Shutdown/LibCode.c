/*
**	Shutdown 2.0 package, LibCode.c module
**
**	Copyright © 1992 by Olaf `Olsen' Barthel
**		All Rights Reserved
*/

#include "ShutdownGlobal.h"

	/* Gadget IDs... */

enum	{	GAD_LIST, GAD_RETURN, GAD_SHUTDOWN };

	/* User interface data. */

STATIC struct Window			*Window;
STATIC struct Screen			*Screen;
STATIC APTR				 VisualInfo;
STATIC struct Gadget			*GadgetList,
					*GadgetArray[GAD_SHUTDOWN + 1];
STATIC struct RastPort			*RPort;

STATIC struct List			 FileList;

STATIC BYTE				 Blocking	= FALSE,
					 Accessing	= FALSE,
					 WindowMode;

STATIC UWORD				 ButtonWidth,
					 DisplayWidth,
					 FontHeight,
					 WindowWidth,
					 WindowHeight;

STATIC BYTE DisplayStrings[7] =
{
	MSG_TURNOFF_TXT, MSG_WAIT_A_MINUTE_TXT, MSG_FILESOPEN1_TXT,
	MSG_FILESOPEN2_TXT, MSG_WORKING1_TXT, MSG_WORKING2_TXT,
	MSG_RESET_TXT
};

STATIC struct TextAttr *DisplayFont;

STATIC struct IntuiText ResetText =
{
	1,0,
	JAM1,
	0,0,
	NULL,
	NULL,
	NULL
};

STATIC UWORD __chip BlankSprite[(2 + 1) * 2] =
{
	0x0000,0x0000,

	0x0000,0x0000,

	0x0000,0x0000
};

STATIC UWORD __chip Stopwatch[(2 + 16) * 2] =
{
	0x0000,0x0000,

	0x0400,0x07C0,
	0x0000,0x07C0,
	0x0100,0x0380,
	0x0000,0x07E0,
	0x07C0,0x1FF8,
	0x1FF0,0x3FEC,
	0x3FF8,0x7FDE,
	0x3FF8,0x7FBE,
	0x7FFC,0xFF7F,
	0x7EFC,0xFFFF,
	0x7FFC,0xFFFF,
	0x3FF8,0x7FFE,
	0x3FF8,0x7FFE,
	0x1FF0,0x3FFC,
	0x07C0,0x1FF8,
	0x0000,0x07E0,

	0x0000,0x0000
};

	/* Local routines for this module. */

STATIC VOID __regargs		CloseDown(BYTE Fast);
STATIC VOID __regargs		ClearList(struct List *List);
STATIC struct Node * __regargs	CreateNode(STRPTR Name);
STATIC VOID __regargs		ShowText(STRPTR String);
STATIC BYTE __regargs		OpenShutdownWindow(BYTE Mode);
STATIC struct Gadget *		CreateAllGadgets(struct Gadget **GadgetArray,struct Gadget **GadgetList,BYTE UseList);
STATIC VOID			CloseAll(VOID);

STATIC VOID __regargs
CloseDown(BYTE Fast)
{
	STATIC struct Requester Requester;

	struct FileInfo		*Node,
				*Next;

	struct ShutdownInfo	*Info,
				*NextInfo;

	STRPTR			 Message;

	ReleaseSemaphore(&GlobalBase -> DevBlockLock);

	ObtainSemaphore(&DeviceWatchSemaphore);

	if(DeviceInUse)
		Message = GetString(MSG_WAIT_A_MINUTE_TXT);
	else
	{
		if(!Fast)
			Message = GetString(MSG_TURNOFF_TXT);
		else
			Message = NULL;
	}

	ReleaseSemaphore(&DeviceWatchSemaphore);

	if(Message)
		ShowText(Message);

	if(!Fast)
		SetPointer(Window,&Stopwatch[0],16,16,-6,0);

	memset(&Requester,0,sizeof(struct Requester));

	Request(&Requester,Window);

	ObtainSemaphore(&GlobalBase -> AccessLock);

	GlobalBase -> Closing = TRUE;

	Info = (struct ShutdownInfo *)GlobalBase -> ShutdownList . mlh_Head;

	ReleaseSemaphore(&GlobalBase -> AccessLock);

	if(Info -> sdi_Node . mln_Succ)
	{
		ULONG Mode;

		GlobalBase -> Shutdown = FALSE;

		ReleaseSemaphore(&GlobalBase -> BlockLock);

		while(NextInfo = (struct ShutdownInfo *)Info -> sdi_Node . mln_Succ)
		{
			Mode = SD_EXIT;

			if(Fast)
				CallHook(Info -> sdi_Hook,&Mode,SDF_RESET);
			else
				CallHook(Info -> sdi_Hook,&Mode,NULL);

			Info = NextInfo;
		}

		ObtainSemaphore(&GlobalBase -> BlockLock);

		GlobalBase -> Shutdown = TRUE;
	}

	ObtainSemaphore(&GlobalBase -> AccessLock);

	Node = (struct FileInfo *)GlobalBase -> AccessList . mlh_Head;

	while(Next = (struct FileInfo *)Node -> Node . mln_Succ)
	{
		if(Node -> Accesses && Node -> Handle)
			Flush(Node -> Handle);

		Node = Next;
	}

	ReleaseSemaphore(&GlobalBase -> AccessLock);

	ObtainSemaphore(&DeviceWatchSemaphore);

	if(DeviceInUse)
		Message = GetString(MSG_WAIT_A_MINUTE_TXT);
	else
	{
		if(Fast)
			Message = GetString(MSG_RESET_TXT);
		else
			Message = GetString(MSG_TURNOFF_TXT);
	}

	ReleaseSemaphore(&DeviceWatchSemaphore);

	ShowText(Message);

	Forbid();

	GlobalBase -> DevShutdown = TRUE;

	Wait(SIG_NOTIFY);

	ObtainSemaphore(&GlobalBase -> DevBlockLock);

	if(Fast)
		ShowText(GetString(MSG_RESET_TXT));
	else
		ShowText(GetString(MSG_TURNOFF_TXT));

	TimeDelay(UNIT_VBLANK,1,0);

	ColdReboot();
}

STATIC VOID __regargs
ClearList(struct List *List)
{
	struct Node	*Node,
			*Next;

	Node = List -> lh_Head;

	while(Next = Node -> ln_Succ)
	{
		Remove(Node);

		FreeVec(Node);

		Node = Next;
	}
}

STATIC struct Node * __regargs
CreateNode(STRPTR Name)
{
	struct Node *Node;

	if(Node = (struct Node *)AllocVec(sizeof(struct Node) + strlen(Name) + 1,MEMF_ANY | MEMF_CLEAR))
	{
		Node -> ln_Name = (STRPTR)(Node + 1);

		strcpy(Node -> ln_Name,Name);
	}

	return(Node);
}

STATIC VOID __regargs
ShowText(STRPTR String)
{
	ResetText . ITextFont	= DisplayFont;
	ResetText . IText	= String;

	ClipBlit(Window -> RPort,0,0,Window -> RPort,Screen -> WBorLeft + INTERWIDTH + 2,Screen -> WBorTop + INTERHEIGHT + 1,DisplayWidth - 4,INTERHEIGHT + FontHeight + INTERHEIGHT,0x00);

	PrintIText(Window -> RPort,&ResetText,(Window -> Width - IntuiTextLength(&ResetText)) / 2,Screen -> WBorTop + INTERHEIGHT + 1 + INTERHEIGHT);
}

STATIC BYTE __regargs
OpenShutdownWindow(BYTE Mode)
{
	STRPTR Message;

	WindowWidth = Screen -> WBorLeft + INTERWIDTH + DisplayWidth + INTERWIDTH + Screen -> WBorRight;

	switch(Mode)
	{
		case MODE_SHUTDOWN:	WindowHeight = Screen -> WBorTop + INTERHEIGHT + 1 + INTERHEIGHT + FontHeight + INTERHEIGHT + 1 + INTERHEIGHT + 2 + FontHeight + 2 + INTERHEIGHT + Screen -> WBorBottom;
					break;

		case MODE_RESET:
		case MODE_FASTRESET:	WindowHeight = Screen -> WBorTop + INTERHEIGHT + 1 + INTERHEIGHT + FontHeight + INTERHEIGHT + 1 + INTERHEIGHT + Screen -> WBorBottom;
					break;

		default:		WindowHeight = Screen -> WBorTop + INTERHEIGHT + 1 + INTERHEIGHT + FontHeight + INTERHEIGHT + 1 + INTERHEIGHT + 2 + 16 * FontHeight + 2 + INTERHEIGHT + 2 + FontHeight + 2 + INTERHEIGHT + Screen -> WBorBottom;
					break;
	}

	if(WindowWidth > Screen -> Width || WindowHeight > Screen -> Height)
		return(FALSE);

	WindowMode = Mode;

	switch(Mode)
	{
		case MODE_SHUTDOWN:	if(!CreateAllGadgets(&GadgetArray[0],&GadgetList,FALSE))
					{
						FreeGadgets(GadgetList);

						return(FALSE);
					}
					else
						break;

		case MODE_LIST:
		case MODE_NAME:		if(!CreateAllGadgets(&GadgetArray[0],&GadgetList,TRUE))
					{
						FreeGadgets(GadgetList);

						return(FALSE);
					}
					else
						break;

		default:		GadgetList = NULL;
					break;
	}

	if(!(Window = OpenWindowTags(NULL,
		WA_Left,		(Screen -> Width - WindowWidth) / 2,
		WA_Top,			(Screen -> Height - WindowHeight) / 2,
		WA_Width,		WindowWidth,
		WA_Height,		WindowHeight,
		WA_IDCMP,		IDCMP_VANILLAKEY | BUTTONIDCMP | LISTVIEWIDCMP,
		WA_RMBTrap,		TRUE,
		WA_CustomScreen,	Screen,
	TAG_DONE)))
		return(FALSE);

	ScreenToFront(Screen);

	switch(Mode)
	{
		case MODE_SHUTDOWN:	ObtainSemaphore(&DeviceWatchSemaphore);

					if(DeviceInUse)
						Message = GetString(MSG_WAIT_A_MINUTE_TXT);
					else
						Message = GetString(MSG_TURNOFF_TXT);

					ReleaseSemaphore(&DeviceWatchSemaphore);

					break;

		case MODE_LIST:		if(FileList . lh_Head -> ln_Succ -> ln_Succ)
						Message = GetString(MSG_FILESOPEN2_TXT);
					else
						Message = GetString(MSG_FILESOPEN1_TXT);

					break;

		case MODE_NAME:		if(FileList . lh_Head -> ln_Succ -> ln_Succ)
						Message = GetString(MSG_WORKING2_TXT);
					else
						Message = GetString(MSG_WORKING1_TXT);

					break;

		case MODE_RESET:	Message = GetString(MSG_RESET_TXT);

					SetPointer(Window,BlankSprite,1,16,0,0);

					break;

		case MODE_FASTRESET:	ObtainSemaphore(&DeviceWatchSemaphore);

					if(DeviceInUse)
						Message = GetString(MSG_WAIT_A_MINUTE_TXT);
					else
						Message = GetString(MSG_RESET_TXT);

					ReleaseSemaphore(&DeviceWatchSemaphore);

					SetPointer(Window,BlankSprite,1,16,0,0);

					break;
	}

	if(GadgetList)
	{
		AddGList(Window,GadgetList,(UWORD)-1,(UWORD)-1,NULL);
		RefreshGList(GadgetList,Window,NULL,(UWORD)-1);
		GT_RefreshWindow(Window,NULL);
	}

	DrawBevelBox(Window -> RPort,Screen -> WBorLeft + INTERWIDTH,Screen -> WBorTop + INTERHEIGHT,DisplayWidth,1 + INTERHEIGHT + FontHeight + INTERHEIGHT + 1,
		GTBB_Recessed,	TRUE,
		GT_VisualInfo,	VisualInfo,
	TAG_DONE);

	ShowText(Message);

	return(TRUE);
}

STATIC struct Gadget *
CreateAllGadgets(struct Gadget **GadgetArray,struct Gadget **GadgetList,BYTE UseList)
{
	struct Gadget		*Gadget;
	struct NewGadget	 NewGadget;
	UWORD			 Counter;

	memset(&NewGadget,0,sizeof(struct NewGadget));

	if(Gadget = CreateContext(GadgetList))
	{
		NewGadget . ng_TextAttr		= DisplayFont;
		NewGadget . ng_VisualInfo	= VisualInfo;
		NewGadget . ng_Flags		= NULL;

		if(UseList)
		{
			Counter = 0;

			NewGadget . ng_LeftEdge		= Screen -> WBorLeft + INTERWIDTH;
			NewGadget . ng_TopEdge		= Screen -> WBorTop + INTERHEIGHT + 1 + INTERHEIGHT + FontHeight + INTERHEIGHT + 1 + INTERHEIGHT;
			NewGadget . ng_GadgetID		= Counter;
			NewGadget . ng_Width		= DisplayWidth;
			NewGadget . ng_Height		= 2 + 16 * FontHeight + 2;

			GadgetArray[Counter++] = Gadget = CreateGadget(LISTVIEW_KIND,Gadget,&NewGadget,
				GTLV_Labels,	&FileList,
				GTLV_ReadOnly,	TRUE,
			TAG_DONE);

			NewGadget . ng_TopEdge		= NewGadget . ng_TopEdge + NewGadget . ng_Height + INTERHEIGHT;
		}
		else
		{
			Counter = GAD_RETURN;

			NewGadget . ng_LeftEdge		= Screen -> WBorLeft + INTERWIDTH;
			NewGadget . ng_TopEdge		= Screen -> WBorTop + INTERHEIGHT + 1 + INTERHEIGHT + FontHeight + INTERHEIGHT + 1 + INTERHEIGHT;
		}

		NewGadget . ng_GadgetText	= GetString(MSG_RETURN_TXT);
		NewGadget . ng_GadgetID		= Counter;
		NewGadget . ng_Width		= ButtonWidth;
		NewGadget . ng_Height		= 2 + FontHeight + 2;

		GadgetArray[Counter++] = Gadget = CreateGadget(BUTTON_KIND,Gadget,&NewGadget,
			GT_Underscore,	'_',
		TAG_DONE);

		NewGadget . ng_GadgetText	= GetString(MSG_RESTART_TXT);
		NewGadget . ng_GadgetID		= Counter;
		NewGadget . ng_LeftEdge		= WindowWidth - (Screen -> WBorLeft + INTERWIDTH + NewGadget . ng_Width);

		GadgetArray[Counter] = Gadget = CreateGadget(BUTTON_KIND,Gadget,&NewGadget,
			GT_Underscore,	'_',
		TAG_DONE);
	}

	return(Gadget);
}

STATIC VOID
CloseAll()
{
	MainProcess = NULL;

	if(NotifySignal)
	{
		SetSignal(0,SIG_NOTIFY);

		FreeSignal(NotifySignal);
	}

	if(Window)
		CloseWindow(Window);

	if(Screen)
		CloseScreen(Screen);

	if(GadgetList)
		FreeGadgets(GadgetList);

	if(VisualInfo)
		FreeVisualInfo(VisualInfo);

	ClearList(&FileList);

	if(Blocking)
	{
		Forbid();

		GlobalBase -> Shutdown = FALSE;

		ReleaseSemaphore(&GlobalBase -> BlockLock);
		ReleaseSemaphore(&GlobalBase -> DevBlockLock);

		Permit();
	}

	if(Accessing)
		ReleaseSemaphore(&GlobalBase -> AccessLock);
}

BYTE
OpenAll(BYTE Mode)
{
	struct Screen	*PublicScreen;
	ULONG		 DisplayID;
	UWORD		 Dummy = (UWORD)~0,Depth;
	BYTE		 Done = TRUE;
	UBYTE		 Char;
	UWORD		 MaxWidth = 0,
			 Width,
			 i;
	STRPTR		 String;

	NewList(&FileList);

	if(!(PublicScreen = LockPubScreen(NULL)))
		return(FALSE);

	DisplayID = GetVPModeID(&PublicScreen -> ViewPort);

	if((Depth = PublicScreen -> RastPort . BitMap -> Depth) > 2)
		Depth = 2;

	UnlockPubScreen(NULL,PublicScreen);

	if(!(Screen = OpenScreenTags(NULL,
		SA_Behind,				TRUE,
		SA_Quiet,				TRUE,
		SA_DisplayID,				DisplayID,
		SA_Depth,				Depth,
		SA_Overscan,				OSCAN_TEXT,
		Depth > 1 ? SA_Pens : TAG_IGNORE,	&Dummy,
		SA_SysFont,				1,
	TAG_DONE)))
		return(FALSE);

	RPort = &Screen -> RastPort;

	DisplayFont = Screen -> Font;

	for(i = 32 ; i < 127 ; i++)
	{
		Char = i;

		if((Width = TextLength(RPort,&Char,1)) > MaxWidth)
			MaxWidth = Width;
	}

	for(i = 160 ; i < 256 ; i++)
	{
		Char = i;

		if((Width = TextLength(RPort,&Char,1)) > MaxWidth)
			MaxWidth = Width;
	}

	ButtonWidth = TextLength(RPort,GetString(MSG_RETURN_TXT),strlen(GetString(MSG_RETURN_TXT)));

	if((Width = TextLength(RPort,GetString(MSG_RESTART_TXT),strlen(GetString(MSG_RESTART_TXT)))) > ButtonWidth)
		ButtonWidth = Width;

	DisplayWidth = 4 + 32 * MaxWidth + 4 + 16;

	for(i = 0 ; i < 7 ; i++)
	{
		String = GetString(DisplayStrings[i]);

		if((Width = TextLength(RPort,String,strlen(String))) > DisplayWidth)
			DisplayWidth = Width;
	}

	ButtonWidth	+= 2 * INTERWIDTH;
	DisplayWidth	+= 2 * INTERWIDTH;

	if(2 * ButtonWidth + INTERWIDTH > DisplayWidth)
		DisplayWidth = 2 * ButtonWidth + INTERWIDTH;

	FontHeight = RPort -> Font -> tf_YSize;

	if(!(VisualInfo = GetVisualInfo(Screen,TAG_DONE)))
		return(FALSE);

	if((NotifySignal = AllocSignal(-1)) == -1)
		return(FALSE);

	switch(Mode)
	{
		case MODE_RESET:

			if(!OpenShutdownWindow(MODE_RESET))
				return(FALSE);
			else
				break;

		case MODE_FASTRESET:

			Blocking = TRUE;

			ObtainSemaphore(&GlobalBase -> BlockLock);

			ObtainSemaphore(&GlobalBase -> DevBlockLock);

			GlobalBase -> Shutdown = TRUE;

			if(!OpenShutdownWindow(MODE_FASTRESET))
				return(FALSE);
			else
				break;

		default:

			Blocking = Accessing = TRUE;

			ObtainSemaphore(&GlobalBase -> AccessLock);

			ObtainSemaphore(&GlobalBase -> BlockLock);

			ObtainSemaphore(&GlobalBase -> DevBlockLock);

			GlobalBase -> Shutdown = TRUE;

			if(GlobalBase -> ShutdownList . mlh_Head -> mln_Succ)
			{
				struct ShutdownInfo	*Info,
							*NextInfo;

				struct Node		*Node;
				ULONG			 Mode;

				Info = (struct ShutdownInfo *)GlobalBase -> ShutdownList . mlh_Head;

				while(NextInfo = (struct ShutdownInfo *)Info -> sdi_Node . mln_Succ)
				{
					Mode = SD_CHECK;

					if(!CallHook(Info -> sdi_Hook,&Mode,NULL))
					{
						if(Node = CreateNode(Info -> sdi_Name))
							AddTail(&FileList,Node);

						Done = FALSE;
					}

					Info = NextInfo;
				}
			}

			if(Done)
			{
				if(GlobalBase -> OpenCount)
				{
					struct Node 	*New;
					struct FileInfo	*Node,
							*Next;

					Node = (struct FileInfo *)GlobalBase -> AccessList . mlh_Head;

					while(Next = (struct FileInfo *)Node -> Node . mln_Succ)
					{
						if(Node -> Accesses && !Node -> Ignore)
						{
							if(New = CreateNode(Node -> Name))
								AddTail(&FileList,New);
						}

						Node = Next;
					}

					if(FileList . lh_Head -> ln_Succ)
					{
						if(!OpenShutdownWindow(MODE_LIST))
							return(FALSE);
					}
					else
					{
						if(!OpenShutdownWindow(MODE_SHUTDOWN))
							return(FALSE);
					}
				}
				else
				{
					if(!OpenShutdownWindow(MODE_SHUTDOWN))
						return(FALSE);
				}
			}
			else
			{
				if(!OpenShutdownWindow(MODE_NAME))
					return(FALSE);
			}

			ReleaseSemaphore(&GlobalBase -> AccessLock);

			Accessing = FALSE;

			break;
	}

	ScreenToFront(Screen);
	ActivateWindow(Window);

	return(TRUE);
}

LONG __saveds __asm
RexxDispatch(register __a0 struct RexxMsg *Message)
{
	STRPTR	Arg	= NULL;
	LONG	Result	= RC_OK;

	if(GlobalBase -> Child)
	{
		if(!Stricmp(Message -> rm_Args[0],"shutdown"))
		{
			if(Message -> rm_Args[1])
			{
				if(!Stricmp(Message -> rm_Args[1],"immediately"))
				{
					if(!Shutdown(SHUTDOWN_FAST))
						Result = RC_ERROR;
				}
				else
				{
					if(!Shutdown(SHUTDOWN_NORMAL))
						Result = RC_ERROR;
					else
						Arg = CreateArgstring("aborted",7);
				}
			}
			else
			{
				if(!Shutdown(SHUTDOWN_NORMAL))
					Result = RC_ERROR;
				else
					Arg = CreateArgstring("aborted",7);
			}
		}
		else
			Result = 1;
	}

	putreg(REG_A0,(LONG)Arg);

	return(Result);
}

LONG __saveds __asm
Shutdown(register __d0 ULONG Mode)
{
	if(!GlobalBase -> Child)
		return(FALSE);
	else
	{
		if(!AttemptSemaphore(&GlobalBase -> ShutdownLock))
			return(FALSE);
		else
		{
			if(Mode == SHUTDOWN_FAST)
			{
				if(!OpenAll(MODE_FASTRESET))
				{
					CloseAll();

					ReleaseSemaphore(&GlobalBase -> ShutdownLock);

					return(FALSE);
				}
				else
				{
					MainProcess = (struct Process *)SysBase -> ThisTask;

					CloseDown(TRUE);
				}
			}

			if(Mode == SHUTDOWN_NORMAL)
			{
				if(!OpenAll(MODE_SHUTDOWN))
				{
					CloseAll();

					ReleaseSemaphore(&GlobalBase -> ShutdownLock);

					return(FALSE);
				}
				else
				{
					struct IntuiMessage	*Massage;
					ULONG			 InputClass,
								 Code,
								 SignalSet;
					struct Gadget		*Gadget;
					BYTE			 Terminated = FALSE;

					MainProcess = (struct Process *)SysBase -> ThisTask;

					while(!Terminated)
					{
						SignalSet = Wait(SIG_WINDOW | SIG_NOTIFY);

						if(SignalSet & SIG_NOTIFY)
						{
							if(WindowMode == MODE_SHUTDOWN)
							{
								STRPTR Message;

								if(DeviceProbablyInUse)
									Message = GetString(MSG_WAIT_A_MINUTE_TXT);
								else
									Message = GetString(MSG_TURNOFF_TXT);

								ShowText(Message);
							}
						}

						if(SignalSet & SIG_WINDOW)
						{
							while(Massage = GT_GetIMsg(Window -> UserPort))
							{
								InputClass	= Massage -> Class;
								Code		= Massage -> Code;
								Gadget		= (struct Gadget *)Massage -> IAddress;

								GT_ReplyIMsg(Massage);

								if(InputClass == IDCMP_VANILLAKEY)
								{
									UBYTE	*W_Code	= GetString(MSG_RETURN_SHORTCUT),
										*R_Code	= GetString(MSG_RESTART_SHORTCUT);

									Code = ToUpper(Code);

									if(Code == *W_Code)
										Terminated = TRUE;
									else
									{
										if(Code == *R_Code)
											CloseDown(FALSE);
									}
								}

								if(InputClass == IDCMP_GADGETUP)
								{
									switch(Gadget -> GadgetID)
									{
										case GAD_RETURN:	Terminated = TRUE;
													break;

										case GAD_SHUTDOWN:	CloseDown(FALSE);

										default:		break;
									}
								}
							}
						}
					}

					CloseAll();

					ReleaseSemaphore(&GlobalBase -> ShutdownLock);

					return(TRUE);
				}
			}
		}
	}

	return(FALSE);
}

APTR __saveds __asm
AddShutdownInfoTagList(register __a0 struct Hook *Hook,register __a1 STRPTR Name,register __a2 struct TagItem *TagList)
{
	if(!GlobalBase -> Closing)
	{
		if(Hook && Name && TagList)
		{
			struct ShutdownInfo	*Info;
			LONG			 Len = strlen(Name);

			if(Info = (struct ShutdownInfo *)AllocVec(sizeof(struct ShutdownInfo) + Len + 1,MEMF_PUBLIC | MEMF_CLEAR))
			{
				ObtainSemaphore(&GlobalBase -> AccessLock);

				Info -> sdi_Hook	= Hook;
				Info -> sdi_Name	= (STRPTR)(Info + 1);

				strcpy(Info -> sdi_Name,Name);

				AddTail((struct List *)&GlobalBase -> ShutdownList,(struct Node *)Info);

				ReleaseSemaphore(&GlobalBase -> AccessLock);

				return(Info);
			}
		}
	}

	return(NULL);
}

LONG __saveds __asm
RemShutdownInfo(register __a0 struct ShutdownInfo *Info)
{
	if(!GlobalBase -> Closing && Info)
	{
		struct ShutdownInfo	*Node,
					*Next;

		ObtainSemaphore(&GlobalBase -> AccessLock);

		Node = (struct ShutdownInfo *)GlobalBase -> ShutdownList . mlh_Head;

		while(Next = (struct ShutdownInfo *)Node -> sdi_Node . mln_Succ)
		{
			if(Info == (struct ShutdownInfo *)Node)
			{
				Remove((struct Node *)Info);

				FreeVec(Info);

				ReleaseSemaphore(&GlobalBase -> AccessLock);

				return(TRUE);
			}

			Node = Next;
		}

		ReleaseSemaphore(&GlobalBase -> AccessLock);
	}

	return(FALSE);
}

/*
**	TransferPanel.c
**
**	File transfer control panel support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

STATIC BOOL	StatusChanged		= FALSE;
STATIC LONG	TransferInfoCount	= 0;

#ifndef __AROS__
STATIC ULONG SAVE_DS ASM
TransferListViewRender(REG(a0) struct Hook *UnusedHook,REG(a2) struct Node *Node,REG(a1) struct LVDrawMsg *Msg)
#else
AROS_UFH3(STATIC ULONG, TransferListViewRender,
 AROS_UFHA(struct Hook *       , UnusedHook , A0),
 AROS_UFHA(struct Node *       , Node, A2),
 AROS_UFHA(struct LVDrawMsg *  , Msg, A1))
#endif
{
#ifdef __AROS__
    AROS_USERFUNC_INIT
#endif
		/* We only know how to redraw lines. */

	if(Msg->lvdm_MethodID == LV_DRAW)
	{
		struct RastPort	*RPort;
		LONG			 Left,Top,Width,Height,
						 FgPen,BgPen,
						 Len,LabelLen;
		STRPTR			 String;
		UWORD			*Pens;

		RPort	= Msg->lvdm_RastPort;

		Left	= Msg->lvdm_Bounds.MinX;
		Top		= Msg->lvdm_Bounds.MinY;
		Width	= Msg->lvdm_Bounds.MaxX - Msg->lvdm_Bounds.MinX + 1;
		Height	= Msg->lvdm_Bounds.MaxY - Msg->lvdm_Bounds.MinY + 1;

		String	= Node->ln_Name;

		Pens	= Msg->lvdm_DrawInfo->dri_Pens;

			/* Determine the rendering pens. */

		if(Msg->lvdm_State == LVR_SELECTED)
		{
			FgPen = FILLTEXTPEN;
			BgPen = FILLPEN;
		}
		else
		{
			FgPen = TEXTPEN;
			BgPen = BACKGROUNDPEN;
		}

		FgPen = Pens[FgPen];
		BgPen = Pens[BgPen];

		SetABPenDrMd(RPort,BgPen,BgPen,JAM2);
		RectFill(RPort,Left,Top,Left + 1,Top + Height - 1);

		Left	+= 2;
		Width	-= 2;

			/* Is this an error message? */

		if(*String++ == '*' && BgPen != Pens[HIGHLIGHTTEXTPEN])
			FgPen = Pens[HIGHLIGHTTEXTPEN];

			/* Determine length of vanilla name. */

		LabelLen = strlen(String);

			/* Try to make it fit. */

		LabelLen = FitText(RPort,Width,String,LabelLen);

			/* Print the vanilla name if possible. */

		if(LabelLen)
		{
			Len = TextLength(RPort,String,LabelLen);

			SetAPen(RPort,FgPen);

			PlaceText(RPort,Left,Top,String,LabelLen);

			Left	+= Len;
			Width	-= Len;
		}

			/* Fill the area to follow the vanilla name. */

		if(Width > 0)
		{
			SetAPen(RPort,BgPen);
			FillBox(RPort,Left,Top,Width,Height);
		}

			/* If the item happens to be disabled, draw the cross-hatch
			 * pattern across it.
			 */

		ListViewStateFill(Msg);

		return(LVCB_OK);
	}
	else
		return(LVCB_UNKNOWN);
#ifdef __AROS__
    AROS_USERFUNC_EXIT
#endif
}

	/* AddTransferInfo(STRPTR Message,...):
	 *
	 *	Add another entry to the transfer information list.
	 */

VOID
AddTransferInfo(BOOL Error,STRPTR Message,...)
{
	UBYTE TempBuffer[256];
	struct Node *Node;
	va_list VarArgs;

	if(Kick30)
	{
		va_start(VarArgs,Message);
		LimitedVSPrintf(sizeof(TempBuffer),&TempBuffer[1],Message,VarArgs);
		va_end(VarArgs);

		if(Error)
			TempBuffer[0] = '*';
		else
			TempBuffer[0] = ' ';
	}
	else
	{
		va_start(VarArgs,Message);
		LimitedVSPrintf(sizeof(TempBuffer),TempBuffer,Message,VarArgs);
		va_end(VarArgs);
	}

	if(Node = CreateNode(TempBuffer))
	{
		LT_SetAttributes(TransferHandle,GAD_TRANSFER_INFORMATION_LIST,
			GTLV_Labels,	~0,
		TAG_DONE);

		if(TransferInfoCount == 100)
		{
			FreeVecPooled(RemHead(&TransferInfoList));

			TransferInfoCount = 99;
		}

		AddTail(&TransferInfoList,Node);

		LT_SetAttributes(TransferHandle,GAD_TRANSFER_INFORMATION_LIST,
			GTLV_Labels,				&TransferInfoList,
			Kick30 ? GTLV_MakeVisible : GTLV_Top,	TransferInfoCount,
		TAG_DONE);

		TransferInfoCount++;
	}
}

STATIC struct LayoutHandle *
CreateTheTransferPanel(VOID)
{
	STATIC struct Hook ListViewHook;

	struct LayoutHandle *Handle;

	InitHook(&ListViewHook,(HOOKFUNC)TransferListViewRender,NULL);

	if(Handle = LT_CreateHandleTags(Window ? Window->WScreen : NULL,
		LAHN_LocaleHook,&LocaleHook,
	TAG_DONE))
	{
		STATIC LONG LabelGroups[5][2] =
		{
			MSG_TRANSFERPANEL_FILE_TXT,
				MSG_TRANSFERPANEL_COMPLETION_TIME_TXT,

			MSG_TRANSFERPANEL_FILE_SIZE_TXT,
				MSG_TRANSFERPANEL_BLOCKS_TRANSFERRED_TXT,

			MSG_TRANSFERPANEL_ESTIMATED_TIME_TXT,
				MSG_TRANSFERPANEL_ELAPSED_TIME_TXT,

			MSG_TRANSFERPANEL_CHARACTERS_PER_SECOND_TXT,
				MSG_TRANSFERPANEL_BLOCK_SIZE_TXT,

			MSG_TRANSFERPANEL_NUMBER_OF_ERRORS_TXT,
				MSG_TRANSFERPANEL_NUMBER_OF_TIMEOUTS_TXT,
		};

		STATIC WORD Pens[] =
		{
			BLOCKPEN,
			TEXTPEN,
			BACKGROUNDPEN,
			FILLTEXTPEN,
			FILLPEN,
			HIGHLIGHTTEXTPEN
		};

		LONG len,max,i,j,MaxPen;

		max = LT_LabelChars(Handle,LocaleString(MSG_TRANSFERPANEL_PROTOCOL_TXT));
		len = LT_LabelChars(Handle,LocaleString(MSG_TRANSFERPANEL_INFORMATION_GAD));

		if(len > max)
			max = len;

		for(i = 0 ; i < 5 ; i++)
		{
			for(j = LabelGroups[i][0] ; j <= LabelGroups[i][1] ; j++)
			{
				len = LT_LabelChars(Handle,LocaleString(j));

				if(len > max)
					max = len;
			}
		}

		for(i = 0, MaxPen = -1 ; i < NUM_ELEMENTS(Pens) ; i++)
		{
			if(Handle->DrawInfo->dri_Pens[Pens[i]] > MaxPen)
				MaxPen = Handle->DrawInfo->dri_Pens[Pens[i]];
		}

		LT_New(Handle,
			LA_Type,VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,VERTICAL_KIND,
				TAG_DONE);
				{
					UBYTE	LocalBuffer[60];
					WORD	Len;

					strcpy(LocalBuffer,FilePart(LastXprLibrary));

					if((Len = strlen(LocalBuffer)) > 3)
					{
						UBYTE ProtocolBuffer[60];

						strcpy(ProtocolBuffer,&LocalBuffer[3]);

						for(i = 0 ; i < Len - 3 ; i++)
						{
							if(ProtocolBuffer[i] == '.')
							{
								ProtocolBuffer[i] = 0;

								break;
							}
						}

						ProtocolBuffer[0] = ToUpper(ProtocolBuffer[0]);

						LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"[%s]",ProtocolBuffer);
					}
					else
						strcpy(LocalBuffer,"-");

					LT_New(Handle,
						LA_Type,			TEXT_KIND,
						LA_LabelID,			MSG_TRANSFERPANEL_PROTOCOL_TXT,
						LA_LabelChars,		max,
						LA_Chars,			50,
						LA_ID,				GAD_TRANSFER_PROTOCOL,
						GTTX_Text,			LocalBuffer,
						GTTX_CopyText,		TRUE,
						GTTX_Border,		TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			LISTVIEW_KIND,
						LA_LabelID,			MSG_TRANSFERPANEL_INFORMATION_GAD,
						LA_LabelChars,		max,
						LA_LabelPlace,		PLACE_Left,
						LA_ID,				GAD_TRANSFER_INFORMATION_LIST,
						LA_Chars,			50,
						LALV_Lines,			2,
						LALV_MaxGrowY,		10,
						LALV_CursorKey,		TRUE,
						GTLV_ReadOnly,		TRUE,
						GTLV_CallBack,		&ListViewHook,
						GTLV_MaxPen,		MaxPen,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			XBAR_KIND,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			BOX_KIND,
						LA_ID,				GAD_TRANSFER_FILE,
						LA_Chars,			50,
						LA_LabelChars,		max,
						LABX_FirstLabel,	MSG_TRANSFERPANEL_FILE_TXT,
						LABX_LastLabel,		MSG_TRANSFERPANEL_COMPLETION_TIME_TXT,
						LABX_ReserveSpace,	TRUE,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,			XBAR_KIND,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,VERTICAL_KIND,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,			BOX_KIND,
							LA_ID,				GAD_TRANSFER_SIZE,
							LA_LabelChars,		max,
							LA_Chars,			15,
							LABX_FirstLabel,	MSG_TRANSFERPANEL_FILE_SIZE_TXT,
							LABX_LastLabel,		MSG_TRANSFERPANEL_BLOCKS_TRANSFERRED_TXT,
							LABX_ReserveSpace,	TRUE,
							LABX_AlignText,		ALIGNTEXT_Right,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			BOX_KIND,
							LA_LabelChars,		max,
							LA_ID,				GAD_TRANSFER_TOTALTIME,
							LABX_FirstLabel,	MSG_TRANSFERPANEL_ESTIMATED_TIME_TXT,
							LABX_LastLabel,		MSG_TRANSFERPANEL_ELAPSED_TIME_TXT,
							LABX_ReserveSpace,	TRUE,
							LABX_AlignText,		ALIGNTEXT_Right,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_New(Handle,
						LA_Type,				VERTICAL_KIND,
						LAGR_LastAttributes,	TRUE,
					TAG_DONE);
					{
						LT_New(Handle,
							LA_Type,			BOX_KIND,
							LA_ID,				GAD_TRANSFER_SECONDS,
							LA_LabelChars,		max,
							LA_Chars,			15,
							LABX_FirstLabel,	MSG_TRANSFERPANEL_CHARACTERS_PER_SECOND_TXT,
							LABX_LastLabel,		MSG_TRANSFERPANEL_BLOCK_SIZE_TXT,
							LABX_ReserveSpace,	TRUE,
							LABX_AlignText,		ALIGNTEXT_Right,
						TAG_DONE);

						LT_New(Handle,
							LA_Type,			BOX_KIND,
							LA_ID,				GAD_TRANSFER_ERRORS,
							LA_LabelChars,		max,
							LABX_FirstLabel,	MSG_TRANSFERPANEL_NUMBER_OF_ERRORS_TXT,
							LABX_LastLabel,		MSG_TRANSFERPANEL_NUMBER_OF_TIMEOUTS_TXT,
							LABX_ReserveSpace,	TRUE,
							LABX_AlignText,		ALIGNTEXT_Right,
						TAG_DONE);

						LT_EndGroup(Handle);
					}

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,XBAR_KIND,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

				LT_New(Handle,
					LA_Type,		HORIZONTAL_KIND,
					LAGR_SameSize,	TRUE,
					LAGR_Spread,	TRUE,
				TAG_DONE);
				{
					LONG len1,len2,Maximum;

					len1 = 4 + strlen(LocaleString(MSG_TRANSFERPANEL_DATA_TRANSFERRED_GAD)) + 1;
					len2 = 8 + strlen(LocaleString(MSG_TRANSFERPANEL_TIME_TO_GO_GAD)) + 1;

					if(len1 > len2)
						Maximum = len1;
					else
						Maximum = len2;

					if(Maximum < 30)
						Maximum = 30;

					LT_New(Handle,
						LA_Type,		GAUGE_KIND,
						LA_Chars,		Maximum,
						LA_LabelPlace,	PLACE_In,
						LA_ID,			GAD_TRANSFER_PERCENT,
						LAGA_NoTicks,	TRUE,
						LAGA_InfoLength,len1,
					TAG_DONE);

					LT_New(Handle,
						LA_Type,		GAUGE_KIND,
						LA_LabelPlace,	PLACE_In,
						LA_ID,			GAD_TRANSFER_TIME,
						LAGA_NoTicks,	TRUE,
						LAGA_InfoLength,len2,
					TAG_DONE);

					LT_EndGroup(Handle);
				}

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
				LAGR_Spread,TRUE,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_TRANSFERPANEL_SKIP_CURRENT_FILE_GAD,
					LA_ID,			GAD_TRANSFER_SKIP,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_TRANSFERPANEL_ABORT_CURRENT_FILE_GAD,
					LA_ID,			GAD_TRANSFER_ABORT_FILE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_TRANSFERPANEL_ABORT_ENTIRE_TRANSFER_GAD,
					LA_ID,			GAD_TRANSFER_ABORT,
					LABT_EscKey,	TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}

			LT_EndGroup(Handle);
		}
	}

	return(Handle);
}

	/* TransferPanel(STRPTR Title):
	 *
	 *	Open the transfer window.
	 */

BOOL
TransferPanel(STRPTR Title)
{
	if(TransferWindow)
		return(TRUE);
	else
	{
		WORD CurrentStatus;

		TransferFailed = TransferAborted = FALSE;

		CurrentStatus = GetStatus();

		if(CurrentStatus != STATUS_DOWNLOAD && CurrentStatus != STATUS_UPLOAD)
		{
			PushStatus(STATUS_DOWNLOAD);
			StatusChanged = TRUE;
		}

		if(TransferHandle = CreateTheTransferPanel())
		{
			LONG WindowLeft,WindowTop,IgnoreWidth,IgnoreHeight;

			TransferZoomed = FALSE;

			GetWindowInfo(WINDOW_FILETRANSFER,&WindowLeft,&WindowTop,&IgnoreWidth,&IgnoreHeight,0,0);

			if(TransferWindow = LT_Build(TransferHandle,
				LAWN_TitleText,		TransferWindowTitle = Title,
				LAWN_HelpHook,		&GuideHook,
				LAWN_Zoom,			TRUE,
				LAWN_Left,			WindowLeft,
				LAWN_Top,			WindowTop,
				LAWN_Parent,		Window,
				LAWN_LimitWidth,	WindowWidth,
				LAWN_LimitHeight,	WindowHeight,
				WA_DepthGadget,		TRUE,
				WA_DragBar,			TRUE,
				WA_RMBTrap,			TRUE,
			TAG_DONE))
			{
				BOOL	Activate,Move;
				LONG	i;

				GuideContext(CONTEXT_TRANSFER);

				if(Config->MiscConfig->AlertMode == ALERT_SCREEN || Config->MiscConfig->AlertMode == ALERT_BEEP_SCREEN)
					Move = Activate = TRUE;
				else
				{
					ULONG IntuiLock = LockIBase(NULL);

					Activate = Move = FALSE;

					if(IntuitionBase->ActiveScreen == Screen)
						Activate = TRUE;
					else
					{
						if(IntuitionBase->ActiveWindow && IntuitionBase->ActiveWindow->UserPort && IntuitionBase->ActiveWindow->UserPort->mp_Flags == PA_SIGNAL && IntuitionBase->ActiveWindow->UserPort->mp_SigTask == FindTask(NULL))
							Activate = TRUE;
					}

					UnlockIBase(IntuiLock);
				}

				if(Config->TransferConfig->TransferNotification == XFERNOTIFY_NEVER || Config->TransferConfig->TransferNotification == XFERNOTIFY_END)
					Move = Activate = FALSE;

				if(Move)
				{
					if(TransferWindow->WScreen->LeftEdge > 0)
					{
						if(TransferWindow->WScreen->TopEdge > 0)
							MoveScreen(TransferWindow->WScreen,-TransferWindow->WScreen->LeftEdge,-TransferWindow->WScreen->TopEdge);
						else
							MoveScreen(TransferWindow->WScreen,-TransferWindow->WScreen->LeftEdge,0);
					}
					else
					{
						if(TransferWindow->WScreen->TopEdge > 0)
							MoveScreen(TransferWindow->WScreen,0,-TransferWindow->WScreen->TopEdge);
					}

					WindowToFront(TransferWindow);

					ScreenToFront(TransferWindow->WScreen);
				}

				if(Activate)
					ActivateWindow(TransferWindow);

				PushWindow(TransferWindow);

				for(i = 0 ; i < 6 ; i++)
				{
					if(i < 4)
					{
						if(i < 2)
						{
							LT_SetAttributes(TransferHandle,GAD_TRANSFER_TOTALTIME,
								LABX_Index,	i,
								LABX_Text,	"-",
							TAG_DONE);

							LT_SetAttributes(TransferHandle,GAD_TRANSFER_ERRORS,
								LABX_Index,	i,
								LABX_Text,	"-",
							TAG_DONE);
						}

						LT_SetAttributes(TransferHandle,GAD_TRANSFER_FILE,
							LABX_Index,	i,
							LABX_Text,	"-",
						TAG_DONE);
					}

					LT_SetAttributes(TransferHandle,GAD_TRANSFER_SIZE,
						LABX_Index,	i,
						LABX_Text,	"-",
					TAG_DONE);

					LT_SetAttributes(TransferHandle,GAD_TRANSFER_SECONDS,
						LABX_Index,	i,
						LABX_Text,	"-",
					TAG_DONE);
				}

				return(TRUE);
			}

			LT_DeleteHandle(TransferHandle);

			TransferHandle = NULL;
		}
	}

	return(FALSE);
}

	/* DeleteTransferPanel(BOOL WaitForPrompt):
	 *
	 *	Close the transfer window.
	 */

VOID
DeleteTransferPanel(BOOL WaitForPrompt)
{
		/* Can any pending garbage. */

	if(TransferError)
		FlushSerialRead();

		/* Wait for user to close the window? */

	if(WaitForPrompt && TransferHandle)
	{
		BOOL Done;

		LT_SetAttributes(TransferHandle,GAD_TRANSFER_SKIP,
			GA_Disabled,TRUE,
		TAG_DONE);

		LT_SetAttributes(TransferHandle,GAD_TRANSFER_ABORT_FILE,
			GA_Disabled,TRUE,
		TAG_DONE);

		Done = FALSE;

		do
		{
			if(Wait(PORTMASK(TransferWindow->UserPort) | SIG_BREAK) & SIG_BREAK)
				break;
			else
			{
				struct IntuiMessage	*Message;
				struct Gadget *MsgGadget;
				ULONG MsgClass;

				while(Message = (struct IntuiMessage *)LT_GetIMsg(TransferHandle))
				{
					MsgClass	= Message->Class;
					MsgGadget	= (struct Gadget *)Message->IAddress;

					LT_ReplyIMsg(Message);

					if(MsgClass == IDCMP_GADGETUP && MsgGadget->GadgetID == GAD_TRANSFER_ABORT)
						Done = TRUE;
				}
			}
		}
		while(!Done);
	}

	TransferAbortState = 0;
	TransferAbortCount = 0;

	TransferError = FALSE;

	if(FileTransferInfo)
	{
		FreeFileTransferInfo(FileTransferInfo);

		FileTransferInfo = NULL;
	}

	if(StatusChanged)
		PopStatus();

	if(TransferHandle)
	{
		PutWindowInfo(WINDOW_FILETRANSFER,TransferHandle->Window->LeftEdge,TransferHandle->Window->TopEdge,TransferHandle->Window->Width,TransferHandle->Window->Height);

		LT_DeleteHandle(TransferHandle);

		TransferHandle = NULL;
	}

	if(!IsListEmpty(&TransferInfoList))
	{
		FreeList(&TransferInfoList);

		TransferInfoCount = 0;
	}

	TransferWindow = NULL;

	ReleaseWindows();
}

/*
**	ASCIIPanel.c
**
**	User interface for built-in ASCII transfer routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_BYTES=1000,GAD_INFORMATION,GAD_CHARDELAY,GAD_LINEDELAY,
			GAD_PACING,GAD_QUIET_TRANSFER,GAD_SKIP,GAD_ABORT
		};

STATIC LayoutHandle	*ASCIIHandle;
STATIC struct List	 MessageList;
STATIC LONG			 MessageCount;

struct Window *
CreateASCIIWindow(BOOL Send)
{
	NewList(&MessageList);

	MessageCount = 0;

	if(ASCIIHandle = LT_CreateHandleTags(Window->WScreen,
		LAHN_LocaleHook,&LocaleHook,
	TAG_DONE))
	{
		struct Window *PanelWindow;
		LONG WindowLeft,WindowTop,IgnoreWidth,IgnoreHeight;

		LT_New(ASCIIHandle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(ASCIIHandle,
				LA_Type,		VERTICAL_KIND,
				LA_LabelID,	MSG_V36_0000,
			TAG_DONE);
			{
				LT_New(ASCIIHandle,
					LA_Type,			BOX_KIND,
					LA_ID,				GAD_BYTES,
					LABX_FirstLabel,	MSG_ASCIIPANEL_BYTES_GAD,
					LABX_LastLabel,		MSG_ASCIIPANEL_LINES_GAD,
					LABX_ReserveSpace,	TRUE,
					LA_Chars,			50,
				TAG_DONE);

				LT_New(ASCIIHandle,
					LA_Type,			LISTVIEW_KIND,
					LA_LabelID,			MSG_ASCIIPANEL_INFORMATION_GAD,
					LA_LabelPlace,		PLACE_Left,
					LA_ID,				GAD_INFORMATION,
					LALV_Lines,			2,
					LALV_MaxGrowX,		70,
					LALV_MaxGrowY,		10,
					LALV_CursorKey,		TRUE,
					GTLV_ReadOnly,		TRUE,
					GTLV_Labels,		&MessageList,
				TAG_DONE);

				LT_EndGroup(ASCIIHandle);
			}

			if(Send)
			{
				LT_New(ASCIIHandle,
					LA_Type,				VERTICAL_KIND,
					LA_LabelID,				MSG_V36_0003,
					LAGR_LastAttributes,	TRUE,
				TAG_DONE);
				{
					LT_New(ASCIIHandle,
						LA_Type,			SliderType,
						LA_LabelID,			MSG_ASCIIPANEL_CHAR_DELAY_GAD,
						LA_ID,				GAD_CHARDELAY,
						GTSL_Min,			0,
						GTSL_Max,			10 * 100,
						LA_WORD,			&Config->TransferConfig->CharDelay,
						GTSL_LevelFormat,	"%s s",
						GTSL_DispFunc,		StandardShowTime,
					TAG_DONE);

					LT_New(ASCIIHandle,
						LA_Type,			SliderType,
						LA_LabelID,			MSG_ASCIIPANEL_LINE_DELAY_GAD,
						LA_ID,				GAD_LINEDELAY,
						GTSL_Min,			0,
						GTSL_Max,			10 * 100,
						LA_WORD,			&Config->TransferConfig->LineDelay,
						GTSL_LevelFormat,	"%s s",
						GTSL_DispFunc,		StandardShowTime,
					TAG_DONE);

					LT_New(ASCIIHandle,
						LA_Type,			CYCLE_KIND,
						LA_LabelID,			MSG_ASCIIPANEL_TEXT_PACING_GAD,
						LA_ID,				GAD_PACING,
						LA_UBYTE,			&Config->TransferConfig->PacingMode,
						LACY_FirstLabel,	MSG_ASCIIPANEL_DIRECT_TXT,
						LACY_LastLabel,		MSG_ASCIIPANEL_KEYDELAY_TXT,
					TAG_DONE);

					LT_New(ASCIIHandle,
						LA_Type,			CHECKBOX_KIND,
						LA_LabelID,			MSG_ASCIIPANEL_QUIET_TRANSFER_GAD,
						LA_ID,				GAD_QUIET_TRANSFER,
						LA_BYTE,			&Config->TransferConfig->QuietTransfer,
					TAG_DONE);

					LT_EndGroup(ASCIIHandle);
				}

				LT_New(ASCIIHandle,
					LA_Type,VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(ASCIIHandle,
						LA_Type,		XBAR_KIND,
						LAXB_FullSize,	TRUE,
					TAG_DONE);

					LT_EndGroup(ASCIIHandle);
				}

				LT_New(ASCIIHandle,LA_Type,HORIZONTAL_KIND,
					LAGR_SameSize,	TRUE,
					LAGR_Spread,	TRUE,
				TAG_DONE);
				{
					LT_New(ASCIIHandle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_TRANSFERPANEL_SKIP_CURRENT_FILE_GAD,
						LA_ID,			GAD_SKIP,
						LABT_ExtraFat,	TRUE,
					TAG_DONE);

					LT_New(ASCIIHandle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_TRANSFERPANEL_ABORT_ENTIRE_TRANSFER_GAD,
						LA_ID,			GAD_ABORT,
						LABT_ExtraFat,	TRUE,
					TAG_DONE);

					LT_EndGroup(ASCIIHandle);
				}
			}
			else
			{
				LT_New(ASCIIHandle,
					LA_Type,	VERTICAL_KIND,
				TAG_DONE);
				{
					LT_New(ASCIIHandle,
						LA_Type,		XBAR_KIND,
						LAXB_FullSize,	TRUE,
					TAG_DONE);

					LT_EndGroup(ASCIIHandle);
				}

				LT_New(ASCIIHandle,
					LA_Type,	HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(ASCIIHandle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_TRANSFERPANEL_ABORT_ENTIRE_TRANSFER_GAD,
						LA_ID,			GAD_ABORT,
						LABT_ExtraFat,	TRUE,
					TAG_DONE);

					LT_EndGroup(ASCIIHandle);
				}
			}

			LT_EndGroup(ASCIIHandle);
		}

		GetWindowInfo(WINDOW_FILETRANSFER,&WindowLeft,&WindowTop,&IgnoreWidth,&IgnoreHeight,0,0);

		if(PanelWindow = LT_Build(ASCIIHandle,
			LAWN_TitleText,		Send ? LocaleString(MSG_ASCIIPANEL_SENDING_TXT) : LocaleString(MSG_ASCIIPANEL_RECEIVING_TXT),
			LAWN_HelpHook,		&GuideHook,
			LAWN_Parent,		Window,
			LAWN_Left,			WindowLeft,
			LAWN_Top,			WindowTop,
			LAWN_LimitWidth,	WindowWidth,
			LAWN_LimitHeight,	WindowHeight,
			WA_DepthGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
		{
			BOOL Activate,Move;

			GuideContext(CONTEXT_ASCII);

			if(Config->MiscConfig->AlertMode == ALERT_SCREEN || Config->MiscConfig->AlertMode == ALERT_BEEP_SCREEN)
				Move = Activate = TRUE;
			else
			{
				ULONG IntuiLock;

				Move = FALSE;

				IntuiLock = LockIBase(0);

				if(IntuitionBase->ActiveScreen == PanelWindow->WScreen)
					Activate = TRUE;
				else
					Activate = FALSE;

				UnlockIBase(IntuiLock);
			}

			if(Move)
			{
				if(PanelWindow->WScreen->LeftEdge > 0)
				{
					if(PanelWindow->WScreen->TopEdge > 0)
						MoveScreen(PanelWindow->WScreen,-PanelWindow->WScreen->LeftEdge,-PanelWindow->WScreen->TopEdge);
					else
						MoveScreen(PanelWindow->WScreen,-PanelWindow->WScreen->LeftEdge,0);
				}
				else
				{
					if(PanelWindow->WScreen->TopEdge > 0)
						MoveScreen(PanelWindow->WScreen,0,-PanelWindow->WScreen->TopEdge);
				}

				WindowToFront(PanelWindow);

				ScreenToFront(PanelWindow->WScreen);
			}

			if(Activate)
				ActivateWindow(PanelWindow);

			PushWindow(PanelWindow);

			return(PanelWindow);
		}

		LT_DeleteHandle(ASCIIHandle);

		ASCIIHandle = NULL;
	}

	return(NULL);
}

VOID
DeleteASCIIWindow(struct Window *Window,BOOL WaitForIt)
{
	if(WaitForIt && Window && ASCIIHandle)
	{
		struct IntuiMessage *Message;
		struct Gadget *MsgGadget;
		ULONG MsgClass;
		BOOL Done;
		LONG i;

		for(i = GAD_CHARDELAY ; i <= GAD_SKIP ; i++)
			LT_SetAttributes(ASCIIHandle,i,GA_Disabled,TRUE,TAG_DONE);

		Done = FALSE;

		do
		{
			if(Wait(PORTMASK(Window->UserPort) | SIG_BREAK) & SIG_BREAK)
				break;

			while(Message = (struct IntuiMessage *)LT_GetIMsg(ASCIIHandle))
			{
				MsgClass	= Message->Class;
				MsgGadget	= (struct Gadget *)Message->IAddress;

				LT_ReplyIMsg(Message);

				if(MsgClass == IDCMP_GADGETUP && MsgGadget->GadgetID == GAD_ABORT)
					Done = TRUE;
			}
		}
		while(!Done);
	}

	if(Window && ASCIIHandle)
		PopWindow();

	if(ASCIIHandle)
	{
		PutWindowInfo(WINDOW_FILETRANSFER,ASCIIHandle->Window->LeftEdge,ASCIIHandle->Window->TopEdge,ASCIIHandle->Window->Width,ASCIIHandle->Window->Height);

		LT_DeleteHandle(ASCIIHandle);

		ASCIIHandle = NULL;
	}

	if(MessageCount)
	{
		FreeList(&MessageList);

		MessageCount = 0;
	}
}

VOID
AddASCIIMessage(CONST_STRPTR Message,...)
{
	UBYTE TempBuffer[256];
	struct Node *Node;
	va_list VarArgs;

	va_start(VarArgs,Message);
	LimitedVSPrintf(sizeof(TempBuffer),TempBuffer,Message,VarArgs);
	va_end(VarArgs);

	if(Node = CreateNode(TempBuffer))
	{
		if(MessageCount == 100)
		{
			FreeVecPooled(RemHead(&MessageList));

			MessageCount = 99;
		}

		LT_SetAttributes(ASCIIHandle,GAD_INFORMATION,
			GTLV_Labels,~0,
		TAG_DONE);

		AddTail(&MessageList,Node);

		LT_SetAttributes(ASCIIHandle,GAD_INFORMATION,
			GTLV_Labels,&MessageList,
			Kick30 ? GTLV_MakeVisible : GTLV_Top,MessageCount,
		TAG_DONE);

		MessageCount++;
	}
}

VOID
UpdateASCIIWindow(LONG Bytes,LONG MaxBytes,LONG Lines)
{
	if(Bytes != -1)
	{
		UBYTE LocalBuffer[40];

		if(MaxBytes)
		{
			LONG Percent = (100 * Bytes) / MaxBytes;

			if(Percent > 100)
				Percent = 100;

			if(LocaleBase)
				LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"%lD (%ld%%)",Bytes,Percent);
			else
				LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,"%ld (%ld%%)",Bytes,Percent);
		}
		else
			LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,ConvNumber,Bytes);

		LT_SetAttributes(ASCIIHandle,GAD_BYTES,
			LABX_Index,	0,
			LABX_Text,	LocalBuffer,
		TAG_DONE);
	}

	if(Lines != -1)
	{
		UBYTE LocalBuffer[40];

		LimitedSPrintf(sizeof(LocalBuffer),LocalBuffer,ConvNumber,Lines);

		LT_SetAttributes(ASCIIHandle,GAD_BYTES,
			LABX_Index,	1,
			LABX_Text,	LocalBuffer,
		TAG_DONE);
	}
}

BOOL
HandleASCIIWindow()
{
	struct IntuiMessage *Message;
	struct Gadget *MsgGadget;
	ULONG MsgClass;

	while(Message = (struct IntuiMessage *)LT_GetIMsg(ASCIIHandle))
	{
		MsgClass	= Message->Class;
		MsgGadget	= (struct Gadget *)Message->IAddress;

		LT_ReplyIMsg(Message);

		if(MsgClass == IDCMP_MOUSEMOVE)
		{
			SendSetup();

			ConfigChanged = TRUE;
		}

		if(MsgClass == IDCMP_GADGETUP)
		{
			switch(MsgGadget->GadgetID)
			{
				case GAD_PACING:
				case GAD_CHARDELAY:
				case GAD_LINEDELAY:

					ASCIISendSetup();

					ConfigChanged = TRUE;

					break;

				case GAD_QUIET_TRANSFER:

					ConfigChanged = TRUE;

					break;

				case GAD_ABORT:

					return(1);

				case GAD_SKIP:

					return(2);
			}
		}
	}

	return(0);
}

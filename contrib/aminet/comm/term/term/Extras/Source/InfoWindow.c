/*
**	InfoWindow.c
**
**	Status information window support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

enum	{	GAD_UPDATE=1,GAD_SESSION,GAD_BYTES_IN_OUT,GAD_CONNECT_STRING,GAD_BBS_INFO,
			GAD_SCREEN_SIZE,GAD_PORT_NAME,GAD_BUFFER_SIZE,GAD_MEMORY,
		};

STATIC struct Process	*InfoProcess;
STATIC LayoutHandle		*InfoHandle;

	/* LocalRefreshInfoWindow():
	 *
	 *	Refresh, i.e. redraw, the contents of the info window.
	 */

STATIC VOID
LocalRefreshInfoWindow(VOID)
{
	if(InfoWindow)
	{
		UBYTE DummyBuffer[256];

		LimitedSPrintf(sizeof(DummyBuffer),DummyBuffer,ConvNumber,BytesIn);

		LT_SetAttributes(InfoHandle,GAD_BYTES_IN_OUT,
			LABX_Index,	0,
			LABX_Text,	DummyBuffer,
		TAG_DONE);

		LimitedSPrintf(sizeof(DummyBuffer),DummyBuffer,ConvNumber,BytesOut);

		LT_SetAttributes(InfoHandle,GAD_BYTES_IN_OUT,
			LABX_Index,	1,
			LABX_Text,	DummyBuffer,
		TAG_DONE);

		Forbid();

		if(XEmulatorBase && XEM_IO)
		{
			if(XEmulatorBase->lib_Version >= 4)
			{
				ULONG Result = XEmulatorInfo(XEM_IO,XEMI_CONSOLE_DIMENSIONS);

				LimitedSPrintf(sizeof(DummyBuffer),DummyBuffer,"%ld × %ld",XEMI_EXTRACT_COLUMNS(Result),XEMI_EXTRACT_LINES(Result));
			}
			else
				strcpy(DummyBuffer,"???");
		}
		else
			LimitedSPrintf(sizeof(DummyBuffer),DummyBuffer,"%ld × %ld",LastColumn + 1,LastLine + 1);

		Permit();

		LT_SetAttributes(InfoHandle,GAD_SCREEN_SIZE,
			GTTX_Text,	DummyBuffer,
		TAG_DONE);

		LimitedSPrintf(sizeof(DummyBuffer),DummyBuffer,ConvNumber,BufferSpace);

		LT_SetAttributes(InfoHandle,GAD_BUFFER_SIZE,
			GTTX_Text,	DummyBuffer,
		TAG_DONE);

		LimitedSPrintf(sizeof(DummyBuffer),DummyBuffer,ConvNumber,AvailMem(MEMF_ANY));

		LT_SetAttributes(InfoHandle,GAD_MEMORY,
			GTTX_Text,	DummyBuffer,
		TAG_DONE);

		LT_SetAttributes(InfoHandle,GAD_BBS_INFO,
			LABX_Index,	0,
			LABX_Text,	CurrentBBSName[0] ? CurrentBBSName : (STRPTR)"-",
		TAG_DONE);

		LT_SetAttributes(InfoHandle,GAD_BBS_INFO,
			LABX_Index,	1,
			LABX_Text,	CurrentBBSNumber[0] ? CurrentBBSNumber : (STRPTR)"-",
		TAG_DONE);

		LT_SetAttributes(InfoHandle,GAD_BBS_INFO,
			LABX_Index,	2,
			LABX_Text,	CurrentBBSComment[0] ? CurrentBBSComment : (STRPTR)"-",
		TAG_DONE);

		LT_SetAttributes(InfoHandle,GAD_BBS_INFO,
			LABX_Index,	3,
			LABX_Text,	UserName[0] ? UserName : (STRPTR)"-",
		TAG_DONE);
	}
}

	/* LocalCloseInfoWindow():
	 *
	 *	Close the info window.
	 */

STATIC VOID
LocalCloseInfoWindow(VOID)
{
	CheckItem(MEN_STATUS_WINDOW,FALSE);

	if(InfoWindow)
		PutWindowInfo(WINDOW_STATUS,InfoWindow->LeftEdge,InfoWindow->TopEdge,InfoWindow->Width,InfoWindow->Height);

	if(InfoHandle)
	{
		LT_DeleteHandle(InfoHandle);

		InfoHandle = NULL;
	}

	InfoWindow = NULL;
}

	/* LocalOpenInfoWindow():
	 *
	 *	Open the info window.
	 */

STATIC BOOL
LocalOpenInfoWindow(VOID)
{
	if(InfoHandle = LT_CreateHandleTags(Window->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
	TAG_DONE))
	{
		LONG Left,Top,Width,Height;

		GetWindowInfo(WINDOW_STATUS,&Left,&Top,&Width,&Height,0,0);

		LT_New(InfoHandle,
			LA_Type,	VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(InfoHandle,
				LA_Type,	VERTICAL_KIND,
			TAG_DONE);
			{
				UBYTE DummyBuffer[256];

				if(!FormatStamp(&SessionStart,DummyBuffer,sizeof(DummyBuffer),FALSE))
					strcpy(DummyBuffer,"-");

				LT_New(InfoHandle,
					LA_Type,			TEXT_KIND,
					LA_LabelID,			MSG_INFOWINDOW_SESSION_START_GAD,
					LA_ID,				GAD_SESSION,
					LA_Chars,			40,
					GTTX_Text,			DummyBuffer,
					GTTX_Border,		TRUE,
					GTTX_CopyText,		TRUE,
				TAG_DONE);

				LT_New(InfoHandle,
					LA_Type,			XBAR_KIND,
				TAG_DONE);

				LT_New(InfoHandle,
					LA_Type,			BOX_KIND,
					LA_ID,				GAD_BYTES_IN_OUT,
					LABX_FirstLabel,	MSG_INFOWINDOW_BYTES_RECEIVED_GAD,
					LABX_LastLabel,		MSG_INFOWINDOW_BYTES_SENT_GAD,
					LABX_ReserveSpace,	TRUE,
					LABX_DrawBox,		TRUE,
					LA_Chars,			40,
				TAG_DONE);

				LT_New(InfoHandle,
					LA_Type,			XBAR_KIND,
				TAG_DONE);

				LT_New(InfoHandle,
					LA_Type,			TEXT_KIND,
					LA_LabelID,			MSG_INFOWINDOW_CONNECTION_MESSAGE_GAD,
					LA_ID,				GAD_CONNECT_STRING,
					GTTX_Text,			BaudBuffer[0] ? BaudBuffer : (STRPTR)"-",
					GTTX_Border,		TRUE,
					GTTX_CopyText,		TRUE,
				TAG_DONE);

				LT_New(InfoHandle,
					LA_Type,			BOX_KIND,
					LA_ID,				GAD_BBS_INFO,
					LABX_FirstLabel,	MSG_INFOWINDOW_BBS_NAME_GAD,
					LABX_LastLabel,		MSG_INFOWINDOW_BBS_USER_NAME_GAD,
					LABX_ReserveSpace,	TRUE,
					LABX_DrawBox,		TRUE,
					LA_Chars,			40,
				TAG_DONE);

				LT_New(InfoHandle,
					LA_Type,			XBAR_KIND,
				TAG_DONE);

				LT_New(InfoHandle,
					LA_Type,			TEXT_KIND,
					LA_LabelID,			MSG_INFOWINDOW_SCREEN_SIZE_GAD,
					LA_ID,				GAD_SCREEN_SIZE,
					GTTX_Border,		TRUE,
					GTTX_CopyText,		TRUE,
				TAG_DONE);

				LT_New(InfoHandle,
					LA_Type,			TEXT_KIND,
					LA_LabelID,			MSG_INFOWINDOW_AREXX_PORT_NAME_GAD,
					LA_ID,				GAD_PORT_NAME,
					GTTX_Text,			RexxPortName,
					GTTX_Border,		TRUE,
					GTTX_CopyText,		TRUE,
				TAG_DONE);

				LT_New(InfoHandle,
					LA_Type,			TEXT_KIND,
					LA_LabelID,			MSG_INFOWINDOW_BUFFER_SIZE_GAD,
					LA_ID,				GAD_BUFFER_SIZE,
					GTTX_Border,		TRUE,
					GTTX_CopyText,		TRUE,
				TAG_DONE);

				LT_New(InfoHandle,
					LA_Type,			TEXT_KIND,
					LA_LabelID,			MSG_INFOWINDOW_FREE_MEMORY_GAD,
					LA_ID,				GAD_MEMORY,
					GTTX_Border,		TRUE,
					GTTX_CopyText,		TRUE,
				TAG_DONE);

				LT_EndGroup(InfoHandle);
			}

			LT_New(InfoHandle,
				LA_Type,VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(InfoHandle,
					LA_Type,	XBAR_KIND,
					LAXB_FullSize,	TRUE,
				TAG_DONE);

				LT_EndGroup(InfoHandle);
			}

			LT_New(InfoHandle,
				LA_Type,	HORIZONTAL_KIND,
			TAG_DONE);
			{
				LT_New(InfoHandle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_V36_1414,
					LA_ID,			GAD_UPDATE,
					LABT_ReturnKey,	TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_EndGroup(InfoHandle);
			}

			LT_EndGroup(InfoHandle);
		}

		if(InfoWindow = LT_Build(InfoHandle,
			LAWN_TitleID,		MSG_INFOWINDOW_STATUS_INFORMATION_TXT,
			LAWN_IDCMP,			IDCMP_CLOSEWINDOW,
			LAWN_HelpHook,		&GuideHook,
			LAWN_Left,			Left,
			LAWN_Top,			Top,
			WA_DepthGadget,		TRUE,
			WA_CloseGadget,		TRUE,
			WA_DragBar,			TRUE,
			WA_RMBTrap,			TRUE,
			WA_SimpleRefresh,	TRUE,
		TAG_DONE))
		{
			LocalRefreshInfoWindow();

			return(TRUE);
		}
		else
		{
			LT_DeleteHandle(InfoHandle);

			InfoHandle = NULL;
		}
	}

	return(FALSE);
}

STATIC VOID SAVE_DS
StatusProcessEntry(VOID)
{
	struct Process *Parent = ThisProcess;

	if(LocalOpenInfoWindow())
	{
		struct IntuiMessage	*Message;
		struct Gadget *MsgGadget;
		ULONG MsgClass;
		BOOL Done;

		InfoProcess = (struct Process *)FindTask(NULL);

		Signal((struct Task *)Parent,SIG_HANDSHAKE);

		Done = FALSE;

		do
		{
			if(Wait(PORTMASK(InfoWindow->UserPort) | SIG_KILL) & SIG_KILL)
				break;
			else
			{
				while(Message = LT_GetIMsg(InfoHandle))
				{
					MsgClass	= Message->Class;
					MsgGadget	= (struct Gadget *)Message->IAddress;

					LT_ReplyIMsg(Message);

					switch(MsgClass)
					{
						case IDCMP_CLOSEWINDOW:

							if(!Done)
							{
								Forbid();

								if(!(SetSignal(0,SIG_KILL) & SIG_KILL))
									Parent = NULL;

								Done = TRUE;
							}

							break;

						case IDCMP_GADGETUP:

							if(MsgGadget->GadgetID == GAD_UPDATE)
								LocalRefreshInfoWindow();

							break;
					}
				}
			}
		}
		while(!Done);

		LocalCloseInfoWindow();
	}

	Forbid();

	InfoProcess = NULL;

	if(Parent)
		Signal((struct Task *)Parent,SIG_HANDSHAKE);
}

VOID
CloseInfoWindow()
{
	ShakeHands((struct Task *)InfoProcess,SIG_KILL);
}

VOID
OpenInfoWindow()
{
	if(!InfoProcess)
	{
		StartProcessWaitForHandshake("term Status Window Process",(TASKENTRY)StatusProcessEntry,
			NP_WindowPtr,	-1,
		TAG_DONE);
	}

	if(InfoProcess)
		CheckItem(MEN_STATUS_WINDOW,TRUE);
	else
		CheckItem(MEN_STATUS_WINDOW,FALSE);
}

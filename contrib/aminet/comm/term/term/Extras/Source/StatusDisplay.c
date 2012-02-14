/*
**	StatusDisplay.c
**
**	Status information display routines
**
**	Copyright � 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* The information to be displayed. */

enum	{
			INFO_STATUS,INFO_BUFFER,INFO_PROTOCOL,INFO_EMULATION,INFO_BAUDRATE,
			INFO_PARAMETERS,INFO_CURRENTTIME,INFO_ONLINETIME,INFO_ONLINECOST,
			INFO_UNITTIME
		};

	/* The current status line display mode. */

enum	{
			MODE_SCREEN_NORMAL,MODE_SCREEN_COMPRESSED,
			MODE_WB_NORMAL,MODE_WB_COMPRESSED
		};

	/* Enumerated text boxes. */

enum	{
			STATUSBOX_STATUS_FONT,STATUSBOX_PROTOCOL_TERMINAL,
			STATUSBOX_RATE_PARAMETERS,STATUSBOX_TIME_ONLINE
		};

	/* What to show in the online time box. */

enum	{
			SHOW_Cost,SHOW_OnlineTime,SHOW_UnitTime
		};

	/* The status server passes this structure to the
	 * rendering routine if the status information
	 * is to be updated.
	 */

struct ObjectCarrier
{
	struct RastPort	 *RPort;
	LONG			  FirstColumn,
					  FullWidth;
	struct TextBox	**BoxArray,
					 *BoxList;
};

	/* A custom message type for the display update server. */

struct UpdateMessage
{
	struct Message	Message;

	APTR			Object;
	UBYTE			Mode,
					Type;
};

	/* The following static strings are displayed in the status
	 * window.
	 */

STATIC CONST_STRPTR			 ConfigBufferState[3],
						 ConfigEmulation[6],
						 ConfigParity[6];

	/* The status modes and the corresponding text IDs. */

STATIC LONG ConfigStatusIDs[] =
{
	MSG_TERMAUX_READY_TXT,
	MSG_TERMAUX_HOLDING_TXT,
	MSG_TERMAUX_DIALING_TXT,
	MSG_TERMAUX_UPLOAD_TXT,
	MSG_TERMAUX_DOWNLOAD_TXT,
	MSG_TERMAUX_BREAKING_TXT,
	MSG_TERMAUX_HANG_UP_TXT,
	MSG_TERMAUX_RECORD_TXT,
	MSG_AREXX_RUNNING_TXT
};

	/* Width of the status line text, required in case the user interface
	 * font happens to be proportional-spaced.
	 */

STATIC LONG				 StatusLineWidth;

	/* The status display update task. */

STATIC struct Task		*StatusDisplayTask;
STATIC struct MsgPort	*StatusDisplayPort;

	/* Display update data */

STATIC WORD				 UpdateSig;
STATIC ULONG			 UpdateMask;
STATIC BOOL				 NeedFullRefresh;

STATIC VOID
DrawSeparator(struct RastPort *RPort)
{
	SetAPen(RPort,Pens[SHADOWPEN]);
	RectFill(RPort,StatusDisplayLeft,StatusDisplayTop,StatusDisplayLeft + StatusDisplayWidth - 1,StatusDisplayTop);

	SetAPen(RPort,Pens[SHINEPEN]);
	RectFill(RPort,StatusDisplayLeft,StatusDisplayTop + 1,StatusDisplayLeft + StatusDisplayWidth - 1,StatusDisplayTop + 1);
}

	/* DoStatusInfo(APTR Object,UBYTE Mode,UBYTE Type,STRPTR String):
	 *
	 *	Display information in the status line area.
	 */

STATIC VOID
DoStatusInfo(APTR Object,UBYTE Mode,UBYTE Type,STRPTR String)
{
		/* What mode of operation is the status area in? */

	switch(Mode)
	{
			/* Compressed mode. */

		case MODE_SCREEN_COMPRESSED:
		{
			struct RastPort *RPort = Object;

			STATIC BYTE Offsets[8] =
			{
				 0,
				-1,	/* Not supported */
				26,
				11,
				40,
				53,
				61,
				72
			};

			STATIC UBYTE Strings[8][20];
			UBYTE LineBuffer[90];
			LONG i,j,k,Width;

			strcpy(Strings[Type],String);

			strcpy(LineBuffer,"         �              �             �            �       �          �         ");

			for(i = 0 ; i < 8 ; i++)
			{
				if(Offsets[i] >= 0)
				{
					j = strlen(Strings[i]);

					for(k = 0 ; k < j ; k++)
						LineBuffer[Offsets[i] + k] = Strings[i][k];
				}
			}

			Width = TextLength(RPort,LineBuffer,80);

			if((StatusLineWidth && StatusLineWidth != Width) || NeedFullRefresh)
			{
				LONG OldPen = ReadAPen(RPort);

				SetAPen(RPort,Pens[TEXTPEN]);
				FillBox(RPort,StatusDisplayLeft,StatusDisplayTop,StatusDisplayWidth,StatusDisplayHeight);
				SetAPen(RPort,OldPen);

				StatusLineWidth = Width;
				NeedFullRefresh = FALSE;
			}

			PlaceText(RPort,StatusDisplayLeft + (StatusDisplayWidth - Width) / 2,StatusDisplayTop,LineBuffer,80);
		}

		break;

			/* Normal mode. */

		case MODE_SCREEN_NORMAL:
		{
			STATIC UBYTE Codes[8][2] =
			{
				{ STATUSBOX_STATUS_FONT,			0, },
				{ STATUSBOX_STATUS_FONT,			1, },

				{ STATUSBOX_PROTOCOL_TERMINAL,	0, },
				{ STATUSBOX_PROTOCOL_TERMINAL,	1, },

				{ STATUSBOX_RATE_PARAMETERS,		0, },
				{ STATUSBOX_RATE_PARAMETERS,		1, },

				{ STATUSBOX_TIME_ONLINE,			0, },
				{ STATUSBOX_TIME_ONLINE,			1 },
			};

			struct ObjectCarrier *Carrier = (struct ObjectCarrier *)Object;

			if(Carrier->RPort)
			{
				if(NeedFullRefresh && !Config->ScreenConfig->SplitStatus)
				{
					struct RastPort	*RPort = Carrier->RPort;
					LONG			 Left;

					if((Left = StatusDisplayLeft + (StatusDisplayWidth - Carrier->FullWidth) / 2) < 0)
						Left = 0;

					Left += Carrier->FirstColumn - SZ_GetBoxInfo(Carrier->BoxList,BOX_LEFT);

					SetAPen(RPort,Pens[BACKGROUNDPEN]);
					FillBox(RPort,StatusDisplayLeft,StatusDisplayTop,StatusDisplayWidth,StatusDisplayHeight);

					DrawSeparator(RPort);

					SZ_SetBoxes(Carrier->BoxList,-1,StatusDisplayTop + 3);
					SZ_MoveBoxes(Carrier->BoxList,Left,0);

					SZ_DrawBoxes(RPort,Carrier->BoxList);

					NeedFullRefresh = FALSE;
				}

				SZ_PrintLine(Carrier->RPort,Carrier->BoxArray[Codes[Type][0]],Codes[Type][1],String);
			}
		}

		break;
	}
}

	/* DoInfo(APTR Object,UBYTE Mode,UBYTE Type,STRPTR String):
	 *
	 *	Post an update message to the status display server.
	 */

STATIC VOID
DoInfo(APTR Object,UBYTE Mode,UBYTE Type,CONST_STRPTR String)
{
	struct UpdateMessage	*Msg;
	LONG					 Len = strlen(String) + 1;

		/* Allocate enough space to hold both the string
		 * and the message.
		 */

	if(Msg = (struct UpdateMessage *)AllocVecPooled(sizeof(struct UpdateMessage) + Len,MEMF_ANY | MEMF_CLEAR))
	{
			/* Fill in the message head. */

		Msg->Message.mn_Length = sizeof(struct UpdateMessage) + Len;

			/* Set up the name pointer. */

		Msg->Message.mn_Node.ln_Name = (STRPTR)(Msg + 1);

			/* Copy the string. */

		strcpy(Msg->Message.mn_Node.ln_Name,String);

			/* Fill in the remaining data. */

		Msg->Object	= Object;
		Msg->Mode		= Mode;
		Msg->Type		= Type;

			/* Post the message. */

		PutMsg(StatusDisplayPort,(struct Message *)Msg);
	}
}

	/* UpdateInfo(APTR Object,UBYTE Mode,UBYTE Type,...):
	 *
	 *	Update the information displayed in the status
	 *	area.
	 */

STATIC VOID
UpdateInfo(APTR Object,UBYTE Mode,UBYTE Type,...)
{
	if(Object)
	{
		UBYTE	 MiniBuffer[50];
		STRPTR	String[1];
		LONG	Numeral[3];
		va_list	 VarArgs;

		va_start(VarArgs,Type);

		switch(Type)
		{
			case INFO_STATUS:
				Numeral[0] = va_arg(VarArgs, LONG);

				LimitedStrcpy(sizeof(MiniBuffer),MiniBuffer,LocaleString(ConfigStatusIDs[Numeral[0]]));
				LimitedStrcat(sizeof(MiniBuffer),MiniBuffer,"         ");

				MiniBuffer[9] = 0;

				break;

			case INFO_BUFFER:
				Numeral[0] = va_arg(VarArgs, LONG);

				if(Mode == MODE_SCREEN_NORMAL)
					DoInfo(Object,Mode,Type,ConfigBufferState[Numeral[0]]);

				va_end(VarArgs);

				return;

			case INFO_UNITTIME:
				Numeral[0] = va_arg(VarArgs, LONG);

				LimitedSPrintf(sizeof(MiniBuffer) - 1,&MiniBuffer[1],"%2ld:%02ld",Numeral[0] / 60,Numeral[0] % 60);

				if(Numeral[0] > 0)
					MiniBuffer[0] = '-';
				else
					MiniBuffer[0] = ' ';

				Type = INFO_ONLINETIME;

				break;

			case INFO_ONLINECOST:
				String[0] = va_arg(VarArgs, STRPTR);

				CopyMem(String[0],MiniBuffer,8);
				LimitedStrcat(sizeof(MiniBuffer),MiniBuffer,"          ");

				MiniBuffer[8] = 0;

				Type = INFO_ONLINETIME;

				break;

			case INFO_CURRENTTIME:
				Numeral[0] = va_arg(VarArgs, LONG);
				Numeral[1] = va_arg(VarArgs, LONG);
				Numeral[2] = va_arg(VarArgs, LONG);

				FormatTime(MiniBuffer,sizeof(MiniBuffer),Numeral[0],Numeral[1],Numeral[2]);
				break;

			case INFO_ONLINETIME:
				Numeral[0] = va_arg(VarArgs, LONG);
				Numeral[1] = va_arg(VarArgs, LONG);
				Numeral[2] = va_arg(VarArgs, LONG);

				LimitedSPrintf(sizeof(MiniBuffer),MiniBuffer,"%02ld:%02ld:%02ld",Numeral[0],Numeral[1],Numeral[2]);
				break;

			case INFO_BAUDRATE:
				Numeral[0] = va_arg(VarArgs, LONG);

				if(LocaleBase)
					LimitedSPrintf(sizeof(MiniBuffer),MiniBuffer,"%lD        ",Numeral[0]);
				else
					LimitedSPrintf(sizeof(MiniBuffer),MiniBuffer,"%ld        ",Numeral[0]);

				MiniBuffer[7] = 0;

				break;

			case INFO_PROTOCOL:
			case INFO_EMULATION:
				String[0] = va_arg(VarArgs, STRPTR);

				CopyMem(String[0],MiniBuffer,12);
				LimitedStrcat(sizeof(MiniBuffer),MiniBuffer,"           ");

				MiniBuffer[12] = 0;

				break;

			case INFO_PARAMETERS:
				Numeral[0] = va_arg(VarArgs, LONG);
				Numeral[1] = va_arg(VarArgs, LONG);
				Numeral[2] = va_arg(VarArgs, LONG);

				if(Mode == MODE_SCREEN_COMPRESSED)
				{
					STATIC UBYTE Parities[5] =
					{
						'N','E','O','M','S'
					};

					LimitedSPrintf(sizeof(MiniBuffer),MiniBuffer,"%ld-%lc-%ld",Numeral[0],Parities[Numeral[1]],Numeral[2]);
				}
				else
					LimitedSPrintf(sizeof(MiniBuffer),MiniBuffer,"%ld-%s-%ld",Numeral[0],ConfigParity[Numeral[1]],Numeral[2]);

				break;
		}

		DoInfo(Object,Mode,Type,MiniBuffer);

		va_end(VarArgs);
	}
}

	/* VisualBeep():
	 *
	 *	Handle the visual part of the display beep.
	 */

STATIC BOOL
VisualBeep(ColourTable **OldPtr,ColourTable **NewPtr)
{
	ColourTable *Old,*New;

	Old = CreateColourTable(Window->WScreen->ViewPort.ColorMap->Count,NULL,NULL);
	New = CreateColourTable(Window->WScreen->ViewPort.ColorMap->Count,NULL,NULL);

	if(New && Old)
	{
		ULONG Red,Green,Blue;
		LONG i;

		GrabColours(&Window->WScreen->ViewPort,Old);
		CopyColours(Old,New);

		for(i = 0 ; i < Old->NumColours ; i++)
		{
			Red		= (Old->Entry[i].Red	 >> 24) + 64;
			Green	= (Old->Entry[i].Green	 >> 24) + 64;
			Blue	= (Old->Entry[i].Blue	 >> 24) + 64;

			if(Red > 255)
				Red = 255;

			if(Green > 255)
				Green = 255;

			if(Blue > 255)
				Blue = 255;

			New->Entry[i].Red	= SPREAD(Red);
			New->Entry[i].Green	= SPREAD(Green);
			New->Entry[i].Blue	= SPREAD(Blue);
		}

		*OldPtr = Old;
		*NewPtr = New;

		return(TRUE);
	}

	DeleteColourTable(Old);
	DeleteColourTable(New);

	*OldPtr = *NewPtr = NULL;

	return(FALSE);
}

	/* StatusDisplayServer(VOID):
	 *
	 *	Yet another asynchronous background task to display
	 *	some information.
	 */

STATIC VOID SAVE_DS
StatusDisplayServer(VOID)
{
		/* Create the interface port. */

	if(StatusDisplayPort = CreateMsgPort())
	{
		struct UpdateMessage	*Msg;
		ULONG					 Signals;

			/* Ring back... */

		Signal((struct Task *)StatusProcess,SIG_HANDSHAKE);

			/* Wait for messages or termination signal. */

		FOREVER
		{
			Signals = Wait(SIG_KILL | PORTMASK(StatusDisplayPort));

				/* Termination? */

			if(Signals & SIG_KILL)
				break;

				/* Message arrival? */

			if(Signals & PORTMASK(StatusDisplayPort))
			{
					/* Process all pending messages. */

				while(Msg = (struct UpdateMessage *)GetMsg(StatusDisplayPort))
				{
					DoStatusInfo(Msg->Object,Msg->Mode,Msg->Type,Msg->Message.mn_Node.ln_Name);

					FreeVecPooled(Msg);
				}
			}
		}

			/* Remove all pending messages. */

		while(Msg = (struct UpdateMessage *)GetMsg(StatusDisplayPort))
			FreeVecPooled(Msg);

			/* Remove the msgport. */

		DeleteMsgPort(StatusDisplayPort);
	}

		/* Lock & quit... */

	Forbid();

	StatusDisplayTask = NULL;

	Signal((struct Task *)StatusProcess,SIG_HANDSHAKE);

	RemTask(NULL);
}

	/* StatusServer():
	 *
	 *	Asynchronous process to continuosly display the current
	 *	terminal settings.
	 */

VOID SAVE_DS
StatusServer()
{
	STATIC struct timeval	 OnlineTime,
							 DeltaTime,
							 LastTime,
							 PeriodTime;
	STATIC LONG				 BeepCount;

	STATIC BOOL				 LocalWasOnline;

	BOOL					 LocalIsOnline;

	BOOL					 PeriodAction;

	struct TextBox			*BoxArray[4],
							*BoxList = NULL,
							*Box;

	struct RastPort			*RPort;

	APTR					 SomeObject = NULL;
	struct ObjectCarrier	 Carrier;

	struct timerequest		*StatusTimeRequest;
	struct MsgPort			*StatusTimePort;
	struct timeval			 Now;

	BOOL					 Background			= FALSE,
							 FlashIt			= FALSE,
							 SetColours			= FALSE,
							 StandardColours	= TRUE,
							 KeepGoing			= TRUE,
							 Beeping			= FALSE;

	WORD					 StatusMode			= Config->ScreenConfig->StatusLine;
	WORD					 ShowCounter		= 0;

	UBYTE					 LastProtocol[MAX_FILENAME_LENGTH],
							 LastEmulationName[MAX_FILENAME_LENGTH];

	LONG					 LastFrozen			= -1,
							 LastEmulation		= -1,
							 LastBitsPerChar	= -1,
							 LastParity			= -1,
							 LastStopBits		= -1,
							 LastStatus			= -1;

	LONG					 LastBaud			= -1;

	LONG					 i,
							 ThisHour,
							 ThisMinute,
							 BoxCounter = 0,
							 FullWidth;
	LONG					 ColumnLeft[4],
							 ColumnWidth[4],
							 Max,
							 Len;

	ColourTable				*Old = NULL,
							*New = NULL;

	BOOL					 AllFine = TRUE;
	LONG					 Mode = 0;

	StatusLineWidth = 0;

	LastProtocol[0] = 0;

	LastEmulationName[0] = 0;

	LocalizeString(ConfigBufferState,MSG_TERMSTATUSDISPLAY_FROZEN_TXT,MSG_TERMSTATUSDISPLAY_RECORDING_TXT);
	LocalizeString(ConfigEmulation,MSG_TERMAUX_ANSI_VT102_TXT,MSG_TERMAUX_HEX_TXT);
	LocalizeString(ConfigParity,MSG_TERMAUX_NONE_TXT,MSG_TERMAUX_SPACE_TXT);

	if(StatusWindow)
		RPort = StatusWindow->RPort;
	else
		RPort = StatusRPort;

	if(RPort)
	{
		SetAPen(RPort,Pens[BACKGROUNDPEN]);
		FillBox(RPort,StatusDisplayLeft,StatusDisplayTop,StatusDisplayWidth,StatusDisplayHeight);

			/* Render the information. */

		if(StatusWindow)
			SZ_SizeSetup(StatusWindow->WScreen,(struct TextAttr *)&UserFont);
		else
			SZ_SizeSetup(Window->WScreen,(struct TextAttr *)&UserFont);

		if(StatusMode == STATUSLINE_COMPRESSED)
		{
			StatusOffset = (StatusDisplayWidth - 80 * UserFontWidth) / 2;

			SetAPen(RPort,Pens[TEXTPEN]);
			FillBox(RPort,StatusDisplayLeft,StatusDisplayTop,StatusDisplayWidth,StatusDisplayHeight);

			SetPens(RPort,Pens[BACKGROUNDPEN],Pens[TEXTPEN],JAM2);

			SetFont(RPort,UserTextFont);
		}
		else
		{
			SetBPen(RPort,Pens[BACKGROUNDPEN]);

				/* Draw a separating line. */

			if(!Config->ScreenConfig->SplitStatus)
				DrawSeparator(RPort);

			SetAPen(RPort,Pens[TEXTPEN]);

			ColumnLeft[0] = SZ_LeftOffsetN(MSG_TERMSTATUSDISPLAY_STATUS_TXT,MSG_TERMSTATUSDISPLAY_FONT_TXT,-1);
			ColumnLeft[1] = SZ_LeftOffsetN(MSG_TERMSTATUSDISPLAY_PROTOCOL_TXT,MSG_TERMSTATUSDISPLAY_TERMINAL_TXT,-1);
			ColumnLeft[2] = SZ_LeftOffsetN(MSG_TERMSTATUSDISPLAY_BAUDRATE_TXT,MSG_TERMSTATUSDISPLAY_PARAMETERS_TXT,-1);
			ColumnLeft[3] = SZ_LeftOffsetN(MSG_TERMSTATUSDISPLAY_TIME_TXT,MSG_TERMSTATUSDISPLAY_ONLINE_TXT,-1);

			Max = 0;

			for(i = 0 ; i < NUM_ELEMENTS(ConfigStatusIDs) ; i++)
			{
				if((Len = SZ_BoxWidth(strlen(LocaleString(ConfigStatusIDs[i])))) > Max)
					Max = Len;
			}

			for(i = 0 ; ConfigBufferState[i] ; i++)
			{
				if((Len = SZ_BoxWidth(strlen(ConfigBufferState[i]))) > Max)
					Max = Len;
			}

			ColumnWidth[0] = Max;

			Max = SZ_BoxWidth(12);

			for(i = 0 ; ConfigEmulation[i] ; i++)
			{
				if((Len = SZ_BoxWidth(strlen(ConfigEmulation[i]))) > Max)
					Max = Len;
			}

			ColumnWidth[1] = Max;

			Max = SZ_BoxWidth(10);

			for(i = 0 ; ConfigParity[i] ; i++)
			{
				if((Len = SZ_BoxWidth(4 + strlen(ConfigParity[i]))) > Max)
					Max = Len;
			}

			ColumnWidth[2] = Max;

			ColumnWidth[3] = SZ_BoxWidth(8);

			FullWidth = 0;

			for(i = 0 ; i < 4 ; i++)
				FullWidth += ColumnWidth[i] + ColumnLeft[i];

			FullWidth += 4 + 3 * InterWidth;

			Carrier.FullWidth	= FullWidth;
			Carrier.FirstColumn	= ColumnLeft[0] + 2;

			if(!Config->ScreenConfig->SplitStatus)
			{
				if(FullWidth > StatusDisplayWidth)
					SZ_SetLeftEdge(StatusDisplayLeft + ColumnLeft[0] + 2);
				else
					SZ_SetLeftEdge(StatusDisplayLeft + (StatusDisplayWidth - FullWidth) / 2 + ColumnLeft[0] + 2);

				SZ_SetAbsoluteTop(StatusDisplayTop + 3);
				SZ_SetTopEdge(StatusDisplayTop + 3);
			}
			else
			{
				SZ_SetLeftEdge(StatusDisplayLeft + ColumnLeft[0] + 2);

				SZ_SetAbsoluteTop(StatusDisplayTop + 1);
				SZ_SetTopEdge(StatusDisplayTop + 1);
			}

			SZ_SetWidth(ColumnWidth[0]);

			BoxArray[BoxCounter++] = Box = SZ_CreateTextBox(&BoxList,
				SZ_Lines,		2,
				SZ_AutoWidth,	TRUE,
			TAG_DONE);

			SZ_SetBoxTitles(Box,LocaleString(MSG_TERMSTATUSDISPLAY_STATUS_TXT),LocaleString(MSG_TERMSTATUSDISPLAY_FONT_TXT),NULL);

			SZ_SetWidth(ColumnWidth[1]);
			SZ_AddLeftOffset(ColumnLeft[1]);

			BoxArray[BoxCounter++] = Box = SZ_CreateTextBox(&BoxList,
				SZ_Lines,		2,
				SZ_AutoWidth,	TRUE,
				SZ_NewColumn,	TRUE,
			TAG_DONE);

			SZ_SetBoxTitles(Box,LocaleString(MSG_TERMSTATUSDISPLAY_PROTOCOL_TXT),LocaleString(MSG_TERMSTATUSDISPLAY_TERMINAL_TXT),NULL);

			SZ_SetWidth(ColumnWidth[2]);
			SZ_AddLeftOffset(ColumnLeft[2]);

			BoxArray[BoxCounter++] = Box = SZ_CreateTextBox(&BoxList,
				SZ_Lines,		2,
				SZ_AutoWidth,	TRUE,
				SZ_NewColumn,	TRUE,
			TAG_DONE);

			SZ_SetBoxTitles(Box,LocaleString(MSG_TERMSTATUSDISPLAY_BAUDRATE_TXT),LocaleString(MSG_TERMSTATUSDISPLAY_PARAMETERS_TXT),NULL);

			SZ_SetWidth(ColumnWidth[3]);
			SZ_AddLeftOffset(ColumnLeft[3]);

			BoxArray[BoxCounter] = Box = SZ_CreateTextBox(&BoxList,
				SZ_Lines,		2,
				SZ_AutoWidth,	TRUE,
				SZ_NewColumn,	TRUE,
			TAG_DONE);

			SZ_SetBoxTitles(Box,LocaleString(MSG_TERMSTATUSDISPLAY_TIME_TXT),LocaleString(MSG_TERMSTATUSDISPLAY_ONLINE_TXT),NULL);

			if(!Box)
				AllFine = FALSE;
			else
				SZ_DrawBoxes(RPort,BoxList);
		}
	}
	else
		AllFine = TRUE;

	if((UpdateSig = AllocSignal(-1)) == -1)
		AllFine = FALSE;
	else
		UpdateMask = 1L << UpdateSig;

		/* Everything fine so far? */

	if(AllFine)
	{
		Forbid();

			/* Create the display server task. */

		if(StatusDisplayTask = LocalCreateTask("term Status Display Task",5,(TASKENTRY)StatusDisplayServer,4000,0))
			WaitForHandshake();

		Permit();
	}

		/* Is the display server task up and running? */

	if(StatusDisplayTask)
	{
			/* Create a timer device request. */

		if(StatusTimePort = (struct MsgPort *)CreateMsgPort())
		{
			if(StatusTimeRequest = (struct timerequest *)CreateIORequest(StatusTimePort,sizeof(struct timerequest)))
			{
				if(!OpenDevice(TIMERNAME,UNIT_VBLANK,(struct IORequest *)StatusTimeRequest,0))
				{
						/* Signal our father process
						 * that we're running.
						 */

					Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);

					if(RPort)
					{
						if(StatusMode == STATUSLINE_COMPRESSED)
						{
							Mode = MODE_SCREEN_COMPRESSED;

							SomeObject = RPort;
						}
						else
						{
							Mode = MODE_SCREEN_NORMAL;

							Carrier.RPort		= RPort;
							Carrier.BoxArray	= BoxArray;
							Carrier.BoxList		= BoxList;

							SomeObject = &Carrier;
						}

						UpdateInfo(SomeObject,Mode,INFO_ONLINETIME,0,0,0);
					}

					memset(&DeltaTime,0,sizeof(DeltaTime));
					GetSysTime(&PeriodTime);

						/* Keep on displaying. */

					while(KeepGoing)
					{
							/* Are we to quit? */

						if(SetSignal(0,SIG_KILL) & SIG_KILL)
							KeepGoing = FALSE;

							/* Get the current time. */

						GetSysTime(&Now);

						SafeObtainSemaphoreShared(&OnlineSemaphore);

							/* A connection has just
							 * been established.
							 */

						if(Online && !LocalWasOnline)
						{
							OnlineTime		= Now;
							LocalWasOnline	= TRUE;
						}

						ReleaseSemaphore(&OnlineSemaphore);

							/* Print the current time. */

						ThisHour	= (Now.tv_secs % (24 * 60 * 60)) / (60 * 60);
						ThisMinute	= (Now.tv_secs % (60 * 60)) / 60;

						UpdateInfo(SomeObject,Mode,INFO_CURRENTTIME,ThisHour,ThisMinute,Now.tv_secs % 60);

							/* Handle routine checkup actions. */

						if(PeriodAction = (BOOL)(PeriodTime.tv_secs < Now.tv_secs && Now.tv_secs - PeriodTime.tv_secs >= ROUTINE_CHECK_INTERVAL))
						{
							PeriodTime = Now;

							Signal((struct Task *)ThisProcess,SIG_CHECK);
						}

						SafeObtainSemaphoreShared(&OnlineSemaphore);
						LocalIsOnline = Online;
						ReleaseSemaphore(&OnlineSemaphore);

						if(LocalIsOnline)
						{
								/* Show the time we have been online
								 * so far.
								 */

							DeltaTime = Now;

							SubTime(&DeltaTime,&OnlineTime);

							if(Now.tv_secs != LastTime.tv_secs)
							{
								UBYTE Sum[20];
								WORD What = 0;

								LastTime = Now;

								switch(Config->ScreenConfig->TimeMode)
								{
									case ONLINETIME_TIME:

										What = SHOW_OnlineTime;
										break;

									case ONLINETIME_COST:

										What = SHOW_Cost;
										break;

									case ONLINETIME_BOTH:

										if(PeriodAction)
											ShowCounter = (ShowCounter + 1) % 3;

										switch(ShowCounter)
										{
											case 0:

												What = SHOW_OnlineTime;
												break;

											case 1:

												What = SHOW_Cost;
												break;

											case 2:

												What = SHOW_UnitTime;
												break;
										}

										break;
								}

								OnlineMinutes = DeltaTime.tv_secs / 60;

								switch(What)
								{
									case SHOW_OnlineTime:

										UpdateInfo(SomeObject,Mode,INFO_ONLINETIME,(DeltaTime.tv_secs % 86400) / 3600,(DeltaTime.tv_secs % 3600) / 60,DeltaTime.tv_secs % 60);
										break;

									case SHOW_Cost:

										CreateSum((QueryAccountantCost() + 5000) / 10000,TRUE,Sum,sizeof(Sum));

										UpdateInfo(SomeObject,Mode,INFO_ONLINECOST,Sum);
										break;

									case SHOW_UnitTime:

										UpdateInfo(SomeObject,Mode,INFO_UNITTIME,QueryAccountantTime(NULL));
										break;
								}
							}
						}
						else
						{
							if(LocalWasOnline)
							{
								LocalWasOnline = FALSE;

								UpdateInfo(SomeObject,Mode,INFO_ONLINETIME,(DeltaTime.tv_secs % 86400) / 3600,(DeltaTime.tv_secs % 3600) / 60,DeltaTime.tv_secs % 60);
							}
						}

							/* Take care of the visual beep
							 * if enabled.
							 */

						if(Beeping)
						{
							if(!(BeepCount--))
							{
								Beeping = FALSE;

								LoadColourTable(VPort,Old,NULL,0);

								DeleteColourTable(Old);
								DeleteColourTable(New);

								Old = New = NULL;

									/* Clear the signal bit. */

								ClrSignal(SIG_BELL);
							}
						}

							/* Are we to show a visual beep? */

						if(SetSignal(0,SIG_BELL) & SIG_BELL)
						{
							if(Config->TerminalConfig->BellMode == BELL_SYSTEM)
								DisplayBeep(Window->WScreen);
							else
							{
								if(Screen && !Config->ScreenConfig->UseWorkbench && !Beeping && (Config->TerminalConfig->BellMode == BELL_VISIBLE || Config->TerminalConfig->BellMode == BELL_BOTH))
								{
									if(VisualBeep(&Old,&New))
									{
										LoadColourTable(VPort,New,NULL,0);

										Beeping = TRUE;

										BeepCount = 1;
									}
								}
							}
						}

							/* Display the current terminal
							 * status.
							 */

						if(LastStatus != GetStatus())
							UpdateInfo(SomeObject,Mode,INFO_STATUS,LastStatus = GetStatus());

							/* Show the current transfer
							 * protocol.
							 */

						if(strcmp(LastProtocol,TransferProtocolName))
						{
							STRPTR String;

							strcpy(LastProtocol,TransferProtocolName);

							if(LastProtocol[0])
								String = LastProtocol;
							else
								String = "-";

							UpdateInfo(SomeObject,Mode,INFO_PROTOCOL,String);
						}

							/* Show the current baud
							 * rate.
							 */

						if(LastBaud != Config->SerialConfig->BaudRate)
							UpdateInfo(SomeObject,Mode,INFO_BAUDRATE,LastBaud = Config->SerialConfig->BaudRate);

							/* Show the current
							 * terminal font.
							 */

						if(LastFrozen != BufferFrozen)
						{
							LastFrozen = BufferFrozen;

							UpdateInfo(SomeObject,Mode,INFO_BUFFER,LastFrozen != TRUE);
						}

							/* Show the current terminal
							 * emulation.
							 */

						if(LastEmulation != Config->TerminalConfig->EmulationMode || (Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL && Stricmp(EmulationName,LastEmulationName)))
						{
							LastEmulation = Config->TerminalConfig->EmulationMode;

							if(Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL)
							{
								UpdateInfo(SomeObject,Mode,INFO_EMULATION,EmulationName);

								strcpy(LastEmulationName,EmulationName);
							}
							else
								UpdateInfo(SomeObject,Mode,INFO_EMULATION,ConfigEmulation[LastEmulation]);
						}

							/* Show the current serial
							 * parameters (parity, etc).
							 */

						if(LastBitsPerChar != Config->SerialConfig->BitsPerChar || LastParity != Config->SerialConfig->Parity || LastStopBits != Config->SerialConfig->StopBits)
						{
							LastBitsPerChar	= Config->SerialConfig->BitsPerChar;
							LastParity		= Config->SerialConfig->Parity;
							LastStopBits	= Config->SerialConfig->StopBits;

							UpdateInfo(SomeObject,Mode,INFO_PARAMETERS,LastBitsPerChar,LastParity,LastStopBits);
						}

							/* Wait another half a second. */

						if(KeepGoing)
						{
							ULONG	Mask;
							BOOL	ResetTime = FALSE;

							StatusTimeRequest->tr_node.io_Command	= TR_ADDREQUEST;
							StatusTimeRequest->tr_time.tv_secs		= 0;
							StatusTimeRequest->tr_time.tv_micro		= MILLION / 2;

							SetSignal(0,PORTMASK(StatusTimePort));
							SendIO((struct IORequest *)StatusTimeRequest);

							FOREVER
							{
								Mask = Wait(SIG_BELL | PORTMASK(StatusTimePort) | SIG_CLOSEWINDOW | SIG_RESETTIME | UpdateMask);

								if(Mask & UpdateMask)
								{
									Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);

									Wait(UpdateMask);

									NeedFullRefresh = TRUE;

									UpdateInfo(SomeObject,Mode,INFO_STATUS,LastStatus = GetStatus());
								}

									/* Close the window and keep quiet? */

								if(Mask & SIG_CLOSEWINDOW)
								{
									Forbid();

									ShakeHands(StatusDisplayTask,SIG_KILL);

									if(BoxList)
									{
										SZ_FreeBoxes(BoxList);

										BoxList = NULL;
									}

									SomeObject = NULL;

									Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);

									Config->ScreenConfig->StatusLine = STATUSLINE_DISABLED;

									Permit();
								}

								if(Mask & SIG_BELL)
								{
									if(Config->TerminalConfig->BellMode == BELL_SYSTEM)
										DisplayBeep(Window->WScreen);
									else
									{
										if(Screen && !Config->ScreenConfig->UseWorkbench && !Beeping && (Config->TerminalConfig->BellMode == BELL_VISIBLE || Config->TerminalConfig->BellMode == BELL_BOTH))
										{
											if(VisualBeep(&Old,&New))
											{
												LoadColourTable(VPort,New,NULL,0);

												Beeping = TRUE;

												BeepCount = 1;
											}
										}
									}
								}

									/* Reset the online time counter. */

								if(Mask & SIG_RESETTIME)
									ResetTime = TRUE;

								if(Mask & PORTMASK(StatusTimePort))
								{
									WaitIO((struct IORequest *)StatusTimeRequest);

									break;
								}
							}

								/* Reset the online time. */

							if(ResetTime)
								GetSysTime(&OnlineTime);
						}

							/* Make the colours blink. */

						if(Screen && !Config->ScreenConfig->UseWorkbench && !Beeping)
						{
							if(Screen == IntuitionBase->FirstScreen)
							{
									/* No main screen window active? */

								if(StatusWindow)
								{
									if(!(Window->Flags & WFLG_WINDOWACTIVE) && !(StatusWindow->Flags & WFLG_WINDOWACTIVE))
										StandardColours = TRUE;
								}
								else
								{
									if(!(Window->Flags & WFLG_WINDOWACTIVE))
										StandardColours = TRUE;
								}

									/* Menu button pressed or window disabled? */

								if(Window->Flags & (WFLG_MENUSTATE | WFLG_INREQUEST))
									StandardColours = TRUE;

									/* User is currently dragging the
									 * mouse in order to mark something
									 * on the screen?
									 */

								if(Marking)
									StandardColours = TRUE;

								Background = FALSE;
							}
							else
							{
								if(!Background)
									StandardColours = TRUE;

								Background = TRUE;
							}

							if(StandardColours)
							{
								if(!SetColours)
								{
									LoadColourTable(VPort,NormalColourTable,NormalColours,PaletteSize);

									SetColours = TRUE;
								}

								StandardColours = FlashIt = FALSE;
							}
							else
							{
									/* Are we to flash the display? */

								if(Config->ScreenConfig->Blinking)
								{
									if(Screen == IntuitionBase->FirstScreen)
									{
										if(FlashIt)
										{
											LoadColourTable(VPort,BlinkColourTable,BlinkColours,PaletteSize);

											SetColours = FALSE;
										}
										else
										{
											LoadColourTable(VPort,NormalColourTable,NormalColours,PaletteSize);

											SetColours = TRUE;
										}
									}

									FlashIt ^= TRUE;
								}
							}
						}
					}

					CloseDevice((struct IORequest *)StatusTimeRequest);
				}

				DeleteIORequest((struct IORequest *)StatusTimeRequest);
			}

			DeleteMsgPort(StatusTimePort);
		}

		if(Old)
			LoadColourTable(VPort,Old,NULL,0);

		DeleteColourTable(Old);
		DeleteColourTable(New);

		ShakeHands(StatusDisplayTask,SIG_KILL);

		if(BoxList)
			SZ_FreeBoxes(BoxList);
	}

	if(UpdateSig != -1)
		FreeSignal(UpdateSig);

		/* Signal the father process that we're done
		 * and quietly remove ourselves.
		 */

	Forbid();

	Signal((struct Task *)ThisProcess,SIG_HANDSHAKE);

	StatusProcess = NULL;
}

	/* ForceStatusUpdate():
	 *
	 *	Make sure that the status display gets updated
	 *	as soon as possible.
	 */

VOID
ForceStatusUpdate()
{
	if(StatusProcess && Window)
	{
		ShakeHands((struct Task *)StatusProcess,UpdateMask);

			/* Ok, the status process is waiting, now update the data */

		if(Marking)
			WindowMarkerStop();

		UpdateTerminalLimits();

			/* Clear the window */

		SetAPen(Window->RPort,Pens[BACKGROUNDPEN]);
		FillBox(Window->RPort,Window->BorderLeft,Window->BorderTop,Window->Width - (Window->BorderLeft + Window->BorderRight),Window->Height - (Window->BorderTop + Window->BorderBottom));

			/* Restart the status process */

		Signal((struct Task *)StatusProcess,UpdateMask);

			/* Repaint the window border, in case it got trashed */

		RefreshWindowFrame(Window);
	}
}

/*
**	XEM.c
**
**	External emulation support routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

typedef VOID (*CALLBACK)(VOID);

long SAVE_DS
xem_sflush(void)
{
	return((long)FlushSerialRead());
}

long SAVE_DS
xem_squery(void)
{
	if(WriteRequest)
	{
		ULONG	Waiting;
		UWORD	Status;

		GetSerialInfo(&Waiting,&Status);

			/* Return error if carrier is lost. */

#ifdef __AROS__
		if((Status & SER_STATF_COMCD) && Config->SerialConfig->CheckCarrier && !Config->SerialConfig->DirectConnection)
#else
		if((Status & CIAF_COMCD) && Config->SerialConfig->CheckCarrier && !Config->SerialConfig->DirectConnection)
#endif
			SetOnlineState(FALSE);
		else
			return((LONG)Waiting);
	}

	return(-1);
}

long SAVE_DS ASM
xem_sread(REG(a0) char *Buffer,REG(d0) long Size,REG(d1) long Timeout)
{
	DB(kprintf("xem_sread(0x%08lx,%ld,%ld)\n",Buffer,Size,Timeout));

		/* Are both IORequests available? */

	if(WriteRequest && ReadRequest)
	{
			/* Valid size parameter? */

		if(Size > 0)
		{
			ULONG	Waiting;
			UWORD	Status;

			GetSerialInfo(&Waiting,&Status);

				/* Return error if carrier is lost. */

			if(Config->SerialConfig->CheckCarrier && !Config->SerialConfig->DirectConnection)
			{
#ifdef __AROS__
				if(Status & SER_STATF_COMCD)
#else
				if(Status & CIAF_COMCD)
#endif
				{
					SetOnlineState(FALSE);

					TransferError = TRUE;

					return(-1);
				}
			}

			/* ALWAYS */
			{
				if(Waiting)
				{
						/* No timeout specified? Read as many
						 * bytes as available.
						 */

					if(!Timeout)
					{
						if(Waiting > Size)
							Waiting = Size;

						if(DoSerialRead(Buffer,Waiting))
							Waiting = ReadRequest->IOSer.io_Actual;

						BytesIn += Waiting;

						return((LONG)Waiting);
					}

						/* Enough data pending to be read? */

					if(Waiting >= Size)
					{
						if(DoSerialRead(Buffer,Size))
							Size = ReadRequest->IOSer.io_Actual;

						BytesIn += Size;

						return(Size);
					}
				}
				else
				{
						/* No timeout & no bytes available:
						 * return immediately.
						 */

					if(!Timeout)
						return(0);
				}
			}

			/* ALWAYS */
			{
				ULONG	SignalSet,
						SerialMask = PORTMASK(ReadPort);

					/* Set up the timer. */

				StartTime(Timeout / MILLION,Timeout % MILLION);

					/* Set up the read request. */

				StartSerialRead(Buffer,Size);

				FOREVER
				{
					SignalSet = Wait(SerialMask | SIG_TIMER);

						/* Receive buffer filled? */

					if(SignalSet & SerialMask)
					{
							/* Abort the timer request. */

						StopTime();

							/* Did the request terminate gracefully? */

						if(WaitSerialRead())
							Size = ReadRequest->IOSer.io_Actual;

						BytesIn += Size;

						return(Size);
					}

						/* Hit by timeout? */

					if(SignalSet & SIG_TIMER)
					{
							/* Abort the read request. */

						StopSerialRead();

							/* Remove the timer request. */

						WaitTime();

							/* Did the driver receive any
							 * data?
							 */

						if(ReadRequest->IOSer.io_Actual)
						{
							BytesIn += ReadRequest->IOSer.io_Actual;

							return((LONG)ReadRequest->IOSer.io_Actual);
						}
						else
						{
								/* Take a second look and query the number of
								 * bytes ready to be received, there may
								 * still be some bytes in the buffer.
								 * Note: this depends on the way the
								 * driver handles read abort.
								 */

							Waiting = GetSerialWaiting();

								/* Don't read too much. */

							if(Size > Waiting)
								Size = Waiting;

								/* Are there any bytes to be transferred? */

							if(Size)
							{
									/* Read the data. */

								if(DoSerialRead(Buffer,Size))
									Size = ReadRequest->IOSer.io_Actual;

								BytesIn += Size;
							}

							return(Size);
						}
					}
				}
			}
		}
		else
			return(0);
	}
	else
		return(-1);
}

long SAVE_DS ASM
xem_toptions(REG(d0) long NumOpts,REG(a0) struct xem_option *Opts[])
{
	if(NumOpts && Opts)
	{
		enum	{	GAD_USE=1,GAD_CANCEL,GAD_SPECIAL };

		LayoutHandle	*Handle;
		ULONG			 Flags = 0;

			/* We only have 32 bits! */

		if(NumOpts > 32)
			NumOpts = 32;

		if(Handle = LT_CreateHandleTags(Window->WScreen,
			LAHN_LocaleHook,	&LocaleHook,
		TAG_DONE))
		{
			struct Window	*PanelWindow;
			LONG			 i,Split;

			if(NumOpts > 16)
				Split = NumOpts / 2;
			else
				Split = -1;

			LT_New(Handle,
				LA_Type,VERTICAL_KIND,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,HORIZONTAL_KIND,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,VERTICAL_KIND,
					TAG_DONE);
					{
						for(i = 0 ; i < NumOpts ; i++)
						{
							if(Opts[i])
							{
								switch(Opts[i]->xemo_type)
								{
									case XEMO_BOOLEAN:

										LT_New(Handle,
											LA_Type,		CHECKBOX_KIND,
											LA_LabelText,	Opts[i]->xemo_description,
											LA_ID,			GAD_SPECIAL + i,
											GTCB_Checked,	String2Boolean(Opts[i]->xemo_value),
										TAG_DONE);

										break;

									case XEMO_LONG:

										LT_New(Handle,
											LA_Type,				INTEGER_KIND,
											LA_LabelText,			Opts[i]->xemo_description,
											LA_ID,					GAD_SPECIAL + i,
											LA_Chars,				15,
											GTIN_Number,			Atol(Opts[i]->xemo_value),
											LAIN_UseIncrementers,	TRUE,
										TAG_DONE);

										break;

									case XEMO_STRING:

										LT_New(Handle,
											LA_Type,		STRING_KIND,
											LA_LabelText,	Opts[i]->xemo_description,
											LA_ID,			GAD_SPECIAL + i,
											LA_Chars,		15,
											GTST_String,	Opts[i]->xemo_value,
											GTST_MaxChars,	Opts[i]->xemo_length - 1,
										TAG_DONE);

										break;

									case XEMO_COMMPAR:

										LT_New(Handle,
											LA_Type,		STRING_KIND,
											LA_LabelText,	Opts[i]->xemo_description,
											LA_ID,			GAD_SPECIAL + i,
											LA_Chars,		15,
											LA_HighLabel,	TRUE,
											GTST_String,	Opts[i]->xemo_value,
											GTST_MaxChars,	Opts[i]->xemo_length - 1,
										TAG_DONE);

										break;

									case XEMO_HEADER:

										LT_New(Handle,
											LA_Type,		TEXT_KIND,
											LA_LabelText,	Opts[i]->xemo_description,
											LA_HighLabel,	TRUE,
											GTTX_Text,		" ",
										TAG_DONE);

										break;

									case XEMO_COMMAND:

										LT_New(Handle,
											LA_Type,		BUTTON_KIND,
											LA_LabelText,	Opts[i]->xemo_description,
											LA_ID,			GAD_SPECIAL + i,
											LA_Chars,		15,
										TAG_DONE);

										break;
								}
							}

							if(i == Split)
							{
								LT_EndGroup(Handle);

								LT_New(Handle,
									LA_Type,VERTICAL_KIND,
								TAG_DONE);
								{
									LT_New(Handle,
										LA_Type,YBAR_KIND,
									TAG_DONE);

									LT_EndGroup(Handle);
								}

								LT_New(Handle,
									LA_Type,VERTICAL_KIND,
								TAG_DONE);
							}
						}

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
					LAGR_SameSize,	TRUE,
					LAGR_Spread,	TRUE,
				TAG_DONE);
				{
					LT_New(Handle,
						LA_Type,		BUTTON_KIND,
						LA_LabelID,		MSG_GLOBAL_SAVE_TXT,
						LA_ID,			GAD_USE,
						LABT_ReturnKey,	TRUE,
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
				LAWN_TitleText,		OptionTitle ? OptionTitle : LocaleString(MSG_V36_1840),
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
				BOOL				 Done = FALSE;
				ULONG				 MsgClass;
				UWORD				 MsgCode;
				struct Gadget		*MsgGadget;
				BOOL				 CheckFlags = FALSE;

				PushWindow(PanelWindow);

				LT_ShowWindow(Handle,TRUE);

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

									LT_UpdateStrings(Handle);

									Done = CheckFlags = TRUE;
									break;

								case GAD_CANCEL:

									Done = TRUE;
									break;

								default:

									if(MsgGadget->GadgetID - GAD_SPECIAL < NumOpts)
									{
										i = MsgGadget->GadgetID - GAD_SPECIAL;

										if(Opts[i]->xemo_type == XEMO_COMMAND || (Opts[i]->xemo_type == XEMO_COMMPAR && MsgCode != '\t'))
										{
											Flags = (1L << i);

											Done = CheckFlags = TRUE;
										}
									}

									break;
							}
						}
					}
				}
				while(!Done);

				PopWindow();

				if(CheckFlags)
				{
					STRPTR String;

					LT_LockWindow(PanelWindow);

					for(i = 0 ; i < NumOpts ; i++)
					{
						if(Opts[i])
						{
							switch(Opts[i]->xemo_type)
							{
								case XEMO_BOOLEAN:

									if(LT_GetAttributes(Handle,GAD_SPECIAL + i,TAG_DONE) != String2Boolean(Opts[i]->xemo_value))
									{
										Flags |= (1L << i);

										if(LT_GetAttributes(Handle,GAD_SPECIAL + i,TAG_DONE))
											strcpy(Opts[i]->xemo_value,"yes");
										else
											strcpy(Opts[i]->xemo_value,"no");

										NewOptions = TRUE;
									}

									break;

								case XEMO_LONG:

									if(Atol(Opts[i]->xemo_value) != LT_GetAttributes(Handle,GAD_SPECIAL + i,TAG_DONE))
									{
										Flags |= (1L << i);

										SPrintf(Opts[i]->xemo_value,"%ld",LT_GetAttributes(Handle,GAD_SPECIAL + i,TAG_DONE));

										NewOptions = TRUE;
									}

									break;

								case XEMO_COMMPAR:
								case XEMO_STRING:

									if(String = LT_GetString(Handle,GAD_SPECIAL + i))
									{
										if(strcmp(Opts[i]->xemo_value,String))
										{
											Flags |= (1L << i);

											strcpy(Opts[i]->xemo_value,LT_GetString(Handle,GAD_SPECIAL + i));

											NewOptions = TRUE;
										}
									}

									break;
							}
						}
					}

					LT_UnlockWindow(PanelWindow);
				}
				else
					Flags = 0;
			}

			LT_DeleteHandle(Handle);
		}

		return(Flags);
	}
	else
		return(0);
}

	/* xem_swrite():
	 *
	 *	Send a few bytes across the serial line.
	 */

long SAVE_DS ASM
xem_swrite(REG(a0) char *Buffer,REG(d0) long Size)
{
	if(WriteRequest)
	{
		SerWrite(Buffer,Size);

		return(0);
	}
	else
		return(-1);
}

	/* xem_sbreak():
	 *
	 *	Send a break signal across the serial line.
	 */

long SAVE_DS
xem_sbreak(void)
{
	if(!WriteRequest)
		return(-1);
	else
	{
		SendBreak();

		return(0);
	}
}

	/* xem_sstart():
	 *
	 *	Restart serial read activity.
	 */

void SAVE_DS
xem_sstart(void)
{
	RestartSerial();
}

	/* xem_sstop():
	 *
	 *	Stop serial read activity.
	 */

long SAVE_DS
xem_sstop(void)
{
	ClearSerial();

	return(0);
}

	/* xem_tgets(STRPTR Prompt,STRPTR Buffer,ULONG Size):
	 *
	 *	Get a string from the user.
	 */

long SAVE_DS ASM
xem_tgets(REG(a0) const char *Prompt,REG(a1) char *Buffer,REG(d0) long Size)
{
	enum	{	GAD_Use=1,GAD_Cancel,GAD_String };

	LayoutHandle	*Handle;
	LONG			 Success = FALSE;
	UBYTE			 LocalBuffer[256];

	if(strlen(Buffer) > sizeof(LocalBuffer) - 1)
	{
		CopyMem(Buffer,LocalBuffer,sizeof(LocalBuffer) - 1);

		LocalBuffer[sizeof(LocalBuffer) - 1] = 0;
	}
	else
		strcpy(LocalBuffer,Buffer);

	if(!Prompt)
		Prompt = LocaleString(MSG_TERMXPR_INPUT_REQUIRED_TXT);

	if(Handle = LT_CreateHandleTags(Window->WScreen,
		LAHN_LocaleHook,	&LocaleHook,
	TAG_DONE))
	{
		struct Window *PanelWindow;

		LT_New(Handle,
			LA_Type,VERTICAL_KIND,
		TAG_DONE);
		{
			LT_New(Handle,
				LA_Type,		VERTICAL_KIND,
				LA_LabelText,	Prompt,
			TAG_DONE);
			{
				LT_New(Handle,
					LA_Type,		STRING_KIND,
					LA_STRPTR,		LocalBuffer,
					LA_Chars,		30,
					GTST_MaxChars,	Size,
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
					LA_LabelID,		MSG_TERMXPR_OKAY_GAD,
					LA_ID,			GAD_Use,
					LABT_ReturnKey,	TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_New(Handle,
					LA_Type,		BUTTON_KIND,
					LA_LabelID,		MSG_GLOBAL_CANCEL_GAD,
					LA_ID,			GAD_Cancel,
					LABT_EscKey,	TRUE,
					LABT_ExtraFat,	TRUE,
				TAG_DONE);

				LT_EndGroup(Handle);
			}
		}

		if(PanelWindow = LT_Build(Handle,
			LAWN_TitleID,		MSG_GLOBAL_ENTER_TEXT_TXT,
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
			BOOL				 Done = FALSE;
			ULONG				 MsgClass;
			UWORD				 MsgCode;
			struct Gadget		*MsgGadget;

			PushWindow(PanelWindow);

			LT_ShowWindow(Handle,TRUE);

			LT_Activate(Handle,GAD_String);

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
							case GAD_String:

								if(MsgCode == '\r')
								{
									strcpy(Buffer,LocalBuffer);

									Success = Done = TRUE;

									LT_PressButton(Handle,GAD_Use);
								}

								break;

							case GAD_Use:

								strcpy(Buffer,LocalBuffer);

								Success = Done = TRUE;
								break;

							case GAD_Cancel:

								Done = TRUE;
								break;
						}
					}
				}
			}
			while(!Done);

			PopWindow();
		}

		LT_DeleteHandle(Handle);
	}

	return(Success);
}

	/* xem_tbeep(ULONG Times,ULONG Delay):
	 *
	 *	Beep the terminal display.
	 */

void SAVE_DS ASM
xem_tbeep(REG(d0) long Times,REG(d1) long UnusedDelay)	/* ZZZ: what does delay do? */
{
	LONG i;

	for(i = 0 ; i < Times ; i++)
	{
			/* Handle the visual part. */

		if(Config->TerminalConfig->BellMode != BELL_AUDIBLE)
		{
			if(StatusProcess)
				Signal((struct Task *)StatusProcess,SIG_BELL);
		}

			/* Let it beep. */

		if(Config->TerminalConfig->BellMode == BELL_AUDIBLE || Config->TerminalConfig->BellMode == BELL_BOTH)
			SoundPlay(SOUND_BELL);
	}
}

	/* xem_macrodispatch(struct XEmulatorMacroKey *XEM_MacroKey):
	 *
	 *	Dispatch a macro key call.
	 */

long SAVE_DS ASM
xem_macrodispatch(REG(a0) struct XEmulatorMacroKey *XEM_MacroKey)
{
	CALLBACK Routine;

		/* If a routine to call is available (most likely xON or xOFF),
		 * make a call to it, else process the macro key data.
		 */

	if(Routine = (CALLBACK)XEM_MacroKey->xmk_UserData)
		(*Routine)();
	else
		SerialCommand(MacroKeys->Keys[XEM_MacroKey->xmk_Qualifier][XEM_MacroKey->xmk_Code - 0x50]);

	return(0);
}

	/* SetEmulatorOptions(BYTE Mode):
	 *
	 *	Save or load the emulator options.
	 */

BOOL
SetEmulatorOptions(LONG Mode)
{
	BOOL Success = FALSE;

		/* Is the library available and running? */

	if(XEmulatorBase && XEM_IO)
	{
			/* Are we using the new library code? */

		if(XEmulatorBase->lib_Version >= 4)
		{
			UBYTE LocalBuffer[MAX_FILENAME_LENGTH];

				/* Get the name of the library. */

			strcpy(LocalBuffer,FilePart(XEmulatorBase->lib_Node.ln_Name));

				/* Does it have any name? */

			if(LocalBuffer[0])
			{
				UBYTE	OtherBuffer[50];
				LONG	i;

					/* Strip the `.library' bit. */

				for(i = strlen(LocalBuffer) - 1 ; i >= 0 ; i--)
				{
					if(LocalBuffer[i] == '.')
					{
						LocalBuffer[i] = 0;

						break;
					}
				}

					/* What are we to do? */

				if(Mode == XEM_PREFS_LOAD)
				{
						/* Restore settings... */

					strcpy(OtherBuffer,"ENV:");

					if(AddPart(OtherBuffer,LocalBuffer,50))
					{
							/* If we can't load them,
							 * reset to defaults.
							 */

						if(!XEmulatorPreferences(XEM_IO,OtherBuffer,Mode))
						{
							strcpy(OtherBuffer,"ENV:xem");

							if(AddPart(OtherBuffer,LocalBuffer,50))
							{
								if(XEmulatorPreferences(XEM_IO,OtherBuffer,Mode))
									Success = TRUE;
							}
						}
						else
							Success = TRUE;

						if(!Success)
							XEmulatorPreferences(XEM_IO,NULL,XEM_PREFS_RESET);
					}
				}
				else
				{
						/* Save settings to ENV: */

					strcpy(OtherBuffer,"ENV:");

					if(AddPart(OtherBuffer,LocalBuffer,50))
					{
						if(XEmulatorPreferences(XEM_IO,OtherBuffer,Mode))
							Success = TRUE;
					}

					if(Success)
					{
						Success = FALSE;

							/* Save settings to ENVARC: */

						strcpy(OtherBuffer,"ENVARC:");

						if(AddPart(OtherBuffer,LocalBuffer,50))
						{
							if(XEmulatorPreferences(XEM_IO,OtherBuffer,Mode))
								Success = TRUE;
						}
					}
				}
			}
		}
	}

		/* Return result. */

	return(Success);
}

	/* SetupEmulator(BOOL OpenConsole):
	 *
	 *	Initialize the XEM_IO structure.
	 */

STATIC BOOL
SetupEmulator(VOID)
{
	if(!XEM_IO)
	{
		if(XEM_IO = (struct XEM_IO *)AllocVec(sizeof(struct XEM_IO),MEMF_ANY | MEMF_CLEAR | MEMF_PUBLIC))
		{
			XEM_IO->xem_window				= Window;
			XEM_IO->xem_font				= CurrentFont;
			XEM_IO->xem_signal				= &XEM_Signal;
			XEM_IO->xem_screendepth			= GetBitMapDepth(Window->WScreen->RastPort.BitMap);

#ifdef USE_GLUE
			XEM_IO->xem_sread				= xem_sread_glue;
			XEM_IO->xem_swrite				= xem_swrite_glue;
			XEM_IO->xem_sflush				= xem_sflush;
			XEM_IO->xem_sbreak				= xem_sbreak;
			XEM_IO->xem_squery				= xem_squery;
			XEM_IO->xem_sstart				= xem_sstart;
			XEM_IO->xem_sstop				= xem_sstop;
			XEM_IO->xem_tbeep				= xem_tbeep_glue;
			XEM_IO->xem_tgets				= xem_tgets_glue;
			XEM_IO->xem_toptions			= xem_toptions_glue;
			XEM_IO->xem_process_macrokeys	= xem_macrodispatch_glue;
#else
			XEM_IO->xem_sread				= xem_sread;
			XEM_IO->xem_swrite				= xem_swrite;
			XEM_IO->xem_sflush				= xem_sflush;
			XEM_IO->xem_sbreak				= xem_sbreak;
			XEM_IO->xem_squery				= xem_squery;
			XEM_IO->xem_sstart				= xem_sstart;
			XEM_IO->xem_sstop				= xem_sstop;
			XEM_IO->xem_tbeep				= xem_tbeep;
			XEM_IO->xem_tgets				= xem_tgets;
			XEM_IO->xem_toptions			= (APTR)xem_toptions;
			XEM_IO->xem_process_macrokeys	= (APTR)xem_macrodispatch;
#endif	/* USE_GLUE */

			return(TRUE);
		}
	}

	return(FALSE);
}

	/* CloseEmulator():
	 *
	 *	Close the emulation library.
	 */

VOID
CloseEmulator(BOOL Exit)
{
	if(XEmulatorBase)
	{
		if(XEM_IO)
		{
			XEmulatorMacroKeyFilter(XEM_IO,NULL);
			XEmulatorCloseConsole(XEM_IO);
			XEmulatorCleanup(XEM_IO);

			FreeVec(XEM_IO);
			XEM_IO = NULL;
		}

		CloseLibrary(XEmulatorBase);
		XEmulatorBase = NULL;

		WindowJob = SingleWindowJob;
		UpdateJobMask(MainJobQueue,XEMJob,0);

		if(!Exit)
		{
			XEM_Signal = 0;

			strcpy(EmulationName,LocaleString(MSG_TERMXEM_NO_EMULATION_TXT));

			RasterEnabled = TRUE;

			ClearCursor();

			Reset();

			DrawCursor();
		}
	}
}

	/* OpenEmulator(STRPTR Name):
	 *
	 *	Open an emulation library.
	 */

BOOL
OpenEmulator(STRPTR Name)
{
	CloseEmulator(FALSE);

	XEM_HostData.Source			= NULL;
	XEM_HostData.Destination	= NULL;
	XEM_HostData.InESC			= FALSE;
	XEM_HostData.InCSI			= FALSE;

	if(XEmulatorBase = OpenLibrary(Name,0))
	{
		ClearCursor();

		Reset();

		if(SetupEmulator())
		{
			BOOL Success;

			SetMask(RPort,DepthMask);

			ClearSerial();
			Success = XEmulatorSetup(XEM_IO);
			RestartSerial();

			if(Success)
			{
				SetEmulatorOptions(XEM_PREFS_LOAD);

				if(XEmulatorOpenConsole(XEM_IO))
				{
					STRPTR LibName = FilePart(XEmulatorBase->lib_Node.ln_Name);

					strcpy(EmulationName,&LibName[3]);

					EmulationName[strlen(EmulationName) - 8] = 0;

					SetupXEM_MacroKeys(MacroKeys);

					if(XEM_Signal == PORTMASK(Window->UserPort))
						WindowJob = WindowAndXEMJob;
					else
					{
						WindowJob = SingleWindowJob;
						UpdateJobMask(MainJobQueue,XEMJob,XEM_Signal);
					}

					return(TRUE);
				}
			}
		}

		DrawCursor();

		CloseLibrary(XEmulatorBase);
		XEmulatorBase = NULL;
	}

	XEM_Signal = 0;

	strcpy(EmulationName,LocaleString(MSG_TERMXEM_NO_EMULATION_TXT));

	WindowJob = SingleWindowJob;
	UpdateJobMask(MainJobQueue,XEMJob,0);

	return(FALSE);
}

	/* xOn():
	 *
	 *	Small local routine, complements XOff().
	 */

STATIC VOID
xOn(VOID)
{
	Clear_xOFF();

	if(Config->SerialConfig->PassThrough)
	{
		UBYTE c = XON;
		SerWrite(&c,1);
	}
}

	/* xOff():
	 *
	 *	Small local routine, complements XOn().
	 */

STATIC VOID
xOff(VOID)
{
	Set_xOFF();

	if(Config->SerialConfig->PassThrough)
	{
		UBYTE c = XOF;
		SerWrite(&c,1);
	}
}

	/* SetupXEM_MacroKeys(struct MacroKeys *Keys):
	 *
	 *	Sets up the internal representation of the macro key
	 *	data to fit the XEM specification.
	 */

VOID
SetupXEM_MacroKeys(struct MacroKeys *Keys)
{
		/* Are we allowed to do what we want to do? */

	if(XEM_MacroKeys && XEmulatorBase && Config->TerminalConfig->EmulationMode == EMULATION_EXTERNAL)
	{
		LONG i,j,k = 0;

			/* Run down the list of qualifiers. */

		for(i = XMKQ_NONE ; i <= XMKQ_CONTROL ; i++)
		{
				/* Run down the function keys. */

			for(j = 0 ; j < 10 ; j++)
			{
					/* If the key has no data attached,
					 * don't use it in the list.
					 */

				if(Keys->Keys[i][j][0])
				{
					XEM_MacroKeys[k].xmk_Type		= XMKT_RAWKEY;
					XEM_MacroKeys[k].xmk_Qualifier	= i;
					XEM_MacroKeys[k].xmk_Code		= 0x50 + j;
					XEM_MacroKeys[k].xmk_UserData	= NULL;

					AddTail(&XEM_MacroList,(struct Node *)&XEM_MacroKeys[k++]);
				}
			}
		}

			/* Take care of the rest, add support for the xON key. */

		XEM_MacroKeys[k].xmk_Type 		= XMKT_COOKED;
		XEM_MacroKeys[k].xmk_Qualifier	= 0;
		XEM_MacroKeys[k].xmk_Code		= XON;
		XEM_MacroKeys[k].xmk_UserData	= (APTR)xOn;

		AddTail(&XEM_MacroList,(struct Node *)&XEM_MacroKeys[k++]);

			/* Take care of the xOFF key. */

		XEM_MacroKeys[k].xmk_Type 		= XMKT_COOKED;
		XEM_MacroKeys[k].xmk_Qualifier	= 0;
		XEM_MacroKeys[k].xmk_Code		= XOF;
		XEM_MacroKeys[k].xmk_UserData	= (APTR)xOff;

		AddTail(&XEM_MacroList,(struct Node *)&XEM_MacroKeys[k]);

			/* Make the emulator notice the new settings. */

		XEmulatorMacroKeyFilter(XEM_IO,(struct XEmulatorMacroKey *)&XEM_MacroList);
	}
}

	/* HandleXEM():
	 *
	 *	Invokes the external emulation signal callback.
	 */

VOID
HandleXEM()
{
	if(!XEmulatorSignal(XEM_IO,XEM_Signal))
	{
		CloseEmulator(FALSE);

		ResetDisplay = TRUE;
		ActivateJob(MainJobQueue,ResetDisplayJob);
	}
}

	/* HandleXEMJob(JobNode *UnusedJob):
	 *
	 *	Just makes a call to HandleXEM().
	 */

BOOL
HandleXEMJob(JobNode *UnusedJob)
{
	HandleXEM();

	return(FALSE);
}

/* $Revision Header * Header built automatically - do not edit! *************
 *
 *	(C) Copyright 1990 by Olaf 'Olsen' Barthel & MXM
 *
 *	Name .....: termLite.c
 *	Created ..: Saturday 8-Jun-91 12:32
 *	Revision .: 0
 *
 *	Date            Author          Comment
 *	=========       ========        ====================
 *	08-Jun-91       Olsen           Created this file!
 *
 * $Revision Header ********************************************************/

	/* System includes. */

#include <intuition/intuitionbase.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <devices/console.h>
#include <devices/conunit.h>
#include <devices/serial.h>
#include <exec/execbase.h>
#include <dos/dosextens.h>
#include <exec/memory.h>
#include <dos/rdargs.h>

#if 0
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#include <clib/icon_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#else
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/icon.h>
#include <proto/exec.h>
#include <proto/dos.h>
#endif

#include <string.h>
#include <stdarg.h>

	/* A handy macro to access AbsExecBase. */
#if 0
#define SysBase		(*(struct ExecBase **)4L)
#endif
	/* The following pragmas provide the nessary support
	 * for reentrancy: they permit to keep the library bases
	 * defined locally.
	 */

	/* exec.library calls. */

#if 0
#pragma libcall SysBase WaitPort 180 801
#pragma libcall SysBase GetMsg 174 801
#pragma libcall SysBase ReplyMsg 17a 901
#pragma libcall SysBase CloseLibrary 19e 901
#pragma libcall SysBase OpenLibrary 228 902
#pragma libcall SysBase AllocVec 2ac 1002
#pragma libcall SysBase FreeVec 2b2 901
#pragma libcall SysBase Forbid 84 0
#pragma libcall SysBase SendIO 1ce 901
#pragma libcall SysBase WaitIO 1da 901
#pragma libcall SysBase AbortIO 1e0 901
#pragma libcall SysBase Wait 13e 1
#pragma libcall SysBase DoIO 1c8 901
#pragma libcall SysBase OpenDevice 1bc 190804
#pragma libcall SysBase CloseDevice 1c2 901
#pragma libcall SysBase CreateIORequest 28e 802
#pragma libcall SysBase DeleteIORequest 294 801
#pragma libcall SysBase CreateMsgPort 29a 0
#pragma libcall SysBase DeleteMsgPort 2a0 801
#pragma libcall SysBase CopyMem 270 9803

	/* dos.library calls. */

#pragma libcall DOSBase ReadArgs 31E 32103
#pragma libcall DOSBase FreeArgs 35A 101
#pragma libcall DOSBase StrToLong 330 2102
#pragma libcall DOSBase VPrintf 3BA 2102

	/* icon.library calls. */

#pragma libcall IconBase GetDiskObject 4E 801
#pragma libcall IconBase FreeDiskObject 5A 801
#pragma libcall IconBase FindToolType 60 9802

	/* intuition.library calls. */

#pragma libcall IntuitionBase LockPubScreen 1FE 801
#pragma libcall IntuitionBase UnlockPubScreen 204 9802
#pragma libcall IntuitionBase OpenWindowTagList 25E 9802
#pragma libcall IntuitionBase CloseWindow 48 801
#pragma libcall IntuitionBase EasyRequestArgs 24C BA9804
#endif
	/* Signal bits to wait for. */

#define SIG_SERIAL	(1 << SerReadPort -> mp_SigBit)
#define SIG_CONSOLE	(1 << ConReadPort -> mp_SigBit)
#define SIG_WINDOW	(1 << Window -> UserPort -> mp_SigBit)

	/* Size of the console read buffer and the serial read buffer. */

#define CON_READ_SIZE	256
#define SER_READ_SIZE	1024

	/* Argument vector offsets. */

enum	{	ARG_DEVICE,ARG_UNIT };

	/* termLite():
	 *
	 *	termLite main program, handles all those neat
	 *	IO functions in just a single routine.
	 */

LONG /* __saveds
termLite(VOID) */
main(VOID)
{
		/* Window definitions, just a lookup table, will get
		 * copied to local stack vars and modified lateron.
		 * (to save the relocs).
		 */

	STATIC ULONG DefaultTags[20 * 2 + 1] =
	{
		WA_Top,			(IPTR)NULL,
		WA_Width,		(IPTR)NULL,
		WA_Height,		(IPTR)NULL,
		WA_MinHeight,		(IPTR)NULL,
		WA_Title,		(IPTR)NULL,
		WA_MinWidth,		(IPTR)NULL,
		WA_MaxWidth,		~0,
		WA_MaxHeight,		~0,
		WA_Left,		0,
		WA_SimpleRefresh,	TRUE,
		WA_SizeGadget,		TRUE,
		WA_DragBar,		TRUE,
		WA_DepthGadget,		TRUE,
		WA_CloseGadget,		TRUE,
		WA_Activate,		TRUE,
		WA_RMBTrap,		TRUE,
		WA_SizeBBottom,		TRUE,
		WA_AutoAdjust,		TRUE,
		WA_RMBTrap,		TRUE,
		WA_IDCMP,		IDCMP_CLOSEWINDOW,

		TAG_DONE
	};

		/* Library bases. */

	struct DosLibrary	*DOSBase;
	struct IntuitionBase	*IntuitionBase;

		/* Workbench data. */

	struct Process		*ThisProcess;
	struct WBStartup	*WBenchMsg;

		/* Our window. */

	struct Window		*Window;

		/* Serial IO data. */

	struct IOExtSer		*SerReadRequest;
	struct IOExtSer		*SerWriteRequest;
	struct MsgPort		*SerReadPort;
	struct MsgPort		*SerWritePort;
	APTR			 SerReadBuffer;

		/* Console IO data. */

	struct IOStdReq		*ConReadRequest;
	struct IOStdReq		*ConWriteRequest;
	struct MsgPort		*ConReadPort;
	struct MsgPort		*ConWritePort;
	APTR			 ConReadBuffer;

		/* Global result code. */

	LONG			 Result = RETURN_FAIL;

		/* Set up the process pointer. */

	ThisProcess = (struct Process *)FindTask(NULL);

		/* Wait for Workbench startup packet. */

	if(!ThisProcess -> pr_CLI)
	{
		WaitPort(&ThisProcess -> pr_MsgPort);

		WBenchMsg = (struct WBStartup *)GetMsg(&ThisProcess -> pr_MsgPort);
	}
	else
		WBenchMsg = NULL;

		/* Are we running under control of Kickstart 2.0? */

	if(SysBase -> LibNode . lib_Version >= 37)
	{
			/* Open dos.library. */

		if((DOSBase = (struct DosLibrary *)OpenLibrary("dos.library",37)))
		{
				/* Open intuition.library. */

			if((IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",37)))
			{
				UBYTE	DeviceName[40];
				LONG	UnitNumber;

					/* Fill in the default serial device
					 * name.
					 */

				strcpy(DeviceName,SERIALNAME);

					/* And set the default unit number
					 * to zero.
					 */

				UnitNumber = 0;

					/* Take a look at the tool icon
					 * if run from Workbench.
					 */

				if(WBenchMsg)
				{
					struct Library *IconBase;

						/* Open icon.library. */

					if((IconBase = OpenLibrary("icon.library",0)))
					{
						struct DiskObject *DiskObject;

							/* Read the tool icon. */

						if((DiskObject = GetDiskObject(WBenchMsg -> sm_ArgList -> wa_Name)))
						{
							UBYTE *Entry;

								/* Check for custom device name. */

							if((Entry = FindToolType(DiskObject -> do_ToolTypes,"DEVICE")))
								strcpy(DeviceName,Entry);

								/* Check for custom device number. */

							if((Entry = FindToolType(DiskObject -> do_ToolTypes,"UNIT")))
								StrToLong(Entry,&UnitNumber);

								/* Release the icon. */

							FreeDiskObject(DiskObject);
						}

							/* Close the library. */

						CloseLibrary(IconBase);
					}
				}
				else
				{
					UBYTE **ArgArray;

						/* Allocate argument array. */

					if((ArgArray = (UBYTE **)AllocVec(sizeof(UBYTE *) * 2,MEMF_PUBLIC|MEMF_CLEAR)))
					{
						struct RDArgs *ArgsPtr;

							/* Read the startup args if any. */

						if((ArgsPtr = (struct RDArgs *)ReadArgs("D=Device/K,U=Unit/N/K",(IPTR *)ArgArray,NULL)))
						{
								/* Set custom device name. */

							if(ArgArray[ARG_DEVICE])
								strcpy(DeviceName,ArgArray[ARG_DEVICE]);

								/* Set custom unit number. */

							if(ArgArray[ARG_UNIT])
								UnitNumber = *(LONG *)ArgArray[ARG_UNIT];

								/* Free the args. */

							FreeArgs(ArgsPtr);
						}

							/* Free the argument array. */

						FreeVec(ArgArray);
					}
				}

					/* Allocate the serial read buffer. */

				if((SerReadBuffer = AllocVec(SER_READ_SIZE,MEMF_PUBLIC)))
				{
						/* Create the serial read port. */

					if((SerReadPort = CreateMsgPort()))
					{
							/* Create the serial write port. */

						if((SerWritePort = CreateMsgPort()))
						{
								/* Create the serial read request. */

							if((SerReadRequest = (struct IOExtSer *)CreateIORequest(SerReadPort,sizeof(struct IOExtSer))))
							{
									/* Create the serial write request. */

								if((SerWriteRequest = (struct IOExtSer *)AllocVec(sizeof(struct IOExtSer),MEMF_PUBLIC)))
								{
										/* Open the device in shared mode. */

									SerReadRequest -> io_SerFlags |= SERF_SHARED;

									if(!OpenDevice(DeviceName,UnitNumber,(struct IORequest *)&SerReadRequest -> IOSer,0))
									{
											/* Clone the IORequest and fill in the
											 * approriate reply port for the write
											 * request.
											 */

										CopyMem(SerReadRequest,SerWriteRequest,sizeof(struct IOExtSer));

										SerWriteRequest -> IOSer . io_Message . mn_ReplyPort = SerWritePort;

											/* Allocate the console read buffer. */

										if((ConReadBuffer = AllocVec(CON_READ_SIZE,MEMF_PUBLIC)))
										{
											struct Screen *DefaultScreen;

												/* Lock the default public screen. */

											if((DefaultScreen = LockPubScreen(NULL)))
											{
												ULONG StackTags[20 * 2 + 1];

													/* Copy window sestup tags to local
													 * stack array and initialize it
													 * according to the size and
													 * characteristics of the default
													 * screen.
													 */

												CopyMem(DefaultTags,StackTags,sizeof(DefaultTags));

												StackTags[ 1] = DefaultScreen -> BarHeight + 1;
												StackTags[ 3] = DefaultScreen -> Width;
												StackTags[ 5] = (DefaultScreen -> Font -> ta_YSize) * 17 + DefaultScreen -> WBorTop + DefaultScreen -> WBorBottom + 1;
												StackTags[ 7] = DefaultScreen -> Font -> ta_YSize + DefaultScreen -> WBorTop + DefaultScreen -> WBorBottom - 1;
												StackTags[ 9] = (IPTR)("termLite © Copyright 1991 by MXM");
												StackTags[11] = DefaultScreen -> Width >> 1;

													/* Open the window. */

												if((Window = OpenWindowTagList(NULL,(struct TagItem *)&StackTags[0])))
												{
														/* Create the console read port. */

													if((ConReadPort = CreateMsgPort()))
													{
															/* Create the console write port. */

														if((ConWritePort = CreateMsgPort()))
														{
																/* Create the console read
																 * request.
																 */

															if((ConReadRequest = (struct IOStdReq *)CreateIORequest(ConReadPort,sizeof(struct IOStdReq))))
															{
																	/* Create the console write request. */

																if((ConWriteRequest = (struct IOStdReq *)AllocVec(sizeof(struct IOStdReq),MEMF_PUBLIC)))
																{
																		/* Set up the console startup information and
																		 * open the approriate device.
																		 */

																	ConReadRequest -> io_Data	= Window;
																	ConReadRequest -> io_Length	= sizeof(struct Window *);

																		/* We want the character mapped simple refresh window
																		 * including cut & paste.
																		 */

																	if(!OpenDevice("console.device",CONU_SNIPMAP,(struct IORequest *)ConReadRequest,0))
																	{
																		BYTE	Terminated = FALSE;
																		ULONG	SignalSet;

																			/* Clone the IORequest and
																			 * fill in the console write
																			 * port.
																			 */

																		CopyMem(ConReadRequest,ConWriteRequest,sizeof(struct IOStdReq));

																		ConWriteRequest -> io_Message . mn_ReplyPort = ConWritePort;

																			/* Unlock the default public
																			 * screen, there probably
																			 * is no need to keep
																			 * it open (our window
																			 * will keep it from
																			 * closing).
																			 */

																		UnlockPubScreen(NULL,DefaultScreen);

																		DefaultScreen = NULL;

																			/* Request characters from the console window. */

																		ConReadRequest -> io_Command		= CMD_READ;
																		ConReadRequest -> io_Length		= CON_READ_SIZE - 1;
																		ConReadRequest -> io_Data		= ConReadBuffer;

																		SendIO((struct IORequest *)ConReadRequest);

																			/* Request a single byte from the
																			 * serial line.
																			 */

																		SerReadRequest -> IOSer . io_Command	= CMD_READ;
																		SerReadRequest -> IOSer . io_Length	= 1;
																		SerReadRequest -> IOSer . io_Data	= SerReadBuffer;

																		SendIO((struct IORequest *)&SerReadRequest -> IOSer);

																		Result = RETURN_OK;

																			/* Go into loop until the
																			 * user hits the close
																			 * gadget.
																			 */

																		while(!Terminated)
																		{
																				/* Wait for signals. */

																			SignalSet = Wait(SIG_SERIAL | SIG_CONSOLE | SIG_WINDOW);

																				/* Did we get a message from the window? */

																			if(SignalSet & SIG_WINDOW)
																			{
																				struct IntuiMessage *Massage;

																					/* Pick up the messages and reply them. The
																					 * only message type we expect is the
																					 * IDCMP_CLOSEWINDOW type.
																					 */

																				while((Massage = (struct IntuiMessage *)GetMsg(Window -> UserPort)))
																				{
																					ReplyMsg(&Massage -> ExecMessage);

																					Terminated = TRUE;
																				}
																			}

																				/* Did we get a message from the
																				 * console?
																				 */

																			if(SignalSet & SIG_CONSOLE)
																			{
																					/* Remove the IORequest from
																					 * the message list.
																					 */

																				WaitIO((struct IORequest *)ConReadRequest);

																					/* Send the resulting characters to the
																					 * serial device.
																					 */

																				SerWriteRequest -> IOSer . io_Command	= CMD_WRITE;
																				SerWriteRequest -> IOSer . io_Data	= ConReadBuffer;
																				SerWriteRequest -> IOSer . io_Length	= ConReadRequest -> io_Actual;

																				DoIO((struct IORequest *)&SerWriteRequest -> IOSer);

																					/* Post another request to the
																					 * console.
																					 */

																				ConReadRequest -> io_Command		= CMD_READ;
																				ConReadRequest -> io_Length		= CON_READ_SIZE - 1;
																				ConReadRequest -> io_Data		= ConReadBuffer;

																				SendIO((struct IORequest *)ConReadRequest);
																			}

																				/* Did we get a signal from
																				 * the serial line?
																				 */

																			if(SignalSet & SIG_SERIAL)
																			{
																				LONG Size;

																					/* Remove IORequest from the
																					 * message list.
																					 */

																				WaitIO((struct IORequest *)&SerReadRequest -> IOSer);

																					/* Echo the single character to
																					 * the console.
																					 */

																				ConWriteRequest -> io_Command	= CMD_WRITE;
																				ConWriteRequest -> io_Data	= SerReadBuffer;
																				ConWriteRequest -> io_Length	= 1;

																				DoIO((struct IORequest *)ConWriteRequest);

																					/* Inquire how many characters
																					 * are still pending at the
																					 * serial port.
																					 */

																				SerReadRequest -> IOSer . io_Command = SDCMD_QUERY;

																				DoIO((struct IORequest *)&SerReadRequest -> IOSer);

																					/* If there are any, transfer them to
																					 * the serial line.
																					 */

																				if((Size = SerReadRequest -> IOSer . io_Actual))
																				{
																					LONG Frag;

																						/* We need to transfer the block in small
																						 * fragments since the serial read buffer
																						 * may be smaller than the amount of data
																						 * to be read.
																						 */

																					while(Size)
																					{
																							/* Determine number of bytes to read. */

																						Frag = Size > SER_READ_SIZE ? SER_READ_SIZE : Size;

																							/* Read the bytes. */

																						SerReadRequest -> IOSer . io_Command	= CMD_READ;
																						SerReadRequest -> IOSer . io_Data	= SerReadBuffer;
																						SerReadRequest -> IOSer . io_Length	= Frag;

																						DoIO((struct IORequest *)&SerReadRequest -> IOSer);

																							/* Transfer them to the window. */

																						ConWriteRequest -> io_Command		= CMD_WRITE;
																						ConWriteRequest -> io_Data		= SerReadBuffer;
																						ConWriteRequest -> io_Length		= Frag;

																						DoIO((struct IORequest *)ConWriteRequest);

																						Size -= Frag;
																					}
																				}

																					/* Post a request for another byte. */

																				SerReadRequest -> IOSer . io_Command	= CMD_READ;
																				SerReadRequest -> IOSer . io_Data	= SerReadBuffer;
																				SerReadRequest -> IOSer . io_Length	= 1;

																				SendIO((struct IORequest *)&SerReadRequest -> IOSer);
																			}
																		}

																			/* The program has been flagged to
																			 * to close down. Abort the pending
																			 * IORequestes and close the
																			 * console device.
																			 */

																		AbortIO((struct IORequest *)ConReadRequest);
																		WaitIO((struct IORequest *)ConReadRequest);

																		AbortIO((struct IORequest *)&SerReadRequest -> IOSer);
																		WaitIO((struct IORequest *)&SerReadRequest -> IOSer);

																		CloseDevice((struct IORequest *)ConReadRequest);
																	}

																		/* Free the console write request. */

																	FreeVec(ConWriteRequest);
																}

																	/* Free the console read request. */

																DeleteIORequest(ConReadRequest);
															}

																/* Delete the console write port. */

															DeleteMsgPort(ConWritePort);
														}

															/* Delete the console read port. */

														DeleteMsgPort(ConReadPort);
													}

														/* Close the window. */

													CloseWindow(Window);
												}

													/* Unlock the default public screen
													 * if necessary.
													 */

												if(DefaultScreen)
													UnlockPubScreen(NULL,DefaultScreen);
											}

												/* Free the console read buffer. */

											FreeVec(ConReadBuffer);
										}

											/* Close the serial device. */

										CloseDevice((struct IORequest *)&SerReadRequest -> IOSer);
									}

										/* Free the serial write request. */

									FreeVec(SerWriteRequest);
								}

									/* Free the serial read request. */

								DeleteIORequest(SerReadRequest);
							}

								/* Delete the serial write port. */

							DeleteMsgPort(SerWritePort);
						}

							/* Delete the serial read port. */

						DeleteMsgPort(SerReadPort);
					}

						/* Delete the serial read buffer. */

					FreeVec(SerReadBuffer);
				}

					/* Close intuition.library. */

				CloseLibrary(&IntuitionBase -> LibNode);
			}

				/* Close dos.library. */

			CloseLibrary(&DOSBase -> dl_lib);
		}
	}

		/* Reply the startup packet if necessary. */

	if(WBenchMsg)
	{
		Forbid();

		ReplyMsg(&WBenchMsg -> sm_Message);
	}

		/* Return the result. */

	return(Result);
}

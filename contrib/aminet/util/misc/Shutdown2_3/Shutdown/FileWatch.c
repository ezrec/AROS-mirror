/*
**	Shutdown 2.0 package, FileWatch.c module
**
**	Copyright © 1992 by Olaf `Olsen' Barthel
**		All Rights Reserved
*/

#include "ShutdownGlobal.h"

	/* The number of patches to install. */

#define NUM_PATCHES	(sizeof(PatchTable) / sizeof(struct PatchInfo))

	/* The patch table entries look like this. */

struct PatchInfo
{
	APTR		 NewRoutine;
	LONG		 Offset;
	ULONG		*Destination;
};

	/* References to write-access routines. */

extern ULONG __far	 LVOOpen,
			 LVOClose,
			 LVOWrite,
			 LVOFPutC,
			 LVOFPuts,
			 LVOFWrite,
			 LVOVFPrintf,
			 LVOVFWritef,
			 LVOPutStr,
			 LVOWriteChars,
			 LVOVPrintf;

	/* The original routine vectors. */

BPTR			(* __asm OldOpen)(register __d1 STRPTR Name,register __d2 LONG Mode,register __a6 struct DosLibrary *DOSBase);
LONG			(* __asm OldClose)(register __d1 BPTR Handle,register __a6 struct DosLibrary *DOSBase);
LONG			(* __asm OldWrite)(register __d1 BPTR Handle,register __d2 APTR Buffer,register __d3 LONG Length,register __a6 struct DosLibrary *DOSBase);
LONG			(* __asm OldFPutC)(register __d1 BPTR FileHandle,register __d2 UBYTE Char,register __a6 struct DosLibrary *DOSBase);
LONG			(* __asm OldFPuts)(register __d1 BPTR FileHandle,register __d2 STRPTR Buffer,register __a6 struct DosLibrary *DOSBase);
LONG			(* __asm OldFWrite)(register __d1 BPTR FileHandle,register __d2 STRPTR Buffer,register __d3 ULONG BlockLen,register __d4 ULONG Blocks,register __a6 struct DosLibrary *DOSBase);
LONG			(* __asm OldVFPrintf)(register __d1 BPTR FileHandle,register __d2 STRPTR Format,register __d3 LONG *Args,register __a6 struct DosLibrary *DOSBase);
LONG			(* __asm OldVFWritef)(register __d1 BPTR FileHandle,register __d2 STRPTR Format,register __d3 LONG *Args,register __a6 struct DosLibrary *DOSBase);
LONG			(* __asm OldPutStr)(register __d1 STRPTR Buffer,register __a6 struct DosLibrary *DOSBase);
LONG			(* __asm OldWriteChars)(register __d1 STRPTR Buffer,register __d2 ULONG Len,register __a6 struct DosLibrary *DOSBase);
LONG			(* __asm OldVPrintf)(register __d1 STRPTR Format,register __d2 LONG *Args,register __a6 struct DosLibrary *DOSBase);

	/* The routines to interface between us and the world out there. */

extern BPTR __asm	NewOpen(register __d1 STRPTR Name,register __d2 LONG Mode);
extern LONG __asm	NewClose(register __d1 BPTR Handle);
extern LONG __asm	NewWrite(register __d1 BPTR Handle,register __d2 APTR Buffer,register __d3 LONG Length);
extern LONG __asm	NewFPutC(register __d1 BPTR FileHandle,register __d2 UBYTE Char);
extern LONG __asm	NewFPuts(register __d1 BPTR FileHandle,register __d2 STRPTR Buffer);
extern LONG __asm	NewFWrite(register __d1 BPTR FileHandle,register __d2 STRPTR Buffer,register __d3 ULONG BlockLen,register __d4 ULONG Blocks);
extern LONG __asm	NewVFPrintf(register __d1 BPTR FileHandle,register __d2 STRPTR Format,register __d3 LONG *Args);
extern LONG __asm	NewVFWritef(register __d1 BPTR FileHandle,register __d2 STRPTR Format,register __d3 LONG *Args);
extern LONG __asm	NewPutStr(register __d1 STRPTR Buffer);
extern LONG __asm	NewWriteChars(register __d1 STRPTR Buffer,register __d2 ULONG Len);
extern LONG __asm	NewVPrintf(register __d1 STRPTR Format,register __d2 LONG *Args);

	/* Our local routines. */

BPTR __asm		MyOpen(register __d1 STRPTR Name,register __d2 LONG Mode);
LONG __asm		MyClose(register __d1 BPTR Handle);
LONG __asm		MyWrite(register __d1 BPTR Handle,register __d2 APTR Buffer,register __d3 LONG Length);
LONG __asm		MyFPutC(register __d1 BPTR FileHandle,register __d2 UBYTE Char);
LONG __asm		MyFPuts(register __d1 BPTR FileHandle,register __d2 STRPTR Buffer);
LONG __asm		MyFWrite(register __d1 BPTR FileHandle,register __d2 STRPTR Buffer,register __d3 ULONG BlockLen,register __d4 ULONG Blocks);
LONG __asm		MyVFPrintf(register __d1 BPTR FileHandle,register __d2 STRPTR Format,register __d3 LONG *Args);
LONG __asm		MyVFWritef(register __d1 BPTR FileHandle,register __d2 STRPTR Format,register __d3 LONG *Args);
LONG __asm		MyPutStr(register __d1 STRPTR Buffer);
LONG __asm		MyWriteChars(register __d1 STRPTR Buffer,register __d2 ULONG Len);
LONG __asm		MyVPrintf(register __d1 STRPTR Format,register __d2 LONG *Args);

	/* Local routines for this module. */

STATIC VOID __regargs	RegisterAccess(BPTR Handle);
STATIC VOID __saveds	ResetCode(VOID);

	/* A table of dos.library routines to be patched. */

STATIC struct PatchInfo PatchTable[] =
{
	NewOpen,	(LONG)&LVOOpen,		(ULONG *)&OldOpen,
	NewClose,	(LONG)&LVOClose,	(ULONG *)&OldClose,
	NewWrite,	(LONG)&LVOWrite,	(ULONG *)&OldWrite,
	NewFPutC,	(LONG)&LVOFPutC,	(ULONG *)&OldFPutC,
	NewFPuts,	(LONG)&LVOFPuts,	(ULONG *)&OldFPuts,
	NewFWrite,	(LONG)&LVOFWrite,	(ULONG *)&OldFWrite,
	NewVFPrintf,	(LONG)&LVOVFPrintf,	(ULONG *)&OldVFPrintf,
	NewVFWritef,	(LONG)&LVOVFWritef,	(ULONG *)&OldVFWritef,
	NewPutStr,	(LONG)&LVOPutStr,	(ULONG *)&OldPutStr,
	NewWriteChars,	(LONG)&LVOWriteChars,	(ULONG *)&OldWriteChars,
	NewVPrintf,	(LONG)&LVOVPrintf,	(ULONG *)&OldVPrintf
};

	/* The devices we want to ignore for now. Some operate on
	 * ram only and are unlikely to get corrupted, some are
	 * handled indirectly by the DevWatch.c module.
	 */

STATIC STRPTR Ignored[] =
{
	"fmsdisk.device",
	"hardfile.device",
	"ramdisk.device",
	"ramdrive.device",
	"asdg.vdisk.device",
	"static.device",
	NULL
};

	/* Keyboard reset handler data. */

STATIC struct MsgPort		*KeyboardPort;
STATIC struct IOStdReq		*KeyboardRequest;
STATIC struct Interrupt		*KeyboardReset;

	/* Number of tasks/processes to use our modified
	 * routines and the approriate access semaphore.
	 */

STATIC struct SignalSemaphore	 UseCountSemaphore;
STATIC ULONG			 UseCount = 0;

	/* Main():
	 *
	 *	Shutdown handler routine, installs the patches and
	 *	watches the keyboard reset.
	 */

VOID __saveds
Main()
{
		/* Remember the current process identifier. */

	HandlerProcess = (struct Process *)SysBase -> ThisTask;

		/* Create keyboard reset reply port. */

	if(KeyboardPort = CreateMsgPort())
	{
			/* Create keyboard reset request. */

		if(KeyboardRequest = CreateIORequest(KeyboardPort,sizeof(struct IOStdReq)))
		{
				/* Open keyboard.device. */

			if(!OpenDevice("keyboard.device",0,KeyboardRequest,0))
			{
					/* Create the keyboard reset notification interrupt. */

				if(KeyboardReset = (struct Interrupt *)AllocVec(sizeof(struct Interrupt),MEMF_PUBLIC | MEMF_CLEAR))
				{
						/* Install the device monitoring wedges. */

					if(WedgeSetup())
					{
							/* Set up the keyboard reset handler. */

						KeyboardReset -> is_Node . ln_Name	= HandlerProcess -> pr_Task . tc_Node . ln_Name;
						KeyboardReset -> is_Code		= (APTR)ResetCode;

							/* Fill in the installation request. */

						KeyboardRequest -> io_Command		= KBD_ADDRESETHANDLER;
						KeyboardRequest -> io_Data		= (APTR)KeyboardReset;

							/* Install the keyboard reset handler. */

						if(!DoIO(KeyboardRequest))
						{
							struct Wedge *WedgeTable;

								/* Allocate the wedge table. */

							if(WedgeTable = (struct Wedge *)AllocMem(sizeof(struct Wedge) * NUM_PATCHES,MEMF_ANY | MEMF_CLEAR))
							{
									/* Who created use? */

								if(GlobalBase -> Father)
								{
									struct FileInfo	*FileInfo,
											*NextInfo;
									WORD		 i;
									BYTE		 Terminated = FALSE;
									ULONG		 SignalSet;

										/* Set up the access count semaphore. */

									InitSemaphore(&UseCountSemaphore);

										/* Open the catalog file. */

									LocaleOpen("shutdown.catalog","english");

										/* Make ourselves known as the child. */

									GlobalBase -> Child = HandlerProcess;

										/* Wake up the father process. */

									Signal(GlobalBase -> Father,SIG_HANDSHAKE);

										/* Clear the father id. */

									GlobalBase -> Father = NULL;

										/* Disable multitasking and interrupt processing. */

									Forbid();
									Disable();

										/* Install all the patches. */

									for(i = 0 ; i < NUM_PATCHES ; i++)
									{
										WedgeTable[i] . Command	= JMP_ABS;
										WedgeTable[i] . Address	= PatchTable[i] . NewRoutine;

										*PatchTable[i] . Destination = (ULONG)SetFunction(DOSBase,PatchTable[i] . Offset,(APTR)&WedgeTable[i]);
									}

										/* Flush the caches. */

									CacheClearU();

										/* Reenable interrupt processing. */

									Enable();

										/* We're up and running now. */

									GlobalBase -> Running = TRUE;

									do
									{
											/* Wait for termination signal. */

										SignalSet = Wait(SIG_KILL | SIG_SHUTDOWN);

											/* Are we to remove ourselves? */

										if(SignalSet & SIG_KILL)
										{
											if(GlobalBase -> Father)
												Terminated = TRUE;
										}

											/* Are we to perform system shutdown? */

										if(SignalSet & SIG_SHUTDOWN)
										{
												/* Are we to close down? */

											if(!GlobalBase -> Closing)
											{
												struct FileInfo		*Node,
															*Next;
												struct ShutdownInfo	*Info,
															*NextInfo;

												BYTE			 DidFlush = FALSE;
												ULONG			 Mode;

													/* Get the file info access lock. */

												ObtainSemaphore(&GlobalBase -> AccessLock);

													/* Block all relevant dos.library functions. */

												ObtainSemaphore(&GlobalBase -> BlockLock);

													/* The system is going down. */

												GlobalBase -> Shutdown = TRUE;

													/* Process the file info list. */

												Node = (struct FileInfo *)GlobalBase -> AccessList . mlh_Head;

												while(Next = (struct FileInfo *)Node -> Node . mln_Succ)
												{
														/* Did the corresponding file get
														 * any write accesses? If so,
														 * flush the data.
														 */

													if(Node -> Accesses && Node -> Handle)
													{
															/* Flush the data. */

														if(Flush(Node -> Handle))
														{
																/* Is this the first file
																 * we have to flush? If so,
																 * open the notification
																 * window.
																 */

															if(!DidFlush)
															{
																OpenAll(MODE_RESET);

																DidFlush = TRUE;
															}
														}
													}

													Node = Next;
												}

													/* Process the callbacks. */

												Info = (struct ShutdownInfo *)GlobalBase -> ShutdownList . mlh_Head;

												while(NextInfo = (struct ShutdownInfo *)Info -> sdi_Node . mln_Succ)
												{
														/* Call the hook. */

													Mode = SD_EXIT;

													CallHook(Info -> sdi_Hook,&Mode,SDF_RESET);

													Info = NextInfo;
												}

													/* Wait for the system to settle. */

												if(DidFlush)
													TimeDelay(UNIT_VBLANK,2,0);

													/* Carry on with the keyboard reset... */

												KeyboardRequest -> io_Command	= KBD_RESETHANDLERDONE;
												KeyboardRequest -> io_Data	= (APTR)KeyboardReset;

												DoIO(KeyboardRequest);
											}

												/* Drop down dead. */

											Wait(NULL);
										}
									}
									while(!Terminated);

										/* We're no longer running. */

									GlobalBase -> Running = FALSE;

										/* Wait for the system to settle. */

									TimeDelay(UNIT_VBLANK,2,0);

										/* Clear the notification signal. */

									SetSignal(0,SIG_NOTICE);

										/* Gain access to the user count semaphore. */

									ObtainSemaphore(&UseCountSemaphore);

										/* Wait until no program uses our patched
										 * routines any longer.
										 */

									while(UseCount)
									{
										ReleaseSemaphore(&UseCountSemaphore);

										Wait(SIG_NOTICE);

										ObtainSemaphore(&UseCountSemaphore);
									}

										/* Release the lock. */

									ReleaseSemaphore(&UseCountSemaphore);

										/* Turn off interrupt processing. */

									Disable();

										/* Redirect the wedges to point to the
										 * original routines.
										 */

									for(i = 0 ; i < NUM_PATCHES ; i++)
										WedgeTable[i] . Address	= (APTR)*PatchTable[i] . Destination;

										/* Flush the caches. */

									CacheClearU();

										/* Reenable interrupt processing. */

									Enable();

										/* Process the file info list. */

									FileInfo = (struct FileInfo *)GlobalBase -> AccessList . mlh_Head;

									while(FileInfo -> Node . mln_Succ)
									{
										NextInfo = (struct FileInfo *)FileInfo -> Node . mln_Succ;

										Remove((struct Node *)FileInfo);

										FreeVec(FileInfo);

										FileInfo = NextInfo;
									}

										/* Reset the device wedges. */

									DeleteWedges();

										/* Close the locale catalog tables. */

									LocaleClose();
								}
								else
									FreeMem(WedgeTable,sizeof(struct Wedge) * NUM_PATCHES);
							}
							else
								GlobalBase -> ErrorCode = ERROR_NOTABLE,

								/* Remove the keyboard reset handler. */

							KeyboardRequest -> io_Command	= KBD_REMRESETHANDLER;
							KeyboardRequest -> io_Data	= (APTR)KeyboardReset;

							DoIO(KeyboardRequest);
						}
						else
							GlobalBase -> ErrorCode = ERROR_NORESET;
					}
					else
						GlobalBase -> ErrorCode = ERROR_WEDGESETUP;

						/* Free the keyboard reset handler. */

					FreeVec(KeyboardReset);
				}
				else
					GlobalBase -> ErrorCode = ERROR_NOKEYRESET;

					/* Close the keyboard.device. */

				CloseDevice(KeyboardRequest);
			}
			else
				GlobalBase -> ErrorCode = ERROR_NOKEYDEVICE;

				/* Delete the keyboard request. */

			DeleteIORequest(KeyboardRequest);
		}
		else
			GlobalBase -> ErrorCode = ERROR_NOKEYREQUEST;

			/* Delete the keyboard reply port. */

		DeleteMsgPort(KeyboardPort);
	}
	else
		GlobalBase -> ErrorCode = ERROR_NOKEYPORT;

	Forbid();

		/* We are no longer running. */

	GlobalBase -> Running	= FALSE;

	GlobalBase -> Child	= NULL;

		/* Ring back and fall through. */

	Signal(GlobalBase -> Father,SIG_HANDSHAKE);
}

	/* ResetCode():
	 *
	 *	The keyboard reset handler code actually calls
	 * this routine.
	 */

STATIC VOID __saveds
ResetCode()
{
	Signal(HandlerProcess,SIG_SHUTDOWN);
}

	/* RegisterAccess(BPTR Handle):
	 *
	 *	Register a write access to a file handle.
	 */

STATIC VOID __regargs
RegisterAccess(BPTR Handle)
{
	struct FileInfo	*FileInfo;
	BYTE		 GotIt = FALSE;

		/* Gain access to the file info list. */

	ObtainSemaphore(&GlobalBase -> AccessLock);

		/* Scan the list for a fitting file handle. */

	FileInfo = (struct FileInfo *)GlobalBase -> AccessList . mlh_Head;

	while(FileInfo -> Node . mln_Succ)
	{
			/* Is this the one we want? */

		if(FileInfo -> Handle == Handle)
		{
				/* Remember the last accessor. */

			FileInfo -> LastAccess = (struct Process *)SysBase -> ThisTask;

				/* Add up the number of write accesses. */

			FileInfo -> Accesses++;

				/* We got a file handle. */

			GotIt = TRUE;

			break;
		}
		else
			FileInfo = (struct FileInfo *)FileInfo -> Node . mln_Succ;
	}

		/* Release the file info access lock. */

	ReleaseSemaphore(&GlobalBase -> AccessLock);

		/* Block if the system is going down. */

	if(GotIt && GlobalBase -> Shutdown)
	{
		ObtainSemaphoreShared(&GlobalBase -> BlockLock);

		ReleaseSemaphore(&GlobalBase -> BlockLock);
	}
}

	/* MyOpen(register __d1 STRPTR Name,register __d2 LONG Mode):
	 *
	 *	Modified Open() routine.
	 */

BPTR __asm
MyOpen(register __d1 STRPTR Name,register __d2 LONG Mode)
{
		/* Is the shutdown process running? */

	if(GlobalBase -> Running)
	{
			/* Is it a real disk device the name refers to? */

		if(strcmp(Name,"*") && Stricmp(Name,"CONSOLE:"))
		{
			UBYTE	Buffer[50];
			BYTE	GotColon	= FALSE,
				IsHandler	= FALSE;
			WORD	Count;
			BPTR	File;

				/* Is the system going down? */

			if(GlobalBase -> Shutdown)
			{
				ObtainSemaphoreShared(&GlobalBase -> BlockLock);

				ReleaseSemaphore(&GlobalBase -> BlockLock);
			}

				/* Increment the routine user count. */

			ObtainSemaphore(&UseCountSemaphore);

			UseCount++;

			ReleaseSemaphore(&UseCountSemaphore);

				/* Look for the device name part of the file name. */

			Count = 0;

			while(Name[Count] && Count < 49)
			{
				if(Name[Count] != ':')
				{
					Buffer[Count] = Name[Count];

					Count++;
				}
				else
				{
					Buffer[Count] = 0;

					GotColon = TRUE;

					break;
				}
			}

				/* Did we get a device name? */

			if(GotColon && Buffer[0])
			{
				struct DosList *DosEntry;

					/* Scan the doslist for device entries. */

				if(DosEntry = LockDosList(LDF_DEVICES | LDF_READ))
				{
						/* Find a matching entry. */

					if(DosEntry = FindDosEntry(DosEntry,Buffer,LDF_DEVICES))
					{
							/* Is it really a device? */

						if(DosEntry -> dol_Type == DLT_DEVICE)
						{
								/* If it has a valid filesystem startup
								 * entry, it's probably a `real' device.
								 * If not, it could be filesystem (handler)
								 * only.
								 */

							if(!TypeOfMem(BADDR(DosEntry -> dol_misc . dol_handler . dol_Startup)))
								IsHandler = TRUE;
							else
							{
								struct FileSysStartupMsg *FSSM = (struct FileSysStartupMsg *)BADDR(DosEntry -> dol_misc . dol_handler . dol_Startup);

									/* Are the device and environment vectors
									 * correct?
									 */

								if(TypeOfMem(BADDR(FSSM -> fssm_Device)) && TypeOfMem(BADDR(FSSM -> fssm_Environ)))
								{
									struct IOStdReq __aligned TestRequest;

									memset(&TestRequest,0,sizeof(TestRequest));

										/* Try to open the corresponding
										 * device driver.
										 */

									if(OpenDevice(&((UBYTE *)BADDR(FSSM -> fssm_Device))[1],FSSM -> fssm_Unit,&TestRequest,FSSM -> fssm_Flags))
										IsHandler = TRUE;
									else
										CloseDevice(&TestRequest);
								}
							}
						}
					}

						/* Release the lock on the doslist. */

					UnLockDosList(LDF_DEVICES | LDF_READ);
				}
			}

				/* If the name refers to a device we will
				 * probably have to install a wedge in the
				 * device BeginIO() vector.
				 */

			if(!IsHandler)
			{
					/* Open the file. */

				if(File = OldOpen(Name,Mode,DOSBase))
				{
					struct FileHandle *FileHandle = (struct FileHandle *)BADDR(File);

						/* Is there any filesystem process to
						 * handle the file IO?
						 */

					if(FileHandle -> fh_Type)
					{
						struct DosList		*DosEntry;
						struct DeviceNode	*DeviceNode = NULL;
						BYTE			 Ignore = FALSE;

							/* Scan the doslist for device entries... */

						if(DosEntry = LockDosList(LDF_DEVICES | LDF_READ))
						{
								/* Scan until the list is exhausted. */

							while(DosEntry = NextDosEntry(DosEntry,LDF_DEVICES | LDF_READ))
							{
									/* If the filesystem process addresses match then
									 * we have finally found the device (not the volume!)
									 * the file is located on.
									 */

								if(DosEntry -> dol_Task == FileHandle -> fh_Type)
								{
										/* Does the device have a correct filesystem
										 * startup entry?
										 */

									if(TypeOfMem(BADDR(DosEntry -> dol_misc . dol_handler . dol_Startup)))
									{
										struct FileSysStartupMsg *FSSM = (struct FileSysStartupMsg *)BADDR(DosEntry -> dol_misc . dol_handler . dol_Startup);

											/* Are the device and the environment vector correct? */

										if(TypeOfMem(BADDR(FSSM -> fssm_Device)) && TypeOfMem(BADDR(FSSM -> fssm_Environ)))
										{
											struct IOStdReq __aligned TestRequest;

											memset(&TestRequest,0,sizeof(TestRequest));

												/* Try to open the corresponding device. */

											if(!OpenDevice(&((UBYTE *)BADDR(FSSM -> fssm_Device))[1],FSSM -> fssm_Unit,&TestRequest,FSSM -> fssm_Flags))
											{
												STRPTR	Name;
												WORD	i;

													/* Install the BeginIO() vector wedge. */

												NewWedge(&((UBYTE *)BADDR(FSSM -> fssm_Device))[1]);

													/* Close the device again. */

												CloseDevice(&TestRequest);

													/* Check the device name to see if we
													 * are to ignore the device driver
													 * activity.
													 */

												DeviceNode = (struct DeviceNode *)DosEntry;

												Name = FilePart(&((UBYTE *)BADDR(FSSM -> fssm_Device))[1]);

												for(i = 0 ; Ignored[i] ; i++)
												{
													if(!Stricmp(Name,Ignored[i]))
													{
														Ignore = TRUE;

														break;
													}
												}

												break;
											}
										}
									}
								}
							}

								/* Release the lock on the list. */

							UnLockDosList(LDF_DEVICES | LDF_READ);
						}

							/* Did we find a matching device node? */

						if(DeviceNode)
						{
							struct FileInfo *FileInfo;

								/* Create a new fileinfo handle. */

							if(FileInfo = (struct FileInfo *)AllocVec(sizeof(struct FileInfo) + strlen(Name),MEMF_ANY|MEMF_CLEAR))
							{
									/* Fill in the common data. */

								FileInfo -> Handle	= File;
								FileInfo -> LastAccess	= (struct Process *)SysBase -> ThisTask;
								FileInfo -> Ignore	= Ignore;
								FileInfo -> DeviceNode	= DeviceNode;

								strcpy(FileInfo -> Name,Name);

									/* Add it to the list. */

								ObtainSemaphore(&GlobalBase -> AccessLock);

								AddTail((struct List *)&GlobalBase -> AccessList,(struct Node *)FileInfo);

									/* Increase the number of files open. */

								GlobalBase -> OpenCount++;

								ReleaseSemaphore(&GlobalBase -> AccessLock);
							}
						}
					}
				}
			}
			else
				File = OldOpen(Name,Mode,DOSBase);

				/* Decrement the patch user count. */

			ObtainSemaphore(&UseCountSemaphore);

			UseCount--;

			ReleaseSemaphore(&UseCountSemaphore);

				/* Notify the shutdown process. */

			Signal(HandlerProcess,SIG_NOTICE);

				/* Return the result. */

			return(File);
		}
		else
			return(OldOpen(Name,Mode,DOSBase));
	}
	else
		return(OldOpen(Name,Mode,DOSBase));
}

	/* MyClose(register __d1 BPTR Handle):
	 *
	 *	Modified Close() routine.
	 */

LONG __asm
MyClose(register __d1 BPTR Handle)
{
		/* Is the shutdown process running? */

	if(GlobalBase -> Running)
	{
		struct FileInfo *FileInfo;
		LONG		 Result;

				/* Increment the routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount++;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Gain access to the fileinfo list. */

		ObtainSemaphore(&GlobalBase -> AccessLock);

		FileInfo = (struct FileInfo *)GlobalBase -> AccessList . mlh_Head;

			/* Look for a fileinfo structure to match
			 * the current filehandle pointer.
			 */

		while(FileInfo -> Node . mln_Succ)
		{
			if(FileInfo -> Handle == Handle)
			{
				GlobalBase -> OpenCount--;

				Remove((struct Node *)FileInfo);

				FreeVec(FileInfo);

				break;
			}

			FileInfo = (struct FileInfo *)FileInfo -> Node . mln_Succ;
		}

		ReleaseSemaphore(&GlobalBase -> AccessLock);

			/* Close the file handle. */

		Result = OldClose(Handle,DOSBase);

			/* Decrement routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount--;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Notify the shutdown process. */

		Signal(HandlerProcess,SIG_NOTICE);

			/* Return the result. */

		return(Result);
	}
	else
		return(OldClose(Handle,DOSBase));
}

	/* MyWrite():
	 *
	 *	Modified Write() routine.
	 */

LONG __asm
MyWrite(register __d1 BPTR Handle,register __d2 APTR Buffer,register __d3 LONG Length)
{
		/* Is the shutdown process still running and are any bytes to write? */

	if(Length && GlobalBase -> Running)
	{
		LONG Result;

			/* Increment the routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount++;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Register the write access. */

		RegisterAccess(Handle);

			/* Write the data. */

		Result = OldWrite(Handle,Buffer,Length,DOSBase);

			/* Decrement the routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount--;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Notify the shutdown process. */

		Signal(HandlerProcess,SIG_NOTICE);

			/* Return the result. */

		return(Result);
	}
	else
		return(OldWrite(Handle,Buffer,Length,DOSBase));
}

	/* MyFPutC():
	 *
	 *	Modified FPutC() routine.
	 */

LONG __asm
MyFPutC(register __d1 BPTR Handle,register __d2 UBYTE Char)
{
		/* Is the shutdown process still running? */

	if(GlobalBase -> Running)
	{
		LONG Result;

			/* Increment routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount++;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Register the write access. */

		RegisterAccess(Handle);

			/* Output the character. */

		Result = OldFPutC(Handle,Char,DOSBase);

			/* Decrement the routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount--;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Notify the shutdown process. */

		Signal(HandlerProcess,SIG_NOTICE);

			/* Return the result. */

		return(Result);
	}
	else
		return(OldFPutC(Handle,Char,DOSBase));
}

	/* MyFPuts(register __d1 BPTR Handle,register __d2 STRPTR String):
	 *
	 *	Modified FPuts() routine.
	 */

LONG __asm
MyFPuts(register __d1 BPTR Handle,register __d2 STRPTR String)
{
		/* Is the shutdown process still running? */

	if(GlobalBase -> Running)
	{
		LONG Result;

			/* Increment routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount++;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Register the write access. */

		RegisterAccess(Handle);

			/* Output the string. */

		Result = OldFPuts(Handle,String,DOSBase);

			/* Decrement the routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount--;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Notify the shutdown process. */

		Signal(HandlerProcess,SIG_NOTICE);

			/* Return the result. */

		return(Result);
	}
	else
		return(OldFPuts(Handle,String,DOSBase));
}

	/* MyFWrite():
	 *
	 *	Modified FWrite() routine.
	 */

LONG __asm
MyFWrite(register __d1 BPTR Handle,register __d2 STRPTR Buffer,register __d3 ULONG BlockLen,register __d4 ULONG Blocks)
{
		/* Is the shutdown process still running? */

	if(GlobalBase -> Running)
	{
		LONG Result;

			/* Increment routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount++;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Register the write access. */

		RegisterAccess(Handle);

			/* Write the data. */

		Result = OldFWrite(Handle,Buffer,BlockLen,Blocks,DOSBase);

			/* Decrement the routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount--;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Notify the shutdown process. */

		Signal(HandlerProcess,SIG_NOTICE);

			/* Return the result. */

		return(Result);
	}
	else
		return(OldFWrite(Handle,Buffer,BlockLen,Blocks,DOSBase));
}

	/* MyVFPrintf():
	 *
	 *	Modified VFPrintf() routine.
	 */

LONG __asm
MyVFPrintf(register __d1 BPTR Handle,register __d2 STRPTR Format,register __d3 LONG *Args)
{
		/* Is the shutdown process still running? */

	if(GlobalBase -> Running)
	{
		LONG Result;

			/* Increment routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount++;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Register the write access. */

		RegisterAccess(Handle);

			/* Output the text. */

		Result = OldVFPrintf(Handle,Format,Args,DOSBase);

			/* Decrement the routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount--;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Notify the shutdown process. */

		Signal(HandlerProcess,SIG_NOTICE);

			/* Return the result. */

		return(Result);
	}
	else
		return(OldVFPrintf(Handle,Format,Args,DOSBase));
}

	/* MyVFWritef():
	 *
	 *	Modified VFWritef() routine.
	 */

LONG __asm
MyVFWritef(register __d1 BPTR Handle,register __d2 STRPTR Format,register __d3 LONG *Args)
{
		/* Is the shutdown process still running? */

	if(GlobalBase -> Running)
	{
		LONG Result;

			/* Increment routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount++;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Register the write access. */

		RegisterAccess(Handle);

			/* Write the data. */

		Result = OldVFWritef(Handle,Format,Args,DOSBase);

			/* Decrement the routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount--;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Notify the shutdown process. */

		Signal(HandlerProcess,SIG_NOTICE);

			/* Return the result. */

		return(Result);
	}
	else
		return(OldVFWritef(Handle,Format,Args,DOSBase));
}

	/* MyPutStr(register __d1 STRPTR Buffer):
	 *
	 *	Modified PutStr() routine.
	 */

LONG __asm
MyPutStr(register __d1 STRPTR Buffer)
{
		/* Is the shutdown process still running? */

	if(GlobalBase -> Running)
	{
		struct Process	*ThisProcess = (struct Process *)SysBase -> ThisTask;
		LONG		 Result;

			/* Increment routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount++;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Register the write access. */

		RegisterAccess(ThisProcess -> pr_COS);

			/* Output the text. */

		Result = OldPutStr(Buffer,DOSBase);

			/* Decrement the routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount--;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Notify the shutdown process. */

		Signal(HandlerProcess,SIG_NOTICE);

			/* Return the result. */

		return(Result);
	}
	else
		return(OldPutStr(Buffer,DOSBase));
}

	/* MyWriteChars():
	 *
	 *	Modified WriteChars() routine.
	 */

LONG __asm
MyWriteChars(register __d1 STRPTR Buffer,register __d2 ULONG Len)
{
		/* Is the shutdown process still running? */

	if(GlobalBase -> Running)
	{
		struct Process	*ThisProcess = (struct Process *)SysBase -> ThisTask;
		LONG		 Result;

			/* Increment routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount++;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Register the write access. */

		RegisterAccess(ThisProcess -> pr_COS);

			/* Write the characters. */

		Result = OldWriteChars(Buffer,Len,DOSBase);

			/* Decrement the routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount--;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Notify the shutdown process. */

		Signal(HandlerProcess,SIG_NOTICE);

			/* Return the result. */

		return(Result);
	}
	else
		return(OldWriteChars(Buffer,Len,DOSBase));
}

	/* MyVPrintf():
	 *
	 *	Modified VPrintf() routine.
	 */

LONG __asm
MyVPrintf(register __d1 STRPTR Format,register __d2 LONG *Args)
{
		/* Is the shutdown process still running? */

	if(GlobalBase -> Running)
	{
		struct Process	*ThisProcess = (struct Process *)SysBase -> ThisTask;
		LONG		 Result;

			/* Increment routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount++;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Register the write access. */

		RegisterAccess(ThisProcess -> pr_COS);

			/* Output the text. */

		Result = OldVPrintf(Format,Args,DOSBase);

			/* Decrement the routine user count. */

		ObtainSemaphore(&UseCountSemaphore);

		UseCount--;

		ReleaseSemaphore(&UseCountSemaphore);

			/* Notify the shutdown process. */

		Signal(HandlerProcess,SIG_NOTICE);

			/* Return the result. */

		return(Result);
	}
	else
		return(OldVPrintf(Format,Args,DOSBase));
}

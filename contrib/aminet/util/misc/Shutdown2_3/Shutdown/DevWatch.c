/*
**	Shutdown 2.0 package, DevWatch.c module
**
**	Copyright © 1992 by Olaf `Olsen' Barthel
**		All Rights Reserved
*/

#include "ShutdownGlobal.h"

	/* The following actions will be blocked since they
	 * generate write-accesses to a device.
	 */

#define ALL_BLOCK_FLAGS		((1 << CMD_READ) | (1 << CMD_WRITE) | (1 << TD_SEEK) | (1 << TD_FORMAT) | (1 << TD_RAWREAD) | (1 << TD_RAWWRITE))

	/* This piece of code gets wedged between the device BeginIO()
	 * vector and the actual routine associated with it. Before the
	 * call finally lands in the BeginIO() routine it has to pass
	 * through our blocking routine.
	 */

struct BeginIOWedge
{
	UWORD	Jsr;		/* The MC680x0 `JSR ea' opcode. */
	APTR	BlockRoutine;	/* The destination address. */

	UWORD	Jmp;		/* The MC680x0 `JMP ea' opcode. */
	APTR	BeginIO;	/* The destination address. */
};

	/* This is a big wedge entry, it stores the effective
	 * BeginIO() wedge and pointers to the base address of
	 * the device which was patched. Also included is a
	 * wedge for the Expunge() vector which prevents the
	 * device from getting expunged before the wedges
	 * are removed.
	 */

struct BigWedge
{
	struct BigWedge		*Next;		/* Next entry in list. */
	struct Device		*Device;	/* The patched device. */

	APTR			 OldExpunge;	/* The old expunge vector. */

	struct BeginIOWedge	 DevBeginIO;	/* The BeginIO() wedge. */
	struct Wedge		 DevExpunge;	/* The Expunge() wedge. */
};

	/* The list of wedges installed and the access semaphore. */

STATIC struct SignalSemaphore	 WedgeSemaphore;
STATIC struct BigWedge		*WedgeChain;

	/* Local data. */

STATIC struct Task		*DeviceWatchTask;

	/* Local routines for this module. */

STATIC VOID __saveds		DeviceWatchServer(VOID);

	/* DeviceWatchServer():
	 *
	 *	This is a watchdog task which continuously monitors
	 *	device IO activity.
	 */

STATIC VOID __saveds
DeviceWatchServer()
{
	struct MsgPort *TimePort;

		/* Initialize the wedge list access semaphore. */

	InitSemaphore(&DeviceWatchSemaphore);

		/* Create the timer reply port. */

	if(TimePort = CreateMsgPort())
	{
		struct timerequest *TimeRequest;

			/* Create the timer IO request. */

		if(TimeRequest = (struct timerequest *)CreateIORequest(TimePort,sizeof(struct timerequest)))
		{
				/* Open the timer.device. */

			if(!OpenDevice(TIMERNAME,UNIT_VBLANK,TimeRequest,NULL))
			{
				BYTE	HadAccesses	= TRUE,
					AllClear	= FALSE;
				ULONG	SignalSet;

					/* Provide handshake signal. */

				Signal(HandlerProcess,SIG_HANDSHAKE);

					/* Go into loop... */

				do
				{
						/* Gain access to the wedge list. */

					ObtainSemaphore(&DeviceWatchSemaphore);

						/* Is the shutdown process up and running? */

					if(MainProcess)
					{
							/* Did any IO activity take place and does
							 * it still make sense to notify the
							 * shutdown process?
							 */

						if(DeviceInUse != HadAccesses && !GlobalBase -> DevShutdown)
						{
								/* Did no access take place in
								 * the last few seconds?
								 */

							if(!DeviceInUse)
								AllClear = TRUE;
							else
								AllClear = FALSE;

								/* Remember the current state. */

							DeviceProbablyInUse = DeviceInUse;

								/* Notify the shutdown process. */

							Signal(MainProcess,SIG_NOTIFY);
						}

							/* Is the system just about to go down
							 * and IO activity increases? If so,
							 * notify the shutdown process.
							 */

						if(!DeviceInUse && !HadAccesses && GlobalBase -> DevShutdown)
							Signal(MainProcess,SIG_NOTIFY);
					}

						/* Remember previous state. */

					HadAccesses = DeviceInUse;

						/* Clear the flag. */

					DeviceInUse = FALSE;

						/* Release the access semaphore. */

					ReleaseSemaphore(&DeviceWatchSemaphore);

						/* Set up the timer IO request. */

					TimeRequest -> tr_node . io_Command	= TR_ADDREQUEST;
					TimeRequest -> tr_time . tv_secs	= 1;
					TimeRequest -> tr_time . tv_micro	= 500000;

						/* Is the shutdown process running? */

					if(MainProcess)
					{
							/* Start the timer. */

						SendIO(TimeRequest);

							/* Clear our notification signal. */

						SetSignal(0,SIG_NOTICE);

							/* Wait for timer or notification. */

						SignalSet = Wait(SIG_TIME | SIG_NOTICE);

							/* IO activity notification signal? */

						if(SignalSet & SIG_NOTICE)
						{
								/* Did we give permission to quit? */

							if(AllClear)
							{
									/* Change the flags. */

								DeviceInUse		= TRUE;
								DeviceProbablyInUse	= TRUE;
								AllClear		= FALSE;

									/* Notify the shutdown process. */

								Forbid();

								if(MainProcess)
									Signal(MainProcess,SIG_NOTIFY);

								Permit();
							}
						}
					}
					else
						SendIO(TimeRequest);

						/* Wait for timer. */

					WaitIO(TimeRequest);
				}
				while(!CheckSignal(SIG_KILL));

					/* Close the timer. */

				CloseDevice(TimeRequest);
			}

				/* Free the timer request. */

			DeleteIORequest(TimeRequest);
		}

			/* Delete the timer reply port. */

		DeleteMsgPort(TimePort);
	}

		/* Disable multitasking. */

	Forbid();

		/* Ring back. */

	Signal(HandlerProcess,SIG_HANDSHAKE);

		/* Remove ourselves. */

	RemTask(DeviceWatchTask = NULL);
}

	/* WedgeSetup():
	 *
	 *	Install the device BeginIO() wedges and start the watchdog.
	 */

BYTE
WedgeSetup()
{
	struct DosList *DosEntry;

		/* Set up the wedge list access semaphore. */

	InitSemaphore(&WedgeSemaphore);

		/* Create the watchdog. */

	DeviceWatchTask = CreateTask(GetString(MSG_DEVTASKNAME_NAME),50,(APTR)DeviceWatchServer,4000);

		/* Wait for ringback. */

	Wait(SIG_HANDSHAKE);

		/* Did we succeed in creating it? */

	if(DeviceWatchTask)
	{
			/* Lock the doslist for device reading. */

		if(DosEntry = LockDosList(LDF_DEVICES | LDF_READ))
		{
				/* Scan for devices... */

			while(DosEntry = NextDosEntry(DosEntry,LDF_DEVICES | LDF_READ))
			{
					/* Does it have a filesystem process attached? */

				if(TypeOfMem(DosEntry -> dol_Task))
				{
						/* Is the device startup valid. */

					if(TypeOfMem(BADDR(DosEntry -> dol_misc . dol_handler . dol_Startup)))
					{
						struct FileSysStartupMsg *FSSM = (struct FileSysStartupMsg *)BADDR(DosEntry -> dol_misc . dol_handler . dol_Startup);

							/* Is the startup message valid? If so,
							 * create a new device list entry.
							 */

						if(TypeOfMem(BADDR(FSSM -> fssm_Device)) && TypeOfMem(BADDR(FSSM -> fssm_Environ)))
							NewWedge(&((UBYTE *)BADDR(FSSM -> fssm_Device))[1]);
					}
				}
			}

				/* Release the lock. */

			UnLockDosList(LDF_DEVICES | LDF_READ);
		}

			/* Return success. */
	
		return(TRUE);
	}
	else
		return(FALSE);
}

	/* DeleteWedges():
	 *
	 *	Disable all the wedges installed by NewWedge().
	 */

VOID
DeleteWedges()
{
	struct BigWedge	*Wedge;

		/* Tell the watchdog task to shut down. */

	Signal(DeviceWatchTask,SIG_KILL);

		/* Wait for ringback. */

	Wait(SIG_HANDSHAKE);

		/* Disable multitasking. */

	Forbid();

		/* Obtain a lock on the wedge list. */

	ObtainSemaphore(&WedgeSemaphore);

	Wedge = WedgeChain;

		/* Walk down the list... */

	while(Wedge)
	{
			/* Redirect the `JSR ea' to the original BeginIO()
			 * routine.
			 */

		Wedge -> DevBeginIO . Jsr		= JMP_ABS;
		Wedge -> DevBeginIO . BlockRoutine	= Wedge -> DevBeginIO . BeginIO;

			/* Make the expunge wedge use the original routine. */

		Wedge -> DevExpunge . Address		= Wedge -> OldExpunge;

			/* Get to the next list entry. */

		Wedge = Wedge -> Next;
	}

		/* Flush the caches. */

	CacheClearU();

		/* Reenable multitasking. */

	Permit();

		/* Release the lock on the list. */

	ReleaseSemaphore(&WedgeSemaphore);
}

	/* NewWedge(STRPTR DeviceName):
	 *
	 *	Install a new BeginIO()/Expunge() wedge.
	 */

VOID
NewWedge(STRPTR DeviceName)
{
	struct BigWedge	*Wedge;
	BYTE		 IsInstalled = FALSE;
	struct Device	*Device;

		/* Gain access to the wedge list. */

	ObtainSemaphore(&WedgeSemaphore);

	Wedge = WedgeChain;

		/* Search the list... */

	while(Wedge)
	{
			/* Does the device in question have a valid name? */

		if(Wedge -> Device -> dd_Library . lib_Node . ln_Name)
		{
				/* Is there already a wedge in place for
				 * this device?
				 */

			if(!strcmp(Wedge -> Device -> dd_Library . lib_Node . ln_Name,DeviceName))
			{
					/* Looks like it. */

				IsInstalled = TRUE;

				break;
			}
		}

			/* Get to the next list entry. */

		Wedge = Wedge -> Next;
	}

		/* Has this device been patched already? */

	if(!IsInstalled)
	{
			/* Disable multitasking and interrupt processing. */

		Disable();

			/* Try to find the device in the list. */

		if(Device = (struct Device *)FindName(&SysBase -> DeviceList,DeviceName))
		{
				/* Allocate a new big wedge. */

			if(Wedge = (struct BigWedge *)AllocMem(sizeof(struct BigWedge),MEMF_PUBLIC | MEMF_CLEAR))
			{
					/* Set up the head and remember the device base address. */

				Wedge -> Next				= WedgeChain;
				Wedge -> Device				= Device;

					/* Install the new redirection code. */

				Wedge -> DevBeginIO . Jsr		= JSR_ABS;
				Wedge -> DevBeginIO . BlockRoutine	= BlockBeginIO;
				Wedge -> DevBeginIO . Jmp		= JMP_ABS;
				Wedge -> DevBeginIO . BeginIO		= SetFunction(Device,DEV_BEGINIO,(APTR)&Wedge -> DevBeginIO);

					/* Patch the expunge function. */

				Wedge -> DevExpunge . Command		= JMP_ABS;
				Wedge -> DevExpunge . Address		= NewDevExpunge;

				Wedge -> OldExpunge			= SetFunction(Device,-18,(APTR)&Wedge -> DevExpunge);

					/* Put the wedge into the list. */

				WedgeChain = Wedge;

					/* Flush the caches. */

				CacheClearU();
			}
		}

			/* Reenable multitasking and interrupt processing. */

		Enable();
	}

		/* Release the access lock. */

	ReleaseSemaphore(&WedgeSemaphore);
}

	/* DevExpunge(register __a6 struct Device *Device):
	 *
	 *	Modified expunge routine.
	 */

ULONG __asm
DevExpunge(register __a6 struct Device *Device)
{
		/* Clear the delayed expunge flag. */

	Device -> dd_Library . lib_Flags &= ~LIBF_DELEXP;

		/* Don't unload the device. */

	return(NULL);
}

	/* BlockRoutine(register __a1 struct IOStdReq *Request):
	 *
	 *	This routine is called before IO processing
	 *	actually gets to the BeginIO() vector. It is
	 *	to notify the watchdog of IO activity and
	 *	blocks IO processing in case the system is
	 *	to go down.
	 */

VOID __asm
BlockRoutine(register __a1 struct IOStdReq *Request)
{
	UWORD Command = Request -> io_Command & ~TDF_EXTCOM;

		/* Is this a likely write command? */

	if(Command < 32)
	{
			/* Is this a write access? */

		if((1L << Command) & ALL_BLOCK_FLAGS)
		{
				/* Change the access flag. */

			ObtainSemaphore(&DeviceWatchSemaphore);

			DeviceInUse = TRUE;

			ReleaseSemaphore(&DeviceWatchSemaphore);

				/* Is the system going down? If so,
				 * block until the lock is released.
				 */

			if(GlobalBase -> DevShutdown)
			{
				ObtainSemaphoreShared(&GlobalBase -> DevBlockLock);

				ReleaseSemaphore(&GlobalBase -> DevBlockLock);
			}

				/* Notify the watchdog. */

			Signal(DeviceWatchTask,SIG_NOTICE);
		}
	}
}

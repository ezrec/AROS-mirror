#include <proto/exec.h>
#include <aros/libcall.h>
#include <dos/filesystem.h>
#include <exec/errors.h>
#include <exec/memory.h>
#include <exec/resident.h>

#include "acdrbase.h"

extern const char name[];
extern const char version[];
extern const APTR inittab[4];
extern void *const acdrfunctable[];
extern const UBYTE acdrdatatable;
extern struct ACDRBase *AROS_SLIB_ENTRY(init,acdrdev)();
extern void AROS_SLIB_ENTRY(open,acdrdev)();
extern BPTR AROS_SLIB_ENTRY(close,acdrdev)();
extern BPTR AROS_SLIB_ENTRY(expunge,acdrdev)();
extern int AROS_SLIB_ENTRY(null,acdrdev)();
extern void AROS_SLIB_ENTRY(beginio,acdrdev)();
extern LONG AROS_SLIB_ENTRY(abortio,acdrdev)();
extern const char acdrhandlerend;

void ACDR_work(struct ACDRBase *);
void *ACDR_GetData(struct ACDRBase *);

int ACDR_entry(void)
{
	/* If the device was executed by accident return error code. */
	return -1;
}

const struct Resident ACDR_resident=
{
	RTC_MATCHWORD,
	(struct Resident *)&ACDR_resident,
	(APTR)&acdrhandlerend,
	RTF_COLDSTART | RTF_AFTERDOS | RTF_AUTOINIT,
	41,
	NT_DEVICE,
	-122,
	(char *)name,
	(char *)&version[6],
	(ULONG *)inittab
};

static const char name[]="cdrom.handler";
static const char version[]="$VER: cdrom.handler 1.15 (2002-07-07)\n";

static const APTR inittab[4]=
{
	(APTR)sizeof(struct ACDRBase),
	(APTR)acdrfunctable,
	(APTR)&acdrdatatable,
	&AROS_SLIB_ENTRY(init,acdrdev)
};

void *const acdrfunctable[]=
{
	&AROS_SLIB_ENTRY(open,acdrdev),
	&AROS_SLIB_ENTRY(close,acdrdev),
	&AROS_SLIB_ENTRY(expunge,acdrdev),
	&AROS_SLIB_ENTRY(null,acdrdev),
	&AROS_SLIB_ENTRY(beginio,acdrdev),
	&AROS_SLIB_ENTRY(abortio,acdrdev),
	(void *)-1
};

const UBYTE acdrdatatable = 0;

AROS_LH2(struct ACDRBase *, init,
 AROS_LHA(struct ACDRBase *, acdrbase, D0),
 AROS_LHA(BPTR,             segList, A0),
      struct ExecBase *, SysBase, 0, acdrdev)
{
	AROS_LIBFUNC_INIT

	struct Task *task;
	APTR stack;

	acdrbase->seglist = segList;
	acdrbase->SysBase = SysBase;
	acdrbase->DOSBase = (struct DOSBase *)OpenLibrary("dos.library",41);
	if (acdrbase->DOSBase)
	{
		NEWLIST(&acdrbase->port.mp_MsgList);
		acdrbase->port.mp_Node.ln_Type = NT_MSGPORT;
		acdrbase->port.mp_SigBit = SIGBREAKB_CTRL_F;
		NEWLIST(&acdrbase->rport.mp_MsgList);
		acdrbase->rport.mp_Node.ln_Type = NT_MSGPORT;
		acdrbase->rport.mp_Flags = PA_SIGNAL;
		acdrbase->rport.mp_SigBit = SIGB_SINGLE;
		NEWLIST(&acdrbase->prport.mp_MsgList);
		acdrbase->rport.mp_Node.ln_Type = NT_MSGPORT;
		acdrbase->rport.mp_Flags = PA_SIGNAL;
		/* signal will be initialized in task */
		task = (struct Task *)AllocMem(sizeof(struct Task), MEMF_PUBLIC | MEMF_CLEAR);
		if (task != NULL)
		{
			acdrbase->port.mp_SigTask = task;
			acdrbase->port.mp_Flags = PA_IGNORE;
			acdrbase->prport.mp_SigTask = task;
			NEWLIST(&task->tc_MemEntry);
			task->tc_Node.ln_Type = NT_TASK;
			task->tc_Node.ln_Name = "cdrom.handler task";
			task->tc_Node.ln_Pri = 10;
			stack = AllocMem(AROS_STACKSIZE, MEMF_PUBLIC);
			if (stack != NULL)
			{
			    	struct TagItem tags[] =
				{
				    {TASKTAG_ARG1, (IPTR)acdrbase},
				    {TAG_DONE	    	    	 }
				};
				
				task->tc_SPLower = stack;
				task->tc_SPUpper = (BYTE *)stack+AROS_STACKSIZE;
    	    	    	    #if AROS_STACK_GROWS_DOWNWARDS
				task->tc_SPReg = (BYTE *)task->tc_SPUpper-SP_OFFSET;
   	    	    	    #else
				task->tc_SPReg = (BYTE *)task->tc_SPLower+SP_OFFSET;
    	    	    	    #endif
				NEWLIST(&acdrbase->process_list);
				acdrbase->GetData=ACDR_GetData;
				if (NewAddTask(task,ACDR_work,NULL,tags) != NULL)
					return acdrbase;
				FreeMem(stack, AROS_STACKSIZE);
			}
			FreeMem(task, sizeof(struct Task));
		}
		CloseLibrary((struct Library *)acdrbase->DOSBase);
	}
	return NULL;
	AROS_LIBFUNC_EXIT
}

#ifdef SysBase
#	undef SysBase
#endif
#define SysBase acdrbase->SysBase

AROS_LH3(void, open,
 AROS_LHA(struct IOFileSys *, iofs, A1),
 AROS_LHA(ULONG,              unitnum, D0),
 AROS_LHA(ULONG,              flags, D1),
           struct ACDRBase *, acdrbase, 1,acdrdev)
{
	AROS_LIBFUNC_INIT

	unitnum = flags = 0;
	acdrbase->device.dd_Library.lib_OpenCnt++;
	acdrbase->rport.mp_SigTask=FindTask(NULL);

	iofs->IOFS.io_Command = -1;
	PutMsg(&acdrbase->port, &iofs->IOFS.io_Message);
	WaitPort(&acdrbase->rport);
	(void)GetMsg(&acdrbase->rport);
	if (iofs->io_DosError == NULL)
	{
		iofs->IOFS.io_Device = &acdrbase->device;
		acdrbase->device.dd_Library.lib_Flags &= ~LIBF_DELEXP;
		iofs->IOFS.io_Error = 0;
		return;
	}
	acdrbase->device.dd_Library.lib_OpenCnt--;
	iofs->IOFS.io_Error = IOERR_OPENFAIL;
	AROS_LIBFUNC_EXIT	
}

#ifdef DOSBase
#	undef DOSBase
#endif
#define DOSBase acdrbase->DOSBase

AROS_LH0(BPTR, expunge, struct ACDRBase *, acdrbase, 3, acdrdev)
{
	AROS_LIBFUNC_INIT

	BPTR retval;

	if (acdrbase->device.dd_Library.lib_OpenCnt) {
		acdrbase->device.dd_Library.lib_Flags |= LIBF_DELEXP;
		return 0;
	}
	RemTask(acdrbase->port.mp_SigTask);
	FreeMem(((struct Task *)acdrbase->port.mp_SigTask)->tc_SPLower,AROS_STACKSIZE);
	FreeMem(acdrbase->port.mp_SigTask, sizeof(struct Task));
	CloseLibrary((struct Library *)DOSBase);
	Remove(&acdrbase->device.dd_Library.lib_Node);
	retval=acdrbase->seglist;
	FreeMem
	(
		(char *)acdrbase-acdrbase->device.dd_Library.lib_NegSize,
			acdrbase->device.dd_Library.lib_NegSize+
			acdrbase->device.dd_Library.lib_PosSize
	);
	return retval;

	AROS_LIBFUNC_EXIT
}

AROS_LH1(BPTR, close,
 AROS_LHA(struct IOFileSys *, iofs, A1),
      struct ACDRBase *, acdrbase, 2, acdrdev)
{
	AROS_LIBFUNC_INIT

	acdrbase->rport.mp_SigTask=FindTask(NULL);
	iofs->IOFS.io_Command = -2;
	PutMsg(&acdrbase->port, &iofs->IOFS.io_Message);
	WaitPort(&acdrbase->rport);
	(void)GetMsg(&acdrbase->rport);
	if (iofs->io_DosError)
		return 0;				// there is still something to do on this volume
	iofs->IOFS.io_Device=(struct Device *)-1;
	if (!--acdrbase->device.dd_Library.lib_OpenCnt)
	{
		/* Delayed expunge pending? */
		if (acdrbase->device.dd_Library.lib_Flags & LIBF_DELEXP)
		{
			/* Then expunge the device */
			return expunge();
		}
	}
	return 0;
	AROS_LIBFUNC_EXIT
}

AROS_LH0I(int, null, struct acdrbase *, acdrbase, 4, acdrdev)
{
	AROS_LIBFUNC_INIT
	return 0;
	AROS_LIBFUNC_EXIT
}

AROS_LH1(void, beginio,
 AROS_LHA(struct IOFileSys *, iofs, A1),
           struct ACDRBase *, acdrbase, 5, acdrdev)
{
	AROS_LIBFUNC_INIT
	/* WaitIO will look into this */
    iofs->IOFS.io_Message.mn_Node.ln_Type = NT_MESSAGE;
	/* Nothing is done quick */
    iofs->IOFS.io_Flags &= ~IOF_QUICK;
	/* So let the device task do it */
    PutMsg(&acdrbase->port, &iofs->IOFS.io_Message);
	AROS_LIBFUNC_EXIT
}

AROS_LH1(LONG, abortio,
 AROS_LHA(struct IOFileSys *, iofs, A1),
           struct ACDRBase *, acdrbase, 6, acdrdev)
{
	AROS_LIBFUNC_INIT
	return 0;
	AROS_LIBFUNC_EXIT
}

static const char acdrhandlerend = 0;

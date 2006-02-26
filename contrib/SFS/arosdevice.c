#include <proto/exec.h>
#include <aros/libcall.h>
#include <aros/asmcall.h>
#include <dos/filesystem.h>
#include <exec/errors.h>
#include <exec/memory.h>
#include <exec/resident.h>

#include "asfsbase.h"
// #include "cdrom.h"
//#include "devsupp.h"

extern const char asfsname[];
extern const char asfsversion[];
extern const APTR asfsinittab[4];
static struct ASFSBase *AROS_SLIB_ENTRY(init,asfsdev)();
static void AROS_SLIB_ENTRY(open,asfsdev)();
static BPTR AROS_SLIB_ENTRY(close,asfsdev)();
static BPTR AROS_SLIB_ENTRY(expunge,asfsdev)();
static int AROS_SLIB_ENTRY(null,asfsdev)();
static void AROS_SLIB_ENTRY(beginio,asfsdev)();
static LONG AROS_SLIB_ENTRY(abortio,asfsdev)();
extern const char asfshandlerend;
static AROS_LP2(struct ASFSBase *, init,
        AROS_LPA(struct ASFSBase *, asfsbase, D0),
        AROS_LPA(BPTR,             segList, A0),
        struct ExecBase *, SysBase, 0, asfsdev);
void ASFS_work(struct ASFSBase *);
void *ASFS_GetData(struct ASFSBase *);

int ASFS_entry(void)
{
        /* If the device was executed by accident return error code. */
        return -1;
}

const struct Resident ASFS_resident=
{
        RTC_MATCHWORD,
        (struct Resident *)&ASFS_resident,
        (APTR)&asfshandlerend,
        RTF_COLDSTART | RTF_AFTERDOS | RTF_AUTOINIT,
        41,
        NT_DEVICE,
        -122,
        (char *)asfsname,
        (char *)&asfsversion[6],
        (ULONG *)asfsinittab
};

const char asfsname[]="sfs.handler";
const char asfsversion[]="$VER: sfs 1.84 (30.11.2005)\r\n";

static const UBYTE asfsdatatable = 0;

static void *const asfsfunctable[]=
{
        &AROS_SLIB_ENTRY(open,asfsdev),
        &AROS_SLIB_ENTRY(close,asfsdev),
        &AROS_SLIB_ENTRY(expunge,asfsdev),
        &AROS_SLIB_ENTRY(null,asfsdev),
        &AROS_SLIB_ENTRY(beginio,asfsdev),
        &AROS_SLIB_ENTRY(abortio,asfsdev),
        (void *)-1
};

const APTR asfsinittab[4]=
{
        (APTR)sizeof(struct ASFSBase),
        (APTR)asfsfunctable,
        (APTR)&asfsdatatable,
        &AROS_SLIB_ENTRY(init,asfsdev)
};

static
AROS_LH2(struct ASFSBase *, init,
 AROS_LHA(struct ASFSBase *, asfsbase, D0),
 AROS_LHA(BPTR,             segList, A0),
      struct ExecBase *, SysBase, 0, asfsdev)
{
        AROS_LIBFUNC_INIT

        struct Task *task;
        APTR stack;

        asfsbase->seglist = segList;
        asfsbase->SysBase = SysBase;
        asfsbase->DOSBase = (struct DOSBase *)OpenLibrary("dos.library",41);
        if (asfsbase->DOSBase)
        {
                NEWLIST(&asfsbase->port.mp_MsgList);
                asfsbase->port.mp_Node.ln_Type = NT_MSGPORT;
                asfsbase->port.mp_SigBit = SIGBREAKB_CTRL_F;
                NEWLIST(&asfsbase->rport.mp_MsgList);
                asfsbase->rport.mp_Node.ln_Type = NT_MSGPORT;
                asfsbase->rport.mp_Flags = PA_SIGNAL;
                asfsbase->rport.mp_SigBit = SIGB_SINGLE;
                NEWLIST(&asfsbase->prport.mp_MsgList);
                asfsbase->rport.mp_Node.ln_Type = NT_MSGPORT;
                asfsbase->rport.mp_Flags = PA_SIGNAL;
                /* signal will be initialized in task */
                task = (struct Task *)AllocMem(sizeof(struct Task), MEMF_PUBLIC | MEMF_CLEAR);
                if (task != NULL)
                {
                        asfsbase->port.mp_SigTask = task;
                        asfsbase->port.mp_Flags = PA_IGNORE;
                        asfsbase->prport.mp_SigTask = task;
                        NEWLIST(&task->tc_MemEntry);
                        task->tc_Node.ln_Type = NT_TASK;
                        task->tc_Node.ln_Name = "sfs.handler task";
                        task->tc_Node.ln_Pri = 10;
                        stack = AllocMem(AROS_STACKSIZE, MEMF_PUBLIC);
                        if (stack != NULL)
                        {
                                struct TagItem tags[] =
                                {
                                    {TASKTAG_ARG1, (IPTR)asfsbase},
                                    {TAG_DONE                    }
                                };

                                task->tc_SPLower = stack;
                                task->tc_SPUpper = (BYTE *)stack+AROS_STACKSIZE;
                            #if AROS_STACK_GROWS_DOWNWARDS
                                task->tc_SPReg = (BYTE *)task->tc_SPUpper-SP_OFFSET;
                            #else
                                task->tc_SPReg = (BYTE *)task->tc_SPLower+SP_OFFSET;
                            #endif
                                NEWLIST(&asfsbase->process_list);
                                asfsbase->GetData=ASFS_GetData;
                                if (NewAddTask(task,ASFS_work,NULL,tags) != NULL)
                                        return asfsbase;
                                FreeMem(stack, AROS_STACKSIZE);
                        }
                                    FreeMem(task, sizeof(struct Task));
                }
                CloseLibrary((struct Library *)asfsbase->DOSBase);
        }
        return NULL;
        AROS_LIBFUNC_EXIT
}

#ifdef SysBase
#       undef SysBase
#endif
#define SysBase asfsbase->SysBase

static
AROS_LH3(void, open,
 AROS_LHA(struct IOFileSys *, iofs, A1),
 AROS_LHA(ULONG,              unitnum, D0),
 AROS_LHA(ULONG,              flags, D1),
           struct ASFSBase *, asfsbase, 1,asfsdev)
{
        AROS_LIBFUNC_INIT

        unitnum = flags = 0;
        asfsbase->device.dd_Library.lib_OpenCnt++;
        asfsbase->rport.mp_SigTask=FindTask(NULL);

        iofs->IOFS.io_Command = -1;
        PutMsg(&asfsbase->port, &iofs->IOFS.io_Message);
        WaitPort(&asfsbase->rport);
        (void)GetMsg(&asfsbase->rport);
        if (iofs->io_DosError == NULL)
        {
                iofs->IOFS.io_Device = &asfsbase->device;
                asfsbase->device.dd_Library.lib_Flags &= ~LIBF_DELEXP;
                iofs->IOFS.io_Error = 0;
                return;
        }
        asfsbase->device.dd_Library.lib_OpenCnt--;
        iofs->IOFS.io_Error = IOERR_OPENFAIL;
        AROS_LIBFUNC_EXIT
}

#ifdef DOSBase
#       undef DOSBase
#endif
#define DOSBase asfsbase->DOSBase

static
AROS_LH0(BPTR, expunge, struct ASFSBase *, asfsbase, 3, asfsdev)
{
        AROS_LIBFUNC_INIT

        BPTR retval;

        if (asfsbase->device.dd_Library.lib_OpenCnt) {
                asfsbase->device.dd_Library.lib_Flags |= LIBF_DELEXP;
                return 0;
        }
        RemTask(asfsbase->port.mp_SigTask);
        FreeMem(((struct Task *)asfsbase->port.mp_SigTask)->tc_SPLower,AROS_STACKSIZE);
        FreeMem(asfsbase->port.mp_SigTask, sizeof(struct Task));
        CloseLibrary((struct Library *)DOSBase);
        Remove(&asfsbase->device.dd_Library.lib_Node);
        retval=asfsbase->seglist;
        FreeMem
        (
                (char *)asfsbase-asfsbase->device.dd_Library.lib_NegSize,
                        asfsbase->device.dd_Library.lib_NegSize+
                        asfsbase->device.dd_Library.lib_PosSize
        );
        return retval;

        AROS_LIBFUNC_EXIT
}

static
AROS_LH1(BPTR, close,
 AROS_LHA(struct IOFileSys *, iofs, A1),
      struct ASFSBase *, asfsbase, 2, asfsdev)
{
        AROS_LIBFUNC_INIT

        asfsbase->rport.mp_SigTask=FindTask(NULL);
        iofs->IOFS.io_Command = -2;
        PutMsg(&asfsbase->port, &iofs->IOFS.io_Message);
        WaitPort(&asfsbase->rport);
        (void)GetMsg(&asfsbase->rport);
        if (iofs->io_DosError)
                return 0;                               // there is still something to do on this volume
        iofs->IOFS.io_Device=(struct Device *)-1;
        if (!--asfsbase->device.dd_Library.lib_OpenCnt)
        {
                /* Delayed expunge pending? */
                if (asfsbase->device.dd_Library.lib_Flags & LIBF_DELEXP)
                {
                        /* Then expunge the device */
                        return expunge();
                }
        }
        return 0;
        AROS_LIBFUNC_EXIT
}

static
AROS_LH0I(int, null, struct asfsbase *, asfsbase, 4, asfsdev)
{
        AROS_LIBFUNC_INIT
        return 0;
        AROS_LIBFUNC_EXIT
}

static
AROS_LH1(void, beginio,
 AROS_LHA(struct IOFileSys *, iofs, A1),
           struct ASFSBase *, asfsbase, 5, asfsdev)
{
        AROS_LIBFUNC_INIT
        /* WaitIO will look into this */
    iofs->IOFS.io_Message.mn_Node.ln_Type = NT_MESSAGE;
        /* Nothing is done quick */
    iofs->IOFS.io_Flags &= ~IOF_QUICK;
        /* So let the device task do it */
    PutMsg(&asfsbase->port, &iofs->IOFS.io_Message);
        AROS_LIBFUNC_EXIT
}

static
AROS_LH1(LONG, abortio,
 AROS_LHA(struct IOFileSys *, iofs, A1),
           struct ASFSBase *, asfsbase, 6, asfsdev)
{
        AROS_LIBFUNC_INIT
        return 0;
        AROS_LIBFUNC_EXIT
}

const char asfshandlerend = 0;

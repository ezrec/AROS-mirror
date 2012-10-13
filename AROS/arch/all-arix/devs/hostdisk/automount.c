#include <fcntl.h>

#include <aros/symbolsets.h>
#include <aros/debug.h>
#include <dos/filehandler.h>
#include <exec/rawfmt.h>
#include <libraries/expansion.h>
#include <libraries/expansionbase.h>
#include <proto/exec.h>
#include <proto/expansion.h>
#include <proto/hostlib.h>
#include <devices/trackdisk.h>

#include LC_LIBDEFS_FILE

extern const char GM_UNIQUENAME(LibName)[];

struct unitExt
{
    struct unit     base;
    LONG            ue_UnitNum;
    LONG            ue_DeviceType;
};

/* Add a bootnode using expansion.library */
static BOOL hd_RegisterVolume(struct DriveGeometry * dg, struct unitExt * unit)
{
    struct ExpansionBase *ExpansionBase;
    struct DeviceNode *devnode;
    TEXT dosdevname[4] = "HD0";
    const ULONG IdDOS = AROS_MAKE_ID('D','O','S','\001');
    const ULONG IdCDVD = AROS_MAKE_ID('C','D','V','D');

    ExpansionBase = (struct ExpansionBase *)OpenLibrary("expansion.library",
                                                        40L);

    if (ExpansionBase)
    {
        IPTR pp[24];

        /* This should be dealt with using some sort of volume manager or such. */
        switch (unit->ue_DeviceType)
        {
            case DG_DIRECT_ACCESS:
                break;
            case DG_CDROM:
                dosdevname[0] = 'C';
                break;
            default:
                D(bug("[ATA>>]:-ata_RegisterVolume called on unknown devicetype\n"));
        }

        if (unit->ue_UnitNum < 10)
            dosdevname[2] += unit->ue_UnitNum % 10;
        else
            dosdevname[2] = 'A' - 10 + unit->ue_UnitNum;

        pp[0]                   = (IPTR)dosdevname;
        pp[1]                   = (IPTR)GM_UNIQUENAME(LibName);
        pp[2]                   = unit->ue_UnitNum;
        pp[DE_TABLESIZE    + 4] = DE_BOOTBLOCKS;
        pp[DE_SIZEBLOCK    + 4] = dg->dg_SectorSize >> 2;
        pp[DE_NUMHEADS     + 4] = dg->dg_Heads;
        pp[DE_SECSPERBLOCK + 4] = 1;
        pp[DE_BLKSPERTRACK + 4] = dg->dg_TrackSectors;
        pp[DE_RESERVEDBLKS + 4] = 2;
        pp[DE_LOWCYL       + 4] = 0;
        pp[DE_HIGHCYL      + 4] = dg->dg_Cylinders - 1;
        pp[DE_NUMBUFFERS   + 4] = 10;
        pp[DE_BUFMEMTYPE   + 4] = MEMF_PUBLIC | MEMF_31BIT;
        pp[DE_MAXTRANSFER  + 4] = 0x00200000;
        pp[DE_MASK         + 4] = 0x7FFFFFFE;
        pp[DE_BOOTPRI      + 4] = ((unit->ue_DeviceType == DG_DIRECT_ACCESS) ? 0 : 10);
        pp[DE_DOSTYPE      + 4] = ((unit->ue_DeviceType == DG_DIRECT_ACCESS) ? IdDOS : IdCDVD);
        pp[DE_CONTROL      + 4] = 0;
        pp[DE_BOOTBLOCKS   + 4] = 2;

        devnode = MakeDosNode(pp);

        if (devnode)
        {
            D(bug("[HostDisk]:hd_RegisterVolume: '%b', type=0x%08lx with StartCyl=%d, EndCyl=%d .. ",
                  devnode->dn_Name, pp[DE_DOSTYPE + 4], pp[DE_LOWCYL + 4], pp[DE_HIGHCYL + 4]));

            AddBootNode(pp[DE_BOOTPRI + 4], ADNF_STARTPROC, devnode, NULL);
            D(bug("done\n"));

            return TRUE;
        }

        CloseLibrary((struct Library *)ExpansionBase);
    }

    return FALSE;
}

static struct unitExt * CreateUnitTask(STRPTR nodename, LONG unitnum, struct HostDiskBase *hdskBase)
{
    struct unitExt * unit = (struct unitExt *)AllocMem(sizeof(struct unitExt), MEMF_PUBLIC | MEMF_CLEAR);

    ObtainSemaphore(&hdskBase->sigsem);

    if (unit != NULL)
    {
        struct IOExtTD iotd; /* Just a dummy to match the UnitEntry signature */
        LONG libnamelen = strlen((char *)GM_UNIQUENAME(LibName));
        LONG nodenamelen = strlen(nodename);
        TEXT taskName[libnamelen + nodenamelen + 2];
        struct Task *unitTask;
        STRPTR unitname = AllocVec(nodenamelen + 1, MEMF_ANY);

        D(bug("hostdisk: in CreateUnitTask func. Allocation of unit memory okay. Setting up unit and calling CreateNewProc ...\n"));

        strcpy(unitname, nodename);
        CopyMem(GM_UNIQUENAME(LibName), taskName, libnamelen);
        taskName[libnamelen] = ' ';
        strcpy(&taskName[libnamelen + 1], unitname);

        unit->ue_UnitNum        = unitnum;
        unit->base.n.ln_Name    = unitname;
        unit->base.usecount     = 1;
        unit->base.hdskBase     = hdskBase;
        unit->base.flags        = UNIT_FREENAME;
        NEWLIST((struct List *)&unit->base.changeints);

        iotd.iotd_Req.io_Unit = (struct Unit *)unit;
        SetSignal(0, SIGF_SINGLE);
        unitTask = NewCreateTask(TASKTAG_PC  , UnitEntry,
                                 TASKTAG_NAME, taskName,
                                 TASKTAG_ARG1, &iotd,
                                 TASKTAG_PRI, 10,
                                 TAG_DONE);

        D(bug("hostdisk: in CreateUnitTask func. NewCreateTask() called. Task = 0x%p\n", unitTask));

        if (unitTask)
        {
            D(bug("hostdisk: in CreateUnitTask func. Waiting for signal from unit task...\n"));
            Wait(SIGF_SINGLE);

            D(bug("hostdisk: in CreateUnitTask func. Unit error %u, flags 0x%02X\n", iotd->iotd_Req.io_Error, unit->flags));
            if (!iotd.iotd_Req.io_Error)
            {
                AddTail((struct List *)&hdskBase->units, &unit->base.n);
                ReleaseSemaphore(&hdskBase->sigsem);
                return unit;
            }
        }

        FreeUnit((struct unit *)unit);
    }

    ReleaseSemaphore(&hdskBase->sigsem);

    return unit;
}

void FreeUnit(struct unit *Unit)
{
    if (Unit->flags & UNIT_FREENAME)
        FreeVec(Unit->n.ln_Name);

    FreeMem(Unit, sizeof(struct unitExt));
}

static VOID HandleDeviceNode(STRPTR nodename, LONG devicetype, struct HostDiskBase *hdskBase)
{
    struct unitExt * unitext = NULL;

    if ((unitext = CreateUnitTask(nodename, hdskBase->unitBase, hdskBase)) != NULL)
    {
        struct DriveGeometry dg;
        if (Host_ProbeGeometry(hdskBase, nodename, &dg) == 0)
        {
            unitext->ue_DeviceType = devicetype;
            hd_RegisterVolume(&dg, unitext);
        }

        hdskBase->unitBase++;
    }
}

static VOID ScanDeviceNodes(STRPTR nodenametemplate, LONG startchar, LONG range, LONG devicetype,
        struct HostDiskBase *hdskBase)
{
    TEXT nodename[64];
    LONG i, file;

    for (i = 0; i < range; i++)
    {
        NewRawDoFmt(nodenametemplate, (VOID_FUNC)RAWFMTFUNC_STRING, nodename, i + startchar);

        HostLib_Lock();
        file = hdskBase->iface->open(nodename, O_RDONLY, 0755);
        AROS_HOST_BARRIER
        HostLib_Unlock();

        if (file != -1)
        {
            HostLib_Lock();
            hdskBase->iface->close(file);
            AROS_HOST_BARRIER
            HostLib_Unlock();

            HandleDeviceNode(nodename, devicetype, hdskBase);
        }
    }

}

static int deviceProbe(struct HostDiskBase *hdskBase)
{
    hdskBase->unitBase = 0; /* Reuse the unitBase field as our "next unit num" */

    ScanDeviceNodes("/dev/hd%lc", 'a', 12, DG_DIRECT_ACCESS, hdskBase);

    ScanDeviceNodes("/dev/sd%lc", 'a', 12, DG_DIRECT_ACCESS, hdskBase);

    ScanDeviceNodes("/dev/sr%ld", 0, 12, DG_CDROM, hdskBase);

    return TRUE;
}

ADD2INITLIB(deviceProbe, 10);

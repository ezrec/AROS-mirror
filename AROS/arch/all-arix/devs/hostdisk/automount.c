/*
 * Define struct stat64 on Linux.
 * Define this before anything, since AROS includes
 * may include POSIX includes (for example aros/debug.h
 * includes string.h)
 */
#define _LARGEFILE64_SOURCE

#include <aros/debug.h>
#include <dos/filehandler.h>
#include <exec/rawfmt.h>
#include <libraries/expansion.h>
#include <libraries/expansionbase.h>
#include <proto/exec.h>
#include <proto/expansion.h>
#include <proto/hostlib.h>

#include <linux/fs.h>

#include LC_LIBDEFS_FILE

extern const char GM_UNIQUENAME(LibName)[];

struct unitExt
{
    struct unit     base;
    LONG            ue_UnitNum;
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
        switch (dg->dg_DeviceType)
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
        pp[1]                   = (IPTR)MOD_NAME_STRING;
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
        pp[DE_BOOTPRI      + 4] = ((dg->dg_DeviceType == DG_DIRECT_ACCESS) ? 0 : 10);
        pp[DE_DOSTYPE      + 4] = ((dg->dg_DeviceType == DG_DIRECT_ACCESS) ? IdDOS : IdCDVD);
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

static int Automount(struct HostDiskBase *hdskBase)
{
    struct ExpansionBase *ExpansionBase = NULL;
    char *unit;
    struct DriveGeometry dg;
    ULONG u, len;

    if (hdskBase->segList)
    {
        D(bug("hostdisk: Loaded from disk, skipping automount.\n"));
        return TRUE;
    }

    len = strlen(hdskBase->DiskDevice) + 5;
    unit = AllocMem(len, MEMF_ANY);
    if (!unit)
        return FALSE;

    for (u = 0; u < 30; u++)
    {
        struct unitExt unitext;
        unitext.ue_UnitNum = u;

        NewRawDoFmt(hdskBase->DiskDevice, (VOID_FUNC)RAWFMTFUNC_STRING, unit, u + hdskBase->unitBase);

        if (Host_ProbeGeometry(hdskBase, unit, &dg) == 0)
        {
            hd_RegisterVolume(&dg, &unitext);
        }
    }

    if (ExpansionBase)
        CloseLibrary(&ExpansionBase->LibNode);
        
    FreeMem(unit, len);
    return TRUE;
}

ADD2INITLIB(Automount, 20);


/*
 * This checks if the system has /dev/hd* entries at all
 * and uses /dev/sd* if not.
 * It is assumed that we have at least /dev/hda.
 */
static int deviceProbe(struct HostDiskBase *hdskBase)
{
    struct stat64 st;
    int res;

#if 1 /* HACK TO BOOT FROM CDROM */
    hdskBase->DiskDevice = "/dev/sr%ld";
    hdskBase->unitBase = 0;
    return TRUE;
#endif

    HostLib_Lock();
    res = hdskBase->iface->stat64("/dev/hda", &st);
    HostLib_Unlock();

    D(bug("hostdisk: /dev/hda check result: %d\n", res));
    if (res == -1)
        hdskBase->DiskDevice = "/dev/sd%lc";

    return TRUE;
}

ADD2INITLIB(deviceProbe, 10);

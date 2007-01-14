/*
 * $Id$
 */

/*
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston,
    MA 02111-1307, USA.
*/

#include <exec/types.h>
#include <exec/resident.h>
#include <exec/io.h>
#include <exec/errors.h>
#include <exec/lists.h>

#include <aros/libcall.h>
#include <aros/symbolsets.h>

#include <oop/oop.h>

#include <devices/sana2.h>
#include <devices/sana2specialstats.h>

#include <utility/utility.h>
#include <utility/tagitem.h>
#include <utility/hooks.h>

#include <hidd/pci.h>

#include <proto/oop.h>
#include <proto/exec.h>
#include <proto/utility.h>

#include "rtl8139.h"
#include "unit.h"
#include LC_LIBDEFS_FILE

struct Library *OOPBase;

AROS_UFH3(void, Enumerator,
    AROS_UFHA(struct Hook *,    hook,       A0),
    AROS_UFHA(OOP_Object *,     pciDevice,  A2),
    AROS_UFHA(APTR,             message,    A1))
{
    AROS_USERFUNC_INIT

D(bug("[RTL8139] init.PCI_Enumerator()\n"));

    LIBBASETYPEPTR LIBBASE = (LIBBASETYPEPTR)hook->h_Data;

	BOOL FoundCompatNIC = FALSE;
    IPTR DeviceID, VendorID, RevisionID, CardCapabilities;
    OOP_GetAttr(pciDevice, aHidd_PCIDevice_VendorID, &VendorID);
    OOP_GetAttr(pciDevice, aHidd_PCIDevice_ProductID, &DeviceID);
    OOP_GetAttr(pciDevice, aHidd_PCIDevice_RevisionID, &RevisionID);
	char *CardName, *CardChipName;
	
    if ((VendorID == 0x10ec)    &&
        (DeviceID == 0x8129))
    {
		FoundCompatNIC = TRUE;
		CardCapabilities = RTLc_HAS_MII_XCVR;
        CardName = "RealTek RTL8129";
		CardChipName = "RTL8129";
    }
    else if ((VendorID == 0x10ec)    &&
                ((DeviceID == 0x8138)  ||
	            (DeviceID == 0x8139)))
    {
		FoundCompatNIC = TRUE;
		if ((DeviceID == 0x8139) && (RevisionID >= 0x20))
		{
			CardCapabilities =   RTLc_HAS_CHIP_XCVR | RTLc_HAS_LNK_CHNG | RTLc_HAS_DESC;
			CardName = "RealTek RTL8139C";
			CardChipName = "RTL8139C";
		}
		else
		{
			CardCapabilities =   RTLc_HAS_CHIP_XCVR | RTLc_HAS_LNK_CHNG;
			CardName = "RealTek RTL8139";
			CardChipName = "RTL8139";
		}
    }
    else if ((VendorID == 0x1113)    &&
        (DeviceID == 0x1211))
    {
		FoundCompatNIC = TRUE;
		CardCapabilities =   RTLc_HAS_CHIP_XCVR | RTLc_HAS_LNK_CHNG;
        CardName = "Accton EN-1207D Fast Ethernet";
		CardChipName = "RTL8139";
    }
    else if ((VendorID == 0x1186)    &&
        (DeviceID == 0x1300))
    {
		FoundCompatNIC = TRUE;
		CardCapabilities =   RTLc_HAS_CHIP_XCVR | RTLc_HAS_LNK_CHNG;
        CardName = "D-Link DFE-538TX";
		CardChipName = "RTL8139";
    }
    else if ((VendorID == 0x018a)    &&
        (DeviceID == 0x0106))
    {
		FoundCompatNIC = TRUE;
		CardCapabilities =   RTLc_HAS_CHIP_XCVR | RTLc_HAS_LNK_CHNG;
        CardName = "LevelOne FPC-0106Tx";
		CardChipName = "RTL8139";
    }
    else if ((VendorID == 0x018a)    &&
        (DeviceID == 0x0106))
    {
		FoundCompatNIC = TRUE;
		CardCapabilities =   RTLc_HAS_CHIP_XCVR | RTLc_HAS_LNK_CHNG;
        CardName = "Compaq HNE-300";
		CardChipName = "RTL8139c";
    }

	if (FoundCompatNIC)
	{
D(bug("[RTL8139] Found %s NIC [%s], PCI_ID %04x:%04x Rev:%d\n", CardName, CardChipName, VendorID, DeviceID, RevisionID));

        struct RTL8139Unit   *unit = CreateUnit(LIBBASE, pciDevice, CardCapabilities, CardName, CardChipName);
        LIBBASE->rtl8139b_unit = unit;

D(bug("%s %s NIC I/O MEM @ %08x\n", unit->rtl8139u_name, unit->rtl8139u_rtl_chipname, unit->rtl8139u_BaseMem));
	}
	
    AROS_USERFUNC_EXIT
}

static int GM_UNIQUENAME(Init)(LIBBASETYPEPTR LIBBASE)
{
D(bug("[RTL8139] init.Init()\n"));

    if (FindTask(RTL8139_TASK_NAME) != NULL)
    {
        D(bug("[RTL8139] device already up and running.\n"));
        return FALSE;
    }

    LIBBASE->rtl8139b_Sana2Info.HardwareType = S2WireType_Ethernet;
    LIBBASE->rtl8139b_Sana2Info.MTU = ETH_MTU;
    LIBBASE->rtl8139b_Sana2Info.AddrFieldSize = 8 * ETH_ADDRESSSIZE;

    OOPBase = OpenLibrary("oop.library",0);

    if (OOPBase != NULL)
    {
        D(bug("[RTL8139] Got oop.library\n"));

        LIBBASE->rtl8139b_UtilityBase = OpenLibrary("utility.library",0);

        if (LIBBASE->rtl8139b_UtilityBase != NULL)
        {
            D(bug("[RTL8139] Got utility.library\n"));

            LIBBASE->rtl8139b_pciDeviceAttrBase = OOP_ObtainAttrBase(IID_Hidd_PCIDevice);

            if (LIBBASE->rtl8139b_pciDeviceAttrBase != 0)
            {
                D(bug("[RTL8139] Got HiddPCIDeviceAttrBase\n"));

                LIBBASE->rtl8139b_pci = OOP_NewObject(NULL, CLID_Hidd_PCI, NULL);

                if (LIBBASE->rtl8139b_pci)
                {
                    D(bug("[RTL8139] Got PCI object\n"));

                    struct Hook FindHook = {
                        h_Entry:    (IPTR (*)())Enumerator,
                        h_Data:     LIBBASE,
                    };

                    struct TagItem Requirements[] = {
                        { TAG_DONE,             0UL }
                    };

                    HIDD_PCI_EnumDevices(   LIBBASE->rtl8139b_pci,
                                            &FindHook,
                                            (struct TagItem *)&Requirements);

                    if (LIBBASE->rtl8139b_unit)
                    {
                        return TRUE;
                    }
                }
            }
        }
    }

    return FALSE;
}

static int GM_UNIQUENAME(Expunge)(LIBBASETYPEPTR LIBBASE)
{
D(bug("[RTL8139] init.Expunge\n"));

    if (LIBBASE->rtl8139b_unit)
    {
        DeleteUnit(LIBBASE, LIBBASE->rtl8139b_unit);
        LIBBASE->rtl8139b_unit = NULL;
    }

    if (LIBBASE->rtl8139b_pciDeviceAttrBase != 0)
        OOP_ReleaseAttrBase(IID_Hidd_PCIDevice);

    LIBBASE->rtl8139b_pciDeviceAttrBase = 0;

    if (LIBBASE->rtl8139b_pci != NULL)
        OOP_DisposeObject(LIBBASE->rtl8139b_pci);

    if (OOPBase != NULL)
        CloseLibrary(OOPBase);

    if (LIBBASE->rtl8139b_UtilityBase != NULL)
        CloseLibrary(LIBBASE->rtl8139b_UtilityBase);

    return TRUE;
}

static const ULONG rx_tags[] = {
    S2_CopyToBuff,
    S2_CopyToBuff16
};

static const ULONG tx_tags[] = {
    S2_CopyFromBuff,
    S2_CopyFromBuff16,
    S2_CopyFromBuff32
};

/*
 * Open device handles currently only one rtl8139 unit.
 */
static int GM_UNIQUENAME(Open)
(
    LIBBASETYPEPTR LIBBASE,
    struct IOSana2Req* req,
    ULONG unitnum,
    ULONG flags
)
{
    struct TagItem *tags;
    struct RTL8139Unit *unit = LIBBASE->rtl8139b_unit;
    struct Opener *opener;
    BYTE error=0;
    int i;

D(bug("[RTL8139] init.OpenDevice\n"));

    req->ios2_Req.io_Unit = NULL;
    tags = req->ios2_BufferManagement;

    req->ios2_BufferManagement = NULL;

    /* Check request size */

    if(req->ios2_Req.io_Message.mn_Length < sizeof(struct IOSana2Req))
        error = IOERR_OPENFAIL;

    /* Get the requested unit */
    if((error == 0) && (unitnum == 0))
    {
        req->ios2_Req.io_Unit = (APTR)unit;
    }
    else error = IOERR_OPENFAIL;

    /* Handle device sharing */

    if(error == 0)
    {
        if(unit->rtl8139u_open_count != 0 && ((unit->rtl8139u_flags & IFF_SHARED) == 0 ||
            (flags & SANA2OPF_MINE) != 0))
            error = IOERR_UNITBUSY;
        unit->rtl8139u_open_count++;
    }

    if(error == 0)
    {
        if((flags & SANA2OPF_MINE) == 0)
        unit->rtl8139u_flags |= IFF_SHARED;
        else if((flags & SANA2OPF_PROM) != 0)
        unit->rtl8139u_flags |= IFF_PROMISC;

        /* Set up buffer-management structure and get hooks */
        opener = AllocVec(sizeof(struct Opener), MEMF_PUBLIC | MEMF_CLEAR);
        req->ios2_BufferManagement = (APTR)opener;

        if(opener == NULL)
        error = IOERR_OPENFAIL;
    }

    if(error == 0)
    {
        NEWLIST(&opener->read_port.mp_MsgList);
        opener->read_port.mp_Flags = PA_IGNORE;
        NEWLIST((APTR)&opener->initial_stats);

        for(i = 0; i < 2; i++)
            opener->rx_function = (APTR)GetTagData(rx_tags[i], (IPTR)opener->rx_function, tags);
        for(i = 0; i < 3; i++)
            opener->tx_function = (APTR)GetTagData(tx_tags[i], (IPTR)opener->tx_function, tags);

        opener->filter_hook = (APTR)GetTagData(S2_PacketFilter, 0, tags);

        Disable();
        AddTail((APTR)&unit->rtl8139u_Openers, (APTR)opener);
        Enable();
    }

    if (error != 0)
        CloseDevice((struct IORequest *)req);
    else
        unit->start(unit);

    req->ios2_Req.io_Error = error;

    return (error !=0) ? FALSE : TRUE;
}

static int GM_UNIQUENAME(Close)
(
    LIBBASETYPEPTR LIBBASE,
    struct IOSana2Req* req
)
{
    struct RTL8139Unit *unit = LIBBASE->rtl8139b_unit;
    struct Opener *opener;

D(bug("[RTL8139] init.CloseDevice\n"));

    unit->stop(unit);

    opener = (APTR)req->ios2_BufferManagement;
    if (opener != NULL)
    {
        Disable();
        Remove((struct Node *)opener);
        Enable();
        FreeVec(opener);
    }

    return TRUE;
}


ADD2INITLIB(GM_UNIQUENAME(Init),0)
ADD2EXPUNGELIB(GM_UNIQUENAME(Expunge),0)
ADD2OPENDEV(GM_UNIQUENAME(Open),0)
ADD2CLOSEDEV(GM_UNIQUENAME(Close),0)

AROS_LH1(void, beginio,
    AROS_LHA(struct IOSana2Req *, req, A1),
    LIBBASETYPEPTR, LIBBASE, 5, RTL8139Dev)
{
    AROS_LIBFUNC_INIT
    struct RTL8139Unit *dev;

D(bug("[RTL8139] init.BeginIO\n"));

    req->ios2_Req.io_Error = 0;
    dev = (APTR)req->ios2_Req.io_Unit;

    if (AttemptSemaphore(&dev->rtl8139u_unit_lock))
    {
        handle_request(LIBBASE, req);
    }
    else
    {
        req->ios2_Req.io_Flags &= ~IOF_QUICK;
        PutMsg(dev->rtl8139u_input_port, (struct Message *)req);
    }

    AROS_LIBFUNC_EXIT
}

AROS_LH1(LONG, abortio,
    AROS_LHA(struct IOSana2Req *, req, A1),
    LIBBASETYPEPTR, LIBBASE, 5, RTL8139Dev)
{
    AROS_LIBFUNC_INIT
    struct RTL8139Unit *dev;
    dev = (APTR)req->ios2_Req.io_Unit;

D(bug("[RTL8139] init.AbortIO\n"));

    Disable();
    if ((req->ios2_Req.io_Message.mn_Node.ln_Type == NT_MESSAGE) &&
        (req->ios2_Req.io_Flags & IOF_QUICK) == 0)
    {
        Remove((struct Node *)req);
        req->ios2_Req.io_Error = IOERR_ABORTED;
        req->ios2_WireError = S2WERR_GENERIC_ERROR;
        ReplyMsg((struct Message *)req);
    }
    Enable();

    return 0;

    AROS_LIBFUNC_EXIT
}

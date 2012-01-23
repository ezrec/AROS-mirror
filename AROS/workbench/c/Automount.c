/*
    Copyright © 1995-2011, The AROS Development Team. All rights reserved.
    $Id: $
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <aros/debug.h>
#include <dos/dos.h>
#include <dos/rdargs.h>
#include <exec/lists.h>
#include <exec/memory.h>
#include <libraries/expansion.h>
#include <libraries/expansionbase.h>
#include <devices/trackdisk.h>
#include <proto/alib.h>
#include <proto/arossupport.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/expansion.h>
#include <proto/utility.h>

#define MAX_UNITS       8

struct HandlerNode
{
    struct MinNode n;
    ULONG          id;
    ULONG          mask;
    char           handler[1];
};

struct MinList handlerlist;
APTR pool;

static struct HandlerNode *FindHandler(ULONG id)
{
    struct HandlerNode *n;

    ForeachNode(&handlerlist, n)
    {
        if (n->id == (id & n->mask))
            return n;
    }
    
    return NULL;
}

static BOOL IsMounted(struct DeviceNode *dn)
{
    BOOL ret = FALSE;
    struct DosList *dl = LockDosList(LDF_DEVICES|LDF_READ);
    
    while ((dl = NextDosEntry(dl, LDF_DEVICES)))
    {
    	if (dl == (struct DosList *)dn)
    	{
    	    ret = TRUE;
    	    break;
    	}
    }

    UnLockDosList(LDF_DEVICES|LDF_READ);
    return ret;
}

/* Create a new device node */
static struct DeviceNode *newDeviceNode(const char *name, ULONG unit, struct IORequest *io)
{
    return NULL;
}

/* Mount (if needed) a DeviceNode as a Dos Device
 */
static ULONG mountDeviceNode(struct DeviceNode *dn, ULONG bn_Flags)
{
    D(bug("[Automount] Checking BootNode %b...\n", dn->dn_Name));

    if ((!dn->dn_Task) && (!dn->dn_SegList) && (!dn->dn_Handler) && dn->dn_Startup)
    {
        struct FileSysStartupMsg *fssm = BADDR(dn->dn_Startup);

        D(bug("[Automount] Not mounted\n"));

        if (fssm->fssm_Environ)
        {
            struct DosEnvec *de = BADDR(fssm->fssm_Environ);

            if (de)
            {
                struct HandlerNode *hn = FindHandler(de->de_DosType);

                if (hn)
                {
                    Printf("Mounting %b with %s\n", dn->dn_Name, hn->handler);

                    dn->dn_Handler = CreateBSTR(hn->handler);
                    if (!dn->dn_Handler)
                    {
                        return ERROR_NO_FREE_STORE;
                    }

                    if (!IsMounted(dn))
                    {
                        D(bug("[Automount] Adding DOS entry...\n"));
                        AddDosEntry((struct DosList *)dn);
                    }

                    if (bn_Flags & ADNF_STARTPROC)
                    {
                        char *buf;
                        ULONG res;

                        D(bug("[Automount] Starting up...\n"));

                        res = AROS_BSTR_strlen(dn->dn_Name);
                        buf = AllocMem(res + 2, MEMF_ANY);
                        if (!buf)
                        {
                            return ERROR_NO_FREE_STORE;
                        }

                        CopyMem(AROS_BSTR_ADDR(dn->dn_Name), buf, res);
                        buf[res++] = ':';
                        buf[res++] = 0;

                        DeviceProc(buf);
                        FreeMem(buf, res);
                    }
                }
            }
        }
    }

    return RETURN_OK;
}

static BOOL unmountDeviceNode(struct DeviceNode *dn)
{
    return FALSE;
}

#define IOStdReq(io)    ((struct IOStdReq *)(io))

static struct IORequest *openUnit(const char *name, ULONG unit)
{
    struct MsgPort *mp;
    
    if ((mp = CreateMsgPort())) {
        struct IORequest *io;
        if ((io = CreateIORequest(mp, sizeof(struct IOStdReq)))) {
            if (0 == OpenDevice(name, unit, io, 0)) {
                return io;
            }
            DeleteIORequest(io);
        }
        DeleteMsgPort(mp);
    }

    return NULL;
}

static void closeUnit(struct IORequest *io)
{
    struct MsgPort *mp = io->io_Message.mn_ReplyPort;
    CloseDevice(io);
    DeleteIORequest(io);
    DeleteMsgPort(mp);
}

/* We only care about removable drives, so
 * TD_CHANGENUM should be sufficient.
 */
BOOL isDrive(struct IORequest *io)
{
    io->io_Command = TD_CHANGENUM;
    return (0 == DoIO(io));
}

BOOL delayOrDie(ULONG sec)
{
        Delay(sec * 50);      /* Rescan every second */
        return FALSE;         /* Not dead yet! */
}

void monitorUnit(CONST_STRPTR device, IPTR unit)
{
    ULONG ui_ChangeNum = 0;
    struct DeviceNode *ui_DeviceNode = NULL;
    struct IORequest *io;

    if ((io = openUnit(device, unit))) {
        /* Scan for changed changenumbers, and remount if needed. */
        do {
            io->io_Command = TD_CHANGENUM;
            if (0 == DoIO(io)) {
                ULONG changenum = IOStdReq(io)->io_Actual;
                if (changenum != ui_ChangeNum) {
                    io->io_Command = TD_CHANGESTATE;
                    if (0 == DoIO(io)) {
                        ULONG changestate = IOStdReq(io)->io_Actual;
                        if (0 == changestate && ui_DeviceNode == NULL) {
                            /* Newly injected */
                            ui_DeviceNode = newDeviceNode(device, unit, io);
                            if (ui_DeviceNode)
                                mountDeviceNode(ui_DeviceNode, 0);
                        } else if (0 != changestate && ui_DeviceNode != NULL) {
                            /* Newly ejected */
                            if (unmountDeviceNode(ui_DeviceNode))
                                ui_DeviceNode = NULL;
                        }
                    }
                }
            }
        } while (!delayOrDie(1));
        closeUnit(io);
    }
}

static LONG parsePrefs(char *buffer, LONG size)
{
    struct CSource csrc = {buffer, size, 0};
    char ident[256];
    LONG res;
    WORD line = 1;

    D(bug("[Automount] parsePrefs()\n"));

    while (csrc.CS_CurChr < csrc.CS_Length)
    {
        struct HandlerNode *tn;
        int i;
        char *p;
        ULONG id = 0;
        ULONG mask = 0xFFFFFFFF;

        DB2(bug("[parsePrefs] Cur %d, Length %d\n", csrc.CS_CurChr, csrc.CS_Length));

        res = ReadItem(ident, 256, &csrc);
        switch (res)
        {
        case ITEM_ERROR:
            return IoErr();

        case ITEM_UNQUOTED:
            if (ident[0] == '#')
            {
            	/* Skip over to the end of line */
                while ((csrc.CS_CurChr < csrc.CS_Length) && (buffer[csrc.CS_CurChr] != '\n'))
		    csrc.CS_CurChr++;

                goto next_line;
            }
            /* Fall through */
        case ITEM_QUOTED:
            p = ident;
            if (Stricmp(ident, "monitor")) {
                res = ReadItem(ident, 256, &csrc);
                if (res == ITEM_UNQUOTED || res == ITEM_QUOTED) {
                    char device[256];
                    char *p = &ident[strlen(ident)];
                    int i;
                    strcpy(device, ident);
                    for (i = 0; i < 256; i++) {
                        snprintf(p, (&ident[sizeof(ident)])-p, " %d [auto]", i);
                        ident[sizeof(ident)-1]=0;
                        NewCreateTask(TASKTAG_NAME, ident,
                                      TASKTAG_PC, monitorUnit,
                                      TASKTAG_ARG1, device,
                                      TASKTAG_ARG2, i,
                                      TAG_END);
                    }
                    break;
                } else {
                    Printf("LINE %ld: Malformed MONITOR line\n", line);
                    return -1;
                }
            }
                
            for (i = 0; i < 4; i++)
            { 
                UBYTE c;

                if (!*p)
                {
                    Printf("LINE %ld: Malformed filesystem ID\n", line);
                    return -1;
                }

                c = *p++;
                switch (c)
                {
                case '\\':
                    c = strtoul(p, &p, 16);
                    break;

                case '?':
                    mask &= ~(0xFF000000 >> (i << 3));
                    c = 0;
                }

                id <<= 8;
                id |= c;
            }
            break;

        default:
            Printf("LINE %ld: Missing filesystem ID\n", line);
            return -1;
        }

        res = ReadItem(ident, 256, &csrc);
        if (res == ITEM_ERROR)
            return IoErr();

        if (res != ITEM_EQUAL)
        {
            Printf("LINE %ld: Unexpected item after filesystem ID\n", line);
            return -1;
        }

        res = ReadItem(ident, 256, &csrc);
        if (res == ITEM_ERROR)
            return IoErr();

        if ((res != ITEM_QUOTED) && (res != ITEM_UNQUOTED))
        {
            Printf("LINE %ld: Missing handler name\n", line);
            return -1;
        }

        res = strlen(ident);
        tn = AllocPooled(pool, sizeof(struct HandlerNode) + res);
        if (tn == NULL)
	    return ERROR_NO_FREE_STORE;
	
	tn->id   = id;
	tn->mask = mask;
        CopyMem(ident, tn->handler, res + 1);

        AddTail((struct List *)&handlerlist, (struct Node *)tn);

next_line:
	/*
	 * Intentional ReadItem() bug workaround.
	 * Ungets '\n' every time, causing an infinite loop without this adjustment.
	 */
        if ((csrc.CS_CurChr < csrc.CS_Length) && (buffer[csrc.CS_CurChr] == '\n'))
        {
            line++;
	    csrc.CS_CurChr++;
	}
    }

    return 0;
}


static LONG LoadPrefs(STRPTR filename) 
{
    struct FileInfoBlock fib;
    char *buffer;
    LONG retval = 0;
    LONG size;
    BPTR fh;

    D(bug("[Automount] LoadPrefs('%s')\n", filename));

    fh = Open(filename, MODE_OLDFILE);
    if (fh)
    {
        if (ExamineFH(fh, &fib))
        {
            if (fib.fib_Size>0)
            {
                buffer = AllocMem(fib.fib_Size, MEMF_PUBLIC | MEMF_CLEAR);
                if (buffer)
                {
                    size = Read(fh, buffer, fib.fib_Size);
                    if (size == fib.fib_Size)
                        retval = parsePrefs(buffer, size);
                    else
                    	retval = IoErr();

                    FreeMem(buffer, fib.fib_Size);
                }
                else
                    retval = ERROR_NO_FREE_STORE;
            }
        }
        else
            retval = IoErr();

        Close(fh);
    }
    
    return retval;
}



/* Needed for ReadArgs() to work properly on some systems */
int __nocommandline = 1;

int main(void)
{
    LONG res;
    struct RDArgs *ra;
    struct {
        IPTR isVerbose;
    } Args = {};

    pool = CreatePool(1024, 1024, MEMF_ANY);
    if (!pool)
    {
        PrintFault(ERROR_NO_FREE_STORE, "Automount");
        return RETURN_FAIL;
    }

    ra = ReadArgs("VERBOSE/S", (IPTR *)&Args, NULL);

    NewList((struct List *)&handlerlist);
    res = LoadPrefs("L:automount-config");

    if (res == 0)
    {
        struct BootNode *n;

        ForeachNode(&((struct ExpansionBase *)ExpansionBase)->MountList, n) {
            res = mountDeviceNode(n->bn_DeviceNode, n->bn_Flags);
            if (res)
                break;
        }
    }
    else if (res != -1) {
    	PrintFault(res, "Automount");
    	return res;
    }


    FreeArgs(ra);
    DeletePool(pool);
    return RETURN_OK;
}

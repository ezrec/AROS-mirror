#include <aros/symbolsets.h>
#include <aros/bootloader.h>
#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/bootloader.h>

#define DEBUG 0
#include <aros/debug.h>

#define TARGET_DIR  "EMU:boot-stage-2/"
#define AROS_BOOT   "AROS.boot"
#define AROSBS_BIN  "AROSBootstrap"
#define AROSBS_CONF "AROSBootstrap.conf"
#define BUFFER_SIZE (64 * 1024)
#define PATHBUFSIZE (128)

static VOID buildPath(CONST_STRPTR volume, CONST_STRPTR filename, STRPTR buffer, LONG size)
{
    __sprintf(buffer, "%s", volume);
    AddPart(buffer, filename, size);
}

static STRPTR createPath(STRPTR volume, STRPTR filename)
{
    STRPTR _return = AllocVec(PATHBUFSIZE, MEMF_ANY);
    buildPath(volume, filename, _return, PATHBUFSIZE);
    return _return;
}

static BOOL createDirStructureIfNeeded(STRPTR dir)
{
    BPTR l;
    STRPTR p = dir;

    while(*(++p) != ':');

    while(TRUE)
    {
        while(*(++p) != '/')
        {
            if (*p == '\0') return TRUE;
        }

        *p = '\0';
        l = Lock(dir, SHARED_LOCK);
        if (l == BNULL)
        {
            D(bug("Creating directory %s\n", dir));
            l = CreateDir(dir);
        }
        if (l == BNULL)
            return FALSE;
        UnLock(l);
        *p = '/';
    }

    return TRUE;
}

static BOOL copyFile(STRPTR source, STRPTR destination)
{
    BPTR src, dst;
    APTR buffer = NULL;
    LONG read = 0;


    src = Open(source, MODE_OLDFILE);
    if (src == BNULL)
        return FALSE;
    dst = Open(destination, MODE_NEWFILE);
    if (dst == BNULL)
    {
        Close(src);
        return FALSE;
    }

    buffer = AllocMem(BUFFER_SIZE, MEMF_ANY);

    do
    {
        read = Read(src, buffer, BUFFER_SIZE);
        Write(dst, buffer, read);
    } while(read == BUFFER_SIZE);

    FreeMem(buffer, BUFFER_SIZE);
    Close(dst);
    Close(src);

    return TRUE;
}

static BOOL copyBootFiles(STRPTR * bootfiles)
{
    STRPTR file = NULL;
    STRPTR * p = bootfiles;

    while((file = *p++) != NULL)
    {
        STRPTR p = file;
        TEXT dstpath[PATHBUFSIZE];

        while(*(p++) != ':');

        buildPath(TARGET_DIR, p, dstpath, PATHBUFSIZE);

        createDirStructureIfNeeded(dstpath);

        D(bug("Copying: %s to %s\n", file, dstpath));

        if (!copyFile(file, dstpath))
        {
            D(bug("Copying FAILED: %s to %s\n", file, dstpath));
            return FALSE;
        }

    }

    return TRUE;
}

static STRPTR * findBootFiles(STRPTR volume, STRPTR archdir)
{
    STRPTR * bootfiles = NULL;
    BPTR l; LONG i = 0;
    TEXT buffer[PATHBUFSIZE];
    TEXT arosbsbinpath[PATHBUFSIZE];
    TEXT arosbsconfpath[PATHBUFSIZE];

    /* Skip EMU */
    if (volume[0] == 'E' && volume[1] == 'M' && volume[2] == 'U')
        return NULL;

    /* Only if volume has AROS.boot */
    buildPath(volume, AROS_BOOT, buffer, PATHBUFSIZE);
    l = Lock(buffer, SHARED_LOCK);

    if (l == BNULL)
        return NULL;

    UnLock(l);

    /* Only if volume has AROSBootstrap.conf */
    __sprintf(arosbsbinpath, "%s/%s", archdir, AROSBS_BIN);
    __sprintf(arosbsconfpath, "%s/%s", archdir, AROSBS_CONF);
    buildPath(volume, arosbsconfpath, buffer, PATHBUFSIZE);
    l = Open(buffer, MODE_OLDFILE);

    if (l == BNULL)
        return NULL;

    bootfiles = AllocVec(sizeof(STRPTR) * 64, MEMF_ANY | MEMF_CLEAR);

    bootfiles[i++] = createPath(volume, arosbsbinpath);
    bootfiles[i++] = createPath(volume, arosbsconfpath);

    /* Naive parsing of configuration file in search of modules to copy */
    while(FGets(l, buffer, PATHBUFSIZE))
    {
        STRPTR p = buffer;

        if (p[0] == '#') continue;

        while (*p != '\n' && *p != '\r' && *p !='\0') { p++;}
        *p = '\0';

        p = buffer;

        while (*p != '\0')
        {
            if (Strnicmp(p, "module", 6) == 0)
            {
                D(bug("Found module: %s\n", p + 7));
                bootfiles[i++] = createPath(volume, p + 7);
                break;
            }
            p++;
        }

    }

    bootfiles[i++] = NULL;

    Close(l);

    return bootfiles;
}

static VOID freeBootFiles(STRPTR * bootfiles)
{
    STRPTR file = NULL;
    STRPTR * p = bootfiles;

    while((file = *p++) != NULL)
        FreeVec(file);

    FreeVec(bootfiles);
}

struct MyBootNode
{
    struct Node mbn_Node;
    TEXT        mbn_Name[108];
};

static int ARIXLoaderInit(APTR Base)
{
    /*
     * 1. Scan available volumes in search of boot files
     * 2. If found, copy them (list taken from AROSBootstrap.conf) to initrd
     * 3. Exit
     */

    struct List bootList;
    STRPTR archdir = "";
    struct DosList *dl;
    APTR BootLoaderBase;

    NEWLIST(&bootList);

    D(bug("ARIX Loader\n"));

    /* Get information on arch directory */
    BootLoaderBase = OpenResource("bootloader.resource");
    if (BootLoaderBase)
    {
        struct Node * node;
        struct List * list = (struct List *)GetBootInfo(BL_Args);
        if (list)
        {
            ForeachNode(list, node)
            {
                if (strncmp(node->ln_Name,"archdir=", 8) == 0)
                {
                    archdir = &node->ln_Name[8];
                    break;
                }
            }
        }
    }

    D(bug("Found ARCH_DIR = %s\n", archdir));


    /* Look for possible boot devices */
    dl = LockDosList(LDF_DEVICES | LDF_READ);

    while((dl = NextDosEntry(dl, LDF_DEVICES | LDF_READ)) != NULL)
    {
        if(dl->dol_Type == DLT_DEVICE)
        {
            struct MyBootNode *mbn = AllocMem(sizeof(struct MyBootNode), MEMF_CLEAR);

            __sprintf(mbn->mbn_Name, "%b:", dl->dol_Name);

            if (strncasecmp("CD", mbn->mbn_Name, 2) == 0)
                mbn->mbn_Node.ln_Pri = 10;
            else if (strncasecmp("SD", mbn->mbn_Name, 2) == 0)
                mbn->mbn_Node.ln_Pri = 5;
            else if (strncasecmp("MMC", mbn->mbn_Name, 3) == 0)
                mbn->mbn_Node.ln_Pri = 6;
            else
                mbn->mbn_Node.ln_Pri = 0;

            Enqueue(&bootList, (struct Node*)mbn);

            D(bug("Found name %s\n", mbn->mbn_Name));
        }
    }

    if (!IsListEmpty(&bootList))
    {
        struct MyBootNode *mbn;
        STRPTR * bootfiles = NULL;

        ForeachNode(&bootList, mbn)
        {
            if ((bootfiles = findBootFiles(mbn->mbn_Name, archdir)) != NULL)
            {
                if (copyBootFiles(bootfiles))
                {
                    freeBootFiles(bootfiles);
                    break;
                }

                freeBootFiles(bootfiles);
            }
        }
    }

    UnLockDosList(LDF_DEVICES | LDF_READ);

    ShutdownA(SD_ACTION_POWEROFF);

    return TRUE;
}

ADD2INITLIB(ARIXLoaderInit, 0);

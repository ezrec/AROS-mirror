#include <aros/symbolsets.h>
#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>

#define DEBUG 0
#include <aros/debug.h>

#define TARGET_DIR  "EMU:boot-stage-2/"
#define AROS_BOOT   "AROS.boot"
#define AROSBS_CONF "boot/AROSBootstrap.conf"
#define BUFFER_SIZE (256 * 1024)

static VOID createPath2(STRPTR volume, STRPTR filename, STRPTR buffer, LONG size)
{
    __sprintf(buffer, "%s", volume);
    AddPart(buffer, filename, size);
}

static STRPTR createPath(STRPTR volume, STRPTR filename)
{
    STRPTR _return = AllocVec(128, MEMF_ANY);
    createPath2(volume, filename, _return, 128);
    return _return;
}

static BOOL createDirIfNeeded(STRPTR dir)
{
    BPTR l;

    l = Lock(dir, SHARED_LOCK);
    if (l == BNULL)
    {
        D(bug("Creating directory %s\n", dir));
        l = CreateDir(dir);
    }
    if (l == BNULL)
        return FALSE;
    UnLock(l);

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
        read = FRead(src, buffer, 1, BUFFER_SIZE);
        FWrite(dst, buffer, 1, read);
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

    /* First, create needed directory */
    createDirIfNeeded(TARGET_DIR);

    while((file = *p++) != NULL)
    {
        STRPTR p= file;
        TEXT dstpath[128];

        while(*(p++) != ':');

        createPath2(TARGET_DIR, p, dstpath, 128);

        /* Works only with one level deep directories */
        p = dstpath;
        while (*(++p) != '/');
        while (*(++p) != '/');
        *p = '\0';
        createDirIfNeeded(dstpath);
        *p = '/';

        D(bug("Copying: %s to %s\n", file, dstpath));

        if (!copyFile(file, dstpath))
            return FALSE;

    }

    return TRUE;
}

#define BUFSIZE (128)

static STRPTR * findBootFiles(STRPTR volume)
{
    STRPTR * bootfiles = NULL;
    BPTR l; LONG i = 0;
    TEXT buffer[BUFSIZE];

    /* Skip EMU */
    if (volume[0] == 'E' && volume[1] == 'M' && volume[2] == 'U')
        return NULL;

    /* Only if volume has AROS.boot */
    createPath2(volume, AROS_BOOT, buffer, BUFSIZE);
    l = Lock(buffer, SHARED_LOCK);

    if (l == BNULL)
        return NULL;

    UnLock(l);

    /* Only if volume has AROSBootstrap.conf */
    createPath2(volume, AROSBS_CONF, buffer, BUFSIZE);
    l = Open(buffer, MODE_OLDFILE);

    if (l == BNULL)
        return NULL;

    bootfiles = AllocVec(sizeof(STRPTR) * 64, MEMF_ANY | MEMF_CLEAR);

    bootfiles[i++] = createPath(volume, "boot/AROSBootstrap");
    bootfiles[i++] = createPath(volume, "boot/AROSBootstrap.conf");

    /* Naive parsing of configuration file in search of modules to copy */
    while(FGets(l, buffer, BUFSIZE))
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

static int ARIXLoaderInit(APTR Base)
{

    /*
     * 1. Scan available volumes in search of boot files
     * 2. If found, copy them (list taken from AROSBootstrap.conf) to initrd
     * 3. Exit
     */

    struct DosList *dl;

    D(bug("ARIX Loader\n"));

    dl = LockDosList(LDF_DEVICES | LDF_READ);

    while((dl = NextDosEntry(dl, LDF_DEVICES | LDF_READ)) != NULL)
    {
        if(dl->dol_Type == DLT_DEVICE)
        {
            TEXT name[108];
            STRPTR * bootfiles = NULL;

            __sprintf(name, "%b:", dl->dol_Name);

            D(bug("Found name %s\n", name));

            if ((bootfiles = findBootFiles(name)) != NULL)
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

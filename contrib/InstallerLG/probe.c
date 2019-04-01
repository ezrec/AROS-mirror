//----------------------------------------------------------------------------
// probe.c:
//
// Environment information retrieval
//----------------------------------------------------------------------------
// Copyright (C) 2018, Ola Söder. All rights reserved.
// Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
//----------------------------------------------------------------------------

#include "alloc.h"
#include "error.h"
#include "eval.h"
#include "probe.h"
#include "util.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#ifdef AMIGA
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <exec/resident.h>
# ifdef __MORPHOS__
#  include <exec/system.h>
# endif
# ifdef __AROS__
#  include <proto/processor.h>
#  include <resources/processor.h>
# endif
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/icon.h>
#include <workbench/workbench.h>
#endif

//----------------------------------------------------------------------------
// Name:        h_cpu_name
// Description: Helper for m_database. Get host CPU architecture.
// Input:       -
// Return:      char *: Host CPU architecture.
//----------------------------------------------------------------------------
static char *h_cpu_name(void)
{
    enum { ERR, PPC, ARM, M68000, M68010, M68020,
           M68030, M68040, M68060, X86, X86_64, ALL };

    static char *cpu[ALL] = { "Unknown", "PowerPC", "ARM",
                              "68000", "68010", "68020", "68030",
                              "68040", "68060", "x86", "x84_64" };
    uint32_t arc = ERR;

    #ifdef __MORPHOS__
    // On MorphOS, there is only PPC (for now) (and no define).
    NewGetSystemAttrs(&arc, sizeof(arc), SYSTEMINFOTYPE_MACHINE);
    arc = arc == 1 ? PPC : ERR;
    #else
    #ifdef __AROS__
    // On AROS, everything is possible.
    APTR ProcessorBase = OpenResource("processor.resource");

    if(ProcessorBase)
    {
        ULONG fam;

        struct TagItem tags[] =
        {
            { GCIT_Family, (IPTR) &fam },
            { TAG_DONE, TAG_DONE }
        };

        GetCPUInfo(tags);

        switch(fam)
        {
            case CPUFAMILY_60X:
            case CPUFAMILY_7X0:
            case CPUFAMILY_74XX:
            case CPUFAMILY_4XX:
               arc = PPC;
               break;

            case CPUFAMILY_ARM_3:
            case CPUFAMILY_ARM_4:
            case CPUFAMILY_ARM_4T:
            case CPUFAMILY_ARM_5:
            case CPUFAMILY_ARM_5T:
            case CPUFAMILY_ARM_5TE:
            case CPUFAMILY_ARM_5TEJ:
            case CPUFAMILY_ARM_6:
            case CPUFAMILY_ARM_7:
               arc = ARM;
               break;

            case CPUFAMILY_MOTOROLA_68000:
               arc = M68000;
               break;

            case CPUFAMILY_AMD_K5:
            case CPUFAMILY_AMD_K6:
            case CPUFAMILY_AMD_K7:
            case CPUFAMILY_INTEL_486:
            case CPUFAMILY_INTEL_PENTIUM:
            case CPUFAMILY_INTEL_PENTIUM_PRO:
            case CPUFAMILY_INTEL_PENTIUM4:
               arc = X86;
               break;

            case CPUFAMILY_AMD_K8:
            case CPUFAMILY_AMD_K9:
            case CPUFAMILY_AMD_K10:
               arc = X86_64;
               break;

            }
    }
    #else
    #ifdef AMIGA
    // AmigaOS3 - Beware, not tested.
    struct ExecBase *AbsSysBase = *((struct ExecBase **)4);
    UWORD flags = AbsSysBase->AttnFlags;

    if(flags & AFF_68010)
    {
        arc = M68010;
    }
    else if(flags & AFF_68020)
    {
        arc = M68020;
    }
    else if(flags & AFF_68030)
    {
        arc = M68030;
    }
    else if(flags & AFF_68040)
    {
        arc = M68040;
    }
    else if(flags & AFF_68060)
    {
        arc = M68060;
    }
    else
    {
        arc = M68000;
    }
    #endif
    #endif
    #endif

    // CPU or 'Unknown'.
    return cpu[arc];
}

//----------------------------------------------------------------------------
// (database <feature> [<checkvalue>])
//    return information about the Amiga that the Installer
//    is running on.
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_database(entry_p contxt)
{
    // We need atleast one argument
    if(c_sane(contxt, 1))
    {
        int memf = -1;
        char *feat = str(CARG(1)),
              *ret = "Unknown";

        if(!strcmp(feat, "cpu"))
        {
            // Get host CPU name.
            ret = h_cpu_name();
        }
        else
        if(!strcmp(feat, "os"))
        {
            // Get OS name.
            #ifdef AMIGA
            if(FindResident("MorphOS"))
            {
                ret = "MorphOS";
            }
            else if(FindResident("processor.resource"))
            {
                ret = "AROS";
            }
            else
            {
                ret = "AmigaOS";
            }
            #else
            ret = "Unknown";
            #endif

        }
        else
        if(!strcmp(feat, "graphics-mem"))
        {
            memf =
            #ifdef AMIGA
            AvailMem(MEMF_CHIP);
            #else
            524288;
            #endif
        }
        else
        if(!strcmp(feat, "total-mem"))
        {
            memf =
            #ifdef AMIGA
            AvailMem(MEMF_ANY);
            #else
            524288;
            #endif
        }

        if(memf != -1)
        {
            ret = get_buf();
            snprintf(get_buf(), buf_size(), "%d", memf);
        }

        // Are we testing for a specific value?
        if(CARG(2) && CARG(2) != end())
        {
            ret = strcmp(ret, str(CARG(2))) ? "0" : "1";
        }

        RSTR(DBG_ALLOC(strdup(ret)));
    }

    // The parser is broken.
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// (earlier <file1> <file2>)
//     true if file1 earlier than file2
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_earlier(entry_p contxt)
{
    // We need two filenames.
    if(c_sane(contxt, 2))
    {
        // One struct per file.
        struct stat old, new;

        // Get information about the first file.
        if(stat(str(CARG(1)), &old))
        {
            // Could not read from file / dir.
            ERR(ERR_READ, str(CARG(1)));
            RNUM(0);
        }

        // Get information about the second file.
        if(stat(str(CARG(2)), &new))
        {
            // Could not read from file / dir.
            ERR(ERR_READ, str(CARG(2)));
            RNUM(0);
        }

        // Is the first older than the second one?
        RNUM
        (
            old.st_mtime < new.st_mtime ? 1 : 0
        );
    }

    // The parser is broken
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// (getassign <name> <opts>)
//     return value of logical name (no `:') `<opts>': 'v'-volumes,
//     'a'-assigns, 'd'-devices
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_getassign(entry_p contxt)
{
    // We need one or more arguments.
    if(c_sane(contxt, 1))
    {
        // On non Amiga systems this is a stub.
        #ifdef AMIGA
	    struct DosList *dl;
        const char *asn = str(CARG(1));
        size_t asnl = strlen(asn);

        if(!asnl)
        {
            // Invalid name of assign, and
            // the empty string is how the
            // CBM installer fails.
            REST;
        }

        // The second argument is optional.
        entry_p opt = CARG(2);

        // The bitmask must contain atleast
        // this LDF_READ.
        ULONG msk = LDF_READ;

        // Parse the option string if it exists.
        if(opt && opt != end())
        {
            const char *o = str(opt);

            if(*o)
            {
                // Translate from Installer speak to DOS
                // speak.
                while(*o)
                {
                    msk |= (*o == 'v') ? LDF_VOLUMES : 0;
                    msk |= (*o == 'd') ? LDF_DEVICES : 0;
                    msk |= (*o == 'a') ? LDF_ASSIGNS : 0;
                    o++;
                }
            }
            else
            {
                // The CBM installer returns an empty
                // string if option string is empty.
                REST;
            }
        }
        else
        // The default behaviour, when no option string is
        // present, is to look for logical assignments.
        {
            msk |= LDF_ASSIGNS;
        }

        // Prepare to walk the device list.
	    dl = (struct DosList *) LockDosList(msk);

        if(dl)
        {
            ULONG bits[] =
            {
                LDF_ASSIGNS,
                LDF_VOLUMES,
                LDF_DEVICES,
                0
            };

            // Iterate over all flags.
            for(size_t i = 0;
                bits[i]; i++)
            {
                // For all flags matching the option
                // string (or its absence), traverse
                // the doslist.
                if(msk & bits[i])
                {
                    struct DosList *dc = (struct DosList *)
                        NextDosEntry(dl, bits[i]);

                    while(dc)
                    {
                        const char *n = B_TO_CSTR(dc->dol_Name);

                        // Ignore case when looking for match.
                        if(!strcasecmp(asn, n))
                        {
                            // Unlock doslist and allocate enough
                            // memory to hold any path.
                            char *r = DBG_ALLOC(calloc(PATH_MAX, 1));
	                        UnLockDosList(msk);

                            if(r)
                            {
                                // The form common to all types. Do we
                                // need to do anything with LDF_VOLUMES?
                                snprintf(r, PATH_MAX, "%s:", n);

                                // Logical assignments. Get the full path
                                // from the lock.
                                if(bits[i] == LDF_ASSIGNS)
                                {
                                    BPTR l = (BPTR) Lock(r, ACCESS_READ);

                                    if(l)
                                    {
                                        NameFromLock(l, r, PATH_MAX);
                                        UnLock(l);
                                    }
                                }
                                // Devices. No other options than 'd' are
                                // allowed in the options string for some
                                // reason (in the CBM installer).
                                else if(bits[i] == LDF_DEVICES)
                                {
                                    if((bits[i] | LDF_READ) == msk)
                                    {
                                        // Cut ':'.
                                        r[asnl] = '\0';
                                    }
                                    else
                                    {
                                        // Clear.
                                        r[0] = '\0';
                                    }
                                }

                                // Success.
                                RSTR(r);
                            }
                            else
                            {
                                // Out of memory
                                PANIC(contxt);
                                REST;
                            }
                        }
                        else
                        {
                            // No match. Next list item.
                            dc = (struct DosList *)
                                NextDosEntry(dc, bits[i]);
                        }
                    }
                }
            }

            // Not found.
	        UnLockDosList(msk);
        }
        else
        {
            // Could not lock doslist. I'm not
            // sure this can happen. I believe
            // LockDosList will block until the
            // end of time if a problem occurs.
            ERR(ERR_READ, asn);
        }
        #endif

        // Return empty string
        // on failure.
        REST;
    }

    // Broken parser.
    RCUR;
}

//----------------------------------------------------------------------------
// (getdevice <path>)
//     returns name of device upon which <path> resides
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_getdevice(entry_p contxt)
{
    // We need a path.
    if(c_sane(contxt, 1))
    {
        #ifdef AMIGA
        // Attempt to lock path.
        BPTR lock = (BPTR) Lock(str(CARG(1)), ACCESS_READ);

        if(lock)
        {
            struct InfoData id;

            // Get vol info from file / dir lock.
            if(Info(lock, &id))
            {
                struct DosList *dl = (struct DosList *) id.id_VolumeNode;
                UnLock(lock);

                if(dl)
                {
                    struct MsgPort *mp = dl->dol_Task;
                    ULONG msk = LDF_READ | LDF_DEVICES;

                    // Search for <path> handler in the
                    // list of devices.
                    dl = (struct DosList *) LockDosList(msk);

                    while(dl && mp != dl->dol_Task)
                    {
                        dl = (struct DosList *)
                            NextDosEntry(dl, LDF_DEVICES);
                    }

                    UnLockDosList(msk);

                    // If we found it, we also found the
                    // name of the device.
                    if(dl)
                    {
                        const char *n = B_TO_CSTR(dl->dol_Name);

                        // strdup(NULL) is undefined.
                        if(n)
                        {
                            RSTR(DBG_ALLOC(strdup(n)));
                        }
                        else
                        {
                            // Unknown error.
                            PANIC(contxt);
                        }
                    }
                }
            }
            else
            {
                UnLock(lock);
            }
        }

        // Could not get information about <path>.
        ERR(ERR_READ, str(CARG(1)));
        #endif

        // Return empty string
        // on failure.
        REST;
    }

    // The parser is broken.
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// (getdiskspace <pathname> [<unit>])                                    (V44)
//     return available space
//
// Refer to Installer.guide 1.20 (25.10.1999) 1995-99 by Amiga Inc.
//----------------------------------------------------------------------------
entry_p m_getdiskspace(entry_p contxt)
{
    // We need a path.
    if(c_sane(contxt, 1))
    {
        #ifdef AMIGA
        // Attempt to lock path.
        const char *n = str(CARG(1));
        BPTR lock = (BPTR) Lock(n, ACCESS_READ);

        // Do we have a lock?
        if(lock)
        {
            struct InfoData id;

            // Retrieve information from lock.
            if(Info(lock, &id))
            {
                long long free = (long long)
                    (id.id_NumBlocks -
                     id.id_NumBlocksUsed) *
                     id.id_BytesPerBlock;

                // Release lock ASAP.
                UnLock(lock);

                // From the Installer.guide 1.20:
                //
                // The parameter <unit> is optional and
                // defines the unit for the returned disk
                // space: "B" (or omitted) is "Bytes", "K"
                // is "Kilobytes", "M" is "Megabytes" and
                // "G" is "Gigabytes".
                if(CARG(2) && CARG(2) != end())
                {
                    switch(*str(CARG(2)))
                    {
                        case 'K':
                        case 'k':
                            free >>= 10;
                            break;

                        case 'M':
                        case 'm':
                            free >>= 20;
                            break;

                        case 'G':
                        case 'g':
                            free >>= 30;
                            break;
                    }
                }

                // Cap the return value.
                RNUM
                (
                    free > INT_MAX ?
                    INT_MAX : (int) free
                );
            }

            // Info() failed. Release lock.
            UnLock(lock);
        }

        // For some reason, we could not
        // acquire a lock on <path>, or,
        // we could get a lock, but failed
        // when trying to retrieve info
        // from the lock.
        ERR(ERR_READ, n);
        #endif

        // Failure.
        RNUM(-1);
    }

    // The parser is broken.
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// (getenv <name>)
//     return value of environment variable
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_getenv(entry_p contxt)
{
    // We need a variable name.
    if(c_sane(contxt, 1))
    {
        // Is there such an environment
        // variable?
        char *env = getenv(str(CARG(1)));

        if(env)
        {
            // Return what we found.
            RSTR(DBG_ALLOC(strdup(env)));
        }

        // Nothing found, return empty
        // string.
        REST;
    }

    // The parser is broken.
    PANIC(contxt);
    RCUR;
}

//----------------------------------------------------------------------------
// (getsize <file>)
//     return size
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_getsize(entry_p contxt)
{
    // We need a file name.
    if(c_sane(contxt, 1))
    {
        // Open the file in read only mode.
        FILE *file = fopen(str(CARG(1)), "r");

        if(file)
        {
            // Seek to the end of the file.
            fseek(file, 0L, SEEK_END);

            // Let the result be the position.
            DNUM = (int) ftell(file);

            // We're done.
            fclose(file);
        }
        else
        {
            // We could not open the file.
            ERR(ERR_READ_FILE, str(CARG(1)));
            DNUM = 0;
        }
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
    }

    // Success, failure or
    // broken parser.
    RCUR;
}

//----------------------------------------------------------------------------
// (getsum <file>)
//     return checksum of file for comparison purposes
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_getsum(entry_p contxt)
{
    // We need a filename.
    if(c_sane(contxt, 1))
    {
        const char *name = str(CARG(1));
        FILE *file = fopen(name, "r");

        if(file)
        {
            // This will yield different results
            // on 32 / 64 bit systems but that's
            // hardly a problem (or?).
            int chr = getc(file),
                alfa = 1, beta = 0;

            // Adler-32 checksum.
            while(chr != EOF)
            {
                alfa = (alfa + chr) % 65521;
                beta = (alfa + beta) % 65521;
                chr = getc(file);
            }

            DNUM = (beta << 16) | alfa;
            fclose(file);
        }
        else
        {
            ERR(ERR_READ_FILE, name);
            DNUM = 0;
        }
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
    }

    // Success, failure or
    // broken parser.
    RCUR;
}

//----------------------------------------------------------------------------
// getversion helper
//
// FIXME - description
//----------------------------------------------------------------------------
int h_getversion(entry_p contxt, const char *name)
{
    // Try to open whatever we have
    // in read only mode.
    FILE *file = fopen(name, "r");
    int ver = -1;

    // Could we open the file?
    if(file)
    {
        int ndx = 0, chr = 0;

        // Version key.
        int key[] = {'$','V','E','R',':', ' ', 0};

        // Read one byte at a time to find the
        // location of the version key if any.
        while(chr != EOF && key[ndx])
        {
            chr = fgetc(file);
            ndx = chr == key[ndx] ? ndx + 1 : 0;
        }

        // If we found the key, we have reached
        // the terminating 0, ergo found a match.
        if(!key[ndx])
        {
            // Fill up buffer with enough data to
            // hold any realistic version string.
            fread(get_buf(), 1, buf_size(), file);

            // Do we have data in the buffer?
            if(!ferror(file))
            {
                // Begin search after first ws.
                char *data = strchr(get_buf(), ' ');

                // If there's no ws we fail.
                if(data)
                {
                    // Major and revision.
                    int maj = 0, rev = 0;

                    // Version string pattern.
                    const char *pat = "%*[^0123456789]%d.%d%*[^\0]";

                    // Try to find version string.
                    if(sscanf(data, pat, &maj, &rev) == 2)
                    {
                        // We found something.
                        ver = (maj << 16) | rev;
                    }
                }
            }
        }

        // Did we have any reading problems?
        if(ferror(file))
        {
            // Could not read from file. This
            // will pick up problems from both
            // fgetc() and fread() above.
            ERR(ERR_READ_FILE, name);
        }

        // We don't need the file anymore.
        fclose(file);
    }
    else
    {
        // Only fail if we're in 'strict' mode.
        if(get_numvar(contxt, "@strict"))
        {
            // Could not read from file.
            ERR(ERR_READ_FILE, name);
        }
        else
        {
            // In non strict mode a non existing
            // file has version 0.
            ver = 0;
        }
    }

    // Version or -1.
    return ver;
}

//----------------------------------------------------------------------------
// (getversion <file> (resident))
//     return version/revision of file, library, etc. as 32 bit num
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//----------------------------------------------------------------------------
entry_p m_getversion(entry_p contxt)
{
    // All we need is a context.
    if(contxt)
    {
        // Unknown version.
        DNUM = 0;

        // Any arguments given?
        if(contxt->children &&
           c_sane(contxt, 1))
        {
            // Name of whatever we're trying to
            // get the version information from.
            const char *name = str(CARG(1));

            // A resident library or device?
            if(get_opt(contxt, OPT_RESIDENT))
            {
                #ifdef AMIGA
                struct Resident *res =
                    (struct Resident *) FindResident(name);

                if(res)
                {
                    int v, r;
                    const char *id = res->rt_IdString;

                    // Version string pattern.
                    const char *p = "%*[^0123456789]%d.%d%*[^\0]";

                    // Try to find the revision, if any, in
                    // the id string. The major part of our
                    // parsed result should match rt_Version.
                    if(sscanf(id, p, &v, &r) == 2 &&
                       v == res->rt_Version)
                    {
                        // We found something.
                        DNUM = (v << 16) | r;
                    }
                    else
                    {
                        // We can't trust the parsed result.
                        // Use what we know, ignore revision.
                        DNUM = res->rt_Version << 16;
                    }
                }
                #else
                DNUM = 0;
                #endif
            }
            // A file of some sort, on disk, not resident.
            else
            {
                int ver = h_getversion(contxt, name);
                DNUM = ver != -1 ? ver : 0;
            }
        }
        #ifdef AMIGA
        else
        {
            // No arguments, return version of Exec.
            extern struct ExecBase *SysBase;
            DNUM = (SysBase->LibNode.lib_Version << 16) |
                    SysBase->SoftVer;
        }
        #endif
    }
    else
    {
        // The parser is broken
        PANIC(contxt);
    }

    // Return whatever we have, could
    // be a failure (0) or version and
    // revision information.
    RCUR;
}

//----------------------------------------------------------------------------
// (iconinfo <parameters>)
//     return information about an icon (V42.12)
//
// Refer to Installer.guide 1.19 (29.4.96) 1995-96 by ESCOM AG
//
// Despite what the Installer.guide says, 'help', 'prompt', 'confirm'
// and 'safe' are dead options in OS 3.9. We choose to ignore the guide
// and mimic the behaviour of the implementation in 3.9. Also, the 3.9
// implementation deletes tooltypes when (gettooltype) is used. This is
// assumed to be a bug and therefore not mimiced.
//----------------------------------------------------------------------------
entry_p m_iconinfo(entry_p contxt)
{
    // We need one or more arguments.
    if(c_sane(contxt, 1))
    {
        entry_p dst     =   get_opt(contxt, OPT_DEST);
        entry_p types[] = { get_opt(contxt, OPT_GETTOOLTYPE),
                            get_opt(contxt, OPT_GETDEFAULTTOOL),
                            get_opt(contxt, OPT_GETSTACK),
                            get_opt(contxt, OPT_GETPOSITION), end() };

        // We need something to work with.
        if(dst)
        {
            // Something is 'dst'.info
            char *file = str(dst);

            #ifdef AMIGA
            // Get icon information.
            struct DiskObject *obj = (struct DiskObject *)
                GetDiskObject(file);
            #else
            char *obj = file;
            #endif

            if(obj)
            {
                // Iterate over all options or until
                // we run into resource problems.
                for(size_t i = 0; types[i] != end() &&
                    !DID_ERR; i++)
                {
                    // If we have an option of any kind.
                    if(types[i])
                    {
                        // Iterate over all its children.
                        for(size_t j = 0;
                            types[i]->children[j] &&
                            types[i]->children[j] != end(); j++)
                        {
                            // Get variable name and option type.
                            int type = types[i]->id;
                            char *name = str(types[i]->children[j]);

                            // Variable names must be atleast one
                            // character long.
                            if(*name)
                            {
                                char *svl = NULL;
                                entry_p val;

                                #ifdef AMIGA
                                // Is this a numerical value?
                                if(type == OPT_GETSTACK ||
                                   type == OPT_GETPOSITION)
                                {
                                    int v =
                                    (
                                        type == OPT_GETSTACK ?
                                        obj->do_StackSize : j == 0 ?
                                        obj->do_CurrentX : obj->do_CurrentY
                                    );

                                    snprintf(get_buf(), buf_size(), "%d", v);
                                    svl = get_buf();
                                }
                                else
                                if(type == OPT_GETDEFAULTTOOL &&
                                   obj->do_DefaultTool)
                                {
                                    svl = obj->do_DefaultTool;
                                }
                                else
                                if(type == OPT_GETTOOLTYPE &&
                                   obj->do_ToolTypes)
                                {
                                    svl = (char *) FindToolType(obj->do_ToolTypes, name);
                                    name = str(types[i]->children[++j]);
                                }

                                // Always a valid value.
                                svl = svl ? svl : "";
                                #else
                                // Testing purposes only.
                                snprintf(get_buf(), buf_size(), "%d:%zu", type, j);
                                svl = get_buf();
                                #endif

                                // Always a valid (string).
                                val = new_string(DBG_ALLOC(strdup(svl)));

                                if(val)
                                {
                                    // If we already have a symbol of the same
                                    // same as in the option, replace the value
                                    // of the old one with the new value.
                                    if(contxt->symbols)
                                    {
                                        for(size_t k = 0;
                                            contxt->symbols[k] &&
                                            contxt->symbols[k] != end();
                                            k++)
                                        {
                                            if(!strcasecmp(contxt->symbols[k]->name, name))
                                            {
                                                kill(contxt->symbols[k]->resolved);
                                                contxt->symbols[k]->resolved = val;
                                                push(global(contxt), contxt->symbols[k]);
                                                val->parent = contxt->symbols[k];

                                                // We no longer own 'val'.
                                                val = NULL;
                                                break;
                                            }
                                        }
                                    }

                                    // No, this is a new symbol. Create, append
                                    // to this function and push to the global
                                    // context.
                                    if(val)
                                    {
                                        entry_p sym = new_symbol(DBG_ALLOC(strdup(name)));

                                        if(sym)
                                        {
                                            // Adopt the value found above.
                                            val->parent = sym;
                                            sym->resolved = val;

                                            if(append(&contxt->symbols, sym))
                                            {
                                                push(global(contxt), sym);
                                                sym->parent = contxt;
                                            }
                                        }
                                        else
                                        {
                                            // Out of memory. Do not
                                            // leak 'val'.
                                            kill(val);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                #ifdef AMIGA
                FreeDiskObject(obj);
                #endif
            }
            else
            {
                // More information? IoErr() is nice.
                ERR(ERR_READ_FILE, file);
                RNUM(0);
            }

            // Success.
            RNUM(1);
        }

        // We need a destination.
        ERR(ERR_MISSING_OPTION, "dest");
        RNUM(0);
    }

    // The parser is broken
    PANIC(contxt);
    RCUR;
}

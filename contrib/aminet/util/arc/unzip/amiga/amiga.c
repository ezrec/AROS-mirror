/*------------------------------------------------------------------------

  amiga.c

  Amiga-specific routines for use with Info-ZIP's UnZip 5.1 and later.
  See History.5xx for revision history.

  Contents:   mapattr()
              mapname()
              do_wild()
              checkdir()
              close_outfile()
              _abort()                (Aztec C only)
             [dateformat()]           (currently not used)
              windowheight()
              version()

  ------------------------------------------------------------------------*/


#define UNZIP_INTERNAL
#include "unzip.h"

/* Globular varibundus -- now declared in SYSTEM_SPECIFIC_GLOBALS in amiga.h */

/* static int created_dir; */      /* used in mapname(), checkdir() */
/* static int renamed_fullpath; */ /* ditto */

#define PERMS   0777
#define MKDIR(path,mode) mkdir(path)


#ifndef S_ISCRIPT          /* not having one implies you have none */
#  define S_IARCHIVE 0020  /* not modified since this bit was last set */
#  define S_IREAD    0010  /* can be opened for reading */
#  define S_IWRITE   0004  /* can be opened for writing */
#  define S_IDELETE  0001  /* can be deleted */
#endif /* S_ISCRIPT */

#ifndef S_IRWD
#  define S_IRWD     0015  /* useful combo of Amiga privileges */
#endif /* !S_IRWD */

#ifndef S_IHIDDEN
#  define S_IHIDDEN  0200  /* hidden supported in future AmigaDOS (someday) */
#endif /* !S_HIDDEN */

#ifndef SFX
/* Make sure the number here matches version.h in the *EXACT* form */
/* UZ_MAJORVER "." UZ_MINORVER PATCHLEVEL vvvv     No non-digits!  */
const char version_id[]  = "\0$VER: UnZip 5.30 ("
#include "env:VersionDate"
   ")\r\n";
#endif /* SFX */


/**********************/
/* Function mapattr() */
/**********************/

int mapattr(__G)      /* Amiga version */
    __GDEF
{
    ulg  tmp = G.crec.external_file_attributes;


    /* Amiga attributes = hsparwed = hidden, script, pure, archive,
     * read, write, execute, delete */

    switch (G.pInfo->hostnum) {
        case AMIGA_:
            if ((tmp & 1) == (tmp>>18 & 1))
                tmp ^= 0x000F0000;      /* PKAZip compatibility kluge */
            /* turn off archive bit for restored Amiga files */
            G.pInfo->file_attr = (unsigned)((tmp>>16) & (~S_IARCHIVE));
            break;

        case UNIX_:   /* preserve read, write, execute:  use logical-OR of */
        case VMS_:    /* user, group, and other; if writable, set delete bit */
            tmp >>= 16;
            tmp = (( tmp>>6 | tmp>>3 | tmp) & 07) << 1;
            G.pInfo->file_attr = (unsigned)(tmp&S_IWRITE? tmp|S_IDELETE : tmp);
            break;

        /* all other platforms:  assume read-only bit in DOS half of attribute
         * word is set correctly ==> will become READ or READ+WRITE+DELETE */
        case FS_FAT_:
        case FS_HPFS_:  /* can add S_IHIDDEN check to MSDOS/OS2/NT eventually */
        case FS_NTFS_:
        case MAC_:
        case ATARI_:
        case TOPS20_:
        default:
            G.pInfo->file_attr = (unsigned)(tmp&1? S_IREAD : S_IRWD);
            break;

    } /* end switch (host-OS-created-by) */

    G.pInfo->file_attr &= 0xff;   /* mask off all but lower eight bits */
    return 0;

} /* end function mapattr() */




/************************/
/*  Function mapname()  */
/************************/

int mapname(__G__ renamed)  /* return 0 if no error, 1 if caution (truncate), */
    __GDEF                  /* 2 if warning (skip because dir doesn't exist), */
    int renamed;            /* 3 if error (skip file), 10 if no memory (skip) */
{                           /*  [also IZ_CREATED_DIR] */
    char pathcomp[FILNAMSIZ];   /* path-component buffer */
    char *pp, *cp=NULL;         /* character pointers */
    char *lastsemi = NULL;      /* pointer to last semi-colon in pathcomp */
    int error = 0;
    register unsigned workch;   /* hold the character being tested */


/*---------------------------------------------------------------------------
    Initialize various pointers and counters and stuff.
  ---------------------------------------------------------------------------*/

    /* can create path as long as not just freshening, or if user told us */
    G.create_dirs = (!G.fflag || renamed);

    G.created_dir = FALSE;      /* not yet */

    /* user gave full pathname:  don't prepend G.rootpath */
    G.renamed_fullpath = (renamed && strchr(G.filename, ':'));

    if (checkdir(__G__ (char *)NULL, INIT) == 10)
        return 10;              /* initialize path buffer, unless no memory */

    *pathcomp = '\0';           /* initialize translation buffer */
    pp = pathcomp;              /* point to translation buffer */
    if (G.jflag)                /* junking directories */
        cp = (char *)strrchr(G.filename, '/');
    if (cp == NULL)             /* no '/' or not junking dirs */
        cp = G.filename;        /* point to internal zipfile-member pathname */
    else
        ++cp;                   /* point to start of last component of path */

/*---------------------------------------------------------------------------
    Begin main loop through characters in filename.
  ---------------------------------------------------------------------------*/

    while ((workch = (uch)*cp++) != 0) {

        switch (workch) {
        case '/':             /* can assume -j flag not given */
            *pp = '\0';
            if ((error = checkdir(__G__ pathcomp, APPEND_DIR)) > 1)
                return error;
            pp = pathcomp;    /* reset conversion buffer for next piece */
            lastsemi = NULL;  /* leave directory semi-colons alone */
            break;

        case ';':             /* VMS version (or DEC-20 attrib?) */
            lastsemi = pp;         /* keep for now; remove VMS ";##" */
            *pp++ = (char)workch;  /*  later, if requested */
            break;

        default:
            /* allow ISO European characters in filenames: */
            if (isprint(workch) || (160 <= workch && workch <= 255))
                *pp++ = (char)workch;
        } /* end switch */
    } /* end while loop */

    *pp = '\0';                   /* done with pathcomp:  terminate it */

    /* if not saving them, remove with VMS version numbers (appended ";###") */
    if (!G.V_flag && lastsemi) {
        pp = lastsemi + 1;
        while (isdigit((uch)(*pp)))
            ++pp;
        if (*pp == '\0')          /* only digits between ';' and end:  nuke */
            *lastsemi = '\0';
    }

/*---------------------------------------------------------------------------
    Report if directory was created (and no file to create:  filename ended
    in '/'), check name to be sure it exists, and combine path and name be-
    fore exiting.
  ---------------------------------------------------------------------------*/

    if (G.filename[strlen(G.filename) - 1] == '/') {
        checkdir(__G__ G.filename, GETPATH);
        if (G.created_dir) {
            if (QCOND2) {
                Info(slide, 0, ((char *)slide, "   creating: %s\n",
                  G.filename));
            }
            return IZ_CREATED_DIR;   /* set dir time (note trailing '/') */
        }
        return 2;   /* dir existed already; don't look for data to extract */
    }

    if (*pathcomp == '\0') {
        Info(slide, 1, ((char *)slide, "mapname:  conversion of %s failed\n",
             G.filename));
        return 3;
    }

    if ((error = checkdir(__G__ pathcomp, APPEND_NAME)) == 1) {
        /* GRR:  OK if truncated here:  warn and continue */
        /* (warn in checkdir?) */
    }
    checkdir(__G__ G.filename, GETPATH);

    return error;

} /* end function mapname() */


static int ispattern(char *p)
{
    register char c;
    while (c = *p++)
        if (c == '\\') {
            if (!*++p)
                return FALSE;
        } else if (c == '?' || c == '*')
            return TRUE;
        else if (c == '[') {
            for (;;) {
                if (!(c = *p++))
                    return FALSE;
                else if (c == '\\') {
                    if (!*++p)
                        return FALSE;
                } else if (c == ']')
                    return TRUE;
            }
        }
    return FALSE;
}

/**********************/
/* Function do_wild() */
/**********************/

char *do_wild(__G__ wildspec)
    __GDEF
    char *wildspec;         /* only used first time on a given dir */
{
/* these statics are now declared in SYSTEM_SPECIFIC_GLOBALS in amiga.h:
    static DIR *wild_dir = NULL;
    static char *dirname, *wildname, matchname[FILNAMSIZ];
    static int notfirstcall = FALSE, dirnamelen;
*/
    struct dirent *file;
    BPTR lok = 0;
    /* Even when we're just returning wildspec, we *always* do so in
     * matchname[]--calling routine is allowed to append four characters
     * to the returned string, and wildspec may be a pointer to argv[].
     */
    if (!G.notfirstcall) {      /* first call:  must initialize everything */
        G.notfirstcall = TRUE;
        /* avoid needless readdir() scans: */
        if (!ispattern(wildspec) || (lok = Lock(wildspec, ACCESS_READ))) {
            if (lok) UnLock(lok);       /* ^^ we ignore wildcard chars if */
            G.dirnamelen = 0;           /* the name matches a real file   */
            strcpy(G.matchname, wildspec);
            return G.matchname;
        }

        /* break the wildspec into a directory part and a wildcard filename */
        if ((G.wildname = strrchr(wildspec, '/')) == NULL
                        && (G.wildname = strrchr(wildspec, ':')) == NULL) {
            G.dirname = "";             /* current dir */
            G.dirnamelen = 0;
            G.wildname = wildspec;
        } else {
            ++G.wildname;     /* point at character after '/' or ':' */
            G.dirnamelen = G.wildname - wildspec;
            if ((G.dirname = (char *)malloc(G.dirnamelen+1)) == NULL) {
                Info(slide, 1, ((char *)slide,
                     "warning:  can't allocate wildcard buffers\n"));
                strcpy(G.matchname, wildspec);
                return G.matchname; /* but maybe filespec was not a wildcard */
            }
            strncpy(G.dirname, wildspec, G.dirnamelen);
            G.dirname[G.dirnamelen] = 0;
        }

        if ((G.wild_dir = opendir(G.dirname)) != NULL) {
            while ((file = readdir(G.wild_dir)) != NULL) {
                if (match(file->d_name, G.wildname, 1)) {  /* ignore case */
                    strcpy(G.matchname, G.dirname);
                    strcpy(G.matchname + G.dirnamelen, file->d_name);
                    return G.matchname;
                }
            }
            /* if we get to here directory is exhausted, so close it */
            closedir(G.wild_dir);
            G.wild_dir = NULL;
        }

        /* return the raw wildspec in case that works (e.g., directory not
         * searchable, but filespec was not wild and file is readable) */
        strcpy(G.matchname, wildspec);
        return G.matchname;
    }

    /* last time through, might have failed opendir but returned raw wildspec */
    if (G.wild_dir == NULL) {
        G.notfirstcall = FALSE;    /* nothing left to try -- reset */
        if (G.dirnamelen > 0)
            free(G.dirname);
        return (char *)NULL;
    }

    /* If we've gotten this far, we've read and matched at least one entry
     * successfully (in a previous call), so dirname has been copied into
     * matchname already.
     */
    while ((file = readdir(G.wild_dir)) != NULL)
        if (match(file->d_name, G.wildname, 1)) {   /* 1 == ignore case */
            /* strcpy(G.matchname, dirname); */
            strcpy(G.matchname + G.dirnamelen, file->d_name);
            return G.matchname;
        }

    closedir(G.wild_dir);  /* have read at least one dir entry; nothing left */
    G.wild_dir = NULL;
    G.notfirstcall = FALSE;  /* reset for new wildspec */
    if (G.dirnamelen > 0)
        free(G.dirname);
    return (char *)NULL;

} /* end function do_wild() */



/***********************/
/* Function checkdir() */
/***********************/

int checkdir(__G__ pathcomp, flag)
    __GDEF
    char *pathcomp;
    int flag;
/*
 * returns:  1 - (on APPEND_xxx) truncated path component
 *           2 - path doesn't exist, not allowed to create
 *           3 - path doesn't exist, tried to create and failed; or
 *               path exists and is not a directory, but is supposed to be
 *           4 - path is too long
 *          10 - can't allocate memory for filename buffers
 */
{
/* these statics are now declared in SYSTEM_SPECIFIC_GLOBALS in amiga.h: */
/*  static int rootlen = 0; */   /* length of rootpath */
/*  static char *rootpath;  */   /* user's "extract-to" directory */
/*  static char *buildpath; */   /* full path (so far) to extracted file */
/*  static char *end;       */   /* pointer to end of buildpath ('\0') */

#   define FN_MASK   7
#   define FUNCTION  (flag & FN_MASK)



/*---------------------------------------------------------------------------
    APPEND_DIR:  append the path component to the path being built and check
    for its existence.  If doesn't exist and we are creating directories, do
    so for this one; else signal success or error as appropriate.
  ---------------------------------------------------------------------------*/

/* GRR:  check path length after each segment:  warn about truncation */

    if (FUNCTION == APPEND_DIR) {
        int too_long = FALSE;

        Trace((stderr, "appending dir segment [%s]\n", pathcomp));
        while ((*G.build_end = *pathcomp++))
            ++G.build_end;
        /* Truncate components over 30 chars? Nah, the filesystem handles it. */
        if ((G.build_end-G.buildpath) > FILNAMSIZ-3)       /* room for "/a\0" */
            too_long = TRUE;                /* check if extracting directory? */
        if (stat(G.buildpath, &G.statbuf)) {  /* path doesn't exist */
            if (!G.create_dirs) { /* told not to create (freshening) */
                free(G.buildpath);
                return 2;         /* path doesn't exist:  nothing to do */
            }
            if (too_long) {
                Info(slide, 1, ((char *)slide,
                  "checkdir error:  path too long: %s\n", G.buildpath));
                free(G.buildpath);
                return 4;         /* no room for filenames:  fatal */
            }
            if (MKDIR(G.buildpath, 0777) == -1) {   /* create the directory */
                Info(slide, 1, ((char *)slide,
                 "checkdir error:  can't create %s\n\
                 unable to process %s.\n", G.buildpath, G.filename));
                free(G.buildpath);
                return 3;      /* path didn't exist, tried to create, failed */
            }
            G.created_dir = TRUE;
        } else if (!S_ISDIR(G.statbuf.st_mode)) {
            Info(slide, 1, ((char *)slide,
                 "checkdir error:  %s exists but is not a directory\n\
                 unable to process %s.\n", G.buildpath, G.filename));
            free(G.buildpath);
            return 3;          /* path existed but wasn't dir */
        }
        if (too_long) {
            Info(slide, 1, ((char *)slide,
                 "checkdir error:  path too long: %s\n", G.buildpath));
            free(G.buildpath);
            return 4;         /* no room for filenames:  fatal */
        }
        *G.build_end++ = '/';
        *G.build_end = '\0';
        Trace((stderr, "buildpath now = [%s]\n", G.buildpath));
        return 0;

    } /* end if (FUNCTION == APPEND_DIR) */

/*---------------------------------------------------------------------------
    GETPATH:  copy full path to the string pointed at by pathcomp, and free
    G.buildpath.  Not our responsibility to worry whether pathcomp has room.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == GETPATH) {
        strcpy(pathcomp, G.buildpath);
        Trace((stderr, "getting and freeing path [%s]\n", pathcomp));
        free(G.buildpath);
        G.buildpath = G.build_end = NULL;
        return 0;
    }

/*---------------------------------------------------------------------------
    APPEND_NAME:  assume the path component is the filename; append it and
    return without checking for existence.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == APPEND_NAME) {
        Trace((stderr, "appending filename [%s]\n", pathcomp));
        while ((*G.build_end = *pathcomp++)) {
            ++G.build_end;
            if ((G.build_end-G.buildpath) >= FILNAMSIZ) {
                *--G.build_end = '\0';
                Info(slide, 1, ((char *)slide,
                   "checkdir warning:  path too long; truncating\n\
                   %s\n                -> %s\n", G.filename, G.buildpath));
                return 1;   /* filename truncated */
            }
        }
        Trace((stderr, "buildpath static now = [%s]\n", G.buildpath));
        return 0;  /* could check for existence here, prompt for new name... */
    }

/*---------------------------------------------------------------------------
    INIT:  allocate and initialize buffer space for the file currently being
    extracted.  If file was renamed with an absolute path, don't prepend the
    extract-to path.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == INIT) {
        Trace((stderr, "initializing buildpath static to "));
        if ((G.buildpath = (char *)malloc(strlen(G.filename)+G.rootlen+1))
              == NULL)
            return 10;
        if ((G.rootlen > 0) && !G.renamed_fullpath) {
            strcpy(G.buildpath, G.rootpath);
            G.build_end = G.buildpath + G.rootlen;
        } else {
            *G.buildpath = '\0';
            G.build_end = G.buildpath;
        }
        Trace((stderr, "[%s]\n", G.buildpath));
        return 0;
    }

/*---------------------------------------------------------------------------
    ROOT:  if appropriate, store the path in G.rootpath and create it if
    necessary; else assume it's a zipfile member and return.  This path
    segment gets used in extracting all members from every zipfile specified
    on the command line.
  ---------------------------------------------------------------------------*/

#if (!defined(SFX) || defined(SFX_EXDIR))
    if (FUNCTION == ROOT) {
        Trace((stderr, "initializing root path to [%s]\n", pathcomp));
        if (pathcomp == NULL) {
            G.rootlen = 0;
            return 0;
        }
        if ((G.rootlen = strlen(pathcomp)) > 0) {
            if (stat(pathcomp, &G.statbuf) || !S_ISDIR(G.statbuf.st_mode)) {
                /* path does not exist */
                if (!G.create_dirs) {
                    G.rootlen = 0;
                    return 2;   /* treat as stored file */
                }
                /* create the directory (could add loop here to scan pathcomp
                 * and create more than one level, but why really necessary?) */
                if (MKDIR(pathcomp, 0777) == -1) {
                    Info(slide, 1, ((char *)slide,
                         "checkdir:  can't create extraction directory: %s\n",
                         pathcomp));
                    G.rootlen = 0; /* path didn't exist, tried to create, and */
                    return 3;  /* failed:  file exists, or 2+ levels required */
                }
            }
            if ((G.rootpath = (char *)malloc(G.rootlen+2)) == NULL) {
                G.rootlen = 0;
                return 10;
            }
            strcpy(G.rootpath, pathcomp);
            if (G.rootpath[G.rootlen-1] != ':' && G.rootpath[G.rootlen-1] != '/')
                G.rootpath[G.rootlen++] = '/';
            G.rootpath[G.rootlen] = '\0';
            Trace((stderr, "rootpath now = [%s]\n", G.rootpath));
        }
        return 0;
    }
#endif /* !SFX || SFX_EXDIR */

/*---------------------------------------------------------------------------
    END:  free G.rootpath, immediately prior to program exit.
  ---------------------------------------------------------------------------*/

    if (FUNCTION == END) {
        Trace((stderr, "freeing rootpath\n"));
        if (G.rootlen > 0)
            free(G.rootpath);
        return 0;
    }

    return 99;  /* should never reach */

} /* end function checkdir() */


/**************************************/
/* Function close_outfile() */
/**************************************/
/* this part differs slightly with Zip */
/*-------------------------------------*/

void close_outfile(__G)
    __GDEF
{
    time_t m_time;
#ifdef USE_EF_UT_TIME
    iztimes z_utime;
#endif
    LONG FileDate();

    if (G.cflag)                /* can't set time or filenote on stdout */
        return;

  /* close the file *before* setting its time under AmigaDos */

    fclose(G.outfile);

#ifdef USE_EF_UT_TIME
    if (G.extra_field &&
        (ef_scan_for_izux(G.extra_field, G.lrec.extra_field_length, 0,
                          &z_utime, NULL) & EB_UT_FL_MTIME)) {
        TTrace((stderr, "close_outfile:  Unix e.f. modif. time = %ld\n",
                         z_utime.mtime));
        m_time = z_utime.mtime;
    } else {
        /* Convert DOS time to time_t format */
        m_time = dos_to_unix_time(G.lrec.last_mod_file_date,
                                  G.lrec.last_mod_file_time);
    }
#else /* !USE_EF_UT_TIME */
    /* Convert DOS time to time_t format */
    m_time = dos_to_unix_time(G.lrec.last_mod_file_date,
                              G.lrec.last_mod_file_time);
#endif /* ?USE_EF_UT_TIME */

#ifdef DEBUG
    Info(slide, 1, ((char *)slide, "\nclose_outfile(): m_time=%s\n",
                         ctime(&m_time)));
#endif

    if (!FileDate(G.filename, &m_time))
        Info(slide, 1, ((char *)slide,
             "warning:  can't set the time for %s\n", G.filename));

  /* set file perms after closing (not done at creation)--see mapattr() */

    chmod(G.filename, G.pInfo->file_attr);

  /* give it a filenote from the zipfile comment, if appropriate */

    if (G.N_flag && G.filenotes[G.filenote_slot]) {
        SetComment(G.filename, G.filenotes[G.filenote_slot]);
        free(G.filenotes[G.filenote_slot]);
        G.filenotes[G.filenote_slot] = NULL;
    }

} /* end function close_outfile() */


/********************************************************************/
/* Load filedate as a separate external file; it's used by Zip, too.*/
/*                                                                  */
#include "amiga/filedate.c"                                      /* */
/*                                                                  */
/********************************************************************/

/********************* do linewise with stat.c **********************/

#  include "amiga/stat.c"
/* this is the exact same stat.c used by Zip */

#  include <stdio.h>

void _abort(void)               /* called when ^C is pressed */
{
    /* echon(); */
    close_leftover_open_dirs();
    fflush(stdout);
    fputs("\n^C\n", stderr);
    exit(1);
}


/**************************************************************/
/* function windowheight() -- uses sendpkt() from filedate.c: */
/**************************************************************/

#include <devices/conunit.h>
#include <dos/dosextens.h>
#include <exec/memory.h>
#include <clib/exec_protos.h>

extern long sendpkt(struct MsgPort *pid, long action, long *args, long nargs);

int windowheight(BPTR fh)
{
    if (fh && IsInteractive(fh)) {
        struct ConUnit *conunit = NULL;
        void *conp = ((struct FileHandle *) (fh << 2))->fh_Type;
        struct InfoData *ind = AllocMem(sizeof(*ind), MEMF_PUBLIC);
        long argp = ((unsigned long) ind) >> 2;

        if (ind && conp && sendpkt(conp, ACTION_DISK_INFO, &argp, 1))
            conunit = (void *) ((struct IOStdReq *) ind->id_InUse)->io_Unit;
        if (ind)
            FreeMem(ind, sizeof(*ind));
        if (conunit)
            return conunit->cu_YMax + 1;
    }
    return INT_MAX;
}


#ifdef AMIGA_VOLUME_LABELS
/* This function is for if we someday implement -$ on the Amiga. */
#  include <dos/dosextens.h>
#  include <dos/filehandler.h>
#  include <clib/macros.h>

BOOL is_floppy(char *path)
{
    BOOL okay = FALSE;
    char devname[32], *debna;
    ushort i;
    BPTR lok = Lock(path, ACCESS_READ), pok;
    struct FileSysStartupMsg *fart;
    struct DeviceNode *debb, devlist = (void *) BADDR((struct DosInfo *)
                                BADDR(DOSBase->dl_Root->rn_Info)->di_DevInfo);
    if (!lok)
        return FALSE;                   /* should not happen */
    if (pok = ParentDir(path)) {
        UnLock(lok);
        UnLock(pok);
        return FALSE;                   /* it's not a root directory path */
    }
    Forbid();
    for (debb = devlist; debb; debb = BADDR(debb->dn_Next))
        if (debb->dn_Type == DLT_DEVICE && (debb->dn_Task == lick->fl_Task))
            if (fart = BADDR(debb->dn_Startup)) {
                debna = (char *) BADDR(fart->fssm_Device) + 1;
                if ((i = debna[-1]) > 31) i = 30;
                strncpy(devname, debna, i);
                devname[i] = 0;
                okay = !strcmp(devname, "trackdisk.device")
                                || !strcmp(devname, "mfm.device")
                                || !strcmp(devname, "messydisk.device");
                break;  /* We only support obvious floppy drives, not tricky */
            }           /* things like removable cartrige hard drives, or    */
    Permit();           /* any unusual kind of floppy device driver.         */
    return okay;
}
#endif /* AMIGA_VOLUME_LABELS */


#ifndef SFX

# if 0
/* As far as I can tell, all the locales AmigaDOS 2.1 knows about all */
/* happen to use DF_MDY ordering, so there's no point in using this.  */

/*************************/
/* Function dateformat() */
/*************************/

#include <clib/locale_protos.h>
#ifdef AZTEC_C
#  include <pragmas/locale_lib.h>
#endif

int dateformat()
{
/*---------------------------------------------------------------------------
    For those operating systems which support it, this function returns a
    value which tells how national convention says that numeric dates are
    displayed.  Return values are DF_YMD, DF_DMY and DF_MDY (the meanings
    should be fairly obvious).
  ---------------------------------------------------------------------------*/
    struct Library *LocaleBase;
    struct Locale *ll;
    int result = DF_MDY;        /* the default */

    if ((LocaleBase = OpenLibrary("locale.library", 0))) {
        if (ll = OpenLocale(NULL)) {
            uch *f = ll->loc_ShortDateFormat;
            /* In this string, %y|%Y is year, %b|%B|%h|%m is month, */
            /* %d|%e is day day, and %D|%x is short for mo/da/yr.   */
            if (!strstr(f, "%D") && !strstr(f, "%x")) {
                uch *da, *mo, *yr;
                if (!(mo = strstr(f, "%b")) && !(mo = strstr(f, "%B"))
                                    && !(mo = strstr(f, "%h")))
                    mo = strstr(f, "%m");
                if (!(da = strstr(f, "%d")))
                    da = strstr(f, "%e");
                if (!(yr = strstr(f, "%y")))
                    yr = strstr(f, "%Y");
                if (yr && yr < mo)
                    result = DF_YMD;
                else if (da && da < mo)
                    result = DF_DMY;
            }
            CloseLocale(ll);
        }
        CloseLibrary(LocaleBase);
    }
    return result;
}

# endif /* 0 */


/************************/
/*  Function version()  */
/************************/


/* NOTE:  the following include depends upon the environment
 *        variable $Workbench to be set correctly.  (Set by
 *        default, by kickstart during startup)
 */
int WBversion = (int)
#include "ENV:Workbench"
;

void version(__G)
   __GDEF
{
/* Define buffers. */

   char buf1[16];  /* compiler name */
   char buf2[16];  /* revstamp */
   char buf3[16];  /* OS */
   char buf4[16];  /* Date */
/*   char buf5[16];  /* Time */

/* format "with" name strings */

#ifdef AMIGA
# ifdef __SASC
   strcpy(buf1,"SAS/C ");
# else
#  ifdef LATTICE
    strcpy(buf1,"Lattice C ");
#  else
#   ifdef AZTEC_C
     strcpy(buf1,"Manx Aztec C ");
#   else
     strcpy(buf1,"UNKNOWN ");
#   endif
#  endif
# endif
/* "under" */
  sprintf(buf3,"AmigaDOS v%d",WBversion);
#else
  strcpy(buf1,"Unknown compiler ");
  strcpy(buf3,"Unknown OS");
#endif

/* Define revision, date, and time strings.
 * NOTE:  Do not calculate run time, be sure to use time compiled.
 * Pass these strings via your makefile if undefined.
 */

#if defined(__VERSION__) && defined(__REVISION__)
  sprintf(buf2,"version %d.%d",__VERSION__,__REVISION__);
#else
# ifdef __VERSION__
  sprintf(buf2,"version %d",__VERSION__);
# else
  sprintf(buf2,"unknown version");
# endif
#endif

#ifdef __DATE__
  sprintf(buf4," on %s",__DATE__);
#else
  strcpy(buf4," unknown date");
#endif

/******
#ifdef __TIME__
  sprintf(buf5," at %s",__TIME__);
#else
  strcpy(buf5," unknown time");
#endif
******/

/* Print strings using "CompiledWith" mask defined in unzip.c (used by all).
 *  ("Compiled with %s%s under %s%s%s%s.")
 */

   printf(LoadFarString(CompiledWith),
     buf1,
     buf2,
     buf3,
     buf4,
     "",    /* buf5 not used */
     "" );  /* buf6 not used */

} /* end function version() */

#endif /* !SFX */

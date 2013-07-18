/*
    Copyright © 2000, The AROS Development Team. All rights reserved.
    $Id$

    Desc: SortCopy from Thomas Richter
    Lang: English
*/

/*****************************************************************
 ** SortCopy                                                    **
 ** © 1996-2000 THOR-Software                                   **
 ** Version 1.34                                                **
 ** sortiert kopieren                                           **
 ** (Hard)Links mitlinken                                       **
 *****************************************************************/

/* Converted to AROS by SDuvan 18.11.2000 (some bugs fixed too) */

/// Includes
#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/memory.h>
#include <exec/alerts.h>
#define DOS_IO 1
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/dosasl.h>

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>

#ifdef  __AROS__
    #define   DEBUG 1
    #include  <aros/debug.h>
    #include  <exec/rawfmt.h>
#else
    #define  D(x)
    #include <thor/tinyio.h>
    #include <dos.h>

    extern void __builtin__emit(int);
    #define BreakPoint __builtin_emit(0x4afc)
#endif

#include <strings.h>
#include <stdarg.h>

///
/// Defines
#define SIGMASK (1<<12)
#define MAXSOFTNEST 16
#define BUFFERSIZE 2048

///
///Strukturen
/* Verwaltung eines Eintrages in der Directory */

struct DirEntry 
{
    struct DirEntry  *de_next;
    struct DirEntry  *de_pred;
    ULONG             de_flags;          /* Siehe unten */
    LONG              de_protection;
    struct DateStamp  de_date;
    ULONG             de_length;
    BPTR              de_sourcelock;     /* Nur in Sonderfällen: Lock auf
					    die Quelldirectory */
    BPTR              de_dirlock;        /* Nur bei Links verwendet: BPTR
					    Directory, wo der Link hinsoll
					    (Ziel!) */
    BPTR              de_linklock;       /* Bei Links: Directory, in dem
					    das Objekt liegt, zu dem der
					    Link besteht */
    struct MinList    de_sublist;
    char              de_name[MAXFILENAMELENGTH];
    char              de_comment[MAXCOMMENTLENGTH];

    /* Name des Zielobjektes  (Quellobject, d.h. Name des Links) in name */
    char              de_linkname[MAXFILENAMELENGTH]; 
};


/* Einträge in den Flags */
#define  DEF_SOFTLINK    (1 << 0)  /* entry is a softlink */
#define  DEF_HARDLINK    (1 << 1)  /* entry is a hardlink */
#define  DEF_DIR         (1 << 2)  /* entry is a directory */
#define  DEF_FORGET      (1 << 3)  /* failed to copy, ignore */
#define  DEF_NORESOLVE   (1 << 4)  /* failed to read the link, do not resolve */
#define  DEF_CREATED     (1 << 5)  /* directory has been created */
#define  DEF_FLATDIR     (1 << 6)  /* directory wasn't created, contents
				      was copied */
#define  DEF_REMOVED     (1 << 7)  /* directory has been removed */


/* Diese Node wird zum Kopieren von Files verwendet. */

struct CopyNode
{
    struct Message     cn_DosMessage; /* a message for the DOS process */
    struct DosPacket   cn_DosPacket;  /* the message to be posted */
    APTR               cn_Buffer;     /* Pointer to the buffer */
    ULONG              cn_BufSize;    /* size of the buffer to allocate */
    ULONG              cn_WriteSize;  /* number of bytes to be written out */
    UWORD              cn_Command;    /* last command sent */
    BOOL               cn_release;    /* release me? */
    struct CopyNode   *cn_BackTick;   /* just to make sure that this is 
					 our packet */
};

///
/// Statics

// These should be allocated by AllocDosObject()
struct FileInfoBlock fib;
struct FileInfoBlock dirfib;
struct AnchorPath ap;

struct DosLibrary  *DOSBase;
struct UtilityBase *UtilityBase;

BOOL   abortFlag;
BOOL   failcondition;
IPTR   tclower;
ULONG  depth;
ULONG  scount;
ULONG  bufsize;
BPTR   rootlock;

struct MinList resolvelist;

BOOL   quiet, quieter, seta, onlya, overwrite, leaveempty, deletecopied,
       force, update;
BOOL   purge, nocopy, sourcewild;

UBYTE  buffer[BUFFERSIZE];
char   failbuffer[512];

struct CopyNode  lastchancenode;
struct MsgPort   ioport;

BOOL  lastchancebusy;

char version[] = "$VER: SortCopy 41.1 (18.11.2000)";


///
/// Prototypes

void PostFailure(LONG error, char *failtext, ...);
BOOL ReadDir(BPTR lock, struct MinList *dirlist, char *name);
BOOL CopyFile(BPTR source, BPTR dest, char *name);
BOOL CopyDir(BPTR sourcelock, BPTR destlock, struct MinList *dirlist);
void Abort(void);
BOOL CheckAbort(void);
BOOL RecursiveCopy(BPTR source, BPTR dest, struct MinList *dirlist, char *name);
void FreeDirList(struct MinList *dirlist, BOOL links);
BOOL IsDir(BPTR lock, char *name);
BOOL FindSoftOrginal(struct DirEntry *de);
BOOL FindHardOrginal(struct DirEntry *de);
BOOL PathRelative(BPTR relpath, BPTR path, char *name, char *buffer,
		  ULONG bufsize);
BOOL ResolveLinks(struct MinList *list, BPTR dest);
BOOL CreateLink(char *name, BPTR melock, char *linkname, BPTR linklock,
		ULONG soft);
void OFlush(void);
BOOL CloneFlags(struct DirEntry *de);
ULONG CopyMain(char *sourcename, char *destname);
BOOL SetAFlag(struct DirEntry *de);
void DoIndent(void);
BOOL DirEmpty(BPTR lock, char *name);
BOOL KillForce(char *name, BOOL force);
BOOL RecursiveDelete(BPTR lock, struct MinList *dirlist, BOOL all, char *name);
void DeleteEntry(struct DirEntry *de, BOOL all);
BOOL RecursiveSetA(BPTR source, struct MinList *dirlist, char *name);
LONG Fatal(void);
BOOL PurgeDir(BPTR sourcelock, BPTR destlock, struct MinList *dirlist,
	      char *name);
void FreeDirEntry(struct DirEntry *de, BOOL locks);
BOOL FindDirEntry(struct MinList *dirlist, struct DirEntry *de);
BOOL DeleteDirList(BPTR source, struct MinList *dirlist, BOOL all);
BOOL MustUpdate(BPTR destlock, struct DirEntry *de);
BOOL CopyFileEntry(struct DirEntry *de, BPTR sourcelock, BPTR destlock);
BOOL CopyDirEntry(struct DirEntry *de, BPTR sourcelock, BPTR destlock);
struct DirEntry *AllocDirEntry(struct FileInfoBlock *fib);
ULONG CopyBodyMatch(struct AnchorPath *ap, char *pattern, BPTR source,
		    BPTR dest);
BOOL FillDirEntry(struct DirEntry *de, struct FileInfoBlock *fib);
BOOL CopyEntry(struct DirEntry *de, BPTR sourcelock, BPTR destlock);
BOOL CopySubDirectory(struct DirEntry *de, BPTR source, BPTR dest);
BOOL PrepareLink(struct DirEntry *de, BPTR source, BPTR dest);
void EnqueueEntry(struct MinList *dirlist, struct DirEntry *de);
BOOL CopyEntryList(struct MinList *dirlist, BPTR dest);
BOOL AddMatch(struct AnchorPath *ap, struct MinList *dirlist);
struct DirEntry *DupDirEntry(struct DirEntry *se);
BOOL DupDirList(struct MinList *source, struct MinList *dest);
BOOL SetAList(BPTR source, struct MinList *dirlist);
ULONG FillDirList(char *pattern, struct MinList *dirlist);
ULONG MainCopyProcess(BPTR source, BPTR dest, struct MinList *dirlist,
		      struct MinList *copylist);
BOOL SetRootLock(struct DirEntry *de);
void FreeCopyNode(struct CopyNode *cn, ULONG *limit);

#ifdef  USE_AROS_PACKETS
struct CopyNode *AllocCopyNode_AROS(BPTR from, ULONG size, ULONG *limit);
BOOL AsyncCopy_AROS(BPTR from, BPTR to, ULONG readsize, char *name);
#else
struct CopyNode *AllocCopyNode(BPTR from, ULONG size, ULONG *limit);
BOOL AsyncCopy(BPTR from, BPTR to, ULONG readsize, char *name);
#endif

///

#define  ARG_TEMPLATE  "FROM/A,TO/A,QT=QUIET/S,QTR=QUIETER/S,SETA=SETARCHIVE/S,NONA=NONAONLY/S,OVR=OVERWRITE/S,LVE=LEAVEEMPTY/S,DELC=DELETECOPIED/S,FORCE/S,UPD=UPDATE/S,PRB=PURGEBACKUP/S,PRO=PURGEONLY/S,BUFSIZE/K/N"

///main
int main(void)
{
    struct Process  *this;
    struct Message  *msg;
    struct RDArgs   *ra;
    
    IPTR    args[14];
    ULONG   res = 0;
    BPTR    oldlock;
    
    memset(args, 0, 14*sizeof(char *));
    
    this = (struct Process *)FindTask(NULL);
    
    if(this->pr_CLI == BNULL)
    {
	msg = WaitPort(&(this->pr_MsgPort));
	msg = GetMsg(&(this->pr_MsgPort));
	Forbid();
	ReplyMsg(msg);

	return 0;
    }

    /* Can't be started from WB */
    
    tclower = (IPTR)(this->pr_Task.tc_SPLower);

    DOSBase = (struct DosLibrary *)(OpenLibrary("dos.library", 37L));

    if(DOSBase != NULL)
    {

	oldlock = CurrentDir(BNULL);
	CurrentDir(oldlock);
	
	UtilityBase = (struct UtilityBase *)OpenLibrary("utility.library", 37L);

	if(UtilityBase != NULL)
	{
	    ra = ReadArgs(ARG_TEMPLATE, args, NULL);

	    if(ra != NULL)
	    {
		quiet = (BOOL)args[2];
		quieter = (BOOL)args[3];

		if(quieter)
		    quiet = TRUE;
		
		seta         = (BOOL)args[4];
		onlya        = (BOOL)args[5];
		overwrite    = (BOOL)args[6];
		leaveempty   = (BOOL)args[7];
		deletecopied = (BOOL)args[8];
		force        = (BOOL)args[9];
		update       = (BOOL)args[10];
		purge        = (BOOL)args[11];
		nocopy       = (BOOL)args[12];

		if(args[13])
		{
		    bufsize = *((LONG *)args[13]);
		}
		else
		{
		    bufsize = 0x7ffffff0;
		}

		if(bufsize < 256)
		    bufsize = 256;

		if(nocopy)
		    purge = TRUE;

		D(bug("Calling CopyMain()\n"));

		res = CopyMain((STRPTR)args[0], (STRPTR)args[1]);
		FreeArgs(ra);
	    }
	    else
	    { /* of ReadArgs */
		PrintFault(IoErr(), "SortCopy failed");
		res = 9;
	    }
	    
	    CloseLibrary((struct Library *)UtilityBase);
	}
	else
	{
	    Printf("SortCopy: Need utility.library V37!\n");
	    res = 32;
	}
	
	CurrentDir(oldlock);
	CloseLibrary((struct Library *)DOSBase);
	
    }
    else
    {
	res = 32;
    }
    
    return res;
}

///
/// Fatal
/* Hole den I/O-Fehlercode, brich bei fatalem Fehler ab */
LONG Fatal(void)
{
    LONG error;
    
    error = IoErr();
    
    if(error)
    {
	abortFlag = TRUE;
    }
    
    return error;
}

///
/// CopyMain
ULONG CopyMain(char *sourcename, char *destname)
{
    BPTR   dest, sourcelock;
    ULONG  res = 0;

    struct MinList dirlist, copylist;

    sourcelock = CurrentDir(BNULL);
    SetSignal(0L, SIGMASK);

    NewList((struct List *)&resolvelist);
    NewList((struct List *)&dirlist);
    NewList((struct List *)&copylist);

    depth = 0;
    CurrentDir(sourcelock);
    
    switch(ParsePatternNoCase(destname, buffer, BUFFERSIZE))
    {
    case 0x01:
	Printf("SortCopy: Wildcards not allowed in destination.\n");
	res = RETURN_ERROR;
	break;
	
    case 0x00:
	break;

    default:
	PostFailure(ERROR_BAD_TEMPLATE, "Wildcard parsing failed");
	res = RETURN_FAIL;
	break;
    }
    
    switch(ParsePatternNoCase(sourcename, buffer, BUFFERSIZE))
    {
    case 0x01:
	sourcewild = TRUE;
	break;

    case 0x00:
	sourcewild = FALSE;
	break;
	
    default:
	PostFailure(ERROR_BAD_TEMPLATE, "Wildcard parsing failed");
	res = RETURN_FAIL;
	break;
    }
    
    D(bug("CopyMain part 2\n"));

    if(!res)
    {
	res = MatchFirst(sourcename, &ap);

	if(res)
	{
	    if(res == ERROR_NO_MORE_ENTRIES)
		res = ERROR_OBJECT_NOT_FOUND;
	    
	    PostFailure(res, "Can't access \"%s\"", sourcename);
	    res = RETURN_ERROR;
	}
	
	MatchEnd(&ap);
    }
    
    D(bug("CopyMain part 3\n"));
    
    if(!res)
    {
	dest = Lock(destname, SHARED_LOCK);

	if(dest == BNULL)
	{
	    D(bug("Destination not found\n"));
	    
	    if(IoErr() == ERROR_OBJECT_NOT_FOUND)
	    {
		if(!nocopy)
		{
		    dest = CreateDir(destname);
		    
		    if(dest == BNULL)
		    {
			PostFailure(Fatal(),
				    "Can't create destination drawer \"%s\"",
				    destname);
			res = RETURN_ERROR;
		    }
		    else
		    {
			if(!(ChangeMode(CHANGE_LOCK, dest, SHARED_LOCK)))
			{
			    PostFailure(Fatal(), 
					"Can't change the lock to \"%s\"",
					destname);
			    UnLock(dest);
			    dest = BNULL; /* SDuvan: Was a bug here! */
			    DeleteFile(destname);
			    res = RETURN_ERROR;
			}
		    }
		}
	    }
	    else
	    {
		PostFailure(Fatal(),
			    "Can't lock destination drawer \"%s\"",destname);
		res = RETURN_ERROR;
	    }
	}
	

	D(bug("CopyMain part 3b\n"));
	
	if(dest != BNULL)
	{
	    D(bug("Checking IsDir() on %s\n", destname));
	    
	    if(!IsDir(dest, destname))
	    {
		D(bug("%s was no dir; calling Printf()\n", destname));
		
		Printf("SortCopy: \"%s\" is not a directory.\n", destname);
		res = RETURN_ERROR;
		
		D(bug("Unlocking dest lock\n"));
		
		UnLock(dest);
		
		D(bug("Dest unlocked\n"));
		
		dest = BNULL;
	    }
	}
	
	D(bug("CopyMain part 3c\n"));
	
	if(dest)
	{
	    CurrentDir(sourcelock);
	    res = FillDirList(sourcename, &dirlist);
	    
	    if(res == 0)
	    {
		D(bug("Calling MainCopyProcess()\n"));
		res = MainCopyProcess(sourcelock, dest, &dirlist, &copylist);
	    }

	    UnLock(dest);
	} /* of if dest */
    }
    
    FreeDirList(&dirlist, TRUE);
    FreeDirList(&copylist, TRUE);
    FreeDirList(&resolvelist, TRUE);

    CurrentDir(sourcelock);

    return res;
}

///
/// FillDirList
ULONG FillDirList(char *pattern, struct MinList *dirlist)
{
    ULONG res = RETURN_OK;
    LONG  err;
    BOOL  found = FALSE;
    
    ap.ap_BreakBits = 0;
    ap.ap_Strlen = 0;
    
    if(!(err = MatchFirst(pattern, &ap)))
    {
	do {
	    found = TRUE;
	    err = 0;

	    if(CheckAbort())
		break;
	    
	    if(!AddMatch(&ap, dirlist))
	    {
		res = RETURN_WARN;
		break;
	    }
	    
	    ap.ap_Flags &= ~APF_DODIR;
	    err = MatchNext(&ap);
	    
	} while(!err);

	if(err != ERROR_NO_MORE_ENTRIES && err)
	{
	    PostFailure(err, "Can't resolve pattern \"%s\"", pattern);
	    res = RETURN_ERROR;
	}
	else if(!found)
	{
	    PostFailure(ERROR_OBJECT_NOT_FOUND, "No information for \"%s\"",
			pattern);
	    res = RETURN_WARN;
	}
    }
    else
    {
	PostFailure(err, "Can't resolve pattern \"%s\"", pattern);
	res = RETURN_ERROR;
    }
    
    MatchEnd(&ap);
    
    return res;
}

///
/// MainCopyProcess
ULONG MainCopyProcess(BPTR source, BPTR dest, struct MinList *dirlist,
		      struct MinList *copylist)
{
    ULONG res = RETURN_OK;
    
    CurrentDir(source);
    
    if(CheckAbort())
	return res;
    
    /* Create a copy of it */
    DupDirList(dirlist, copylist);

    /* And now for the heavy stuff */
    if(CheckAbort())
	return res;
    
    /* Copy the list contents */
    if(!(CopyEntryList(dirlist, dest)))
	res = RETURN_WARN;

    /* Move all the other stuff to the resolvelist */
    FreeDirList(dirlist, FALSE);

    if(CheckAbort())
	return res;

    /* create now all links */
    if((!nocopy) && (resolvelist.mlh_Head->mln_Succ != NULL))
    {
	if(!quieter)
	    Printf("\nResolving outstanding links...\n\n");
	
	if(!(ResolveLinks(&resolvelist,dest)))
	    res = 4;
	
	if(!quieter)
	    Printf("\n");
    }
    
    FreeDirList(&resolvelist,TRUE);
    
    if(CheckAbort())
	return res;
    
    /* Set now the archive bits in the source drawer */
    if(seta && (failcondition == 0) && (!nocopy))
    {
	if(DupDirList(copylist, dirlist))
	{
	    if(!quieter)
		Printf("\n\nSetting the archive bits, please wait...\n");
	    
	    if(!(SetAList(BNULL, dirlist)))
		res = 3;
	}
    }
    
    FreeDirList(dirlist, TRUE);
    
    if(CheckAbort())
	return res;
    
    /* delete now the source files if the user selected this */
    if(deletecopied && (res == 0) && (!nocopy))
    {
	if(failcondition)
	{
	    Printf("\nA fail condition occured while copying. The deletion"
		   "of files is therefore disabled for security.\n");
	}
	else
	{
	    if(!quieter)
		Printf("\nRemoving old files...\n\n");
	    
	    if(!(DeleteDirList(BNULL, copylist, FALSE)))
		res = 2;
	}
    }
    
    CurrentDir(source);
    
    return res;
}

///
/// CopyEntryList
BOOL CopyEntryList(struct MinList *dirlist, BPTR dest)
{
    struct DirEntry *de, *next;
    
    BOOL  fine = TRUE;
    

    /* Copy all files, create directories and setup dummies for the
       linked files and directories */
    if(!nocopy)
    {
	if(!CopyDir(BNULL, dest, dirlist))
	{
	    fine = FALSE;
	}
    }

    /* Purge the destination: Not good if we gave a pattern... */
    
    /* First free all the files and get the locks for the links */
    for(de = (struct DirEntry *)(dirlist->mlh_Head); de->de_next; )
    {
	next = de->de_next;
	
	if(de->de_flags & (DEF_SOFTLINK | DEF_HARDLINK))
	{
	    Remove((struct Node *)de);
	    PrepareLink(de, de->de_sourcelock, dest);
	}
	else
	{
	    if(!(de->de_flags & DEF_DIR))
	    {
		Remove((struct Node *)de);
		FreeDirEntry(de, TRUE);
	    }
	}
	
	de = next;
    }
    
    /* Now copy the subdirectories recursively */
    for(de = (struct DirEntry *)(dirlist->mlh_Head); de->de_next; )
    {
	next = de->de_next;
	
	if(CheckAbort())
	    break;
	
	if(!(de->de_flags & DEF_FORGET))
	{
	    if(de->de_flags & DEF_DIR)
	    {
		if(!SetRootLock(de))
		    fine = FALSE;
		
		if(!CopySubDirectory(de, de->de_sourcelock, dest))
		    fine = FALSE;
	    }
	}
	
	SetRootLock(NULL);
	Remove((struct Node *)de);
	FreeDirEntry(de, TRUE);
	de = next;
    }
    
    FreeDirList(dirlist, FALSE);
    
    return fine;
}

///
/// AddMatch
BOOL AddMatch(struct AnchorPath *ap, struct MinList *dirlist)
{
    struct DirEntry *de;
    
    BPTR  oldlock;
    LONG  protection;
    
    protection = ap->ap_Info.fib_Protection;
    
    if(!(de = AllocDirEntry(&(ap->ap_Info))))
	return FALSE;
    
    oldlock = CurrentDir(ap->ap_Current->an_Lock);
    
    de->de_sourcelock = DupLock(ap->ap_Current->an_Lock);
    
    if(de->de_sourcelock != BNULL)
    {	
	/* We need to call this first to find out if this
	   is a directory or not.... */
	
	FillDirEntry(de, &(ap->ap_Info));
	
	if((de->de_flags & DEF_DIR) == 0)
	{
	    if(onlya && (protection & FIBF_ARCHIVE))
		de->de_flags |= DEF_FORGET;
	}
	else if(!sourcewild)
	{
	    de->de_flags |= DEF_FLATDIR;
	}
	
	EnqueueEntry(dirlist,de);
    } 
    else
    {
	PostFailure(IoErr(), "Can't lock source directory \"%s\"",
		    ap->ap_Info.fib_FileName);
	FreeDirEntry(de, TRUE);
	de = NULL;
    }
    
    CurrentDir(oldlock);
    
    if(de != NULL)
	return TRUE;
    else
	return FALSE;
}

///
///IsDir
BOOL IsDir(BPTR lock, char *name)
{
    if(!Examine(lock, &fib))
    {
	PostFailure(Fatal(), "Can't examine object \"%s\"", name);
	return FALSE;
    }
    
    /* No need to worry about links.... they are already resolved when we
       Lock the object.. */
    
    if(fib.fib_DirEntryType > 0)
	return TRUE;
    else
	return FALSE;
}

///
///OFlush
void OFlush(void)
{
    Flush(Output());
}

///
/// Abort
void Abort(void)
{
    abortFlag = TRUE;
    PrintFault(ERROR_BREAK, NULL);
}

///
/// CheckAbort
BOOL CheckAbort(void)
{
    if(abortFlag)
	return TRUE;
    
// FIXME:  Take care of stack check
#ifndef __AROS__
    if((getreg(REG_A7) - tclower) < 1024)
    {
	PrintFault(ERROR_TOO_MANY_LEVELS, "SortCopy failed");
	Printf("Increase the stack size with \"STACK\" and retry.\n");

	abortFlag = TRUE;

	return TRUE;
    }
#endif

    if(CheckSignal(SIGMASK))
    {
	Abort();

	return TRUE;
    }

    return FALSE;
}

///
/// DoIndent
void DoIndent(void)
{
    ULONG count = depth;

    while(count-- > 0)
	Printf(" ");

}

///
///PostFailure
void PostFailure(LONG error, char *failtext, ...)
{
    va_list args;
    
    va_start(args, failtext);

    if(error)
    {

#ifdef  __AROS__
	VNewRawDoFmt(failtext, RAWFMTFUNC_STRING, failbuffer, args);
#else
	tinyvsprintf(failbuffer, failtext, args);
#endif

	Fault(error, failbuffer, buffer, BUFFERSIZE - 1);
	Printf("\nSortCopy : %s \n", buffer);
	failcondition = TRUE;
    }
    
    va_end(args);
}

///
///FindSoftOrginal
BOOL FindSoftOrginal(struct DirEntry *de)
{
    BPTR dirlock, objectlock = BNULL;

    struct FileLock *flock;

    BOOL  fine = FALSE;
    LONG  err  = 0;

    de->de_flags |= DEF_NORESOLVE;

    if(scount > MAXSOFTNEST)
    {
	SetIoErr(ERROR_TOO_MANY_LEVELS);
	return FALSE;
    }

    dirlock = CurrentDir(BNULL);
    CurrentDir(dirlock);

// NOTICE: Using lock internals here

    flock = (struct FileLock *)(BADDR(dirlock));
    
    if(flock->fl_Task == NULL)
    {            /* softlink at NIL:? Bizarre! */
	SetIoErr(ERROR_OBJECT_WRONG_TYPE);
	return FALSE;
    }

    if(!ReadLink(flock->fl_Task, dirlock, de->de_name, buffer, BUFFERSIZE))
	return FALSE;


    /* Locate the object the link is going to */
    if(!(dirlock = Lock(buffer, SHARED_LOCK)))
	return FALSE;
    
    if(Examine(dirlock, &fib))
    {
	objectlock = ParentDir(dirlock);
	
	if(objectlock != BNULL)
	{
	    strcpy(de->de_linkname, FilePart(buffer));
	    
	    /* Fill in file name */
	    fine = TRUE;
	}
    }
    
    err = IoErr();
    UnLock(dirlock);
    
    if(!fine)
    {
	SetIoErr(err);
	return FALSE;
    }
    
    CurrentDir(objectlock);
    switch(fib.fib_DirEntryType)
    {
    case ST_SOFTLINK:
	scount++;
	fine = FindSoftOrginal(de);
	err = IoErr();
	UnLock(objectlock);
	scount--;
	break;

    case ST_LINKDIR:
	de->de_flags |= DEF_DIR;

	/* läuft hier hinein! */

    case ST_LINKFILE:
	fine = FindHardOrginal(de);
	err = IoErr();
	UnLock(objectlock);
	break;

    default:
	if(fib.fib_DirEntryType > 0)
	    de->de_flags |= DEF_DIR;

	de->de_linklock = objectlock;
	err = 0;
	fine = TRUE;
	break;
    }

    CurrentDir(dirlock);

    if(fine)
    {
	de->de_flags &= ~DEF_NORESOLVE;
    }
    
    SetIoErr(err);
    return fine;
    
}

///
///FindHardOrginal
BOOL FindHardOrginal(struct DirEntry *de)
{
    BPTR lock;      /* Resolve the link by getting the name */
    LONG err;
    
    if(!(lock = Lock(de->de_name, SHARED_LOCK)))
	return FALSE;
    
    if(Examine(lock, &fib))
    {
	de->de_linklock = ParentDir(lock);

	if(de->de_linklock != BNULL)
	{
	    strcpy(de->de_linkname, fib.fib_FileName);
	    UnLock(lock);

	    return TRUE;
	}
    }

    err = IoErr();
    UnLock(lock);
    SetIoErr(err);

    return FALSE;
	
/* old stuff, way too complicated...

   fine = NameFromLock(lock, buffer, BUFFERSIZE);
   err = IoErr();
   UnLock(lock);
   
   if(!fine)
   {
       SetIoErr(err);
       return FALSE;
   }

   strcpy(de->de_linkname, FilePart(buffer));
   *PathPart(buffer) = 0;
   de->de_linklock = Lock(buffer,SHARED_LOCK);

   if(!(de->de_linklock))
       return FALSE;
   else
       return TRUE;
   
   */
}

///
///SetAFlag
BOOL SetAFlag(struct DirEntry *de)
{
    BPTR  lock;
    LONG  err = 0;
    BPTR  oldlock = BNULL;
    BPTR  parent;

    if(CheckAbort())
	return FALSE;
    
    if(de->de_sourcelock != BNULL)
	oldlock = CurrentDir(de->de_sourcelock);
    
    lock = Lock(de->de_name, SHARED_LOCK);

    if(lock != BNULL)
    {
	if(Examine(lock, &fib))
	{
	    parent = (BPTR)(-1);

	    /* check if this the root directory. If so, don't set the A bit */
	    if(fib.fib_DirEntryType > 0)
	    {
		parent = ParentDir(lock);

		if(parent != BNULL)
		{
		    UnLock(parent);
		}
		else
		{
		    err = IoErr();
		}
	    }

	    if(parent != BNULL)
	    {
		if((!(fib.fib_Protection & FIBF_ARCHIVE)) ||
		   (de->de_flags & (DEF_SOFTLINK | DEF_HARDLINK)))
		{
		    if(!SetProtection(de->de_name, 
				      fib.fib_Protection | FIBF_ARCHIVE))
			err = IoErr();
		}
	    }
	}
	else
	    err = IoErr();

	UnLock(lock);
    }
    else
	err = IoErr();

    if(de->de_sourcelock)
	CurrentDir(oldlock);

    if(err)
    {
	PostFailure(err,"Can't set the archive bit of \"%s\"", de->de_name);
	return FALSE;
    }
    
    return TRUE;
}

///
///CloneFlags
BOOL CloneFlags(struct DirEntry *de)
{
    /* Die Directory wurde nicht angelegt, sondern der Inhalt
       hineinkopiert */
    if((de->de_flags & DEF_DIR) && (de->de_flags & (DEF_FLATDIR | DEF_REMOVED)))
	return TRUE;

    if(!SetComment(de->de_name, de->de_comment))
    {
	PostFailure(IoErr(), "Can't set the file comment of \"%s\"",
		    de->de_name);

	return FALSE;
    }
    
    if(!SetFileDate(de->de_name, &(de->de_date)))
    {
	PostFailure(IoErr(), "Can't set the file date of \"%s\"",
		    de->de_name);

	return FALSE;
    }

    if(!SetProtection(de->de_name,de->de_protection))
    {
	PostFailure(IoErr(), "Can't set protection flags of \"%s\"",
		    de->de_name);

	return FALSE;
    }

    return TRUE;
}

///
///EnqueueEntry
void EnqueueEntry(struct MinList *dirlist, struct DirEntry *de)
{
    struct DirEntry *cmp;
    
    /* Suche jetzt die richtige Einfügeposition für die neue Node */
    for(cmp = (struct DirEntry *)(dirlist->mlh_Head); cmp->de_next;
	cmp = cmp->de_next)
    {
	if(Strnicmp(cmp->de_name, de->de_name, 108) > 0) 
	    break;
    }

    if(cmp->de_next == NULL)
    {
	AddTail((struct List *)dirlist, (struct Node *)de);
    }
    else
    {
	cmp = cmp->de_pred;

	if(cmp->de_pred == NULL)
	    AddHead((struct List *)dirlist, (struct Node *)de);
	else
	    Insert((struct List *)dirlist, (struct Node *)de,
		   (struct Node *)cmp);
    }
}

///
///ReadDir
/* Eine komplette Directory zusammenbauen. Directory ist in BPTR lock */
BOOL ReadDir(BPTR lock, struct MinList *dirlist, char *dirname)
{
    struct DirEntry *de = NULL;
    LONG             protection;
    
    CurrentDir(lock);

    if(!Examine(lock, &dirfib))
    {
	PostFailure(Fatal(), "Can't examine the directory \"%s\"", dirname);

	return FALSE;
    }
    
    /* So, jetzt können wir den Rest der Directory lesen */
    for(;;)
    {
	if(CheckAbort())
	    return FALSE;
	
	if(!ExNext(lock, &dirfib))
	    break;
	
	protection = dirfib.fib_Protection;
	
	if(!(de = AllocDirEntry(&dirfib)))
	    return FALSE;
	
	FillDirEntry(de, &dirfib);

	/* selbst wenn es Probleme gibt, das Teil mit aufnehmen */
	
	if((de->de_flags & DEF_DIR) == 0)
	{
	    if(onlya && (protection & FIBF_ARCHIVE))
		de->de_flags |= DEF_FORGET;
	}
	
	EnqueueEntry(dirlist, de);
    }
    
    if(IoErr() == ERROR_NO_MORE_ENTRIES)
    {
	return TRUE;
    }
    else
    {
	PostFailure(Fatal(), "Can't examine the directory \"%s\"", dirname);

	return FALSE;
    }
}

///
/// CopyFile
BOOL CopyFile(BPTR source, BPTR dest, char *name)
{
    ULONG size;
    
    if(ExamineFH(source, &fib))
    {
	size = fib.fib_Size;
    }
    else
    {
	/* Works like this, too, but takes *huge* amount of memoy */
	size = 0xffffffff;
    }
    
#ifdef USE_AROS_PACKETS
    return AsyncCopy_AROS(source, dest, size, name);
#else
    return AsyncCopy(source, dest, size, name);
#endif
}

///
/// MustUpdate
BOOL MustUpdate(BPTR destlock, struct DirEntry *de)
{
    BPTR  dest;
    BOOL  fine = TRUE;

    CurrentDir(destlock);
    dest = Lock(de->de_name, SHARED_LOCK);

    if(dest != BNULL)
    {
	if(!Examine(dest, &fib))
	{
	    UnLock(dest);
	    PostFailure(Fatal(), "Can't examine the destination file \"%s\"",
			de->de_name);
	    de->de_flags |= DEF_FORGET;
	    fine = FALSE;
	}
	else
	{
	    if(fib.fib_Size == de->de_length)
	    {
		if(CompareDates(&(fib.fib_Date), &(de->de_date)) <= 0)
		    fine = FALSE;    /* no need to copy the file,
					we suppose they are identical */
	    }
	}

	UnLock(dest);
    }
    else
    {
	if(IoErr() != ERROR_OBJECT_NOT_FOUND)
	{
	    PostFailure(Fatal(), "Can't lock the destination file \"%s\"",
			de->de_name);
	    de->de_flags |= DEF_FORGET;
	    fine = FALSE;
	}
    }
    
    return fine;
}

///
/// CopyFileEntry
/* Ein ganzes File kopieren. Achtung! Das muß auch wirklich ein File
   sein! */
BOOL CopyFileEntry(struct DirEntry *de, BPTR sourcelock, BPTR destlock)
{
    BOOL  fine = TRUE;
    BPTR  source, dest, tmp;
    LONG  err;
    struct MinList dirlist;

    if((!update) || MustUpdate(destlock, de))
    {
	CurrentDir(sourcelock);
	source = Open(de->de_name, MODE_OLDFILE);
	
	if(source == BNULL)
	{
	    PostFailure(IoErr(), "Can't open the source file \"%s\"",
			de->de_name);
	    de->de_flags |= DEF_FORGET;

	    return FALSE;
	}

	CurrentDir(destlock);

	/* Now check the destination object. Delete it if required */
	if(overwrite)
	{
	    tmp = Lock(de->de_name, EXCLUSIVE_LOCK);

	    if(tmp != BNULL)
	    {
		if(ChangeMode(CHANGE_LOCK, tmp, SHARED_LOCK))
		{
		    /* Make it delete-able */
		    if(SetProtection(de->de_name, 0))
		    {
			if(Examine(tmp, &fib))
			{
			    switch(fib.fib_DirEntryType)
			    {
			    default:
				if(fib.fib_DirEntryType > 0)
				{
				    Printf("found obsolete directory, deleting.\n");
				    depth++;
				    NewList((struct List *)&dirlist);

				    if(ReadDir(tmp, &dirlist, de->de_name))
				    {
					fine = DeleteDirList(tmp, &dirlist,
							     TRUE);
				    }
				    else
					fine = FALSE;

				    CurrentDir(destlock);
				    FreeDirList(&dirlist, TRUE);
				    depth--;

				    if(!fine)
					break;
				}
				else
				    break;

			    case ST_SOFTLINK:
			    case ST_LINKDIR:
				UnLock(tmp);
				tmp = BNULL;
				fine = DeleteFile(de->de_name);
				break;
			    }
			}
			else
			    fine = FALSE;
		    }
		    else 
			fine = FALSE;
		}
		else
		    fine = FALSE;
		
		if(tmp)
		{
		    err = IoErr();
		    UnLock(tmp);
		    SetIoErr(err);
		}
	    }
	    else
	    {
		if(IoErr() != ERROR_OBJECT_NOT_FOUND)
		    fine = FALSE;
	    }
	    
	    if(!fine)
	    {
		err = IoErr();
		Close(source);
		PostFailure(err, "Can't delete the old entry \"%s\"",
			    de->de_name);
		de->de_flags |= DEF_FORGET; /* nicht kopiert -> nicht löschen */

		return FALSE;
	    }
	}
	
	dest = Open(de->de_name, MODE_NEWFILE);

	if (dest == BNULL)
	{
	    err = IoErr();
	    Close(source);
	    PostFailure(err, "Can't create the destination file \"%s\"",
			de->de_name);
	    de->de_flags |= DEF_FORGET;

	    return FALSE;
	}

	fine = CopyFile(source, dest, de->de_name);
	Close(dest);
	Close(source);

	if(!fine)
	{
	    CurrentDir(destlock);
	    DeleteFile(de->de_name);
	    PostFailure(Fatal(), "Can't copy the file \"%s\"", de->de_name);
	    de->de_flags |= DEF_FORGET;  /* nicht kopiert -> nicht löschen */

	    return FALSE;
	}
	else
	{
	    de->de_flags |= DEF_CREATED;

	    if(!quiet)
		Printf("\n");
	}
    }
    else
    {
	if(!quiet)
	    Printf("(up to date)\n");
    }
    
    return TRUE;
}

///
/// CopyDirEntry
BOOL CopyDirEntry(struct DirEntry *de, BPTR sourcelock, BPTR destlock)
{
    BPTR  newlock;
    BPTR  dest;
    LONG  err = 0;

    CurrentDir(destlock);

    newlock = Lock(de->de_name, SHARED_LOCK);

    if(newlock != BNULL)
    {
	if(!IsDir(newlock, de->de_name))
	{
	    UnLock(newlock);

	    if(overwrite)
	    {
		/* This is NOT a directory, erase it now */
		SetProtection(de->de_name, 0);
		
		if(!(DeleteFile(de->de_name)))
		{
		    PostFailure(IoErr(), 
				"Can't remove \"%s\" to make room for a new directory",
				de->de_name);
		    de->de_flags |= DEF_FORGET;

		    return TRUE;
		}
	    }
	    else
	    {
		PostFailure(ERROR_OBJECT_WRONG_TYPE, 
			    "Destination \"%s\" is not a directory",
			    de->de_name);
		de->de_flags |= DEF_FORGET;

		return TRUE;
	    }

	    newlock = BNULL;
	}
    }
    else
	err = IoErr();

    if(newlock != BNULL)
    {
	UnLock(newlock);
	
	/* This is not good in the case of a lock !*/
	if(de->de_flags & (DEF_SOFTLINK | DEF_HARDLINK))
	{
	    Printf("Warning: The name \"%s\" of the link is already in use.\n",
		   de->de_name);
	    de->de_flags |= DEF_NORESOLVE | DEF_FORGET;  /* Diesen Link nicht erzeugen */

	    return FALSE;
	}
	else if(!quiet)
	{
	    Printf("\n");
	}
    }
    else
    {
	if(err != ERROR_OBJECT_NOT_FOUND && err != 0)
	{
	    PostFailure(IoErr(), "Can't lock the directory \"%s\"", 
			de->de_name);
	    de->de_flags |= DEF_FORGET;

	    return FALSE;
	}
	else
	{
	    /* Don't create a directory in the case we replace it later
	       by a link */
	    if(de->de_flags & (DEF_SOFTLINK | DEF_HARDLINK))
	    {
		dest = Open(de->de_name, MODE_NEWFILE);

		if(dest == BNULL)
		{
		    PostFailure(Fatal(), 
				"Can't create dummy link directory \"%s\"",
				de->de_name);
		    de->de_flags |= DEF_FORGET; /* But try to resolve later */
		} 
		else
		{
		    de->de_flags |= DEF_CREATED;

		    if(!quiet)
			Printf("(created)\n");

		    Close(dest);
		}
	    }
	    else
	    {
		newlock = CreateDir(de->de_name);

		if(newlock == BNULL)
		{
		    PostFailure(Fatal(), "Can't create new directory \"%s\"",
				de->de_name);
		    de->de_flags |= DEF_FORGET;

		    return FALSE;
		}
		else
		{
		    de->de_flags |= DEF_CREATED;

		    if(!quiet)
			Printf("(created)\n");

		    UnLock(newlock);
		}
	    }
	}
    }
    
    return TRUE;
}

///
/// CopyEntry
BOOL CopyEntry(struct DirEntry *de, BPTR sourcelock, BPTR destlock)
{
    BPTR oldlock;

    if(de->de_flags & DEF_FORGET ||
       ((de->de_flags & DEF_DIR) && (de->de_flags & DEF_FLATDIR)))
	return TRUE;

    if(!quiet)
    {
	DoIndent();
	Printf("%s...",de->de_name);
	OFlush();
    }

    if(de->de_flags & DEF_DIR)
    {
	if(!CopyDirEntry(de, sourcelock, destlock))
	    return FALSE;
    }
    else
    {
	if(!CopyFileEntry(de, sourcelock, destlock))
	    return FALSE;
    }
    
    if(!(de->de_flags & DEF_FORGET))
    {
	if((de->de_flags & (DEF_DIR | DEF_SOFTLINK | DEF_HARDLINK)) == 0)
	{
	    oldlock = CurrentDir(destlock);
	    CloneFlags(de);
	    CurrentDir(oldlock);
	}
    }
    
    return TRUE;
}

///
/// SetRootLock
BOOL SetRootLock(struct DirEntry *de)
{
    BOOL  fine = TRUE;
    BPTR  oldlock;

    if(rootlock)
	UnLock(rootlock);

    rootlock = BNULL;

    if(de != NULL)
    {
	if(de->de_flags & DEF_DIR)
	{
	    /* Are we copying the root of a tree ? */
	    UnLock(rootlock);
	    oldlock = CurrentDir(de->de_sourcelock);
	    rootlock = Lock(de->de_name, SHARED_LOCK);

	    if(!rootlock)
	    {
		PostFailure(Fatal(), "Can't look the root directory \"%s\"\n",
			    de->de_name);
		fine = FALSE;
	    }

	    CurrentDir(oldlock);
	}
    }
    
    return fine;
}

///
/// CopyDir
/* Eine Directory nach Vorschrift kopieren */
BOOL CopyDir(BPTR sourcelock, BPTR destlock, struct MinList *dirlist)
{
    struct DirEntry *de;
    BOOL fine = TRUE;

    for(de = (struct DirEntry *)(dirlist->mlh_Head); de->de_next; 
	de = de->de_next)
    {
	if(CheckAbort())
	{
	    return fine;
	}
	
	if(sourcelock == BNULL)
	{
	    if(!SetRootLock(de))
		fine = FALSE;
	}
	
	if(!(CopyEntry(de, (de->de_sourcelock) ? (de->de_sourcelock) :
		       sourcelock, destlock)))
	    fine = FALSE;
    }
    
    if(sourcelock == BNULL)
	SetRootLock(BNULL);
    
    return fine;
}

///
/// DupDirList
BOOL DupDirList(struct MinList *source, struct MinList *dest)
{
    struct DirEntry *se, *de;

    NewList((struct List *)dest);

    for(se = (struct DirEntry *)(source->mlh_Head); se->de_next;
	se = se->de_next)
    {
	if(!(de = DupDirEntry(se)))
	{
	    FreeDirList(dest, TRUE);

	    return FALSE;
	}

	AddTail((struct List *)dest, (struct Node *)de);
    }
    
    return TRUE;
}

///
/// DupDirEntry
struct DirEntry *DupDirEntry(struct DirEntry *se)
{
    struct DirEntry *de;

    de = AllocMem(sizeof(struct DirEntry), MEMF_PUBLIC);

    if(!de)
    {
	abortFlag = TRUE;
	PostFailure(Fatal(), "Can't allocate directory buffer");

	return NULL;
	/* Liste der bestehenden DirEntries wird von oben abgeräumt */
    }
    
    /* Kopiere jetzt die Daten */
    memcpy(de, se, sizeof(struct DirEntry));
    de->de_sourcelock = BNULL;
    de->de_linklock   = BNULL;
    de->de_dirlock    = BNULL;                       /* Erstmal löschen */
    NewList((struct List *)(&(de->de_sublist)));    /* Wird nicht geduppt */
    
    if(se->de_sourcelock == 0 ||
       (de->de_sourcelock = DupLock(se->de_sourcelock)))
    {
	if(se->de_linklock == 0 || (de->de_linklock = DupLock(se->de_linklock)))
	{
	    if(se->de_dirlock == 0 || (de->de_dirlock=DupLock(se->de_dirlock)))
	    {
		return de;
	    }
	}
    }
    
    PostFailure(Fatal(), "Can't duplicate directory buffer");
    FreeDirEntry(de, TRUE);

    return NULL;
}

///
/// AllocDirEntry
struct DirEntry *AllocDirEntry(struct FileInfoBlock *fib)
{
    struct DirEntry *de;

    de = AllocMem(sizeof(struct DirEntry), MEMF_PUBLIC);

    if(!de)
    {
	abortFlag = TRUE;
	PostFailure(Fatal(), "Can't allocate directory buffer");

	return NULL;
	/* Liste der bestehenden DirEntries wird von oben abgeräumt */
    }
    
    /* Kopiere jetzt die Daten */
    strcpy(de->de_name, fib->fib_FileName);
    strcpy(de->de_comment, fib->fib_Comment);
    de->de_protection = fib->fib_Protection;
    de->de_date = fib->fib_Date;
    de->de_flags = 0L;
    de->de_sourcelock = BNULL;
    de->de_linklock = BNULL;
    de->de_dirlock = BNULL;    /* Muß vom Caller ausgefüllt werden */
    de->de_linkname[0] = 0;
    de->de_length = fib->fib_Size;
    NewList((struct List *)(&(de->de_sublist)));
    
    return de;
}

///
/// FillDirEntry
BOOL FillDirEntry(struct DirEntry *de, struct FileInfoBlock *fib)
{
    BOOL fine = TRUE;

    switch(fib->fib_DirEntryType)
    {
    case ST_SOFTLINK:
	de->de_flags |= DEF_SOFTLINK;

	/* We never set this up as a directory */
	fine = FindSoftOrginal(de);
	break;

    case ST_LINKDIR:
	de->de_flags |= DEF_DIR;

	/* Fall through */
    case ST_LINKFILE:
	de->de_flags |= DEF_HARDLINK;
	fine = FindHardOrginal(de);
	break;

    default:
	if(fib->fib_DirEntryType > 0)
	    de->de_flags |= DEF_DIR;

	break;
    }

    if(!fine)
    {
	PostFailure(IoErr(), "Can't resolve link \"%s\"", de->de_name);
	de->de_flags |= DEF_NORESOLVE;
    }
    
    return fine;
}

///
///FreeDirEntry
void FreeDirEntry(struct DirEntry *de, BOOL locks)
{
    FreeDirList(&(de->de_sublist), locks);
    UnLock(de->de_sourcelock);
    UnLock(de->de_dirlock);
    UnLock(de->de_linklock);
    FreeMem(de, sizeof(struct DirEntry));
}

///
///FreeDirList
void FreeDirList(struct MinList *dirlist, BOOL locks)
{
    struct DirEntry *de, *next;

    for(de = (struct DirEntry *)(dirlist->mlh_Head); de->de_next; )
    {
	next = de->de_next;
	Remove((struct Node *)de);

	if((de->de_flags & (DEF_SOFTLINK | DEF_HARDLINK)) && (locks == FALSE))
	{
	    AddTail((struct List *)&resolvelist, (struct Node *)(de));
	}
	else
	{
	    FreeDirEntry(de, locks);
	}

	de = next;
    }
}

///
/// DirEmpty
BOOL DirEmpty(BPTR lock, char *name)
{
    if(Examine(lock, &fib))
    {
	if(ExNext(lock, &fib))
	    return FALSE;
	
	if(IoErr() == ERROR_NO_MORE_ENTRIES)
	    return TRUE;

	PostFailure(IoErr(), "Can't inspect directory \"%s\"", name);

	return FALSE;
    }
    
    PostFailure(IoErr(), "Can't inspect directory \"%s\"", name);

    return FALSE;
}

///
/// KillForce
BOOL KillForce(char *name, BOOL force)
{
    BPTR lock;

    lock = Lock(name, EXCLUSIVE_LOCK);

    if(lock != BNULL)
    {
	UnLock(lock);

	if(force)
	    SetProtection(name, 0);

	/* We'll see if this works... */
	
	if(DeleteFile(name))
	    return TRUE;

	PostFailure(IoErr(), "Can't remove empty directory \"%s\"", name);

	return FALSE;
    }

    if(IoErr() != ERROR_OBJECT_IN_USE)
    {
	PostFailure(IoErr(), "Can't remove empty directory \"%s\"", name);
    }

    return FALSE;
}

///
/// CopySubDirectory
BOOL CopySubDirectory(struct DirEntry *de, BPTR source, BPTR dest)
{
    BPTR  newdest = BNULL;
    BPTR  destlock;
    BPTR  newsource;
    BOOL  fine;

    /* find new destination lock of the directory */
    CurrentDir(dest);

    if(de->de_flags & DEF_FLATDIR)
    {
	destlock = dest;
    }
    else
    {
	newdest = Lock(de->de_name, SHARED_LOCK);

	if(newdest == BNULL)
	{
	    if((IoErr() != ERROR_OBJECT_NOT_FOUND) || (!nocopy))
		PostFailure(Fatal(), "Can't lock the new directory \"%s\"",
			    de->de_name);

	    de->de_flags |= DEF_FORGET;

	    return FALSE;
	}
	else
	{
	    destlock = newdest;
	}
    }
    
    /* find the new source of the directory */
    CurrentDir((de->de_sourcelock) ? (de->de_sourcelock) : (source));
    newsource = Lock(de->de_name, SHARED_LOCK);

    if(newsource == BNULL)
    {
	PostFailure(IoErr(), "Can't lock the source directory \"%s\"",
		    de->de_name);
	de->de_flags |= DEF_FORGET;
	UnLock(newdest);

	return FALSE;
    }

    if(!quieter) 
    {
	if(!quiet)
	    Printf("\n");

	DoIndent();
	Printf("%sing the directory \"%s\":\n", (nocopy) ? ("Check") :("Copy"),
	       de->de_name);
    }

    depth++;
    
    fine = RecursiveCopy(newsource, destlock, &(de->de_sublist), de->de_name);
    UnLock(newsource);

    if((de->de_flags & DEF_CREATED) && 
       (de->de_protection & FIBF_ARCHIVE) && onlya)
    {
	/* Falls erzeugt, überprüfe, ob leer. In diesem Falle löschen */
	if((!leaveempty) && (!nocopy) && DirEmpty(newdest, de->de_name))
	{
	    UnLock(newdest);
	    newdest = BNULL;
	    CurrentDir(dest);

	    if(KillForce(de->de_name, TRUE))
	    {
		if(!quieter)
		{
		    DoIndent();
		    Printf("\"%s\" is empty. Removed.\n", de->de_name);
		    de->de_flags |= DEF_REMOVED;
		}
	    }
	}
    }
    
    if(fine)
    {
	CurrentDir(dest);
	if(!CloneFlags(de))  /* smart enough not to set the flags if REMOVED or FLATDIR */
	    fine = FALSE;
    }
    
    depth--;

    if(!quiet)
    {
	DoIndent();
	Printf("...done with directory \"%s\".\n", de->de_name);
    }
    
    UnLock(newdest);

    return fine;
}

///
///PrepareLink
BOOL PrepareLink(struct DirEntry *de, BPTR source, BPTR dest)
{
    BOOL  fine = TRUE;

    if(!(de->de_sourcelock))
    {
	if(rootlock == BNULL)
	{
	    PostFailure(ERROR_OBJECT_NOT_FOUND,
			"The root lock of the link \"%s\" is not set.\n",
			de->de_name);
	    fine = FALSE;
	}
	else if(!(de->de_sourcelock = DupLock(rootlock)))
	{
	    fine = FALSE;
	    PostFailure(IoErr(),
			"Can't duplock root dir of link \"%s\"",de->de_name);
	}
    }

    if(!(de->de_dirlock = DupLock(dest)))
    {
	PostFailure(IoErr(), "Can't duplock destination dir of link \"%s\"",
		    de->de_name);
	fine = FALSE;
    }

    if(!fine)
	de->de_flags |= DEF_NORESOLVE;

    AddTail((struct List *)&resolvelist, (struct Node *)(de));

    return fine;
}

///
///RecursiveCopy
BOOL RecursiveCopy(BPTR source, BPTR dest, struct MinList *dirlist,
		   char *dirname)
{
    struct DirEntry  *de, *next;
    BOOL              fine = TRUE;

    CurrentDir(source);

    if(CheckAbort())
	return TRUE;

    if(!ReadDir(source, dirlist, dirname))
    {
	FreeDirList(dirlist, FALSE);

	return FALSE;
    }

    if(CheckAbort())
    {
	FreeDirList(dirlist, FALSE);

	return TRUE;
    }

    /* Copy all files, create directories and setup dummies for the
       linked files and directories */
    if(!nocopy) 
    {
	if(!CopyDir(source, dest, dirlist))
	{
	    fine = FALSE;
	}
    }
    
    if(CheckAbort())
    {
	FreeDirList(dirlist, FALSE);

	return fine;
    }
    
    /* Purge the destination */
    if(purge)
    {
	if(!PurgeDir(source, dest, dirlist, dirname))
	{
	    fine = FALSE;
	}
    }
    
    if(CheckAbort())
    {
	FreeDirList(dirlist, FALSE);

	return fine;
    }
    
    /* First free all the files and get the locks for the links */
    for(de = (struct DirEntry *)(dirlist->mlh_Head); de->de_next; )
    {
	next = de->de_next;

	if(de->de_flags & (DEF_SOFTLINK | DEF_HARDLINK))
	{
	    Remove((struct Node *)de);
	    PrepareLink(de, source, dest);
	}
	else
	{
	    if(!(de->de_flags & DEF_DIR))
	    {
		Remove((struct Node *)de);
		FreeDirEntry(de, TRUE);
	    }
	}

	de = next;
    }
    
    /* Now copy the subdirectories recursively */
    for(de = (struct DirEntry *)(dirlist->mlh_Head); de->de_next; ) 
    {
	next = de->de_next;
	
	if(CheckAbort())
	    break;

	if(!(de->de_flags & DEF_FORGET))
	{
	    if(de->de_flags & DEF_DIR)
	    {
		if(!(CopySubDirectory(de, source, dest)))
		    fine = FALSE;
	    }
	}

	Remove((struct Node *)(de));
	FreeDirEntry(de, TRUE);
	de = next;
    }

    FreeDirList(dirlist, FALSE);

    return fine;
}

///
///PathRelative
BOOL PathRelative(BPTR relpath, BPTR path, char *name, char *buffer,
		  ULONG bufsize)
{
    char  *bufend;
    char  *slash;
    BPTR   lock, newlock;

    bufend = buffer + bufsize - strlen(name) - 1;

    if(bufend < buffer)
    {
	SetIoErr(ERROR_LINE_TOO_LONG);
	return FALSE;
    }

    strcpy(bufend, name);
    slash = NULL;
    lock = DupLock(path);

    if(lock==BNULL)
	return FALSE;

    for(;;)
    {
	if(SameLock(relpath, lock) == LOCK_SAME)
	    break;  /* If done */
	
	if(Examine(lock, &fib) == FALSE)
	{
	    UnLock(lock);
	    
	    return FALSE;                           /* Can't examine */
	}

	bufend--;
	slash = bufend;
	bufend = bufend - strlen(fib.fib_FileName);

	if(bufend < buffer)
	{
	    SetIoErr(ERROR_LINE_TOO_LONG);
	    UnLock(lock);

	    return FALSE;
	}

	strcpy(bufend, fib.fib_FileName);
	*slash = '/';
	newlock = ParentDir(lock);
	UnLock(lock);
	lock = newlock;
	
	if(newlock == BNULL)
	{
	    /* Either something got wrong or the object we are looking for is
	       not related to the relative path */
	    *slash = ':';

	    if(IoErr() == 0)
		SetIoErr(ERROR_OBJECT_NOT_FOUND);

	    return FALSE;
	}
    }
    
    /*
      if(slash)
      {
          newlock = ParentDir(lock);
          UnLock(lock);
    
	  if(newlock == BNULL)
	  {
	      if(IoErr() == 0)
	          *slash = ':';
	      else
	          return FALSE;
	  }
	  else
	      UnLock(newlock);
      }
    */

    strcpy(buffer, bufend);
    UnLock(lock);

    return TRUE;
}

///
///CreateLink
BOOL CreateLink(char *name, BPTR melock, char *linkname, BPTR linklock,
		ULONG soft)
{
    BPTR  lock;
    BOOL  unlock;
    
    /* First, get a lock to the object we are linking to */
    /* linkname MIGHT be NULL. In this case, we already have a lock */
    if(linkname)
    {
	CurrentDir(linklock);
	lock = Lock(linkname, SHARED_LOCK);

	if(lock == BNULL)
	{
	    PostFailure(IoErr(), "Can't lock the object \"%s\" the link is going to",
			linkname);

	    return FALSE;
	}

	unlock = TRUE;
    }
    else
    {
	lock = linklock;
	unlock = FALSE;
    }

    /* if this is a softlink, get the full name of the object */
    if(soft) 
    {
	if(!NameFromLock(lock, buffer, BUFFERSIZE))
	{
	    PostFailure(IoErr(), "Can't find the full name of \"%s\"", 
			linkname);

	    if(unlock)
		UnLock(lock);

	    return FALSE;
	}
    }

    /* Now find the place of the lock. Delete the placeholder */
    CurrentDir(melock);

    if(!DeleteFile(name))
    {
	PostFailure(IoErr(), "Can't delete the placeholder \"%s\"", name);

	if(unlock)
	    UnLock(lock);

	return FALSE;
    }

    if(soft)
    {
	if(!MakeLink(name, (SIPTR)buffer, TRUE))
	{
	    PostFailure(IoErr(), "Can't create the softlink \"%s\"", name);

	    if (unlock)
		UnLock(lock);

	    return FALSE;
	}
    }
    else
    {
	if(!MakeLink(name, (SIPTR)lock, FALSE))
	{
	    PostFailure(IoErr(), "Can't create the hardlink \"%s\"", name);

	    if(unlock)
		UnLock(lock);

	    return FALSE;
	}
    }
    
    if(unlock)
	UnLock(lock);
    
    return TRUE;
}

///
///ResolveLinks
BOOL ResolveLinks(struct MinList *list, BPTR dest)
{
    struct DirEntry *de;

    BOOL  fine, resolved;
    BPTR  destlock;
    BOOL  rc = TRUE;

    for(de = (struct DirEntry *)(list->mlh_Head); de->de_next; de = de->de_next)
    {
	if(CheckAbort())
	    break;
	
	if(!quiet)
	{
	    DoIndent();
	    Printf("%s...", de->de_name);
	    OFlush();
	}
	
	resolved = FALSE;

	if(de->de_flags & (DEF_FORGET | DEF_NORESOLVE))
	{
	    if(de->de_flags & DEF_NORESOLVE)
		Printf("\nDue to a former error, the link to \"%s\" can't be resolved.\n",
		       de->de_name);
	}
	else
	{
	    fine = PathRelative(de->de_sourcelock, de->de_linklock,
				de->de_linkname, buffer, BUFFERSIZE);

	    if(!fine)
	    {
		if(IoErr() != ERROR_OBJECT_NOT_FOUND)
		{
		    PostFailure(IoErr(), "Can't find relative path of \"%s\"",
				de->de_name);
		    rc = FALSE;
		}
		else
		{
		    /* This might be O.K. */
		    if(SameLock(de->de_sourcelock, dest) == LOCK_SAME_VOLUME)
		    {
			Printf("Warning: Link destination of \"%s\" not copied, using the original path.\n",
			       de->de_name);
			OFlush();

			if(CreateLink(de->de_name, de->de_dirlock,
				      de->de_linkname, de->de_linklock,
				      de->de_flags & DEF_SOFTLINK))
			{
			    resolved = TRUE;
			}
		    }
		  
		    if(!resolved) 
		    {
			if(!(de->de_flags & DEF_DIR))
			{
			    Printf("Warning: Link destination of \"%s\" not present, using a copy instead.\n",
				   de->de_name);
			    OFlush();
			    resolved = TRUE;
			}
		    }
		}
	    }
	    else
	    {
		/* Found the relative path. Find a lock to the object 
		   the link goes to */
		CurrentDir(dest);
		destlock = Lock(buffer, SHARED_LOCK);

		/* If the destination can't be found... */
		if(destlock == BNULL)
		{
		    if(IoErr() == ERROR_OBJECT_NOT_FOUND)
		    {
			if(!(de->de_flags & DEF_DIR))
			{
			    Printf("Warning: Link destination of \"%s\" not present, using a copy instead.\n",
				   de->de_name);
			    OFlush();
			    resolved = TRUE;
			}
		    }
		    else
		    {
			PostFailure(IoErr(), "Can't lock the object \"%s\" the link is relative to",
				    buffer);
			rc = FALSE;
		    }
		}
		else
		{
		    /* Found the destination */
		    if(CreateLink(de->de_name, de->de_dirlock, NULL, destlock,
				  de->de_flags & DEF_SOFTLINK))
		    {
			resolved = TRUE;

			if(!quiet)
			    Printf("\n");
		    }

		    UnLock(destlock);
		}
	    }
	}

	if(!resolved && (de->de_flags & DEF_CREATED))
	{
	    CurrentDir(de->de_dirlock);

	    if(de->de_flags & DEF_DIR)
	    {
		/* Create an empty directory instead */
		if(DeleteFile(de->de_name))
		{
		    if((de->de_flags & DEF_CREATED) &&
		       (de->de_protection & FIBF_ARCHIVE) && onlya)
		    {
			Printf("Warning: The archived directory link \"%s\" was left out.\n",
			       de->de_name);
			rc = FALSE;
			resolved = FALSE;
		    }
		    else if((destlock = CreateDir(de->de_name)) != BNULL)
		    {
			UnLock(destlock);
			resolved = TRUE;
			Printf("Warning: Created an empty directory for \"%s\" instead.\n",
			       de->de_name);
			rc = FALSE;
		    }
		}
		else 
		    PostFailure(IoErr(), "Can't delete the placeholder \"%s\"",
				de->de_name);
	    }
	    else
	    {
		DeleteFile(de->de_name);      /* Don't mind if this worked */
		Printf("Warning: The link to \"%s\" can't be resolved.\n",
		       de->de_name);
		rc = FALSE;
	    }
	}
	
	if(resolved)
	{
	    CurrentDir(de->de_dirlock);
	    CloneFlags(de);
	}
    }
    
    return rc;
}

///
/// DeleteEntry
void DeleteEntry(struct DirEntry *de, BOOL all)
{
    BPTR  oldlock = BNULL;

    if(all || (!(onlya && (de->de_protection & FIBF_ARCHIVE))))
    {
	if(CheckAbort())
	    return;

	if(!quiet)
	{
	    DoIndent();
	    Printf("%s...", de->de_name);
	    OFlush();
	}
	
	if(de->de_sourcelock)
	    oldlock = CurrentDir(de->de_sourcelock);

	if(force)
	    SetProtection(de->de_name, 0);
	
	if(!DeleteFile(de->de_name))
	{
	    PostFailure(IoErr(), "Can't delete the file \"%s\"", de->de_name);
	}
	else
	{
	    if(!quiet)
		Printf("(deleted)\n");
	}
    }
    
    if(de->de_sourcelock)
	CurrentDir(oldlock);

    Remove((struct Node *)(de));
    FreeDirEntry(de, TRUE);
}

///
/// DeleteDirList
BOOL DeleteDirList(BPTR source, struct MinList *dirlist, BOOL all)
{
    struct DirEntry *de, *next;
    
    BPTR  newsource;
    BOOL  fine = TRUE;
    
    if(CheckAbort())
    {
	FreeDirList(dirlist, TRUE);

	return TRUE;
    }

    /* Delete all files */
    for(de = (struct DirEntry *)(dirlist->mlh_Head); de->de_next; )
    {
	next = de->de_next;
	
	if((de->de_flags & DEF_DIR) == 0)
	    DeleteEntry(de, all);
	
	de = next;
    }
    
    /* Delete recursively all directories first ! */
    for(de = (struct DirEntry *)(dirlist->mlh_Head); de->de_next; )
    {
	next = de->de_next;
	
	if(CheckAbort())
	    break;

	if(de->de_flags & DEF_DIR)
	{
	    CurrentDir((de->de_sourcelock) ? (de->de_sourcelock) : (source));
	    newsource = Lock(de->de_name, SHARED_LOCK);

	    if(newsource == BNULL)
	    {
		PostFailure(Fatal(), "Can't lock new directory \"%s\"",
			    de->de_name);
		de->de_flags |= DEF_FORGET;
	    } 
	    else
	    {
		if(!quieter)
		{
		    DoIndent();
		    Printf("Cleaning the directory \"%s\":\n", de->de_name);
		}

		depth++;

		if(!RecursiveDelete(newsource, &(de->de_sublist), all, 
				    de->de_name))
		    fine = FALSE;

		depth--;

		if(!quiet)
		{
		    DoIndent();
		    Printf("...done with directory \"%s\".\n", de->de_name);
		}
		
		CurrentDir((de->de_sourcelock) ? (de->de_sourcelock) :
			   (source));
		
		if(DirEmpty(newsource, de->de_name))
		{
		    UnLock(newsource);
		    
		    if(force)
			SetProtection(de->de_name, 0);
		    
		    if(!DeleteFile(de->de_name))
		    {
			PostFailure(IoErr(), 
				    "Can't delete the directory \"%s\"",
				    de->de_name);
		    }
		} 
		else
		    UnLock(newsource);
	    }

	    Remove((struct Node *)de);
	    FreeDirEntry(de, TRUE);
	}

	de = next;
    }

    /* Delete now all (archived) files */
    CurrentDir(source);

    /* Free now all remaining stuff */
    for(de = (struct DirEntry *)(dirlist->mlh_Head); de->de_next; )
    {
	next = de->de_next;
	DeleteEntry(de, FALSE);
	de = next;
    }

    FreeDirList(dirlist, TRUE);

    return fine;
}

///
/// RecursiveDelete
BOOL RecursiveDelete(BPTR source, struct MinList *dirlist, BOOL all,
		     char *dirname)
{
    CurrentDir(source);
    
    if(CheckAbort())
	return TRUE;
    
    if(!ReadDir(source, dirlist, dirname))
    {
	FreeDirList(dirlist, TRUE);

	return FALSE;    /* Continue anyway */
    }
    
    return DeleteDirList(source, dirlist, all);
}

///
/// SetAList
BOOL SetAList(BPTR source, struct MinList *dirlist)
{
    struct DirEntry *de, *next;
 
    BPTR  newsource;
    BOOL  fine = TRUE;

    CurrentDir(source);

    /* Set the archive bit of all files */
    for(de = (struct DirEntry *)(dirlist->mlh_Head); de->de_next; )
    {
	next = de->de_next;
	
	if((de->de_flags & DEF_DIR) == 0)
	{
	    SetAFlag(de);
	    Remove((struct Node *)de);
	    FreeDirEntry(de, TRUE);
	}
	
	de = next;
    }
    
    /* Set the archive bit of all directories */
    for(de = (struct DirEntry *)(dirlist->mlh_Head); de->de_next; )
    {
	next = de->de_next;
	
	if(CheckAbort())
	    break;
	
	if(de->de_flags & DEF_DIR) 
	{
	    CurrentDir((de->de_sourcelock) ? (de->de_sourcelock) : (source));
	    newsource = Lock(de->de_name, SHARED_LOCK);

	    if(newsource == BNULL)
	    {
		PostFailure(IoErr(), "Can't lock new directory \"%s\"",
			    de->de_name);
		de->de_flags |= DEF_FORGET;
	    }
	    else
	    {
		if(!quiet)
		{
		    DoIndent();
		    Printf("Setting the archive bit of the directory \"%s\"...\n",
			   de->de_name);
		}
		
		depth++;

		if(!(RecursiveSetA(newsource, &(de->de_sublist), de->de_name)))
		    fine = FALSE;

		depth--;
		
		CurrentDir(source);
		SetAFlag(de);
		UnLock(newsource);
	    }

	    Remove((struct Node *)de);
	    FreeDirEntry(de, TRUE);
	}

	de = next;
    }

    CurrentDir(source);

    /* Set now the A flag of all remaining stuff */
    for(de = (struct DirEntry *)(dirlist->mlh_Head); de->de_next; )
    {
	next = de->de_next;
	
	SetAFlag(de);
	Remove((struct Node *)(de));
	FreeDirEntry(de, TRUE);
	de = next;
    }

    FreeDirList(dirlist, TRUE);

    return fine;
}

///
/// RecursiveSetA
BOOL RecursiveSetA(BPTR source, struct MinList *dirlist, char *dirname)
{
    if(CheckAbort())
	return TRUE;
    
    CurrentDir(source);
    
    if(!ReadDir(source, dirlist, dirname))
    {
	FreeDirList(dirlist, TRUE);

	return FALSE;

    }
    
    if(CheckAbort())
    {
	FreeDirList(dirlist, TRUE);

	return TRUE;
    }
    
    return SetAList(source, dirlist);
}

///
/// FindDirEntry
BOOL FindDirEntry(struct MinList *dirlist, struct DirEntry *de)
{
    struct DirEntry *look;

    for(look = (struct DirEntry *)(dirlist->mlh_Head); look->de_next;
	look = look->de_next)
    {
	if(!(Stricmp(de->de_name, look->de_name)))
	    return TRUE;
    }
    
    return FALSE;
}

///
/// PurgeDir
BOOL PurgeDir(BPTR sourcelock, BPTR destlock, struct MinList *dirlist,
	      char *dirname)
{
    struct MinList   destlist;
    struct DirEntry *de, *next;

    BOOL  result;
    
    NewList((struct List *)&destlist);

    if(!quiet)
    {
	DoIndent();
	Printf("Purging...");
	OFlush();
    }

    depth++;
    
    /* Read destination directory */
    
    if(!ReadDir(destlock, &destlist, dirname))
    {
	FreeDirList(&destlist, TRUE);
	Printf("\n");
	depth--;

	return FALSE;
    }

    /* Remove all the stuff we already have in the source */
    for(de = (struct DirEntry *)(destlist.mlh_Head); de->de_next; )
    {
	next = de->de_next;

	if(FindDirEntry(dirlist, de))
	{
	    Remove((struct Node *)de);
	    FreeDirEntry(de, TRUE);
	}

	de = next;
    }

    Printf("\n");
    result = DeleteDirList(destlock, &destlist, TRUE);
    
    depth--;

    if(!quiet)
    {
	DoIndent();
	Printf("...done\n");
    }
    
    return result;
}


///
/// AllocCopyNode
struct CopyNode *AllocCopyNode(BPTR from, ULONG size, ULONG *limit)
{
    struct CopyNode *cn = NULL;
    
    if(size > *limit)
    {
	size = *limit;
    }
    
    if(size)
    {
	cn = AllocVec(sizeof(struct CopyNode) + size, 
		      MEMF_PUBLIC | MEMF_NO_EXPUNGE);
    }

    if(cn == NULL)
    {
	if(lastchancebusy == FALSE)
	{
	    lastchancebusy  = TRUE;
	    cn              = &lastchancenode;
	    cn->cn_Buffer   = buffer;
	    cn->cn_release  = FALSE;
	    size            = BUFFERSIZE;
	}
    }
    else
    {
	*limit         -= size;
	cn->cn_Buffer   = (cn + 1);
	cn->cn_release  = TRUE;
    }
    
    if(cn != NULL)
    {
	cn->cn_DosMessage.mn_Node.ln_Type  = NT_MESSAGE;
	cn->cn_DosMessage.mn_Node.ln_Pri   = 0;
	cn->cn_DosMessage.mn_Node.ln_Name  = (APTR)(&(cn->cn_DosPacket));
	cn->cn_DosMessage.mn_ReplyPort     = &ioport;
	cn->cn_DosPacket.dp_Port           = &ioport;
	cn->cn_DosMessage.mn_Length        = sizeof(struct StandardPacket);
	cn->cn_DosPacket.dp_Link           = &(cn->cn_DosMessage);
	cn->cn_DosPacket.dp_Type           = ACTION_READ;   /* Read */
	cn->cn_Command                     = ACTION_READ;
	cn->cn_DosPacket.dp_Arg1           = ((struct FileHandle *)BADDR(from))->fh_Arg1;
	cn->cn_DosPacket.dp_Arg2           = (SIPTR)cn->cn_Buffer;
	cn->cn_DosPacket.dp_Arg3           = size;
	cn->cn_BufSize                     = size;
	cn->cn_BackTick                    = cn;
    }

    return cn;
}


///
/// FreeCopyNode
void FreeCopyNode(struct CopyNode *cn, ULONG *limit)
{
    if(cn->cn_release)
    {
	*limit += cn->cn_BufSize;
	FreeVec(cn);
    }
    else 
	lastchancebusy = FALSE;
}


///
/// AsyncCopy
BOOL AsyncCopy(BPTR from, BPTR to, ULONG readsize, char *name)
{
    struct MsgPort  *readport, *writeport;
    struct CopyNode *cn;

    ULONG  buffer;
    ULONG  packetcount;
    ULONG  limit;
    BOOL   busy, stop, report;
    LONG   readerror, writeerror;
    LONG   arg1;
    LONG   signals;

    ioport.mp_Node.ln_Type  = NT_MSGPORT;
    ioport.mp_Node.ln_Pri   = 0;
    ioport.mp_Node.ln_Name  = NULL;
    ioport.mp_Flags         = PA_SIGNAL;
    ioport.mp_SigBit        = SIGB_DOS;
    ioport.mp_SigTask       = FindTask(NULL);
    NewList(&(ioport.mp_MsgList));
    limit                   = AvailMem(MEMF_PUBLIC | MEMF_LARGEST);
    limit                 >>= 1;
    
    if(limit > 8192)
    {
	limit -= 8192;
    }
    else
	limit = 0;
    
    readport  = ((struct FileHandle *)BADDR(from))->fh_Type;
    writeport = ((struct FileHandle *)BADDR(to))->fh_Type;
    arg1      = ((struct FileHandle *)BADDR(to))->fh_Arg1;

    /* if source or destination is NIL:, return no error
       but copy zero bytes anyhow */
    if((readport == NULL) || (writeport == NULL))
	return 0;

    /* if there's nothing to do, abort now */
    if(readsize == 0)
	return 0;
    
    readerror      = 0;
    writeerror     = 0;
    packetcount    = 0;
    stop           = FALSE;
    SetIoErr(0L);
    lastchancebusy = FALSE;
    report         = TRUE;
    
    for(;;)
    {
	/* Check whether we should still read bytes from the input */
	busy = FALSE;
	
	/* If user hit ^C, then abort reading */
	if(CheckAbort())
	    stop = TRUE;

	if(stop) 
	{
	    readsize = 0;   /* Do not demand reading if halt required */
	}

	if(readsize)
	{
	    /* Go for a reasonable buffer size */
	    buffer = 4096;

	    if(bufsize > buffer)
		buffer = bufsize;
	    
	    if(buffer > readsize)
		buffer = readsize;

	    if(buffer > limit)
	    {
		if(limit > BUFFERSIZE)
		{
		    buffer = limit;
		}
	    }
	    
	    cn = AllocCopyNode(from, buffer, &limit);

	    if(cn != NULL)
	    {
		/* Might fill in another value if no memory */
		readsize -= cn->cn_BufSize;

		SendPkt(&(cn->cn_DosPacket), readport, &ioport);
		busy = TRUE;
		packetcount++;
	    }
	    else
	    {
		/* Allocation of the packet failed. If at least one packet 
		   is busy, wait for its return as we may re-use it.
		   else generate an error */
		if(packetcount == 0)
		{
		    readerror = ERROR_NO_FREE_STORE;
		    SetIoErr(readerror);
		    PostFailure(Fatal(), "Can't read from \"%s\"", name);
		    stop = TRUE;
		    /* Fatal sets also abortFlag */
		}
	    }
	}
	
	cn = (struct CopyNode *)GetMsg(&ioport);

	if(cn != NULL)
	{
	    busy = TRUE;
	    
	    /* A Queue packet failure ? */
	    if(cn->cn_BackTick != cn)
	    {
		Alert(AN_QPktFail);
	    }
	    else
	    {
		if(cn->cn_Command == ACTION_READ)
		{
		    if(cn->cn_DosPacket.dp_Res1 > 0) 
		    {
			/* If this is an emergency stop, do not send a 
			   write request */
			if(CheckAbort())
			{
			    packetcount--;
			    FreeCopyNode(cn, &limit);
			}
			else
			{
			    /* Everything went fine, initialize this packet
			       for writing and let it go */
			    cn->cn_DosPacket.dp_Type  = ACTION_WRITE;
			    cn->cn_Command            = ACTION_WRITE;
			    cn->cn_DosPacket.dp_Arg1  = arg1;
			    cn->cn_DosPacket.dp_Arg2  = (SIPTR)cn->cn_Buffer;
			    readsize                 += cn->cn_BufSize;
			    cn->cn_DosPacket.dp_Arg3  = cn->cn_DosPacket.dp_Res1;
			    cn->cn_WriteSize          = cn->cn_DosPacket.dp_Res1;
			    readsize                 -= cn->cn_DosPacket.dp_Res1;
			    SendPkt(&(cn->cn_DosPacket), writeport, &ioport);
			}
		    }
		    else
		    {
			/* Uhoh, something went wrong. An error? */
			if(cn->cn_DosPacket.dp_Res1 < 0)
			{   /* An error! */

			    /* If this is the first fault, print it */

			    if(readerror == 0)
			    {
				if(report)
				{
				    if(ErrorReport(cn->cn_DosPacket.dp_Res2,
						   REPORT_STREAM, (SIPTR)from,
						   NULL))
				    {
					/* Cancled */
					report = FALSE;
				    }
				} /* continue with the next read-packet, 
				     release this */
				
				/* Abort and print error in case the user
				   cancled */
				if(report == FALSE)
				{
				    readerror = cn->cn_DosPacket.dp_Res2;
				    SetIoErr(readerror);
				    PostFailure(readerror,
						"Can't read from \"%s\"", name);
				    stop = TRUE;
				}
			    }

			    /* A read error is non-fatal */
			}
			else
			    stop = TRUE;   /* EOF, Reading done */

			packetcount--;
			FreeCopyNode(cn, &limit);
		    }
		}
		else if(cn->cn_Command == ACTION_WRITE)
		{
		    if(cn->cn_DosPacket.dp_Res1 != cn->cn_BufSize)
		    {
			if(cn->cn_DosPacket.dp_Res1 < 0)
			{
			    /* A real error? If so, report to the user 
			       if not aborted anyhow */
			    if(report)
			    {
				if(ErrorReport(cn->cn_DosPacket.dp_Res2,
					       REPORT_STREAM, (SIPTR)to, NULL))
				{
				    /* User canceled */
				    report = FALSE;
				}
				else
				{
				    /* Re-initialize the packet, and retry
				       the write request */
				    cn->cn_DosPacket.dp_Type = ACTION_WRITE;
				    cn->cn_DosPacket.dp_Arg1 = arg1;
				    cn->cn_DosPacket.dp_Arg2 = (SIPTR)cn->cn_Buffer;
				    cn->cn_DosPacket.dp_Arg3 = cn->cn_WriteSize;
				    SendPkt(&(cn->cn_DosPacket), writeport,
					    &ioport);
				    CheckAbort();
				    continue;
				}
			    }
			}
			
			/* If this is the first write error, print it */
			if(report == FALSE && writeerror == 0)
			{
			    if(cn->cn_DosPacket.dp_Res1 < 0)
			    {
				writeerror = cn->cn_DosPacket.dp_Res2;
			    }
			    else
				writeerror = ERROR_OBJECT_WRONG_TYPE;

			    SetIoErr(writeerror);
			    PostFailure(Fatal(), "Can't write to \"%s\"", name);

			    /* Fatal() sets also abortFlag and therefore
			       stops reading */
			}
		    }

		    packetcount--;
		    FreeCopyNode(cn, &limit);
		}
		else 
		    Alert(AN_QPktFail);  /* Huh, an unkown command returned? */
	    }
	}
	
	if(packetcount == 0 && readsize == 0)
	    break;

	if(!busy)
	{
	    signals = Wait((ULONG)(SIGF_DOS | SIGBREAKF_CTRL_C));

	    if(signals & SIGBREAKF_CTRL_C)
		Abort();
	}
	else
	    CheckAbort();
    }
    
    if(readerror || writeerror)
	return FALSE;
    
    return TRUE;
}

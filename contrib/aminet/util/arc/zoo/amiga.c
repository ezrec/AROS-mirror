#ifndef LINT
static char	amigaid[] = "@(#) amiga.c 1.1 91/08/03 23:21:00";

#endif				/* LINT */

/*
 * machine.c for Amiga.
 *
 * This file is (C) Copyright 1991 by Olaf Seibert. All rights reserved. May
 * be used and distributed under the same conditions as the other
 * non-public-domain files that are part of Zoo.
 */

#undef TEXT			/* typedef in exec/types.h */
#include <assert.h>
#include <exec/types.h>
#include <libraries/dos.h>
#include <signal.h>

#ifdef __STDC__
#define __args
//#include <functions.h>
#else
BPTR		Lock(), CreateDir(), ParentDir();
long		Examine(), ExNext();
void	       *AllocMem();
struct MsgPort *DeviceProc();
long		SetSignal();

#endif

int iswild	PARMS((char *));
void endfile	PARMS((int));
void fcbpath	PARMS((char *, char *));
void ToDateStamp PARMS((struct DateStamp * datestamp, unsigned date, unsigned time));
void ToMSDate	PARMS((unsigned *date, unsigned *time, struct DateStamp * datestamp));
void _abort	PARMS((void));
long Chk_Abort	PARMS((void));

/*
 * WARNING:  This file assumes that ZOOFILE is a standard buffered file.
 * It will have to be modified if ZOOFILE is changed to be an unbuffered
 * file descriptor or to any other kind of file.
 */

#ifdef UNBUF_IO
/*
 * Function tell() returns the current seek position for a file
 * descriptor. 4.3BSD on VAX-11/785 has an undocumented tell() function
 * but it may not exist on all implementations, so we code one here to be
 * on the safe side.  It is needed for unbuffered I/O only.
 */
long lseek	PARMS((int, long, int));
long
tell(fd)
int		fd;
{
    return lseek(fd, 0L, 1);
}

#endif


/*
 * Function zootrunc() truncates a file.
 */

int
zootrunc(f)
ZOOFILE 	f;
{
    fflush(f);                  /* just in case it matters */
    /* Cannot truncate on AmigaOS < 2.0 */
}

/*
 * Function fixfname() converts the supplied filename to a syntax legal
 * for the host system.  It is used during extraction.
 */

char	       *
fixfname(fname)
char	       *fname;
{
    return fname;		/* default is no-op */
}

/*
 * Function isuadir() returns 1 if the supplied name is a directory, else
 * it returns 0.
 */

int
isuadir(path)
char	       *path;
{
    struct FileInfoBlock *fib;
    int 	    its_a_dir;

    its_a_dir = 0;
    if (fib = malloc(sizeof (*fib))) {
	BPTR		lock;

	if (lock = Lock(path, ACCESS_READ)) {
	    Examine(lock, fib);
	    its_a_dir = (fib->fib_DirEntryType > 0);
	    UnLock(lock);
	}
	free(fib);
    }
    return its_a_dir;
}

/* No standard UNIX-compatible time routines */

/*
 * Function getutime() gets the date and time of the file name supplied.
 * Date and time is in MSDOS format.
 */
int
getutime(path, date, time)
char	       *path;
unsigned       *date,
	       *time;
{
    struct FileInfoBlock *fib;

    *date = 0;
    *time = 0;

    if (fib = malloc(sizeof (*fib))) {
	BPTR		lock;

	if (lock = Lock(path, ACCESS_READ)) {
	    Examine(lock, fib);

	    ToMSDate(date, time, &fib->fib_Date);
	    UnLock(lock);
	}
	free(fib);
    }
    return 0;
}

/*
 * Function setutime() sets the date and time of the filename supplied.
 * Date and time is in MSDOS format.
 */

int
setutime(path, date, time)
char	       *path;
unsigned int	date,
		time;
{
    struct DateStamp DateStamp;
    extern int SetFileDate PARMS((char *file, struct DateStamp * date));

    ToDateStamp(&DateStamp, date, time);

    return !SetFileDate(path, &DateStamp);
}

#ifdef AZTEC_C
# ifndef  EXEC_MEMORY_H
#	include <exec/memory.h>
# endif
# ifndef ACTION_SET_DATE
#	define ACTION_SET_DATE		34L
# endif

int
SetFileDate(path, date)
char	       *path;
struct DateStamp *date;
{
    struct MsgPort *task;
    BPTR	    lock,
		    plock;
    UBYTE	   *bcplstring;
    int 	    result;

    if (!(bcplstring = (UBYTE *) AllocMem(64L, (long) MEMF_PUBLIC)))
	return 0;
    result = 0;
    if (!(task = DeviceProc(path))) {
	goto abort;
    }
    if (!(lock = Lock(path, SHARED_LOCK))) {
	goto abort;
    }
    plock = ParentDir(lock);
    UnLock(lock);

    /* Strip pathnames first */
    strcpy((char *) (bcplstring + 1), nameptr(path));
    *bcplstring = strlen((char *) (bcplstring + 1));

    result = dos_packet(task, ACTION_SET_DATE, NULL, plock,
		      (ULONG) bcplstring >> 2, (ULONG) date, 0L, 0L, 0L);

    UnLock(plock);
abort:
    FreeMem((void *) bcplstring, 64L);
    return result;
}

#endif


/* No standard UNIX-specific file attribute routines */

/*
 * Get file attributes.  Currently only the lowest nine of the *IX mode
 * bits are used.  Also we return bit 23=0 and bit 22=1, which means use
 * portable attribute format, and use attribute value instead of using
 * default at extraction time.
 */

unsigned long
getfattr(f)
ZOOFILE 	f;
{
    return NO_FATTR;		/* inaccessible -- no attributes */
}

/*
 * Set file attributes.  Only the lowest nine bits are used.
 */

int
setfattr(f, a)
char	       *f;		/* filename */
unsigned long	a;		/* atributes to set */
{
    return 0;			/* cannot do */
}

int
iswild(name)
char	       *name;
{
    return (strchr(name, '*') != NULL) || (strchr(name, '?') != NULL);
}

#define FMAX	3		/* Number of different filename patterns */


/* #include "various.h" */
/* #include "zoo.h" *//* solely to define PATHSIZE */

#ifdef SPECNEXT
/*-

nextfile() returns the name of the next source file matching a filespec.

INPUT
   what: A flag specifying what to do. If "what" is 0, nextfile()
      initializes itself.     If "what" is 1, nextfile() returns the next
      matching filename.
   filespec:  The filespec, usually containing wildcard characters, that
      specifies which files are needed.  If "what" is 0, filespec must be
      the filespec for which matching filenames are needed.  If "what" is 1,
      nextfile() does not use "filespec" and "filespec" should be NULL to
      avoid an assertion error during debugging.
   fileset:  nextfile() can keep track of more than one set of filespecs.
      The fileset specifies which filespec is being matched and therefore
      which set of files is being considered.  "fileset" can be in the
      range 0:FMAX.   Initialization of one fileset does not affect the
      other filesets.

OUTPUT
   IF what == 0 THEN
      return value is NULL
   ELSE IF what == 1 THEN
      IF a matching filename is found THEN
	 return value is pointer to matching filename including supplied path
      ELSE
	 IF at least one file matched previously but no more match THEN
	    return value is NULL
	 ELSE IF supplied filespec never matched any filename THEN
	    IF this is the first call with what == 1 THEN
	       return value is pointer to original filespec
	    ELSE
		return value is NULL
	    END IF
	 END IF
       END IF
   END IF

NOTE

   Initialization done when "what"=0 is not dependent on the correctness
   of the supplied filespec but simply initializes internal variables
   and makes a local copy of the supplied filespec.  If the supplied
   filespec was illegal, the only effect is that the first time that
   nextfile() is called with "what"=1, it will return the original
   filespec instead of a matching filename.	   That the filespec was
   illegal will become obvious when the caller attempts to open the
   returned filename for input/output and the open attempt fails.

USAGE HINTS

nextfile() can be used in the following manner:

      char *filespec;			  -- will point to filespec
      char *this_file;			  -- will point to matching filename
      filespec = parse_command_line();    -- may contain wildcards
      FILE *stream;

      nextfile (0, filespec, 0);          -- initialize fileset 0
      while ((this_file = nextfile(1, (char *) NULL, 0)) != NULL) {
	 stream = fopen (this_file, "whatever");
	 if (stream == NULL)
	    printf ("could not open %s\n", this_file);
	 else
	     perform_operations (stream);
      }
*/

static int	first_time[FMAX + 1];
static BPTR	filelock[FMAX + 1];
static struct FileInfoBlock *fib[FMAX + 1];

char	       *
nextfile(what, filespec, fileset)
int		what;		/* whether to initialize or match	 */
register char  *filespec;	/* filespec to match if initializing	 */
register int	fileset;	/* which set of files			 */
{
    static char     pathholder[FMAX + 1][PATHSIZE];	/* holds a pathname to
							 * return */
    static char     saved_fspec[FMAX + 1][PATHSIZE];	/* our own copy of
							 * filespec   */

    long	    success;
    char	   *ph_fs;
    BPTR	    fl_fs;
    struct FileInfoBlock *fib_fs;

    assert(fileset >= 0 && fileset <= FMAX);
    ph_fs = pathholder[fileset];
    fl_fs = filelock[fileset];
    fib_fs = fib[fileset];
    if (what == 0) {
	assert(filespec != NULL);
	strcpy(saved_fspec[fileset], nameptr(filespec));        /* save the filespec */
	strcpy(ph_fs, filespec);/* save the path */
	first_time[fileset] = 1;
	return NULL;
    }
    assert(what == 1);
    assert(filespec == NULL);
    assert(first_time[fileset] == 0 || first_time[fileset] == 1);

    success = DOSFALSE;
    if (first_time[fileset]) {  /* first time -- initialize etc. */
	filelock[fileset] = fl_fs = 0;
	if (!iswild(saved_fspec[fileset])) {
	    goto notwild;
	}
	if (fib[fileset] = fib_fs = malloc(sizeof (struct FileInfoBlock))) {
	    char	   *np,
			    n;

	    np = nameptr(ph_fs);
	    n = *np;
	    *np = '\0';
	    if (filelock[fileset] = fl_fs = Lock(ph_fs, ACCESS_READ)) {
		Examine(fl_fs, fib_fs);
		assert(fib->fib_DirEntryType > 0);
	    }
	    *np = n;
	}
    }
    /* find first or next matching file; skip directories (type > 0) */
    if (fib_fs) {
	for (;;) {
	    success = ExNext(fl_fs, fib_fs);
	    if (success == DOSFALSE)
		break;
	    if (fib_fs->fib_DirEntryType > 0)
		continue;
	    if (match_half(fib_fs->fib_FileName, saved_fspec[fileset]))
		break;
	}
    }
    if (success == DOSFALSE) {  /* if error status */
notwild:
	if (first_time[fileset]) {      /* if file never matched then */
	    first_time[fileset] = 0;
	    return ph_fs;	/* return original filespec */
	} else {		/* else */
	    endfile(fileset);
	    return NULL;	/* return NULL for no more   */
	}
    } else {			/* a file matched */
	first_time[fileset] = 0;
	/* add path info	 */
	strcpy(nameptr(ph_fs), fib_fs->fib_FileName);
	return ph_fs;		/* matching path  */
    }
    /* NOTREACHED */
}				/* nextfile */

void
endfile(fileset)
int		fileset;
{
    assert(fileset >= 0 && fileset <= FMAX);
    first_time[fileset] = 0;
    if (fib[fileset]) {
	free(fib[fileset]);
	fib[fileset] = NULL;
    }
    if (filelock[fileset]) {
	UnLock(filelock[fileset]);
	filelock[fileset] = 0;
    }
}

#endif

char	       *
mktemp(name)
char	       *name;
{
    char	   *x;

    if (x = strchr(name, 'X')) {
	struct DateStamp ds;
	long		rnd;
	BPTR		lock;

	DateStamp(&ds);
	rnd = (ds.ds_Days << 17) + (ds.ds_Minute << 6) + ds.ds_Tick;

retry:
	rnd += (rnd >> 24);
	sprintf(x, "%06lx", rnd & 0x00ffffff);
	if (lock = Lock(name, ACCESS_READ)) {
	    UnLock(lock);
	    rnd--;
	    goto retry;
	}
    }
    return name;
}

#ifdef AZTEC_C
# ifndef __STDC__
/*
 * A bit of a rough (and rude) implementation of realloc
 */
void	       *
realloc(oldptr, size)
char	       *oldptr;
unsigned int	size;
{
    char	   *newptr;

    newptr = malloc(size);
    if (newptr) {
	CopyMem(oldptr, newptr, (long) size);
	free(oldptr);
    }
    return newptr;
}

# endif
#endif

void
makedir(name)
char	       *name;
{
    BPTR	    lock;

    if (lock = CreateDir(name)) {
	UnLock(lock);
    }
}

#define BASEYEAR		 1978
#define DAYS_PER_YEAR		 365
#define HOURS_PER_DAY		 24
#define MINUTES_PER_HOUR	 60
#define SECONDS_PER_MINUTE  60

#define DAYS_PER_WEEK		 7
#define MONTHS_PER_YEAR 	 12

#define MINUTES_PER_DAY 	 (MINUTES_PER_HOUR * HOURS_PER_DAY)
#define SECONDS_PER_DAY 	 ((long) SECONDS_PER_MINUTE * \
				  MINUTES_PER_HOUR * HOURS_PER_DAY)

#define DATE_MIN		 0x21	/* 1-jan-80 */

#define LeapYear(year)  ((year & 3) == 0)       /* From 1-Mar-1901 to
						 * 28-Feb-2100 */

int		daycount[MONTHS_PER_YEAR] = {
    31, 28, 31, 30, 31, 30,
    31, 31, 30, 31, 30, 31
};

void
ToDateStamp(datestamp, date, time)
struct DateStamp *datestamp;
unsigned	date;
unsigned	time;
{
    {
	int		hours,
			minutes,
			seconds;

	seconds = (time & 31) * 2;
	time >>= 5;
	minutes = time & 63;
	time >>= 6;
	hours = time;

	datestamp->ds_Minute = MINUTES_PER_HOUR * hours + minutes;
	datestamp->ds_Tick = TICKS_PER_SECOND * seconds;
    }

    {
	unsigned	i,
			j,
			t;
	int		year,
			month,
			day;

	if (date < DATE_MIN)
	    date = DATE_MIN;

	day = date & 31;
	date >>= 5;
	month = (date & 15) - 1;
	date >>= 4;
	year = date + 1980;

	if ((unsigned) month > 11 ||
	    (unsigned) day > (unsigned) daycount[month]) {
	    day = 31;
	    month = 11;
	    year = 1979;
	}
	j = year - BASEYEAR;

	/*
	 * Get the next lower full leap period (4 years and a day) since
	 * ...
	 */
	t = (year - BASEYEAR) & ~3;
	i = t;
	t = (t / 4) * (4 * DAYS_PER_YEAR + 1);

	/* t now is the number of days in 4 whole years since ... */

	while (i < j) {
	    t += DAYS_PER_YEAR;
	    if (LeapYear(i + BASEYEAR)) {
		t++;
	    }
	    i++;
	}

	/* t now is the number of days in whole years since ... */

	for (i = 0; i < month; i++) {
	    t += daycount[i];
	    if (i == 1 && LeapYear(year)) {
		t++;
	    }
	}

	/* t now is the number of days in whole months since ... */

	t += day - 1;

	/* t now is the number of days in whole days since ... */

	datestamp->ds_Days = t;
    }
}

void
ToMSDate(date, time, datestamp)
unsigned       *date;
unsigned       *time;
register struct DateStamp *datestamp;
{
    {
	unsigned short	hours,
			minutes,
			seconds;

	hours = datestamp->ds_Minute / MINUTES_PER_HOUR;
	minutes = datestamp->ds_Minute % MINUTES_PER_HOUR;
	seconds = datestamp->ds_Tick / TICKS_PER_SECOND;

	*time = (hours << 11) | (minutes << 5) | (seconds / 2);
    }
    {
	register long	days,
			i,
			t;
	int		year,
			month,
			day;

	days = datestamp->ds_Days;

	year = BASEYEAR + (days / (4 * DAYS_PER_YEAR + 1)) * 4;
	days %= 4 * DAYS_PER_YEAR + 1;
	while (days) {
	    t = DAYS_PER_YEAR;
	    if (LeapYear(year))
		t++;
	    if (days < t)
		break;
	    days -= t;
	    year++;
	}
	days++;
	for (i = 0; i < MONTHS_PER_YEAR; i++) {
	    t = daycount[i];
	    if (i == 1 && LeapYear(year))
		t++;
	    if (days <= t)
		break;
	    days -= t;
	}
	month = i + 1;
	day = days;

	*date = ((year - 1980) << 9) | (month << 5) | day;
    }
}

void spec_init	PARMS((void));
void
spec_init()
{
#ifndef NOSIGNAL
    signal(SIGINT, _abort);
#endif
}

void
zooexit(code)
int		code;
{
    int 	    i;

#ifdef SPECNEXT
    for (i = 0; i <= FMAX; i++)
	endfile(i);
#endif

    exit(code);
}

/*
 * called by DB debugger and on CTRL-C.
 */
void
_abort()
{
    zooexit(20);
}

#ifdef NEED_SIGNAL

/*
 * A minimalistic signal implementation, that ignores the signal number,
 * which is assumed to be SIGINT.
 */

static void	(*Handler) PARMS((int));

void
(*signal(sig, handler)) PARMS((int))
int		sig;
void		(*handler) PARMS((int));
{
    void	    (*oldhandler) PARMS((int));

    assert(sig == SIGINT);

    oldhandler = Handler;
    Handler = handler;
    Chk_Abort();

    return oldhandler;
}

/*
 * This function should have the name that is used by your C library to
 * poll CTRL-C.
 */

long
Chk_Abort PARMS((void))
{
    if (Handler == SIG_IGN) {
	/*
	 * Don't clear the signal; if the user wants to abort we'll do
	 * that when it is allowed again.
	 */
	return 0;
    }
    if ((SetSignal(0L, SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C)) {
	void		(*oldhandler) PARMS((int));

	oldhandler = Handler;
	Handler = SIG_DFL;
	if (oldhandler == SIG_DFL) {
	    _abort();           /* must call zooexit() */
	    zooexit(20);        /* just in case */
	}
	(*oldhandler) (SIGINT);
	return 1;
    }
    return 0;
}

#endif

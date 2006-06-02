/*
 *              BUFFER.C                                                                                        vi:ts=4
 *
 *      Copyright (c) Eddy Carroll, September 1994.
 *      Updated 5.3.2000 Thomas Richter, THOR
 *
 *              This module maintains the review buffer used by SnoopDos to record
 *              the past events that have occurred.
 */             

#define TESTING         0

#include "system.h"
#include "snoopdos.h"
#include "patches.h"

#define INVALID_HUNK            ((USHORT)(-1))

#if 0
#define DB(s)   KPrintF(s)
#else
#define DB(s)
#endif

static UBYTE       *BufferStart;                /* Ptr to start of our buffer           */
static UBYTE       *BufferEnd;                  /* Ptr to end of our buffer                     */
static UBYTE       *BufferCur;                  /* Ptr to next free loc in buffer       */
static ULONG            BufferSize;                     /* Size of current buffer                       */

/*
 *              This is the mapping that assigns format ID's to the various
 *              fields we can output.
 */
FieldInit FieldTypes[] = {
        EF_ACTION,              'a',    10,             MSG_ACTION_COL,
        EF_CALLADDR,    'c',    8,              MSG_CALL_COL,
        EF_DATE,                'd',    9,              MSG_DATE_COL,
        EF_HUNKOFFSET,  'h',    11,             MSG_HUNK_COL,
        EF_PROCID,              'i',    8,              MSG_PROCESSID_COL,
        EF_FILENAME,    'n',    27,             MSG_FILENAME_COL,
        EF_OPTIONS,             'o',    7,              MSG_OPTIONS_COL,
        EF_PROCNAME,    'p',    18,             MSG_PROCNAME_COL,
        EF_RESULT,              'r',    4,              MSG_RESULT_COL,
        EF_SEGNAME,             's',    20,             MSG_SEGNAME_COL,
        EF_TIME,                't',    8,              MSG_TIME_COL,
        EF_COUNT,               'u',    5,              MSG_COUNT_COL,
        EF_END,                 0,              0,              0
};

/*
 *              InitBuffers()
 *
 *              Initialises certain variables associated with the buffer routines.
 *              Must be called before any of the buffer functions are called.
 */
void InitBuffers(void)
{
        InitSemaphore(&BufSem);
        NewList(&EventList);
}

/*
 *              SetTotalBufferSize(size, alloctype)
 *
 *              This function is used to change the size of the buffer. If no buffer
 *              has yet been created, one is allocated. If there is not enough room
 *              to create a new buffer without freeing the existing buffer first,
 *              then FALSE is returned, _unless_ the alloctype parameter is
 *              SETBUF_FORCENEW, in which case it simply frees the existing buffer
 *              first, losing any existing history info.
 *
 *              In general, the old buffer's contents are copied across to the
 *              new buffer so that the user's history isn't destroyed.
 *
 *              The intention is that the caller tries to use this call with
 *              the tryalways flag set to SETBUF_KEEPOLD first; if that fails,
 *              then it can display a requester to the user saying that this will
 *              cause the existing buffer info to be lost permanently if you go
 *              ahead--Yes/No?  and then do a retry with alloctype set to
 *              SETBUF_FORCENEW if the user so chooses.
 *
 *              In any case, when allocating a new buffer, the routine will always
 *              allocate a buffer if possible, even if it can't allocate the full
 *              size requested.
 *
 *              Returns FALSE if there wasn't enough room to allocate the new buffer
 *              (the minimum buffer size is something like 4K.)
 *
 *              Note that if the size of the buffer is being reduced, the caller
 *              is responsible for updating the display to reflect the new status.
 *
 *              Note also that the whole idea of only freeing the buffer if there's
 *              no other option is great in principle; in practise, it turned out to
 *              be too much work to support copying buffers for such an infrequent
 *              operation, so we don't do it after all.
 */
int SetTotalBufferSize(ULONG newsize, int alloctype)
{
        UBYTE *oldbuf = BufferStart;
        ULONG oldsize = BufferSize;
        UBYTE *newbuf;

        ObtainSemaphore(&BufSem);
        ClearBuffer();          /* Clear the existing buffer first */

        /*
         *              First, work out if we have enough memory to satisfy the request
         *              without freeing our old buffer first. If we don't, and if
         *              alloctype is SETBUF_KEEPOLD, then we fail immediately.
         */
        if (newsize < MIN_BUF_SIZE)
                newsize = MIN_BUF_SIZE;

        newbuf = AllocMem(newsize, MEMF_ANY);
        if (!newbuf) {
                /*
                 *              No memory available -- we _may_ need to bomb out at this
                 *              point. If we didn't have a buffer already, then we
                 *              always go ahead and try and allocate some, regardless
                 *              of alloctype.
                 */
                if (oldbuf && alloctype == SETBUF_KEEPOLD) {
                        ReleaseSemaphore(&BufSem);
                        return (FALSE);
                }

                /*
                 *              Okay, try and allocate the memory even though it means
                 *              freeing up the old buffer first.
                 */
                if (oldbuf) {
                        FreeMem(oldbuf, oldsize);
                        oldbuf = NULL;
                }
                newbuf = AllocMem(newsize, MEMF_ANY);
                if (!newbuf) {
                        /*
                         *              Couldn't allocate memory on second attempt. Finally,
                         *              we just grab the largest chunk we can and use that
                         *              instead.
                         */
                        ULONG freesize;

                        Forbid();
                        freesize = AvailMem(MEMF_ANY | MEMF_LARGEST);
                        if (freesize < MIN_BUF_SIZE) {
                                Permit();
                                ReleaseSemaphore(&BufSem);
                                return (FALSE);
                        }
                        if (freesize < newsize) /* freesize > newsize _is_ possible! */
                                newsize = freesize;
                        newbuf  = AllocMem(newsize, MEMF_ANY);
                        Permit();
                        if (!newbuf) {
                                /*
                                 *              Should never happen, but better safe than sorry
                                 */
                                ReleaseSemaphore(&BufSem);
                                return (FALSE);
                        }
                }
        }
        /*
         *              Okay, we finally have some memory so initialise the global
         *              buffer variables for use by everyone else.
         */
        BufferStart = newbuf;
        BufferSize  = newsize;
        BufferEnd   = newbuf + newsize;
        BufferCur   = BufferStart;

        if (oldbuf != NULL) {
                // CopyEvents(newbuf, newsize, oldbuf, oldsize);
                FreeMem(oldbuf, oldsize);
        }
        NewList(&EventList);            /* Probably need to delete this eventually */
        ReleaseSemaphore(&BufSem);
        return (TRUE);
}

// void CopyEvents(UBYTE *newbuf, ULONG newsize,
//                              UBYTE *oldbuf, ULONG oldsize) {}

/*
 *              ClearBuffer()
 *
 *              Clears the current buffer by a very simple process -- it simply
 *              erases the current list of linked items, and resets RealFirstSeq etc.
 *              to be the next available event number. This means that any events
 *              that were partially in progress will spot, on their completition,
 *              that their associated event number is now < RealFirstSeq, and so
 *              won't try and write into the now-cleared buffer, thus avoiding
 *              corruption.
 *
 *              BaseSeq is used to allow the new buffer count to commence from
 *              1 again, rather than keeping the old number (which might be
 *              into the hundreds by this stage!)
 *
 *              Note: we are a little bit naughty in some of the patches in that
 *              after allocating a brand new event, we sometimes write into it
 *              immediately _without_ semaphore protection (to fill in the
 *              initial details etc.) This is not terribly wise, but since there
 *              is never anything to deliberately cause a task switch, it's
 *              reasonably safe. Adding semaphore protection to all these cases
 *              would complicate things quite a bit.
 */
void ClearBuffer(void)
{
        ObtainSemaphore(&BufSem);
        Delay(5);       /* Just to be safe, let everyone else run for 0.1 seconds */

        NewList(&EventList);
        BufferCur = BufferStart;

        BaseSeq                 =
        FirstSeq                =
        RealFirstSeq    =
        TopSeq                  =
        LastDrawnTopSeq =
        BottomSeq               =
        MaxScannedSeq   =
        EndSeq                  =
        EndCompleteSeq  = NextSeq;

        ReleaseSemaphore(&BufSem);
}

/*
 *              CleanupBuffers()
 *
 *              Frees up the buffer on exit, along with any other pertinent
 *              resources that were allocated by this module.
 */
void CleanupBuffers(void)
{
        if (BufferStart) {
                ObtainSemaphore(&BufSem);
                FreeMem(BufferStart, BufferSize);
                if (BufferSize == 0)
                        ShowError("Warning! BufferSize is zero for some reason!");
                BufferStart = NULL;
                BufferSize  = 0;
                NewList(&EventList);
                ReleaseSemaphore(&BufSem);
        }
}

/*
 *              GetNewEvent(stringsize)
 *
 *              Allocates a new event structure from the list from the buffer
 *              and returns a pointer to it. If necessary, one of the existing
 *              buffers will be flushed to make room for the new event.
 *
 *              The stringsize is the maximum amount of extra space to allocate
 *              after the end of the event to accomodate string storage. A pointer
 *              to this data area can be obtained by accessing the first location
 *              after the event itself. For example, GetNewEvent(20) would return
 *              an event with 20 additional data bytes after it. Note that this
 *              _includes_ any terminating zeros that may be present in the
 *              strings.
 */
Event *GetNewEvent(int stringsize)
{
        UBYTE *startev;
        ULONG totalsize;
        Event *ev;

        /* Calculate total size and ensure it's a longword multiple */
        totalsize = (sizeof(Event) + stringsize + 3) & ~3;

        if (BufferStart == NULL || totalsize > (MIN_BUF_SIZE/2))
                return (NULL);  /* Should never happen but doesn't hurt to check */

        DB(" [GNE-Go] ");
        if (!AttemptSemaphoreHeavely(&BufSem))
                return NULL;

        /*
        ObtainSemaphore(&BufSem);
         */
#if 0
        ev = (Event *)BufferStart;
        ReleaseSemaphore(&BufSem);
        return (ev);
#endif

        startev = HeadNode(&EventList);
        if (IsListEmpty(&EventList)) {
                /*
                 *              First time being called, so simply begin at the
                 *              start of the buffer.
                 */
                BufferCur = BufferStart;
        } else {
                /*
                 *              There are already some nodes. Try and allocate a new node,
                 *              flushing nodes from the start of the list as necessary.
                 */
                for (;;) {
                        if (startev < BufferCur) {
                                /*
                                 *              From BufferCur to the end of the buffer is free space,
                                 *              so we can check for a quick fit. If we get one, then
                                 *              we go for it, otherwise we move BufferCur back to the
                                 *              start of the buffer and start flushing events from
                                 *              there.
                                 */
                                if (totalsize <= (BufferEnd - BufferCur))
                                        break;  /* Got it */
                                BufferCur = BufferStart;        /* Else wrap to start of list */
                        }
                        /*
                         *              Now delete events from here to the end of the buffer until
                         *              we finally have enough room in the buffer. If we run off
                         *              the end of the buffer, let the main loop go around again
                         *              to reset everything.
                         */
                        while (startev >= BufferCur && (startev - BufferCur) < totalsize) {
                                RemHead(&EventList);
                                if (IsListEmpty(&EventList)) {
                                        /* This really shouldn't be printed unless
                                         * debugging is turned on. Fixed,
                                         * THOR
                                         */
                                        DB("Uhoh --- list is empty and shouldn't be!\n");
                                        Delay(50);
                                        startev = BufferCur = BufferStart;
                                        break;
                                }
                                startev = HeadNode(&EventList);
                        }
                        if (startev > BufferCur)
                                break;  /* Got it */
                }
        }
        ev                 = (Event *)BufferCur;
        BufferCur += totalsize;

        /*
         *              Allocate our new event from the current buffer block (which
         *              we are now assured will have enough space in it)
         */
        ev->seqnum      = ++NextSeq;                    /* Give it a unique sequence number */
        ev->status      = ES_CREATING;                  /* Mark event as not-to-be-touched      */
        ev->flags   = EFLG_NONE;                        /* No date/segment lookup as yet        */
        AddTail(&EventList, (Node *)ev);
        /*
         *              Update current first seq num so patch code knows when an
         *              event currently being created has become invalid
         */
        RealFirstSeq = ((Event *)HeadNode(&EventList))->seqnum;
        DateStamp(&ev->datestamp);                      /* Store current date                           */
        DB(" [GNEEnd] ");
        ReleaseSemaphore(&BufSem);
        return (ev);
}

/*
 *              ParseFormatString(formatstr, evformat, maxfields)
 *
 *              Parses a text format string containing a list of format identifiers
 *              (%d, %s, etc.) with optional field widths (%20s, %10d) into an
 *              internal list of formatting codes that can be quickly processed
 *              when displaying output.
 *
 *              formatstr points to the format string. evformat points to the
 *              EventFormat[] array to hold the output.
 *
 *              maxfields is the maximum number of format fields that will
 *              be recognised -- any more than this will be ignored. evformat[]
 *              must be able to hold at least maxfields elements. If a field is
 *              duplicated, only the first occurrance is recognised.
 *
 *              Returns the length any output produced using this format array will
 *              have (including separating spaces)
 */
int ParseFormatString(char *formatstr, EventFormat *evformat, int maxfields)
{
        int evindex = 0;
        int totlen  = 0;
        char *p     = formatstr;

        while (*p) {
                int width = 0;
                int i;
                char type;
                char ch;

                if (*p++ != '%')
                        continue;

                /*
                 *              Got a format specifier, so now parse it.
                 *
                 *              We ignore any negative prefix -- in the real printf,
                 *              it means left-align this field, and all our fields are
                 *              left-aligned by default anyway.
                 */
                if (*p == '-')
                        p++;
                while (*p >= '0' && *p <= '9') {
                        width = (width * 10) + *p - '0';
                        p++;
                }
                if (!*p)
                        break;

                ch = *p++;
                if (ch >= 'A' && ch <= 'Z')
                        ch |= 0x20;

                for (i = 0; FieldTypes[i].type != EF_END; i++) {
                        if (FieldTypes[i].idchar == ch)
                                break;
                }
                type = FieldTypes[i].type;
                if (type != EF_END) {
                        /*
                         *              Matched a field. Now try and insert it. But first,
                         *              make sure we don't have a duplicate.
                         */
                        int j;

                        for (j = 0; j < evindex; j++)
                                if (evformat[j].type == type)   /* Got a duplicate */
                                        break;

                        if (j < evindex)                /* Skip over duplicates */
                                continue;       

                        if (width == 0)
                                width = FieldTypes[i].defwidth;

                        if (width < 1)                          width = 1;
                        if (width > MAX_FIELD_LEN)      width = MAX_FIELD_LEN;

                        evformat[evindex].type           = type;
                        evformat[evindex].width          = width;
                        evformat[evindex].titlemsgid = FieldTypes[i].titlemsgid;
                        totlen += width + 1;
                        evindex++;
                        if (evindex >= (maxfields-1))
                                break;
                }
        }
        evformat[evindex].type = EF_END;
        if (totlen)                             /* Compensate for the extra 'space' we counted */
                totlen--;

        return (totlen);
}

/*
 *              BuildFormatString(evformat, formatstr, maxlen)
 *
 *              Converts an existing event format array into an ascii string
 *              corresponding to the format that can be parsed by ParseFormatString.
 *              The string is guaranteed to be at most maxlen chars in length
 *              (including terminating \0);
 */
void BuildFormatString(EventFormat *evformat, char *formatstr, int maxlen)
{
        char *p   = formatstr;
        char *end = formatstr + maxlen - 6;     /* Max length of format element is 5 */

        while (evformat->type != EF_END) {
                int j;

                if (p >= end)   /* Never exceed string length */
                        break;
                /*
                 *              Find the event type in our mapping array so that we can
                 *              determine the default width. This lets us choose not
                 *              to include the width in the ascii field, for neatness.
                 */
                for (j = 0; FieldTypes[j].type != evformat->type; j++)
                        ;
                *p++ = '%';
                if (FieldTypes[j].defwidth != evformat->width) {
                        mysprintf(p, "%ld", evformat->width);
                        p += strlen(p);
                }
                *p++ = FieldTypes[j].idchar;
                *p++ = ' ';     /* Space out entries for neatness */
                evformat++;
        }
        p[-1] = '\0';   /* Replace final space with a terminator instead. */
}

/*
 *              ltoa(buffer, val, numdigits)
 *              ltoap(buffer, val, numdigits)
 *
 *              Converts the given value to an ascii hex string of up to numdigits
 *              (with leading zeros). No zero termination is performed. The output
 *              is stored in the first numdigits characters of buffer.
 *
 *              ltoap is similar, except that it pads leading zeros with spaces.
 *
 *              Returns a pointer to the buffer.
 */
char *ltoa(char *buffer, unsigned long val, int numdigits)
{
        static char HexDigits[] = "0123456789ABCDEF";
        char *p = buffer + numdigits - 1;

        while (p >= buffer) {
                *p-- = HexDigits[val & 0x0F];
                val >>= 4;
        }
        return (buffer);
}

char *ltoap(char *buffer, unsigned long val, int numdigits)
{
        static char HexDigits[] = "0123456789ABCDEF";
        char *p = buffer + numdigits - 1;

        do {
                *p-- = HexDigits[val & 0x0F];
                val >>= 4;
        } while (val && p >= buffer);

        while (p >= buffer)
                *p-- = ' ';

        return (buffer);
}

/*
 *              SetEventDateStr(event)
 *
 *              This function converts the datestamp in the passed event into a date
 *              and time string, also stored in the event.
 */
void SetEventDateStr(Event *ev)
{
        struct DateTime datetime;

        datetime.dat_Stamp              = ev->datestamp;
        datetime.dat_Format             = FORMAT_DOS;
        datetime.dat_Flags              = 0;
        datetime.dat_StrDay             = NULL;
        datetime.dat_StrDate    = ev->date;
        datetime.dat_StrTime    = ev->time;
        DateToStr(&datetime);
}

/*
 *              CheckSegTracker()
 *
 *              Checks to see if SegTracker is loaded and sets the SegTracker
 *              global variable accordingly. Should be called every now and
 *              again to keep us up to date (e.g. whenever a new window is
 *              opened).
 */
void CheckSegTracker(void)
{
        Forbid();
        SegTrackerActive = (FindSemaphore("SegTracker") != NULL);
        Permit();
}

/*
 *              SetEventSegmentStr(event)
 *
 *              This function converts the calladdr in the passed event into a segment
 *              name and hunk/offset pair if SegTracker is loaded, or into an invalid
 *              string if SegTracker is not loaded.
 *
 *              See the Enforcer/Segtracker documentation (from Michael Sinz) for
 *              more info on how this works.
 *
 *              Note that if the passed-in event->segname is NULL, then it will
 *              be reset to an "unknown module" string and no attempt to locate
 *              the module will be made. It is thus imperative that the _caller_
 *              only calls this function once for each event. The easiest way
 *              to ensure this is to use the EFLG_DONESEG flag in the ev->flags
 *              register to track whether the call has been made or not.
 *
 *              New: we now detect ROM addresses and search the resident module
 *                       list ourselves looking for the module that contains it.
 */
void SetEventSegmentStr(Event *ev)
{
        typedef char (*__asm SegTrack(reg_a0 ULONG,reg_a1 ULONG *,reg_a2 ULONG *));

        static struct {
                Semaphore       seg_Semaphore;
                SegTrack        *seg_Find;
        } *segsem;

        ULONG hunk;
        ULONG offset;
        char *segname = NULL;

        ev->hunk = INVALID_HUNK;
        if (ev->segname == NULL) {
                ev->segname     = "Unknown module";
                return;
        }

        Forbid();
        if (!segsem)
                segsem = (void *)FindSemaphore("SegTracker");

        if (segsem)
                segname = segsem->seg_Find(ev->calladdr, &hunk, &offset);

        if (segname) {
                strncpy(ev->segname, segname, MAX_SEGTRACKER_LEN);
                ev->segname[MAX_SEGTRACKER_LEN-1] = 0;
                ev->hunk   = hunk;
                ev->offset = offset;
        } else {
                if (ev->calladdr >= RomStart && ev->calladdr <= RomEnd) {
                        /*
                         *              Let's search the resident list for it ourselves.
                         *              and see if we can't do any better.
                         */
                        struct Resident *res;
                        ULONG *pres = (ULONG *)((struct ExecBase *)SysBase)->ResModules;
                        UBYTE *p;

                        while (*pres) {
                                if (*pres & 0x80000000) {
                                        pres = (ULONG *)(*pres & 0x7FFFFFFF);
                                        continue;
                                }
                                res = (struct Resident *)*pres++;
                                if (ev->calladdr >= (ULONG)res
                                    && ev->calladdr <= (ULONG)res->rt_EndSkip) {
                                        ev->hunk    = 0;
                                        ev->offset  = ev->calladdr - (ULONG)res;
                                        strcpy(ev->segname, "ROM: ");
                                        strncat(ev->segname + 5, res->rt_IdString,
                                                                                     MAX_SEGTRACKER_LEN - 6);
                                        ev->segname[MAX_SEGTRACKER_LEN-1] = 0;
                                        /*
                                         *              Now filter out any remaining carriage returns
                                         *              and linefeeds, since they look ugly when printed
                                         *              We also convert any open brackets into '\0's
                                         *              since these are just dates which we don't care
                                         *              about -- it looks neater to just display the
                                         *              module name and version.
                                         */
                                        for (p = ev->segname; *p; p++) {
                                                if (*p < ' ')
                                                        *p = ' ';
                                                if (*p == '(')
                                                        *p = '\0';
                                        }
                                        break;
                                }
                        }
                }
                if (ev->hunk == INVALID_HUNK)
                        ev->segname = MSG(MSG_SEG_MODULE_NOT_FOUND);
        }
        Permit();
}

/*
 *              FormatEvent(eventformat, event, outputstr, start, end)
 *
 *              Formats the specified event according to the passed in format
 *              array. Only that portion of the fully formatted event which
 *              lies from the 'start' to 'end' characters is produced, and
 *              output always starts at the beginning of the outputstr.
 *
 *              For example, a start of 4 and end of 8 will produce a formatted
 *              string of exactly 5 characters.
 *
 *              Each entry in the eventformat array is decoded, and text which
 *              corresponds to the entry type is copied to the output string. If
 *              the text is longer than the entry width setting, it is truncated;
 *              if shorter, it is padded with spaces.
 *
 *              Each event is separated by one space character.
 *
 *              If the event pointer is NULL, then the title headings are returned
 *              instead of the event contents.
 */
void FormatEvent(EventFormat *eventformat, Event *event,
                                 char *outstr, int start, int end)
{
        static char hexbuf8[]  = "00000000";
        static char hexbuf13[] = "0000:00000000";
        static char countbuf[] = "             ";

        EventFormat *ef = eventformat;
        int savechar = 0;
        char *savep;
        char *p;
        int col = 0;

        end++;          /* Make exclusive rather than inclusive */

        while (ef->type != EF_END && col < end) {
                int width = ef->width;

                /*
                 *              First skip over any entries that fall before desired section
                 */
                if ((col + width) < start) {
                        col += width + 1;
                        if (col > start)
                                *outstr++ = ' ';        /* Add leading space */
                        ef++;
                        continue;
                }

                /*
                 *              Now parse our current entry
                 */
                if (event) {
                        switch (ef->type) {

                        case EF_PROCNAME:       p = event->procname;                                    break;
                        case EF_ACTION:         p = event->action;                                              break;
                        case EF_OPTIONS:        p = event->options;                                             break;
                        case EF_RESULT:         p = event->result;                                              break;
                        case EF_CALLADDR:
                                p = ltoap(hexbuf8, event->calladdr, 8);
                                if (*p == ' ')
                                        p++;
                                break;

                        case EF_PROCID:
                                p = ltoap(hexbuf8, event->processid,8);
                                if (*p == ' ')
                                        p++;
                                break;

                        case EF_FILENAME:
                        {
                                /*
                                 *              This is a little trickier since we allow the user
                                 *              to view it left-aligned or right-aligned. For
                                 *              right-aligned, we temporarily replace the first char
                                 *              with a « to indicate there are additional characters
                                 *              to the left, then restore it after we've copied the
                                 *              string data later on. This is much quicker (and
                                 *              safer) than copying the entire string to a temporary
                                 *              just so we can patch a single character of it.
                                 */
                                int len;

                                p = event->filename;
                                if (RightAligned) {
                                        len = strlen(p);
                                        if (len > width) {
                                                p += len - width;
                                                if (width > 1) {
                                                        savep    = p;
                                                        savechar = *p;
                                                        *p       = '«';
                                                }
                                        }
                                }
                                break;
                        }

                        case EF_DATE:
                                if ((event->flags & EFLG_DONEDATE) == 0) {
                                        SetEventDateStr(event);
                                        event->flags |= EFLG_DONEDATE;
                                }
                                p = event->date;
                                break;

                        case EF_TIME:
                                if ((event->flags & EFLG_DONEDATE) == 0) {
                                        SetEventDateStr(event);
                                        event->flags |= EFLG_DONEDATE;
                                }
                                p = event->time;
                                break;

                        case EF_SEGNAME:
                                if ((event->flags & EFLG_DONESEG) == 0) {
                                        SetEventSegmentStr(event);
                                        event->flags |= EFLG_DONESEG;
                                }
                                p = event->segname;
                                break;

                        case EF_COUNT:
                                /*
                                 *              We subtract BaseSeq from the event number when
                                 *              displaying it so that we have a convenient way of
                                 *              making the event number appear to return to 1,
                                 *              while internally still ensuring that the event
                                 *              number is ALWAYS monotonically increasing, even
                                 *              across buffer clears and the like.
                                 */
                                mysprintf(countbuf, "%ld", event->seqnum - BaseSeq);
                                p = countbuf;
                                break;

                        case EF_HUNKOFFSET:
                                if ((event->flags & EFLG_DONESEG) == 0) {
                                        SetEventSegmentStr(event);
                                        event->flags |= EFLG_DONESEG;
                                }
                                if (event->hunk == INVALID_HUNK) {
                                        /*
                                         *              If we couldn't locate the module address,
                                         *              then don't bother printing the hunk/offset
                                         *              info since it will only be zero anyway.
                                         */
                                        p = ""; /* Couldn't find module so don't print hunk addr */
                                } else {
                                        /*
                                         *              Only output 6 digits of offset and 2 digits
                                         *              of hunk, but arrange that we can handle a
                                         *              full 8 / 4 combination if necessary
                                         */
                                        p = ltoa(hexbuf13+7, event->offset, 6);
                                        *--p = ':';
                                        p = ltoap(p-2, event->hunk, 2);
                                }
                                break;

                        default:
                                p = "<Invalid>";
                                break;
                        }
                } else {
                        /*
                         *              Format column headings instead
                         */
                        p = MSG(ef->titlemsgid);
                }

                /*
                 *              Okay, so now we have p pointing to our string. We now need to
                 *              copy a sufficient number of characters into the string to
                 *              fulfill our need.
                 */
                if (col < start) {
                        /*
                         *              Our first special case is where we have to skip over some of
                         *              the early characters, i.e. chars to left are clipped.
                         */
                        while (col < start && *p) {
                                col++;
                                width--;
                                p++;
                        }
                        if (col < start) {
                                /*
                                 *              Ran out of characters in our string, so pretend we
                                 *              have a null string and let the following code pad
                                 *              the remaining width with spaces.
                                 */
                                width -= (start - col);
                                col    = start;
                        }
                }

                /*
                 *              Now copy characters intact into string
                 */
                if ((col + width) > end) {
                        /*
                         *              This field needs to be clipped. We do this quite simply
                         *              by adjusting the width to be the remaining number of
                         *              chars in the string.
                         */
                        width = end - col;
                }

                /*
                 *              Now copy width chars from our string to the output buffer,
                 *              padding with spaces as necessary.
                 */
                while (width && *p) {
                        *outstr++ = *p++;
                        width--;
                        col++;
                }
                if (width) {
                        memset(outstr, ' ', width);
                        outstr += width;
                        col    += width;
                }
                *outstr++ = ' ';                        /* Space in between output fields       */
                col++;                                          /* onto next column                                     */
                ef++;                                           /* Onto the next format field           */
                /*
                 *              Finally, restore the character we replaced with « earlier
                 *              (if any)
                 */
                if (savechar) {
                        *savep   = savechar;
                        savechar = 0;
                }
        }

        /*
         *              Okay, we've generated the string as requested. Now check to see if
         *              we need to pad the remainder of the string with spaces (i.e. the
         *              number of fields listed wasn't as wide as the whole window).
         *
         *              Ideally, we will never be passed in a start/end pair wider than
         *              the width of the formatted output, but you never know...
         */
        if (col <= end) {
                memset(outstr, ' ', end-col+1);
                outstr += end-col+1;
        }
        outstr[-1] = '\0';              /* Remove final terminating space */
}

/*
 *              UnderlineTitles(eventformat, outstr, underchar)
 *
 *              This function creates a string of underlines which matches the
 *              event headings in the passed event format, such that if the
 *              headings were printed out in full, the underline string would
 *              line up properly. underchar is the char used for underlining.
 *
 *              The reason we can't just take the obvious approach and generate
 *              a title string, then convert all non-space chars to minus, is
 *              because we can have titles like "Process Name" and we want the
 *              underline to be continuous, not break between process and name.
 *
 *              Returns a pointer to the newly formatted string (outstr). Outstr
 *              must have room to store the entire width, as defined by the
 *              event format.
 */
char *UnderlineTitles(EventFormat *ef, char *outstr, char underchar)
{
        char *p = outstr;

        while (ef->type != EF_END) {
                int width    = ef->width;
                int titleid  = ef->titlemsgid;
                int titlelen = strlen(MSG(titleid));

                if (titlelen > width)
                        titlelen = width;

                memset(p, underchar, titlelen);
                p     += titlelen;
                width -= titlelen-1;            /* Include an extra space between cols */
                if (width) {
                        memset(p, ' ', width);
                        p += width;
                }
                ef++;
        }
        *--p = 0;                                               /* Replace final space with a null */
        return (outstr);
}

#if TESTING
/*
 *              TestEventFields()
 *
 *              Simple test function to check out our format conversion routines
 */
void TestEventFields(void)
{
        static char inline[200];
        static char formatstr[200];
        static EventFormat evformat[50];

        while (gets(inline) != NULL) {
                int len;

                if (*inline == 'q')
                        break;

                len = ParseFormatString(inline, evformat, 50);
                BuildFormatString(evformat, formatstr, 200);

                printf("Converted string: >>%s<<, length = %d\n", formatstr, len);
        }
}

/*
 *              TestFormat
 *
 *              Test our text formatting routines
 */
void TestFormat()
{
        static char inline[200];
        static char formatstr[200];
        static EventFormat evformat[50];
        static char outstr[500];
        Event *ev;
        int showtitles = 0;

        for (;;) {
                int len;

                printf("Enter a format string (q to quit): ");
                gets(inline);
                if (*inline == 'q')
                        return;

                len = ParseFormatString(inline, evformat, 50);
                BuildFormatString(evformat, formatstr, 200);
                printf("Actual string is >> %s << (%d chars)\n", formatstr, len);

                /*
                 *              Get ourselves a new event
                 */
                ev = GetNewEvent(0);
                if (!ev) {
                        printf("Uhoh! Couldn't allocate event!\n");
                        continue;
                }
                ev->procname  = "[ 1] SAS/C compiler";
                ev->filename  = "s:startup-sequence";
                ev->action    = "Open";
                ev->result    = "Okay";
                ev->calladdr  = 0x12345678;
                ev->processid = 0xDEADBEEF;
                DateStamp(&ev->datestamp);

                for (;;) {
                        int start, end;

                        printf("Now enter start,end values (-1 to quit, s to toggle): ");
                        gets(inline);
                        if (*inline == 'q')
                                return;
                        if (*inline == 's') {
                                showtitles = !showtitles;
                                printf("Now printing %s\n", showtitles ? "titles" : "contents");
                                continue;
                        }
                        sscanf(inline, "%d,%d", &start, &end);
                        if (start == -1)
                                break;

                        if (showtitles)
                                FormatEvent(evformat, NULL, outstr, start, end);
                        else
                                FormatEvent(evformat, ev, outstr, start, end);
                        printf("|%s|\n", outstr);
                }
        }
}

/*
 *              TestEvents
 *
 *              Creates some random events, then prints them out
 */
void TestEvents(void)
{
        int howmany = 0;
        int i;

        printf("Generate how many events? ");
        scanf("%d", &howmany);

        for (i = 0; i < howmany; i++) {
                Event *ev = GetNewEvent((rand() & 63));

                printf("Event head: %8x   Event new: %8x\n",
                                (UBYTE *)HeadNode(&EventList) - BufferStart,
                                (UBYTE *)ev - BufferStart);
        }
}
#endif

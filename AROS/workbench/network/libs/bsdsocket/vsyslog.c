/*
    Copyright © 2000-2007, The AROS Development Team. All rights reserved.
    $Id$

    Desc:
    Lang: English
*/

#include <stdio.h>
#include <time.h>

#include <libraries/bsdsocket.h>

#include "bsdsocket_intern.h"

static int _asprintf(char **strp, const char *format, ...);
static int _vasprintf(char **strp, const char *format, va_list args);

/*****************************************************************************

    NAME */

        AROS_LH3(void, vsyslog,

/*  SYNOPSIS */
        AROS_LHA(int,          level,  D0),
        AROS_LHA(const char *, format, A0),
        AROS_LHA(va_list,      args,   A1),

/*  LOCATION */
        struct bsdsocketBase *, SocketBase, 43, BSDSocket)

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    int errno_val = SocketBase->bsd_errno.value;
    const char *errmsg, *fp;
    char *msg, *cp, *freeformat;
    int msgs;
    struct sockaddr_in sa;

    /* If we need a syslog socket, create one. */
    if (SocketBase->bsd_syslog < 0) {
        SocketBase->bsd_syslog = socket(AF_INET, SOCK_DGRAM, 0);
    }

    if (SocketBase->bsd_syslog < 0) {
        /* Nothing to log to! */
        return;
    }

    sa.sin_family = AF_INET;
    sa.sin_port = htons(514);
    sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (errno_val != 0) {
        struct TagItem tags[2] = {
            { SBTM_GETREF(SBTC_ERRNOSTRPTR), errno_val },
            { TAG_END }
        };

        if (0 == SocketBaseTagList(&tags[0])) {
            errmsg = (APTR)tags[0].ti_Data;
        } else {
            errmsg = "Unknown error";
        }
    } else {
        errmsg = "Success";
    }

    /* Count the number of occurrances of '%m' in the format string */
    msgs = 0;
    fp = format;
    do {
        cp = strstr(fp, "%m");
        if (cp != NULL) {
            fp = cp + 2;
            msgs++;
        }
    } while (cp != NULL);

    if (msgs) {
        int errmsg_len = strlen(errmsg);
        int total_size = strlen(format) + errmsg_len * msgs + 1;
        char *ffp;

        freeformat = AllocVec(total_size, MEMF_ANY);
        if (!freeformat) {
            /* Not enough space to print the message... */
            return;
        }

        /* Inject the error messages
         * NOTE: There is a potential problem where an error message
         *       could contain a '%' format escape sequence, and lead
         *       to mis-reading the vprintf() argument list, but I
         *       think this is not a huge problem, as this library
         *       should be in full control of the error message strings
         *       from SBTC_ERRNOSTRPTR. - jason.mcmullan@gmail.com
         */
        fp = format;
        ffp = freeformat;
        do {
            cp = strstr(fp, "%m");
            if (cp) {
                CopyMem(fp, ffp, cp - fp);
                ffp += cp - fp;
                CopyMem(errmsg, ffp, errmsg_len);
                ffp += errmsg_len;
                fp = cp + 2;
            }
        } while (cp);
        strcpy(ffp, fp);

        format = freeformat;
    } else {
        freeformat = NULL;
    }

    /* Get the expanded message we want to send */
    if (_vasprintf(&msg, format, args) >= 0) {
        char *rfc5424;
        /* Calculate the RFC 5424 string for the message:
         * <pri>v year-mo-dyThh:mm:ss.ppZ hostname taskname pid - - msg
         */
        struct Task *me = FindTask(NULL);
        char hostname[64];
        struct tm tm;
        time_t t;

        time(&t);
        gmtime_r(&t, &tm);

        gethostname(hostname, sizeof(hostname));
        hostname[sizeof(hostname)-1]=0;

        if (_asprintf(&rfc5424, "<%d>%d %04d-%02d-%02dT%02d:%02d:%02d.%02dZ %s %s %llu - - %s",
                    level, 1, 
                    tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, 
                    tm.tm_hour, tm.tm_min, tm.tm_sec, 0,
                    hostname,
                    me->tc_Node.ln_Name,
                    (unsigned long long)(IPTR)me, msg) >= 0) {
            sendto(SocketBase->bsd_syslog, rfc5424, strlen(rfc5424), 0, (struct sockaddr *)&sa, sizeof(sa));
            FreeVec(rfc5424);
        }

        FreeVec(msg);
    }

    if (freeformat) {
        FreeVec(freeformat);
    }

    AROS_LIBFUNC_EXIT

} /* vsyslog */

static int _vasprintf(char **strp, const char *format, va_list args)
{
    char tiny[1], *str;
    int len;

    len = vsnprintf(tiny, sizeof(tiny), format, args);
    if (len < 0)
        return -1;

    str = AllocVec(len + 1, MEMF_ANY);

    str[0] = 0;
    *strp = str;

    return vsnprintf(str, len + 1, format, args);
}

static int _asprintf(char **strp, const char *format, ...)
{
    int rc;
    va_list args;

    va_start(args, format);
    rc = _vasprintf(strp, format, args);
    va_end(args);

    return rc;
}


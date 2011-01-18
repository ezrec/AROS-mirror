/******************************************************************************

    MODUL
	$Id$

    DESCRIPTION
	Provide standard clipboard.device interface functions like Open,
	Close, Read, Write, etc.

	This is typed from "Amiga ROM Kernel Reference Manual: Devices"
	Page 50 and following.

    NOTES
	These functions are usefull for writing and reading simple FTXT.
	Writing and reading complex FTXT, ILBM, etc. requires more work.
	Under 2.0 it is highly recommended that you use iffparse.library.

******************************************************************************/

/* Includes */
#include <exec/types.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <exec/memory.h>
#include <devices/clipboard.h>
#include "clipboard.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <proto/exec.h>
#include <proto/alib.h>

/* Globale Variable */

/* Interne Defines & Strukturen */
/*#define DEBUG*/

/* Interne Variable */

/* Interne Prototypes */
static int	      ReadLong	 (struct IOClipReq *, ULONG *);
static struct cbbuf * FillCBData (struct IOClipReq *, ULONG);


/*****************************************************************************

    NAME
	CBOpen - Open the clipboard.device

    PARAMETER
	ULONG unit;

    RESULT

    RETURN
	struct IOClipReq * ior;     Initialized IOClipReq-structure or NULL

    DESCRIPTION
	Opens the clipboard.device. A clipboard unit number must be passed in
	as an argument. By default, the unit number should be 0 (currently
	valid unit numbers are 0-255).

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	29. May 1992	ada created

******************************************************************************/

struct IOClipReq * CBOpen (ULONG unit)
{
    struct MsgPort * mp;
    struct IOStdReq * ior;

    if (mp = CreatePort (NULL, NULL)) {
	if (ior = (struct IOStdReq *)CreateExtIO (mp,
		sizeof (struct IOClipReq))) {
	    if (!(OpenDevice ((STRPTR)"clipboard.device", unit,
		    (struct IORequest *)ior, NULL))) {
#ifdef DEBUG
    printf ("Open of clipboard.device unit %d successful\n", unit);
#endif
		return ((struct IOClipReq *)ior);
	    }
	}
    }

#ifdef DEBUG
    printf ("No clipboard.device unit %d\n", unit);
#endif

    return (NULL);
} /* CBOpen */


/*****************************************************************************

    NAME
	CBClose - Close the clipboard.device

    PARAMETER
	struct IOClipReq * ior;

    RESULT

    RETURN
	void

    DESCRIPTION
	Close the clipboard.device unit which was opened via CBOpen().

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	29. May 1992	ada created

******************************************************************************/

void CBClose (struct IOClipReq * ior)
{
    struct MsgPort * mp;

    mp = ior->io_Message.mn_ReplyPort;

    CloseDevice ((struct IORequest *)ior);
    DeleteExtIO ((struct IORequest *)ior);
    DeletePort (mp);
} /* CBClose */


/*****************************************************************************

    NAME
	CBWriteFTXT - Write a string of text to the clipboard

    PARAMETER
	struct IOClipReq * ior;
	char		 * string;

    RESULT

    RETURN
	int success;	    TRUE/FALSE

    DESCRIPTION
	Write a NULL terminated string of text to the clipboard. The string
	will be written in simple FTXT format.

	Note that this function pads odd length strings automatically to
	conform to the IFF standard.

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	29. May 1992	ada created

******************************************************************************/

int CBWriteFTXT (struct IOClipReq * ior, char * string)
{
    ULONG length;	/* EVEN string-length */
    ULONG slen; 	/* length of string */
    BOOL  odd;		/* ODD(slen) */
    static ULONG IFF_header[] = {
	ID_FORM,
	0,
	ID_FTXT,
	ID_CHRS,
	0
    };

    slen = strlen (string);
    odd  = slen & 1;

    length = odd ? slen+1 : slen;

    /* initial setup for Offset, Error and ClipID */
    ior->io_Offset  = 0;
    ior->io_Error   = 0;
    ior->io_ClipID  = 0;

    /* Create the IFF-header information and write it */
    IFF_header[1] = length + 12;
    IFF_header[4] = slen;

    ior->io_Data    = (STRPTR)IFF_header;
    ior->io_Length  = sizeof (IFF_header);
    ior->io_Command = CMD_WRITE;
    DoIO ((struct IORequest *)ior);

    /* write string itself */
    ior->io_Data    = (STRPTR)string;
    ior->io_Length  = length;	       /* We can write slen+1 Bytes since
					  a C-String always has a '\0'-Byte
					  at his end. */
    ior->io_Command = CMD_WRITE;
    DoIO ((struct IORequest *)ior);

    /* Tell the clipboard we are done writing */
    ior->io_Command = CMD_UPDATE;
    DoIO ((struct IORequest *)ior);

    return (ior->io_Error ? FALSE : TRUE);
} /* CBWriteFTXT */


/*****************************************************************************

    NAME
	CBQueryFTXT

    PARAMETER
	struct IOClipReq * ior;

    RESULT

    RETURN
	int result;	    TRUE/FALSE

    DESCRIPTION
	Check if the clipboard contains a FTXT.

    NOTES
	If this function returns TRUE, you MUST either call CBReadCHRS()
	until it returns FALSE or CBReadDone() to tell the clipboard.device
	that we are done reading.

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	29. May 1992	ada created

******************************************************************************/

int CBQueryFTXT (struct IOClipReq * ior)
{
    ULONG cbuff[3];

    /* initial setup for Offset, Error and ClipID */
    ior->io_Offset  = 0;
    ior->io_Error   = 0;
    ior->io_ClipID  = 0;

    /* Look for "FORM[size]FTXT" */
    ior->io_Data    = (STRPTR)cbuff;
    ior->io_Length  = 12;	    /* 3*4 Bytes */
    ior->io_Command = CMD_READ;
    DoIO ((struct IORequest *)ior);

    /* Have we at least 12 bytes and a valid IFF-Header ? */
    if (ior->io_Actual == 12L && cbuff[0] == ID_FORM && cbuff[2] == ID_FTXT) {
#ifdef DEBUG
    printf ("FTXT ok\n");
#endif
	return (TRUE);
    }

#ifdef DEBUG
    printf ("No FTXT\n");
#endif

    CBReadDone (ior);
    return (FALSE);
} /* CBQueryFTXT */


/*****************************************************************************

    NAME
	CBReadCHRS

    PARAMETER
	struct IOClipReq * ior;

    RESULT

    RETURN
	struct cbbuf * cbbuf;	    NULL on failure (out-of-memory or
				    no more CHRS-chunks).

    DESCRIPTION
	Reads and returns the text in the next CHRS chunk (if any) from the
	clipboard.

	Allocates memory to hold data in next CHRS chunk.

    NOTES
	The caller must free the returned buffer when done via CBFreeBuf().

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	29. May 1992	ada created

******************************************************************************/

struct cbbuf * CBReadCHRS (struct IOClipReq * ior)
{
    ULONG chunk;
    ULONG size;
    struct cbbuf * buf;

    /* Find next CHRS chunk */
    buf = NULL;

    for (;;) {
	if (!(ReadLong (ior, &chunk)) )
	    break;

	if (!(ReadLong (ior, &size)) )
	    break;

	/* if it is a CHRS-hunk */
	if (chunk == ID_CHRS) {
#ifdef DEBUG
    printf ("Found CHRS\n");
#endif
#ifdef DEBUG
    printf ("CHRS length = %d\n", size);
#endif
	    if (size) { /* Read data */
		buf = FillCBData (ior, size);
		break;
	    }
	} else { /* else skip this hunk */
	    if (size & 1)
		size ++;

	    ior->io_Offset += size;
	}
    }

    /* tell clipboard we are done */
    if (!buf)
	CBReadDone (ior);

#ifdef DEBUG
    printf ("Found '%s'\n", buf->mem);
#endif

    return (buf);
} /* CBReadCHRS */


static int ReadLong (struct IOClipReq * ior, ULONG * ldata)
{
    ior->io_Data    = (STRPTR)ldata;
    ior->io_Length  = 4;
    ior->io_Command = CMD_READ;

    DoIO ((struct IORequest *)ior);

    if (ior->io_Actual == 4 && !ior->io_Error)
	return (TRUE);

    return (FALSE);
}


static struct cbbuf * FillCBData (struct IOClipReq * ior, ULONG size)
{
    register UBYTE * to;
    register UBYTE * from;
    register ULONG   t;
    register ULONG   count;
    struct cbbuf   * buf;

    if (!size)
	return (NULL);

    if (size & 1)
	size ++;

    if (buf = AllocMem (sizeof (struct cbbuf) + size, MEMF_PUBLIC)) {
	buf->size = sizeof (struct cbbuf) + size;

	ior->io_Data	= (STRPTR)buf->mem;
	ior->io_Length	= size;
	ior->io_Command = CMD_READ;

	to = (UBYTE *)buf->mem;
	count = NULL;

	if (!(DoIO ((struct IORequest *)ior)) && ior->io_Actual == size) {
	    for (t=0, from=to; t<size; t++) {
		if (*from) {
		    *to ++ = * from;
		    count ++;
		}

		from ++;
	    } /* for all bytes in the buffer */

	    *to = 0;	/* terminate buffer with 0 */
	    buf->count = count;
	} else { /* if DoIO && number of bytes */
	    FreeMem (buf, buf->size);
	    return (NULL);
	}
    }

    return (buf);
} /* FillCBData */


/*****************************************************************************

    NAME
	CBReadDone - Tell clipboard we are done reading

    PARAMETER
	struct IOClipReq * ior;

    RESULT

    RETURN
	void

    DESCRIPTION
	Reads past end of clipboard file until io_Actual is equal to 0. This
	tells the clipboard that we are done reading.

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	29. May 1992	ada created

******************************************************************************/

void CBReadDone (struct IOClipReq * ior)
{
    char tmp_buffer[64];

    ior->io_Data    = (STRPTR)tmp_buffer;
    ior->io_Length  = sizeof(tmp_buffer);
    ior->io_Command = CMD_READ;

    /* falls through immediately if io_Actual == 0 */
    while (ior->io_Actual) {
	if (DoIO ((struct IORequest *)ior))
	    break;
    }

} /* CBReadDone */


/*****************************************************************************

    NAME
	CBFreeBuf - Free buffer allocated by CBReadCHRS()

    PARAMETER
	struct cbbuf * buf;

    RESULT

    RETURN
	void

    DESCRIPTION
	Frees a buffer allocated by CBReadCHRS().

    NOTES

    BUGS

    EXAMPLES

    SEE ALSO

    INTERNALS

    HISTORY
	29. May 1992	ada created

******************************************************************************/

void CBFreeBuf (struct cbbuf * buf)
{
    FreeMem (buf, buf->size);
} /* CBFreeBuf */


/******************************************************************************
*****  ENDE clipboard.c
******************************************************************************/

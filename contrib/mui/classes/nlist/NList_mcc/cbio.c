/***************************************************************************

 NList.mcc - New List MUI Custom Class
 Registered MUI class, Serial Number:

 Copyright (C) 1996-2004 by Gilles Masson,
                            Carsten Scholling <aphaso@aphaso.de>,
                            Przemyslaw Grunchala,
                            Sebastian Bauer <sebauer@t-online.de>,
                            Jens Langner <Jens.Langner@light-speed.de>

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 NList classes Support Site:  http://www.sf.net/projects/nlist-classes

 $Id$

***************************************************************************/

#include "exec/types.h"
#include "exec/ports.h"
#include "exec/io.h"
#include "exec/memory.h"
#include "devices/clipboard.h"

#define CBIO 1

#include "cbio.h"

#include <proto/exec.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/****** cbio/CBOpen *************************************************
*
*   NAME
*       CBOpen() -- Open the clipboard.device
*
*   SYNOPSIS
*       ior = CBOpen(unit)
*
*       struct IOClipReq *CBOpen( ULONG )
*
*   FUNCTION
*       Opens the clipboard.device.  A clipboard unit number
*       must be passed in as an argument.  By default, the unit
*       number should be 0 (currently valid unit numbers are
*       0-255).
*
*   RESULTS
*       A pointer to an initialized IOClipReq structure, or
*       a NULL pointer if the function fails.
*
*********************************************************************/


struct IOClipReq *CBOpen(ULONG unit)
{
  struct MsgPort *mp;
  struct IOClipReq *ior;

  if((mp = CreateMsgPort()))
  {
    if((ior = (struct IOClipReq *)CreateIORequest(mp,sizeof(struct IOClipReq))))
    {
      if(!(OpenDevice("clipboard.device",unit,(struct IORequest *)ior,0L)))
      {
        return (ior);
      }

      DeleteIORequest((struct IORequest *)ior);
    }

    DeleteMsgPort(mp);
  }

  return(NULL);
}

/****** cbio/CBClose ************************************************
*
*   NAME
*       CBClose() -- Close the clipboard.device
*
*   SYNOPSIS
*       CBClose()
*
*       void CBClose()
*
*   FUNCTION
*       Close the clipboard.device unit which was opened via
*       CBOpen().
*
*********************************************************************/

void CBClose(struct IOClipReq *ior)
{
  struct MsgPort *mp;

  mp = ior->io_Message.mn_ReplyPort;

  CloseDevice((struct IORequest *)ior);
  DeleteIORequest((struct IORequest *)ior);
  DeleteMsgPort(mp);
}

/****** cbio/CBWriteFTXT *********************************************
*
*   NAME
*       CBWriteFTXT() -- Write a string of text to the clipboard.device
*
*   SYNOPSIS
*       success = CBWriteFTXT( ior, string)
*
*       int CBWriteFTXT(struct IOClipReq *, char *)
*
*   FUNCTION
*       Write a NULL terminated string of text to the clipboard.
*       The string will be written in simple FTXT format.
*
*       Note that this function pads odd length strings automatically
*       to conform to the IFF standard.
*
*   RESULTS
*       TRUE if the write succeeded, else FALSE.
*
*********************************************************************/

int CBWriteFTXT(struct IOClipReq *ior,char *string)
{
  ULONG length, slen;
  BOOL odd;
  int success;

  slen = strlen(string);
  odd = (slen & 1);               /* pad byte flag */

  length = (odd) ? slen+1 : slen;

  /* initial set-up for Offset, Error, and ClipID */

  ior->io_Offset = 0;
  ior->io_Error  = 0;
  ior->io_ClipID = 0;

  /* Create the IFF header information */

  WriteLong(ior, (long *) "FORM");     /* "FORM"             */
  length+=12L;                         /* + "[size]FTXTCHRS" */
  WriteLong(ior, (long *) &length);    /* total length       */
  WriteLong(ior, (long *) "FTXT");     /* "FTXT"             */
  WriteLong(ior, (long *) "CHRS");     /* "CHRS"             */
  WriteLong(ior, (long *) &slen);      /* string length      */

  /* Write string */
  ior->io_Data    = (STRPTR)string;
  ior->io_Length  = slen;
  ior->io_Command = CMD_WRITE;
  DoIO( (struct IORequest *) ior);

  /* Pad if needed */
  if (odd)
  {
    ior->io_Data   = (STRPTR)"";
    ior->io_Length = 1L;
    DoIO( (struct IORequest *) ior);
  }

  /* Tell the clipboard we are done writing */

  ior->io_Command=CMD_UPDATE;
  DoIO( (struct IORequest *) ior);

  /* Check if io_Error was set by any of the preceding IO requests */
  success = ior->io_Error ? FALSE : TRUE;

  return(success);
}

int WriteLong(struct IOClipReq *ior, long *ldata)
{
  ior->io_Data    = (STRPTR)ldata;
  ior->io_Length  = 4L;
  ior->io_Command = CMD_WRITE;
  DoIO( (struct IORequest *) ior);

  if (ior->io_Actual == 4)
  {
    return( ior->io_Error ? FALSE : TRUE);
  }
  return(FALSE);
}

/****** cbio/CBQueryFTXT **********************************************
*
*   NAME
*       CBQueryFTXT() -- Check to see if clipboard contains FTXT
*
*   SYNOPSIS
*       result = CBQueryFTXT( ior )
*
*       int CBQueryFTXT(struct IOClipReq *)
*
*   FUNCTION
*       Check to see if the clipboard contains FTXT.  If so,
*       call CBReadCHRS() one or more times until all CHRS
*       chunks have been read.
*
*   RESULTS
*       TRUE if the clipboard contains an FTXT chunk, else FALSE.
*
*   NOTES
*       If this function returns TRUE, you must either call
*       CBReadCHRS() until CBReadCHRS() returns FALSE, or
*       call CBReadDone() to tell the clipboard.device that
*       you are done reading.
*
*********************************************************************/

int CBQueryFTXT(struct IOClipReq *ior)
{
  ULONG cbuff[4];
  /* initial set-up for Offset, Error, and ClipID */
  ior->io_Offset = 0;
  ior->io_Error  = 0;
  ior->io_ClipID = 0;
  /* Look for "FORM[size]FTXT" */
  ior->io_Command = CMD_READ;
  ior->io_Data    = (STRPTR)cbuff;
  ior->io_Length  = 12;
  DoIO( (struct IORequest *) ior);
  /* Check to see if we have at least 12 bytes */
  if (ior->io_Actual == 12L)
  {
    /* Check to see if it starts with "FORM" */
    if (cbuff[0] == ID_FORM)
    {
      /* Check to see if its "FTXT" */
      if (cbuff[2] == ID_FTXT)
          return(TRUE);
    }
    /* It's not "FORM[size]FTXT", so tell clipboard we are done */
  }
  CBReadDone(ior);
  return(FALSE);
}


/****** cbio/CBReadCHRS **********************************************
*
*   NAME
*       CBReadCHRS() -- Reads the next CHRS chunk from clipboard
*
*   SYNOPSIS
*       cbbuf = CBReadCHRS( ior )
*
*       struct cbbuf *CBReadCHRS(struct IOClipReq * )
*
*   FUNCTION
*       Reads and returns the text in the next CHRS chunk
*       (if any) from the clipboard.
*
*       Allocates memory to hold data in next CHRS chunk.
*
*   RESULTS
*       Pointer to a cbbuf struct (see cb.h), or a NULL indicating
*       a failure (e.g., not enough memory, or no more CHRS chunks).
*
*       ***Important***
*
*       The caller must free the returned buffer when done with the
*       data by calling CBFreeBuf().
*
*   NOTES
*       This function strips NULL bytes, however, a full reader may
*       wish to perform more complete checking to verify that the
*       text conforms to the IFF standard (stripping data as required).
*
*       Under 2.0, the AllocVec() function could be used instead of
*       AllocMem() in which case the cbbuf structure may not be
*       needed.
*
*********************************************************************/

struct cbbuf *CBReadCHRS(struct IOClipReq *ior)
{
  ULONG chunk,size;
  struct cbbuf *buf;
  int looking;
  /* Find next CHRS chunk */
  looking = TRUE;
  buf = NULL;
  while (looking)
  {
    looking = FALSE;
    if (ReadLong(ior,&chunk))
    {
      /* Is CHRS chunk ? */
      if (chunk == ID_CHRS)
      {
        /* Get size of chunk, and copy data */
        if (ReadLong(ior,&size))
        {
          if (size)
            buf=FillCBData(ior,size);
        }
      }
      /* If not, skip to next chunk */
      else
      {
        if (ReadLong(ior,&size))
        {
          looking = TRUE;
          if (size & 1)
            size++;    /* if odd size, add pad byte */
          ior->io_Offset += size;
        }
      }
    }
  }
  if (buf == NULL)
    CBReadDone(ior);        /* tell clipboard we are done */
  return(buf);
}


int ReadLong(struct IOClipReq *ior, ULONG *ldata)
{
  ior->io_Command = CMD_READ;
  ior->io_Data    = (STRPTR)ldata;
  ior->io_Length  = 4L;
  DoIO( (struct IORequest *) ior);
  if (ior->io_Actual == 4)
  {
    return( ior->io_Error ? FALSE : TRUE);
  }
  return(FALSE);
}


struct cbbuf *FillCBData(struct IOClipReq *ior,ULONG size)
{
  register UBYTE *to,*from;
  register ULONG x,count;
  ULONG length;
  struct cbbuf *buf,*success;

  success = NULL;

  if((buf = AllocMem(sizeof(struct cbbuf),MEMF_PUBLIC)))
  {
    length = size;
    if (size & 1)
      length++;            /* if odd size, read 1 more */
    if((buf->mem = AllocMem(length+1L,MEMF_PUBLIC)))
    {
      buf->size = length+1L;
      ior->io_Command = CMD_READ;
      ior->io_Data    = (STRPTR)buf->mem;
      ior->io_Length  = length;
      to = buf->mem;
      count = 0L;
      if (!(DoIO( (struct IORequest *) ior)))
      {
        if (ior->io_Actual == length)
        {
          success = buf;      /* everything succeeded */
          /* strip NULL bytes */
          for (x=0, from=buf->mem ;x<size;x++)
          {
            if (*from)
            {
              *to = *from;
              to++;
              count++;
            }
            from++;
          }
          *to=0x0;            /* Null terminate buffer */
          buf->count = count; /* cache count of chars in buf */
        }
      }
      if (!(success))
        FreeMem(buf->mem,buf->size);
    }
    if (!(success))
      FreeMem(buf,sizeof(struct cbbuf));
  }
  return(success);
}

/****** cbio/CBReadDone **********************************************
*
*   NAME
*       CBReadDone() -- Tell clipboard we are done reading
*
*   SYNOPSIS
*       CBReadDone( ior )
*
*       void CBReadDone(struct IOClipReq * )
*
*   FUNCTION
*       Reads past end of clipboard file until io_Actual is equal to 0.
*       This is tells the clipboard that we are done reading.
*
*********************************************************************/

void CBReadDone(struct IOClipReq *ior)
{
  char buffer[256];

  ior->io_Command = CMD_READ;
  ior->io_Data    = (STRPTR)buffer;
  ior->io_Length  = 254;

  /* falls through immediately if io_Actual == 0 */

  while (ior->io_Actual)
  {
    if (DoIO( (struct IORequest *) ior))
      break;
  }
}

/****** cbio/CBFreeBuf **********************************************
*
*   NAME
*       CBFreeBuf() -- Free buffer allocated by CBReadCHRS()
*
*   SYNOPSIS
*       CBFreeBuf( buf )
*
*       void CBFreeBuf( struct cbbuf * )
*
*   FUNCTION
*       Frees a buffer allocated by CBReadCHRS().
*
*********************************************************************/

void CBFreeBuf(struct cbbuf *buf)
{
  FreeMem(buf->mem, buf->size);
  FreeMem(buf, sizeof(struct cbbuf));
}


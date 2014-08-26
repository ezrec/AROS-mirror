/*
 *  Multiview 3
 *
 *  Copyright © 2007-2011 Ilkka Lehtoranta <ilkleht@yahoo.com>
 *  All rights reserved.
 *  
 *  $Id$
 */

#warning "!!!!!!!REPLACE THIS WITH SOME SYSTEM SOLUTION!!!!!!!"
#define USE_INLINE_STDARG 1

#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <devices/clipboard.h>
#if defined(__AROS__)
// FIXMDE: AROS
#define CODESET_UTF8 1
#else
#include <libraries/charsets.h>
#endif
#include <libraries/locale.h>
#include <proto/alib.h>
#if !defined(__AROS__)
#include <proto/charsets.h>
#endif
#include <proto/datatypes.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/iffparse.h>
#include <proto/intuition.h>

#define ID_FTXT	MAKE_ID('F','T','X','T')
#define ID_CHRS	MAKE_ID('C','H','R','S')
#define ID_UTF8	MAKE_ID('U','T','F','8')


STATIC APTR clipboard_open(void)
{
	struct IOClipReq *io;
	struct MsgPort *mp;

	mp = CreateMsgPort();

	if ((io = (struct IOClipReq *)CreateIORequest(mp,sizeof(struct IOClipReq))))
	{
		if (!(OpenDevice("clipboard.device", 0, (struct IORequest *)io, 0)))
		{
			return io;
		}
		DeleteIORequest((struct IORequest *)io);
	}

	DeleteMsgPort(mp);
	return NULL;
}


STATIC void clipboard_close(struct IOClipReq *io)
{
	if (io)
	{
		struct MsgPort *mp;

		mp = io->io_Message.mn_ReplyPort;

		CloseDevice((struct IORequest *)io);
		DeleteIORequest((struct IORequest *)io);
		DeleteMsgPort(mp);
	}
}


STATIC ULONG clipboard_write_data(struct IOClipReq *io, CONST_APTR data, ULONG len)
{
	LONG rc;

	io->io_Command = CMD_WRITE;
	io->io_Data    = (APTR)data;
	io->io_Length  = len;
	DoIO( (struct IORequest *)io);

	if (io->io_Actual != len)
	{
		io->io_Error = 1;
	}

	rc = io->io_Error ? FALSE : TRUE;
	return rc;
}


STATIC VOID clipboard_pad_text(struct IOClipReq *io, ULONG textlen)
{
	if (textlen & 1) clipboard_write_data(io, "", 1);
}


STATIC ULONG clipboard_write_header_and_text(struct IOClipReq *io, CONST_STRPTR string, ULONG slen, ULONG ulen)
{
	ULONG rc;

	struct
	{
		ULONG form;
		ULONG totalsize;
		ULONG ftxt;
		ULONG type;
		ULONG strlen;
	} iffheader;

	io->io_Offset = 0;
	io->io_Error  = 0;
//	io->io_ClipID = 0;

	/* FIXME: For correct operation we should also store font name. Used font
	 * is relevant with guides written in Japanese for example.
	 */

	iffheader.form      = ID_FORM;
	iffheader.totalsize = (slen & 1 ? slen + 1 : slen) + (ulen & 1 ? ulen + 1 : ulen) + 12 + 8;
	iffheader.ftxt      = ID_FTXT;
	iffheader.type      = ID_CHRS;
	iffheader.strlen    = slen;

	rc = FALSE;

	if (clipboard_write_data(io, &iffheader, sizeof(iffheader)))
	{
		if (clipboard_write_data(io, string, slen))
		{
			clipboard_pad_text(io, slen);
			rc = TRUE;
		}
	}

	return rc;
}


STATIC ULONG clipboard_write_utf8(struct IOClipReq *io, CONST_STRPTR utext, ULONG ulen)
{
	ULONG rc;

	struct
	{
		ULONG type;
		ULONG strlen;
	} utf8_header;

	/* FIXME: For correct operation we should also store font name. Used font
	 * is relevant with guides written in Japanese for example.
	 */

	utf8_header.type   = ID_UTF8;
	utf8_header.strlen = ulen;

	rc = FALSE;

	if (clipboard_write_data(io, &utf8_header, sizeof(utf8_header)))
	{
		if (clipboard_write_data(io, utext, ulen))
		{
			clipboard_pad_text(io, ulen);
			rc = TRUE;
		}
	}

	return rc;
}

STATIC VOID clipboard_finalize(struct IOClipReq *io)
{
	io->io_Command = CMD_UPDATE;
	DoIO((struct IORequest *)io);
}

static void clips_write(CONST_STRPTR stext, LONG slen, CONST_STRPTR utext, LONG ulen)
{
	APTR ctx;
	
	if ((ctx = clipboard_open()))
	{
		if (clipboard_write_header_and_text(ctx, stext, slen, ulen))
		{
			if (clipboard_write_utf8(ctx, utext, ulen))
			{
			}
		}
		
		clipboard_finalize(ctx);
		clipboard_close(ctx);
	}
}

//==============================================================================
// This function is fully automatic and is used for writing a single
// null-terminated buffer of text.
//
// codeset is either CODESET_LATIN1 or CODESET_UTF8

VOID clipboard_write_text(CONST_STRPTR string, ULONG codeset)
{
#if !defined(__AROS__)
// FIXME: AROS (maybe codesets.library)
	if (string)
	{
		struct Library *CharsetsBase = OpenLibrary("charsets.library", 52);
		
		if (CharsetsBase)
		{
			CONST_STRPTR stext = stext, utext = utext;
			STRPTR buf;
			LONG slen = slen, ulen = ulen, buflen, srcmib, dstmib;
		
			buflen = strlen(string) + 1;
		
			if (codeset == CODESET_UTF8)
			{
				srcmib = MIBENUM_UTF_8;
				dstmib = MIBENUM_SYSTEM;
				ulen   = buflen;
				utext  = string;
			}
			else	// Some system default maybe, who knows
			{
				srcmib = MIBENUM_SYSTEM;
				dstmib = MIBENUM_UTF_8;
				slen   = buflen;
				stext  = string;
			}

			ConvertTags((STRPTR)string, -1, NULL, -1, srcmib, dstmib, CST_GetDestBytes, (IPTR)&buflen, TAG_DONE);
			buf = AllocTaskPooled(buflen);
			
			if (buf)
			{
				if (codeset == CODESET_UTF8)
				{
					slen  = buflen;
					stext = buf;
				}
				else
				{
					ulen  = buflen;
					utext = buf;
				}
				
				ConvertTags((STRPTR)string, -1, buf, buflen, srcmib, dstmib, TAG_DONE);

				clips_write(stext, slen - 1, utext, ulen - 1);	// do not write terminating NULL
				
				FreeTaskPooled(buf, buflen);
			}
			
			CloseLibrary(CharsetsBase);
		}
	}
#endif
}


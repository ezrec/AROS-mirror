/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#ifndef ASYNCIO_H
#define ASYNCIO_H


/*****************************************************************************/


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef EXEC_PORTS_H
#include <exec/ports.h>
#endif

#ifndef DOS_DOS_H
#include <dos/dos.h>
#endif


/*****************************************************************************/


/* This structure is public only by necessity, don't muck with it yourself, or
 * you're looking for trouble
 */
struct AsyncFile
{
    BPTR                  af_File;
    ULONG                 af_BlockSize;
    struct MsgPort       *af_Handler;
    APTR                  af_Offset;
    LONG                  af_BytesLeft;
    ULONG	          af_BufferSize;
    APTR	          af_Buffers[2];
    struct StandardPacket af_Packet;
    struct MsgPort        af_PacketPort;
    ULONG                 af_CurrentBuf;
    ULONG                 af_SeekOffset;
    UBYTE	          af_PacketPending;
    UBYTE	          af_ReadMode;
};


/*****************************************************************************/


#define MODE_READ   0  /* read an existing file                             */
#define MODE_WRITE  1  /* create a new file, delete existing file if needed */
#define MODE_APPEND 2  /* append to end of existing file, or create new     */

#define MODE_START   -1   /* relative to start of file         */
#define MODE_CURRENT  0   /* relative to current file position */
#define MODE_END      1   /* relative to end of file	     */


/*****************************************************************************/


#ifdef __SASC_60
__stdargs struct AsyncFile *OpenAsync(const STRPTR fileName, UBYTE accessMode, LONG bufferSize);
__stdargs LONG CloseAsync(struct AsyncFile *file);
__stdargs LONG ReadAsync(struct AsyncFile *file, APTR buffer, LONG numBytes);
__stdargs LONG ReadCharAsync(struct AsyncFile *file);
__stdargs LONG WriteAsync(struct AsyncFile *file, APTR buffer, LONG numBytes);
__stdargs LONG WriteCharAsync(struct AsyncFile *file, UBYTE ch);
__stdargs LONG SeekAsync(struct AsyncFile *file, LONG position, BYTE mode);
#else

/* AROS: We have no __stdargs keyword */
struct AsyncFile * /* __stdargs */ OpenAsync(const STRPTR fileName, UBYTE accessMode, LONG bufferSize);
LONG /* __stdargs */ CloseAsync(struct AsyncFile *file);
LONG /* __stdargs */ ReadAsync(struct AsyncFile *file, APTR buffer, LONG numBytes);
LONG /* __stdargs */ ReadCharAsync(struct AsyncFile *file);
LONG /* __stdargs */ WriteAsync(struct AsyncFile *file, APTR buffer, LONG numBytes);
LONG /* __stdargs */ WriteCharAsync(struct AsyncFile *file, UBYTE ch);
LONG /* __stdargs */ SeekAsync(struct AsyncFile *file, LONG position, BYTE mode);
#endif

/*****************************************************************************/


#endif /* ASYNCIO_H */


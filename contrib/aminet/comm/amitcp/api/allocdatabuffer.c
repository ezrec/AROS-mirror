/*
 * Copyright (c) 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *                    Helsinki University of Technology, Finland.
 *                    All rights reserved.
 *
 * Created: Sun Jun 13 01:09:17 1993 too
 * Last modified: Wed Feb 16 08:31:10 1994 jraja
 *
 * HISTORY
 * $Log$
 * Revision 1.1  2001/12/25 22:21:39  henrik
 * lots of assembler in api/amiga_api.h that needs to be fixed
 * better not do it since my knowledge of assembler i almost zero
 * think one need to change adresses to something like eax,ebx or something?
 *
 * Revision 1.4  1994/02/16  06:31:55  jraja
 * Eliminated unnecessary checks :-) (see api/allocdatabuffer.h).
 *
 * Revision 1.3  1994/02/14  00:42:52  ppessi
 * Eliminated unnecessary allocations.
 *
 * Revision 1.2  1994/01/20  02:18:00  jraja
 * Added #include <conf.h> as the first include.
 *
 * Revision 1.1  1993/06/12  22:57:18  too
 * Initial revision
 *
 *
 */

#include <conf.h>

#include <exec/types.h>

#include <sys/malloc.h>

#include <api/amiga_api.h>
#include <api/allocdatabuffer.h>

BOOL doAllocDataBuffer(struct DataBuffer * DB, int size)
{
  if (DB->db_Addr)
    bsd_free(DB->db_Addr, M_TEMP);
  
  if ((DB->db_Addr = bsd_malloc(size, M_TEMP, M_WAITOK)) == NULL) {
    DB->db_Size = 0;
    return FALSE;
  }
  DB->db_Size = size;
  return TRUE;
}

VOID freeDataBuffer(struct DataBuffer * DB)
{
  if (DB->db_Addr)
    bsd_free(DB->db_Addr, M_TEMP);
  DB->db_Size = 0;
  DB->db_Addr = NULL;
}

BOOL allocDataBuffer(struct DataBuffer * DB, int size)
{
  if (DB->db_Size < size)
    return doAllocDataBuffer(DB, size);
  else
    return TRUE;
}

/*
 * allocdatabuffer.h
 *
 * 	Copyright (c) 1993 OHT-ATCP Group
 * 	    All rights reserved
 *
 * Created: Mon Apr 26 11:58:58 1993 too
 * Last modified: Sun Jun 13 01:23:43 1993 too
 *
 * $Id$
 *
 * HISTORY
 * $Log$
 * Revision 1.2  2001/12/26 19:30:19  henrik
 * Aros version
 *
 * Revision 1.1  2001/12/25 22:21:39  henrik
 *
 * Revision 1.3  1993/06/12  22:58:57  too
 * Added prototype for freeDataBuffer. Now part of allocDataBuffer is
 * inline function so it is faster.
 *
 * Revision 1.2  1993/06/07  12:37:20  too
 * Changed inet_ntoa, netdatabase functions and WaitSelect() use
 * separate buffers for their dynamic buffers
 *
 * Revision 1.1  1993/04/27  10:24:18  too
 * Initial revision
 *
 *
 */

#ifndef ALLOCDATABUFFER_H
#define ALLOCDATABUFFER_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef API_AMIGA_API_H
#include <api/amiga_api.h>
#endif

VOID freeDataBuffer(struct DataBuffer * DB);
BOOL doAllocDataBuffer(struct DataBuffer * DB, int size);

BOOL allocDataBuffer(struct DataBuffer * DB, int size);

#endif /* ALLOCDATABUFFER_H */



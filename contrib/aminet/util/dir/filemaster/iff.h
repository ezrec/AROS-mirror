/*
     Filemaster - Multitasking directory utility.
     Copyright (C) 2000  Toni Wilen
     
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
     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* 
 *
 * iff.h:	General Definitions for IFFParse modules
 *
 * 10/20/91
 */

#ifndef IFFP_IFF_H
#define IFFP_IFF_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef EXEC_MEMORY_H
#include <exec/memory.h>
#endif
#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif
#ifndef UTILITY_HOOKS_H
#include <utility/hooks.h>
#endif
#ifndef LIBRARIES_IFFPARSE_H
#include <libraries/iffparse.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX
#define	MAX(a,b)	((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define	MIN(a,b)	((a) < (b) ? (a) : (b))
#endif
#ifndef ABS
#define	ABS(x)		((x) < 0 ? -(x) : (x))
#endif

/* Locale stuff */
#include "iffpstringids.h"

#ifndef TEXTTABLE_H
struct AppString
{
  LONG as_ID;
  STRPTR as_Str;
};
#endif

extern struct AppString AppStrings[];
#define SI(i)  GetString(i)


#define CkErr(expression)  {if (!error) error = (expression);}
#define ChunkMoreBytes(cn)	(cn->cn_Size - cn->cn_Scan)
#define IS_ODD(a)		(a & 1)

#define IFF_OKAY	0L
#define	CLIENT_ERROR	1L
#define NOFILE          5L

#define message printf

/* Generic Chunk ID's we may encounter */
#define	ID_ANNO		MAKE_ID('A','N','N','O')
#define	ID_AUTH		MAKE_ID('A','U','T','H')
#define	ID_CHRS		MAKE_ID('C','H','R','S')
#define	ID_Copyright	MAKE_ID('(','c',')',' ')
#define	ID_CSET		MAKE_ID('C','S','E','T')
#define	ID_FVER		MAKE_ID('F','V','E','R')
#define	ID_NAME		MAKE_ID('N','A','M','E')
#define ID_TEXT		MAKE_ID('T','E','X','T')
#define ID_BODY		MAKE_ID('B','O','D','Y')


struct ParseInfo
{
  /* general parse.c related */
  struct IFFHandle *iff;	/* to be alloc'd with AllocIFF */
  UBYTE *filename;		/* current filename of this ui */
  LONG *propchks;		/* properties to get */
  LONG *collectchks;		/* properties to collect */
  LONG *stopchks;		/* stop on these (like BODY) */
  BOOL opened;			/* this iff has been opened */
  BOOL clipboard;		/* file is clipboard */
  BOOL hunt;			/* we are parsing a complex file */
  BOOL Reserved1;		/* must be zero for now */
  struct Chunk *copiedchunks;
  struct Chunk *newchunks;

  ULONG Reserved[8];
};


struct Chunk
{
  struct Chunk *ch_Next;
  long ch_Type;
  long ch_ID;
  long ch_Size;
  void *ch_Data;
};

#endif /* IFFP_IFF_H */

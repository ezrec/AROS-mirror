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

#include <exec/types.h>

#ifdef AROS
#include <dos/bptr.h>
#endif

//WORD currentdir(struct FMList*);
//void closefile(struct FMHandle*);
//LONG seek(struct FMHandle*,LONG,LONG);
//LONG readfile(struct FMHandle*);
//LONG readbufferfile(struct FMHandle*,void*,LONG);
//LONG writefile(struct FMHandle*,void*,LONG);
//struct FMHandle* openfile(struct FMList*,UBYTE*,ULONG);
//BPTR fmlock(struct FMList*,UBYTE*);
//WORD fmexamine(struct FMList*,BPTR,struct FileInfoBlock*,UBYTE*);
//WORD fmexaminefile(struct FMList*,struct FileInfoBlock*,UBYTE*);
//WORD fmexnext(struct FMList*,BPTR,struct FileInfoBlock*);
//WORD fmsetfiledate(struct FMList*,UBYTE*,struct DateStamp*);
//WORD fmfiledate(struct FMList*,struct FMNode*,ULONG);
//WORD fmsetcomment(struct FMList*,UBYTE*,UBYTE*);
//WORD fmcomment(struct FMList*,struct FMNode*,UBYTE*);
//WORD fmsetrename(struct FMList*,UBYTE*,UBYTE*);
//WORD fmrename(struct FMList*,struct FMNode*,UBYTE*);
//WORD fmsetprotection(struct FMList*,UBYTE*,ULONG);
//WORD fmprotect(struct FMList*,struct FMNode*,ULONG);
//struct FMNode* fmcreatedir(struct FMList*,UBYTE*);
//WORD examinefile(struct FMList*,struct FMNode*); //update node's attributes
//BPTR fminitdirread(struct FMList*,struct FileInfoBlock*,UBYTE*);
void *allocmem(LONG);
void freemem(void*);
//void *allocvec(struct FMList*,LONG,LONG);
void *allocvecpooled(LONG);
void freevecpooled(void*);
//struct IORequest* opendevice(struct FMList*,UBYTE*,WORD,ULONG,WORD);
void closedevice(struct IORequest*);
WORD checkdoserr(void);
void dirmeters(void);
//void updadirmeter(struct FMList*);

#define OFNORMAL 0x01
#define OFBUFFER 0x80000000
#define OFDECRUNCH 0x04
#define OFWRITE 0x10
#define OFFAKE 0x20
#define OFRELAXED 0x40

struct FMHandle {
	BPTR	handle;
	UBYTE	*filename;
	UBYTE	*path;	
	LONG	bufsize;
	LONG	decbufsize;
	UBYTE	*buffer;
	UBYTE	*decbuffer;
	LONG	position;
	LONG	size;
	WORD	flags;
	struct	FMList *owner;
	LONG	date;
};


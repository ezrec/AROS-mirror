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

/*
void * AsmCreatePool(register ULONG __asm("d0"),register ULONG __asm("d1"),register ULONG __asm("d2"),register struct ExecBase * __asm("a6"));
void AsmDeletePool(register void * __asm("a0"),register struct ExecBase * __asm("a6"));
void * AsmAllocPooled(register void * __asm("a0"),register ULONG __asm("d0"),register struct ExecBase * __asm("a6"));
void AsmFreePooled(register void * __asm("a0"),register void * __asm("a1"),register ULONG __asm("d0"),register struct ExecBase * __asm("a6"));

#define	CreatePool(x,y,z)	AsmCreatePool(x,y,z,(struct ExecBase *)SysBase)
#define	DeletePool(x)		AsmDeletePool(x,(struct ExecBase *)SysBase)
#define	AllocPooled(x,y)	AsmAllocPooled(x,y,(struct ExecBase *)SysBase)
#define	FreePooled(x,y,z)	AsmFreePooled(x,y,z,(struct ExecBase *)SysBase)

#define	LibCreatePool(x,y,z)	AsmCreatePool(x,y,z,(struct ExecBase *)SysBase)
#define	LibDeletePool(x)		AsmDeletePool(x,(struct ExecBase *)SysBase)
#define	LibAllocPooled(x,y)	AsmAllocPooled(x,y,(struct ExecBase *)SysBase)
#define	LibFreePooled(x,y,z)	AsmFreePooled(x,y,z,(struct ExecBase *)SysBase)
*/

#include <proto/exec.h>

#define	LibCreatePool(x,y,z)	CreatePool(x,y,z)
#define	LibDeletePool(x)		DeletePool(x)
#define	LibAllocPooled(x,y)	AllocPooled(x,y)
#define	LibFreePooled(x,y,z)	FreePooled(x,y,z)


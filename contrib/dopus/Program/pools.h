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
#warning Where is the code for this ?

#if 0
void * __asm AsmCreatePool(register __d0 ULONG,register __d1 ULONG,register __d2 ULONG,register __a6 struct ExecBase *);
void __asm AsmDeletePool(register __a0 void *,register __a6 struct ExecBase *);
void * __asm AsmAllocPooled(register __a0 void *,register __d0 ULONG,register __a6 struct ExecBase *);
void __asm AsmFreePooled(register __a0 void *,register __a1 void *,register __d0 ULONG,register __a6 struct ExecBase *);

#define	CreatePool(x,y,z)	AsmCreatePool(x,y,z,(struct ExecBase *)SysBase)
#define	DeletePool(x)		AsmDeletePool(x,(struct ExecBase *)SysBase)
#define	AllocPooled(x,y)	AsmAllocPooled(x,y,(struct ExecBase *)SysBase)
#define	FreePooled(x,y,z)	AsmFreePooled(x,y,z,(struct ExecBase *)SysBase)

#define	LibCreatePool(x,y,z)	AsmCreatePool(x,y,z,(struct ExecBase *)SysBase)
#define	LibDeletePool(x)	AsmDeletePool(x,(struct ExecBase *)SysBase)
#define	LibAllocPooled(x,y)	AsmAllocPooled(x,y,(struct ExecBase *)SysBase)
#define	LibFreePooled(x,y,z)	AsmFreePooled(x,y,z,(struct ExecBase *)SysBase)

#endif

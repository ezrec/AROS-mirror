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

struct ReqScrollStruct
	{
	ULONG	TopEntryNumber;		/* 0 */
	ULONG	NumEntries;		/* 4 */
	UWORD	LineSpacing;		/* 8 */
	ULONG	NumLines;		/* 10 */
	struct Gadget *PropGadget;	/* 14 */
	APTR	RedrawAll;		/* 18 */
	APTR	ReadMore;		/* 22 */
	APTR	ScrollAndDraw;		/* 26 */
	WORD	VersionNumber;		/* 30 */
	LONG	rfu1;			/* 32 */
	LONG	rfu2;			/* 36 */
	struct	GfxBase *gfxbase;	/* 40 */
	struct	FMList *fmlist;		/* 44 */
	UWORD	flags;			/* 48 */
};


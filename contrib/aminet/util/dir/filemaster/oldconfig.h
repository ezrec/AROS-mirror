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


#define OLDCONFIGVERSION 14

/* #define MAXPATH 512 */
#define MAXCOLORS3022 16
#define MAXCOLS3022 4
#define MAXROWS3022 2
/* #define MAXLISTS 10 */
#define MAXWINDOWLISTS3022 8
/* #define MAXPATHLISTS 10 */
#define MAXPATTERN3022 128
/* #define MAXMSG 256 */
/* #define BOTTOMGADGETS 4 */
#define MAXCOMMANDS3022 20
/* #define FMPENS 11 */
#define MAXCMENUS3022 27
#define CONFIGSIZE3022 64
#define CMENUCONFIGSIZE3022 8

struct OwnCommand3022 {
	UBYTE	gadgetname[16];
	UBYTE	namematch[48];
	UBYTE	matchbytes[64];
	UBYTE	command[64];
	ULONG	stack;
	UWORD	flags;
};

struct Match3022 {
	ULONG	match;
	UBYTE	patname[MAXPATTERN3022];
	UBYTE	patflags[32];
	ULONG	patsize;
	ULONG	patdate;
};

struct ScreenConfig3022 {
	UWORD	width;
	UWORD	height;
	UWORD	depth;
	ULONG	screenmode;
	UWORD	overscan;
	UWORD	autoscroll;
};

struct FMConfig3022 {
	struct	ScreenConfig3022 mainscreen;
	struct	ScreenConfig3022 textscreen;
	WORD	windowtop;
	WORD	windowleft;
	WORD	windowheight;
	WORD	windowwidth;
	WORD	sleepwindowtop;
	WORD	sleepwindowleft;
	UBYTE	pad1;	/* HACKCHECK must be zero */
	UBYTE	sliderh;
	UBYTE	pad2;	/* HACKCHECK must be nonzero */
	UBYTE	sliderw;
	UBYTE	pad3;
	UBYTE	spaceh;
	UBYTE	pad4;
	UBYTE	spacew;
	UBYTE	whitepen;
	UBYTE	blackpen;
	UBYTE	dirpen;
	UBYTE	filepen;
	UBYTE	txtpen;
	UBYTE	hilipen;
	UBYTE	backpen;
	UBYTE	backfillpen;
	UBYTE	mainbackfillpen;
	UBYTE	sourcepen;
	UBYTE	destpen;
	UBYTE	sliderpen;
	UBYTE	stringpen;
	BYTE	pri[3];
	UBYTE	doubleclick;
	UBYTE	rightmouse;
	UBYTE	middlemouse;
	UBYTE	screentype;	/* 0=custom,1=public custom,2=workbench,3=on public */
	UBYTE	pubscreen[MAXPUBSCREENNAME+1];
	ULONG	flags;
	UWORD	filelen;
	UWORD	commlen;
	UWORD	datelen;
	BYTE	mainpriority;
	BYTE	subpriority;
	LONG	dosbuffersize;	/*  HACKCHECK bits 31-24 = zero */

	UBYTE	pad[128];

	struct	TextAttr txtfontattr;
	UBYTE	txtfontname[80];
	struct	TextAttr listfontattr;
	UBYTE	listfontname[80];
	struct	TextAttr txtshowfontattr;
	UBYTE	txtshowfontname[80];
	struct	TextAttr reqfontattr;
	UBYTE	reqfontname[80];
	struct	TextAttr smallfontattr;
	UBYTE	smallfontname[80];

	/* letter,shortcut,scqualifier,frontpen,backpen,priority */
	WORD	listinfocmenu;
	WORD	listinfo[MAXCOLS3022+1][MAXROWS3022+2];

	UBYTE	cmenuconfig[CMENUCONFIGSIZE3022*MAXCMENUS3022];
	UBYTE	cmenuownconfig[CMENUCONFIGSIZE3022*MAXCOMMANDS3022];
	UBYTE	configdata[MAXCMENUS3022*CONFIGSIZE3022];

	UBYTE	shellwindow[64];
	UBYTE	colors[MAXCOLORS3022*3];

	struct	Match3022 match[MAXWINDOWLISTS3022];
	struct	OwnCommand3022 owncommand[MAXCOMMANDS3022];
	};

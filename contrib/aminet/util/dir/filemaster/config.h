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

#include "fmgui.h"

void commandanswer (struct GUIBase *);
#ifndef AROS
WORD allocconfig (struct CMenuConfig *, WORD);
void *getconfig (struct CMenuConfig *);
#endif

struct CMenuConfig *getconfignumber (WORD);
WORD findbuild (struct CMenuConfig *);
UBYTE *findbuildstring (struct CMenuConfig *);

#define FMCONFIGVERSION 21

#define USERCOMMANDS 60
#define BUILDCOMMANDS 31
#define TOTALCOMMANDS (USERCOMMANDS+BUILDCOMMANDS)
#define FMPENS 12

#ifndef CMENUCDEF
#define CMENUCDEF
struct CMenuConfig
{
  UBYTE cmenucount;
  UBYTE position;		// 0=invisible,1=gadget bar,2=corner,4=menubar
  UBYTE frontpen;
  UBYTE backpen;
  UBYTE priority;
  UBYTE shortcut[24];
  UBYTE label[10];
  WORD moreconfiglen;
  void *moreconfig;
};

struct ExecuteFlags
{
  unsigned shellwindow:1;
  unsigned cd:2;
  unsigned rescan:2;
  unsigned startfront:2;
  unsigned endfront:2;
  unsigned iconify:1;
  BYTE timeout;
  ULONG stack;
};

struct OwnCommand
{
  UBYTE namematch[128];
  UBYTE matchbytes[128];
  UBYTE command[128];
  struct ExecuteFlags ef;
};

struct Match
{
  UBYTE patname[PATTERN];
  UBYTE patflags[32];
  UBYTE patcomm[80];
  ULONG patsize;
  ULONG patsize2;
  ULONG patdate;
  ULONG patdate2;
  unsigned mcase:2;
  unsigned mfiles:2;
  unsigned mdirs:2;
  unsigned msize:3;
  unsigned mdate:3;
  unsigned mflags:1;
  unsigned mcomm:1;
  unsigned msortby:3;
  unsigned msortud:1;
  unsigned mmix:1;
  unsigned mdrawersfirst:1;
};

struct DirListLayout
{
  UBYTE width;
  unsigned type:4;
  unsigned rightaligned:1;
  unsigned rightlock:1;
};

struct ScreenConfig
{
  UWORD width;
  UWORD height;
  ULONG screenmode;
  unsigned depth:4;
  unsigned overscan:3;
  unsigned autoscroll:1;
};

struct FMConfig
{
  struct ScreenConfig mainscreen;
  struct ScreenConfig textscreen;
  WORD windowtop;
  WORD windowleft;
  WORD windowheight;
  WORD windowwidth;
  WORD sleepwindowtop;
  WORD sleepwindowleft;
  UBYTE sliderh;
  UBYTE sliderw;
  UBYTE spaceh;
  UBYTE spacew;
  UBYTE whitepen;
  UBYTE blackpen;
  UBYTE devicepen;
  UBYTE dirpen;
  UBYTE filepen;
  UBYTE txtpen;
  UBYTE hilipen;
  UBYTE backpen;
  UBYTE backfillpen;
  UBYTE mainbackfillpen;
  UBYTE sourcepen;
  UBYTE destpen;
  UBYTE sliderpen;
  UBYTE stringpen;
  BYTE pri[3];
  UBYTE doubleclick;
  UBYTE rightmouse;
  UBYTE middlemouse;
  UBYTE screentype;		//0=custom,1=public custom,2=workbench,3=on public
  ULONG flags;
  BYTE mainpriority;
  BYTE subpriority;
  LONG dosbuffersize;		// HACKCHECK bits 31-24 = zero

  struct DirListLayout dl[5];

  struct TextAttr txtfontattr;
  struct TextAttr listfontattr;
  struct TextAttr txtshowfontattr;
  struct TextAttr reqfontattr;
  struct TextAttr smallfontattr;

  WORD listinfo[COLS + 1][ROWS + 2];
  UBYTE cmenutype;
  UBYTE cmenucolumns;		//rows and columns!
  UBYTE cmenuposition;

  UBYTE colors[COLORS * 3];

  UBYTE txtfontname[80];
  UBYTE listfontname[80];
  UBYTE txtshowfontname[80];
  UBYTE reqfontname[80];
  UBYTE smallfontname[80];
  UBYTE pubscreen[MAXPUBSCREENNAME + 1];
  UBYTE hotkey[16];

  UBYTE usepicturedt:1;
  UBYTE usesampledt:1;
  UBYTE useexecutedt:1;
  UBYTE useicondt:1;
  UBYTE usefontdt:1;
  UBYTE useascdt:1;
  UBYTE usemoddt:1;
  UBYTE usehexdt:1;

  struct CMenuConfig cmenuconfig[TOTALCOMMANDS];

};
#endif

#define QUITCONFIG	 1
#define SLEEPCONFIG	 2
#define STODCONFIG	 3
#define CONFIGCONFIG	 4
#define EXCLUDECONFIG	 5
#define INCLUDECONFIG	 6
#define PARSECONFIG	 7

#define	DRIVESCONFIG	 8
#define	PARENTCONFIG	 9
#define	INVERTCONFIG	10
#define	CLEARCONFIG	11
#define	OPERATECONFIG	12
#define	COPYCONFIG	13
#define	COPYASCONFIG	14
#define	MOVECONFIG	15
#define	DELETECONFIG	16
#define	MAKEDIRCONFIG	17
#define	SHOWASCCONFIG	18
#define	SHOWHEXCONFIG	19
#define	SHOWPICCONFIG	20
#define	PLAYMODCONFIG	21
#define	DISKINFOCONFIG	22
#define	EXECUTECONFIG	23
#define	DISKSIZECONFIG	24
#define	MODINFOCONFIG	25
#define	HEARCONFIG	26
#define	FILEEDITCONFIG	27
#define DISKEDITCONFIG	28
#define	ABOUTCONFIG	29
#define	REGISTERCONFIG	30
#define EMPTYCONFIG	31

#define MSUBPROC	0x01
#define MDOUBLED	0x02
#define MWINDOWED	0x04
#define MHSCROLL	0x08
#define MVSCROLL	0x10
#define MDOSREQ		0x20
#define MLOCALE		0x40

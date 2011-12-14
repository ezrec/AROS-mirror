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
#define FMCONFIGVERSION 21
#define USERCOMMANDS 60
#define BUILDCOMMANDS 31
#define TOTALCOMMANDS (USERCOMMANDS+BUILDCOMMANDS)
#define FMPENS 12
#ifndef CMENUCDEF
#define CMENUCDEF
#endif
#define NDEVICE         0x4000
#define NASSIGN         0x2000
#define NDIRECTORY      0x1000
#define NFILE           0x0800
#define NKEYBOARD       0x0400
#define NMATCHED        0x0200
#define NBUFFER         0x0100
#define NSELECTED       0x0001
#define OFNORMAL 0x01
#define OFBUFFER 0x80000000
#define OFDECRUNCH 0x04
#define OFWRITE 0x10
#define OFFAKE 0x20
#define OFRELAXED 0x40
#define LALLOCATED 0x8000
#define LSLIDERALLOC 0x4000
#define LACTIVE 0x2000
#define LABORTREQ 0x1000
#define LSUBPROC 0x800
#define LDESTINATION 0x0004
#define LSOURCE 0x0008
#define LDIRLIST 0x0010
#define LDEVLIST 0x0020
#define LUPDATEMSG 0x0040
#define NDLEFT(n)       (((UBYTE*)n)+fmmain.leftoffset)
#define NDLENGTH(n)     (((UBYTE*)n)+fmmain.lenoffset)
#define NDFILE(n)       (((UBYTE*)n)+fmmain.fileoffset)
#define NDPROT(n)       (((UBYTE*)n)+fmmain.protoffset)
#define NDDATE(n)       (((UBYTE*)n)+fmmain.dateoffset)
#define NDCOMM(n)       (((UBYTE*)n)+fmmain.commoffset)
#define FMPORTNAME "fm_port" 
#define PATH 1024
#define COLORS 16
#define COLS 4
#define ROWS 2
#define LISTS 10
#define WINDOWLISTS 8
#define PATHLISTS 10
#define PATTERN 128
#define MSG 256

#include <libraries/locale.h>
#include <proto/locale.h>
#include <proto/exec.h>
#include <proto/alib.h>
#include <proto/icon.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <devices/inputevent.h> 
#include <graphics/gfxbase.h>  
#include <aros/bigendianio.h> 	
#include <intuition/intuition.h>
#include <libraries/commodities.h>
#include <intuition/sghooks.h>
#include <graphics/gfx.h>
#include <graphics/gfxmacros.h>
#include <utility/tagitem.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <aros/debug.h>
#include <stdio.h>
#include <clib/boopsistubs.h>
#include <exec/types.h>
#include <dos/bptr.h>
#include <proto/bgui.h> 
#include <proto/aros.h> 
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <proto/graphics.h> 
#include <proto/intuition.h>
#include <proto/locale.h> 
#include <proto/reqtools.h> 
#include <proto/gadtools.h>
#include <proto/utility.h>
#include <proto/commodities.h>
#include <proto/workbench.h>
#include <proto/console.h>
#include <proto/asl.h>
#include <proto/timer.h>
#include <proto/diskfont.h>
#include <proto/keymap.h>
#include "reqscroll.h"
#include "fmgui.h"

struct CMenuConfig {
        UBYTE   cmenucount;
        UBYTE   position; /*  0=invisible,1=gadget bar,2=corner,4=menubar */
        UBYTE   frontpen;
        UBYTE   backpen;
        UBYTE   priority;
        UBYTE   shortcut[24];
        UBYTE   label[10];
        WORD    moreconfiglen;
        void    *moreconfig;
};

struct ExecuteFlags {
        unsigned shellwindow:1;
        unsigned cd:2;
        unsigned rescan:2;
        unsigned startfront:2;
        unsigned endfront:2;
        unsigned iconify:1;
        BYTE timeout;
        ULONG stack;
};

struct OwnCommand {
        UBYTE   namematch[128];
        UBYTE   matchbytes[128];
        UBYTE   command[128];
        struct  ExecuteFlags ef;
};

struct Match {
        UBYTE   patname[PATTERN];
        UBYTE   patflags[32];
        UBYTE   patcomm[80];
        ULONG   patsize;
        ULONG   patsize2;
        ULONG   patdate;
        ULONG   patdate2;
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

struct DirListLayout {
        UBYTE width;
        unsigned type:4;
        unsigned rightaligned:1;
        unsigned rightlock:1;
};
struct ScreenConfig {
	UWORD	width;
	UWORD	height;
	ULONG	screenmode;
	unsigned depth:4;
	unsigned overscan:3;
	unsigned autoscroll:1;
};

struct FMConfig {
	struct	ScreenConfig mainscreen;
	struct	ScreenConfig textscreen;
	WORD	windowtop;
	WORD	windowleft;
	WORD	windowheight;
	WORD	windowwidth;
	WORD	sleepwindowtop;
	WORD	sleepwindowleft;
	UBYTE	sliderh;
	UBYTE	sliderw;
	UBYTE	spaceh;
	UBYTE	spacew;
	UBYTE	whitepen;
	UBYTE	blackpen;
	UBYTE	devicepen;
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
	ULONG	flags;
	BYTE	mainpriority;
	BYTE	subpriority;
	LONG	dosbuffersize;	/*  HACKCHECK bits 31-24 = zero */

	struct	DirListLayout dl[5];

	struct	TextAttr txtfontattr;
	struct	TextAttr listfontattr;
	struct	TextAttr txtshowfontattr;
	struct	TextAttr reqfontattr;
	struct	TextAttr smallfontattr;

	WORD	listinfo[COLS+1][ROWS+2];
	UBYTE	cmenutype;
	UBYTE	cmenucolumns;	/* rows and columns! */
	UBYTE	cmenuposition;

	UBYTE	colors[COLORS*3];

	UBYTE	txtfontname[80];
	UBYTE	listfontname[80];
	UBYTE	txtshowfontname[80];
	UBYTE	reqfontname[80];
	UBYTE	smallfontname[80];
	UBYTE	pubscreen[MAXPUBSCREENNAME+1];
	UBYTE	hotkey[16];

	UBYTE	usepicturedt:1;
	UBYTE	usesampledt:1;
	UBYTE	useexecutedt:1;
	UBYTE	useicondt:1;
	UBYTE	usefontdt:1;
	UBYTE	useascdt:1;
	UBYTE	usemoddt:1;
	UBYTE	usehexdt:1;

	struct	CMenuConfig cmenuconfig[TOTALCOMMANDS];

	};

struct FMHandle {
        BPTR    handle;
        UBYTE   *filename;
        UBYTE   *path;
        LONG    bufsize;
        LONG    decbufsize;
        UBYTE   *buffer;
        UBYTE   *decbuffer;
        LONG    position;
        LONG    size;
        WORD    flags;
        struct  FMList *owner;
        LONG    date;
};

struct ProcMsg {
        struct CMenuConfig *cmc;
};
     
struct FMPort {
        struct MsgPort msgport;
        struct FMMain *fmmain;
        struct FMConfig *fmconfig;
};

struct FMNode {
        struct  FMNode *succ;
        struct  FMNode *pred;
        UWORD   nodelen;
        UWORD   flags;
        ULONG   numlen;
        ULONG   numprot;
        ULONG   numdate;
};

struct FMList {
        struct  FMNode *head;
        struct  FMNode *tail;
        struct  FMNode *tailpred;
        struct  ListInfo *li;
        UWORD   keyline;
        UWORD   keylinew;
        UWORD   totlines;
        UWORD   firstline;
        UWORD   totlinesw;
        UWORD   firstlinew;
        UWORD   flags;
        UWORD   listnumber;
        struct  FMNode *lastclicked;
        struct  FMList *pair;
        UBYTE   fmmessage1[MSG];
        UBYTE   fmmessage2[MSG];
        UBYTE   path[PATH];
        UBYTE   workname[32];
        BPTR    oldcur;
};

struct GadKeyTab {
	struct Gadget *g;
	struct CMenuConfig *cmc;
	UBYTE key[2];
	unsigned listnum:4;
	unsigned onelist:1;
	unsigned unused:1;
};

struct FMPathlist {
	struct	FMNode *head;
	struct	FMNode *tail;
	struct	FMNode *tailpred;
	LONG	cnt;
	UWORD	firstlinew;
	UWORD	firstline;
	UBYTE	path[PATH];
};

#define LISTGADGETS 3

struct ListInfo {
	struct	FMList *list;
	UWORD	width;
	UWORD	height;
	UWORD	x;
	UWORD	y;
	UWORD	ex;
	UWORD	ey;
	UWORD	dirwidth;
	UWORD	dirheight;
	UWORD	dirx;	
	UWORD	diry;
	UWORD	edirx;
	UWORD	ediry;
	UWORD	visiblelines;
	UWORD	visiblelinesw;
	UWORD	topliney;
	struct	Gadget slider1;
	struct	PropInfo pinfo1;
	struct	Image im1;
	struct	Image sim1;
	struct	Gadget slider2;
	struct	PropInfo pinfo2;
	struct	Image im2;
	struct	Image sim2;
	struct	Gadget string;
	struct	StringInfo sinfo;
	struct	StringExtend stext;
	struct	ReqScrollStruct rscroll1;
	struct	ReqScrollStruct rscroll2;
	struct	Gadget taskgadget;
	struct	Gadget ltaskgadget;
	struct	Gadget rtaskgadget;
	struct	Border bord1;
	struct	Border bord2;
	struct	Border bord3;
	struct	Border bord4;
	UWORD	linumber;
	WORD	xy1[2*5];
	WORD	xy2[2*5];
/* 	WORD	lihack;  lihack==number */
};

struct FMMain {
	struct	Screen *naytto;
	struct	Window *ikkuna;
	struct	FMList *sourcedir;
	struct	FMList *destdir;
	WORD	txtysize;
	WORD	txtxsize;
	WORD	txtbaseline;
	WORD	listysize;
	WORD	listxsize;
	WORD	listbaseline;
	WORD	reqysize;
	WORD	reqxsize;
	WORD	reqbaseline;
	WORD	txtshowysize;
	WORD	txtshowxsize;
	WORD	txtshowbaseline;
	WORD	messageliney1;
	WORD	messageliney2;
	WORD	bottomliney;
	WORD	bottomlinewidth;
	struct	ListInfo *li[WINDOWLISTS];
	WORD	uselist[WINDOWLISTS];

	struct	RastPort *rp;
	struct	TextFont *txtfont;
	struct	TextFont *listfont;
	struct	TextFont *txtshowfont;
	struct	TextFont *reqfont;
	struct	TextFont *normalfont;
	struct	TextFont *smallfont;
	struct	TextAttr tattr;
	struct	Process *myproc;
	struct	Process *timeproc;
	APTR	gtvisual;
	APTR	oldwinptr;
	struct	Gadget *firstgadget;
	UBYTE	undobuf[PATH];

	WORD	gadgetxy[TOTALCOMMANDS*4];
	WORD	cmenux,cmenuy;
	WORD	totalcmenuwidth,totalcmenuheight;
	WORD	cmenuwidth;		/* min gadget width */
	WORD	realcmenuspace;		/* width/height of cmenu space */
	WORD	cmenugadnumperline;	/* number of gadgets per line */
	WORD	cmenu;			/* first gadget */
	UBYTE	*cmenuptr;		/* ptr to texts */

	UWORD	leftoffset;
	UWORD	lenoffset;
	UWORD	fileoffset;
	UWORD	protoffset;
	UWORD	dateoffset;
	UWORD	commoffset;
	UWORD	maxdirlistwidth;
	UWORD	totnodelen;
	UWORD	filelen;
	UWORD	commlen;
	UWORD	datelen;
	UWORD	lenlen;
	UWORD	protlen;
	UWORD	memmeterlen;
	UBYTE	version;
	UBYTE	revision;
	UBYTE	betaversion;
	UBYTE	betarevision;

	/*  global clock-task variables */

	volatile signed clock:2;
	volatile unsigned kill:1;
	volatile unsigned sleepon:3;
	volatile unsigned fmactive:2;
	volatile UBYTE passhotkey;
	volatile LONG timer;

	UWORD	modflag;
	APTR	modptr;
	UBYTE	*starts[33];
	struct	IOAudio *audio;

	ULONG	secs;
	ULONG	mics;
	UBYTE	kick;
	UBYTE	quitti;
	UBYTE	wincnt;
	WORD	newconfig;
	BPTR	oldcur;

	struct	FMPathlist *pathlist;
	WORD	numpathlist;
	struct	FMList dlist;
	struct	SignalSemaphore gfxsema;
	struct	SignalSemaphore msgsema;
	struct	SignalSemaphore poolsema;
	APTR	pool;
	struct	MsgPort *appiconport;
	struct	AppIcon *appicon;
    	struct DiskObject * dobj;  
	struct	MsgPort	*appwinport;
	struct	AppWindow *appwin;
	struct	MsgPort *cxport;
	CxObj	*broker;
	struct	FMNode *framenode;
	struct	FMList *framelist;
	struct	GadKeyTab gadkeytab[TOTALCOMMANDS+4*WINDOWLISTS];
	struct	InputXpression *ix[TOTALCOMMANDS+4*WINDOWLISTS];
	struct	FMPort *fmport;
	struct	ProcMsg *pmsg;

	UBYTE	fmtitlename[128];
	UBYTE	fmtitle[32];
	UBYTE	fmdate[32];
	UBYTE	regname[32];
	UWORD	shareware;
};


void ongadget(struct Gadget*,WORD);
void offgadget(struct Gadget*,WORD);
void runcommand(struct GadKeyTab*);
WORD isdatatypes(struct FMHandle*,LONG);
WORD checkmatch(UBYTE*,UBYTE*,UBYTE*,struct FMNode*,struct Match*,WORD);
WORD smallevent(struct Window*,ULONG*,UWORD*);
void clearlist(struct FMList*);
struct FMNode* dupnode(struct FMNode*);
WORD duplist(struct FMList*,struct FMList*);
void siirra1(UBYTE*,UBYTE*,WORD);
struct FMNode* dupnode(struct FMNode*);
struct FMNode* allocnode(void);
void removenode(struct FMList*,struct FMNode*);
UBYTE* siirra2(UBYTE*,UBYTE*);
struct FMNode* findselnode(struct FMList*);
struct FMNode* findselnodeall(struct FMList*);
WORD sselected(struct FMList*,WORD);
struct FMNode* fmcreatedir(struct FMList*,UBYTE*);
struct FMNode* disknode(struct FMList*,UBYTE*);
void sformat(UBYTE*,UBYTE*,...);
void sformatti(UBYTE*,UBYTE*,va_list args);
void sformatmsg(UBYTE*,LONG,...);
struct FMNode* scrollifneeded(struct FMList*,UWORD*,UWORD*);
void parselist(struct FMList*,WORD);
void outputlistline(struct FMList*,struct FMNode*);
void fmmessage(struct FMList*);
void outputlist(struct FMList*);
void endofdirtxt(struct FMList*,struct FMNode*);
WORD dosrequest(struct FMList*,WORD,UBYTE*,...);
WORD dosrequestmsg(struct FMList*,WORD,SIPTR,...);
WORD request(UBYTE*,WORD,WORD,UBYTE*,...);
WORD requestmsg(UBYTE*,WORD,WORD,LONG,...);
WORD askstring(struct Screen*,UBYTE*,UBYTE*,UBYTE*,WORD);
WORD asklong(struct Screen*,UBYTE*,UBYTE*,LONG*,LONG,LONG);
void strcpymsg(UBYTE*,WORD);
ULONG dstolong(struct DateStamp*);
void longtods(struct DateStamp*,ULONG);
void longtodatestring(UBYTE*,ULONG);
ULONG datestringtolong(UBYTE*);
void csortlist(struct FMList*);
void fmoutput(struct FMList*);
void updatelist(struct FMList*);
void updadirmeter(struct FMList*);
void dirmeters(void);
void siirran(UBYTE*,UBYTE*,WORD);
void fittext(struct RastPort*,UBYTE*,WORD,WORD,WORD,WORD,WORD);
UBYTE* siirrabstr(BSTR,UBYTE*);
void textextent(struct RastPort*,UBYTE*,WORD*,WORD*);
WORD strtohex(UBYTE*);
void sethockeys(void);
ULONG waitsig(ULONG);
void copyus(UBYTE*,UBYTE*);
UBYTE scanus(UBYTE*);
void memseti(void*,UBYTE,LONG);


WORD currentdir(struct FMList*);
void closefile(struct FMHandle*);
LONG seek(struct FMHandle*,LONG,LONG);
LONG readfile(struct FMHandle*);
LONG readbufferfile(struct FMHandle*,void*,LONG);
LONG writefile(struct FMHandle*,void*,LONG);
struct FMHandle* openfile(struct FMList*,UBYTE*,ULONG);
BPTR fmlock(struct FMList*,UBYTE*);
WORD fmexamine(struct FMList*,BPTR,struct FileInfoBlock*,UBYTE*);
WORD fmexaminefile(struct FMList*,struct FileInfoBlock*,UBYTE*);
WORD fmexnext(struct FMList*,BPTR,struct FileInfoBlock*);
WORD fmsetfiledate(struct FMList*,UBYTE*,struct DateStamp*);
WORD fmfiledate(struct FMList*,struct FMNode*,ULONG);
WORD fmsetcomment(struct FMList*,UBYTE*,UBYTE*);
WORD fmcomment(struct FMList*,struct FMNode*,UBYTE*);
WORD fmsetrename(struct FMList*,UBYTE*,UBYTE*);
WORD fmrename(struct FMList*,struct FMNode*,UBYTE*);
WORD fmsetprotection(struct FMList*,UBYTE*,ULONG);
WORD fmprotect(struct FMList*,struct FMNode*,ULONG);
struct FMNode* fmcreatedir(struct FMList*,UBYTE*);
WORD examinefile(struct FMList*,struct FMNode*);
BPTR fminitdirread(struct FMList*,struct FileInfoBlock*,UBYTE*);
void *allocmem(LONG);
void freemem(void*);
void *allocvec(struct FMList*,LONG,LONG);
void *allocvecpooled(LONG);
void freevecpooled(void*);
struct IORequest* opendevice(struct FMList*,UBYTE*,WORD,ULONG,WORD);
void closedevice(struct IORequest*);
WORD checkdoserr(void);
void dirmeters(void);
void updadirmeter(struct FMList*);
void initproc(struct FMList*,UBYTE*);
void endproc(struct FMList*);
WORD setalloc(struct FMList*,WORD);
struct ProcMsg *sinitproc(void);
void deinitproc(struct ProcMsg*);
WORD tstsubproc(WORD);
WORD testabort(struct FMList*);
WORD askabort(struct FMList*);
void changename(struct FMList*,UBYTE*);
void priority(struct CMenuConfig*);
void commandanswer(struct GUIBase*);
WORD allocconfig(struct CMenuConfig*,WORD);
void *getconfig(struct CMenuConfig*);
struct CMenuConfig *getconfignumber(WORD);
WORD findbuild(struct CMenuConfig*);
UBYTE *findbuildstring(struct CMenuConfig*);



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

#include <defines/bgui.h>
#include <defines/aros.h>
#include <defines/exec.h>
#include <defines/dos.h>
#include <defines/icon.h>
#include <defines/graphics.h>
#include <defines/intuition.h>
#include <defines/locale.h>
#include <defines/reqtools.h>
#include <defines/gadtools.h>
#include <defines/utility.h>
#include <defines/commodities.h>
#include <defines/workbench.h>
#include <defines/console.h>
#include <defines/asl.h>
#include <defines/timer.h>
#include <defines/diskfont.h>
#include <defines/keymap.h>


void ongadget (struct Gadget *, WORD);
void offgadget (struct Gadget *, WORD);
//void runcommand(struct GadKeyTab*);
//WORD isdatatypes(struct FMHandle*,LONG);
//WORD checkmatch(UBYTE*,UBYTE*,UBYTE*,struct FMNode*,struct Match*,WORD);
WORD smallevent (struct Window *, ULONG *, UWORD *);
//void clearlist(struct FMList*);
struct FMNode *dupnode (struct FMNode *);
//WORD duplist(struct FMList*,struct FMList*);
void siirra1 (UBYTE *, UBYTE *, WORD);
struct FMNode *dupnode (struct FMNode *);
struct FMNode *allocnode (void);
//void removenode(struct FMList*,struct FMNode*);
UBYTE *siirra2 (UBYTE *, UBYTE *);
//struct FMNode* findselnode(struct FMList*);
//struct FMNode* findselnodeall(struct FMList*);
//WORD sselected(struct FMList*,WORD);

struct FMNode *fmcreatedir (struct FMList *, UBYTE *);

//struct FMNode* disknode(struct FMList*,UBYTE*);
void sformat (UBYTE *, UBYTE *, ...);
void sformatti (UBYTE *, UBYTE *, UBYTE *);
void sformatmsg (UBYTE *, LONG, ...);
//struct FMNode* scrollifneeded(struct FMList*,UWORD*,UWORD*);
//void parselist(struct FMList*,WORD);
//void outputlistline(struct FMList*,struct FMNode*);
//void fmmessage(struct FMList*);
//void outputlist(struct FMList*);
void endofdirtxt (struct FMList *, struct FMNode *);
//WORD dosrequest(struct FMList*,WORD,UBYTE*,...);
//WORD dosrequestmsg(struct FMList*,WORD,LONG,...);
WORD request (UBYTE *, WORD, WORD, UBYTE *, ...);
WORD requestmsg (UBYTE *, WORD, WORD, LONG, ...);
WORD askstring (struct Screen *, UBYTE *, UBYTE *, UBYTE *, WORD);
WORD asklong (struct Screen *, UBYTE *, UBYTE *, LONG *, LONG, LONG);
void strcpymsg (UBYTE *, WORD);
//ULONG dstolong(struct DateStamp*);
//void longtods(struct DateStamp*,ULONG);
void longtodatestring (UBYTE *, ULONG);
ULONG datestringtolong (UBYTE *);
//void csortlist(struct FMList*);
//void fmoutput(struct FMList*);
//void updatelist(struct FMList*);

#ifndef AROS
void updadirmeter (struct FMList *);
#endif

void dirmeters (void);
void siirran (UBYTE *, UBYTE *, WORD);
void fittext (struct RastPort *, UBYTE *, WORD, WORD, WORD, WORD, WORD);
UBYTE *siirrabstr (UBYTE *, UBYTE *);
void textextent (struct RastPort *, UBYTE *, WORD *, WORD *);
WORD strtohex (UBYTE *);
void sethockeys (void);
ULONG waitsig (ULONG);

void copyus (UBYTE *, UBYTE *);
UBYTE scanus (UBYTE *);
void memseti (void *, UBYTE, LONG);

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

#include "config.h"
#include "reqscroll.h"

struct GadKeyTab
{
  struct Gadget *g;
  struct CMenuConfig *cmc;
  UBYTE key[2];
  unsigned listnum:4;
  unsigned onelist:1;
  unsigned unused:1;
};

struct FMList
{
  struct FMNode *head;
  struct FMNode *tail;
  struct FMNode *tailpred;
  struct ListInfo *li;
  UWORD keyline;
  UWORD keylinew;
  UWORD totlines;
  UWORD firstline;
  UWORD totlinesw;
  UWORD firstlinew;
  UWORD flags;
  UWORD listnumber;
  struct FMNode *lastclicked;
  struct FMList *pair;
  UBYTE fmmessage1[MSG];
  UBYTE fmmessage2[MSG];
  UBYTE path[PATH];
  UBYTE workname[32];
  BPTR oldcur;
};

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

#define NDLEFT(n)	(((UBYTE*)n)+fmmain.leftoffset)
#define NDLENGTH(n)	(((UBYTE*)n)+fmmain.lenoffset)
#define NDFILE(n)	(((UBYTE*)n)+fmmain.fileoffset)
#define NDPROT(n)	(((UBYTE*)n)+fmmain.protoffset)
#define NDDATE(n)	(((UBYTE*)n)+fmmain.dateoffset)
#define NDCOMM(n)	(((UBYTE*)n)+fmmain.commoffset)

struct FMNode
{
  struct FMNode *succ;
  struct FMNode *pred;
  UWORD nodelen;
  UWORD flags;
  ULONG numlen;
  ULONG numprot;
  ULONG numdate;
};

#define NDEVICE		0x4000
#define NASSIGN		0x2000
#define NDIRECTORY	0x1000
#define NFILE		0x0800
#define NKEYBOARD	0x0400
#define NMATCHED	0x0200
#define NBUFFER		0x0100
#define NSELECTED	0x0001

struct FMPathlist
{
  struct FMNode *head;
  struct FMNode *tail;
  struct FMNode *tailpred;
  LONG cnt;
  UWORD firstlinew;
  UWORD firstline;
  UBYTE path[PATH];
};

#define LISTGADGETS 3

struct ListInfo
{
  struct FMList *list;
  UWORD width;
  UWORD height;
  UWORD x;
  UWORD y;
  UWORD ex;
  UWORD ey;
  UWORD dirwidth;
  UWORD dirheight;
  UWORD dirx;
  UWORD diry;
  UWORD edirx;
  UWORD ediry;
  UWORD visiblelines;
  UWORD visiblelinesw;
  UWORD topliney;
  struct Gadget slider1;
  struct PropInfo pinfo1;
  struct Image im1;
  struct Image sim1;
  struct Gadget slider2;
  struct PropInfo pinfo2;
  struct Image im2;
  struct Image sim2;
  struct Gadget string;
  struct StringInfo sinfo;
  struct StringExtend stext;
  struct ReqScrollStruct rscroll1;
  struct ReqScrollStruct rscroll2;
  struct Gadget taskgadget;
  struct Gadget ltaskgadget;
  struct Gadget rtaskgadget;
  struct Border bord1;
  struct Border bord2;
  struct Border bord3;
  struct Border bord4;
  UWORD linumber;
  WORD xy1[2 * 5];
  WORD xy2[2 * 5];
//      WORD    lihack; // lihack==number
};

struct FMMain
{
  struct Screen *naytto;
  struct Window *ikkuna;
  struct FMList *sourcedir;
  struct FMList *destdir;
  WORD txtysize;
  WORD txtxsize;
  WORD txtbaseline;
  WORD listysize;
  WORD listxsize;
  WORD listbaseline;
  WORD reqysize;
  WORD reqxsize;
  WORD reqbaseline;
  WORD txtshowysize;
  WORD txtshowxsize;
  WORD txtshowbaseline;
  WORD messageliney1;
  WORD messageliney2;
  WORD bottomliney;
  WORD bottomlinewidth;
  struct ListInfo *li[WINDOWLISTS];
  WORD uselist[WINDOWLISTS];

  struct RastPort *rp;
  struct TextFont *txtfont;
  struct TextFont *listfont;
  struct TextFont *txtshowfont;
  struct TextFont *reqfont;
  struct TextFont *normalfont;
  struct TextFont *smallfont;
  struct TextAttr tattr;
  struct Process *myproc;
  struct Process *timeproc;
  APTR gtvisual;
  APTR oldwinptr;
  struct Gadget *firstgadget;
  UBYTE undobuf[PATH];

  WORD gadgetxy[TOTALCOMMANDS * 4];
  WORD cmenux, cmenuy;
  WORD totalcmenuwidth, totalcmenuheight;
  WORD cmenuwidth;		//min gadget width
  WORD realcmenuspace;		//width/height of cmenu space
  WORD cmenugadnumperline;	//number of gadgets per line
  WORD cmenu;			//first gadget
  UBYTE *cmenuptr;		//ptr to texts

  UWORD leftoffset;
  UWORD lenoffset;
  UWORD fileoffset;
  UWORD protoffset;
  UWORD dateoffset;
  UWORD commoffset;
  UWORD maxdirlistwidth;
  UWORD totnodelen;
  UWORD filelen;
  UWORD commlen;
  UWORD datelen;
  UWORD lenlen;
  UWORD protlen;
  UWORD memmeterlen;
  UBYTE version;
  UBYTE revision;
  UBYTE betaversion;
  UBYTE betarevision;

  // global clock-task variables

  volatile signed clock:2;
  volatile unsigned kill:1;
  volatile unsigned sleepon:3;
  volatile unsigned fmactive:2;
  volatile UBYTE passhotkey;
  volatile LONG timer;

  UWORD modflag;
  APTR modptr;
  UBYTE *starts[33];
  struct IOAudio *audio;

  ULONG secs;
  ULONG mics;
  UBYTE kick;
  UBYTE quitti;
  UBYTE wincnt;
  WORD newconfig;
  BPTR oldcur;

  struct FMPathlist *pathlist;
  WORD numpathlist;
  struct FMList dlist;
  struct SignalSemaphore gfxsema;
  struct SignalSemaphore msgsema;
  struct SignalSemaphore poolsema;
  APTR pool;
  struct MsgPort *appiconport;
  struct AppIcon *appicon;
  struct DiskObject *dobj;
  struct MsgPort *appwinport;
  struct AppWindow *appwin;
  struct MsgPort *cxport;
  CxObj *broker;
  struct FMNode *framenode;
  struct FMList *framelist;
  struct GadKeyTab gadkeytab[TOTALCOMMANDS + 4 * WINDOWLISTS];
  struct InputXpression *ix[TOTALCOMMANDS + 4 * WINDOWLISTS];
  struct FMPort *fmport;
  struct ProcMsg *pmsg;

  UBYTE fmtitlename[128];
  UBYTE fmtitle[32];
  UBYTE fmdate[32];
  UBYTE regname[32];
  UWORD shareware;
};

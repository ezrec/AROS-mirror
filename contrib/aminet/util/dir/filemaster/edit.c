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
#include <aros/debug.h>  
#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/filehandler.h>
#include <devices/trackdisk.h>
#include <stdio.h>
#include <string.h>
#include "fmnode.h"
#include "fmlocale.h"
#include "fmgui.h"

extern struct UtilityBase *UtilityBase;
extern struct ExecBase *SysBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;

int RELVERIFY, BOOLGADGET, GADGHIMAGE, LONGINT, STRGADGET;
extern struct FMConfig *fmconfig;
extern struct TextFont *txtshowfont;
extern struct FMMain fmmain;

struct DiskEditConfig {
	unsigned lowcyl:2;
};

struct fmboolgadget {
	struct	Gadget gadget;
	struct	Border border1;
	struct	Border border2;
	struct	Border border3;
	struct	Border border4;
	WORD	xy1[2*5];
	WORD	xy2[2*5];
};
struct	trackgadget {
	struct	Gadget gadget;
	struct	StringInfo si;
	struct	StringExtend se;
	UBYTE	buffer[16];
};

struct Edit {
	WORD	type;
	struct	FMList *list;
	struct	FMNode *node;
	struct	FMHandle *handle;
	WORD	hexwidth;
	WORD	ascwidth;
	WORD	editheight;
	WORD	infoliney;
	WORD	columns;
	WORD	rows;
	WORD	xoffsethex;
	WORD	xoffsetasc;
	WORD	yoffset;
	WORD	screenbytes;
	LONG	offset;
	UBYTE	*buffer;
	UBYTE	*usebuffer;
	LONG	usebufsize;
	LONG	bufsize;
	WORD	cursorx;
	WORD	cursory;
	WORD 	cursoroffset;
	WORD	mode;
	UBYTE	searchstring[64];
	UBYTE	changed;
	UBYTE	pad1;
	LONG	oldsearch;

	WORD	xoffsetbtsh;
	WORD	xwidthbtsh;
	struct	DosEnvec *dosenv;
	struct	IOStdReq *ioreq;
	WORD	screenoffset;
	LONG	blocklen;
	LONG	blocks;
	LONG	tracks;
	LONG	sectors;
	LONG	heads;
	LONG	lowoffset;
	LONG	maxoffset;
	LONG	rootoffset;
	UBYTE	devname[32];
	struct	CMenuConfig *cmc;
	struct	DiskEditConfig *dec;

	struct	trackgadget blockgad;
	struct	trackgadget trackgad;
	struct	trackgadget secgad;
	struct	trackgadget headgad;

	struct	fmboolgadget exitgadget;
	struct	fmboolgadget writegadget;
	struct	fmboolgadget undogadget;
	struct	fmboolgadget searchgadget;
	WORD	gads;
	UBYTE	keyshorts[4];
	UBYTE	btsh[4];
	WORD	btshxw[2*4];
};

void editor(struct Edit*);
WORD editwindownewsize(struct Edit*);
void editevent(struct Edit*);
void editoutput(struct Edit*);
void editcursor(struct Edit*,WORD,WORD);
void convertmouse(struct Edit*,WORD*,WORD*);
void editoutputline(struct Edit*);
void editoutputlistline(struct Edit*,UBYTE*,LONG,WORD);
WORD readfileedit(struct Edit*);
WORD writefileedit(struct Edit*);
WORD readoffset(struct Edit*,LONG);
void calcblocks(struct Edit*);
void search(struct Edit*,WORD);
void blockinfo(struct Edit*);
void askline(struct Edit*);
WORD editprocessgadget(struct Edit*,struct IntuiMessage*,WORD);
void checkblocks(struct Edit*,WORD);
void freeedit(struct Edit*);
void limitblocks(struct Edit*,WORD,WORD);

#ifdef AROS
WORD hexconvert(UBYTE*,UBYTE*,WORD,WORD);
#else
WORD __asm hexconvert(register __a0 UBYTE*,register __a1 UBYTE*,register __d0 WORD,register __d1 WORD);
#endif

WORD windownewsize(void);
void freelistgads(void);
void freegadgets(struct Gadget*);
void draw3dbox(struct RastPort*,WORD,WORD,WORD,WORD,WORD);
WORD diskio(struct FMList*,struct IOStdReq*);
WORD finddevice(BPTR,struct DosList**,struct DosList**);
void doborder(WORD*,struct Border*,struct Border*,WORD*,struct Border*,struct Border*,WORD,WORD);

#ifdef AROS
ULONG checksum(UBYTE *,LONG);
#else
ULONG __asm checksum(register __a0 UBYTE *,register __d0 LONG);
#endif

void fileedit(void)
{
struct ProcMsg *pm;
struct Edit *edit;
UBYTE oname[24];
D(bug("edit.c fileedit 168 \n")); 
pm=sinitproc();
D(bug("edit.c fileedit 173 \n")); 
if(!(edit=allocmem(sizeof(struct Edit)))) 
{
D(bug("edit.c fileedit 176 \n")); 
goto endi;
}
D(bug("edit.c fileedit 179 \n")); 
edit->type=0;
edit->list=fmmain.sourcedir;
edit->cmc=pm->cmc;
D(bug("edit.c fileedit 183 \n")); 
strcpy(oname,edit->list->workname);
D(bug("edit.c fileedit 185 \n")); 
strcpy(edit->list->workname,edit->cmc->label);
D(bug("edit.c fileedit 187 \n")); 
if(sselected(edit->list,1)) {
D(bug("edit.c fileedit 189 \n")); 
	if((edit->node=findselnode(edit->list))) {
	D(bug("edit.c fileedit 191 \n")); 
		if((edit->handle=openfile(edit->list,NDFILE(edit->node),OFNORMAL))) {
			D(bug("edit.c fileedit 193 \n")); 
			editor(edit);
			closefile(edit->handle);
		}
		edit->node->flags&=~NSELECTED;
		outputlistline(edit->list,edit->node);
	}
}
strcpy(edit->list->workname,oname);
freemem(edit);
endi:
deinitproc(pm);
}
void diskedit(void)
{
struct ProcMsg *pm;
struct Edit *edit;
struct DosList *doslist=0;
struct DosList *vollist;
struct FileSysStartupMsg *fssm;
struct DosEnvec *de;
LONG apu1;
WORD dl=0,nodos=0;
BPTR lock=0,rlock;
UBYTE oname[24];

pm=sinitproc();
if(!(edit=allocmem(sizeof(struct Edit)))) goto endi;
edit->type=1;
edit->cmc=pm->cmc;
edit->dec=getconfig(edit->cmc);
edit->list=fmmain.sourcedir;
edit->node=findselnodeall(edit->list);
strcpy(oname,edit->list->workname);
strcpy(edit->list->workname,edit->cmc->label);
if(!edit->node&&edit->list->flags&LDIRLIST) {
	lock=fmlock(edit->list,edit->list->path);
} else if(edit->node&&(edit->node->flags&(NDEVICE|NFILE|NDIRECTORY))) {
	if(edit->list->flags&LDEVLIST) {
		fmmain.myproc->pr_WindowPtr=(APTR)-1;
		lock=Lock(NDFILE(edit->node),SHARED_LOCK);
		fmmain.myproc->pr_WindowPtr=fmmain.ikkuna;
		nodos=1;
	} else {
		lock=fmlock(edit->list,NDFILE(edit->node));
	}
	edit->node->flags&=~NSELECTED;
} else {
	strcpymsg(edit->list->fmmessage1,MSG_MAIN_NOSOURCEFILE);
	fmmessage(edit->list);
	goto dend;
}
if(!lock&&!nodos) goto dend;

apu1=0;
memseti(edit->devname,0,32);
doslist=LockDosList(LDF_VOLUMES|LDF_DEVICES|LDF_READ);
dl=1;
if(!lock) {
	strcpy(edit->devname,NDFILE(edit->node));
	edit->devname[strlen(edit->devname)-1]=0;
	doslist=FindDosEntry(doslist,edit->devname,LDF_DEVICES);
	if(doslist) apu1=1;
} else if(finddevice(lock,&doslist,&vollist)) {
	siirrabstr(doslist->dol_Name,edit->devname);
	apu1=1;
}
edit->devname[strlen(edit->devname)]=':';
if(apu1) {

	fssm=(struct FileSysStartupMsg*)BADDR(doslist->dol_misc.dol_handler.dol_Startup);
	if(fssm) {

		de=edit->dosenv=(struct DosEnvec*)BADDR(fssm->fssm_Environ);

		edit->blocklen=edit->dosenv->de_SizeBlock<<2;

edit->ioreq=(struct IOStdReq*)opendevice(edit->list,(UBYTE*)BADDR(fssm->fssm_Device)+1,fssm->fssm_Unit,fssm->fssm_Flags,sizeof(struct IOStdReq));
		apu1=de->de_Surfaces*de->de_BlocksPerTrack*512L;
		if(!edit->dec->lowcyl) edit->lowoffset=de->de_LowCyl*apu1;
		edit->maxoffset=(de->de_HighCyl+1)*apu1;
		UnLockDosList(LDF_VOLUMES|LDF_DEVICES|LDF_READ);
		dl=0;
		if(lock) {
		//	edit->offset=((struct FileLock*)(lock<<2))->fl_Key*(de->de_SizeBlock<<2);
			rlock=Lock(edit->devname,SHARED_LOCK);
			if(rlock) {
		//	edit->rootoffset=((struct FileLock*)(rlock<<2))->fl_Key*(de->de_SizeBlock<<2);
				UnLock(rlock);
			}
		}
		if(edit->ioreq) {
			editor(edit);
			closedevice((struct IORequest*)edit->ioreq);
		}
	}
}
dend:
if(dl) UnLockDosList(LDF_VOLUMES|LDF_DEVICES|LDF_READ);
if(lock) UnLock(lock);
outputlist(edit->list);
strcpy(edit->list->workname,oname);
freemem(edit);
endi:
deinitproc(pm);
}

void editor(struct Edit *edit)
{
UWORD flags;

flags=fmconfig->flags;
fmconfig->flags&=~MSUBPROC;
edit->list->flags|=LACTIVE;
edit->oldsearch=-1;
if(editwindownewsize(edit)) editevent(edit);
freeedit(edit);
edit->list->fmmessage1[0]=0;
edit->list->fmmessage2[0]=0;
fmmessage(edit->list);
fmconfig->flags=flags;
edit->list->flags&=~LACTIVE;
windownewsize();
}

void editevent(struct Edit *edit)
{
struct Gadget *cmgad;
struct fmboolgadget *fbg;
WORD apu1,apu2;

ULONG class;
UWORD code;
struct IntuiMessage *message=0;
struct AppMessage *appiconmsg=0,*appwinmsg=0;
ULONG signalmask;
WORD quit=0;
WORD mousex,mousey,curx,cury;
LONG offset;
struct InputEvent ie;
UBYTE chartab[2];
WORD asciicode,mode;
WORD oldhex=-1;
//struct DosEnvec *de;

ie.ie_Class=IECLASS_RAWKEY;
ie.ie_SubClass=0;
//de=edit->dosenv;
signalmask=1L<<fmmain.ikkuna->UserPort->mp_SigBit;
if(fmmain.appiconport) signalmask|=(1L<<fmmain.appiconport->mp_SigBit);
if(fmmain.appwinport) signalmask|=(1L<<fmmain.appwinport->mp_SigBit);

while(!quit) {
	for(;;) {
		message=(struct IntuiMessage*)GetMsg(fmmain.ikkuna->UserPort);
		if(fmmain.appiconport) appiconmsg=(struct AppMessage*)GetMsg(fmmain.appiconport);
		if(fmmain.appwinport) appwinmsg=(struct AppMessage*)GetMsg(fmmain.appwinport);
		if(!message&&!appiconmsg&&!appwinmsg) {
			Wait(signalmask);
		} else {
			break;
		}
	}
	if(appwinmsg) {
		ReplyMsg((struct Message*)appwinmsg);
		appwinmsg=0;
	}
	if(appiconmsg) {
		ScreenToFront(fmmain.naytto);
		WindowToFront(fmmain.ikkuna);
		ReplyMsg((struct Message*)appiconmsg);
		appiconmsg=0;
	}
	if(!message) continue;

	class=message->Class;
	code=message->Code;
	mousex=message->MouseX;
	mousey=message->MouseY;
	mousex-=fmmain.ikkuna->BorderLeft;
	mousey-=fmmain.ikkuna->BorderTop;

	switch(class)
	{
	case IDCMP_CLOSEWINDOW:
	quit=1;
	break;
	case IDCMP_REFRESHWINDOW:
	BeginRefresh(fmmain.ikkuna);
	EndRefresh(fmmain.ikkuna,1);
	break;
	case IDCMP_NEWSIZE:
	if(!editwindownewsize(edit)) quit=1;
	break;
	case IDCMP_RAWKEY:
	curx=edit->cursorx;
	cury=edit->cursory;
	offset=edit->offset;
	mode=edit->mode;
	if(edit->mode>=0) {
		switch(code)
		{
		case 0x1b:	/* esc */
		mode=-1;
		editcursor(edit,-1,-1);
		break;
		case 0x2e:	/* 5 */
		mode^=1;
		break;
		case 0x4c:	/* up */
		case 0x3e:
			if(cury) cury--;
		break;
		case 0x4d:	/* down */
		case 0x1e:
			if(cury<edit->rows-1) cury++;
		break;
		case 0x4f:	/* left */
		case 0x2d:
			if(curx) curx--;
		break;
		case 0x4e:	/* right */
		case 0x2f:
			if(curx<edit->columns-1) curx++;
		break;
		}
		if(mode!=edit->mode||curx!=edit->cursorx||cury!=edit->cursory) {
			if(edit->mode!=mode) edit->mode=mode;
			editcursor(edit,curx,cury);
			break;
		}
	} else {
		switch(code)
		{
		case 0x3d:	/* home */
		offset=0;
		break;
		case 0x1d:	/* end */
		if(edit->type)
			offset=edit->maxoffset-edit->blocklen;
			else
			offset=edit->handle->size;
		break;
		case 0x4f:	/* left */
		case 0x3e:
		if(edit->type)
			offset-=edit->bufsize;
			else
			offset-=edit->columns;
		break;
		case 0x4e:	/* right */
		case 0x1e:
		if(edit->type)
			offset+=edit->bufsize;
			else
			offset+=edit->columns;
		break;
		case 0x3f:	/* page up */
		if(edit->type) {
			edit->screenoffset-=edit->screenbytes-edit->columns;
			editoutput(edit);
			break;
		}
		case 0x4c:	/* up */
		if(edit->type)
			offset-=edit->bufsize;
			else
			offset-=edit->screenbytes-edit->columns;
		break;
		case 0x1f:	/* page down */
		if(edit->type) {
			edit->screenoffset+=edit->screenbytes-edit->columns;
			editoutput(edit);
			break;
		}
		case 0x4d:	/* down */
		if(edit->type)
			offset+=edit->bufsize;
			else
			offset+=edit->screenbytes-edit->columns;
		break;
		case 0x2e:	/* root */
		if(edit->type) offset=edit->rootoffset;
		break;
		}
		if(offset!=edit->offset) {
			if(!readoffset(edit,offset)) quit=1;
			break;
		}
	}
	ie.ie_Code=code&(~0x80);
	ie.ie_Qualifier=message->Qualifier&(~0x80);
	ie.ie_EventAddress=(APTR*)*((ULONG*)message->IAddress);
	if (MapRawKey(&ie,chartab,2,0)==1) {
		asciicode=chartab[0];
		switch(edit->mode)
		{
		case -1:	/* none */
		asciicode=ToUpper(asciicode);
		if(!(code&0x80)) {
			if(asciicode==0x0d&&!edit->type) askline(edit);
			apu1=(message->Qualifier&3)?-1:1;
			apu2=-1;
			if(asciicode==edit->btsh[0]) apu2=0;
			if(asciicode==edit->btsh[3]) apu2=3;
			if(asciicode==edit->btsh[2]) apu2=2;
			if(asciicode==edit->btsh[1]) apu2=1;
			limitblocks(edit,apu2,apu1);
		}
		if(asciicode) {
			fbg=&edit->exitgadget;
			for(apu1=0;apu1<4;apu1++) {
				cmgad=&fbg->gadget;
				if(asciicode==cmgad->GadgetID) {
					if(editprocessgadget(edit,message,cmgad->GadgetID)) quit=1;
					break;
				}
				fbg++;
			}
		}
		break;
		case 0:		/* hex */
		case 1:		/* asc */
			if(code&0x80) break;
			if(edit->mode==0) {
				asciicode=ToUpper(asciicode);
				asciicode-='0';
				if(asciicode>9) asciicode-=7;
				if(asciicode>15||asciicode<0) break;
				if(oldhex==-1) {
					oldhex=asciicode;
					break;
				} else {
					asciicode|=oldhex<<4;
				}
			}
			curx=edit->cursorx;cury=edit->cursory;
			editcursor(edit,-1,-1);
			edit->buffer[edit->cursoroffset]=asciicode;
			edit->usebuffer[edit->cursoroffset]=1;
			editoutputline(edit);
			if(curx<edit->columns-1) {
				curx++;
			} else {
				curx=0;
				cury++;
				if(cury>=edit->rows) cury=edit->rows-1;
			}
			editcursor(edit,curx,cury);
			oldhex=-1;
		break;
		}
	}
	break;
	case IDCMP_MOUSEBUTTONS:
		if(mousey<=edit->editheight+edit->yoffset) {
			switch(code)
			{
			case MENUDOWN:
			editcursor(edit,-1,-1);
			edit->mode=-1;
			editcursor(edit,-1,-1);
			ReportMouse(0,fmmain.ikkuna);
			break;
			case SELECTDOWN:
				ReportMouse(1,fmmain.ikkuna);
				convertmouse(edit,&mousex,&mousey);
				editcursor(edit,mousex,mousey);
			break;
			case SELECTUP:
				ReportMouse(0,fmmain.ikkuna);
				convertmouse(edit,&mousex,&mousey);
				editcursor(edit,mousex,mousey);
			break;
			}
		} else {
			editcursor(edit,-1,-1);
			edit->mode=-1;
			editcursor(edit,-1,-1);
			ReportMouse(0,fmmain.ikkuna);
			if((code==SELECTDOWN||code==MENUDOWN)&&mousey>=edit->editheight+edit->yoffset+fmconfig->spaceh&&mousey<=edit->editheight+edit->yoffset+fmconfig->spaceh+fmmain.txtysize) {
				if(edit->type) {				
					if(mousex>=edit->xoffsethex&&mousex<=edit->xoffsethex+64-fmconfig->spacew) readoffset(edit,edit->rootoffset);
					for(apu1=0;apu1<4;apu1++) {
						if(mousex>=edit->btshxw[apu1*2]&&mousex<=edit->btshxw[apu1*2+1]) limitblocks(edit,apu1,code==SELECTDOWN?1:-1);
					}
				} else {
					if(code==SELECTDOWN&&mousex>=edit->xoffsetasc&&mousex<=edit->xoffsetasc+edit->ascwidth) askline(edit);
				}
			}
		}			
	break;
	case IDCMP_MOUSEMOVE:
		convertmouse(edit,&mousex,&mousey);
		editcursor(edit,mousex,mousey);
	break;
	case IDCMP_GADGETUP:
	case IDCMP_GADGETDOWN:
	apu1=((struct Gadget*)message->IAddress)->GadgetID;
	if(apu1==1) {
		edit->blocks=edit->blockgad.si.LongInt;
		checkblocks(edit,1);
	} else if(apu1<=4) {
		edit->tracks=edit->trackgad.si.LongInt;
		edit->sectors=edit->secgad.si.LongInt;
		edit->heads=edit->headgad.si.LongInt;
		edit->blocks=edit->tracks*edit->sectors*edit->heads;
		checkblocks(edit,0);
	}
	if(apu1>=32) {
		if(editprocessgadget(edit,message,apu1)) quit=1;
	}
	break;
	}



	ReplyMsg((struct Message*)message);
	message=0;

}
ReportMouse(0,fmmain.ikkuna);
}

WORD readoffset(struct Edit *edit,LONG offset)
{
edit->oldsearch=-1;
switch(edit->type)
{
case 0:
if(offset>edit->handle->size-edit->bufsize) offset=edit->handle->size-edit->bufsize;
break;
case 1:
if(offset>=edit->maxoffset) offset=edit->maxoffset-edit->bufsize;
break;
}
if(offset<0) offset=0;
edit->offset=offset;
if(!readfileedit(edit)) return(0);
editoutput(edit);
return(1);
}

void convertmouse(struct Edit *edit,WORD *amousex,WORD *amousey)
{
WORD mousex,mousey;

mousex=*amousex;mousey=*amousey;
mousey-=edit->yoffset;
if(mousey>=0) mousey/=fmmain.listysize; else mousey=0;
if(mousex>=edit->xoffsetasc) {
	edit->mode=1;
	mousex-=edit->xoffsetasc;
	mousex/=fmmain.listxsize;
} else {
	edit->mode=0;
	mousex-=edit->xoffsethex;
	if(mousex>=0) mousex/=(fmmain.listxsize*2); else mousex=0;
}
if(mousey>=edit->rows) mousey=edit->rows-1;
if(mousex>=edit->columns) mousex=edit->columns-1;
*amousex=mousex;*amousey=mousey;
}

void editoutputline(struct Edit *edit)
{
ObtainSemaphore(&fmmain.gfxsema);
SetAPen(fmmain.rp,fmconfig->txtpen);
SetDrMd(fmmain.rp,JAM2);
SetFont(fmmain.rp,fmmain.listfont);
editoutputlistline(edit,edit->buffer+edit->cursory*edit->columns+edit->screenoffset,edit->bufsize-edit->cursory*edit->columns-edit->screenoffset,edit->cursory);
SetFont(fmmain.rp,fmmain.txtfont);
ReleaseSemaphore(&fmmain.gfxsema);
}
void editoutput(struct Edit *edit)
{
WORD yy;
UBYTE *memory;
LONG bsize;

if(edit->type) {
	if(edit->screenoffset>edit->bufsize-edit->screenbytes) edit->screenoffset=edit->bufsize-edit->screenbytes;
	if(edit->screenoffset<0) edit->screenoffset=0;
}
memory=edit->buffer+edit->screenoffset;
bsize=edit->bufsize-edit->screenoffset;
ObtainSemaphore(&fmmain.gfxsema);
SetAPen(fmmain.rp,fmconfig->txtpen);
SetDrMd(fmmain.rp,JAM2);
SetFont(fmmain.rp,fmmain.listfont);
for(yy=0;yy<edit->rows;yy++) {
	editoutputlistline(edit,memory,bsize,yy);
	bsize-=edit->columns;
	memory+=edit->columns;
}
SetFont(fmmain.rp,fmmain.txtfont);
ReleaseSemaphore(&fmmain.gfxsema);
edit->cursorx=-1;
editcursor(edit,-1,-1);
if(edit->type) calcblocks(edit);
}

void editoutputlistline(struct Edit *edit,UBYTE *memory,LONG bsize,WORD yy)
{
UBYTE vara[1000];
WORD yyy,yyyb;
UBYTE *ptr;

yyyb=yy*fmmain.listysize+edit->yoffset;
yyy=yyyb+fmmain.listbaseline;
if(bsize>=edit->columns) {
	hexconvert(memory,vara,edit->columns,edit->columns);
	Move(fmmain.rp,edit->xoffsethex,yyy);
	Text(fmmain.rp,vara,edit->columns*2);
	Move(fmmain.rp,edit->xoffsetasc,yyy);
	Text(fmmain.rp,vara+edit->columns*2+1,edit->columns);
} else if (bsize>0) {
	hexconvert(memory,vara,bsize,bsize);
	Move(fmmain.rp,edit->xoffsethex,yyy);
	Text(fmmain.rp,vara,bsize*2);
	Move(fmmain.rp,edit->xoffsetasc,yyy);
	Text(fmmain.rp,vara+bsize*2+1,bsize);
	SetAPen(fmmain.rp,fmconfig->backpen);
	RectFill(fmmain.rp,edit->xoffsethex+bsize*fmmain.listxsize*2,yyyb,edit->xoffsethex+(edit->columns*2)*fmmain.listxsize-1,yyyb+fmmain.listysize-1);
	RectFill(fmmain.rp,edit->xoffsetasc+bsize*fmmain.listxsize,yyyb,edit->xoffsetasc+(edit->columns)*fmmain.listxsize-1,yyyb+fmmain.listysize-1);
	SetAPen(fmmain.rp,fmconfig->txtpen);
} else {
	SetAPen(fmmain.rp,fmconfig->backpen);
	RectFill(fmmain.rp,edit->xoffsethex,yyyb,edit->xoffsethex+edit->columns*fmmain.listxsize*2-1,yyyb+fmmain.listysize-1);
	RectFill(fmmain.rp,edit->xoffsetasc,yyyb,edit->xoffsetasc+edit->columns*fmmain.listxsize-1,yyyb+fmmain.listysize-1);
	SetAPen(fmmain.rp,fmconfig->txtpen);
}		
ptr=edit->usebuffer+yy*edit->columns+edit->screenoffset;
SetDrMd(fmmain.rp,COMPLEMENT);
for(yyy=0;yyy<edit->columns;yyy++) {
	if(ptr[yyy]==1) fmmain.rp->Mask=255;
	if(ptr[yyy]==2) fmmain.rp->Mask=3;
	if(ptr[yyy]) {
		RectFill(fmmain.rp,edit->xoffsethex+yyy*fmmain.listxsize*2,yyyb,edit->xoffsethex+((yyy+1)*fmmain.listxsize)*2-1,yyyb+fmmain.listysize-1);
		RectFill(fmmain.rp,edit->xoffsetasc+yyy*fmmain.listxsize,yyyb,edit->xoffsetasc+((yyy+1)*fmmain.listxsize)-1,yyyb+fmmain.listysize-1);
	}

}
fmmain.rp->Mask=255;
SetDrMd(fmmain.rp,JAM2);
}

void editcursor(struct Edit *edit,WORD x,WORD y)
{
WORD yyyb,xxxb;
UBYTE vara[40];
LONG offset;
UBYTE *aschex;

ObtainSemaphore(&fmmain.gfxsema);
SetDrMd(fmmain.rp,COMPLEMENT);
fmmain.rp->Mask=1;
if(edit->cursorx>=0) {
	yyyb=edit->cursory*fmmain.listysize+edit->yoffset;
	xxxb=edit->cursorx*fmmain.listxsize;
	RectFill(fmmain.rp,edit->xoffsethex+xxxb*2,yyyb,edit->xoffsethex+(xxxb+fmmain.listxsize)*2-1,yyyb+fmmain.listysize-1);
	RectFill(fmmain.rp,edit->xoffsetasc+xxxb,yyyb,edit->xoffsetasc+xxxb+fmmain.listxsize-1,yyyb+fmmain.listysize-1);
}
if(x>=0&&y>=0) {
	edit->cursorx=x;
	edit->cursory=y;
	if(y*edit->columns+x>=edit->bufsize) {
		x=edit->cursorx=(edit->bufsize-1)%edit->columns;
		y=edit->cursory=(edit->bufsize-1)/edit->columns;
	}
	yyyb=edit->cursory*fmmain.listysize+edit->yoffset;
	xxxb=edit->cursorx*fmmain.listxsize;
	RectFill(fmmain.rp,edit->xoffsethex+xxxb*2,yyyb,edit->xoffsethex+(xxxb+fmmain.listxsize)*2-1,yyyb+fmmain.listysize-1);
	RectFill(fmmain.rp,edit->xoffsetasc+xxxb,yyyb,edit->xoffsetasc+xxxb+fmmain.listxsize-1,yyyb+fmmain.listysize-1);
	edit->cursoroffset=y*edit->columns+x;
} else {
	edit->cursorx=-1;
}
SetDrMd(fmmain.rp,JAM2);
fmmain.rp->Mask=255;
ReleaseSemaphore(&fmmain.gfxsema);

switch(edit->mode)
{
case 0:
aschex=getstring(MSG_FDEDIT_HEX);
break;
case 1:
aschex=getstring(MSG_FDEDIT_ASC);
break;
default:
aschex=" ";
break;
}
switch(edit->type)
{
case 0:
offset=edit->offset;
if(edit->cursorx>=0) offset+=edit->cursory*edit->columns+edit->cursorx;
sformatmsg(vara,MSG_FDEDIT_FILEINFO1,offset,edit->handle->size,aschex);
fittext(fmmain.rp,vara,-1,edit->xoffsetasc-1,edit->infoliney+1,edit->ascwidth,0);
break;
case 1:
blockinfo(edit);
edit->blocks=edit->offset/edit->blocklen;
break;
}

}

WORD editwindownewsize(struct Edit *edit)
{
WORD height,width,tfh;
struct Gadget *gad;
struct StringInfo *si;
struct fmboolgadget *fbg;
struct trackgadget *tg;
WORD cnt;
WORD apu1,apu2,bottomx,apu3,apu4,bottomwidth;
UBYTE *ptr1;
UBYTE gadname[16];
LONG offset;

freeedit(edit);
ObtainSemaphore(&fmmain.gfxsema);
SetAPen(fmmain.rp,fmconfig->backpen);
RectFill(fmmain.rp,fmmain.ikkuna->BorderLeft,fmmain.ikkuna->BorderTop,fmmain.ikkuna->Width+fmmain.ikkuna->BorderLeft,fmmain.ikkuna->Height+fmmain.ikkuna->BorderTop);
height=fmmain.ikkuna->Height-fmmain.ikkuna->BorderTop-fmmain.ikkuna->BorderBottom;
width=fmmain.ikkuna->Width-fmmain.ikkuna->BorderLeft-fmmain.ikkuna->BorderRight;
tfh=fmmain.txtysize+1;

edit->cursorx=-1;
edit->mode=-1;
edit->columns=((width-3*fmconfig->spacew)/(fmmain.listxsize*3))&0xfffe;
edit->rows=(height-5*tfh-3*fmconfig->spaceh+1)/fmmain.listysize;
edit->screenbytes=edit->columns*edit->rows;
edit->ascwidth=edit->columns*fmmain.listxsize;
edit->hexwidth=edit->ascwidth*2;
edit->ascwidth++;
edit->hexwidth++;
edit->editheight=edit->rows*fmmain.listysize+1;
apu1=(((width-3*fmconfig->spacew-edit->ascwidth-edit->hexwidth)+2)/3);
edit->xoffsethex=fmconfig->spacew+1+apu1;
edit->xoffsetasc=edit->xoffsethex+edit->hexwidth+apu1+fmconfig->spacew;
edit->yoffset=fmconfig->spaceh+1;
if (fmconfig->flags&MDOUBLED) {
	draw3dbox(fmmain.rp,2,1,width-2*2,height-2*1,1);
	SetAPen(fmmain.rp,fmconfig->mainbackfillpen);
	RectFill(fmmain.rp,2,1,width-2*2+1,height-2*1);
} else {
	draw3dbox(fmmain.rp,1,1,width-2*1,height-2*1,1);
	SetAPen(fmmain.rp,fmconfig->mainbackfillpen);
	RectFill(fmmain.rp,1,1,width-2*1,height-2*1);
}
draw3dbox(fmmain.rp,edit->xoffsethex-1,edit->yoffset-1,edit->hexwidth,edit->editheight,0);
draw3dbox(fmmain.rp,edit->xoffsetasc-1,edit->yoffset-1,edit->ascwidth,edit->editheight,0);
edit->infoliney=edit->yoffset+edit->editheight+fmconfig->spaceh-1;

switch(edit->type)
{
case 0: /* file editor */
draw3dbox(fmmain.rp,edit->xoffsethex-1,edit->infoliney,edit->hexwidth,tfh,0);
draw3dbox(fmmain.rp,edit->xoffsetasc-1,edit->infoliney,edit->ascwidth,tfh,0);
fittext(fmmain.rp,edit->handle->filename,-1,edit->xoffsethex-1,edit->infoliney+1,edit->hexwidth,0);
edit->bufsize=edit->screenbytes;
if(edit->handle->size<edit->bufsize) edit->bufsize=edit->handle->size;
break;
case 1: /* disk editor */
draw3dbox(fmmain.rp,edit->xoffsethex-1,edit->infoliney,64,tfh,0);
fittext(fmmain.rp,edit->devname,-1,edit->xoffsethex-1,edit->infoliney+1,64,0);
edit->xoffsetbtsh=edit->xoffsethex+64+fmconfig->spacew-1;
edit->xwidthbtsh=edit->hexwidth-64-fmconfig->spacew;
draw3dbox(fmmain.rp,edit->xoffsetbtsh,edit->infoliney,edit->xwidthbtsh,tfh,0);
draw3dbox(fmmain.rp,edit->xoffsetasc-1,edit->infoliney,edit->ascwidth,tfh,0);
edit->bufsize=edit->blocklen;
break;
}

fmmain.bottomliney=height-tfh-fmconfig->spaceh+1;
fmmain.messageliney1=height-3*tfh-2*fmconfig->spaceh+1;
fmmain.messageliney2=fmmain.messageliney1+tfh;

SetDrMd(fmmain.rp,JAM1);
bottomwidth=0;
ptr1=getstring(MSG_FDEDIT_GADGETS);
bottomx=fmmain.ikkuna->Width-fmmain.ikkuna->BorderLeft-fmmain.ikkuna->BorderRight-fmconfig->spacew/3*2;
fbg=&edit->exitgadget;
for(cnt=0;cnt<4;cnt++) {
	gad=&fbg->gadget;
	gad->TopEdge=fmmain.bottomliney-2;
	gad->GadgetID=edit->keyshorts[cnt]=scanus(ptr1);
	apu1=0;
	while(*ptr1) {
		if(*ptr1!='_') gadname[apu1++]=*ptr1;
		ptr1++;
	}
	ptr1++;
	gadname[apu1]=0;
	textextent(fmmain.rp,gadname,&apu1,&apu2);
	gad->Width=(apu1+2*fmconfig->spaceh);
	bottomwidth+=gad->Width;
	gad->Height=fmmain.txtysize+2+1;
	bottomx-=gad->Width;
	gad->LeftEdge=bottomx;
	doborder(fbg->xy1,&fbg->border1,&fbg->border2,fbg->xy2,&fbg->border3,&fbg->border4,gad->Height,gad->Width);
	gad->GadgetRender=&fbg->border1;
	gad->SelectRender=&fbg->border3;
	gad->Activation=RELVERIFY;
	gad->GadgetType=BOOLGADGET;
	gad->Flags=GADGHIMAGE;
	fbg++;
	gad->NextGadget=&fbg->gadget;
	SetAPen(fmmain.rp,fmconfig->backpen);
	RectFill(fmmain.rp,gad->LeftEdge+2,gad->TopEdge+1,gad->Width+gad->LeftEdge-3,gad->Height+gad->TopEdge-2);
	SetAPen(fmmain.rp,fmconfig->txtpen);
	Move(fmmain.rp,gad->LeftEdge+fmconfig->spaceh,gad->TopEdge+fmmain.txtbaseline+1);
	Text(fmmain.rp,gadname,strlen(gadname));
}
gad->NextGadget=0;
AddGList(fmmain.ikkuna,&edit->exitgadget.gadget,-1,-1,0);
SetDrMd(fmmain.rp,JAM2);
SetAPen(fmmain.rp,fmconfig->txtpen);

if(edit->type) {
	apu3=0;
	CopyMem(getstring(MSG_FDEDIT_DISKBTSH),edit->btsh,4);
	for(cnt=0;cnt<4;cnt++) {
		gadname[0]=edit->btsh[cnt];
		gadname[1]=0;
		textextent(fmmain.rp,gadname,&apu1,&apu2);
		if(apu3<apu1) apu3=apu1;
	}
	apu4=edit->xwidthbtsh/4;
	tg=&edit->blockgad;
	apu1=edit->xoffsetbtsh+fmconfig->spacew;
	for(cnt=0;cnt<4;cnt++) {
		Move(fmmain.rp,apu1,edit->infoliney+fmmain.txtbaseline+1);
		edit->btshxw[cnt*2]=fmmain.rp->cp_x;
		Text(fmmain.rp,&edit->btsh[cnt],1);
		edit->btshxw[cnt*2+1]=fmmain.rp->cp_x;
		gad=&tg->gadget;
		si=&tg->si;
		gad->TopEdge=edit->infoliney+1;
		gad->Height=tfh-1;
		gad->LeftEdge=apu1+apu3+fmconfig->spacew;
		gad->Width=apu4-2*fmconfig->spacew-apu3;
		apu1+=apu4;
		gad->Flags=GFLG_STRINGEXTEND;
		gad->Activation=RELVERIFY|LONGINT;
		gad->GadgetType=STRGADGET;
		gad->SpecialInfo=si;
		gad->GadgetID=cnt+1;
		si->Buffer=tg->buffer;
		tg->buffer[0]='0';
		si->UndoBuffer=fmmain.undobuf;
		si->Extension=&tg->se;
		si->MaxChars=8;
		tg->se.Font=fmmain.txtfont;
		tg->se.Pens[0]=tg->se.ActivePens[0]=fmconfig->txtpen;
		tg->se.Pens[1]=tg->se.ActivePens[1]=fmconfig->backpen;
		tg++;
		gad->NextGadget=&tg->gadget;
	}
	gad->NextGadget=0;
	AddGList(fmmain.ikkuna,&edit->blockgad.gadget,-1,-1,0);
}

draw3dbox(fmmain.rp,fmconfig->spacew,fmmain.messageliney1-1,width-2*fmconfig->spacew,2*tfh,0);
fmmain.bottomlinewidth=width-bottomwidth-fmconfig->spacew-2*fmconfig->spacew/3*2;
draw3dbox(fmmain.rp,fmconfig->spacew,fmmain.bottomliney-1,fmmain.bottomlinewidth,tfh,0);
fmmain.bottomlinewidth-=2*fmconfig->spacew;
if(fmmain.bottomlinewidth<0) fmmain.bottomlinewidth=0;

fmconfig->windowtop=fmmain.ikkuna->TopEdge;
fmconfig->windowleft=fmmain.ikkuna->LeftEdge;
fmconfig->windowheight=fmmain.ikkuna->Height;
fmconfig->windowwidth=fmmain.ikkuna->Width;
RefreshGList(&edit->exitgadget.gadget,fmmain.ikkuna,0,-1);
edit->gads=1;
ReleaseSemaphore(&fmmain.gfxsema);

edit->buffer=allocvec(edit->list,edit->bufsize,MEMF_CLEAR);
if(!edit->buffer) goto editend;
edit->usebufsize=edit->bufsize;
if(edit->screenbytes>edit->usebufsize) edit->usebufsize=edit->screenbytes;
edit->usebuffer=allocvec(edit->list,edit->usebufsize,MEMF_CLEAR);
if(!edit->usebuffer) goto editend;

offset=edit->offset;
edit->offset=-1;
if(!readoffset(edit,offset)) goto editend;
editoutput(edit);
return(1);
editend:
freeedit(edit);
return(0);
}

void freeedit(struct Edit *edit)
{
freelistgads();
freegadgets(fmmain.firstgadget); fmmain.firstgadget=0;
freemem(edit->buffer);
edit->buffer=0;
freemem(edit->usebuffer);
edit->usebuffer=0;
if(edit->gads) RemoveGList(fmmain.ikkuna,&edit->exitgadget.gadget,-1);
edit->gads=0;
}

WORD readfileedit(struct Edit *edit)
{
WORD ret;

memseti(edit->usebuffer,0,edit->usebufsize);
switch(edit->type)
{
case 0:
if(seek(edit->handle,edit->offset,OFFSET_BEGINNING)<0) return(0);
if(readbufferfile(edit->handle,edit->buffer,edit->bufsize)<0) return(0);
return(1);
break;
case 1:
edit->ioreq->io_Command=CMD_READ;
edit->ioreq->io_Data=edit->buffer;
edit->ioreq->io_Length=edit->blocklen;
edit->ioreq->io_Offset=edit->offset+edit->lowoffset;
ret=diskio(edit->list,edit->ioreq);
if(!ret) memseti(edit->buffer,0,edit->blocklen);
edit->ioreq->io_Command=TD_MOTOR;
edit->ioreq->io_Length=0;
diskio(edit->list,edit->ioreq);
edit->changed=1;
return(ret);
break;
}
return 0;
}

WORD writefileedit(struct Edit *edit)
{
if(!requestmsg(edit->list->workname,MSG_YES,MSG_NO,MSG_MAIN_AREYOUSURE)) return(1);
memseti(edit->usebuffer,0,edit->usebufsize);
switch(edit->type)
{
case 0:
if(seek(edit->handle,edit->offset,OFFSET_BEGINNING)<0) return(0);
if(writefile(edit->handle,edit->buffer,edit->bufsize)<0) return(0);
strcpymsg(edit->list->fmmessage1,MSG_MAIN_COMPLETED);
fmmessage(edit->list);
return(1);
break;
case 1:
if(checksum(edit->buffer,edit->blocklen)) {
        if(requestmsg(edit->list->workname,MSG_YES,MSG_NO,MSG_FDEDIT_CALCCHECKSUM)) {
                *((ULONG*)&edit->buffer[4*5])=0;
                *((ULONG*)&edit->buffer[4*5])=-checksum(edit->buffer,edit->blocklen);
                editoutput(edit);
        }
}
edit->ioreq->io_Command=CMD_WRITE;
edit->ioreq->io_Data=edit->buffer;
edit->ioreq->io_Length=edit->blocklen;
edit->ioreq->io_Offset=edit->offset+edit->lowoffset;
diskio(edit->list,edit->ioreq);
edit->ioreq->io_Command=CMD_UPDATE;
if(!diskio(edit->list,edit->ioreq)) {
	edit->ioreq->io_Command=CMD_CLEAR;
	diskio(edit->list,edit->ioreq);
} else {
	strcpymsg(edit->list->fmmessage1,MSG_MAIN_COMPLETED);
	fmmessage(edit->list);
}
edit->ioreq->io_Command=TD_MOTOR;
edit->ioreq->io_Length=0;
diskio(edit->list,edit->ioreq);
return(1);
break;
}
return 0;
}

void blockinfo(struct Edit *edit)
{
ULONG type1,type2;
ULONG *ptr;
UBYTE name[40],date1[40],date2[40],vara[100],cnt[16];
WORD dtype;
struct FMList *list;
UBYTE *aschex;

list=edit->list;
list->fmmessage1[0]=0;list->fmmessage2[0]=0;
ptr=(ULONG*)edit->buffer;

switch(edit->mode)
{
case 0:
aschex=getstring(MSG_FDEDIT_HEX);
break;
case 1:
aschex=getstring(MSG_FDEDIT_ASC);
break;
default:
aschex=" ";
break;
}
type1=ptr[0];
type2=ptr[127];
dtype=MSG_FDEDIT_DISKUNKNOWN;
if(type1==2&&type2==1) {
	dtype=MSG_FDEDIT_DISKROOT3;
	if(edit->changed) {
		siirrabstr((BSTR)MKBADDR(edit->buffer+0x1b0),name);	
		longtodatestring(date1,dstolong((struct DateStamp*)(edit->buffer+0x1a4)));
		longtodatestring(date2,dstolong((struct DateStamp*)(edit->buffer+0x1e4)));
		sformatmsg(list->fmmessage1,MSG_FDEDIT_DISKROOT1,name,date2,date1);
		sformatmsg(list->fmmessage2,MSG_FDEDIT_DISKROOT2,ptr[79]);
	}
}
if(type1==2&&type2==0xfffffffd) {
	dtype=MSG_FDEDIT_DISKFILE3;
	if(edit->changed) {
		siirrabstr((BSTR)MKBADDR(edit->buffer+0x1b0),name);
		longtodatestring(date1,dstolong((struct DateStamp*)(edit->buffer+0x1a4)));
		sformatmsg(list->fmmessage1,MSG_FDEDIT_DISKFILE1,name,ptr[81],date1);
		sformatmsg(list->fmmessage2,MSG_FDEDIT_DISKFILE2,ptr[2],ptr[4],ptr[124],ptr[125],ptr[126]);
	}
}
if(type1==2&&type2==2) {
	dtype=MSG_FDEDIT_DISKDIR3;
	if(edit->changed) {
		siirrabstr((BSTR)MKBADDR(edit->buffer+0x1b0),name);
		longtodatestring(date1,dstolong((struct DateStamp*)(edit->buffer+0x1a4)));
		sformatmsg(list->fmmessage1,MSG_FDEDIT_DISKDIR1,name,date1);
		sformatmsg(list->fmmessage2,MSG_FDEDIT_DISKDIR2,ptr[124],ptr[125]);
	}
}
if(type1==16&&type2==0xfffffffd) {
	dtype=MSG_FDEDIT_DISKLIST3;
	if(edit->changed) {
		sformatmsg(list->fmmessage1,MSG_FDEDIT_DISKLIST1,ptr[2],ptr[125],ptr[126]);
	}
}
if(type1==8&&edit->dosenv->de_DosType==0x444f5300) {
	dtype=MSG_FDEDIT_DISKDATA3;
	if(edit->changed) {
		sformatmsg(list->fmmessage1,MSG_FDEDIT_DISKDATA1,ptr[1],ptr[2],ptr[3],ptr[4]);
	}
}
cnt[0]=0;

if(edit->mode>=0) sformat(cnt,"%03.3d %s",edit->cursoroffset+edit->screenoffset,aschex);

sformatmsg(vara,MSG_FDEDIT_DISKINFO1,checksum(edit->buffer,edit->blocklen)?'-':'+',getstring(dtype),edit->screenoffset,cnt);
fittext(fmmain.rp,vara,-1,edit->xoffsetasc-1,edit->infoliney+1,edit->ascwidth,0);

if(edit->changed) fmmessage(list);
edit->changed=0;

}

void checkblocks(struct Edit *edit,WORD mode)
{
struct DosEnvec *de;

de=edit->dosenv;
if(!mode) {
	if(edit->tracks<0) edit->tracks=0;
	if(edit->tracks>de->de_HighCyl) edit->tracks=de->de_HighCyl;
	if(edit->sectors<0) edit->sectors=0;
	if(edit->sectors>=de->de_BlocksPerTrack) edit->sectors=de->de_BlocksPerTrack-1;
	if(edit->heads<0) edit->heads=0;
	if(edit->heads>=de->de_Surfaces) edit->heads=de->de_Surfaces-1;
	edit->blocks=edit->tracks*edit->dosenv->de_Surfaces*edit->dosenv->de_BlocksPerTrack;
	edit->blocks+=edit->heads*edit->dosenv->de_BlocksPerTrack;
	edit->blocks+=edit->sectors;
}
calcblocks(edit);
readoffset(edit,edit->blocks*edit->blocklen);
}

void calcblocks(struct Edit *edit)
{
struct DosEnvec *de;

de=edit->dosenv;
edit->sectors=edit->blocks%de->de_BlocksPerTrack;
edit->tracks=edit->blocks/(de->de_BlocksPerTrack*de->de_Surfaces);
edit->heads=(edit->blocks/de->de_BlocksPerTrack)%de->de_Surfaces;
sformat(edit->blockgad.buffer,"%ld",edit->blocks);
edit->blockgad.si.LongInt=edit->blocks;
sformat(edit->trackgad.buffer,"%ld",edit->tracks);
edit->trackgad.si.LongInt=edit->tracks;
sformat(edit->secgad.buffer,"%ld",edit->sectors);
edit->secgad.si.LongInt=edit->sectors;
sformat(edit->headgad.buffer,"%ld",edit->heads);
edit->headgad.si.LongInt=edit->heads;
RefreshGList(&edit->blockgad.gadget,fmmain.ikkuna,0,4);
}

void limitblocks(struct Edit *edit,WORD mode,WORD dir)
{
LONG *ptr;
struct DosEnvec *de;

if(mode<0) return;
de=edit->dosenv;
ptr=&edit->blocks;
ptr[mode]+=dir;
if(edit->blocks<0) edit->blocks=0;
if(edit->blocks>=edit->maxoffset/edit->blocklen) edit->blocks=edit->maxoffset/edit->blocklen-1;

if(edit->sectors<0) {
	edit->sectors=de->de_BlocksPerTrack-1;
	edit->heads--;
}
if(edit->sectors>=de->de_BlocksPerTrack) {
	edit->sectors=0;
	edit->heads++;
}
if(edit->heads<0) {
	edit->heads=de->de_Surfaces-1;
	edit->tracks--;
}
if(edit->heads>=de->de_Surfaces) {
	edit->heads=0;
	edit->tracks++;
}
checkblocks(edit,!mode?1:0);
}

void search(struct Edit *edit,WORD mode)
{
UBYTE str[64];
WORD len,len2;
WORD cnt;
UBYTE apu1,apu2;
UBYTE *sbuf=0;
LONG size;
LONG apu3,apu4;
LONG offset;
UBYTE hexdec;
LONG bufsize;
LONG oldoffset;

edit->list->fmmessage1[0]=0;
edit->list->fmmessage2[0]=0;
fmmessage(edit->list);
edit->list->flags&=~LABORTREQ;

if(!mode||edit->searchstring[0]==0) {
	if(!askstring(0,edit->list->workname,getstring(MSG_FDEDIT_SEARCH),edit->searchstring,32)) return;
}
len=strlen(edit->searchstring);
if(edit->searchstring[0]=='$') {
	hexdec=1;
	cnt=0;
	if(!(len&1)||len<=1) goto fail;
	len2=len-1;
	len=0;
	while(len2) {
		apu1=ToUpper(edit->searchstring[cnt*2+1])-'0';
		if(apu1>9) apu1-=7;
		if(apu1>15) goto fail;
		apu2=ToUpper(edit->searchstring[cnt*2+2])-'0';
		if(apu2>9) apu2-=7;
		if(apu2>15) goto fail;
		str[cnt++]=apu1<<4|apu2;
		len2-=2;
		len++;
	}
} else {
	hexdec=0;
	for(cnt=0;cnt<len;cnt++) str[cnt]=ToUpper(edit->searchstring[cnt]);
	str[cnt]=0;
	if(cnt<=0) goto fail;
}
if(edit->type) {
	bufsize=((fmconfig->dosbuffersize&0x00ffffff)/edit->blocklen)*edit->blocklen;
	sbuf=allocvec(edit->list,bufsize,0);
	if(sbuf) {
		edit->list->flags|=LUPDATEMSG;
		if(edit->oldsearch!=-1) 
			offset=edit->oldsearch+edit->blocklen;
			else
			offset=edit->offset;
		edit->oldsearch=-1;
		sformatmsg(edit->list->fmmessage1,MSG_FDEDIT_SEARCHING,offset/edit->blocklen,edit->maxoffset/edit->blocklen);
		fmmessage(edit->list);
		while(offset<edit->maxoffset) {
			sformatmsg(edit->list->fmmessage1,MSG_FDEDIT_SEARCHING,offset/edit->blocklen,edit->maxoffset/edit->blocklen);
			if(testabort(edit->list)) break;
			size=bufsize;
			if(size>edit->maxoffset-offset) {
				size=edit->maxoffset-offset;
				apu4=size;
			} else {
				apu4=size-edit->blocklen;
			}
			oldoffset=edit->offset;
			edit->offset=offset;
			calcblocks(edit);
			edit->offset=oldoffset;
			edit->ioreq->io_Command=CMD_READ;
			edit->ioreq->io_Data=sbuf;
			edit->ioreq->io_Length=size;
			edit->ioreq->io_Offset=offset+edit->lowoffset;
			if(!diskio(edit->list,edit->ioreq)) break;
			if(!hexdec) for(apu3=0;apu3<size;apu3++) sbuf[apu3]=ToUpper(sbuf[apu3]);
			for(apu3=0;apu3<apu4;apu3++) {
				if(!memcmp(str,&sbuf[apu3],len)) {
					apu4=apu3+offset;
					offset+=apu3;
					offset=(offset/edit->blocklen)*edit->blocklen;
					readoffset(edit,offset);
					apu3=apu4;
					apu4-=edit->offset;
					for(cnt=0;cnt<len;cnt++) {
						if(apu4+cnt>=0&&apu4+cnt<=edit->blocklen) edit->usebuffer[apu4+cnt]=2;
						}
					editoutput(edit);
					edit->oldsearch=apu3;
					goto sok;
				}
			}
			offset+=bufsize-edit->blocklen;
		}
		DisplayBeep(0);
	}
} else {
	bufsize=(fmconfig->dosbuffersize&0x00ffffff);
	sbuf=allocvec(edit->list,bufsize,0);
	if(sbuf) {
		edit->list->flags|=LUPDATEMSG;
		if(edit->oldsearch!=-1) 
			offset=edit->oldsearch+1;
			else
			offset=edit->offset;
		edit->oldsearch=-1;
		sformatmsg(edit->list->fmmessage1,MSG_FDEDIT_SEARCHING,offset,edit->handle->size);
		fmmessage(edit->list);
		while(offset<edit->handle->size) {
			sformatmsg(edit->list->fmmessage1,MSG_FDEDIT_SEARCHING,offset,edit->handle->size);
			if(testabort(edit->list)) break;
			if(seek(edit->handle,offset,OFFSET_BEGINNING)<0) break;
			size=bufsize;
			if(size>edit->handle->size-edit->handle->position) {
				size=edit->handle->size-edit->handle->position;
				apu4=size;
			} else {
				apu4=size-128;
			}
			if(readbufferfile(edit->handle,sbuf,size)<0) break;
			if(!hexdec) for(apu3=0;apu3<size;apu3++) sbuf[apu3]=ToUpper(sbuf[apu3]);
			for(apu3=0;apu3<apu4;apu3++) {
				if(!memcmp(str,&sbuf[apu3],len)) {
					apu4=apu3+offset;
					offset+=(apu3-edit->screenbytes/2);
					readoffset(edit,offset);
					apu3=apu4;
					apu4-=edit->offset;
					for(cnt=0;cnt<len;cnt++) {
						if(apu4+cnt>=0&&apu4+cnt<=edit->screenbytes) edit->usebuffer[apu4+cnt]=2;
						}
					editoutput(edit);
					edit->oldsearch=apu3;
					goto sok;
					}
			}
			offset+=bufsize-128;
		}
		DisplayBeep(0);
	}
}
sok:
if(edit->type) {
	edit->ioreq->io_Command=TD_MOTOR;
	edit->ioreq->io_Length=0;
	diskio(edit->list,edit->ioreq);
}
edit->list->flags&=~LUPDATEMSG;
edit->list->fmmessage1[0]=0;
fmmessage(edit->list);
freemem(sbuf);
return;
fail:
requestmsg(edit->list->workname,0,MSG_OK,MSG_FDEDIT_ILLEGALSEARCH);
}

void askline(struct Edit *edit)
{
LONG offset;

offset=edit->offset;
if(!asklong(0,edit->list->workname,getstring(MSG_FDEDIT_BYTENUMBER),&offset,0,edit->handle->size)) return;
readoffset(edit,offset);
}

WORD editprocessgadget(struct Edit *edit,struct IntuiMessage *msg,WORD id)
{
if(edit->keyshorts[3]==id) {
	search(edit,msg->Qualifier&0x80);
}
if(edit->keyshorts[0]==id) return(1);
if(edit->keyshorts[2]==id) {
	if(!readfileedit(edit)) return(1);
	editoutput(edit);
}
if(edit->keyshorts[1]==id) {
	writefileedit(edit);
	editoutput(edit);
}
return(0);
}

void *diskeditconfigdefault(struct CMenuConfig *cmc)
{
WORD ret;

ret=allocconfig(cmc,sizeof(struct DiskEditConfig));
if(ret<0) return(cmc->moreconfig);
if(!ret) return(0);
return(cmc->moreconfig);
}

WORD diskeditconfig(struct GUIBase *gb,struct CMenuConfig *cmc)
{
struct DiskEditConfig *config;
WORD c,lowcyl;

config=getconfig(cmc);
lowcyl=config->lowcyl;
setguigroup(gb,1,0);
reqinfomsg(gb,MSG_FDEDIT_CONFIG_NOLOWCYL,100,guiUC|guiLEFT);
setguigroup(gb,2,0);
reqcycle2msg(gb,100,&lowcyl);
commandanswer(gb);
c=quickreq(gb);
config->lowcyl=lowcyl;
return(c);
}

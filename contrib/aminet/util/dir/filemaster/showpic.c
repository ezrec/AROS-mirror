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
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <graphics/displayinfo.h>
#include <intuition/gadgetclass.h>
#include <datatypes/datatypes.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/pictureclass.h>
#include <proto/datatypes.h>
#include <dos/dos.h>

#include <stdio.h>
#include <string.h>

#include "fmnode.h"
#include "ilbm.h"
#include "fmlocale.h"

extern struct UtilityBase *UtilityBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;


WORD showpicture(struct FMHandle*,struct CMenuConfig*);
WORD showicon(struct FMHandle*,struct CMenuConfig*);
WORD showfont(struct FMHandle*,struct CMenuConfig*);
LONG rbytes(struct FMHandle*,UBYTE*,LONG,LONG*);
WORD decode(struct FMHandle*,struct ShowScreen*,struct FMBitMapHeader*,LONG);
void clipit(WORD,WORD,struct Rectangle*,struct Rectangle*,struct Rectangle*,struct Rectangle*,struct Rectangle*);

#ifndef AROS
extern WORD __asm unpack(register __a0 UBYTE**,register __a1 struct BitMap*,register __a2 WORD*,register __d0 WORD,register __d1 LONG,register __d2 WORD,register __d3 UBYTE,register __a6 struct ShowScreen*);
extern WORD __asm unpacknp(register __a0 UBYTE**,register __a1 struct BitMap*,register __a2 WORD*,register __d0 WORD,register __d1 LONG,register __d2 WORD,register __d3 UBYTE,register __a6 struct ShowScreen*);
void __asm initbitmapi(register __d0 WORD);
#else /* AROS */
extern WORD unpack(UBYTE**, struct BitMap*, WORD*, WORD, LONG, WORD, UBYTE, struct ShowScreen*);
extern WORD unpacknp(UBYTE**, struct BitMap*, WORD*, WORD, LONG, WORD, UBYTE, struct ShowScreen*);
void initbitmapi(WORD);
#endif /* AROS */

ULONG power2(WORD);
ULONG modefallback(ULONG,WORD,WORD,WORD);

extern struct FMConfig *fmconfig;
extern struct FMMain fmmain;

static void pictureinfo(struct FMHandle *h,WORD width,WORD height,WORD deep,ULONG modeid,UBYTE *name)
{
WORD apu1;
LONG apu3;
struct DisplayInfo di;
IPTR infotags[10];

h->owner->fmmessage2[0]=0;
if(width&&height) {
	apu3=power2(deep);
	infotags[0]=0; infotags[1]=0; infotags[2]=0; infotags[3]=0;
	if (GetDisplayInfoData(0,(UBYTE*)&di,sizeof(struct DisplayInfo),DTAG_DISP,modeid)>0) {
		apu1=0;
		if(di.PixelSpeed<=35)
			infotags[apu1++]=(IPTR)getstring(MSG_SHOWPIC_SHIRES);
		else if(di.PixelSpeed<=70)
			infotags[apu1++]=(IPTR)getstring(MSG_SHOWPIC_HIRES);
		if(di.PropertyFlags&DIPF_IS_LACE) infotags[apu1++]=(IPTR)getstring(MSG_SHOWPIC_LACE);
		if(di.PropertyFlags&DIPF_IS_HAM) {
			if(deep<=6) {
				infotags[apu1++]=(IPTR)getstring(MSG_SHOWPIC_HAM6);
				apu3=4096;
			} else {
				infotags[apu1++]=(IPTR)getstring(MSG_SHOWPIC_HAM8);
				apu3=262144;
			}
		}
		if(di.PropertyFlags&DIPF_IS_EXTRAHALFBRITE) infotags[apu1++]=(IPTR)getstring(MSG_SHOWPIC_EHB);
	}	
	sformat(h->owner->fmmessage2,getstring(MSG_SHOWPIC_INFO2),width,height,deep,apu3,infotags[0],infotags[1],infotags[2]);
}
sformat(h->owner->fmmessage1,getstring(MSG_SHOWPIC_INFO1),name,h->filename,h->size);
fmmessage(h->owner);
}

void __saveds showpic(void)
{
struct ProcMsg *pm;
struct FMList *slist;
register struct FMNode *node;
struct FMHandle *h;
WORD ret;

pm=sinitproc();
slist=fmmain.sourcedir;
if (!(setalloc(slist,1))) {
	initproc(0,0);
	goto endi;
}
initproc(slist,pm->cmc->label);
priority(pm->cmc);
ret=0;
if(sselected(slist,1)) {
	while((node=findselnode(slist))) {
		if ((h=openfile(slist,NDFILE(node),OFNORMAL|OFDECRUNCH))) {
			ret=isdatatypes(h,GID_PICTURE);
			if(ret==-99) {
				if (!Strnicmp(h->filename+strlen(h->filename)-5,".info",5)) {
					ret=showicon(h,pm->cmc);
				} else if (!Strnicmp(h->filename+strlen(h->filename)-5,".font",5)) {
					ret=showfont(h,pm->cmc);
				} else {		
					ret=showpicture(h,pm->cmc);
				}
			}
			closefile(h);
		}
		node->flags&=~NSELECTED;
		outputlistline(slist,node);
		if (!ret) break;
	}
}
endproc(slist);
endi:
deinitproc(pm);
}

WORD showdatatypepicture(struct FMHandle *h,struct DataType *dt,struct CMenuConfig *cmc)
{
struct DataTypeHeader *dth;
Object *obj;
struct ShowScreen shows;
struct BitMapHeader *bmhd=0;
ULONG modeid=0;
struct BitMap *bm;
struct gpLayout layout;
ULONG *creg,*palette=0;
LONG ncol,crsize,palsize;
UBYTE *src;
LONG type;
WORD ret=0;

changename(h->owner,cmc->label);
priority(cmc);

memseti(&shows,0,sizeof(struct ShowScreen));
dth=dt->dtn_Header;
if(h->flags&OFDECRUNCH) {
	type=DTST_RAM;
	src=h->decbuffer;
} else {
	type=DTST_FILE;
	src=h->filename;
}
pictureinfo(h,0,0,0,0,dth->dth_Name);
if(!(obj=NewDTObject(src,DTA_SourceType,type,DTA_GroupID,GID_PICTURE,PDTA_Remap,0,TAG_DONE))) goto error;
if(GetDTAttrs(obj,PDTA_ModeID,&modeid,PDTA_BitMapHeader,&bmhd,TAG_DONE)!=2) goto error;
pictureinfo(h,bmhd->bmh_Width,bmhd->bmh_Height,bmhd->bmh_Depth,modeid,dth->dth_Name);
layout.MethodID=DTM_PROCLAYOUT;
layout.gpl_GInfo=0;
layout.gpl_Initial=TRUE;
if(!DoDTMethodA(obj,0,0,(Msg)&layout)) goto error;
if(GetDTAttrs(obj,PDTA_BitMap,&bm,PDTA_CRegs,&creg,PDTA_NumColors,&ncol,TAG_DONE)!=3) goto error;
if(!bm||!creg||!ncol) goto error;
crsize=(ncol*3)*4;
palsize=crsize+2*4;
palette=allocvec(h->owner,palsize,MEMF_CLEAR);
if(!palette) goto error;
palette[0]=ncol<<16;
CopyMem(creg,&palette[1],crsize);
if(openshowscreen(h->owner,bmhd->bmh_Width,bmhd->bmh_Height,bmhd->bmh_Depth,modeid,bmhd->bmh_Width,bmhd->bmh_Height,bmhd->bmh_Depth,&shows)) {
	LoadRGB32(&shows.screen->ViewPort,palette);
	BltBitMap(bm,0,0,shows.bitmap,0,0,bmhd->bmh_Width,bmhd->bmh_Height,0xC0,0xFF,0);
	WaitBlit();
	SetPointer(shows.window,shows.mouse,2,16,0,0);
	ret=waitbutton(shows.window)+1;
	closeshowscreen(&shows);
}
goto picerr;
error:
if(!ret) requestmsg(h->owner->workname,0,MSG_OK,MSG_MAIN_DATATYPEERROR,dth->dth_Name);
picerr:
freemem(palette);
if(obj) DisposeDTObject(obj);
return(ret);
}
					

WORD showpicture(struct FMHandle *h,struct CMenuConfig *cmc)
{
struct ShowScreen shows;
struct FMList *list;
ULONG modeid;
struct FMBitMapHeader bmhd;

UBYTE *cmap=0;
ULONG longi1, longi2, len1;
ULONG ifflen;
WORD fbmhd=0,fmodeid=0,fcmap=0,retcode=0;

ULONG cmaplen = 0;
WORD wide,high,deep,sdeep;
WORD apu1;

#ifndef V39
UBYTE *ptr1;
#endif
#ifdef V39
ULONG apu3;
ULONG *cmapapu1;
WORD apu2;
#endif

memseti(&shows,0,sizeof(struct ShowScreen));
list=h->owner;
changename(list,cmc->label);
priority(cmc);
if (readbufferfile(h,&longi1,4)!=4) goto serr;
sformat(list->fmmessage1,getstring(MSG_FILECLICK_PICTURE),"ILBM",h->filename);
fmmessage(list);
modeid=fmodeid=0;
wide=320;high=256;sdeep=deep=1;

if (readbufferfile(h,&ifflen,4)!=4) goto serr;
if (readbufferfile(h,&longi2,4)!=4) goto serr;
ifflen-=4;
if (longi1!=ID_FORM||longi2!=ID_ILBM) {
	requestmsg(list->workname,0,MSG_OK,MSG_SHOWPIC_NOPICTURE,h->filename);
	goto serr;
}
if (ifflen>h->size-8) goto scorrupt;
for(;;) {
	if (readbufferfile(h,&longi1,4)!=4) goto serr;
	if (readbufferfile(h,&len1,4)!=4) goto serr;
	ifflen-=8;
	if (longi1==ID_BODY) break;
	switch(longi1)
	{
		case ID_BMHD:
		if (len1!=sizeof(struct FMBitMapHeader)) goto scorrupt;
		if (readbufferfile(h,&bmhd,len1)!=len1) goto serr;
		ifflen-=len1;
		fbmhd=1;
		wide=(RowBytes(bmhd.w))>=(RowBytes(bmhd.pageWidth))?bmhd.w:bmhd.pageWidth;
		high=MAX(bmhd.h,bmhd.pageHeight);
		deep=bmhd.nPlanes;
		sdeep=deep<=8?deep:8;
		if(bmhd.compression>1) goto scorrupt;
		break;
		case ID_CAMG:
		if (len1!=4) goto scorrupt;
		if (readbufferfile(h,&modeid,len1)!=len1) goto serr;
		ifflen-=len1;
		fmodeid=1;
		break;
		case ID_CMAP:
		cmaplen=len1;
		if (!(cmap=allocvec(list,cmaplen,0))) goto serr;
		if (readbufferfile(h,cmap,cmaplen)!=cmaplen) goto serr;
		ifflen-=cmaplen;
		if (cmaplen&1) {
			if (seek(h,1,OFFSET_CURRENT)<0) goto serr;
			ifflen--;
		}
		fcmap=1;
		break;
		default:
		if (len1&1) len1++;
		if (seek(h,len1,OFFSET_CURRENT)<0) goto serr;
		ifflen-=len1;
		break;
	}
}	
if (!fbmhd) goto scorrupt;
if(fmodeid&(!(modeid&MONITOR_ID_MASK))||((modeid&EXTENDED_MODE)&&(!(modeid&0xFFFF0000)))) modeid&=(~(EXTENDED_MODE|SPRITES|GENLOCK_AUDIO|GENLOCK_VIDEO|VP_HIDE));
if(fmodeid&(modeid&0xFFFF0000)&&(!(modeid&0x00001000))) fmodeid=0;
if(!fmodeid) {
	modeid=0;
	if (wide>=640) modeid|=HIRES;
	if (high>=400) modeid|=LACE;
	if (deep==6) modeid|=HAM;
}
pictureinfo(h,wide,high,deep,modeid,"ILBM");

if (!openshowscreen(list,wide,high,sdeep,modeid,wide,high,sdeep,&shows)) goto serr;

if (fcmap) {
#ifdef V39
if (!(bmhd.Flags&BMHDF_CMAPOK)) {
	apu2=0; for(apu1=0;apu1<cmaplen;apu1++) { if (cmap[apu1]&0x0f) apu2=1; }
	if (!apu2) {
		for(apu1=0;apu1<cmaplen;apu1++) cmap[apu1]|=cmap[apu1]>>4;
	}
}
if (!(cmapapu1=allocvec(list,3*cmaplen*4+8,MEMF_CLEAR))) goto serr;
cmapapu1[0]=(cmaplen/3)<<16;
for(apu1=0;apu1<cmaplen;apu1++) {
	apu3=cmap[apu1];
	cmapapu1[apu1+1]=(apu3<<24)|(apu3<<16)|(apu3<<8)|apu3;
}
LoadRGB32(&shows.screen->ViewPort,cmapapu1);
freemem(cmapapu1);
#else
ptr1=cmap;
for(apu1=0;apu1<cmaplen/3;apu1++) {
	SetRGB4(&shows.screen->ViewPort,apu1,ptr1[0]>>4,ptr1[1]>>4,ptr1[2]>>4);
	ptr1+=3;
}
#endif
}

if(!(retcode=decode(h,&shows,&bmhd,ifflen))) {
	SetPointer(shows.window,shows.mouse,2,16,0,0);
	retcode=waitbutton(shows.window);
}
retcode++;
goto serr;

scorrupt:
requestmsg(list->workname,0,MSG_OK,MSG_SHOWPIC_CORRUPT,h->filename,"ILBM");
serr:
closeshowscreen(&shows);
freemem(cmap);
return(retcode);
}

WORD decode(struct FMHandle *h,struct ShowScreen *shows,struct FMBitMapHeader *bmhd,LONG ifflen)
{
LONG bufsize,nbufsize;
UBYTE *bufptr,*bufend;
WORD apu1,ret=0;
UBYTE *ptr1;
struct BitMap *bm;
/*struct BitMap bm2;*/
struct Window *win;

win=shows->window;
bm=shows->tempbm;

bufsize=h->bufsize; bufend=h->buffer+bufsize;
if (!(nbufsize=rbytes(h,h->buffer,bufsize,&ifflen))) goto decerr;
apu1=0;
for(;;) {
	bufptr=h->buffer;
	if (bmhd->compression==1)
		unpack(&bufptr,bm,&apu1,bmhd->h,nbufsize,((bmhd->w+15)>>4)<<1,bmhd->masking,shows);
		else
		unpacknp(&bufptr,bm,&apu1,bmhd->h,nbufsize,((bmhd->w+15)>>4)<<1,bmhd->masking,shows);
	if (apu1==bmhd->h) break;
	ret=testbutton(win);
	if(ret) break;
	ptr1=h->buffer;
	while(bufptr!=bufend) *ptr1++=*bufptr++;
	if (!(rbytes(h,ptr1,bufend-ptr1,&ifflen))) goto decerr;
}
decerr:
return(ret);
}

LONG rbytes(struct FMHandle *h,UBYTE *bptr,LONG bsize,LONG *ifflen)
{
LONG len;

if (!*ifflen) return(0);
len=*ifflen;
if (len>bsize) len=bsize;
if (readbufferfile(h,bptr,len)!=len) return(0);
*ifflen-=len;
return(len);
}


void clipit(WORD wide,WORD high,
	struct Rectangle *spos,struct Rectangle *dclip,
	struct Rectangle *txto,struct Rectangle *stdo,
	struct Rectangle *maxo)
{
struct  Rectangle *besto;
WORD minx,maxx,miny,maxy;
WORD txtw,txth,stdw,stdh,bestw,besth;

txtw=txto->MaxX-txto->MinX+1;
txth=txto->MaxY-txto->MinY+1;
stdw=stdo->MaxX-stdo->MinX+1;
stdh=stdo->MaxY-stdo->MinY+1;

if((wide<=txtw)&&(high<=txth)) {
	besto=txto;
	bestw=txtw;
	besth=txth;
} else {
	besto=stdo;
	bestw=stdw;
	besth=stdh;
}
spos->MinX=minx=besto->MinX-((wide-bestw)>>1);
maxx=wide+minx-1;
if(maxx>maxo->MaxX) maxx=maxo->MaxX;
if(minx<maxo->MinX) minx=maxo->MinX;
spos->MinY=miny=besto->MinY-((high-besth)>>1);
spos->MinY=miny=MIN(spos->MinY,txto->MinY);
maxy=high+miny-1;
if(maxy>maxo->MaxY) maxy=maxo->MaxY;
if(miny<maxo->MinY) miny=maxo->MinY;
dclip->MinX=minx;
dclip->MinY=miny;
dclip->MaxX=maxx;
dclip->MaxY=maxy;
}

ULONG power2(WORD exp)
{
ULONG ret=1;
while(exp--) ret<<=1;
return(ret);
}

#define MODE_ID_MASK (LACE|HIRES|HAM|EXTRA_HALFBRITE)
ULONG modefallback(ULONG oldmode,WORD wide,WORD high,WORD deep)
{
ULONG newmode=0,bestmode;
struct TagItem tags[6];

if(fmmain.kick>=39) {
	tags[0].ti_Tag = BIDTAG_DIPFMustHave;
	tags[0].ti_Data = (oldmode & HAM ? DIPF_IS_HAM : 0);
	tags[0].ti_Data |= (oldmode & EXTRA_HALFBRITE ? DIPF_IS_EXTRAHALFBRITE : 0);
	tags[1].ti_Tag = BIDTAG_NominalWidth;
	tags[1].ti_Data = wide;
	tags[2].ti_Tag = BIDTAG_NominalHeight;
	tags[2].ti_Data = high;
	tags[3].ti_Tag = BIDTAG_Depth;
	tags[3].ti_Data = deep;
	tags[4].ti_Tag = BIDTAG_SourceID;
	tags[4].ti_Data = (FindDisplayInfo(oldmode) ? oldmode : (oldmode & (~(MONITOR_ID_MASK | SPRITES|GENLOCK_AUDIO|GENLOCK_VIDEO|VP_HIDE))));
	tags[5].ti_Tag = TAG_DONE;
	if((bestmode = BestModeIDA(tags)) != INVALID_ID) newmode = bestmode;
} else {
    	newmode = oldmode & MODE_ID_MASK;
}
if(ModeNotAvailable(newmode)) {
	newmode=0;
	if (wide>=640) newmode|=HIRES;
	if (high>=400) newmode|=LACE;
	if (deep==6) newmode|=HAM;
}
return(newmode);
}

WORD openshowscreen(struct FMList *list,WORD wide,WORD high,WORD deep,ULONG modeid,WORD realw,WORD realh,WORD reald,struct ShowScreen *shows)
{
struct Rectangle spos, dclip, txto, stdo, maxo;
WORD oscan,apu2;
IPTR scrtags[24];
#ifndef V39
WORD apu1;
#endif

memseti(scrtags,0,24*4);
shows->gfxbase=GfxBase;shows->tempbm=0;
shows->screen=0; shows->window=0; shows->bitmap=0; shows->mouse=0;
shows->mouse=allocvec(list,32,MEMF_CHIP|MEMF_CLEAR);
if(!shows->mouse) goto serr;
#ifdef V39
if(!(shows->bitmap=AllocBitMap(realw,realh,reald,BMF_CLEAR|BMF_DISPLAYABLE|BMF_INTERLEAVED,0))) {
	if(!(shows->bitmap=AllocBitMap(realw,realh,reald,BMF_CLEAR|BMF_DISPLAYABLE,0))) {
		requestmsg(list->workname,0,MSG_OK,MSG_MAIN_ERRNOBITMAP);
		goto serr;
	}
}
#else
if(!(shows->bitmap=allocmem(sizeof(struct BitMap)))) goto serr;
InitBitMap(shows->bitmap,reald,realw,realh);
apu2=1;
for(apu1=0;apu1<reald;apu1++) {
	if(!(shows->bitmap->Planes[apu1]=allocvec(0,RASSIZE(realw,realh),MEMF_CHIP|MEMF_CLEAR))) apu2=0;
}
if(!apu2) {
	requestmsg(list->workname,0,MSG_OK,MSG_MAIN_ERRNOBITMAP);
	goto serr;
}
#endif
if(ModeNotAvailable(modeid)) modeid=modefallback(modeid,wide,high,deep);

spos.MinX=0; spos.MinY=0;
oscan=((QueryOverscan(modeid,&txto,OSCAN_TEXT))&&(QueryOverscan(modeid,&stdo,OSCAN_STANDARD))&&(QueryOverscan(modeid,&maxo,OSCAN_MAX)));

scrtags[0]=SA_Left; scrtags[1]=spos.MinX;
scrtags[2]=SA_Top; scrtags[3]=spos.MinY;
scrtags[4]=SA_Width; scrtags[5]=wide;
scrtags[6]=SA_Height; scrtags[7]=high;
scrtags[8]=SA_Depth;if(fmmain.kick<39&&deep>6) scrtags[9]=6; else scrtags[9]=deep;
scrtags[10]=SA_Type; scrtags[11]=CUSTOMSCREEN;
scrtags[12]=SA_BitMap; scrtags[13]=(IPTR)(shows->bitmap);
scrtags[14]=SA_DisplayID; scrtags[15]=modeid;
scrtags[16]=SA_Quiet; scrtags[17]=1;
scrtags[18]=SA_AutoScroll; scrtags[19]=1;
scrtags[20]=SA_BackFill; scrtags[21]=(IPTR)LAYERS_NOBACKFILL;
if (oscan) {
	clipit(wide,high,&spos,&dclip,&txto,&stdo,&maxo);
	scrtags[22]=SA_DClip; scrtags[23]=(IPTR)&dclip;
}

if (!(shows->screen=OpenScreenTagList(0,(struct TagItem*)scrtags))) {
	requestmsg(list->workname,0,MSG_OK,MSG_MAIN_SCRERR1);
	goto serr;
}
if (!(shows->window=OpenWindowTags(0,
	WA_Left,0,WA_Top,0,WA_Width,wide,WA_Height,high,
	WA_IDCMP,IDCMP_MOUSEBUTTONS|IDCMP_ACTIVEWINDOW|IDCMP_VANILLAKEY,
	WA_Flags,WFLG_BACKDROP|WFLG_BORDERLESS|WFLG_ACTIVATE|WFLG_RMBTRAP|WFLG_NOCAREREFRESH,
	WA_BackFill,LAYERS_NOBACKFILL,WA_SimpleRefresh,TRUE,
	WA_CustomScreen,shows->screen,WA_BusyPointer,TRUE,
	TAG_DONE))) {
	requestmsg(list->workname,0,MSG_OK,MSG_MAIN_WINERR1);
	goto serr;
}

apu2=1;
#ifndef V39
shows->tempbm=allocmem(sizeof(struct BitMap));
if(shows->tempbm) {
	InitBitMap(shows->tempbm,reald,realw,1);
	for(apu1=0;apu1<reald;apu1++) {
		if(!(shows->tempbm->Planes[apu1]=allocvec(0,shows->tempbm->BytesPerRow,MEMF_CHIP|MEMF_CLEAR))) apu2=0;
	}
}
#else
shows->tempbm=AllocBitMap(realw,1,reald,BMF_CLEAR,0);
#endif
CopyMem(&shows->screen->RastPort,&shows->temprp,sizeof(struct RastPort));
shows->temprp.Layer=0;shows->temprp.BitMap=shows->tempbm;
if(!shows->tempbm||!apu2) {
	requestmsg(list->workname,0,MSG_OK,MSG_MAIN_ERRNOBITMAP);
	goto serr;
}
return(1);
serr:
closeshowscreen(shows);
return(0);
}

void closeshowscreen(struct ShowScreen *shows)
{
#ifndef V39
WORD apu1;
#endif

if(shows->window) {
	ClearPointer(shows->window);
	CloseWindow(shows->window);
}
freemem(shows->mouse);
if(shows->screen) CloseScreen(shows->screen);
if(shows->bitmap) {
	#ifdef V39
	FreeBitMap(shows->bitmap);
	#else
	for(apu1=0;apu1<8;apu1++) freemem(shows->bitmap->Planes[apu1]);
	freemem(shows->bitmap);	
	#endif
}
if(shows->tempbm) {
	#ifndef V39
	for(apu1=0;apu1<8;apu1++) freemem(shows->tempbm->Planes[apu1]);
	freemem(shows->tempbm);
	#else
	FreeBitMap(shows->tempbm);
	#endif
}
shows->screen=0; shows->window=0; shows->bitmap=0; shows->mouse=0;
shows->tempbm=0;
}

WORD testbutton(struct Window *win)
{
struct IntuiMessage *im;
WORD ret=0;

while((im=(struct IntuiMessage*)GetMsg(win->UserPort))) {
	if (im->Class==IDCMP_MOUSEBUTTONS) {
		if(im->Code==SELECTDOWN) ret=1;
		if(im->Code==MENUDOWN) ret=-1;
	}
	if (im->Class==IDCMP_VANILLAKEY) {
		if(im->Code==13) ret=1;
		if(im->Code==27) ret=-1;
	}
	ReplyMsg((struct Message*)im);
}	
return(ret);
}
WORD waitbutton(struct Window *window)
{
ULONG class;
UWORD code;
WORD ignorebutton=0,started=1;
WORD ret=0;

for(;;) {
	if(smallevent(window,&class,&code)) {
		ret=-1;
		break;
	}
	if(ignorebutton) ignorebutton--;
	if(class==IDCMP_ACTIVEWINDOW&&!started) ignorebutton=2; else started=0;
	if(class==IDCMP_MOUSEBUTTONS&&!ignorebutton) {
		if(code==SELECTDOWN) {
			ret=1;
			break;
		}
		if(code==MENUDOWN) {
			ret=-1;
			break;
		}
	}
	if(class==IDCMP_VANILLAKEY) {
		if(code==0x0d) {
			ret=1;
			break;
		}
		if(code==27) {
			ret=-1;
			break;
		}
	}
}
return(ret);
}

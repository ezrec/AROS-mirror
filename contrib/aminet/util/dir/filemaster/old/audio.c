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
#include <dos/dos.h>
#include <graphics/gfxbase.h>
#include <datatypes/datatypes.h>
#include <datatypes/datatypesclass.h>
#include <datatypes/soundclass.h>
#include <devices/audio.h>  
#include <proto/all.h>
#include <proto/datatypes.h>
#include <stdio.h>
#include <string.h>
#include "fmnode.h"
#include "child.h"
#include "fmdos.h"
#include "fmlocale.h"
#include "fmgui.h"
#include "audio.h"

extern struct ExecBase *SysBase;
void stopoldmod(void);
struct IOAudio* allocaudio(struct FMList*);
void modinfotxt(struct FMList*);
WORD checkoldaudio(struct FMList*);

extern struct FMMain fmmain;
extern struct FMConfig *fmconfig;
extern UBYTE auddev[];

APTR __asm startptplay(register __a0 UBYTE**);
void __asm stopptplay(register __a0 APTR);

WORD modplay(struct FMHandle*,struct CMenuConfig*);

void __saveds playmod(void)
{
struct ProcMsg *pm;
struct FMList *slist;
struct FMNode *node;
struct FMHandle *h;

pm=sinitproc();
priority(pm->cmc);
slist=fmmain.sourcedir;
if (!(setalloc(slist,1))) {
	initproc(0,0);
	goto endi;
}
initproc(slist,pm->cmc->label);
priority(pm->cmc);
if(sselected(slist,1)) {
	if ((node=findselnode(slist))) {
		if (h=openfile(slist,NDFILE(node),OFNORMAL|OFDECRUNCH)) {
			modplay(h,pm->cmc);
			closefile(h);
		}
		node->flags&=~NSELECTED;
		outputlistline(slist,node);
	}
}
endproc(slist);
endi:
deinitproc(pm);
}

void stopoldmod(void)
{
WORD cnt;
if (fmmain.modflag==MODPLAY) {
	fmmain.modflag=0;
	if (fmmain.modptr) stopptplay(fmmain.modptr);	
	fmmain.modptr=0;
	for(cnt=0;cnt<33;cnt++) {
		freemem(fmmain.starts[cnt]);
		fmmain.starts[cnt]=0;
	}
	if(fmmain.audio) {
		closedevice((struct IORequest*)fmmain.audio);
		fmmain.audio=0;
	}
}
}

struct Sample {
	UBYTE name[22];
	UWORD length;
	UBYTE finetune;
	UBYTE volume;
	UWORD repeat;
	UWORD replen;
};
struct MK {
	UBYTE name[20];
	struct Sample sample[31];
	UBYTE songlength;
	UBYTE none;
	UBYTE positions[128];
	ULONG mk;
};

WORD modplay(struct FMHandle *h,struct CMenuConfig *cmc)
{
UBYTE tpats[1084];
UBYTE *tpatptr,*ptr1;
LONG patts=1084,cnt,samplelen,totsamples,maxpat=0;
WORD smpcnt=0,checked=0;
struct MK *mk;
struct FMList *list;

list=h->owner;
stopoldmod();
changename(list,cmc->label);
priority(cmc);
sformatmsg(list->fmmessage1,MSG_FILECLICK_MODULE,h->filename);
fmmessage(list);
if(checkoldaudio(list)) return(0);
tpatptr=tpats;
list->flags|=LUPDATEMSG;
if (readbufferfile(h,tpatptr,patts)==patts) {
	ptr1=tpatptr+952;
	while(ptr1!=(tpatptr+1080)) {
		if (*ptr1>maxpat) maxpat=*ptr1;
		ptr1++;
	}
	maxpat++;
	maxpat<<=10;
	fmmain.starts[0]=allocvec(list,patts+maxpat,MEMF_PUBLIC);
	fmmain.starts[1]=allocvec(list,4,MEMF_CHIP|MEMF_CLEAR|MEMF_PUBLIC);
	if(fmmain.starts[0]&&fmmain.starts[1]) {
		ptr1=tpatptr;
		for(cnt=0;cnt<patts;cnt++) *(fmmain.starts[0]+cnt)=*ptr1++;
		sformatmsg(list->fmmessage2,MSG_AUDIO_MODINFOTXT1,(LONG)maxpat>>10);
		if (readbufferfile(h,fmmain.starts[0]+patts,maxpat)==maxpat) {
			mk=(struct MK*)fmmain.starts[0];
			totsamples=0;
			for(cnt=2;cnt<33;cnt++) {
				samplelen=((LONG)(mk->sample[cnt-2].length))<<1;
				sformatmsg(list->fmmessage2,MSG_AUDIO_MODINFOTXT2,(LONG)(cnt-2),samplelen,(WORD)(totsamples*100/(h->size-(maxpat+patts))));
				if(testabort(list)&&askabort(list)) goto merr;
				totsamples+=samplelen;
				if (samplelen) {
					smpcnt++;
					if(!(fmmain.starts[cnt]=allocvec(list,samplelen,MEMF_CHIP|MEMF_CLEAR|MEMF_PUBLIC))) goto merr;
					if(h->size-h->position<samplelen) {
						if(checked) {
							samplelen=-1;
						} else {
							samplelen=h->size-h->position;
							checked=1;
						}
					}
					if(readbufferfile(h,fmmain.starts[cnt],samplelen)!=samplelen) goto merr;
				}
			}
			goto modok;
		}
	}
}
merr:
for (cnt=0;cnt<33;cnt++) {
	freemem(fmmain.starts[cnt]);
	fmmain.starts[cnt]=0;
}
return(0);
modok:
list->flags&=~LUPDATEMSG;
fmmain.modflag=MODPLAY;
sformatmsg(list->fmmessage1,MSG_AUDIO_MODINFOTXT3,(LONG)(maxpat>>10),(LONG)*(fmmain.starts[0]+950),(LONG)smpcnt,totsamples);
list->fmmessage2[0]=0;
if (!(fmmain.audio=allocaudio(list))) {
	stopoldmod();
} else if (!(fmmain.modptr=startptplay(fmmain.starts))) {
	strcpy(list->fmmessage1,getstring(MSG_AUDIO_NOCIA));
	stopoldmod();
}
fmmessage(list);
return(1);
}

void __saveds modinfo(void)
{
struct ProcMsg *pm;
struct FMList *slist;
register struct FMNode *node;
struct FMHandle *h;
WORD ret=1;

pm=sinitproc();
priority(pm->cmc);
slist=fmmain.sourcedir;
if (!(setalloc(slist,1))) {
	initproc(0,0);
	goto endi;
}
initproc(slist,pm->cmc->label);
priority(pm->cmc);
if(fmmain.modflag==MODPLAY&&findselnode(slist)) {
	ret=requestmsg(pm->cmc->label,MSG_YES,MSG_NO,MSG_AUDIO_LOAD);
}
if (ret&&(node=findselnode(slist))) {
	if (h=openfile(slist,NDFILE(node),OFNORMAL|OFDECRUNCH)) {
		modplay(h,pm->cmc);
	}
	node->flags&=~NSELECTED;
	closefile(h);
}
if(fmmain.modflag==MODPLAY) {
	modinfotxt(slist);
} else {
	strcpymsg(slist->fmmessage1,MSG_MAIN_NOSOURCEFILE);
	fmmessage(slist);
}
outputlist(slist);
endproc(slist);
endi:
deinitproc(pm);
}

WORD showtext(struct FMHandle*,WORD);

void modinfotxt(struct FMList *l)
{
WORD cnt,cnt2;
UBYTE *txt,*txt2;
UBYTE *ptr,*ptr2;
LONG len,rep,replen;
WORD fine,vol;
struct FMHandle *h;
UBYTE name[21];
UBYTE sample[23];

ptr2=name;
for(cnt2=0;cnt2<20;cnt2++) {
	if(*(fmmain.starts[0]+cnt2)>=32) *ptr2++=*(fmmain.starts[0]+cnt2);
}
*ptr2=0;
if(!(txt2=allocvec(l,3000,0))) return;
ptr=fmmain.starts[0]+20; txt=txt2;
strcpy(txt,getstring(MSG_AUDIO_MODINFO1));
txt+=strlen(txt); *txt++='\n';
for(cnt=0;cnt<31;cnt++) {
	len=((LONG)(*((UWORD*)(ptr+22))))<<1;
	rep=((LONG)(*((UWORD*)(ptr+26))))<<1;
	replen=((LONG)(*((UWORD*)(ptr+28))))<<1;
	fine=ptr[24]&0x0f;
	if(fine>=8) fine-=16;
	vol=ptr[25];
	ptr2=sample;
	for(cnt2=0;cnt2<22;cnt2++) {
		if(ptr[cnt2]>=32) *ptr2++=ptr[cnt2];
	}
	*ptr2=0;
	sformatmsg(txt,MSG_AUDIO_MODINFO2,cnt,sample,len,fine,vol,rep,replen);
	txt+=strlen(txt); *txt++='\n';
	ptr+=30;
}
*txt++=0;
if (h=openfile(l,name,OFFAKE|OFNORMAL)) {
	h->decbuffer=txt2;
	h->size=txt-txt2-1;
	showtext(h,1);
	closefile(h);
}
freemem(txt2);
}

WORD playsample(struct FMHandle*,struct CMenuConfig*);

void __saveds playsamplefile(void)
{
struct ProcMsg *pm;
struct FMList *slist;
register struct FMNode *node;
struct FMHandle *h;

pm=sinitproc();
slist=fmmain.sourcedir;
if (!(setalloc(slist,1))) {
	initproc(slist,0);
	goto endi;
}
initproc(slist,pm->cmc->label);
priority(pm->cmc);
if(sselected(slist,1)) {
	if ((node=findselnode(slist))) {
		if (h=openfile(slist,NDFILE(node),OFNORMAL|OFDECRUNCH)) {
			if(!isdatatypes(h,GID_SOUND)) playsample(h,pm->cmc);
		}
		node->flags&=~NSELECTED;
		outputlistline(slist,node);
		closefile(h);
	}
}
endproc(slist);
endi:
deinitproc(pm);
}
void __asm initaudio(register __a0 UBYTE*,register __d0 LONG,register __d1 WORD);
void __asm endaudio(void);
void __asm waitaudio(register __a0 struct DosLibrary*);
void __asm startaudio(register __a0 UBYTE*,register __d0 LONG);
struct AudioConfig {
	LONG	smemlen;
	UWORD	hz;
	unsigned type:2; //0=ask,1=disk,2=mem
	unsigned askhz:2;
};

WORD playdatatypesample(struct FMHandle *h,struct DataType *dt,struct CMenuConfig *cmc)
{
Object *obj;
LONG type;
UBYTE *src;
LONG slen,secs;
struct VoiceHeader *vhdr;
struct timerequest *str;
struct DataTypeHeader *dth;
WORD ret=0;

changename(h->owner,cmc->label);
priority(cmc);
dth=dt->dtn_Header;
sformatmsg(h->owner->fmmessage1,MSG_FILECLICK_SOUND,dth->dth_Name,h->filename);
fmmessage(h->owner);

if(h->flags&OFDECRUNCH) {
	type=DTST_RAM;
	src=h->decbuffer;
} else {
	type=DTST_FILE;
	src=h->filename;
}
if(!(obj=NewDTObject(src,DTA_SourceType,type,DTA_GroupID,GID_SOUND,SDTA_Volume,64L,SDTA_Cycles,1L,TAG_DONE))) goto error;
if(GetDTAttrs(obj,SDTA_SampleLength,&slen,SDTA_VoiceHeader,&vhdr,TAG_DONE)!=2) goto error;
if(!slen||!vhdr) goto error;
sformatmsg(h->owner->fmmessage1,MSG_AUDIO_INFO1,dth->dth_Name,h->filename,slen,(LONG)vhdr->vh_SamplesPerSec);
fmmessage(h->owner);

str=(struct timerequest*)opendevice(h->owner,"timer.device",UNIT_VBLANK,0,sizeof(struct timerequest));
if(!str) goto error;
str->tr_node.io_Command=TR_ADDREQUEST;
secs=slen/vhdr->vh_SamplesPerSec;
str->tr_time.tv_micro=(1000000/vhdr->vh_SamplesPerSec)*(slen%vhdr->vh_SamplesPerSec);
SendIO((struct IORequest*)str);
DoMethod(obj,DTM_TRIGGER,0,STM_PLAY,0);
WaitIO((struct IORequest*)str);
closedevice((struct IORequest*)str);

error:
if(obj) DisposeDTObject(obj);
return(ret);
}

WORD playsample(struct FMHandle *h,struct CMenuConfig *cmc)
{
struct GUIBase *gb;
struct AudioConfig *sconfig;
ULONG longi1,longi2,ifflen,len1,clock;
struct VoiceHeader vhdr;
WORD ret=0,fvhdr=0,iff=0;
WORD period;
UBYTE *varabuf=0;
UBYTE *smem1=0,*smem2=0,*chippi=0,*vara;
LONG readlen,smemlen,playmem,totlen;
LONG value;
UBYTE ptype;

extern struct GfxBase *GfxBase;
extern struct DosLibrary *DOSBase;

changename(h->owner,cmc->label);
priority(cmc);
if(!(sconfig=getconfig(cmc))) return(0);
sconfig->smemlen&=~1;
sformatmsg(h->owner->fmmessage1,MSG_FILECLICK_SOUND,"8SVX",h->filename);
fmmessage(h->owner);
stopoldmod();
if(checkoldaudio(h->owner)) return(0);
smemlen=sconfig->smemlen;
if(readbufferfile(h,&longi1,4)!=4) goto perr;
if(readbufferfile(h,&ifflen,4)!=4) goto perr;
if(readbufferfile(h,&longi2,4)!=4) goto perr;
ifflen-=4;
if(longi1=='FORM'&&longi2=='8SVX') iff=1;
vhdr.vh_SamplesPerSec=sconfig->hz;
if (iff) {
	if(ifflen>h->size-8) goto pcorrupt;
	for(;;) {
		if (readbufferfile(h,&longi1,4)!=4) goto perr;
		if (readbufferfile(h,&len1,4)!=4) goto perr;
		ifflen-=8;
		if (longi1=='BODY') break;
		switch(longi1)
		{
			case 'VHDR':
			if(len1!=sizeof(struct VoiceHeader)) goto pcorrupt;
			if(readbufferfile(h,&vhdr,len1)!=len1) goto perr;
			ifflen-=len1;
			fvhdr=1;
			break;
			default:
			if (len1&1) len1++;
			if (seek(h,len1,OFFSET_CURRENT)<0) goto perr;
			ifflen-=len1;
			break;
		}
	}		
	if(!fvhdr) goto pcorrupt;
} else {
	ifflen=h->size;
	if(seek(h,0,OFFSET_BEGINNING)<0) goto perr;
}
if(!iff||sconfig->askhz) {
	gb=getguibase(cmc->label);
	setconnectgroup(gb,1,0,0);
	reqinfomsg(gb,MSG_AUDIO_ASKFREQUENCY,100,guiUC|guiLEFT);
	reqinteger(gb,100,&value,4000,64000);
	buttonbarmsg(gb,MSG_OK,1,MSG_CANCEL,0,0);
	if(!quickreq(gb)) goto perr;
	vhdr.vh_SamplesPerSec=value;
}

#ifdef V39
if(GfxBase->DisplayFlags&REALLY_PAL) clock=3546895; else clock=3579545;
#else
if(GfxBase->DisplayFlags&PAL) clock=3546895; else clock=3579545;
#endif
sformatmsg(h->owner->fmmessage1,MSG_AUDIO_INFO1,"8SVX",h->filename,h->size,(LONG)vhdr.vh_SamplesPerSec);
fmmessage(h->owner);
period=clock/vhdr.vh_SamplesPerSec;
ptype=sconfig->type;
if(!ptype) {
	ptype=1;
	if(!requestmsg(getstring(MSG_MAIN_REQUEST),MSG_AUDIO_TYPEANSWER1,MSG_AUDIO_TYPEANSWER2,MSG_AUDIO_ASKTYPE)) ptype=2;
}
if(ptype==2)
	smem1=allocvec(h->owner,h->size,MEMF_CHIP);
	else
	smem1=allocvec(h->owner,smemlen,MEMF_CHIP);
if(!smem1) goto perr;
if(ptype==1) {
	smem2=allocvec(h->owner,smemlen,MEMF_CHIP);
	if(!smem2) goto perr;
}
if(!(fmmain.audio=allocaudio(h->owner))) goto perr;
initaudio(chippi,4,period);
playmem=0; totlen=ifflen;
h->owner->flags|=LUPDATEMSG;
if(ptype==1) {
	while(ifflen>0) {
		waitaudio(DOSBase);
		vara=smem1;smem1=smem2;smem2=vara;
		if(smemlen>ifflen) readlen=ifflen; else readlen=smemlen;
		if(readbufferfile(h,smem1,readlen)!=readlen) goto perr;
		ifflen-=readlen;
		playmem+=readlen;
		sformatmsg(h->owner->fmmessage2,MSG_AUDIO_INFO2,playmem,totlen,(WORD)(playmem*100/totlen));
		if(testabort(h->owner)) {
			endaudio();
			if(askabort(h->owner)) {
				ret=1;
				break;
			}
			initaudio(chippi,4,period);
		}
		startaudio(smem1,readlen);
		WaitTOF();
		WaitTOF();
	}
} else {
	if(readbufferfile(h,smem1,ifflen)!=ifflen) goto perr;
	while(ifflen>0) {
		if(smemlen>ifflen) readlen=ifflen; else readlen=smemlen;
		sformatmsg(h->owner->fmmessage2,MSG_AUDIO_INFO2,playmem,totlen,(WORD)(playmem*100/totlen));
		fmmessage(h->owner);
		waitaudio(DOSBase);
		ifflen-=readlen;
		if(testabort(h->owner)) {
			endaudio();
			if(askabort(h->owner)) {
				ret=1;
				break;
			}
			initaudio(chippi,4,period);
		}
		startaudio(smem1+playmem,readlen);
		WaitTOF();
		WaitTOF();
		playmem+=readlen;
	}
}
h->owner->flags&=~LUPDATEMSG;
if(ret)	{
	initaudio(chippi,4,period);
} else {
	sformatmsg(h->owner->fmmessage2,MSG_AUDIO_INFO2,playmem,totlen,(WORD)(playmem*100/totlen));
	fmmessage(h->owner);
}
waitaudio(DOSBase);
startaudio(chippi,4);
waitaudio(DOSBase);
endaudio();
ret=1;
goto perr;

pcorrupt:
requestmsg(h->owner->workname,0,MSG_OK,MSG_AUDIO_CORRUPT8SVX,h->filename);
perr:
endaudio();
freemem(smem1);
freemem(smem2);
freemem(chippi);
freemem(varabuf);
if(fmmain.audio) {
	closedevice((struct IORequest*)fmmain.audio);
	fmmain.audio=0;
}
return(ret);
}



struct IOAudio* allocaudio(struct FMList *list)
{
struct MsgPort *mp;
struct IOAudio *io;
UBYTE whichchan;

whichchan=15;
if(mp=CreateMsgPort()) {
	if(io=(struct IOAudio*)CreateIORequest(mp,sizeof(struct IOAudio))) {
		io->ioa_Request.io_Message.mn_Node.ln_Pri=127;
		io->ioa_Request.io_Command=ADCMD_ALLOCATE;
		io->ioa_Request.io_Flags=ADIOF_NOWAIT;
		io->ioa_AllocKey=0;
		io->ioa_Data=&whichchan;
		io->ioa_Length=1;
		if(!OpenDevice(auddev,0,(struct IORequest*)io,0)) return(io);
		DeleteIORequest((struct IOAudio*)io);
	}
	DeleteMsgPort(mp);
}
requestmsg(list->workname,0,MSG_OK,MSG_FMDOS_OPENERR,auddev);
return(0);
}

WORD checkoldaudio(struct FMList *l)
{
if(fmmain.audio) {
	requestmsg(l->workname,0,MSG_OK,MSG_AUDIO_NOAUDIO);
	return(1);
}
return(0);
}

void *hearconfigdefault(struct CMenuConfig *cmc)
{
struct AudioConfig *config;
WORD ret;

ret=allocconfig(cmc,sizeof(struct AudioConfig));
if(ret<0) return(cmc->moreconfig);
if(!ret) return(0);
config=(struct AudioConfig*)cmc->moreconfig;
config->hz=18000;
config->smemlen=40000;
return(cmc->moreconfig);
}

WORD hearconfig(struct GUIBase *gb,struct CMenuConfig *cmc)
{
WORD c;
struct AudioConfig *ac;
WORD askhz,playmethod;
LONG freq,buf;

ac=getconfig(cmc);
askhz=ac->askhz;
setguigroup(gb,1,0);
reqinfomsg(gb,MSG_AUDIO_CONFIG_ASKHZ,200,guiUC|guiLEFT);
reqinfomsg(gb,MSG_AUDIO_CONFIG_PLAYMETHOD,201,guiUC|guiLEFT);
reqinfomsg(gb,MSG_AUDIO_CONFIG_FREQUENCY,202,guiUC|guiLEFT);
reqinfomsg(gb,MSG_AUDIO_CONFIG_BUFFER,203,guiUC|guiLEFT);
setguigroup(gb,2,0);
reqcycle2msg(gb,200,&askhz);
playmethod=ac->type;
reqcyclemsg(gb,201,&playmethod,MSG_AUDIO_CONFIG_PLAYMETHOD1,MSG_AUDIO_CONFIG_PLAYMETHOD2,MSG_AUDIO_CONFIG_PLAYMETHOD3,0);
freq=ac->hz;
reqinteger(gb,202,&freq,4000,64000);
buf=ac->smemlen;
reqinteger(gb,203,&buf,4000,99999999);
commandanswer(gb);
c=quickreq(gb);
if(c) {
	ac->type=playmethod;
	ac->askhz=askhz;
	ac->smemlen=buf;
	ac->hz=freq;
}
return(c);
}

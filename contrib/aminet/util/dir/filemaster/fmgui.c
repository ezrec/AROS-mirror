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

#include <exec/memory.h>

#include <stdio.h>
#include <string.h>
#include <intuition/gadgetclass.h>
#include <intuition/iobsolete.h>
#include "fmgui.h"
#include "fmlocale.h"
#include "fmnode.h"

extern struct UtilityBase *UtilityBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;

/* #define GUIDEBUG */

extern struct FMConfig *fmconfig;
extern struct FMMain fmmain;
extern struct UtilityBase *UtilityBase;  

struct GUISlot *getguislotonly(struct GUIBase *gb)
{
struct GUIGroup *gg;
struct GUISlot *gs;

if(!gb) return(0);
gg=gb->groups[gb->currentgroup];
if(!gg) return(0);
gs=&gg->slots[gg->used];
gg->used++;
return(gs);
}

struct GUISlot *getguislot(struct GUIBase *gb,Object *obj,WORD id,WORD type,void *data)
{
struct GUISlot *gs;
gs=getguislotonly(gb);
if(gs) {
	gs->obj=obj;
	gs->id=id;
	gs->type=type;
	gs->data=data;
}
return(gs);
}

static void guimulticonvert2(void **text,IPTR *msgs)
{
void **txtptr;
txtptr=text;
while(*msgs) {
	*txtptr++=getstring(*msgs++);
	*txtptr++=(UBYTE*)*msgs++;
}
*txtptr=0;
}

static void guimulticonvert(UBYTE **text,IPTR *msgs)
{
UBYTE **txtptr;
txtptr=text;
while(*msgs) {
	*txtptr++=getstring(*msgs++);
}
*txtptr=0;
}

/* INFO */

static UBYTE infocen[]={"\33c"};
static UBYTE infoleft[]={"\33l"};
static UBYTE infouc1[]={"\33z"};
static UBYTE infouc2[]={"\33-z"};

static struct GUISlot *reqinfo2(struct GUIBase *gb,UBYTE *txt,WORD id,UWORD flags)
{
WORD numlines;
UBYTE *ptr1,*mem;
struct GUISlot *gs;
UBYTE sh;

#ifdef GUIDEBUG
printf("INFO:\n");
#endif

sh=0;
numlines=1;
ptr1=txt;
while(*ptr1) {
	if(*ptr1=='\n') numlines++;
	ptr1++;
}
mem=allocmem(strlen(txt)+16);
if(!mem) return(0);
ptr1=mem;
*ptr1=0;
if(flags&guiLEFT) strcpy(ptr1,infoleft);
ptr1+=strlen(ptr1);
if(flags&guiCENTER) strcpy(ptr1,infocen);
ptr1+=strlen(ptr1);
if(flags&guiUC) {
	while(*txt) {
		if(*txt=='_') {
			sh=ToUpper(txt[1]);
			strcpy(ptr1,infouc1);
			ptr1+=strlen(ptr1);
			*ptr1++=txt[1];
			strcpy(ptr1,infouc2);
			ptr1+=strlen(ptr1);
			txt+=2;
		} else *ptr1++=*txt++;
	}
	*ptr1=0;
} else {
	strcpy(ptr1,txt);
}

gs=getguislot(gb,BGUI_NewObject(BGUI_INFO_GADGET,
	INFO_HorizOffset,4,INFO_VertOffset,4,
	INFO_TextFormat,mem,
	INFO_MinLines,numlines,
	INFO_FixTextWidth,TRUE,
	FRM_Type,FRTYPE_BUTTON,
	FRM_Flags,FRF_RECESSED,
	TAG_DONE),id,0,mem);
if(gs) gs->shortcut[0]=sh;
return(gs);
}

struct GUISlot *reqinfo(struct GUIBase *gb,UBYTE *txt,WORD id,UWORD flags)
{
return(reqinfo2(gb,txt,id,flags));
}

struct GUISlot *reqinfomsg(struct GUIBase *gb,WORD msg,WORD id,UWORD flags)
{
return(reqinfo2(gb,getstring(msg),id,flags));
}

/* CYCLE */

struct CycleData {
	WORD *selected;
	UBYTE *cycles[64];
	UBYTE data;
};

static struct GUISlot *reqcyclei(struct GUIBase *gb,WORD id,WORD *selected,UBYTE **cycles)
{
struct CycleData *cd;
WORD num,len;
UBYTE **cycs;
UBYTE *ptr;

cycs=cycles;
len=0;
while(*cycs) {
	len+=strlen(*cycs)+1;
	cycs++;
}
cd=allocmem(sizeof(struct CycleData)+len);
if(!cd) return(0);
cd->selected=selected;
cycs=cycles;
num=0;
ptr=&cd->data;
while(*cycs) {
	cd->cycles[num]=ptr;
	strcpy(ptr,*cycs);
	ptr+=strlen(ptr)+1;
	num++;
	cycs++;
}
#ifdef GUIDEBUG
printf("CYCLE:\n");
#endif
return(getguislot(gb,BGUI_NewObject(BGUI_CYCLE_GADGET,
	CYC_Active,*cd->selected,
	CYC_Labels,&cd->cycles[0],
	CYC_Popup,TRUE,
	GA_ID,id,
	TAG_DONE),id,CYCLE_KIND,cd));
}

struct GUISlot *reqcycle2msg(struct GUIBase *gb,WORD id,WORD *selected)
{
UBYTE *cycletab[3];
cycletab[0]=getstring(MSG_NON);
cycletab[1]=getstring(MSG_YESN);
cycletab[2]=0;
return(reqcyclei(gb,id,selected,cycletab));
}

struct GUISlot *reqcyclemsg(struct GUIBase *gb,WORD id,WORD *selected,SIPTR cycles,...)
{
UBYTE *cycletab[64];
guimulticonvert(cycletab,&cycles);
return(reqcyclei(gb,id,selected,cycletab));
}

struct GUISlot *reqcycle(struct GUIBase *gb,WORD id,WORD *selected,UBYTE *cycles,...)
{
return(reqcyclei(gb,id,selected,&cycles));
}

/* TOGGLE */

struct GUISlot *reqtoggle(struct GUIBase *gb,UBYTE *txt,WORD id,UWORD flags,BOOL *selected)
{
#ifdef GUIDEBUG
printf("TOGGLE:\n");
#endif
return(getguislot(gb,BGUI_NewObject(BGUI_BUTTON_GADGET,
	GA_ToggleSelect,TRUE,
	LAB_Label,txt,
	LAB_Underscore,'_',
	GA_Selected,*selected,
	GA_ID,id,
	TAG_DONE),id,10,selected));
}

struct GUISlot *reqtogglemsg(struct GUIBase *gb,WORD txt,WORD id,UWORD flags,BOOL *selected)
{
return(reqtoggle(gb,getstring(txt),id,flags,selected));
}

/*  BUTTON */

struct GUISlot *reqbutton(struct GUIBase *gb,UBYTE *txt,WORD id,UWORD flags)
{
#ifdef GUIDEBUG
printf("BUTTON:\n");
#endif
return(getguislot(gb,BGUI_NewObject(BGUI_BUTTON_GADGET,
	LAB_Label,txt,
	LAB_Underscore,'_',
	GA_ID,id,
	TAG_DONE),
	id,BUTTON_KIND,0));
}

struct GUISlot *reqbuttonmsg(struct GUIBase *gb,WORD msg,WORD id,UWORD flags)
{
return(reqbutton(gb,getstring(msg),id,flags));
}

/* STRING */

struct GUISlot *reqstring(struct GUIBase *gb,WORD id,UBYTE *str,WORD max)
{
WORD vis;

#ifdef GUIDEBUG
printf("STRING:\n");
#endif
vis=max;
if(max>16) vis=16;
return(getguislot(gb,BGUI_NewObject(BGUI_STRING_GADGET,
	STRINGA_Justification,GACT_STRINGCENTER,
	STRINGA_TextVal,str,
	STRINGA_MinCharsVisible,vis,
	STRINGA_MaxChars,max,
	GA_TabCycle,TRUE,
	GA_ID,id,
	TAG_DONE),id,STRING_KIND,str));
}

/* INTEGER */

struct GUISlot *reqinteger(struct GUIBase *gb,WORD id,LONG *value,LONG min,LONG max)
{
#ifdef GUIDEBUG
printf("INTEGER:\n");
#endif
return(getguislot(gb,BGUI_NewObject(BGUI_STRING_GADGET,
	STRINGA_IntegerMin,min,
	STRINGA_IntegerMax,max,
	STRINGA_Justification,GACT_STRINGCENTER,
	STRINGA_LongVal,*value,
	STRINGA_MaxChars,10,
	STRINGA_MinCharsVisible,10,
	GA_TabCycle,TRUE,
	GA_ID,id,
	TAG_DONE),id,INTEGER_KIND,value));
}

/* PALETTE */

struct GUISlot *reqpalette(struct GUIBase *gb,WORD id,UBYTE *col)
{
#ifdef GUIDEBUG
printf("PALETTE:\n");
#endif
return(getguislot(gb,BGUI_NewObject(BGUI_PALETTE_GADGET,
	FRM_Type,FRTYPE_BUTTON,
	FRM_Recessed,TRUE,
	PALETTE_Depth,fmconfig->mainscreen.depth,
	PALETTE_CurrentColor,*col,
	GA_ID,id,
	TAG_DONE),id,PALETTE_KIND,col));
}

/* ------------------------------------------------------ */

static void activatestrings(struct GUIBase *gb)
{
WORD cnt1,cnt2;
struct GUIGroup *gg;
struct GUISlot *gs;

for(cnt1=0;cnt1<GUIGROUPS;cnt1++) {
	gg=gb->groups[cnt1];
	if(gg) {
		for(cnt2=0;cnt2<gg->used;cnt2++) {
			gs=&gg->slots[cnt2];
			if(gs->type==STRING_KIND||gs->type==INTEGER_KIND) {
				ActivateGadget((struct Gadget*)gs->obj,gb->win,0);
				return;
			}
		}
	}
}
}

static void settabcycle(struct GUIBase *gb)
{
ULONG ss[64];
WORD cnt1,cnt2,cnt3,cnt11,cnt22;
struct GUISlot *gs,*gs2;
struct GUIGroup *gg,*gg2;
struct wmGadgetKey wmg;
Object *obj;

cnt3=0;
ss[cnt3++]=WM_TABCYCLE_ORDER;
for(cnt1=0;cnt1<GUIGROUPS;cnt1++) {
	gg=gb->groups[cnt1];
	if(gg) {
		for(cnt2=0;cnt2<gg->used;cnt2++) {
			gs=&gg->slots[cnt2];
			if(gs->type==0&&gs->shortcut[0]&&gs->id>0) {
				obj=0;
				for(cnt11=0;cnt11<GUIGROUPS;cnt11++) {
					gg2=gb->groups[cnt11];
					if(gg2) {
						for(cnt22=0;cnt22<gg2->used;cnt22++) {
							gs2=&gg2->slots[cnt22];
							if(gs2!=gs&&gs2->id==gs->id) {
								obj=gs2->obj;
								break;
							}
						}
					}
					if(obj) break;
				}
				if(obj) {
					wmg.MethodID=WM_GADGETKEY;
					wmg.wmgk_Requester=0;
					wmg.wmgk_Object=obj;
					wmg.wmgk_Key=gs->shortcut;
					DoMethodA(gb->obj,(Msg)&wmg);
				}
			}
			if(gs->type==STRING_KIND||gs->type==INTEGER_KIND) {
				ss[cnt3++]=(IPTR)gs->obj;
			}
		}
	}
}
ss[cnt3++]=0;
if(cnt3>2) DoMethodA(gb->obj,(Msg)ss);
}

void setconnectgroup(struct GUIBase *gb,WORD group,WORD alignment,WORD dest)
{
setguigroup(gb,group,alignment);
connectgroups(gb,dest,group);
}

void connectgroups(struct GUIBase *gb,WORD destgroup,WORD srcgroup)
{
struct GUISlot *gs;
WORD apu1;

apu1=gb->currentgroup;
gb->currentgroup=destgroup;
gs=getguislotonly(gb);
gs->type=1000+srcgroup;
gb->currentgroup=apu1;
#ifdef GUIDEBUG
printf("CONNECTED: %ld -> %ld\n",srcgroup,destgroup);
#endif
}

struct GUIBase *getguibase(UBYTE *title)
{
struct GUIBase *gb;
gb=allocmem(sizeof(struct GUIBase));
if(gb) {
	gb->title=title;
}
setguigroup(gb,0,1);
return(gb);
}

void setguigroup(struct GUIBase *gb,WORD group,WORD alignment)
{
struct GUIGroup **ggptr;
struct GUIGroup *gg;

gb->currentgroup=group;
ggptr=&gb->groups[gb->currentgroup];
if(!ggptr[0]) {
	gg=allocmem(sizeof(struct GUIGroup));
	gg->alignment=alignment;
	ggptr[0]=gg;
}
#ifdef GUIDEBUG
printf("SETGROUP %ld, alignment %ld\n",group,alignment);
#endif
}

void freereq(struct GUIBase *gb)
{
WORD cnt1,cnt2;
struct GUIGroup *gg;
WORD dispo=1;

closereq(gb);
#ifdef GUIDEBUG
printf("FREEREQ\n");
#endif
if(!gb) return;
if(gb->obj) {
	DisposeObject(gb->obj);
	dispo=0;
}
for(cnt1=0;cnt1<GUIGROUPS;cnt1++) {
	gg=gb->groups[cnt1];
	if(gg) {
		for(cnt2=0;cnt2<gg->used;cnt2++) {
			if(dispo&&gg->slots[cnt2].obj) DisposeObject(gg->slots[cnt2].obj);
			if(gg->slots[cnt2].data) {
				switch(gg->slots[cnt2].type)
				{
				case 0:	// INFO
				case CYCLE_KIND:
				freemem(gg->slots[cnt2].data);
				break;
				}
			}
		}
		freemem(gg);
	}
}
freemem(gb);
}

IPTR initgroup[]={
	GROUP_BackFill,4,
	GROUP_HorizOffset,4,
	GROUP_VertOffset,4,
	GROUP_Spacing,GRSPACE_NORMAL
};

Object *recurseslot(struct GUIBase *gb,WORD group)
{
Object *obj=0;
struct GUIGroup *gg;
struct GUISlot *gs;
IPTR *memtag;
IPTR *tptr;
WORD cnt1;

#ifdef GUIDEBUG
printf("RECURSESLOT: %ld:\n",group);
#endif
memtag=allocmem(sizeof(memtag[0])*256);
if(!memtag) goto error;
tptr=memtag;
gg=gb->groups[group];
if(!gg) goto error;
if(gg->alignment) {
	*tptr++=GROUP_Style;
	*tptr++=GRSTYLE_VERTICAL;
}
if(!group) {
	CopyMem(initgroup,tptr,sizeof(initgroup));
	tptr+=sizeof(initgroup)/sizeof(initgroup[0]);
} else {
	*tptr++=GROUP_Spacing;
	*tptr++=GRSPACE_NORMAL;
}

if(group==GUIGROUPS-1) {
	*tptr++=GROUP_EqualWidth;
	*tptr++=TRUE;
}

for(cnt1=0;cnt1<gg->used;cnt1++) {
	gs=&gg->slots[cnt1];
	if(gs->type>=1000) {
		*tptr++=GROUP_Member;
		*tptr++=(IPTR)recurseslot(gb,gs->type-1000);
		*tptr++=TAG_END;
		*tptr++=0;
	} else {
		*tptr++=GROUP_Member;
		*tptr++=(IPTR)gs->obj;
		*tptr++=TAG_END;
		*tptr++=0;
		#ifdef GUIDEBUG
		printf("- Object %08.8lx (%ld) added\n",gs->obj,gs->type);
		#endif
	}
}
*tptr=TAG_DONE;
#ifdef GUIDEBUG
printf("- %ld longs\n",tptr-memtag);
#endif
obj=BGUI_NewObjectA(BGUI_GROUP_GADGET,(struct TagItem*)memtag);
#ifdef GUIDEBUG
printf("-> Object %08.8lx\n",obj);
#endif
error:
freemem(memtag);
return(obj);
}

IPTR  initrequtags[]={
	WINDOW_Title,0,
	WINDOW_Screen,0,
	WINDOW_Font,0,
	WINDOW_FallBackFont,0,
	WINDOW_Position,POS_CENTERSCREEN,
	WINDOW_SizeGadget,FALSE,
	WINDOW_Activate,TRUE,
	WINDOW_RMBTrap,TRUE,
	WINDOW_SmartRefresh,TRUE,
	WINDOW_AutoKeyLabel,TRUE,
	WINDOW_MasterGroup
};

BOOL initrequ(struct GUIBase *gb)
{
IPTR memtag[(sizeof(initrequtags) + sizeof(IPTR) - 1)/sizeof(IPTR)+32];
IPTR *tptr;

#ifdef GUIDEBUG
printf("INITREQU");
#endif
if(!gb) return(0);
tptr=memtag;
CopyMem(initrequtags,tptr,sizeof(initrequtags));
tptr[1]=(IPTR)gb->title;
if(gb->screen) tptr[3]=(IPTR)gb->screen; else tptr[3]=(IPTR)fmmain.naytto;
tptr[5]=(IPTR)&fmconfig->reqfontattr;
tptr[7]=(IPTR)&fmconfig->smallfontattr;
tptr+=sizeof(initrequtags)/sizeof(ULONG);
*tptr++=(IPTR)recurseslot(gb,0);
*tptr++=0;
*tptr=0;
gb->obj=BGUI_NewObjectA(BGUI_WINDOW_OBJECT,(struct TagItem*)memtag);
#ifdef GUIDEBUG
printf("INITREQU -> %08.8lx\n",gb->obj);
#endif
settabcycle(gb);
return((BOOL)(gb->obj?1:0));
}

BOOL openreq(struct GUIBase *gb)
{
#ifdef GUIDEBUG
printf("OPENREQ\n");
#endif
if(gb&&gb->obj) {
	if((gb->win=(struct Window*)DoMethod(gb->obj,WM_OPEN))) return(1);
}
return(0);
}

void closereq(struct GUIBase *gb)
{
#ifdef GUIDEBUG
printf("CLOSEREQ\n");
#endif
if(!gb||!gb->obj) return;
gb->win=0;
DoMethod(gb->obj,WM_CLOSE);
}

void sleepreq(struct GUIBase *gb)
{
if(!gb||!gb->obj) return;
DoMethod(gb->obj,WM_SLEEP);
}
void wakereq(struct GUIBase *gb)
{
if(!gb||!gb->obj) return;
DoMethod(gb->obj,WM_WAKEUP);
}

BOOL openinitreq(struct GUIBase *gb)
{
if(!gb->obj) initrequ(gb);
if(!gb->obj) return(0);
if(openreq(gb)) return(1);
return(0);
}

BOOL initreq(struct GUIBase *gb)
{
return(initrequ(gb));
}

WORD quickreq(struct GUIBase *gb)
{
WORD ret;

ret=0;
if(openinitreq(gb)) {
	for(;;) {
		ret=reqmsghandler(gb);
		if(ret<100) break;
	}
}
freereq(gb);
return(ret);
}

void startbuttonbar(struct GUIBase *gb)
{
setguigroup(gb,GUIGROUPS-1,0);
connectgroups(gb,0,GUIGROUPS-1);
}
void buttonbari(struct GUIBase *gb,void **buttons)
{
startbuttonbar(gb);
while(*buttons) {
	reqbutton(gb,buttons[0],(IPTR)buttons[1],guiUC);
	buttons+=2;
}
}

void buttonbar(struct GUIBase *gb,void *button,...)
{
buttonbari(gb,&button);
}
void buttonbarmsg(struct GUIBase *gb,SIPTR button,...)
{
void *text[32];
guimulticonvert2(text,(IPTR *)&button);
buttonbari(gb,text);
}

WORD reqinfowindow(UBYTE *title,UBYTE *body,UWORD flags,SIPTR button,...)
{
struct GUIBase *gb;
void *text[32];
WORD ret;

ret=0;
guimulticonvert2(text,(IPTR *)&button);
gb=getguibase(title);
reqinfo(gb,body,-1,flags);
buttonbari(gb,text);
if(openinitreq(gb)) ret=reqmsghandler(gb);
freereq(gb);
return(ret);
}

static void checkgui(struct GUIBase *gb,WORD id)
{
struct GUISlot *gs;
struct GUIGroup *gg;
struct CycleData *cd;
WORD cnt1,cnt2;
IPTR data;

for(cnt1=0;cnt1<GUIGROUPS;cnt1++) {
	gg=gb->groups[cnt1];
	if(gg) {
		for(cnt2=0;cnt2<gg->used;cnt2++) {
			gs=&gg->slots[cnt2];
			if(id==-1||gs->id==id) {
				switch(gs->type)
				{
				case STRING_KIND:
				GetAttr(STRINGA_TextVal,gs->obj,&data);
				strcpy((UBYTE*)gs->data,(UBYTE*)data);
				break;
				case INTEGER_KIND:
				GetAttr(STRINGA_LongVal,gs->obj,gs->data);
				break;
				case CYCLE_KIND:
				cd=(struct CycleData*)gs->data;
				GetAttr(CYC_Active,gs->obj,&data);
				*cd->selected=data;
				break;
				case 10:
				GetAttr(GA_SELECTED,gs->obj,&data);
				*((BOOL*)gs->data)=data;
				break;
				case PALETTE_KIND:
				GetAttr(PALETTE_CurrentColor,gs->obj,&data);
				*((UBYTE*)gs->data)=data;
				break;
				}
			}
		}
	}
}
}


LONG getobject(struct GUIBase *gb,WORD id)
{
WORD cnt1,cnt2;
struct GUISlot *gs;
struct GUIGroup *gg;
struct CycleData *cd;
SIPTR data=-1;

for(cnt1=0;cnt1<GUIGROUPS;cnt1++) {
	gg=gb->groups[cnt1];
	if(gg) {
		for(cnt2=0;cnt2<gg->used;cnt2++) {
			gs=&gg->slots[cnt2];
			if(gs->id==id) {
				switch(gs->type)
				{
				case STRING_KIND:
				GetAttr(STRINGA_TextVal,gs->obj,(IPTR*)gs->data);
				break;
				case INTEGER_KIND:
				GetAttr(STRINGA_LongVal,gs->obj,(IPTR*)gs->data);
				break;
				case CYCLE_KIND:
				cd=(struct CycleData*)gs->data;
				GetAttr(CYC_Active,gs->obj,(IPTR*)&data);
				*cd->selected=data;
				break;
				case 10:
				GetAttr(GA_SELECTED,gs->obj,(IPTR*)&data);
				break;
				case PALETTE_KIND:
				GetAttr(PALETTE_CurrentColor,gs->obj,(IPTR*)&data);
				break;
				}
			}
		}
	}
}
#ifdef GUIDEBUG
printf("GETOBJ: %ld, %ld\n",id,data);
#endif
return(data);
}

void setobject(struct GUIBase *gb,WORD id,LONG data)
{
WORD cnt1,cnt2;
struct GUISlot *gs;
struct GUIGroup *gg;
struct CycleData *cd;

#ifdef GUIDEBUG
printf("SETOBJ: %ld, %ld\n",id,data);
#endif

for(cnt1=0;cnt1<GUIGROUPS;cnt1++) {
	gg=gb->groups[cnt1];
	if(gg) {
		for(cnt2=0;cnt2<gg->used;cnt2++) {
			gs=&gg->slots[cnt2];
			if(gs->id==id) {
				switch(gs->type)
				{
				case STRING_KIND:
				SetGadgetAttrs((struct Gadget*)gs->obj,gb->win,0,STRINGA_TextVal,data,TAG_DONE);
				break;
				case INTEGER_KIND:
				SetGadgetAttrs((struct Gadget*)gs->obj,gb->win,0,STRINGA_LongVal,data,TAG_DONE);
				*((LONG*)gs->data)=data;
				break;
				case CYCLE_KIND:
				cd=(struct CycleData*)gs->data;
				SetGadgetAttrs((struct Gadget*)gs->obj,gb->win,0,CYC_Active,data,TAG_DONE);
				*cd->selected=data;
				break;
				case 10:
				SetGadgetAttrs((struct Gadget*)gs->obj,gb->win,0,GA_SELECTED,data,TAG_DONE);
				*((BOOL*)gs->data)=data;
				break;
				case PALETTE_KIND:
				SetGadgetAttrs((struct Gadget*)gs->obj,gb->win,0,PALETTE_CurrentColor,data,TAG_DONE);
				break;
				}
			}
		}
	}
}
}

void denaobject(struct GUIBase *gb,WORD id,BOOL flag)
{
WORD cnt1,cnt2;
struct GUISlot *gs;
struct GUIGroup *gg;

#ifdef GUIDEBUG
printf("DENAOBJ: %ld, %ld\n",id,flag);
#endif

for(cnt1=0;cnt1<GUIGROUPS;cnt1++) {
	gg=gb->groups[cnt1];
	if(gg) {
		for(cnt2=0;cnt2<gg->used;cnt2++) {
			gs=&gg->slots[cnt2];
			if(gs->id==id) {
				SetGadgetAttrs((struct Gadget*)gs->obj,gb->win,0,GA_Disabled,flag,TAG_DONE);
			}
		}
	}
}
}


WORD reqmsghandler(struct GUIBase *gb)
{
Object *obj;
IPTR signal;
ULONG sigs;
ULONG rc;
WORD ret;

#ifdef GUIDEBUG
printf("MSGHANDLER\n");
#endif
if(!gb) return(0);
obj=gb->obj;
if(!obj) return(0);
GetAttr(WINDOW_SigMask,obj,&signal);
ret=-2;
while(ret==-2) {
	sigs=Wait(signal|SIGBREAKF_CTRL_C);
	if(sigs&SIGBREAKF_CTRL_C) ret=-1;
	if(sigs&signal) {
		while((rc=HandleEvent(obj))!=WMHI_NOMORE) {
			if(rc!=WMHI_IGNORE) {
				if(rc==WMHI_ACTIVE) activatestrings(gb);
				if(rc==WMHI_CLOSEWINDOW) {
					ret=0;
				} else if(rc<1000) {
					ret=rc;
				}
			}
		}
	}
}
checkgui(gb,-1);
return(ret);
}

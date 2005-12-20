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

#include <time.h>
#include <dos/datetime.h>
#include <proto/locale.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <strings.h>
#include <stdarg.h>
#include <dos/dos.h>
#include <proto/locale.h>
#include <clib/locale_protos.h>
#include "fmnode.h"
#include "fmlocale.h"


//int formathook;

void setsliders(struct FMList*);

#ifndef AROS
extern void __asm RealTimeScroll(register __a0 struct ReqScrollStruct*);
#else /* AROS */
extern void RealTimeScroll(struct ReqScrollStruct*);
#endif /* AROS */

struct FMNode *iteratenode(struct FMNode**);
UBYTE *scanchar(UBYTE*,UBYTE);

void shellsortlist(struct FMList*,struct FMNode*,struct Library*,WORD,WORD,WORD);

extern struct FMMain fmmain;
extern struct FMConfig *fmconfig;
extern struct FMList fmlist[];

void clearlisti(struct List *list)
{
struct Node *node;

while (list->lh_Head->ln_Succ) {
	node=(struct Node*)list->lh_Head;
	Remove(node);
	freevecpooled(node);
}
}

void clearlist(struct FMList *list)
{
list->lastclicked=0; list->fmmessage1[0]=0; list->fmmessage2[0]=0;
list->firstline=0;
list->keyline=0;
clearlisti((struct List*)list);
}


void shellsortlist(struct FMList *list,struct FMNode *node,struct Library *ub,WORD fo,WORD stype,WORD sud)
{
struct FMNode *node2;
struct FMNode **sortbuf,**ptr1,*ptr2;
WORD nodes;
WORD range,i,j,apu;

node2=node;
nodes=0;
while(node2->succ) {
	node2=node2->succ;
	nodes++;
}

if(nodes<=1) return;

sortbuf=allocmem((LONG)nodes*4);
if(!sortbuf) return;

ptr1=sortbuf+nodes; i=nodes;
while(i--) *--ptr1=(struct FMNode*)RemTail((struct List*)list);

for(range=nodes/2;range>0;range/=2) {
	for(i=range;i<nodes;i++) {
		j=i-range;
		while(j>=0) {
			apu=0;
			switch(stype)
			{
			case 0:
			if(Stricmp(NDFILE(sortbuf[j]),NDFILE(sortbuf[j+range]))>0) apu=1;
			break;
			case 1:
			if(sortbuf[j]->numlen>sortbuf[j+range]->numlen) apu=1;
			break;
			case 2:
			if(sortbuf[j]->numdate>sortbuf[j+range]->numdate) apu=1;
			break;
			case 3:
			if(Stricmp(NDCOMM(sortbuf[j]),NDCOMM(sortbuf[j+range]))>0) apu=1;
			break;
			}
			if(apu^sud) {
				ptr2=sortbuf[j];
				sortbuf[j]=sortbuf[j+range];
				sortbuf[j+range]=ptr2;
				j-=range;
				continue;
			}
			break;
		}
	}
}

ptr1=sortbuf; i=nodes;
while(i--) AddTail((struct List*)list,(struct Node*)*ptr1++);

freemem(sortbuf);
}

void csortlist(struct FMList *list)
{
struct FMList list2;
struct FMNode *node,*node2;
struct Match *match;
UWORD ftest;

if((list->flags&LDEVLIST)||(!list->li)) return;
NewList((struct List*)&list2);
match=getconfignumber(PARSECONFIG)->moreconfig;
shellsortlist(list,list->head,UtilityBase,fmmain.fileoffset,0,0);
if(match->msortby||match->msortud) shellsortlist(list,list->head,UtilityBase,fmmain.fileoffset,match->msortby,match->msortud);
if(match->mdrawersfirst) ftest=NFILE; else ftest=NDIRECTORY;
if(!match->mmix) {
	node=list->head;
	while(node->succ) {
		node2=node->succ;
		if(node->flags&ftest) {
			Remove((struct Node*)node);
			AddTail((struct List*)&list2,(struct Node*)node);
		}
		node=node2;
	}
	if(match->msortby==1&&ftest==NDIRECTORY) shellsortlist(&list2,list2.head,UtilityBase,fmmain.fileoffset,0,0);
	while(list->head->succ) {
		node=list->head;
		Remove((struct Node*)node);
		AddTail((struct List*)&list2,(struct Node*)node);
	}
	if(list2.head->succ) {
		list->head=list2.head;
		list->tailpred=list2.tailpred;
		list->head->pred=(struct FMNode*)&list->head;
		list->tailpred->succ=(struct FMNode*)&list->tail;
	} else {
		NewList((struct List*)list);
	}
}
}

struct FMNode *disknode(struct FMList *list,UBYTE *newname)
{
struct FMNode *node,*node2,*node3;
BPTR lock;

if(!currentdir(list)) return(0);
lock=Lock(newname,SHARED_LOCK);
if(!lock) return(0);
UnLock(lock);
node=allocnode();
if(!node) return(0);
siirran(NDFILE(node),newname,fmmain.filelen);
examinefile(list,node);
node->flags|=NMATCHED;

node2=list->head;
while(node2->succ) {
	node3=node2->succ;
	if (!Stricmp(NDFILE(node2),newname)) removenode(list,node2);
	node2=node3;
}
AddTail((struct List*)list,(struct Node*)node);
list->totlines++;
return(node);
}

struct FMNode* dupnode(struct FMNode *node)
{
struct FMNode *newnode;
if ((newnode=(struct FMNode*)allocvecpooled(fmmain.totnodelen))) {
	CopyMem(node,newnode,fmmain.totnodelen);
	newnode->succ=0;newnode->pred=0;
}
return(newnode);
}

WORD duplist(struct FMList *list1,struct FMList *list2)
{
struct FMNode *node1;
struct FMNode *node2;

for(node1=list1->head;node1->succ;node1=node1->succ) {
	if (!(node2=dupnode(node1))) {
		clearlist(list2);
		return(FALSE);
	} else {
		AddTail((struct List*)list2,(struct Node*)node2);
	}
}
return(TRUE);
}

void removenode(struct FMList *list,struct FMNode *node)
{
Remove((struct Node*)node);
freevecpooled(node);
list->totlines--;
if (list->totlines-list->firstline<list->li->visiblelines) list->firstline=list->totlines-list->li->visiblelines;
if ((WORD)list->firstline<0) list->firstline=0;
setsliders(list);
}

struct FMNode* scrollifneeded(struct FMList *l,UWORD *keyline,UWORD *keylinew)
{
struct FMNode *node;
struct ReqScrollStruct *rss, *rssw;
WORD apu1;
UWORD rfl,rflw;
WORD vscroll=0,hscroll=0;
LONG apu2;
LONG kline,klinew;

rss=&l->li->rscroll1;
rssw=&l->li->rscroll2;

if(*keyline>65000) *keyline=0;
if(*keyline>=l->totlines) *keyline=l->totlines-1;
if(*keylinew>65000) *keylinew=0;
if(*keylinew>=l->totlinesw) *keylinew=l->totlinesw-1;
kline=*keyline;
klinew=*keylinew;

node=(struct FMNode*)l;
iteratenode(&node);
apu1=kline;
while(apu1--) iteratenode(&node);

if(kline>=l->firstline+l->li->visiblelines) {
	l->firstline=kline-1;
	vscroll=1;
} else if (kline<l->firstline) {
	apu2=kline-(LONG)(l->li->visiblelines-2);
	if(apu2<0) apu2=0;
	l->firstline=apu2;
	vscroll=1;
}
if(vscroll) {
	rfl=rss->TopEntryNumber;
	updatelist(l);
	rss->TopEntryNumber=rfl;
	RealTimeScroll(rss);
	rss->fmlist->firstline=rss->TopEntryNumber;
}

if(klinew>=l->firstlinew+l->li->visiblelinesw) {
	l->firstlinew=klinew-1;
	hscroll=1;
} else if (klinew<l->firstlinew) {
	apu2=klinew-(LONG)(l->li->visiblelinesw-2);
	if(apu2<0) apu2=0;
	l->firstlinew=apu2;
	hscroll=1;
}
if(hscroll) {
	rflw=rssw->TopEntryNumber;
	updatelist(l);
	rssw->TopEntryNumber=rflw;
	RealTimeScroll(rssw);
	rssw->fmlist->firstlinew=rssw->TopEntryNumber;
}
return(node);
}

struct FMNode* findselnodeall(struct FMList *list)
{
struct FMNode *node;
UWORD line=0,linew;

node=(struct FMNode*)list;
for(;;) {
	iteratenode(&node);
	if(!node) break;
	line++;
	if (node->flags&NSELECTED) {
		linew=list->firstlinew;
		scrollifneeded(list,&line,&linew);
		return(node);
	}
}
return(0);
}

struct FMNode* findselnode(struct FMList *list)
{
struct FMNode *node;
UWORD line=0,linew;

if (list->flags&LDIRLIST) {
	node=(struct FMNode*)list;
	for(;;) {
		iteratenode(&node);
		if(!node) break;
		line++;
		if (node->flags&NSELECTED&&node->flags&NFILE) {
			linew=list->firstlinew;
			scrollifneeded(list,&line,&linew);
			return(node);
		}
	}
}
return(0);
}

#ifndef AROS
void __asm formathook(void);
void __asm rawdo(register __a0 UBYTE*,register __a1 void*,register __a3 UBYTE*);
#else

AROS_UFH3(VOID, formathook,
AROS_UFCA(struct Hook *, putCharFunc, A0),
AROS_UFCA(struct Locale *, locale, A2),
AROS_UFCA(char , c, A1))
{
    AROS_USERFUNC_INIT

    char * xyz = putCharFunc->h_Data;
    *xyz = c;
    putCharFunc->h_Data = (ULONG)putCharFunc->h_Data + 1;

    AROS_USERFUNC_EXIT
}

void rawdo(UBYTE*, void*, UBYTE*);
#endif


void sformatmsg(UBYTE *ptr,LONG form,...)
{
va_list args;
va_start(args,form);
sformatti(ptr,getstring(form),args);
}

void sformat(UBYTE *ptr,UBYTE *form,...)
{
va_list args;
D(bug("sformat nodes.c 376...........\n"));
va_start(args,form);
D(bug("sformat nodes.c 377...........\n")); 
sformatti(ptr,form,args);
}

void sformatti(UBYTE *ptr,UBYTE *form,UBYTE *args)
{
struct Hook hook;
extern struct Locale *locale;
D(bug("nodes.c 385 sformatti...........\n")); 
ObtainSemaphore(&fmmain.msgsema);
if(locale&&!(fmconfig->flags&MLOCALE)) {
	hook.h_Data=ptr;
	hook.h_Entry=(HOOKFUNC)formathook;
D(bug("FormatString nodes.c 388...........\n"));  
	FormatString(locale,form,args, &hook);
D(bug("FormatString nodes.c 391...........\n"));

  } else {
 	rawdo(form,args,ptr);
 }
D(bug("FormatString nodes.c 398...........\n"));
ReleaseSemaphore(&fmmain.msgsema);
D(bug("FormatString nodes.c 400...........\n"));
}

UBYTE scanus(UBYTE *ptr)
{
while(*ptr) {
	if (*ptr=='_') return(ToUpper(*(ptr+1)));
	ptr++;
}
return(0);
}

void copyus(UBYTE *ptr1,UBYTE *ptr2)
{
while(*ptr2) {
	if(*ptr2!='_') *ptr1++=*ptr2;
	ptr2++;
}
*ptr1=0;
}

struct FMNode *allocnode(void)
{
struct FMNode *node;

node=(struct FMNode*)allocvecpooled(fmmain.totnodelen);
if(node) node->nodelen=fmmain.totnodelen;
return(node);
}

void strcpymsg(UBYTE *ptr,WORD msg)
{
strcpy(ptr,getstring(msg));
}

struct FMNode *iteratenode(struct FMNode **node)
{
struct FMNode *node2;

node2=*node;
if (node2&&(node2->succ)) node2=node2->succ;
while(node2&&node2->succ&&!(node2->flags&NMATCHED)) node2=node2->succ;
if(node2==0||node2->succ==0) node2=0;
*node=node2;
return(node2);
}


WORD sselected(struct FMList *list,WORD mode)
{
struct FMNode *node;

if(!(list->flags&LDIRLIST)) {
	strcpymsg(list->fmmessage1,MSG_MAIN_NOSOURCE);
	fmmessage(list);
} else {
	if(!mode) return(1);
	node=(struct FMNode*)list;
	for(;;) {
		iteratenode(&node);
		if(!node) break;
		if(node->flags&NSELECTED) {
			if(mode>0&&node->flags&NFILE) return(1);
			if(mode<0&&node->flags&(NFILE|NDIRECTORY)) return(1);
		}
	}
	strcpymsg(list->fmmessage1,MSG_MAIN_NOSOURCEFILE);
	fmmessage(list);
}
return(0);
}

ULONG dstolong(struct DateStamp *ds)
{
return(((ULONG)ds->ds_Days*24*60+(ULONG)ds->ds_Minute)*60+(ULONG)ds->ds_Tick/TICKS_PER_SECOND);
}
void longtods(struct DateStamp *ds,ULONG date)
{
ds->ds_Tick=(date%60)*TICKS_PER_SECOND;
date/=60;
ds->ds_Minute=date%(60*24);
date/=60*24;
ds->ds_Days=date;
}

ULONG datestringtolong(UBYTE *date)
{
struct DateTime dt;
UBYTE varatime[34];

if(!date[0]) return(0xffffffff);
memseti(&dt,0,sizeof(struct DateTime));
dt.dat_Format=4;
strcpy(varatime,date);
dt.dat_StrDate=varatime;
dt.dat_StrTime=scanchar(varatime,' ');
if(dt.dat_StrTime) {
	*dt.dat_StrTime++=0;
}
if(StrToDate(&dt)) return(dstolong(&dt.dat_Stamp));
return(0xffffffff);
}

void longtodatestring(UBYTE *sdate,ULONG date)
{
struct DateTime dt;
UBYTE pattime[18];
UBYTE patdate[34];

memseti(sdate,0,fmmain.datelen);
if(date==0xffffffff) return;
longtods(&dt.dat_Stamp,date);
dt.dat_Format=4;
dt.dat_Flags=0;
dt.dat_StrDay=0;
dt.dat_StrDate=patdate;
dt.dat_StrTime=pattime;
if(!(DateToStr(&dt))) return;
strcpy(patdate+strlen(patdate)+1,pattime);
patdate[strlen(patdate)]=32;
siirran(sdate,patdate,fmmain.datelen);
}

void siirran(UBYTE *dst,UBYTE *src,WORD len)
{
strncpy(dst,src,len);
*(dst+len)=0;
}

void fittext(struct RastPort *rp,UBYTE *txt,WORD txtlen,WORD x,WORD y,WORD width,WORD mode)
{
WORD len;
struct TextExtent txtext;

width--;
if(txtlen<0) txtlen=strlen(txt);
len=TextFit(rp,txt,txtlen,&txtext,0,1,width,32767);
ObtainSemaphore(&fmmain.gfxsema);
SetAPen(rp,fmconfig->backpen);
WaitTOF();
RectFill(rp,x,y,width+x,y+fmmain.txtysize-1);
if(!mode)
	Move(rp,x,y+fmmain.txtbaseline);
	else
	Move(rp,x+(width-txtext.te_Width)/2,y+fmmain.txtbaseline);
SetAPen(rp,fmconfig->txtpen);
Text(rp,txt,len);
ReleaseSemaphore(&fmmain.gfxsema);
}

WORD strtohex(UBYTE *ptr)
{
UBYTE bapu1,bapu2;

bapu1=ToUpper(ptr[0]);
bapu2=ToUpper(ptr[1]);
if(((bapu2<'0'||bapu2>'9')&&(bapu2<'A'||bapu2>'F'))||((bapu1<'0'||bapu1>'9')&&(bapu1<'A'||bapu1>'F'))) return(-1);
if(bapu1>='A') bapu1-=7;
if(bapu2>='A') bapu2-=7;
bapu1-='0';
bapu2-='0';
return((WORD)(bapu1<<4|bapu2));
}

void memseti(void *dst,UBYTE ch,LONG len)
{
UBYTE *dst2=(UBYTE*)dst;
while(len--) *dst2++=ch;
}
/*
UBYTE *scanchar(UBYTE *ptr,UBYTE ch)
{
while(*ptr) {
	if (*ptr==ch) return(ptr);
	ptr++;
}
return(0);
}
*/

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
#include <dos/dos.h>
#include <string.h>

#include "fmnode.h"
#include "fmlocale.h"

extern struct UtilityBase *UtilityBase;
extern struct ExecBase *SysBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;

void setsliders(struct FMList*);
struct FMNode *iteratenode(struct FMNode**);

extern struct FMConfig *fmconfig;
extern struct FMMain fmmain;
extern UBYTE space[];

static void outputline(struct FMList *list,struct FMNode *node,WORD maxlen,WORD offset,WORD xx,WORD yy)
{
UBYTE varatxt[512];
UBYTE *ptr1=NULL,*ptr2;
WORD rlen,len,cnt1,apu1;
struct DirListLayout *dll;

memseti(varatxt,32,fmmain.maxdirlistwidth+1);
Move(fmmain.rp,xx,yy+fmmain.listbaseline);
rlen=0;
if (node&&node->succ) {
	if(node->flags&NSELECTED)
		SetDrMd(fmmain.rp,JAM2|INVERSVID);
		else
		SetDrMd(fmmain.rp,JAM2);
	if(node->flags&(NASSIGN|NDIRECTORY))
		SetAPen(fmmain.rp,fmconfig->dirpen);
		else
		SetAPen(fmmain.rp,fmconfig->filepen);
	if(node->flags&NDEVICE) SetAPen(fmmain.rp,fmconfig->devicepen);
	if(node->flags&NKEYBOARD)
		SetBPen(fmmain.rp,fmconfig->hilipen);
		else
		SetBPen(fmmain.rp,fmconfig->backpen);
	ptr2=varatxt;
	if (list->flags & LDIRLIST) {
		for(cnt1=0;cnt1<5;cnt1++) {
			dll=&fmconfig->dl[cnt1];
			switch(dll->type)
			{
			case 0:
			ptr1=NDLENGTH(node);
			break;	
			case 1:
			ptr1=NDFILE(node);
			break;	
			case 2:
			ptr1=NDPROT(node);
			break;	
			case 3:
			ptr1=NDDATE(node);
			break;	
			case 4:
			ptr1=NDCOMM(node);
			break;
			}
			len=strlen(ptr1);
			if(dll->rightlock) {
				ptr2=varatxt+list->li->visiblelinesw-dll->width;
			}
			if(dll->rightaligned) {
				apu1=dll->width-len;
				if(apu1<0) apu1=0;
				CopyMem(ptr1,ptr2+apu1,len);
			} else {
				CopyMem(ptr1,ptr2,len);
			}
			ptr2+=dll->width;
			*ptr2++=32;
		}
	} else {
		CopyMem(NDFILE(node),ptr2,strlen(NDFILE(node)));
		ptr2+=8;
		if (node->flags & NDEVICE) {
			CopyMem(NDCOMM(node),ptr2,strlen(NDCOMM(node)));
			ptr2=varatxt+list->li->visiblelinesw-6;
			CopyMem(NDCOMM(node)+fmmain.commlen/2,ptr2,strlen(NDCOMM(node)+fmmain.commlen/2));
		}
	}
	rlen=fmmain.maxdirlistwidth-offset;
	if (rlen>maxlen) rlen=maxlen;
	if (rlen>0) Text(fmmain.rp,&varatxt[offset],rlen); else rlen=0;


} else {
	SetDrMd(fmmain.rp,JAM2);
}



if (rlen<maxlen) Text(fmmain.rp,space,maxlen-rlen);


}

#ifdef __AROS__
AROS_UFH5(void, scrollndraw,
        AROS_UFHA(WORD, fline, D0),
        AROS_UFHA(WORD, liney, D1),
        AROS_UFHA(WORD, samount, D2),
        AROS_UFHA(WORD, nlines, D3),
        AROS_UFHA(struct ReqScrollStruct*, rscroll, A0))
{
    AROS_USERFUNC_INIT
#else
void scrollndraw(void)
{
#endif
/* fline=line to draw */
/* liney=pixel offset */
/* samount=scroll amount */
/* nlines=lines to draw */

WORD xx,yy,aa;
struct FMList *list;
struct FMNode *node;
struct ListInfo *li;

#ifndef AROS
struct ReqScrollStruct *rscroll;
WORD fline,liney,samount,nlines;
fline=(WORD)getreg(REG_D0);
liney=(WORD)getreg(REG_D1);
samount=(WORD)getreg(REG_D2);
nlines=(WORD)getreg(REG_D3);
rscroll=(struct ReqScrollStruct*)getreg(REG_A0);
#endif

if (!fmmain.ikkuna) return;
ObtainSemaphore(&fmmain.gfxsema);
SetFont(fmmain.rp,fmmain.listfont);
list=rscroll->fmlist;
li=list->li;
yy=li->diry; xx=li->dirx;
node=(struct FMNode*)list;
iteratenode(&node);
if (!rscroll->flags) {
	if(rscroll->LineSpacing==1) {
		samount*=fmmain.listysize;
		liney*=fmmain.listysize;
	}
	ScrollRaster(fmmain.rp,0,samount,xx,yy,xx+li->dirwidth-1,yy+li->dirheight-1);
	yy+=liney;
	list->firstline=rscroll->TopEntryNumber;
	while(fline>0) { fline--; iteratenode(&node); }
	while(nlines>0) {
		nlines--;
		outputline(list,node,li->visiblelinesw,list->firstlinew,xx,yy);
		iteratenode(&node);
		yy+=fmmain.listysize;
	}
} else {
	if(rscroll->LineSpacing==1) {
		samount*=fmmain.listxsize;
		liney*=fmmain.listxsize;
	}
	ScrollRaster(fmmain.rp,samount,0,xx,yy,xx+li->dirwidth-1,yy+li->dirheight-1);
	aa=list->firstline;
	while(aa>0) { iteratenode(&node); aa--; }
	xx+=liney;
	list->firstlinew=rscroll->TopEntryNumber;
	if (nlines) {
		for(aa=0;aa<li->visiblelines;aa++) {
			outputline(list,node,nlines,fline,xx,yy);
			iteratenode(&node);
			yy+=fmmain.listysize;
		}
	}
	}
SetDrMd(fmmain.rp,JAM2);
SetBPen(fmmain.rp,fmconfig->backpen);
SetFont(fmmain.rp,fmmain.txtfont);
ReleaseSemaphore(&fmmain.gfxsema);
#ifdef __AROS__
AROS_USERFUNC_EXIT
#endif
}

void outputlistline(struct FMList *list,struct FMNode *node)
{
struct FMNode *node2;
struct ListInfo *li;
WORD cnt=0,yy;

li=list->li;
if (!fmmain.ikkuna||!li) return;
node2=(struct FMNode*)list;
iteratenode(&node2);
while(node!=node2) { iteratenode(&node2); cnt++; }
cnt-=list->firstline;
yy=li->diry;
if (cnt<li->visiblelines) {
	ObtainSemaphore(&fmmain.gfxsema);
	SetFont(fmmain.rp,fmmain.listfont);
	yy+=cnt*fmmain.listysize;
	outputline(list,node,li->visiblelinesw,list->firstlinew,li->dirx,yy);
	SetDrMd(fmmain.rp,JAM2);
	SetBPen(fmmain.rp,fmconfig->backpen);
	SetFont(fmmain.rp,fmmain.txtfont);
	ReleaseSemaphore(&fmmain.gfxsema);
}
}

void drawall(struct ReqScrollStruct *rscroll)
{
struct FMList *list;
struct ListInfo *li;
struct FMNode *node;
WORD aa,yy;

list=rscroll->fmlist;
li=list->li;
if (!fmmain.ikkuna||!li) return;
ObtainSemaphore(&fmmain.gfxsema);
SetFont(fmmain.rp,fmmain.listfont);
yy=li->diry;
node=(struct FMNode*)list;
iteratenode(&node);
aa=list->firstline;
while(aa>0) { iteratenode(&node); aa--; }
for(aa=0;aa<li->visiblelines;aa++) {
	outputline(list,node,li->visiblelinesw,list->firstlinew,li->dirx,yy);
	iteratenode(&node);
	yy+=fmmain.listysize;
}
SetDrMd(fmmain.rp,JAM2);
SetBPen(fmmain.rp,fmconfig->backpen);
SetFont(fmmain.rp,fmmain.txtfont);
ReleaseSemaphore(&fmmain.gfxsema);
}

void outputlist(struct FMList *list)
{
if(list&&list->li) {
	updatelist(list);
	fmoutput(list);
}
}

void fmoutput(struct FMList *list)
{
if (fmmain.ikkuna&&list->li) {
	list->li->rscroll1.NumEntries=list->totlines;
	list->li->rscroll2.NumEntries=list->totlinesw;
	drawall(&list->li->rscroll1);
	list->li->rscroll1.TopEntryNumber=list->firstline;
	list->li->rscroll2.TopEntryNumber=list->firstlinew;
}
}

void updatelist(struct FMList *list)
{
struct FMNode *node;
LONG totlines,firstline,visiblelines;
WORD aa=0;

node=(struct FMNode*)list;
while(iteratenode(&node)) aa++;
list->totlines=aa;
totlines=list->totlines;
firstline=list->firstline;
visiblelines=list->li->visiblelines;
if (totlines-firstline<visiblelines) firstline=totlines-visiblelines;
if (firstline<0) firstline=0;
list->firstline=firstline;
list->totlinesw=fmmain.maxdirlistwidth;
setsliders(list);
}


static void createsliderimage(struct Image *im,WORD type)
{
struct RastPort rp;
struct BitMap bm;
WORD apu1,width,height;
UBYTE *ptr1;

InitRastPort(&rp);
InitBitMap(&bm,im->Depth,im->Width,im->Height);
ptr1=(UBYTE*)im->ImageData;
for(apu1=0;apu1<im->Depth;apu1++) {
	bm.Planes[apu1]=ptr1;
	ptr1+=RASSIZE(im->Width,im->Height);
}
rp.BitMap=&bm;
width=im->Width-1;
height=im->Height-1;
SetAPen(&rp,fmconfig->sliderpen);
RectFill(&rp,0,0,width,height);
if(!type) SetAPen(&rp,fmconfig->whitepen); else SetAPen(&rp,fmconfig->blackpen);
Move(&rp,0,im->Height);
Draw(&rp,0,0);
Draw(&rp,width-1,0);
if(fmconfig->flags&MDOUBLED) {
	Move(&rp,1,im->Height);
	Draw(&rp,1,0);
}
if(type) SetAPen(&rp,fmconfig->whitepen); else SetAPen(&rp,fmconfig->blackpen);
Move(&rp,width,0);
Draw(&rp,width,height);
Draw(&rp,1,height);
if(fmconfig->flags&MDOUBLED) {
	Move(&rp,width-1,im->Height);
	Draw(&rp,width-1,1);
}
}

static void setpot(struct Gadget *gadg,UWORD overlap,UWORD totlines,UWORD vislines,UWORD topline,UWORD type)
{
UWORD hidden,body,pot;
WORD oldpos;
WORD size;
struct PropInfo *pi;

if (totlines>vislines) hidden=totlines-vislines; else hidden=0;
if (topline>hidden) topline=hidden;
if (hidden>0) 
	body=(UWORD)(((ULONG)(vislines-overlap)*MAXBODY)/(totlines-overlap));
	else
	body=MAXBODY;
if (hidden>0)
	pot=(UWORD)(((ULONG)topline*MAXPOT)/hidden);
	else
	pot=0;
pi=(struct PropInfo*)gadg->SpecialInfo;
if (fmmain.ikkuna) {
	if (type) {
		if(pi->HorizPot!=pot||pi->HorizBody!=body) {
			NewModifyProp(gadg,fmmain.ikkuna,0,FREEHORIZ|PROPBORDERLESS,pot,MAXPOT,body,MAXBODY,1);
			oldpos=RemoveGadget(fmmain.ikkuna,gadg);
			size=((ULONG)body*(ULONG)(gadg->Width))/(ULONG)65535;
			if(size<8) {
				size=8;
				if(fmconfig->flags&MDOUBLED) size*=2;
			}
			((struct Image*)gadg->GadgetRender)->Width=((struct Image*)gadg->SelectRender)->Width=size;
			createsliderimage((struct Image*)gadg->GadgetRender,0);
			createsliderimage((struct Image*)gadg->SelectRender,1);
			AddGadget(fmmain.ikkuna,gadg,oldpos);
 			RefreshGList(gadg,fmmain.ikkuna,0,1);
		}
	} else {
		if(pi->VertPot!=pot||pi->VertBody!=body) {
			NewModifyProp(gadg,fmmain.ikkuna,0,FREEVERT|PROPBORDERLESS,MAXPOT,pot,MAXBODY,body,1);
			oldpos=RemoveGadget(fmmain.ikkuna,gadg);
			size=((ULONG)body*(ULONG)(gadg->Height))/(ULONG)65535;
			if(size<8) size=8;
			((struct Image*)gadg->GadgetRender)->Height=((struct Image*)gadg->SelectRender)->Height=size;
			createsliderimage((struct Image*)gadg->GadgetRender,0);
			createsliderimage((struct Image*)gadg->SelectRender,1);
			AddGadget(fmmain.ikkuna,gadg,oldpos);
			RefreshGList(gadg,fmmain.ikkuna,0,1);
		}
	}
}
}

void setsliders(struct FMList *list)
{
if(list->li&&fmmain.ikkuna) {
	setpot(&list->li->slider1,1,list->totlines,list->li->visiblelines,list->firstline,0);
	setpot(&list->li->slider2,1,list->totlinesw,list->li->visiblelinesw,list->firstlinew,1);
}
}

/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include "dopuslib.h"

#define LIST_IDBASE 32767

#define GAD_SLIDER 0
#define GAD_UP     1
#define GAD_DOWN   2

extern char nullstring[];
//int _FPERR,_SIGFPE;

void DisplayView(struct DOpusListView *);
void dohilite(struct DOpusListView *,int);
void savepens(struct DOpusListView *);
void restorepens(struct DOpusListView *);
int scroll_view(struct DOpusListView *,int,int *,int);
int view_valid(struct DOpusListView *,int);

char nullstring[]="                                                                                                                                ";



/*****************************************************************************

    NAME */

	AROS_LH2(int, AddListView,

/*  SYNOPSIS */
	AROS_LHA(struct DOpusListView *, view, A0),
	AROS_LHA(int, count, D0),

/*  LOCATION */
	struct Library *, DOpusBase, 40, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)

	struct RastPort *rp;
	int a,addcount=0;

	while (view && (addcount<count)) {

		view->key=NULL; view->selected=NULL;
		view->gadadd=0; view->chk=view->itemselected;
		if (view->items)
			for (view->count=0;view->items[view->count] &&
				(!(view->flags&DLVF_ENDNL) || view->items[view->count][0]);view->count++);
		else view->count=0;

		if (!(view->listgads=(struct Gadget *)LAllocRemember(&view->key,sizeof(struct Gadget)*3,MEMF_CLEAR)) ||
			!(view->listimage=(struct Image *)LAllocRemember(&view->key,sizeof(struct Image),MEMF_CLEAR)) ||
			!(view->listprop=(struct PropInfo *)LAllocRemember(&view->key,sizeof(struct PropInfo),MEMF_CLEAR)) ||
			!(view->selected=(char *)LAllocRemember(&view->key,view->count,MEMF_CLEAR))) {
			LFreeRemember(&view->key);
			break;
		}

		if (view->selectarray) CopyMem(view->selectarray,view->selected,view->count);
		if (view->slidercol==-1) view->slidercol=1;
		if (view->sliderbgcol==-1) view->slidercol=0;
		if (view->textcol==-1) view->textcol=1;
		if (view->boxhi==-1) view->boxhi=1;
		if (view->boxlo==-1) view->boxlo=2;
		if (view->arrowfg==-1) view->arrowfg=1;
		if (view->arrowbg==-1) view->arrowbg=0;
		if (view->itemfg==-1) view->itemfg=1;
		if (view->itembg==-1) view->itembg=0;
		rp=view->window->RPort;
		view->fw=rp->Font->tf_XSize;
		view->fh=rp->Font->tf_YSize;
		if (view->fh<8) view->fh=8;
		view->fb=rp->Font->tf_Baseline;
		view->ty=(view->fh-8)/2;
		view->lines=view->h/view->fh;
		view->columns=view->w/view->fw;
		if (view->columns>128) view->columns=128;
		view->yo=view->y+((view->h-(view->lines*view->fh))/2);
		view->xo=view->x+((view->w-(view->columns*view->fw))/2);
		if (view->flags&DLVF_CHECK) {
			view->xo+=22;
			view->columns=(view->w-22)/view->fw;
		}
		if (view->topitem>(view->count-view->lines)) view->topitem=view->count-view->lines;
		if (view->topitem<0) view->topitem=0;

		savepens(view);
		SetDrMd(rp,JAM2);
		if (view->flags&DLVF_DUMB)
			Do3DBox(rp,view->x,view->y,view->w,view->h,view->boxlo,view->boxhi);
		else Do3DBox(rp,view->x,view->y,view->w,view->h,view->boxhi,view->boxlo);
		Do3DBox(rp,view->x+view->w+4,view->y,view->sliderwidth+4,view->h-16,view->boxhi,view->boxlo);
		DoArrow(rp,view->x+view->w+4,view->y+view->h-14,view->sliderwidth+4,6,view->arrowfg,view->arrowbg,0);
		Do3DBox(rp,view->x+view->w+4,view->y+view->h-14,view->sliderwidth+4,6,view->boxhi,view->boxlo);
		DoArrow(rp,view->x+view->w+4,view->y+view->h-6,view->sliderwidth+4,6,view->arrowfg,view->arrowbg,1);
		Do3DBox(rp,view->x+view->w+4,view->y+view->h-6,view->sliderwidth+4,6,view->boxhi,view->boxlo);

		if (view->title) {
			SetAPen(rp,view->textcol);
			a=strlen(view->title);
			if (view->flags&DLVF_TTOP) Move(rp,view->x,(view->y-view->fh-2)+view->fb);
			else Move(rp,view->x-6-(a*view->fw),view->yo+view->fb);
			Text(rp,view->title,a);
		}

		view->listimage->Width=view->sliderwidth; view->listimage->Height=view->h-18;
		view->listimage->Depth=1; view->listimage->PlaneOnOff=view->slidercol;
		view->listprop->Flags=FREEVERT|PROPBORDERLESS;
		view->listprop->VertBody=0xffff;
		view->listgads[0].NextGadget=&view->listgads[1];
		view->listgads[0].LeftEdge=view->x+view->w+6;
		view->listgads[0].TopEdge=view->y+1;
		view->listgads[0].Width=view->sliderwidth;
		view->listgads[0].Height=view->h-18;
		view->listgads[0].Flags=GADGHNONE;
		view->listgads[0].Activation=GADGIMMEDIATE|RELVERIFY|FOLLOWMOUSE;
		view->listgads[0].GadgetType=PROPGADGET;
		view->listgads[0].GadgetRender=(APTR)view->listimage;
		view->listgads[0].MutualExclude=view->sliderbgcol;
		view->listgads[0].SpecialInfo=(APTR)view->listprop;
		view->listgads[0].GadgetID=LIST_IDBASE+(view->listid*3);
		view->listgads[1].LeftEdge=view->x+view->w+2;
		view->listgads[1].TopEdge=view->y+view->h-15;
		view->listgads[1].Width=view->sliderwidth+8;
		view->listgads[1].Height=8;
		view->listgads[1].Flags=GADGHCOMP;
		view->listgads[1].Activation=GADGIMMEDIATE|RELVERIFY;
		view->listgads[1].GadgetType=BOOLGADGET;
		view->listgads[1].GadgetID=LIST_IDBASE+(view->listid*3)+1;

		CopyMem((char *)&view->listgads[1],(char *)&view->listgads[2],sizeof(struct Gadget));
		view->listgads[1].NextGadget=&view->listgads[2];
		view->listgads[2].TopEdge+=8;
		view->listgads[2].GadgetID++;

		if (view->flags&DLVF_HIREC)
			AddGadgetBorders(&view->key,&view->listgads[1],2,view->boxhi,view->boxlo);

		AddGList(view->window,view->listgads,-1,3,NULL);
		view->gadadd=3;
		view->mx=view->xo+(view->columns*view->fw)-1;
		view->my=view->yo+(view->lines*view->fh)-1;
		view->oldoffset=-1;
		restorepens(view);

		FixSliderBody(view->window,&view->listgads[0],view->count,view->lines,0);
		FixSliderPot(view->window,&view->listgads[0],view->topitem,view->count,view->lines,2);
		DisplayView(view);

		view=view->next;
		++addcount;
	}
	return(addcount);
	
	AROS_LIBFUNC_EXIT
}

/*****************************************************************************

    NAME */

	AROS_LH5(void, FixSliderBody,

/*  SYNOPSIS */
	AROS_LHA(struct Window *, win, A0),
	AROS_LHA(struct Gadget *, gad, A1),
	AROS_LHA(int, count, D0),
	AROS_LHA(int, lines, D1),
	AROS_LHA(int, show,  D2),

/*  LOCATION */
	struct Library *, DOpusBase, 45, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)


	USHORT body,gh,ih,min;
	struct Image *image;
	struct PropInfo *pinfo;
	float div;
	int vh,oh;

	pinfo=(struct PropInfo *)gad->SpecialInfo;
	if (pinfo->Flags&FREEVERT) {
		vh=0;
		gh=gad->Height;
		min=KNOBVMIN;
	}
	else {
		vh=1;
		gh=gad->Width;
		min=KNOBHMIN;
	}
	if (count<=lines) body=0xffff;
	else {
		body=((float)0x7fff)/(((float)count)/((float)lines));
		body<<=1;
	}
	image=(struct Image *)gad->GadgetRender;
	if (body==0) ih=min;
	else {
		div=(float)0xffff/(float)body;
		if (div==0) div=1;
		ih=(int)((float)((float)gh/div));
		if (ih<min) ih=min;
	}
	if (vh) pinfo->HorizBody=body;
	else pinfo->VertBody=body;
	if (pinfo->Flags&AUTOKNOB) {
		if (show && win)
			NewModifyProp(gad,win,NULL,pinfo->Flags,pinfo->HorizPot,pinfo->VertPot,
				pinfo->HorizBody,pinfo->VertBody,1);
	}
	else {
		if (vh) {
			oh=image->Width; image->Width=ih;
		}
		else {
			oh=image->Height; image->Height=ih;
		}
		if (show && win && (oh!=ih || show==2)) ShowSlider(win,gad);
	}
	
	AROS_LIBFUNC_EXIT
}

/*****************************************************************************

    NAME */

	AROS_LH6(void, FixSliderPot,

/*  SYNOPSIS */
	AROS_LHA(struct Window *, win, A0),
	AROS_LHA(struct Gadget *, gad, A1),
	AROS_LHA(int, off, 	D0),
	AROS_LHA(int, count, 	D1),
	AROS_LHA(int, lines, 	D2),
	AROS_LHA(int, show,  	D3),

/*  LOCATION */
	struct Library *, DOpusBase, 46, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)


	USHORT vert,vh,gh,ih,te,oh;
	float div;
	struct Image *image;
	struct PropInfo *pinfo;

	image=(struct Image *)gad->GadgetRender;
	pinfo=(struct PropInfo *)gad->SpecialInfo;
	if (pinfo->Flags&FREEVERT) {
		vh=0;
		gh=gad->Height; ih=image->Height;
	}
	else {
		vh=1;
		gh=gad->Width; ih=image->Width;
	}

	if (count<=lines) vert=0;
	else vert=(off*0xffff)/(count-lines);
	if (vert==0) te=0;
	else {
		div=(float)0xffff/(float)vert;
		if (div==0) div=1;
		te=(int)((float)((float)(gh-ih)/div));
	}
	if (vh) pinfo->HorizPot=vert;
	else pinfo->VertPot=vert;
	if (pinfo->Flags&AUTOKNOB) {
		if (show && win)
			NewModifyProp(gad,win,NULL,pinfo->Flags,pinfo->HorizPot,pinfo->VertPot,
				pinfo->HorizBody,pinfo->VertBody,1);
	}
	else {
		if (vh) {
			oh=image->LeftEdge; image->LeftEdge=te;
		}
		else {
			oh=image->TopEdge; image->TopEdge=te;
		}
		if (show && win && (oh!=te || show==2)) ShowSlider(win,gad);
	}
	
	AROS_LIBFUNC_EXIT
}



/*****************************************************************************

    NAME */

	AROS_LH2(void, ShowSlider,

/*  SYNOPSIS */
	AROS_LHA(struct Window *, win, A0),
	AROS_LHA(struct Gadget *, gad, A1),

/*  LOCATION */
	struct Library *, DOpusBase, 85, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)

	int ob,ot,y,old_pen,old_mode;
	struct Image *image;
	struct PropInfo *pinfo;
	struct RastPort *r;
	
// kprintf("ShowSlider()\n");

	pinfo=(struct PropInfo *)gad->SpecialInfo;

	if (pinfo->Flags&AUTOKNOB) {
		RefreshGList(gad,win,NULL,1);
		return;
	}

	image=(struct Image *)gad->GadgetRender;

	r=win->RPort;
	old_pen=r->FgPen;
	old_mode=r->DrawMode;

	SetAPen(r,gad->MutualExclude);
	SetDrMd(r,JAM1);

	if (pinfo->Flags&FREEVERT) {
		ob=image->TopEdge+image->Height;
		ot=image->TopEdge; y=gad->LeftEdge+gad->Width+1;
		if (ot>=0) RectFill(r,gad->LeftEdge-2,gad->TopEdge-1,y,gad->TopEdge+ot-1);
		if (ob<=gad->Height) RectFill(r,gad->LeftEdge-2,gad->TopEdge+ob,y,gad->TopEdge+gad->Height);
		if (gad->MutualExclude) {
			RectFill(r,gad->LeftEdge-2,gad->TopEdge+ot,gad->LeftEdge-1,gad->TopEdge+ob);
			RectFill(r,y-1,gad->TopEdge+ot,y,gad->TopEdge+ob);
		}
	}
	else {
		ob=image->LeftEdge+image->Width;
		ot=image->LeftEdge; y=gad->TopEdge+gad->Height+1;
		if (ot>=0) RectFill(r,gad->LeftEdge-2,gad->TopEdge-1,gad->LeftEdge+ot-1,y-1);
		if (ob<=gad->Width) RectFill(r,gad->LeftEdge+ob,gad->TopEdge-1,gad->LeftEdge+gad->Width+1,y-1);
		if (gad->MutualExclude) {
			RectFill(r,gad->LeftEdge+ot,gad->TopEdge-1,gad->LeftEdge+ob,gad->TopEdge-1);
			RectFill(r,gad->LeftEdge+ot,y-1,gad->LeftEdge+ob,y-1);
		}
	}

	DrawImage(r,image,gad->LeftEdge,gad->TopEdge);

	SetAPen(r,old_pen);
	SetDrMd(r,old_mode);
	
	AROS_LIBFUNC_EXIT
}

void DisplayView(view)
struct DOpusListView *view;
{
	struct RastPort *rp;
	int y,a,b,top,bot,dif,dir,start,end,step,w;
	char buf[128];
	static UWORD ditherdata[2]={0x8888,0x2222};

	savepens(view);
	if (view->oldoffset==-1 || (dif=(view->oldoffset-view->topitem))>=view->lines || dif<=-view->lines) {
		top=view->topitem;
		bot=view->topitem+view->lines-1;
		dir=0;
	}
	else if (dif<0) {
		top=view->topitem+view->lines+dif;
		bot=view->topitem+view->lines-1;
		dir=view->fh;
	}
	else if (dif>0) {
		top=view->topitem;
		bot=view->topitem+dif-1;
		dir=-view->fh;
	}
	else return;

	rp=view->window->RPort;
	SetAPen(rp,view->itemfg); SetBPen(rp,view->itembg);
	SetDrMd(rp,JAM2);
	start=view->topitem; end=view->topitem+view->lines; step=1;
	if (dir>0) y=view->yo+view->fb+(view->fh*(view->lines-1));
	else {
		y=view->yo+view->fb;
		if (dir) {
			start=end-1; end=view->topitem-1; step=-1;
		}
	}

	if (!dir && view->flags&DLVF_CHECK)
	{
	    ScrollRaster(rp,0,view->lines*view->fh,view->x,view->yo,view->xo-1,view->my);
	}

	w=view->w-(view->xo-view->x);
	for (a=start;a!=end;a+=step) {

		if (a>=top && a<=bot) {

			CopyMem(nullstring,buf,view->columns);
			if (view->items && a<view->count && view->items[a] &&
				(!(view->flags&DLVF_ENDNL) || view->items[a][0])) {
				b=strlen(view->items[a]); if (b>view->columns) b=view->columns;
				CopyMem(view->items[a],buf,b);
			}
			Move(rp,view->xo,y);

			if (dir) {
				if (dir<0) ClipBlit(rp,view->xo,view->yo,rp,view->xo,view->yo+view->fh,
					w,(view->lines-1)*view->fh,0xc0);
				else ClipBlit(rp,view->xo,view->yo+view->fh,rp,view->xo,view->yo,
					w,(view->lines-1)*view->fh,0xc0);
				if (view->flags&DLVF_CHECK) ScrollRaster(rp,0,dir,view->x,view->yo,view->xo-1,view->my);
			}

			Text(rp,buf,view->columns);
			if (view->selectarray && view->selectarray[a]&LVARRAY_DISABLED) {
				rp->AreaPtrn=ditherdata; rp->AreaPtSz=1;
				SetDrMd(rp,JAM1);
				SetAPen(rp,view->itembg);
				RectFill(rp,view->xo,y-view->fb,rp->cp_x-1,(y-view->fb)+view->fh-1);
				SetAPen(rp,view->itemfg);
				SetDrMd(rp,JAM2);
				rp->AreaPtrn=NULL; rp->AreaPtSz=0;
			}

			if (a<view->count) {
				if ((view->flags&DLVF_MULTI && view->selected[a]&LVARRAY_SELECTED) ||
					(view->flags&DLVF_LEAVE && view->itemselected==a)) {
					if (!(view->flags&DLVF_TOGGLE) || view->chk==view->itemselected) {
						if (view->flags&DLVF_CHECK) DrawCheckMark(rp,view->xo-18,y+view->ty-view->fb,1);
						else {
							SetDrMd(rp,COMPLEMENT);
							if (dir<0) b=0;
							else if (dir>0) b=view->lines-1;
							else b=a-view->topitem;
							dohilite(view,b);
							SetDrMd(rp,JAM2);
						}
					}
				}
			}

		}
		if (!dir) y+=view->fh;
	}
	view->oldoffset=view->topitem;
	restorepens(view);
	

}

/*****************************************************************************

    NAME */

	AROS_LH3(int, GetSliderPos,

/*  SYNOPSIS */
	AROS_LHA(struct Gadget *, gad, A0),
	AROS_LHA(int, count, D0),
	AROS_LHA(int, lines, D1),

/*  LOCATION */
	struct Library *, DOpusBase, 47, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)

	int i;
	USHORT vertpot;
	struct PropInfo *pinfo;
	float div;

	pinfo=(struct PropInfo *)gad->SpecialInfo;
	if (pinfo->Flags&FREEVERT) vertpot=pinfo->VertPot;
	else vertpot=pinfo->HorizPot;

	if (count<lines || vertpot==0) return(0);
	div=(float)0xffff/(float)vertpot;
	if (div==0) div=1;
	i=(int)((float)((float)((count-lines)+1)/div));
	if (i>(count-lines)) i=count-lines;
	if (i<0) i=0;
	return(i);
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH2(struct DOpusListView *, ListViewIDCMP,

/*  SYNOPSIS */
	AROS_LHA(struct DOpusListView *, view, A0),
	AROS_LHA(struct IntuiMessage *, imsg,  A1),

/*  LOCATION */
	struct Library *, DOpusBase, 41, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)

	int gadgetid,list,x,y,rep,xo,histate=0,offset,itemnum,newoffset,lastout=0,temp;
	ULONG class,idcmpflags;
	USHORT code;

	class=imsg->Class;
	code=imsg->Code;
	idcmpflags=view->window->IDCMPFlags;

	if (class==GADGETUP || class==GADGETDOWN) {
		gadgetid=((struct Gadget *)imsg->IAddress)->GadgetID;
		if (gadgetid<LIST_IDBASE) return((struct DOpusListView *)-1);
		gadgetid-=LIST_IDBASE; list=gadgetid/3;
		gadgetid%=3;
		while (view) {
			if (view->listid==list && view->window==imsg->IDCMPWindow) break;
			view=view->next;
		}
		if (!view) return((struct DOpusListView *)-1);
		ReplyMsg((struct Message *)imsg);

		if (!(idcmpflags&IDCMP_MOUSEMOVE)) ModifyIDCMP(view->window,idcmpflags|IDCMP_MOUSEMOVE);

		switch (gadgetid) {
			case GAD_SLIDER:
				view->topitem=GetSliderPos(&view->listgads[0],view->count,view->lines);
				DisplayView(view);
				if (view->sliderbgcol) ShowSlider(view->window,&view->listgads[0]);
				if (class==IDCMP_GADGETDOWN) {
					FOREVER {
						Wait(1<<view->window->UserPort->mp_SigBit);
						while ((imsg=(struct IntuiMessage *)GetMsg(view->window->UserPort))) {
							class=imsg->Class;
							ReplyMsg((struct Message *)imsg);
							if (class==IDCMP_MOUSEMOVE) {
								view->topitem=GetSliderPos(&view->listgads[0],view->count,view->lines);
								if (view->sliderbgcol) ShowSlider(view->window,&view->listgads[0]);
								DisplayView(view);
							}
							else if (class==IDCMP_GADGETUP) break;
						}
						if (class==IDCMP_GADGETUP) break;
					}
					view->topitem=GetSliderPos(&view->listgads[0],view->count,view->lines);
					if (view->sliderbgcol) ShowSlider(view->window,&view->listgads[0]);
					DisplayView(view);
				}
				break;

			case GAD_UP:
				if (view->topitem==0) break;
				--view->topitem;
			case GAD_DOWN:
				if (gadgetid==GAD_DOWN) {
					if (view->topitem>view->count-(view->lines+1)) break;
					++view->topitem;
				}
				FixSliderPot(view->window,&view->listgads[0],view->topitem,view->count,view->lines,1);
				DisplayView(view);
				Delay(5);
				FOREVER {
					if ((imsg=(struct IntuiMessage *)GetMsg(view->window->UserPort))) {
						class=imsg->Class; code=imsg->Code;
						ReplyMsg((struct Message *)imsg);
						if (class==IDCMP_GADGETUP || (class==IDCMP_MOUSEBUTTONS && code==SELECTUP))
							break;
					}
					if (gadgetid==GAD_UP) {
						if (view->topitem==0) break;
						--view->topitem;
					}
					else {
						if (view->topitem>view->count-(view->lines+1)) break;
						++view->topitem;
					}
					FixSliderPot(view->window,&view->listgads[0],view->topitem,view->count,view->lines,1);
					DisplayView(view);
					if (view->flags&DLVF_SLOW) Delay(1);
					else WaitTOF();
				}
				break;
		}
		ModifyIDCMP(view->window,idcmpflags);
		return(NULL);
	}
	else if (class==IDCMP_MOUSEBUTTONS && code==SELECTDOWN) {
		x=imsg->MouseX;
		y=imsg->MouseY;

		while (view) {
			if (view->window==imsg->IDCMPWindow && !(view->flags&DLVF_DUMB)) {
				if (view->flags&DLVF_CHECK) xo=view->xo-22;
				else xo=view->xo;
				if (x>=xo && x<view->mx && y>=view->yo && y<view->my) break;
			}
			view=view->next;
		}
		if (!view) return((struct DOpusListView *)-1);
		ReplyMsg((struct Message *)imsg);

		offset=(y-view->yo)/view->fh;
		itemnum=offset+view->topitem;
		if (itemnum>=view->count) {
			itemnum=view->count-1;
			offset=itemnum-view->topitem;
			if (offset<0) return(NULL);
		}

		ModifyIDCMP(view->window,idcmpflags|IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE|IDCMP_INTUITICKS);
		if (!(rep=view->window->Flags&WFLG_REPORTMOUSE))
			view->window->Flags|=WFLG_REPORTMOUSE;

		savepens(view);
		if (view_valid(view,itemnum)) {
			dohilite(view,offset);
			histate=1;
		}

		FOREVER {
			while ((imsg=(struct IntuiMessage *)GetMsg(view->window->UserPort))) {
				class=imsg->Class;
				code=imsg->Code;
				x=imsg->MouseX;
				y=imsg->MouseY;
				ReplyMsg((struct Message *)imsg);

				if (class==IDCMP_MOUSEBUTTONS) {
					if (code==SELECTUP) break;
					if (code==MENUUP) {
						if (histate) {
							histate=0;
							dohilite(view,offset);
						}
						break;
					}
				}
				else if (class==IDCMP_INTUITICKS) {
					if ((newoffset=((y-view->yo)/view->fh))!=offset) {
						newoffset=scroll_view(view,newoffset,&histate,offset);
						if (newoffset>=0 && newoffset<view->lines) {
							offset=newoffset;
							temp=itemnum;
							itemnum=view->topitem+offset;
							if (itemnum!=temp && view_valid(view,itemnum)) {
								histate=1;
								dohilite(view,offset);
							}
						}
					}
				}
				else if (class==IDCMP_MOUSEMOVE) {
					if ((newoffset=((y-view->yo)/view->fh))!=offset || x<xo || x>=view->mx || y<view->yo) {
						if (y<view->yo || y>view->yo+view->h) {
							if (lastout) continue;
							lastout=1;
						}
						else lastout=0;
						if (view->count<view->lines &&
							newoffset>=view->count &&
							offset>=view->count-1 && histate) continue;
						if (histate) {
							histate=0;
							dohilite(view,offset);
						}
						if (x<xo || x>=view->mx) continue;
						newoffset=scroll_view(view,newoffset,&histate,0);
						if (newoffset>=0 && newoffset<view->lines) {
							offset=newoffset;
							itemnum=view->topitem+offset;
							if (view_valid(view,itemnum)) {
								histate=1;
								dohilite(view,offset);
							}
						}
					}
					else if (!histate) {
						if (view_valid(view,itemnum)) {
							histate=1;
							dohilite(view,offset);
						}
					}
				}
			}
			if (class==IDCMP_MOUSEBUTTONS && (code==SELECTUP || code==MENUUP)) break;
			Wait(1<<view->window->UserPort->mp_SigBit);
		}

		if (histate) {
			if (view->flags&DLVF_MULTI) {
				view->selected[itemnum]^=LVARRAY_SELECTED;
				if (view->flags&DLVF_CHECK) {
					dohilite(view,offset);
					SetAPen(view->window->RPort,view->itemfg);
					DrawCheckMark(view->window->RPort,
						view->xo-18,
						view->yo+(view->fh*offset)+view->ty,
						(view->selected[itemnum]&LVARRAY_SELECTED));
				}
			}
			else if (view->flags&DLVF_LEAVE) {
				dohilite(view,offset);
				SetAPen(view->window->RPort,view->itemfg);
				if (view->itemselected==itemnum) {
					if (view->flags&DLVF_TOGGLE) {
						if (view->chk==view->itemselected) {
							if (view->flags&DLVF_CHECK) {
								DrawCheckMark(view->window->RPort,
									view->xo-18,
									view->yo+(view->fh*offset)+view->ty,
									0);
							}
							else dohilite(view,offset);
							view->chk=-1;
						}
						else {
							if (view->flags&DLVF_CHECK) {
								DrawCheckMark(view->window->RPort,
									view->xo-18,
									view->yo+(view->fh*offset)+view->ty,
									1);
							}
							else dohilite(view,offset);
							view->chk=view->itemselected;
						}
					}
				}
				else {
					if (view->flags&DLVF_CHECK) {
						DrawCheckMark(view->window->RPort,
							view->xo-18,
							view->yo+(view->fh*offset)+view->ty,
							1);
					}
					else dohilite(view,offset);
					if (view->chk>-1) {
						y=view->chk-view->topitem;
						if (y>=0 && y<view->lines) {
							if (view->flags&DLVF_CHECK) {
								DrawCheckMark(view->window->RPort,
									view->xo-18,
									view->yo+(view->fh*y)+view->ty,
									0);
							}
							else dohilite(view,y);
						}
					}
					view->chk=itemnum;
				}
			}
			else dohilite(view,offset);
		}
		if (!rep) view->window->Flags&=~WFLG_REPORTMOUSE;
		ModifyIDCMP(view->window,idcmpflags);
		restorepens(view);
		if (histate) {
			view->itemselected=itemnum;
			return(view);
		}
		return(NULL);
	}
	return((struct DOpusListView *)-1);
	
	AROS_LIBFUNC_EXIT
}

void dohilite(view,a)
struct DOpusListView *view;
int a;
{
	int x,mode;

	mode=view->window->RPort->DrawMode;
	SetDrMd(view->window->RPort,COMPLEMENT);
	if (view->flags&DLVF_CHECK) x=view->xo-22;
	else x=view->xo;
	RectFill(view->window->RPort,x,view->yo+(view->fh*a),view->mx,view->yo+(view->fh*(a+1))-1);
	SetDrMd(view->window->RPort,mode);
}

void savepens(view)
struct DOpusListView *view;
{
	view->ofg=view->window->RPort->FgPen;
	view->obg=view->window->RPort->BgPen;
	view->odm=view->window->RPort->DrawMode;
}

void restorepens(view)
struct DOpusListView *view;
{
	SetAPen(view->window->RPort,view->ofg);
	SetBPen(view->window->RPort,view->obg);
	SetDrMd(view->window->RPort,view->odm);
}


/*****************************************************************************

    NAME */

	AROS_LH2(int, RefreshListView,

/*  SYNOPSIS */
	AROS_LHA(struct DOpusListView *, view, A0),
	AROS_LHA(int, count, D0),

/*  LOCATION */
	struct Library *, DOpusBase, 42, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)

	int realcount=0;

	while (view && realcount<count) {
		if (view->items)
			for (view->count=0;view->items[view->count] &&
				(!(view->flags&DLVF_ENDNL) || view->items[view->count][0]);view->count++);
		else view->count=0;
		if (view->selectarray) {
			if ((view->selected=(char *)LAllocRemember(&view->key,view->count,MEMF_CLEAR)))
				CopyMem(view->selectarray,view->selected,view->count);
		}
		if (view->topitem>(view->count-view->lines)) view->topitem=view->count-view->lines;
		if (view->topitem<0) view->topitem=0;
		view->chk=view->itemselected;
		FixSliderBody(view->window,&view->listgads[0],view->count,view->lines,0);
		FixSliderPot(view->window,&view->listgads[0],view->topitem,view->count,view->lines,2);
		view->oldoffset=-1;
		DisplayView(view);
		view=view->next;
		++realcount;
	}
	return(realcount);
	
	AROS_LIBFUNC_EXIT
}

/*****************************************************************************

    NAME */

	AROS_LH2(int, RemoveListView,

/*  SYNOPSIS */
	AROS_LHA(struct DOpusListView *, view, A0),
	AROS_LHA(int, count, D0),

/*  LOCATION */
	struct Library *, DOpusBase, 43, DOpus)
/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS

    SEE ALSO

    INTERNALS

    HISTORY
	27-11-96    digulla automatically created from
			    asl_lib.fd and clib/asl_protos.h

*****************************************************************************/
{
	AROS_LIBFUNC_INIT
	AROS_LIBBASE_EXT_DECL(struct Library *,DOpusBase)


	int realcount=0;

	while (view && realcount<count) {
		if (view->gadadd) RemoveGList(view->window,view->listgads,view->gadadd);
		LFreeRemember(&view->key);
		view=view->next;
		++realcount;
	}
	return(realcount);
	
	AROS_LIBFUNC_EXIT
}

int scroll_view(view,offset,histate,oldoffset)
struct DOpusListView *view;
int offset,*histate,oldoffset;
{
	int draw=0;

	if (view->count<view->lines && offset>=view->count)
		return(view->count-1);

	if (offset<0) {
		if (view->topitem>0) {
			--view->topitem;
			draw=1;
		}
		offset=0;
	}
	else if (offset>=view->lines) {
		if (view->topitem<=view->count-(view->lines+1)) {
			++view->topitem;
			draw=1;
		}
		offset=view->lines-1;
	}

	if (draw) {
		if (*histate) {
			*histate=0;
			dohilite(view,oldoffset);
		}
		FixSliderPot(view->window,&view->listgads[0],view->topitem,view->count,view->lines,1);
		DisplayView(view);
	}
	return(offset);
}

int view_valid(view,itemnum)
struct DOpusListView *view;
int itemnum;
{
	if (itemnum<view->count &&
		(!view->selectarray || !(view->selectarray[itemnum]&LVARRAY_DISABLED)))
		return(1);
	return(0);
}

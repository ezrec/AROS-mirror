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

#define isinside(x,y,x1,y1,x2,y2) (x>=x1 && x<=x2 && y>=y1 && y<=y2)

void ShowRMBGadName(struct RastPort *,struct RMBGadget *,int);
void HighlightRMBGad(struct RastPort *,struct RMBGadget *,int);
int makeusstring(char *,char *,int *,int);


/*****************************************************************************

    NAME */

	AROS_LH7(int, AddGadgets,

/*  SYNOPSIS */
	AROS_LHA(struct Window *, win, A0),
	AROS_LHA(struct Gadget *, firstgad, A1),
	AROS_LHA(char **, text, A2),
	AROS_LHA(int, count, 	D0),
	AROS_LHA(int, fg, 	D1),
	AROS_LHA(int, bg, 	D2),
	AROS_LHA(int, add, 	D3),

/*  LOCATION */
	struct Library *, DOpusBase, 61, DOpus)
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

	int num=0,realcount=0,of,ob,a,x,y,b,c,up,xp,yp,bl,len,offset;
	char buf[80];
	struct RastPort *rp;
	struct Gadget *gad;

	rp=win->RPort; gad=firstgad;
	of=rp->FgPen; ob=rp->BgPen;
	x=rp->Font->tf_XSize; y=rp->Font->tf_YSize; bl=rp->Font->tf_Baseline;

	while (gad && num<count) {
		while (text && text[realcount] && text[realcount]!=(char *)-1 &&
			!text[realcount][0]) ++realcount;
		if (text && !text[realcount]) text=NULL;

		if (gad->GadgetType==GTYP_STRGADGET) {
			Do3DStringBox(rp,
				gad->LeftEdge-(gad->MutualExclude*2),gad->TopEdge-gad->MutualExclude,
				gad->Width+(gad->MutualExclude*4),gad->Height+(gad->MutualExclude*2),
				fg,bg);
		}
		else if (gad->GadgetType==GTYP_PROPGADGET) {
			Do3DBox(rp,
				gad->LeftEdge-2,gad->TopEdge-1,
				gad->Width+4,gad->Height+2,
				fg,bg);
		}
		else if (gad->MutualExclude==GAD_CYCLE) {
			Do3DCycleBox(rp,
				gad->LeftEdge+2,gad->TopEdge+1,
				gad->Width-4,gad->Height-2,
				fg,bg);
		}
		else if (gad->MutualExclude==GAD_CHECK) {
			Do3DBox(rp,
				gad->LeftEdge+2,gad->TopEdge+1,
				22,9,
				fg,bg);
		}
		else if (gad->MutualExclude!=GAD_NONE && gad->MutualExclude!=GAD_RADIO) {
			Do3DBox(rp,
				gad->LeftEdge+2,gad->TopEdge+1,
				gad->Width-4,gad->Height-2,
				fg,bg);
		}

		SetAPen(rp,of);
		SetBPen(rp,ob);

		if (text && text[realcount] && text[realcount]!=(char *)-1 &&
			text[realcount][0]) {
			a=strlen(text[realcount]);
			up=-1;
			for (b=0,c=0;b<a;b++) {
				if (text[realcount][b]=='_' && up==-1) up=c;
				else buf[c++]=text[realcount][b];
			}
			buf[c]=0;
			if (up>-1) --a;
			len=TextLength(rp,text[realcount],a);

			if (gad->GadgetType==GTYP_STRGADGET ||
				(gad->GadgetType==GTYP_PROPGADGET && !(((struct PropInfo *)gad->SpecialInfo)->Flags&FREEHORIZ)) ||
				gad->MutualExclude==GAD_GLASS ||
				gad->MutualExclude==GAD_CYCLE) {

				if (gad->GadgetType==GTYP_STRGADGET) offset=gad->MutualExclude;
				else offset=0;

				xp=gad->LeftEdge-(offset*2)-(len+x);
				yp=gad->TopEdge+((gad->Height+(offset*2)-y)/2)+bl-offset;
			}
			else if (gad->GadgetType==GTYP_PROPGADGET) {
				xp=gad->LeftEdge+((gad->Width-len)/2);
				yp=gad->TopEdge-(y*2)+bl;
			}
			else {
				switch (gad->MutualExclude) {
					case GAD_CHECK:
						xp=gad->LeftEdge+36;
						break;
					case GAD_RADIO:
						xp=gad->LeftEdge+gad->Width+8;
						break;
					default:
						xp=gad->LeftEdge+((gad->Width-len)/2);
						break;
				}
				yp=gad->TopEdge+((((gad->MutualExclude==GAD_CHECK)?12:gad->Height)-y)/2)+bl;
			}

			UScoreText(rp,buf,xp,yp,up);
		}

		if (gad->GadgetType==GTYP_BOOLGADGET &&
			gad->MutualExclude!=GAD_CHECK &&
			gad->MutualExclude!=GAD_RADIO &&
			gad->Activation&GACT_TOGGLESELECT &&
			gad->Flags&GFLG_SELECTED) HiliteGad(gad,rp);

		gad=gad->NextGadget;
		++num;
		++realcount;
	}
	if (add) {
		AddGList(win,firstgad,-1,realcount,NULL);
		gad=firstgad;
		for (a=0;a<realcount && gad;a++) {
			if (gad->GadgetType!=GTYP_PROPGADGET) {
				if (gad->GadgetType==GTYP_STRGADGET ||
					gad->MutualExclude==GAD_RADIO ||
					(gad->GadgetType==GTYP_BOOLGADGET && gad->MutualExclude==GAD_CHECK &&
						gad->Activation&GACT_TOGGLESELECT && gad->Flags&GFLG_SELECTED))
						RefreshGList(gad,win,NULL,1);
			}
			gad=gad->NextGadget;
		}
	}
	return(realcount);
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH2(int, DoRMBGadget,

/*  SYNOPSIS */
	AROS_LHA(struct RMBGadget *, gad,	A0),
	AROS_LHA(struct Window *, window,	A1),

/*  LOCATION */
	struct Library *, DOpusBase, 60, DOpus)
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

	ULONG class;
	USHORT code;
	BOOL inside;
	struct RastPort *rp;
	int x,y,x1,y1,ret=-1;
	ULONG idcmpflags,windowflags;
	struct IntuiMessage *Msg;
	struct Gadget dummy_gadget;

	x=window->MouseX; y=window->MouseY;
	while (gad) {
		if (gad->w>0 && gad->h>0) {
			x1=gad->x+gad->w-1; y1=gad->y+gad->h-1;
			if (isinside(x,y,gad->x,gad->y,x1,y1)) break;
		}
		gad=gad->next;
	}
	if (!gad) return(-1);
	rp=window->RPort;

	idcmpflags=window->IDCMPFlags;
	ModifyIDCMP(window,IDCMP_MOUSEBUTTONS|IDCMP_MOUSEMOVE|IDCMP_INACTIVEWINDOW);

	windowflags=window->Flags;
	window->Flags|=WFLG_REPORTMOUSE;

	if (isinside(x,y,gad->x,gad->y,x1,y1)) {
		if (gad->flags&RGF_ALTTEXT) ShowRMBGadName(rp,gad,1);
		HighlightRMBGad(rp,gad,1);
		inside=TRUE;
	}
	else inside=FALSE;

	dummy_gadget.LeftEdge=0;
	dummy_gadget.TopEdge=0;
	dummy_gadget.Width=window->Width;
	dummy_gadget.Height=window->Height;
	dummy_gadget.Flags=GFLG_GADGHNONE;
	dummy_gadget.Activation=GACT_IMMEDIATE;
	dummy_gadget.GadgetType=GTYP_BOOLGADGET;
	dummy_gadget.GadgetRender=NULL;
	dummy_gadget.SelectRender=NULL;
	dummy_gadget.GadgetText=NULL;
	AddGList(window,&dummy_gadget,0,1,NULL);

	FOREVER {
		while ((Msg=(struct IntuiMessage *)GetMsg(window->UserPort))) {
			class=Msg->Class;
			code=Msg->Code;
			x=Msg->MouseX;
			y=Msg->MouseY;
			ReplyMsg((struct Message *)Msg);

			if (class==IDCMP_INACTIVEWINDOW) goto end_rmb;
			if (class==IDCMP_MOUSEBUTTONS && code==MENUUP) {
				if (inside) ret=gad->id;
				goto end_rmb;
			}
			if (class==IDCMP_MOUSEMOVE) {
				if (!(isinside(x,y,gad->x,gad->y,x1,y1))) {
					if (inside) {
						HighlightRMBGad(rp,gad,0);
						if (gad->flags&RGF_ALTTEXT) ShowRMBGadName(rp,gad,0);
						inside=FALSE;
					}
				}
				else {
					if (!inside) {
						if (gad->flags&RGF_ALTTEXT) ShowRMBGadName(rp,gad,1);
						HighlightRMBGad(rp,gad,1);
						inside=TRUE;
					}
				}
			}
		}
		Wait(1<<window->UserPort->mp_SigBit);
	}
end_rmb:
	if (inside) {
		HighlightRMBGad(rp,gad,0);
		if (gad->flags&RGF_ALTTEXT) ShowRMBGadName(rp,gad,0);
	}
	RemoveGList(window,&dummy_gadget,1);
	ModifyIDCMP(window,idcmpflags);
	window->Flags=windowflags;
	return(ret);
	
	AROS_LIBFUNC_EXIT
}

void ShowRMBGadName(rp,gad,a)
struct RastPort *rp;
struct RMBGadget *gad;
int a;
{
	struct RMBGadgetText *text;
	int len,l,uspos,old;
	char buf[100];

	if ((text=&gad->txt[a])) {
		old=rp->DrawMode;
		SetDrMd(rp,JAM2);
		SetAPen(rp,text->bg);
		RectFill(rp,gad->x+text->x,gad->y+text->y,gad->x+text->x+text->w-1,gad->y+text->y+text->h-1);
		SetAPen(rp,text->fg);
		SetBPen(rp,text->bg);
		if (text->text) {
			len=makeusstring(text->text,buf,&uspos,99);
			while ((l=TextLength(rp,buf,len))>text->w) {
				if ((--len)==0) break;
			}
			buf[len]=0;
			UScoreText(rp,buf,gad->x+text->x+((text->w-l)/2),
				gad->y+text->y+((text->h-rp->Font->tf_YSize)/2)+rp->Font->tf_Baseline,uspos);
		}
		SetDrMd(rp,old);
	}
}

void HighlightRMBGad(rp,gad,state)
struct RastPort *rp;
struct RMBGadget *gad;
int state;
{
	if (gad->flags&RGF_ALTBORD) DrawBorder(rp,gad->high_bord[state],gad->x,gad->y);
	else {
		int old;

		old=rp->DrawMode;
		SetDrMd(rp,COMPLEMENT);
		RectFill(rp,gad->x,gad->y,gad->x+gad->w-1,gad->y+gad->h-1);
		SetDrMd(rp,old);
	}
}

int makeusstring(from,to,uspos,size)
char *from,*to;
int *uspos,size;
{
	int len,c;

	*uspos=-1;
	for (c=0,len=0;len<size;c++) {
		if (!from[c]) break;
		else if (from[c]=='_' && *uspos==-1) *uspos=len;
		else to[len++]=from[c];
	}
	to[len]=0;
	return(len);
}



/*****************************************************************************

    NAME */

	AROS_LH4(void, DoCycleGadget,

/*  SYNOPSIS */
	AROS_LHA(struct Gadget *, gad,		A0),
	AROS_LHA(struct Window *, window,	A1),
	AROS_LHA(char **	, choices,	A2),
	AROS_LHA(int 		, select,	D0),

/*  LOCATION */
	struct Library *, DOpusBase, 78, DOpus)
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

	int op,a;
	struct RastPort *rp;

	rp=window->RPort;
	op=rp->FgPen;
	SetAPen(rp,rp->BgPen);
	RectFill(rp,gad->LeftEdge+22,gad->TopEdge+1,
		gad->LeftEdge+gad->Width-3,gad->TopEdge+gad->Height-2);
	SetAPen(rp,op);
	if (choices && choices[select]) {
		Move(rp,gad->LeftEdge+20+((gad->Width-22-((a=strlen(choices[select]))*rp->Font->tf_XSize))/2),
			gad->TopEdge+rp->Font->tf_Baseline+((gad->Height-rp->Font->tf_YSize)/2));
		Text(rp,choices[select],a);
	}
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH5(void, UScoreText,

/*  SYNOPSIS */
	AROS_LHA(struct RastPort *, rp,		A0),
	AROS_LHA(char *, buf,	A1),
	AROS_LHA(int, xp,	D0),
	AROS_LHA(int, yp,	D1),
	AROS_LHA(int, up,	D2),

/*  LOCATION */
	struct Library *, DOpusBase, 79, DOpus)
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

	int a,x;

	Move(rp,xp,yp); Text(rp,buf,(a=strlen(buf)));
	if (up>-1 && up<a) {
		x=TextLength(rp,buf,up);
		Move(rp,xp+x,yp+2);
		Draw(rp,rp->cp_x+(TextLength(rp,&buf[up],1))-1,rp->cp_y);
	}
	
	AROS_LIBFUNC_EXIT
}

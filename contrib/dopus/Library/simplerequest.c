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

/* AROS: It uses some obsolete values */
#include <intuition/iobsolete.h>

#include "dopuslib.h"


char *get_uscore_text(struct DOpusRemember **,char *,int);

static char *contstring[]={"_Continue"};

#include <proto/exec.h>

/*****************************************************************************

    NAME */

	AROS_LH2(int, DoSimpleRequest,

/*  SYNOPSIS */
	AROS_LHA(struct Window *, window, A0),
	AROS_LHA(struct DOpusSimpleRequest *, simple, A1),

/*  LOCATION */
	struct Library *, DOpusBase, 68, DOpus)
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

	int
		textlen,a,b,c,cw,cy,cb,sw,height,maxlen,llen,num,gwidth=0,gspace,gx,ty,ac,ogy,
		gadgetid,ls,gy,last,gnum,lines,dl,strgad,sfy,hnum,rows=1,macc,ogx,
		maxwidth,winwidth,winheight,xoffset,yoffset,d;
	struct NewWindow *newwindow;
	struct DOpusRemember *key;
	struct Gadget *gadgets,*contgad,*gad,*glassgad=NULL;
	struct Screen *screen;
	struct Window *Window;
	struct RastPort *rp,testrp;
	struct TextFont *fnt,*tfnt;
	struct StringInfo *sinfo;
	struct IntuiMessage *Msg;
	struct StringExtend *extend;
	char *text,*keys,*buf,ch,*strbuf,**gadptr;
	ULONG class;
	USHORT code;

	if (!simple) return(0);
	if (!window) {
		screen=IntuitionBase->FirstScreen;
		fnt=GfxBase->DefaultFont;
	}
	else {
		screen=window->WScreen;
		fnt=window->RPort->Font;
	}
	if (simple->font) fnt=simple->font;
	if (IntuitionBase->LibNode.lib_Version>36) tfnt=fnt;
	else tfnt=screen->RastPort.Font;
	InitRastPort(&testrp);
	SetFont(&testrp,fnt);

	if (simple->flags&SRF_BORDERS) maxwidth=screen->Width-screen->WBorLeft-screen->WBorRight;
	else maxwidth=screen->Width;

	ac=macc=0;
	for (a=0,num=0,hnum=0;;a++) {
		if (!simple->gads[a]) break;
		if (simple->gads[a][0]) {
			if (simple->gads[a][0]=='\n') {
				++rows;
				++hnum;
				if (ac>macc) macc=ac;
				ac=0;
			}
			else {
				if ((b=TextLength(&testrp,simple->gads[a],strlen(simple->gads[a]))+fnt->tf_XSize)>gwidth)
					gwidth=b;
				++num; ++ac;
			}
		}
		else {
			++hnum;
			++ac;
		}
	}
	gnum=num;
	if (simple->strbuf) ++num;
	if (ac>macc) macc=ac;

	key=NULL; textlen=strlen(simple->text);

	cw=fnt->tf_XSize; cy=fnt->tf_YSize; cb=fnt->tf_Baseline;
	sw=(maxwidth/cw)-2; sfy=tfnt->tf_YSize;

	if (!(text=(char *)LAllocRemember(&key,textlen+2,MEMF_CLEAR)) ||
		!(keys=(char *)LAllocRemember(&key,gnum,MEMF_CLEAR)) ||
		!(newwindow=(struct NewWindow *)LAllocRemember(&key,sizeof(struct NewWindow),MEMF_CLEAR)) ||
		!(gadgets=(struct Gadget *)LAllocRemember(&key,sizeof(struct Gadget)*(num+1),MEMF_CLEAR)) ||
		!(contgad=(struct Gadget *)LAllocRemember(&key,sizeof(struct Gadget),MEMF_CLEAR)) ||
		!(gadptr=(char **)LAllocRemember(&key,(num+hnum+1)*4,MEMF_CLEAR)) ||
		!(buf=(char *)LAllocRemember(&key,sw+2,0))) {
		LFreeRemember(&key);
		return(0);
	}

	if (simple->strbuf) {
		if (!(sinfo=(struct StringInfo *)LAllocRemember(&key,sizeof(struct StringInfo),MEMF_CLEAR)) ||
			!(extend=(struct StringExtend *)LAllocRemember(&key,sizeof(struct StringExtend),MEMF_CLEAR)) ||
			!(strbuf=(char *)LAllocRemember(&key,simple->strlen+1,MEMF_CLEAR))) {
			LFreeRemember(&key);
			return(0);
		}
		strcpy(strbuf,simple->strbuf);
		if (simple->flags&SRF_GLASS)
			glassgad=LAllocRemember(&key,sizeof(struct Gadget),MEMF_CLEAR);
	}

	CopyMem(simple->text,text,textlen);
	if (text[textlen-1]!=10) {
		text[textlen]=10;
		++textlen;
	}

	height=maxlen=llen=ls=0;
	for (a=0;a<textlen;a++) {
		if (text[a]==10 || llen==sw) {
			b=a;
			if (llen==sw) {
				while (a>ls) {
					if (text[a]==' ' || text[a]=='.' || text[a]==',') {
						llen=a-ls;
						text[a]=10;
						break;
					}
					--a; --llen;
				}
				if (a==ls) {
					a=b;
					llen=a-ls;
				}
			}
			height+=cy+1;
			if ((llen=TextLength(&testrp,&text[ls],llen)+fnt->tf_XSize)>maxlen)
				maxlen=llen;
			llen=0; ls=a;
		}
		else ++llen;
	}

	if (gwidth<100) gwidth=100;
	if (gwidth*macc>maxwidth) {
		hnum=0;
		gwidth=(gnum)?maxwidth/gnum:0;
	}

	newwindow->DetailPen=255;
	newwindow->BlockPen=255;
	newwindow->IDCMPFlags=IDCMP_GADGETUP|IDCMP_VANILLAKEY|IDCMP_MOUSEBUTTONS;
	newwindow->Flags=WFLG_ACTIVATE|WFLG_BORDERLESS|WFLG_RMBTRAP;
	newwindow->Type=CUSTOMSCREEN;

	if (simple->flags&SRF_BORDERS) {
		xoffset=screen->WBorLeft;
		yoffset=screen->WBorTop+screen->Font->ta_YSize+1;
		newwindow->Title=simple->title;
	}
	else {
		xoffset=0;
		yoffset=0;
	}

	newwindow->Width=maxlen+8;
	a=(gwidth*macc)+((macc+2)*10);
	if (simple->strbuf && a<((42*tfnt->tf_XSize)+20)) a=(42*tfnt->tf_XSize)+20;
	if (simple->flags&SRF_BORDERS) {
		newwindow->Width+=screen->WBorLeft+screen->WBorRight;
		a+=screen->WBorLeft+screen->WBorRight;
		newwindow->Flags=WFLG_ACTIVATE|WFLG_RMBTRAP|WFLG_DRAGBAR|WFLG_DEPTHGADGET;
	}
	if (newwindow->Width<a) newwindow->Width=a;
	if (newwindow->Width>screen->Width) newwindow->Width=screen->Width;

	newwindow->Height=height+16+(num?((cy+5)*rows):0);
	if (simple->flags&SRF_BORDERS) newwindow->Height+=yoffset+screen->WBorBottom;
	if (simple->strbuf) newwindow->Height+=sfy+4;
	if (newwindow->Height>screen->Height) newwindow->Height=screen->Height;

	if (window) {
		newwindow->LeftEdge=window->LeftEdge+((window->Width-newwindow->Width)/2);
		newwindow->TopEdge=window->TopEdge+((window->Height-newwindow->Height)/2);
	}
	else {
		newwindow->LeftEdge=(screen->Width-newwindow->Width)/2;
		newwindow->TopEdge=(screen->Height-newwindow->Height)/2;
	}

	if (newwindow->LeftEdge+newwindow->Width>screen->Width)
		newwindow->LeftEdge=screen->Width-newwindow->Width;
	if (newwindow->TopEdge+newwindow->Height>screen->Height)
		newwindow->TopEdge=screen->Height-newwindow->Height;
	if (newwindow->LeftEdge<0) newwindow->LeftEdge=0;
	if (newwindow->TopEdge<0) newwindow->TopEdge=0;

	newwindow->Screen=screen;

	winwidth=newwindow->Width;
	winheight=newwindow->Height;

	if (simple->flags&SRF_BORDERS) {
		winwidth-=screen->WBorLeft+screen->WBorRight;
		winheight-=yoffset+screen->WBorBottom;
	}

	ogx=gx=(winwidth-(gwidth*macc))/(macc+1);
	gspace=gx+gwidth;

	ogy=gy=winheight-4-(num?((cy+5)*rows):0);
	if (simple->strbuf) lines=(gy-(sfy+8)-(7+cb))/(cy+1);
	else lines=(gy-(7+cb))/(cy+1);

	last=a=0; strgad=1;
	for (b=0;b<(num+hnum);b++) {
		if (last && simple->strbuf) {
			gadgets[a].LeftEdge=10+cw+xoffset;
			c=cb+7+(lines*cy);
			gadgets[a].TopEdge=c+(((ogy-c)-sfy)/2)+yoffset+1;
			gadgets[a].Width=winwidth-20-(cw*2);
			gadgets[a].Height=sfy;
			gadgets[a].Flags=GFLG_GADGHCOMP|GFLG_STRINGEXTEND;
			gadgets[a].Activation=GACT_RELVERIFY|GACT_TOGGLESELECT;
			if (simple->flags&SRF_LONGINT) gadgets[a].Activation|=LONGINT;
			if (simple->flags&SRF_CENTJUST) gadgets[a].Activation|=STRINGCENTER;
			if (simple->flags&SRF_RIGHTJUST) gadgets[a].Activation|=STRINGRIGHT;
			gadgets[a].GadgetType=GTYP_STRGADGET;
			gadgets[a].GadgetID=65535;
			gadgets[a].SpecialInfo=(APTR)sinfo;
			sinfo->Buffer=strbuf;
			sinfo->UndoBuffer=LAllocRemember(&key,simple->strlen+2,MEMF_CLEAR);
			sinfo->MaxChars=simple->strlen+1;

			if (simple->flags&SRF_EXTEND)
				CopyMem((char *)simple->value,(char *)extend,sizeof(struct StringExtend));
			else {
				extend->Font=fnt;
				extend->Pens[0]=simple->fg;
				extend->Pens[1]=simple->bg;
				extend->ActivePens[0]=simple->fg;
				extend->ActivePens[1]=simple->bg;
			}
			sinfo->Extension=extend;

			strgad=a;
			if (glassgad) {
				gadgets[a].LeftEdge+=24;
				gadgets[a].Width-=24;

				glassgad->LeftEdge=4+cw+xoffset;
				glassgad->TopEdge=gadgets[a].TopEdge-2;
				glassgad->Width=24;
				glassgad->Height=sfy+4;
				glassgad->Flags=GFLG_GADGHCOMP;
				glassgad->Activation=GACT_RELVERIFY;
				glassgad->GadgetType=GTYP_BOOLGADGET;
				glassgad->GadgetID=65534;
			}
		}
		else if (simple->gads[b][0]) {
			if (simple->gads[b][0]=='\n') {
				gy+=cy+6;
				gx=ogx-gspace;
			}
			else if (a<gnum) {
				gadgets[a].LeftEdge=gx+xoffset;
				gadgets[a].TopEdge=gy+yoffset;
				gadgets[a].Width=gwidth;
				gadgets[a].Height=cy+5;
				gadgets[a].Flags=GADGHCOMP;
				gadgets[a].Activation=RELVERIFY;
				gadgets[a].GadgetType=GTYP_BOOLGADGET;
				gadgets[a].GadgetID=simple->rets[a];
				d=0;
				keys[a]=LToLower(simple->gads[b][0]);
				for (c=0;c<a;c++) {
					if (keys[c]==keys[a]) {
						keys[a]=LToLower(simple->gads[b][1]);
						d=1;
						break;
					}
				}
				gadptr[a]=get_uscore_text(&key,simple->gads[b],(simple->strbuf)?-1:d);
				if (last) break;
				if (a==gnum-1 && simple->strbuf) last=1;
				++a;
			}
		}
		if ((gx+=gspace)>winwidth) {
			num=a+1;
			if (simple->strbuf) {
				last=1;
				++num;
			}
			else if (a<num) break;
		}
	}

	for (a=0;a<num;a++) gadgets[a].NextGadget=(a<num-1)?&gadgets[a+1]:NULL;

	contgad->Width=(cw*10)<100?100:cw*10;
	contgad->LeftEdge=(newwindow->Width-contgad->Width)/2;
	contgad->TopEdge=gy+yoffset;
	contgad->Height=cy+4;
	contgad->Flags=GFLG_GADGHCOMP; contgad->Activation=GACT_RELVERIFY;
	contgad->GadgetType=GTYP_BOOLGADGET;

	if (num && (simple->flags&SRF_RECESSHI))
		AddGadgetBorders(&key,contgad,1,simple->hi,simple->lo);

	if (!(Window=OpenWindow(newwindow))) {
		LFreeRemember(&key);
		return(0);
	}
	rp=Window->RPort;
	SetFont(rp,fnt);

	if ((screen->Flags&15)==CUSTOMSCREEN)
		ModifyIDCMP(Window,GADGETUP|VANILLAKEY|INACTIVEWINDOW|MOUSEBUTTONS);

	if (simple->hi==-1) simple->hi=2;
	if (simple->lo==-1) simple->lo=1;
	if (simple->fg==-1) simple->fg=1;
	if (simple->bg==-1) simple->bg=0;

	if (!(simple->flags&SRF_BORDERS))
		Do3DBox(rp,2,1,Window->Width-4,Window->Height-2,simple->hi,simple->lo);

	SetAPen(rp,simple->bg);
	RectFill(rp,xoffset+2,yoffset+1,xoffset+winwidth-3,yoffset+winheight-2);
	SetAPen(rp,simple->fg); SetBPen(rp,simple->bg);

	ty=7+cb+yoffset;
	llen=dl=0;
	for (a=0;a<textlen;a++) {
		buf[llen]=text[a];
		if (text[a]==10 || llen==sw) {
			if (simple->flags&SRF_NOCENTER) Move(rp,Window->BorderLeft+6,ty);
			else Move(rp,(Window->Width-(TextLength(rp,buf,llen)))/2,ty);
			Text(rp,buf,llen);
			llen=0; ty+=cy+1; ++dl;
			if (dl>lines && a<textlen-1) {
				dl=0;
				if (num) AddGadgets(Window,contgad,contstring,1,simple->hi,simple->lo,1);
				c=0;
				FOREVER {
					Wait(1<<Window->UserPort->mp_SigBit);
					while ((Msg=(struct IntuiMessage *)GetMsg(Window->UserPort))) {
						class=Msg->Class; code=Msg->Code;
						if (class==IDCMP_GADGETUP)
							gadgetid=((struct Gadget *)Msg->IAddress)->GadgetID;
						ReplyMsg((struct Message *)Msg);
						if (class==IDCMP_INACTIVEWINDOW) {
							if (IntuitionBase->ActiveScreen==screen) ActivateWindow(Window);
						}
						else if (num) {
							if (class==IDCMP_VANILLAKEY) {
								ch=LToLower(code);
								if (ch=='\r' || ch=='c') {
									c=1;
									SelectGadget(Window,contgad);
									break;
								}
								if (ch=='') {
									CloseWindow(Window);
									LFreeRemember(&key);
									return(0);
								}
							}
							else if (class==IDCMP_GADGETUP) {
								c=1;
								break;
							}
						}
						else if (!num && class==IDCMP_MOUSEBUTTONS && code==SELECTDOWN) c=1;
					}
					if (c) break;
				}
				if (num) RemoveGList(Window,contgad,1);
				SetAPen(rp,simple->bg);
				RectFill(rp,xoffset+2,yoffset+1,xoffset+winwidth-3,yoffset+winheight-2);
				SetAPen(rp,simple->fg);
				SetBPen(rp,simple->bg);
				ty=7+cb+yoffset;
			}
		}
		else ++llen;
	}

	if (num) {
		if (simple->flags&SRF_RECESSHI)
			AddGadgetBorders(&key,gadgets,num,simple->hi,simple->lo);
		AddGadgets(Window,gadgets,gadptr,num,simple->hi,simple->lo,1);
	}
	if (simple->strbuf) {
		SetAPen(rp,0);
		RectFill(rp,gadgets[strgad].LeftEdge,gadgets[strgad].TopEdge,
			gadgets[strgad].LeftEdge+gadgets[strgad].Width-1,
			gadgets[strgad].TopEdge+gadgets[strgad].Height-1);
		Delay(5);
		ActivateStrGad(&gadgets[strgad],Window);
		if (glassgad) {
			if (simple->flags&SRF_RECESSHI)
				AddGadgetBorders(&key,glassgad,1,simple->hi,simple->lo);
			AddGadgets(Window,glassgad,NULL,1,simple->hi,simple->lo,1);
			DoGlassImage(rp,glassgad,simple->hi,simple->lo,0);
		}
	}

	FOREVER {
		Wait(1<<Window->UserPort->mp_SigBit);
		while ((Msg=(struct IntuiMessage *)GetMsg(Window->UserPort))) {
			class=Msg->Class; code=Msg->Code;
			if (class==GADGETUP) {
				gad=(struct Gadget *)Msg->IAddress;
				gadgetid=gad->GadgetID;
			}
			ReplyMsg((struct Message *)Msg);
			switch (class) {
				case MOUSEBUTTONS:
					if (!num && (code==MENUDOWN || code==SELECTDOWN)) {
						CloseWindow(Window);
						LFreeRemember(&key);
						return(1);
					}
					if (simple->strbuf) ActivateStrGad(&gadgets[strgad],Window);
					break;
				case INACTIVEWINDOW:
					if (IntuitionBase->ActiveScreen==screen) {
						ActivateWindow(Window);
						if (simple->strbuf) {
							Delay(5);
							ActivateStrGad(&gadgets[strgad],Window);
						}
					}
					break;
				case VANILLAKEY:
					ch=LToLower(code);
					if (ch=='' || ch=='\r') {
						if (ch=='') a=0;
						else a=1;
						for (gadgetid=0;gadgetid<gnum;gadgetid++)
							if (simple->rets[gadgetid]==a) break;
						if (simple->rets[gadgetid]!=a) break;
					}
					else {
						for (gadgetid=0;gadgetid<gnum;gadgetid++)
							if (ch==keys[gadgetid]) break;
						if (ch!=keys[gadgetid]) break;
					}
					if (gadgetid>=gnum) gadgetid=gnum-1;
					if (gadgets[gadgetid].Activation&TOGGLESELECT) {
						HiliteGad(&gadgets[gadgetid],rp);
						gadgetid=gad->GadgetID;
					}
					else {
						SelectGadget(Window,&gadgets[gadgetid]);
						gadgetid=simple->rets[gadgetid];
					}
				case GADGETUP:
					if (simple->strbuf && gadgetid==65534) {
						struct DOpusFileReq filereq;
						char dirbuf[256],filebuf[32],*ptr;

						filereq.title="Select directory";
						filereq.dirbuf=dirbuf;
						filereq.filebuf=filebuf;
						filereq.window=Window;
						filereq.x=-2;
						filereq.y=-2;
						filereq.lines=15;
						if (simple->flags&SRF_DIRGLASS) filereq.flags=DFRF_DIRREQ;
						else filereq.flags=0;
						filereq.filearraykey=NULL;

						strcpy(dirbuf,strbuf);
						if (!(simple->flags&SRF_DIRGLASS)) {
							if ((ptr=BaseName(dirbuf))>dirbuf) {
								strcpy(filebuf,ptr);
								*ptr=0;
							}
							else if (CheckExist(dirbuf,NULL)<1) {
								dirbuf[0]=0;
								strcpy(filebuf,strbuf);
							}
							else filebuf[0]=0;
						}
						if (FileRequest(&filereq)) {
							strcpy(strbuf,dirbuf);
							if (!(simple->flags&SRF_DIRGLASS))
								TackOn(strbuf,filebuf,256);
						}
						RefreshStrGad(&gadgets[strgad],Window);
						ActivateStrGad(&gadgets[strgad],Window);
						break;
					}
					CloseWindow(Window);
					if (gadgetid && simple->strbuf) strcpy(simple->strbuf,strbuf);
					LFreeRemember(&key);
					return(gadgetid);
			}
		}
	}
    AROS_LIBFUNC_EXIT
}

char *get_uscore_text(key,text,pos)
struct DOpusRemember **key;
char *text;
int pos;
{
	char *newtext;
	int a,len,cp;

	len=strlen(text);
	if (!(newtext=LAllocRemember(key,len+2,0))) return(text);
	for (a=0,cp=0;a<len;a++) {
		if (cp==pos) newtext[cp++]='_';
		newtext[cp++]=text[a];
	}
	newtext[cp]=0;
	return(newtext);
}

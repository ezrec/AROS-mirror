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

#include "dopus.h"
#include "ppdata.h"

int showfont(name,size,np)
char *name;
int size,np;
{
	int base,y,fred,t,len;
	unsigned char a;
	struct TextFont *font;
	char fontbuf[256],*fontptr;
	static UWORD fcols[2]={0xfff,0};
	struct TextAttr sfattr;

	sfattr.ta_Name=(STRPTR)name;
	sfattr.ta_Style=sfattr.ta_Flags=0;
	sfattr.ta_YSize=size;

	if (DiskfontBase) font=OpenDiskFont(&sfattr);
	else font=OpenFont(&sfattr);
	if (!font || !(setupfontdisplay(1,NULL))) {
		doerror(IoErr());
		return(0);
	}

	base=font->tf_Baseline;
	SetFont(font_rp,font);
	SetAPen(font_rp,1);
	SetDrMd(font_rp,JAM1);

	y=base;
	a=font->tf_LoChar;
	len=t=0;

	Move(font_rp,0,y);
	fontptr=fontbuf;

	FOREVER {
		len+=TextLength(font_rp,(char *)&a,1);
		if (len>fontscreen->Width || t>254) {
			Text(font_rp,fontbuf,t);
			y+=size;
			Move(font_rp,0,y);
			len=t=0; fontptr=fontbuf;
			if (y-base>fontscreen->Height) break;
		}
		*(fontptr++)=a; ++t;
		if ((++a)>font->tf_HiChar) a=font->tf_LoChar;
	}

	ScreenToFront(fontscreen);
	ActivateWindow(fontwindow);
	FadeRGB4(fontscreen,fcols,2,1,config->fadetime);
	show_global_font=font;

	fred=WaitForMouseClick(0,fontwindow);

	show_global_font=NULL;
	if (fred!=-3) FadeRGB4(fontscreen,fcols,2,-1,config->fadetime);

	cleanup_fontdisplay();
	CloseFont(font);

	return((fred==0)?1:-1);
}
	
int setupfontdisplay(depth,coltab)
int depth;
UWORD *coltab;
{
	font_scr.Width=GfxBase->NormalDisplayColumns;
	font_scr.Height=STDSCREENHEIGHT;
	font_scr.Depth=depth;
	if (!(fontscreen=OpenScreen((struct NewScreen *)&font_scr))) return(0);
	font_win.Width=fontscreen->Width;
	font_win.Height=fontscreen->Height;
	font_win.Screen=fontscreen;
	if (!(fontwindow=OpenWindow(&font_win))) {
		CloseScreen(fontscreen);
		return(0);
	}
	font_rp=fontwindow->RPort;
	if (coltab) {
		int a,num;

		num=1<<depth;
		if (num>16) num=16;
		for (a=0;a<num;a++) coltab[a]=GetRGB4(fontscreen->ViewPort.ColorMap,a);
	}
	LoadRGB4(&fontscreen->ViewPort,nullpalette,1<<depth);
	setnullpointer(fontwindow);
	return(1);
}

void cleanup_fontdisplay()
{
	if (blankscreen) ScreenToFront(blankscreen);
	else ScreenToFront(Window->WScreen);
	if (fontwindow) {
		CloseWindow(fontwindow);
		fontwindow=NULL;
	}
	if (fontscreen) {
		CloseScreen(fontscreen);
		fontscreen=NULL;
	}
	ActivateWindow(Window);
}

void readhelp(file)
char *file;
{
	doreadhelp(file);
	if (dopus_firsthelp) screen_gadgets[SCRGAD_TINYHELP].Flags&=~GFLG_DISABLED;
	else screen_gadgets[SCRGAD_TINYHELP].Flags|=GFLG_DISABLED;
	if (Window && config->generalscreenflags&SCR_GENERAL_TINYGADS)
		dolittlegads(&screen_gadgets[SCRGAD_TINYHELP],&globstring[STR_TINY_BUTTONS][4],1);
}

void doreadhelp(file)
char *file;
{
	int a,b,helpsize,inmsg;
	char *helpbuf,*buf;
	struct Help *temph,*curhelp;

	freehelp();
	if (!file[0] || !(buf=AllocMem(10000,MEMF_CLEAR))) return;
	if (readfile(file,&helpbuf,&helpsize)) {
		FreeMem(buf,10000);
		return;
	}

	curhelp=NULL;
	inmsg=0;
	for (a=0;a<helpsize;a++) {
		if (helpbuf[a]=='*' && !inmsg) {
			inmsg=1; b=0;
			continue;
		}
		else {
			if (inmsg==1) {
				if (b==9999 || helpbuf[a]=='\n') {
					buf[b]=0;
					if (!(temph=LAllocRemember(&help_key,sizeof(struct Help),MEMF_CLEAR))) break;
					if (!(temph->lookup=LAllocRemember(&help_key,b+1,MEMF_CLEAR))) break;
					CopyMem(buf,temph->lookup,b);
					if (curhelp) curhelp->next=temph;
					else dopus_firsthelp=temph;
					curhelp=temph;
					inmsg=2; b=0;
					continue;
				}					
				buf[b]=helpbuf[a];
				++b;
				continue;
			}
			else if (inmsg==2) {
				if (b==9999 || helpbuf[a]=='^') {
					if (curhelp) {
						buf[b]=0;
						if (!(curhelp->message=LAllocRemember(&help_key,b+1,MEMF_CLEAR))) break;
						CopyMem(buf,curhelp->message,b);
					}
					inmsg=0; b=0;
					continue;
				}
				buf[b]=helpbuf[a];
				++b;
				continue;
			}
		}
	}
	FreeMem(buf,10000);
	FreeMem(helpbuf,helpsize);
}

void dohelp(name,func,key,qual,defmsg)
char *name,*func;
int key,qual;
char *defmsg;
{
	char *msg=NULL,*buf=NULL,buf1[40],buf2[30];
	struct Help *help;
	int s,s1;

	if (!dopus_firsthelp) return;
	help=dopus_firsthelp;
	while (help) {
		if (help->lookup) {
			if (help->lookup[0]=='*' && func) {
				if (LStrCmpI(help->lookup,func)==0) {
					msg=help->message;
					break;
				}
			}
			else if (name) {
				if (LStrCmpI(help->lookup,name)==0) {
					msg=help->message;
					break;
				}
			}
		}
		help=help->next;
	}
	if (!msg) {
		if (defmsg) msg=defmsg;
		else msg=globstring[STR_HELP_NOT_AVAILABLE];
	}
	if (key>0) {
		RawkeyToStr(key,qual,buf2,NULL,30);
		strcpy(buf1,globstring[STR_KEY]); strcat(buf1,buf2); strcat(buf1,"\n\n");
		s1=strlen(buf1);
		s=strlen(msg)+1;
		if ((buf=AllocMem(s+s1,MEMF_CLEAR))) {
			CopyMem(buf1,buf,s1);
			CopyMem(msg,&buf[s1],s);
			s+=s1;
			msg=buf;
		}
	}
	simplerequest(msg,globstring[STR_CONTINUE],NULL);
	unbusy();
	if (buf) FreeMem(buf,s);
	return;
}

void checkstringgads(a)
int a;
{
	--a;
	checkdir(str_pathbuffer[a],&path_strgadget[a]);
	strcpy(dopus_curwin[a]->directory,str_pathbuffer[a]);
}

void setdirsize(dir,byte,win)
struct Directory *dir;
int byte,win;
{
	if (status_iconified || win==-1) return;

	if (dir->size>0) {
		dopus_curwin[win]->bytestot-=dir->size;
		if (dir->selected) dopus_curwin[win]->bytessel-=dir->size;
	}
	if (byte<-1) byte=-1;
	dir->size=byte;
	if (dir->size!=-1) {
		dopus_curwin[win]->bytestot+=byte;
		if (dir->selected) dopus_curwin[win]->bytessel+=byte;
	}
}

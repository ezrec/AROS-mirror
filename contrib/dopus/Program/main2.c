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

/* AROS: Needs some obsolete defines */
#include <intuition/iobsolete.h>

#define EXALL_NUM 2

void freedir(dir,win)
struct DirectoryWindow *dir;
int win;
{
	struct Directory *free,*work;

	if (dir) {
		if (dir->firstentry && dir->total>0) {
			if (dir->firstentry) {
				free=dir->firstentry;
				while (free) {
					if (free->next) work=free->next;
					else work=NULL;
					free->next=NULL;
					free->last=NULL;

					free_file_memory(free);
					free=work;
				}
			}
		}
		dir->firstentry=dir->firstfile=dir->firstdir=NULL;
		dir->oldoff=dir->oldhoff=-1;
		dir->offset=dir->total=dir->filesel=
			dir->dirsel=dir->bytessel=dir->bytestot=
			dir->dirtot=dir->filetot=dir->hoffset=0;
		dir->custhandler[0]=0;
		dir->realdevice[0]=0;
		dir->volumename[0]=0;
		dir->flags=0;
		if (Window && win>-1) {
			fixhorizprop(win);
			last_selected_entry=NULL;
		}
	}
}

int getdir(dir,win,incmess)
struct DirectoryWindow *dir;
int win,incmess;
{
	int tot=1,a,use_exall=0,exall_entry,exall_continue,exall_bufsize;
	BPTR mylock;
	char buf[256];
	
	/* AROS:  We have no __aligned keyword so we allocate the FIB 
	struct FileInfoBlock __aligned fileinfo;
	*/
	
	struct FileInfoBlock * fileinfo;
	
	struct ExAllControl *exall_control;
	struct ExAllData *exall_buffer,*exall_current;
	struct MsgPort *deviceport;
	
	fileinfo = AllocDosObject(DOS_FIB, NULL);
	if (NULL == fileinfo)
		return 0;

	endnotify(win);
	freedir(dir,win);

	vert_propinfo[win].VertPot=0;
	if (Window && !status_iconified) {
		refreshwindow(win,1);
		if (win==data_active_window)
			SetAPen(main_rp,screen_pens[(int)config->disknameselbg].pen);
		else SetAPen(main_rp,screen_pens[(int)config->disknamebg].pen);
		rectfill(main_rp,
			scrdata_diskname_xpos[win]+2,
			scrdata_diskname_ypos,
			scrdata_diskname_width[win],
			scrdata_diskname_height-2);
		SetAPen(main_rp,screen_pens[1].pen);
	}

	if (!(mylock=Lock(dir->directory,ACCESS_READ))) {
		if (Window) doerror(IoErr());
		for (a=0;a<30;a++) {
			if (str_pathbuffer[win][a]==':' || str_pathbuffer[win][a]==0) break;
		}
		if (str_pathbuffer[win][a]==':') LStrnCpy(dir->realdevice,str_pathbuffer[win],a+1);
			goto failexit;
	}

	strcpy(buf,str_pathbuffer[win]);
	if (getroot(buf,NULL)) {
		strcpy(dir->volumename,buf);
		strcat(buf,":");
		if ((deviceport=(struct MsgPort *)DeviceProc(buf)))
			get_device_task(mylock,dir->realdevice,deviceport);
	}

	if (config->dirflags&DIRFLAGS_EXPANDPATHS) {
		PathName(mylock,buf,256);
		strcpy(str_pathbuffer[win],buf);
		if (Window) checkdir(str_pathbuffer[win],&path_strgadget[win]);
		else checkdir(str_pathbuffer[win],NULL);
		strcpy(dir->directory,str_pathbuffer[win]);
	}

	Examine(mylock,fileinfo);
	if (fileinfo->fib_DirEntryType<0) {
		if (Window) doerror(ERROR_OBJECT_WRONG_TYPE);
		UnLock(mylock);
		goto failexit;
	}
	dir->firstentry=NULL;
	if (Window) {
		if (incmess) dostatustext(globstring[STR_READING_CHANGED_DIRECTORY]);
		else dostatustext(globstring[STR_READING_DIRECTORY]);
		busy();
	}
	copy_datestamp(&fileinfo->fib_Date,&dir->dirstamp);

	if (config->dirflags&DIRFLAGS_EXALL && system_version2>=OSVER_40) {
		if ((exall_control=AllocDosObject(DOS_EXALLCONTROL,NULL))) {
			if ((exall_buffer=(struct ExAllData *)
				AllocMem((exall_bufsize=((sizeof(struct ExAllData)+160)*EXALL_NUM)),MEMF_CLEAR))) {
				use_exall=1;
				exall_entry=0;
				exall_continue=1;
				exall_control->eac_Entries=0;
				exall_control->eac_LastKey=0;
				exall_control->eac_MatchString=NULL;
				exall_control->eac_MatchFunc=NULL;
				exall_current=NULL;
			}
			else {
				FreeDosObject(DOS_EXALLCONTROL,exall_control);
				exall_control=NULL;
			}
		}
	}

	FOREVER {
		fileinfo->fib_OwnerUID=fileinfo->fib_OwnerGID=0;
		if (use_exall) {
			if (exall_entry>=exall_control->eac_Entries || !exall_current) {
				if (!exall_continue) break;
				exall_continue=ExAll(mylock,exall_buffer,exall_bufsize,ED_COMMENT,exall_control);
				exall_entry=0;
				exall_current=exall_buffer;
				continue;
			}
			if (exall_current->ed_Name)
				strcpy(fileinfo->fib_FileName,exall_current->ed_Name);
			else fileinfo->fib_FileName[0]=0;
			fileinfo->fib_DirEntryType=exall_current->ed_Type;
			fileinfo->fib_Size=exall_current->ed_Size;
			fileinfo->fib_Protection=exall_current->ed_Prot;
			fileinfo->fib_Date.ds_Days=exall_current->ed_Days;
			fileinfo->fib_Date.ds_Minute=exall_current->ed_Mins;
			fileinfo->fib_Date.ds_Tick=exall_current->ed_Ticks;
			if (exall_current->ed_Comment)
/*
				BtoCStr((BSTR)exall_current->ed_Comment,fileinfo->fib_Comment,79);
*/
				strcpy(fileinfo->fib_Comment,exall_current->ed_Comment);
			else fileinfo->fib_Comment[0]=0;
			++exall_entry;
			exall_current=exall_current->ed_Next;
		}
		else {
			if (!(ExNext(mylock,fileinfo))) break;
		}

		if (status_haveaborted && Window) {
			myabort();
			dir->flags|=DWF_ABORTED;
			tot=-1;
			break;
		}
		if (!(addfile(dir,win,fileinfo->fib_FileName,
			(fileinfo->fib_DirEntryType>0)?-1:fileinfo->fib_Size,
			fileinfo->fib_DirEntryType,
			&fileinfo->fib_Date,fileinfo->fib_Comment,fileinfo->fib_Protection,0,
			FALSE,NULL,NULL,fileinfo->fib_OwnerUID,fileinfo->fib_OwnerGID))) {
			if (Window) doerror(IoErr());
			tot=0;
			break;
		}
		else if (Window) fixprop(win);
	}
	UnLock(mylock);

	if (Window) {
		unbusy();
		if (tot) {
			seename(win);
			if (tot==1) okay();
		}
		refreshwindow(win,1);
	}
	if (use_exall) {
		if (exall_control) FreeDosObject(DOS_EXALLCONTROL,exall_control);
		if (exall_buffer) FreeMem(exall_buffer,exall_bufsize);
	}
	startnotify(win);
	
	FreeDosObject(DOS_FIB, fileinfo);
	return(1);
	
failexit:
	FreeDosObject(DOS_FIB, fileinfo);
	return 0;

}

void fixprop(win)
int win;
{
	if (win>-1 && !status_iconified) {
		FixSliderBody(Window,&vert_propgad[win],dopus_curwin[win]->total,scrdata_dispwin_lines,0);
		fixvertprop(win);
	}
}

void fixvertprop(win)
int win;
{
	if (win>-1 && !status_iconified) {
		FixSliderPot(Window,&vert_propgad[win],dopus_curwin[win]->offset,
			dopus_curwin[win]->total,scrdata_dispwin_lines,2);
	}
}

void fixhorizprop(win)
int win;
{
	if (win>-1 && !status_iconified) {
		fixhlen(win);
		if (Window) {
			FixSliderBody(NULL,&horiz_propgad[win],dopus_curwin[win]->hlen,scrdata_dispwin_nchars[win],0);
			FixSliderPot(Window,&horiz_propgad[win],dopus_curwin[win]->hoffset,dopus_curwin[win]->hlen,scrdata_dispwin_nchars[win],2);
		}
	}
}

int doposprop(win)
int win;
{
	int i;

	if (win<0 || status_iconified) return(0);
	if (!(status_flags&STATUS_NEWLOOK) &&
		vert_propgad[win].MutualExclude) ShowSlider(Window,&vert_propgad[win]);
	i=GetSliderPos(&vert_propgad[win],dopus_curwin[win]->total,scrdata_dispwin_lines);
	if (dopus_curwin[win]->offset==i) return(0);
	dopus_curwin[win]->offset=i;
	displaydir(win);
	return(1);
}

void doposhprop(win)
int win;
{
	int i;

	if (!status_iconified && win>-1) {
		if (!(status_flags&STATUS_NEWLOOK) &&
			horiz_propgad[win].MutualExclude) ShowSlider(Window,&horiz_propgad[win]);
		i=GetSliderPos(&horiz_propgad[win],dopus_curwin[win]->hlen,scrdata_dispwin_nchars[win]);
		if (dopus_curwin[win]->hoffset==i) return;
		dopus_curwin[win]->hoffset=i; dopus_curwin[win]->oldoff=-1;
		displaydir(win);
	}
}

void checkdir(str,gad)
char *str;
struct Gadget *gad;
{
	struct StringInfo *sinfo;

	TackOn(str,NULL,256);
	if (gad) {
		sinfo=(struct StringInfo *)gad->SpecialInfo;
		gad->Flags|=GADGHNONE;
		if (!system_version2) {
			sinfo->BufferPos=0; RefreshGList(gad,Window,NULL,1);
		}
		sinfo->BufferPos=strlen(str); RefreshGList(gad,Window,NULL,1);
		gad->Flags&=~GADGHNONE;
	}
}

void verticalscroll(win,dir)
int win,dir;
{
	if (win<0 || dopus_curwin[win]->total<=scrdata_dispwin_lines) return;

	if (dir<0) {
		--dopus_curwin[win]->offset;
		if (dopus_curwin[win]->offset<0) {
			dopus_curwin[win]->offset=0;
			return;
		}
	}
	else {
		++dopus_curwin[win]->offset;
		if (dopus_curwin[win]->offset>dopus_curwin[win]->total-scrdata_dispwin_lines) {
			dopus_curwin[win]->offset=dopus_curwin[win]->total-scrdata_dispwin_lines;
			return;
		}
	}
	fixvertprop(win);
	displaydir(win);
}

void horizontalscroll(win,dir)
int win,dir;
{
	if (win<0 || dopus_curwin[win]->total==0 || dopus_curwin[win]->firstentry->type==ENTRY_DEVICE)
		return;
	if (dir<0) {
 		if (dopus_curwin[win]->hoffset==0) return;
		--dopus_curwin[win]->hoffset;
	}
	else {
		if (dopus_curwin[win]->hoffset==dopus_curwin[win]->hlen-scrdata_dispwin_nchars[win]) return;
		++dopus_curwin[win]->hoffset;
	}
	refreshwindow(win,1);
}

void findfirstsel(win,type)
int win,type;
{
	struct Directory *sel;
	int a;

	if (win<0 || dopus_curwin[win]->total<=scrdata_dispwin_lines) return;
	sel=dopus_curwin[win]->firstentry; a=0;
	while (sel) {
		if (sel->selected) {
			if (type==-2 || ENTRYTYPE(sel->type)==type) break;
		}
		sel=sel->next;
		++a;
	}
	if (sel) {
		dopus_curwin[win]->offset=a;
		if (dopus_curwin[win]->offset>dopus_curwin[win]->total-scrdata_dispwin_lines)
			dopus_curwin[win]->offset=dopus_curwin[win]->total-scrdata_dispwin_lines;
		fixvertprop(win);
		displaydir(win);
	}
	return;
}

void findfirstchar(win,c)
int win;
char c;
{
	struct Directory *sel;
	int a=0,b=0,file=0;

	if (isupper(c)) { file=1; c=LToLower(c); }
	if (dopus_curwin[win]->total<=scrdata_dispwin_lines) return;
	sel=dopus_curwin[win]->firstentry;
	while (sel) {
		if (LToLower(sel->name[0])>=c) {
			if (!file || ENTRYTYPE(sel->type)==ENTRY_FILE) {
				b=1;
				break;
			}
		}
		if (sel->next) sel=sel->next;
		else sel=NULL;
		++a;
	}
	if (!b) dopus_curwin[win]->offset=dopus_curwin[win]->total-scrdata_dispwin_lines;
	else {
		dopus_curwin[win]->offset=a;
		if (dopus_curwin[win]->offset>dopus_curwin[win]->total-scrdata_dispwin_lines)
			dopus_curwin[win]->offset=dopus_curwin[win]->total-scrdata_dispwin_lines;
	}
	if (dopus_curwin[win]->offset<0) dopus_curwin[win]->offset=0;
	fixvertprop(win);
	displaydir(win);
}

void doinfodisplay(entry,state)
struct Directory *entry;
int state;
{
	char buf[MAXDISPLAYLENGTH],pbuf[10],dbuf[30];

	buf[0]=0;
	if (entry->selected==state) {
		getprot(entry->protection,pbuf);
		nodayseedate(&(entry->date),dbuf);
		lsprintf(buf,"%8s %18s ",pbuf,dbuf);
		if (entry->type!=ENTRY_CUSTOM && entry->comment)
			strcat(buf,entry->comment);
	}
	dostatustext(buf);
}

void nodayseedate(ds,date)
struct DateStamp *ds;
char *date;
{
	char datebuf[16],timebuf[16];
	struct DOpusDateTime dt;

	copy_datestamp(ds,&dt.dat_Stamp);
	initdatetime(&dt,datebuf,timebuf,0);
	strcpy(date,datebuf); strcat(date," "); strcat(date,timebuf);
}

void displaydirgiven(win,dir,bypass)
int win;
struct Directory *dir;
char bypass;
{
	struct Directory *display;
	int off;

	if (win<0 ||
		(!bypass && !(config->dynamicflags&UPDATE_SCROLL))) return;

	display=dopus_curwin[win]->firstentry;
	off=0;
	while (display) {
		if (display==dir) break;
		display=display->next;
		++off;
	}
	if (!display) return;
	dopus_curwin[win]->offset=off;
	if (dopus_curwin[win]->oldoff>-1) {
		if (dopus_curwin[win]->offset>=dopus_curwin[win]->oldoff &&
			dopus_curwin[win]->offset<dopus_curwin[win]->oldoff+scrdata_dispwin_lines)
			dopus_curwin[win]->offset=dopus_curwin[win]->oldoff;
	}
	if (dopus_curwin[win]->offset>dopus_curwin[win]->total-scrdata_dispwin_lines)
		dopus_curwin[win]->offset=dopus_curwin[win]->total-scrdata_dispwin_lines;
	if (dopus_curwin[win]->offset<0) dopus_curwin[win]->offset=0;
	if (dopus_curwin[win]->offset!=dopus_curwin[win]->oldoff) {
		displaydir(win);
		fixvertprop(win);
	}
}

void endfollow(win)
int win;
{
	if (win>-1 && scrdata_old_offset_store>-1) {
		dopus_curwin[win]->offset=scrdata_old_offset_store;
		if (dopus_curwin[win]->offset>(dopus_curwin[win]->total-scrdata_dispwin_lines))
			dopus_curwin[win]->offset=dopus_curwin[win]->total-scrdata_dispwin_lines;
		if (dopus_curwin[win]->offset<0) dopus_curwin[win]->offset=0;
		fixvertprop(win);
		displaydir(win);
		scrdata_old_offset_store=-1;
	}
}

void displaydir(win)
int win;
{
	int d,a,l,tl,bl,b,ds,my,c,f,pw,px,lpst8,sc,to,y;
	char sbuf[MAXDISPLAYLENGTH];
	struct Directory *display;

	if (win<0 || status_iconified) return;
	l=dopus_curwin[win]->oldoff-dopus_curwin[win]->offset;
	display=dopus_curwin[win]->firstentry;
	if (l<0) {
		tl=scrdata_dispwin_lines-1+l; bl=scrdata_dispwin_lines; a=-l;
	}
	else {
		tl=-1; bl=l; a=l;
	}
	b=scrdata_dispwin_lines-1;
	if (dopus_curwin[win]->oldoff==-1 || a>b) {
		tl=-1; bl=scrdata_dispwin_lines; ds=0; sc=0;
	}
	else {
		b=ABSI(l);
		if (config->dynamicflags&4 && b>(scrdata_dispwin_lines/4)) {
			tl=-1; bl=scrdata_dispwin_lines; ds=0; sc=0;
		}
		else {
			if (b==0) d=0;
			else d=-(dopus_curwin[win]->oldoff-dopus_curwin[win]->offset)/b;
			ds=1; sc=d;
		}
	}
	for (a=0;a<dopus_curwin[win]->offset;a++) {
		if (!display) break;
		display=display->next;
	}
	if (sc<0 && ds) {
		c=b-1;
		for (a=0;a<c;a++) {
			if (!display) break;
			display=display->next;
		}
	}
	if (ds) {
		if (sc<0) my=scrdata_font_baseline;
		else my=(scrdata_font_baseline+scrdata_dirwin_height)-scrdata_font_ysize;
	}
	else my=scrdata_font_ysize+scrdata_font_baseline+(tl*scrdata_font_ysize);
	my+=scrdata_dirwin_ypos[win];
	lpst8=scrdata_dirwin_height-scrdata_font_ysize;

	++tl;
	for (a=0;a<tl;a++) {
		if (!display) break;
		if (sc<0) display=display->last;
		else display=display->next;
	}
	pw=scrdata_dispwin_nchars[win]; px=scrdata_dirwin_xpos[win]; to=0;
	if (dopus_curwin[win]->oldhoff!=-1) {
		f=dopus_curwin[win]->oldhoff-dopus_curwin[win]->hoffset;
		if (f && f>-scrdata_dispwin_nchars[win] && f<scrdata_dispwin_nchars[win]) {
			if (f>0) ClipBlit(&dir_rp[win],scrdata_dirwin_xpos[win],scrdata_dirwin_ypos[win],&dir_rp[win],
				(f*scrdata_font_xsize)+scrdata_dirwin_xpos[win],scrdata_dirwin_ypos[win],scrdata_dirwin_width[win]-(f*scrdata_font_xsize),scrdata_dirwin_height,0xc0);
			else {
				f=-f;
				ClipBlit(&dir_rp[win],(f*scrdata_font_xsize)+scrdata_dirwin_xpos[win],scrdata_dirwin_ypos[win],&dir_rp[win],
					scrdata_dirwin_xpos[win],scrdata_dirwin_ypos[win],scrdata_dirwin_width[win]-(f*scrdata_font_xsize),scrdata_dirwin_height,0xc0);
				px=(scrdata_dirwin_width[win]-(f*scrdata_font_xsize))+scrdata_dirwin_xpos[win]; to=scrdata_dispwin_nchars[win]-f;
			}
			pw=f;
		}
	}
	for (a=tl;a<bl;a++) {
		setdispcol(display,win);
		builddisplaystring(display,sbuf,win);
		y=my;
		if (ds) {
			if (sc>0) ClipBlit(&dir_rp[win],scrdata_dirwin_xpos[win],scrdata_font_ysize+scrdata_dirwin_ypos[win],&dir_rp[win],
				scrdata_dirwin_xpos[win],scrdata_dirwin_ypos[win],scrdata_dirwin_width[win],lpst8,0xc0);
			else ClipBlit(&dir_rp[win],scrdata_dirwin_xpos[win],scrdata_dirwin_ypos[win],&dir_rp[win],
				scrdata_dirwin_xpos[win],scrdata_font_ysize+scrdata_dirwin_ypos[win],scrdata_dirwin_width[win],lpst8,0xc0);
		}
		else my+=scrdata_font_ysize;
		entry_text(&dir_rp[win],display,&sbuf[to],pw,px,y);
		if (display) {
			if (sc<0) display=display->last;
			else display=display->next;
		}
	}
	dopus_curwin[win]->oldoff=dopus_curwin[win]->offset;
	dopus_curwin[win]->oldhoff=dopus_curwin[win]->hoffset;
}

void display_entry(entry,win,x,y)
struct Directory *entry;
int win,x,y;
{
	char dispbuf[MAXDISPLAYLENGTH];

	setdispcol(entry,win);
	builddisplaystring(entry,dispbuf,win);
	entry_text(&dir_rp[win],entry,dispbuf,scrdata_dispwin_nchars[win],x,y);
}

void entry_text(rp,entry,buf,len,x,y)
struct RastPort *rp;
struct Directory *entry;
char *buf;
int len,x,y;
{
	int a,final=0;

	Move(rp,x,y);
	if (entry && entry->type==ENTRY_CUSTOM && entry->subtype==CUSTOMENTRY_DIRTREE) {
		char pbuf[MAXDISPLAYLENGTH];

		strcpy(pbuf,buf);
		for (a=0;a<len;a++) {
			if (buf[a]=='|' || buf[a]=='+' || buf[a]=='-')
				buf[a]=' ';
			else if (buf[a]!=' ') break;
		}
		Text(rp,buf,len);
		if (entry->selected) SetAPen(rp,screen_pens[(int)config->dirsselfg].pen);
		else SetAPen(rp,screen_pens[(int)config->dirsfg].pen);
		y-=scrdata_font_baseline;

		for (a=0;a<len;a++) {
			if (pbuf[a]=='|') {
				if (pbuf[a+1]=='-') draw_dirtree_gfx(rp,x,y,DIRTREEGFX_VERTCROSS);
				else draw_dirtree_gfx(rp,x,y,DIRTREEGFX_VERT);
			}
			else if (pbuf[a]=='-') {
				draw_dirtree_gfx(rp,x,y,(final)?DIRTREEGFX_FINALCROSS:DIRTREEGFX_CROSS);
				final=1-final;
			}
			else if (pbuf[a]=='+') draw_dirtree_gfx(rp,x,y,DIRTREEGFX_BRANCH);
			else if (pbuf[a]!=' ') break;
			x+=scrdata_font_xsize;
		}
	}
	else Text(rp,buf,len);
}

void builddisplaystring(display,sbuf,win)
struct Directory *display;
char *sbuf;
int win;
{
	char buf[MAXDISPLAYLENGTH],sizebuf[20];
	char *sptr,*dptr,*eptr;
	int a,b,namelen;

	if (!display) CopyMem(str_space_string,sbuf,scrdata_dispwin_nchars[win]);
	else switch (ENTRYTYPE(display->type)) {
		case ENTRY_CUSTOM:
			CopyMem(str_space_string,sbuf,scrdata_dispwin_nchars[win]);
			if (display->comment) {
				a=strlen(display->comment)-dopus_curwin[win]->hoffset;
				if (a>scrdata_dispwin_nchars[win]) a=scrdata_dispwin_nchars[win];
				if (a>0) CopyMem(&display->comment[dopus_curwin[win]->hoffset],sbuf,a);
			}
			break;
		case ENTRY_DEVICE:
			CopyMem(str_space_string,sbuf,scrdata_dispwin_nchars[win]);
			CopyMem(display->name,sbuf,strlen(display->name));
			CopyMem(display->comment,&sbuf[scrdata_dispwin_nchars[win]-5],5);
			break;
		default:
			namelen=config->displaylength[win][0];
			if (display->type<0 || (display->type>0 && display->size>0)) {
				lsprintf(sizebuf,"%7ld ",display->size);
				if ((a=strlen(sizebuf))>8) {
					a-=8;
					if (namelen>(30-a)) namelen=30-a;
				}
			}
			else LStrnCpy(sizebuf,str_space_string,8);
			dptr=buf; eptr=buf+dopus_curwin[win]->hlen+1;

			for (a=0;a<DISPLAY_LAST+1;a++) {
				switch (config->displaypos[win][a]) {
					case DISPLAY_NAME:
						b=namelen;
						sptr=display->name;
						while (*sptr && b) {
							*dptr++=*sptr++;
							--b;
						}
						while (b--) *dptr++=' ';
						break;
					case DISPLAY_SIZE:
						sptr=sizebuf;
						while (*sptr) *dptr++=*sptr++;
						break;
					case DISPLAY_PROTECT:
						sptr=display->protbuf;
						while (*sptr) *dptr++=*sptr++; *dptr++=' ';
						break;
					case DISPLAY_DATE:
						sptr=display->datebuf;
						while (*sptr) *dptr++=*sptr++; *dptr++=' ';
						break;
					case DISPLAY_COMMENT:
						b=config->displaylength[win][1];
						if ((sptr=display->comment)) {
							while (*sptr && b) {
								*dptr++=*sptr++;
								--b;
							}
						}
						while (b--) *dptr++=' ';
						break;
					case DISPLAY_FILETYPE:
						b=config->displaylength[win][2];
						if ((sptr=display->description)) {
							while (*sptr && b) {
								*dptr++=*sptr++;
								--b;
							}
						}
						while (b--) *dptr++=' ';
						break;
					case DISPLAY_OWNER:
						if (AccountsBase) {
							b=config->displaylength[win][3];
							if (!display->network || !(sptr=display->network->owner))
								sptr=globstring[STR_NETWORK_NO_OWNER];
							while (*sptr && b) {
								*dptr++=*sptr++;
								--b;
							}
							while (b--) *dptr++=' ';
						}
						break;
					case DISPLAY_GROUP:
						if (AccountsBase) {
							b=config->displaylength[win][4];
							if (!display->network || !(sptr=display->network->group))
								sptr=globstring[STR_NETWORK_NO_GROUP];
							while (*sptr && b) {
								*dptr++=*sptr++;
								--b;
							}
							while (b--) *dptr++=' ';
						}
						break;
					case DISPLAY_NETPROT:
						if (AccountsBase) {
							b=10;
							if (display->network) {
								sptr=display->network->net_protbuf;
								while (*sptr && b) {
									*dptr++=*sptr++;
									--b;
								}
							}
							while (b--) *dptr++=' ';
						}
						break;
				}
			}
			while (dptr<eptr) *dptr++=' ';
			CopyMem(&buf[dopus_curwin[win]->hoffset],sbuf,scrdata_dispwin_nchars[win]);
			break;
	}
}

void setdispcol(display,win)
struct Directory *display;
int win;
{
	int fg,bg;

	if (display && !status_iconified) {
		switch (ENTRYTYPE(display->type)) {
			case ENTRY_FILE:
				if (display->selected) {
					fg=config->filesselfg;
					bg=config->filesselbg;
				}
				else {
					fg=config->filesfg;
					bg=config->filesbg;
				}
				break;
			case ENTRY_DEVICE:
				if (display->size==DLT_DEVICE || display->size==DLT_VOLUME) {
					if (display->selected) {
						fg=config->filesselfg;
						bg=config->filesselbg;
					}
					else {
						fg=config->filesfg;
						bg=config->filesbg;
					}
					break;
				}
			case ENTRY_DIRECTORY:
				if (display->selected) {
					fg=config->dirsselfg;
					bg=config->dirsselbg;
				}
				else {
					fg=config->dirsfg;
					bg=config->dirsbg;
				}
				break;
			case ENTRY_CUSTOM:
				if (display->subtype==CUSTOMENTRY_USER) {
					if (display->selected) {
						fg=config->filesselfg;
						bg=config->filesselbg;
					}
					else {
						if ((display->size&0xff)==0xff) fg=config->filesfg;
						else fg=display->size&0xff;
						if ((display->size&0xff00)==0xff00) bg=config->filesbg;
						else bg=(display->size&0xff00)>>8;
					}
				}
				else {
					if (display->selected) {
						fg=config->dirsselfg;
						bg=config->dirsselbg;
					}
					else {
						fg=config->dirsfg;
						bg=config->dirsbg;
					}
				}
				break;
		}
	}
	else {
		fg=bg=config->filesbg;
	}
	SetAPen(&dir_rp[win],screen_pens[fg].pen);
	SetBPen(&dir_rp[win],screen_pens[bg].pen);
}

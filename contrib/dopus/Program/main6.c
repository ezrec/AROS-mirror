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
#include <dos/notify.h>

/* AROS: We need some obsolete defines */
#include <intuition/iobsolete.h>

void get_printdir_data(pddata)
struct PrintDirData *pddata;
{
	char buf[256];
	int win;

	if (pddata->win<0 || pddata->win>1) win=data_active_window;
	else win=pddata->win;

	pddata->entry=dopus_curwin[win]->firstentry;
	pddata->titlebuf[0]=0;
	if ((dopus_curwin[win]->filesel+
		dopus_curwin[win]->dirsel)>0) pddata->onlysel=1;
	else pddata->onlysel=0;

	switch (ENTRYTYPE(pddata->entry->type)) {
		case ENTRY_DEVICE:
			strcpy(pddata->titlebuf,globstring[STR_DEVICE_LIST]);
			break;
		case ENTRY_CUSTOM:
			switch (pddata->entry->subtype) {
				case CUSTOMENTRY_DIRTREE:
					strcpy(pddata->titlebuf,globstring[STR_DIR_TREE]);
					strcat(pddata->titlebuf,"\n");
					goto printdiskname;
				default:
					strcpy(pddata->titlebuf,globstring[STR_CUSTOM_LIST]);
					break;
			}
			break;
		default:
printdiskname:						
			strcat(pddata->titlebuf,globstring[STR_DIRECTORY]);
			if (expand_path(str_pathbuffer[win],buf))
				strcat(pddata->titlebuf,buf);
			else strcat(pddata->titlebuf,str_pathbuffer[win]);
			break;
	}
	strcat(pddata->titlebuf,"\n");
}

void startnotify(win)
int win;
{
	if (!dos_notify_req[win]) return;
	main_proc->pr_WindowPtr=(APTR)-1;
	endnotify(win);
 	if (config->dynamicflags&UPDATE_NOTIFY && str_pathbuffer[win][0]) {
 		if (dopus_curwin[win]->disktot>(2*(1<<20))) {
	 		strcpy(dos_notify_names[win],str_pathbuffer[win]);
			dos_notify_req[win]->nr_UserData=win;
			dos_notify_req[win]->nr_Flags=NRF_SEND_MESSAGE;
			dos_notify_req[win]->nr_stuff.nr_Msg.nr_Port=count_port;
			if (!(StartNotify(dos_notify_req[win])))
				dos_notify_names[win][0]=0;
		}
	}
	if (config->errorflags&ERROR_ENABLE_DOS) main_proc->pr_WindowPtr=(APTR)Window;
}

void endnotify(win)
int win;
{
	if (dos_notify_req[win] && dos_notify_names[win][0]) {
		EndNotify(dos_notify_req[win]);
		dos_notify_names[win][0]=0;
	}
}

void startnotifies()
{
	int a;

	for (a=0;a<2;a++) {
		if (LStrCmpI(dos_notify_names[a],str_pathbuffer[a]))
			startnotify(a);
	}
}

void endnotifies()
{
	int a;

	for (a=0;a<2;a++) endnotify(a);
}

int dormbgadget(x1,y1,gad1,gad2)
int x1,y1;
struct newdopusfunction *gad1,*gad2;
{
	struct RMBGadget rmbgad;
	int a;

	if (config->generalscreenflags&SCR_GENERAL_INDICATERMB)
		do3dbox(main_rp,x1+2,y1+1,scrdata_gadget_width-4,scrdata_gadget_height-2);
	rmbgad.flags=RGF_ALTTEXT; rmbgad.id=0;
	
	if (gadget_unsel_border) {
		rmbgad.flags|=RGF_ALTBORD;
		rmbgad.high_bord[0]=gadget_unsel_border;
		rmbgad.high_bord[1]=gadget_sel_border;
	}
	for (a=0;a<2;a++) {
		rmbgad.txt[a].x=2;
		rmbgad.txt[a].y=1;
		rmbgad.txt[a].w=scrdata_gadget_width-4;
		rmbgad.txt[a].h=scrdata_gadget_height-2;
	}
	rmbgad.x=x1;
	rmbgad.y=y1;
	rmbgad.w=scrdata_gadget_width;
	rmbgad.h=scrdata_gadget_height;
	rmbgad.txt[0].fg=screen_pens[(int)gad1->fpen].pen;
	rmbgad.txt[0].bg=screen_pens[(int)gad1->bpen].pen;
	rmbgad.txt[0].text=gad1->name;
	rmbgad.txt[1].fg=screen_pens[(int)gad2->fpen].pen;
	rmbgad.txt[1].bg=screen_pens[(int)gad2->bpen].pen;
	rmbgad.txt[1].text=gad2->name;
	if (status_iconified<1) SetFont(main_rp,scr_font[FONT_GADGETS]);
	a=DoRMBGadget(&rmbgad,Window);
	if (status_iconified<1) SetFont(main_rp,scr_font[FONT_GENERAL]);
	if (config->generalscreenflags&SCR_GENERAL_INDICATERMB)
		drawgadgetcorner(main_rp,x1+2,y1+1);
	return(a);
}

int gadgetfrompos(x,y)
int x,y;
{
	int xg,yg,gad;

	if (y<scrdata_gadget_ypos-1 || x<scrdata_gadget_xpos) return(-1);
	x-=scrdata_gadget_xpos; y-=(scrdata_gadget_ypos-1);
	xg=x/scrdata_gadget_width; yg=y/scrdata_gadget_height;
	gad=(yg*7)+xg;
	if (gad<0 || gad>=(scr_gadget_rows*7)) return(-1);
	xg=(gad%7)*scrdata_gadget_width; yg=(gad/7)*scrdata_gadget_height;
	if (x>=xg && x<xg+scrdata_gadget_width && y>=yg && y<yg+scrdata_gadget_height) return(gad);
	return(-1);
}

int isvalidgad(gad)
struct newdopusfunction *gad;
{
	return((gad->name && gad->name[0] && gad->function && gad->function[0]));
}

int getpal()
{
	int p;
	struct Screen *screen;
	ULONG modeid=LORES_KEY;
	struct DisplayInfo displayinfo;

	p=(GfxBase->DisplayFlags&PAL)?1:0;
	
#warning Unimplemented DisplayInfo funcs
kprintf("getpal(): Unimplemented DisplayInfo funcs\n");
#if 1
	(void)displayinfo;
	(void)modeid;
	(void)screen;
#else
	if (system_version2>=OSVER_37) {
		if ((screen=LockPubScreen(NULL))) {
			if ((modeid=GetVPModeID(&(screen->ViewPort)))!=INVALID_ID) {
				if (!((modeid&MONITOR_ID_MASK)==NTSC_MONITOR_ID ||
					(modeid&MONITOR_ID_MASK)==PAL_MONITOR_ID))
					modeid=LORES_KEY;
			}
			UnlockPubScreen(NULL,screen);
		}
		if (GetDisplayInfoData(NULL,(UBYTE *)&displayinfo,sizeof(struct DisplayInfo),DTAG_DISP,modeid)) {
			if (displayinfo.PropertyFlags&DIPF_IS_PAL) p=1;
			else p=0;
		}
	}
#endif	
	return(p);
}

void quickfixmenus()
{
	int a;
	ULONG class;
	USHORT code;

	if (Window->MenuStrip) {
		Forbid();
		a=0;
		while (getintuimsg()) {
			class=IMsg->Class; code=IMsg->Code;
			if (class==MOUSEBUTTONS && code==MENUUP) {
				a=1;
				break;
			}
			ReplyMsg((struct Message *)IMsg);
		}
		if (!a) {
			Window->Flags&=~RMBTRAP;
			sendmouseevent(IECLASS_RAWMOUSE,IECODE_RBUTTON|IECODE_UP_PREFIX,0,0);
			sendmouseevent(IECLASS_RAWMOUSE,IECODE_RBUTTON,0,0);
		}
		Permit();
	}
}

void sendmouseevent(class,code,x,y)
UBYTE class;
UWORD code;
int x,y;
{
	struct InputEvent inputev;

	input_req->io_Command=IND_WRITEEVENT;
	input_req->io_Flags=0;
	input_req->io_Length=sizeof(struct InputEvent);
	input_req->io_Data=(APTR)&inputev;
	inputev.ie_NextEvent=NULL;
	inputev.ie_Class=class;
	inputev.ie_Code=code;
	inputev.ie_Qualifier=0;
	inputev.ie_X=x;
	inputev.ie_Y=y;
	DoIO((struct IORequest *)input_req);
}

char *getfiledescription(name,win)
char *name;
int win;
{
	int a;
	char buf[256];
	struct dopusfiletype *type;

	for (a=0;a<DISPLAY_LAST+1;a++) {
		if (config->displaypos[win][a]==DISPLAY_FILETYPE) break;
	}
	if (a>DISPLAY_LAST || !name[0]) return(NULL);
	StrCombine(buf,str_pathbuffer[win],name,256);
	if (!(type=checkfiletype(buf,-1,0))) return(NULL);
	if ((strcmp(type->type,"Default")==0) ||
		(strcmp(type->type,globstring[STR_FTYPE_DEFAULT])==0))
		return(globstring[STR_FTYPE_UNKNOWN]);
	return(type->type);
}

void fixhlen(win)
int win;
{
	int a;

	if (dopus_curwin[win]->total>0 &&
		(dopus_curwin[win]->firstentry->type==ENTRY_CUSTOM ||
		dopus_curwin[win]->firstentry->type==ENTRY_DEVICE)) {
		if (dopus_curwin[win]->firstentry->type==ENTRY_DEVICE)
			dopus_curwin[win]->hlen=scrdata_dispwin_nchars[win];
	}
	else {
		dopus_curwin[win]->hlen=0;
		if (dopus_curwin[win]->total>0 || str_pathbuffer[win][0]) {
			for (a=0;a<=DISPLAY_LAST;a++) {
				if (config->displaypos[win][a]==DISPLAY_NAME)
					dopus_curwin[win]->hlen+=config->displaylength[win][0];
				else if (config->displaypos[win][a]==DISPLAY_SIZE)
					dopus_curwin[win]->hlen+=8;
				else if (config->displaypos[win][a]==DISPLAY_PROTECT)
					dopus_curwin[win]->hlen+=9;
				else if (config->displaypos[win][a]==DISPLAY_DATE) {
					if (config->dateformat&DATE_12HOUR)
						dopus_curwin[win]->hlen+=20;
					else dopus_curwin[win]->hlen+=19;
				}
				else if (config->displaypos[win][a]==DISPLAY_COMMENT)
					dopus_curwin[win]->hlen+=config->displaylength[win][1];
				else if (config->displaypos[win][a]==DISPLAY_FILETYPE)
					dopus_curwin[win]->hlen+=config->displaylength[win][2];
				else if (AccountsBase) {
					if (config->displaypos[win][a]==DISPLAY_OWNER)
						dopus_curwin[win]->hlen+=config->displaylength[win][3];
					else if (config->displaypos[win][a]==DISPLAY_GROUP)
						dopus_curwin[win]->hlen+=config->displaylength[win][4];
					else if (config->displaypos[win][a]==DISPLAY_NETPROT)
						dopus_curwin[win]->hlen+=10;
				}
			}
		}
		if (dopus_curwin[win]->hlen<scrdata_dispwin_nchars[win])
			dopus_curwin[win]->hlen=scrdata_dispwin_nchars[win];
		if (dopus_curwin[win]->hoffset>=(dopus_curwin[win]->hlen-scrdata_dispwin_nchars[win]))
			dopus_curwin[win]->hoffset=dopus_curwin[win]->hlen-scrdata_dispwin_nchars[win];
	}
}


/* Get the name of the screen we are currently on */

char *get_our_pubscreen()
{
	char *name;

	if (Window) {
		/* Under 2.0 we lock the pubscreen list, and scan it to find
		   the screen we're on */
		if (system_version2) {
			struct List *pubscreenlist;
			struct PubScreenNode *node;

			if ((pubscreenlist=LockPubScreenList())) {
				for (node=(struct PubScreenNode *)pubscreenlist->lh_Head;
					node->psn_Node.ln_Succ;
					node=(struct PubScreenNode *)node->psn_Node.ln_Succ) {

					/* See if this node is our screen */
					if (node->psn_Screen==Window->WScreen) {

						/* Get name pointer */
						name=node->psn_Node.ln_Name;
						break;
					}
				}
				UnlockPubScreenList();
			}
		}

		/* Otherwise use default title */
		if (!name) name=Window->WScreen->DefaultTitle;
	}

	/* If no window open, use port name */
	else name=str_arexx_portname;

	return(name);
}


/* Change name of arexx port */

void change_port_name(name)
char *name;
{
	Forbid();
	RemPort(arexx_port);
	strncpy(str_arexx_portname,name,29);
	str_arexx_portname[29]=0;
	AddPort(arexx_port);
	Permit();
}

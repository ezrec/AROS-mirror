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
#include <graphics/gels.h>

int CXBRK(void);
void chkabort(void);

int  CXBRK(void) { return(0); }  /* Disable Lattice CTRL/C handling */
void chkabort(void) { return; } 

void main(argc,argv)
int argc;
char *argv[];
{
	int a,in,iconstart,sup,ck,nsee;
	struct WBStartup *WBmsg;
	struct WBArg *p;
	struct DiskObject *dobj;
	char **toolarray,*s;
	BPTR lock;


	/* Work out what version of the Operating System we're running.
	   Anything less than v37 (2.04) is counted as being v34 (1.3) */

	if (SysBase->LibNode.lib_Version>=40) system_version2=OSVER_40;
	else if (SysBase->LibNode.lib_Version==39) system_version2=OSVER_39;
	else if (SysBase->LibNode.lib_Version==38) system_version2=OSVER_38;
	else if (SysBase->LibNode.lib_Version==37) system_version2=OSVER_37;
	else system_version2=OSVER_34;


	/* Get pointer to our Process structure and set our WindowPtr for
	   errors to -1 (no errors appear). */

	main_proc=(struct Process *) FindTask(NULL);


	/* status_flags contains various flags; initialise it to 0 to start with */

	status_flags=0;


	/* Attempt to open the DOPUS.LIBRARY. Look first in default search path, and
	   then look for it on the distribution disk. If we can't find it, or
	   if our stack size is too low (must be >=16384 bytes), display an
	   appropriate alert and exit */

	if (!(DOpusBase=(struct DOpusBase *)OpenLibrary("dopus.library",DOPUSLIB_VERSION)))
		DOpusBase=(struct DOpusBase *)OpenLibrary("dopus:libs/dopus.library",DOPUSLIB_VERSION);

	if (!DOpusBase || main_proc->pr_StackSize<16384) {
		IntuitionBase=(struct IntuitionBase *) OpenLibrary("intuition.library",0);
		if (!DOpusBase) {
			char buf[80];

			lsprintf(&buf[2],"\xf                You need dopus.library v%ld+ to use Directory Opus!",DOPUSLIB_VERSION);
			buf[0]=0;
			buf[1]=0;
			buf[(strlen(buf)+1)]=0;
			DisplayAlert(RECOVERY_ALERT,buf,26);
		}
		else DisplayAlert(RECOVERY_ALERT,"\x00\x00\xf                       Stack must be at least 16384 bytes!\x00\x00",26);
		CloseLibrary((struct Library *) IntuitionBase);
#warning Changed _exit(0) to exit(0)
#ifdef __SASC_60
		exit(0);
#else
		exit(0);
#endif
	}


	windowptr_save=main_proc->pr_WindowPtr;
	main_proc->pr_WindowPtr=(APTR)-1;

	/* Initialise and open the other libraries that we need or want */

	GfxBase=DOpusBase->GfxBase;
	IntuitionBase=DOpusBase->IntuitionBase;
	LayersBase=(struct Library *)DOpusBase->LayersBase;

	DiskfontBase=OpenLibrary("diskfont.library",0);
	IconBase=OpenLibrary("icon.library",0);
	WorkbenchBase=OpenLibrary("workbench.library",37);
	CxBase=OpenLibrary("commodities.library",37);
	RexxSysBase=OpenLibrary("rexxsyslib.library",0);
	UtilityBase=(struct UtilityBase *)OpenLibrary("utility.library",37);

	AccountsBase=NULL;

	Forbid();
	if (FindName(&SysBase->LibList,"services.library")) {
		if ((AccountsBase=OpenLibrary("accounts.library",0))) {
#warning We do not have these libraries
#if 0
			user_info=AllocUserInfo();
			group_info=AllocGroupInfo();
#endif
		}
	}
	Permit();

	PPBase=open_dopus_library("powerpacker.library",0);
	MUSICBase=open_dopus_library("inovamusic.library",0);


	/* Restore window pointer now that we've got our libraries */

	main_proc->pr_WindowPtr=windowptr_save;


	/* Construct the version string from the #defined version numbers */

#ifdef DOPUS_SECRET_VERSION
	lsprintf(str_version_string,"%ld.%s",DOPUS_VERSION,DOPUS_SECRET_VERSION);
#else
	lsprintf(str_version_string,"%ld.%s",DOPUS_VERSION,DOPUS_REVISION);
#endif
#ifdef DOPUS_LETTERREV
	strcat(str_version_string,DOPUS_LETTERREV);
#endif


	/* Initialise various data */

	str_hunt_name[0]=0;
	str_search_string[0]=0;
	str_filter[0]=0;
	str_filter_parsed[0]=0;
	str_last_rexx_result[0]=0;

	func_single_file[0]=0;
	func_external_file[0]=0;

	for (a=0;a<NUMFONTS;a++) scr_font[a]=NULL;
	for (a=0;a<3;a++) external_mod_segment[a]=0;

	for (a=0;a<2;a++) {
		dos_notify_req[a]=NULL;
		dos_notify_names[a][0]=0;
		dopus_curwin[a]=dopus_firstwin[a]=NULL;
		data_buffer_count[a]=0;
		completion[a].memory=NULL;
		completion[a].firstentry=NULL;
		completion[a].currententry=NULL;
		completion[a].path[0]=0;
	}

	for (a=0;a<16;a++) screen_pens[a].alloc=0;

	func_reselection.reselection_list=NULL;

	old_pr_cis=main_proc->pr_CIS;
	old_pr_cos=main_proc->pr_COS;
	old_pr_consoletask=main_proc->pr_ConsoleTask;

	nil_file_handle=Open("NIL:",MODE_NEWFILE);
	main_proc->pr_CIS=nil_file_handle;
	main_proc->pr_COS=nil_file_handle;
	
#warning Set pr_ConsoleTask to something sensible instead of NULL
	main_proc->pr_ConsoleTask= NULL; // (APTR)((struct FileHandle *)BADDR(nil_file_handle))->fh_Type;

	scrdata_is_pal=getpal();
kprintf("returned from getpal()\n");
	iconstart=in=0;
	strcpy(str_config_basename,"DirectoryOpus");
	ck=1;

	if (argc>1) {
		for (a=1;a<argc;a++) {
			if (argv[a][0]=='-') {
				switch (argv[a][1]) {
					case 'i': iconstart=1; break;
					case 'b': iconstart=2; break;
					case 'c': LStrnCpy(str_config_basename,&argv[a][2],256); break;
					case 'g': ck=0; break;
					case 'q': status_flags|=STATUS_IANSCRAP; break;
					case 'Q': status_flags|=STATUS_IANSCRAP2; break;
				}
			}
		}
	}
	else if (argc==0 && IconBase) {
		WBmsg=(struct WBStartup *)argv;
		p=WBmsg->sm_ArgList;
		if ((dobj=GetDiskObject(p->wa_Name))) {
			toolarray=(char **)dobj->do_ToolTypes;
			if ((s=(char *)FindToolType((UBYTE **)toolarray,"ICONSTART")))
				iconstart=atoi(s);
			if ((s=(char *)FindToolType((UBYTE **)toolarray,"BUTTONSTART")))
				iconstart=atoi(s)?2:iconstart;
			if ((s=(char *)FindToolType((UBYTE **)toolarray,"CONFIGFILE")))
				LStrnCpy(str_config_basename,s,256);
			if ((s=(char *)FindToolType((UBYTE **)toolarray,"CHECK")))
				ck=atoi(s);
			FreeDiskObject(dobj);
		}
	}
kprintf("CallingBaseName()\n");

	if ((s=strstri(BaseName(str_config_basename),".CFG"))) *s=0;

	allocstrings();
	if (system_version2>=OSVER_39) nullpalette=(UWORD *)astring(3072);
	else nullpalette=(UWORD *)astring(64);

	scrdata_norm_width=GfxBase->NormalDisplayColumns;
	scrdata_norm_height=GfxBase->NormalDisplayRows;

	main_proc->pr_WindowPtr=(APTR)-1;
	

	FindSystemFile("DOpusRT",str_dopusrt_path,256,SYSFILE_COMMAND);
kprintf("System file found\n");
	config=(struct Config *)astring(sizeof(struct Config));
	filetype_key=NULL;
	dopus_firsttype=NULL;
	dopus_firstgadbank=NULL;
	dopus_curgadbank=NULL;
	dopus_firsthotkey=NULL;

	read_configuration(0);
kprintf("Configuration read\n");	

	if (!(lock=Lock("ENV:",ACCESS_READ))) Assign("ENV:","RAM:");
	else UnLock(lock);

	if (!(arexx_port=CreateUniquePort("DOPUS",str_arexx_portname,&system_dopus_runcount)))
		quit();
kprintf("ARexx port created\n");		

	rexx_signalbit=1<<arexx_port->mp_SigBit;

	if (!(install_arbiter()) ||
		!(count_port=LCreatePort(NULL,0)) ||
		!(general_port=LCreatePort(0,0))) quit();

	if (system_version2) {
		if (WorkbenchBase) {
			if (!(appmsg_port=LCreatePort(0,0))) quit();
		}
		for (a=0;a<2;a++) {
			if ((dos_notify_req[a]=LAllocRemember(&general_key,
				sizeof(struct NotifyRequest),
				MEMF_CLEAR)))
				dos_notify_req[a]->nr_Name=dos_notify_names[a];
		}
	}

	if (!(keyboard_req=(struct IOStdReq *)
		LCreateExtIO(general_port,sizeof(struct IOStdReq)))) quit();
	if ((OpenDevice("keyboard.device",0,(struct IORequest *)keyboard_req,0))!=0) {
		LDeleteExtIO((struct IORequest *)keyboard_req); keyboard_req=NULL;
		quit();
	}
	
kprintf("Keyboard opened\n");

	if (!(input_req=(struct IOStdReq *)
		LCreateExtIO(general_port,sizeof(struct IOStdReq)))) quit();
	if ((OpenDevice("input.device",0,(struct IORequest *)input_req,0))!=0) {
		LDeleteExtIO((struct IORequest *)input_req); input_req=NULL;
		quit();
	}

kprintf("input device opened\n");

#warning The below crashes DOpus and is a BUG in AROS. Must be commented out to work

#if 0
	ramdisk_lock=Lock("RAM:",ACCESS_READ);
#endif
	
kprintf("ramdisk locked\n");
	strcpy(str_select_pattern[0],"*");

kprintf("initing menus\n");
	init_menus();
kprintf("Menus inited\n");


	if (ck && (FindTask("dopus_hotkeez"))) {
		status_iconified=1;
		if (!(simplerequest(globstring[STR_DOPUS_ALREADY_RUNNING],
			globstring[STR_RUN],str_cancelstring,NULL)))
			quit();
		status_iconified=0;
	}
	
kprintf("hotkey stuff done\n");

	if (scrdata_is_pal) data_colorclock=3546895;
	else data_colorclock=3579545;

	setup_externals();
kprintf("externals setup\n");
	
	setupchangestate();
kprintf("chagestate setup\n");
	
	do_remember_config(remember_data);
	
kprintf("do_remember_config() called\n");

	if (!(dir_memory_pool=LibCreatePool(MEMF_ANY|MEMF_CLEAR,16384,1024)))
		quit();
		
kprintf("dir memory pool created\n");

	allocdirbuffers(config->bufcount);
kprintf("dirbuffers alloced\n");

	for (a=0;a<2;a++) horiz_propimage[a].Width=65;

	hotkey_task=(struct Task *) CreateTask("dopus_hotkeez",config->priority+1,hotkeytaskcode,2048);

kprintf("hotkey task created\n");

	if (iconstart) SetUp(-1);

	sup=nsee=0;
	if (config->autodirs[0][0] || config->autodirs[1][0]) {
		if (!iconstart) {
			sup=1;
			SetUp(1);
			busy();
		}
		nsee=1;
		for (a=0;a<2;a++) {
			if (config->autodirs[a][0]) {
				strcpy(str_pathbuffer[a],config->autodirs[a]);
				checkdir(str_pathbuffer[a],(sup)?&path_strgadget[a]:NULL);
				strcpy(dopus_curwin[a]->directory,str_pathbuffer[a]);
				getdir(dopus_curwin[a],a,0);
			}
		}
	}
	
	kprintf("autodirs checked\n");
	
	if (iconstart) {
		status_configuring=0;
		iconify(sup+1,iconstart-1,0);
	}
	else if (!sup) {
kprintf("Calling SetUp()\n");
		SetUp(1);
kprintf("SetUp() done\n");
	}
	if (nsee) {
		for (a=0;a<2;a++) {
			seename(a);
			checkdir(str_pathbuffer[a],&path_strgadget[a]);
			fixhorizprop(a);
		}
	}
	if (!iconstart) unbusy();
	
	kprintf("enter event loop\n");
	doidcmp();
}

int SetUp(tit)
int tit;
{
	int y,a,b,c,count=0,lim=48,num,other_offset;
	struct Screen scrbuf,*pubscr=NULL;
	struct DimensionInfo dims;
	DisplayInfoHandle *handle;
	struct dopusgadgetbanks *bank;
	struct RastPort testrastport;

	removewindowgadgets(Window);

	SetTaskPri((struct Task *)main_proc,config->priority);
	if (hotkey_task) SetTaskPri(hotkey_task,config->priority+1);
	status_configuring=-1; status_iconified=0;

	main_scr.ViewModes=HIRES;
	config->screenmode=checkscreenmode(config->screenmode);

	main_win.Flags=WFLG_NW_EXTENDED;
	mainwindow_tags[0].ti_Tag=TAG_SKIP;
	mainwindow_tags[0].ti_Data=0;
	if (status_flags&STATUS_IANSCRAP2) {
		mainwindow_tags[2].ti_Tag=TAG_SKIP;
		mainwindow_tags[2].ti_Data=0;
	}

	if (config->screenmode==MODE_PUBLICSCREEN && system_version2 &&
		(pubscr=LockPubScreen(config->pubscreen_name))) {
		CopyMem((char *)pubscr,(char *)&scrbuf,sizeof(struct Screen));
	}
	else {
		GetWBScreen(&scrbuf);
		if (config->screenmode==MODE_PUBLICSCREEN) config->screenmode=MODE_WORKBENCHUSE;
	}

	setup_draw_info();

	if (config->screenmode==MODE_WORKBENCHUSE || config->screenmode==MODE_PUBLICSCREEN)
		status_publicscreen=1;
	else {
		status_publicscreen=0;
		if (config->screenmode==MODE_WORKBENCHCLONE) {
			mainscreen_tags[SCREENTAGS_DISPLAYID].ti_Data=clone_screen(NULL,&main_scr);
		}
		else if (!system_version2) {
			if (config->screenflags&SCRFLAGS_DEFWIDTH)
				main_scr.Width=GfxBase->NormalDisplayColumns;
			else main_scr.Width=config->scrw;
			if (config->screenflags&SCRFLAGS_DEFHEIGHT) {
				if (config->screenmode==HIRESLACE_KEY)
					main_scr.Height=GfxBase->NormalDisplayRows*2;
				else main_scr.Height=GfxBase->NormalDisplayRows;
			}
			else main_scr.Height=config->scrh;
			if (config->screenmode==HIRESLACE_KEY) main_scr.ViewModes|=LACE;
		}
		else {
#warning Unimplemeted DisplayInfo funcs
kprintf("Setup(): Unimplemeted DisplayInfo funcs\n");
			if ( 0 /* AROS (handle=FindDisplayInfo(config->screenmode)) &&
				(GetDisplayInfoData(handle,(UBYTE *)&dims,sizeof(struct DimensionInfo),
				DTAG_DIMS,0))
			*/ ) {
#if 0			
				mainscreen_tags[SCREENTAGS_DISPLAYID].ti_Data=(ULONG)config->screenmode;
				if (config->screenflags&SCRFLAGS_DEFWIDTH)
					main_scr.Width=(dims.TxtOScan.MaxX-dims.TxtOScan.MinX)+1;
				else main_scr.Width=config->scrw;
				if (config->screenflags&SCRFLAGS_DEFHEIGHT)
					main_scr.Height=(dims.TxtOScan.MaxY-dims.TxtOScan.MinY)+1;
				else main_scr.Height=config->scrh;
#endif
			}
			else {
				main_scr.Width=scrbuf.Width;
				main_scr.Height=scrbuf.Height;
				main_scr.ViewModes=scrbuf.ViewPort.Modes;
			}
		}
	}

	if (config->scrdepth<2) config->scrdepth=2;
	main_scr.Depth=config->scrdepth;

	if (config->gadgetrows<0 || config->gadgetrows>6) config->gadgetrows=6;
	scr_gadget_rows=config->gadgetrows;
	if (data_gadgetrow_offset+scr_gadget_rows>6) data_gadgetrow_offset=0;

tryfonts:
	if (count==5) {
		status_iconified=1;
		simplerequest(globstring[STR_UNABLE_TO_OPEN_WINDOW],globstring[STR_CONTINUE],NULL);
		status_iconified=0;
		quit();
	}
	else if (count==4) {
		for (a=0;a<NUMFONTS;a++) {
			if (config->fontsizes[a]>8) {
				strcpy(config->fontbufs[a],"topaz.font");
				config->fontsizes[a]=8;	
			}
		}
	}
	else if (count) {
		if (Window) busy();
		for (a=0;a<NUMFONTS;a++) {
			if (config->fontsizes[a]>lim) config->fontsizes[a]/=2;
		}
		lim/=2;
	}

	config->fontsizes[0]=8;
	for (a=0;a<FONT_LAST;a++) {
		freefont(a);
		b=config->fontsizes[a];
		scr_font[a]=getfont(config->fontbufs[a],&b,(a<3));
		config->fontsizes[a]=b;
	}

	if (Window && count) unbusy();

	scrdata_font_xsize=scr_font[FONT_DIRS]->tf_XSize;
	scrdata_font_ysize=scr_font[FONT_DIRS]->tf_YSize;
	scrdata_font_baseline=scr_font[FONT_DIRS]->tf_Baseline;

	screen_attr.ta_Name=(STRPTR)config->fontbufs[FONT_GENERAL];
	menu_attr.ta_Name=(STRPTR)config->fontbufs[FONT_MENUS];
	menu_attr.ta_YSize=config->fontsizes[FONT_MENUS];
	main_screen_attr.ta_Name=(STRPTR)config->fontbufs[FONT_SCREEN];
	main_screen_attr.ta_YSize=config->fontsizes[FONT_SCREEN];

	if (tit>-1) {
		if (config->screenmode==MODE_WORKBENCHUSE) main_win.Type=WBENCHSCREEN;
		else if (pubscr) {
			main_win.Type=PUBLICSCREEN;
			mainwindow_tags[0].ti_Tag=WA_PubScreen;
			mainwindow_tags[0].ti_Data=(ULONG)pubscr;
		}
		else {
			main_win.Type=CUSTOMSCREEN;

			if (system_version2) {
				main_scr.LeftEdge=(GfxBase->NormalDisplayColumns-main_scr.Width)/2;
				if (main_scr.LeftEdge<0) main_scr.LeftEdge=0;
			}
			else main_scr.LeftEdge=0;

			main_scr.TopEdge=0;
			if (config->screenflags&SCRFLAGS_HALFHEIGHT) {
				if (main_scr.Height<400) config->screenflags&=~SCRFLAGS_HALFHEIGHT;
				else {
					main_scr.Height/=2;
					main_scr.TopEdge=main_scr.Height;
				}
			}

			if (!MainScreen) {
				if (!(MainScreen=(struct Screen *) OpenScreen((struct NewScreen *)&main_scr))) {
					status_iconified=1;
					simplerequest(globstring[STR_UNABLE_TO_OPEN_WINDOW],globstring[STR_CONTINUE],NULL);
					status_iconified=0;
					if (config->screenmode==HIRES_KEY && config->scrw==640 &&
						config->scrh==200+(scrdata_is_pal*56) && config->screenflags==0 &&
						config->scrdepth==2) quit();
					config->screenmode=HIRES_KEY;
					config->scrw=640;	
					config->scrh=200+(scrdata_is_pal*56);
					config->scrdepth=2;
					config->screenflags=0;
					if (pubscr) UnlockPubScreen(NULL,pubscr);
					return(SetUp(tit));
				}
				main_vp=&(MainScreen->ViewPort);
				load_palette(MainScreen,config->new_palette);

				main_win.Screen=MainScreen;
				if (system_version2) PubScreenStatus(MainScreen,0);
				if (status_flags&STATUS_IANSCRAP2) ShowTitle(MainScreen,FALSE);
				CopyMem((char *)MainScreen,(char *)&scrbuf,sizeof(struct Screen));
			}
		}
	}

	if (config->generalscreenflags&SCR_GENERAL_WINBORDERS && MainScreen)
		status_publicscreen=1;

	/* Screen clock bar */

	scrdata_clock_height=scr_font[FONT_CLOCK]->tf_YSize+2;
	if (scrdata_clock_height<10) scrdata_clock_height=10;

	/* Status bar */

	if (config->generalscreenflags&SCR_GENERAL_TITLESTATUS)
		scrdata_status_height=0;
	else scrdata_status_height=scr_font[FONT_STATUS]->tf_YSize+3;

	/* Disk name bars */

	scrdata_diskname_height=scr_font[FONT_NAMES]->tf_YSize+2;

	/* String gadgets */

	scrdata_string_height=config->stringheight;
	if (scrdata_string_height<scr_font[FONT_STRING]->tf_YSize)
		scrdata_string_height=scr_font[FONT_STRING]->tf_YSize;

	/* Calculate window borders */

	if (status_publicscreen) {
		if (MainScreen) {
			scrdata_xoffset=MainScreen->WBorLeft;	
			scrdata_yoffset=MainScreen->WBorTop+scr_font[FONT_SCREEN]->tf_YSize+1;
		}
		else {
			scrdata_xoffset=scrbuf.WBorLeft;
			scrdata_yoffset=scrbuf.WBorTop+scrbuf.Font->ta_YSize+1;
		}
	}
	else {
		scrdata_xoffset=0;
		if (status_flags&STATUS_IANSCRAP2) scrdata_yoffset=0;
		else if (MainScreen) scrdata_yoffset=MainScreen->BarHeight+1;
		else scrdata_yoffset=GfxBase->DefaultFont->tf_YSize+3;
	}

	/* Calculcate minimum window width and height */

	scr_min_width=(config->sliderwidth+8)*2+(config->arrowsize[1]+config->arrowsize[2])*4;
	if (scr_gadget_rows) scr_min_width+=8*(scr_font[FONT_GADGETS]->tf_XSize*2);
	scr_min_width+=scrdata_font_xsize*8;

	scr_min_height=
		((status_publicscreen)?scrdata_yoffset:0)+
		scrdata_status_height+
		scrdata_diskname_height+
		(config->arrowsize[0]*3)+
		config->sliderheight+
		scrdata_string_height+
		scrdata_clock_height+
		(scr_font[FONT_GADGETS]->tf_YSize*2)+
		20;

	if (main_scr.Width<scr_min_width) main_scr.Width=scr_min_width;
	if (main_scr.Height<scr_min_height) main_scr.Height=scr_min_height;
	if (Window) WindowLimits(Window,scr_min_width,scr_min_height,-1,-1);
	else {
		main_win.MinWidth=scr_min_width;
		main_win.MinHeight=scr_min_height;
	}

	/* Get window size */

	if (status_publicscreen) main_win.Width=config->scr_winw;
	else main_win.Width=config->scrw;
	if (config->screenflags&SCRFLAGS_DEFWIDTH || main_win.Width<0)
		main_win.Width=scrbuf.Width;
	if (main_win.Width>scrbuf.Width) main_win.Width=scrbuf.Width;

	if (status_publicscreen) main_win.Height=config->scr_winh;
	else main_win.Height=main_scr.Height;
	if (config->screenflags&SCRFLAGS_DEFHEIGHT || main_win.Height<0) {
		main_win.Height=scrbuf.Height;
		if (status_publicscreen) {
			config->scr_winy=scrbuf.BarHeight+1;
			main_win.Height-=config->scr_winy;
		}
	}
	if (main_win.Height>scrbuf.Height) main_win.Height=scrbuf.Height;

	/* If window is already open use it's dimensions (this is a mess actually) */
	if (Window) {
		main_win.Width=Window->Width;
		main_win.Height=Window->Height;
	}

	/* Adjust for window borders */

	if (status_publicscreen) {
		scrdata_width=main_win.Width-scrdata_xoffset-scrbuf.WBorRight;
		scrdata_height=main_win.Height-scrdata_yoffset-scrbuf.WBorBottom;
	}
	else {
		scrdata_width=main_scr.Width-scrdata_xoffset;
		scrdata_height=main_scr.Height-scrdata_yoffset;
	}

	/* Screen clock bar */

	scrdata_clock_width=scrdata_width-((config->generalscreenflags&SCR_GENERAL_TINYGADS)?115:1);
	scrdata_clock_xpos=scrdata_xoffset+2;
	scrdata_clock_ypos=(scrdata_yoffset+scrdata_height+1)-scrdata_clock_height+1;

	/* Disk name bars */

	scrdata_diskname_ypos=scrdata_status_height+1+scrdata_yoffset;

	/* Fuck knows */

	scrdata_scr_height=scrdata_height;
	scrdata_gadget_height=scr_font[FONT_GADGETS]->tf_YSize+2;

	FOREVER {
		other_offset=
			(scrdata_diskname_ypos-scrdata_yoffset)+
			scrdata_diskname_height+
			scrdata_string_height+
			config->sliderheight+
			(scr_gadget_rows*scrdata_gadget_height)+
			scrdata_clock_height+9;

		scrdata_dispwin_ypos=scrdata_diskname_ypos+scrdata_diskname_height;

		scrdata_dispwin_height=scrdata_scr_height-other_offset;/*+scrdata_yoffset;*/

		if (((config->arrowsize[0]*2)+8+scrdata_font_ysize)>scrdata_dispwin_height ||
			(other_offset>scrdata_scr_height-(config->sliderheight+32))) {

			if (scr_gadget_rows==6) scr_gadget_rows=3;
			else if (scr_gadget_rows>0) --scr_gadget_rows;
			else {
				++count;
				goto tryfonts;
			}
			continue;
		}
		else break;
	}

	scrdata_dispwin_lines=scrdata_dispwin_height/scrdata_font_ysize;
	scrdata_dirwin_height=scrdata_dispwin_lines*scrdata_font_ysize;
	scrdata_dirwin_bottom=
		scrdata_yoffset+
		(scrdata_scr_height-
		scrdata_string_height-
		config->sliderheight-
		(scr_gadget_rows*scrdata_gadget_height)-
		scrdata_clock_height-32);

	scrdata_gadget_ypos=scrdata_dirwin_bottom+33+scrdata_string_height+config->sliderheight;

	scrdata_prop_height=scrdata_dispwin_height-((config->arrowsize[0]+2)*2);

	scrdata_gadget_width=scrdata_width/8;
	scrdata_drive_width=0;
	InitRastPort(&testrastport);
	SetFont(&testrastport,scr_font[FONT_GADGETS]);
	for (a=0;a<USEDRIVECOUNT;a++) {
		if ((b=TextLength(&testrastport,config->drive[a].name,
			strlen(config->drive[a].name)))>scrdata_drive_width) scrdata_drive_width=b;
	}
	if (scrdata_drive_width>0) {
		scrdata_drive_width+=4;
		if (scrdata_drive_width<=scrdata_gadget_width)
			scrdata_gadget_width=((scrdata_width-scrdata_drive_width)/7);
		scrdata_drive_width=scrdata_width-(scrdata_gadget_width*7);
	}
	else {
		scrdata_drive_width=0;
		scrdata_gadget_width=scrdata_width/7;
	}

	string_extend.Font=scr_font[FONT_STRING];
	if (config->stringfgcol==config->stringbgcol)
		++config->stringfgcol;
	if (config->stringselfgcol==config->stringselbgcol)
		++config->stringselfgcol;
	path_stringinfo[0].Extension=&string_extend;
	path_stringinfo[1].Extension=&string_extend;

	if (scr_gadget_rows) {
		bank=dopus_firstgadbank;
		for (num=0;bank;num++) bank=bank->next;
		scr_gadget_bank_count=(num*6)/scr_gadget_rows;
	}
	else scr_gadget_bank_count=0;

	if (config->generalscreenflags&SCR_GENERAL_GADSLIDERS && scr_gadget_rows) {
		int slider_width=0;

		if (scrdata_drive_width) slider_width+=16;
		if (scr_gadget_bank_count>1) slider_width+=16;

		scrdata_gadget_width=((scrdata_width-slider_width-scrdata_drive_width)/7);
		a=scrdata_width-((scrdata_gadget_width*7)+scrdata_drive_width);

		if (scrdata_drive_width) {
			if (scr_gadget_bank_count>1) {
				drive_propgad.Width=gadget_propgad.Width=(a/2)-8;
				if (a%2) ++gadget_propgad.Width;
			}
			else {
				drive_propgad.Width=a-8;
				if (drive_propgad.Width>8) {
					scrdata_drive_width+=drive_propgad.Width-8;
					drive_propgad.Width=8;
				}
			}
		}
		else gadget_propgad.Width=a-8;
	}

	scrdata_gadget_offset=(scrdata_width-((scrdata_gadget_width*7)+scrdata_drive_width))/2;
	if (config->generalscreenflags&SCR_GENERAL_GADSLIDERS && scr_gadget_rows) {
		if (scr_gadget_bank_count>1) scrdata_gadget_offset-=gadget_propgad.Width+8;
		else if (scrdata_drive_width) scrdata_gadget_offset-=drive_propgad.Width+8;
	}
	if (scrdata_gadget_offset>scrdata_width) scrdata_gadget_offset=0;

	scrdata_gadget_offset+=scrdata_xoffset;
	scrdata_gadget_xpos=scrdata_gadget_offset+scrdata_drive_width;

#warning Always assuming SCR_GENERAL_NEWLOOKPROP because AROS prop gadgets dont support non AUTOKNOB (= custom image as knob) yet

/*	if (config->generalscreenflags&SCR_GENERAL_NEWLOOKPROP && system_version2) { */
	if (1) {
		status_flags|=STATUS_NEWLOOK;
		for (a=0;a<2;a++) {
			vert_propinfo[a].Flags=AUTOKNOB|PROPNEWLOOK|FREEVERT|PROPBORDERLESS;
			horiz_propinfo[a].Flags=AUTOKNOB|PROPNEWLOOK|FREEHORIZ|PROPBORDERLESS;
		}
		drive_propinfo.Flags|=AUTOKNOB|PROPNEWLOOK|FREEVERT|PROPBORDERLESS;
		gadget_propinfo.Flags|=AUTOKNOB|PROPNEWLOOK|FREEVERT|PROPBORDERLESS;
	}
	else {
		status_flags&=~STATUS_NEWLOOK;
		for (a=0;a<2;a++) {
			vert_propinfo[a].Flags=FREEVERT|PROPBORDERLESS;
			horiz_propinfo[a].Flags=FREEHORIZ|PROPBORDERLESS;
		}
		drive_propinfo.Flags=FREEVERT|PROPBORDERLESS;
		gadget_propinfo.Flags=FREEVERT|PROPBORDERLESS;
	}

	if (config->generalscreenflags&SCR_GENERAL_GADSLIDERS && scr_gadget_rows) {

		if (scrdata_drive_width>0) {
			scrdata_gadget_xpos+=drive_propgad.Width+8;

			drive_propgad.LeftEdge=scrdata_gadget_offset+scrdata_drive_width+4;
			drive_propgad.TopEdge=scrdata_gadget_ypos+1;
			drive_propgad.Height=scr_gadget_rows*scrdata_gadget_height-4;
			drive_propimage.Width=drive_propgad.Width;
			drive_propimage.Height=drive_propgad.Height;
			FixSliderBody(NULL,&drive_propgad,USEDRIVECOUNT,scr_gadget_rows,0);
		}

		if (scr_gadget_bank_count>1) {
			gadget_propgad.LeftEdge=scrdata_gadget_xpos+(scrdata_gadget_width*7)+4;
			gadget_propgad.TopEdge=scrdata_gadget_ypos+1;
			gadget_propgad.Height=scr_gadget_rows*scrdata_gadget_height-4;
			gadget_propimage.Width=gadget_propgad.Width;
			gadget_propimage.Height=gadget_propgad.Height;
			FixSliderBody(NULL,&gadget_propgad,scr_gadget_bank_count,1,0);
		}

		if (status_flags&STATUS_NEWLOOK) gadget_propimage.Height=0;
	}

	if (tit>-1) {
		layoutcenter(config->windowdelta);

		if (config->generalscreenflags&SCR_GENERAL_TINYGADS) {
			for (a=0;a<6;a++) {
				screen_gadgets[SCRGAD_TINYHELP+a].LeftEdge=
					scrdata_clock_xpos+scrdata_clock_width+(a*19)-1;
				screen_gadgets[SCRGAD_TINYHELP+a].TopEdge=
					scrdata_clock_ypos-2;
				screen_gadgets[SCRGAD_TINYHELP+a].Height=scrdata_clock_height;
			}
			screen_gadgets[SCRGAD_TINYHELP].Flags&=~GFLG_SELECTED;
			screen_gadgets[SCRGAD_TINYAREXX].NextGadget=&screen_gadgets[SCRGAD_TINYHELP];
		}
		else screen_gadgets[SCRGAD_TINYAREXX].NextGadget=&screen_gadgets[SCRGAD_MOVELEFT1];

		for (y=0;y<2;y++) {
			fixhlen(y);
			vert_propimage[y].LeftEdge=horiz_propimage[y].LeftEdge=0;
			vert_propimage[y].TopEdge=horiz_propimage[y].TopEdge=0;
			vert_propimage[y].Width=config->sliderwidth;
			vert_propimage[y].Height=0;
			horiz_propimage[y].Height=config->sliderheight;
			vert_propimage[y].ImageData=horiz_propimage[y].ImageData=NULL;
			vert_propimage[y].PlanePick=horiz_propimage[y].PlanePick=0;
			vert_propimage[y].NextImage=horiz_propimage[y].NextImage=NULL;
			FixSliderBody(NULL,&horiz_propgad[y],dopus_curwin[y]->hlen,scrdata_dispwin_nchars[y],0);
		}

		if (status_publicscreen) {
			if (MainScreen) {
				main_win.LeftEdge=config->scr_winx;
				main_win.TopEdge=config->scr_winy;
			}
			else {
				main_win.LeftEdge=config->wbwinx;
				main_win.TopEdge=config->wbwiny;
			}
			if (main_win.LeftEdge+main_win.Width>scrbuf.Width)
				main_win.LeftEdge=scrbuf.Width-main_win.Width;
			if (main_win.TopEdge+main_win.Height>scrbuf.Height)
				main_win.TopEdge=scrbuf.Height-main_win.Height;
			if (main_win.LeftEdge<0) main_win.LeftEdge=0;
			if (main_win.TopEdge<0) main_win.TopEdge=0;
			main_win.Flags|=WFLG_ACTIVATE|WFLG_RMBTRAP|WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET;
			main_win.Title=str_arexx_portname;
		}
		else {
			main_win.LeftEdge=0;
			main_win.TopEdge=0;
			main_win.Flags|=WFLG_ACTIVATE|WFLG_BACKDROP|WFLG_RMBTRAP|WFLG_BORDERLESS;
			main_win.Title=NULL;
		}

		if (config->generalscreenflags&SCR_GENERAL_NEWLOOKMENU)
			main_win.Flags|=WFLG_NEWLOOKMENUS;

		if (!Window) {
			if (!(Window=OpenWindow((struct NewWindow *)&main_win))) {
				if (MainScreen) ScreenToFront(MainScreen);
				simplerequest(globstring[STR_UNABLE_TO_OPEN_WINDOW],globstring[STR_CONTINUE],NULL);
				if (config->screenmode==HIRES_KEY && config->scr_winw==640 &&
					config->scr_winh==200+(scrdata_is_pal*56) && config->scrdepth==2 &&
					config->screenflags==0) quit();
				config->screenmode=HIRES_KEY;
				config->scr_winw=640;
				config->scr_winh=200+(scrdata_is_pal*56);
				config->scrdepth=2;
				config->screenflags=0;
				if (MainScreen) CloseScreen(MainScreen);
				MainScreen=NULL;
				if (pubscr) UnlockPubScreen(NULL,pubscr);
				return(SetUp(tit));
			}
			main_win.Width=Window->Width;
			main_win.Height=Window->Height;
			if (pubscr) ScreenToFront(pubscr);
			get_colour_table();
		}
		else {
			RefreshWindowFrame(Window);
			SetAPen(main_rp,0);
			rectfill(main_rp,
				scrdata_xoffset,
				scrdata_yoffset,
				scrdata_width,
				scrdata_height);
		}

		main_rp=Window->RPort;
		main_rp->GelsInfo=&drag_gelsinfo;
		InitGels(&drag_dummy_sprite1,&drag_dummy_sprite2,&drag_gelsinfo);

		string_extend.Pens[0]=screen_pens[(int)config->stringfgcol].pen;
		string_extend.Pens[1]=screen_pens[(int)config->stringbgcol].pen;
		string_extend.ActivePens[0]=screen_pens[(int)config->stringselfgcol].pen;
		string_extend.ActivePens[1]=screen_pens[(int)config->stringselbgcol].pen;

		drive_propimage.PlaneOnOff=screen_pens[(int)config->slidercol].pen;
		gadget_propimage.PlaneOnOff=screen_pens[(int)config->slidercol].pen;
		drive_propgad.MutualExclude=screen_pens[(int)config->sliderbgcol].pen;
		gadget_propgad.MutualExclude=screen_pens[(int)config->sliderbgcol].pen;

		for (y=0;y<2;y++) {
			vert_propimage[y].PlaneOnOff=screen_pens[(int)config->slidercol].pen;
			horiz_propimage[y].PlaneOnOff=screen_pens[(int)config->slidercol].pen;
			vert_propimage[y].NextImage=horiz_propimage[y].NextImage=NULL;
			vert_propgad[y].MutualExclude=screen_pens[(int)config->sliderbgcol].pen;
			horiz_propgad[y].MutualExclude=screen_pens[(int)config->sliderbgcol].pen;
		}

		if (status_publicscreen) {
#warning Function AddAppWindowA not implemented yet
		
			if (WorkbenchBase && !dopus_appwindow)
				dopus_appwindow=
				/* AROS: not implemented
				    AddAppWindowA(APPWINID,0,Window,appmsg_port,NULL); */
				    NULL;

			SetWindowTitles(Window,(char *)-1,str_arexx_portname);
			screen_gadgets[SCRGAD_LEFTPARENT].Width=Window->BorderLeft+2;
			screen_gadgets[SCRGAD_RIGHTPARENT].Width=Window->BorderRight+2;

			size_gadgets[0].LeftEdge=Window->Width-Window->BorderRight-1;
			size_gadgets[0].TopEdge=Window->Height-16;
			size_gadgets[0].Width=Window->BorderRight+1;
			size_gadgets[0].Height=17;

			size_gadgets[1].LeftEdge=Window->Width-32;
			size_gadgets[1].TopEdge=Window->Height-Window->BorderBottom-1;
			size_gadgets[1].Width=33;
			size_gadgets[1].Height=Window->BorderBottom+1;
		}
		else {
			screen_gadgets[SCRGAD_LEFTPARENT].Width=2;
			screen_gadgets[SCRGAD_RIGHTPARENT].Width=2;
		}

		if (Window->WScreen->ViewPort.Modes&LACE) status_flags|=STATUS_SQUAREPIXEL;
		else status_flags&=~STATUS_SQUAREPIXEL;

		if (config->generalflags&GENERAL_DRAG) allocdragbuffers();

		b=(scrdata_dispwin_height-scrdata_dirwin_height)/2;
		if (b<0) b=0;

		if (scrdata_dispwin_lines<5) c=scrdata_font_ysize/2;
		else if (scrdata_dispwin_lines<9) c=scrdata_font_ysize*2;
		else c=scrdata_font_ysize*3;

		for (a=0;a<2;a++) {
			scrdata_dirwin_ypos[a]=scrdata_dispwin_ypos+b;
			CopyMem((char *)main_rp,(char *)&dir_rp[a],sizeof(struct RastPort));
			SetFont(&dir_rp[a],scr_font[FONT_DIRS]);
			SetDrMd(&dir_rp[a],JAM2);

			scr_scroll_borders[a].MinX=
				scrdata_dirwin_xpos[a]+(((scrdata_dispwin_nchars[a]/2)-6)*scrdata_font_xsize);
			scr_scroll_borders[a].MaxX=
				scrdata_dirwin_xpos[a]+(((scrdata_dispwin_nchars[a]/2)+6)*scrdata_font_xsize);
			scr_scroll_borders[a].MinY=
				scrdata_dirwin_ypos[a]+(((scrdata_dispwin_lines/2)*scrdata_font_ysize)-c);
			scr_scroll_borders[a].MaxY=
				scrdata_dirwin_ypos[a]+(((scrdata_dispwin_lines/2)*scrdata_font_ysize)+c);
		}

		if (scrdata_status_height>0) scrdata_status_width=scrdata_width-4;
		else {
			struct Gadget *gad;
			int min=0,max=0;

			if (status_publicscreen) {
				gad=Window->FirstGadget;

				while (gad) {
					if (gad->GadgetType&GTYP_SYSGADGET && gad->TopEdge==0) {
						if (gad->LeftEdge<0) {
							if (gad->LeftEdge<min) min=gad->LeftEdge;
						}
						else if (gad->LeftEdge+gad->Width>max) max=gad->LeftEdge+gad->Width;
					}
					gad=gad->NextGadget;
				}

				scrdata_status_width=Window->Width-max+min-8-Window->WScreen->BarHBorder;
			}
			else {
				gad=MainScreen->FirstGadget;

				while (gad) {
					if (gad->LeftEdge<min) min=gad->LeftEdge;
					gad=gad->NextGadget;
				}

				scrdata_status_width=MainScreen->Width+min-8-MainScreen->BarHBorder;
			}
		}

		scrdata_status_xpos=2+scrdata_xoffset;
		scrdata_status_ypos=1+scrdata_yoffset;

		if (MainScreen) ScreenToFront(MainScreen);
		if (config->errorflags&ERROR_ENABLE_DOS)
			main_proc->pr_WindowPtr=(APTR)Window;
		else
			main_proc->pr_WindowPtr=(APTR)-1;

		layout_menus();

		setupdisplay(1);

		initclock();
	}

	status_iconified=0;
	if (tit==1) dostatustext(globstring[STR_WELCOME_TO_DOPUS]);
	else if (tit==2) dostatustext(globstring[STR_WELCOME_BACK_TO_DOPUS]);
	if (tit>-1 && !(status_flags&STATUS_DONEREXX)) {
		status_flags|=STATUS_DONEREXX;
		rexx_command(config->startupscript,NULL);
	}
	status_configuring=0;

	if (pubscr) UnlockPubScreen(NULL,pubscr);
	return(1);
}

void setupdisplay(all)
int all;
{
	int a;
	drawscreen();
	for (a=0;a<2;a++) {
		if (config->generalscreenflags&SCR_GENERAL_NEWLOOKPROP && system_version2)
			RefreshGList(horiz_propgad,Window,NULL,2);
		fixhorizprop(a);
		if (!all) doposhprop(a);
	}
	setupgadgets();
	drawgadgets((all)?1:-1,0);
	fix_rmb_gadgets();
	if (config->generalscreenflags&SCR_GENERAL_TINYGADS)
		dolittlegads(&screen_gadgets[SCRGAD_TINYFILTER],&globstring[STR_TINY_BUTTONS][6],1);

	if (scr_gadget_rows) {
		if (config->generalscreenflags&SCR_GENERAL_GADSLIDERS) {
			if (scrdata_drive_width) {
				FixSliderPot(NULL,&drive_propgad,data_drive_offset,
					USEDRIVECOUNT,scr_gadget_rows,0);
				ShowSlider(Window,&drive_propgad);
			}
			if (scr_gadget_bank_count>1) {
				fixgadgetprop();
				ShowSlider(Window,&gadget_propgad);
			}
		}
		data_drive_offset-=scr_gadget_rows;
		nextdrives();
	}

	for (a=0;a<2;a++) {
		refreshwindow(a,3);
		doposprop(a);
	}
}

void drawscreen()
{
	int a;

	SetDrMd(main_rp,JAM1);

	dolittlegads(&screen_gadgets[SCRGAD_TINYBUFFERLIST],globstring[STR_TINY_BUTTONS],
		config->generalscreenflags&SCR_GENERAL_TINYGADS?10:4);

	/* Directory Windows */

	for (a=0;a<2;a++) {
		SetAPen(main_rp,screen_pens[(int)config->filesbg].pen);
		rectfill(main_rp,
			scrdata_dispwin_xpos[a]+2,
			scrdata_dispwin_ypos,
			scrdata_dispwin_width[a],
			scrdata_dispwin_height);
		do3dbox(main_rp,
			scrdata_dispwin_xpos[a]+2,
			scrdata_dispwin_ypos,
			scrdata_dispwin_width[a],
			scrdata_dispwin_height);
		do3dbox(main_rp,
			scrdata_diskname_xpos[a]+2,
			scrdata_diskname_ypos,
			scrdata_diskname_width[a],
			scrdata_diskname_height-2);
	}

	/* Sliders/String gadgets */

	for (a=0;a<2;a++) {
		do3dbox(main_rp,
			vert_propgad[a].LeftEdge-2,
			vert_propgad[a].TopEdge-1,
			config->sliderwidth+4,
			scrdata_prop_height+2);
		do3dbox(main_rp,
			horiz_propgad[a].LeftEdge-2,
			horiz_propgad[a].TopEdge-1,
			horiz_propgad[a].Width+4,
			config->sliderheight+2);
		Do3DStringBox(main_rp,
			path_strgadget[a].LeftEdge,
			path_strgadget[a].TopEdge,
			path_strgadget[a].Width,
			scrdata_string_height,
			screen_pens[(int)config->gadgettopcol].pen,
			screen_pens[(int)config->gadgetbotcol].pen);
	}

	/* Arrows */

	doarrowgadgets(&screen_gadgets[SCRGAD_MOVEUP1],0);
	doarrowgadgets(&screen_gadgets[SCRGAD_MOVEDOWN1],1);
	doarrowgadgets(&screen_gadgets[SCRGAD_MOVELEFT1],3);
	doarrowgadgets(&screen_gadgets[SCRGAD_MOVERIGHT1],2);
	doarrowgadgets(&screen_gadgets[SCRGAD_BUFFERLEFT1],3);
	doarrowgadgets(&screen_gadgets[SCRGAD_BUFFERRIGHT1],2);

	/* Status bar */

	if (scrdata_status_height>0) {
		do3dbox(main_rp,
			scrdata_status_xpos,
			scrdata_status_ypos,
			scrdata_status_width,
			scrdata_status_height-2);
		SetAPen(main_rp,screen_pens[(int)config->statusbg].pen);
		rectfill(main_rp,
			scrdata_status_xpos,
			scrdata_status_ypos,
			scrdata_status_width,
			scrdata_status_height-2);
	}

	/* Clock bar */

	if (!clock_task) {
		SetAPen(main_rp,screen_pens[(int)config->clockbg].pen);
		rectfill(main_rp,
			scrdata_clock_xpos,
			scrdata_clock_ypos,
			scrdata_clock_width-3,
			scrdata_clock_height-2);
		do3dbox(main_rp,
			scrdata_clock_xpos,
			scrdata_clock_ypos-1,
			scrdata_clock_width-3,
			scrdata_clock_height-2);
	}

	doactive(0,0);

	SetAPen(main_rp,screen_pens[1].pen);
}

struct TextFont *getfont(font,size,noprop)
char *font;
int *size,noprop;
{
	BPTR lock;
	struct TextFont *tf;
	static struct TextAttr sfont={NULL,0,0,0};

	sfont.ta_Name=(STRPTR)font; sfont.ta_YSize=*size;
	if ((tf=OpenFont(&sfont)) && tf->tf_YSize==sfont.ta_YSize &&
		(!(tf->tf_Flags&FPF_PROPORTIONAL) || !noprop)) return(tf);
	if (tf) CloseFont(tf);
	main_proc->pr_WindowPtr=(APTR)-1;
	if (!(lock=Lock("FONTS:",ACCESS_READ))) {
		if (config->errorflags&ERROR_ENABLE_DOS) main_proc->pr_WindowPtr=(APTR)Window;
		strcpy(font,"topaz.font");
		*size=8;
		sfont.ta_YSize=8;
		tf=OpenFont(&sfont);
	}
	else {
		UnLock(lock);
		if (!DiskfontBase || !(tf=OpenDiskFont(&sfont))) {
			if ((tf=OpenFont(&sfont))) *size=tf->tf_YSize;
		}
		if (!tf || (tf->tf_Flags&FPF_PROPORTIONAL && noprop)) {
			if (tf) CloseFont(tf);
			strcpy(font,"topaz.font");
			*size=8;
			sfont.ta_YSize=8;
			tf=OpenFont(&sfont);
		}
	}
	if (config->errorflags&ERROR_ENABLE_DOS) main_proc->pr_WindowPtr=(APTR)Window;
	return(tf);
}

void allocstrings()
{
	int a;

	for (a=0;a<16;a++) rexx_args[a]=astring(256);
	str_arexx_command=astring(256);
	for (a=0;a<3;a++) str_select_pattern[a]=astring(80);
	for (a=0;a<2;a++) {
		rexx_pathbuffer[a]=astring(256);
		dopus_specialwin[a]=(struct DirectoryWindow *)astring(sizeof(struct DirectoryWindow));
	}
	menu_menuitem=(struct MenuItem *)astring(MENUCOUNT*sizeof(struct MenuItem));
	menu_itemnames=astring(MENUCOUNT*MAXMENULENGTH);
	menu_intuitext=(struct IntuiText *)astring(MENUCOUNT*2*sizeof(struct IntuiText));
	main_gadgets=(struct Gadget *)astring((GADCOUNT/2)*sizeof(struct Gadget));
	drive_gadgets=(struct Gadget *)astring(6*sizeof(struct Gadget));
	str_space_string=astring(MAXDISPLAYLENGTH);
	for (a=0;a<MAXDISPLAYLENGTH-1;a++) str_space_string[a]=' ';
	str_last_statustext=astring(512);
	remember_data=(struct RememberData *)astring(sizeof(struct RememberData));
}

char *astring(len)
int len;
{
	char *foo;

	if (!(foo=LAllocRemember(&general_key,len,MEMF_CLEAR))) quit();
	return(foo);
}

void allocdragbuffers()
{
	int a,w;

	freedragbuffers();
	drag_sprite.Depth=Window->WScreen->RastPort.BitMap->Depth;
	if (drag_sprite.Depth > 8) drag_sprite.Depth = 8; /* AROS FIX sg */
	
	a=35;
	w=(gettextlength(scr_font[FONT_DIRS],str_space_string,&a,0)+15);
	a=(scrdata_dispwin_width[0]>scrdata_dispwin_width[1])?scrdata_dispwin_width[0]:scrdata_dispwin_width[1];
	if (w>a) w=a;
	drag_sprite.Width=w/16;
	drag_sprite.Height=scr_font[FONT_DIRS]->tf_YSize;

	if (!(drag_bob_buffer=AllocRaster(drag_sprite.Width*16,drag_sprite.Height*drag_sprite.Depth)))
		return;

	if (!(drag_bob_savebuffer=AllocRaster(drag_sprite.Width*16,drag_sprite.Height*drag_sprite.Depth))) {
		freedragbuffers();
		return;
	}

	InitBitMap(&drag_bob_bitmap,drag_sprite.Depth,drag_sprite.Width*16,drag_sprite.Height);
	for (a=0;a<drag_sprite.Depth;a++) {
		drag_bob_bitmap.Planes[a]=
			drag_bob_buffer+(RASSIZE(drag_sprite.Width*16,drag_sprite.Height)*a);
	}
	for (;a<8;a++) drag_bob_bitmap.Planes[a]=NULL;

	InitRastPort(&drag_bob_rastport);
	drag_bob_rastport.BitMap=&drag_bob_bitmap;
	SetFont(&drag_bob_rastport,scr_font[FONT_DIRS]);
	SetDrMd(&drag_bob_rastport,JAM1);
	drag_sprite.ImageData=(WORD *)drag_bob_buffer;
	drag_sprite.PlanePick=(1<<drag_sprite.Depth)-1;
	drag_bob.SaveBuffer=(WORD *)drag_bob_savebuffer;

}

void freedragbuffers()
{
	if (drag_bob_buffer)
		FreeRaster(drag_bob_buffer,drag_sprite.Width*16,drag_sprite.Height*drag_sprite.Depth);
	if (drag_bob_savebuffer)
		FreeRaster(drag_bob_savebuffer,drag_sprite.Width*16,drag_sprite.Height*drag_sprite.Depth);
	drag_bob_buffer=drag_bob_savebuffer=NULL;
}

void load_palette(screen,palette)
struct Screen *screen;
ULONG *palette;
{
	int numcols;

	numcols=1<<screen->RastPort.BitMap->Depth;
	if (numcols>32) numcols=32;

	if (system_version2>=OSVER_39) {
		ULONG backup_palette[98];

		CopyMem((char *)palette,(char *)&backup_palette[1],numcols*3*sizeof(ULONG));
		backup_palette[0]=numcols<<16;
		backup_palette[(numcols*3)+1]=0;

		LoadRGB32(&screen->ViewPort,backup_palette);
	}
	else {
		UWORD backup_palette[32];
		int a,b;

		for (a=0,b=0;a<numcols*3;b++) {
			backup_palette[b]=0;
			backup_palette[b]|=(palette[a++]&0xf0000000)>>20;
			backup_palette[b]|=(palette[a++]&0xf0000000)>>24;
			backup_palette[b]|=(palette[a++]&0xf0000000)>>28;
		}

		LoadRGB4(&screen->ViewPort,backup_palette,numcols);
	}
}

void get_palette(screen,palette)
struct Screen *screen;
ULONG *palette;
{
	int numcols;

	numcols=1<<screen->RastPort.BitMap->Depth;
	if (numcols > 256) numcols = 256; /* AROS FIX sg */
	
	if (system_version2>=OSVER_39) {
		GetRGB32(screen->ViewPort.ColorMap,0,numcols,palette);
	}
	else {
		int a,b;
		ULONG colour;

		for (a=0,b=0;a<numcols;a++) {
			colour=GetRGB4(screen->ViewPort.ColorMap,a);
			palette[b++]=(((colour>>8)&0xf)<<28)|0x0fffffff;
			palette[b++]=(((colour>>4)&0xf)<<28)|0x0fffffff;
			palette[b++]=((colour&0xf)<<28)|0x0fffffff;
		}
	}
}

struct Library *open_dopus_library(name,ver)
char *name;
int ver;
{
	char buf[80];
	struct Library *lib;

	if ((lib=OpenLibrary(name,ver))) return(lib);
	FindSystemFile(name,buf,80,SYSFILE_LIBRARY);
	return(OpenLibrary(buf,ver));
}

void read_configuration(def)
int def;
{
	char buf[256];

	freedynamiccfg();
	get_config_file(buf,".CFG");
	if (def || !(readsetup(buf))) getdefaultconfig();

	read_data_files(1);
}

void read_data_files(fb)
int fb;
{
	char buf[256];

	if (get_data_file(buf,"HLP",fb))
		readhelp(buf);

	get_data_file(buf,"STR",fb);
	readstrings(buf);
}

int get_data_file(buf,suff,fb)
char *buf,*suff;
int fb;
{
	char temp[40];

	lsprintf(temp,"DO_%s.%s",config->language,suff);
	if (!config->language[0] || !(FindSystemFile(temp,buf,256,SYSFILE_DATA))) {
		if (!fb) {
			buf[0]=0;
			return(0);
		}
		lsprintf(temp,"DirectoryOpus.%s",suff);
		FindSystemFile(temp,buf,256,SYSFILE_DATA);
	}
	return(1);
}

void get_config_file(buf,suff)
char *buf,*suff;
{
	char temp[256];

	StrCombine(temp,str_config_basename,suff,256);
	if (CheckExist(temp,NULL)<0) {
		strcpy(buf,temp);
		return;
	}
	if (BaseName(str_config_basename))
		StrCombine(temp,BaseName(str_config_basename),suff,256);
	if (!(FindSystemFile(temp,buf,256,SYSFILE_DATA))) {
		StrCombine(temp,"DirectoryOpus",suff,256);
		if (!(FindSystemFile(temp,buf,256,SYSFILE_DATA))) {
			if (CheckExist("DOpus:S",NULL)) StrCombine(buf,"DOpus:S/DirectoryOpus",suff,256);
			else StrCombine(buf,"S:DirectoryOpus",suff,256);
		}
	}
}

void setup_draw_info()
{
	int a,b;
	struct DrawInfo *drinfo;
	struct Screen *wbscreen;

	if (system_version2) {
		mainscreen_tags[SCREENTAGS_DISPLAYID].ti_Data=0;
		if ((wbscreen=LockPubScreen("Workbench"))) {
			drinfo=GetScreenDrawInfo(wbscreen);
			b=drinfo->dri_NumPens;
			if (b>NUMDRIPENS) b=NUMDRIPENS;
			for (a=0;a<b;a++) scr_drawinfo[a]=drinfo->dri_Pens[a];
			FreeScreenDrawInfo(wbscreen,drinfo);
			UnlockPubScreen(NULL,wbscreen);
		}
	}
	else for (a=0;a<NUMDRIPENS;a++) scr_drawinfo[a]=(USHORT)~0;

	scr_drawinfo[SHINEPEN]=config->gadgettopcol;
	scr_drawinfo[SHADOWPEN]=config->gadgetbotcol;
	scr_drawinfo[NUMDRIPENS]=(USHORT)~0;
}

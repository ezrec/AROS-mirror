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

#include <libraries/asl.h>
#include <dos/dosextens.h>

#include "dopuslib.h"
#include "filereq.h"

static struct PropInfo
	defaultprop={PROPBORDERLESS|FREEVERT,0,0,0,0xffff};
static struct Image
	defaultimage={0,0,8,22,1,NULL,0,1,NULL};

static struct IntuiText
	defaulttext[6]={
		{1,0,JAM1,0,0,NULL,(UBYTE *)"File",NULL},
		{1,0,JAM1,0,0,NULL,(UBYTE *)"Drawer",NULL},
		{1,0,JAM1,0,2,NULL,(UBYTE *)"Accept",NULL},
		{1,0,JAM1,0,2,NULL,(UBYTE *)"Cancel",NULL},
		{1,0,JAM1,0,2,NULL,(UBYTE *)"Parent",NULL},
		{1,0,JAM1,0,2,NULL,(UBYTE *)"Drives",NULL}};

static struct Gadget
	defaultgadgets[9]={
		{NULL,64,47,240,8,GADGHCOMP,RELVERIFY|TOGGLESELECT,STRGADGET,
			NULL,NULL,NULL,NULL,NULL,FILENAME,NULL},
		{NULL,64,34,240,8,GADGHCOMP,RELVERIFY|TOGGLESELECT,STRGADGET,
			NULL,NULL,NULL,NULL,NULL,DRAWERNAME,NULL},
		{NULL,8,59,132,12,GADGHCOMP,RELVERIFY,BOOLGADGET,NULL,NULL,
			NULL,NULL,NULL,OKAY,NULL},
		{NULL,176,59,132,12,GADGHCOMP,RELVERIFY,BOOLGADGET,NULL,NULL,
			NULL,NULL,NULL,CANCEL,NULL},
		{NULL,292,14,16,8,GADGHCOMP,GADGIMMEDIATE|RELVERIFY,BOOLGADGET,
			NULL,NULL,NULL,NULL,NULL,FILENAMEUP,NULL},
		{NULL,292,22,16,8,GADGHCOMP,GADGIMMEDIATE|RELVERIFY,BOOLGADGET,
			NULL,NULL,NULL,NULL,NULL,FILENAMEDOWN,NULL},
		{NULL,8,14,132,12,GADGHCOMP,RELVERIFY,BOOLGADGET,NULL,NULL,
			NULL,NULL,NULL,PARENT,NULL},
		{NULL,176,14,132,12,GADGHCOMP,RELVERIFY,BOOLGADGET,NULL,NULL,
			NULL,NULL,NULL,DRIVES,NULL},
		{NULL,296,30,8,22,GADGHNONE,GADGIMMEDIATE|FOLLOWMOUSE|RELVERIFY,PROPGADGET,
			NULL,NULL,NULL,NULL,NULL,FILENAMEPOS,NULL}};

static struct NewWindow defaultwindow={
	15,25,316,115,0,1,IDCMP_GADGETUP|IDCMP_GADGETDOWN|IDCMP_MOUSEMOVE|IDCMP_MOUSEBUTTONS,
	WFLG_SMART_REFRESH|WFLG_NOCAREREFRESH|WFLG_ACTIVATE|WFLG_RMBTRAP|WFLG_DRAGBAR,
	NULL,NULL,NULL,NULL,NULL,0,0,0,0,CUSTOMSCREEN};

extern char nullstring[];

/*****************************************************************************

    NAME */

	AROS_LH1(int, FileRequest,

/*  SYNOPSIS */
	AROS_LHA(struct DOpusFileReq *, freq, A0),

/*  LOCATION */
	struct Library *, DOpusBase, 5, DOpus)
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
	struct Library *AslBase;

	if (freq->dirbuf[0] && (CheckExist(freq->dirbuf,NULL)<0)) {
		char *ptr;
		if ((ptr=BaseName(freq->dirbuf))) {
			if (freq->filebuf) {
				int a;

				LStrnCpy(freq->filebuf,ptr,30);
				a=strlen(freq->filebuf);
				if (freq->filebuf[a-1]=='/') freq->filebuf[a-1]=0;
			}
			*ptr=0;
		}
	}

	if ((AslBase=OpenLibrary("asl.library",0))) {
		struct TagItem asltags[10];
		struct FileRequester *fr;
		int font,res,a;
		char initialfont[40],*ptr;
		APTR request;

		for (a=0;a<10;a++) {
			asltags[a].ti_Tag=TAG_SKIP;
			asltags[a].ti_Data=0;
		}
		asltags[9].ti_Tag=TAG_END;

		if (freq->flags&DFRF_FONT) font=1;
		else font=0;

		if (freq->title) {
			asltags[0].ti_Tag=ASLFR_TitleText;
			asltags[0].ti_Data=(ULONG)freq->title;
		}

		asltags[1].ti_Tag=ASLFR_Window;
		asltags[1].ti_Data=(ULONG)((freq->window)?freq->window:IntuitionBase->ActiveWindow);

		if (freq->x>0) {
			asltags[2].ti_Tag=ASLFR_InitialLeftEdge;
			asltags[2].ti_Data=freq->x;
		}

		if (freq->y>0) {
			asltags[3].ti_Tag=ASLFR_InitialTopEdge;
			asltags[3].ti_Data=freq->y;
		}

		asltags[4].ti_Tag=ASLFR_InitialHeight;
		asltags[4].ti_Data=freq->lines*14;

		if (font) {
			if ((ptr=BaseName(freq->dirbuf))) {
				strcpy(initialfont,ptr);
				a=strlen(initialfont);
				if (initialfont[a-1]=='/') initialfont[a-1]=0;
			}
			else initialfont[0]=0;
			asltags[5].ti_Tag=ASLFO_InitialName;
			asltags[5].ti_Data=(ULONG)initialfont;
			asltags[6].ti_Tag=ASLFO_InitialSize;
			asltags[6].ti_Data=atoi(freq->filebuf);
		}
		else {
			if (freq->filebuf) {
				asltags[5].ti_Tag=ASL_File;
				asltags[5].ti_Data=(ULONG)freq->filebuf;
			}
			if (freq->dirbuf) {
				asltags[6].ti_Tag=ASL_Dir;
				asltags[6].ti_Data=(ULONG)freq->dirbuf;
			}
		}

		asltags[7].ti_Tag=ASL_FuncFlags;

		if (font) asltags[7].ti_Data=FONF_NEWIDCMP;
		else {
			if (freq->flags&DFRF_SAVE) asltags[7].ti_Data=FILF_SAVE|FILF_NEWIDCMP;
			else if (freq->flags&DFRF_MULTI) asltags[7].ti_Data=FILF_MULTISELECT|FILF_NEWIDCMP;
			else asltags[7].ti_Data=FILF_NEWIDCMP;

			if (freq->flags&DFRF_DIRREQ) {
				asltags[8].ti_Tag=ASL_ExtFlags1;
				asltags[8].ti_Data|=FIL1F_NOFILES;
			}
		}

		if ((request=AllocAslRequest((font)?ASL_FontRequest:ASL_FileRequest,asltags))) {
			if ((res=AslRequest(request,NULL))) {
				if (font) {
					if (freq->dirbuf) {
						StrCombine(freq->dirbuf,"FONTS:",((struct FontRequester *)request)->fo_Attr.ta_Name,256);
						strcat(freq->dirbuf,"/");
					}
					if (freq->filebuf) {
						LSprintf(freq->filebuf,"%ld",((struct FontRequester *)request)->fo_Attr.ta_YSize);
					}
				}
				else {
					fr=(struct FileRequester *)request;
					if (freq->dirbuf) strcpy(freq->dirbuf,fr->fr_Drawer);
					if (!(freq->flags&DFRF_DIRREQ)) {
						if (freq->filebuf) strcpy(freq->filebuf,fr->fr_File);
					}
					if (freq->flags&DFRF_MULTI && fr->fr_NumArgs>0) {
						if ((freq->filearray=
							LAllocRemember(&freq->filearraykey,(fr->fr_NumArgs+1)*4,MEMF_CLEAR))) {
							for (a=0;a<fr->fr_NumArgs;a++) {
								if ((freq->filearray[a]=LAllocRemember(&freq->filearraykey,
									strlen(fr->fr_ArgList[a].wa_Name)+1,MEMF_CLEAR))) {
									strcpy(freq->filearray[a],fr->fr_ArgList[a].wa_Name);
								}
							}
						}
					}
				}
			}
			FreeAslRequest(request);
		}
		CloseLibrary(AslBase);
		return(res);
	}
	else {
		ULONG class;
		USHORT code,gadgetid,qual;
		struct FileReqData *freqdata;
		unsigned char *dirbuf;
		int i,ret=1;

		if (!(freqdata=AllocMem(sizeof(struct FileReqData),MEMF_CLEAR)))
			return(ERROR_NOMEM);
		freqdata->freq=freq;
		if (!(initrequester(freqdata))) {
			close_req(freqdata);
			return(ERROR_NOMEM);
		}
		dirbuf=freq->dirbuf;

		if ((i=get_filenames(freqdata))==CANCEL) {
			close_req(freqdata);
			freemulti(freq);
			return(ERROR_CANCEL);
		}
		if (i!=OKAY) {
			FOREVER {
				Wait(1<<freqdata->fr_Window->UserPort->mp_SigBit);
				while (getintuimsg(freqdata->fr_Window,&class,&code,&qual,&gadgetid)) {
					if ((i=do_idcmp(freqdata,class,code,qual,gadgetid))==CANCEL) {
						close_req(freqdata);
						freemulti(freq);
						return(ERROR_CANCEL);
					}
					if (i==OKAY) {
						close_req(freqdata);
						goto acceptreq;
					}
				}
			}
		}
		else close_req(freqdata);
acceptreq:
		checkdrawer(dirbuf);
		if (freq->flags&DFRF_DIRREQ) {
			if (i==OKAY || dirbuf[0]) ret=1;
			else ret=0;
		}
		return(ret);
	}
    AROS_LIBFUNC_EXIT
}

void close_req(freqdata)
struct FileReqData *freqdata;
{
	struct direntry *file;
	struct DOpusFileReq *freq;
	char *buf;
	int b;

	if (freqdata) {
		freq=freqdata->freq;
		if (freqdata->fr_Window) {
			freqdata->freq->x=freqdata->fr_Window->LeftEdge;
			freqdata->freq->y=freqdata->fr_Window->TopEdge;
			CloseWindow(freqdata->fr_Window);
		}
		if (freqdata->flags&DFRF_MULTI && (file=freqdata->firstfile)) {
			b=1;
			while (file) {
				if (file->select) ++b;
				file=file->next;
			}
			if ((freq->filearray=(char **)LAllocRemember(&freq->filearraykey,sizeof(char *)*b,MEMF_CLEAR))) {
				if ((buf=(char *)LAllocRemember(&freq->filearraykey,b*32,MEMF_CLEAR))) {
					file=freqdata->firstfile; b=0;
					while (file) {
						if (file->select) {
							freq->filearray[b]=&buf[b*32];
							strcpy(freq->filearray[b],file->name);
							++b;
						}
						file=file->next;
					}
					freq->filearray[b]=&buf[b*32];
					freq->filearray[b][0]=0;
				}
				else freemulti(freq);
			}
		}
		deallocate_entries(freqdata);
		if (freqdata->filetype)
			FreeMem(freqdata->filetype,sizeof(int)*freqdata->freq->lines);
		if (freqdata->finfo) FreeMem(freqdata->finfo,sizeof(struct FileInfoBlock));
		FreeMem(freqdata,sizeof(struct FileReqData));
	}
}

int displayfiles(freqdata)
struct FileReqData *freqdata;
{
	struct direntry *work;
	struct RastPort *rp;
	int a,b,y,nlim,slim;

	if (freqdata->fileoffset==freqdata->oldfileoffset) return(0);
	freqdata->oldfileoffset=freqdata->fileoffset;
	if ((work=freqdata->firstfile)) {
		for (a=0;a<freqdata->fileoffset;a++)
			if (!(work=work->next)) break;
	}
	rp=freqdata->fr_Window->RPort;

	y=freqdata->fy+freqdata->fb;
	nlim=freqdata->width-7; slim=7;
	if (nlim<8) {
		nlim=8;
		slim=freqdata->width-8;
	}
	if (nlim>freqdata->width) {
		nlim=freqdata->width;
		slim=0;
	}
	SetDrMd(rp,JAM2);
	for (b=0;b<freqdata->freq->lines;b++) {
		Move(rp,10,y);
		if (work) {
			freqdata->filetype[b]=work->type;
			if (work->type<0) {
				if (work->select) {
					SetAPen(rp,2);
					SetBPen(rp,3);
				}
				else SetAPen(rp,1);
			}
			else if (work->type>0) SetAPen(rp,3);
			else SetAPen(rp,2);
			a=strlen(work->name); if (a>nlim) a=nlim;
			Text(rp,work->name,a);
			if (a<nlim) Text(rp,nullstring,nlim-a);
			if (slim) Text(rp,work->size,slim);
			if (work->select) SetBPen(rp,0);
			work=work->next;
		}
		else Text(rp,nullstring,freqdata->width);
		y+=freqdata->fh;
	}
	
	return 0;
}

void deallocate_entries(freqdata)
struct FileReqData *freqdata;
{
	LFreeRemember(&freqdata->filekey);
	freqdata->firstfile=freqdata->firstdir=NULL;
	freqdata->fileentries=freqdata->fileoffset=0; freqdata->oldfileoffset=-1;
}

void doposprop(freqdata)
struct FileReqData *freqdata;
{
	freqdata->fileoffset=GetSliderPos(&freqdata->reqgads[8],freqdata->fileentries,freqdata->freq->lines);
	displayfiles(freqdata);
}

int get_filenames(freqdata)
struct FileReqData *freqdata;
{
	BPTR lock;
	ULONG class;
	USHORT code,qual,gadgetid;
	int ret=0;
	char buf[33];

	freqdata->firstfile=freqdata->firstdir=NULL;

	if (!(lock=Lock(freqdata->freq->dirbuf,ACCESS_READ))) {
		freqdata->oldfileoffset=-1; freqdata->fileentries=0;
		fixprop(freqdata);
		displayfiles(freqdata);
		return(0);
	}

	Examine(lock,freqdata->finfo);
	if (freqdata->finfo->fib_DirEntryType<0) {
		UnLock(lock);
		freqdata->oldfileoffset=-1;
		displayfiles(freqdata);
		return(0);
	}

	while (ExNext(lock,freqdata->finfo)) {
		while (getintuimsg(freqdata->fr_Window,&class,&code,&qual,&gadgetid)) {
			ret=gettingdirmsg(freqdata,class,code,qual,gadgetid);
			if (ret==OKAY || ret==CANCEL || ret==INTERRUPT) {
				if (lock) UnLock(lock);
				return(ret);
			}
		}
		if (freqdata->finfo->fib_DirEntryType<0) {
			if (freqdata->flags&DFRF_FONT) {
				LSprintf(buf,"%ld",atoi(freqdata->finfo->fib_FileName));
				if (strcmp(buf,freqdata->finfo->fib_FileName)) continue;
			}
			else if (freqdata->flags&DFRF_DIRREQ) continue;
		}
		addfileentry(freqdata,freqdata->finfo->fib_FileName,
			freqdata->finfo->fib_DirEntryType,freqdata->finfo->fib_Size);
		freqdata->oldfileoffset=-1;
		fixprop(freqdata);
		doposprop(freqdata);
	}
	if (lock) UnLock(lock);
	freqdata->oldfileoffset=-1;
	displayfiles(freqdata);
	return(ret);
}

int getnewdrawer(freqdata)
struct FileReqData *freqdata;
{
	checkdrawer(freqdata->freq->dirbuf);
	refreshdrawergad(freqdata);
	activatefilegad(freqdata);
	clearfiles(freqdata);
	return(get_filenames(freqdata));
}

void clearfiles(freqdata)
struct FileReqData *freqdata;
{
	deallocate_entries(freqdata);
	doprops(freqdata);
	fixprop(freqdata);
	displayfiles(freqdata);
}

int getnew_file(freqdata,work)
struct FileReqData *freqdata;
struct direntry *work;
{
	int i,b;

	if (work->type>0) {
		TackOn(freqdata->freq->dirbuf,work->name,256);
		i=getnewdrawer(freqdata);
		if (i!=OKAY && i!=CANCEL && i!=INTERRUPT) i=INTERRUPT;
		return(i);
	}
	if (work->type==0) {
		strcpy(freqdata->freq->dirbuf,work->name);
		i=getnewdrawer(freqdata);
		if (i!=OKAY && i!=CANCEL && i!=INTERRUPT) i=INTERRUPT;
		return(i);
	}
	if ((strcmp(freqdata->freq->filebuf,work->name))==0) b=1;
	else b=0;
	strcpy(freqdata->freq->filebuf,work->name);
	RefreshGList(&freqdata->reqgads[0],freqdata->fr_Window,NULL,1);
	return(b);
}

int do_idcmp(freqdata,class,code,qual,gadgetid)
struct FileReqData *freqdata;
ULONG class;
USHORT code,qual,gadgetid;
{
	int ret=0,x,y,a,b,ty;
	struct RastPort *rp;
	struct direntry *file;
	LONG sec,mic;

	switch (class) {
		case IDCMP_GADGETDOWN:
			switch (gadgetid) {
				case FILENAMEPOS:
					doposprop(freqdata);
					FOREVER {
						Wait(1<<freqdata->fr_Window->UserPort->mp_SigBit);
						while (getintuimsg(freqdata->fr_Window,&class,&code,&qual,&gadgetid)) {
							if (class==GADGETUP) break;
							if (class==MOUSEMOVE) doposprop(freqdata);
						}
						if (class==GADGETUP) break;
					}
					break;
				case FILENAMEUP:
					repeatscroll(freqdata,-1,0);
					break;
				case FILENAMEDOWN:
					repeatscroll(freqdata,1,0);
					break;
			}
			activatefilegad(freqdata);
			return(0);
		case IDCMP_GADGETUP:
			switch (gadgetid) {
				case FILENAMEPOS:
					doposprop(freqdata);
					break;
				case CANCEL:
					ret=CANCEL;
					break;
				case FILENAME:
					if (qual&IEQUALIFIER_LSHIFT || qual&IEQUALIFIER_RSHIFT) {
						activatestrgad(freqdata->fr_Window,&freqdata->reqgads[1]);
						return(-1);
					}
					ret=OKAY;
					break;
				case OKAY:
					ret=OKAY;
					break;
				case DRAWERNAME:
					refreshdrawergad(freqdata);
					if (qual&IEQUALIFIER_LSHIFT || qual&IEQUALIFIER_RSHIFT) {
						activatefilegad(freqdata);
						return(-1);
					}
					if ((ret=getnewdrawer(freqdata))!=OKAY && ret!=CANCEL && ret!=INTERRUPT)
						ret=INTERRUPT;
					return(ret);
				case PARENT:
					if (doparent(freqdata)) {
						if ((ret=getnewdrawer(freqdata))!=OKAY && ret!=CANCEL && ret!=INTERRUPT)
							ret=INTERRUPT;
					}
					break;
				case DRIVES:
					activatefilegad(freqdata);
					if ((ret=listdevices(freqdata))!=OKAY && ret!=CANCEL && ret!=INTERRUPT)
						ret=INTERRUPT;
					break;
			}
			activatefilegad(freqdata);
			return(ret);
			break;
		case IDCMP_MOUSEBUTTONS:
			x=freqdata->fr_Window->MouseX; y=freqdata->fr_Window->MouseY;
			if (code==SELECTDOWN &&
				x>9 && x<freqdata->ww+10 && y>=freqdata->fy && y<freqdata->fy+(freqdata->freq->lines*freqdata->fh)) {
				a=(y-freqdata->fy)/freqdata->fh;
				if ((file=getfileentry(freqdata,a))) {
					b=1; ty=freqdata->fy+(a*freqdata->fh);
					rp=freqdata->fr_Window->RPort;
					SetDrMd(rp,COMPLEMENT);
					RectFill(rp,10,ty,freqdata->ww+9,ty+(freqdata->fh-1));
					if (freqdata->flags&DFRF_MULTI && file->type<0) {
						file->select=1-file->select;
						b=1;
					}
					else {
						freqdata->fr_Window->Flags|=WFLG_REPORTMOUSE;
						FOREVER {
							Wait(1<<freqdata->fr_Window->UserPort->mp_SigBit);
							while (getintuimsg(freqdata->fr_Window,&class,&code,&qual,&gadgetid)) {
								if (class==MOUSEMOVE) {
									x=freqdata->fr_Window->MouseX; y=freqdata->fr_Window->MouseY;
									if (x<10 || x>freqdata->ww+9 || y<ty || y>=ty+freqdata->fh) {
										if (b) {
											b=0;
											RectFill(rp,10,ty,freqdata->ww+9,ty+(freqdata->fh-1));
										}
									}
									else if (!b) {
										b=1;
										RectFill(rp,10,ty,freqdata->ww+9,ty+(freqdata->fh-1));
									}
								}
								else if (class==MOUSEBUTTONS && code==SELECTUP) break;
							}
							if (class==MOUSEBUTTONS && code==SELECTUP) break;
						}
						freqdata->fr_Window->Flags&=~WFLG_REPORTMOUSE;
					}
					if (b) {
						if (!(freqdata->flags&DFRF_MULTI))
							RectFill(rp,10,ty,freqdata->ww+9,ty+(freqdata->fh-1));
						SetDrMd(rp,JAM2);
						CurrentTime((ULONG *)&sec,(ULONG *)&mic);
						if ((ret=getnew_file(freqdata,file))==1 &&
							DoubleClick(freqdata->prevsec,freqdata->prevmic,sec,mic)) ret=OKAY;
						else if (ret!=OKAY && ret!=CANCEL && ret!=INTERRUPT) {
							freqdata->prevsec=sec; freqdata->prevmic=mic;
							ret=0;
						}
					}
					else SetDrMd(rp,JAM2);
				}
			}
			else if (code==MENUDOWN &&
				x>9 && x<freqdata->ww+10 && y>28 && y<37+(freqdata->freq->lines*freqdata->fh)) {
				a=((freqdata->freq->lines*freqdata->fh)/2)+30;
				if (y<(a-freqdata->fh)) b=-1;
				else if (y>(a+freqdata->fh)) b=1;
				else b=0;
				repeatscroll(freqdata,b,1);
			}
			activatefilegad(freqdata);
			return(ret);
			break;
		default:
			activatefilegad(freqdata);
			break;
	}
	return(0);
}

/* AROS This function has been completely rewritten, since strudt DosInfo is private */
#if 0
int listdevices(freqdata)
struct FileReqData *freqdata;
{
	char devname[32];
	
	struct DeviceList *devlist;
	struct RootNode *rootnode;
	struct DosInfo *dosinfo;
	ULONG class;
	USHORT code,qual,gadgetid;
	int ret;

	clearfiles(freqdata);
	rootnode=(struct RootNode *) DOSBase->dl_Root;
	dosinfo=(struct DosInfo *) BADDR(rootnode->rn_Info);
	devlist=(struct DeviceList *) BADDR(dosinfo->di_DevInfo);

	while (devlist) {
		while (getintuimsg(freqdata->fr_Window,&class,&code,&qual,&gadgetid)) {
			ret=gettingdirmsg(freqdata,class,code,qual,gadgetid);
			if (ret==OKAY || ret==CANCEL || ret==INTERRUPT) return(ret);
		}

		if (devlist->dl_Type!=DLT_DEVICE || devlist->dl_Task  ) {
			/* AROS: We don't mess with BSTRs anymore */
			strncpy(devname, devlist->dl_DevName, 31);
/*			BtoCStr(devlist->dl_Name,devname,32); */
			LStrCat(devname,":");
			addfileentry(freqdata,devname,0,devlist->dl_Type);
		}
		devlist=(struct DeviceList *) BADDR(devlist->dl_Next);
	}
	freqdata->oldfileoffset=-1;
	fixprop(freqdata);
	doposprop(freqdata);
	return(-1);
}
#else
int listdevices(freqdata)
struct FileReqData *freqdata;
{
	char devname[32];
	
	struct DosList *dlist;
	ULONG class;
	USHORT code,qual,gadgetid;
	int ret;
	
kprintf("listdevices()\n");	
	

	clearfiles(freqdata);
	
	dlist = LockDosList(LDF_READ | LDF_ALL);
	
	while ((dlist = NextDosEntry(dlist, LDF_ALL)) != NULL) {
		
		while (getintuimsg(freqdata->fr_Window,&class,&code,&qual,&gadgetid)) {
			ret=gettingdirmsg(freqdata,class,code,qual,gadgetid);
			if (ret==OKAY || ret==CANCEL || ret==INTERRUPT) return(ret);
		}

#warning AROS has no dol_Task entry
		if (dlist->dol_Type!=DLT_DEVICE /* AROS: || dlist->dol_Task */ ) {
			/* AROS: We don't mess with BSTRs anymore */
			strncpy(devname, dlist->dol_DevName, 31);
/*			BtoCStr(devlist->dl_Name,devname,32); */
			LStrCat(devname,":");
			addfileentry(freqdata,devname,0,dlist->dol_Type);
		}
	}
	UnLockDosList(LDF_READ | LDF_ALL);
	
	freqdata->oldfileoffset=-1;
	fixprop(freqdata);
	doposprop(freqdata);
	
kprintf("return from listdevices()\n");	
	
	return(-1);
}

#endif


char *devtypes[5]={"  <DEV>","  <ASN>","  <VOL>","  <DFR>","  <NBD>"};

int addfileentry(freqdata,name,type,size)
struct FileReqData *freqdata;
char *name;
int type,size;
{
	struct direntry *add,*afterdir=NULL,*work,*new;
	int adir=0;

	if (freqdata->firstfile) {
		if (type<0) {
			if (freqdata->firstdir) add=freqdata->firstdir;
			else {
				add=freqdata->firstfile;
				while (add && add->type>0) add=add->next;
			}
		}
		else add=freqdata->firstfile;
		while (add) {
			if (LStrCmpI(name,add->name)<=0 ||
				(type>0 && add==freqdata->firstdir)) {
				if (add->last) afterdir=add->last;
				else adir=1;
				break;
			}
			add=add->next;
		}
	}

	if (!(new=(struct direntry *)
		LAllocRemember(&freqdata->filekey,sizeof(struct direntry),MEMF_CLEAR)))
		return(0);

	if (afterdir) {
		work=afterdir;
		add=afterdir->next;
		work->next=new;
		new->last=work;
		work=new;
		work->next=add;
		add->last=work;
	}
	else if (adir==1) {
		add=freqdata->firstfile;
		work=new;
		work->next=add;
		add->last=work;
		freqdata->firstfile=work;
	}
	else {
		if (freqdata->firstfile) {
			work=freqdata->firstfile;
			while (work->next) work=work->next;
			work->next=new;
			new->last=work;
			work=new;
		}
		else {
			work=new;
			freqdata->firstfile=work;
		}
	}
	strcpy(work->name,name);
	if (type==0) strcpy(work->size,devtypes[size]);
	else if (type<0) {
		LSprintf(work->size,"%7ld",size);
		if ((work->last && work->last->type>0) || (!work->last && type<0))
			freqdata->firstdir=work;
	}
	else strcpy(work->size,"    Dir");
	work->type=type;
	++freqdata->fileentries;
	
	return 0;
}

int repeatscroll(freqdata,dir,menu)
struct FileReqData *freqdata;
int dir,menu;
{
	ULONG class;
	USHORT code,gadgetid;
	int x,y,m;

	if ((dir==-1 && freqdata->fileoffset==0) ||
		(dir==1 && freqdata->fileoffset>=freqdata->fileentries-freqdata->freq->lines))
		return(0);
	freqdata->fileoffset+=dir;
	displayfiles(freqdata);
	doprops(freqdata);
	if (menu) m=((freqdata->freq->lines*freqdata->fh)/2)+30;
	Delay(5);
	FOREVER {
		if (getintuimsg(freqdata->fr_Window,&class,&code,NULL,&gadgetid) &&
			(((class==GADGETUP) || ((class==MOUSEBUTTONS) && (code==SELECTUP))) ||
			(menu && (class==MOUSEBUTTONS) && (code==MENUUP)))) break;
		if (menu) {
			x=freqdata->fr_Window->MouseX; y=freqdata->fr_Window->MouseY;
			if (x>9 && x<freqdata->ww+10) {
				if (y<(m-freqdata->fh)) dir=-1;
				else if (y>(m+freqdata->fh)) dir=1;
				else dir=0;
			}
		}
		if ((dir==-1 && freqdata->fileoffset==0) ||
			(dir==1 && freqdata->fileoffset>=freqdata->fileentries-freqdata->freq->lines))
			break;
		freqdata->fileoffset+=dir;
		displayfiles(freqdata);
		doprops(freqdata);
	}
	
	return 0;
}

void doprops(freqdata)
struct FileReqData *freqdata;
{
	FixSliderPot(freqdata->fr_Window,&freqdata->reqgads[8],freqdata->fileoffset,freqdata->fileentries,freqdata->freq->lines,1);
}

void fixprop(freqdata)
struct FileReqData *freqdata;
{
	FixSliderBody(freqdata->fr_Window,&freqdata->reqgads[8],freqdata->fileentries,freqdata->freq->lines,1);
}

int doparent(freqdata)
struct FileReqData *freqdata;
{
	int i,b;

	if ((i=strlen(freqdata->freq->dirbuf))<2) return(0);
	--i;
	if (freqdata->freq->dirbuf[i]==':') return(0);
	if (freqdata->freq->dirbuf[i]=='/') --i;
	for (b=i;b>-1;b--) {
		if (freqdata->freq->dirbuf[b]=='/') break;
		if (freqdata->freq->dirbuf[b]==':') { ++b; break; }
	}
	if (b<0) b=0;
	freqdata->freq->dirbuf[b]=0;
	RefreshGList(&freqdata->reqgads[1],freqdata->fr_Window,NULL,1);
	return(1);
}

void checkdrawer(buf)
char *buf;
{
	int i;

	if ((i=strlen(buf))>1) {
		--i;
		if (buf[i]!='/' && buf[i]!=':') LStrCat(buf,"/");
	}
}

void refreshdrawergad(freqdata)
struct FileReqData *freqdata;
{
	freqdata->drawernamesinfo.BufferPos=strlen(freqdata->freq->dirbuf);
	RefreshGList(&freqdata->reqgads[1],freqdata->fr_Window,NULL,1);
}

void activatestrgad(win,gad)
struct Window *win;
struct Gadget *gad;
{
	struct StringInfo *sinfo;

	sinfo=(struct StringInfo *)gad->SpecialInfo;
	sinfo->BufferPos=strlen((char *)sinfo->Buffer);
	RefreshGList(gad,win,NULL,1);
	ActivateGadget(gad,win,NULL);
}

int initrequester(freqdata)
struct FileReqData *freqdata;
{
	int a,b;
	struct DOpusFileReq *freq;
	struct RastPort *rp;
	struct Screen *screen;
	struct TextFont *font;

	freq=freqdata->freq;
	freq->filearraykey=NULL; freq->filearray=NULL;
	freqdata->flags=freq->flags;
	if (freqdata->flags&DFRF_DIRREQ) freqdata->flags&=~DFRF_MULTI;

	if (IntuitionBase->LibNode.lib_Version>35) freqdata->version2=1;
	screen=(freq->window)?freq->window->WScreen:IntuitionBase->ActiveScreen;

	freqdata->sfh=screen->RastPort.Font->tf_YSize;
	freqdata->sfw=screen->RastPort.Font->tf_XSize;
	if (freqdata->sfh<8) freqdata->sfh=8;
	if (freqdata->sfw<6) freqdata->sfw=6;

	if (screen->RastPort.Font->tf_Flags&FPF_PROPORTIONAL)
		font=GfxBase->DefaultFont;
	else font=screen->RastPort.Font;

	freqdata->fh=font->tf_YSize; freqdata->fw=font->tf_XSize;
	freqdata->fb=font->tf_Baseline;

	CopyMem((char *)&defaultprop,(char *)&freqdata->filenameprop,sizeof(struct PropInfo));
	freqdata->filenamesinfo.Buffer=(STRPTR)freq->filebuf;
	freqdata->filenamesinfo.MaxChars=31;
	freqdata->drawernamesinfo.Buffer=(STRPTR)freq->dirbuf;
	freqdata->drawernamesinfo.MaxChars=236;

	if (freq->lines<5) freq->lines=5;
	else if (freq->lines>((screen->Height-(freqdata->sfh*5)+35)/freqdata->fh))
		freq->lines=(screen->Height-(freqdata->sfh*5)+35)/freqdata->fh;

	if (!(freqdata->filetype=AllocMem(sizeof(int)*freq->lines,MEMF_CLEAR)))
		return(0);

	CopyMem((char *)&defaultgadgets,(char *)&freqdata->reqgads,sizeof(struct Gadget)*9);
	CopyMem((char *)&defaulttext,(char *)&freqdata->reqtext,sizeof(struct IntuiText)*6);
	CopyMem((char *)&defaultimage,(char *)&freqdata->filenameimage,sizeof(struct Image));
	freqdata->reqgads[0].SpecialInfo=(APTR)&freqdata->filenamesinfo;
	freqdata->reqgads[1].SpecialInfo=(APTR)&freqdata->drawernamesinfo;
	freqdata->reqgads[8].GadgetRender=(APTR)&freqdata->filenameimage;
	freqdata->reqgads[8].SpecialInfo=(APTR)&freqdata->filenameprop;
	for (a=0;a<8;a++) freqdata->reqgads[a].NextGadget=&freqdata->reqgads[a+1];
	for (a=0;a<4;a++) freqdata->reqgads[a].GadgetText=&freqdata->reqtext[a];
	for (a=6;a<8;a++) freqdata->reqgads[a].GadgetText=&freqdata->reqtext[a-2];
	CopyMem((char *)&defaultwindow,(char *)&freqdata->reqwin,sizeof(struct NewWindow));
	freqdata->reqwin.Height=(freq->lines*freqdata->fh)+(freqdata->sfh*5)+31+((freqdata->flags&DFRF_DIRREQ)?0:4);
	if (freqdata->reqwin.Height>screen->Height) {
		freq->lines=(screen->Height-((freqdata->sfh*5)+35))/freqdata->fh;
		freqdata->reqwin.Height=(freq->lines*freqdata->fh)+(freqdata->sfh*5)+35;
	}
	if (freqdata->flags&DFRF_DIRREQ) freqdata->reqwin.Height-=freqdata->sfh+2;
	freqdata->reqwin.Width=(freqdata->fw*35)+36;
	if (freqdata->reqwin.Width>screen->Width) {
		a=(screen->Width-36)/freqdata->fw;
		freqdata->width=a;
		freqdata->reqwin.Width=(freqdata->fw*a)+36;
	}
	else freqdata->width=35;
	freqdata->ww=freqdata->width*freqdata->fw;

	freqdata->reqwin.Screen=screen;
	if (freq->x==-2) freqdata->reqwin.LeftEdge=(screen->Width-freqdata->reqwin.Width)/2;
	else if (freq->x<0) freqdata->reqwin.LeftEdge=15;
	else freqdata->reqwin.LeftEdge=freq->x;
	if (freq->y==-2) freqdata->reqwin.TopEdge=(screen->Height-freqdata->reqwin.Height)/2;
	else if (freq->y<0) freqdata->reqwin.TopEdge=25;
	else freqdata->reqwin.TopEdge=freq->y;
	if (freqdata->reqwin.LeftEdge+freqdata->reqwin.Width>screen->Width)
		freqdata->reqwin.LeftEdge=screen->Width-freqdata->reqwin.Width;
	if (freqdata->reqwin.TopEdge+freqdata->reqwin.Height>screen->Height)
		freqdata->reqwin.TopEdge=screen->Height-freqdata->reqwin.Height;
	if (freq->title) freqdata->reqwin.Title=(STRPTR)freq->title;
	else freqdata->reqwin.Title=(STRPTR)"File request";
	if (freqdata->flags&DFRF_SAVE && !freqdata->version2) freqdata->reqwin.BlockPen=2;

	if (!(freqdata->finfo=AllocMem(sizeof(struct FileInfoBlock),MEMF_CLEAR)))
		return(0);

	freqdata->oldfileoffset=-1;

	if (!(freqdata->fr_Window=OpenWindow(&freqdata->reqwin))) return(0);
	rp=freqdata->fr_Window->RPort;
	SetFont(rp,font);
	SetDrMd(rp,JAM2);
	freqdata->sfy=freqdata->fr_Window->BorderTop+2;
	freqdata->fy=freqdata->sfy+freqdata->sfh+7;

	a=freq->lines*freqdata->fh;
	freqdata->reqgads[1].TopEdge=freqdata->fy+a+5;
	if (!(freqdata->flags&DFRF_DIRREQ))
		freqdata->reqgads[0].TopEdge=freqdata->reqgads[1].TopEdge+freqdata->sfh+6;
	freqdata->reqgads[2].TopEdge=
		freqdata->reqgads[(freqdata->flags&DFRF_DIRREQ?1:0)].TopEdge+freqdata->sfh+4;
	freqdata->reqgads[3].TopEdge=freqdata->reqgads[2].TopEdge;
	freqdata->reqgads[4].TopEdge=freqdata->fy+a-15;
	freqdata->reqgads[5].TopEdge=freqdata->fy+a-7;
	freqdata->reqgads[6].TopEdge=freqdata->reqgads[7].TopEdge=freqdata->sfy;
	freqdata->reqgads[8].TopEdge=freqdata->fy+1;
	freqdata->reqgads[0].LeftEdge=freqdata->reqgads[1].LeftEdge=(freqdata->sfw*6)+16;
	freqdata->reqgads[3].LeftEdge=freqdata->reqgads[7].LeftEdge=freqdata->ww-104;
	freqdata->reqgads[4].LeftEdge=freqdata->reqgads[5].LeftEdge=freqdata->ww+12;
	freqdata->reqgads[8].LeftEdge=freqdata->ww+16;
	freqdata->reqgads[0].Width=freqdata->reqgads[1].Width=(freqdata->ww+24)-freqdata->reqgads[0].LeftEdge;
	if ((freqdata->sfw*6)>132) {
		freqdata->reqgads[2].Width=freqdata->reqgads[3].Width=
			freqdata->reqgads[6].Width=freqdata->reqgads[7].Width=
			freqdata->sfw*7;
		freqdata->reqgads[3].LeftEdge-=((freqdata->sfw*7)-132);
		freqdata->reqgads[7].LeftEdge=freqdata->reqgads[3].LeftEdge;
	}
	freqdata->reqgads[0].Height=freqdata->reqgads[1].Height=freqdata->sfh;
	freqdata->reqgads[2].Height=freqdata->reqgads[3].Height=
		freqdata->reqgads[6].Height=freqdata->reqgads[7].Height=freqdata->sfh+4;
	freqdata->reqgads[8].Height=a-18;

	for (b=2;b<8;b++)
		Do3DBox(rp,freqdata->reqgads[b].LeftEdge+2,freqdata->reqgads[b].TopEdge+1,
			freqdata->reqgads[b].Width-4,freqdata->reqgads[b].Height-2,2,1);
	if (!(freqdata->flags&DFRF_DIRREQ))
		Do3DStringBox(rp,freqdata->reqgads[0].LeftEdge,freqdata->reqgads[0].TopEdge,freqdata->reqgads[0].Width,freqdata->sfh,2,1);
	Do3DStringBox(rp,freqdata->reqgads[1].LeftEdge,freqdata->reqgads[1].TopEdge,freqdata->reqgads[1].Width,freqdata->sfh,2,1);
	for (b=4;b<6;b++) DoArrow(rp,freqdata->ww+14,freqdata->reqgads[b].TopEdge+1,12,6,1,0,b-4);
	Do3DBox(rp,10,freqdata->fy,freqdata->ww,a,2,1);
	Do3DBox(rp,freqdata->ww+14,freqdata->fy,12,a-16,2,1);

	freqdata->reqtext[0].LeftEdge=freqdata->reqtext[1].LeftEdge=-(freqdata->reqgads[0].LeftEdge-10);
	for (b=2;b<6;b++) freqdata->reqtext[b].LeftEdge=(freqdata->reqgads[2].Width-(freqdata->sfw*6))/2;

	AddGList(freqdata->fr_Window,&freqdata->reqgads[(freqdata->flags&DFRF_DIRREQ)?1:0],-1,-1,NULL);
	RefreshGList(&freqdata->reqgads[(freqdata->flags&DFRF_DIRREQ)?1:0],freqdata->fr_Window,NULL,-1);
	activatefilegad(freqdata);
	return(1);
}

int getintuimsg(win,class,code,qual,gadgetid)
struct Window *win;
ULONG *class;
USHORT *code,*qual,*gadgetid;
{
	struct IntuiMessage *msg;

	if ((msg=(struct IntuiMessage *)GetMsg(win->UserPort))) {
		*class=msg->Class; *code=msg->Code;
		if (qual) *qual=msg->Qualifier;
		if (msg->Class==GADGETDOWN || msg->Class==GADGETUP)
			*gadgetid=((struct Gadget *)msg->IAddress)->GadgetID;
		ReplyMsg((struct Message *)msg);
		return(1);
	}
	return(0);
}

int gettingdirmsg(freqdata,class,code,qual,gadgetid)
struct FileReqData *freqdata;
ULONG class;
USHORT code,qual,gadgetid;
{
	int ret;

	if ((ret=do_idcmp(freqdata,class,code,qual,gadgetid))!=-1) {
		freqdata->oldfileoffset=-1;
		return(ret);
	}
	return(0);
}

void activatefilegad(freqdata)
struct FileReqData *freqdata;
{
	activatestrgad(freqdata->fr_Window,
		&freqdata->reqgads[(freqdata->flags&DFRF_DIRREQ)?1:0]);
}

struct direntry *getfileentry(freqdata,which)
struct FileReqData *freqdata;
int which;
{
	int i,b;
	struct direntry *work;

	i=freqdata->fileoffset+which;
	if (!(work=freqdata->firstfile)) return(0);
	for (b=0;b<i;b++) if (!(work=work->next)) return(0);
	return(work);
}

void freemulti(freq)
struct DOpusFileReq *freq;
{
	if (freq->flags&DFRF_MULTI) LFreeRemember(&freq->filearraykey);
	freq->filearray=NULL;
}

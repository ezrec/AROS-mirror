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

enum {
	PRINT_ASPECT,
	PRINT_IMAGE,
	PRINT_SHADE,
	PRINT_PLACE,
	PRINT_FORMFD,
	PRINT_TITLE,
	PRINT_OKAY,
	PRINT_CANCEL,
	PRINT_ABORT};

static ULONG *colourtable_8=NULL,ctsize;
static UWORD *copperlist,*colourtable_4=NULL,colourlist[4];
static struct View *view;
static struct ViewPort *ivp;
static char *picbuffer,*picturename;
static unsigned char *bodyptr;
static CRange cyclerange[6];
static ULONG buffersize,bufferpos;
static int
	numcolours,currange,copperheight,viewmode,
	screenwidth,screenheight,
	imagewidth,imageheight,
	bitmapwidth,bitmapheight,
	minwidth,minheight,maxwidth,maxheight,depth,coppersize,
	viewflags;
static char specialformat;
static BitMapHeader bmhead;
static struct BitMap *iffbm[2];
static struct Window *iffwindow;
static struct DOpusRemember *iffkey;

#ifdef DO_DBUF
static struct DBufInfo *dbufinfo;
static struct MsgPort *dbufport;
#endif

char iffscreenname[34]="";

struct AnimFrame {
	struct AnimFrame *next;
	AnimHdr *animheader;
	unsigned char *delta;
	unsigned char *cmap;
	int cmapsize;
};

static struct TagItem
	loresscreentags[3]={
		{SA_DisplayID,LORES_KEY},
		{SA_Pens,(ULONG)scr_drawinfo},
		{0,0}};

static struct ExtNewScreen
	printscreen={
		0,0,320,0,2,
		0,1,0,
		CUSTOMSCREEN|SCREENQUIET|SCREENBEHIND|NS_EXTENDED,
		&screen_attr,NULL,NULL,NULL,loresscreentags};

static struct NewWindow
	iffwin={
		0,0,0,0,
		255,255,
		IDCMP_MOUSEBUTTONS|IDCMP_RAWKEY|IDCMP_INTUITICKS|IDCMP_INACTIVEWINDOW,
		WFLG_RMBTRAP|WFLG_BORDERLESS|WFLG_SIMPLE_REFRESH|WFLG_NOCAREREFRESH,
		NULL,NULL,NULL,NULL,NULL,
		0,0,0,0,CUSTOMSCREEN},
	printwin={
		0,0,320,0,0,0,
		IDCMP_GADGETUP|IDCMP_VANILLAKEY,
		WFLG_RMBTRAP|WFLG_BORDERLESS,
		NULL,NULL,NULL,NULL,NULL,
		0,0,0,0,CUSTOMSCREEN};

static struct Image
	printcheckimage={7,2,13,7,1,NULL,1,0,NULL},
	printnullcheckimage={7,2,13,7,1,NULL,0,0,NULL},
	animframeimage;

static struct Gadget
	printgadgets[8]={
		{&printgadgets[1],142,104,123,14,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
		NULL,NULL,NULL,GAD_CYCLE,NULL,PRINT_ASPECT,NULL},
		{&printgadgets[2],142,120,123,14,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
		NULL,NULL,NULL,GAD_CYCLE,NULL,PRINT_IMAGE,NULL},
		{&printgadgets[3],142,136,123,14,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
		NULL,NULL,NULL,GAD_CYCLE,NULL,PRINT_SHADE,NULL},
		{&printgadgets[4],142,152,123,14,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
		NULL,NULL,NULL,GAD_CYCLE,NULL,PRINT_PLACE,NULL},
		{&printgadgets[5],18,168,26,11,GFLG_GADGIMAGE|GFLG_GADGHIMAGE|GFLG_SELECTED,GACT_RELVERIFY|GACT_TOGGLESELECT,GTYP_BOOLGADGET,
		(APTR)&printnullcheckimage,(APTR)&printcheckimage,NULL,GAD_CHECK,NULL,PRINT_FORMFD,NULL},
		{&printgadgets[6],142,168,26,11,GFLG_GADGIMAGE|GFLG_GADGHIMAGE,GACT_RELVERIFY|GACT_TOGGLESELECT,GTYP_BOOLGADGET,
		(APTR)&printnullcheckimage,(APTR)&printcheckimage,NULL,GAD_CHECK,NULL,PRINT_TITLE,NULL},
		{&printgadgets[7],20,186,100,12,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
		NULL,NULL,NULL,0,NULL,PRINT_OKAY,NULL},
		{NULL,196,186,100,12,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
		NULL,NULL,NULL,0,NULL,PRINT_CANCEL,NULL}},
	abortprintgad={
		NULL,60,0,200,80,GFLG_GADGHCOMP,GACT_RELVERIFY,GTYP_BOOLGADGET,
		NULL,NULL,NULL,0,NULL,PRINT_ABORT,NULL};

char
	*printgadtxt[9],
	*printabortgadtxt[2]={NULL,NULL},
	*print_aspect_txt[2],
	*print_image_txt[2],
	*print_shade_txt[3],
	*print_placement_txt[2],

	**print_gads_txt[4]={
		print_aspect_txt,
		print_image_txt,
		print_shade_txt,
		print_placement_txt};

static char
	print_gads_sel[4]={0,0,0,0},
	print_gads_max[4]={1,1,2,1};

static struct AnimFrame
	*first_anim_frame,*current_anim_frame,*last_anim_frame,
	initial_anim_cmap;
static int framecount,framenum,framedisp;
static char doublebuffer;
static int animspeed,framespersec,animrestart;
static struct BitMap *animframe_bm[2],*initialframe_bm;
static USHORT basepalette[4]={0xaaa,0x000,0xfff,0x679};
static union printerIO *print_request; /* IO request for Print operation */
static int palette32,got_dpan;
static int animtype;
static struct PCHGHeader *pchghead;
static char *sham;

int LoadPic(name)
char *name;
{
#warning LoadPic() not implemented becasuse of missing DisplayInfo funcs

kprintf("LoadPic() not implemented becasuse of missing DisplayInfo funcs\n");
return IFFERR_NOMEMORY;
#if 0

	int
		retcode,a,gotbody,brush,scroll,mp=1,isanim,extflag=0,
		colourptr,coloursize,maxframes;
	ULONG chunkid,chunksize,chunkbuf[3];
	struct DimensionInfo *dimension;
	DisplayInfoHandle *handle;
	char dimbuf[256],*ptr;
	DPAnimChunk dpan;
	struct AnimFrame *cur_frame=NULL,*frame;

	picturename=name;
	if (readfile(name,&picbuffer,(int *)&buffersize)) return(0);

	viewmode=-1; copperlist=NULL; iffwindow=NULL;
	iffscreen=NULL; iffscreenname[0]=0;
	copperheight=currange=gotbody=brush=0; scroll=1;
	colourptr=-1; doublebuffer=0; animspeed=50000;
	bodyptr=NULL; bufferpos=isanim=0; iffkey=NULL; first_anim_frame=NULL;
	framecount=0; framespersec=20; maxframes=65535; specialformat=0;
	initialframe_bm=NULL; palette32=0;
	last_anim_frame=NULL; initial_anim_cmap.cmap=NULL;
	got_dpan=0; animtype=0;
	pchghead=NULL; sham=NULL;

	for (a=0;a<2;a++) {
		iffbm[a]=NULL;
		animframe_bm[a]=NULL;
	}

#ifdef DO_DBUF
	dbufinfo=NULL; dbufport=NULL;
#endif

	if (!chunkread(chunkbuf,sizeof(ULONG)*3) ||
		chunkbuf[0]!=ID_FORM ||
		(chunkbuf[2]!=ID_ANIM && chunkbuf[2]!=ID_ILBM)) {
		retcode=IFFERR_NOTILBM;
		goto endiff;
	}
	if (chunkbuf[2]==ID_ANIM) isanim=1;

	while (bufferpos<=buffersize) {
		if (!chunkread(&chunkid,sizeof(int)) ||
			!chunkread(&chunksize,sizeof(int)) ||
			((bufferpos+chunksize)>buffersize)) break;
		switch (chunkid) {
			case ID_FORM:
				bufferpos+=4;
				continue;
			case ID_DPAN:
				if (!isanim) break;
				if (!(chunkread(&dpan,sizeof(DPAnimChunk)))) {
					retcode=IFFERR_BADIFF;
					goto endiff;
				}
				chunksize=0;
				maxframes=dpan.nframes;
				if ((framespersec=dpan.framespersecond)>0)
					animspeed=1000000/dpan.framespersecond;
				got_dpan=1;
				break;
			case ID_ANHD:
				if (isanim && framecount<maxframes &&
					(frame=LAllocRemember(&iffkey,sizeof(struct AnimFrame),MEMF_CLEAR))) {
					frame->animheader=(AnimHdr *)(picbuffer+bufferpos);
					if (frame->animheader->interleave!=1) doublebuffer=1;
					if (!animtype) animtype=frame->animheader->operation;

					if (cur_frame) cur_frame->next=frame;
					else first_anim_frame=frame;
					cur_frame=frame;
				}
				break;
			case ID_DLTA:
				if (isanim && framecount<maxframes && cur_frame) {
					cur_frame->delta=picbuffer+bufferpos;
					++framecount;
				}
				break;
			case ID_BMHD:
				if (!chunkread(&bmhead,sizeof(BitMapHeader))) {
					retcode=IFFERR_BADIFF;
					goto endiff;
				}
				chunksize=0;
				break;
			case ID_CMAP:
				if (colourptr==-1) {
					colourptr=bufferpos; coloursize=chunksize;
					initial_anim_cmap.cmap=(unsigned char *)&picbuffer[bufferpos];
					initial_anim_cmap.cmapsize=chunksize;
				}
				else if (isanim && framecount<maxframes && cur_frame) {
					cur_frame->cmap=(unsigned char *)&picbuffer[bufferpos];
					cur_frame->cmapsize=chunksize;
				}
				break;
			case ID_GRAB:
				brush=1;
				break;
			case ID_CAMG:
				if (!chunkread(&viewmode,sizeof(int))) {
					retcode=IFFERR_BADIFF;
					goto endiff;
				}
				chunksize=0;
				break;
			case ID_CTBL:
				if ((copperlist=LAllocRemember(&iffkey,chunksize,MEMF_CLEAR))) {
					coppersize=chunksize;
					copperheight=chunksize/32;
					CopyMem((char *)&picbuffer[bufferpos],(char *)copperlist,chunksize);
					CopyMem((char *)&picbuffer[bufferpos],(char *)colourlist,8);
				}
				break;
			case ID_DYCP:
				mp=0;
				break;
			case ID_SHAM:
#warning We have no pchgr lib
kprintf("HAVE NO PCHG LIB/PCHG_SHAM2PCHG()\n");
				if (!( 0 /* pchghead=PCHG_SHAM2PCHG((UWORD *)&picbuffer[bufferpos],chunksize,(viewmode&LACE)?2:1)*/)) {
					retcode=IFFERR_NOMEM;
					goto endiff;
				}
				sham=&picbuffer[bufferpos];
				specialformat=1;
				break;
			case ID_PCHG:
				pchghead=(struct PCHGHeader *)&picbuffer[bufferpos];
				specialformat=1;
				break;
			case ID_CRNG:
				if (currange<6) {
					CopyMem((char *)&picbuffer[bufferpos],(char *)&cyclerange[currange],sizeof(CRange));
					if (cyclerange[currange].rate<=CRNG_NORATE ||
						cyclerange[currange].low==cyclerange[currange].high)
						cyclerange[currange].active=0;
					if (cyclerange[currange].rate>CRNG_NORATE)
						cyclerange[currange].rate=16384/cyclerange[currange].rate;
					else cyclerange[currange].rate=0;
					++currange;
				}
				break;
			case ID_BODY:
				if (!gotbody) {
					gotbody=1;
					bodyptr=picbuffer+bufferpos;
				}
				break;
		}
		if (chunksize) {
			bufferpos+=chunksize;
			if (chunksize&1) ++bufferpos;
		}
	}

	if (!gotbody) {
		retcode=IFFERR_BADIFF;
		goto endiff;
	}

	if (copperlist) specialformat=1;

	depth=bmhead.nPlanes;

	if (colourptr>-1) {
		numcolours=coloursize/3;

		if (system_version2>=OSVER_39 && !specialformat)
			colourtable_8=LAllocRemember(&iffkey,(coloursize+2)*4,MEMF_CLEAR);

		colourtable_4=LAllocRemember(&iffkey,numcolours*sizeof(UWORD),MEMF_CLEAR);
	}

	if (!colourtable_8 && !colourtable_4) {
		retcode=IFFERR_BADIFF;
		goto endiff;
	}

	if ((config->viewbits&VIEWBITS_8BITSPERGUN || bmhead.flags&BMHF_CMAPOK) &&
		colourtable_8) palette32=1;

	build_palettes(&picbuffer[colourptr],coloursize,colourtable_4,colourtable_8);

	imagewidth=bmhead.w;
	imageheight=bmhead.h;
	screenwidth=bmhead.w;
	screenheight=bmhead.h;

	minwidth=320;
	minheight=200;

	maxwidth=368;
	if (scrdata_is_pal) maxheight=281;
	else maxheight=241;

	if (viewmode==-1 || brush) {
		viewflags=viewmode; viewmode=0;
		if (bmhead.pageWidth>maxwidth) viewmode|=HIRES;
		if (bmhead.pageHeight>maxheight) viewmode|=LACE;
		if (system_version2<OSVER_39) {
			if (depth>6) {
				retcode=IFFERR_BADMODE;
				goto endiff;
			}
			if (depth==6) {
				if (viewflags==-1 || viewflags&HAM) viewmode|=HAM;
				else viewmode|=EXTRA_HALFBRITE;
			}
		}
		viewflags=0;
	}

	if (system_version2) {
		if (viewmode&0xfffe000) {
			if (!(ModeNotAvailable(viewmode))) extflag=1;
			else if ((handle=FindDisplayInfo(viewmode)) &&
				(GetDisplayInfoData(handle,dimbuf,256,DTAG_NAME,0)))
				extflag=1;
		}
	}

	if (extflag) {
		if (ModeNotAvailable(viewmode)) {
			if (system_version2<OSVER_39) {
				retcode=IFFERR_BADMODE;
				goto endiff;
			}
			else viewmode=-1;
		}

		if (system_version2>=OSVER_39 &&
			(viewmode==-1 || config->viewbits&VIEWBITS_BESTMODEID)) {

			ULONG newmode;

			if (!(ModeNotAvailable((newmode=
				BestModeID(
					BIDTAG_NominalWidth,imagewidth,
					BIDTAG_NominalHeight,imageheight,
					BIDTAG_Depth,depth,
					BIDTAG_SourceID,viewmode,
					BIDTAG_MonitorID,(GetVPModeID(&Window->WScreen->ViewPort)&MONITOR_ID_MASK),
					TAG_END))))) viewmode=newmode;
		}

		if ((handle=FindDisplayInfo(viewmode))) {
			if (!(a=GetDisplayInfoData(handle,dimbuf,256,DTAG_NAME,0))) {
				DisplayInfoHandle *temphandle;
				ULONG tempviewmode;

				tempviewmode=viewmode;
				tempviewmode&=~(HAM_KEY|EXTRAHALFBRITE_KEY);

				if ((temphandle=FindDisplayInfo(tempviewmode)))
					a=GetDisplayInfoData(temphandle,dimbuf,256,DTAG_NAME,0);
			}
			if (a && (((struct NameInfo *)dimbuf)->Name)) {
				strcpy(iffscreenname,((struct NameInfo *)dimbuf)->Name);
				if ((ptr=strchr(iffscreenname,':'))) strcpy(iffscreenname,ptr+1);
			}
			GetDisplayInfoData(handle,dimbuf,256,DTAG_DIMS,0);
			dimension=(struct DimensionInfo *)dimbuf;
			maxwidth=(dimension->StdOScan.MaxX-dimension->StdOScan.MinX)+1;
			maxheight=(dimension->StdOScan.MaxY-dimension->StdOScan.MinY)+1;
			if (depth>dimension->MaxDepth) {
				retcode=IFFERR_BADMODE;
				goto endiff;
			}
		}
		else extflag=0;
		viewflags=0;
		if ((viewmode&SUPER_KEY)==SUPER_KEY) viewflags|=SUPERHIRES|HIRES;
		else if (viewmode&HIRES_KEY) viewflags|=HIRES;
		if (viewmode&LORESLACE_KEY) viewflags|=LACE;
		if (viewmode&HAM_KEY) viewflags|=HAM;
		if (viewmode&EXTRAHALFBRITE_KEY) viewflags|=EXTRA_HALFBRITE;
		if (!extflag) viewmode=viewflags;
	}

	if (!extflag) {
		viewmode&=0x0000ffff;
		viewmode&=~(SPRITES|GENLOCK_AUDIO|GENLOCK_VIDEO|VP_HIDE|DUALPF|PFBA|EXTENDED_MODE);
		viewflags=viewmode;

		if (viewflags&HIRES) {
			if (viewflags&SUPERHIRES) maxwidth*=4;
			else maxwidth*=2;
			minwidth*=2;
		}
	}

	if (screenwidth<minwidth) screenwidth=minwidth;
	else if (screenwidth>maxwidth) screenwidth=maxwidth;

	if (!extflag && viewflags&LACE) {
		maxheight*=2;
		minheight*=2;
	}
	if (screenheight<minheight) screenheight=minheight;
	else if (screenheight>maxheight) screenheight=maxheight;

	if (imagewidth<screenwidth) bitmapwidth=screenwidth;
	else bitmapwidth=imagewidth;
	if (imageheight<screenheight) bitmapheight=screenheight;
	else bitmapheight=imageheight;

	if (bitmapwidth>screenwidth || bitmapheight>screenheight) scroll=3;
	dotitle();

	if (isanim && first_anim_frame) {
		if (!(animframe_bm[0]=getbitmap(bitmapwidth,bitmapheight,bmhead.nPlanes))) {
			retcode=IFFERR_NOMEM;
			goto endiff;
		}
		if (doublebuffer) {
			if ((animframe_bm[1]=getbitmap(bitmapwidth,bitmapheight,bmhead.nPlanes))) {
				initialframe_bm=getbitmap(bitmapwidth,bitmapheight,bmhead.nPlanes);
#ifdef DO_DBUF
				if (system_version2>=OSVER_39 &&
					(dbufinfo=AllocDBufInfo(ivp))) {
					if (dbufport=LCreatePort(NULL,0)) {
						dbufinfo->dbi_SafeMessage.mn_ReplyPort=dbufport;
						dbufinfo->dbi_SafeMessage.mn_Length=sizeof(struct Message);
					}
				}
#endif
			}
		}
	}

	if (doublebuffer) {
		for (a=0;a<2;a++) {
			if (animframe_bm[a]) {
				if (!(iffbm[a]=LAllocRemember(&iffkey,sizeof(struct BitMap),MEMF_CLEAR))) {
					retcode=IFFERR_NOMEMORY;
					goto endiff;
				}
				CopyMem((char *)animframe_bm[a],(char *)iffbm[a],sizeof(struct BitMap));
			}
		}
	}
	else if (!(iffbm[0]=getbitmap(bitmapwidth,bitmapheight,depth))) {
		retcode=IFFERR_NOMEMORY;
		goto endiff;
	}

	if (system_version2) {
		struct TagItem iffscr_tags[10];
		ULONG err=0;

		iffscr_tags[0].ti_Tag=SA_Width;
		iffscr_tags[0].ti_Data=bitmapwidth;
		iffscr_tags[1].ti_Tag=SA_Height;
		iffscr_tags[1].ti_Data=bitmapheight;
		iffscr_tags[2].ti_Tag=SA_Depth;
		iffscr_tags[2].ti_Data=depth;
		iffscr_tags[3].ti_Tag=SA_BitMap;
		iffscr_tags[3].ti_Data=(ULONG)iffbm[0];
		iffscr_tags[4].ti_Tag=SA_Behind;
		iffscr_tags[4].ti_Data=TRUE;
		iffscr_tags[5].ti_Tag=SA_DisplayID;
		iffscr_tags[5].ti_Data=viewmode;
		iffscr_tags[6].ti_Tag=SA_AutoScroll;
		iffscr_tags[6].ti_Data=TRUE;
		iffscr_tags[7].ti_Tag=SA_Overscan;
		iffscr_tags[7].ti_Data=OSCAN_MAX;
		iffscr_tags[8].ti_Tag=SA_ErrorCode;
		iffscr_tags[8].ti_Data=(ULONG)&err;
		iffscr_tags[9].ti_Tag=TAG_END;
		iffscr_tags[9].ti_Data=0;

		iffscreen=OpenScreenTagList(NULL,iffscr_tags);
	}
	else {
		struct NewScreen iffscr;

		iffscr.LeftEdge=0;
		iffscr.TopEdge=0;
		iffscr.Width=screenwidth;
		iffscr.Height=screenheight;
		iffscr.Depth=depth;
		iffscr.DetailPen=0;
		iffscr.BlockPen=0;
		iffscr.ViewModes=viewflags;
		iffscr.Type=CUSTOMSCREEN|SCREENBEHIND|CUSTOMBITMAP;
		iffscr.Font=NULL;
		iffscr.DefaultTitle=NULL;
		iffscr.Gadgets=NULL;
		iffscr.CustomBitMap=iffbm[0];

		iffscreen=OpenScreen(&iffscr);
	}

	if (!iffscreen) {
		retcode=IFFERR_NOSCREEN;
		goto endiff;
	}

	ivp=&(iffscreen->ViewPort);
	LoadRGB4(ivp,nullpalette,1<<depth);

	iffwin.Screen=iffscreen;
	iffwin.Width=iffscreen->Width;
	iffwin.Height=iffscreen->Height;

	if (!(iffwindow=OpenWindow(&iffwin))) {
		retcode=IFFERR_NOSCREEN;
		goto endiff;
	}

	setnullpointer(iffwindow);

	if (isanim && first_anim_frame) {
		framenum=1;
		framedisp=animrestart=0;
		current_anim_frame=first_anim_frame;

		rletobuffer(bodyptr,bmhead.w,bmhead.h,animframe_bm[0],bmhead.masking,bmhead.compression);

		if (doublebuffer) {
			if (animframe_bm[1]) {
				BltBitMap(animframe_bm[0],0,0,animframe_bm[1],0,0,bmhead.w,bmhead.h,0xc0,0xff,NULL);
				if (initialframe_bm)
					BltBitMap(animframe_bm[0],0,0,initialframe_bm,0,0,bmhead.w,bmhead.h,0xc0,0xff,NULL);
				WaitBlit();
			}
		}
		else {
			animframeimage.LeftEdge=(bitmapwidth-bmhead.w)/2;
			animframeimage.TopEdge=(bitmapheight-bmhead.h)/2;
			doanimframe();
		}
	}
	else readpic(&bmhead,bodyptr,iffbm[0]);

	if (numcolours>ivp->ColorMap->Count) numcolours=ivp->ColorMap->Count;

	if (!copperlist) {
		if (!system_version2) {
			int x,y;

			if (viewflags&HIRES) x=(scrdata_norm_width-screenwidth)/2;
			else x=((scrdata_norm_width/2)-screenwidth)/2;
			if (viewflags&LACE) y=((scrdata_norm_height*2)-screenheight)/2;
			else y=(scrdata_norm_height-screenheight)/2;
			ivp->DxOffset=x;
			ivp->DyOffset=y;
			MakeScreen(iffscreen);
		}
		if (viewflags&HAM || pchghead) {
			if (palette32) LoadRGB32(ivp,colourtable_8);
			else LoadRGB4(ivp,colourtable_4,numcolours);
		}
	}

	if (pchghead) {
#warning We have no pchgh lib
kprintf("HAVE NO PCHG LIB/PCHG_SetUserCopList()\n");
#if 0
		if (sham) PCHG_SetUserCopList(0,0,ivp,pchghead,&pchghead[1],sham);
		else PCHG_ParsePCHG(pchghead,ivp);
#endif
	}

	ActivateWindow(iffwindow);
	ScreenToFront(iffscreen);

	if (copperlist) {
		view=ViewAddress();
		if ((copperheight>>1)>scrdata_norm_height) view->DyOffset=14+(system_version2?8:0);
		if (mp) LoadRGB4(ivp,colourlist,4);
		if (!InitDHIRES(mp)) {
			retcode=IFFERR_NOMEMORY;
			goto endiff;
		}
	}
	else if (!specialformat && !(viewflags&HAM)) {
		if (palette32) FadeRGB32(iffscreen,&colourtable_8[1],numcolours,1,config->fadetime);
		else FadeRGB4(iffscreen,colourtable_4,numcolours,1,config->fadetime);
	}

	if (currange) initcycle(ivp,colourtable_4,numcolours,cyclerange,currange);

	if (!(a=WaitForMouseClick(scroll,iffwindow))) retcode=1;
	else retcode=-1;

	if (checkcycling()) togglecycle();
	stopcycle();

	if (specialformat) {
		ScreenToBack(iffscreen);
		FreeVPortCopLists(ivp);
	}
	else if (a!=-3 && !(viewflags&HAM)) {
		if (palette32) FadeRGB32(iffscreen,&colourtable_8[1],numcolours,-1,config->fadetime);
		else FadeRGB4(iffscreen,colourtable_4,numcolours,-1,config->fadetime);
	}
	
endiff:
#ifdef DO_DBUF
	if (dbufinfo) FreeDBufInfo(dbufinfo);
	if (dbufport) LDeletePort(dbufport);
#endif
	cleanupbitmap();
	ActivateWindow(Window);
	if (pchghead && sham) {
		FreeMem(pchghead,sizeof(struct PCHGHeader)+((pchghead->LineCount+31)/32)*4);
	}
	LFreeRemember(&iffkey);
	if (picbuffer) FreeMem(picbuffer,buffersize);
	first_anim_frame=NULL;
	colourtable_8=NULL; colourtable_4=NULL;
	return(retcode);
#endif	
}

struct BitMap *getbitmap(width,height,depth)
int width,height,depth;
{
	struct BitMap *bm;
	int a;

	if (system_version2>=OSVER_39) {
		return((AllocBitMap(width,height,depth,BMF_DISPLAYABLE|BMF_CLEAR,NULL)));
	}
	if (!(bm=AllocMem(sizeof(struct BitMap),MEMF_CLEAR))) return(NULL);
	InitBitMap(bm,depth,width,height);
	for (a=0;a<8;a++) bm->Planes[a]=NULL;
	for (a=0;a<depth;a++) {
		if (!(bm->Planes[a]=AllocRaster(width,height))) {
			freebitmap(bm,width,height);
			return(NULL);
		}
	}
	return(bm);
}

void freebitmap(bm,width,height)
struct BitMap *bm;
{
	int a;

	if (bm) {
		if (system_version2>=OSVER_39) FreeBitMap(bm);
		else {
			for (a=0;a<8;a++) if (bm->Planes[a]) FreeRaster(bm->Planes[a],width,height);
			FreeMem(bm,sizeof(struct BitMap));
		}
	}
}

void cleanupbitmap()
{
	int num;

	if (blankscreen) ScreenToFront(blankscreen);
	else ScreenToFront(Window->WScreen);
	if (iffwindow) {
		CloseWindow(iffwindow);
		iffwindow=NULL;
	}
	if (iffscreen) {
		CloseScreen(iffscreen);
		iffscreen=NULL;
	}
	if (iffbm[0] && !doublebuffer) freebitmap(iffbm[0],bitmapwidth,bitmapheight);
	for (num=0;num<2;num++) {
		if (animframe_bm[num]) freebitmap(animframe_bm[num],bitmapwidth,bitmapheight);
	}
	if (initialframe_bm) freebitmap(initialframe_bm,bitmapwidth,bitmapheight);
}

void rletobuffer(source,sourcewidth,sourceheight,dest,mask,comp)
unsigned char *source;
int sourcewidth,sourceheight;
struct BitMap *dest;
char mask,comp;
{
	struct RLEinfo picinfo;

	picinfo.sourceptr=source;
	picinfo.destplanes=dest->Planes;
	picinfo.imagebpr=((sourcewidth+15)/16)*2;
	picinfo.imageheight=sourceheight;
	picinfo.imagedepth=dest->Depth;
	picinfo.destbpr=dest->BytesPerRow;
	picinfo.destheight=dest->Rows;
	picinfo.destdepth=dest->Depth;
	picinfo.masking=mask;
	picinfo.compression=comp;
	picinfo.offset=0;

	decoderle(&picinfo);
}

void readpic(bmhead,source,bmap)
BitMapHeader *bmhead;
unsigned char *source;
struct BitMap *bmap;
{
	int rowbytes,bmrbytes,rows;
	int byteoff=0;
	struct RLEinfo picinfo;

	rowbytes=((bmhead->w+15)/16)*2;
	if (system_version2>=OSVER_39) bmrbytes=((GetBitMapAttr(bmap,BMA_WIDTH)+15)/16)*2;
	else bmrbytes=bmap->BytesPerRow;

	if ((rows=bmhead->h)>bmap->Rows) rows=bmap->Rows;

	if (!system_version2) {
		if (rowbytes<bmrbytes) byteoff=(bmrbytes-rowbytes)/2;
		else byteoff=0;
		if (rows<bmap->Rows) byteoff+=((bmap->Rows-rows)/2)*bmrbytes;
	}

	picinfo.sourceptr=source;
	picinfo.destplanes=bmap->Planes;
	picinfo.imagebpr=rowbytes;
	picinfo.imageheight=rows;
	picinfo.imagedepth=bmhead->nPlanes;
	picinfo.destbpr=bmrbytes;
	picinfo.destheight=bmap->Rows;
	picinfo.destdepth=bmap->Depth;
	picinfo.masking=bmhead->masking;
	picinfo.compression=bmhead->compression;
	picinfo.offset=byteoff;

	decoderle(&picinfo);
}

void decoderle(rleinfo)
struct RLEinfo *rleinfo;
{
	register char *source,*dest;
	register int copy,col;
	register char count;
	int plane,row,bmoffset,planes;

	planes=rleinfo->imagedepth;
	if (rleinfo->masking==1) ++planes;
	source=rleinfo->sourceptr;

	switch (rleinfo->compression) {
		case 1:
			bmoffset=rleinfo->offset;
			for (row=0;row<rleinfo->imageheight;row++) {
				for (plane=0;plane<planes;plane++) {
					if (plane<rleinfo->destdepth && rleinfo->destplanes[plane])
						dest=(char *)(rleinfo->destplanes[plane]+bmoffset);
					else dest=NULL;

					for (col=0;col<rleinfo->imagebpr;) {
						if ((count=*source++)>=0) {
							copy=count+1;
							col+=copy;
							if (dest) {
								while (copy--) *dest++=*source++;
							}
							else source+=copy;
						}
						else if (count!=-128) {
							copy=1-count;
							col+=copy;
							if (dest) {
								while (copy--) *dest++=*source;
							}
							++source;
						}
					}
				}
				bmoffset+=rleinfo->destbpr;
			}
			break;
		case 0:
			bmoffset=rleinfo->offset;
			for (row=0;row<rleinfo->imageheight;row++) {
				for (plane=0;plane<planes;plane++) {
					if (plane<rleinfo->destdepth)
						CopyMem(source,(char *)rleinfo->destplanes[plane]+bmoffset,rleinfo->imagebpr);
					source+=rleinfo->imagebpr;
				}
				bmoffset+=rleinfo->destbpr;
			}
			break;
	}
}

void doanimframe()
{
	struct AnimFrame *frame;

	if (last_anim_frame) {
		frame=last_anim_frame;
		last_anim_frame=last_anim_frame->next;
	}
	else {
		frame=&initial_anim_cmap;
		last_anim_frame=first_anim_frame;
	}

	if (frame->cmap) {
		build_palettes(frame->cmap,frame->cmapsize,colourtable_4,colourtable_8);
		if (palette32) LoadRGB32(ivp,colourtable_8);
		else LoadRGB4(ivp,colourtable_4,numcolours);
	}

	if (doublebuffer && iffbm[1]) {
#ifdef DO_DBUF
		if (dbufport) {
			ChangeVPBitMap(ivp,iffbm[framedisp],dbufinfo);
			WaitPort(dbufport);
			GetMsg(dbufport);
		}
		else {
			iffscreen->RastPort.BitMap=iffbm[framedisp];
			iffscreen->ViewPort.RasInfo->BitMap=iffbm[framedisp];
			MakeScreen(iffscreen);
			RethinkDisplay();
		}
#else
		iffscreen->RastPort.BitMap=iffbm[framedisp];
		iffscreen->ViewPort.RasInfo->BitMap=iffbm[framedisp];
		MakeScreen(iffscreen);
		RethinkDisplay();
#endif
	}
	else {
		BltBitMap(animframe_bm[framedisp],0,0,
			iffwindow->RPort->BitMap,animframeimage.LeftEdge,animframeimage.TopEdge,
			bmhead.w,bmhead.h,0xc0,0xff,NULL);
		WaitBlit();
	}
	if (animframe_bm[1]) framedisp=1-framedisp;

	if (!current_anim_frame) {
		if (animframe_bm[1]) {
			if (initialframe_bm) {
				BltBitMap(initialframe_bm,0,0,
					animframe_bm[framedisp],0,0,
					bmhead.w,bmhead.h,0xc0,0xff,NULL);
				WaitBlit();
			}
			else rletobuffer(bodyptr,bmhead.w,bmhead.h,
				animframe_bm[framedisp],bmhead.masking,bmhead.compression);
		}
		if (got_dpan || framenum==0) {
			current_anim_frame=first_anim_frame;
			last_anim_frame=first_anim_frame;
			framenum=1;
		}
		else framenum=0;
	}

	if (framenum>0) {
		switch (current_anim_frame->animheader->operation) {
			case 0:
				if (current_anim_frame->delta)
					rletobuffer(current_anim_frame->delta,bmhead.w,bmhead.h,
						animframe_bm[framedisp],bmhead.masking,bmhead.compression);
				break;
			case 5:
				doriff(current_anim_frame->delta,animframe_bm[framedisp],
					(current_anim_frame->animheader->bits&2 || !animframe_bm[1]),bmhead.w,0);
				break;
			case 7:
				doriff7(current_anim_frame->delta,animframe_bm[framedisp],
					bmhead.w,(current_anim_frame->animheader->bits&1)?2:1);
				break;
			case 8:
				doriff(current_anim_frame->delta,animframe_bm[framedisp],
					1,bmhead.w,(current_anim_frame->animheader->bits&1)?2:1);
				break;
		}
		++framenum;
		current_anim_frame=current_anim_frame->next;
	}
}

void doriff(delta,image,xor,sourcewidth,size)
unsigned char *delta;
struct BitMap *image;
int xor,sourcewidth,size;
{
	int plane,*dptr;
	unsigned char *data;

	sourcewidth=((sourcewidth+15)/16)*2;

	if ((dptr=(int *)delta)) {
		for (plane=0;plane<image->Depth;plane++) {
			if (dptr[plane]) {
				data=(unsigned char *)delta+dptr[plane];
				if (size==0) {
					if (xor) decode_riff_xor(data,image->Planes[plane],image->BytesPerRow,sourcewidth);
					else decode_riff_set(data,image->Planes[plane],image->BytesPerRow,sourcewidth);
				}
				else if (size==1)
					decode_riff_short((USHORT *)data,image->Planes[plane],image->BytesPerRow,sourcewidth);
				else decode_riff_long((ULONG *)data,image->Planes[plane],image->BytesPerRow,sourcewidth);
			}
		}
	}
}

void doriff7(delta,image,sourcewidth,size)
unsigned char *delta;
struct BitMap *image;
int sourcewidth,size;
{
	int plane,*dptr;
	unsigned char *data;
	unsigned char *opcode;

	sourcewidth=((sourcewidth+15)/16)*2;

	if ((dptr=(int *)delta)) {
		for (plane=0;plane<image->Depth;plane++) {
			if (dptr[plane] && dptr[plane+8]) {
				opcode=(unsigned char *)delta+dptr[plane];
				data=(unsigned char *)delta+dptr[plane+8];
				if (size==1)
					decode_riff7_short((USHORT *)data,opcode,image->Planes[plane],image->BytesPerRow,sourcewidth);
				else decode_riff7_long((ULONG *)data,opcode,image->Planes[plane],image->BytesPerRow,sourcewidth);
			}
		}
	}
}

void decode_riff_xor(delta,image,rowbytes,sourcebytes)
unsigned char *delta;
char *image;
int rowbytes,sourcebytes;
{
	int column,opcount;
	unsigned char *data,ch;

	for (column=0;column<sourcebytes;column++) {
		data=image+column;
		opcount=*delta++;
		while (opcount--) {
			ch=*delta++;
			if (ch) {
				if (!(ch&128)) data+=rowbytes*ch;
				else {
					ch&=127;
					while (ch--) {
						*data^=*delta++;
						data+=rowbytes;
					}
				}
			}
			else {
				ch=*delta++;
				while (ch--) {
					*data^=*delta;
					data+=rowbytes;
				}
				++delta;
			}
		}
	}
}

void decode_riff_set(delta,image,rowbytes,sourcebytes)
unsigned char *delta;
char *image;
int rowbytes,sourcebytes;
{
	int column,opcount;
	unsigned char *data,ch;

	for (column=0;column<sourcebytes;column++) {
		data=image+column;
		opcount=*delta++;
		while (opcount--) {
			ch=*delta++;
			if (ch) {
				if (!(ch&128)) data+=rowbytes*ch;
				else {
					ch&=127;
					while (ch--) {
						*data=*delta++;
						data+=rowbytes;
					}
				}
			}
			else {
				ch=*delta++;
				while (ch--) {
					*data=*delta;
					data+=rowbytes;
				}
				++delta;
			}
		}
	}
}

void decode_riff_short(delta,image,rowbytes,sourcebytes)
USHORT *delta;
unsigned char *image;
int rowbytes,sourcebytes;
{
	int column,opcount;
	unsigned char *data;
	USHORT ch;

	sourcebytes/=2;

	for (column=0;column<sourcebytes;column++) {
		data=(unsigned char *)(image+(column*2));
		opcount=*delta++;
		while (opcount--) {
			ch=*delta++;
			if (ch) {
				if (!(ch&0x8000)) data+=rowbytes*ch;
				else {
					ch&=0x7fff;
					while (ch--) {
						*(USHORT *)data=*delta++;
						data+=rowbytes;
					}
				}
			}
			else {
				ch=*delta++;
				while (ch--) {
					*(USHORT *)data=*delta;
					data+=rowbytes;
				}
				++delta;
			}
		}
	}
}

void decode_riff_long(delta,image,rowbytes,sourcebytes)
ULONG *delta;
unsigned char *image;
int rowbytes,sourcebytes;
{
	int column,opcount;
	unsigned char *data;
	ULONG ch;

	sourcebytes/=4;

	for (column=0;column<sourcebytes;column++) {
		data=(unsigned char *)(image+(column*4));
		opcount=*delta++;
		while (opcount--) {
			ch=*delta++;
			if (ch) {
				if (!(ch&0x80000000)) data+=rowbytes*ch;
				else {
					ch&=0x7fffffff;
					while (ch--) {
						*(ULONG *)data=*delta++;
						data+=rowbytes;
					}
				}
			}
			else {
				ch=*delta++;
				while (ch--) {
					*(ULONG *)data=*delta;
					data+=rowbytes;
				}
				++delta;
			}
		}
	}
}

void decode_riff7_short(delta,opcode,image,rowbytes,sourcebytes)
USHORT *delta;
unsigned char *opcode;
unsigned char *image;
int rowbytes,sourcebytes;
{
	int column,opcount;
	unsigned char *data,ch;

	sourcebytes/=2;

	for (column=0;column<sourcebytes;column++) {
		data=(unsigned char *)(image+(column*2));
		opcount=*opcode++;
		while (opcount--) {
			ch=*opcode++;
			if (ch) {
				if (!(ch&0x80)) data+=rowbytes*ch;
				else {
					ch&=0x7f;
					while (ch--) {
						*(USHORT *)data=*delta++;
						data+=rowbytes;
					}
				}
			}
			else {
				ch=*opcode++;
				while (ch--) {
					*(USHORT *)data=*delta;
					data+=rowbytes;
				}
				++delta;
			}
		}
	}
}

void decode_riff7_long(delta,opcode,image,rowbytes,sourcebytes)
ULONG *delta;
unsigned char *opcode;
unsigned char *image;
int rowbytes,sourcebytes;
{
	int column,opcount;
	unsigned char *data,ch;

	sourcebytes/=4;

	for (column=0;column<sourcebytes;column++) {
		data=(unsigned char *)(image+(column*4));
		opcount=*opcode++;
		while (opcount--) {
			ch=*opcode++;
			if (ch) {
				if (!(ch&0x80)) data+=rowbytes*ch;
				else {
					ch&=0x7f;
					while (ch--) {
						*(ULONG *)data=*delta++;
						data+=rowbytes;
					}
				}
			}
			else {
				ch=*opcode++;
				while (ch--) {
					*(ULONG *)data=*delta;
					data+=rowbytes;
				}
				++delta;
			}
		}
	}
}

int WaitForMouseClick(tits,wind)
int tits;
struct Window *wind;
{
	struct IntuiMessage *msg;
	ULONG class;
	USHORT code,qual;
	int ticks,waitfor=0,pon=0,waitbits,retcode=1,gottimer=0,origspeed,anim,delta;
	struct timerequest treq;

	while ((msg=(struct IntuiMessage *) GetMsg(wind->UserPort)))
		ReplyMsg((struct Message *)msg);
	if (!first_anim_frame && config->showdelay>0) {
		waitfor=config->showdelay*10; ticks=0;
	}

	origspeed=animspeed;

	waitbits=1<<wind->UserPort->mp_SigBit;
	if (first_anim_frame) {
		if (!(OpenDevice(TIMERNAME,UNIT_VBLANK,&treq.tr_node,0))) {
			treq.tr_node.io_Message.mn_ReplyPort=general_port;
			treq.tr_node.io_Command=TR_ADDREQUEST;
			treq.tr_node.io_Flags=0;
			treq.tr_time.tv_secs=0;
			treq.tr_time.tv_micro=10;
			SendIO(&treq.tr_node);
			waitbits|=1<<general_port->mp_SigBit;
			gottimer=1;
		}
		if (!(anim=(!(config->viewbits&VIEWBITS_PAUSEANIMS))) && doublebuffer)
			doanimframe();
	}

	FOREVER {
		if (first_anim_frame) {
			while (GetMsg(general_port));
			if (CheckIO(&treq.tr_node)) {
				WaitIO(&treq.tr_node);
				treq.tr_time.tv_secs=0;
				treq.tr_time.tv_micro=animspeed;
				SendIO(&treq.tr_node);
				if (anim) doanimframe();
			}
		}
		while ((msg=(struct IntuiMessage *) GetMsg(wind->UserPort))) {
			class=msg->Class; code=msg->Code; qual=msg->Qualifier;
			ReplyMsg((struct Message *) msg);
			switch (class) {
				case IDCMP_INTUITICKS:
					if (waitfor && (++ticks)>=waitfor) retcode=0;
					break;
				case IDCMP_RAWKEY:
					if (code>=0x50 && code<=0x59) {
						code-=0x50;
						animspeed=(code*10000)+10;
						break;
					}
					switch (code) {
						case 0x0b:
							if ((animspeed+=10000)>1000000) animspeed=1000000;
							break;
						case 0x0c:
							if ((animspeed-=10000)<10) animspeed=10;
							break;
						case 0x0d:
							animspeed=origspeed;
							break;
						case 0x21:
							anim=1-anim;
							break;
						case 0x36:
							if (first_anim_frame) {
								anim=0;
								doanimframe();
							}
							break;
						case CURSOR_LEFT:
						case 0x2d:
						case CURSOR_RIGHT:
						case 0x2f:
							if (tits!=3) break;
							if (qual&IEQUALIFIER_CONTROL) delta=-bitmapwidth;
							else if (qual&IEQUALIFIER_ANYSHIFT) delta=-(screenwidth-20);
							else delta=-2;
							if (code==CURSOR_RIGHT || code==0x2f) delta=-delta;
							if (system_version2) MoveScreen(wind->WScreen,-delta,0);
							else {
								ivp->RasInfo->RxOffset+=delta;
								docheckrasscroll(wind->WScreen);
							}
							break;
						case CURSOR_UP:
						case 0x3e:
						case CURSOR_DOWN:
						case 0x1e:
							if (tits!=3) break;
							if (qual&IEQUALIFIER_CONTROL) delta=bitmapheight;
							else if (qual&IEQUALIFIER_ANYSHIFT) delta=screenheight-20;
							else delta=2;
							if (code==CURSOR_UP || code==0x3e) {
								delta=-delta;
								if (system_version2) {
									if (wind->WScreen->TopEdge-delta>0)
										delta=wind->WScreen->TopEdge;
								}
							}
							if (system_version2) MoveScreen(wind->WScreen,0,-delta);
							else {
								ivp->RasInfo->RyOffset+=delta;
								docheckrasscroll(wind->WScreen);
							}
							break;
						case 0x44:
							if (tits==2) retcode=-2;
							else retcode=0;
							break;
						case 0x45:
							retcode=-1;
							break;
						case 0x10:
							if (!(qual&IEQUALIFIER_REPEAT)) retcode=0;
							break;
						case 0x42:
							if (tits&1 && currange) togglecycle();
							break;
						case 0x5f:
						case 0x19:
						case 0x40:
							if (!(qual&IEQUALIFIER_REPEAT)) {
								ModifyIDCMP(wind,(wind->IDCMPFlags&~IDCMP_INACTIVEWINDOW)|IDCMP_ACTIVEWINDOW);
								if (tits&1) gfxprint(wind,&iffscreen->RastPort,0,0,bitmapwidth,bitmapheight,1);
								else gfxprint(wind,wind->RPort,0,0,wind->Width,wind->Height,0);
							}
							break;
						case 0x39:
							if (pon) setnullpointer(wind);
							else ClearPointer(wind);
							pon^=1;
							break;
						case 0x1a:
							if (colourtable_4 && !specialformat)
								LoadRGB4(ivp,colourtable_4,numcolours);
							break;
						case 0x1b:
							if (colourtable_8 && !specialformat)
								LoadRGB32(ivp,colourtable_8);
							break;
					}
					break;
				case IDCMP_ACTIVEWINDOW:
					ModifyIDCMP(wind,(wind->IDCMPFlags&~IDCMP_ACTIVEWINDOW)|IDCMP_INACTIVEWINDOW);
					break;
				case IDCMP_INACTIVEWINDOW:
					if (IntuitionBase->ActiveScreen!=wind->WScreen) {
						retcode=-3;
						break;
					}
					code=SELECTDOWN;
				case IDCMP_MOUSEBUTTONS:
					if (code==SELECTDOWN) retcode=0;
					else if (code==MENUDOWN) retcode=-1;
					break;
			}
			if (retcode!=1) break;
		}
		if (retcode!=1) break;
		Wait(waitbits);
	}
	if (first_anim_frame && gottimer) {
		if (!(CheckIO(&treq.tr_node)))
			AbortIO(&treq.tr_node);
		WaitIO(&treq.tr_node);
		CloseDevice(&treq.tr_node);
		while (GetMsg(general_port));
	}
	return(retcode);
}

int chunkread(buf,nbytes)
void *buf;
ULONG nbytes;
{
	if (bufferpos+nbytes>buffersize) return(0);
	CopyMem((char *)&picbuffer[bufferpos],(char *)buf,nbytes);
	bufferpos+=nbytes;
	return(1);
}

void getcolstring(str)
char *str;
{
	if (viewflags&HAM || specialformat) {
		if (bmhead.nPlanes==8) strcpy(str,"256K");
		else strcpy(str,"4096");
	}
	else lsprintf(str,"%ld",1<<bmhead.nPlanes);
}

void dotitle()
{
	char title[100],modes[140],cols[30];

	getcolstring(cols);
	getviewmodes(modes);
	lsprintf(title,"%ld x %ld x %ld (page %ld x %ld) %s cols (%s)",
		bmhead.w,bmhead.h,bmhead.nPlanes,bmhead.pageWidth,bmhead.pageHeight,
		cols,modes);
	dostatustext(title);
}

void gfxprint(wind,rast,x,y,w,h,iff)
struct Window *wind;
struct RastPort *rast;
int x,y,w,h,iff;
{

#warning Gfx Printing not implemented

kprintf("NOT IMPLEMENTE gfxprint() BECAUSE OF MISSING STUFF\n");
return;
#if 0

	struct IntuiMessage *msg;
	struct Window *pwin;
	struct Screen *pscr;
	struct RastPort *prp;
	ULONG class;
	USHORT code,gadgetid,qual;
	struct PrinterData *pd;
	struct Preferences *prefs;
	int a,b,abort=0,goff,fnum;
	char buf[200],modes[140],*ptr,pactcode[6],cols[30],title[120];
	APTR save;
	struct DOpusRemember *pkey=NULL;
	static char firsttime;

	if (scrdata_is_pal) {
		printscreen.Height=printwin.Height=256;
		goff=28;
	}
	else {
		printscreen.Height=printwin.Height=200;
		goff=0;
	}

	for (a=0;a<7;a++) {
		if (a==5) printgadtxt[a]=globstring[STR_PRINTTITLE];
		else if (a==6) printgadtxt[a]=globstring[STR_PRINT];
		else printgadtxt[a]=globstring[STR_ASPECT+a];
		if (a<6) {
			for (b=0;;b++) {
				if (!printgadtxt[a][b]) break;
				if (printgadtxt[a][b]=='_') {
					pactcode[a]=tolower(printgadtxt[a][b+1]);
					break;
				}
			}
		}
	}

	printgadtxt[7]=str_cancelstring;
	printgadtxt[8]=NULL;
	printabortgadtxt[0]=globstring[STR_ABORT_PRINT];
	for (a=0;a<2;a++) {
		print_aspect_txt[a]=globstring[STR_PORTRAIT+a];
		print_image_txt[a]=globstring[STR_POSITIVE+a];
		print_placement_txt[a]=globstring[STR_CENTER+a];
	}
	for (a=0;a<3;a++) print_shade_txt[a]=globstring[STR_BLACK_WHITE+a];

	if (!(pscr=OpenScreen((struct NewScreen *)&printscreen))) return;
	LoadRGB4(&(pscr->ViewPort),basepalette,4);
	printwin.Screen=pscr;
	if (!(pwin=OpenWindow(&printwin))) {
		CloseScreen(pscr);
		return;
	}
	prp=pwin->RPort;
	SetFont(prp,scr_font[FONT_GENERAL]);
	SetAPen(prp,1);
	Do3DFrame(prp,0,4+goff,320,88,globstring[STR_DESCRIPTION],2,1);
	Do3DFrame(prp,0,98+goff,320,86,globstring[STR_PRINT_CONTROL],2,1);

	if (iff) {
		ptr=BaseName(picturename);
		getcolstring(cols);
		lsprintf(buf,"%12s : %s",
			globstring[STR_FILE],
			ptr);
		iffinfotxt(prp,buf,7,19+goff);

		lsprintf(buf,"%12s : %ld x %ld",
			globstring[STR_IMAGE_SIZE],
			bmhead.w,
			bmhead.h);
		iffinfotxt(prp,buf,7,35+goff);

		if (first_anim_frame) {
			if (framenum<2) fnum=1;
			else fnum=framenum-1;
			lsprintf(buf,"%12s : %ld %s %ld @ %ld/sec",
				globstring[STR_NUM_FRAMES],
				fnum,
				globstring[STR_OF],
				framecount+(1-got_dpan),
				framespersec);
			iffinfotxt(prp,buf,7,43+goff);

			lsprintf(buf,"%12s : ANIM Op %ld",
				globstring[STR_ANIM_TYPE],
				animtype);
			iffinfotxt(prp,buf,7,51+goff);

			lsprintf(title,"IFF ANIM : %s   %s %ld %s %ld   %ld x %ld x %s\n\n",
				ptr,
				globstring[STR_FRAME],
				fnum,
				globstring[STR_OF],
				framecount+1,
				bmhead.w,
				bmhead.h,
				cols);
		}
		else {
			lsprintf(buf,"%12s : %ld x %ld",
				globstring[STR_PAGE_SIZE],
				bmhead.pageWidth,
				bmhead.pageHeight);
			iffinfotxt(prp,buf,7,43+goff);

			lsprintf(buf,"%12s : %ld x %ld",
				globstring[STR_SCREEN_SIZE],
				iffscreen->Width,
				iffscreen->Height);
			iffinfotxt(prp,buf,7,51+goff);

			lsprintf(title,"IFF ILBM : %s   %ld x %ld x %s\n\n",
				ptr,
				bmhead.w,
				bmhead.h,
				cols);
		}
		lsprintf(buf,"%12s : %ld",
			globstring[STR_DEPTH],
			bmhead.nPlanes);
		iffinfotxt(prp,buf,7,67+goff);

		lsprintf(buf,"%12s : %s",
			globstring[STR_COLOURS],
			cols);
		iffinfotxt(prp,buf,7,75+goff);

		getviewmodes(modes);
		lsprintf(buf,"%12s : %s",
			globstring[STR_SCREEN_MODES],
			modes);
		iffinfotxt(prp,buf,7,83+goff);
	}
	else if (show_global_font) {
		char fontname[40],*ptr;

		strcpy(fontname,show_global_font->tf_Message.mn_Node.ln_Name);
		if ((ptr=strstri(fontname,".font"))) *ptr=0;

		lsprintf(buf,"%12s : %s",
			globstring[STR_FONT],
			fontname);
		iffinfotxt(prp,buf,7,19+goff);

		lsprintf(buf,"%12s : %ld",
			globstring[STR_FONT_SIZE],
			show_global_font->tf_YSize);
		iffinfotxt(prp,buf,7,35+goff);

		lsprintf(buf,"%12s : %ld (%ld - %ld)",
			globstring[STR_NUM_CHARS],
			(show_global_font->tf_HiChar-show_global_font->tf_LoChar)+1,
			show_global_font->tf_LoChar,
			show_global_font->tf_HiChar);
		iffinfotxt(prp,buf,7,51+goff);

		lsprintf(buf,"%12s :",
			globstring[STR_FONT_STYLE]);
		if ((show_global_font->tf_Style&15)==0) strcat(buf," NORMAL");
		else {
			if (show_global_font->tf_Style&FSF_UNDERLINED) strcat(buf," ULINED");
			if (show_global_font->tf_Style&FSF_BOLD) strcat(buf," BOLD");
			if (show_global_font->tf_Style&FSF_ITALIC) strcat(buf," ITALIC");
			if (show_global_font->tf_Style&FSF_EXTENDED) strcat(buf," EXTEND");
		}
		iffinfotxt(prp,buf,7,67+goff);

		lsprintf(buf,"%12s :",globstring[STR_FONT_FLAGS]);
		if (show_global_font->tf_Flags&FPF_TALLDOT) strcat(buf," TALL");
		if (show_global_font->tf_Flags&FPF_WIDEDOT) strcat(buf," WIDE");
		if (show_global_font->tf_Flags&FPF_PROPORTIONAL)
			strcat(buf," PROP");
		iffinfotxt(prp,buf,7,75+goff);

		lsprintf(title,"Font : %s/%ld   %ld chars (%ld - %ld)\n\n",
			fontname,show_global_font->tf_YSize,
			(show_global_font->tf_HiChar-show_global_font->tf_LoChar)+1,
			show_global_font->tf_LoChar,
			show_global_font->tf_HiChar);
	}
	else if (show_global_icon) {
		lsprintf(buf,"%12s : %s",
			globstring[STR_ICON],
			show_global_icon_name);
		iffinfotxt(prp,buf,7,27+goff);

		lsprintf(buf,"%12s : %s",
			globstring[STR_ICON_TYPE],
			icon_type_names[show_global_icon->do_Type-1]);
		iffinfotxt(prp,buf,7,43+goff);

		lsprintf(buf,"%12s : %s",
			globstring[STR_ICON_ALTERNATE],
			(show_global_icon->do_Gadget.Flags&GFLG_GADGHIMAGE)?
			globstring[STR_YES]:globstring[STR_NO]);
		iffinfotxt(prp,buf,7,59+goff);

		if (show_global_icon->do_DefaultTool) {
			lsprintf(buf,"%12s : %s",
				globstring[STR_ICON_DEFAULTTOOL],
				show_global_icon->do_DefaultTool);
			iffinfotxt(prp,buf,7,75+goff);
		}
		lsprintf(title,"Workbench %s : %s\n\n",
			globstring[STR_ICON],
			show_global_icon_name);
	}

	for (a=0;a<8;a++) printgadgets[a].TopEdge+=goff;
	printcheckimage.ImageData=(USHORT *)DOpusBase->pdb_check;
	AddGadgetBorders(&pkey,printgadgets,4,2,1);
	AddGadgetBorders(&pkey,&printgadgets[PRINT_OKAY],2,2,1);
	AddGadgets(pwin,printgadgets,printgadtxt,8,2,1,1);

	if (!firsttime) {
		struct Preferences prefs;

		GetPrefs(&prefs,sizeof(struct Preferences));
		print_gads_sel[PRINT_ASPECT]=prefs.PrintAspect;
		print_gads_sel[PRINT_IMAGE]=prefs.PrintImage;
		print_gads_sel[PRINT_SHADE]=prefs.PrintShade;

		firsttime=1;
	}

	for (a=0;a<4;a++) {
		DoCycleGadget(&printgadgets[a],
			pwin,
			print_gads_txt[a],
			print_gads_sel[a]);
	}

	ScreenToFront(pscr);
	ActivateWindow(pwin);
	save=main_proc->pr_WindowPtr;
	main_proc->pr_WindowPtr=(APTR)pwin;

	FOREVER {
		Wait(1<<pwin->UserPort->mp_SigBit);
		while ((msg=(struct IntuiMessage *) GetMsg(pwin->UserPort))) {
			class=msg->Class; code=msg->Code; qual=msg->Qualifier;
			if (class==IDCMP_GADGETUP)
				gadgetid=((struct Gadget *)msg->IAddress)->GadgetID;
			ReplyMsg((struct Message *) msg);
			switch (class) {
				case IDCMP_VANILLAKEY:
					if (qual&IEQUALIFIER_REPEAT) break;
					for (a=0;a<6;a++) {
						if (code==pactcode[a] || code==toupper(pactcode[a])) {
							if (a<4) {
								if (code==toupper(pactcode[a])) qual|=IEQUALIFIER_LSHIFT;
								SelectGadget(pwin,&printgadgets[a]);
								gadgetid=a;
								goto docyclegads;
							}
							else if (a==4) {
								printgadgets[4].Flags^=GFLG_SELECTED;
								RefreshGList(&printgadgets[4],pwin,NULL,1);
							}
							else if (a==5) {
								printgadgets[5].Flags^=GFLG_SELECTED;
								RefreshGList(&printgadgets[5],pwin,NULL,1);
							}
						}
					}
					if (code==globstring[STR_PRINT][0]) {
						SelectGadget(pwin,&printgadgets[6]);
						goto doprint;
					}
					else if (code=='' || code==' ' || code==tolower(str_cancelstring[0])) {
						SelectGadget(pwin,&printgadgets[7]);
						goto endprint;
					}
					break;

				case IDCMP_GADGETUP:
					if (gadgetid<PRINT_FORMFD) {
docyclegads:
						if (qual&IEQUALIFIER_ANYSHIFT) {
							if ((--print_gads_sel[gadgetid])<0)
								print_gads_sel[gadgetid]=print_gads_max[gadgetid];
						}
						else {
							if ((++print_gads_sel[gadgetid])>print_gads_max[gadgetid])
								print_gads_sel[gadgetid]=0;
						}
						DoCycleGadget(&printgadgets[gadgetid],
							pwin,
							print_gads_txt[gadgetid],
							print_gads_sel[gadgetid]);
					}
					else switch (gadgetid) {
						case PRINT_CANCEL:
							goto endprint;
						case PRINT_OKAY:
							goto doprint;
					}
					break;
			}
		}
	}
doprint:
	RemoveGList(pwin,printgadgets,-1);
	SetAPen(prp,0);
	RectFill(prp,0,0,319,pwin->Height-1);
	SetAPen(prp,1);
	abortprintgad.TopEdge=(pwin->Height-80)/2;
	AddGadgetBorders(&pkey,&abortprintgad,1,2,1);
	AddGadgets(pwin,&abortprintgad,printabortgadtxt,1,2,1,1);
	SetBusyPointer(pwin);
	if ((print_request=(union printerIO *) LCreateExtIO(general_port,sizeof(union printerIO)))) {
		if (!(OpenDevice("printer.device",0,(struct IORequest *)print_request,0))) {
			pd=(struct PrinterData *)print_request->iodrp.io_Device;
			prefs=&pd->pd_Preferences;
			prefs->PrintAspect=print_gads_sel[PRINT_ASPECT];
			prefs->PrintImage=print_gads_sel[PRINT_IMAGE];
			prefs->PrintShade=print_gads_sel[PRINT_SHADE];
			prefs->PrintXOffset=0;

			print_request->ios.io_Command=CMD_WRITE;
			print_request->ios.io_Data="\033#1";
			print_request->ios.io_Length=-1;
			while (DoIO((struct IORequest *)print_request)) {
				reqoverride=pwin;
				a=simplerequest(globstring[STR_ERROR_INITIALISING_PRINTER],
					globstring[STR_TRY_AGAIN],str_cancelstring,NULL);
				reqoverride=NULL;
				if (!a) goto closeprinter;
			}

			if (printgadgets[5].Flags&GFLG_SELECTED) {
				print_request->ios.io_Command=CMD_WRITE;
				print_request->ios.io_Data=title;
				print_request->ios.io_Length=-1;
				if (DoIO((struct IORequest *)print_request)) goto closeprinter;
			}

			print_request->iodrp.io_Command=PRD_DUMPRPORT;
			print_request->iodrp.io_RastPort=rast;
			print_request->iodrp.io_ColorMap=wind->WScreen->ViewPort.ColorMap;
			if (system_version2) {
				print_request->iodrp.io_Modes=GetVPModeID(&(wind->WScreen->ViewPort));
			}
			else {
				print_request->iodrp.io_Modes=wind->WScreen->ViewPort.Modes;
			}
			print_request->iodrp.io_SrcX=x;
			print_request->iodrp.io_SrcY=y;
			print_request->iodrp.io_SrcWidth=w;
			print_request->iodrp.io_SrcHeight=h;
			print_request->iodrp.io_Special=SPECIAL_FULLCOLS|SPECIAL_ASPECT|SPECIAL_NOFORMFEED;
			if (!print_gads_sel[PRINT_PLACE])
				print_request->iodrp.io_Special|=SPECIAL_CENTER;
			if (!(printgadgets[4].Flags&GFLG_SELECTED))
				print_request->iodrp.io_Special|=SPECIAL_NOFORMFEED;

			SendIO((struct IORequest *)print_request);
			FOREVER {
				Wait(1<<pwin->UserPort->mp_SigBit|1<<general_port->mp_SigBit);
				while (GetMsg(general_port));
				while (msg=(struct IntuiMessage *) GetMsg(pwin->UserPort)) {
					class=msg->Class;
					if (class==IDCMP_GADGETUP)
						gadgetid=((struct Gadget *)msg->IAddress)->GadgetID;
					ReplyMsg((struct Message *) msg);
					if (class==IDCMP_GADGETUP && gadgetid==PRINT_ABORT) {
						reqoverride=pwin;
						abort=simplerequest(globstring[STR_REALLY_ABORT_PRINT],
							globstring[STR_ABORT],globstring[STR_CONTINUE],NULL);
						reqoverride=NULL;
						if (abort) AbortIO((struct IORequest *)print_request);
						break;
					}
				}
				if (abort || (CheckIO((struct IORequest *)print_request))) break;
			}
			WaitIO((struct IORequest *)print_request);
			if (!abort && printgadgets[4].Flags&GFLG_SELECTED) {
				print_request->ios.io_Command=CMD_WRITE;
				print_request->ios.io_Data="\f";
				print_request->ios.io_Length=-1;
				DoIO((struct IORequest *)print_request);
			}
closeprinter:
			CloseDevice((struct IORequest *)print_request);
		}
		LDeleteExtIO((struct IORequest *)print_request);
	}
endprint:
	for (a=0;a<8;a++) printgadgets[a].TopEdge-=goff;
	main_proc->pr_WindowPtr=save;
	ActivateWindow(iffwindow);
	ScreenToBack(pscr);
	CloseWindow(pwin);
	CloseScreen(pscr);
	LFreeRemember(&pkey);
#endif
}

int InitDHIRES(mp)
int mp;
{
#warning Not implemented because of copper usage
kprintf("InitDHIRES(): NOT IMPLEMENTED BECAUSE OF COPPER MACRO USAGE\n");
return 0;

#if 0
	int line,creg,top,oscan,lace;
	struct UCopList *ucop;

	if (!(ucop=(struct UCopList *)AllocMem(sizeof(struct UCopList),MEMF_CLEAR)))
		return(0);

	lace=(viewflags&LACE)?1:0;
	top=copperheight;
	if (lace) top/=2;

	if (mp) {
		if (screenwidth>640) oscan=14;
		else oscan=16;
		CINIT(ucop,top*13);
		for (line=0;line<top;line++) {
			CWAIT(ucop,(line-1)<<lace,112);
			for (creg=4;creg<oscan;creg++) {
				CMove(ucop,(long *)(0xdff180+(creg*2)),copperlist[(line<<(4+lace))+creg]);
				CBump(ucop);
			}
		}
	}
	else {
		CINIT(ucop,copperheight*17);
		for (line=0;line<copperheight;line++) {
			CWAIT(ucop,line-1,122);
			for (creg=4;creg<16;creg++) {
				CMove(ucop,(long *)(0xdff180+(creg*2)),copperlist[(line<<4)+creg]);
				CBump(ucop);
			}
		}
	}

	CEND(ucop);
	Forbid();
	ivp->UCopIns=ucop;
	Permit();
	RethinkDisplay();
	return(1);
#endif
}

void getviewmodes(modes)
char *modes;
{
	if (iffscreenname[0]) strcpy(modes,iffscreenname);
	else {
		if (viewflags&SUPERHIRES) {
			strcpy(modes,"SUPERHIRES");
			if (bmhead.w>1280) strcat(modes," OSCAN");
		}
		else if (viewflags&HIRES) {
			strcpy(modes,"HIRES");
			if (bmhead.w>640) strcat(modes," OSCAN");
		}
		else {
			strcpy(modes,"LORES");
			if (bmhead.w>320) strcat(modes," OSCAN");
		}
		if (viewflags&LACE) strcat(modes," LACE");
	}
	if (copperlist) strcat(modes," DHIRES");
	else if (pchghead) {
		if (sham) strcat(modes," SHAM");
		else strcat(modes," PCHG");
	}
	else {
		if (viewflags&HAM) strcat(modes," HAM");
		else if (viewflags&EXTRA_HALFBRITE) strcat(modes," EHB");
	}
}

void docheckrasscroll(scr)
struct Screen *scr;
{
	if (ivp->RasInfo->RxOffset+screenwidth>bitmapwidth)
		ivp->RasInfo->RxOffset=bitmapwidth-ivp->DWidth;
	if (ivp->RasInfo->RxOffset<0) ivp->RasInfo->RxOffset=0;
	if (ivp->RasInfo->RyOffset+screenheight>bitmapheight)
		ivp->RasInfo->RyOffset=bitmapheight-ivp->DHeight;
	if (ivp->RasInfo->RyOffset<0) ivp->RasInfo->RyOffset=0;
	MakeScreen(scr);
	RethinkDisplay();
}

void iffinfotxt(r,buf,x,y)
struct RastPort *r;
char *buf;
int x,y;
{
	int a,l;

	a=strlen(buf); l=(316-x)/8;
	Move(r,x,y);
	Text(r,buf,(a>l)?l:a);
}

void build_palettes(colourdata,coloursize,ctable4,ctable8)
unsigned char *colourdata;
int coloursize;
UWORD *ctable4;
ULONG *ctable8;
{
	int a,b;
	unsigned int rgb[3];

	if (ctable8) {
		for (a=0,b=1;a<coloursize;a++,b++)
/*
			ctable8[b]=(colourdata[a]<<24)|0x00ffffff;
*/
			ctable8[b]=
				((colourdata[a]<<24)&0xff000000)|
				((colourdata[a]<<16)&0x00ff0000)|
				((colourdata[a]<<8)&0x0000ff00)|
				((colourdata[a]&0x000000ff));

		ctable8[0]=numcolours<<16;
		ctable8[(numcolours*3)+1]=0;
	}

	if (ctable4) {
		for (a=0;a<numcolours;a++) {
			for (b=0;b<3;b++) rgb[b]=(unsigned int)(((unsigned char)(*(colourdata++)))>>4);
			ctable4[a]=(rgb[0]<<8)+(rgb[1]<<4)+rgb[2];
		}
	}
}

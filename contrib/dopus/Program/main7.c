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
#include "music.h"

/* AROS: Needs some obsolete defines */
#include <intuition/iobsolete.h>

int showpic(fullname,np)
char *fullname;
int np;
{
	int res,a;
	char buf[256];

	if (checkexec(fullname) && checkisfont(fullname,buf))
		return((showfont(buf,atoi(BaseName(fullname)),np)));

	a=strlen(fullname);
	if (a>5 && LStrCmpI(&fullname[a-5],".info")==0) {
		if ((res=readicon(fullname,np))==0) return(1);
		if (res==-2) {
			doerror(IoErr());
			return(0);
		}
		if (res==-3) {
			dostatustext(globstring[STR_NO_CHIP_FOR_ICON]);
			return(0);
		}
		return(res);
	}

	if (!(res=LoadPic(fullname))) doerror(IoErr());
	else if (res==IFFERR_NOTILBM) dostatustext(globstring[STR_FILE_NOT_ILBM]);
	else if (res==IFFERR_BADIFF) dostatustext(globstring[STR_ERROR_IN_IFF]);
	else if (res==IFFERR_NOMEMORY) dostatustext(globstring[STR_NO_CHIP_FOR_PICTURE]);
	else if (res==IFFERR_BADMODE) dostatustext(globstring[STR_SCREEN_MODE_UNAVAILABLE]);
	else if (res==IFFERR_NOSCREEN) dostatustext(globstring[STR_UNABLE_TO_OPEN_WINDOW]);
	if (res==-1) return(-1);
	else if (!res || res<-1) return(0);
	return(1);
}

int checkisfont(pathname,fontname)
char *pathname,*fontname;
{
	int a;
	char fontsize[33],fontpath[256],*ptr;

	strcpy(fontpath,pathname);
	if ((ptr=BaseName(fontpath))) {
		strcpy(fontsize,ptr);
		*(--ptr)=0;
		if ((ptr=BaseName(fontpath))) {
			for (a=0;;a++) {
				if (!(isdigit(fontsize[a]))) break;
			}
			if (!fontsize[a]) {
				strcat(fontpath,".font");
				if (CheckExist(fontpath,NULL)) {
					strcpy(fontname,fontpath);
					return(1);
				}
			}
		}
	}
	return(0);
}

int readicon(name,np)
char *name;
int np;
{
	struct DiskObject *dobj;
	struct Gadget *gad;
	struct Image *icon_image[2];
	UWORD coltab[16];
	int fred,ret,width,height,x,y,x1,y1,imagenum,depth;

	if (!IconBase) return(0);

	name[(strlen(name)-5)]=0;
	if (!(dobj=GetDiskObject(name))) return(-2);

	gad=(struct Gadget *)&(dobj->do_Gadget);

	icon_image[0]=(struct Image *)gad->GadgetRender;
	depth=icon_image[0]->Depth;
	if (depth>4) depth=4;

	if (!icon_image[0] || !(setupfontdisplay(depth,coltab))) {
		FreeDiskObject(dobj);
		return(-3);
	}

	if (gad->Flags&GADGHIMAGE) icon_image[1]=(struct Image *)gad->SelectRender;
	else icon_image[1]=icon_image[0];

	for (x=0;x<2;x++) icon_image[x]->NextImage=NULL;

	imagenum=0;

	ScreenToFront(fontscreen);
	ActivateWindow(fontwindow);

	width=fontscreen->Width;
	height=fontscreen->Height;

	DrawImage(font_rp,icon_image[0],
		((width-icon_image[0]->Width)/2),
		((height-icon_image[0]->Height)/2));

	FadeRGB4(fontscreen,coltab,(1<<depth),1,config->fadetime);
	show_global_icon=dobj;
	show_global_icon_name=BaseName(name);

	FOREVER {
		if ((fred=WaitForMouseClick(2,fontwindow))==-2) {
			imagenum=1-imagenum;
			x=(width-icon_image[imagenum]->Width)/2;
			y=(height-icon_image[imagenum]->Height)/2;
			x1=x+icon_image[imagenum]->Width;
			y1=y+icon_image[imagenum]->Height;
			DrawImage(font_rp,icon_image[imagenum],x,y);
			drawrecaround(font_rp,0,0,x,y,x1,y1,width,height);
		}
		else {
			if (fred==0 || fred==-3) ret=TRUE;
			else ret=-1;
			break;
		}
	}
	show_global_icon=NULL;
	if (fred!=-3) FadeRGB4(fontscreen,coltab,(1<<depth),-1,config->fadetime);
/*cleanicon:*/
	FreeDiskObject(dobj);
	cleanup_fontdisplay();
	return(ret);
}

void drawrecaround(r,l,t,x,y,x1,y1,width,height)
struct RastPort *r;
int l,t,x,y,x1,y1,width,height;
{
	char o;

	o=r->FgPen; SetAPen(r,0);
	if (x>0) RectFill(r,l,t,l+x-1,t+height-1);
	if (y>0) RectFill(r,l,t,l+width-1,t+y-1);
	if (x1<l+width) RectFill(r,l+x1,t,l+width-1,t+height-1);
	if (y1<t+height) RectFill(r,l,t+y1,l+width-1,t+height-1);
	SetAPen(r,o);
}

int doplay8svx(fname,loop)
char *fname;
int loop;
{

#warning Audio not supported yet in AROS
return 0;
#if 0
	Voice8Header *pvoice8header=NULL;
	UBYTE *p8data;
	ULONG speed,size,class,rsize,chan=0;
	USHORT code;
	char *psample,*ry,*compressbuf;
	ChunkHeader *p8chunk;
	int a,b,stereo,*vxcheck,finish,playsize;
	struct IOAudio *audioptr[2];
	UBYTE *playdata[2];
	static UBYTE audiochannels[2][4]={{9,1,8,4},{6,2,4,8}};

	for (a=0;a<2;a++) {
		audio_port[a]=NULL;
		audio_req1[a]=audio_req2[a]=NULL;
	}
	audiodata=NULL;
	status_flags&=~STATUS_AUDIOLED;

	if ((a=readfile(fname,&audiodata,(int *)&audio_size))) {
		if (a==-1) return(0);
		return(-2);
	}
	vxcheck=(int *)audiodata;
	if (audio_size<12 || vxcheck[0]!=ID_FORM || vxcheck[2]!=ID_8SVX) {
		size=audio_size;
		p8data=audiodata;
		psample=p8data;
	}
	else {
		p8data=audiodata+12;
		size=0;
		while (p8data<audiodata+vxcheck[1]) {
			p8chunk=(ChunkHeader *)p8data;
			p8data+=sizeof(ChunkHeader);
			switch (p8chunk->ckID) {
				case ID_VHDR:
					pvoice8header=(Voice8Header *)p8data;
					break;
				case ID_BODY:
					psample=p8data;
					size=p8chunk->ckSize;
					break;
				case ID_CHAN:
					CopyMem((char *)p8data,(char *)&chan,4);
					break;
			}
			p8data+=p8chunk->ckSize;
			if (p8chunk->ckSize&1) ++p8data;
		}
	}

	if (size>vxcheck[1]) {
		size/=2; chan=0;
	}

	if (pvoice8header) {
		if (pvoice8header->sCompression==1) {
			size-=2;
			if (!(compressbuf=LAllocRemember(&audio_key,size*2,MEMF_CLEAR))) return(-2);
			DUnpack(psample+2,size,compressbuf,psample[1]);
			psample=compressbuf; size*=2;
		}
		speed=data_colorclock/pvoice8header->samplesPerSec;
	}
	else speed=data_colorclock/10000;

	if (chan==6) {
		size/=2;
		stereo=size;
	}
	else stereo=0;

	for (a=0;a<2;a++) {
		if (!(audio_req1[a]=LAllocRemember(&audio_key,sizeof(struct IOAudio),MEMF_CLEAR)) ||
			!(audio_req2[a]=LAllocRemember(&audio_key,sizeof(struct IOAudio),MEMF_CLEAR)) ||
			!(audio_port[a]=LCreatePort(NULL,0))) return(-2);
	}

	for (a=0;a<2;a++) {
		audio_req1[a]->ioa_Request.io_Message.mn_ReplyPort=audio_port[a];
		audio_req1[a]->ioa_Request.io_Message.mn_Node.ln_Pri=75;
		audio_req1[a]->ioa_Data=audiochannels[a];
		audio_req1[a]->ioa_Length=sizeof(audiochannels[a]);
		if (OpenDevice("audio.device",0,(struct IORequest *)audio_req1[a],0)) return(-6);
	}

	playsize=(size<25600)?size:25600;
	for (a=0;a<2;a++) {
		if (!(playdata[a]=LAllocRemember(&audio_key,playsize,MEMF_CHIP|MEMF_CLEAR)))
			return(-2);
	}

	ry=psample;
	for (a=0;a<2;a++) CopyMem(psample+(a*stereo),(char *)playdata[a],playsize);

	for (a=0;a<2;a++) {
		audio_req1[a]->ioa_Request.io_Command=CMD_WRITE;
		audio_req1[a]->ioa_Request.io_Flags=ADIOF_PERVOL;
		audio_req1[a]->ioa_Volume=64;
		audio_req1[a]->ioa_Period=(UWORD)speed;
		audio_req1[a]->ioa_Cycles=1;
		CopyMem((char *)audio_req1[a],(char *)audio_req2[a],sizeof(struct IOAudio));
		audio_req1[a]->ioa_Data=(UBYTE *)playdata[a];
		audio_req2[a]->ioa_Data=(UBYTE *)playdata[a]+12800;
		audioptr[a]=audio_req2[a];
	}
	rsize=size;

	if (config->viewbits&VIEWBITS_FILTEROFF) {
		if (filteroff()) status_flags|=STATUS_AUDIOLED;
		else status_flags&=~STATUS_AUDIOLED;
	}

	if (size<=25600) {
		FOREVER {
			for (b=0;b<2;b++) {
				audio_req1[b]->ioa_Length=size;
				BeginIO((struct IORequest *)audio_req1[b]);
			}
			a=0;
			FOREVER {
				Wait(1<<audio_port[0]->mp_SigBit|1<<audio_port[1]->mp_SigBit|1<<Window->UserPort->mp_SigBit);
				while (getintuimsg()) {
					class=IMsg->Class; code=IMsg->Code;
					ReplyMsg((struct Message *)IMsg);
					if (class==IDCMP_MOUSEBUTTONS) {
						if (code==MENUDOWN) a=-1;
						else if (code==SELECTDOWN) a=-2;
					}
					else if (class==IDCMP_RAWKEY) {
						if (code==0x45) a=-1;
						else if (code==0x10 || code==0x32) a=-2;
					}
					if (a<0) {
						if (a==-2) return(1);
						return(a);
					}
				}
				for (b=0;b<2;b++) if (GetMsg(audio_port[b])) ++a;
				if (a>=2) break;
			}
			if (!loop) return(1);
		}
	}
	else {
		finish=0;
		FOREVER {
			size=rsize;
			psample=ry;
			for (a=0;a<2;a++) {
				CopyMem(psample+(a*stereo),(char *)playdata[a],playsize);
				CopyMem((char *)audio_req1[a],(char *)audio_req2[a],sizeof(struct IOAudio));
				audio_req1[a]->ioa_Data=(UBYTE *)playdata[a];
				audio_req2[a]->ioa_Data=(UBYTE *)playdata[a]+12800;
				audio_req1[a]->ioa_Length=audio_req2[a]->ioa_Length=12800;
				audioptr[a]=audio_req2[a];
			}
			psample+=playsize;
			size-=25600;
			for (a=0;a<2;a++) BeginIO((struct IORequest *)audio_req1[a]);
			for (a=0;a<2;a++) BeginIO((struct IORequest *)audio_req2[a]);
			a=0;
			FOREVER {
				while (getintuimsg()) {
					class=IMsg->Class; code=IMsg->Code;
					ReplyMsg((struct Message *)IMsg);
					if (class==IDCMP_MOUSEBUTTONS) {
						if (code==SELECTDOWN) return(1);
						if (code==MENUDOWN) return(-1);
					}
					else if (class==IDCMP_RAWKEY) {
						if (code==0x45) return(-1);
						else if (code==0x10 || code==0x32) return(1);
					}
				}
				for (b=0;b<2;b++) {
					if (GetMsg(audio_port[b]) && !(a&(1<<b))) {
						if (size>0) {
							if (audioptr[b]==audio_req1[b]) audioptr[b]=audio_req2[b];
							else audioptr[b]=audio_req1[b];
							audioptr[b]->ioa_Length=(size<12800)?size:12800;
							CopyMem(psample+(b*stereo),(char *)audioptr[b]->ioa_Data,audioptr[b]->ioa_Length);
						}
						a|=1<<b;
					}
				}
				if (a==3) {
					if (size<=0) {
						if (finish) {
							finish=0;
							break;
						}
						finish=1;
					}
					else {
						size-=audioptr[0]->ioa_Length;
						psample+=12800;
						for (b=0;b<2;b++) BeginIO((struct IORequest *)audioptr[b]);
					}
					a=0;
				}
				Wait(1<<audio_port[0]->mp_SigBit|1<<audio_port[1]->mp_SigBit|1<<Window->UserPort->mp_SigBit);
			}
			if (!loop) return(1);
		}
	}
#endif
}

void kill8svx()
{
#warning Audio not supported yet in AROS
return;
#if 0
	int a;

	for (a=0;a<2;a++) {
		if (audio_req2[a] && audio_req2[a]->ioa_Request.io_Device) {
			audio_req2[a]->ioa_Request.io_Command=CMD_FLUSH;
			DoIO((struct IORequest *)audio_req2[a]);
		}
		if (audio_req1[a] && audio_req1[a]->ioa_Request.io_Device) {
			audio_req1[a]->ioa_Request.io_Command=CMD_FLUSH;
			DoIO((struct IORequest *)audio_req1[a]);
			CloseDevice((struct IORequest *)audio_req1[a]);
		}
		audio_req1[a]=NULL;
		audio_req2[a]=NULL;
	}
	for (a=0;a<2;a++) {
		if (audio_port[a]) {
			while (GetMsg(audio_port[a]));
			LDeletePort(audio_port[a]);
			audio_port[a]=NULL;
		}
	}
	if (audiodata && audio_size) FreeMem(audiodata,audio_size);
	LFreeRemember(&audio_key);
	audiodata=NULL;
	if (status_flags&STATUS_AUDIOLED) {
		filteron();
		status_flags&=~STATUS_AUDIOLED;
	}
#endif
}

void handle8svxerror(res)
int res;
{
	switch (res) {
		case 0: doerror(IoErr()); break;
		case -2: doerror(103); break;
		case -3:
		case -4: dostatustext(globstring[STR_ERROR_IN_IFF]); break;
		case -6: dostatustext(globstring[STR_CANT_ALLOCATE_AUDIO]); break;
	}
}

int playmod(name)
char *name;
{

#warning We lack PlayModule() from inovamusic library
return 0;
#if 0
	int a;

	if ((a=PlayModule(name,1))) {
		switch (a) {
			case ML_NOMEM: doerror(103); break;
			case ML_BADMOD:
				dostatustext(globstring[STR_NOT_ST_MOD]);
				break;
			case ML_NOMOD: doerror(205); break;
			default:
				doerror(0);
				break;
		}
		FlushModule();
		return(0);
	}
	return(1);
#endif
}

int check_is_module(name)
char *name;
{

#warning We do not have inovamusic library
return 0;
#if 0
	int a;

	if (MUSICBase) {
		a=IsModule(name);
		if (a>0 && a<100) return(1);
	}
	return(0);
#endif
}

static char codetodelta[16]={-34,-21,-13,-8,-5,-3,-2,-1,0,1,2,3,5,8,13,21};

char DUnpack(source,n,dest,x)
char *source;
int n;
char *dest,x;
{
	unsigned char d;
	int i,lim;

	lim=n<<1;
	for (i=0;i<lim;++i) {
		d=source[i>>1];
		if (i&1) d&=0xf;
		else d>>=4;
		x+=codetodelta[d];
		dest[i]=x;
	}
	return(x);
}

int EnvoyPacket(device,action,action2,data,buffer)
char *device;
ULONG action,action2;
UWORD data;
APTR buffer;
{
	struct MsgPort *handler;
	ULONG args[2];

	if (!(handler=(struct MsgPort *)DeviceProc(device))) return(-1);

	args[0]=data;
	args[1]=(ULONG)buffer;
	if (!(SendPacket(handler,action,args,2))) {
		if (action2) return(!(SendPacket(handler,action2,args,2)));
		return(1);
	}
	return(0);
}

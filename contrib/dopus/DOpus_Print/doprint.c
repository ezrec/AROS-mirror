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

#include "print.h"
#include "ppdata.h"

struct PrintHandle {
	BPTR filehandle;
	struct MsgPort *port;
	struct IOStdReq *ioreq;
	char *filename;
	int current_line;
	int current_page;
	int paper_lines;
	int paper_width;
	int paper_height;
	char buffer[1024];
	char datebuf[12];
	char fileoutput;
	struct RequesterBase *reqbase;
	struct RastPort *req_rp;
	int total_pos;
	int filesize;
	int progress_y;
	int req_width;
};

int do_header_footer(struct PrintHandle *,PrintData *,int);
int do_printstyle(struct PrintHandle *,int,int);
int print_str(struct PrintHandle *,char *,int);
int printer_command(struct RequesterBase *,struct PrintHandle *,char *);
int check_print_abort(struct RequesterBase *);
void show_progress(struct PrintHandle *);

char
	*esc_styles[]={
		"\x1b[0m",         /* STYLE_NORMAL       */
		"\x1b[1m",         /* STYLE_BOLD         */
		"\x1b[3m",         /* STYLE_ITALICS      */
		"\x1b[4m",         /* STYLE_UNDERLINED   */
		"\x1b[4\"z",       /* STYLE_DOUBLESTRIKE */
		"\x1b[6\"z"};      /* STYLE_SHADOW       */

int printfile(reqbase,filename,printdata,requester)
struct RequesterBase *reqbase;
char *filename;
PrintData *printdata;
struct Requester *requester;
{
	BPTR fileh=0;
	int a,lastspace=-1,margin,filesize,ret=0;
	int buffersize,size,pos=0,bufpos=0;
	char *buffer=NULL,*linebuffer,*marginbuf;
	struct PrintHandle *handle;
	struct DOpusRemember *memkey=NULL;
	struct DOpusDateTime *datetime;
	struct TextFont *font;

	FOREVER {
		if (CheckExist(filename,&filesize)<0) break;
		if (!(check_error(reqbase,string_table[STR_UNABLE_TO_OPEN_FILE],0)))
			return(0);
	}

	if (!(datetime=LAllocRemember(&memkey,sizeof(struct DOpusDateTime),MEMF_CLEAR)) ||
		!(handle=LAllocRemember(&memkey,sizeof(struct PrintHandle),MEMF_CLEAR))) {
		LFreeRemember(&memkey);
		return(0);
	}

	handle->paper_lines=(printdata->bottom_margin-printdata->top_margin)+1;
	handle->paper_height=handle->paper_lines;
	if (printdata->headfoot[FOOTER].headfoot_flags) handle->paper_lines-=2;
	handle->paper_width=(printdata->right_margin-printdata->left_margin)+1;
	handle->filename=BaseName(filename);
	handle->reqbase=reqbase;

	if ((margin=printdata->left_margin-1)>0) {
		if (!(marginbuf=LAllocRemember(&memkey,margin+1,MEMF_CLEAR)))
			goto ENDPRINT;
		for (a=0;a<margin;a++) marginbuf[a]=' ';
	}
	if (!(linebuffer=LAllocRemember(&memkey,(handle->paper_width*2)+4,MEMF_CLEAR)))
		goto ENDPRINT;

	if (Request(requester,reqbase->rb_window)) {
		font=reqbase->rb_window->RPort->Font;
		handle->req_rp=requester->ReqLayer->rp;
		handle->req_width=requester->Width;
		Do3DBox(handle->req_rp,2,1,
			requester->Width-4,requester->Height-2,
			reqbase->rb_shine,reqbase->rb_shadow);
		SetAPen(handle->req_rp,reqbase->rb_fg);
		SetBPen(handle->req_rp,reqbase->rb_bg);
		SetDrMd(handle->req_rp,JAM2);
		SetFont(handle->req_rp,font);
		handle->progress_y=font->tf_YSize*3;
		print_status(handle,
			(printdata->print_flags&PRINTFLAG_FILE && printdata->output_file[0])?
				string_table[STR_CREATING_FILE]:
			string_table[STR_OPENING_PRINTER],
			handle->progress_y);
	}

	if (printdata->print_flags&PRINTFLAG_FILE && printdata->output_file[0]) {
		FOREVER {
			if ((handle->filehandle=Open(printdata->output_file,
				(a=(CheckExist(printdata->output_file,NULL))?MODE_READWRITE:MODE_NEWFILE)))) {
				if (a==MODE_READWRITE) Seek(handle->filehandle,0,OFFSET_END);
				handle->fileoutput=1;
				break;
			}
			if (!(check_error(reqbase,string_table[STR_UNABLE_TO_OPEN_OUTPUT],0)))
				goto ENDPRINT;
		}
	}
	else {
		struct Preferences *prefs;

		if (!(handle->port=LCreatePort(NULL,0)) ||
			!(handle->ioreq=(struct IOStdReq *)LCreateExtIO(handle->port,sizeof(struct IOStdReq))))
			goto ENDPRINT;

		FOREVER {
			if (!(OpenDevice("printer.device",0,(struct IORequest *)handle->ioreq,0))) break;
			if (!(check_error(reqbase,string_table[STR_UNABLE_TO_OPEN_PRINTER],0))) {
				LDeleteExtIO((struct IORequest *)handle->ioreq);
				handle->ioreq=NULL;
				goto ENDPRINT;
			}
		}
		prefs=&(((struct PrinterData *)handle->ioreq->io_Device)->pd_Preferences);

		prefs->PrintLeftMargin=1;
		prefs->PrintRightMargin=printdata->right_margin;

		prefs->PaperLength=printdata->bottom_margin+2;

		if (printdata->print_pitch==PITCH_PICA) prefs->PrintPitch=PICA;
		else if (printdata->print_pitch==PITCH_FINE) prefs->PrintPitch=FINE;
		else if (printdata->print_pitch==PITCH_ELITE) prefs->PrintPitch=ELITE;

		if (printdata->text_quality==QUALITY_DRAFT) prefs->PrintQuality=DRAFT;
		else prefs->PrintQuality=LETTER;

		if (!(printer_command(reqbase,handle,"\033#1"))) goto ENDPRINT;
	}

	if (handle->req_rp) {
		SetAPen(handle->req_rp,reqbase->rb_bg);
		RectFill(handle->req_rp,2,1,
			requester->Width-3,requester->Height-2);
		print_status(handle,string_table[STR_LOADING_FILE],handle->progress_y);
	}

#ifndef __AROS__
	if (PPBase) {
		FOREVER {
			if ((a=ppLoadData(filename,
				DECR_POINTER,
				MEMF_CLEAR,
				(UBYTE **)&buffer,
				&buffersize,
				NULL))==PP_PASSERR) {
				if (!(check_error(reqbase,string_table[STR_INCORRECT_PASSWORD],0)))
					goto ENDPRINT;
			}
			else break;
		}
		size=buffersize;
		filesize=buffersize;
	}
#endif

	if (!buffer) {
		buffersize=filesize;
		while (!(buffer=AllocMem(buffersize,MEMF_CLEAR))) {
			buffersize/=2;
			if (buffersize==0) goto ENDPRINT;
		}
		if (!(fileh=Open(filename,MODE_OLDFILE))) goto ENDPRINT;
		size=Read(fileh,buffer,buffersize);
	}

	handle->filesize=filesize;

	if (handle->req_rp) {
		print_status(handle,
			string_table[STR_PRINTING_FILE],
			font->tf_YSize);
		print_status(handle,
			string_table[STR_PRESS_ESCAPE],
			(font->tf_YSize*5));
		show_progress(handle);
	}

	DateStamp(&datetime->dat_Stamp);
	datetime->dat_Format=FORMAT_DOS;
	datetime->dat_StrDate=handle->datebuf;
	StampToStr(datetime);

	FOREVER {
		if (bufpos>=size && (!fileh || (size=Read(fileh,buffer,buffersize))<1)) {
			if (pos>0) {
				if ((marginbuf && !(print_str(handle,marginbuf,-1))) ||
					!(print_str(handle,linebuffer,pos))) goto ENDPRINT;
			}
			if (handle->current_line<=handle->paper_lines) {
				if (handle->current_line==0) {
					for (a=1;a<printdata->top_margin;a++)
						if (!(print_str(handle,"\n",1))) goto ENDPRINT;
					++handle->current_page;
				}
				if (printdata->headfoot[FOOTER].headfoot_flags) {
					for (;handle->current_line<handle->paper_lines;handle->current_line++)
						if (!(print_str(handle,"\n",1))) goto ENDPRINT;
					if (!(do_header_footer(handle,printdata,FOOTER)))
						break;
				}
			}
			if (handle->current_line>0 &&
				printdata->print_flags&PRINTFLAG_EJECT &&
				!(print_str(handle,"\f",1))) break;
			break;
		}
		bufpos=0;
		while (bufpos<size) {
			if (check_print_abort(reqbase)) goto ENDPRINT;
			if (handle->current_line<handle->paper_lines) {
				if (handle->current_line==0) {
					for (a=1;a<printdata->top_margin;a++)
						if (!(print_str(handle,"\n",1))) goto ENDPRINT;
					++handle->current_page;
				}
				if (handle->current_line==0 &&
					printdata->headfoot[HEADER].headfoot_flags) {
					if (!(do_header_footer(handle,printdata,HEADER))) goto ENDPRINT;
					handle->current_line+=2;
				}
				else {
					while (bufpos<size && handle->current_line<handle->paper_lines) {
						if (check_print_abort(reqbase)) goto ENDPRINT;

						switch (buffer[bufpos]) {

							/* Tab character; pad buffer with spaces to next tab stop */
							case '\t':
								if ((pos%printdata->tab_size)==0) a=printdata->tab_size;
								else a=(((pos/printdata->tab_size)+1)*printdata->tab_size)-pos;
								while (pos<=handle->paper_width && (a--))
									linebuffer[(pos++)]=' ';
								lastspace=pos-1;
								break;

							/* Form feed */
							case '\f':
								if (pos>0) {
									if (marginbuf && !(print_str(handle,marginbuf,margin)))
										goto ENDPRINT;
									if (!(print_str(handle,linebuffer,pos))) goto ENDPRINT;
								}
								if (printdata->headfoot[FOOTER].headfoot_flags) {
									for (;handle->current_line<handle->paper_lines;handle->current_line++)
										if (!(print_str(handle,"\n",1))) goto ENDPRINT;
								}
								else {
									handle->current_line=0;
									if (!(print_str(handle,"\f",1))) goto ENDPRINT;
								}
								lastspace=-1; pos=0;
								show_progress(handle);
								break;

							/* New line */
							case '\n':

								/* Only store newline character in buffer if we are not
								   on the last line of the page */

								if (handle->current_line<handle->paper_height-1 || handle->fileoutput)
									linebuffer[pos++]='\n';

								if (marginbuf && !(print_str(handle,marginbuf,margin)))
									goto ENDPRINT;
								if (!(print_str(handle,linebuffer,pos))) goto ENDPRINT;
								lastspace=-1; pos=0; ++handle->current_line;
								show_progress(handle);
								break;

							default:
								if (isspace(buffer[bufpos])) lastspace=pos;
								else if (ispunct(buffer[bufpos])) {
									if (lastspace==-1 || pos>lastspace+8) lastspace=pos;
								}
								linebuffer[pos++]=buffer[bufpos];
								if (pos>handle->paper_width) {
									if (marginbuf && !(print_str(handle,marginbuf,margin)))
										goto ENDPRINT;
									if (lastspace>0 && lastspace<pos) {
										a=lastspace+1;
										if (!(print_str(handle,linebuffer,a-(isspace(linebuffer[lastspace])?1:0))))
											goto ENDPRINT;
										pos-=a;
										if (pos>0) CopyMem(&linebuffer[a],linebuffer,pos);
									}
									else {
										if (!(print_str(handle,linebuffer,handle->paper_width)))
											goto ENDPRINT;
										linebuffer[0]=linebuffer[handle->paper_width];
										pos=1;
									}

									if (handle->current_line<handle->paper_height-1 || handle->fileoutput) {
										if (!(print_str(handle,"\n",1))) goto ENDPRINT;
									}

									if (pos<0) {
										pos=0;
										show_progress(handle);
									}
									lastspace=-1;
									for (a=0;a<pos;a++) {
										if (isspace(linebuffer[a])) lastspace=a;
										else if (ispunct(linebuffer[a])) {
											if (lastspace==-1 || pos>lastspace+8) lastspace=a;
										}
									}
									++handle->current_line;
								}
								break;
						}
						++bufpos;
						++handle->total_pos;
					}
				}
			}
			else {
				if (!(do_header_footer(handle,printdata,FOOTER))) goto ENDPRINT;
				handle->current_line=0;
				if (!(print_str(handle,"\f",1))) goto ENDPRINT;
			}
		}
	}

	ret=1;

ENDPRINT:
	if (buffer) FreeMem(buffer,buffersize);
	if (fileh) Close(fileh);
	if (handle->filehandle) Close(handle->filehandle);
	if (handle->ioreq) {
		CloseDevice((struct IORequest *)handle->ioreq);
		LDeleteExtIO((struct IORequest *)handle->ioreq);
	}
	if (handle->port) LDeletePort(handle->port);
	if (handle->req_rp) EndRequest(requester,reqbase->rb_window);
	LFreeRemember(&memkey);
	return(ret);
}

int do_header_footer(handle,printdata,type)
struct PrintHandle *handle;
PrintData *printdata;
int type;
{
	int a,b;
	int leftoffset;
	int len;

	if (printdata->headfoot[type].headfoot_flags) {
		leftoffset=printdata->left_margin-1;

		if (!(do_printstyle(handle,printdata->headfoot[type].text_style,1)))
			return(0);

		for (a=0;a<sizeof(handle->buffer);a++) handle->buffer[a]=' ';

		if (printdata->headfoot[type].headfoot_flags&HEADFOOTFLAG_DATE)
			CopyMem(handle->datebuf,&handle->buffer[leftoffset+1],9);

		if (printdata->headfoot[type].headfoot_flags&HEADFOOTFLAG_TITLE) {
			char titlebuf[40];

			if (printdata->headfoot[type].headfoot_title[0])
				strcpy(titlebuf,printdata->headfoot[type].headfoot_title);
			else strcpy(titlebuf,handle->filename);

			if ((a=strlen(titlebuf))>handle->paper_width) {
				a=handle->paper_width;
				b=0;
			}
			else b=(handle->paper_width-a)/2;
			b+=leftoffset;
			CopyMem(titlebuf,&handle->buffer[b],a);
		}

		if (printdata->headfoot[type].headfoot_flags&HEADFOOTFLAG_PAGE) {
			char pagebuf[20];

			lsprintf(pagebuf,"%s %ld",string_table[STR_PAGE],(long int)handle->current_page);

			a=leftoffset+(handle->paper_width-strlen(pagebuf)-1);
			CopyMem(pagebuf,&handle->buffer[a],strlen(pagebuf));
		}

		if (type==FOOTER && !(print_str(handle,"\n",-1))) return(0);

		len=leftoffset+handle->paper_width;

		if (type==HEADER || handle->fileoutput) {
			handle->buffer[leftoffset+handle->paper_width]='\n';
			len+=1;
		}

		if (!(print_str(handle,handle->buffer,len))) return(0);

		if (type==HEADER && !(print_str(handle,"\n",-1))) return(0);

		if (!(do_printstyle(handle,printdata->headfoot[type].text_style,0)))
			return(0);
	}
	return(1);
}

int do_printstyle(handle,style,turnon)
struct PrintHandle *handle;
int style,turnon;
{
	int a=1;

	if (handle->ioreq) {
		if (turnon) a=print_str(handle,esc_styles[style],-1);
		else {
			if ((a=print_str(handle,esc_styles[0],-1))) {
				if (style==STYLE_DOUBLESTRIKE)
					a=print_str(handle,DOUBLESTRIKE_OFF,-1);
				else if (style==STYLE_SHADOW)
					a=print_str(handle,SHADOW_OFF,-1);
			}
		}
	}
	return(a);
}

int print_str(handle,string,stlen)
struct PrintHandle *handle;
char *string;
int stlen;
{
	int len,success,seek;

	if ((len=stlen)==-1) {
		if ((len=strlen(string))>handle->paper_width)
			len=handle->paper_width;
	}

	if (len<1) return(1);

	FOREVER {
		seek=0;
		if (handle->filehandle) {
			success=((seek=Write(handle->filehandle,string,len))==len);
		}
		else if (handle->ioreq) {
			handle->ioreq->io_Length=len;
			handle->ioreq->io_Data=(APTR)string;
			handle->ioreq->io_Command=CMD_WRITE;
			success=(!(DoIO((struct IORequest *)handle->ioreq)));
		}
		else return(0);
		if (success) return(1);
		if (!(check_error(handle->reqbase,string_table[STR_PRINT_ERROR],0))) return(0);
		if (seek>0) Seek(handle->filehandle,-seek,0);
	}
}

int printer_command(reqbase,handle,command)
struct RequesterBase *reqbase;
struct PrintHandle *handle;
char *command;
{
	handle->ioreq->io_Length=-1;
	handle->ioreq->io_Data=(APTR)command;
	handle->ioreq->io_Command=CMD_WRITE;

	FOREVER {
		if (!(DoIO((struct IORequest *)handle->ioreq))) break;
		if (!(check_error(reqbase,string_table[STR_UNABLE_TO_OPEN_PRINTER],0)))
			return(0);
	}
	return(1);
}

int check_print_abort(reqbase)
struct RequesterBase *reqbase;
{
	struct IntuiMessage *msg;
	int abort=0;

	while (msg=(struct IntuiMessage *)GetMsg(reqbase->rb_window->UserPort)) {
		if (msg->Class==IDCMP_VANILLAKEY && msg->Code==0x1b) {
			if (check_error(reqbase,string_table[STR_REALLY_ABORT],1))
				abort=1;
		}
		ReplyMsg((struct Message *)msg);
	}
	return(abort);
}

void show_progress(handle)
struct PrintHandle *handle;
{
	float percent;
	char buf[20];

	if (!handle->total_pos || !handle->filesize) percent=0;
	else percent=((float)((float)handle->total_pos/(float)handle->filesize))*100;

	lsprintf(buf,"%3ld%% %s ",(long int)percent,string_table[STR_COMPLETE]);
	print_status(handle,buf,handle->progress_y);
}

void print_status(handle,text,y)
struct PrintHandle *handle;
char *text;
int y;
{
	int x;
	struct TextFont *font;
	struct RequesterBase *reqbase;

	reqbase=handle->reqbase;
	font=handle->req_rp->Font;
	x=(handle->req_width-((strlen(text))*font->tf_XSize))/2;
	if (x>1) {
		SetAPen(handle->req_rp,reqbase->rb_bg);
		RectFill(handle->req_rp,
			2,y,
			x,y+font->tf_YSize);
	}
	SetAPen(handle->req_rp,reqbase->rb_fg);
	UScoreText(handle->req_rp,text,x,y+font->tf_Baseline,-1);
	if (handle->req_rp->cp_x<handle->req_width-2) {
		SetAPen(handle->req_rp,reqbase->rb_bg);
		RectFill(handle->req_rp,
			handle->req_rp->cp_x,y,
			handle->req_width-3,y+font->tf_YSize);
	}
	SetAPen(handle->req_rp,reqbase->rb_fg);
}

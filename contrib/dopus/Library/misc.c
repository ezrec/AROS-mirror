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

#include <proto/console.h>

/*****************************************************************************

    NAME */

	AROS_LH5(int, RawkeyToStr,

/*  SYNOPSIS */
	AROS_LHA(USHORT, code,	D0),
	AROS_LHA(USHORT, qual,	D1),
	AROS_LHA(char *, buf,	A0),
	AROS_LHA(char *, kbuf,	A1),
	AROS_LHA(int, len, 	D2),

/*  LOCATION */
	struct Library *, DOpusBase, 59, DOpus)
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

	struct InputEvent inev;
	struct MsgPort *port;
	struct IOStdReq *req;
	struct Process *myproc;
	struct ConsoleDevice *ConsoleDevice;
	char ocbuf[20],cbuf[20],*foo;

	if (buf) buf[0]=0; ocbuf[0]=cbuf[0]=0;
	if (code!=~0 && code!=0xff) {
		if (code&IECODE_UP_PREFIX) code-=0x80;
		if (code>=0x50 && code<=0x59) LSprintf(cbuf,"F%ld",code-0x4f);
		else if (code!=~0 && code!=0xff) {
			foo=NULL;
			switch (code) {
				case 0x45: foo="ESCAPE"; break;
				case 0x46: foo="DELETE"; break;
				case 0x41: foo="BACKSPACE"; break;
				case 0x42: foo="TAB"; break;
				case 0x44:
				case 0x2b: foo="RETURN"; break;
				case 0x5f: foo="HELP"; break;
				case 0x60:
				case 0x4c: foo="CURSOR-UP"; break;
				case 0x4d: foo="CURSOR-DOWN"; break;
				case 0x4f: foo="CURSOR-LEFT"; break;
				case 0x4e: foo="CURSOR-RIGHT"; break;
				case 0x43: foo="ENTER"; break;
				case 0x40: foo="SPACE"; break;
			}
			if (foo) strcpy(cbuf,foo);
			if (!(myproc=(struct Process *)FindTask(NULL))) return(0);
			port=&myproc->pr_MsgPort;
			if (!(req=(struct IOStdReq *)LCreateExtIO(port,sizeof(struct IOStdReq)))) return(0);
			if (OpenDevice("console.device",-1,(struct IORequest *)req,0)) {
				LDeleteExtIO((struct IORequest *)req);
				return(0);
			}
			ConsoleDevice=(struct ConsoleDevice *)req->io_Device;
			inev.ie_NextEvent=NULL;
			inev.ie_Class=IECLASS_RAWKEY;
			inev.ie_SubClass=NULL;
			inev.ie_Code=code;
			inev.ie_Qualifier=qual&(IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT);
			inev.ie_EventAddress=NULL;
			ocbuf[0]=0;
			RawKeyConvert(&inev,ocbuf,2,NULL);
			ocbuf[1]=0;
			CloseDevice((struct IORequest *)req);
			LDeleteExtIO((struct IORequest *)req);
			if (kbuf) kbuf[0]=ocbuf[0];
			if (!foo) strcpy(cbuf,ocbuf);
		}
	}
	if (buf) {
		if (qual&IEQUALIFIER_LCOMMAND) StrConcat(buf,"LAMIGA + ",len);
		if (qual&IEQUALIFIER_RCOMMAND) StrConcat(buf,"RAMIGA + ",len);
		if (qual&IEQUALIFIER_CONTROL) StrConcat(buf,"CONTROL + ",len);
		if (qual&IEQUALIFIER_LSHIFT) StrConcat(buf,"LSHIFT + ",len);
		if (qual&IEQUALIFIER_RSHIFT) StrConcat(buf,"RSHIFT + ",len);
		if (qual&IEQUALIFIER_LALT) StrConcat(buf,"LALT + ",len);
		if (qual&IEQUALIFIER_RALT) StrConcat(buf,"RALT + ",len);
		if ((code==~0 || code==0xff || code==0) && buf[0]) buf[strlen(buf)-3]=0;
		else if (cbuf[0]) {
			StrToUpper(cbuf,ocbuf);
			StrConcat(buf,"'",len);
			StrConcat(buf,ocbuf,len);
			StrConcat(buf,"'",len);
		}
	}
	return(1);
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH4(int, CheckNumGad,

/*  SYNOPSIS */
	AROS_LHA(struct Gadget *, gad,	A0),
	AROS_LHA(struct Window *, win,	A1),
	AROS_LHA(int, min, 	D0),
	AROS_LHA(int, max, 	D1),

/*  LOCATION */
	struct Library *, DOpusBase, 64, DOpus)
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

	struct StringInfo *sinfo;
	int a,b;

	sinfo=(struct StringInfo *)gad->SpecialInfo;
	a=b=atoi((char *)sinfo->Buffer);
	if (a<min) a=min;
	else if (a>max) a=max;
	LSprintf((char *)sinfo->Buffer,"%ld",a);
	if (a!=b) {
		if (win) RefreshStrGad(gad,win);
		return(1);
	}
	return(0);
	
	AROS_LIBFUNC_EXIT
}


/*****************************************************************************

    NAME */

	AROS_LH4(int, CheckHexGad,

/*  SYNOPSIS */
	AROS_LHA(struct Gadget *, gad,	A0),
	AROS_LHA(struct Window *, win,	A1),
	AROS_LHA(int, min, 	D0),
	AROS_LHA(int, max, 	D1),

/*  LOCATION */
	struct Library *, DOpusBase, 65, DOpus)
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

	struct StringInfo *sinfo;
	int a,b;

	sinfo=(struct StringInfo *)gad->SpecialInfo;
	b=Atoh((char *)sinfo->Buffer,0);
	if (sinfo->Buffer[0]==0) a=0;
	else {
		a=b;
		if (a<min) a=min;
		else if (a>max) a=max;
	}
	LSprintf((char *)sinfo->Buffer,"%lx",a);
	if (a!=b) {
		if (win) RefreshStrGad(gad,win);
		return(1);
	}
	return(0);
	
	AROS_LIBFUNC_EXIT
}

/*****************************************************************************

    NAME */

	AROS_LH3(void, Decode_RLE,

/*  SYNOPSIS */
	AROS_LHA(char *, source,	A0),
	AROS_LHA(char *, dest,		A1),
	AROS_LHA(int, size, 	D0),

/*  LOCATION */
	struct Library *, DOpusBase, 94, DOpus)
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

	register int a;
	register char copy,count;

	for (a=0;a<size;) {
		if ((count=source[a++])>=0) {
			copy=count+1;
			while (copy--) *dest++=source[a++];
		}
		else if (count!=-128) {
			copy=1-count;
			while (copy--) *dest++=source[a];
			++a;
		}
	}
	
	AROS_LIBFUNC_EXIT
}

static struct TagItem
	busytags[2]={
		{WA_BusyPointer,1},
		{TAG_DONE,0}};

/* AROS: No  __chip keyword */
extern USHORT /* __chip  */ busydata13[];
extern USHORT /* __chip  */ busydata20[];

/*****************************************************************************

    NAME */

	AROS_LH1(void, SetBusyPointer,

/*  SYNOPSIS */
	AROS_LHA(struct Window *, wind,	A0),

/*  LOCATION */
	struct Library *, DOpusBase, 50, DOpus)
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

	if (IntuitionBase->LibNode.lib_Version>38)
		SetWindowPointerA(wind,busytags);
	else if (IntuitionBase->LibNode.lib_Version<36)
		SetPointer(wind,busydata13,22,16,-6,0);
	else SetPointer(wind,busydata20,16,16,-6,0);
	
	AROS_LIBFUNC_EXIT
}

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

int __saveds DoRawkeyToStr(register UWORD code __asm("d0"),
    register UWORD qual __asm("d1"),
    register char *buf __asm("a0"),
    register char *kbuf __asm("a1"),
    register int len __asm("d2"))
{
    struct InputEvent inev;
    struct MsgPort *port;
    struct IOStdReq *req;
    struct Process *myproc;
    struct ConsoleDevice *ConsoleDevice;
    char ocbuf[20],cbuf[20],*foo;

    if (buf) buf[0]=0;
    ocbuf[0]=cbuf[0]=0;
    if (code!=(UWORD)~0 && code!=0xff) {
        if (code&IECODE_UP_PREFIX) code-=0x80;
        if (code>=0x50 && code<=0x59) LSprintf(cbuf,"F%ld",code-0x4f);
        else if (code!=(UWORD)~0 && code!=0xff) {
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
            if (foo) LStrCpy(cbuf,foo);
            if (!(myproc=(struct Process *)FindTask(NULL))) return(0);
            port=&myproc->pr_MsgPort;
            if (!(req=(struct IOStdReq *)CreateExtIO(port,sizeof(struct IOStdReq)))) return(0);
            if (OpenDevice("console.device",-1,(struct IORequest *)req,0)) {
                DeleteExtIO((struct IORequest *)req);
                return(0);
            }
            ConsoleDevice=(struct ConsoleDevice *)req->io_Device;
            inev.ie_NextEvent=NULL;
            inev.ie_Class=IECLASS_RAWKEY;
            inev.ie_SubClass=0;
            inev.ie_Code=code;
            inev.ie_Qualifier=qual&(IEQUALIFIER_LSHIFT|IEQUALIFIER_RSHIFT);
            inev.ie_EventAddress=NULL;
            ocbuf[0]=0;
            RawKeyConvert(&inev,ocbuf,2,NULL);
            ocbuf[1]=0;
            CloseDevice((struct IORequest *)req);
            DeleteExtIO((struct IORequest *)req);
            if (kbuf) kbuf[0]=ocbuf[0];
            if (!foo) LStrCpy(cbuf,ocbuf);
        }
    }
    if (buf) {
        if (qual&IEQUALIFIER_LCOMMAND) DoStrConcat(buf,"LAMIGA + ",len);
        if (qual&IEQUALIFIER_RCOMMAND) DoStrConcat(buf,"RAMIGA + ",len);
        if (qual&IEQUALIFIER_CONTROL) DoStrConcat(buf,"CONTROL + ",len);
        if (qual&IEQUALIFIER_LSHIFT) DoStrConcat(buf,"LSHIFT + ",len);
        if (qual&IEQUALIFIER_RSHIFT) DoStrConcat(buf,"RSHIFT + ",len);
        if (qual&IEQUALIFIER_LALT) DoStrConcat(buf,"LALT + ",len);
        if (qual&IEQUALIFIER_RALT) DoStrConcat(buf,"RALT + ",len);
        if ((code==(UWORD)~0 || code==0xff/* || code==0*/) && buf[0]) buf[strlen(buf)-3]=0;
        else if (cbuf[0]) {
            StrToUpper(cbuf,ocbuf);
            DoStrConcat(buf,"'",len);
            DoStrConcat(buf,ocbuf,len);
            DoStrConcat(buf,"'",len);
        }
    }
    return(1);
}

int __saveds DoCheckNumGad(register struct Gadget *gad __asm("a0"),
    register struct Window *win __asm("a1"),
    register int min __asm("d0"),
    register int max __asm("d1"))
{
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
}

int __saveds DoCheckHexGad(register struct Gadget *gad __asm("a0"),
    register struct Window *win __asm("a1"),
    register int min __asm("d0"),
    register int max __asm("d1"))
{
    struct StringInfo *sinfo;
    int a,b;

    sinfo=(struct StringInfo *)gad->SpecialInfo;
    b=DoAtoh((char *)sinfo->Buffer,0);
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
}

void __saveds DoDecode_RLE(register char *source __asm("a0"),
    register char *dest __asm("a1"),
    register int size __asm("d0"))
{
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
}

const static struct TagItem
    busytags[2]={
        {WA_BusyPointer,1},
        {TAG_DONE,0}};

//extern UWORD __chip busydata13[];
//extern UWORD __chip busydata20[];

void __saveds DoSetBusyPointer(register struct Window *wind __asm("a0"))
{
//    if (IntuitionBase->LibNode.lib_Version>38)
        SetWindowPointerA(wind,busytags);
//    else if (IntuitionBase->LibNode.lib_Version<36)
//        SetPointer(wind,busydata13,22,16,-6,0);
//    else SetPointer(wind,busydata20,16,16,-6,0);
}

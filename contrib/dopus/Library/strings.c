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

__saveds int DoStrCombine(register char *buf __asm("a0"),
    register char *one __asm("a1"),
    register char *two __asm("a2"),
    register int lim __asm("d0"))
{
    register int a;

    a=strlen(one); if (a>=lim) a=lim-1;
    LStrnCpy(buf,one,a); buf[a]=0;
    return(DoStrConcat(buf,two,lim));
}

__saveds int DoStrConcat(register char *buf __asm("a0"),
    register char *cat __asm("a1"),
    register int lim __asm("d0"))
{
    register int a,b;

    a=strlen(cat); b=strlen(buf);
    --lim;
    if (a+b<lim) {
        LStrnCpy(&buf[b],cat,a); buf[b+a]=0;
        return(1);
    }
    if (lim>b) LStrnCpy(&buf[b],cat,lim-b); buf[lim]=0;
    return(0);
}

__saveds int DoAtoh(register unsigned char *buf __asm("a0"),
    register int len __asm("d0"))
{
    int a,c,d,e,f;

    c=e=0;
    for (a=0;;a++) {
        if (!buf[a] || !((buf[a]>='0' && buf[a]<='9') || (buf[a]>='a' && buf[a]<='f') ||
            (buf[a]>='A' && buf[a]<='F'))) break;
        if ((++e)==len) break;
    }
    if (e==0) return(0);
    f=1;
    for (a=1;a<e;a++) f*=16;
    for (a=0;a<e;a++) {
        if (buf[a]>='0' && buf[a]<='9') d=buf[a]-'0';
        else if (buf[a]>='A' && buf[a]<='F') d=10+(buf[a]-'A');
        else if (buf[a]>='a' && buf[a]<='f') d=10+(buf[a]-'a');
        c+=(d*f);
        f/=16;
    }
    return(c);
}

/*
; Directory Opus 4
; Original GPL release version 4.12
; Copyright 1993-2000 Jonathan Potter
; 
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
; 
; All users of Directory Opus 4 (including versions distributed
; under the GPL) are entitled to upgrade to the latest version of
; Directory Opus version 5 at a reduced price. Please see
; http://www.gpsoft.com.au for more information.
; 
; The release of Directory Opus 4 under the GPL in NO WAY affects
; the existing commercial status of Directory Opus 5.
*/
#define PROTO_DOPUS_H
#include "dopuslib.h"

extern ULONG RangeSeed;

void __saveds GetWBScreen(register struct Screen *screen __asm("a0"))
{
    struct Screen *scr;

D(bug("GetWBScreen()\n");/*Delay(50);*/)
    if ((scr = LockPubScreen(NULL/*"Workbench"*/)))
     {
      CopyMem(scr,screen,sizeof(struct Screen));
      UnlockPubScreen(NULL,scr);
     }
}

void __saveds Seed(register int seed __asm("d0"))
{
D(bug("Seed()\n");/*Delay(50);*/)
    RangeSeed = FastRand(seed);
}

UWORD /*__saveds*/ Random(register UWORD limit __asm("d0"))
{
D(bug("Random()\n");/*Delay(50);*/)
    return RangeRand(limit+1);
}

void /*__saveds*/ BtoCStr(register BSTR bstr __asm("a0"), register char *cstr __asm("a1"), register int len __asm("d0"))
{
#ifndef __AROS__
    char *c = (char *)BADDR(bstr);
    short i = 0;

//D(bug("BtoCStr()\n"));
    len--;
    i = *((UBYTE *)c);
    c++;
    if (i < len) len = i;
    for(i=0;i<len;i++) *(cstr++)=*(c++);
    *cstr=0;
#else
    short l;

    l = AROS_BSTR_strlen(bstr);
    if (l + 1 > len)
        l = len - 1;
    CopyMem(AROS_BSTR_ADDR(bstr), cstr, l);
    cstr[l] = 0;
#endif
}

void SwapMem(register char *src __asm("a0"), register char *dst __asm("a1"), register int size __asm("d0"))
{
    register char tmp;

//D(bug("SwapMem()\n");/*Delay(50);*/)
    for(;size;size--)
     {
      tmp = *dst;
      *dst++ = *src;
      *src++ = tmp;
     }
}

#ifndef __AROS__
void stuffChar(register char c __asm("d0"), register char **buf __asm("a3"))
{
//asm("moveb %d0,%a3@+");
    *(*buf) ++ = c;
}
#endif

#include <stdarg.h>
#ifdef __AROS__
#include <exec/rawfmt.h>
#else
#include <clib/alib_stdio_protos.h>
#endif

void __saveds __stdargs LSprintf(char *buf, char *fmt, ...)
 {
  va_list args;

//D(bug("LSprintf()\n");/*Delay(50);*/)
  va_start(args,fmt);
//    movem.l a2/a3/a6,-(sp)
//    move.l 4*4(sp),a3
//    move.l 5*4(sp),a0
//    lea.l 6*4(sp),a1
//    lea.l stuffChar(pc),a2
//    move.l 4,a6
//    jsr _LVORawDoFmt(a6)
#ifdef __AROS__
  VNewRawDoFmt(fmt,RAWFMTFUNC_STRING,buf,args);
#else
  RawDoFmt(fmt,args,stuffChar,&buf2);
#endif
//  sprintf(buf,fmt,args);
  va_end(args);
//    movem.l (sp)+,a2/a3/a6
//    rts
 }

unsigned char __saveds LToUpper(register unsigned char c __asm("d0"))
{
//D(bug("LToUpper()\n");/*Delay(50);*/)
    return ToUpper(c);
}

unsigned char __saveds LToLower(register unsigned char c __asm("d0"))
{
//D(bug("LToLower()\n");/*Delay(50);*/)
    return ToLower(c);
}

void __saveds StrToUpper(register unsigned char *src __asm("a0"), register unsigned char *dst __asm("a1"))
{
//D(bug("LStrToUpper()\n");/*Delay(50);*/)
    while((*dst++ = ToUpper(*src++)));
}

void __saveds StrToLower(register unsigned char *src __asm("a0"), register unsigned char *dst __asm("a1"))
{
//D(bug("LStrToLower()\n");/*Delay(50);*/)
    while((*dst++ = ToLower(*src++)));
}

void /*__saveds*/ LStrnCat(register char *s1 __asm("a0"), register char *s2 __asm("a1"), register int len __asm("d0"))

{
D(bug("LStrnCat()\n");/*Delay(50);*/)
    while (*s1++);
    for(s1--;len;len--)
     {
      *s1++ = *s2++;
      if (!(*s2)) break;
     }
    *s1=0;
}

void /*__saveds*/ LStrCat(register char *s1 __asm("a0"), register char *s2 __asm("a1"))

{
D(bug("LStrCat()\n");/*Delay(50);*/)
    LStrnCat(s1,s2,0xffff);
}

void /*__saveds*/ LStrCpy(register char *dst __asm("a0"), register const char *src __asm("a1"))

{
//char *d=dst,*s=src;
    while((*dst++ = *src++));
//D(bug("LStrCpy(%s,%s)\n",d,s);Delay(25);)
//    *dst=0;

//strcpy(dst,src);
}

void /*__saveds*/ LStrnCpy(register char *dst __asm("a0"), register const char *src __asm("a1"), register int len __asm("d0"))

{
    char c;

//D(bug("LStrnCpy()\n");Delay(50);)
    while (len-- >= 0)
     {
      c = *src++;

      *dst++ = c;
      if (!c) break;
     }
    *(--dst) = 0;
//strncpy(dst,src,len);
}

int __saveds LStrnCmpI(register char *s1 __asm("a0"), register char *s2 __asm("a1"), register int len __asm("d0"))

{
    register char c1,c2,diff;
//D(bug("LStrnCmpI()\n");/*Delay(50);*/)

    while(1)
     {
      c1 = ToUpper(*s1++);
      c2 = ToUpper(*s2++);

      diff = c1-c2;

      if (diff>0) return 1;
      else if (diff<0) return -1;
      else if ((c1 ==0) || (len == 0)) break;
      len--;
     }
    return 0;
}

int /*__saveds*/ LStrCmpI(register char *s1 __asm("a0"), register char *s2 __asm("a1"))

{
//D(bug("LStrCmpI()\n");/*Delay(50);*/)
    return LStrnCmpI(s1,s2,0x7fff);
}

int /*__saveds*/ LStrnCmp(register char *s1 __asm("a0"), register char *s2 __asm("a1"), register int len __asm("d0"))

{
    register char c1,c2,diff;

//D(bug("LStrnCmp()\n");/*Delay(50);*/)
    while(1)
     {
      c1 = *s1++;
      c2 = *s2++;

      diff = c1-c2;

      if (diff>0) return 1;
      else if (diff<0) return -1;
      else if ((c1 ==0) || (len == 0)) break;
      len--;
     }
    return 0;
}

int /*__saveds*/ LStrCmp(register char *s1 __asm("a0"),
            register char *s2 __asm("a1"))

{
//D(bug("LStrCmp()\n");/*Delay(50);*/)
    return LStrnCmp(s1,s2,0x7fff);
}

int atoi(const char *str)
{
    LONG i;

//D(bug("atoi()\n");Delay(50);)
    StrToLong(str,&i);

    return i;
}

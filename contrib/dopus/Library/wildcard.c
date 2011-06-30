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

#define PROTO_DOPUS_H
#include "dopuslib.h"
//#include <proto/dos.h>

/*#define upper(c) ((c>='a' && c<='z')?(c-32):c)

struct MatchData {
    unsigned char *str,*pat;
    int levels,cas,flags;
};
*/

int DoMatchPattern(register unsigned char *pat __asm("a0"), register unsigned char *str __asm("a1"), register int cas __asm("d0"));
void DoParsePattern(register unsigned char *pat __asm("a0"), register unsigned char *patbuf __asm("a1"), register int cas __asm("d0"));
//extern int WildMatch(struct MatchData *),findendor(struct MatchData *,int);
//extern struct DOpusBase *DOpusBase;

void /*__saveds*/ LParsePattern(register unsigned char *pat __asm("a0"),register unsigned char *patbuf __asm("a1"))
{
//D(bug("LParsePattern()\n");Delay(50));
    DoParsePattern(pat,patbuf,0);
}

void /*__saveds*/ LParsePatternI(register unsigned char *pat __asm("a0"),register unsigned char *patbuf __asm("a1"))
{
//D(bug("LParsePatternI()\n");Delay(50));
    DoParsePattern(pat,patbuf,1);
}

int /*__saveds*/ LMatchPattern(register unsigned char *pat __asm("a0"), register unsigned char *str __asm("a1"))
{
//D(bug("LMatchPattern()\n");Delay(50));
  return DoMatchPattern(pat,str,0);
}
int /*__saveds*/ LMatchPatternI(register unsigned char *pat __asm("a0"), register unsigned char *str __asm("a1"))
{
//D(bug("LMatchPatternI()\n");Delay(50));
  return DoMatchPattern(pat,str,1);
}

int __saveds DoMatchPattern(register unsigned char *pat __asm("a0"), register unsigned char *str __asm("a1"), register int cas __asm("d0"))
{
//    if (DOSBase->dl_lib.lib_Version>35) {
        int suc,old;

        old=DOSBase->dl_Root->rn_Flags&RNF_WILDSTAR;
        DOSBase->dl_Root->rn_Flags|=RNF_WILDSTAR;
        suc = (cas ? MatchPatternNoCase(pat,str) : MatchPattern(pat,str));
        if (!old) DOSBase->dl_Root->rn_Flags&=~RNF_WILDSTAR;
        return(suc);
/*
    }
    else {
        struct MatchData mdata;

        mdata.str=str;
        mdata.pat=pat;
        mdata.levels=mdata.flags=0;
        mdata.cas=cas;
        return(WildMatch(&mdata));
    }
*/
}

void __saveds DoParsePattern(register unsigned char *pat __asm("a0"), register unsigned char *patbuf __asm("a1"), register int cas __asm("d0"))
{
//    if (DOSBase->dl_lib.lib_Version>35) {
        int len,old;

        old=DOSBase->dl_Root->rn_Flags&RNF_WILDSTAR;
        DOSBase->dl_Root->rn_Flags|=RNF_WILDSTAR;
        len=(strlen(pat)*2)+2;

    (cas?ParsePatternNoCase(pat,patbuf,len):ParsePattern(pat,patbuf,len));

        if (cas) ParsePatternNoCase(pat,patbuf,len);
        else ParsePattern(pat,patbuf,len);
        if (!old) DOSBase->dl_Root->rn_Flags&=~RNF_WILDSTAR;
/*
    }
    else {
        int or=0,class=0,rep=0;

        while (*pat) {
            switch (*pat) {
                case '*':
                    if (rep) {
                        *patbuf++=P_REPEND;
                        rep=0;
                    }
                    *patbuf++=P_ANY;
                    break;
                case '?':
                    if (rep) {
                        *patbuf++=P_REPEND;
                        rep=0;
                    }
                    *patbuf++=P_SINGLE;
                    break;
                case '(':
                    if (rep) rep=0;
                    *patbuf++=P_ORSTART;
                    ++or;
                    break;
                case '|':
                    if (rep) rep=0;
                    if (or) *patbuf++=P_ORNEXT;
                    else *patbuf++='|';
                    break;
                case ')':
                    if (rep) {
                        *patbuf++=P_REPEND;
                        rep=0;
                    }
                    if (or) {
                        *patbuf++=P_OREND;
                        --or;
                    }
                    else *patbuf++=')';
                    break;
                case '~':
                    if (rep) {
                        *patbuf++=P_REPEND;
                        rep=0;
                    }
                    *patbuf++=P_NOT;
                    break;
                case '[':
                    if (class) *patbuf++='[';
                    else {
                        *patbuf++=P_CLASS;
                        class=1;
                    }
                    break;
                case ']':
                    if (class) {
                        *patbuf++=P_CLASS;
                        class=0;
                    }
                    else *patbuf++=']';
                    if (rep) {
                        *patbuf++=P_REPEND;
                        rep=0;
                    }
                    break;
                case '#':
                    if (*(pat+1)=='?' || *(pat+1)=='*') {
                        *patbuf++=P_ANY;
                        ++pat;
                    }
                    else {
                        *patbuf++=P_REPBEG;
                        rep=1;
                    }
                    break;
                case '%':
                    break;
                case '\'':
                    ++pat;
                default:
                    *patbuf++=(cas)?upper(*pat):*pat;
                    if (rep && !class) {
                        *patbuf++=P_REPEND;
                        rep=0;
                    }
                    break;
            }
            ++pat;
        }
        *patbuf=0;
    }
*/
}
/*
WildMatch(mdata)
struct MatchData *mdata;
{
    int last,match,not=0,lnot,other=0,wild=0;
    unsigned char ch,*str,mc,*pat=NULL;

    if (!(mdata->flags&2)) {
        for (last=0;;last++) {
            if (mdata->pat[last]>0x7f) {
                if (mdata->pat[last]!=P_ANY && mdata->pat[last]!=P_NOT &&
                    mdata->pat[last]!=P_SINGLE) other=1;
                wild=1;
            }
            else if (!mdata->pat[last]) {
                if (wild) break;
                if (mdata->cas) return((LStrCmpI((char *)mdata->pat,(char *)mdata->str)==0));
                return((strcmp((char *)mdata->pat,(char *)mdata->str)==0));
            }
        }
        if (!other) {
            match=1; lnot=0;
            FOREVER {
                if (*(mdata->pat)==P_NOT) {
                    if (!(*++mdata->pat)) break;
                    not=1-not; lnot=0;
                }
                else if (*(mdata->pat)==P_ANY) {
                    if (!(*++mdata->pat)) {
                        if (not) match=1-match;
                        return(match);
                    }
                    pat=mdata->pat; match=lnot=1;
                }
                else if (*(mdata->pat)==P_SINGLE) {
                    ++mdata->str;
                    match=1;
                    if (!(*++mdata->pat)) {
                        if (*mdata->str && pat) {
                            mdata->pat=pat;
                            continue;
                        }
                        break;
                    }
                }
                else {
                    mc=mdata->cas?upper(*(mdata->str)):*(mdata->str);
                    if (*mdata->str) ++mdata->str;
                    if (*(mdata->pat)!=mc) {
                        match=0;
                        if (!lnot) break;
                        if (pat && *(mdata->str)) {
                            mdata->pat=pat;
                            if (*(mdata->str-1)==*pat) --mdata->str;
                        }
                        else if (!(*++mdata->pat)) break;
                    }
                    else {
                        match=1;
                        if (!(*++mdata->pat)) {
                            if (*mdata->str && pat) {
                                mdata->pat=pat;
                                continue;
                            }
                            break;
                        }
                    }
                }
            }
            if (match && !(*(mdata->str))) return(1-not);
            return(not);
        }
        mdata->flags|=2;
    }

    for (;*(mdata->pat);mdata->pat++) {
doswitch:
        switch (*(mdata->pat)) {
            case P_NOT:
                not=1-not;
                continue;
            case P_REPBEG:
                ++mdata->pat;
                mc=mdata->cas?upper(*(mdata->str)):*(mdata->str);
                if ((ch=*(mdata->pat))==P_CLASS) {
                    for (last=256,match=0;*(++mdata->pat) && *(mdata->pat)!=P_CLASS;last=*(mdata->pat)) {
                        if (((*mdata->pat)=='-')?(mc<=*(++mdata->pat) &&
                            mc>=last):(mc==*(mdata->pat))) {
                            match=1; ch=*(mdata->pat);
                        }
                    }
                    if (!match) continue;
                }
                else if (ch>0x7f) goto doswitch;
                if (mdata->cas) ch=upper(ch);
                while (mc==ch) ++mdata->str;
            case P_REPEND:
                continue;
            case P_SINGLE:
                if (!(*(mdata->str))) return(not);
                ++mdata->str;
                continue;
            case P_ORSTART:
                str=mdata->str; match=0;
                FOREVER {
                    ++mdata->pat; mdata->str=str;
                    ++mdata->levels;
                    if (WildMatch(mdata)) {
                        --mdata->levels;
                        match=1;
                        break;
                    }
                    --mdata->levels;
                    findendor(mdata,1);
                    if (*(mdata->pat)!=P_ORNEXT) break;
                }
                if (!match) {
                    if (!mdata->levels) return(not);
                    return(0);
                }
                findendor(mdata,0);
                continue;
            case P_ORNEXT:
            case P_OREND:
                if (mdata->levels) {
                    --mdata->pat;
                    return(1);
                }
                continue;
            case P_ANY:
                match=1;
                if (*(++mdata->pat)) {
                    pat=mdata->pat;
                    while (!WildMatch(mdata)) {
                        if (!(*(++mdata->str))) {
                            match=0;
                            break;
                        }
                        mdata->pat=pat;
                    }
                }
                if (not) return(1-match);
                return(match);
            case P_CLASS:
                pat=mdata->pat;
                if (*(mdata->pat+1)=='~' || *(mdata->pat+1)=='^') {
                    lnot=1;
                    ++mdata->pat;
                }
                else lnot=0;
                mc=mdata->cas?upper(*(mdata->str)):*(mdata->str);
                for (last=256,match=0;*(++mdata->pat) && *(mdata->pat)!=P_CLASS;last=*(mdata->pat))
                    if (((*mdata->pat)=='-')?(mc<=*(++mdata->pat) &&
                        mc>=last):(mc==*(mdata->pat))) match=1-lnot;
                if (match==not) {
                    mdata->pat=pat;
                    return(0);
                }
                ++mdata->str;
                continue;
            default:
                mc=mdata->cas?upper(*(mdata->str)):*(mdata->str);
                if (mc!=*(mdata->pat)) return(not);
                ++mdata->str;
                continue;
        }
    }
    if (!(*(mdata->str))) return(1-not);
    return(not);
}

findendor(mdata,a)
struct MatchData *mdata;
int a;
{
    int lev=1;

    if (*(mdata->pat)==P_OREND) ++mdata->pat;
    while (*(mdata->pat)) {
        if (*(mdata->pat)==P_ORNEXT && a && lev==1) return(0);
        if (*(mdata->pat)==P_ORSTART) ++lev;
        else if (*(mdata->pat)==P_OREND) {
            --lev;
            if (lev==0) return(0);
        }
        ++mdata->pat;
    }
}
*/

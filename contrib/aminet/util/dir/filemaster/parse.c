/*
     Filemaster - Multitasking directory utility.
     Copyright (C) 2000  Toni Wilen
     
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
     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <exec/types.h>
#include <exec/memory.h>
#include <strings.h>
#include <stdarg.h>
#include <dos/dos.h>

#include "fmnode.h"
#include "fmlocale.h"
#include "fmgui.h"

extern struct UtilityBase *UtilityBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;

void offgadget(struct Gadget*,WORD);
void ongadget(struct Gadget*,WORD);

extern struct FMMain fmmain;
extern struct FMConfig *fmconfig;

WORD checkmatch(UBYTE *token1,UBYTE *token2,UBYTE *token3,struct FMNode *node,struct Match *match,WORD matchtype)
{
WORD newnode=0,matched=0,ret=0;
UBYTE apuflags[16];

if(match->mcase) {
	if(match->mcase==2) {
		if(MatchPattern(token2,NDFILE(node))) newnode=1;
	} else {
		if(MatchPatternNoCase(token1,NDFILE(node))) newnode=1;
	}
}
if(match->mflags) {
	siirran(apuflags,NDPROT(node),fmmain.protlen);
	apuflags[fmmain.protlen]=0;
	if(MatchPatternNoCase(token3,apuflags)) newnode=1;
}
if(match->msize&&(node->flags&NFILE)) {
	switch(match->msize)
	{
	case 1: /* same */
	if(node->numlen==match->patsize) newnode=1;
	break;
	case 2: /* notsame */
	if(node->numlen!=match->patsize) newnode=1;
	break;
	case 3: /* smaller */
	if(node->numlen<match->patsize) newnode=1;
	break;
	case 4: /* larger */
	if(node->numlen>match->patsize) newnode=1;
	break;
	case 5: /* between */
	if(node->numlen>=match->patsize&&node->numlen<=match->patsize2) newnode=1;
	break;
	case 6: /* notbetween */
	if(node->numlen<match->patsize||node->numlen>match->patsize2) newnode=1;
	break;
	}
}
if(match->mdate) {
	switch(match->mdate)
	{
	case 1: /* older */
	if(node->numdate<match->patdate) newnode=1;
	break;
	case 2: /* newer */
	if(node->numdate>match->patdate) newnode=1;
	break;
	case 3: /* between */
	if(node->numdate>=match->patdate&&node->numdate<=match->patdate) newnode=1;
	break;
	case 4: /* notbetween */
	if(node->numdate<match->patdate||node->numdate>match->patdate) newnode=1;
	break;
	}
}
if(node->flags&NFILE&&(match->mfiles==1||(match->mfiles==0&&newnode))) matched=1;
if(node->flags&NDIRECTORY&&(match->mdirs==1||(match->mdirs==0&&newnode))) matched=1;

switch(matchtype)
{
case 0:
node->flags&=~NMATCHED;
if((node->flags&(NDEVICE|NASSIGN|NBUFFER))||matched) {
	node->flags|=NMATCHED;
	ret=1;
}
break;
case 10:
if(matched) {
	node->flags|=NSELECTED;
	ret=1;
}
break;
case 20:
if(matched) {
	node->flags&=~NSELECTED;
	ret=1;
}
break;
}
if(!(node->flags&NMATCHED)) node->flags&=~NSELECTED;
return(ret);
}

void parselist(struct FMList *list,WORD matchtype)
{
struct FMNode *node;
UBYTE *token1,*token2,*token3;
UBYTE *ptr1,*ptr2;
struct Match *match = NULL;
UBYTE patname[PATTERN];
UBYTE patflags[32];

if(!list->li) return;
token1=allocvec(list,3000,0);
if(!token1) goto parseend;
token2=token1+1000;
token3=token1+1000;
if(matchtype<10) match=getconfignumber(PARSECONFIG)->moreconfig;
if(matchtype>=10&&matchtype<19) match=getconfignumber(INCLUDECONFIG)->moreconfig;
if(matchtype>=20) match=getconfignumber(EXCLUDECONFIG)->moreconfig;

if(fmmain.kick<39&&match->mcase) {
	ptr1=match->patname;
	ptr2=patname;
	while(*ptr1) *ptr2++=ToUpper(*ptr1++);
	*ptr2=0;
} else {
	strcpy(patname,match->patname);
}
strcpy(patflags,match->patflags);

ParsePatternNoCase(patname,token1,1000);
ParsePattern(patname,token2,1000);
ParsePatternNoCase(patflags,token3,1000);
for(node=list->head;node->succ;node=node->succ) {
	checkmatch(token1,token2,token3,node,match,matchtype);
}
parseend:
freemem(token1);
}

WORD parsematch(struct FMList *list,struct CMenuConfig *cmc)
{
struct GUIBase *gb;
WORD c,pc;
WORD ccase,cfiles,cdirs,csize,cdate,cflags,ccomm,csortby,csortud,cmix,cdfirst;
LONG sizeis,sizeis2;
UBYTE patdate[32];
UBYTE patdate2[32];
UBYTE titlestr[80];
struct Match *match;

match=getconfig(cmc);
if(cmc->cmenucount==PARSECONFIG) pc=1; else pc=0;
sformat(titlestr,"%s (%ld)",findbuildstring(cmc),(WORD)list->li->linumber);

gb=getguibase(titlestr);

setconnectgroup(gb,14,0,0);

setconnectgroup(gb,1,1,14);
reqinfomsg(gb,MSG_PARSE_NAMEMATCH,100,guiUC|guiLEFT);
reqinfomsg(gb,MSG_PARSE_SIZEMATCH,101,guiUC|guiLEFT);
reqinfomsg(gb,MSG_PARSE_DATEMATCH,102,guiUC|guiLEFT);
reqinfomsg(gb,MSG_PARSE_PROTMATCH,103,guiUC|guiLEFT);
reqinfomsg(gb,MSG_PARSE_COMMENTMATCH,104,guiUC|guiLEFT);
if(pc) reqinfomsg(gb,MSG_PARSE_SORTBY,105,guiUC|guiLEFT);

setconnectgroup(gb,2,1,14);
reqstring(gb,100,match->patname,sizeof(match->patname));

setconnectgroup(gb,3,0,2);
sizeis=match->patsize;
reqinteger(gb,101,&sizeis,0,999999999);
sizeis2=match->patsize2;
reqinteger(gb,101,&sizeis2,0,999999999);
setconnectgroup(gb,4,0,2);
longtodatestring(patdate,match->patdate);
longtodatestring(patdate2,match->patdate2);
reqstring(gb,102,patdate,sizeof(patdate));
reqstring(gb,102,patdate2,sizeof(patdate2));
setguigroup(gb,2,0);
reqstring(gb,103,match->patflags,sizeof(match->patflags));
reqstring(gb,104,match->patcomm,sizeof(match->patcomm));
if(pc) {
	csortby=match->msortby;
	reqcyclemsg(gb,106,&csortby,MSG_PARSE_NAMEMATCH,MSG_PARSE_SIZEMATCH,MSG_PARSE_DATEMATCH,MSG_PARSE_COMMENTMATCH,0);
}

setconnectgroup(gb,5,1,14);
ccase=match->mcase;
reqcyclemsg(gb,100,&ccase,MSG_CONFIG_OFF,MSG_PARSE_CASEINSENSITIVE,MSG_PARSE_CASESENSITIVE,0);
csize=match->msize;
reqcyclemsg(gb,101,&csize,MSG_CONFIG_OFF,MSG_PARSE_SIZEEQUAL,MSG_PARSE_SIZENOEQUAL,MSG_PARSE_SIZESMALLER,MSG_PARSE_SIZELARGER,MSG_PARSE_BETWEEN,MSG_PARSE_NOTBETWEEN,0);
cdate=match->mdate;
reqcyclemsg(gb,102,&cdate,MSG_CONFIG_OFF,MSG_PARSE_DATEOLDER,MSG_PARSE_DATENEWER,MSG_PARSE_BETWEEN,MSG_PARSE_NOTBETWEEN,0);
cflags=match->mflags;
reqcyclemsg(gb,103,&cflags,MSG_CONFIG_OFF,MSG_CONFIG_ON,0);
ccomm=match->mcomm;
reqcyclemsg(gb,104,&ccomm,MSG_CONFIG_OFF,MSG_CONFIG_ON,0);
if(pc) {
	csortud=match->msortud;
	reqcyclemsg(gb,105,&csortud,MSG_PARSE_SORTUP,MSG_PARSE_SORTDOWN,0);
}

setconnectgroup(gb,13,0,0);

setconnectgroup(gb,6,1,13);
if(pc) reqinfomsg(gb,MSG_PARSE_MIX,110,guiUC|guiLEFT);
reqinfomsg(gb,MSG_PARSE_FILES,111,guiUC|guiLEFT);
setconnectgroup(gb,7,1,13);
if(pc) {
	cmix=match->mmix;
	reqcycle2msg(gb,110,&cmix);
}
cfiles=match->mfiles;
reqcyclemsg(gb,111,&cfiles,MSG_PARSE_TYPE1,MSG_PARSE_TYPE2,MSG_PARSE_TYPE3,0);
setconnectgroup(gb,8,1,13);
if(pc) reqinfomsg(gb,MSG_PARSE_DRAWERS,120,guiUC|guiLEFT);
reqinfomsg(gb,MSG_PARSE_DIRS,121,guiUC|guiLEFT);
setconnectgroup(gb,9,1,13);
if(pc) {
	cdfirst=match->mdrawersfirst;
	reqcycle2msg(gb,120,&cdfirst);
}
cdirs=match->mdirs;
reqcyclemsg(gb,121,&cfiles,MSG_PARSE_TYPE1,MSG_PARSE_TYPE2,MSG_PARSE_TYPE3,0);

buttonbarmsg(gb,MSG_OK,1,MSG_CANCEL,0,0);

c=quickreq(gb);

if(c) {
	match->mcase=ccase;
	match->mfiles=cfiles;
	match->mdirs=cdirs;
	match->msize=csize;
	match->mdate=cdate;
	match->mflags=cflags;
	match->mcomm=ccomm;
	if(pc) {
		match->msortby=csortby;
		match->msortud=csortud;
		match->mmix=cmix;
		match->mdrawersfirst=cdfirst;
	}
	match->patsize=sizeis;
	match->patsize2=sizeis2;
	match->patdate=datestringtolong(patdate);
	match->patdate2=datestringtolong(patdate2);
	if(match->patdate==0xffffffff) {
		match->patdate=0;
		match->mdate=0;
	}
	if(match->patdate2==0xffffffff) {
		match->patdate2=0;
		match->mdate=0;
	}
	return(1);
}
return(0);
}

void __saveds parse(void)
{
struct ProcMsg *pm;
struct FMList *list;

pm=sinitproc();
list=fmmain.sourcedir;
if(!(setalloc(list,1))) {
	initproc(0,0); 
	goto endi;
}
offgadget(&list->li->taskgadget,LISTGADGETS);
initproc(list,pm->cmc->label);
if(parsematch(list,pm->cmc)) {
	csortlist(list);
	switch(pm->cmc->cmenucount)
	{
	case INCLUDECONFIG:
	parselist(list,10);
	break;
	case EXCLUDECONFIG:
	parselist(list,20);
	break;
	case PARSECONFIG:
	parselist(list,0);
	break;
	}
	outputlist(list);
	endofdirtxt(list,0);
}
ongadget(&list->li->taskgadget,LISTGADGETS);
endproc(list);
endi:
deinitproc(pm);
}

void *parseconfigdefault(struct CMenuConfig *cmc)
{
struct Match *config;
WORD ret;
struct DateStamp ds;
LONG date;

ret=allocconfig(cmc,sizeof(struct Match));
if(ret<0) return(cmc->moreconfig);
if(!ret) return(0);
DateStamp(&ds);
date=dstolong(&ds);
config=cmc->moreconfig;
config->mcase=1;
config->patname[0]='#';
config->patname[1]='?';
config->patdate=date;
return(cmc->moreconfig);
}

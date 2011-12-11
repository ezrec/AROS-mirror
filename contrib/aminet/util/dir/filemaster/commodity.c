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

#include <aros/debug.h>
#include <exec/types.h>
#include <libraries/commodities.h>
#include <proto/commodities.h>

#include <stdio.h>
#include "fmnode.h"
extern struct Library *CxBase; 
extern struct ExecBase *SysBase; 

extern struct FMMain fmmain;
extern struct FMConfig *fmconfig;
static struct NewBroker fmbroker = {
	NB_VERSION,
	"FileMaster","FileMaster 3.1 © 1992-1997 by Toni Wilen","Directory utility",
	0,COF_SHOW_HIDE,0,0,0
};


static CxObj *CustomHotKey(UBYTE *Code,struct MsgPort *Port,LONG ID)
{
CxObj *Filter;
CxObj *Sender;

if((Filter = CxFilter(Code))) {
	if((Sender = CxSender(Port,ID))) {
		AttachCxObj(Filter,Sender);
		if(!CxObjError(Filter)) return(Filter);
	}
	DeleteCxObjAll(Filter);
}
return(0);
}


WORD checkhotkeys(struct IntuiMessage *im)
{
struct InputEvent ie;
WORD cnt1;
D(bug("commodity.c 63 \n"));  
if(!CxBase) return(-1);
D(bug("commodity.c 65 \n"));  
ie.ie_Class=IECLASS_RAWKEY;
D(bug("commodity.c 67 \n")); 
ie.ie_SubClass=0;
D(bug("commodity.c 69 \n")); 
ie.ie_Code=im->Code;
D(bug("commodity.c 71 \n")); 
ie.ie_Qualifier=im->Qualifier;
D(bug("commodity.c 72 \n")); 
//ie.ie_EventAddress=(APTR*)*((ULONG*)im->IAddress);
D(bug("commodity.c 75 \n")); 
for(cnt1=0;cnt1<TOTALCOMMANDS+LISTGADGETS*WINDOWLISTS;cnt1++) {
D(bug("commodity.c 77 \n")); 
	if(fmmain.ix[cnt1]) {
D(bug("commodity.c 79 \n")); 
		if(MatchIX(&ie,fmmain.ix[cnt1])) return(cnt1);
D(bug("commodity.c 81 \n")); 
	}
}
return(-1);
}

static void clearhotkeys(void)
{
WORD cnt1;
for(cnt1=0;cnt1<TOTALCOMMANDS+LISTGADGETS*WINDOWLISTS;cnt1++) {
	freemem(fmmain.ix[cnt1]);
	fmmain.ix[cnt1]=0;
}
}

static void sethotkeys(void)
{
struct GadKeyTab *gkt;
UBYTE *ptr;
WORD cnt1,apu1;
D(bug("commodity.c 101 \n")); 
clearhotkeys();
D(bug("commodity.c 103 \n")); 
if(!CxBase) return;

for(cnt1=0;cnt1<TOTALCOMMANDS+LISTGADGETS*WINDOWLISTS;cnt1++) {
	gkt=&fmmain.gadkeytab[cnt1];
	if(gkt->cmc) ptr=gkt->cmc->shortcut; else ptr=gkt->key;
	if(ptr&&*ptr) {
		fmmain.ix[cnt1]=allocmem(sizeof(struct InputXpression));
		if(fmmain.ix[cnt1]) {
			apu1=ParseIX(ptr,fmmain.ix[cnt1]);
			if(apu1) {
				freemem(fmmain.ix[cnt1]);
				fmmain.ix[cnt1]=0;
			}
		}
	}
AttachCxObj(fmmain.broker,CustomHotKey(ptr,fmmain.cxport,cnt1+1));
}
}

void removebroker(void)
{
CxMsg *cxmsg;

clearhotkeys();
if(fmmain.broker) DeleteCxObjAll(fmmain.broker);
if(fmmain.cxport) {
	RemPort(fmmain.cxport);
	while((cxmsg=(CxMsg*)GetMsg(fmmain.cxport))) ReplyMsg((struct Message*)cxmsg);
	DeleteMsgPort(fmmain.cxport);
}
fmmain.broker=0;
fmmain.cxport=0;
}

WORD createbroker(void)
{
if((fmmain.broker=CxBroker(&fmbroker,0))) {
	sethotkeys();
	ActivateCxObj(fmmain.broker,TRUE);
	return(TRUE);
}
return(0);
}

WORD createbrokerall(void)
{
if(!(fmmain.cxport=CreateMsgPort())) return(0);
fmmain.cxport->mp_Node.ln_Name=(STRPTR)fmbroker.nb_Name;
AddPort(fmmain.cxport);
fmbroker.nb_Port=fmmain.cxport;
fmbroker.nb_Pri=0;
if(createbroker()) return(TRUE);
DeleteMsgPort(fmmain.cxport);
fmmain.cxport=0;
return(0);
}

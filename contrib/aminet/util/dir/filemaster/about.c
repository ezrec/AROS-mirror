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
#include <exec/execbase.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <proto/bgui.h>
#include <libraries/reqtools.h>
#include <strings.h>
#include <stdio.h>
#include "fmnode.h"
#include "fmlocale.h"
#include "fmgui.h"

extern struct FMMain fmmain;
extern struct ExecBase *SysBase;
extern UBYTE verlib[];
/*extern UBYTE regform[];*/

UBYTE ecs[] = { "ECS" };
UBYTE ocs[] = { "OCS" };

void aboutti (WORD);

/*
void __saveds registering(void)
{
struct GUIBase *gb;
struct ProcMsg *pm;
UBYTE *regtxt;
WORD c;
ULONG cputype,fputype,chipmem,fastmem,kver,krev;
ULONG apu;
UBYTE fputext[8];
UBYTE filename[32];
UWORD attn;
UBYTE wbtext[16];
UBYTE chipset[32];
struct MemHeader *mhead;
struct FMHandle *h;
struct FMList *list;
UWORD *ptr;
struct Library *versionbase;

pm=sinitproc();
priority(pm->cmc);
list=fmmain.sourcedir;
if (!(setalloc(list,1))) { initproc(0,0); return; }
initproc(list,pm->cmc->label);
regtxt=allocmem(strlen(regform)+512);
if(!regtxt) goto nomem;

filename[0]='P'; filename[1]='R'; filename[2]='T'; filename[3]=':';
filename[4]=0;

if(GfxBase->ChipRevBits0&(GFXF_AA_ALICE|GFXF_AA_LISA)) {
	strcpy(chipset,"AGA");
} else {
	sformat(chipset,"Agnus %s Denise %s",GfxBase->ChipRevBits0&GFXF_HR_AGNUS?ecs:ocs,GfxBase->ChipRevBits0&GFXF_HR_DENISE?ecs:ocs);
}
apu=(ULONG)SysBase->LibNode.lib_Node.ln_Name;
apu&=~0x3ffff;
ptr=(UWORD*)apu;
kver=ptr[6]; krev=ptr[7];
wbtext[0]='?'; wbtext[1]=0;
versionbase=(struct Library*)OpenLibrary(verlib,0);
if(versionbase) {
	sformat(wbtext,"%ld.%ld",versionbase->lib_Version,versionbase->lib_Revision);
	CloseLibrary(versionbase);
}
attn=SysBase->AttnFlags;
cputype=68000;
if(attn&AFF_68010) cputype=68010;
if(attn&AFF_68020) cputype=68020;
if(attn&AFF_68030) cputype=68030;
if(attn&AFF_68040) cputype=68040;
if(attn&AFF_68060) cputype=68060;
fputype=0;
if(attn&AFF_68881) fputype=68881;
if(attn&AFF_68882) fputype++;
if(attn&AFF_FPU40) {
	if(attn&AFF_68060) fputype=68060; else fputype=68040;
}
if(!fputype) {
	fputext[0]='-';
	fputext[1]=0;
} else {
	sformat(fputext,"%ld",fputype);
}
fastmem=0; chipmem=0;
Disable();
mhead=(struct MemHeader*)&SysBase->MemList.lh_Head;
while(mhead->mh_Node.ln_Succ) {
	apu=(ULONG)mhead->mh_Upper-(ULONG)mhead->mh_Lower;
	apu+=0xffff; apu&=0xffff0000;
	apu>>=10;
	if(mhead->mh_Attributes&MEMF_FAST) fastmem+=apu;
	if(mhead->mh_Attributes&MEMF_CHIP) chipmem+=apu;
	mhead=(struct MemHeader*)mhead->mh_Node.ln_Succ;
}
Enable();
sformat(regtxt,regform,
	kver,krev,wbtext,
	cputype,fputext,
	chipset,
	chipmem,fastmem,
	fmmain.fmtitle,fmmain.fmdate
	);

gb=getguibase(getstring(MSG_MAIN_REGISTERTITLE));
setconnectgroup(gb,1,0,0);
reqinfomsg(gb,MSG_MAIN_REGISTERTITLE,100,guiUC|guiLEFT);
reqstring(gb,100,filename,30);
buttonbarmsg(gb,MSG_OK,1,MSG_CANCEL,0,0);
c=quickreq(gb);
if(c) {
	h=openfile(list,filename,OFWRITE);
	if(h) {
		writefile(h,regtxt,strlen(regtxt));
		closefile(h);
	}
}
nomem:
freemem(regtxt);
endproc(list);
deinitproc(pm);
}
*/

void __saveds
about (void)
{
  struct ProcMsg *pm;
D(bug("about.c 155...........\n"));    
  pm = sinitproc ();
D(bug("about.c 157...........\n")); 
  priority (pm->cmc);
  fmmain.wincnt++;
  initproc (0, 0);
D(bug("about 161...........\n")); 
  aboutti (0);
  fmmain.wincnt--;
  deinitproc (pm);
  D(bug("about.c 165...........\n"));
}

UBYTE aboutstring[] = {
  ISEQ_C "%s %s\n\nAuthor:\n\nToni Wilen\nSudentie 4\n13600 HML\n"
    "Finland\n\nemail: twilen@sci.fi\nwww: www.sci.fi/~twilen\n\n"
};

void
aboutti (WORD reg)
{
  UBYTE abostring[1000];
  if (reg && fmmain.regname[0])
    return;

  D(bug("about.c 179...........\n"));
 
  sformat (abostring, aboutstring, fmmain.fmtitle, fmmain.fmdate, getstring (MSG_MAIN_PROGRAMMEDBY));

 D(bug("about.c 182...........\n"));         
  reqinfowindow (getstring (MSG_MAIN_ABOUT), abostring, guiCENTER, MSG_OK, 1,
		 0);
}

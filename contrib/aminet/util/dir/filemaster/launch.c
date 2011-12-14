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
#include <libraries/locale.h>
#include <proto/locale.h>
#include <proto/exec.h>
#include <proto/alib.h>
 
#include <proto/icon.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <devices/inputevent.h>
#include <graphics/gfxbase.h>
#include <aros/bigendianio.h>
#include <intuition/intuition.h>
#include <libraries/commodities.h>
#include <intuition/sghooks.h>
#include <graphics/gfx.h>
#include <graphics/gfxmacros.h>
#include <utility/tagitem.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <aros/debug.h>
#include <stdio.h>

#include <libraries/commodities.h>
#include <exec/types.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <exec/memory.h>
#include <exec/execbase.h>
/* #include <pragmas/exec_pragmas.h> */
#include <clib/exec_protos.h>
#include <proto/datatypes.h>
#include <string.h>
#include <intuition/intuition.h>
#include <intuition/iobsolete.h>
#include <stdio.h>

#ifndef AROS
#include <proto/xfdmaster.h>
#endif

#include "fmnode.h"
#include "fmlocale.h"

extern struct UtilityBase *UtilityBase;
extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct LocaleBase *LocaleBase;
extern struct xfdMasterBase *xfdMasterBase;

void executeownnum (void);
WORD setalloc (struct FMList *, WORD);
void stopoldmod (void);
void textextent (struct RastPort *, UBYTE *, WORD *, WORD *);

extern struct FMMain fmmain;
extern struct FMConfig *fmconfig;
extern APTR launchtable[];
extern struct FMList fmlist[];

WORD
launch (void (*address) (void), struct CMenuConfig *cmc, WORD flag,
	WORD subproc)
{
  struct ProcMsg *pmsg;
  struct Process *proc;
  struct IntuiMessage *imsg;
  WORD cnt;
  WORD ret = 0;
  
  if (!address)
    return (1);
  for (cnt = 0; cnt < LISTS; cnt++)
    fmlist[cnt].flags &= ~LABORTREQ;
  pmsg = allocvec (0, sizeof (struct ProcMsg), MEMF_CLEAR | MEMF_PUBLIC);
  if (!pmsg)
    return (0);
  pmsg->cmc = cmc;
  D(bug("launch.c 103...........\n")); 
  SetSignal (0, SIGBREAKF_CTRL_F);
  fmmain.pmsg = pmsg;
  D(bug("launch.c 106...........\n")); 

  if (subproc & MSUBPROC)
    {
      /* flag?"FM Timer":"<<<FileMaster Unknown>>>" */
      if (!
	  (proc =
	   CreateNewProcTags (NP_Entry, address, NP_StackSize, 10000,
			      NP_Priority, fmconfig->mainpriority, NP_Name,
			      "FM", TAG_DONE)))
	goto fail;
      Signal ((struct Task *) proc, SIGBREAKF_CTRL_F);
	D(bug("launch.c 117...........\n")); 
      while (fmmain.pmsg)
	Wait (SIGBREAKF_CTRL_F);
      ret = 1;
    }
  else
    {
      Signal ((struct Task *) fmmain.myproc, SIGBREAKF_CTRL_F);
      address ();
      ret = 1;

    }
  SetSignal (0, SIGBREAKF_CTRL_F);
fail:
  if (fmmain.ikkuna)
    {
      while ((imsg = (struct IntuiMessage *) GetMsg (fmmain.ikkuna->UserPort)))
	ReplyMsg ((struct Message *) imsg);
    }
return (ret);
}

void
runcommand (struct GadKeyTab *gkt)
{
  WORD apu1;
  struct Gadget *g;
  struct CMenuConfig *cmc;
D(bug("launch.c 142...........\n")); 
  ReportMouse (0, fmmain.ikkuna);
  for (apu1 = 0; apu1 < LISTS; apu1++)
    fmlist[apu1].lastclicked = 0;
  g = gkt->g;
  if (g)
    {
      fmmain.sourcedir->fmmessage1[0] = 0;
      fmmain.sourcedir->fmmessage2[0] = 0;
      cmc = gkt->cmc;

      if (cmc && cmc->cmenucount)
	{
	  if (cmc->cmenucount < 100)
	    {
	      if (cmc->cmenucount <= CONFIGCONFIG
		  || (cmc->cmenucount >= DRIVESCONFIG
		      && cmc->cmenucount <= CLEARCONFIG)
		  || cmc->cmenucount == FILEEDITCONFIG
		  || cmc->cmenucount == DISKEDITCONFIG)
		{
		  launch (launchtable[cmc->cmenucount], cmc, 0, 0);
		}
	      else
		{
		  launch (launchtable[cmc->cmenucount], cmc, 0, MSUBPROC);
		}
	    }
	  else
	    {
	      launch (executeownnum, cmc, 0, MSUBPROC);
	    }
	}
    }
}

void
deinitproc (struct ProcMsg *pm)
{
/*
  if (xfdMasterBase)
    CloseLibrary ((struct Library *) xfdMasterBase);

#ifdef V39
  CloseLibrary (CxBase);
  CloseLibrary (DataTypesBase);
#endif
*/
D(bug("launch.c 187...........\n"));      
/*
  CloseLibrary ((struct Library *) DOSBase);
  CloseLibrary ((struct Library *) GfxBase);
  CloseLibrary ((struct Library *) IntuitionBase);
  CloseLibrary (UtilityBase);
  CloseLibrary (GadToolsBase);
  CloseLibrary (KeymapBase);
  CloseLibrary (WorkbenchBase);
  CloseLibrary (LocaleBase);
  CloseLibrary (IconBase);
  CloseLibrary (BGUIBase);
  freemem (pm);
*/
}

extern UBYTE gfxlib[];
extern UBYTE intlib[];
extern UBYTE comlib[];
extern UBYTE utilib[];
extern UBYTE gadlib[];
extern UBYTE keylib[];
extern UBYTE wblib[];
extern UBYTE loclib[];
extern UBYTE bguilib[];
extern UBYTE xfdlib[];
extern UBYTE datlib[];
extern UBYTE icolib[];


struct ProcMsg *__saveds sinitproc (void)
{
  struct ProcMsg *msg;
D(bug("launch.c 216...........\n")); 
  Wait (SIGBREAKF_CTRL_F);
  msg = fmmain.pmsg;
  fmmain.pmsg = 0;
D(bug("launch.c 220...........\n")); 
  Signal ((struct Task *) fmmain.myproc, SIGBREAKF_CTRL_F);
D(bug("launch.c 222...........\n")); 
/* 
 OpenLibrary ("dos.library", 0);
  OpenLibrary (gfxlib, 0);
  OpenLibrary (intlib, 0);
  OpenLibrary (utilib, 0);
  OpenLibrary (gadlib, 0);
  OpenLibrary (keylib, 0);
  OpenLibrary (wblib, 0);
  OpenLibrary (loclib, 0);
  OpenLibrary (icolib, 0);
  OpenLibrary (bguilib, 0);
  OpenLibrary (xfdlib, 0);
#ifdef V39
  OpenLibrary (comlib, 0);
  OpenLibrary (datlib, 0);
#endif
*/
  return (msg);
}

void
priority (struct CMenuConfig *cmc)
{
#ifndef __AROS__
  if (cmc)
    SetTaskPri (FindTask (0), fmconfig->pri[cmc->priority]);
#endif
}

void
initproc (struct FMList *list, UBYTE * txt)
{
  struct FMList *plist;
  struct Process *proc;
  UBYTE *txt2;

  if ((IPTR) txt != 1)
    txt2 = txt;
  else
    txt2 = 0;

  if (txt && list)
    {
      list->flags |= LACTIVE;
      list->oldcur = (BPTR)1;
      if (fmconfig->flags & MSUBPROC)
	list->flags |= LSUBPROC;
      plist = list->pair;
      if (plist)
	{
	  plist->flags |= LACTIVE;
	  plist->oldcur = (BPTR)1;
	  strcpy (plist->workname, txt2);
	  plist->fmmessage1[0] = 0;
	  plist->fmmessage2[0] = 0;
	}
      list->fmmessage1[0] = 0;
      list->fmmessage2[0] = 0;
      fmmessage (list);
    }
  changename (list, txt2);
  proc = (struct Process *) FindTask (0);
  if (fmmain.ikkuna)
    proc->pr_WindowPtr = fmmain.ikkuna;
}

void
changename (struct FMList *list, UBYTE * name)
{
/* struct Task *task; */

  if (!list || !name)
    return;
/*
task=FindTask(0);
if(name&&fmconfig->flags&MSUBPROC) {
	Disable();
	strcpy(task->tc_Node.ln_Name,"FM ");
	strcpy(task->tc_Node.ln_Name+3,name);
	Enable();
}
*/
  if (list)
    strcpy (list->workname, name);
}

void
endproc (struct FMList *list)
{
  BPTR prevlock;
  struct IntuiMessage *imsg;

  if (!(list->flags & LACTIVE))
    return;
  Forbid ();
  setalloc (list, 0);
  if (list->pair)
    {
      setalloc (list->pair, 0);
      list->pair->flags &= ~(LACTIVE | LSUBPROC | LUPDATEMSG);
      list->pair->workname[0] = 0;
      list->pair->pair = 0;
    }
  if (list->flags & LSUBPROC && list->oldcur != (BPTR)1)
    {
      prevlock = CurrentDir (list->oldcur);
      if (prevlock)
	UnLock (prevlock);
    }
  list->workname[0] = 0;
  list->pair = 0;
  list->flags &= ~(LACTIVE | LSUBPROC | LUPDATEMSG);
  if (fmmain.ikkuna)
    {
      while ((imsg = (struct IntuiMessage *) GetMsg (fmmain.ikkuna->UserPort)))
	ReplyMsg ((struct Message *) imsg);
    }
  Permit ();
}

WORD
tstsubproc (WORD mode)
{
  extern struct FMList fmlist[];
  WORD cnt;

retry:
  Forbid ();
  for (cnt = 0; cnt < 10; cnt++)
    {
      if ((fmlist[cnt].flags & LACTIVE) || fmmain.wincnt)
	{
	  Permit ();
	  if (requestmsg
	      (getstring (MSG_MAIN_WARNING), MSG_RETRY, MSG_CANCEL,
	       MSG_MAIN_PROCESSES))
	    goto retry;
	  return (1);
	}
    }
  Permit ();
  return (0);
}

/*  0=free,1=alloc */

#define GNUM 4

WORD
setalloc (struct FMList * list, WORD aa)
{
  extern struct FMMain fmmain;

  if (aa == 1 && list->flags & LALLOCATED)
    return (0);
  if (aa == 1)
    {
      list->flags |= LALLOCATED;
      if (list->li)
	offgadget (&list->li->slider1, 3);
      return (1);
    }
  if (!aa)
    {
      list->flags &= ~LALLOCATED;
      if (list->li)
	ongadget (&list->li->slider1, 3);
    }
  return (1);
}

void
offgadget (struct Gadget *gadg, WORD cnt)
{
  WORD pos, cntv = 0;
  struct Gadget *gadgv;

  gadgv = gadg;
  while (cnt-- && gadg)
    {
      pos = RemoveGadget (fmmain.ikkuna, gadg);
      gadg->Flags|=GADGDISABLED;
      AddGadget (fmmain.ikkuna, gadg, pos);
      gadg = gadg->NextGadget;
      cntv++;
    }
  if (cntv)
    RefreshGList (gadgv, fmmain.ikkuna, 0, cntv);
}

void
ongadget (struct Gadget *gadg, WORD cnt)
{
  WORD pos, cntv = 0;
  struct Gadget *gadgv;
  UBYTE txt[2];
  WORD apu1, apu2;

  gadgv = gadg;
  while (cnt-- && gadg)
    {
      pos = RemoveGadget (fmmain.ikkuna, gadg);
      gadg->Flags &= ~GADGDISABLED;
      if (gadg->GadgetType == BOOLGADGET)
	{
	  ObtainSemaphore (&fmmain.gfxsema);
	  SetAPen (fmmain.rp, fmconfig->backpen);
	  RectFill (fmmain.rp, gadg->LeftEdge, gadg->TopEdge,
		    gadg->LeftEdge + gadg->Width,
		    gadg->TopEdge + gadg->Height);
	  SetAPen (fmmain.rp, fmconfig->txtpen);
	  SetSoftStyle (fmmain.rp, 1, 1);
	  txt[0] = (UBYTE)(IPTR) gadg->UserData;
	  txt[1] = 0;
	  textextent (fmmain.rp, txt, &apu1, &apu2);
	  Move (fmmain.rp, (gadg->Width - apu1 + 1) / 2 + gadg->LeftEdge,
		gadg->TopEdge + fmmain.txtbaseline + 2);
	  Text (fmmain.rp, txt, 1);
	  SetSoftStyle (fmmain.rp, 0, 1);
	  ReleaseSemaphore (&fmmain.gfxsema);
	}
      AddGadget (fmmain.ikkuna, gadg, pos);
      cntv++;
      gadg = gadg->NextGadget;
    }
  if (cntv)
    RefreshGList (gadgv, fmmain.ikkuna, 0, cntv);
}

WORD
testabort (struct FMList *list)
{
  struct IntuiMessage *message;

  extern struct FMList fmlist[];

  if (!(fmconfig->flags & MSUBPROC))
    {
      while ((message =
	     (struct IntuiMessage *) GetMsg (fmmain.ikkuna->UserPort)))
	{
	  if (message->Class==MOUSEBUTTONS && message->Code==MENUDOWN && list->flags&LACTIVE) list->flags|=LABORTREQ;
	  ReplyMsg((struct Message*)message);
	}
    }
  if (list->flags & LACTIVE && list->flags & LABORTREQ)
    return (1);
  return (0);
}

WORD
askabort (struct FMList * list)
{
  WORD ret;
  UWORD oldflag;

  oldflag = list->flags;
  list->flags &= ~(LABORTREQ | LUPDATEMSG);
  ret =
    request (getstring (MSG_MAIN_ABORT), MSG_YES, MSG_NO, "%s (%lU)",
	     list->workname, list->listnumber);
  if (ret)
    {
      strcpymsg (list->fmmessage1, MSG_MAIN_ABORTED);
      list->fmmessage2[0] = 0;
      fmmessage (list);
    }
  else
    {
      if (oldflag & LUPDATEMSG)
	list->flags |= LUPDATEMSG;
    }
  return (ret);
}

void
askmodabort (void)
{
  if (fmmain.modflag
      && requestmsg (getstring (MSG_MAIN_AREYOUSURE), MSG_YES, MSG_NO,
		     MSG_MAIN_ABORTMODPLAY))
    stopoldmod ();
}

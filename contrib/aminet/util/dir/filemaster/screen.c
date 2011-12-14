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
#include <aros/bigendianio.h>
#include <graphics/gfxbase.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <devices/inputevent.h>
 
#include <dos/dosextens.h>
#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <exec/semaphores.h>
#include <intuition/intuition.h>
#include <intuition/sghooks.h>
#include <intuition/iobsolete.h>
#include <stdio.h>
#include <string.h>

#include <proto/icon.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include "fmnode.h"
#include "fmlocale.h"


WORD avaanaytto (WORD);
WORD suljenaytto (WORD);
void teeruutu (WORD, WORD);
void draw3dbox (struct RastPort *, WORD, WORD, WORD, WORD, WORD);
struct TextFont *openfont (struct TextAttr *);
WORD windownewsize (void);
void setcmenu (void);
void docmenusize (void);
void textextent (struct RastPort *, UBYTE *, WORD *, WORD *);
ULONG power2 (WORD);
void freegadgets (struct Gadget *);
void drives (struct FMList *);
void updatetasknumbers (struct ListInfo *);

struct ListInfo *alloclistgads (WORD, WORD, WORD, WORD, WORD, WORD, WORD,
				WORD);
void addlistgads (void);
void freelistgads (void);
void doborder (WORD *, struct Border *, struct Border *, WORD *,
	       struct Border *, struct Border *, WORD, WORD);
WORD avaanayttoa (UBYTE *, WORD);
void gadgettables (void);
void executeownnum (void);

WORD errorreq (WORD, UBYTE *, WORD, WORD);

extern struct FMConfig *fmconfig;
extern struct FMMain fmmain;
extern struct FMList fmlist[];
extern struct ExecBase *SysBase;
extern UBYTE dflib[];
extern UBYTE topaz[];
extern UBYTE workbench[];

WORD
avaanaytto (WORD flag)
{
  UBYTE emsg[100];
  if (flag == 2)
    return (avaanayttoa (emsg, 2));
  if (avaanayttoa (emsg, flag))
    return (1);
  strcpy (fmconfig->listfontname, topaz);

  fmconfig->listfontattr.ta_YSize = 8;
  strcpy (fmconfig->txtfontname, topaz);
  fmconfig->txtfontattr.ta_YSize = 8;
  strcpy (fmconfig->reqfontname, topaz);
  fmconfig->reqfontattr.ta_YSize = 8;
  fmconfig->txtfontattr.ta_Name = fmconfig->txtfontname;
  fmconfig->listfontattr.ta_Name = fmconfig->listfontname;
  fmconfig->reqfontattr.ta_Name = fmconfig->reqfontname;
  if (avaanayttoa (emsg, flag))
    return (1);
  fmconfig->mainscreen.width = 640;
  fmconfig->mainscreen.height = 256;
  fmconfig->mainscreen.depth = 2;
  fmconfig->mainscreen.autoscroll = 0;
  fmconfig->mainscreen.overscan = 1;
  fmconfig->screentype = 0;
  if (avaanayttoa (emsg, flag))
    return (1);
  errorreq (MSG_REQ_FATALERR, emsg, 0, MSG_OK);
  return (0);
}

WORD
avaanayttoa (UBYTE * emsg, WORD flag)
{
  WORD apu1, apu2;
  UBYTE *ptr1, *ptr2;
  WORD penlist[2];
  ULONG coltable[COLORS * 3 + 8];
  struct DrawInfo *di;
  struct Screen *vscr;
  WORD ret = 1;
#define WFLAGS WFLG_ACTIVATE|WFLG_RMBTRAP|WFLG_SMART_REFRESH

struct NewWindow uusiikkuna = {
    0, 0, 0, 0, 0, 2,
    IDCMP_MOUSEMOVE | IDCMP_RAWKEY | IDCMP_MOUSEBUTTONS | IDCMP_GADGETUP |
      IDCMP_GADGETDOWN | IDCMP_MOUSEMOVE | IDCMP_REFRESHWINDOW |
      IDCMP_CLOSEWINDOW | IDCMP_NEWSIZE | IDCMP_DISKINSERTED |
      IDCMP_DISKREMOVED | IDCMP_ACTIVEWINDOW | IDCMP_INACTIVEWINDOW,
    0,
    0, 0, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0
  };
D(bug("screen.c 138 entering.........\n"));    
  if (flag == 3)
    {
      penlist[0] = -1;
      penlist[1] = -1;

      if (!(fmmain.normalfont = OpenFont (&fmmain.tattr)))
	goto anaytto1;
      if (!(fmmain.smallfont = OpenFont (&fmconfig->smallfontattr)))
	goto anaytto1;
      if (!(fmmain.txtfont = OpenFont (&fmconfig->txtfontattr)))
	goto anaytto1;
      if (!(fmmain.listfont = OpenFont (&fmconfig->listfontattr)))
	goto anaytto1;
      if (!(fmmain.reqfont = OpenFont (&fmconfig->reqfontattr)))
	goto anaytto1;
      if (!(fmmain.txtshowfont = OpenFont (&fmconfig->txtshowfontattr)))
	goto anaytto1;

      fmmain.txtysize = fmmain.txtfont->tf_YSize;
      fmmain.txtxsize = fmmain.txtfont->tf_XSize;
      fmmain.txtbaseline = fmmain.txtfont->tf_Baseline;
      fmmain.listysize = fmmain.listfont->tf_YSize;
      fmmain.listxsize = fmmain.listfont->tf_XSize;
      fmmain.listbaseline = fmmain.listfont->tf_Baseline;
      fmmain.reqysize = fmmain.reqfont->tf_YSize;
      fmmain.reqxsize = fmmain.reqfont->tf_XSize;
      fmmain.reqbaseline = fmmain.reqfont->tf_Baseline;
      fmmain.txtshowysize = fmmain.txtshowfont->tf_YSize;
      fmmain.txtshowxsize = fmmain.txtshowfont->tf_XSize;
      fmmain.txtshowbaseline = fmmain.txtshowfont->tf_Baseline;
    }
  fmmain.myproc = (struct Process *) FindTask (0);

  if (flag == 3)
    {
      if (fmconfig->screentype <= 1)
	{
	     if (!(fmmain.naytto = (struct Screen *)OpenScreenTags(0,
                SA_Width,fmconfig->mainscreen.width,
                SA_Height,fmconfig->mainscreen.height,
                SA_Type,CUSTOMSCREEN,
                SA_Title,fmmain.fmtitlename,
                SA_Font,&fmconfig->txtfontattr,
                SA_Pens,penlist,
                SA_DisplayID,fmconfig->mainscreen.screenmode,
                SA_Overscan,fmconfig->mainscreen.overscan,
                SA_AutoScroll,fmconfig->mainscreen.autoscroll,
                SA_Depth,fmconfig->mainscreen.depth,
                SA_Interleaved,TRUE,
                SA_PubName,fmconfig->screentype==1?"FM":0,
                TAG_DONE))) 
		{
D(bug("screeen.c: 202...\n"));  
	      strcpymsg (emsg, MSG_MAIN_SCRERR1);
	      fmconfig->mainscreen.width = 640;
	      fmconfig->mainscreen.height = 256;
	      fmconfig->mainscreen.screenmode = HIRES_KEY;
	      fmconfig->mainscreen.overscan = 1;
	      fmconfig->mainscreen.autoscroll = 0;
	      fmconfig->mainscreen.depth = 2;
	      ret = 0;
	      goto anaytto1;

	    }
	  else
	    {
	      ShowTitle (fmmain.naytto, FALSE);
	      apu1 = power2 (fmconfig->mainscreen.depth);
	      ptr2 = fmconfig->colors;
	      ptr1 = (UBYTE *) coltable;
#ifdef V39
	      *ptr1++ = 0;
	      *ptr1++ = apu1;
	      *ptr1++ = 0;
	      *ptr1++ = 0;
	      for (apu2 = 0; apu2 < apu1; apu2++)
		{
		  *ptr1++ = *ptr2;
		  *ptr1++ = *ptr2;
		  *ptr1++ = *ptr2;
		  *ptr1++ = *ptr2;
		  ptr2++;
		  *ptr1++ = *ptr2;
		  *ptr1++ = *ptr2;
		  *ptr1++ = *ptr2;
		  *ptr1++ = *ptr2;
		  ptr2++;
		  *ptr1++ = *ptr2;
		  *ptr1++ = *ptr2;
		  *ptr1++ = *ptr2;
		  *ptr1++ = *ptr2;
		  ptr2++;
		}
	      *ptr1++ = 0;
	      *ptr1++ = 0;
	      *ptr1++ = 0;
	      *ptr1 = 0;
	      LoadRGB32 (&fmmain.naytto->ViewPort, coltable);
#else
	      for (apu2 = 0; apu2 < apu1; apu2++)
		{
		  *ptr1 = (*ptr2++) >> 4;
		  ptr1++;
		  *ptr1 = (*ptr2++) & 0xf0;
		  *ptr1 |= ((*ptr2++) & 0xf0) >> 4;
		  ptr1++;
		}
	      LoadRGB4 (&fmmain.naytto->ViewPort, (UWORD *) coltable, apu1);
#endif
	    }
	}
      else
	{
	  fmmain.naytto = LockPubScreen (fmconfig->pubscreen);
          printf("hello");
	  if (!fmmain.naytto)
	    {
	      strcpy (fmconfig->pubscreen, workbench);
	      fmmain.naytto = LockPubScreen (fmconfig->pubscreen);
	      if (!fmmain.naytto)
		{
		  strcpymsg (emsg, MSG_SCR_SCRERR2);
		  ret = 0;
		  goto anaytto1;
		}
	    }
	  fmconfig->mainscreen.depth =
	    fmmain.naytto->RastPort.BitMap->Depth <=
	    4 ? fmmain.naytto->RastPort.BitMap->Depth : 4;
	}
      if (fmmain.naytto->Width / 2 > fmmain.naytto->Height)
	{
	  fmconfig->flags |= MDOUBLED;
	  fmconfig->spacew = 2 * fmconfig->spaceh;
	  fmconfig->sliderw = 2 * fmconfig->sliderh;
	}
      else
	{
	  fmconfig->flags &= ~MDOUBLED;
	  fmconfig->spacew = fmconfig->spaceh;
	  fmconfig->sliderw = fmconfig->sliderh;
	}
      if (!(fmmain.gtvisual = GetVisualInfo (fmmain.naytto, 0)))
	{
	  strcpymsg (emsg, MSG_MAIN_SCRERR1);
	  ret = 0;
	  goto anaytto1;
	}
      if ((di = GetScreenDrawInfo (fmmain.naytto)))
	{
	  fmconfig->whitepen = di->dri_Pens[SHINEPEN];
	  fmconfig->blackpen = di->dri_Pens[SHADOWPEN];
	  FreeScreenDrawInfo (fmmain.naytto, di);
	}
    }

  if (fmconfig->screentype <= 1)
    uusiikkuna.Type = CUSTOMSCREEN;
  if (fmconfig->screentype == 2)
    uusiikkuna.Type = PUBLICSCREEN;
  if (fmconfig->flags & MWINDOWED)
    {
      uusiikkuna.Flags =
	WFLAGS | WFLG_SIZEGADGET | WFLG_DRAGBAR | WFLG_DEPTHGADGET |
	WFLG_CLOSEGADGET | WFLG_SIZEBBOTTOM | WFLG_GIMMEZEROZERO;
      uusiikkuna.Title = fmmain.fmtitlename;
      uusiikkuna.TopEdge = fmconfig->windowtop;
      uusiikkuna.LeftEdge = fmconfig->windowleft;
      uusiikkuna.Height = fmconfig->windowheight;
      uusiikkuna.Width = fmconfig->windowwidth;
    }
  else
    {
      uusiikkuna.Flags = WFLAGS | WFLG_BACKDROP | WFLG_BORDERLESS;
      uusiikkuna.Title = 0;
      uusiikkuna.TopEdge = fmmain.naytto->WBorTop + fmmain.txtysize + 1;
      uusiikkuna.Height = fmconfig->mainscreen.height - uusiikkuna.TopEdge;
      uusiikkuna.Width = fmconfig->mainscreen.width;
    }

  uusiikkuna.Screen = fmmain.naytto;
  uusiikkuna.BlockPen = fmconfig->sliderpen;
  uusiikkuna.DetailPen = fmconfig->backpen;
  if (!
      (fmmain.ikkuna =
       (struct Window *) OpenWindowTags (&uusiikkuna, WA_AutoAdjust, TRUE,
					 TAG_DONE)))
    {
      strcpymsg (emsg, MSG_MAIN_WINERR1);
      ret = 0;
      fmconfig->windowtop = 0;
      fmconfig->windowleft = 0;
      fmconfig->windowheight = fmconfig->mainscreen.height;
      fmconfig->windowwidth = fmconfig->mainscreen.width;
      goto anaytto1;
    }
  fmmain.oldwinptr = fmmain.myproc->pr_WindowPtr;
  fmmain.myproc->pr_WindowPtr = fmmain.ikkuna;
  fmmain.rp = fmmain.ikkuna->RPort;
  SetFont (fmmain.rp, fmmain.txtfont);
  ReportMouse (0, fmmain.ikkuna);

  if (!windownewsize ())
    {
      strcpymsg (emsg, MSG_SCR_MEMERR);
      ret = 0;
      goto anaytto1;
   }

D(bug("screen.c: 362...\n")); 

  vscr = LockPubScreen (workbench);
  if (vscr == fmmain.naytto)
    {
     D(bug("screen.c: 365...\n"));  
      fmmain.appwinport = CreateMsgPort ();
      if (fmmain.appwinport)
	fmmain.appwin =
	  AddAppWindowA (0, 0, fmmain.ikkuna, fmmain.appwinport, 0);

    }
  D(bug("screen.c: 372...\n"));  
  if (vscr)
    UnlockPubScreen (0, vscr);

anaytto1:
D(bug("screen.c anaytto1: entering\n"));        
  if (!fmmain.naytto || !fmmain.ikkuna || !fmmain.txtfont || !fmmain.listfont
      || !ret)
    {
      suljenaytto (flag);
      return (ret);
    }
  if (fmmain.naytto && fmconfig->screentype == 1)
    PubScreenStatus (fmmain.naytto, 0);
  return (1);
}

WORD
suljenaytto (WORD flag)
{
  struct AppMessage *appmsg;
D(bug("screen.c suljenaytto entering........\n"));   
 if (flag == 3)
    {
      for (;;)
	{
	  if (fmmain.naytto && fmconfig->screentype <= 1
	      && fmmain.naytto->FirstWindow
	      && fmmain.naytto->FirstWindow->NextWindow)
	    {
	      if (!requestmsg
		  (getstring (MSG_MAIN_WARNING), MSG_RETRY, MSG_CANCEL,
		   MSG_MAIN_WINDOWS))
		return (0);
	    }
	  else
	    {
	      break;
	    }
	}
      if (fmmain.naytto && fmconfig->screentype == 1)
	{
	  for (;;)
	    {
	      if (!((PubScreenStatus (fmmain.naytto, PSNF_PRIVATE)) & 1))
		{
		  if (!requestmsg
		      (getstring (MSG_MAIN_WARNING), MSG_RETRY, MSG_CANCEL,
		       MSG_MAIN_VISITORWINDOWS))
		    return (0);
		}
	      else
		{
		  break;
		}
	    }
	}
D(bug("screen.c: 425...\n"));  
       ObtainSemaphore (&fmmain.gfxsema);

      if (fmmain.txtfont)
	{
	  CloseFont (fmmain.txtfont);
	  fmmain.txtfont = 0;
	}
      if (fmmain.listfont)
	{
	  CloseFont (fmmain.listfont);
	  fmmain.listfont = 0;
	}
      if (fmmain.reqfont)
	{
	  CloseFont (fmmain.reqfont);
	  fmmain.reqfont = 0;
	}
      if (fmmain.txtshowfont)
	{
	  CloseFont (fmmain.txtshowfont);
	  fmmain.txtshowfont = 0;
	}
      if (fmmain.normalfont)
	{
	  CloseFont (fmmain.normalfont);
	  fmmain.normalfont = 0;
	}
      if (fmmain.smallfont)
	{
	  CloseFont (fmmain.smallfont);
	  fmmain.smallfont = 0;
	}
      freegadgets (fmmain.firstgadget);
      fmmain.firstgadget = 0;
      freelistgads ();
      if (fmmain.appwin)
	{
	  RemoveAppWindow (fmmain.appwin);
	  fmmain.appwin = 0;
	}
      if (fmmain.appwinport)
	{
	  while ((appmsg = (struct AppMessage *) GetMsg (fmmain.appwinport)))
	    ReplyMsg ((struct Message *) appmsg);
	  DeleteMsgPort (fmmain.appwinport);
	  fmmain.appwinport = 0;
	}
    }
  ObtainSemaphore (&fmmain.gfxsema);
  if (fmmain.ikkuna)
     {
        fmconfig->windowtop = fmmain.ikkuna->TopEdge;
        fmconfig->windowleft = fmmain.ikkuna->LeftEdge;
        fmconfig->windowheight = fmmain.ikkuna->Height;
        fmconfig->windowwidth = fmmain.ikkuna->Width;
        fmmain.myproc->pr_WindowPtr = fmmain.oldwinptr;
        CloseWindow (fmmain.ikkuna);
     	fmmain.ikkuna = 0;
    }
  ReleaseSemaphore (&fmmain.gfxsema);
  if (flag == 3)
    {
      if (fmmain.gtvisual)
	{
	  FreeVisualInfo (fmmain.gtvisual);
	  fmmain.gtvisual = 0;
	}
      if (fmmain.naytto)
	{
	  if (fmconfig->screentype <= 1)
	    {
	      CloseScreen (fmmain.naytto);
	    }
	  else
	    {
	      UnlockPubScreen (0, fmmain.naytto);
	    }
	  fmmain.naytto = 0;
	}
      ReleaseSemaphore (&fmmain.gfxsema);
    }
  return (1);
}

WORD
windownewsize (void)
{
  WORD height, width, apu1;
  struct FMList *list;

D(bug("screen.c windownewsize entering 530.........\n"));  

  ObtainSemaphore (&fmmain.gfxsema);

  freelistgads ();
  freegadgets (fmmain.firstgadget);
  fmmain.firstgadget = 0;

  SetAPen (fmmain.rp, fmconfig->backpen);
  RectFill (fmmain.rp, fmmain.ikkuna->BorderLeft, fmmain.ikkuna->BorderTop,
	    fmmain.ikkuna->Width + fmmain.ikkuna->BorderLeft,
	    fmmain.ikkuna->Height + fmmain.ikkuna->BorderTop);
  height =
    fmmain.ikkuna->Height - fmmain.ikkuna->BorderTop -
    fmmain.ikkuna->BorderBottom;
  width =
    fmmain.ikkuna->Width - fmmain.ikkuna->BorderLeft -
    fmmain.ikkuna->BorderRight;
  docmenusize ();
  teeruutu (width, height);
  addlistgads ();

  ReleaseSemaphore (&fmmain.gfxsema);

  setcmenu ();
 D(bug("screen.c 555.........\n"));  
  for (apu1 = 0; apu1 < WINDOWLISTS; apu1++)
    {
      if (fmmain.li[apu1])
	{
	  list = fmmain.li[apu1]->list;
	  if (list->flags == 0)
	    drives (list);
	  else
	    outputlist (list);
	  if (list->flags & LALLOCATED)
	    offgadget (&fmmain.li[apu1]->slider1, LISTGADGETS);
	}
D(bug("screen.c current.........\n"));  
      fmmain.uselist[apu1] = 0;
    }
D(bug("screen.c 569 .........\n"));  
  dirmeters ();
  fmmessage (fmmain.sourcedir);
  RefreshGList (fmmain.ikkuna->FirstGadget, fmmain.ikkuna, 0, -1);
  updatetasknumbers (0);
  fmconfig->windowtop = fmmain.ikkuna->TopEdge;
  fmconfig->windowleft = fmmain.ikkuna->LeftEdge;
  fmconfig->windowheight = fmmain.ikkuna->Height;
  fmconfig->windowwidth = fmmain.ikkuna->Width;
  return (1);
}

void
freegadgets (struct Gadget *g)
{
  struct Gadget *gg;

  if (g)
    {
      RemoveGList (fmmain.ikkuna, g, -1);
      while (g)
	{
	  gg = g;
	  g = g->NextGadget;
	  freemem (gg);
	}
    }
}

void
teeruutu (WORD width, WORD height)
{
  WORD tfh, sw, sh;
  WORD apu1, apu2, apu3, apu4, apu5, apu6, apu7, apu8, apu9, apu10, ly, sy;
  WORD colums, colweight;
  WORD rows[COLS + 1], rowweight[COLS + 1];
  WORD newcols[COLS + 1];
  WORD maxrows;
  WORD listwidth, listheight;
  WORD minwidth, minheight, widthi;
  WORD *xy;

  sh = fmconfig->sliderh + 2;
  sw = fmconfig->sliderw + 2;
  tfh = fmmain.txtysize + 1;

  if (!fmconfig->cmenutype)
    {
      fmmain.realcmenuspace = fmmain.cmenuwidth * fmconfig->cmenucolumns;
    }
  else
    {
      fmmain.realcmenuspace = fmconfig->cmenucolumns * (tfh + 1);
    }

  textextent (fmmain.rp, "0", &apu1, &apu2);
  textextent (fmmain.rp, ">", &apu3, &apu2);
  if (apu1 < apu3)
    apu1 = apu3;
  textextent (fmmain.rp, "<", &apu3, &apu2);
  if (apu1 < apu3)
    apu1 = apu3;
  widthi = apu1 + 2 * fmconfig->spaceh + 1;

  listwidth = width - fmconfig->spacew;
  listheight = height - 2 * fmconfig->spaceh - 3 * tfh - fmconfig->spacew / 3;
  if (fmconfig->cmenutype && fmconfig->cmenuposition > 0)
    listheight--;

  if (!fmconfig->cmenutype)
    listwidth -= fmmain.realcmenuspace;
  else
    listheight -= fmmain.realcmenuspace;
  apu1 = 0;
  colweight = 0;
  maxrows = 0;
  while (fmconfig->listinfo[apu1][0])
    {
      colweight += (WORD) (fmconfig->listinfo[apu1][0] & 0x7f);
      apu2 = 1;
      rowweight[apu1] = 0;
      while (fmconfig->listinfo[apu1][apu2] & 0x7f)
	{
	  rowweight[apu1] += (WORD) (fmconfig->listinfo[apu1][apu2] & 0x7f);
	  apu2++;
	}
      if (apu2 > maxrows)
	maxrows = apu2;
      rows[apu1] = apu2 - 1;
      apu1++;
    }
  colums = apu1;
  maxrows--;

  apu3 = 0;
  apu5 = sw + 2 * fmconfig->spacew + fmconfig->spacew / 3;
  for (apu1 = 0; apu1 < colums; apu1++)
    {
      apu2 =
	((LONG) listwidth * (LONG) (fmconfig->listinfo[apu1][0] & 0x7f)) /
	colweight;
      apu2 -= apu5;
      apu2 = (apu2 / fmmain.listxsize) * fmmain.listxsize;
      apu2 += apu5;
      apu3 += apu2;
      newcols[apu1] = apu2;
    }
  sy = 0;
  if (!fmconfig->cmenutype)
    {
      fmmain.realcmenuspace =
	width - apu3 - fmconfig->spacew - fmconfig->spacew / 3;
      fmmain.cmenuwidth = (fmmain.realcmenuspace) / fmconfig->cmenucolumns;
      apu1 = height - 4 * fmconfig->spaceh - 3 * tfh;
      fmmain.cmenuy = fmconfig->spaceh;
      fmmain.totalcmenuwidth = fmmain.realcmenuspace;
      fmmain.cmenugadnumperline = (apu1 / (tfh + 1));
      fmmain.totalcmenuheight = fmmain.cmenugadnumperline * (tfh + 1);
    }
  else
    {
      fmmain.cmenugadnumperline = width / fmmain.cmenuwidth;
      fmmain.cmenux = 0;
      fmmain.totalcmenuwidth = width;
      fmmain.totalcmenuheight = fmmain.realcmenuspace;
      if (fmconfig->cmenutype && !fmconfig->cmenuposition)
	{
	  sy = fmmain.realcmenuspace;
	  fmmain.cmenuy = 0;
	}
    }

  if (fmconfig->flags & MDOUBLED)
    {
      draw3dbox (fmmain.rp, 2, sy + 1, width - 2 * 2, height - 2 * 1 - sy, 1);
      SetAPen (fmmain.rp, fmconfig->mainbackfillpen);
      RectFill (fmmain.rp, 2, sy + 1, width - 2 * 2 + 1, height - 2 * 1);
    }
  else
    {
      draw3dbox (fmmain.rp, 1, sy + 1, width - 2 * 1, height - 2 * 1 - sy, 1);
      SetAPen (fmmain.rp, fmconfig->mainbackfillpen);
      RectFill (fmmain.rp, 1, sy + 1, width - 2 * 1, height - 2 * 1);
    }

  apu3 = 0;
  apu4 = 0;
  apu8 = fmconfig->cmenuposition;
  minwidth = 3 * fmconfig->spacew + sw + 3 * fmmain.listxsize;
  if (minwidth < (4 * widthi + 32))
    minwidth = 4 * widthi + 32;
  minheight = 5 * fmconfig->spaceh + 2 * tfh + sh + 3 * fmmain.listysize;
  ly = 0;
  for (apu1 = 0; apu1 < colums; apu1++)
    {
      if (!apu8 && !fmconfig->cmenutype)
	{
	  apu4 += (fmconfig->spacew / 3 * 2);
	  fmmain.cmenux = apu4;
	  apu4 += fmmain.realcmenuspace;
	  apu4 -= (fmconfig->spacew / 3);
	}
      apu8--;
      apu5 = sy;
      apu6 = newcols[apu1];
      if (apu6 < minwidth)
	apu6 = minwidth;
      apu10 = 0;
      for (apu2 = 0; apu2 < rows[apu1]; apu2++)
	{
	  apu7 =
	    ((LONG) listheight *
	     (LONG) (fmconfig->listinfo[apu1][apu2 + 1] & 0x7f)) /
	    rowweight[apu1];
	  if (apu7 < minheight)
	    apu7 = minheight;
	  if (fmmain.uselist[apu3] > 0)
	    {
	      apu9 = fmmain.uselist[apu3] - 1;
	    }
	  else
	    {
	      for (apu9 = 0; apu9 < WINDOWLISTS; apu9++)
		{
		  if (!fmlist[apu9].li)
		    break;
		}
	    }
	  fmmain.li[apu3] =
	    alloclistgads (apu4, apu5, apu6, apu7,
			   fmconfig->listinfo[apu1][apu2 + 1] & 0x80, apu9,
			   apu3, widthi);
	  if (fmmain.li[apu3])
	    apu10 += fmmain.li[apu3]->height;
	  apu5 += apu7;
	  apu3++;

	}
      if (apu10 > ly)
	ly = apu10;
      apu4 += apu6;
    }
  ly += fmconfig->spaceh + sy;

  if (!fmconfig->cmenutype && apu8 >= 0)
    fmmain.cmenux =
      apu4 + fmmain.ikkuna->BorderLeft + fmconfig->spacew / 3 * 2;

  if (!fmconfig->cmenutype)
    WindowLimits (fmmain.ikkuna,
		  colums * (minwidth + 2 * fmconfig->spacew) +
		  fmmain.realcmenuspace + fmmain.naytto->WBorLeft +
		  fmmain.naytto->WBorRight,
		  maxrows * minheight + 3 * fmconfig->spaceh + 5 * tfh +
		  fmmain.naytto->WBorTop + fmmain.naytto->WBorBottom, -1, -1);
  else
    WindowLimits (fmmain.ikkuna,
		  colums * (minwidth + 2 * fmconfig->spacew) +
		  fmmain.naytto->WBorLeft + fmmain.naytto->WBorRight,
		  maxrows * minheight + fmmain.realcmenuspace +
		  3 * fmconfig->spaceh + 5 * tfh + fmmain.naytto->WBorTop +
		  fmmain.naytto->WBorBottom, -1, -1);

  if (fmconfig->cmenutype && fmconfig->cmenuposition == 1)
    {
      fmmain.cmenuy = ly - 2;
      ly += fmmain.realcmenuspace + 1;
    }
  fmmain.messageliney1 = ly;
  fmmain.messageliney2 = fmmain.messageliney1 + tfh;
  ly += 2 * tfh;

  if (fmconfig->cmenutype && fmconfig->cmenuposition == 2)
    {
      ly++;
      fmmain.cmenuy = ly;
      ly += fmmain.realcmenuspace;
    }

  fmmain.bottomliney = ly + fmconfig->spaceh;
  ly += tfh + fmconfig->spaceh;

  if (fmconfig->cmenutype && fmconfig->cmenuposition == 3)
    {
      ly++;
      fmmain.cmenuy = ly;
    }

  draw3dbox (fmmain.rp, fmconfig->spacew, fmmain.messageliney1 - 1,
	     width - 2 * fmconfig->spacew, 2 * tfh, 0);

  xy = fmmain.gadgetxy;
  apu5 = 0;
  if (!fmconfig->cmenutype)
    {
      apu1 =
	fmmain.cmenux + (fmmain.realcmenuspace -
			 fmmain.cmenuwidth * fmconfig->cmenucolumns) / 2;
      for (apu4 = 0; apu4 < fmconfig->cmenucolumns; apu4++)
	{
	  apu2 = fmmain.cmenuy;
	  for (apu3 = 0; apu3 < fmmain.cmenugadnumperline; apu3++)
	    {
	      *xy++ = apu1 + 1;
	      *xy++ = apu2;
	      *xy++ = fmmain.cmenuwidth - 1;
	      *xy++ = tfh + 1;
	      apu2 += tfh + 1;
	      apu5++;
	      if (apu5 >= TOTALCOMMANDS)
		break;
	    }
	  if (apu5 >= TOTALCOMMANDS)
	    break;
	  apu1 += fmmain.cmenuwidth;
	}
    }
  else
    {
      apu6 = (fmmain.totalcmenuwidth << 4) / fmmain.cmenugadnumperline;
      apu2 = fmmain.cmenuy;
      for (apu3 = 0; apu3 < fmconfig->cmenucolumns; apu3++)
	{
	  apu1 = fmmain.cmenux << 4;
	  for (apu4 = 0; apu4 < fmmain.cmenugadnumperline; apu4++)
	    {
	      xy[0] = apu1 >> 4;
	      xy[1] = apu2;
	      xy[2] = ((apu1 + apu6) >> 4) - xy[0];
	      xy[3] = tfh + 1;
	      xy += 4;
	      apu1 += apu6;
	      apu5++;
	      if (apu5 >= TOTALCOMMANDS)
		break;
	    }
	  if (apu5 >= TOTALCOMMANDS)
	    break;
	  apu2 += tfh + 1;
	}
    }
  *xy = -1;

  apu4 = 0;
  for (apu3 = 0; apu3 < TOTALCOMMANDS; apu3++)
    {
      if (fmconfig->cmenuconfig[apu3].position == 2)
	{
	  textextent (fmmain.rp, fmconfig->cmenuconfig[apu3].label, &apu1,
		      &apu2);
	  apu4 += (apu1 + 2 * fmconfig->spaceh);
	}
    }
  fmmain.bottomlinewidth =
    width - apu4 - fmconfig->spacew - 2 * fmconfig->spacew / 3 * 2;
  draw3dbox (fmmain.rp, fmconfig->spacew, fmmain.bottomliney - 1,
	     fmmain.bottomlinewidth, fmmain.txtfont->tf_YSize + 1, 0);
  fmmain.bottomlinewidth -= 2 * fmconfig->spacew;
  if (fmmain.bottomlinewidth < 0)
    fmmain.bottomlinewidth = 0;
}

void
addlistgads (void)
{
  WORD cnt;

  for (cnt = 0; cnt < WINDOWLISTS; cnt++)
    {
      if (fmmain.li[cnt])
	{
	  AddGList (fmmain.ikkuna, &fmmain.li[cnt]->slider1, -1,
		    LISTGADGETS + 3, 0);
	}
    }
}

void
freelistgads (void)
{
  WORD cnt, cnt2;

  cnt2 = 0;
  for (cnt = 0; cnt < WINDOWLISTS; cnt++)
    {
      fmlist[cnt].li = 0;
      if (fmmain.li[cnt])
	{
	  RemoveGList (fmmain.ikkuna, &fmmain.li[cnt]->slider1,
		       LISTGADGETS + 3);
	  fmmain.uselist[cnt2++] = fmmain.li[cnt]->list->listnumber + 1;
	  freemem (fmmain.li[cnt]->im1.ImageData);
	  freemem (fmmain.li[cnt]->im2.ImageData);
	  freemem (fmmain.li[cnt]->sim1.ImageData);
	  freemem (fmmain.li[cnt]->sim2.ImageData);
	  freemem (fmmain.li[cnt]);
	  fmmain.li[cnt] = 0;
	}
    }
  if (cnt2)
    while (cnt2 != WINDOWLISTS)
      fmmain.uselist[cnt2++] = 0;
}

extern void drawall (struct ReqScrollStruct *);
extern void scrollndraw (LONG, LONG, LONG, LONG, struct ReqScrollStruct *);

struct ListInfo *
alloclistgads (WORD x, WORD y, WORD width, WORD height, WORD mirror,
	       WORD listnum, WORD linum, WORD widthi)
{
  struct Gadget *slider1, *slider2, *string1;
  struct ReqScrollStruct *req1, *req2;
  struct Image *im1, *im2, *sim1, *sim2;
  struct ListInfo *li;
  struct Border *bd1, *bd2, *bd3, *bd4;
  WORD *xy1, *xy2;
  WORD tfh, sw, sh;
  WORD apu1, apu2, apu3, apu4, apu5;

  tfh = fmmain.txtysize + 1;
  sw = fmconfig->sliderw + 2;
  sh = fmconfig->sliderh + 2;

  li = allocvec (0, sizeof (struct ListInfo), MEMF_CLEAR | MEMF_PUBLIC);
  if (!li)
    return (0);
  li->slider1.NextGadget = &li->slider2;
  li->slider2.NextGadget = &li->string;
  li->string.NextGadget = &li->taskgadget;
  li->taskgadget.NextGadget = &li->ltaskgadget;
  li->ltaskgadget.NextGadget = &li->rtaskgadget;

  slider1 = &li->slider1;
  slider2 = &li->slider2;
  string1 = &li->string;
  req1 = &li->rscroll1;
  req2 = &li->rscroll2;
  im1 = &li->im1;
  im2 = &li->im2;
  sim1 = &li->sim1;
  sim2 = &li->sim2;

  slider2->Activation = slider1->Activation =
    GADGIMMEDIATE | RELVERIFY | FOLLOWMOUSE;
  slider2->GadgetType = slider1->GadgetType = PROPGADGET;
  slider1->SpecialInfo = &li->pinfo1;
  slider2->SpecialInfo = &li->pinfo2;
  slider1->GadgetRender = im1;
  slider2->GadgetRender = im2;
  slider1->SelectRender = sim1;
  slider2->SelectRender = sim2;
  slider1->Flags = slider2->Flags = GADGIMAGE | GADGHIMAGE;
  li->pinfo1.Flags = FREEHORIZ | PROPBORDERLESS;
  li->pinfo2.Flags = FREEVERT | PROPBORDERLESS;
  li->pinfo1.HorizBody = 65535;
  li->pinfo1.HorizPot = 65535;
  li->pinfo2.VertBody = 65535;
  li->pinfo2.VertPot = 65535;
  im1->Depth = im2->Depth = fmconfig->mainscreen.depth;
  sim1->Depth = sim2->Depth = fmconfig->mainscreen.depth;
  im1->PlanePick = im2->PlanePick = power2 (fmconfig->mainscreen.depth) - 1;
  sim1->PlanePick = sim2->PlanePick = power2 (fmconfig->mainscreen.depth) - 1;
  im1->Width = sim1->Width = sw - 2;
  im2->Height = sim2->Height = sh - 2;

  string1->Flags = GFLG_STRINGEXTEND;
  string1->Activation = RELVERIFY;
  string1->GadgetType = STRGADGET;
  string1->SpecialInfo = (APTR) & li->sinfo;
  li->sinfo.Buffer = fmlist[listnum].path;
  li->sinfo.UndoBuffer = fmmain.undobuf;
  li->sinfo.Extension = &li->stext;
  li->sinfo.MaxChars = 510;
  li->stext.Font = fmmain.txtfont;
  li->stext.Pens[0] = li->stext.ActivePens[0] = fmconfig->stringpen;
  li->stext.Pens[1] = li->stext.ActivePens[1] = fmconfig->backpen;

  slider1->GadgetID = linum + 101;
  slider2->GadgetID = linum + 111;
  string1->GadgetID = linum + 201;

  req1->gfxbase = req2->gfxbase = GfxBase;
  req1->VersionNumber = req2->VersionNumber = 2;
  req1->RedrawAll = req2->RedrawAll = (APTR) & drawall;
  req1->ScrollAndDraw = req2->ScrollAndDraw = (APTR) & scrollndraw;
  req1->flags = 0;
  req2->flags = 1;
  req1->PropGadget = slider1;
  req2->PropGadget = slider2;
  if (fmconfig->flags & MVSCROLL)
    req1->LineSpacing = 1;
  else
    req1->LineSpacing = fmmain.listysize;
  if (fmconfig->flags & MHSCROLL)
    req2->LineSpacing = 1;
  else
    req2->LineSpacing = fmmain.listxsize;

  req1->fmlist = req2->fmlist = &fmlist[listnum];

  li->list = &fmlist[listnum];
  li->linumber = linum;
  fmlist[listnum].li = li;

  li->visiblelinesw =
    (width - 2 * fmconfig->spacew - sw -
     fmconfig->spacew / 3) / fmmain.listxsize;
  li->visiblelines =
    (height - 5 * fmconfig->spaceh - 2 * tfh - sh) / fmmain.listysize;
  req1->NumLines = li->visiblelines;
  req2->NumLines = li->visiblelinesw;
  li->x = x;
  li->y = y;
  li->dirwidth = li->visiblelinesw * fmmain.listxsize;
  li->dirheight = li->visiblelines * fmmain.listysize;
  li->diry = 2 * fmconfig->spaceh + tfh + 1 + y;
  if (!mirror)
    li->dirx = 2 * fmconfig->spacew + sw + 1 + x;
  else
    li->dirx = fmconfig->spacew + 1 + x;
  li->edirx = li->dirx + li->dirwidth;
  li->ediry = li->diry + li->dirheight;
  draw3dbox (fmmain.rp, li->dirx - 1, li->diry - 1, li->dirwidth + 2,
	     li->dirheight + 1, 0);
  li->topliney = fmconfig->spaceh + 1 + y;
  draw3dbox (fmmain.rp, li->dirx - 1, li->topliney - 1, li->dirwidth + 2, tfh,
	     0);

  if (!mirror)
    slider1->LeftEdge = apu1 = fmconfig->spacew + 1 + x;
  else
    slider1->LeftEdge = apu1 =
      2 * fmconfig->spacew + li->dirwidth + 2 + 1 + x;
  slider1->TopEdge = apu2 = li->diry;
  slider1->Width = apu3 = sw - 2;
  slider1->Height = apu4 = li->dirheight - 1;
  apu5 = RASSIZE (sw, apu4 + 2) * im1->Depth;
  im1->ImageData = allocvec (0, apu5, MEMF_CLEAR | MEMF_CHIP);
  sim1->ImageData = allocvec (0, apu5, MEMF_CLEAR | MEMF_CHIP);
  draw3dbox (fmmain.rp, apu1 - 1, apu2 - 1, apu3 + 2, apu4 + 2, 0);
  if (!mirror)
    slider2->LeftEdge = apu1 = fmconfig->spacew * 2 + sw + 1 + x;
  else
    slider2->LeftEdge = apu1 = fmconfig->spacew + 1 + x;
  slider2->TopEdge = apu2 = li->diry + (li->dirheight + 1) + fmconfig->spaceh;
  slider2->Width = apu3 = (li->dirwidth + 2) - 2;
  slider2->Height = apu4 = sh - 2;
  apu5 = RASSIZE (apu3, sh + 2) * im2->Depth;
  im2->ImageData = allocvec (0, apu5, MEMF_CLEAR | MEMF_CHIP);
  sim2->ImageData = allocvec (0, apu5, MEMF_CLEAR | MEMF_CHIP);
  draw3dbox (fmmain.rp, apu1 - 1, apu2 - 1, apu3 + 2, apu4 + 2, 0);

  bd1 = &li->bord1;
  bd2 = &li->bord2;
  bd3 = &li->bord3;
  bd4 = &li->bord4;
  xy1 = li->xy1;
  xy2 = li->xy2;
  string1->Height = apu4 = tfh - 1;
  string1->TopEdge = apu2 =
    li->diry + (li->dirheight + 1) + 2 * fmconfig->spaceh + sh;
  apu2 -= 2;
  apu4 += 4;

  li->width = width;
  li->height = height;
  li->ex = x + width;
  li->ey = y + height;

  apu5 = fmconfig->spacew * 2 / 3 + x;

  li->ltaskgadget.TopEdge = li->taskgadget.TopEdge = li->rtaskgadget.TopEdge =
    apu2;
  li->ltaskgadget.Width = li->taskgadget.Width = li->rtaskgadget.Width =
    widthi - 1;
  li->ltaskgadget.Height = li->taskgadget.Height = li->rtaskgadget.Height =
    apu4 - 1;
  li->ltaskgadget.GadgetType = li->taskgadget.GadgetType =
    li->rtaskgadget.GadgetType = BOOLGADGET;
  li->ltaskgadget.Activation = li->taskgadget.Activation =
    li->rtaskgadget.Activation = RELVERIFY;
  li->ltaskgadget.Flags = li->taskgadget.Flags = li->rtaskgadget.Flags =
    GADGHIMAGE;
  li->ltaskgadget.GadgetRender = li->taskgadget.GadgetRender =
    li->rtaskgadget.GadgetRender = bd1;
  li->ltaskgadget.SelectRender = li->taskgadget.SelectRender =
    li->rtaskgadget.SelectRender = bd3;

  li->ltaskgadget.LeftEdge = apu5;
  li->ltaskgadget.GadgetID = linum + 411;
  li->ltaskgadget.UserData = (APTR) '<';
  apu5 += widthi;
  li->taskgadget.LeftEdge = apu5;
  li->taskgadget.GadgetID = linum + 401;
  li->taskgadget.UserData = (APTR) (IPTR)linum;
  apu5 += widthi;
  li->rtaskgadget.LeftEdge = apu5;
  li->rtaskgadget.GadgetID = linum + 421;
  li->rtaskgadget.UserData = (APTR)'>';
  apu5 += widthi;
  doborder (xy1, bd1, bd2, xy2, bd3, bd4, apu4, widthi);

  apu5 += fmconfig->spacew * 2 / 3;
  string1->LeftEdge = apu1 = apu5;
  apu3 = (2 * fmconfig->spacew + (li->dirwidth + 2) + sw) - apu5 + x;
  string1->Width = (apu3 / fmmain.txtxsize) * fmmain.txtxsize;
  draw3dbox (fmmain.rp, apu1, apu2 + 1, apu3, apu4 - 2, 0);

  if (!im1->ImageData || !im2->ImageData || !sim1->ImageData
      || !sim2->ImageData)
    {
      freemem (im1->ImageData);
      freemem (im2->ImageData);
      freemem (sim1->ImageData);
      freemem (sim2->ImageData);
      freemem (li);
      li = 0;
    }
  return (li);
}

struct TextFont *

openfont (struct TextAttr *ta)
{
  struct TextFont *tf;
  extern struct Library *DiskfontBase;
  APTR winptr;

  winptr = fmmain.myproc->pr_WindowPtr;
  fmmain.myproc->pr_WindowPtr = (APTR) - 1;
  tf = OpenFont (ta);
  if (tf)
    goto retu;
  DiskfontBase = OpenLibrary (dflib, 37);
  if (!DiskfontBase)
    goto normal;
  tf = OpenDiskFont (ta);
  CloseLibrary (DiskfontBase);
  if (tf)
    goto retu;
normal:
  ta->ta_YSize = 8;
  ta->ta_Style = 0;
  ta->ta_Flags = 0;
  strcpy (ta->ta_Name, topaz);
  tf = OpenFont (ta);
retu:
  fmmain.myproc->pr_WindowPtr = winptr;
  return (tf);
}

void
docmenusize (void)
{
  WORD apu1, apu2, cnt, txoffset;
  struct RastPort rp;

  InitRastPort (&rp);
  SetFont (&rp, fmmain.txtfont);
  apu1 = 0;
  for (cnt = 0; cnt < TOTALCOMMANDS; cnt++)
    {
      textextent (&rp, fmconfig->cmenuconfig[cnt].label, &apu2, &txoffset);
      if (apu1 < apu2)
	apu1 = apu2;
    }
  fmmain.cmenuwidth = apu1 + 6;
}

void
setcmenu (void)
{
  WORD bottomx, cmenudelay;
  WORD cnt2, apu1, apu2;
  UBYTE *ptr1;
  struct Gadget *gad;
  struct Gadget *pgad = 0;
  struct Border *bo1, *bo2, *bo3, *bo4;
  WORD *xy1, *xy2, *xy;
  struct CMenuConfig *cmc;

  freegadgets (fmmain.firstgadget);
  fmmain.firstgadget = 0;
  bottomx =
    fmmain.ikkuna->Width - fmmain.ikkuna->BorderLeft -
    fmmain.ikkuna->BorderRight - fmconfig->spacew / 3 * 2;
  cmenudelay = fmmain.cmenu;
  ObtainSemaphore (&fmmain.gfxsema);
  SetDrMd (fmmain.rp, JAM1);
  cnt2 = 0;
  cmc = &fmconfig->cmenuconfig[0];
  xy = fmmain.gadgetxy;
  while (*xy >= 0)
    {
      ptr1 =
	allocvec (0,
		  sizeof (struct Gadget) + sizeof (struct Border) * 4 +
		  4 * (2 * 5), MEMF_CLEAR | MEMF_PUBLIC);
      gad = (struct Gadget *) ptr1;

      if (fmmain.firstgadget == 0)
	{
	  fmmain.firstgadget = gad;
	}
      else
	{
	  pgad->NextGadget = gad;
	}
      pgad = gad;
      gad->Activation = RELVERIFY;
      gad->GadgetType = BOOLGADGET;
      gad->Flags = GADGHIMAGE;
      ptr1 += sizeof (struct Gadget);
      bo1 = (struct Border *) ptr1;
      bo2 = (struct Border *) (ptr1 + sizeof (struct Border));
      bo3 = (struct Border *) (ptr1 + 2 * sizeof (struct Border));
      bo4 = (struct Border *) (ptr1 + 3 * sizeof (struct Border));
      xy1 = (WORD *) (ptr1 + 4 * sizeof (struct Border));
      xy2 = (WORD *) (ptr1 + 4 * sizeof (struct Border) + 2 * 5 * 2);

      while (cnt2 < TOTALCOMMANDS)
	{
	  if (cmc->position == 1)
	    {
	      if (cmenudelay)
		{
		  cnt2 += cmenudelay;
		  cmc += cmenudelay;
		  cmenudelay = 0;
		  continue;
		}
	      gad->LeftEdge = *xy++;
	      gad->TopEdge = *xy++;
	      gad->Width = *xy++;
	      gad->Height = *xy++;
	      break;
	    }
	  else if (cmc->position == 2)
	    {
	      gad->TopEdge = fmmain.bottomliney - 2;
	      textextent (fmmain.rp, cmc->label, &apu1, &apu2);
	      gad->Width = (apu1 + 2 * fmconfig->spaceh);
	      gad->Height = fmmain.txtysize + 2 + 1;
	      bottomx -= gad->Width;
	      gad->LeftEdge = bottomx;
	      break;
	    }
	  cmc++;
	  cnt2++;
	}
      if (cnt2 >= TOTALCOMMANDS)
	{
	  gad->LeftEdge = *xy++;
	  gad->TopEdge = *xy++;
	  gad->Width = *xy++;
	  gad->Height = *xy++;
	}

      doborder (xy1, bo1, bo2, xy2, bo3, bo4, gad->Height, gad->Width);
      gad->GadgetRender = bo1;
      gad->SelectRender = bo3;

      if (cnt2 < TOTALCOMMANDS)
	SetAPen (fmmain.rp, cmc->backpen);
      else
	SetAPen (fmmain.rp, fmconfig->backpen);
      if (fmconfig->flags & MDOUBLED)
	RectFill (fmmain.rp, gad->LeftEdge + 2, gad->TopEdge + 1,
		  gad->Width + gad->LeftEdge - 3,
		  gad->Height + gad->TopEdge - 2);
      else
	RectFill (fmmain.rp, gad->LeftEdge + 1, gad->TopEdge + 1,
		  gad->Width + gad->LeftEdge - 2,
		  gad->Height + gad->TopEdge - 2);
      Move (fmmain.rp, gad->LeftEdge + fmconfig->spaceh,
	    gad->TopEdge + fmmain.txtbaseline + 1);
      if (cnt2 < TOTALCOMMANDS)
	{
	  if (cmc->cmenucount != EMPTYCONFIG)
	    {
	      SetAPen (fmmain.rp, cmc->frontpen);
	      Text (fmmain.rp, cmc->label, strlen (cmc->label));
	    }
	  cmc++;
	  cnt2++;
	}
    }
  SetDrMd (fmmain.rp, JAM2);
  AddGList (fmmain.ikkuna, fmmain.firstgadget, -1, -1, 0);
  RefreshGList (fmmain.firstgadget, fmmain.ikkuna, 0, -1);
  ReleaseSemaphore (&fmmain.gfxsema);
  gadgettables ();
}

void
gadgettables (void)
{
  struct CMenuConfig *cmc;
  struct GadKeyTab *gkt;
  struct Gadget *g;
  WORD cnt1, apu2;

  gkt = &fmmain.gadkeytab[0];
  memseti (gkt, 0,
	   sizeof (struct GadKeyTab) * (WINDOWLISTS * LISTGADGETS +
					TOTALCOMMANDS));

  for (cnt1 = 0; cnt1 < WINDOWLISTS; cnt1++)
    {
      if (fmmain.li[cnt1])
	{
	  apu2 = fmmain.li[cnt1]->list->listnumber;
	  gkt->g = &fmmain.li[cnt1]->taskgadget;
	  gkt->key[0] = fmmain.li[cnt1]->list->listnumber + '0';
	  gkt->listnum = apu2;
	  gkt++;
	  gkt->g = &fmmain.li[cnt1]->ltaskgadget;
	  gkt->key[0] = '<';
	  gkt->listnum = apu2;
	  gkt->onelist = 1;
	  gkt++;
	  gkt->g = &fmmain.li[cnt1]->rtaskgadget;
	  gkt->key[0] = '>';
	  gkt->listnum = apu2;
	  gkt->onelist = 1;
	  gkt++;
	}
      else
	{
	  gkt->unused = 1;
	  gkt++;
	  gkt->unused = 1;
	  gkt++;
	  gkt->unused = 1;
	  gkt++;
	}
    }
  g = fmmain.firstgadget;
  apu2 = 0;
  for (cnt1 = 0; cnt1 < TOTALCOMMANDS; cnt1++)
    {
      cmc = &fmconfig->cmenuconfig[cnt1];
      if (g
	  && (cmc->position == 2
	      || (cmc->position == 1 && apu2 >= fmmain.cmenu)))
	{
	  gkt->g = g;
	  g->GadgetID = cnt1 + 301;
	  g = g->NextGadget;
	}
      else
	{
	  gkt->g = (struct Gadget *) 2L;
	}
      if (cmc->position == 1)
	apu2++;
      gkt->listnum = -1;
      gkt->cmc = &fmconfig->cmenuconfig[cnt1];
      gkt++;
    }
}

void
doborder (WORD * xy1, struct Border *bo1, struct Border *bo2, WORD * xy2,
	  struct Border *bo3, struct Border *bo4, WORD h, WORD w)
{
  h--;
  w--;
  if (fmconfig->flags & MDOUBLED)
    {
      xy1[3] = h;
      xy1[4] = 1;
      xy1[5] = h;
      xy1[6] = 1;
      xy1[8] = w;
      xy2[0] = w;
      xy2[1] = h;
      xy2[2] = w;
      xy2[4] = w - 1;
      xy2[5] = 1;
      xy2[6] = xy2[4];
      xy2[7] = h;
      xy2[8] = 1;
      xy2[9] = h;
    }
  else
    {
      xy1[5] = xy1[7] = h;
      xy1[2] = xy1[4] = w;
      xy1[0] = xy1[1] = xy1[3] = xy1[8] = xy1[9] = 0;
/*
	xy1[1]=h;
	xy1[4]=w;
	xy1[3]=xy1[5]=xy1[6]=xy1[8]=0;
*/
      xy2[0] = w;
      xy2[1] = 1;
      xy2[2] = w;
      xy2[3] = h;
      xy2[4] = 1;
      xy2[5] = h;
      xy2[6] = xy2[8] = xy2[4];
      xy2[7] = xy2[9] = xy2[5];
    }
  bo1->FrontPen = bo4->FrontPen = fmconfig->whitepen;
  bo1->Count = bo2->Count = bo3->Count = bo4->Count = 5;
  bo1->XY = xy1;
  bo1->NextBorder = bo2;
  bo2->FrontPen = bo3->FrontPen = fmconfig->blackpen;
  bo2->XY = xy2;
  bo3->XY = xy1;
  bo3->NextBorder = bo4;
  bo4->XY = xy2;

}

void
textextent (struct RastPort *rp, UBYTE * ptr, WORD * width, WORD * offset)
{
  struct TextExtent te;

  TextExtent (rp, ptr, strlen (ptr), &te);
  *offset = te.te_Extent.MinX;
  *width = te.te_Extent.MaxX + te.te_Extent.MinX;
}

void
textextentuc (struct RastPort *rp, UBYTE * ptr, WORD * width, WORD * offset)
{
  struct TextExtent te;
  UBYTE varaptr[100];

  copyus (varaptr, ptr);
  TextExtent (rp, varaptr, strlen (varaptr), &te);
  *width = te.te_Extent.MaxX + te.te_Extent.MinX;
  *offset = te.te_Extent.MinX;
}

void
draw3dbox (struct RastPort *rp, WORD x, WORD y, WORD w, WORD h, WORD type)
{
  UBYTE col1, col2;

/* 1=raised 0=lowered */
  SetAPen (rp, fmconfig->backpen);
  RectFill (rp, x, y, x + w, y + h);
  if (type)
    {
      col1 = fmconfig->whitepen;
      col2 = fmconfig->blackpen;
    }
  else
    {
      col1 = fmconfig->blackpen;
      col2 = fmconfig->whitepen;
    }
  x--;
  y--;
  w++;
  h++;
  if (fmconfig->flags & MDOUBLED)
    {
      x--;
      w += 2;
    }
  Move (rp, x, y);
  SetAPen (rp, col1);
  Draw (rp, x + w, y);
  SetAPen (rp, col2);
  Draw (rp, x + w, y + h);
  Draw (rp, x, y + h);
  SetAPen (rp, col1);
  Draw (rp, x, y);
  if (fmconfig->flags & MDOUBLED)
    {
      Move (rp, x + 1, y);
      Draw (rp, x + 1, y + h - 1);
      SetAPen (rp, col2);
      Move (rp, x + w - 1, y + 1);
      Draw (rp, x + w - 1, y + h);
    }
}


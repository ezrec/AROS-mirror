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

#define CATCOMP_ARRAY 
#include <libraries/reqtools.h>
#include <aros/debug.h>
#include <aros/bigendianio.h>
#include <graphics/gfxbase.h>
#include <workbench/icon.h>
#include <devices/inputevent.h>
#include <devices/timer.h>   
#include <libraries/commodities.h>
#include <dos/rdargs.h>  
#include <string.h> 
#include <proto/timer.h>
#include <aros/libcall.h>
#include <proto/icon.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <aros/debug.h>
#include <dos/dosextens.h>
#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <libraries/locale.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <stdio.h>
#include <strings.h>
#include <graphics/gfxbase.h>

#ifndef AROS
#include <libraries/xfdmaster.h>
#endif

#include <datatypes/datatypes.h>
#include "fmnode.h"
#include "fmlocale.h"
#include "child.h"

#include "filemaster_strings.h"
#define OK        (0)
struct timerequest * timeRequest = NULL;
struct MsgPort * timePort;
void gquit (void);
void gsleep (void);
void gstod (void);
void gconfig (void);
void parse (void);

void gdrives (void);
void gparent (void);
void ginvert (void);
void gclear (void);
void operate (void);
void copyfiles (void);
void movefiles (void);
void deletefiles (void);
void createdir (void);
void showtextfile (void);
void showpic (void);
void playmod (void);
void diskinfo (void);
void execute (void);
void disksize (void);
void modinfo (void);
void playsamplefile (void);
void fileedit (void);
void diskedit (void);
void about (void);

/*void registering(void);*/

UBYTE doslib[] = { "dos.library" };
UBYTE gfxlib[] = { "graphics.library" };
UBYTE intlib[] = { "intuition.library" };
UBYTE wblib[] = { "workbench.library" };
UBYTE gadlib[] = { "gadtools.library" };
UBYTE utilib[] = { "utility.library" };
UBYTE keylib[] = { "keymap.library" };
UBYTE loclib[] = { "locale.library" };
UBYTE dflib[] = { "diskfont.library" };
UBYTE icolib[] = { "icon.library" };
UBYTE verlib[] = { "version.library" };
UBYTE reqlib[] = { "reqtools.library" };
UBYTE xfdlib[] = { "xfdmaster.library" };
UBYTE datlib[] = { "datatypes.library" };
UBYTE comlib[] = { "commodities.library" };
UBYTE bguilib[] = { "bgui.library" };

UBYTE auddev[] = { "audio.device" };
UBYTE workbench[] = { "Workbench" };

UBYTE topaz[] = { "topaz.font" };

void *launchtable[BUILDCOMMANDS + 1];

void readconfig (void);
int main (void);
void fmclock (void);
static WORD init (void);

WORD deloldexe (WORD);
void initexes (WORD);

void initpathlist (void);
void freepathlist (void);
void event (WORD);
WORD avaanaytto (WORD);
WORD suljenaytto (WORD);
void teeruutu (void);
void clearlisti (struct List *);
void drives (struct FMList *);
void getdir (void);
void stopoldmod (void);
ULONG power2 (WORD);
WORD errorreq (WORD, UBYTE *, WORD, WORD);
void recalc (void);

UBYTE defcolors[8 * 3] = {
  0xaa, 0xaa, 0xaa,
  0x00, 0x00, 0x00,
  0xff, 0xff, 0xff,
  0x66, 0x88, 0xbb,
  0xff, 0x00, 0x00,
  0x00, 0xff, 0x00,
  0x00, 0x00, 0xff,
  0xff, 0xff, 0x00
};

extern UBYTE fmname[];
extern UBYTE clockformatstring[];
UBYTE space[] =
  {
"                                                                                                                                                                                                   "
};
UBYTE nformatstring[20];
UBYTE nformatstring2[20];
struct FMMain fmmain;
struct FMConfig *fmconfig;
struct FMList fmlist[LISTS];

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;

#ifdef DETACH
struct ExecBase *SysBase;
struct DosLibrary *DOSBase;
#else

extern struct ExecBase *SysBase;
extern struct DosLibrary *DOSBase;
#endif
struct UtilityBase *UtilityBase;
struct Library *GadToolsBase;
struct Library *DiskfontBase;
struct Library *KeymapBase;
struct Library *WorkbenchBase;
struct Library *IconBase;
struct Device * TimerBase;
struct Library *DataTypesBase;
struct Library *CxBase;
struct Locale *locale;
struct Catalog *catalog;
struct xfdMasterBase *xfdMasterBase;
struct Library *BGUIBase;


int
main (void)
{
  WORD aa;
  BPTR prevlock;
  BPTR handle;
  UBYTE *errptr = 0;
  UBYTE *ptr1;
  UBYTE prgname[200];
  UBYTE prgname2[100];
  struct timerequest tr;
  WORD sleep = 0;

#ifdef DETACH
  SysBase = (*((struct ExecBase **) 4));
  DOSBase = (struct DosLibrary *) OpenLibrary (doslib, 0);
#endif

  GetProgramName (prgname, 200);
  SetProgramName ("FM Main");
  fmmain.kick = SysBase->LibNode.lib_Version;
  fmmain.myproc = (struct Process *) FindTask (0);
  if (SetTaskPri ((struct Task *) fmmain.myproc, 5) == 4)
    sleep = 1;
#ifndef V39
  GfxBase = (struct GfxBase *) OpenLibrary (gfxlib, 37);
  IntuitionBase = (struct IntuitionBase *) OpenLibrary (intlib, 37);
  CxBase = OpenLibrary (comlib, 37);
  UtilityBase = OpenLibrary (utilib, 37);
  GadToolsBase = OpenLibrary (gadlib, 37);
  KeymapBase = OpenLibrary (keylib, 0);
  WorkbenchBase = OpenLibrary (wblib, 37);
  LocaleBase = OpenLibrary (loclib, 38);
  if (LocaleBase)
    locale = OpenLocale (0);
  BGUIBase = OpenLibrary (bguilib, 41);
#else
  GfxBase = (struct GfxBase *) OpenLibrary (gfxlib, 39);
  IntuitionBase = (struct IntuitionBase *) OpenLibrary (intlib, 39);
  CxBase = OpenLibrary (comlib, 37);
  UtilityBase = (struct UtilityBase *)OpenLibrary (utilib, 39);
  GadToolsBase = OpenLibrary (gadlib, 39);
  KeymapBase = OpenLibrary (keylib, 0);
  WorkbenchBase = OpenLibrary (wblib, 37);

  if ((LocaleBase = (struct LocaleBase *)OpenLibrary (loclib, 38)))
    {
      if (!(locale = OpenLocale (0)))
	{
	  errptr = "Couldn't open current default locale";
	  goto error2;
	}
    }

  DataTypesBase = OpenLibrary (datlib, 39);
  BGUIBase = OpenLibrary (bguilib, 41);
#endif
  fmconfig = allocvec (0, sizeof (struct FMConfig), MEMF_CLEAR | MEMF_PUBLIC);
  if (!fmconfig)
    goto error2;
  memseti (&tr, 0, sizeof (struct timerequest));

D(bug("timer...........\n"));  

                /* Set up the timer.device interface. */
  timePort = CreateMsgPort();
                if(timePort != NULL)
                {
                    timeRequest = (struct timerequest *)CreateIORequest(timePort,sizeof(*timeRequest));
                    if(timeRequest != NULL)
                    {
                        if(OpenDevice(TIMERNAME,UNIT_VBLANK,(struct IORequest *)timeRequest,0) == OK)
                            TimerBase = timeRequest->tr_node.io_Device;
			else 
				goto error2;
                    }
		   	 else
			goto error2;
		}
		else
		goto error2;	


//if(OpenDevice("timer.device",UNIT_MICROHZ,(struct IORequest*)&tr,0L)) 
//printf("timer.device error");
//goto error2;
//TimerBase=(struct Library*)tr.tr_node.io_Device;

D(bug("fungerar..........\n"));
  if (!GfxBase || !IntuitionBase || !UtilityBase || !GadToolsBase
      || !KeymapBase || !WorkbenchBase)
    goto error2;

  if (!BGUIBase)
    {
      errorreq (MSG_MAIN_WARNING, "No bgui.library v41 or better found", 0,
		MSG_OK);
      goto error2;
    }
  if (FindPort (FMPORTNAME))
    {
      if (!errorreq
	  (MSG_MAIN_AREYOUSURE, getstring (MSG_MAIN_FMALREADY), MSG_YES,
	   MSG_NO))
	goto error2;
    }
  IconBase = OpenLibrary (icolib, 37);
  if (IconBase)
    {
      strcpy (prgname2, "PROGDIR:");
      strcpy (prgname2 + 8, FilePart (prgname));

#ifndef AROS
   fmmain.dobj = GetDiskObjectNew (prgname2);
#else
    fmmain.dobj = GetDiskObject (prgname2);    
#endif
    }
#ifndef AROS
  xfdMasterBase = (struct xfdMasterBase *) OpenLibrary (xfdlib, 36);
#endif
  if (!init ())
    goto error4;
  fmmain.fmport =
    allocvec (0, sizeof (struct FMPort) + 32, MEMF_CLEAR | MEMF_PUBLIC);
  ptr1 = (UBYTE *) (fmmain.fmport + 1);
  if (!fmmain.fmport)
    goto error4;
  fmmain.fmport->msgport.mp_Node.ln_Type = NT_MSGPORT;
  fmmain.fmport->msgport.mp_Node.ln_Pri = 5;
  fmmain.fmport->msgport.mp_Node.ln_Name = ptr1;
  strcpy (ptr1, FMPORTNAME);
  fmmain.fmport->msgport.mp_Flags = PA_IGNORE;
  fmmain.fmport->msgport.mp_SigTask = fmmain.myproc;
  fmmain.fmport->fmmain = &fmmain;
  fmmain.fmport->fmconfig = fmconfig;
  AddPort (&fmmain.fmport->msgport);
  if (!sleep)
    {
D(bug("fm.c 283\n")); 
     if (!avaanaytto (3))
	 goto error5;
       if (!fmmain.regname[0])
	{
D(bug("launch about? \n"));
	launch ((APTR) & about, getconfignumber (ABOUTCONFIG), 0, MSUBPROC);
D(bug("back from launch? \n"));  
	}
    }
  fmconfig->flags |= MSUBPROC | MHSCROLL | MVSCROLL;
  fmmain.clock = 1;
D(bug("launch fmclock? \n"));  

  if (!launch ((APTR) & fmclock, 0, 1, MSUBPROC))
  goto error5;

D(bug("back from launch? \n"));  
  fmmain.clock = -1;

  for (;;)
    {
D(bug("fm.c 325 \n"));  
      event (sleep);
D(bug("fm.c 327 \n"));  
     sleep = 0;
      if (suljenaytto (3))
	break;
    }
D(bug("back from suljenaytto ? fm.c 332\n"));  

error5:
  fmmain.kill = 1;

  if (fmmain.clock < 0)
    {
      fmmain.clock = 0;
D(bug("fm.c 344 \n"));
      while (!fmmain.clock)
	WaitTOF ();
D(bug("fm.c 347 \n")); 
    }

  stopoldmod ();

error4:
  if (fmmain.oldcur != (BPTR)1)
    {
D(bug("fm.c 352 \n")); 
      prevlock = CurrentDir (fmmain.oldcur);
      if (prevlock)
	UnLock (prevlock);
    }
D(bug("fm.c 357 \n")); 

  deloldexe (1);

  for (aa = 0; aa < LISTS; aa++)

    clearlist (&fmlist[aa]);
  clearlisti ((struct List *) &fmmain.dlist);
  freepathlist ();

  if (fmmain.pool)
    DeletePool (fmmain.pool);
D(bug("fm.c 371 \n")); 

  if (fmmain.fmport)
    {
      RemPort (&fmmain.fmport->msgport);
      freemem (fmmain.fmport);
    }
  if (fmmain.dobj)
    FreeDiskObject (fmmain.dobj);
error2:
  if (fmconfig)
    {
      for (aa = 0; aa < TOTALCOMMANDS; aa++)
	freemem (fmconfig->cmenuconfig[aa].moreconfig);
      freemem (fmconfig);
    }
  CloseLibrary ((struct Library *) xfdMasterBase);

  if (DataTypesBase)
    CloseLibrary (DataTypesBase);

  if (LocaleBase)
    {
      CloseLocale (locale);
      CloseCatalog (catalog);
      CloseLibrary ((struct Library *)LocaleBase);
    }

  if (TimerBase)
    CloseDevice ((struct IORequest *) &tr);
  CloseLibrary (CxBase);
  CloseLibrary ((struct Library *)UtilityBase);
  CloseLibrary (GadToolsBase);
  CloseLibrary ((struct Library *) GfxBase);
  CloseLibrary ((struct Library *) IntuitionBase);
  CloseLibrary (KeymapBase);
  CloseLibrary (WorkbenchBase);
  CloseLibrary (IconBase);
  CloseLibrary (DiskfontBase);
  CloseLibrary (BGUIBase);

  if (errptr)
    {
      if ((handle = Open ("CON:20/100/600/50/Fatal error", 1005)))
	{
	  Write (handle, "\n ", 2);
	  Write (handle, errptr, strlen (errptr));
	  Write (handle, "\n", 1);
	  Delay (200);
	  Close (handle);
	}
    }
  fmmain.myproc->pr_WindowPtr = fmmain.oldwinptr;

#ifdef DETACH
  CloseLibrary ((struct Library *) DOSBase);
#endif

  D(bug("fm.c main end...... \n"));
  return 0;
} /** main ends ****/




#ifndef AROS
void __asm formathook (void);
#endif

static WORD
init (void)
{
  APTR winptr;
  WORD aa;
  UBYTE *ptr1, *ptr3;
  struct Screen *wbscr;
  UBYTE teststring1[34];
  WORD apu1;
  WORD height, width;
  ULONG screenmode;
  UWORD apu2;
  struct DateStamp ds;
  struct CMenuConfig *cmc;
#ifdef V39
  ULONG ctable[3];
#else
  LONG apu3;
#endif
D(bug("Init...........\n"));    
  memseti (launchtable, 0, sizeof (launchtable));
  launchtable[QUITCONFIG] = (void *) gquit;
  launchtable[SLEEPCONFIG] = (void *) gsleep;
  launchtable[STODCONFIG] = (void *) gstod;
  launchtable[CONFIGCONFIG] = (void *) &gconfig;
  launchtable[EXCLUDECONFIG] = (void *) parse;
  launchtable[INCLUDECONFIG] = (void *) parse;
  launchtable[PARSECONFIG] = (void *) parse;
  launchtable[DRIVESCONFIG] = (void *) gdrives;
  launchtable[PARENTCONFIG] = (void *) gparent;
  launchtable[INVERTCONFIG] = (void *) ginvert;
  launchtable[CLEARCONFIG] = (void *) gclear;
  launchtable[OPERATECONFIG] = (void *) operate;
  launchtable[COPYCONFIG] = (void *) copyfiles;
  launchtable[COPYASCONFIG] = (void *) copyfiles;
  launchtable[MOVECONFIG] = (void *) movefiles;
  launchtable[DELETECONFIG] = (void *) deletefiles;
  launchtable[MAKEDIRCONFIG] = (void *) createdir;
  launchtable[SHOWASCCONFIG] = (void *) showtextfile;
  launchtable[SHOWHEXCONFIG] = (void *) showtextfile;
  launchtable[SHOWPICCONFIG] = (void *) showpic;
  launchtable[PLAYMODCONFIG] = (void *) playmod;
  launchtable[DISKINFOCONFIG] = (void *) diskinfo;
  launchtable[EXECUTECONFIG] = (void *) execute;
  launchtable[DISKSIZECONFIG] = (void *) disksize;
  launchtable[MODINFOCONFIG] = (void *) modinfo;
  launchtable[HEARCONFIG] = (void *) playsamplefile;
  launchtable[FILEEDITCONFIG] = (void *) fileedit;
  launchtable[DISKEDITCONFIG] = (void *) diskedit;
  launchtable[ABOUTCONFIG] = (void *) about;
  launchtable[REGISTERCONFIG] = 0;	/*(void*)registering; */
  launchtable[EMPTYCONFIG] = 0;
  DateStamp (&ds);

  for (aa = 0; aa < LISTS; aa++)
    {
      NewList ((struct List *) &fmlist[aa]);
      fmlist[aa].listnumber = aa;
    }

  NewList ((struct List *) &fmmain.dlist);
  InitSemaphore (&fmmain.gfxsema);
  InitSemaphore (&fmmain.msgsema);
  InitSemaphore (&fmmain.poolsema);
D(bug("InitSemaphore ...........\n"));
  
  fmmain.version = fmname[4];
  fmmain.revision = fmname[5];
  fmmain.betaversion = fmname[6];
  fmmain.betarevision = fmname[7];
  fmmain.sourcedir = &fmlist[0];
  fmmain.destdir = &fmlist[1];
  fmmain.oldcur = (BPTR)1;
D(bug("fmmain init ...........\n"));  
  CopyMem (defcolors, fmconfig->colors, 8 * 3);
  fmconfig->mainscreen.depth = 2;
  fmconfig->mainscreen.overscan = 1;

  if ((wbscr = LockPubScreen (workbench)))
    {
      UBYTE depth;
      screenmode = GetVPModeID (&wbscr->ViewPort);
      if (screenmode == INVALID_ID)
	screenmode = 0;
      height = wbscr->Height;
      width = wbscr->Width;
      depth = GetBitMapAttr(wbscr->RastPort.BitMap, BMA_DEPTH);
      fmconfig->mainscreen.depth = depth > 4 ? 4 : depth;
      if (wbscr->Flags & AUTOSCROLL)
	fmconfig->mainscreen.autoscroll = 1;
      apu2 = 0;
      for (apu1 = 0; apu1 < power2 (fmconfig->mainscreen.depth); apu1++)
	{
#ifdef V39
	  GetRGB32 (wbscr->ViewPort.ColorMap, apu1, 1, ctable);
	  fmconfig->colors[apu2++] = ctable[0] >> 24;
	  fmconfig->colors[apu2++] = ctable[1] >> 24;
	  fmconfig->colors[apu2++] = ctable[2] >> 24;
#else
	  apu3 = GetRGB4 (wbscr->ViewPort.ColorMap, apu1);
	  fmconfig->colors[apu2++] = (apu3 >> 4) & 0xf0;
	  fmconfig->colors[apu2++] = (apu3) & 0xf0;
	  fmconfig->colors[apu2++] = (apu3 << 4) & 0xf0;
#endif
	}
      UnlockPubScreen (0, wbscr);
    }
  else
    {
      width = 640;
      height = 256;
      screenmode = HIRES_KEY | PAL_MONITOR_ID;
      if ((fmmain.kick < 39 && GfxBase->DisplayFlags & NTSC)
	  || (fmmain.kick >= 39 && !(GfxBase->DisplayFlags & REALLY_PAL)))
	{
	  height = 200;
	  screenmode = HIRES_KEY | NTSC_MONITOR_ID;
	}
    }
D(bug("fmconfig ...........\n"));  
  fmconfig->mainscreen.height = height;
  fmconfig->mainscreen.width = width;
  fmconfig->mainscreen.screenmode = screenmode;
  fmconfig->windowwidth = width;
  fmconfig->windowheight = height;
  fmconfig->textscreen.height = height;
  fmconfig->textscreen.width = width;
  fmconfig->textscreen.screenmode = screenmode;
  fmconfig->textscreen.depth = 2;

  strcpy (fmconfig->listfontname, topaz);
  fmconfig->listfontattr.ta_YSize = 8;
  strcpy (fmconfig->txtfontname, topaz);
  fmconfig->txtfontattr.ta_YSize = 8;
  strcpy (fmconfig->txtshowfontname, topaz);
  fmconfig->txtshowfontattr.ta_YSize = 8;
  strcpy (fmconfig->reqfontname, topaz);
  fmconfig->reqfontattr.ta_YSize = 8;
  strcpy (fmconfig->smallfontname, "topaz.font");
  fmconfig->smallfontattr.ta_YSize = 5;
  fmmain.tattr.ta_Name = topaz;
  fmmain.tattr.ta_YSize = 8;
  fmmain.filelen = 30;
  fmmain.commlen = 80;

  fmconfig->spaceh = 3;
  fmconfig->sliderh = 7;
  fmconfig->dirpen = 2;
  fmconfig->devicepen = 1;
  fmconfig->filepen = 1;
  fmconfig->txtpen = 1;
  fmconfig->hilipen = 3;
  fmconfig->backpen = 0;
  fmconfig->blackpen = 1;
  fmconfig->whitepen = 2;
  fmconfig->backfillpen = 0;
  fmconfig->mainbackfillpen = 3;
  fmconfig->sourcepen = 3;
  fmconfig->destpen = 6;
  fmconfig->sliderpen = 3;
  fmconfig->stringpen = 1;
  fmconfig->screentype = 1;
  fmconfig->mainpriority = 2;
  fmconfig->subpriority = -1;
  fmconfig->pri[0] = -1;
  fmconfig->pri[1] = 0;
  fmconfig->pri[2] = 1;
  fmconfig->dosbuffersize = 10000;
  fmconfig->doubleclick = 25;
  strcpy (fmconfig->pubscreen, "Workbench");

  fmconfig->useexecutedt = 1;
  fmconfig->usesampledt = 1;

  fmconfig->listinfo[0][0] = 100;
  fmconfig->listinfo[0][1] = 100;

  fmconfig->listinfo[1][0] = 100;
  fmconfig->listinfo[1][1] = 100 | 0x80;

  fmconfig->cmenuposition = 1;
  fmconfig->cmenucolumns = 1;


  D(bug("MSG_MAIN_CMENUTXT= %d\n", MSG_MAIN_CMENUTXT));
   ptr1 = fmmain.cmenuptr = getstring (MSG_MAIN_CMENUTXT);

  D(bug("MSG_MAIN_CMENUKEYS= %d\n", MSG_MAIN_CMENUKEYS));

   ptr3 = getstring (MSG_MAIN_CMENUKEYS);

  for (apu2 = 0; apu2 < TOTALCOMMANDS; apu2++)
    {
      cmc = &fmconfig->cmenuconfig[apu2];
      cmc->position = 0xff;
      cmc->frontpen = fmconfig->txtpen;
      cmc->backpen = fmconfig->backpen;
      cmc->priority = 1;
    }
  cmc = &fmconfig->cmenuconfig[0];
  for (apu2 = 0; apu2 < BUILDCOMMANDS; apu2++)
    {
      if (apu2 < 7)
	cmc->position = 2;
      else
	cmc->position = 1;
      cmc->cmenucount = apu2 + 1;
D(bug("fm.c 621...........\n")); 
      strncpy (cmc->label, ptr1, sizeof(cmc->label));
      strncpy (cmc->shortcut, ptr3, sizeof(cmc->shortcut));
D(bug("fm.c 624...........\n")); 
      ptr3 += strlen (ptr3) + 1;
D(bug("fm.c 648...........\n")); 
      cmc++;
      ptr1 += strlen (ptr1) + 1;
    }
D(bug("fm.c 652...........\n")); 
  initexes (BUILDCOMMANDS);

  winptr = fmmain.myproc->pr_WindowPtr;
  fmmain.myproc->pr_WindowPtr = (APTR) - 1;

D(bug("fm.c 658...........\n"));
  if (LocaleBase)
    catalog = OpenCatalog (0, "FileMaster.catalog", OC_BuiltInLanguage, "english",
  		   TAG_DONE);

  readconfig ();
  D(bug("fm.c 641...........\n")); 

  fmmain.myproc->pr_WindowPtr = winptr;
  fmconfig->txtfontattr.ta_Name = fmconfig->txtfontname;
  fmconfig->listfontattr.ta_Name = fmconfig->listfontname;
  fmconfig->reqfontattr.ta_Name = fmconfig->reqfontname;
  fmconfig->txtshowfontattr.ta_Name = fmconfig->txtshowfontname;
  fmconfig->smallfontattr.ta_Name = fmconfig->smallfontname;
   D(bug("fm.c 672...........\n"));  
  SetTaskPri ((struct Task *) fmmain.myproc, fmconfig->mainpriority);
  D(bug("fm.c 674...........\n"));  
  fmmain.datelen = 32;
  longtodatestring (teststring1,
		    (6523L * 24L * 60L + 11 * 60 + 11L) * 60L + 11);
  D(bug("fm.c 678...........\n"));  
  fmmain.datelen = strlen (teststring1);
  if (fmmain.kick >= 39)
    fmmain.pool =
      CreatePool (MEMF_PUBLIC | MEMF_CLEAR, sizeof (struct FMNode) * 64,
		  sizeof (struct FMNode) * 64);
D(bug("fm.c 684...........\n"));  
  recalc ();
D(bug("fm.c 685...........\n"));  
  if (!fmconfig->dl[0].width)
    {
      fmconfig->dl[0].width = fmmain.filelen;
      fmconfig->dl[0].type = 1;
      fmconfig->dl[1].width = fmmain.lenlen;
      fmconfig->dl[1].type = 0;
      fmconfig->dl[1].rightlock = 1;
      fmconfig->dl[1].rightaligned = 1;
      fmconfig->dl[2].width = fmmain.protlen;
      fmconfig->dl[2].type = 2;
      fmconfig->dl[3].width = fmmain.datelen;
      fmconfig->dl[3].type = 3;
      fmconfig->dl[4].width = fmmain.commlen;
      fmconfig->dl[4].type = 4;
    }
  fmmain.maxdirlistwidth =
    fmconfig->dl[0].width + fmconfig->dl[1].width + fmconfig->dl[2].width +
    fmconfig->dl[3].width + fmconfig->dl[4].width + 4;

#ifdef V39
  if (fmmain.betaversion)
{
D(bug("fm.c 707...........\n"));  
    sformat (fmmain.fmtitle, "FileMaster %ld.%ld BETA '020", fmmain.version,
             fmmain.revision);

}
  else
D(bug("fm.c 713...........\n"));  
    sformat (fmmain.fmtitle, "FileMaster %ld.%ld '020", fmmain.version,
	     fmmain.revision);
#else
D(bug(" fm.c 718.......\n")); 


  if (fmmain.betaversion)
{
  sformat (fmmain.fmtitle, "FileMaster %ld.%ld %ld", fmmain.version, fmmain.revision, fmmain.betaversion);
  else
  sformat (fmmain.fmtitle, "FileMaster %ld.%ld", fmmain.version, fmmain.revision);
}
#endif

  if (fmmain.regname[0])
    sformat (fmmain.fmtitlename, "%s %s %s", fmmain.fmtitle, getstring (MSG_MAIN_REGISTERED), fmmain.regname);
  else
D(bug(" fm.c 732.......\n"));  

    sformat (fmmain.fmtitlename, "%s %s", fmmain.fmtitle, getstring (MSG_MAIN_UNREGISTERED));

  longtodatestring (fmmain.fmdate, *((ULONG *) & fmname[8]));

  return (1);
}

void
recalc (void)
{
  WORD aa;
  UBYTE *ptr1;
  UBYTE teststring1[34];
D(bug("fm.c 748...........\n"));  
  for (aa = 0; aa < LISTS; aa++)
    clearlist (&fmlist[aa]);
D(bug("fm.c 751...........\n"));  
  freepathlist ();
D(bug("fm.c 753...........\n"));  
  Forbid ();
D(bug("fm.c 755...........\n"));  
  sformat (teststring1, "%lU", 100000000);
D(bug("fm.c 757...........\n"));  
  fmmain.lenlen = strlen (teststring1);
  fmmain.memmeterlen = fmmain.lenlen - 1;
  ptr1 = nformatstring;
  *ptr1++ = '%';
  if (fmmain.lenlen >= 10)
    *ptr1++ = '1';
  *ptr1++ = fmmain.lenlen % 10 + '0';
  *ptr1++ = 'l';
  *ptr1++ = 'U';
  *ptr1 = 0;
  ptr1 = nformatstring2;
  *ptr1++ = '%';
  if (fmmain.memmeterlen >= 10)
    *ptr1++ = '1';
  *ptr1++ = fmmain.memmeterlen % 10 + '0';
  *ptr1++ = 'l';
  *ptr1++ = 'U';
  *ptr1 = 0;
  fmmain.protlen = 8;
  fmmain.leftoffset = fmmain.lenoffset = sizeof (struct FMNode);
  fmmain.fileoffset = fmmain.lenoffset + fmmain.lenlen + 1;
  fmmain.protoffset = fmmain.fileoffset + fmmain.filelen + 1;
  fmmain.dateoffset = fmmain.protoffset + fmmain.protlen + 1;
  fmmain.commoffset = fmmain.dateoffset + fmmain.datelen + 1;
  fmmain.totnodelen = (fmmain.commoffset + fmmain.commlen + 1 + 3) & (~0x03);
D(bug("fm.c 783...........\n"));  
  Permit ();
D(bug("fm.c 785...........\n"));  
  initpathlist ();
D(bug("fm.c 790...........\n"));  
}

CONST_STRPTR GetString(struct LocaleInfo *li, ULONG id)
{
    CONST_STRPTR retval;
    int i;

    for (i = 0; CatCompArray[i].cca_ID != 0; i++)
    	    if (CatCompArray[i].cca_ID == id)
    	    	    break;

    if (CatCompArray[i].cca_ID == 0)
    	    return "--placeholder--";

 D(bug("fm.c 796...........\n"));
    if (catalog)
      {
	D(bug("fm.c 799...........\n")); 
        retval = GetCatalogStr(catalog, id, CatCompArray[i].cca_Str);
	D(bug("fm.c 801...........\n")); 
      } else {
	D(bug("fm.c 803...........\n"));
        retval = CatCompArray[i].cca_Str;
	D(bug("fm.c 805...........\n")); 
      }
 
    return retval;
}

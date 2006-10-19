/* devsupp.c:
 *
 * Support routines for the device handler.
 * - debugging
 * - Mountlist "Startup" field parsing
 *
 * ----------------------------------------------------------------------
 * This code is (C) Copyright 1993,1994 by Frank Munkert.
 * All rights reserved.
 * This software may be freely distributed and redistributed for
 * non-commercial purposes, provided this notice is included.
 * ----------------------------------------------------------------------
 * History:
 * 
 * 20-Jul-02 sheutlin  - made 2 Versions of Get_Startup() because AROS
 *                       doesn't understand startup fields (yet)
 *                     - put last part of Get_Startup() into an own function
 *                       (OpenCDRom()) and optimized it a little bit
 * 12-Oct-94   fmu   Get_Startup() modified.
 * 17-May-94   fmu   New option MAYBELOWERCASE (=ML).
 * 09-Apr-94   fmu   Larger buffer for startup strings.
 * 02-Jan-94   fmu   New options XPOS and YPOS.
 * 11-Dec-93   fmu   Memory type can now be chosen by the user:
 *                   new options CHIP, DMA and ANY.
 * 11-Dec-93   fmu   The assembly code stubs for the debugging process
 *                   are no longer necessary; the debugger code is now
 *                   called with CreateNewProcTags().
 * 21-Nov-93   fmu   New option SCANINTERVAL.
 * 14-Nov-93   fmu   Added Handle_Control_Packet for 'cdcontrol' program.
 * 10-Nov-93   fmu   New options SHOWVERSION and HFSFIRST.
 * 23-Oct-93   fmu   MacHFS options added.
 * 15-Oct-93   fmu   Adapted to new VOLUME structure.
 * 07-Jul-02 sheutlin  various changes when porting to AROS
 *                     - global variables are now in struct Global global
 *                     - "Version" is defined somewhere else (struct Resident)
 *                     - replaced memcpy by CopyMem
 */

/*
 * Extract information from Mountlist "Startup" field.
 */

#ifndef __AROS__
#define USE_INLINE_STDARG
#endif

#include <proto/exec.h>
#include <proto/dos.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include <exec/resident.h>
#include <exec/types.h>
#include <dos/dostags.h>
#include <dos/rdargs.h>

#include "device.h"
#include "devsupp.h"
#include "intui.h"
#include "params.h"
#include "cdcontrol.h"
#include "globals.h"
#include "debug.h"

extern struct Globals *global;
extern const struct Resident ACDR_resident;

#ifdef SysBase
#	undef SysBase
#endif
#define SysBase global->SysBase
#ifdef DOSBase
#	undef DOSBase
#endif
#define DOSBase global->DOSBase

#ifdef __AROS__
APTR AllocDosObjectOwnTags(ULONG type, Tag tag1, ...) {

	AROS_SLOWSTACKTAGS_PRE(tag1)
	retval = AllocDosObject(type, AROS_SLOWSTACKTAGS_ARG(tag1));
	AROS_SLOWSTACKTAGS_POST
}
#else
#define AllocDosObjectOwnTags AllocDosObjectTags
#endif

int OpenCDRom() {

	global->g_cd = Open_CDROM
		(
			global->g_device,
			global->g_unit,
			global->g_trackdisk,
			global->g_memory_type,
			global->g_std_buffers,
			global->g_file_buffers
		);
	if (global->g_cd)
	{
		global->g_disk_inserted =
			(
				Test_Unit_Ready (global->g_cd) ||
				Test_Unit_Ready (global->g_cd)
			);
		return TRUE;
	}
	if (
			global->g_retry_mode &&
			global->g_cdrom_errno != CDROMERR_NO_MEMORY &&
			global->g_cdrom_errno != CDROMERR_MSGPORT &&
			global->g_cdrom_errno != CDROMERR_IOREQ
		)
	{
		return TRUE;
	}
	switch (global->g_cdrom_errno)
	{
	case CDROMERR_NO_MEMORY:
		Display_Error
		(
			"Out of memory: cannot allocate buffers\n"
			"(Try CHIP, FAST, DMA or ANY option.)"
		);
		break;
	case CDROMERR_MSGPORT:
		Display_Error ("Cannot open the message port.");
		break;
	case CDROMERR_IOREQ:
		Display_Error ("Cannot open the I/O request structure.");
		break;
	case CDROMERR_DEVICE:
		Display_Error
		(
			"Cannot open \"%s\" unit %ld",
			global->g_device,
			(int) global->g_unit
		);
     break;
	case CDROMERR_BLOCKSIZE:
		Display_Error ("Cannot access CDROM drive: illegal blocksize.");
		break;
	default:
		break;
	}
	return FALSE;
}

#ifdef __AROS__
int Get_Startup(struct FileSysStartupMsg *fssm) {
struct DosEnvec *de = (struct DosEnvec *)BADDR(fssm->fssm_Environ);

	if (fssm != -1)
	{
		if (strlen(AROS_BSTR_ADDR(fssm->fssm_Device))<sizeof(global->g_device))
		{
			strcpy(global->g_device, AROS_BSTR_ADDR(fssm->fssm_Device));
			global->g_unit = fssm->fssm_Unit;
			global->g_memory_type = MEMF_ANY;
			global->g_retry_mode = FALSE;
			global->g_map_to_lowercase = FALSE;
			global->g_maybe_map_to_lowercase = TRUE;
			global->g_use_rock_ridge = TRUE;
			global->g_trackdisk = TRUE;
			global->g_show_version_numbers = FALSE;
#warning "TODO: mount both images if hybrid disk"
			global->g_hfs_first = FALSE;
			global->g_std_buffers = de->de_NumBuffers;
			global->g_file_buffers = de->de_NumBuffers;
			strcpy(global->g_data_fork_extension, "dfork");
			strcpy(global->g_resource_fork_extension, "rfork");
			global->g_convert_hfs_filenames = FALSE;
			global->g_convert_hfs_spaces = FALSE;
			global->g_scan_interval = 3;
			global->g_play_cdda_command[0] = 0;
			global->g_xpos = NO_ICON_POSITION;
			global->g_ypos = NO_ICON_POSITION;
			return OpenCDRom();
		}
	}
	return FALSE;
}
#else
int Get_Startup (LONG p_startup)
{
  enum {
    ARG_DEVICE,
    ARG_UNIT,
    ARG_CHIP,
    ARG_FAST,
    ARG_DMA,
    ARG_ANY,
    ARG_RETRY,
    ARG_LOWERCASE,
    ARG_MAYBELOWERCASE,
    ARG_ROCKRIDGE,
    ARG_TRACKDISK,
    ARG_MACTOISO,
    ARG_CONVERTSPACES,
    ARG_SHOWVERSION,
    ARG_HFSFIRST,
    ARG_STDBUFFERS,
    ARG_FILEBUFFERS,
    ARG_DATAEXT,
    ARG_RESOURCEEXT,
    ARG_SCANINTERVAL,
    ARG_PLAYCDDA,
    ARG_XPOS,
    ARG_YPOS,
    ARGCOUNT
  };

  STRPTR Args[ARGCOUNT],Index;
  static UBYTE LocalBuffer[250];
  struct RDArgs *ArgsPtr;
  int result = FALSE,len,i;
  int cnt;

  /* Clear the argument vector. */
  memset (Args, 0, sizeof(Args));

  /* valid startup entry? */
  if (!p_startup) {
    Display_Error ("Filesystem startup entry invalid");
    return FALSE;
  }

  if (p_startup != -1) {
  
    /* Get the contents of the startup field. */
    len = ((STRPTR)(BADDR(p_startup)))[0];
    if (len > sizeof (LocalBuffer) - 1)
      len = sizeof (LocalBuffer) - 1;
    CopyMem (((STRPTR)(BADDR(p_startup))) + 1, LocalBuffer, len);

    /* Provide null-termination. */
    LocalBuffer[len] = 0;

    /* Remove leading quotes. */
    for (i = 0 ; i < len ; i++) {
      if (LocalBuffer[i] != ' ') {
        if (LocalBuffer[i] == '\"')
	  LocalBuffer[i] = ' ';
        break;
      }
    }
    

    /* Remove trailing quotes. */
    for (i = len - 1 ; i >= 0 ; i--) {
      if (LocalBuffer[i] != ' '){
        if (LocalBuffer[i] == '\"')
	  LocalBuffer[i] = ' ';
        break;
      }
    }

    /* Replace "-" by spaces, except "--" which is replaced by "-". */
    Index = LocalBuffer;
    for (i = 0 ; i < len ; i++) {
      if (LocalBuffer[i] == '-') {
        if (i+1 < len && LocalBuffer[i+1] == '-') {
          *Index++ = '-';
	  i++;
        } else
          *Index++ = ' ';
      } else
        *Index++ = LocalBuffer[i];
    }

    /* Provide null-termination. */
    *Index = 0;

    /* Don't forget the newline, or ReadArgs won't work. */
    strcat ((char *) LocalBuffer, "\n");
    
  }

  ArgsPtr = (struct RDArgs *) AllocDosObjectOwnTags (DOS_RDARGS,TAG_DONE);
  if (ArgsPtr)
  {
    /* Don't prompt for input! */
    ArgsPtr -> RDA_Flags |= RDAF_NOPROMPT;

    /* Set up for local parsing. */
    ArgsPtr->RDA_Source.CS_Buffer = LocalBuffer;
    ArgsPtr->RDA_Source.CS_Length = strlen ((char *) LocalBuffer);
    ArgsPtr->RDA_Source.CS_CurChr = 0;

    /* Read the arguments. */
    if (ReadArgs ((UBYTE *)
      		  "D=DEVICE,U=UNIT/N,C=CHIP/S,F=FAST/S,DMA/S,ANY/S,"
		  "RETRY/S,L=LOWERCASE/S,ML=MAYBELOWERCASE/S,"
   		  "R=ROCKRIDGE/S,T=TRACKDISK/S,"
		  "MI=MACTOISO/S,CS=CONVERTSPACES/S,"
		  "SV=SHOWVERSION/S,HF=HFSFIRST/S,"
		  "SB=STDBUFFERS/K/N,FB=FILEBUFFERS/K/N,"
		  "DE=DATAEXT/K,RE=RESOURCEEXT/K,"
		  "SI=SCANINTERVAL/K/N,PC=PLAYCDDA/K,"
		  "X=XPOS/K/N,Y=YPOS/K/N",
		  (LONG *) Args, ArgsPtr)) {
      result = TRUE;

      if (Args[ARG_DEVICE]) {
        len = strlen((char *) (Args[ARG_DEVICE]));

        if (len >= sizeof (global->g_device)) {
 	  Display_Error ("Device name entry too long");
 	  result = FALSE;
        } else
	  strcpy (global->g_device, (char *) (Args[ARG_DEVICE]));
      } else {
        Display_Error("Device name entry missing");
	result = FALSE;
      }

      global->g_unit = *(long *) (Args[ARG_UNIT]);

      global->g_memory_type = MEMF_CHIP;
      cnt = 0;
      if (Args[ARG_FAST] != NULL) {
        global->g_memory_type = MEMF_FAST;
	cnt++;
      }
      if (Args[ARG_CHIP] != NULL) {
        global->g_memory_type = MEMF_CHIP;
	cnt++;
      }
      if (Args[ARG_DMA] != NULL) {
        global->g_memory_type = MEMF_24BITDMA;
	cnt++;
      }
      if (Args[ARG_ANY] != NULL) {
        global->g_memory_type = MEMF_ANY;
	cnt++;
      }
      if (cnt > 1) {
        Display_Error ("Only ONE memory option may be used!");
	result = FALSE;
      }

      global->g_retry_mode = (Args[ARG_RETRY] != NULL);
      global->g_map_to_lowercase = (Args[ARG_LOWERCASE] != NULL);
      global->g_maybe_map_to_lowercase = (Args[ARG_MAYBELOWERCASE] != NULL);

      if (global->g_map_to_lowercase && global->g_maybe_map_to_lowercase) {
        Display_Error ("Options L and ML are mutually exclusive!");
	result = FALSE;
      }

      global->g_use_rock_ridge = (Args[ARG_ROCKRIDGE] != NULL);
      global->g_trackdisk = (Args[ARG_TRACKDISK] != NULL);
      global->g_show_version_numbers = (Args[ARG_SHOWVERSION] != NULL);
      global->g_hfs_first = (Args[ARG_HFSFIRST] != NULL);

      if (Args[ARG_STDBUFFERS]) {      
        global->g_std_buffers = *(long *) (Args[ARG_STDBUFFERS]);
        if (global->g_std_buffers <= 0) {
          Display_Error ("Illegal number of standard buffers: %ld", global->g_std_buffers);
	  result = FALSE;
        }
      } else
        global->g_std_buffers = 5;
      
      if (Args[ARG_FILEBUFFERS]) {
        global->g_file_buffers = *(long *) (Args[ARG_FILEBUFFERS]);
        if (global->g_file_buffers <= 0) {
          Display_Error ("Illegal number of file buffers: %ld", global->g_std_buffers);
	  result = FALSE;
        }
      } else
        global->g_file_buffers = 5;

      if (Args[ARG_DATAEXT])
        strcpy (global->g_data_fork_extension, (char *) Args[ARG_DATAEXT]);

      if (Args[ARG_RESOURCEEXT])
        strcpy (global->g_resource_fork_extension, (char *) Args[ARG_RESOURCEEXT]);

      global->g_convert_hfs_filenames = (Args[ARG_MACTOISO] != NULL);
      global->g_convert_hfs_spaces = (Args[ARG_CONVERTSPACES] != NULL);

      if (Args[ARG_SCANINTERVAL]) {
        global->g_scan_interval = *(long *) (Args[ARG_SCANINTERVAL]);
        if (global->g_scan_interval < 0)
          global->g_scan_interval = 0;
      } else
        global->g_scan_interval = 3;

      if (Args[ARG_PLAYCDDA]) {
        len = strlen((char *) (Args[ARG_PLAYCDDA]));

        if (len >= sizeof (global->g_play_cdda_command)) {
 	  Display_Error ("PLAYCDDA command name too long");
 	  result = FALSE;
        } else
	  strcpy (global->g_play_cdda_command, (char *) (Args[ARG_PLAYCDDA]));
      } else
        global->g_play_cdda_command[0] = 0;

      if (Args[ARG_XPOS])
        global->g_xpos = *(LONG *) (Args[ARG_XPOS]);
      if (Args[ARG_YPOS])
        global->g_ypos = *(LONG *) (Args[ARG_YPOS]);

      FreeArgs(ArgsPtr);
    } else {
      Fault(IoErr (), (UBYTE *) "", LocalBuffer, sizeof (LocalBuffer));
      Display_Error ("Error while parsing \"Startup\" field in Mountlist:\n%s",
      		     LocalBuffer + 2);
    }

    FreeDosObject (DOS_RDARGS, ArgsPtr);
  } else
    Display_Error ("Out of memory");

  if (!result)
    return FALSE;

	return OpenCDRom();
}
#endif

int Handle_Control_Packet (ULONG p_type, ULONG p_par1, ULONG p_par2)
{
  switch (p_type) {
  case CDCMD_LOWERCASE:
    global->g_map_to_lowercase = p_par1;
    break;
  case CDCMD_MACTOISO:
    global->g_convert_hfs_filenames = p_par1;
    break;
  case CDCMD_CONVERTSPACES:
    global->g_convert_hfs_spaces = p_par1;
    break;
  case CDCMD_SHOWVERSION:
    global->g_show_version_numbers = p_par1;
    break;
  case CDCMD_HFSFIRST:
    global->g_hfs_first = p_par1;
    break;
  case CDCMD_DATAEXT:
    strcpy (global->g_data_fork_extension, (char *) p_par1);
    break;
  case CDCMD_RESOURCEEXT:
    strcpy (global->g_resource_fork_extension, (char *) p_par1);
    break;
  default:
    return 999;
  }
  return 0;
}

#if !defined(NDEBUG) || defined(DEBUG_SECTORS)

char *typetostr (int ty)
{
    switch(ty) {
    case ACTION_DIE:		return("DIE");
    case ACTION_FINDUPDATE: 	return("OPEN-RW");
    case ACTION_FINDINPUT:	return("OPEN-OLD");
    case ACTION_FINDOUTPUT:	return("OPEN-NEW");
    case ACTION_READ:		return("READ");
    case ACTION_WRITE:		return("WRITE");
    case ACTION_END:		return("CLOSE");
    case ACTION_SEEK:		return("SEEK");
    case ACTION_EXAMINE_NEXT:	return("EXAMINE NEXT");
    case ACTION_EXAMINE_OBJECT: return("EXAMINE OBJ");
    case ACTION_INFO:		return("INFO");
    case ACTION_DISK_INFO:	return("DISK INFO");
    case ACTION_PARENT: 	return("PARENTDIR");
    case ACTION_DELETE_OBJECT:	return("DELETE");
    case ACTION_CREATE_DIR:	return("CREATEDIR");
    case ACTION_LOCATE_OBJECT:	return("LOCK");
    case ACTION_COPY_DIR:	return("DUPLOCK");
    case ACTION_FREE_LOCK:	return("FREELOCK");
    case ACTION_SET_PROTECT:	return("SETPROTECT");
    case ACTION_SET_COMMENT:	return("SETCOMMENT");
    case ACTION_RENAME_OBJECT:	return("RENAME");
    case ACTION_INHIBIT:	return("INHIBIT");
    case ACTION_RENAME_DISK:	return("RENAME DISK");
    case ACTION_MORE_CACHE:	return("MORE CACHE");
    case ACTION_WAIT_CHAR:	return("WAIT FOR CHAR");
    case ACTION_FLUSH:		return("FLUSH");
    case ACTION_SCREEN_MODE:	return("SCREENMODE");
    case ACTION_IS_FILESYSTEM:	return("IS_FILESYSTEM");
    case ACTION_SAME_LOCK:      return("SAME_LOCK");
    case ACTION_COPY_DIR_FH:    return("COPY_DIR_FH");
    case ACTION_PARENT_FH:      return("PARENT_FH");
    case ACTION_EXAMINE_FH:     return("EXAMINE_FH");
    case ACTION_FH_FROM_LOCK:   return("FH_FROM_LOCK");
    case ACTION_CURRENT_VOLUME: return("CURRENT_VOLUME");
    case ACTION_READ_LINK:	return("READ LINK");
    case ACTION_MAKE_LINK:	return("MAKE LINK");
    case ACTION_USER:		return("USER");
    default:			return("---------UNKNOWN-------");
    }
}

#if !(defined(__AROS__) || defined(__MORPHOS__))

/*
 *  DEBUGGING CODE.	You cannot make DOS library calls that access other
 *  devices from within a DOS device driver because they use the same
 *  message port as the driver.  If you need to make such calls you must
 *  create a port and construct the DOS messages yourself.  I do not
 *  do this.  To get debugging info out another PROCESS is created to which
 *  debugging messages can be sent.
 *
 *  You want the priority of the debug process to be larger than the
 *  priority of your DOS handler.  This is so if your DOS handler crashes
 *  you have a better idea of where it died from the debugging messages
 *  (remember that the two processes are asyncronous from each other).
 */

/*
 *  BTW, the DOS library used by debugmain() was actually opened by
 *  the device driver.	Note: DummyMsg cannot be on debugmain()'s stack
 *  since debugmain() goes away on the final handshake.
 */

#ifdef LATTICE
void __saveds debugmain (void)
#else
void debugmain (void)
#endif	/* LATTICE */
{
    MSG *msg;
    short len;
    void *fh;
#ifdef LOG_MESSAGES
    void *out;
#endif
    global->Dbport = CreateMsgPort ();
    fh = (void *) Open ((UBYTE *) "con:0/0/640/200/debugwindow", 1006);
    PutMsg(global->Dback, &global->DummyMsg);
#ifdef LOG_MESSAGES
#ifdef LOG_TO_PAR
    out = (void *) Open ((UBYTE *) "PAR:", 1006);
#else
    out = (void *) Open ((UBYTE *) "ram:cd.log", 1006);
#endif
#endif
    for (;;) {
	WaitPort(global->Dbport);
	msg = GetMsg(global->Dbport);
	len = msg->mn_Length;
	if (len == 0)
	    break;
	--len;			      /*  Fix length up   */
	Write((BPTR) fh, msg+1, len);
#ifdef LOG_MESSAGES
	Write((BPTR) out, msg+1, len);
#endif
	FreeMem(msg,sizeof(MSG)+len+1);
    }
    Close ((BPTR) fh);
#ifdef LOG_MESSAGES
    Close ((BPTR) out);
#endif
    DeleteMsgPort(global->Dbport);
    PutMsg(global->Dback,&global->DummyMsg);	      /*  Kill handshake  */
}

void dbinit (void)
{
    TASK *task = FindTask(NULL);

    global->Dback = CreateMsgPort();
    if (CreateNewProcTags (
			   NP_Entry, debugmain,
        		   NP_Name, "DEV_DB",
       			   NP_Priority, task->tc_Node.ln_Pri+1,
       			   NP_StackSize, 4096,
       			   TAG_DONE)) {
      WaitPort(global->Dback);				    /* handshake startup    */
      GetMsg(global->Dback);				    /* remove dummy msg     */
      dbprintf("Debugger running:" HANDLER_VERSION "%s, %s\n",
#define asString(x) #x
#if defined(LATTICE)
	       "SAS/C" asString(__VERSION__) "." asString(__REVISION__),
#elif defined(__GNUC__)
	       "GNU C " __VERSION__,
#else
	       "???",
#endif
      	       __TIME__);
    };
}

void dbuninit (void)
{
    MSG killmsg;

    if (global->Dbport) {
	killmsg.mn_Length = 0;	    /*	0 means die	    */
	PutMsg(global->Dbport,&killmsg);
	WaitPort(global->Dback);	    /*	He's dead jim!      */
	GetMsg(global->Dback);
	DeleteMsgPort(global->Dback);

	/*
	 *  Since the debug process is running at a greater priority, I
	 *  am pretty sure that it is guarenteed to be completely removed
	 *  before this task gets control again.  Still, it doesn't hurt...
	 */

	Delay(100);		    /*	ensure he's dead    */
    }
}

void dbprintf (char *format, ...)
{
    va_list arg;
    char buf[256];
    MSG *msg;

    va_start (arg, format);
    if (global->Dbport && !global->DBDisable) {
	vsprintf (buf, format, arg);
	msg = AllocMem(sizeof(MSG)+strlen(buf)+1, MEMF_PUBLIC|MEMF_CLEAR);
	msg->mn_Length = strlen(buf)+1;     /*	Length NEVER 0	*/
	strcpy((char *) (msg+1), buf);
	PutMsg(global->Dbport,msg);
    }
    va_end (arg);
}

/* This hack is intended to suppress linking DOS access functions from libnix.
   Libnix v1.2 is written badly and vsprintf() uses the same code as vfprintf()
   whicn in turn relies on fputc(). This causes linker to add whole
   dos.library-based I/O stuff to the code together with BREAK checking
   functions. One of bad effects of this is that the handler can't be linked at
   all due to lack of exit() function (i think even if it would link it is
   unlikely to work properly) */
int __fflush(void);
int __fflush() {
    return 1;
}

#endif /* !(__AROS__ || __MORPHOS__) */
#endif /* !NDEBUG || DEBUG_SECTORS */

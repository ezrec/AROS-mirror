/* device.c:
 *
 * Handler for ISO-9660 (+ Rock Ridge) + HFS CDROM filing system.
 * Based on DOSDEV V1.10 (2 Nov 87) by Matthew Dillon.
 *
 * ----------------------------------------------------------------------
 * This code is (C) Copyright 1993,1994 by Frank Munkert.
 * All rights reserved.
 * This software may be freely distributed and redistributed for
 * non-commercial purposes, provided this notice is included.
 * ----------------------------------------------------------------------
 * History:
 *
 * 27-Jul-01   sheutlin - fixed SAME_LOCK
 * 03-Nov-94   fmu   Truncate file names to 30 characters.
 * 12-Oct-94   fmu   Return startup packet even if the scsi device cannot
 *                   be opened immediately.
 * 01-Sep-94   fmu   - Added ACTION_EXAMINE_FH.
 *		     - Added ACTION_PARENT_FH.
 *		     - Added ACTION_MAKE_LINK (-> error message).
 *		     - Automatically remove volume if all locks on the
 *		       the volume have been freed.
 * 17-May-94   fmu   New option MAYBELOWERCASE (=ML).
 * 20-Apr-94   fmu   Improved implementation of ACTION_INHIBIT.
 * 17-Apr-94   fmu   Fixed bug concerning TRACKDISK disk change recognition.
 * 12-Apr-94   fmu   Adapted ACTION_CURRENT_VOLUME to new filehandle
 *		     management.
 * 09-Apr-94   fmu   Volume management: locks and filehandles will not
 *                   be forgotten if a CDROM is removed from the drive.
 * 06-Feb-94   dmb   - Full support for ACTION_INHIBIT
 *                   - Change Check_Disk() for trackdisk support
 * 05-Jan-93   fmu   - Retry displaying CD-DA icon if WB is not open.
 *                   - id_UnitNumber of InfoData set to SCSI unit number.
 *                   - Added Make_FSSM().
 * 01-Jan-93   fmu   Support for symbolic links on RockRidge disks.
 * 11-Dec-93   fmu   - ACTION_FLUSH always returns DOSTRUE.
 *                   - ISO volume names are mapped to lowercase if
 *                     the option LOWERCASE has been selected.
 * 26-Nov-93   fmu   Some packets are now handled even if no disk
 *                   is inserted.
 * 21-Nov-93   fmu   - User programmable diskchange check interval.
 *                   - Better support for ACTION_INHIBIT.
 *                   - Handles filenames with ';'.
 * 15-Nov-93   fmu   Missing return value for 'handler' inserted.
 * 14-Nov-93   fmu   Added ACTION_USER packet for 'cdcontrol' program.
 * 15-Oct-93   fmu   Adapted to new VOLUME structure.
 * 10-Oct-93   fmu   - Creates volume node for 'no DOS' disks.
 *		     - Volume node now contains the correct volume
 *		       creation date.
 * 09-Oct-93   fmu   - New format for mountlist startup field.
 *		     - SAS/C support.
 *		     - Debug process assembly tag adapted to small
 *		       memory model.
 *		     - Get_Startup moved to file devsupp.c.
 * 03-Oct-93   fmu   - New buffering options 'S' and 'C'.
 *                   - Fixed bug in cdlock.
 *		     - Fixed bug in ACTION_CURRENT_VOLUME.
 * 27-Sep-93   fmu   Added ACTION_SAME_LOCK
 * 25-Sep-93   fmu   - Send 'disk inserted' / 'disk removed' event via
 *                     input.device if disk has been changed.
 *                   - Corrected bug in ACTION_DISK_INFO.
 * 24-Sep-93   fmu   - Added fast memory option 'F'.
 *                   - Added ACTION_IS_FILESYSTEM.
 *                   - Added 'write protected' error for write actions.
 *                   - Added ACTION_CURRENT_VOLUME.
 *                   - Unload handler code after ACTION_DIE.
 *                   - Immediately terminate program if called from CLI.
 *                   - Added library version number.
 *                   - Set volume label to "Unnamed" for disks without name.
 * 16-Sep-93   fmu   Added code to detect whether a lock stems from the
 *                   current volume or from another volume which has
 *                   been removed from the drive.
 */

/*
 *  Debugging routines are disabled by simply attempting to open the
 *  file "debugoff", turned on again with "debugon".  No prefix may be
 *  attached to these names (you must be CD'd to TEST:).
 *
 *  See Documentation for a detailed discussion.
 */
 

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>

#include <aros/debug.h>
#include <aros/asmcall.h>
#include <dos/dosasl.h>
#include <dos/dosextens.h>
#include <dos/filesystem.h>
#include <exec/errors.h>
#include <exec/resident.h>

#include "acdrbase.h"

#include "device.h"
#include "intui.h"
#include "devsupp.h"
#include "cdcontrol.h"
#include "params.h"
#include "rock.h"

#include "clib_stuff.h"

extern const char name[];
extern const char version[];
extern const APTR inittab[4];
extern void *const acdrfunctable[];
extern const UBYTE acdrdatatable;
extern struct ACDRBase *AROS_SLIB_ENTRY(init, acdrdev)();
extern void AROS_SLIB_ENTRY(open, acdrdev)();
extern BPTR AROS_SLIB_ENTRY(close, acdrdev)();
extern BPTR AROS_SLIB_ENTRY(expunge, acdrdev)();
extern int AROS_SLIB_ENTRY(null, acdrdev)();
extern void AROS_SLIB_ENTRY(beginio, acdrdev)();
extern LONG AROS_SLIB_ENTRY(abortio,acdrdev)();
extern void handler();
extern const char acdrhandlerend;

int ACDR_entry(void) {

	/* If the device was executed by accident return error code. */
	return -1;
}

const struct Resident ACDR_resident=
{
	RTC_MATCHWORD,
	(struct Resident *)&ACDR_resident,
	(APTR)&acdrhandlerend,
	RTF_COLDSTART | RTF_AFTERDOS | RTF_AUTOINIT,
	41,
	NT_DEVICE,
	-122,
	(char *)name,
	(char *)&version[6],
	(ULONG *)inittab
};

static const char name[]="cdrom.handler";
static const char version[]="$VER: cdrom.handler 1.15 (2001-07-21)\n";

static const APTR inittab[4]=
{
	(APTR)sizeof(struct ACDRBase),
	(APTR)acdrfunctable,
	(APTR)&acdrdatatable,
	&AROS_SLIB_ENTRY(init, acdrdev)
};

void *const acdrfunctable[]=
{
	&AROS_SLIB_ENTRY(open, acdrdev),
	&AROS_SLIB_ENTRY(close, acdrdev),
	&AROS_SLIB_ENTRY(expunge, acdrdev),
	&AROS_SLIB_ENTRY(null, acdrdev),
	&AROS_SLIB_ENTRY(beginio, acdrdev),
	&AROS_SLIB_ENTRY(abortio, acdrdev),
	(void *)-1
};

const UBYTE acdrdatatable = 0;

AROS_UFH3(struct ACDRBase *, AROS_SLIB_ENTRY(init,acdrdev),
 AROS_UFHA(struct ACDRBase *, acdrbase, D0),
 AROS_UFHA(BPTR,              segList,  A0),
 AROS_UFHA(struct ExecBase *, SysBase,  A6)
)
{
	AROS_USERFUNC_INIT

	struct Task *task;
	APTR stack;

	acdrbase->seglist = segList;
	acdrbase->SysBase = SysBase;
	acdrbase->DOSBase = (struct DOSBase *)OpenLibrary("dos.library", 41);
	if (acdrbase->DOSBase)
	{
		acdrbase->UtilityBase = OpenLibrary("utility.library", 41);
		if (acdrbase->UtilityBase)
		{
#warning "Maybe init_intui() volume specfic (open())"
			Init_Intui(acdrbase);
			NEWLIST(&acdrbase->port.mp_MsgList);
			acdrbase->port.mp_Node.ln_Type = NT_MSGPORT;
			acdrbase->port.mp_SigBit = SIGBREAKB_CTRL_F;
			NEWLIST(&acdrbase->rport.mp_MsgList);
			acdrbase->rport.mp_Node.ln_Type = NT_MSGPORT;
			acdrbase->rport.mp_Flags = PA_SIGNAL;
			acdrbase->rport.mp_SigBit = SIGB_SINGLE;
			task = (struct Task *)AllocMem(sizeof(struct Task), MEMF_PUBLIC | MEMF_CLEAR);
			if (task)
			{
				acdrbase->port.mp_SigTask = task;
				acdrbase->port.mp_Flags = PA_IGNORE;
				NEWLIST(&task->tc_MemEntry);
				task->tc_Node.ln_Type = NT_TASK;
				task->tc_Node.ln_Name = "AmiCDRom.handler task";
				stack = AllocMem(AROS_STACKSIZE, MEMF_PUBLIC);
				if (stack)
				{
					task->tc_SPLower = stack;
					task->tc_SPUpper = (BYTE *)stack+AROS_STACKSIZE;
#if AROS_STACK_GROWS_DOWNWARDS
					task->tc_SPReg = (BYTE *)task->tc_SPUpper-SP_OFFSET-sizeof(APTR);
					((APTR *)task->tc_SPUpper)[-1] = acdrbase;
#else
					task->tc_SPReg = (BYTE *)task->tc_SPLower-SP_OFFSET+sizeof(APTR);
					*(APTR *)task->tc_SPLower = acdrbase;
#endif
					if (AddTask(task, handler, NULL))
						return acdrbase;
					FreeMem(stack, AROS_STACKSIZE);
				}
				FreeMem(task, sizeof(struct Task));
			}
			CloseLibrary(acdrbase->UtilityBase);
		}
		CloseLibrary((struct Library *)acdrbase->DOSBase);
	}
	return NULL;
	AROS_USERFUNC_EXIT
}

#include "baseredef.h"

CDROM   *g_cd;
VOLUME  *g_volume;
CDROM_OBJ *g_top_level_obj;
ULONG g_timer_sigbit;
char	*g_vol_name;
int	g_inhibited;		/* Number of active INHIBIT(TRUE) packets */
int	g_scan_interval;	/* Time between successive diskchange checks */
int     g_scan_time;		/* Countdown for diskchange */
int     g_time;			/* Current time */

void Send_Timer_Request(struct ACDRBase *);
int Open_Timer_Device(struct ACDRBase *);
void Mount (struct ACDRBase *);

AROS_LH3(void, open,
 AROS_LHA(struct IOFileSys *, iofs,    A1),
 AROS_LHA(ULONG,              unitnum, D0),
 AROS_LHA(ULONG,              flags,   D1),
           struct ACDRBase *, acdrbase, 1, acdrdev)
{
	AROS_LIBFUNC_INIT

	ULONG g_memory_type;

	unitnum = flags = 0;
	/* because of global variables we allow only one opener */
	if (acdrbase->device.dd_Library.lib_OpenCnt==0)
	{
		acdrbase->device.dd_Library.lib_OpenCnt++;
		acdrbase->rport.mp_SigTask = FindTask(NULL);
		if (StrLen(iofs->io_Union.io_OpenDevice.io_DeviceName)<sizeof(g_device))
		{
			StrCpy(g_device, iofs->io_Union.io_OpenDevice.io_DeviceName);
			g_unit = iofs->io_Union.io_OpenDevice.io_Unit;
#warning "Why memory type!?"
			g_memory_type = MEMF_ANY;
			g_map_to_lowercase = NULL;
			g_maybe_map_to_lowercase = 1;
			g_use_rock_ridge = 1;
#warning "hardcoded to trackdisk"
			g_trackdisk = 1;
			g_show_version_numbers = NULL;
#warning "TODO: mount both images when hybrid disk"
			g_hfs_first = NULL;
			g_std_buffers = 5;
			g_file_buffers = 5;
//			g_data_fork_extension (hfs?)
//			g_resource_fork_extension (hfs?)
			g_convert_hfs_filenames = NULL;
			g_convert_hfs_spaces = NULL;
			g_scan_interval = 3;
#warning "TODO: write prefs prg to control options"
			g_play_cdda_command[0] = 0;
			g_cd = Open_CDROM
				(
					acdrbase,
					g_device,
					g_unit,
					g_trackdisk,
					g_memory_type,
					g_std_buffers,
					g_file_buffers
				);
			if (g_cd)
			{
				g_disk_inserted =
					(
						Test_Unit_Ready(acdrbase, g_cd) ||
						Test_Unit_Ready(acdrbase, g_cd)
					);
				g_inhibited = 0;
				g_timer_sigbit = 0;
				if (Open_Timer_Device(acdrbase))
				{
					Send_Timer_Request(acdrbase);
					g_scan_time = g_scan_interval;
					g_time = 0;
				}
				g_vol_name = AllocMem(128, MEMF_PUBLIC | MEMF_CLEAR);
				if (g_cd)
				{
					/* Mount volume (if any disk is inserted): */
					Mount(acdrbase);
				}
				g_cd->port->mp_SigTask = acdrbase->port.mp_SigTask;
				iofs->IOFS.io_Unit = (struct Unit *)g_top_level_obj;
				return;
			}
			switch (g_cdrom_errno)
			{
			case CDROMERR_NO_MEMORY :
				Display_Error(acdrbase, "Out of memory: cannot allocate buffers.");
				break;
			case CDROMERR_MSGPORT :
				Display_Error(acdrbase, "Cannot open the message port.");
				break;
			case CDROMERR_IOREQ :
				Display_Error(acdrbase, "Cannot open the I/O request structure.");
				break;
			case CDROMERR_DEVICE :
				Display_Error(acdrbase, "Cannot open \"%s\" unit %ld", g_device, (int)g_unit);
				break;
			case CDROMERR_BLOCKSIZE :
				Display_Error(acdrbase, "Cannot access CDROM drive: illegal blocksize.");
				break;
			default :
			}
		}
		acdrbase->device.dd_Library.lib_OpenCnt--;
	}
	iofs->IOFS.io_Error = IOERR_OPENFAIL;
	AROS_LIBFUNC_EXIT;
}	

AROS_LH0(BPTR, expunge, struct ACDRBase *, acdrbase, 3, acdrdev)
{
	AROS_LIBFUNC_INIT
	BPTR retval;

	if (acdrbase->device.dd_Library.lib_OpenCnt)
	{
		acdrbase->device.dd_Library.lib_Flags |= LIBF_DELEXP;
		return 0;
	}
	RemTask(acdrbase->port.mp_SigTask);
	FreeMem
		(
			((struct Task *)acdrbase->port.mp_SigTask)->tc_SPLower,
			AROS_STACKSIZE
		);
	FreeMem(acdrbase->port.mp_SigTask, sizeof(struct Task));
#warning "Maybe close_intui() volume specfic (close())"
	Close_Intui(acdrbase);
	CloseLibrary(UtilityBase);
	CloseLibrary((struct Library *)DOSBase);
	Remove(&acdrbase->device.dd_Library.lib_Node);
	retval = acdrbase->seglist;
	FreeMem
		(
			(char *)acdrbase-acdrbase->device.dd_Library.lib_NegSize,
			acdrbase->device.dd_Library.lib_NegSize + acdrbase->device.dd_Library.lib_PosSize
		);
	return retval;

	AROS_LIBFUNC_EXIT
}

void Cleanup_Timer_Device(struct ACDRBase *acdrbase);
void Unmount(struct ACDRBase *);

AROS_LH1(BPTR, close,
 AROS_LHA(struct IOFileSys *, iofs, A1),
      struct ACDRBase *, acdrbase, 2, acdrdev)
{
	AROS_LIBFUNC_INIT

	acdrbase->rport.mp_SigTask = FindTask(NULL);
	if (
			!g_cd ||
			(
				acdrbase->port.mp_MsgList.lh_Head->ln_Succ==0 &&
				(!g_volume && (!g_volume->locks && !g_volume->file_handles))
			)
		)
	{
		/* remove timer device and any pending timer requests: */
		if (g_timer_sigbit)
			Cleanup_Timer_Device(acdrbase);
		if (g_cd)
			Unmount(acdrbase);
		FreeMem((ULONG *)g_vol_name,128);
		if (g_cd)
			Cleanup_CDROM(acdrbase, g_cd);
		iofs->IOFS.io_Device = (struct Device *)-1;
		if (!--acdrbase->device.dd_Library.lib_OpenCnt)
		{
			/* Delayed expunge pending? */
			if (acdrbase->device.dd_Library.lib_Flags & LIBF_DELEXP)
			{
				/* Then expunge the device */
				return expunge();
			}
		}
	}
	else
		iofs->IOFS.io_Error = ERROR_OBJECT_IN_USE;
	return 0;
	AROS_LIBFUNC_EXIT
}

AROS_LH0I(int, null, struct ACDRBase *, acdrbase, 4, acdrdev)
{
	AROS_LIBFUNC_INIT
	return 0;
	AROS_LIBFUNC_EXIT
}

AROS_LH1(void, beginio,
 AROS_LHA(struct IOFileSys *, iofs, A1),
           struct ACDRBase *, acdrbase, 5, acdrdev)
{
	AROS_LIBFUNC_INIT
	/* WaitIO will look into this */
	iofs->IOFS.io_Message.mn_Node.ln_Type = NT_MESSAGE;
	/* Nothing is done quick */
	iofs->IOFS.io_Flags &= ~IOF_QUICK;
	/* So let the device task do it */
	PutMsg(&acdrbase->port, &iofs->IOFS.io_Message);
	AROS_LIBFUNC_EXIT
}

AROS_LH1(LONG, abortio,
 AROS_LHA(struct IOFileSys *, iofs, A1),
           struct ACDRBase *, acdrbase, 6, acdrdev)
{
	AROS_LIBFUNC_INIT
	return 0;
	AROS_LIBFUNC_EXIT
}



/*
 *  Since this code might be called several times in a row without being
 *  unloaded, you CANNOT ASSUME GLOBALS HAVE BEEN ZERO'D!!  This also goes
 *  for any global/static assignments that might be changed by running the
 *  code.
 */

PROC	*DosProc;     /* Our Process				    	*/
DEVNODE *DosNode;     /* Our DOS node.. created by DOS for us	    	*/
struct DeviceList *DevList;     /* Device List structure for our volume node   	*/

struct MsgPort *g_timer_mp;   		/*  timer message port		*/
struct timerequest *g_timer_io; /*  timer i/o request		*/
ULONG g_dos_sigbit;

char	g_device[80];		/* SCSI device name */
short	g_unit;			/* SCSI unit */
short	g_use_rock_ridge;	/* Use Rock Ridge flag 'R' */
short	g_maybe_map_to_lowercase;/* Conditional map to lower case flag 'ML' */
short	g_map_to_lowercase;	/* Map to lower case flag 'L' */
int     g_trackdisk;		/* Use trackdisk calls instead of SCSI-direct */

int	g_std_buffers;		/* Number of buffers for standard SCSI access */
int	g_file_buffers;		/* Number of buffers for contiguous reads */

t_bool	g_show_version_numbers; /* Show version numbers */

t_bool	g_disk_inserted;	/* Is a disk inserted? */

char	g_play_cdda_command[80];/* Command invoked if appicon is activated */

UBYTE   playing;		/* CD-DA play status */

struct MsgPort *DosTask;


void btos(BSTR, char *);
void *NextNode (NODE *);
LOCK *cdlock(CDROM_OBJ *, int);
void cdunlock (LOCK *);
char *typetostr (int);
int Check_For_Volume_Name_Prefix (char *);
void Fill_FileInfoBlock(struct ACDRBase *, FIB *, CDROM_INFO *, VOLUME *);
int Mount_Check(struct ACDRBase *);
void Check_Disk (struct ACDRBase *acdrbase);
void Send_Event(struct ACDRBase *, int);
void Install_TD_Interrupt (void);
void Uninstall_TD_Interrupt (void);
void Remove_Volume_Node (struct ACDRBase *, struct DeviceList *);

/*
 *  Don't call the entry point main().  This way, if you make a mistake
 *  with the compile options you'll get a link error.
 */
static const ULONG sizes[]=
{
    0,
    offsetof(struct ExAllData,ed_Type),
    offsetof(struct ExAllData,ed_Size),
    offsetof(struct ExAllData,ed_Prot),
    offsetof(struct ExAllData,ed_Days),
    offsetof(struct ExAllData,ed_Comment),
    offsetof(struct ExAllData,ed_OwnerUID),
    sizeof(struct ExAllData)
};

LONG SystemTagsO(struct ACDRBase *acdrbase, STRPTR command, Tag tag1, ...) {
	AROS_SLOWSTACKTAGS_PRE(tag1)
	retval = SystemTagList (command, AROS_SLOWSTACKTAGS_ARG(tag1));
	AROS_SLOWSTACKTAGS_POST
}

void handler (struct ACDRBase *acdrbase) {
struct IOFileSys *iofs;
short   error;
char    buf[256];
ULONG   signals;
playing = FALSE;

	acdrbase->port.mp_SigBit = SIGBREAKB_CTRL_F;
	acdrbase->port.mp_Flags = PA_SIGNAL;

	g_dos_sigbit = 1L << acdrbase->port.mp_SigBit;

	/*
		Here begins the endless loop, waiting for requests over our
		message port and executing them.  Since requests are sent over
		our message port, this precludes being able to call DOS functions
		ourselves (that is why the debugging routines are a separate process)
	*/

	for (;;)
	{
		signals = Wait(g_dos_sigbit | g_timer_sigbit | g_app_sigbit);
		if (signals & g_timer_sigbit)
		{
			GetMsg (g_timer_mp);
			g_time++;
			if (g_cd)
			{

				/* icon retry (every second): */
				if (g_retry_show_cdda_icon)
					Show_CDDA_Icon(acdrbase);
				/* diskchange check: */
				if (g_scan_interval)
				{
					if (g_scan_time == 1)
					{
						if (!g_inhibited)
							Check_Disk(acdrbase);
						g_scan_time = g_scan_interval;
					}
					else
						g_scan_time--;
				}
			}
			Send_Timer_Request(acdrbase);
		}
		if (signals & g_app_sigbit)
		{
		struct Message *msg;
			while ((msg = GetMsg (g_app_port)))
			{
				ReplyMsg (msg);
				if (g_play_cdda_command[0])
					SystemTagsO(acdrbase, (UBYTE *) g_play_cdda_command,
						SYS_Input, Open ((UBYTE *) "NIL:", MODE_OLDFILE),
						SYS_Output, Open ((UBYTE *) "NIL:", MODE_NEWFILE),
						SYS_Asynch, TRUE,
						TAG_END);
				else
				{
				int res;
					if (playing)
						res = Stop_Play_Audio(acdrbase, g_cd);
					else
						res = Start_Play_Audio(acdrbase, g_cd);
					if (!res)
						Display_Error(acdrbase, "Cannot perform play audio command!");
					else
						playing = !playing;
				}
			}
		}
		if (!(signals & g_dos_sigbit))
			continue;
		while ((iofs = (struct IOFileSys *)GetMsg(&acdrbase->port)))
		{
kprintf("got message %ld\n", iofs->IOFS.io_Command);
			error = 0;
			if (!g_cd)
			{
				iofs->io_DosError = ERROR_INVALID_COMPONENT_NAME;
				ReplyMsg(&iofs->IOFS.io_Message);
				continue;	        
			}
			else if (g_inhibited)
			{
				switch (iofs->IOFS.io_Command)
				{
				/* packets we will handle even if the handler is inhibited: */
				case FSA_INHIBIT:
				case FSA_MORE_CACHE:
				case FSA_DISK_INFO:
					break;
				/* packets we cannot handle because the handler is inhibited: */
				default:
					iofs->io_DosError = ERROR_NOT_A_DOS_DISK;
					ReplyMsg(&iofs->IOFS.io_Message);
					continue;	        
				}
			}
			else if (DevList == NULL)
			{
				switch (iofs->IOFS.io_Command)
				{
				/* packets we will handle even if no disk is inserted: */
				case ACTION_USER:
				case FSA_IS_FILESYSTEM:
				case FSA_INHIBIT:
				case FSA_MORE_CACHE:
				case FSA_CLOSE:
					break;
				/* packets we cannot handle because no disk is inserted: */
				default:
					iofs->io_DosError = (g_disk_inserted ? ERROR_NOT_A_DOS_DISK : ERROR_NO_DISK);
					ReplyMsg(&iofs->IOFS.io_Message);
					continue;	        
				}
			}
			switch(iofs->IOFS.io_Command)
			{
#if 0
			case ACTION_USER:	    /*  Mode,Par1,Par2		    Bool    */
				error = Handle_Control_Packet (packet->dp_Arg1,packet->dp_Arg2, packet->dp_Arg3);
				break;
#endif
			case FSA_OPEN_FILE:  /*  FileHandle,Lock,Name        Bool    */
			{
				if (Mount_Check (acdrbase))
				{
				CDROM_OBJ *obj;
				CDROM_OBJ *parentdir = (CDROM_OBJ *)iofs->IOFS.io_Unit;
				int       offs;

					if (parentdir->volume != g_volume)
					{
					/* old lock from another disk: */
						error = ERROR_DEVICE_NOT_MOUNTED;
						goto openbreak;
					}
					StrCpy(buf, iofs->io_Union.io_OPEN_FILE.io_Filename);
					offs = Check_For_Volume_Name_Prefix (buf);
					if (
							(iofs->io_Union.io_OPEN_FILE.io_FileMode==FMF_CLEAR) ||
							(iofs->io_Union.io_OPEN_FILE.io_FileMode==FMF_CREATE)
						)
					{
						if (offs && (buf[offs] ==0))
							error = ERROR_OBJECT_WRONG_TYPE;
						else
							error = ERROR_DISK_WRITE_PROTECTED;
						goto openbreak;
					}
					obj = Open_Object(acdrbase, parentdir, buf + offs);
					if (obj)
					{
						if (obj->symlink_f)
						{
							error = ERROR_IS_SOFT_LINK;
							goto openbreak;
						}
						if (obj->directory_f)
						{
							error = ERROR_OBJECT_WRONG_TYPE;
							goto openbreak;
						}
					}
					else
					{
						if (iso_errno == ISOERR_ILLEGAL_NAME)
						{
							error = ERROR_INVALID_COMPONENT_NAME;
							goto openbreak;
						}
						else if (iso_errno == ISOERR_NOT_FOUND)
							error = ERROR_OBJECT_NOT_FOUND;
						else if (iso_errno == ISOERR_NO_MEMORY)
						{
							error = ERROR_NO_FREE_STORE;
							goto openbreak;
						}
						else if (iso_errno == ISOERR_IS_SYMLINK)
						{
							error = ERROR_IS_SOFT_LINK;
							goto openbreak;
						}
						else
						{
							error = 333;
							goto openbreak;
						}
					}
					if (!error)
					{
						g_volume->file_handles++;
						iofs->IOFS.io_Unit = (struct Unit *)obj;
						Register_File_Handle(acdrbase, obj);
					}
					else
						iofs->IOFS.io_Unit = 0;
				}
				else
					error = ERROR_NO_DISK;
			}
			openbreak:
				break;
			case FSA_READ:	    /*	 FHArg1,CPTRBuffer,Length   ActLength  */
			{
			CDROM_OBJ *obj = (CDROM_OBJ *)iofs->IOFS.io_Unit;

				if (obj->volume != g_volume)
				{
				/* old lock from another disk: */
					error = ERROR_DEVICE_NOT_MOUNTED;
					break;
				}
				iofs->io_Union.io_READ.io_Length = Read_From_File
					(
						acdrbase,
						(CDROM_OBJ *)iofs->IOFS.io_Unit,
						(char *) iofs->io_Union.io_READ.io_Buffer,
						iofs->io_Union.io_READ.io_Length
					);
			}
			break;
			case FSA_CLOSE:	    /*	 FHArg1 		    Bool:TRUE  */
			{
			CDROM_OBJ *obj = (CDROM_OBJ *)iofs->IOFS.io_Unit;
kprintf("close(%lx)\n",iofs->IOFS.io_Unit);
#warning "TODO: in case of last handle removed -> see cdunlock()"
				if (obj->volume != g_volume)
				{
				/* old lock from another disk: */
					error = ERROR_DEVICE_NOT_MOUNTED;
					break;
				}
				Unregister_File_Handle(acdrbase, obj);
				Close_Object(acdrbase, obj);
				g_volume->file_handles--;
			}
			break;
			case FSA_SEEK:	    /*	 FHArg1,Position,Mode	    OldPosition*/
			{
			CDROM_OBJ *obj = (CDROM_OBJ *)iofs->IOFS.io_Unit;
		    
				if (obj->volume != g_volume)
				{
				/* old lock from another disk: */
					error = ERROR_DEVICE_NOT_MOUNTED;
					break;
				}
				if (
						!Seek_Position
							(
								obj,
								iofs->io_Union.io_SEEK.io_Offset,
								iofs->io_Union.io_SEEK.io_SeekMode
							)
					)
				{
					error = ERROR_SEEK_ERROR;
					iofs->io_Union.io_SEEK.io_Offset = -1;
				}
				else
					iofs->io_Union.io_SEEK.io_Offset = obj->pos;
			}
			break;
			case FSA_EXAMINE_NEXT: /*   Lock,Fib		      Bool */
			{
			FIB       *fib = iofs->io_Union.io_EXAMINE_NEXT.io_fib;
			CDROM_OBJ *dir = (CDROM_OBJ *)iofs->IOFS.io_Unit;
			CDROM_INFO info;
kprintf("examineNext(%lx)\n", iofs->IOFS.io_Unit);
				if (dir->volume != g_volume)
				{
					/* old lock from another disk: */
					error = ERROR_DEVICE_NOT_MOUNTED;
					break;
				}
				if (!dir->directory_f)
				{
					error = ERROR_OBJECT_WRONG_TYPE;
					break;
				}
				if (Examine_Next(acdrbase, dir, &info,(unsigned long *) &fib->fib_DiskKey))
				{
					error = 0;
					Fill_FileInfoBlock(acdrbase, fib, &info, dir->volume);
				}
				else
				{
					error = ERROR_NO_MORE_ENTRIES;
				}
				break;
			}
			case FSA_EXAMINE:     /*   FHArg1,Fib		      Bool */
kprintf("examine(%lx, %lx)\n", iofs->IOFS.io_Unit, iofs->io_Union.io_EXAMINE.io_Mode);
				if (iofs->io_Union.io_EXAMINE.io_Mode>ED_OWNER)
					error = ERROR_BAD_NUMBER;
				else
				{
					FIB fib;
					CDROM_INFO info;
					CDROM_OBJ *obj = (CDROM_OBJ *)iofs->IOFS.io_Unit;
					if (obj->volume != g_volume)
					{
						/* old lock from another disk: */
						error = ERROR_DEVICE_NOT_MOUNTED;
						break;
					}
					fib.fib_DiskKey = 0;
					error = 0;
					if (!CDROM_Info(acdrbase, obj, &info))
						error = -1;
					else
					{
					struct ExAllData *ead = iofs->io_Union.io_EXAMINE.io_ead;
					STRPTR next,end,name;
						next = (STRPTR)ead+sizes[iofs->io_Union.io_EXAMINE.io_Mode];
						end = (STRPTR)ead+iofs->io_Union.io_EXAMINE.io_Size;
						if (next>end)
							error = ERROR_BUFFER_OVERFLOW;
						else
						{
							Fill_FileInfoBlock(acdrbase, &fib, &info, obj->volume);
							switch (iofs->io_Union.io_EXAMINE.io_Mode)
							{
							case ED_OWNER :
								ead->ed_OwnerUID = fib.fib_OwnerUID;
								ead->ed_OwnerGID = fib.fib_OwnerGID;
							case ED_COMMENT :
								ead->ed_Comment = next;
								name = fib.fib_Comment;
								while (*name)
								{
									*next++ = *name++;
									if ((next+1)>end)
									{
										error = ERROR_BUFFER_OVERFLOW;
										goto examinebreak;
									}
								}
								*next = 0;
							case ED_DATE :
								ead->ed_Days = fib.fib_Date.ds_Days;
								ead->ed_Mins = fib.fib_Date.ds_Minute;
								ead->ed_Ticks = fib.fib_Date.ds_Tick;
							case ED_PROTECTION :
								ead->ed_Prot = fib.fib_Protection;
							case ED_SIZE :
								ead->ed_Size = fib.fib_Size;
							case ED_TYPE :
								ead->ed_Type = fib.fib_DirEntryType;
							case ED_NAME :
								ead->ed_Name = next;
								name = fib.fib_FileName;
								while (*name)
								{
									*next++ = *name++;
									if ((next+1)>end)
									{
										error = ERROR_BUFFER_OVERFLOW;
										goto examinebreak;
									}
								}
								*next = 0;
							case 0 :
								ead->ed_Next = (struct ExAllData *)
									(
										(
											(ULONG)next + AROS_PTRALIGN - 1
										) &
										~(AROS_PTRALIGN - 1)
									);
							}
							iofs->io_DirPos = fib.fib_DiskKey;
						}
					}
				}
			examinebreak:
				break;
			case FSA_DISK_INFO:  /*	InfoData	  Bool:TRUE    */
			{
#warning "volume check missing"
				if (Mount_Check(acdrbase))
				{
				INFODATA *id = iofs->io_Union.io_INFO.io_Info;
					error = 0;
					MemSet (id, 0, sizeof(*id));
					id->id_UnitNumber = g_unit;
					id->id_VolumeNode = MKBADDR(DevList);
					id->id_InUse = 0;
					id->id_DiskState = ID_WRITE_PROTECTED;
					if (g_inhibited)
					{
						id->id_DiskType = 0x42555359 /* "BUSY" */;
						id->id_NumBlocks	 = 0;
						id->id_BytesPerBlock = 0;
					}
					else
					{
						id->id_DiskType = ID_DOS_DISK;
						id->id_NumBlocks	 = Volume_Size (g_volume);
						id->id_BytesPerBlock = Block_Size (g_volume);
					}
					id->id_NumBlocksUsed = id->id_NumBlocks;
				}
			}
			break;
			case FSA_IS_FILESYSTEM:   /*  -                      Bool */
				iofs->io_Union.io_IS_FILESYSTEM.io_IsFilesystem = DOSTRUE;
				break;
			case FSA_OPEN:	/*   Lock,Name,Mode		Lock	   */
			{
#warning "lock is always shared"
#warning "I hope iofs->IOFS.io_Unit is NULL"
kprintf("open(%lx, %s)\n",iofs->IOFS.io_Unit, iofs->io_Union.io_OPEN.io_Filename);
				if (Mount_Check(acdrbase))
				{
				CDROM_OBJ *parentdir = (CDROM_OBJ *)iofs->IOFS.io_Unit;
				CDROM_OBJ *obj;
				int offs;
					if (parentdir->volume != g_volume)
					{
					/* old lock from another disk: */
						error = ERROR_DEVICE_NOT_MOUNTED;
						break;
					}
					StrCpy(buf, iofs->io_Union.io_OPEN.io_Filename);
					offs = Check_For_Volume_Name_Prefix (buf);
					if (buf[offs]==0)
					{
						if (parentdir)
							obj = Clone_Object(acdrbase, parentdir);
						else
							obj = Open_Top_Level_Directory(acdrbase, g_volume);
					}
					else
					{
						obj = Open_Object(acdrbase, parentdir, buf + offs);
					}
					if (obj)
					{
						if (obj->symlink_f)
							error = ERROR_IS_SOFT_LINK;
						else
						{
							g_volume->file_handles++;
							iofs->IOFS.io_Unit=(struct Unit *)obj;
kprintf("open()=%lx\n", obj);
							Register_File_Handle(acdrbase, obj);
						}
					}
					else
					{
						if (iso_errno == ISOERR_SCSI_ERROR)
						{
							error = ERROR_OBJECT_NOT_FOUND;
							Unmount(acdrbase);
						}
						else if (iso_errno == ISOERR_ILLEGAL_NAME)
							error = ERROR_INVALID_COMPONENT_NAME;
						else if (iso_errno == ISOERR_NOT_FOUND)
							error = ERROR_OBJECT_NOT_FOUND;
						else if (iso_errno == ISOERR_NO_MEMORY)
							error = ERROR_NO_FREE_STORE;
						else if (iso_errno == ISOERR_IS_SYMLINK)
							error = ERROR_IS_SOFT_LINK;
						else
							error = 333;
					}
				}
				else
				    error = ERROR_NO_DISK;
			}
			break;
#if 0
			case ACTION_COPY_DIR:   /*	 Lock,			    Lock       */
			{
				if (packet->dp_Arg1)
				{
				CDROM_OBJ *obj = (CDROM_OBJ *)iofs->IOFS.io_Unit;
				CDROM_OBJ *new;

					if (obj->volume != g_volume)
					{
					/* old lock from another disk: */
						error = ERROR_DEVICE_NOT_MOUNTED;
						break;
					}
					new = Clone_Object (obj);
					if (!new)
						error = ERROR_NO_FREE_STORE;
					else
						packet->dp_Res1 = (long) CTOB (cdlock (new, ACCESS_READ));
				}
				else
					packet->dp_Res1 = 0;
			}
			break;
			case ACTION_CURRENT_VOLUME: /* FHArg1                   DevList    */
			{
			CDROM_OBJ *obj = (CDROM_OBJ*) packet->dp_Arg1;
				if (obj)
					packet->dp_Res1 = (long) CTOB (Find_Dev_List (obj));
				else
					packet->dp_Res1 = (long) CTOB (DevList);
				break;
			}
			case ACTION_INHIBIT:    /*	 Bool			    Bool       */
				if (packet->dp_Arg1 != DOS_FALSE)
				{
				/* true meens forbid access */
					g_inhibited++;
					if (DevList)
						Unmount();
					Hide_CDDA_Icon ();
					g_cd->t_changeint2 = -2;
				}
				else
				{
				/* false meens access allowed */
					if (g_inhibited)
						g_inhibited--;
					if (g_inhibited == 0)
					{
						g_disk_inserted = FALSE;
						Check_Disk();
					}
				}
				break;
#endif
			/*
				FINDINPUT and FINDOUTPUT normally should return the
				'write protected' error. If the field 'Name', however,
				designates the root (e.g. CD0:), then the 'wrong type'
				error should be returned. Otherwise, AmigaDOS would do
				some funny things (such as saying 'Volume CD0: is write-
				protected') if you try to mount the handler with the
				field 'Mount' set to 1.
			*/
			case FSA_SAME_LOCK: /*    Lock  Lock                 Bool       */
kprintf("samelock(%lx,%lx\n",iofs->io_Union.io_SAME_LOCK.io_Lock[0], iofs->io_Union.io_SAME_LOCK.io_Lock[1]);
				iofs->io_Union.io_SAME_LOCK.io_Same=Same_Objects
					(
						(CDROM_OBJ *)iofs->io_Union.io_SAME_LOCK.io_Lock[0],
						(CDROM_OBJ *)iofs->io_Union.io_SAME_LOCK.io_Lock[1]
					) ? LOCK_SAME : LOCK_DIFFERENT;
kprintf("samelock()=%ld\n",iofs->io_Union.io_SAME_LOCK.io_Same);
				break;
			case FSA_READ_SOFTLINK: /*  Lock Name Buf Length         NumChar    */
			{
#if 0
			CDROM_OBJ *obj;
			CDROM_OBJ *parentdir = (CDROM_OBJ *)iofs->IOFS.io_Unit;
			char *outbuf = (char *)iofs->IOFS.io_Union.io_READ_SOFTLINK.io_Buffer;
			t_ulong maxlength = iofs->IOFS.io_Union.io_READ_SOFTLINK.io_Size;
			int offs;
			char buf[256];
			int res;

				if (parentdir->volume != g_volume)
				{
					/* old lock from another disk: */
					error = ERROR_DEVICE_NOT_MOUNTED;
					break;
				}
				offs = Check_For_Volume_Name_Prefix ((char *) packet->dp_Arg2);
				obj = Open_Object (parentdir, (char *) packet->dp_Arg2 + offs);
				if (obj)
				{
					res = Get_Link_Name (obj, buf, sizeof (buf));
					if (res == 0 ||
						strlen (buf) + strlen (g_vol_name+1) + 1 >= maxlength)
						strncpy (outbuf, "illegal_link", maxlength - 1);
					else
					{
						if (buf[0] == ':')
							strcpy (outbuf, g_vol_name+1);
						else
							outbuf[0] = 0;
						strcat (outbuf, buf);
					}
					outbuf[maxlength - 1] = 0;
					Close_Object (obj);
				}
				else
				{
					if (iso_errno == ISOERR_ILLEGAL_NAME)
						error = ERROR_INVALID_COMPONENT_NAME;
					else if (iso_errno == ISOERR_NOT_FOUND)
						error = ERROR_OBJECT_NOT_FOUND;
					else if (iso_errno == ISOERR_NO_MEMORY)
						error = ERROR_NO_FREE_STORE;
					else if (iso_errno == ISOERR_IS_SYMLINK)
						error = ERROR_IS_SOFT_LINK;
					else
						error = 333;
				}
#endif
				error = ERROR_ACTION_NOT_KNOWN;
				break;
			}
			case FSA_CREATE_HARDLINK:
			case FSA_CREATE_SOFTLINK:
			case FSA_RELABEL:
			case FSA_WRITE:
			case FSA_SET_PROTECT:
			case FSA_DELETE_OBJECT:
			case FSA_RENAME:
			case FSA_CREATE_DIR:
			case FSA_SET_COMMENT:
			case FSA_SET_DATE:
			case FSA_SET_FILE_SIZE:
				error = ERROR_DISK_WRITE_PROTECTED;
				break;
			/*
				A few other packet types which we do not support
			*/
			case FSA_MORE_CACHE: /*	 #BufsToAdd		    Bool       */
			case FSA_WAIT_CHAR:  /*	 Timeout, ticks 	    Bool       */
			default:
				error = ERROR_ACTION_NOT_KNOWN;
				break;
			}
			if (iofs)
			{
				iofs->io_DosError = error;
				ReplyMsg(&iofs->IOFS.io_Message);
			}
		}
	}
}


/*
 *  PACKET ROUTINES.	Dos Packets are in a rather strange format as you
 *  can see by this and how the PACKET structure is extracted in the
 *  GetMsg() of the main routine.
 */

/*
 *  DOS MEMORY ROUTINES
 *
 *  DOS makes certain assumptions about LOCKS.	A lock must minimally be
 *  a FileLock structure, with additional private information after the
 *  FileLock structure.  The longword before the beginning of the structure
 *  must contain the length of structure + 4.
 *
 *  NOTE!!!!! The workbench does not follow the rules and assumes it can
 *  copy lock structures.  This means that if you want to be workbench
 *  compatible, your lock structures must be EXACTLY sizeof(struct FileLock).
 */

/*
 *  Convert a BSTR into a normal string.. copying the string into buf.
 *  I use normal strings for internal storage, and convert back and forth
 *  when required.
 */

void btos(BSTR bstr, char *buf) {
int len=AROS_BSTR_StrLen(bstr);

	buf[len]=0;
	while (len)
	{
		buf[len]=AROS_BSTR_getchar(bstr, len);
		len--;
	}
}

/*
 *  Some EXEC list handling routines not found in the EXEC library.
 */

void *NextNode (NODE *node)
{
    node = node->mln_Succ;
    if (node->mln_Succ == NULL)
	return(NULL);
    return(node);
}

#if 0
/*
 *  The lock function.	The file has already been checked to see if it
 *  is lockable given the mode.
 */

LOCK *cdlock(CDROM_OBJ *cdfile, int mode)
{
  LOCK *lock = dosalloc (sizeof(LOCK));

  cdfile->volume->locks++;
  lock->fl_Key = (LONG) Location (cdfile);
  lock->fl_Link = (long) cdfile;
  lock->fl_Access = ACCESS_READ;
  lock->fl_Task = &DosProc->pr_MsgPort;
  lock->fl_Volume = (BPTR)MKBADDR(DevList);
  Register_Lock (lock);
  return(lock);
}

void cdunlock (LOCK *lock)
{
  CDROM_OBJ *obj = (CDROM_OBJ *) lock->fl_Link;

  Unregister_Lock (lock);
  --obj->volume->locks;
  
  /* if all locks and filehandles have been removed, and if the volume
   * is not the current volume, then the volume node may be removed:
   */
  if (obj->volume != g_volume &&
      obj->volume->locks == 0 && obj->volume->file_handles == 0) {     
    VOLUME *vol = obj->volume;
    
    Forbid ();
      Remove_Volume_Node (vol->devlist);
    Permit ();
    Close_Object (obj);
    Close_Volume (vol);
    Send_Event (FALSE);
  } else
    Close_Object (obj);

  dosfree ((ulong *) lock);				/* free lock	    */
}
#endif

/*
 *  GETLOCKFILE(bptrlock)
 *
 *  Return the CDROM_OBJ (file or directory) associated with the
 *  given lock, which is passed as a BPTR.
 *
 *  According to the DOS spec, the only way a NULL lock will ever be
 *  passed to you is if the DosNode->dn_Lock is NULL, but I'm not sure.
 *  In anycase, If a NULL lock is passed to me I simply assume it means
 *  the root directory of the CDROM.
 */


/*
 * If p_pathname contains a ':' character, return the position of the first
 * character after ':'
 * Otherwise, return 0.
 */

int Check_For_Volume_Name_Prefix (char *p_pathname)
{
  char *pos = StrChr (p_pathname, ':');
  
  return pos ? (pos - p_pathname) + 1 : 0;
}

/*
 * Fills a FileInfoBlock with the information contained in the
 * directory record of a CD-ROM directory or file.
 */

void Fill_FileInfoBlock
	(
		struct ACDRBase *acdrbase,
		FIB *p_fib,
		CDROM_INFO *p_info,
		VOLUME *p_volume
	)
{
char *src = p_info->name;
char *dest = AROS_BSTR_ADDR(p_fib->fib_FileName);
int len = p_info->name_length;

	if (p_info->symlink_f)
		p_fib->fib_DirEntryType = ST_SOFTLINK;
	else
		p_fib->fib_DirEntryType = p_info->directory_f ? ST_USERDIR : ST_FILE;

	/*
		I don't know exactly why I have to set fib_EntryType, but other
		handlers (e.g. DiskHandler by J Toebes et.al.) also do this.
	*/

	p_fib->fib_EntryType = p_fib->fib_DirEntryType;

	if (len == 1 && *src == ':')
	{
		/* root of file system: */
		p_fib->fib_DirEntryType = ST_ROOT;
		/* file name == volume name: */
		CopyMem(g_vol_name+1, AROS_BSTR_ADDR(p_fib->fib_FileName), (int)(g_vol_name[0])+1);
	}
	else
	{
		/* copy file name: */
		if (g_show_version_numbers)
		{
			AROS_BSTR_setstrlen(p_fib->fib_FileName,len);
			for (; len; len--)
				*dest++ = *src++;
		}
		else
		{
		short i, real_len=len;
			for (i=0; i<len; i++)
			{
				if (*src == ';')
					real_len = i;
				*dest++ = *src++;
			}
			AROS_BSTR_setstrlen(p_fib->fib_FileName, real_len);
		}

		if (
				(g_map_to_lowercase && p_volume->protocol == PRO_ISO) ||
				(g_maybe_map_to_lowercase && !p_volume->mixed_char_filenames)
			)
		{
		/* convert ISO filename to lowercase: */
		int i, len = AROS_BSTR_StrLen(p_fib->fib_FileName);
		char *cp = AROS_BSTR_ADDR(p_fib->fib_FileName);

			for (i=0; i<len; i++, cp++)
				*cp = ToLower (*cp);
		}

		/* truncate to 30 characters and terminate with null: */
		{
		int len = AROS_BSTR_StrLen(p_fib->fib_FileName);
			if (len > 30)
			{
				AROS_BSTR_setstrlen(p_fib->fib_FileName,30);
				len = 30;
			}
//			p_fib->fib_FileName[len+1] = 0;
		}
	}
	p_fib->fib_Protection = 0;
	p_fib->fib_Size = p_info->file_length;
	p_fib->fib_NumBlocks = p_info->file_length >> 11;
	if (p_info->symlink_f)
	{
		StrCpy(AROS_BSTR_ADDR(p_fib->fib_Comment),"Symbolic link");
		AROS_BSTR_setstrlen(p_fib->fib_Comment,13);
	}
	else
		AROS_BSTR_setstrlen(p_fib->fib_Comment, 0);

	p_fib->fib_Date.ds_Days   = p_info->date / (24 * 60 * 60);
	p_fib->fib_Date.ds_Minute = (p_info->date % (24 * 60 * 60)) / 60;
	p_fib->fib_Date.ds_Tick   = (p_info->date % 60) * TICKS_PER_SECOND;
kprintf("examined %s\n",AROS_BSTR_ADDR(p_fib->fib_FileName));
}

/*
 * Create Volume node and add to the device list. This will
 * cause the WORKBENCH to recognize us as a disk. If we don't
 * create a Volume node, Wb will not recognize us.
 */
 
void Create_Volume_Node
	(
		struct ACDRBase *acdrbase,
		LONG p_disk_type,
		ULONG p_volume_date
	)
{
struct DosList *dl;

	dl = (struct DosList *)Find_Volume_Node(acdrbase, g_vol_name+1);
	if (dl)
	{
		DevList = (struct DeviceList *)dl;
	}
	else
	{
		DevList = (struct DeviceList *)dl =
			MakeDosEntry(g_vol_name+1, DLT_VOLUME);
//		dl->dl_DiskType = p_disk_type;
		dl->dol_Unit = (struct Unit *)g_top_level_obj;
		dl->dol_Device = &acdrbase->device;
		dl->dol_misc.dol_volume.dol_VolumeDate.ds_Days =
			p_volume_date / (24 * 60 * 60);
		dl->dol_misc.dol_volume.dol_VolumeDate.ds_Minute =
			(p_volume_date % (24 * 60 * 60)) / 60;
		dl->dol_misc.dol_volume.dol_VolumeDate.ds_Tick =
			(p_volume_date % 60) * TICKS_PER_SECOND;
		AddDosEntry((struct DosList *)dl);
		Register_Volume_Node(acdrbase, (struct DeviceList *)dl);
	}
}

/*
 * Mount a volume.
 */

void Mount(struct ACDRBase *acdrbase) {
char buf[33];

	if (Has_Audio_Tracks(acdrbase, g_cd))
		Show_CDDA_Icon(acdrbase);
	g_volume = Open_Volume(acdrbase, g_cd, g_use_rock_ridge);
	if (!g_volume)
	{
		return;
	}
	else
	{
		g_disk_inserted = TRUE;
		g_cd->t_changeint2 = g_cd->t_changeint;
		g_top_level_obj = Open_Top_Level_Directory(acdrbase, g_volume);
		if (!g_top_level_obj)
		{
			return;
		}
	}

	Volume_ID (acdrbase, g_volume, buf, sizeof (buf)-1);  
	g_vol_name[0] = StrLen (buf);
	CopyMem(buf, g_vol_name+1, StrLen (buf));

	if (!(g_vol_name[0]))
		CopyMem("\7Unnamed", g_vol_name, 8);

	/* AmigaDOS expects the BCPL string g_vol_name to be null-terminated: */
	g_vol_name[(int)(g_vol_name[0])+1] = 0;

	/* convert ISO volume name to lowercase: */
	if (
			(g_map_to_lowercase && g_volume->protocol == PRO_ISO) ||
			(g_maybe_map_to_lowercase && !g_volume->mixed_char_filenames)
		)
	{
	int i;
		for (i=0; i<g_vol_name[0]; i++)
			g_vol_name[i+1] = ToLower (g_vol_name[i+1]);
	}

	g_volume->file_handles = Reinstall_File_Handles ();

	Create_Volume_Node
		(
			acdrbase, ID_DOS_DISK, Volume_Creation_Date(acdrbase, g_volume)
		);
	g_volume->devlist = DevList;
	g_cd->t_changeint2 = g_cd->t_changeint;
	Send_Event(acdrbase, TRUE);
}

/*  Remove a volume node from the DOS list.
 *  (Must be nested between Forbid() and Permit()!)
 *  Since DOS uses singly linked lists, we must (ugg) search it manually to find
 *  the link before our Volume entry.
 */

void Remove_Volume_Node
	(
		struct ACDRBase *acdrbase,
		struct DeviceList* p_volume
	)
{
struct DeviceList *dl;

	Unregister_Volume_Node(acdrbase, p_volume);

	dl = (struct DeviceList *)LockDosList(LDF_WRITE | LDF_VOLUMES);
	if (dl)
	{
		while ((dl) && (dl != p_volume))
			dl = (struct DeviceList *)NextDosEntry((struct DosList *)dl, LDF_VOLUMES);
		if (dl == p_volume)
		{
			RemDosEntry((struct DosList *)dl);
			FreeDosEntry((struct DosList *)dl);
		}
		UnLockDosList(LDF_WRITE | LDF_VOLUMES);
	}
}

/*  Unmount a volume, and optionally unload the handler code.
 */

void Unmount(struct ACDRBase *acdrbase) {

	g_cd->t_changeint2 = g_cd->t_changeint;
	Hide_CDDA_Icon(acdrbase);

	Forbid ();
	if (DevList)
	{
		Close_Object(acdrbase, g_top_level_obj);

		if (g_volume->locks == 0 && g_volume->file_handles == 0)
		{
			Remove_Volume_Node (acdrbase, DevList);
			Close_Volume(acdrbase, g_volume);
		}
		else
		{
//			DevList->dl_Task = NULL;
		}
		DevList = NULL;
	}
	Send_Event(acdrbase, FALSE);
	g_volume = 0;
	Permit ();
}

/*
 * Mount_Check returns 1 if a valid disk is inserted in the drive. A check is
 * only performed if previously the drive was empty.
 */

int Mount_Check(struct ACDRBase *acdrbase) {

	if (!g_disk_inserted)
	{
		/*
			No disk was inserted up to now: we will check whether
			a disk has been inserted by sending the test unit ready
			command. We have to send the command twice because
			the first SCSI command after inserting a new disk is
			always rejected.
		*/
		if (
				Test_Unit_Ready (acdrbase, g_cd) ||
				Test_Unit_Ready (acdrbase, g_cd)
			)
		{
			g_disk_inserted = TRUE;
			Mount(acdrbase);
		}
		else
		{
			return 0;
		}
		if (DevList)
			return 1;
		else
		{
			/* Mount() did not succeed: */
			return 0;
		}
	}
	return 1;
}

/*
 *  Open timer device structures:
 */

int Open_Timer_Device(struct ACDRBase *acdrbase) {

	g_timer_mp = CreateMsgPort ();
	if (!g_timer_mp)
	{
		return 0;
	}
	g_timer_io = (struct timerequest *)CreateIORequest (g_timer_mp, sizeof (struct timerequest));
	if (!g_timer_io)
	{
		DeleteMsgPort (g_timer_mp);
		return 0;
	}
	if (OpenDevice ((UBYTE *) TIMERNAME, UNIT_VBLANK,(struct IORequest *) g_timer_io, 0))
	{
		DeleteIORequest ((struct IORequest *) g_timer_io);
		DeleteMsgPort (g_timer_mp);
		return 0;
	}
	g_timer_sigbit = 1L << g_timer_mp->mp_SigBit;
	g_timer_mp->mp_SigTask = acdrbase->port.mp_SigTask;
	return 1;
}

/*
 *  Remove timer device structures:
 */

void Cleanup_Timer_Device (struct ACDRBase *acdrbase) {
  /* remove any pending requests: */
	if (!CheckIO ((struct IORequest *) g_timer_io))
		AbortIO ((struct IORequest *) g_timer_io);
	WaitIO ((struct IORequest *) g_timer_io);

	CloseDevice ((struct IORequest *) g_timer_io);
	DeleteIORequest ((struct IORequest *) g_timer_io);
	DeleteMsgPort (g_timer_mp);
}

/*
 *  Send timer request
 */

void Send_Timer_Request(struct ACDRBase *acdrbase) {
	g_timer_io->tr_node.io_Command = TR_ADDREQUEST;
	g_timer_io->tr_time.tv_secs = 1;
	g_timer_io->tr_time.tv_micro = 0;
	SendIO ((struct IORequest *) g_timer_io);
}

/*
 *  Check whether the disk has been removed or inserted.
 */

void Check_Disk(struct ACDRBase *acdrbase) {
int i;
unsigned long l1, l2;

	if (g_cd->use_trackdisk)
	{
		i =
			(
				Test_Unit_Ready(acdrbase, g_cd) ||
				Test_Unit_Ready(acdrbase, g_cd)
			);
		l1 = g_cd->t_changeint;
		l2 = g_cd->t_changeint2;
		if (l1!=l2 && i)
		{
			g_disk_inserted = TRUE;
			if (DevList)
				Unmount (FALSE);
			Delay (50);
			Clear_Sector_Buffers (g_cd);
			Mount(acdrbase);
		}
		if (l1!=l2 && !i)
		{
			g_disk_inserted = FALSE;
			playing = FALSE;
			if (DevList)
				Unmount (FALSE);
			g_cd->t_changeint2 = g_cd->t_changeint;
		}
	}
	else
	{
		if (g_disk_inserted)
		{
			if (Test_Unit_Ready(acdrbase, g_cd))
			{
			}
			else
			{
				g_disk_inserted = FALSE;
				playing = FALSE;
				if (DevList)
					Unmount (FALSE);
				Hide_CDDA_Icon(acdrbase);
			}
		}
		if (!g_disk_inserted)
		{
			if (
					Test_Unit_Ready(acdrbase, g_cd) ||
					Test_Unit_Ready(acdrbase, g_cd)
				)
			{
				g_disk_inserted = TRUE;
				Clear_Sector_Buffers (g_cd);
				Mount(acdrbase);
			}
		}
	}
}

/* The following lines will generate a `disk inserted/removed' event, in order
 * to get Workbench to rescan the DosList and update the list of
 * volume icons.
 */

void Send_Event(struct ACDRBase *acdrbase, int p_inserted) {
struct IOStdReq *InputRequest;
struct MsgPort *InputPort;

	InputPort = (struct MsgPort *) CreateMsgPort ();
	if (InputPort)
	{
		InputRequest = (struct IOStdReq *)CreateIORequest
			(InputPort, sizeof (struct IOStdReq));
		if (InputRequest)
		{
			if (!OpenDevice ((UBYTE *) "input.device", 0,
      		       (struct IORequest *) InputRequest, 0))
			{
			static struct InputEvent InputEvent;

				MemSet (&InputEvent, 0, sizeof (struct InputEvent));
				InputEvent.ie_Class = p_inserted ? IECLASS_DISKINSERTED : IECLASS_DISKREMOVED;
				InputRequest->io_Command = IND_WRITEEVENT;
				InputRequest->io_Data = &InputEvent;
				InputRequest->io_Length = sizeof (struct InputEvent);

				DoIO ((struct IORequest *) InputRequest);

				CloseDevice ((struct IORequest *) InputRequest);
			}
			DeleteIORequest ((struct IORequest *)InputRequest);
		}
		DeleteMsgPort (InputPort);
	}
}

static const char acdrhandlerend = 0;

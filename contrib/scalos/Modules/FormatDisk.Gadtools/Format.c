// Format.c
// $Date$
// $Revision$


/************************************************************************************************/
/*				  	Format64			  			*/
/*			       		Version 1.05			  			*/
/*									  			*/
/*	This is a replacement for the AmigaDOS "Format" command.  It sports    			*/
/*	(for the Workbench user) more friendly user-interface, although it is  			*/
/*	virtually identical to the standard Format command where the CLI	  		*/
/*	interface is concerned.						  			*/
/*									  			*/
/*	NewFormat v1.00 is Copyright 1992 by Dave Schreiber, All Rights Reserved.		*/
/*	Format64  v1.00 is Copyright 1998 by Jarkko Vatjus-Anttila				*/
/*	This program may not be sold for more than a small copying and shipping			*/
/*	and handling fee, except by written permission of Dave Schreiber.	  		*/
/*									  			*/
/*	Version list:							  			*/
/*		1.00	- First release (August 31, 1992)                              		*/
/*		1.00	- Update to 64 bit command set. Support for disks >4GB         		*/
/*		1.01	- Small bug fixes. Made the program to use ANSI C functions.		*/
/*										   		*/
/*		1.05 (jmc)									*/
/*			- Bug fix: Some drives weren't initialized.				*/
/*			- Bug fix: Some devices weren't open.					*/
/*			- GUI.c Improvemnts : Added "DirCache", "Trashcan"			*/
/*			  field gadgets, replaced, "Quick Format" chekbox			*/
/*			  gadget by a button gadget.						*/
/*			  Added messages string informations about Drive, Volume,		*/
/*			  if drive support NSD or not.						*/
/*			- Added: "FindFileSysResEntry()" from information.module.		*/
/*												*/
/************************************************************************************************/


//-------------------------------------------
//---------------     MEMO      -------------
//-------------------------------------------
// UNREADABLE_DISK	42414400	BAD\0
// DOS_DISK		444F5300	DOS\0
// FFS_DISK		444F5301	DOS\1
// INTER_DOS_DISK	444F5302	DOS\2
// INTER_FFS_DISK	444F5303	DOS\3
// FASTDIR_DOS_DISK	444F5304	DOS\4
// FASTDIR_FFS_DISK	444F5305	DOS\5
// NOT_REALLY_DOS	4E444F53	NDOS
// KICKSTART_DISK	4B49434B	KICK
// MSDOS_DISK		4d534400	MSD\0
//-------------------------------------------
//-------------------------------------------

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

#include <exec/types.h>
#include <exec/exec.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/io.h>

#include <intuition/intuition.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <dos/filehandler.h>

#include <workbench/startup.h>
#include <libraries/gadtools.h>
#include <workbench/icon.h>
#include <devices/trackdisk.h>
#include <dos/rdargs.h>

#include <clib/alib_protos.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/icon.h>
#include <proto/graphics.h>

#include <resources/filesysres.h>	// +jmc+

#include "Format.h"
#include "GUI.h"

#include <defs.h>
#include <Year.h>
#include "Format_disk_rev.h"



#include <proto/locale.h>
#define	FormatDisk_NUMBERS
#define	FormatDisk_ARRAY
#define	FormatDisk_CODE
#include STR(SCALOSLOCALE)

#include <libraries/asl.h>
#include <proto/asl.h>
//-----------------------------------------------------------------------------

BOOL QuickFmt=FALSE;
BOOL Verify=TRUE;
BOOL Icon=FALSE;
BOOL FFS;
BOOL intl;
BOOL DirCache;		// +jmc+
BPTR StdErr = (BPTR)NULL;
BOOL fromcli;

//-----------------------------------------------------------------------------

LONG args[7] = { 0,0,0,0,0,0,0 };

extern Rect box;

//-----------------------------------------------------------------------------

char *Version = VERSTAG COPYRIGHT;
char temp[32];
char volumeName[256];
extern char deviceName[64];
extern char NewVolumeName[];
char DosTypeString[16];
STRPTR ShortDostype="";

//-----------------------------------------------------------------------------

static void BtoCstring(BSTR bstr, STRPTR Buffer, size_t BuffLen);
static UBYTE MakePrintable(UBYTE ch);
static void GetDeviceName(BPTR dLock, STRPTR DeviceName, size_t MaxLen);
static const struct FileSysEntry *FindFileSysResEntry(ULONG DosType);
static CONST_STRPTR FindVersionString(const UBYTE *block, size_t BlockLen);

const char versTag[] = VERSTAG;	     // $VER version string

static char TextDeviceName[128];
static char TextDeviceOnDev[64];
static char TextDeviceHandler[128];

static ULONG TypeNewDostype;
BOOL FastFSType;

//-----------------------------------------------------------------------------

#if !defined(__amigaos4__) && !defined(__AROS__)
#include <dos.h>

long _stack = 16384;	// stack size.
#endif

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Library *GadToolsBase, *IconBase, *DiskfontBase;
#ifndef __amigaos4__
T_UTILITYBASE UtilityBase;
#endif
T_LOCALEBASE LocaleBase = NULL;

#ifdef __amigaos4__
struct IntuitionIFace *IIntuition;
struct GraphicsIFace *IGraphics;
struct GadToolsIFace *IGadTools;
struct IconIFace *IIcon;
struct LocaleIFace *ILocale;
struct DiskfontIFace *IDiskfont;
#endif

//-----------------------------------------------------------------------------

static struct Locale *FormatDiskLocale;
static struct Catalog *FormatDiskCatalog;

struct FormatDateHookData
	{
	STRPTR fdhd_Buffer;		// buffer to write characters to
	size_t fdhd_Length;		// length of buffer
	};

static STRPTR ScaFormatString(CONST_STRPTR FormatString, STRPTR Buffer, size_t MaxLen, LONG NumArgs, ...);

static M68KFUNC_P3_PROTO(void, FormatDateHookFunc,
	A0, struct Hook *, theHook,
	A2, struct Locale *, locale,
	A1, char, ch);

static void ByteCount(STRPTR Buffer, size_t BuffLen, LONG NumBlocks, LONG BytesPerBlock);
static char TextDeviceUsed[128];
static char TextDeviceFree[128];
static char TextDeviceSize[128];
static char DeviceBlockSize[128];

static char BufferRequest[512]="";

//static char DeviceState[128];

extern LONG TextHighCyl;
extern LONG TextLayoutBPT;
LONG TestCapacity = 0;

//-----------------------------------------------------------------------------

BOOL checkfor64bitcommandset(struct IOStdReq *io);


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


int main(int argc, char *argv[])
{
	UWORD disk;
	ULONG i;
	char statusString[80], driveName[64];
	char newName[108];
	char TrashName[108] = "";
	BPTR driveLock;
	struct Process *process;
	APTR oldWdw;
	DriveLayout junk;
	prepResult stat=eCancel; // +jmc+
	int SetFFS;
	int SetIntl;
	int SetDirCache;	// +jmc
	struct FormatSettings Parms;
	struct WBStartup *WBenchMsg = (struct WBStartup *)argv;


	memset(&Parms, 0, sizeof(Parms));
	Parms.newName = newName;
	Parms.TrashName = TrashName;
	Parms.ffs = ID_DOS_DISK;

	//  Open the various shared libraries that are needed
	IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library",39);
	GfxBase = (struct GfxBase *) OpenLibrary("graphics.library", 39);
	IconBase = (struct Library *) OpenLibrary("icon.library", 39);
	GadToolsBase = (struct Library *) OpenLibrary("gadtools.library",39);
#ifndef __amigaos4__
	UtilityBase = (T_UTILITYBASE) OpenLibrary("utility.library", 39);
#endif
	DiskfontBase = (struct Library *) OpenLibrary("diskfont.library", 39);

	LocaleBase = (T_LOCALEBASE) OpenLibrary("locale.library", 39);

	if (IntuitionBase==NULL || GfxBase==NULL || IconBase==NULL || GadToolsBase==NULL
		|| DiskfontBase==NULL
#ifndef __amigaos4__
		|| UtilityBase==NULL
#endif
	)
		cleanup(100);

#ifdef __amigaos4__
	IIntuition = (struct IntuitionIFace *)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
	IGraphics = (struct GraphicsIFace *)GetInterface((struct Library *)GfxBase, "main", 1, NULL);
	IIcon = (struct IconIFace *)GetInterface((struct Library *)IconBase, "main", 1, NULL);
	IGadTools = (struct GadToolsIFace *)GetInterface((struct Library *)GadToolsBase, "main", 1, NULL);
	IDiskfont = (struct DiskfontIFace *)GetInterface((struct Library *)DiskfontBase, "main", 1, NULL);
	if (LocaleBase)
		ILocale = (struct LocaleIFace *)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);

	if (IIntuition==NULL || IGraphics==NULL || IIcon==NULL || IGadTools==NULL
		|| IDiskfont==NULL
	)
		cleanup(100);
#endif

	if (LocaleBase)
		{
		FormatDiskLocale = OpenLocale(NULL);
		FormatDiskCatalog = OpenCatalogA(NULL, "Scalos/FormatDisk.catalog", NULL);
		}

	d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	fromcli = argc != 0;

	if (!fromcli)
		{
		if (WBenchMsg->sm_NumArgs==1)
			{
			printError("Please select a drive to format and try again!",NULL,NULL);
			cleanup(100);
			}

		TypeNewDostype = 0UL;
		FastFSType = TRUE;
		strcpy(TextDeviceSize,"");

		// From GUI.c
		SetupScreen();

		for (disk = 1; disk < WBenchMsg->sm_NumArgs && stat != eQuit; disk++)
			{
			STRPTR LockStr;

			//  Get the volume/device name of the disk to format
			getVolumeName(volumeName,WBenchMsg->sm_ArgList,disk);

			//  If there is no lock to the volume, that means it is
			//  unformatted, so just use the name given to us by Workbench
			if (WBenchMsg->sm_ArgList[disk].wa_Lock==(BPTR)NULL)
				{
				strcpy(volumeName,WBenchMsg->sm_ArgList[disk].wa_Name);

				d1(KPrintF("%s/%s/%ld: * GET *** volumeToDevName(%lx,%s,%lx)\n", __FILE__, __FUNC__, __LINE__, WBenchMsg->sm_ArgList[disk].wa_Lock,volumeName,&junk));

				volumeToDevName(WBenchMsg->sm_ArgList[disk].wa_Lock, volumeName, &junk);

				d1(KPrintF("%s/%s/%ld: LOCK=NULL volumeName: Device Name=<%s>\n", __FILE__, __FUNC__, __LINE__, volumeName));
				d1(KPrintF("%s/%s/%ld: LOCK=NULL volumeName:junk.devName=<%s> junk.unit=[%ld]\n", __FILE__, __FUNC__, __LINE__, junk.devName, junk.unit));

				}
			else
				{
				if ((LockStr = (STRPTR)AllocVec(256,0)) != NULL)
					{
					size_t len;

					NameFromLock(WBenchMsg->sm_ArgList[disk].wa_Lock, (STRPTR)LockStr, 256);

					if (WBenchMsg->sm_ArgList[disk].wa_Name && strlen(WBenchMsg->sm_ArgList[disk].wa_Name) > 0)
						AddPart(LockStr, WBenchMsg->sm_ArgList[disk].wa_Name, 256);

					len=strlen(LockStr);

					if(len)
						{
						if(LockStr[len-1] != ':')
							{
							d1(KPrintF("%s/%s/%ld: Wrong type!!! len = %ld= %s\n", __FILE__, __FUNC__, __LINE__, len, LockStr));

							printError((STRPTR) GetLocString(MSGID_OBJECT), FilePart(LockStr), (STRPTR) GetLocString(MSGID_IS_WRONG_TYPE));
							return ERROR_OBJECT_WRONG_TYPE;
							}
						else
							{
							if (!volumeToDevName(WBenchMsg->sm_ArgList[disk].wa_Lock,deviceName,&junk))
								{
      								printError((STRPTR) GetLocString(MSGID_DRIVE_NOFOUND),(STRPTR) GetLocString(MSGID_VOLUME_SELECTED), LockStr);
								d1(KPrintF("%s/%s/%ld: Can't find the drive for %s\n", __FILE__, __FUNC__, __LINE__, LockStr));
								return RETURN_WARN;
								}
							}
						}
					FreeVec(LockStr);
					}

				d1(KPrintF("%s/%s/%ld: LOCK=OK BUFFER deviceName: Device Name=<%s>\n", __FILE__, __FUNC__, __LINE__, deviceName));
				d1(KPrintF("%s/%s/%ld: LOCK=OK BUFFER deviceName:junk.devName=<%s> junk.unit=[%ld]\n", __FILE__, __FUNC__, __LINE__, junk.devName, junk.unit));

				}

			GetDeviceName(WBenchMsg->sm_ArgList[disk].wa_Lock, deviceName, strlen(deviceName)); // +jmc+

			d1(KPrintF("%s/%s/%ld: ** FIRST CHECK DOS TYPE TO USE: TypeNewDostype=[%lx] **\n", __FILE__, __FUNC__, __LINE__, TypeNewDostype));

//-----------------------------------------------------------------------------

			if ((TypeNewDostype == ID_FFS_DISK) || (TypeNewDostype == ID_FASTDIR_FFS_DISK))	// 444F5301 [DOS\1] or 444F5305 [DOS\5] or 444F5300 [DOS\0].
				{
				FFS = TRUE;
				SetFFS = 1;
				}
			else
				{
				FFS = FALSE;
				SetFFS = 0;
				}

			if ((TypeNewDostype == ID_INTER_FFS_DISK) || (TypeNewDostype == ID_INTER_DOS_DISK))	// 444F5303 [DOS\3] or 444F5302 [DOS\2].
				{
				intl = TRUE;
				SetIntl = 1;
				}
			else
				{
				intl = FALSE;
				SetIntl = 0;
				}


			if ((TypeNewDostype == ID_FASTDIR_FFS_DISK) || (TypeNewDostype == ID_FASTDIR_DOS_DISK))	// 444F5305 [DOS\5] or 444F5304	[DOS\4].
				{
				DirCache = TRUE;
				SetDirCache = 1;
				}
			else
				{
				DirCache = FALSE;
				SetDirCache = 0;
				}

			d1(KPrintF("%s/%s/%ld: ** FIRST CHECK FFS: FFS=[%ld] **\n", __FILE__, __FUNC__, __LINE__, FFS));

			if (FFS == TRUE)
				{
				if (DirCache == TRUE)
					Parms.ffs = ID_FASTDIR_FFS_DISK;
				else if (intl == TRUE)
					Parms.ffs = ID_INTER_FFS_DISK;
				else
					Parms.ffs = ID_FFS_DISK;
				}
			else
      				{
				if (DirCache == TRUE)
					Parms.ffs = ID_FASTDIR_DOS_DISK;
				else if (intl == TRUE)
					Parms.ffs = ID_INTER_DOS_DISK;
				else
					Parms.ffs = ID_DOS_DISK;
				SetFFS = 1;				// Here: Force enabling of FFS gadget in all case.
				}


			if ( (strlen(NameOfDevice) > 0) &&  (0 == (stricmp(NameOfDevice, "mfm.device"))) )
				{
				TypeNewDostype = ID_MSDOS_DISK;
				d1(KPrintF("%s/%s/%ld: FOUND MSDOS drive Dostype = ««%lx»»\n", __FILE__, __FUNC__, __LINE__, TypeNewDostype));
				}


			d1(KPrintF("%s/%s/%ld: ** SECOND CHECK FFS: FFS=[%ld] INTL = %ld DIRCACHE = %ld Parms.ffs=$%lx\n", __FILE__, __FUNC__, __LINE__, FFS, intl, DirCache, Parms.ffs));

			if ( (0UL != TypeNewDostype) && (Parms.ffs != TypeNewDostype) )
				{
				FFS = FALSE;
				Parms.ffs = TypeNewDostype;
				FastFSType = FALSE;
				SetFFS = 0;			// [ FFS = FALSE ] and [ Parms.ffs = TypeNewDostype ] SO Disable FFS gadget.
				}

//----------------------------------------------------------------------------

			sprintf(DosTypeString, "%c%c%c%c",
				MakePrintable((Parms.ffs >> 24) & 0xff),
				MakePrintable((Parms.ffs >> 16) & 0xff),
				MakePrintable((Parms.ffs >>  8) & 0xff),
				MakePrintable(Parms.ffs & 0xff));

				ShortDostype = DosTypeString;

			d1(KPrintF("%s/%s/%ld: ** THIRD CHECK FFS: FFS=[%ld] INTL = %ld DIRCACHE = %ld Parms.ffs=$%lx ShortDostype=[%s]\n", __FILE__, __FUNC__, __LINE__, FFS, intl, DirCache, Parms.ffs, ShortDostype));

//-----------------------------------------------------------------------------


			// Open the options window: from GUI.c
			if ((stat=OpenPrepWindow(volumeName, SetFFS, SetIntl, SetDirCache))==0)
				{
				if (SetFFS)
					FFS = TRUE;	// NOTE: Initialize FFS value for message class according to SetFFS value, before getPrepInput() call.

				// Get the users input: from GetInputFromWindow.c
				stat=getPrepInput();

				Parms.icon = Icon;

				d1(KPrintF("%s/%s/%ld: [] PASS1: RETURNED FROM GUI: FFS = %ld INTL = %ld DIRCACHE = %ld Parms.ffs=««%lx»»\n", __FILE__, __FUNC__, __LINE__, FFS, intl, DirCache, Parms.ffs));

				d1(KPrintF("%s/%s/%ld: Parms.ffs= ««%lx»» FastFSType=%ld\n", __FILE__, __FUNC__, __LINE__, Parms.ffs, FastFSType));

				if (FastFSType)
					{
					if (FFS == TRUE)
						{
						if (DirCache == TRUE)
							Parms.ffs = ID_FASTDIR_FFS_DISK;
						else if (intl == TRUE)
							Parms.ffs = ID_INTER_FFS_DISK;
						else
							Parms.ffs = ID_FFS_DISK;
						}
					else
      						{
						if (DirCache == TRUE)
							Parms.ffs = ID_FASTDIR_DOS_DISK;
						else if (intl == TRUE)
							Parms.ffs = ID_INTER_DOS_DISK;
						else
							Parms.ffs = ID_DOS_DISK;
						}
					}
				
				d1(KPrintF("%s/%s/%ld: [] PASS2: RETURNED FROM GUI: FFS = %ld INTL = %ld DIRCACHE = %ld Parms.ffs=««%lx»»\n", __FILE__, __FUNC__, __LINE__, FFS, intl, DirCache, Parms.ffs));


				sprintf(DosTypeString, "%c%c%c%c",
					MakePrintable((Parms.ffs >> 24) & 0xff),
					MakePrintable((Parms.ffs >> 16) & 0xff),
					MakePrintable((Parms.ffs >>  8) & 0xff),
					MakePrintable(Parms.ffs & 0xff));

				ShortDostype = DosTypeString;

				d1(KPrintF("%s/%s/%ld: [] PASS2: ShortDostype = %s Parms.ffs=««%lx»»\n", __FILE__, __FUNC__, __LINE__, ShortDostype, Parms.ffs));


				// And close the window: from GUI.c
				ClosePrepWindow();

				if (stat == eQuick)
					{
					Parms.quick = TRUE;
					Parms.verify = FALSE;
					}

				// If the user selected 'OK'
				// +jmc+: "eQuick" added to "enum prepResult" in Format.h,
				// this is the QuickFormat option replacement pessing a button (GD_QuickFmtGadget) from GUI.c.
				// "eQuick" class mesage type is checked from GetInputFromWindow.c with all other gadgets.
				if ((stat == eOK) || (stat == eQuick))
					{
					// Get the new name of the disk
					strcpy(newName,((struct StringInfo *)
						(PrepGadgets[GD_NameGadget]->SpecialInfo))->Buffer);

					// Get the name of trashcan.
					if (Parms.icon)
						{
						strcpy(TrashName, ((struct StringInfo *)
							(PrepGadgets[GD_TrashCanNameGadget]->SpecialInfo))->Buffer);
						}

					d1(KPrintF("%s/%s/%ld: get from string gadget: newName=<%s>\n", __FILE__, __FUNC__, __LINE__, newName));

					strcpy(NewVolumeName,newName);
					strcpy(deviceName,volumeName);

//-----------------------------------------------------------------------------------------------

					if (strlen(BufferRequest) == 0)
						{
						TestCapacity = (TextHighCyl + 1) * TextLayoutBPT;

						if (TestCapacity > 2 * 1000000)
							{
							sprintf(BufferRequest, GetLocString(MSGID_DEVICE_SHORTSIZE), ((TestCapacity/1000)/1000));
							strcat(BufferRequest, " Gb");
							}
						else
							{
							sprintf(BufferRequest, GetLocString(MSGID_DEVICE_SHORTSIZE), TestCapacity);
							strcat(BufferRequest, " Kb");
							}
						}
 
//-----------------------------------------------------------------------------------------------

					// Ask the user for verification
					if (askAreYouSure(volumeName, (WBenchMsg->sm_ArgList[disk].wa_Lock!=(BPTR)NULL), BufferRequest))
						{
						struct IOExtTD *io1;
						DriveLayout layout;
						struct MsgPort *devPort;
						BOOL writeProtCont=TRUE;


						// If the volume lock is NULL, assume that the volumeName string holds
						// a valid device pointer
						if (WBenchMsg->sm_ArgList[disk].wa_Lock==(BPTR)NULL)
							{
							strcpy(deviceName,volumeName);
							d1(KPrintF("%s/%s/%ld: deviceName=<%s>\n", __FILE__, __FUNC__, __LINE__, deviceName));
							}

						// Get the drive name (if possible)
						d1(KPrintF("%s/%s/%ld: * GET *** volumeToDevName(%lx,%s,%lx)\n", __FILE__, __FUNC__, __LINE__, WBenchMsg->sm_ArgList[disk].wa_Lock,volumeName,&layout));

						if (!volumeToDevName(WBenchMsg->sm_ArgList[disk].wa_Lock,deviceName,&layout))
							{
      							printError((STRPTR) GetLocString(MSGID_DRIVE_NOFOUND), (STRPTR) GetLocString(MSGID_VOLUME_SELECTED), deviceName);
							d1(KPrintF("%s/%s/%ld: Can't find the drive for %s\n", __FILE__, __FUNC__, __LINE__, deviceName));
							return RETURN_WARN;
							}

						d1(KPrintF("%s/%s/%ld: DriveName=<%s>\n", __FILE__, __FUNC__, __LINE__, deviceName));

						// This port will be used to communicate with the filesystem
						devPort=DeviceProc(deviceName);
						if (devPort==NULL)
							{
							printError((STRPTR) GetLocString(MSGID_DRIVE_NOFOUND), (STRPTR) GetLocString(MSGID_VOLUME_SELECTED), deviceName);
							d1(KPrintF("%s/%s/%ld: Can't find the drive for %s\n", __FILE__, __FUNC__, __LINE__, deviceName));
							return RETURN_WARN;
							}

						// Open the disk device
						d1(KPrintF("%s/%s/%ld: * CALL *** OpenDrive(%s,%ld,%lx)\n", __FILE__, __FUNC__, __LINE__, layout.devName,layout.unit,layout.flags));

						if ((io1=OpenDrive(layout.devName,layout.unit,layout.flags))!=NULL)
							{
							//  Determine the write protect status
							io1->iotd_Req.io_Data=NULL;
							io1->iotd_Req.io_Length=0;
							io1->iotd_Req.io_Command=TD_PROTSTATUS;
							DoIO((struct IORequest *)io1);

							d1(KPrintF("%s/%s/%ld: layout.devName=<%s> layout.unit=[%ld]\n", __FILE__, __FUNC__, __LINE__, layout.devName, layout.unit));

							// Loop while the disk stays protected and user keeps pressing Retry
							while (io1->iotd_Req.io_Actual!=0 && (writeProtCont=alertIsWriteProtected(volumeName)))
							DoIO((struct IORequest *)io1);

							// If the disk is not write-protected
							if (writeProtCont)
								{
								// Do a full format if the user did not select the Quick option,
								// checkfor64bitcommandset(): from NSD_64bit.c
								if (checkfor64bitcommandset((struct IOStdReq *)io1) == TRUE)
									{
									// BOOL NsdSupport used for "StatusIText" message (yes or not drive support NSD 64bit support),
									// result inserted to "StatusTextNSDSupport" string buffer.
									// See StatusRender() from GUI.c
									NsdSupport=TRUE;
									if (WBenchMsg == NULL)
										printf("***Device %s supports 64bit commands! No size limit!!\n", deviceName);
									}
								else
									{
									if (WBenchMsg == NULL)
										printf("***Device %s does NOT support 64bit commands. 4GB limit activated!!\n", deviceName);
									}
								}
							CloseDrive(io1);
							}
						// Disk device closed

						d1(KPrintF("%s/%s/%ld: FROM WB FIRST CHECK: TypeNewDostype ?? = $%lx \n", __FILE__, __FUNC__, __LINE__, ffs));

						// If everythings OK, open the status window
						if ((stat=OpenStatusWindow(statusString))==0)
							{

							d1(KPrintF("%s/%s/%ld: FROM WB PASS2: FFS=%ld INTL=%ld Dostype to use: ffs=$%lx QuickFormat=%ld Parms.verify=%ld CreateIcon=%ld\n", \
								__FILE__, __FUNC__, __LINE__, FFS, intl, ffs, Parms.quick, Parms.verify, Parms.icon));

							// And format the disk
							formatVolume(&(WBenchMsg->sm_ArgList[disk].wa_Lock), volumeName, &Parms, statusString);

							// Were done, so close the status window
							CloseStatusWindow();
							}
						}
					}
				}
			}
			// Free the visual info, etc.
			CloseDownScreen();
		}

	else     // Weve been run from the CLI -- UNFINISHED !!!

		{
      		// Open a 'stderr' I/O channel to the shell
      		// (the normal stderr was not opened since we used _main() )
		StdErr=Open("CONSOLE:",MODE_OLDFILE);

		// Make sure requestors dont open
		process=(struct Process *)FindTask(0L);
		oldWdw=process->pr_WindowPtr;
		process->pr_WindowPtr=(APTR)(-1);

		// Get the command-line arguments
		parseArgs(driveName,volumeName, &FFS, &intl, &Parms.icon, TrashName, &Parms.quick, &Parms.verify);

		for (i=0; i<strlen(volumeName); i++)
			{
			if (volumeName[i] == ':' || volumeName[i] == '/')
				{
				Write(Output(), "Volumename contains illegal characters!\n", 40);
				cleanup(0);
				}
			}

		// Get a lock on the selected drive
		// (note:  NULL is a valid return value;  it means that the disk we
		// want to format is itself unformatted)
		driveLock=Lock(driveName,ACCESS_READ);
		strcpy(temp,driveName);

		// Get the volume/drive name
		if (volumeToDevName(driveLock,temp,&junk))
			{
			// Wait for the user to tell us to go ahead
/*			Write(Output(),"Insert the disk to be formatted in drive ",41);
			Write(Output(),temp,strlen(temp));
			Write(Output()," and press RETURN ",18);
			Read(Input(),temp2,1);
*/

			// GetDevInfo(driveLock, temp, strlen(temp));

			GetDeviceName(driveLock, temp, strlen(temp));

			d1(KPrintF("%s/%s/%ld: TypeNewDostype=$%lx ID_FFS_DISK=$%lx ID_INTER_FFS_DISK=$%lx ID_DOS_DISK=$%lx\n", \
				__FILE__, __FUNC__, __LINE__, TypeNewDostype, ID_FFS_DISK, ID_INTER_FFS_DISK, ID_DOS_DISK));

//-----------------------------------------------------------------------------

			if (TypeNewDostype == ID_FFS_DISK)
				FFS = TRUE;

			if ((TypeNewDostype == ID_INTER_FFS_DISK) || (TypeNewDostype == ID_INTER_DOS_DISK))
				intl = TRUE;

			if (FFS == TRUE)
				{
				if (intl == TRUE)
					Parms.ffs = ID_INTER_FFS_DISK;
				else
					Parms.ffs = ID_FFS_DISK;
				}
			else
				{
				if (intl == TRUE)
					Parms.ffs = ID_INTER_DOS_DISK;
				else
					Parms.ffs = ID_DOS_DISK;
				}

			if ((TypeNewDostype != ID_FFS_DISK) || (TypeNewDostype != ID_INTER_FFS_DISK) || (TypeNewDostype != ID_INTER_DOS_DISK))
				{
				// FFS = FALSE;
				intl = FALSE;
				Parms.ffs = TypeNewDostype;
				}

//-----------------------------------------------------------------------------


			d1(KPrintF("%s/%s/%ld: FROM CLI: FFS=%ld INTL=%ld Parms.ffs=$%lx QuickFormat=%ld Parms.verify=%ld CreateIcon=%ld\n", \
					__FILE__, __FUNC__, __LINE__, FFS, intl, Parms.ffs, Parms.quick, Parms.verify, Parms.icon));

			// Format the disk
			formatVolume(&driveLock, temp, &Parms, statusString);
			}
		else
			{
			Write(Output(),"Cannot find the specified drive!\n",33);
			}

		Close(StdErr);

		// Restore the old contents of this pointer
		process->pr_WindowPtr = oldWdw;

		Write(Output(),"\n",1);
		}

	cleanup(0);

	return(0);
}

//----------------------------------------------------------------------------

CONST_STRPTR GetLocString(ULONG MsgId)
{
	struct FormatDisk_LocaleInfo li;

	li.li_Catalog = FormatDiskCatalog;
#ifndef __amigaos4__
	li.li_LocaleBase = LocaleBase;
#else
	li.li_ILocale = ILocale;
#endif

	return GetFormatDiskString(&li, MsgId);
}

//-----------------------------------------------------------------------------
// Ask the user if she really wants to format the disk
BOOL askAreYouSure(char *volumeName,BOOL truncColon, char *DeviceSize)
{
	char name[36];
	APTR args[3];
	UBYTE result;
	struct EasyStruct ez;

	// Get the device name
	strcpy(name,volumeName);

	// Truncate the trailing colon if so specified
	if (truncColon)
		name[strlen(name)-1]='\0';

	// Setup the device name as the argument to the requestor
	args[0]=name;
	args[1]=DeviceSize;
	args[2]=NULL;


	ez.es_StructSize = sizeof(ez);
	ez.es_Flags = 0;
	ez.es_Title = (STRPTR) GetLocString(MSGID_REQ_SURE_TITLE);
	ez.es_TextFormat = (STRPTR) GetLocString(MSGID_REQ_SURE_BODY);
	ez.es_GadgetFormat = (STRPTR) GetLocString(MSGID_REQ_SURE_GADGETS);

	// (+jmc+) Open the request
	result=EasyRequestArgs(NULL, &ez, NULL, args);

	// return(result==1);	// 
	return(result);		// +jmc+
}

//-----------------------------------------------------------------------------
// Print one, two, or three lines of error messages in an EasyRequestor
// or to 'StdErr'
void printError(CONST_STRPTR first, CONST_STRPTR second, CONST_STRPTR third)
{
	static const char *oneLine="%s";
	static const char *twoLine="%s\n%s";
	static const char *threeLine="%s\n%s\n%s";
	struct EasyStruct errorReq;
	APTR args[4];
	args[0] = args[3] = NULL;

	errorReq.es_StructSize = sizeof(errorReq);
	errorReq.es_Flags = 0;
	errorReq.es_Title = (STRPTR) GetLocString(MSGID_REQ_SURE_TITLE);
	errorReq.es_TextFormat = NULL;
	errorReq.es_GadgetFormat = (STRPTR) GetLocString(MSGID_GADGET_OK);

	// If we're running from the CLI
	if (fromcli)
		{
		// And a StdErr handle was opened successfully
		if (StdErr!=(BPTR)NULL)
			{
			char LF=0x0A;

			// Print the first line
			if (first != NULL)
				{
				Write(StdErr, (STRPTR) first, strlen(first));
				Write(StdErr," ",1);
				}

			// Print the second line
			if (second != NULL)
				{
				Write(StdErr, (STRPTR) second, strlen(second));
				Write(StdErr," ",1);
				}

			// Print the third line
			if (third != NULL)
				{
				Write(StdErr, (STRPTR) third, strlen(third));
				Write(StdErr, " ", 1);
				}

			 // Print the terminating carriage return
			Write(StdErr,&LF,1);
			}
		}
	else  		// Otherwise, we're running from Workbench, so put up the requestor
		{
		// Three lines
		if (third != NULL)
			{
			args[2] = (APTR) third;
			args[1] = (APTR) second;
			args[0] = (APTR) first;
			errorReq.es_TextFormat = (STRPTR) threeLine;
			}
		else if (second != NULL)   // Two lines
			{
			args[1] = (APTR) second;
			args[0] = (APTR) first;
			errorReq.es_TextFormat = (STRPTR) twoLine;
			}
		else if (first != NULL)    // One line
			{
			args[0] = (APTR) first;
			errorReq.es_TextFormat = (STRPTR) oneLine;
			}
		// Put up the requestor
		EasyRequestArgs(NULL,&errorReq,NULL,args);
		}
	return;
}

//-----------------------------------------------------------------------------
// Get the name of a volume, given a lock to that volume
void getVolumeName(char *name,struct WBArg *argList,UWORD disk)
{
	// Get the name
	if (NameFromLock(argList[disk].wa_Lock,name,256) == DOSFALSE)
		// Or return <unknown> if the name couldn't be determined
		strcpy(name,"<unknown>");
	return;
}

//-----------------------------------------------------------------------------
// Alert the user to the fact that the disk is write protected, and give
// the user a chance to unprotect the disk
BOOL alertIsWriteProtected(CONST_STRPTR devName)
{
	struct EasyStruct writeProtected;
	APTR args[2];
	BYTE result;

	// Setup the device name as the argument to the requestor
	args[0] = (APTR) devName;
	args[1] = NULL;

	writeProtected.es_StructSize = sizeof(writeProtected);
	writeProtected.es_Flags = 0;
	writeProtected.es_Title = (STRPTR) GetLocString(MSGID_REQ_SURE_TITLE);
	writeProtected.es_TextFormat = (STRPTR) GetLocString(MSGID_VOLUME_PROTECTED);
	writeProtected.es_GadgetFormat = (STRPTR) GetLocString(MSGID_REQ_RETRY_CANCEL);

	// (+jmc+) Open the request
	result=EasyRequestArgs(NULL, &writeProtected, NULL, args);

	// return(result==1);	// 
	return(result);		// +jmc+
}

//-----------------------------------------------------------------------------
// Exit from the program, closing any open libraries
void cleanup(ULONG err)
{
#ifdef __amigaos4__
	if (IDiskfont)
		DropInterface((struct Interface *)IDiskfont);
#endif
	if (DiskfontBase)
		CloseLibrary((struct Library *) DiskfontBase);

#ifndef __amigaos4__
	if (UtilityBase)
		CloseLibrary((struct Library *) UtilityBase);
#endif

#ifdef __amigaos4__
	if (IIntuition)
		DropInterface((struct Interface *)IIntuition);
#endif
	if (IntuitionBase!=NULL)
		CloseLibrary((struct Library *)IntuitionBase);

#ifdef __amigaos4__
	if (IGraphics)
		DropInterface((struct Interface *)IGraphics);
#endif
	if (GfxBase!=NULL)
		CloseLibrary((struct Library *)GfxBase);

#ifdef __amigaos4__
	if (IGadTools)
		DropInterface((struct Interface *)IGadTools);
#endif
	if (GadToolsBase!=NULL)
		CloseLibrary(GadToolsBase);

#ifdef __amigaos4__
	if (IIcon)
		DropInterface((struct Interface *)IIcon);
#endif
	if (IconBase!=NULL)
		CloseLibrary(IconBase);

	if (LocaleBase!=NULL)
		{
		if (FormatDiskLocale)
			{
			CloseLocale(FormatDiskLocale);
			FormatDiskLocale = NULL;
			}
		if (FormatDiskCatalog)
			{
			CloseCatalog(FormatDiskCatalog);
			FormatDiskCatalog = NULL;
			}
#ifdef __amigaos4__
		if (ILocale)
			DropInterface((struct Interface *)ILocale);
#endif
		CloseLibrary((struct Library *) LocaleBase);
		LocaleBase = NULL;
		}


	exit(err);
}

//-----------------------------------------------------------------------------
// Update the status window (from Workbench), or CLI output (from the CLI)
// If running from the CLI, this also checks to see if the user has pressed
// control-C
BOOL updateStatWindow(char *string,UWORD percent)
{
	UWORD width;
	ULONG class1;
	UWORD code;
	struct TagItem tags[3];
	static char msg[80];
	char CR = 0x0d;
	UWORD AbortButton_key;

	struct IntuiMessage *mesg;

	AbortButton_key = FindUnderscoredToLower((STRPTR) GetLocString(MSGID_GUI_STATUSWINDOW_ABORT));

	// If this is NULL, were running from the CLI
	if (fromcli)
		{
		// Write the string to the CLI, followed by a carriage return (but not a line feed)
		Write(Output(),string,strlen(string));
		Write(Output(),&CR,1);

		// Check to see if the user pressed Control-C
		if ( (SetSignal(0,0) & SIGBREAKF_CTRL_C) == SIGBREAKF_CTRL_C)
			{
			// If he did, print "***Break" and return TRUE, to signal that
			// the user aborted the formatting process
			Write(Output(),"\n***Break\n",10);
			return(TRUE);
			}
		// Otherwise, continue
		return(FALSE);
		}

	// This code is used to update the status window that is displayed when
	// the user runs NewFormat from the Workbench.
	// This puts the message into the text-display gadget.
	// This copy is so that the caller can change the contents of 'string'
	// upon return (which can't be done without the copy, since GadTools
	// wont copy the contents of string  to an internal buffer).

	strcpy(msg,string);
	tags[0].ti_Tag=GTTX_Text;
	tags[0].ti_Data=(ULONG)msg;

	tags[1].ti_Tag=TAG_DONE;
	tags[1].ti_Data=0UL;

	GT_SetGadgetAttrsA(StatusGadgets[GD_StatusGadget], StatusWnd,NULL, tags);

	// Fill the status box with the current percentage of completion
	SetAPen(StatusWnd->RPort,3);
	width=box.left+((box.width-3)*percent)/1000;
	RectFill(StatusWnd->RPort,box.left+2,box.top+1,width,box.top+box.height-2);

	// Check user input
	mesg=GT_GetIMsg(StatusWnd->UserPort);

	// Loop while there are messages
	while (mesg!=NULL)
		{
		class1 = mesg->Class;
		code = mesg->Code;
		GT_ReplyIMsg(mesg);

		switch(class1)
			{
			// Return TRUE (user abort) if the user pressed Abort button gadget.
			case IDCMP_VANILLAKEY:
				if (code == AbortButton_key) return(TRUE);
				break;

			// Or if the user pressed the 'Stop' gadget
			case IDCMP_GADGETUP:
				return(TRUE);
			}
		// Get the next message
		mesg=GT_GetIMsg(StatusWnd->UserPort);
		}
	return(FALSE);
	}

//-----------------------------------------------------------------------------
//------------------- From information.module ---------------------------------
//-----------------------------------------------------------------------------

static void BtoCstring(BSTR bstr, STRPTR Buffer, size_t BuffLen)
{
	size_t Len;

#ifdef __AROS__
	// AROS needs special handling because it uses NULL-terminated
	// strings on some platforms.
	Len = AROS_BSTR_strlen(bstr);
	if (Len >= BuffLen)
		Len = BuffLen  - 2;
	strncpy(Buffer, AROS_BSTR_ADDR(bstr), Len);
#else
	const char *bString = BADDR(bstr);

	*Buffer = '\0';

	Len = *bString;
	if (Len >= BuffLen)
		Len = BuffLen  - 2;

	strncpy(Buffer, bString + 1, Len);
#endif
	Buffer[Len] = '\0';
}

//-----------------------------------------------------------------------------

static UBYTE MakePrintable(UBYTE ch)
{
	if (ch < ' ')
		ch += '0';

	return ch;
}
//-----------------------------------------------------------------------------

static void ByteCount(STRPTR Buffer, size_t BuffLen, LONG NumBlocks, LONG BytesPerBlock)
{
	CONST_STRPTR Decimal;

	if (FormatDiskLocale)
		Decimal = FormatDiskLocale->loc_DecimalPoint;
	else
		Decimal = ".";

	// Normalize block size to KBytes
	while (BytesPerBlock > 1024)
		{
		BytesPerBlock >>= 1;
		NumBlocks <<= 1;
		}
	while (BytesPerBlock < 1024)
		{
		BytesPerBlock <<= 1;
		NumBlocks >>= 1;
		}

	d1(KPrintF(__FILE__ "/%s/%ld: NumBlocks=%lu\n", __FUNC__, __FILE__, __FUNC__, __LINE__, NumBlocks));
	
        if (NumBlocks > 2 * 1024 * 1024)
		{
		// > 2 GB
		ULONG GBytes = NumBlocks / (1024 * 1024);
		ULONG MBytes = NumBlocks / 1024 - GBytes * 1024;

		d1(KPrintF(__FILE__ "/%s/%ld: GBytes=%lu  MBytes=%lu\n", __FUNC__, __FILE__, __FUNC__, __LINE__, GBytes, MBytes));

		if (GBytes > 200)
			ScaFormatString("%lDG", Buffer, BuffLen, 1, GBytes);
		else if (GBytes > 20)
			ScaFormatString("%lD%s%1ldG", Buffer, BuffLen, 3, GBytes, Decimal, MBytes / 100);
		else
			ScaFormatString("%lD%s%-02ldG", Buffer, BuffLen, 3, GBytes, Decimal, MBytes / 10);
		}
	else if (NumBlocks > 2 * 1024)
		{
		// > 2 MB
		ULONG MBytes = NumBlocks / 1024;
		ULONG KBytes = NumBlocks % 1024;

		if (MBytes > 200)
			ScaFormatString("%lDM", Buffer, BuffLen, 1, MBytes);
		else if (MBytes > 20)
			ScaFormatString("%lD%s%1ldM", Buffer, BuffLen, 3, MBytes, Decimal, KBytes / 100);
		else
			ScaFormatString("%lD%s%-02ldK", Buffer, BuffLen, 3, MBytes, Decimal, KBytes / 10);
		}
	else
		{
		ScaFormatString("%lDK", Buffer, BuffLen, 1, NumBlocks);
		}

}

//-----------------------------------------------------------------------------

static STRPTR ScaFormatString(CONST_STRPTR formatString, STRPTR Buffer, size_t MaxLen, LONG NumArgs, ...)
{
	va_list args;

	va_start(args, NumArgs);

	if (FormatDiskLocale)
		{
		ULONG *ArgArray;

		ArgArray = malloc(sizeof(ULONG) * NumArgs);
		if (ArgArray)
			{
			struct FormatDateHookData fd;
			struct Hook fmtHook;
			ULONG n;
			STATIC_PATCHFUNC(FormatDateHookFunc)

			for (n = 0; n < NumArgs; n++)
				ArgArray[n] = va_arg(args, LONG);

			fmtHook.h_Entry = (HOOKFUNC) PATCH_NEWFUNC(FormatDateHookFunc);
			fmtHook.h_Data = &fd;

			fd.fdhd_Buffer = Buffer;
			fd.fdhd_Length = MaxLen;

			FormatString(FormatDiskLocale, (STRPTR) formatString, ArgArray, &fmtHook);

			free(ArgArray);
			}
		}
	else
		{
		vsprintf(Buffer, formatString, args);
		}

	va_end(args);

	return Buffer;
}

//-----------------------------------------------------------------------------

static M68KFUNC_P3(void, FormatDateHookFunc,
	A0, struct Hook *, theHook,
	A2, struct Locale *, locale,
	A1, char, ch)
{
	struct FormatDateHookData *fd = (struct FormatDateHookData *) theHook->h_Data;

	(void) locale;

	if (fd->fdhd_Length >= 1)
		{
		*(fd->fdhd_Buffer)++ = ch;
		fd->fdhd_Length--;
		}
}
M68KFUNC_END

//-----------------------------------------------------------------------------

static void GetDeviceName(BPTR dLock, STRPTR DeviceName, size_t MaxLen)
{
	struct InfoData *info = malloc(sizeof(struct InfoData));

	strcpy(DeviceName, "");
	if (info)
		{
		struct DosList *VolumeNode;
		ULONG UsedPercent;
		char ByteCountText[16];

		Info(dLock, info);


		d1(kprintf(__FILE__ "/%s/%ld: DiskType=%ld  DiskState=%ld\n", __FUNC__, __FILE__, __FUNC__, __LINE__, info->id_DiskType, info->id_DiskState));



		VolumeNode = (struct DosList *) BADDR(info->id_VolumeNode);

		d1(KPrintF("%s/%s/%ld: VolumeNode=%08lx  Type=%08lx\n", __FILE__, __FUNC__, __LINE__, VolumeNode, VolumeNode->dol_Type));

		if (VolumeNode)
			{
			//char DosTypeString[16];
			const struct FileSysEntry *fse;
			char FseHandler[] = "";
			LONG FseVersion;
			LONG FseRevision;

			d1(KPrintF("%s/%s/%ld: *** dol_DiskType = %lx OR VolumeNode->dol_misc.dol_volume.dol_DiskType=%lx ?? ***\n", \
				__FILE__, __FUNC__, __LINE__, VolumeNode->dol_Type, VolumeNode->dol_misc.dol_volume.dol_DiskType));

			d1(KPrintF("%s/%s/%ld: *** GO SEARCH:  FindFileSysResEntry(%lx) ***\n", \
				__FILE__, __FUNC__, __LINE__, VolumeNode->dol_misc.dol_volume.dol_DiskType));

			fse = FindFileSysResEntry(VolumeNode->dol_misc.dol_volume.dol_DiskType);
			if (fse)
				{
				LONG *seg;

				d1(KPrintF(__FILE__ "/%s/%ld: *** fse OK! =%08lx ***\n", __FUNC__, __FILE__, __FUNC__, __LINE__, fse));

				if (fse->fse_Node.ln_Name)
					stccpy(FseHandler, fse->fse_Node.ln_Name, sizeof(FseHandler));

				if (fse->fse_DosType)
					TypeNewDostype = fse->fse_DosType;

				d1(KPrintF("%s/%s/%ld: fse->fse_DosType=TypeNewDostype=<%lx> fse->fse_Node.ln_Name=<%s>\n", \
						__FILE__, __FUNC__, __LINE__, TypeNewDostype, fse->fse_Node.ln_Name));

				FseVersion = fse->fse_Version >> 16;
				FseRevision = fse->fse_Version & 0xffff;

				sprintf(TextDeviceHandler, "%s %ld.%ld",
					FseHandler,
					(long)FseVersion, (long)FseRevision);

				d1(KPrintF("%s/%s/%ld: FseHandler=%s FseVersion/FseRevision=%ld.%ld\n", __FILE__, __FUNC__, __LINE__, FseHandler, FseVersion, FseRevision));

				seg = (LONG *) BADDR(fse->fse_SegList);

				d1(KPrintF("%s/%s/%ld: seg=%08lx\n", __FILE__, __FUNC__, __LINE__, seg));

				while (seg)
					{
					CONST_STRPTR VersionString;
					size_t SegLength = sizeof(LONG) * seg[-1];

					d1(KPrintF("%s/%s/%ld: seg=%08lx  len=%lu\n", __FILE__, __FUNC__, __LINE__, seg, SegLength));

					VersionString = FindVersionString((UBYTE *) seg, SegLength);
					if (VersionString)
						{
						strcpy(TextDeviceHandler, VersionString);

						d1(KPrintF("%s/%s/%ld: VersionString=%s\n", __FILE__, __FUNC__, __LINE__, VersionString));
						break;
						}

					seg = (LONG *) BADDR(*seg);
					}
				}

			d1(KPrintF("%s/%s/%ld: dol_DiskType ?? = %lx\n", __FILE__, __FUNC__, __LINE__, VolumeNode->dol_misc.dol_volume.dol_DiskType));

			sprintf(DosTypeString, "%c%c%c%c",
				MakePrintable((VolumeNode->dol_misc.dol_volume.dol_DiskType >> 24) & 0xff),
				MakePrintable((VolumeNode->dol_misc.dol_volume.dol_DiskType >> 16) & 0xff),
				MakePrintable((VolumeNode->dol_misc.dol_volume.dol_DiskType >>  8) & 0xff),
				MakePrintable(VolumeNode->dol_misc.dol_volume.dol_DiskType & 0xff));

			if (VolumeNode->dol_Task && VolumeNode->dol_Task->mp_SigTask)
				{
				struct DosList *dl;

				stccpy(DeviceName, ((struct Task *) VolumeNode->dol_Task->mp_SigTask)->tc_Node.ln_Name, MaxLen);

				sprintf(TextDeviceOnDev, "on %s", DeviceName);


				d1(KPrintF("%s/%s/%ld: ** TextDeviceOnDev=<%s> **\n", __FILE__, __FUNC__, __LINE__, TextDeviceOnDev));

				dl = LockDosList(LDF_DEVICES | LDF_READ);

				dl = FindDosEntry(dl, DeviceName, LDF_DEVICES);
				d1(KPrintF(__FILE__ "/%s/%ld: dl=%08lx\n", __FUNC__, __FILE__, __FUNC__, __LINE__, dl));
				d1(KPrintF("%s/%s/%ld: FindDosEntry(%08lx, %s, LDF_DEVICES)\n", __FILE__, __FUNC__, __LINE__, dl, DeviceName));

				if (dl)
					{
					struct FileSysStartupMsg *fsm;

					d1(KPrintF("%s/%s/%ld:  *** dl OK! =%08lx ***\n", __FILE__, __FUNC__, __LINE__, dl));

					d1(KPrintF(__FILE__ "/%s/%ld: dl=%08lx  Type=%08lx\n", __FUNC__, __FILE__, __FUNC__, __LINE__, dl, dl->dol_Type));
					d1(KPrintF(__FILE__ "/%s/%ld: dl=%08lx  dol_handler=%08lx\n", __FUNC__, __FILE__, __FUNC__, __LINE__, dl, dl->dol_misc.dol_handler.dol_Handler));

					if (dl->dol_misc.dol_handler.dol_Handler)
						{
						char Handler[128];

						BtoCstring(dl->dol_misc.dol_handler.dol_Handler, Handler, sizeof(Handler));

						d1(KPrintF(__FILE__ "/%s/%ld: ** Handler=<%s> **\n", __FUNC__, __FILE__, __FUNC__, __LINE__, Handler));

						sprintf(TextDeviceHandler, "%s", Handler);
						}

					fsm = BADDR(dl->dol_misc.dol_handler.dol_Startup);

					d1(KPrintF("%s/%s/%ld: *** fsm=%08lx ***\n", __FILE__, __FUNC__, __LINE__, fsm));

					if (fsm)
						{
						char DevName[128];
						struct DosEnvec *env;

						d1(KPrintF(__FILE__ "/%s/%ld: *** fms OK! =%08lx ***\n", __FUNC__, __FILE__, __FUNC__, __LINE__, fsm));

						BtoCstring(fsm->fssm_Device, DevName, sizeof(DevName));
						sprintf(TextDeviceName, "%s, %ld", DevName, fsm->fssm_Unit);

						d1(KPrintF("%s/%s/%ld: DevName=<%s> fsm->fssm_Unit=%ld\n", __FILE__, __FUNC__, __LINE__, DevName, fsm->fssm_Unit));

						env = BADDR(fsm->fssm_Environ);

						if (env && env->de_TableSize >= DE_DOSTYPE)
							{
							d1(KPrintF("%s/%s/%ld: env->de_DosType=$%lx [%c%c%c%c]\n", \
								__FILE__, __FUNC__, __LINE__, env->de_DosType, ((env->de_DosType >> 24) & 0xff),
									((env->de_DosType >> 16) & 0xff),
									((env->de_DosType >> 8) & 0xff),
									(env->de_DosType & 0xff)));

							TypeNewDostype = env->de_DosType;

							sprintf(DosTypeString, "%c%c%c%c",
								MakePrintable((env->de_DosType >> 24) & 0xff),
								MakePrintable((env->de_DosType >> 16) & 0xff),
								MakePrintable((env->de_DosType >>  8) & 0xff),
								MakePrintable(env->de_DosType & 0xff));

							d1(KPrintF("%s/%s/%ld: DosTypeStrin[env->de_DosType Made printable]=%s\n", __FILE__, __FUNC__, __LINE__, DosTypeString));
							}

//-------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------
						d1(KPrintF("%s/%s/%ld: Info Type=%08lx id_NumBlocks=%ld id_BytesPerBlock=%ld id_BytesPerBlock=%ld id_NumBlocksUsed=%ld\n", \
							__FILE__, __FUNC__, __LINE__, info->id_DiskType, info->id_NumBlocks, info->id_BytesPerBlock, info->id_BytesPerBlock, info->id_NumBlocksUsed));


						UsedPercent = (100 * info->id_NumBlocksUsed) / info->id_NumBlocks;

						ByteCount(ByteCountText, sizeof(ByteCountText), info->id_NumBlocksUsed, info->id_BytesPerBlock);
							ScaFormatString(GetLocString(MSGID_DEVICE_BLOCKS_PERCENT_USED),
							TextDeviceUsed, sizeof(TextDeviceUsed), 
							3, ByteCountText, info->id_NumBlocksUsed, UsedPercent);

						d1(kprintf("%s/%s/%ld: ByteCountText=%s TextDeviceUsed=%s\n", __FILE__, __FUNC__, __LINE__, ByteCountText,TextDeviceUsed));


						ByteCount(ByteCountText, sizeof(ByteCountText),info->id_NumBlocks - info->id_NumBlocksUsed, info->id_BytesPerBlock);
						ScaFormatString(GetLocString(MSGID_DEVICE_BLOCKS_PERCENT_FREE),
							TextDeviceFree, sizeof(TextDeviceFree),
							3, ByteCountText, info->id_NumBlocks - info->id_NumBlocksUsed, 100 - UsedPercent);

						d1(kprintf("%s/%s/%ld: ByteCountText=%s TextDeviceFree=%s\n", __FILE__, __FUNC__, __LINE__, ByteCountText, TextDeviceFree));


						ByteCount(ByteCountText, sizeof(ByteCountText), info->id_NumBlocks, info->id_BytesPerBlock);
						ScaFormatString(GetLocString(MSGID_DEVICE_BLOCKS),
							TextDeviceSize, sizeof(TextDeviceSize), 2, ByteCountText, info->id_NumBlocks);

						d1(kprintf("%s/%s/%ld: ByteCountText=%s TextDeviceSize=%s\n", __FILE__, __FUNC__, __LINE__, ByteCountText, TextDeviceSize));


						ScaFormatString("Block size: %lD bytes", DeviceBlockSize, sizeof(DeviceBlockSize), 1, info->id_BytesPerBlock);
						d1(kprintf("%s/%s/%ld: DeviceBlockSize=%s\n", __FILE__, __FUNC__, __LINE__, DeviceBlockSize));


						sprintf(BufferRequest, "%s\n%s\n%s",TextDeviceUsed, TextDeviceFree, TextDeviceSize); 

						// ScaFormatString("Capacity: %lD", TextDeviceSize, sizeof(TextDeviceSize), 1, ((TextHighCyl + 1 ) * TextLayoutBPT);


						d1(KPrintF("%s/%s/%ld: %s\n", __FILE__, __FUNC__, __LINE__, ByteCountText));
//-------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------

						}
					}

				UnLockDosList(LDF_DEVICES | LDF_READ);
				}
			}

		d1(kprintf(__FILE__ "/%s/%ld: DeviceName=<%s>\n", __FUNC__, __FILE__, __FUNC__, __LINE__, DeviceName));

		free(info);
		}
}

//-----------------------------------------------------------------------------

static const struct FileSysEntry *FindFileSysResEntry(ULONG DosType)
{
	struct FileSysResource *FileSysRes;

	FileSysRes = OpenResource(FSRNAME);
	if (FileSysRes)
		{
		const struct FileSysEntry *fse;
		char Buffer_Dostype[8];
		char Buffer_FseNodeType[8];

		sprintf(Buffer_Dostype, "%c%c%c%c",
			MakePrintable((DosType >> 24) & 0xff),
			MakePrintable((DosType >> 16) & 0xff),
			MakePrintable((DosType >>  8) & 0xff),
			MakePrintable(DosType & 0xff));

		for (fse = (const struct FileSysEntry *) FileSysRes->fsr_FileSysEntries.lh_Head;
			fse != (const struct FileSysEntry *) &FileSysRes->fsr_FileSysEntries.lh_Tail;
			fse = (const struct FileSysEntry *) fse->fse_Node.ln_Succ)
			{

			sprintf(Buffer_FseNodeType, "%c%c%c%c",
				MakePrintable((fse->fse_DosType >> 24) & 0xff),
				MakePrintable((fse->fse_DosType >> 16) & 0xff),
				MakePrintable((fse->fse_DosType >>  8) & 0xff),
				MakePrintable(fse->fse_DosType & 0xff));

			d1(KPrintF("%s/%s/%ld: COMPARE: fse->fse_DosType =%lx [%s] WITH DosType =%lx [%s]\n", \
				__FILE__, __FUNC__, __LINE__, fse->fse_DosType, Buffer_FseNodeType, DosType, Buffer_Dostype));

			if (fse->fse_DosType == DosType)
				return fse;
			}
		}

	return NULL;
}
//----------------------------------------------------------------------------


static CONST_STRPTR FindVersionString(const UBYTE *block, size_t BlockLen)
{
	static char VersMask[10] = "$";

	// do not use statically initalized "$VER" variable here
	// since it might confuse the "version" command.
	strcat(VersMask, "VER: ");

	while (BlockLen--)
		{
		if (0 == memcmp(VersMask, block, strlen(VersMask)))
			return (CONST_STRPTR)(block + strlen(VersMask));

		block++;
		}

	return NULL;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


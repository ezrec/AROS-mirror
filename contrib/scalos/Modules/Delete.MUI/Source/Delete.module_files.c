// Delete.module_files.c
// $Date$
// $Revision$


//     ___       ___.
//   _/  /_______\  \_     ___ ___ __ _                       _ __ ___ ___.
//__//  / _______ \  \\___/                                               \___.
//_/ | '  \__ __/  ` | \_/        © Copyright 1999, Christopher Page       \__.
// \ | |    | |__  | | / \   Released as Free Software under the GNU GPL   /.
//  >| .    |  _/  . |<   >--- --- -- -                       - -- --- ---<.
// / \  \   | |   /  / \ /   This file is part of the ScalosDelete code    \.
// \  \  \_/   \_/  /  / \  and it is released under the GNU GPL. Please   /.
//  \  \           /  /   \   read the "COPYING" file which should have   /.
// //\  \_________/  /\\ //\    been included in the distribution arc.   /.
//- --\   _______   /-- - --\      for full details of the license      /-----
//-----\_/       \_/---------\   ___________________________________   /------
//                            \_/                                   \_/.
//
// Description:
//
//  Filesystem functions and listview hook functions
//
// Functions:
//
//  int  estrlen      (STRPTR)
//  APTR ConstructList(APTR, FileInfoBlock *)
//  void DestructList (APTR, FileInfoBlock *)
//  LONG DisplayList  (char **, FileInfoBlock *)
//  LONG CompareList  (FileInfoBlock *, FileInfoBlock *)
//  BOOL ListFiles    (WBStartup *)
//  LONG CopyFile     (STRPTR, STRPTR, ULONG)
//  void UpdateDrawer (STRPTR)
//  LONG FileDelete   (STRPTR, BOOL, APTR)
//  LONG DeleteIcon   (STRPTR, BOOL, APTR)
//  LONG DirDelete    (STRPTR, BOOL, APTR)
//  void KillFiles    (WBStartup *)
//  void DirCount     (STRPTR, DetailStats *)
//  BOOL GetDetails   (WBStartup *)
//
// Detail:
//
//  This file contains the routines that do the real work. The xxxxList()
//  functions are hook functions used in the main list in the interface,
//  the remainder of the functions either count the selected files and
//  directories or do the actual file/ directory deletion. Several of the
//  routines contain a large numbr of debugging commands, if you don't
//  define NDEBUG somewhere (either in your preprocessor options or in
//  the header) then you'll need to link debug.lib and have something
//  watching the serial port... if you have defined NDEBUG then the
//  preprocessor removes all the debugging commands...
//
// Modification History:
//
//  [02 June 1999, Chris <chris@worldfoundry.demon.co.uk>]
//
//  Converted header notice to GNU GPL version, checked and recommented where
//  required.
//
// Fold Markers:
//
//  Start: /*GFS*/.
//    End: /*GFE*/ .

#include"Delete.module.h"

struct DetailStats
{
    ULONG ds_Files;
    ULONG ds_Bytes;
    ULONG ds_Dirs ;
};

// These make life easier later, they are allocated/ freed in InitMUI()/ ExitMUI()....
STRPTR         FileBuffer = NULL;
STRPTR         PathBuffer = NULL;
STRPTR         DirBuffer  = NULL;
char TextBuffer[80 + 1];

struct FileInfoBlock *GlobalFIB  =  NULL; // This makes some of the routines easier to follow..
BOOL           Unprotect  = FALSE; // If TRUE all files are automagically unprotected

ULONG          SizeCount  = 0    ; // Number of bytes being deleted in total
BOOL           GotDetails = FALSE; // If TRUE then Sizecount is the size of all files and subdirectories
ULONG          DoneSize   =     0; // Number of bytes we've deleted already

//----------------------------------------------------------------------------

static struct ScaWindowStruct *FindScalosWindow(BPTR dirLock);

//----------------------------------------------------------------------------

/* int estrlen(STRPTR)                                                       */
/* -=-=-=-=-=-=-=-=-=-                                                       */
/* This routine does exactly the same thing as ANSI strlen() except that if  */
/* it recieves a NULL pointer, it returns 0 rather than causing enforcer hits*/

size_t estrlen(CONST_STRPTR strdat)
{
///
	if(strdat)
		return strlen(strdat);

	return(0);
///
}


/* APTR ConstructList(APTR, FileInfoBlock *)                                 */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-                                 */
/* This simply creates a copy of the provided FIB. No processing or messing  */
/* around, just a straight allocate + copy...                                */

SAVEDS(APTR) ConstructList(struct Hook *hook, APTR MemPool, const struct FileInfoBlock *AddFib)
{
///
	struct FileInfoBlock *NewFib = NULL;

	(void) hook;

	// Allocate a new FIB.
	if(NewFib = AllocPooled(MemPool, sizeof(struct FileInfoBlock)))
		{
		*NewFib = *AddFib;
		}

	return(NewFib);
///
}


/* void DestructList(APTR, FileInfoBlock *)                                  */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=                                  */
/* Destruct hook function used by the main list, this frees the provided FIB */
/* from the memory pool (so it damn well better have been allocated in it!!) */

SAVEDS(void) DestructList(struct Hook *hook, APTR MemPool, struct FileInfoBlock *OldFib)
{
///
	(void) hook;
	FreePooled(MemPool, OldFib, sizeof(struct FileInfoBlock));
///
}


/* LONG  DisplayList(char **, FileInfoBlock *)                               */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-                               */
/* Main list display hook, some messing around has to be done to make sure   */
/* that directories are highlighted, but nothing too complex...              */

SAVEDS(LONG) DisplayList(struct Hook *hook, char **Array, struct FileInfoBlock *ShowFib)
{
///
	static char NameBuffer[1+108], SizeBuffer[20], DateBuffer[24], TimeBuffer[12];
	static struct DateTime Date;

	(void) hook;

	if(ShowFib)
		{
	        Date.dat_Stamp  = ShowFib->fib_Date;
	        Date.dat_Format = FORMAT_DOS;
	        Date.dat_Flags  = DTF_SUBST;
	        Date.dat_StrDay = NULL;
	        Date.dat_StrDate= DateBuffer;
	        Date.dat_StrTime= TimeBuffer;
	        DateToStr(&Date);

		if(ShowFib->fib_DirEntryType < 0)
			{
			stccpy(NameBuffer, ShowFib->fib_FileName, sizeof(NameBuffer));
			snprintf(SizeBuffer, sizeof(SizeBuffer), "\33r%lu", (unsigned long) ShowFib->fib_Size);
			}
		else
			{
			stccpy(NameBuffer, "\0333", sizeof(NameBuffer));
			safe_strcat(NameBuffer, ShowFib->fib_FileName, sizeof(NameBuffer));
			stccpy(SizeBuffer, "\33r\33I[6.22]", sizeof(SizeBuffer));
			}
	        *Array++ = NameBuffer;
	        *Array++ = SizeBuffer;
	        *Array++ = DateBuffer;
	        *Array   = TimeBuffer;
		}
	else
		{
		*Array++ = (char *) GetLocString(MSG_LVT_NAME);
		*Array++ = (char *) GetLocString(MSG_LVT_SIZE);
		*Array++ = (char *) GetLocString(MSG_LVT_DATE);
		*Array   = (char *) GetLocString(MSG_LVT_TIME);
		}

	return(0);
///
}


/* LONG CompareList(FileInfoBlick *, FileInfoBlock *)                        */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=                        */
/* Compare hook function wich takes two FIBs and returns <0 if Fib1 < Fib2,  */
/* 0 if they are equal or >0 if Fib1 > Fib2. Directories automagically go    */
/* before files regardless of alphabetical ordering...                       */

SAVEDS(LONG) CompareList(struct Hook *hook,
	struct FileInfoBlock *Fib1, struct FileInfoBlock *Fib2)
{
///
	(void) hook;

	if(Fib1->fib_DirEntryType == Fib2->fib_DirEntryType)
		{
		return(Stricmp(Fib1->fib_FileName, Fib2->fib_FileName));
		}
	else 	
		{
		if(Fib1->fib_DirEntryType < 0)
			return(1);
		else
			return(-1);
		}
///
}

struct Hook ConList = {{NULL, NULL}, HOOKFUNC_DEF(ConstructList), NULL};
struct Hook DesList = {{NULL, NULL}, HOOKFUNC_DEF(DestructList ), NULL};
struct Hook DisList = {{NULL, NULL}, HOOKFUNC_DEF(DisplayList  ), NULL};
struct Hook CmpList = {{NULL, NULL}, HOOKFUNC_DEF(CompareList  ), NULL};

/* BOOL ListFiles(struct WBStartup *)                                        */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=                                        */
/* Before the main GUI is opened, this routine constructs a list of all the  */
/* files / directories to be deleted (on the first level, not recursively!)  */
/* It detects an attempt to delete a volume, and allows the user to cancel   */
/* the whole operation immediately. The main file list is a list of FIBs so  */
/* the display routine can do nice things like sorting directories and files */
/* seperately and displaying sizes etc.  If anything goes wrong, or the user */
/* cancels the operation, then this routine returns FALSE, otherwise TRUE.   */

BOOL ListFiles(struct WBStartup *WBStart)
{
///
	struct WBArg         *lf_Args       = WBStart->sm_ArgList;
	struct FileInfoBlock *lf_Fib        = NULL;
	LONG           lf_Num        = 0;
	BPTR           lf_File       = (BPTR)NULL;

	ULONG          lf_FileCount  = 0;
	ULONG          lf_DirCount   = 0;

	d1(kprintf("ListFiles(): Started\n");)

	if(WBStart->sm_NumArgs <= 1)
		{
		MUI_Request(MUI_App, WI_Delete, 0,
			(char *) GetLocString(MSG_ERROR_TITLE),
			(char *) GetLocString(MSG_GAD_OK),
			(char *) GetLocString(MSG_BODY_NOFILES));
		return(FALSE);
		}

	d1(kprintf("ListFiles(): Passed arglist check\n");)

	if(!(lf_Fib = AllocDosObject(DOS_FIB, NULL)))
		{
		MUI_Request(MUI_App, WI_Delete, 0,
			(char *) GetLocString(MSG_ERROR_TITLE),
			(char *) GetLocString(MSG_GAD_OK),
			(char *) GetLocString(MSG_BODY_NOLIST ));
		return(FALSE);
		}

	d1(kprintf("ListFiles(): Obtained FileInfoblock\n");)

	for(lf_Num = 1; lf_Num < WBStart->sm_NumArgs; lf_Num ++)
		{
		d1(KPrintF("ListFiles(): Checking entry %ld. Lock 0x%08lX Name <%s>\n", \
			lf_Num, lf_Args[lf_Num].wa_Lock, lf_Args[lf_Num].wa_Name);)

		// If got a lock, change to it
		if(lf_Args[lf_Num].wa_Lock)
			{
			BPTR lf_Old;

			lf_Old = CurrentDir(lf_Args[lf_Num].wa_Lock);

			// Got a filename?
			if(estrlen(lf_Args[lf_Num].wa_Name))
				{
				d1(KPrintF("ListFiles(): Got filename\n");)

				// Lock it...
				if(lf_File = Lock(lf_Args[lf_Num].wa_Name, ACCESS_READ))
					{
			                // Grab the FIB and bung it in the files list.
			                Examine(lf_File, lf_Fib);
			                DoMethod(LV_Files, MUIM_NList_InsertSingle, lf_Fib, MUIV_NList_Insert_Sorted);
					if(lf_Fib->fib_DirEntryType < 0)
						{
						d1(KPrintF("ListFiles(): Entry is a file\n");)
						lf_FileCount ++;
						SizeCount += lf_Fib->fib_Size;
						}
					else
						{
						d1(KPrintF("ListFiles(): Entry is a directory?\n");)
						lf_DirCount ++;
						}
			                UnLock(lf_File);
					}

			// Try for the icon too...
			stccpy(FileBuffer, lf_Args[lf_Num].wa_Name, MAX_FILENAME_LEN);
			safe_strcat(FileBuffer, ".info", MAX_FILENAME_LEN);

			if(lf_File = Lock(FileBuffer, ACCESS_READ))
				{
				d1(KPrintF("ListFiles(): Obtained icon for file\n");)

				// Grab the FIB and bung it in the files list.
				Examine(lf_File, lf_Fib);
				DoMethod(LV_Files, MUIM_NList_InsertSingle, lf_Fib, MUIV_NList_Insert_Sorted);
				if(lf_Fib->fib_DirEntryType < 0)
					{
					lf_FileCount ++;
					SizeCount += lf_Fib->fib_Size;
					}
				UnLock(lf_File);
				}

				}
			else
				{
				d1(KPrintF("ListFiles(): No filename avilable\n");)

				// Resolve a name...
				NameFromLock(lf_Args[lf_Num].wa_Lock, FileBuffer, MAX_FILENAME_LEN);

				// Got a name?
				if(strlen(FileBuffer))
					{
					// Is it a volume?
					if(FileBuffer[strlen(FileBuffer) -1] == ':')
						{
						d1(KPrintF("ListFiles(): WARNING. Filename contains volume name\n");)

						// AGH!! VOLUME DELETE ENCOUNTERED! PANIC!!!!!!
						if(!MUI_Request(MUI_App, WI_Delete, 0,
							(char *) GetLocString(MSG_WARN_TITLE),
							(char *) GetLocString(MSG_GAD_NOYES),
							(char *) GetLocString(MSG_BODY_VOLUME),
							FileBuffer))
                                                        {
							FreeDosObject(DOS_FIB, lf_Fib);
							CurrentDir(lf_Old);
							return(FALSE);
							}
						}
					}

				// Well, either this was a directory, or Joe Sixpack out there is deleting a volume.
				Examine (lf_Args[lf_Num].wa_Lock, lf_Fib);
				if(lf_Fib->fib_DirEntryType >= 0)
					{
					lf_DirCount ++;
					}
				DoMethod(LV_Files, MUIM_NList_InsertSingle, lf_Fib, MUIV_NList_Insert_Sorted);

				// Try for the icon again...
				safe_strcat(FileBuffer, ".info", MAX_FILENAME_LEN);
				if(lf_File = Lock(FileBuffer, ACCESS_READ))
					{
			                d1(kprintf("ListFiles(): Obtained icon for file\n");)

			                // Grab the FIB and bung it in the files list.
			                Examine(lf_File, lf_Fib);
					if(lf_Fib->fib_DirEntryType < 0)
						{
						lf_FileCount ++;
						SizeCount += lf_Fib->fib_Size;
						}
			                DoMethod(LV_Files, MUIM_NList_InsertSingle, lf_Fib, MUIV_NList_Insert_Sorted);
			                UnLock(lf_File);
					}
				}

			// Better get back to home.
			CurrentDir(lf_Old);
			}
		}

	// better not forget this....
	FreeDosObject(DOS_FIB, lf_Fib);

	d1(kprintf("ListFiles(): Compiling stats display\n");)

	snprintf(TextBuffer, sizeof(TextBuffer),
		GetLocString(MSG_LABEL_PROG),
		lf_FileCount,
		SizeCount,
		lf_DirCount);
	set(TX_ReadOut, MUIA_Text_Contents, TextBuffer);

	// Small cheat to avoid divide-by-zero errors...
	SizeCount++;

	// Disable the details option if there are no directories...
	set(BT_Details, MUIA_Disabled, (lf_DirCount == 0));

	d1(kprintf("ListFiles(): All done\n");)

	return(TRUE);
///
}


/* ULONG CopyFile(STRPTR, STRPTR, ULONG)                                     */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-                                     */
/* You'd think that a common operation like a copy would at least be         */
/* supported in dos.library.... ho hum. This function copies one file        */
/* to a destination (doesn't check if it exists though) though a buffer      */
/*                                                                           */
/* Parameters:                                                               */
/*      SourceName  File to copy. (including path)                           */
/*      DestName    filename of the copy (+ path)                            */

LONG CopyFile(STRPTR SourceName, STRPTR DestName, ULONG BufferSize)
{
///
	char *Buffer  = NULL;
	BPTR  Source  = (BPTR)NULL;
	BPTR  Dest    = (BPTR)NULL;
	LONG  ReadLen =    0;

	// Stats for readout..
	ULONG FileSize=    0;
	ULONG Written =    0;

	LONG  Result  = RESULT_OK;
	LONG  TempSig = 0;

	d1(KPrintF("CopyFile(): Copying %s to %s, %ldk buffer specified\n", SourceName, DestName, BufferSize);)

	snprintf(TextBuffer, sizeof(TextBuffer),
		GetLocString(MSG_BODY_COPYING), FilePart(SourceName));
	set(TX_Progress, MUIA_Text_Contents, TextBuffer);
	set(GA_Progress, MUIA_Gauge_Current, 0);

	if(Buffer = malloc(BufferSize * 1024))
		{
	        d1(kprintf("CopyFile(): Buffer allocated at 0x%08lX\n", Buffer);)

		if(Source = Open(SourceName, MODE_OLDFILE))
			{
		        Seek(Source, 0, OFFSET_END);
		        FileSize = Seek(Source, 0, OFFSET_BEGINNING);

		        d1(kprintf("CopyFile(): Source file opened, size is %ld bytes\n", FileSize);)

			if(Dest   = Open(DestName, MODE_NEWFILE))
				{
				d1(KPrintF("CopyFile(): Destination opened, doing copy\n");)

				do 	{
					DoMethod(MUI_App, MUIM_Application_InputBuffered);

					ReadLen = Read(Source, Buffer, (BufferSize * 1024));
					if(ReadLen)
						{
						if(Write(Dest, Buffer, ReadLen) != ReadLen)
							{
							Result = RESULT_FAIL;
							}
						}
					Written += ReadLen;

					if(FileSize > 0)
						{
						set(GA_Progress, MUIA_Gauge_Current, (Written * 100)/FileSize);
						}
					else
						{
						set(GA_Progress, MUIA_Gauge_Current, 100);
						}

					if(DoMethod(MUI_App, MUIM_Application_NewInput, &TempSig) == STOP_IT)
						Result = RESULT_HALT;
					} while((ReadLen == (BufferSize * 1024)) && (Result == RESULT_OK));
				Close(Dest);

				d1(KPrintF("CopyFile(): Copy finished, copy was%s successful\n", (Result != RESULT_OK) ? " not":"");)

				if(Result != RESULT_OK)
					DeleteFile(DestName);
				}
			else
				{
				Result = RESULT_FAIL;
				}
		        Close(Source);
			}
		else
			{
			Result = RESULT_FAIL;
			}
		free(Buffer);
		}
	else
		{
		Result = RESULT_FAIL;
		}

	d1(kprintf("CopyFile(): Copy complete\n");)

	set(TX_Progress, MUIA_Text_Contents, NULL);
	set(GA_Progress, MUIA_Gauge_Current, 0);

	return(Result);
///
}


/* LONG UpdateDrawer(STRPTR)                                                 */
/* -=-=-=-=-=-=-=-=-=-=-=-=-                                                 */
/* This calls the Scalos update routine to redraw the directory. I suppose I */
/* could have got it to do this once all the files have been deleted, for    */
/* speed if nothing else, but that'd probably need another file list and..   */
/* probably more trouble than it's worth IMO.                                */

void UpdateDrawer(STRPTR Name)
{
///
	struct ScaUpdateIcon_IW  Update ;
	STRPTR            TempPos    = NULL;

	TempPos = FilePart(Name);
	stccpy(FileBuffer, TempPos, MAX_FILENAME_LEN);
	TempPos[0] = '\0';

	if(Update.ui_iw_Lock = Lock(Name, SHARED_LOCK))
		{
	        Update.ui_iw_Name = FileBuffer;
	        SCA_UpdateIcon(WSV_Type_IconWindow, &Update, sizeof(struct ScaUpdateIcon_IW));

		safe_strcat(FileBuffer, ".info", MAX_FILENAME_LEN);
	        SCA_UpdateIcon(WSV_Type_IconWindow, &Update, sizeof(struct ScaUpdateIcon_IW));

	        UnLock(Update.ui_iw_Lock);
		}
///
}


/* LONG FileDelete(STRPTR, BOOL)                                             */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-                                             */
/* This routine is mainly here to allow the user to confirm file deletion and*/
/* copy the file to trash if required by the settings. If neither a confirm  */
/* or trashcan copy are required then this is a bit of a waste of space I    */
/* suppose... but it's only just over 20k anyway...                          */

LONG FileDelete(STRPTR FileName, BOOL Trash, APTR UndoStep)
{
///
	LONG  CurrentPos = 0;
	IPTR  Confirm    = 0;
	BPTR  FileLk     = (BPTR)NULL;

	LONG  Result     = 0;
	LONG  TempSig    = 0;

	d1(KPrintF("%s/%s/%ld: processing <%s>\n", __FILE__, __FUNC__, __LINE__, FileName));

	snprintf(TextBuffer, sizeof(TextBuffer),
		GetLocString(MSG_BODY_REMOVE), FilePart(FileName));
	set(TX_Progress, MUIA_Text_Contents, TextBuffer);

	// Ask the user to confirm?
	get(CM_FileConf, MUIA_Selected, &Confirm);
	if(Confirm)
		{
		if(!MUI_Request(MUI_App, WI_Delete, 0,
			(char *) GetLocString(MSG_REQ_TITLE),
			(char *) GetLocString(MSG_GAD_NOYES),
			(char *) GetLocString(MSG_BODY_CFILE),
			FileName))
			{
			return(RESULT_CANCEL);
		        }
		}

	d1(KPrintF("%s/%s/%ld: Confirm stage passed\n", __FILE__, __FUNC__, __LINE__));

	// Copy the file to the trashcan if required.
	if(Trash)
		{
		d1(KPrintF("%s/%s/%ld: Attempting to copy to trashcan\n", __FILE__, __FUNC__, __LINE__));
		d1(KPrintF("%s/%s/%ld: FileBuffer=<%s>  FileName=<%s>\n", __FILE__, __FUNC__, __LINE__, FileBuffer, FileName));

	        CurrentPos = strlen(FileBuffer);
		if(!AddPart(FileBuffer, FilePart(FileName), MAX_FILENAME_LEN))
			{
			// Unable to create filename. Bomb.
			if(!MUI_Request(MUI_App, WI_Delete, 0,
				(char *) GetLocString(MSG_ERROR_TITLE),
				(char *) GetLocString(MSG_GAD_NOYES),
				(char *) GetLocString(MSG_BODY_BUFERR),
				FileName))
				{
		                return(RESULT_FAIL);
				}
			}
		else
			{
			// got filename, copy the file.
			Result = CopyFile(FileName, FileBuffer, 8);

			// Problem?
			if(Result == RESULT_FAIL)
				{
				if(!MUI_Request(MUI_App, WI_Delete, 0,
					(char *) GetLocString(MSG_ERROR_TITLE),
					(char *) GetLocString(MSG_GAD_NOYES),
					(char *) GetLocString(MSG_BODY_COPYFAIL),
					FileName))
					{
					FileBuffer[CurrentPos] = '\0';
					return(RESULT_FAIL);
					}
				// Stopped it completely?
				}
			else if(Result == RESULT_HALT)
				{
				FileBuffer[CurrentPos] = '\0';
				return(RESULT_HALT);
				}
			}
		FileBuffer[CurrentPos] = '\0';
		}

	d1(KPrintF("%s/%s/%ld: Trash copy stage passed, checking for protection\n", __FILE__, __FUNC__, __LINE__));

	// Check for protected status...
	if(FileLk = Lock(FileName, SHARED_LOCK))
		{
		d1(KPrintF("%s/%s/%ld: Locked file, examining\n", __FILE__, __FUNC__, __LINE__));

		if(Examine(FileLk, GlobalFIB))
			{
			d1(KPrintF("%s/%s/%ld: examined\n", __FILE__, __FUNC__, __LINE__));

		        // protected?
			if(GlobalFIB->fib_Protection & FIBF_DELETE)
				{
				d1(KPrintF("%s/%s/%ld: File is protected, asking user to verify\n", __FILE__, __FUNC__, __LINE__));
				if(!Unprotect)
					{
				        Result = MUI_Request(MUI_App, WI_Delete, 0,
						(char *) GetLocString(MSG_REQ_TITLE),
						(char *) GetLocString(MSG_GAD_PROTECT),
						(char *) GetLocString(MSG_BODY_PROTECT),
						FilePart(FileName));

					if(Result == 2)
						Unprotect = TRUE;
					}

				d1(KPrintF("%s/%s/%ld: User replied with response %ld, Unprotect set to %s\n", \
					__FILE__, __FUNC__, __LINE__, Result, Unprotect ? "TRUE" : "FALSE"));

				if(Unprotect || (Result ==  1))
					{
					d1(KPrintF("%s/%s/%ld: Changing protection to 0x%08lX\n", __FILE__, __FUNC__, __LINE__, GlobalFIB->fib_Protection & ~FIBB_DELETE));
					SetProtection(FileName, GlobalFIB->fib_Protection & ~FIBF_DELETE);
					}
				}

		        // Add to the size...
			}
	        UnLock(FileLk);
		}
	else
		{
		d1(KPrintF("%s/%s/%ld: Lock(%s) failed, IoErr=%ld\n", __FILE__, __FUNC__, __LINE__, FileName, IoErr()));
		}

	d1(KPrintF("%s/%s/%ld: protection stage passed, checking for abort and exiting\n", __FILE__, __FUNC__, __LINE__));

	if(DoMethod(MUI_App, MUIM_Application_NewInput, &TempSig) == STOP_IT)
		return(RESULT_HALT);

	return(RESULT_OK);
///
}


/* LONG DeleteIcon(STRPTR, BOOL)                                             */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-                                             */
/* This attempts to lock the specified filename and, if the lock was obtained*/
/* delete it. It's just a fast'n'simple way to remove the icons for files and*/
/* directories.                                                              */

LONG DeleteIcon(STRPTR FileName, BOOL Trash, APTR UndoStep)
{
///
	BPTR IconLock = (BPTR)NULL;
	LONG Result   = RESULT_OK;

	d1(KPrintF("%s/%s/%ld: Deleting icon <%s>\n", __FILE__, __FUNC__, __LINE__, FileName));

	// Attempt to lock it...
	if(IconLock = Lock(FileName, SHARED_LOCK))
		{
	        Examine(IconLock, GlobalFIB);
	        UnLock(IconLock);

		Result = FileDelete(FileName, Trash, UndoStep);

		if(Result == RESULT_OK)
			{
			if(!DeleteFile(FileName))
				{
				Fault(IoErr(), (char *) GetLocString(MSG_BODY_NOICON), TextBuffer, sizeof(TextBuffer) - 1);
				if ( !MUI_Request(MUI_App, WI_Delete, 0,
					(char *) GetLocString(MSG_ERROR_TITLE),
					(char *) GetLocString(MSG_GAD_OK_ABORT),
					TextBuffer,
					FileName))
					{
					Result = RESULT_HALT;
					}
				}
			}
	        DoneSize += GlobalFIB->fib_Size;
		}

	return(Result);
///
}


/* LONG DirDelete(STRPTR, BOOL)                                              */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=                                              */
/* This is a recursive routine which attempts to remove all files and sub-   */
/* directories from the named directory before deleting it. It deleted the   */
/* sub-directories by calling itself with the new directory name. If Trash   */
/* is TRUE then the directory, and all it's contents, are copied to the      */
/* global trashcan before deleting (if one has been specified)               */

LONG DirDelete(STRPTR DirName, BOOL Trash, APTR UndoStep)
{
///
	struct ScaUpdateIcon_IW  Update ;
	struct FileInfoBlock    *ExFIB     = NULL;
	struct FileInfoBlock     OldFIB          ;
	ULONG             Position  = 0;
	LONG              ExNextRtn = 0;
	LONG              Result    = 0;
	LONG              TempSig   = 0;
	BPTR              TrashLock = (BPTR)NULL;
	BPTR              DirLock   = (BPTR)NULL;
	IPTR              Confirm   = 0;

	d1(kprintf("%s()/%ld: Trying to delete %s\n", __FUNCTION__, __LINE__, DirName);)

	DEBUG_CURRENTDIR;

	snprintf(TextBuffer, sizeof(TextBuffer),
		GetLocString(MSG_BODY_REMOVE), FilePart(DirName));
	set(TX_Progress, MUIA_Text_Contents, TextBuffer);

	get(CM_DirConf, MUIA_Selected, &Confirm);
	if(Confirm)
		{
		if(!MUI_Request(MUI_App, WI_Delete, 0,
			(char *) GetLocString(MSG_REQ_TITLE),
			(char *) GetLocString(MSG_GAD_NOYES),
			(char *) GetLocString(MSG_BODY_CDIR),
			DirName))
			{
			return(RESULT_CANCEL);
			}
		}

	d1(kprintf("%s()/%ld: Confirm stage passed\n", __FUNCTION__, __LINE__);)

	if(Trash)
		{
		Position = strlen(FileBuffer);
		AddPart(FileBuffer, FilePart(DirName), MAX_FILENAME_LEN);

		// Does the directory already exist?
		if(!(TrashLock = Lock(FileBuffer, SHARED_LOCK)))
			{
		        // No, try to create it.
			if(!(TrashLock = CreateDir(FileBuffer)))
				{
			        // Create failed. Moan at user.
				if(!MUI_Request(MUI_App, WI_Delete, 0,
					(char *) GetLocString(MSG_ERROR_TITLE),
					(char *) GetLocString(MSG_GAD_NOYES),
					(char *) GetLocString(MSG_BODY_BADTD)))
					{
					FileBuffer[Position] = '\0';
					return(RESULT_CANCEL);
					}
				}
			}
		if(TrashLock)
			UnLock(TrashLock);
		}

	d1(kprintf("%s()/%ld: Trashcan stage passed\n", __FUNCTION__, __LINE__);)

	if(!(ExFIB = AllocDosObject(DOS_FIB, NULL)))
		{
		MUI_Request(MUI_App, WI_Delete, 0,
			(char *) GetLocString(MSG_ERROR_TITLE),
			(char *) GetLocString(MSG_GAD_OK),
		        (char *) GetLocString(MSG_BODY_NOSCAN));
		if(Trash)
			FileBuffer[Position] = '\0';
		return(RESULT_FAIL);
		}

	d1(kprintf("%s()/%ld: Obtained FIB, locking and examining, DirName=<%s>\n", __FUNCTION__, __LINE__, DirName);)

	if(DirLock = Lock(DirName, SHARED_LOCK))
		{
		BPTR OldDir;

		OldDir = CurrentDir(DirLock);

		DEBUG_CURRENTDIR;

		if(Examine(DirLock, ExFIB))
			{
		        d1(kprintf("%s()/%ld: Examined, checking protection\n", __FUNCTION__, __LINE__);)

		        // protected?
			if(ExFIB->fib_Protection & FIBF_DELETE)
				{
				if(!Unprotect)
				        {
					Result = MUI_Request(MUI_App, WI_Delete, 0,
						(char *) GetLocString(MSG_REQ_TITLE),
						(char *) GetLocString(MSG_GAD_PROTECT),
						(char *) GetLocString(MSG_BODY_PROTECT),
				                FilePart(DirName));

					if(Result == 2)
						Unprotect = TRUE;
				        }

				if(Unprotect || (Result ==  1))
					{
					SetProtection(DirName, ExFIB->fib_Protection - FIBB_DELETE);
					}
				}

			d1(kprintf("%s()/%ld: Protection stage complete, checking contents\n", __FUNCTION__, __LINE__);)

			DEBUG_CURRENTDIR;

			if(ExNext(DirLock, ExFIB))
				{
				do 	{
					DEBUG_CURRENTDIR;

			                DoMethod(MUI_App, MUIM_Application_InputBuffered);

			                // Copy and jump to next entry (allows delete).
					OldFIB = *ExFIB;
					ExNextRtn = ExNext(DirLock, ExFIB);

					DEBUG_CURRENTDIR;
					d1(kprintf("%s()/%ld: fib_DirEntryType=%ld\n", __FUNCTION__, __LINE__, OldFIB.fib_DirEntryType);)

					if (ST_SOFTLINK == OldFIB.fib_DirEntryType)
					{
						// It's a softlink!
						d1(kprintf("%s()/%ld: <%s> is a softlink, removing it\n", __FUNCTION__, __LINE__, OldFIB.fib_FileName);)

						Result = FileDelete(OldFIB.fib_FileName, Trash, UndoStep);
						d1(kprintf("%s()/%ld: FileDelete() returned %ld\n", __FUNCTION__, __LINE__, Result);)
						if(Result == RESULT_OK)
							{
							d1(kprintf("%s()/%ld: calling DeleteFile(%s)\n", __FUNCTION__, __LINE__, OldFIB.fib_FileName);)
							if(!DeleteFile(OldFIB.fib_FileName))
								{
								Fault(IoErr(), (char *) GetLocString(MSG_BODY_NODEL), TextBuffer, sizeof(TextBuffer) - 1);
								if (!MUI_Request(MUI_App, WI_Delete, 0,
									(char *) GetLocString(MSG_ERROR_TITLE),
									(char *) GetLocString(MSG_GAD_OK_ABORT),
									TextBuffer,
									OldFIB.fib_FileName))
									{
									CurrentDir(OldDir);
									UnLock(DirLock);
									return RESULT_HALT;
									}
								}
							}

				                DoneSize += OldFIB.fib_Size;
					}
					else if(OldFIB.fib_DirEntryType < 0)
						{
						// It's a file!
						d1(kprintf("%s()/%ld: <%s> is a file, removing it\n", __FUNCTION__, __LINE__, OldFIB.fib_FileName);)

						{
						BPTR fLock = Lock(OldFIB.fib_FileName, ACCESS_READ);
						if (fLock)
							UnLock(fLock);
						else
							d1(KPrintF("%s/%s/%ld: Lock(%s) failed, IoErr=%ld\n", __FILE__, __FUNC__, __LINE__, OldFIB.fib_FileName, IoErr()));
						}

						Result = FileDelete(OldFIB.fib_FileName, Trash, UndoStep);
						d1(kprintf("%s()/%ld: FileDelete() returned %ld\n", __FUNCTION__, __LINE__, Result);)
						if(Result == RESULT_OK)
							{
							d1(kprintf("%s()/%ld: calling DeleteFile(%s)\n", __FUNCTION__, __LINE__, OldFIB.fib_FileName);)
							if(!DeleteFile(OldFIB.fib_FileName))
								{
								Fault(IoErr(), (char *) GetLocString(MSG_BODY_NODEL), TextBuffer, sizeof(TextBuffer) - 1);
								if (!MUI_Request(MUI_App, WI_Delete, 0,
									(char *) GetLocString(MSG_ERROR_TITLE),
									(char *) GetLocString(MSG_GAD_OK_ABORT),
									TextBuffer,
									OldFIB.fib_FileName))
									{
									CurrentDir(OldDir);
									UnLock(DirLock);
									return RESULT_HALT;
									}
								}
							}

				                DoneSize += OldFIB.fib_Size;
						}
					else
						{
						d1(kprintf("%s()/%ld: %s is a directory, removing it\n", __FUNCTION__, __LINE__, OldFIB.fib_FileName);)

				                Result = DirDelete(OldFIB.fib_FileName, Trash, UndoStep);
						if(Result == RESULT_OK)
							{
							if(!DeleteFile(OldFIB.fib_FileName))
								{
								Fault(IoErr(), (char *) GetLocString(MSG_BODY_NODEL), TextBuffer, sizeof(TextBuffer) - 1);
								if (!MUI_Request(MUI_App, WI_Delete, 0,
									(char *) GetLocString(MSG_ERROR_TITLE),
									(char *) GetLocString(MSG_GAD_OK_ABORT),
									TextBuffer,
									OldFIB.fib_FileName))
									{
									CurrentDir(OldDir);
									UnLock(DirLock);
									return RESULT_HALT;
									}

								}
							}
						}

					d1(kprintf("%s()/%ld: Updating directory window\n", __FUNCTION__, __LINE__);)

			                // Update the window. This must be done as the window for the
			                // directory could, in theory, be opened already...
					Update.ui_iw_Lock = DirLock;
			                Update.ui_iw_Name = OldFIB.fib_FileName;
			                SCA_UpdateIcon(WSV_Type_IconWindow, &Update, sizeof(struct ScaUpdateIcon_IW));

					if(DoMethod(MUI_App, MUIM_Application_NewInput, &TempSig) == STOP_IT)
						Result = RESULT_HALT;

			                // Only bother to update the GUI if the details have been obtained..
					if(GotDetails)
						set(GA_Progress, MUIA_Gauge_Current, (DoneSize * 100) / SizeCount);
					} while(ExNextRtn && (Result != RESULT_HALT));
				}
			}
		CurrentDir(OldDir);
		UnLock(DirLock);
		}

	d1(kprintf("%s()/%ld: Finished working\n", __FUNCTION__, __LINE__);)

	FreeDosObject(DOS_FIB, ExFIB);
	if(Trash)
		FileBuffer[Position] = '\0';

	DEBUG_CURRENTDIR;

	if(Result == RESULT_HALT)
		return(RESULT_HALT);
	else
		return(RESULT_OK);
///
}


/* void KillFiles(WBStartup *)                                               */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-                                               */
/* This is the routine which attempts to delete the files the user selected  */
/* on the Scalos screen. Files are simply deleted (along with their icons)   */
/* but directories are deleted by the DirDelete() routine.                   */

void KillFiles(struct WBStartup *WBStart)
{
///
	struct ScaUpdateIcon_IW  kf_Update ;
	struct WBArg            *kf_Args   = WBStart->sm_ArgList;
	struct FileInfoBlock    *kf_Fib    = NULL;
	LONG              kf_Num    = 0;
	BPTR              kf_File   = (BPTR)NULL;
	LONG              kf_Result = 0;
	IPTR              Trash     = FALSE;
	STRPTR            TrashDir  = NULL;
	APTR 		  UndoStep = NULL;
	struct ScaWindowStruct *ws = NULL;
	struct ScaWindowList *wl;

	d1(KPrintF("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	get(CM_TrashCan, MUIA_Selected, &Trash);

	wl = SCA_LockWindowList(SCA_LockWindowList_Shared);
	if (wl)
		{
		struct ScaWindowStruct *ws = wl->wl_WindowStruct;

		UndoStep = (APTR) DoMethod(ws->ws_WindowTask->mt_MainObject,
			SCCM_IconWin_BeginUndoStep);

		SCA_UnLockWindowList();
		}

	if(Trash)
		{
		get(ST_TrashDir, MUIA_String_Contents, &TrashDir);
		}

	if(!(kf_Fib = AllocDosObject(DOS_FIB, NULL)))
		{
		MUI_Request(MUI_App, WI_Delete, 0,
			(char *) GetLocString(MSG_ERROR_TITLE),
			(char *) GetLocString(MSG_GAD_OK),
	                (char *) GetLocString(MSG_BODY_NOLIST));
	        return;
		}

	if(!(GlobalFIB = AllocDosObject(DOS_FIB, NULL)))
		{
		MUI_Request(MUI_App, WI_Delete, 0,
			(char *) GetLocString(MSG_ERROR_TITLE),
			(char *) GetLocString(MSG_GAD_OK),
	                (char *) GetLocString(MSG_BODY_NOLIST));
	        return;
		}

	set(GP_Progress, MUIA_ShowMe, TRUE);

	for(kf_Num = 1; ((kf_Num < WBStart->sm_NumArgs) && (kf_Result != RESULT_HALT)); kf_Num ++)
		{
		DoMethod(MUI_App, MUIM_Application_InputBuffered);

		if(Trash)
		        {
			struct ScaWindowStruct *wsFile;

			if(estrlen(TrashDir))
				{
				stccpy(FileBuffer, TrashDir, MAX_FILENAME_LEN);
				}
			else
				{
				stccpy(FileBuffer, "SYS:TrashCan/", MAX_FILENAME_LEN);
				}

			wsFile = FindScalosWindow(kf_Args[kf_Num].wa_Lock);
			if (wsFile)
				{
				DoMethod(wsFile->ws_WindowTask->mt_MainObject,
					SCCM_IconWin_AddUndoEvent,
					UNDO_Delete,
					UNDOTAG_CopySrcDirLock, kf_Args[kf_Num].wa_Lock,
					UNDOTAG_CopySrcName, kf_Args[kf_Num].wa_Name,
					UNDOTAG_CopyDestName, FileBuffer,
					TAG_END
					);

				SCA_UnLockWindowList();
				}
		        }

		d1(KPrintF("%s/%s/%ld: Processing %ld\n", __FILE__, __FUNC__, __LINE__, kf_Num));

		// If got a lock, change to it
		if(kf_Args[kf_Num].wa_Lock)
		        {
			BPTR  kf_Old;
			
			kf_Old = CurrentDir(kf_Args[kf_Num].wa_Lock);

			d1(KPrintF("%s/%s/%ld: Filename: %s Lock: 0x%08lX\n", __FILE__, __FUNC__, __LINE__, kf_Args[kf_Num].wa_Name, kf_Args[kf_Num].wa_Lock));

			// Got a filename?
			if(estrlen(kf_Args[kf_Num].wa_Name))
				{
				d1(KPrintF("%s/%s/%ld: Passed estrlen()\n", __FILE__, __FUNC__, __LINE__));

		                // Lock it...
				if(kf_File = Lock(kf_Args[kf_Num].wa_Name, ACCESS_READ))
					{
					d1(KPrintF("%s/%s/%ld: Passed Lock()\n", __FILE__, __FUNC__, __LINE__));

					// Grab the FIB and bung it in the files list.
					Examine(kf_File, kf_Fib);
					UnLock(kf_File);

					if (kf_Fib->fib_DirEntryType < 0)
						{
						kf_Result = FileDelete(kf_Fib->fib_FileName, Trash, UndoStep);
						if(kf_Result == RESULT_OK)
							{
							if(!DeleteFile(kf_Fib->fib_FileName))
								{
								Fault(IoErr(), (char *) GetLocString(MSG_BODY_NODEL), TextBuffer, sizeof(TextBuffer) - 1);
								MUI_Request(MUI_App, WI_Delete, 0,
									(char *) GetLocString(MSG_ERROR_TITLE),
									(char *) GetLocString(MSG_GAD_OK),
									TextBuffer,
									kf_Fib->fib_FileName);
								}
			
							stccpy(PathBuffer, kf_Fib->fib_FileName, MAX_FILENAME_LEN);
							safe_strcat(PathBuffer, ".info", MAX_FILENAME_LEN);

							kf_Result = DeleteIcon(PathBuffer, Trash, UndoStep);
							}
						}
					else
						{
						kf_Result = DirDelete(DirBuffer, Trash, UndoStep);
						if(kf_Result == RESULT_OK)
							{
							UnLock(kf_Args[kf_Num].wa_Lock);
							kf_Args[kf_Num].wa_Lock = (BPTR)NULL;

							if(!DeleteFile(DirBuffer))
								{
								Fault(IoErr(), (char *) GetLocString(MSG_BODY_NODEL), TextBuffer, sizeof(TextBuffer) - 1);
								MUI_Request(MUI_App, WI_Delete, 0,
									(char *) GetLocString(MSG_ERROR_TITLE),
									(char *) GetLocString(MSG_GAD_OK),
									TextBuffer,
									DirBuffer);
								}

							safe_strcat(DirBuffer, ".info", MAX_FILENAME_LEN);
							kf_Result = DeleteIcon(DirBuffer, Trash, UndoStep);

							DirBuffer[strlen(DirBuffer) - 5] = '\0';
							UpdateDrawer(DirBuffer);
							}
						}
					DoneSize += kf_Fib->fib_Size;
					}
				else
					{
					d1(KPrintF("%s/%s/%ld: Failed Lock()\n", __FILE__, __FUNC__, __LINE__));

					stccpy(PathBuffer, kf_Args[kf_Num].wa_Name, MAX_FILENAME_LEN);
					safe_strcat(PathBuffer, ".info", MAX_FILENAME_LEN);

					kf_Result = DeleteIcon(PathBuffer, Trash, UndoStep);
					}

				// Better get back to home.
		                CurrentDir(kf_Old);

				d1(KPrintF("%s/%s/%ld: Updating\n", __FILE__, __FUNC__, __LINE__));

		                kf_Update.ui_iw_Lock = kf_Args[kf_Num].wa_Lock;
		                kf_Update.ui_iw_Name = kf_Args[kf_Num].wa_Name;
		                SCA_UpdateIcon(WSV_Type_IconWindow, &kf_Update, sizeof(struct ScaUpdateIcon_IW));

		                kf_Update.ui_iw_Lock = kf_Args[kf_Num].wa_Lock;
		                kf_Update.ui_iw_Name = PathBuffer;
		                SCA_UpdateIcon(WSV_Type_IconWindow, &kf_Update, sizeof(struct ScaUpdateIcon_IW));

				// No name. Must be a directory :/
				}
			else
				{
		                CurrentDir(kf_Old);

		                // Resolve a name...
		                NameFromLock(kf_Args[kf_Num].wa_Lock, DirBuffer, 256);

				if(!strcmp(DirBuffer, FileBuffer))
					{
					MUI_Request(MUI_App, WI_Delete, 0,
						(char *) GetLocString(MSG_ERROR_TITLE),
						(char *) GetLocString(MSG_GAD_OK),
						"\33c%s\nis the trashcan!\nUse the \"Empty Trashcan\"\nto clear it");
					}
				else
					{
					kf_Result = DirDelete(DirBuffer, Trash, UndoStep);
					if(kf_Result == RESULT_OK)
						{
						UnLock(kf_Args[kf_Num].wa_Lock);
						kf_Args[kf_Num].wa_Lock = (BPTR)NULL;

						if(!DeleteFile(DirBuffer))
							{
							Fault(IoErr(), (char *) GetLocString(MSG_BODY_NODEL), TextBuffer, sizeof(TextBuffer) - 1);
							MUI_Request(MUI_App, WI_Delete, 0,
								(char *) GetLocString(MSG_ERROR_TITLE),
								(char *) GetLocString(MSG_GAD_OK),
								TextBuffer,
								DirBuffer);
							}

						safe_strcat(DirBuffer, ".info", MAX_FILENAME_LEN);
						kf_Result = DeleteIcon(DirBuffer, Trash, UndoStep);

						DirBuffer[strlen(DirBuffer) - 5] = '\0';
						UpdateDrawer(DirBuffer);
						}
					}
				}
		        }
		d1(KPrintF("%s/%s/%ld:  Processing complete\n", __FILE__, __FUNC__, __LINE__));

		set(GA_Progress, MUIA_Gauge_Current, (DoneSize * 100) / SizeCount);
		}

	// Release the fibs...
	FreeDosObject(DOS_FIB, kf_Fib   );
	FreeDosObject(DOS_FIB, GlobalFIB);

	if (ws)
		{
		if (UndoStep)
			{
			DoMethod(ws->ws_WindowTask->mt_MainObject,
				SCCM_IconWin_EndUndoStep,
				UndoStep);
			}
		SCA_UnLockWindowList();
		}

	d1(KPrintF("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
///
}


/* void DirCount(STRPTR, DetailStats *)                                      */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=                                      */
/* Resursive directory & file count routine. If the user has selected one or */
/* more directories then this handles counting the number of files and sub-  */
/* directories within the selected directories. It calls itself to compile   */
/* the information on any directories it encountered.                        */

void DirCount(STRPTR DirName, struct DetailStats *Stats)
{
///
	struct FileInfoBlock  *ExFIB     = NULL;
	struct FileInfoBlock   OldFIB          ;
	LONG            ExNextRtn = 0;
	BPTR            DirLock   = (BPTR)NULL;

	snprintf(TextBuffer, sizeof(TextBuffer),
		GetLocString(MSG_BODY_SCAN), FilePart(DirName));
	set(TX_Progress, MUIA_Text_Contents, TextBuffer);

	// Allocate some room for the FIB..
	if(!(ExFIB = AllocDosObject(DOS_FIB, NULL)))
		{
		MUI_Request(MUI_App, WI_Delete, 0,
			(char *) GetLocString(MSG_ERROR_TITLE),
			(char *) GetLocString(MSG_GAD_OK),
			(char *) GetLocString(MSG_BODY_NOSCAN));
		return;
		}

	// Lock and change to the directory....
	if(DirLock = Lock(DirName, SHARED_LOCK))
		{
		BPTR OldDir;
		
		OldDir = CurrentDir(DirLock);

		if(Examine(DirLock, ExFIB))
			{
			if(ExNext(DirLock, ExFIB))
				{
				do 	{
					DoMethod(MUI_App, MUIM_Application_InputBuffered);

			                // Copy and jump to next entry (allows delete).
			                CopyMem(ExFIB, &OldFIB, sizeof(struct FileInfoBlock));
			                ExNextRtn = ExNext(DirLock, ExFIB);

					if(OldFIB.fib_DirEntryType < 0)
						{
						// It's a file!
						Stats->ds_Files ++;
						Stats->ds_Bytes += OldFIB.fib_Size;
						}
					else
						{
						// It's a directory. Recurse down a level..
						Stats->ds_Dirs ++;
						DirCount(OldFIB.fib_FileName, Stats);
						}

					} while(ExNextRtn);
				}
			}
		CurrentDir(OldDir);
		UnLock(DirLock);
		}

	FreeDosObject(DOS_FIB, ExFIB);
///
}


/* BOOL GetDetails(WBStartup *)                                              */
/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=                                              */
/* Very similar to ListFiles() except this obtains the statistics for the    */
/* whole tree of selected directories (ie: it gives a much better picture of */
/* exactly how much really needs to be deleted). This uses DirCount() to     */
/* obtain the info on directories and devices and it will, almost certainly, */
/* take a fair amount of time to complete (not to mention eat stack space...)*/

BOOL GetDetails(struct WBStartup *WBStart)
{
///
	struct WBArg         *gd_Args       = WBStart->sm_ArgList;
	struct FileInfoBlock *gd_Fib        = NULL;
	struct DetailStats    gd_Details    = { 0, 0, 0 };
	LONG           gd_Num        = 0;
	BPTR           gd_File       = (BPTR)NULL;


	if(MUI_Request(MUI_App, WI_Delete, 0,
		(char *) GetLocString(MSG_WARN_TITLE),
		(char *) GetLocString(MSG_GAD_NOYES),
		(char *) GetLocString(MSG_BODY_DETAIL)))
                {
		if(!(gd_Fib = AllocDosObject(DOS_FIB, NULL)))
			{
		        MUI_Request(MUI_App, WI_Delete, 0,
				(char *) GetLocString(MSG_ERROR_TITLE),
				(char *) GetLocString(MSG_GAD_OK),
		                (char *) GetLocString(MSG_BODY_NOLIST ));
		        return(FALSE);
			}

	        set(MUI_App, MUIA_Application_Sleep, TRUE);

		for(gd_Num = 1; gd_Num < WBStart->sm_NumArgs; gd_Num ++)
			{
		        // If got a lock, change to it
			if(gd_Args[gd_Num].wa_Lock)
				{
				BPTR gd_Old;
				
				gd_Old = CurrentDir(gd_Args[gd_Num].wa_Lock);

		                // Got a filename?
				if(estrlen(gd_Args[gd_Num].wa_Name))
					{
					// Lock it...
					if(gd_File = Lock(gd_Args[gd_Num].wa_Name, ACCESS_READ))
						{
			                        Examine(gd_File, gd_Fib);
						if(gd_Fib->fib_DirEntryType < 0)
							{
							// File, add to the file stuff....
							gd_Details.ds_Files ++;
							gd_Details.ds_Bytes += gd_Fib->fib_Size;
							}
			                        UnLock(gd_File);
						}

					// Try for the icon too...
					stccpy(FileBuffer, gd_Args[gd_Num].wa_Name, MAX_FILENAME_LEN);
					safe_strcat(FileBuffer, ".info", MAX_FILENAME_LEN);

					if(gd_File = Lock(FileBuffer, ACCESS_READ))
						{
			                        // Grab the FIB and bung it in the files list.
			                        Examine(gd_File, gd_Fib);
						if(gd_Fib->fib_DirEntryType < 0)
							{
							gd_Details.ds_Files ++;
							gd_Details.ds_Bytes += gd_Fib->fib_Size;
							}
			                        UnLock(gd_File);
						}

			                // No filename, must be a directory
					}
				else
					{
			                // Resolve a name...
				        NameFromLock(gd_Args[gd_Num].wa_Lock, FileBuffer, MAX_FILENAME_LEN);

			                // Directory or volume, ours is not to reason why....
			                Examine (gd_Args[gd_Num].wa_Lock, gd_Fib);
			                gd_Details.ds_Dirs ++;
			                DirCount(FileBuffer, &gd_Details);

			                // Try for the icon again...
				        safe_strcat(FileBuffer, ".info", MAX_FILENAME_LEN);

					if(gd_File = Lock(FileBuffer, ACCESS_READ))
						{
			                        // Grab the FIB and bung it in the files list.
			                        Examine(gd_File, gd_Fib);
						if(gd_Fib->fib_DirEntryType < 0)
							{
				                        gd_Details.ds_Files ++;
				                        gd_Details.ds_Bytes += gd_Fib->fib_Size;
							}
			                        UnLock(gd_File);
						}
					}

		                // Better get back to home.
		                CurrentDir(gd_Old);
				}
			}

	        // better not forget this....
	        FreeDosObject(DOS_FIB, gd_Fib);

		snprintf(TextBuffer, sizeof(TextBuffer),
			GetLocString(MSG_LABEL_PROG),
			gd_Details.ds_Files,
			gd_Details.ds_Bytes,
			gd_Details.ds_Dirs);
	        set(TX_ReadOut, MUIA_Text_Contents, TextBuffer);

	        SizeCount = gd_Details.ds_Bytes + 1;
	        GotDetails = TRUE;

	        set(TX_Progress, MUIA_Text_Contents, NULL);
	        set(MUI_App, MUIA_Application_Sleep, FALSE);
		}

	return(TRUE);
///
}

//----------------------------------------------------------------------------

///
static struct ScaWindowStruct *FindScalosWindow(BPTR dirLock)
{
	struct ScaWindowList *wl;
	BOOL Found = FALSE;

	d1(KPrintF(__FILE__ "/%s/%ld: START \n", __FUNC__, __LINE__));

	wl = SCA_LockWindowList(SCA_LockWindowList_Shared);

	if (wl)
		{
		struct ScaWindowStruct *ws;

		for (ws = wl->wl_WindowStruct; !Found && ws; ws = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ)
			{
			if ((BNULL == dirLock && BNULL == ws->ws_Lock) || (LOCK_SAME == SameLock(dirLock, ws->ws_Lock)))
				{
				return ws;
				}
			}
		SCA_UnLockWindowList();
		}

	return NULL;
}
///

//----------------------------------------------------------------------------


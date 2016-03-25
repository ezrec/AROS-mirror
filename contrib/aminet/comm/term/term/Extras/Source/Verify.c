/*
**	Verify.c
**
**	Check if files/drawers/programs are where they should be
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

struct Path
{
	BPTR path_Next,path_Lock;
};

	/* FindPath(struct Window *Parent,STRPTR Path,BOOL CanCreate,LONG *Error):
	 *
	 *	For the complete path of a file, this checks if the
	 *	path leading to the file actually exists. This is
	 *	primarily for files the program should create. Thus,
	 *	the file in question may not yet exist.
	 */

BOOL
FindPath(struct Window *Parent,STRPTR Path,BOOL CanCreate,LONG *Error)
{
	UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
	STRPTR Index;

	strcpy(LocalBuffer,Path);

	Index = PathPart(LocalBuffer);

	*Index = 0;

	return(FindDrawer(Parent,LocalBuffer,CanCreate,Error));
}

	/* FindDrawer(struct Window *Parent,STRPTR Drawer,BOOL CanCreate,LONG *Error):
	 *
	 *	Check if a given drawer exists and give
	 *	the user the choice to create it if necessary.
	 */

BOOL
FindDrawer(struct Window *Parent,STRPTR Drawer,BOOL CanCreate,LONG *Error)
{
	LONG LocalError;
	BPTR FileLock;
	BOOL Result;
	APTR OldPtr;

	if(!Config->MiscConfig->ProtectiveMode)
		return(TRUE);

	Result = FALSE;

	if(!Error)
		Error = &LocalError;

		/* Lock out DOS requesters */

	DisableDOSRequesters(&OldPtr);

		/* Does the drawer exist? */

	if(FileLock = Lock(Drawer,ACCESS_READ))
	{
		D_S(struct FileInfoBlock,FileInfo);

			/* Take a closer look */

		if(Examine(FileLock,FileInfo))
		{
				/* Is this actually a file? */

			if(FileInfo->fib_DirEntryType < 0)
				*Error = ERR_NOT_A_DRAWER;
			else
			{
				*Error = 0;

				Result = TRUE;
			}
		}
		else
			*Error = IoErr();

		UnLock(FileLock);
	}
	else
	{
		LONG Result2 = IoErr();

			/* If it doesn't exist, see if we should ask the user to create it */

		if(Result2 == ERROR_OBJECT_NOT_FOUND && Parent && CanCreate)
		{
			UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
			BOOL LocalCanCreate;
			STRPTR Index;

			LocalCanCreate = FALSE;

				/* Chop off the last path part */

			strcpy(LocalBuffer,Drawer);

			Index = PathPart(LocalBuffer);

			*Index = 0;

				/* Get a lock on the parent drawer */

			if(FileLock = Lock(LocalBuffer,ACCESS_READ))
			{
				D_S(struct InfoData,InfoData);

					/* Inquire volume information */

				if(Info(FileLock,InfoData))
				{
						/* Can we write to this volume? */

					if(InfoData->id_DiskState == ID_VALIDATED)
						LocalCanCreate = TRUE;
				}

				UnLock(FileLock);
			}

				/* Give it a try? */

			if(LocalCanCreate)
			{
				LT_LockWindow(Parent);

					/* Ask if we should create it */

				if(ShowRequest(Parent,LocaleString(MSG_VERIFY_NO_DRAWER_TXT),LocaleString(MSG_GLOBAL_YES_NO_TXT),Drawer))
				{
					if(FileLock = CreateDir(Drawer))
					{
						Result2 = 0;

						Result = TRUE;

						UnLock(FileLock);
					}
					else
						Result2 = IoErr();
				}
				else
				{
					Result2 = 0;
					Result = TRUE;
				}

				LT_UnlockWindow(Parent);
			}
		}

		*Error = Result2;
	}

	EnableDOSRequesters(OldPtr);

	if(Window && !Result && *Error)
	{
		LT_LockWindow(Parent);

		if(*Error == ERR_NOT_A_DRAWER)
			ShowError(Parent,*Error,0,Drawer);
		else
			ShowError(Parent,ERR_DRAWER_NOT_FOUND,*Error,Drawer);

		LT_UnlockWindow(Parent);
	}

	return(Result);
}

	/* FindFile(struct Window *Parent,STRPTR File,LONG *Error):
	 *
	 *	Try to locate a file.
	 */

BOOL
FindFile(struct Window *Parent,STRPTR File,LONG *Error)
{
	LONG LocalError;
	BPTR FileLock;
	BOOL Result;
	APTR OldPtr;

	if(!Config->MiscConfig->ProtectiveMode)
		return(TRUE);

	Result = FALSE;

	if(!Error)
		Error = &LocalError;

		/* Lock out DOS requesters */

	DisableDOSRequesters(&OldPtr);

		/* Try to get a lock on the file */

	if(FileLock = Lock(File,ACCESS_READ))
	{
		D_S(struct FileInfoBlock,FileInfo);

			/* Take a closer look */

		if(Examine(FileLock,FileInfo))
		{
				/* Is it actually a drawer? */

			if(FileInfo->fib_DirEntryType > 0)
				*Error = ERR_NOT_A_FILE;
			else
			{
				*Error = 0;

				Result = TRUE;
			}
		}
		else
			*Error = IoErr();

		UnLock(FileLock);
	}
	else
		*Error = IoErr();

	EnableDOSRequesters(OldPtr);

	if(Window && !Result && *Error)
	{
		LT_LockWindow(Parent);

		if(*Error == ERR_NOT_A_FILE)
			ShowError(Parent,*Error,0,File);
		else
			ShowError(Parent,ERR_FILE_NOT_FOUND,*Error,File);

		LT_UnlockWindow(Parent);
	}

	return(Result);
}

	/* FindARexxComment(STRPTR LocalBuffer,LONG Len):
	 *
	 *	Check for a valid ARexx file comment.
	 */

STATIC BOOL
FindARexxComment(STRPTR LocalBuffer,LONG Len)
{
	UBYTE c;
	LONG i;

	for(i = 0 ; i < Len - 1 ; i++)
	{
		c = LocalBuffer[i];

			/* Stop on invalid characters. */

		if((c < ' ' && c != '\r' && c != '\n' && c != '\a') || (c >= 127 && c < 160))
			break;
		else
		{
				/* Looks like the typical */
				/* introductory comment line. */

			if(c == '/' && LocalBuffer[i + 1] == '*')
				return(TRUE);
		}
	}

	return(FALSE);
}

	/* FindProgram(struct Window *Parent,STRPTR Program,LONG *Error):
	 *
	 *	Try to find a program given by name. Firstly, try to access the
	 *	file by its name. If unsuccessful, search the Shell path list.
	 *	As a last resort, search the global AmigaDOS resident program
	 *	list. Note that while this routine may fail program execution
	 *	may still work. WShell for example maintains its own internal
	 *	resident list.
	 */

BOOL
FindProgram(struct Window *Parent,STRPTR Program,LONG *Error)
{
	UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
	BOOL Result,CanExecute;
	LONG LocalError;
	BPTR FileLock;
	APTR OldPtr;
	LONG i;

	if(!Config->MiscConfig->ProtectiveMode)
		return(TRUE);

	Result		= FALSE;
	CanExecute	= FALSE;

	if(!Error)
		Error = &LocalError;

		/* Chop off the program arguments */

	while(*Program == ' ')
		Program++;

	strcpy(LocalBuffer,Program);

	for(i = 0 ; i < strlen(LocalBuffer) ; i++)
	{
		if(LocalBuffer[i] == ' ')
		{
			LocalBuffer[i] = 0;
			break;
		}
	}

	Program = LocalBuffer;

		/* Lock out DOS requesters */

	DisableDOSRequesters(&OldPtr);

		/* Try to lock the program */

	if(FileLock = Lock(Program,ACCESS_READ))
	{
		D_S(struct FileInfoBlock,FileInfo);

			/* Take a closer look */

		if(Examine(FileLock,FileInfo))
		{
				/* Is it actually a drawer? */

			if(FileInfo->fib_DirEntryType > 0)
				*Error = ERR_NOT_A_FILE;
			else
			{
				BOOL GotIt;

					/* So far, so good */

				*Error = 0;

				Result	= TRUE;
				GotIt	= FALSE;

					/* Is it marked as being executable? */

				if(!(FileInfo->fib_Protection & FIBF_EXECUTE))
				{
					BPTR FileHandle;
					ULONG Value;

						/* Look at the contents */

					if(FileHandle = Open(Program,MODE_OLDFILE))
					{
						if(Read(FileHandle,&Value,sizeof(ULONG)) == sizeof(ULONG))
						{
								/* Not a failproof check, but better than nothing */

							if(Value == HUNK_HEADER)
							{
								CanExecute = TRUE;
								GotIt = TRUE;
							}
						}

						Close(FileHandle);
					}
				}

				if(!GotIt)
				{
						/* Does it have the script bit set? */

					if(FileInfo->fib_Protection & FIBF_SCRIPT)
						CanExecute = TRUE;
					else
					{
						BPTR FileHandle;

							/* Now check if it's an ARexx script. */

						if(FileHandle = Open(Program,MODE_OLDFILE))
						{
							UBYTE LocalBuffer[256];
							LONG Len;

							if((Len = Read(FileHandle,LocalBuffer,sizeof(LocalBuffer))) > 0)
							{
								if(CanExecute = FindARexxComment(LocalBuffer,Len))
									Result = TRUE;
							}

							Close(FileHandle);
						}
					}
				}
			}
		}

		UnLock(FileLock);
	}
	else
	{
		LONG Result2 = IoErr();

		if(Result2 == ERROR_DEVICE_NOT_MOUNTED)
			*Error = ERROR_OBJECT_NOT_FOUND;
		else
			*Error = Result2;
	}

		/* Give it another try */

	if(!Result || !CanExecute)
	{
			/* Take care */

		if(ThisProcess->pr_CLI)
		{
			struct CommandLineInterface	*CLI;
			BOOL InitialCD,GotIt;
			BPTR StartCD = BNULL,Drawer;
			struct Path *Path;

			CLI			= BADDR(ThisProcess->pr_CLI);
			InitialCD	= TRUE;
			GotIt		= FALSE;

				/* Run down the Shell path search list */

			for(Path = BADDR(CLI->cli_CommandDir) ; Path && !GotIt ; Path = BADDR(Path->path_Next))
			{
				Drawer = CurrentDir(Path->path_Lock);

				if(InitialCD)
				{
					StartCD		= Drawer;
					InitialCD	= FALSE;
				}

					/* Try to lock the program */

				if(FileLock = Lock(Program,ACCESS_READ))
				{
					D_S(struct FileInfoBlock,FileInfo);

						/* Take a closer look */

					if(Examine(FileLock,FileInfo))
					{
							/* Is this a file marked as being executable? */

						if(FileInfo->fib_DirEntryType < 0)
						{
							if(!(FileInfo->fib_Protection & FIBF_EXECUTE))
							{
								BPTR FileHandle;
								ULONG Value;

									/* Look at the contents */

								if(FileHandle = Open(Program,MODE_OLDFILE))
								{
									if(Read(FileHandle,&Value,sizeof(ULONG)) == sizeof(ULONG))
									{
											/* Not a failproof check, but better than nothing */

										if(Value == HUNK_HEADER)
											GotIt = TRUE;
									}

									Close(FileHandle);
								}
							}

							if(!GotIt)
							{
									/* Does it look like a script? */

								if(FileInfo->fib_Protection & FIBF_SCRIPT)
									GotIt = TRUE;
								else
								{
									BPTR FileHandle;

										/* Now check if it's an ARexx script. */

									if(FileHandle = Open(Program,MODE_OLDFILE))
									{
										UBYTE LocalBuffer[256];
										LONG Len;

										if((Len = Read(FileHandle,LocalBuffer,sizeof(LocalBuffer))) > 0)
										{
											if(CanExecute = FindARexxComment(LocalBuffer,Len))
												GotIt = TRUE;
										}

										Close(FileHandle);
									}
								}
							}
						}
					}

					UnLock(FileLock);
				}
			}

			if(!GotIt)
			{
				BPTR CommandDir;

				if(CommandDir = Lock("C:",ACCESS_READ))
				{
					Drawer = CurrentDir(CommandDir);

					if(InitialCD)
					{
						StartCD		= Drawer;
						InitialCD	= FALSE;
					}

						/* Try to lock the program */

					if(FileLock = Lock(Program,ACCESS_READ))
					{
						D_S(struct FileInfoBlock,FileInfo);

							/* Take a closer look */

						if(Examine(FileLock,FileInfo))
						{
								/* Is this a file marked as being executable? */

							if(FileInfo->fib_DirEntryType < 0)
							{
								if(!(FileInfo->fib_Protection & FIBF_EXECUTE))
								{
									BPTR FileHandle;
									ULONG Value;

										/* Look at the contents */

									if(FileHandle = Open(Program,MODE_OLDFILE))
									{
										if(Read(FileHandle,&Value,sizeof(ULONG)) == sizeof(ULONG))
										{
												/* Not a failproof check, but better than nothing */

											if(Value == HUNK_HEADER)
												GotIt = TRUE;
										}

										Close(FileHandle);
									}
								}

								if(!GotIt)
								{
										/* Does it look like a script? */

									if(FileInfo->fib_Protection & FIBF_SCRIPT)
										GotIt = TRUE;
									else
									{
										BPTR FileHandle;

											/* Now check if it's an ARexx script. */

										if(FileHandle = Open(Program,MODE_OLDFILE))
										{
											UBYTE LocalBuffer[256];
											LONG Len;

											if((Len = Read(FileHandle,LocalBuffer,sizeof(LocalBuffer))) > 0)
											{
												if(CanExecute = FindARexxComment(LocalBuffer,Len))
													GotIt = TRUE;
											}

											Close(FileHandle);
										}
									}
								}
							}
						}

						UnLock(FileLock);
					}

					UnLock(CommandDir);
				}
			}

			if(!InitialCD)
				CurrentDir(StartCD);

			if(GotIt)
			{
				CanExecute = TRUE;

				*Error = 0;

				Result = TRUE;
			}
		}
	}

		/* As a last resort, scan the resident program list */

	if(!Result)
	{
		Forbid();

		if(FindSegment(Program,NULL,DOSFALSE) || FindSegment(Program,NULL,DOSTRUE))
		{
			CanExecute = TRUE;

			*Error = 0;

			Result = TRUE;
		}

		Permit();
	}

		/* Last check for ARexx script. */

	if(!Result)
	{
		UBYTE RexxName[MAX_FILENAME_LENGTH];

		strcpy(RexxName,"REXX:");

		if(AddPart(RexxName,Program,sizeof(RexxName)))
		{
			BPTR FileHandle;

				/* Now check if it's an ARexx script. */

			if(FileHandle = Open(RexxName,MODE_OLDFILE))
			{
				UBYTE LocalBuffer[256];
				LONG Len;

				if((Len = Read(FileHandle,LocalBuffer,sizeof(LocalBuffer))) > 0)
				{
					if(CanExecute = FindARexxComment(LocalBuffer,Len))
						Result = TRUE;
				}

				Close(FileHandle);
			}
		}
	}

		/* Last check for AmigaDOS script */

	if(!Result)
	{
		UBYTE ScriptName[MAX_FILENAME_LENGTH];

		strcpy(ScriptName,"S:");

		if(AddPart(ScriptName,Program,sizeof(ScriptName)))
		{
			if(FileLock = Lock(ScriptName,ACCESS_READ))
			{
				D_S(struct FileInfoBlock,FileInfo);

				if(Examine(FileLock,FileInfo))
				{
					if(FileInfo->fib_DirEntryType < 0 && (FileInfo->fib_Protection & FIBF_SCRIPT) && FileInfo->fib_Size > 0)
						CanExecute = Result = TRUE;
				}

				UnLock(FileLock);
			}
		}
	}

	if(!CanExecute && Result)
	{
		*Error = ERROR_NOT_EXECUTABLE;

		Result = FALSE;
	}

	EnableDOSRequesters(OldPtr);

	if(Window && !Result && *Error)
	{
		LT_LockWindow(Parent);

		if(*Error == ERR_NOT_A_FILE)
			ShowError(Parent,*Error,0,Program);
		else
			ShowError(Parent,ERR_PROGRAM_NOT_FOUND,*Error,Program);

		LT_UnlockWindow(Parent);
	}

	return(Result);
}

	/* FindLibDev(struct Window *Parent,STRPTR File,LONG Type,LONG *Error):
	 *
	 *	Try to locate a file.
	 */

BOOL
FindLibDev(struct Window *Parent,STRPTR File,LONG Type,LONG *Error)
{
	UBYTE LocalBuffer[MAX_FILENAME_LENGTH];
	BOOL Result,Replaced;
	STRPTR OriginalFile = NULL;
	struct Node	*Node;
	LONG LocalError;
	BPTR FileLock;
	APTR OldPtr;
	BOOL NotDone;

	if(!Config->MiscConfig->ProtectiveMode)
		return(TRUE);

	Result		= FALSE;
	Replaced	= FALSE;

	if(!Error)
		Error = &LocalError;

	Forbid();

	if(Type == NT_LIBRARY)
		Node = FindName(&SysBase->LibList,PathPart(File));
	else
		Node = FindName(&SysBase->DeviceList,PathPart(File));

	Permit();

	if(Node)
	{
		*Error = 0;

		return(TRUE);
	}

		/* Lock out DOS requesters */

	DisableDOSRequesters(&OldPtr);

	do
	{
		NotDone = FALSE;

			/* Try to get a lock on the file */

		if(FileLock = Lock(File,ACCESS_READ))
		{
			D_S(struct FileInfoBlock,FileInfo);

				/* Take a closer look */

			if(Examine(FileLock,FileInfo))
			{
					/* Is it actually a drawer? */

				if(FileInfo->fib_DirEntryType > 0)
					*Error = ERR_NOT_A_FILE;
				else
				{
					if(ValidateFile(File,Type,NULL))
					{
						*Error = 0;

						Result = TRUE;
					}
					else
						*Error = ERROR_OBJECT_WRONG_TYPE;
				}
			}
			else
				*Error = IoErr();

			UnLock(FileLock);
		}
		else
			*Error = IoErr();

		if(!Result && !Replaced)
		{
			Replaced = TRUE;

			strcpy(LocalBuffer,Type == NT_LIBRARY ? "Libs:" : "Devs:");

			OriginalFile = File;

			if(AddPart(LocalBuffer,File,sizeof(LocalBuffer)))
			{
				File = LocalBuffer;

				NotDone = TRUE;
			}
		}
	}
	while(NotDone);

	if(Replaced)
		File = OriginalFile;

	EnableDOSRequesters(OldPtr);

	if(Window && !Result && *Error)
	{
		LT_LockWindow(Parent);

		if(*Error == ERR_NOT_A_FILE)
			ShowError(Parent,*Error,0,File);
		else
			ShowError(Parent,ERR_FILE_NOT_FOUND,*Error,File);

		LT_UnlockWindow(Parent);
	}

	return(Result);
}

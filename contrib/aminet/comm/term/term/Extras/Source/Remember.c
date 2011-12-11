/*
**	Remember.c
**
**	Login learn routines
**
**	Copyright © 1990-1996 by Olaf `Olsen' Barthel
**		All Rights Reserved
**
**	:ts=4
*/

#ifndef _GLOBAL_H
#include "Global.h"
#endif

	/* How many characters to remember.*/

#define REMEMBER_OUT	40
#define REMEMBER_IN		256

	/* The size of the aux buffer.*/

#define RECORD_AUX_BUFFER_LENGTH 1024

	/* Yet another node variant.*/

struct RecordNode
{
	struct Node		Node;
	struct timeval	Time;
};

	/* What type of text was recorded.*/

enum	{	RECORD_OUTPUT,RECORD_INPUT };

	/* The last ten bytes received.*/

STATIC UBYTE			 RememberOutputData[2 * REMEMBER_OUT + 10];
STATIC WORD				 RememberOutputSize;

	/* The last 256 bytes sent.*/

STATIC UBYTE			 RememberInputData[2 * REMEMBER_IN + 10];
STATIC WORD				 RememberInputSize;

	/* The name of the BBS we are currently recording for.*/

STATIC UBYTE			 RecordName[50];

	/* Recorded lines.*/

STATIC struct List		*RecordList;
STATIC LONG				 RecordCount;

	/* Auxilary data.*/

STATIC STRPTR			 RecordAuxBuffer;

	/* Start of recording.*/

STATIC struct timeval	 RecordTime;

	/* FinishRecord():
	 *
	 *	Finish the recording process, store the
	 *	recorded data in a file.
	 */

VOID
FinishRecord()
{
	if(RecordCount)
	{
		UBYTE					 DummyBuffer[MAX_FILENAME_LENGTH];
		struct FileRequester	*FileRequest;

		BlockWindows();

		if(RecordName[0])
			strcpy(DummyBuffer,RecordName);
		else
		{
			strcpy(DummyBuffer,LocaleString(MSG_SCREENPANEL_UNKNOWN_TXT));

			strcpy(RecordName,DummyBuffer);
		}

		FixName(DummyBuffer);

		DummyBuffer[27] = 0;

		LimitedStrcat(sizeof(DummyBuffer),DummyBuffer,".term");

		if(FileRequest = SaveFile(Window,LocaleString(MSG_SAVE_RECORDED_FILE_TXT),NULL,"#?.(term|rexx)",DummyBuffer,sizeof(DummyBuffer)))
		{
			BPTR			FileHandle;
			LONG			Error = 0;
			struct timeval	Delta = { };

			FreeAslRequest(FileRequest);

			if(FileHandle = Open(DummyBuffer,MODE_NEWFILE))
			{
				PhoneEntry *ActiveEntry;

				SetIoErr(0);

				if(FPrintf(FileHandle,LocaleString(MSG_RECORDFILE_HEADER_TXT),RecordName) < 1)
					Error = IoErr();

				if(!Error)
				{
					struct RecordNode *Node;
					BOOL UsesSpace,UsesQuotes;
					STRPTR Header;
					LONG i,Len;

					for(Node = (struct RecordNode *)RecordList->lh_Head ; Node->Node.ln_Succ && !Error ; Node = (struct RecordNode *)Node->Node.ln_Succ)
					{
						if(Delta.tv_secs != Node->Time.tv_secs)
						{
							LONG Secs;

							Secs = (3 * Node->Time.tv_secs) / 2 + (3 * Node->Time.tv_micro) / 2000000;

							SetIoErr(0);

							if(FPrintf(FileHandle,"TIMEOUT %ld\n",Secs) < 1)
							{
								Error = IoErr();

								break;
							}
							else
								Delta = Node->Time;
						}

						SetIoErr(0);

							/* Now things get a little difficult.As the ARexx */
							/* parser is bound to get into trouble with trailing */
							/* and leading spaces and `trouble' will appear like */
							/* a picnic if a double quote shows up in the text, */
							/* we need to find out about these exceptions. */

						Len = strlen(Node->Node.ln_Name);

						for(i = 0, UsesSpace = UsesQuotes = FALSE ; i < Len ; i++)
						{
							if(Node->Node.ln_Name[i] == ' ' && (i == 0 || i == Len - 1))
							{
								UsesSpace = TRUE;

								if(UsesQuotes)
									break;
							}

							if(Node->Node.ln_Name[i] == '\"')
							{
								UsesQuotes = TRUE;

								if(UsesSpace)
									break;
							}
						}

						if(Node->Node.ln_Type == RECORD_OUTPUT)
							Header = "WAIT";
						else
							Header = "SEND";

						if(FPrintf(FileHandle,"%s TEXT \"",Header) < 1)
							Error = IoErr();
						else
						{
							if(UsesSpace || UsesQuotes)
							{
								STRPTR From,String,First,Last;
								UBYTE c;

								From = String = (STRPTR)Node->Node.ln_Name;

								First = String;
								Last = String + Len - 1;

								c = 0;

								while(*String)
								{
									switch(*String)
									{
										case ' ':

											if(String == First || String == Last)
												c = ' ';

											break;

										case '\"':

											c = '\"';
											break;
									}

									if(c)
									{
										STRPTR Code;

										if(From < String)
										{
											LONG Len = (LONG)(String - From);

											if(FWrite(FileHandle,From,Len,1) != 1)
											{
												Error = IoErr();
												break;
											}
										}

										if(c == ' ')
											Code = "\\*32";
										else
											Code = "\\*34";

										if(FPrintf(FileHandle,Code) < 1)
										{
											Error = IoErr();
											break;
										}

										From = String + 1;
										c = 0;
									}

									String++;
								}

								if(!Error && From < String)
								{
									LONG Len = (LONG)(String - From);

									if(FWrite(FileHandle,From,Len,1) != 1)
										Error = IoErr();
								}
							}
							else
							{
								if(FPrintf(FileHandle,Node->Node.ln_Name) < 1)
									Error = IoErr();
							}

							if(!Error)
							{
								if(FPrintf(FileHandle,"\"\n") < 1)
									Error = IoErr();
							}
						}
					}
				}

				Close(FileHandle);

				if(ActiveEntry = LockActiveEntry(GlobalPhoneHandle))
				{
					if(ShowRequest(Window,LocaleString(MSG_USE_AS_LOGIN_SCRIPT_TXT),LocaleString(MSG_GLOBAL_YES_NO_TXT),ActiveEntry->Header->Name))
					{
						if(!ActiveEntry->Config->CommandConfig)
						{
							if(!CreateConfigEntry(ActiveEntry->Config,PREF_COMMAND))
								ShowError(Window,ERROR_NO_FREE_STORE,0,NULL);
						}

						if(ActiveEntry->Config->CommandConfig)
						{
							PhonebookChanged = TRUE;

							LimitedSPrintf(sizeof(ActiveEntry->Config->CommandConfig->LoginMacro),ActiveEntry->Config->CommandConfig->LoginMacro,"\\a %s",DummyBuffer);
						}
					}

					UnlockActiveEntry(GlobalPhoneHandle);
				}
			}
			else
				Error = IoErr();

			if(!Error && Config->MiscConfig->CreateIcons && IconBase)
			{
				struct DiskObject *Icon;

				if(!(Icon = GetDiskObject("ENV:def_term")))
				{
					if(!(Icon = GetDiskObject("ENV:def_rexx")))
					{
						if(Icon = GetDefDiskObject(WBPROJECT))
							Icon->do_DefaultTool = "RX";
					}
				}

				if(Icon)
				{
					Icon->do_CurrentX = Icon->do_CurrentY = NO_ICON_POSITION;

					PutDiskObject(DummyBuffer,Icon);

					FreeDiskObject(Icon);
				}
			}

			if(Error)
				ShowError(Window,ERR_SAVE_ERROR,Error,DummyBuffer);
		}

		DeleteRecord();

		ReleaseWindows();
	}
}

	/* CreateRecord(STRPTR BBSName):
	 *
	 *	Start recording incoming and outgoing text.
	 */

BOOL
CreateRecord(CONST_STRPTR BBSName)
{
	DeleteRecord();

	if(RecordList = CreateList())
	{
		if(RecordAuxBuffer = (STRPTR)AllocVecPooled(RECORD_AUX_BUFFER_LENGTH,MEMF_ANY))
		{
			strcpy(RecordName,BBSName);

			GetSysTime(&RecordTime);

			return(TRUE);
		}

		FreeVecPooled(RecordList);

		RecordList = NULL;
	}

	return(FALSE);
}

	/* DeleteRecord():
	 *
	 *	Stop recording, free auxilary buffers.
	 */

VOID
DeleteRecord()
{
	FreeVecPooled(RecordAuxBuffer);
	RecordAuxBuffer = NULL;

	DeleteList(RecordList);
	RecordList = NULL;

	RecordCount = 0;
}

	/* RememberResetOutput():
	 *
	 *	Reset output monitoring.
	 */

VOID
RememberResetOutput()
{
	RememberOutputSize = 0;
}

	/* RememberOutputText(STRPTR String,LONG Size):
	 *
	 *	Remember the last ten characters received.
	 */

VOID
RememberOutputText(STRPTR String,LONG Size)
{
	if(Size >= REMEMBER_OUT)
	{
		CopyMem(String + Size - REMEMBER_OUT,RememberOutputData,REMEMBER_OUT);

		RememberOutputSize = REMEMBER_OUT;
	}
	else
	{
		if(RememberOutputSize + Size > REMEMBER_OUT)
		{
			LONG Delta = RememberOutputSize + Size - REMEMBER_OUT,i;

			for(i = 0 ; i < RememberOutputSize - Delta ; i++)
				RememberOutputData[i] = RememberOutputData[Delta + i];

			while(i < REMEMBER_OUT)
				RememberOutputData[i++] = *String++;

			RememberOutputSize = REMEMBER_OUT;
		}
		else
		{
			CopyMem(String,&RememberOutputData[RememberOutputSize],Size);

			RememberOutputSize += Size;
		}
	}
}

	/* RememberResetInput():
	 *
	 *	Reset input monitoring.
	 */

VOID
RememberResetInput()
{
	RememberInputSize = 0;
}

	/* RememberInputText(STRPTR String,LONG Size):
	 *
	 *	Remember the last 256 characters sent.
	 */

VOID
RememberInputText(STRPTR String,LONG Size)
{
	if(Size >= REMEMBER_IN)
	{
		CopyMem(String + Size - REMEMBER_IN,RememberInputData,REMEMBER_IN);

		RememberInputSize = REMEMBER_IN;
	}
	else
	{
		if(RememberInputSize + Size > REMEMBER_IN)
		{
			LONG Delta = RememberInputSize + Size - REMEMBER_IN,i;

			for(i = 0 ; i < RememberInputSize - Delta ; i++)
				RememberInputData[i] = RememberInputData[Delta + i];

			while(i < REMEMBER_IN)
				RememberInputData[i++] = *String++;

			RememberInputSize = REMEMBER_IN;
		}
		else
		{
			CopyMem(String,&RememberInputData[RememberInputSize],Size);

			RememberInputSize += Size;
		}
	}
}

	/* RememberSpill():
	 *
	 *	`Spill' the collected text, put it into the list.
	 */

VOID
RememberSpill()
{
	if(RecordList)
	{
		struct RecordNode	*Node;
		struct timeval		 Delta,Backup;

			/* Calculate the difference in time
			 * between the last line stored and this
			 * line getting stored.
			 */

		GetSysTime(&Backup);

		Delta = Backup;
		SubTime(&Delta,&RecordTime);

		RecordTime = Backup;

		if(!Delta.tv_secs)
			Delta.tv_secs = 1;

		Delta.tv_micro = 0;

		if(RememberOutputSize)
		{
			TranslateBack(RememberOutputData,RememberOutputSize,RecordAuxBuffer,RECORD_AUX_BUFFER_LENGTH);

			if(Node = (struct RecordNode *)AllocVecPooled(sizeof(struct RecordNode) + strlen(RecordAuxBuffer) + 1,MEMF_ANY))
			{
				Node->Node.ln_Name = (STRPTR)(Node + 1);
				Node->Node.ln_Type = RECORD_OUTPUT;

				strcpy(Node->Node.ln_Name,RecordAuxBuffer);

				Node->Time = Delta;

				AddTail(RecordList,(struct Node *)Node);

				RecordCount++;
			}
		}

		if(RememberInputSize)
		{
			TranslateBack(RememberInputData,RememberInputSize,RecordAuxBuffer,RECORD_AUX_BUFFER_LENGTH);

			if(Node = (struct RecordNode *)AllocVecPooled(sizeof(struct RecordNode) + strlen(RecordAuxBuffer) + 1,MEMF_ANY))
			{
				Node->Node.ln_Name = (STRPTR)(Node + 1);
				Node->Node.ln_Type = RECORD_INPUT;

				strcpy(Node->Node.ln_Name,RecordAuxBuffer);

				Node->Time = Delta;

				AddTail(RecordList,(struct Node *)Node);

				RecordCount++;
			}
		}
	}

	RememberOutputSize = RememberInputSize = 0;
}

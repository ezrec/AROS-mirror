/*********************************************************************
----------------------------------------------------------------------

	Mystic_HandlerWrapper
	
	this module wraps calls to the
	old, buggy background pichandler (v0.99f) to the new,
	more advanced pic handler (v0.99g).

----------------------------------------------------------------------
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <utility/tagitem.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <libraries/cybergraphics.h>
#include <intuition/intuition.h>
#include <exec/memory.h>
#include <dos/dos.h>

#include <assert.h>

#include <clib/macros.h>

#include <proto/asl.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/guigfx.h>
#include <proto/cybergraphics.h>
#include <proto/wb.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/icon.h>

#include "Mystic_Handler.h"
#include "Mystic_Texts.h"
#include "Mystic_HandlerWrapper.h"




/*
**	status = getpichandlerstatus(pichandler)
**
*/

int GetPicHandlerStatus(struct pichandler *ph)
{
	int status = PICHSTATUS_IDLE;
	if (ph)
	{
		status = ph->status;
	}
	
	return status;
}



/*
**	appendpichandler(pichandler, filepatterns)
**
*/

BOOL AppendPicHandler(struct pichandler *ph, char **filepatterns)
{
	BOOL success = FALSE;

	if (ph && filepatterns)
	{
		success = PicHandler_AddPatternList(ph->newPicHandler, filepatterns, NULL);
	}
	
	return success;
}


/*
**	createpichandler(filepatterns, tags)
**
*/

struct pichandler * STDARGS CreatePicHandler(char **filepatternlist,
	BOOL asyncscanning,
	...)
{
	struct pichandler *wrappedpichandler;
	va_list va;
	struct TagItem *tags;
	BOOL recursive;
	int sortmode;
	char *rejectpattern;
	BOOL reverse;
	int bufferpercent;
	BOOL autocrop;
	BOOL simplescanning;
	BOOL includedirs;
	
	va_start(va, asyncscanning);
#ifdef __MORPHOS__
	tags = (struct TagItem *) va->overflow_arg_area;
#else
	tags = (struct TagItem *) va;
#endif

	if (wrappedpichandler = Malloc(sizeof(struct pichandler)))
	{
		BOOL success = FALSE;

		wrappedpichandler->newPicHandler = NULL;
		wrappedpichandler->lockedID = PICID_UNDEFINED;
		wrappedpichandler->requestedID = PICID_UNDEFINED;
		wrappedpichandler->pic = NULL;
	//	wrappedpichandler->picname = NULL;
		wrappedpichandler->picinfo = NULL;
		wrappedpichandler->atliststart = FALSE;
		wrappedpichandler->asyncscanning = asyncscanning;
		wrappedpichandler->waitforfirstpicture = TRUE;
		wrappedpichandler->status = PICHSTATUS_IDLE;
		wrappedpichandler->errortext = NULL;
		wrappedpichandler->error = 0;
		

		recursive = (BOOL) GetTagData(PICH_Recursive, TRUE, tags);
		simplescanning = (BOOL) GetTagData(PICH_SimpleScanning, FALSE, tags);
		sortmode = (int) GetTagData(PICH_SortMode, SORTMODE_NONE, tags);
		rejectpattern = (char *) GetTagData(PICH_Reject, NULL, tags);
		reverse = (BOOL) GetTagData(PICH_SortReverse, FALSE, tags);
		bufferpercent = GetTagData(PICH_BufferPercent, 10, tags);
		autocrop = GetTagData(PICH_AutoCrop, FALSE, tags);
		includedirs = GetTagData(PICH_IncludeDirs, FALSE, tags);
		
		if (wrappedpichandler->newPicHandler = 
				PicHandler_Create(NULL,
					PICH_Recursive, recursive,
					PICH_SortMode, sortmode,
					PICH_SortReverse, reverse,
					PICH_Reject, rejectpattern, 
					PICH_BufferPercent, bufferpercent,
					PICH_SimpleScanning, simplescanning,
					PICH_IncludeDirs, includedirs,
					PICH_Notify, 
						PHNOTIFY_LOADING + 
						PHNOTIFY_PIC_AVAILABLE + 
						PHNOTIFY_SCANNED_NUMBER + 
						PHNOTIFY_SCANNING +
						PHNOTIFY_ID_ERROR_LOADING,
					PICH_AutoCrop, autocrop,
					TAG_DONE))
		{
			success = TRUE;
			if (filepatternlist)
			{
				success = PicHandler_AddPatternList(wrappedpichandler->newPicHandler,
						filepatternlist, TAG_DONE);
			}
		}

		if (!success)
		{		
			Free(wrappedpichandler);
			wrappedpichandler = NULL;
		}
	}

	return wrappedpichandler;
}


/*
**	setsortmode(pichandler)
**
*/

void SetSortMode(struct pichandler *pichandler, ULONG sortmode, BOOL reverse)
{
	if (pichandler)
	{
		PicHandler_SetAttributes(pichandler->newPicHandler,
			PICH_SortMode, sortmode, 
			PICH_SortReverse, reverse,
			TAG_DONE);
	}
}


/*
**	deletepichandler(pichandler)
**
*/

void DeletePicHandler(struct pichandler *pichandler)
{
	if (pichandler)
	{
		if (pichandler->lockedID != PICID_UNDEFINED)
		{
			PicHandler_ReleasePicture(pichandler->newPicHandler, pichandler->lockedID);
			pichandler->pic = NULL;
		}

		Free(pichandler->errortext);

		PicHandler_FreePicInfo(pichandler->picinfo);

	//	Free(pichandler->picname);

		PicHandler_Delete((struct PicHandler *) pichandler->newPicHandler);
		Free(pichandler);
	}
}



/*
**	newpicsignal(pichandler)
*/

ULONG NewPicSignal(struct pichandler *pichandler)
{
	if (pichandler)
	{
		return PicHandler_GetSigMask(pichandler->newPicHandler);
	}
	else
	{
		return 0;
	}
}


/*
**	name = getpicfile(pichandler)
*/

char *GetPicFile(struct pichandler *ph)
{
	char *fullname = NULL;
	
	if (ph)
	{
		if (ph->picinfo)
		{
			assert(ph->pic);
			fullname = _StrDup(ph->picinfo->fullname);
		}
	}
	
	return fullname;
}


/*
**	name = getpicformat(pichandler)
*/

char *GetPicFormat(struct pichandler *ph)
{
	char *fullname = NULL;
	
	if (ph)
	{
		if (ph->picinfo)
		{
			assert(ph->pic);
			fullname = _StrDup(ph->picinfo->formatname);
		}
	}
	
	return fullname;
}


/*
**	isdir = getpicfiletype(pichandler)
*/

BOOL GetPicFileType(struct pichandler *ph)
{
	BOOL isdir = FALSE;
	
	if (ph)
	{
		if (ph->picinfo)
		{
			assert(ph->pic);
			isdir = ph->picinfo->isdirectory;
		}
	}
	
	return isdir;
}



/*
**	pic = getpic(pichandler)
**
*/

PICTURE *GetPic(struct pichandler *handler)
{
	PICTURE *pic = NULL;

	if (handler)
	{
		if (pic = handler->pic)
		{
			assert(handler->picinfo);
		}		
	}
	
	return pic;
}




/*
**	nextpic(pichandler)
**
*/

BOOL NextPic(struct pichandler *handler)
{
	BOOL result = FALSE;

	if (handler)
	{
		ULONG numpix;
		
		numpix = PicHandler_GetNumber(handler->newPicHandler);
		
		if (numpix > 0)
		{
			ULONG requestID, currentID;

			handler->requestedID = PICID_UNDEFINED;


			PicHandler_LockFileList(handler->newPicHandler);
			
			currentID = PicHandler_GetCurrentID(handler->newPicHandler);
			requestID = PicHandler_Seek(handler->newPicHandler, PHLIST_RELATIVE, 1);
						
			if ((requestID != PICID_UNDEFINED && requestID == currentID) || handler->atliststart)
			{
				requestID = PicHandler_Seek(handler->newPicHandler, PHLIST_START, 0);
				currentID = PICID_UNDEFINED;
				handler->atliststart = FALSE;
			}

			PicHandler_UnLockFileList(handler->newPicHandler);

			
			if (requestID != PICID_UNDEFINED && requestID != currentID)
			{
				if (result = PicHandler_RequestPic(handler->newPicHandler, requestID))
				{
					handler->requestedID = requestID;
				}
			}
			
		}
	}

	return result;
}



/*
**	currentpic(pichandler)
**
*/

BOOL CurrentPic(struct pichandler *handler)
{
	BOOL result = FALSE;

	if (handler)
	{
		ULONG numpix;
		
		numpix = PicHandler_GetNumber(handler->newPicHandler);
		
		if (numpix > 0)
		{
			ULONG requestID;

			handler->requestedID = PICID_UNDEFINED;

			PicHandler_LockFileList(handler->newPicHandler);
			
			requestID = PicHandler_GetCurrentID(handler->newPicHandler);
						
			if (requestID == PICID_UNDEFINED)
			{
				requestID = PicHandler_Seek(handler->newPicHandler, PHLIST_START, 0);
				handler->atliststart = FALSE;
			}

			PicHandler_UnLockFileList(handler->newPicHandler);

			
			if (requestID != PICID_UNDEFINED)
			{
				if (result = PicHandler_RequestPic(handler->newPicHandler, requestID))
				{
					handler->requestedID = requestID;
				}
			}
			
		}
	}

	return result;
}



/*
**	prevpic(pichandler)
**
*/

BOOL PrevPic(struct pichandler *handler)
{
	BOOL result = FALSE;

	if (handler)
	{
		ULONG numpix;
		
		numpix = PicHandler_GetNumber(handler->newPicHandler);
		
		if (numpix > 0)
		{
			ULONG requestID, currentID;

			handler->requestedID = PICID_UNDEFINED;


			PicHandler_LockFileList(handler->newPicHandler);
			
			currentID = PicHandler_GetCurrentID(handler->newPicHandler);
			requestID = PicHandler_Seek(handler->newPicHandler, PHLIST_RELATIVE, -1);
			
			if (requestID != PICID_UNDEFINED)
			{
				if (requestID == currentID)
				{
					requestID = PicHandler_Seek(handler->newPicHandler, PHLIST_END, 0);
					handler->atliststart = FALSE;
				}
			}

			PicHandler_UnLockFileList(handler->newPicHandler);

						
			if (requestID != PICID_UNDEFINED)
			{
				if (PicHandler_RequestPic(handler->newPicHandler, requestID))
				{
					if (requestID != handler->lockedID)
					{
						handler->requestedID = requestID;
						result = TRUE;
					}
				}
			}
		}
	}

	return result;
}






#if 0
/*
**	lockpichandler(handler)
**	unlockpichandler(handler)
*/

void LockPicHandler(struct pichandler *pichandler)
{
}
void UnLockPicHandler(struct pichandler *pichandler)
{
}
#endif



/*
**	n = gettotalpicnumber(handler)
*/

int GetTotalPicNumber(struct pichandler *pichandler)
{
	int n = 0;

	if (pichandler)
	{
		n = PicHandler_GetNumber(pichandler->newPicHandler);
	}
	
	return n;
}



/*
**	n = getcurrentpicnumber(handler)
*/

int GetCurrentPicNumber(struct pichandler *pichandler)
{
	int n = -1;

	if (pichandler)
	{
		if (!pichandler->atliststart)
		{
			ULONG id = PicHandler_GetCurrentID(pichandler->newPicHandler);
			
			if (id != PICID_UNDEFINED)
			{
				n = PicHandler_GetListIndex(pichandler->newPicHandler, id);
			}
		}
	}
	
	return n;
}



/*
**	BOOL = atliststart(handler)
*/

BOOL AtListStart(struct pichandler *pichandler)
{
	if (pichandler)
	{
		return pichandler->atliststart;
	}
	return FALSE;
}


/*
**	n = getnextpicnumber(handler)
*/

int GetNextPicNumber(struct pichandler *pichandler)
{
	int n = -1;

	if (pichandler)
	{
		ULONG currentID, nextID;


		PicHandler_LockFileList(pichandler->newPicHandler);

		nextID = PicHandler_GetID(pichandler->newPicHandler, PHLIST_RELATIVE, 1);
		currentID = PicHandler_GetCurrentID(pichandler->newPicHandler);

		PicHandler_UnLockFileList(pichandler->newPicHandler);
	
		
		if (nextID != PICID_UNDEFINED)
		{
			if (currentID == nextID)
			{
				n = 0;
			}
			else
			{
				n = nextID;
			}
		}
	}
	
	return n;
}





/*
**	ExpungePicHandler(handler)
*/

void ExpungePicHandler(struct pichandler *handler)
{
	if (handler)
	{
		if (handler->lockedID != PICID_UNDEFINED)
		{
			PicHandler_ReleasePicture(handler->newPicHandler, handler->lockedID);
			handler->pic = NULL;

		//	Free(handler->picname);
		//	handler->picname = NULL;
			
			PicHandler_FreePicInfo(handler->picinfo);
			handler->picinfo = NULL;
			
			Free(handler->errortext);
			handler->errortext = NULL;

			handler->lockedID = PICID_UNDEFINED;
			handler->requestedID = PICID_UNDEFINED;
		}
		else
		{
			assert(handler->pic == NULL);
			assert(handler->picinfo == NULL);
		}
	}
}


/*
**	ResetPicHandler(handler)
*/

void ResetPicHandler(struct pichandler *pichandler)
{
	if (pichandler)
	{
		PicHandler_Seek(pichandler->newPicHandler, PHLIST_START, 0);
		pichandler->atliststart = TRUE;
	}
}



/*
**	ReloadPic(handler)
*/

BOOL ReloadPic(struct pichandler *handler)
{
	BOOL result = FALSE;

	if (handler)
	{
		ULONG requestID;

		requestID = PicHandler_GetCurrentID(handler->newPicHandler);
		if (requestID != PICID_UNDEFINED)
		{
			if (result = PicHandler_RequestPic(handler->newPicHandler, requestID))
			{
				handler->requestedID = requestID;
			}
		}
	}
	
	return result;
}





/*
**	GetPicError(handler)
*/

LONG GetPicError(struct pichandler *handler)
{
//	return ERROR_NO_FREE_STORE;
	return handler->error;
}



/*
**	BOOL NewPicAvailable(handler, LONG *error)
*/

BOOL NewPicAvailable(struct pichandler *ph, LONG *error)
{
	BOOL picavailable = FALSE;

	if (error) *error = 0;

	if (ph)
	{
		struct PHMessage *msg;
	
		while (msg = PicHandler_GetNotification(ph->newPicHandler))
		{
			switch (msg->msgtype)
			{
				case PHNOTIFY_LOADING:
					if (msg->loading)
					{
						ph->status |= PICHSTATUS_LOADING;
					}
					else
					{
						ph->status &= ~PICHSTATUS_LOADING;
					}
					break;
					

				case PHNOTIFY_ID_ERROR_LOADING:

					if (msg->id == ph->requestedID)
					{
						char errortext[200];
						char *picname;

						picname = PicHandler_GetName(ph->newPicHandler, ph->requestedID);

						if (picname)
						{						
							sprintf(errortext, MVTEXT_ERROR_LOADING, picname);
							Free(picname); 
						}
						else
						{
							sprintf(errortext, MVTEXT_ERROR_LOADING2);
						}

						ph->error = msg->error;
						Free(ph->errortext);
						ph->errortext = _StrDup(errortext);
						
				/////		printf("load error: %s\n", errortext);
					
						if (error) *error = msg->error;
					}
					break;
	
	
				case PHNOTIFY_SCANNING:
				case PHNOTIFY_SCANNED_NUMBER:

					if (msg->msgtype == PHNOTIFY_SCANNING)
					{
						if (msg->scanning)
						{
							ph->status |= PICHSTATUS_SCANNING;
						}
						else
						{
							ph->status &= ~PICHSTATUS_SCANNING;
						}
					}
					
					if (ph->waitforfirstpicture)
					{				
						if (ph->asyncscanning && msg->msgtype == PHNOTIFY_SCANNED_NUMBER)
						{
							if (PicHandler_GetNumber(ph->newPicHandler) > 0)
							{
								ULONG requestid;
								
								requestid = PicHandler_GetCurrentID(ph->newPicHandler);
								if (requestid != PICID_UNDEFINED)
								{
									if (PicHandler_RequestPic(ph->newPicHandler, requestid))
									{
										ph->requestedID = requestid;
									}
								}
								ph->waitforfirstpicture = FALSE;
							}						
						}
						else if (!ph->asyncscanning && msg->msgtype == PHNOTIFY_SCANNING)
						{
							if (msg->scanning == FALSE)
							{
								if (PicHandler_GetNumber(ph->newPicHandler) > 0)
								{
									ULONG requestid;
									
									PicHandler_Seek(ph->newPicHandler, PHLIST_START, 0);
									
									requestid = PicHandler_GetCurrentID(ph->newPicHandler);
									if (requestid != PICID_UNDEFINED)
									{
										if (PicHandler_RequestPic(ph->newPicHandler, requestid))
										{
											ph->requestedID = requestid;
										}
									}
								}						
								ph->waitforfirstpicture = FALSE;
							}
						}
					}
					
					break;

				case PHNOTIFY_PIC_AVAILABLE:
					if (ph->requestedID == msg->id)
					{
						PICINFO *newpicinfo;
						PICTURE *newpic;
					//	char *newpicname;
					//	newpicname = PicHandler_GetName(ph->newPicHandler, ph->requestedID);
						
						newpicinfo = PicHandler_GetPicInfo(ph->newPicHandler, ph->requestedID);
						newpic = PicHandler_ObtainPicture(ph->newPicHandler, ph->requestedID);

						if (newpic && newpicinfo)
						{
						//	Free(ph->picname);
						//	ph->picname = newpicname;
						//	newpicname = NULL;
						
							PicHandler_FreePicInfo(ph->picinfo);
							ph->picinfo = newpicinfo;
							newpicinfo = NULL;

							ph->error = 0;
							Free(ph->errortext);
							ph->errortext = NULL;

							ph->pic = newpic;
					
							if (ph->lockedID != PICID_UNDEFINED)
							{
								PicHandler_ReleasePicture(ph->newPicHandler, ph->lockedID);
							}
							
							ph->lockedID = ph->requestedID;
							
							picavailable = TRUE;
						}
						else
						{
							printf("error locking picture from pichandler! newpic: %lx, newpicinfo: %lx\n", newpic, newpicinfo);
						}

						PicHandler_FreePicInfo(newpicinfo);
					//	Free(newpicname);

						ph->requestedID = PICID_UNDEFINED;
					}
					break;

				default:
					break;
			}

			PicHandler_Acknowledge(ph->newPicHandler, msg);
		}
	
	}

	return picavailable;
}



/*
**	ReleasePic(pichandler)
**	release the current picture.
*/

void ReleasePic(struct pichandler *ph)
{
	if (ph)
	{
		PicHandler_FreePicInfo(ph->picinfo);
		ph->picinfo = NULL;		
	
	//	Free(ph->picname);
	//	ph->picname = NULL;

		ph->pic = NULL;
	
		if (ph->lockedID != PICID_UNDEFINED)
		{
			PicHandler_ReleasePicture(ph->newPicHandler, ph->lockedID);
		}
		
		ph->lockedID = PICID_UNDEFINED;
	}
}



/*
**	SetPicHandlerBuffer(pichandler, percent)
*/

void SetPicHandlerBuffer(struct pichandler *ph, int percent)
{
	if (ph)
	{
		PicHandler_SetAttributes(ph->newPicHandler,
			PICH_BufferPercent, percent,
			TAG_DONE);
	}
}


/*
**	SetPicHandlerAutoCrop(pichandler, bool)
*/

void SetPicHandlerAutoCrop(struct pichandler *ph, BOOL autocrop)
{
	if (ph)
	{
		PicHandler_SetAttributes(ph->newPicHandler,
			PICH_AutoCrop, autocrop,
			TAG_DONE);
	}
}


/*
**	success = RenamePicture(pichandler, newname)
*/

BOOL RenamePicture(struct pichandler *ph, char *newname)
{
	BOOL result = FALSE;

	if (ph && newname)
	{
		if (ph->lockedID != PICID_UNDEFINED)
		{
			result = PicHandler_Rename(ph->newPicHandler, ph->lockedID, newname);
		}
	}

	return result;
}


/*
**	success = RemovePicture(pichandler)
*/

BOOL RemovePicture(struct pichandler *ph)
{
	BOOL result = FALSE;

	if (ph)
	{
		if (ph->lockedID != PICID_UNDEFINED)
		{
			result = PicHandler_Remove(ph->newPicHandler, ph->lockedID);
		}
	}

	return result;
}


/*
**	list = PicHandlerCreateFileList(struct pichandler *handler)
*/


char **PicHandlerCreateFileList(struct pichandler *handler)
{
	return PicHandler_CreateFileList(handler->newPicHandler);
}
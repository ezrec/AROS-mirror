/*********************************************************************
----------------------------------------------------------------------

	MysticView
	filehandling

----------------------------------------------------------------------
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <utility/tagitem.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <libraries/cybergraphics.h>
#include <intuition/intuition.h>
#include <exec/memory.h>
#include <dos/dostags.h>

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

#include "Mystic_FileHandling.h"
#include "Mystic_Subtask.h"
#include "Mystic_Global.h"
#include "Mystic_Window.h"
#include "Mystic_Tools.h"
#include "Mystic_Texts.h"
#include "Mystic_Debug.h"

#include <assert.h>





char *FileReq(struct mview *mv, char *oldname, char *title)
{
	char *newname = NULL;
	
	if (oldname && title)
	{
		char *oldpathname = DupPath(oldname);
		char *oldfilename = _StrDup(FilePart(oldname));
		
		if (oldpathname && oldfilename)
		{
			char *newpathname = NULL, *newfilename = NULL;

			if (FileRequest(mv->destfreq, mv->window->window, title,
				oldpathname, oldfilename,
				&newpathname, &newfilename))
			{
				newname = FullName(newpathname, newfilename);
			}
			Free(newpathname);
			Free(newfilename);
		}
		
		Free(oldfilename);
		Free(oldpathname);
	}

	return newname;
}



static struct EasyStruct copyproblemreq =
{
	sizeof(struct EasyStruct), 0,

	MVTEXT_PROBLEM,
	MVREQ_ERROR_COPYING,
	MVREQ_RETRYCHANGECANCEL

};

static struct EasyStruct confirmoverwritereq =
{
	sizeof(struct EasyStruct), 0,

	MVTEXT_REQUEST,
	MVREQ_OVERWRITE,
	MVREQ_OVERWRITECHANGECANCEL
};


static struct EasyStruct errordeletereq =
{
	sizeof(struct EasyStruct), 0,
	MVTEXT_PROBLEM,
	MVREQ_ERROR_DELETING,
	MVREQ_RETRYCANCEL
};


static struct EasyStruct confirmdeletereq =
{
	sizeof(struct EasyStruct), 0,

	MVTEXT_REQUEST,
	MVREQ_SURE_DELETING,
	MVREQ_DELETECANCEL
};


LONG subfunc(APTR data)
{
	return SystemTags((char *) data, TAG_DONE);	
}


/*********************************************************************
----------------------------------------------------------------------

	success = PicDelete(struct mview *mv, char *name, int confirmlevel)

----------------------------------------------------------------------
*********************************************************************/

BOOL PicDelete(struct mview *mv, char *name, int confirmlevel)
{
	BOOL success = FALSE;

	if (name)
	{
		if (Exists(name))
		{
			BOOL delete = FALSE;
			if (confirmlevel != CONFIRM_NEVER)
			{
				int result = EasyRequest(mv->window->window, 
					&confirmdeletereq, NULL, 
					(IPTR)FilePart(name));
				if (result == 1)
				{
					delete = TRUE;
				}
			}
			else
			{
				delete = TRUE;
			}

			if (delete)
			{
				char *command;
				long error;
				BOOL retry;

				do
				{
					error = RETURN_FAIL;

					if ((command = Malloc(strlen(name) + 100)))
					{
						sprintf(command, "delete >NIL: <NIL: %c%s%c QUIET\n", 34,name,34);
						CallSubTask(subfunc, command, globalpriority, PICHANDLESTACK, &error);

						Free(command);
					}
			
					if (error == RETURN_OK)
					{
						if ((command = Malloc(strlen(name) + 100)))
						{
							sprintf(command, "delete >NIL: <NIL: %c%s.info%c QUIET\n", 34,name,34);
							CallSubTask(subfunc, command, globalpriority, PICHANDLESTACK, NULL);
							Free(command);
						}
						success = TRUE;
						retry = FALSE;
					}
					else
					{
						retry = (EasyRequest(mv->window->window, &errordeletereq, NULL, (IPTR)name) == 1);
					}
			
				} while (retry);
			}
		}
	}
	
	return success;
}



/*********************************************************************
----------------------------------------------------------------------

	success = PicCopy(struct mview *mv, char *sname, char *dname, int confirmlevel)

----------------------------------------------------------------------
*********************************************************************/

BOOL PicCopy(struct mview *mv, char *sname, char *dname, int confirmlevel)
{
	BOOL success = FALSE;

	if (sname && dname)
	{
		BOOL change = FALSE;
		char *command;
		char *destname = _StrDup(dname);

		while (destname)
		{
			long error = 0;
			
			if (Exists(destname))
			{
				if (confirmlevel != CONFIRM_NEVER)
				{
					int result = EasyRequest(mv->window->window, 
						&confirmoverwritereq, NULL, 
						(IPTR)FilePart(destname));

					if (result == 1)
					{
						change = FALSE;
					}
					else if (result == 2)
					{
						change = TRUE;
					}
					else
					{
						break;
					}
				}
			}

			if (change)
			{
				char *newname;
				change = FALSE;
				if ((newname = FileReq(mv, destname, MVTEXT_SELECTDESTINATION)))
				{
					Free(destname);
					destname = newname;
					continue;
				}
				else
				{
					break;
				}
			}
			
		
			if ((command = Malloc(strlen(sname) + strlen(destname) + 100)))
			{
				sprintf(command, "copy <NIL: >NIL: from %c%s%c to %c%s%c QUIET CLONE\n", 34,sname,34, 34,destname,34);
				CallSubTask(subfunc, command, globalpriority, PICHANDLESTACK, &error);
				Free(command);
			}

			if (error == RETURN_OK)
			{
				if ((command = Malloc(strlen(sname) + strlen(destname) + 100)))
				{
					sprintf(command, "copy <NIL: >NIL: from %c%s.info%c to %c%s.info%c QUIET CLONE\n", 34,sname,34, 34,destname,34);
					CallSubTask(subfunc, command, globalpriority, PICHANDLESTACK, NULL);
					Free(command);
				}

				success = TRUE;				
				break;
			}
			else
			{
				int result;

				result = EasyRequest(mv->window->window, &copyproblemreq, NULL, 
					(IPTR)sname, (IPTR)destname);
				
				if (result == 1)
				{
					continue;
				}
				else if (result == 2)
				{
					change = TRUE;
					continue;
				}
				else
				{
					break;
				}
			}
		}
		
		Free(destname);
	}

	return success;
}



/*********************************************************************
----------------------------------------------------------------------

	success = PicMove(struct mview *mv, char *sname, char *dname, int confirmlevel)

----------------------------------------------------------------------
*********************************************************************/

BOOL PicMove(struct mview *mv, char *sname, char *dname, int confirmlevel)
{
	BOOL success = FALSE;

	if (sname && dname)
	{
		BOOL change = FALSE;
		char *command;
		char *destname = _StrDup(dname);

		while (destname)
		{
			long error = 0;
			
			if (Exists(destname))
			{
				if (confirmlevel != CONFIRM_NEVER)
				{
					int result = EasyRequest(mv->window->window, 
						&confirmoverwritereq, NULL, 
						(IPTR)FilePart(destname));

					if (result == 1)
					{
						change = FALSE;
					}
					else if (result == 2)
					{
						change = TRUE;
					}
					else
					{
						break;
					}
				}
			}

			if (change)
			{
				char *newname;
				change = FALSE;
				if ((newname = FileReq(mv, destname, MVTEXT_SELECTDESTINATION)))
				{
					Free(destname);
					destname = newname;
					continue;
				}
				else
				{
					break;
				}
			}
			
		
			if ((command = Malloc(strlen(sname) + strlen(destname) + 100)))
			{
				sprintf(command, "copy <NIL: >NIL: from %c%s%c to %c%s%c QUIET CLONE\n", 34,sname,34, 34,destname,34);
				CallSubTask(subfunc, command, globalpriority, PICHANDLESTACK, &error);
				Free(command);
			}

			if (error == RETURN_OK)
			{
				if ((command = Malloc(strlen(sname) + strlen(destname) + 100)))
				{
					sprintf(command, "copy <NIL: >NIL: from %c%s.info%c to %c%s.info%c QUIET CLONE\n", 34,sname,34, 34,destname,34);
					CallSubTask(subfunc, command, globalpriority, PICHANDLESTACK, NULL);
					Free(command);
				}

				PicDelete(mv, sname, CONFIRM_NEVER);

				success = TRUE;				
				break;
			}
			else
			{
				int result;

				result = EasyRequest(mv->window->window, &copyproblemreq, NULL, 
					(IPTR)sname, (IPTR)destname);
				
				if (result == 1)
				{
					continue;
				}
				else if (result == 2)
				{
					change = TRUE;
					continue;
				}
				else
				{
					break;
				}
			}
		}
		
		Free(destname);
	}

	return success;
}



/*********************************************************************
----------------------------------------------------------------------

	success = PerformFileAction(fullsourcename, fulldestname, action, confirmlevel)

	copy/move a picture (via background task)

----------------------------------------------------------------------
*********************************************************************/

BOOL PerformFileAction(struct mview *mv, char *fullsourcename, char *fulldestname, int action, int confirmlevel)
{
	BOOL success = FALSE;

	switch (action)
	{
		case FILEACTION_COPY:
			if (fullsourcename && fulldestname)
			{
				if (!SameFile(fullsourcename, fulldestname))
				{
					success = PicCopy(mv, fullsourcename, fulldestname, confirmlevel);
				}
				else
				{
					EasyRequest(mv->window->window, &copysamefilereq, NULL, NULL);
				}
			}
			break;

		case FILEACTION_MOVE:
			if (fullsourcename && fulldestname)
			{
				if (!SameFile(fullsourcename, fulldestname))
				{
					success = PicMove(mv, fullsourcename, fulldestname, confirmlevel);
				}
				else
				{
					EasyRequest(mv->window->window, &renamesamefilereq, NULL, NULL);
				}
			}
			break;

		case FILEACTION_DELETE:

			success = PicDelete(mv, fulldestname, confirmlevel);
			break;
		
		default:
			break;
	}


	return success;
}


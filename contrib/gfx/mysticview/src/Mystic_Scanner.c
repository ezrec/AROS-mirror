/*********************************************************************
----------------------------------------------------------------------

	MysticView
	scanner

----------------------------------------------------------------------
*********************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <utility/tagitem.h>
#include <exec/memory.h>

#include <assert.h>

#include <clib/macros.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/guigfx.h>
#include <proto/dos.h>

#include "Mystic_Global.h"
#include "Mystic_Tools.h"
#include "Mystic_Debug.h"

#include "Mystic_Scanner.h"
#include "Mystic_Handler.h"


/*--------------------------------------------------------------------

	completed = Scan(filelist, scanlistnode, abortsignals, scanindex, pichandler)
	
--------------------------------------------------------------------*/

BOOL Scan(struct FileList *filelist, struct ScanListNode *snode, ULONG abortsignals, ULONG *scanindex, struct PicHandler *handler)
{
	BOOL complete = TRUE;
	char fullname[MAXPATHNAMELEN+MAXFILENAMELEN];
	struct AnchorPath ALIGNED anchor;
	long error;
	BPTR oldlock, curlock;
	char *argument;
	char *rejectpattern;
	


	assert(filelist && snode);



	#ifndef NDEBUG
	{
		DB(kprintf("*** scanner received scan job: "));
		DB(kprintf("%s\n", snode->filepattern));
	}
	#endif	



	//
	//	create pattern string for files to be rejected
	//

	if (snode->rejectpattern)
	{
		if (rejectpattern = Malloc(strlen(snode->rejectpattern)*3+2))
		{
			ParsePatternNoCase(snode->rejectpattern, rejectpattern, strlen(snode->rejectpattern)*3+2);
		}
	}
	else
	{
		rejectpattern = NULL;
	
	//	if (rejectpattern = Malloc(6))
	//	{
	//		ParsePatternNoCase("", rejectpattern, 5);
	//	}
	}



	//
	//	Scan
	//

	if (argument = snode->filepattern)
	{
		BOOL iswild = FALSE;
		char *tbuf;
		int l;

		memset(&anchor, 0, sizeof(struct AnchorPath));
		
		l = strlen(argument) * 2 + 2;
		if (tbuf = Malloc(l))
		{
			iswild = ParsePatternNoCase(argument, tbuf, l);
			Free(tbuf);
		}

		MatchFirst(argument, &anchor);


		while (!(error = IoErr()))
		{
			BOOL isdir = FALSE;

			//	abort signal?
			
			if (SetSignal(0, 0) & abortsignals)
			{
				DB(kprintf(">>>*** picscanner received abort signal!!!\n"));
				complete = FALSE;
				break;
			}


			//	enter directories
			
			if (anchor.ap_Info.fib_DirEntryType > 0)
			{
				isdir = TRUE;
				if (snode->recursive)
				{			
					if (!(anchor.ap_Flags & APF_DIDDIR))
					{
						anchor.ap_Flags |= APF_DODIR;
					}
					anchor.ap_Flags &= ~APF_DIDDIR;
				}
			}


			//	error?

			if (error && (error != ERROR_NO_MORE_ENTRIES))
			{
				complete = FALSE;
				break;
			}



			//	insert into filelist

			if (snode->includedirs || !isdir)
			{
				BOOL include = TRUE;
			
				curlock = DupLock(anchor.ap_Current->an_Lock);
				oldlock = CurrentDir(curlock);

				if (NameFromLock(curlock, fullname, MAXPATHNAMELEN))
				{
					if (AddPart(fullname, anchor.ap_Info.fib_FileName, MAXPATHNAMELEN+MAXFILENAMELEN))
					{
						char *formatname = NULL; 			
					
						if (!isdir)
						{
							if (rejectpattern)
							{
								include = !MatchPatternNoCase(rejectpattern, fullname);
							}
							if (include && !snode->simplescanning)
							{
								include = IsPicture(fullname, GGFX_FormatName, &formatname, TAG_DONE);
							}
						}
						
						if (include)
						{
							struct FileListNode *fnode;
							long filesize;
							
							filesize = anchor.ap_Info.fib_Size;
							
							if (fnode = CreateFileListNode(fullname, formatname ? formatname : "unknown", filesize, *scanindex, &anchor.ap_Info.fib_Date, isdir))
							{
								ObtainSemaphore(&filelist->locksemaphore);
								InsertFileNode(filelist, fnode);
								ReleaseSemaphore(&filelist->locksemaphore);
								(*scanindex)++;
								PostPHMessage(handler, PHNOTIFY_SCANNED_NUMBER, *scanindex);
							}
						}
						
						FreeVec(formatname);
					}
				}			

				CurrentDir(oldlock);
				UnLock(curlock);
			}
		
		
			MatchNext(&anchor);
		}

		MatchEnd(&anchor);
	}


	Free(rejectpattern);

	
	return complete;
}

/*********************************************************************
----------------------------------------------------------------------

	MysticView
	pichandler

	implements scanning, loading, preloading, caching,
	notification

	achtung:

		- der loader kann nur PHNOTIFY_PIC_AVAILABLE broadcasten,
		  wenn er selbst keinen Lock auf die loadedlist hält.
		  es käme zu einem zyklischen semaphore-deadlock zwischen
		  maintask, pichandler und loader:

		  loadtask:		Lock(loadedlist)
		  				PostMessage(
		  					ObtainPicture(
		  						Lock(loadedlist)/Unlock(loadedlist)
		  					)
		  					PutMsg()
		  				)
		  				Unlock(loadedlist)

		  maintask:		GetMsg()
		  				ObtainPicture(
		  					Lock(loadedlist)/Unlock(loadedlist)
		  				)
		  				Acknowdledge()

		  				...

		  handletask:	GetMsg()
		  				DeleteMsg(
		  					ReleasePicture()
		  					Lock(loadedlist)/Unlock(loadedlist)
		  				)

----------------------------------------------------------------------
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#ifdef MATH_68882
	#include <m68881.h>
#endif

#include <utility/tagitem.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <libraries/cybergraphics.h>
#include <intuition/intuition.h>
#include <exec/memory.h>
#include <devices/timer.h>

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
#include <proto/timer.h>

#include <clib/alib_protos.h>

#include "Mystic_Handler.h"
#include "Mystic_Subtask.h"
#include "Mystic_Global.h"
#include "Mystic_Tools.h"
#include "Mystic_Debug.h"
#include "Mystic_Scanner.h"


//
//	internal prototypes
//

struct FileList *CreateFileList(int sortmode, BOOL reverse);
void DeleteFileList(struct FileList *filelist);
void DeleteFileListNode(struct FileListNode *picnode);
BOOL FlushLoadedList(struct LoadedList *llist, int flushmode, int bufferpercent);
BOOL BufferFull(struct LoadedList *llist, int bufferpercent);

static BOOL PicHandler_SetSortMode(struct PicHandler *pichandler, int sortmode, BOOL reverse);

LONG SAVEDS loadfunc(struct subtask *subtask, BYTE abortsignal);
BOOL SAVEDS loadinitfunc(struct subtask *data);



#define	NUMPRELOAD			20			// number of files inserted when a pic is requested (max 30)
#define	NUMCACHE			100			// must be >= NUMPRELOAD

#define	TOLOADFACTOR			(0.5f)	// 0.5!
#define	LOADEDFACTOR			(0.7f)	// 0.7!

#define	FLUSHTOLOADTHRESHOLD	5		// 5!
#define FLUSHLOADEDTHRESHOLD	5		// 5!



/*
**	public-memory functions:
*/

APTR MalloclearPublic(ULONG size)
{
	return AllocVec(size, MEMF_PUBLIC | MEMF_CLEAR);
}

char *_StrDupPublic(char *s)
{
	char *s2 = NULL;

	if (s)
	{
		int l = strlen(s);
		if (s2 = AllocVec(l + 1, MEMF_PUBLIC))
		{
			if (l)
			{
				strcpy(s2, s);
			}
			else
			{
				*s2 = 0;
			}
		}
	}

	return s2;
}

void FreePublic(APTR mem)
{
	FreeVec(mem);
}




/*********************************************************************
----------------------------------------------------------------------

	FILELIST / SORTING

----------------------------------------------------------------------
*>>******************************************************************/


/*--------------------------------------------------------------------

	comparison functions for filenodes
	by file name, full name, date, format, size, comment, ...

--------------------------------------------------------------------*/

static int FileNode_CompareFileDateR(struct FileListNode *a, struct FileListNode *b)
{
	if (a->isdirectory == b->isdirectory)
	{
		return CompareDates(&a->datestamp, &b->datestamp);
	}
	return (a->isdirectory ? -1 : 1);
}

static int FileNode_CompareFileDate(struct FileListNode *a, struct FileListNode *b)
{
	if (a->isdirectory == b->isdirectory)
	{
		return -CompareDates(&a->datestamp, &b->datestamp);
	}
	return (a->isdirectory ? -1 : 1);
}


static int FileNode_CompareFileNameR(struct FileListNode *a, struct FileListNode *b)
{
	if (a->isdirectory == b->isdirectory)
	{
		return -Stricmp(FilePart(a->fullname), FilePart(b->fullname));
	}
	return (a->isdirectory ? -1 : 1);
}


static int FileNode_CompareFileName(struct FileListNode *a, struct FileListNode *b)
{
	if (a->isdirectory == b->isdirectory)
	{
		return Stricmp(FilePart(a->fullname), FilePart(b->fullname));
	}
	return (a->isdirectory ? -1 : 1);
}



static int FileNode_CompareFullNameR(struct FileListNode *a, struct FileListNode *b)
{
	if (a->isdirectory == b->isdirectory)
	{
		return -Stricmp(a->fullname, b->fullname);
	}
	return (a->isdirectory ? -1 : 1);
}

static int FileNode_CompareFullName(struct FileListNode *a, struct FileListNode *b)
{
	if (a->isdirectory == b->isdirectory)
	{
		return Stricmp(a->fullname, b->fullname);
	}
	return (a->isdirectory ? -1 : 1);
}



static int FileNode_CompareNaturalOrderR(struct FileListNode *a, struct FileListNode *b)
{
	if (a->isdirectory == b->isdirectory)
	{
		if (a->scanindex < b->scanindex)
		{
			return 1;
		}
		else if (a->scanindex > b->scanindex)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}
	return (a->isdirectory ? -1 : 1);
}

static int FileNode_CompareNaturalOrder(struct FileListNode *a, struct FileListNode *b)
{
	if (a->isdirectory == b->isdirectory)
	{
		if (a->scanindex < b->scanindex)
		{
			return -1;
		}
		else if (a->scanindex > b->scanindex)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return (a->isdirectory ? -1 : 1);
}



static int FileNode_CompareFileSizeR(struct FileListNode *a, struct FileListNode *b)
{
	if (a->isdirectory == b->isdirectory)
	{
		if (a->filesize < b->filesize)
		{
			return 1;
		}
		else if (a->filesize > b->filesize)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}
	return (a->isdirectory ? -1 : 1);
}

static int FileNode_CompareFileSize(struct FileListNode *a, struct FileListNode *b)
{
	if (a->isdirectory == b->isdirectory)
	{
		if (a->filesize < b->filesize)
		{
			return -1;
		}
		else if (a->filesize > b->filesize)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return (a->isdirectory ? -1 : 1);
}




/*--------------------------------------------------------------------

	FindFileListNode(filelist, ID)

	find file node with the given ID

	- caller must be owner of the list

--------------------------------------------------------------------*/

static struct FileListNode *FindFileListNode(struct FileList *filelist, ULONG id)
{
	struct Node *node = filelist->list->lh_Head, *nextnode, *foundnode = NULL;

	while (nextnode = node->ln_Succ)
	{
		if (((struct FileListNode *) node)->picID == id)
		{
			foundnode = node;
			break;
		}
		node = nextnode;
	}

	return (struct FileListNode *) foundnode;
}


/*--------------------------------------------------------------------

	FindToLoadListNode(tlist, ID)

	find file node with the given ID

	- caller must be owner of the list

--------------------------------------------------------------------*/

static struct ToLoadNode *FindToLoadNode(struct ToLoadList *tlist, ULONG id)
{
	struct Node *node = tlist->list->lh_Head, *nextnode, *foundnode = NULL;

	while (nextnode = node->ln_Succ)
	{
		if (((struct ToLoadNode *) node)->picID == id)
		{
			foundnode = node;
			break;
		}
		node = nextnode;
	}

	return (struct ToLoadNode *) foundnode;
}




/*--------------------------------------------------------------------

	newnode = SeekFileNode(currentnode, steps, failatend)
	move in a filelist by a (relative) number of steps.
	- caller must be owner of the list
	- if failatend=TRUE, returns NULL if the list seeks past end
	  or before start.

--------------------------------------------------------------------*/

static struct FileListNode *SeekFileNode(struct FileListNode *currentnode, int steps, BOOL failatend)
{
	struct Node *newnode = &currentnode->node;

	if (steps < 0)
	{
		while (steps++ < 0)
		{
			if (newnode->ln_Pred->ln_Pred)
			{
				newnode = newnode->ln_Pred;
			}
			else
			{
				if (failatend)
				{
					newnode = NULL;
				}
				break;
			}
		}
	}
	else if (steps > 0)
	{
		while (steps-- > 0)
		{
			if (newnode->ln_Succ->ln_Succ)
			{
				newnode = newnode->ln_Succ;
			}
			else
			{
				if (failatend)
				{
					newnode = NULL;
				}
				break;
			}
		}
	}

	return (struct FileListNode *) newnode;
}



/*--------------------------------------------------------------------

	newnode = SeekCurrentFileNodeS(filelist, mode, steps)
	move in a filelist by a (relative) number of steps.
	- caller must be owner of the list

--------------------------------------------------------------------*/

static struct FileListNode *SeekCurrentFileNode(struct FileList *flist, int mode, int steps)
{
	struct FileListNode *fnode = NULL;

	assert(flist);

	if (!IsListEmpty(flist->list))
	{
		switch (mode)
		{
			case PHLIST_RELATIVE:
				fnode = SeekFileNode(flist->currentnode, steps, FALSE);
				break;

			case PHLIST_START:
				fnode = SeekFileNode((struct FileListNode *) flist->list->lh_Head, steps, FALSE);
				break;

			case PHLIST_END:
				fnode = SeekFileNode((struct FileListNode *) flist->list->lh_TailPred, steps, FALSE);
				break;
		}
	}

	return fnode;
}


/*--------------------------------------------------------------------

	InsertFileNode(filelist, filelistnode, comparisonfunc)

	insert a filenode to a sorted filelist
	(implements binary search)

--------------------------------------------------------------------*/

void InsertFileNode(struct FileList *filelist, struct FileListNode *newnode)
{

	ObtainSemaphore(&filelist->locksemaphore);

	if (filelist->numentries == 0)
	{
		AddTail(filelist->list, &newnode->node);
	}
	else
	{
		if (filelist->sortmode == SORTMODE_RANDOM)
		{
			int newindex;
			struct FileListNode *currentnode = (struct FileListNode *) filelist->list->lh_Head;

			newindex = getrandom(0, filelist->numentries - 1);

			if (newindex > 0)
			{
				currentnode = SeekFileNode(currentnode, newindex, FALSE);
				Insert(filelist->list, &newnode->node, &currentnode->node);
			}
			else
			{
				AddHead(filelist->list, &newnode->node);
			}
		}
		else
		{
			struct FileListNode *currentnode = (struct FileListNode *) filelist->list->lh_Head;
			int currentindex = 0;
			int lowerindex = 0;
			int upperindex = filelist->numentries - 1;
			int (*cmp)(struct FileListNode *a, struct FileListNode *b);

			switch (filelist->sortmode)
			{
				default:

				case SORTMODE_DATE:
					if (filelist->reverse)
					{
						cmp = FileNode_CompareFileDateR;
					}
					else
					{
						cmp = FileNode_CompareFileDate;
					}
					break;

				case SORTMODE_ALPHA_FULL:
					if (filelist->reverse)
					{
						cmp = FileNode_CompareFullNameR;
					}
					else
					{
						cmp = FileNode_CompareFullName;
					}
					break;

				case SORTMODE_ALPHA_FILE:
					if (filelist->reverse)
					{
						cmp = FileNode_CompareFileNameR;
					}
					else
					{
						cmp = FileNode_CompareFileName;
					}
					break;

				case SORTMODE_NONE:
					if (filelist->reverse)
					{
						cmp = FileNode_CompareNaturalOrderR;
					}
					else
					{
						cmp = FileNode_CompareNaturalOrder;
					}
					break;

				case SORTMODE_FILESIZE:
					if (filelist->reverse)
					{
						cmp = FileNode_CompareFileSizeR;
					}
					else
					{
						cmp = FileNode_CompareFileSize;
					}
					break;
			}

			for(;;)
			{
				currentnode = SeekFileNode(currentnode, (lowerindex + (upperindex-lowerindex)/2) - currentindex, FALSE);
				currentindex = lowerindex + (upperindex-lowerindex)/2;

				if (cmp(newnode, currentnode) > 0)
				{
					if (lowerindex == upperindex)
					{
						Insert(filelist->list, &newnode->node, &currentnode->node);
						break;
					}
					else
					{
						lowerindex = currentindex + 1;
					}
				}
				else
				{
					if (lowerindex == upperindex)
					{
						if (currentindex == 0)
						{
							AddHead(filelist->list, &newnode->node);
						}
						else
						{
							currentnode = SeekFileNode(currentnode, -1, FALSE);
							Insert(filelist->list, &newnode->node, &currentnode->node);
						}
						break;
					}
					else
					{
						upperindex = currentindex;
					}
				}
			}
		}
	}

	filelist->numentries++;

	if (filelist->numentries == 1)
	{
		assert (filelist->currentnode == NULL);
		filelist->currentnode = newnode;
	}

	ReleaseSemaphore(&filelist->locksemaphore);
}


/*--------------------------------------------------------------------

	SortFileList(filelist)

--------------------------------------------------------------------*/

BOOL SortFileList(struct FileList *filelist, int sortmode, BOOL reverse)
{
	BOOL success = FALSE;

	if (filelist)
	{
		struct FileList *tempfilelist;

		if (tempfilelist = CreateFileList(sortmode, reverse))
		{
			struct FileListNode *fnode;

			ObtainSemaphore(&filelist->locksemaphore);

			//	remove nodes from old list, insert them into temporary filelist:


			DB(kprintf("*** resorting filelist...\n"));

			while (fnode = (struct FileListNode *) RemHead(filelist->list))
			{
				InsertFileNode(tempfilelist, fnode);
			}

			//	delete old list, link to temporary filelist's list,
			//	unlink temporary filelist's list, delete temporary filelist

			DeleteList(filelist->list);
			filelist->list = tempfilelist->list;
			tempfilelist->list = NULL;
			DeleteFileList(tempfilelist);

			filelist->sortmode = sortmode;
			filelist->reverse = reverse;

			ReleaseSemaphore(&filelist->locksemaphore);

			success = TRUE;
		}
	}

	return success;
}

/*<<*****************************************************************/



/*********************************************************************
----------------------------------------------------------------------

	SUBTASK SCANNER

----------------------------------------------------------------------
*>>******************************************************************/

/*--------------------------------------------------------------------

	DeleteScanListNode(scanlistnode)

--------------------------------------------------------------------*/

void DeleteScanListNode(struct ScanListNode *scannode)
{
	if (scannode)
	{
		FreePublic(scannode->filepattern);
		FreePublic(scannode->rejectpattern);
		FreePublic(scannode);
	}
}


/*--------------------------------------------------------------------

	scannode = CreateScanListNode(patternlist, rejectpattern, recursive, pattern, includedirs, simplescanning)

--------------------------------------------------------------------*/

struct ScanListNode *CreateScanListNode(char *filepattern, char *rejectpattern, BOOL recursive, BOOL includedirs, BOOL simplescanning)
{
	struct ScanListNode *slnode;
	BOOL success = FALSE;

	if (slnode = MalloclearPublic(sizeof(struct ScanListNode)))
	{
		if (slnode->filepattern = _StrDupPublic(filepattern))
		{
			slnode->rejectpattern = _StrDupPublic(rejectpattern);

			slnode->node.ln_Type = NT_USER;
			slnode->node.ln_Pri = 0;
			slnode->recursive = recursive;
			slnode->simplescanning = simplescanning;
			slnode->includedirs = includedirs;
			success = TRUE;
		}

		if (!success)
		{
			DeleteScanListNode(slnode);
			slnode = NULL;
		}
	}

	return slnode;
}



/*--------------------------------------------------------------------

	ClearScanList(scanlist)

	- caller must be owner of the list

--------------------------------------------------------------------*/

void ClearScanList(struct ScanList *scanlist)
{
	if (scanlist->list)
	{
		struct ScanListNode *snode;

		while (snode = (struct ScanListNode *) RemHead(scanlist->list))
		{
			DeleteScanListNode(snode);
		}
		scanlist->numentries = 0;
		scanlist->numscanned = 0;
	}
}


/*--------------------------------------------------------------------

	DeleteScanList(scanlist)

--------------------------------------------------------------------*/

void DeleteScanList(struct ScanList *scanlist)
{
	if (scanlist)
	{
		RemSemaphore(&scanlist->locksemaphore);

		assert(scanlist->list);

		ClearScanList(scanlist);

		DeleteList(scanlist->list);

		Free(scanlist);
	}
}


/*--------------------------------------------------------------------

	scanlist = CreateScanList()

--------------------------------------------------------------------*/

struct ScanList *CreateScanList(void)
{
	struct ScanList *scanlist;
	BOOL success = FALSE;

	if (scanlist = Malloclear(sizeof(struct ScanList)))
	{
		scanlist->locksemaphore.ss_Link.ln_Pri = 0;
		scanlist->locksemaphore.ss_Link.ln_Name = PICLIST_SEMAPHORE_NAME;

		AddSemaphore(&scanlist->locksemaphore);

		if (scanlist->list = CreateList())
		{
			success = TRUE;
		}
	}

	if (!success)
	{
		DeleteScanList(scanlist);
		scanlist = NULL;
	}

	return scanlist;
}


/*--------------------------------------------------------------------

	scanlistnode = ScanList_AddPattern(
		scanlist,
		filepattern,
		rejectpattern,
		recursive,
		includedirs,
		simplescanning

	add a single file(pattern) to the scanlist

--------------------------------------------------------------------*/

struct ScanListNode *ScanList_AddPattern(struct ScanList *scanlist,
	char *filepattern, char *rejectpattern, BOOL recursive, BOOL includedirs, BOOL simplescanning)
{
	struct ScanListNode *slnode = NULL;

	if (scanlist)
	{
		if (slnode = CreateScanListNode(filepattern, rejectpattern, recursive, includedirs, simplescanning))
		{
			assert(scanlist->list);

			ObtainSemaphore(&scanlist->locksemaphore);

			Enqueue(scanlist->list, &slnode->node);

			scanlist->numentries++;

			ReleaseSemaphore(&scanlist->locksemaphore);
		}
	}

	return slnode;
}



/*--------------------------------------------------------------------

	scanclosefunc

--------------------------------------------------------------------*/

void scanclosefunc(struct PicHandler *handler)
{
	assert(handler);

	DeleteScanList(handler->scanlist);
	handler->scanlist = NULL;

	DB(kprintf("*** scanner closing down.\n"));

}


/*--------------------------------------------------------------------

	scaninitfunc

--------------------------------------------------------------------*/

BOOL SAVEDS scaninitfunc(struct subtask *data)
{
	struct PicHandler *handler = (struct PicHandler *) data;

	assert(handler->scanlist == NULL);

	if (handler->scanlist = CreateScanList())
	{
		return TRUE;
	}

	scanclosefunc(handler);
	return FALSE;
}


/*--------------------------------------------------------------------

	scanfunc

--------------------------------------------------------------------*/

LONG SAVEDS scanfunc(struct subtask *subtask, BYTE abortsignal)
{
	struct PicHandler *handler;
	ULONG signals;
	struct ScanList *scanlist;
	struct ScanListNode *snode;
	BOOL check;
	ULONG scanindex = 0;
	ULONG abortsignals;

	BOOL scanning = FALSE;
	int numscanned = 0;


	handler = (struct PicHandler *) ObtainData(subtask);

	abortsignals = 1 << abortsignal;


	scanlist = handler->scanlist;

	do
	{
		Delay(4);

		do
		{
			check = FALSE;


			//	remove a node from the scanlist

			ObtainSemaphore(&scanlist->locksemaphore);

			if (snode = (struct ScanListNode *) RemHead(scanlist->list))
			{
				scanlist->numentries--;
			}

			ReleaseSemaphore(&scanlist->locksemaphore);


			//	process this node, insert the resulting files to the filelist

			if (snode)
			{
				if (!scanning)
				{
					scanning = TRUE;
					PostPHMessage(handler, PHNOTIFY_SCANNING, TRUE);
				}

				check = Scan(handler->filelist, snode, abortsignals, &scanindex, handler);

				if (scanindex != numscanned)
				{
					numscanned = scanindex;
				}

				ObtainSemaphore(&scanlist->locksemaphore);
				if (scanlist->numentries == 0)
				{
					PostPHMessage(handler, PHNOTIFY_SCANNING, FALSE);
					scanning = FALSE;
				}
				ReleaseSemaphore(&scanlist->locksemaphore);

				DeleteScanListNode(snode);
			}

		} while (check);

		signals = SetSignal(0, abortsignals);

	} while (!(signals & abortsignals));



	scanclosefunc(handler);

	ReleaseData(subtask);

	return NULL;
}

/*<<*****************************************************************/



/*********************************************************************
----------------------------------------------------------------------

	FILELIST

----------------------------------------------------------------------
*>>******************************************************************/

/*--------------------------------------------------------------------

	DeleteFileListNode(filelistnode)

--------------------------------------------------------------------*/

void DeleteFileListNode(struct FileListNode *fnode)
{
	if (fnode)
	{
		Free(fnode->formatID);
		Free(fnode->fullname);
		Free(fnode);
	}
}


/*--------------------------------------------------------------------

	picnode = CreateFileListNode(fullname, formatID, filesize, scanindex, datestamp, isdirectory)

--------------------------------------------------------------------*/

struct FileListNode *CreateFileListNode(char *fullname, char *formatID, long filesize, int scanindex, struct DateStamp *datestamp, BOOL isdir)
{
	struct FileListNode *pnode;
	BOOL success = FALSE;

	if (pnode = Malloclear(sizeof(struct FileListNode)))
	{
		if (pnode->fullname = _StrDup(fullname))
		{
			ULONG id;

			pnode->formatID = _StrDup(formatID);
			pnode->filesize = filesize;

			while ((id = GetUniqueID()) == PICID_UNDEFINED);
			pnode->picID = id;

			pnode->scanindex = scanindex;

			pnode->node.ln_Type = NT_USER;
			pnode->node.ln_Pri = 0;

			pnode->isdirectory = isdir;

			memcpy(&pnode->datestamp, datestamp, sizeof(struct DateStamp));

			success = TRUE;
		}

		if (!success)
		{
			DeleteFileListNode(pnode);
			pnode = NULL;
		}
	}

	return pnode;
}


/*--------------------------------------------------------------------

	RemFileListNode(filelist, filelistnode)

	remove list node. it is not deleted.
	- assures integrity of the currentnode field
	- caller must be owner of the list

--------------------------------------------------------------------*/

void RemFileListNode(struct FileList *flist, struct FileListNode *fnode)
{
	assert (flist && fnode);

//	assert(fnode->node->ln_Succ);		// StormC problem ?!?
//	assert(fnode->node->ln_Pred);		// StormC sukks

	if (fnode == flist->currentnode)
	{
		if (fnode->node.ln_Succ->ln_Succ)
		{
			flist->currentnode = (struct FileListNode *) fnode->node.ln_Succ;
		}
		else if (fnode->node.ln_Pred->ln_Pred)
		{
			flist->currentnode = (struct FileListNode *) fnode->node.ln_Pred;
		}
		else
		{
			flist->currentnode = NULL;
		}
	}

	Remove(&fnode->node);
}


/*--------------------------------------------------------------------

	DeleteFileList(filelist)

--------------------------------------------------------------------*/

void DeleteFileList(struct FileList *filelist)
{
	if (filelist)
	{
		if (filelist->list)
		{
			struct FileListNode *fnode;

			while (fnode = (struct FileListNode *) RemHead(filelist->list))
			{
				DeleteFileListNode(fnode);
			}

			DeleteList(filelist->list);
		}

		Free(filelist);
	}
}


/*--------------------------------------------------------------------

	filelist = CreateFileList(sortmode, reverse)

--------------------------------------------------------------------*/

struct FileList *CreateFileList(int sortmode, BOOL reverse)
{
	struct FileList *filelist;
	BOOL success = FALSE;

	if (filelist = Malloclear(sizeof(struct FileList)))
	{
		InitSemaphore(&filelist->locksemaphore);
		if (filelist->list = CreateList())
		{
			filelist->numentries = 0;
			filelist->sortmode = sortmode;
			filelist->reverse = reverse;
			filelist->currentnode = NULL;
			success = TRUE;
		}
	}

	if (!success)
	{
		DeleteFileList(filelist);
		filelist = NULL;
	}

	return filelist;
}

/*<<*****************************************************************/



/*********************************************************************
----------------------------------------------------------------------

	TOLOADLIST

----------------------------------------------------------------------
*>>******************************************************************/

/*--------------------------------------------------------------------

	DeleteToLoadNode(toloadnode)

--------------------------------------------------------------------*/

void DeleteToLoadNode(struct ToLoadNode *tnode)
{
	if (tnode)
	{
		Free(tnode->fullname);
		Free(tnode);
	}
}

/*--------------------------------------------------------------------

	toloadnode = CreateToLoadNode(ID, fullname, pri, isdirectory)

--------------------------------------------------------------------*/

struct ToLoadNode *CreateToLoadNode(ULONG picID, char *fullname, float pri, BOOL isdirectory)
{
	struct ToLoadNode *tnode;
	BOOL success = FALSE;

	if (tnode = Malloclear(sizeof(struct ToLoadNode)))
	{
		if (tnode->fullname = _StrDup(fullname))
		{
			tnode->picID = picID;
			tnode->node.ln_Type = NT_USER;
			tnode->pri = pri;
			tnode->node.ln_Pri = (int)(pri * 127.0f);
			tnode->isdirectory = isdirectory;
			success = TRUE;
		}
	}

	if (!success)
	{
		DeleteToLoadNode(tnode);
		tnode = NULL;
	}

	return tnode;
}


/*--------------------------------------------------------------------

	DeleteToLoadList(tlist)

--------------------------------------------------------------------*/

void DeleteToLoadList(struct ToLoadList *tlist)
{
	if (tlist)
	{
		if (tlist->list)
		{
			struct ToLoadNode *tnode;

			while (tnode = (struct ToLoadNode *) RemHead(tlist->list))
			{
				DeleteToLoadNode(tnode);
			}

			DeleteList(tlist->list);
		}
		Free(tlist);
	}
}


/*--------------------------------------------------------------------

	tlist = CreateToLoadList()

--------------------------------------------------------------------*/

struct ToLoadList *CreateToLoadList(void)
{
	struct ToLoadList *tlist;
	BOOL success = FALSE;

	if (tlist = Malloclear(sizeof(struct ToLoadList)))
	{
		InitSemaphore(&tlist->locksemaphore);
		if (tlist->list = CreateList())
		{
			tlist->numentries = 0;
			success = TRUE;
		}
	}

	if (!success)
	{
		DeleteToLoadList(tlist);
		tlist = NULL;
	}

	return tlist;
}



/*--------------------------------------------------------------------

	LowerToLoadListPriority(tlist)

	lower toloadlist's priorities, remove nodes with priority 0

--------------------------------------------------------------------*/

void LowerToLoadListPriority(struct ToLoadList *tlist)
{
	struct Node *node, *nextnode;

	assert (tlist);

	ObtainSemaphore(&tlist->locksemaphore);

	if (node = tlist->list->lh_Head)
	{
		while (nextnode = node->ln_Succ)
		{
			struct ToLoadNode *tlnode = (struct ToLoadNode *) node;

			tlnode->pri *= (float) TOLOADFACTOR;
			node->ln_Pri = (int)(tlnode->pri * 127.0f);

			if (node->ln_Pri < FLUSHTOLOADTHRESHOLD)
			{
				Remove(node);
				DeleteToLoadNode((struct ToLoadNode *) node);
				tlist->numentries--;
			}

			node = nextnode;
		}
	}

	ReleaseSemaphore(&tlist->locksemaphore);
}



/*--------------------------------------------------------------------

	EnqueueToLoadList(tlist, tnode)

	enqueue toloadnode to toloadlist

	- search and remove node with the current ID
	- lower all priorities by the same factor
	- enqueue current node

--------------------------------------------------------------------*/

void EnqueueToLoadList(struct FileList *flist, struct ToLoadList *tlist, struct ToLoadNode *newtnode)
{
	struct Node *node, *nextnode;

	assert (flist && tlist && newtnode);

	ObtainSemaphore(&tlist->locksemaphore);

	//	remove node with the new node's ID (if present)

	if (node = tlist->list->lh_Head)
	{
		while (nextnode = node->ln_Succ)
		{
			if (((struct ToLoadNode *) node)->picID == newtnode->picID)
			{
				Remove(node);
				DeleteToLoadNode((struct ToLoadNode *) node);
				tlist->numentries--;
				break;
			}
			node = nextnode;
		}
	}


	//	enqueue new node

	Enqueue(tlist->list, &newtnode->node);
	tlist->numentries++;

//	ReleaseSemaphore(&flist->locksemaphore);
	ReleaseSemaphore(&tlist->locksemaphore);
}


/*--------------------------------------------------------------------

	success = RequestIDToLoad(loadedlist, filelist, toloadlist, id)

	insert an ID to the toloadlist.

--------------------------------------------------------------------*/

static int preloadoffset[] =
		{0, 1, 2, 3, 4, 5, 6, 7,
		 8, 9, 10, 11, 12, 13, 14, 15, 16,
		 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};

BOOL RequestIDToLoad(struct PicHandler *ph, ULONG id)
{
	struct FileListNode *filenode, *tempfilenode;
	int i;
	float priority;
	BOOL success = FALSE;

	struct FileList *flist;
	struct ToLoadList *tlist;

	assert(ph);

	flist = ph->filelist;
	tlist = ph->toloadlist;


	if (flist && tlist)
	{
		ObtainSemaphore(&flist->locksemaphore);

		filenode = FindFileListNode(flist, id);

		if (filenode)
		{
			BOOL primary = TRUE;

			success = TRUE;

			priority = 1.0f;


			ObtainSemaphore(&tlist->locksemaphore);

			for (i = 0; i < NUMPRELOAD && success; ++i)
			{
				tempfilenode = SeekFileNode(filenode, preloadoffset[i], TRUE);

				if (tempfilenode)
				{
					struct ToLoadNode *tnode;
					success = FALSE;

					if (tnode = CreateToLoadNode(tempfilenode->picID, tempfilenode->fullname,
						priority, tempfilenode->isdirectory))
					{
						tnode->primary = primary;
						EnqueueToLoadList(flist, tlist, tnode);
						success = TRUE;
					}
				}
				priority *= TOLOADFACTOR;
				primary = FALSE;
			}

			LowerToLoadListPriority(tlist);

			ReleaseSemaphore(&tlist->locksemaphore);
		}

		ReleaseSemaphore(&flist->locksemaphore);
	}

	return success;
}

/*<<*****************************************************************/



/*********************************************************************
----------------------------------------------------------------------

	NOTIFICATION

----------------------------------------------------------------------
*>>******************************************************************/


void DeletePHMessage(struct PicHandler *handler, struct PHMessage *msg)
{
	if (msg)
	{
		if (msg->msgtype == PHNOTIFY_PIC_AVAILABLE)
		{
			PicHandler_ReleasePicture(handler, msg->id);
		}

		Free(msg->fullname);
		Free(msg);
	}
}

struct PHMessage *CreatePHMessage(struct PicHandler *handler, ULONG msgtype, ULONG *data)
{
	struct PHMessage *msg;
	BOOL success = FALSE;

	if (msg = Malloclear(sizeof(struct PHMessage)))
	{
		assert(handler);
		assert(handler->replyport);
		assert(msgtype != PHNOTIFY_NOTHING);

		msg->notifylistnode.ln_Type = NT_USER;
		msg->msg.mn_ReplyPort = handler->replyport;
		msg->msg.mn_Length = sizeof(struct PHMessage);
		msg->msgtype = msgtype;


		switch (msgtype)
		{
			case PHNOTIFY_SCANNING:
				msg->scanning = (BOOL) data[0];
				success = TRUE;
				break;

			case PHNOTIFY_SCANNED_NUMBER:
				msg->numEntries = data[0];
				success = TRUE;
				break;

			case PHNOTIFY_LOADING:
				msg->loading = data[0];
				success = TRUE;
				break;

			case PHNOTIFY_ID_LOADING:
				msg->id = data[0];
				msg->loading = data[1];
				success = TRUE;
				break;

			case PHNOTIFY_ID_AVAILABLE:
				msg->id = data[0];
				success = TRUE;
				break;

			case PHNOTIFY_ID_ERROR_LOADING:
				msg->id = data[0];
				msg->error = (LONG) data[1];
				success = TRUE;
				break;

			case PHNOTIFY_PIC_AVAILABLE:

				//	we lock this picture and ensure availability.
				//	when the message is acknowledged, we release
				//	the picture. in the meantime, the calling task
				//	itself had the oppurtunity to obtain a lock
				//	on the picture.

				msg->id = PICID_UNDEFINED;

				if (PicHandler_ObtainPicture(handler, data[0]))
				{
					msg->id = data[0];
					success = TRUE;
				}
				else
				{
					printf("warning: PHNOTIFY_PIC_AVAILABLE broadcasted, but ID not available!\n");
				}
				break;

			default:
				assert(FALSE);
				break;
		}

	}

	if (!success)
	{
		DeletePHMessage(handler, msg);
		msg = NULL;
	}

	return msg;
}




BOOL STDARGS PostPHMessage(struct PicHandler *handler, ULONG msgtype, ...)
{
	va_list va;
	ULONG *data;
	BOOL success = TRUE;

	va_start(va, msgtype);
#ifdef __MORPHOS__
	data = (ULONG *) va->overflow_arg_area;
#else
	data = (ULONG *) va;
#endif

	if (handler)
	{
		ObtainSemaphore(&handler->notifysemaphore);

		if (handler->notifymask & msgtype)
		{
			struct PHMessage *msg;

			success = FALSE;

			if (msg = CreatePHMessage(handler, msgtype, data))
			{
				assert(handler->msgport);

				AddTail(&handler->notifylist, &msg->notifylistnode);

				PutMsg(handler->msgport, (struct Message *) msg);

				success = TRUE;
			}
		}

		ReleaseSemaphore(&handler->notifysemaphore);
	}

	return success;
}


struct PHMessage *PicHandler_GetNotification(struct PicHandler *handler)
{
	struct PHMessage *msg = NULL;

	if (handler)
	{
		msg = (struct PHMessage *) GetMsg(handler->msgport);
	}

	return msg;
}


void PicHandler_Acknowledge(struct PicHandler *handler, struct PHMessage *msg)
{
	if (handler && msg)
	{
		ReplyMsg((struct Message *) msg);
	}
}



/*<<*****************************************************************/



/*********************************************************************
----------------------------------------------------------------------

	SUBTASK PICHANDLER

----------------------------------------------------------------------
*>>******************************************************************/

/*--------------------------------------------------------------------

	pichandler subtask closedown func

--------------------------------------------------------------------*/

void pichandleclosefunc(struct PicHandler *handler)
{
	struct PHMessage *phmsg;

	assert(handler);


	DB(kprintf("***pichandler closes down loadtask.\n"));
	AbortSubTask(handler->loadtask);
	CloseSubTask(handler->loadtask);
	handler->loadtask = NULL;
	DB(kprintf("***pichandler closed down loadtask successfully.\n"));

	AbortSubTask(handler->scantask);
	CloseSubTask(handler->scantask);
	handler->scantask = NULL;


	DeleteFileList(handler->filelist);
	handler->filelist = NULL;


	while (phmsg = (struct PHMessage *) GetMsg(handler->replyport))
	{
		ObtainSemaphore(&handler->notifysemaphore);
		Remove(&phmsg->notifylistnode);
		DeletePHMessage(handler, phmsg);
		ReleaseSemaphore(&handler->notifysemaphore);
	}


	DeleteMsgPort(handler->replyport);
	handler->replyport = NULL;

	DB(kprintf("*** pichandler closing down.\n"));

}


/*--------------------------------------------------------------------

	pichandler subtask init func

	create replyport, setup lists, create scantask,
	create loadtask

--------------------------------------------------------------------*/

BOOL SAVEDS pichandleinitfunc(struct subtask *data)
{
	struct PicHandler *handler = (struct PicHandler *) data;

	handler->replyport = CreateMsgPort();
	handler->filelist = CreateFileList(handler->sortmode, handler->reverse);

	handler->scantask = SubTask(scanfunc, handler, SCANSTACK, globalpriority, "MysticView Scanner (%x)", scaninitfunc, FALSE);

	handler->loadtask = SubTask(loadfunc, handler, LOADSTACK, globalpriority, "MysticView Loader (%x)", loadinitfunc, FALSE);

	if (handler->replyport && handler->scantask && handler->loadtask && handler->filelist)
	{
		return TRUE;
	}
	else
	{
		pichandleclosefunc(handler);
		return FALSE;
	}
}


/*--------------------------------------------------------------------

	pichandler subtask main

--------------------------------------------------------------------*/

LONG SAVEDS pichandlefunc(struct subtask *subtask, BYTE abortsignal)
{
	struct PicHandler *handler;
	ULONG signals;
	ULONG abortsignals = 1 << abortsignal;
	ULONG replysignals;

	handler = (struct PicHandler *) ObtainData(subtask);

	replysignals = 1 << handler->replyport->mp_SigBit;



	do
	{
		signals = Wait(abortsignals | replysignals);

		if (signals & replysignals)
		{
			struct PHMessage *phmsg;

			while (phmsg = (struct PHMessage *) GetMsg(handler->replyport))
			{
				ObtainSemaphore(&handler->notifysemaphore);

				Remove(&phmsg->notifylistnode);
				DeletePHMessage(handler, phmsg);

				ReleaseSemaphore(&handler->notifysemaphore);
			}
		}

	} while (!(signals & abortsignals));


	pichandleclosefunc(handler);

	ReleaseData(subtask);

	return NULL;
}





/*--------------------------------------------------------------------

	pichandler = PicHandler_Create(tags)

--------------------------------------------------------------------*/

struct PicHandler * STDARGS PicHandler_Create(ULONG dummy, ...)
{
	va_list va;
	struct TagItem *tags;

	struct PicHandler *pichandler;
	BOOL success = FALSE;

	va_start(va, dummy);
#ifdef __MORPHOS__
	tags = (struct TagItem *) va->overflow_arg_area;
#else
	tags = (struct TagItem *) va;
#endif

	if (pichandler = Malloclear(sizeof(struct PicHandler)))
	{
		NewList(&pichandler->notifylist);
		InitSemaphore(&pichandler->notifysemaphore);

		pichandler->sortmode = (int) GetTagData(PICH_SortMode, SORTMODE_NONE, tags);
		pichandler->reverse = (BOOL) GetTagData(PICH_SortReverse, FALSE, tags);
		pichandler->recursive = (BOOL) GetTagData(PICH_Recursive, TRUE, tags);
		pichandler->includedirs = (BOOL) GetTagData(PICH_IncludeDirs, FALSE, tags);
		pichandler->simplescanning = (BOOL) GetTagData(PICH_SimpleScanning, FALSE, tags);
		pichandler->rejectpattern = _StrDup((char *) GetTagData(PICH_Reject, NULL, tags));
		pichandler->notifymask = GetTagData(PICH_Notify, PHNOTIFY_NOTHING, tags);
		pichandler->bufferpercent = GetTagData(PICH_BufferPercent, 40, tags);
		pichandler->autocrop = GetTagData(PICH_AutoCrop, FALSE, tags);

		if (pichandler->msgport = CreateMsgPort())
		{
			if (pichandler->toloadlist = CreateToLoadList())
			{
				if (pichandler->pichandletask = SubTask(pichandlefunc, pichandler, PICHANDLESTACK, globalpriority, "MysticView PicHandler (%x)", pichandleinitfunc, FALSE))
				{
					success = TRUE;
				}
			}
		}
	}

	if (!success)
	{
		PicHandler_Delete(pichandler);
		pichandler = NULL;
	}

 	return pichandler;
}


/*********************************************************************
----------------------------------------------------------------------

	PicHandler_Delete(pichandler)

----------------------------------------------------------------------
*********************************************************************/

void PicHandler_Delete(struct PicHandler *pichandler)
{
	if (pichandler)
	{
		struct PHMessage *phmsg;


		//	shut down as gently as possible:

		PicHandler_SetAttributes(pichandler, PICH_Notify, PHNOTIFY_NOTHING, TAG_DONE);

		while (phmsg = PicHandler_GetNotification(pichandler))
		{
			PicHandler_Acknowledge(pichandler, phmsg);
		}


		assert(pichandler->pichandletask);

		AbortSubTask(pichandler->pichandletask);
		CloseSubTask(pichandler->pichandletask);


		while (phmsg = PicHandler_GetNotification(pichandler))
		{
			DeletePHMessage(pichandler, phmsg);
		}


		DeleteMsgPort(pichandler->msgport);


		DeleteToLoadList(pichandler->toloadlist);

		Free(pichandler->rejectpattern);

		Free(pichandler);
	}
}


/*********************************************************************
----------------------------------------------------------------------

	PicHandler_GetSigMask(pichandler)

----------------------------------------------------------------------
*********************************************************************/

ULONG PicHandler_GetSigMask(struct PicHandler *pichandler)
{
	ULONG sigmask = NULL;

	if (pichandler)
	{
		assert(pichandler->msgport);

		sigmask = 1 << (pichandler->msgport->mp_SigBit);
	}

	return sigmask;
}


/*********************************************************************
----------------------------------------------------------------------

	success = PicHandler_AddPatternList(pichandler, filepatternlist, tags);

	add files/patterns to a pichandler.

	tags:
		PICH_Recursive		(override pichandler default)
		PICH_Reject			(override pichandler default)
		PICH_IncludeDirs	(override pichandler default)
		PICH_SimpleScanning	(override pichandler default)

----------------------------------------------------------------------
*********************************************************************/

BOOL STDARGS PicHandler_AddPatternList(
	struct PicHandler *pichandler,
	char **patternlist,
	...)
{
	BOOL result = FALSE;

	if (pichandler && patternlist)
	{
		va_list va;
		struct TagItem *tags;
		char *rejectpattern;
		BOOL recursive, includedirs, simplescanning;
		char *filepattern;

		va_start(va, patternlist);
#ifdef __MORPHOS__
		tags = (struct TagItem *) va->overflow_arg_area;
#else
		tags = (struct TagItem *) va;
#endif

		recursive = (BOOL) GetTagData(PICH_Recursive, (ULONG) pichandler->recursive, tags);
		rejectpattern = (char *) GetTagData(PICH_Reject, (ULONG) pichandler->rejectpattern, tags);
		includedirs = (BOOL) GetTagData(PICH_IncludeDirs, (ULONG) pichandler->includedirs, tags);
		simplescanning = (BOOL) GetTagData(PICH_SimpleScanning, (ULONG) pichandler->simplescanning, tags);


		result = TRUE;

		while (result && (filepattern = *patternlist++))
		{
			result = !!ScanList_AddPattern(pichandler->scanlist,
				filepattern, rejectpattern, recursive, includedirs, simplescanning);
		}
	}

	return result;
}




/*********************************************************************
----------------------------------------------------------------------

	success = PicHandler_SetAttributes(pichandler,tags);

	set pichandler attributes

	PICH_Notify
	PICH_SortMode		new global default
	PICH_Reverse		new global default
	PICH_Recursive		new global default
	PICH_IncludeDirs	new global default
	PICH_RejectPattern	new global default
	PICH_SimpleScanning	new global default

----------------------------------------------------------------------
*********************************************************************/

BOOL STDARGS PicHandler_SetAttributes(
	struct PicHandler *pichandler,
	...)
{
	BOOL success = FALSE;

	if (pichandler)
	{
		va_list va;
		struct TagItem *tags;
		int sortmode;
		BOOL reverse;
		int errors = 0;
		char *rejectpattern;
		int bufferpercent;


		va_start(va, pichandler);
#ifdef __MORPHOS__
		tags = (struct TagItem *) va->overflow_arg_area;
#else
		tags = (struct TagItem *) va;
#endif

		sortmode = (int) GetTagData(PICH_SortMode, pichandler->sortmode, tags);
		reverse = (BOOL) GetTagData(PICH_SortReverse, pichandler->reverse, tags);

		errors += PicHandler_SetSortMode(pichandler, sortmode, reverse) ? 0 : 1;

		pichandler->autocrop = GetTagData(PICH_AutoCrop, pichandler->autocrop, tags);

		ObtainSemaphore(&pichandler->notifysemaphore);
		pichandler->notifymask = GetTagData(PICH_Notify, pichandler->notifymask, tags);
		ReleaseSemaphore(&pichandler->notifysemaphore);

		pichandler->recursive = (BOOL) GetTagData(PICH_Recursive, pichandler->recursive, tags);
		pichandler->includedirs = (BOOL) GetTagData(PICH_IncludeDirs, pichandler->includedirs, tags);
		pichandler->simplescanning = (BOOL) GetTagData(PICH_SimpleScanning, pichandler->simplescanning, tags);

		bufferpercent = GetTagData(PICH_BufferPercent, pichandler->bufferpercent, tags);
		if (bufferpercent < pichandler->bufferpercent)
		{
			FlushLoadedList(pichandler->loadedlist, 0, bufferpercent);
		}
		pichandler->bufferpercent = bufferpercent;

		rejectpattern = (char *) GetTagData(PICH_Reject, ~0, tags);
		if ((ULONG) rejectpattern != ~0)
		{
			if (rejectpattern = _StrDup(rejectpattern))
			{
				char *t;

				//!!!!! ObtainSemaphore(

				Forbid();
				t = pichandler->rejectpattern;
				pichandler->rejectpattern = rejectpattern;
				Free(t);
				Permit();

				//!!!!! ReleaseSemaphore(
			}
			else
			{
				errors++;
			}
		}

		success = (errors == 0);
	}

	return success;
}



/*--------------------------------------------------------------------

	success = PicHandler_SetSortMode(pichandler, sortmode, reverse)

--------------------------------------------------------------------*/

static BOOL PicHandler_SetSortMode(struct PicHandler *pichandler, int sortmode, BOOL reverse)
{
	BOOL success = TRUE;

	if (pichandler)
	{
		pichandler->sortmode = sortmode;			// set as default

		if (pichandler->filelist)
		{
			ObtainSemaphore(&pichandler->filelist->locksemaphore);

			if (sortmode == SORTMODE_RANDOM || pichandler->filelist->sortmode != sortmode || pichandler->filelist->reverse != reverse)
			{
				success = SortFileList(pichandler->filelist, sortmode, reverse);
			}

			ReleaseSemaphore(&pichandler->filelist->locksemaphore);
		}
	}

	return success;
}



/*********************************************************************
----------------------------------------------------------------------

	ID = PicHandler_Seek(pichandler,mode,n);

	move the current ID in filelist

----------------------------------------------------------------------
*********************************************************************/

ULONG PicHandler_Seek(struct PicHandler *pichandler, int mode, int n)
{
	ULONG ID = PICID_UNDEFINED;

	if (pichandler)
	{
		if (pichandler->filelist)
		{
			struct FileListNode *fnode;

			ObtainSemaphore(&pichandler->filelist->locksemaphore);

			fnode = pichandler->filelist->currentnode =
				SeekCurrentFileNode(pichandler->filelist, mode, n);

			if (fnode)
			{
				ID = fnode->picID;
			}

			ReleaseSemaphore(&pichandler->filelist->locksemaphore);
		}
	}

	return ID;
}


/*********************************************************************
----------------------------------------------------------------------

	ID = PicHandler_GetID(pichandler,mode,n);

	get any ID from the filelist relative to the current ID,
	without actually moving the current ID

----------------------------------------------------------------------
*********************************************************************/

ULONG PicHandler_GetID(struct PicHandler *pichandler, int mode, int n)
{
	ULONG ID = PICID_UNDEFINED;

	if (pichandler)
	{
		if (pichandler->filelist)
		{
			struct FileListNode *fnode;

			ObtainSemaphore(&pichandler->filelist->locksemaphore);

			fnode = SeekCurrentFileNode(pichandler->filelist, mode, n);

			if (fnode)
			{
				ID = fnode->picID;
			}

			ReleaseSemaphore(&pichandler->filelist->locksemaphore);
		}
	}

	return ID;
}


/*********************************************************************
----------------------------------------------------------------------

	ID = PicHandler_GetCurrentID(pichandler);

	get the current ID from the filelist

----------------------------------------------------------------------
*********************************************************************/

ULONG PicHandler_GetCurrentID(struct PicHandler *pichandler)
{
	ULONG ID = PICID_UNDEFINED;

	if (pichandler)
	{
		if (pichandler->filelist)
		{
			struct FileListNode *fnode;

			ObtainSemaphore(&pichandler->filelist->locksemaphore);

			fnode = SeekCurrentFileNode(pichandler->filelist, PHLIST_RELATIVE, 0);

			if (fnode)
			{
				ID = fnode->picID;
			}

			ReleaseSemaphore(&pichandler->filelist->locksemaphore);
		}
	}

	return ID;
}


/*********************************************************************
----------------------------------------------------------------------

	n = PicHandler_GetNumber(pichandler);

	get the number of pictures in the list

----------------------------------------------------------------------
*********************************************************************/

ULONG PicHandler_GetNumber(struct PicHandler *pichandler)
{
	ULONG n = 0;

	if (pichandler)
	{
		if (pichandler->filelist)
		{
			ObtainSemaphore(&pichandler->filelist->locksemaphore);

			n = pichandler->filelist->numentries;

			ReleaseSemaphore(&pichandler->filelist->locksemaphore);
		}
	}

	return n;
}


/*********************************************************************
----------------------------------------------------------------------

	success = PicHandler_RequestPic(pichandler, ID);

	request the picture with the given ID

----------------------------------------------------------------------
*********************************************************************/

BOOL PicHandler_RequestPic(struct PicHandler *pichandler, ULONG id)
{
	BOOL success = FALSE;

	if (pichandler)
	{
		if (PicHandler_ObtainPicture(pichandler, id))
		{
			PostPHMessage(pichandler, PHNOTIFY_ID_AVAILABLE, id);
			PostPHMessage(pichandler, PHNOTIFY_PIC_AVAILABLE, id);
			PicHandler_ReleasePicture(pichandler, id);

			DB(kprintf(">> cache hit\n"));
		}
		else
		{
			DB(kprintf(">> cache miss\n"));
		}

		success = RequestIDToLoad(pichandler, id);
	}

	return success;
}



/*********************************************************************
----------------------------------------------------------------------

	picture = PicHandler_ObtainPicture(pichandler, ID);

	obtain access to the picture with the given ID

----------------------------------------------------------------------
*********************************************************************/

PICTURE *PicHandler_ObtainPicture(struct PicHandler *pichandler, ULONG id)
{
	PICTURE *pic = NULL;

	if (pichandler)
	{
		struct LoadedList *llist;

		if (llist = pichandler->loadedlist)
		{
			struct Node *node, *nextnode;

			ObtainSemaphore(&llist->locksemaphore);

			if (node = llist->list->lh_Head)
			{
				while (nextnode = node->ln_Succ)
				{
					struct LoadedNode *lnode = (struct LoadedNode *) node;

					if (lnode->picID == id)
					{
						lnode->lockcount++;
						pic = lnode->picture;
						break;
					}

					node = nextnode;
				}
			}

			ReleaseSemaphore(&llist->locksemaphore);
		}
	}

	return pic;
}



/*********************************************************************
----------------------------------------------------------------------

	PicHandler_ReleasePicture(pichandler, ID);

	release access to the picture with the given ID

----------------------------------------------------------------------
*********************************************************************/

void PicHandler_ReleasePicture(struct PicHandler *pichandler, ULONG id)
{
	if (pichandler)
	{
		struct LoadedList *llist;

		if (llist = pichandler->loadedlist)
		{
			struct Node *node, *nextnode;

			ObtainSemaphore(&llist->locksemaphore);

			if (node = llist->list->lh_Head)
			{
				while (nextnode = node->ln_Succ)
				{
					struct LoadedNode *lnode = (struct LoadedNode *) node;

					if (lnode->picID == id)
					{
						assert (lnode->lockcount > 0);
						lnode->lockcount--;
						break;
					}

					node = nextnode;
				}
			}

			ReleaseSemaphore(&llist->locksemaphore);
		}
	}
}


/*<<*****************************************************************/





/*********************************************************************
----------------------------------------------------------------------

	LOADER

----------------------------------------------------------------------
*>>******************************************************************/


/*--------------------------------------------------------------------

	DeleteLoadedNode(loadednode)

--------------------------------------------------------------------*/

void DeleteLoadedNode(struct LoadedNode *lnode)
{
	if (lnode)
	{
		//assert (lnode->lockcount == 0);

		if (lnode->lockcount)
		{
			printf("*** MysticView debugging message: DeleteLoadedNode() lnode->lockcount == %ld\n", lnode->lockcount);
		}

		DeletePicture(lnode->picture);
		Free(lnode->fullname);
		Free(lnode);
	}
}

/*--------------------------------------------------------------------

	loadednode = CreateLoadedNode(ID, picture, fullname, pri, loadtime)

--------------------------------------------------------------------*/

struct LoadedNode *CreateLoadedNode(ULONG picID, PICTURE *pic, char *fullname, float pri, float loadtime)
{
	struct LoadedNode *lnode;
	BOOL success = FALSE;

	if (lnode = Malloclear(sizeof(struct LoadedNode)))
	{
		if (lnode->fullname = _StrDup(fullname))
		{
			ULONG width, height, pixelformat;

			if (GetPictureAttrs(pic, PICATTR_Width, &width,
				PICATTR_Height, &height,
				PICATTR_PixelFormat, &pixelformat, TAG_DONE) == 3)
			{
				lnode->numbytes = width * height *
					(pixelformat == PIXFMT_0RGB_32 ? 4 : 1);
			}
			else
			{
				assert(FALSE);
			}

			lnode->picture = pic;
			lnode->picID = picID;
			lnode->node.ln_Type = NT_USER;

			loadtime = sqrt(loadtime);
			lnode->pri = MIN(loadtime * pri, 1.0f);
			lnode->node.ln_Pri = (int)(pri * 127.0f);

			success = TRUE;
		}
	}

	if (!success)
	{
		DeleteLoadedNode(lnode);
		lnode = NULL;
	}

	return lnode;
}


/*--------------------------------------------------------------------

	DeleteLoadedList(llist)

--------------------------------------------------------------------*/

void DeleteLoadedList(struct LoadedList *llist)
{
	if (llist)
	{
		if (llist->list)
		{
			struct LoadedNode *lnode;

			while (lnode = (struct LoadedNode *) RemHead(llist->list))
			{
				llist->numbytes -= lnode->numbytes;
				llist->numentries -= 1;
				DeleteLoadedNode(lnode);
			}

			DeleteList(llist->list);
		}

		assert(llist->numbytes == 0);
		assert(llist->numentries == 0);

		Free(llist);
	}
}


/*--------------------------------------------------------------------

	llist = CreateLoadedList()

--------------------------------------------------------------------*/

struct LoadedList *CreateLoadedList(void)
{
	struct LoadedList *llist;
	BOOL success = FALSE;

	if (llist = Malloclear(sizeof(struct LoadedList)))
	{
		InitSemaphore(&llist->locksemaphore);
		if (llist->list = CreateList())
		{
			llist->numentries = 0;
			success = TRUE;
		}
	}

	if (!success)
	{
		DeleteLoadedList(llist);
		llist = NULL;
	}

	return llist;
}



/*--------------------------------------------------------------------

	bufferfull = BufferFull(llist, bufferpercent)

--------------------------------------------------------------------*/

BOOL BufferFull(struct LoadedList *llist, int bufferpercent)
{
	BOOL full = FALSE;

	if (llist)
	{
		full = (llist->numbytes * 100 / 
			(AvailMem(MEMF_ANY) /*- llist->numbytes*/) > bufferpercent);

		//		printf("bufferrate: %ld  buffered: %ld  free: %ld\n", llist->numbytes * 100 / (AvailMem(MEMF_ANY) - llist->numbytes), llist->numbytes, AvailMem(MEMF_ANY));
		//		if (full) printf("buffer IS FULL.\n");
	}

	return full;
}


/*--------------------------------------------------------------------

	somethingflushed = FlushLoadedList(llist, flushmode, bufferpercent)

	flushmode = 0	flush NUMCACHE, flush memory if buffer full
	flushmode = 1	flush ALL
	flushmode = 2	flush very low priority pics if buffer full

--------------------------------------------------------------------*/

BOOL FlushLoadedList(struct LoadedList *llist, int flushmode, int bufferpercent)
{
	BOOL somethingflushed = FALSE;
	struct Node *node, *nextnode;

	assert (llist);

	ObtainSemaphore(&llist->locksemaphore);

	if (flushmode == 2)
	{
		for(;;)
		{
			BOOL couldremove = FALSE;
			if (node = llist->list->lh_TailPred)
			{
				while (nextnode = node->ln_Pred)
				{
					struct LoadedNode *lnode = (struct LoadedNode *) node;

					if (lnode->lockcount == 0 && node->ln_Pri < FLUSHLOADEDTHRESHOLD)
					{
						DB(kprintf("!>> flushing %lx (low pri): %s\n", lnode->picID, lnode->fullname));

						Remove(node);

						llist->numbytes -= lnode->numbytes;
						llist->numentries--;
						DeleteLoadedNode(lnode);

						somethingflushed = TRUE;
						couldremove = TRUE;
						break;
					}

					node = nextnode;
				}
			}
			if (!couldremove || !BufferFull(llist, bufferpercent))
			{
				break;
			}
		}
	}




	if (flushmode == 0)
	{
		while (llist->numentries > NUMCACHE)
		{
			BOOL couldremove = FALSE;

			if (node = llist->list->lh_TailPred)
			{
				while (nextnode = node->ln_Pred)
				{
					struct LoadedNode *lnode = (struct LoadedNode *) node;

					if (lnode->lockcount == 0)
					{
						DB(kprintf("!>> flushing %lx (numcache): %s\n", lnode->picID, lnode->fullname));

						Remove(node);

						llist->numbytes -= lnode->numbytes;
						llist->numentries--;
						DeleteLoadedNode(lnode);

						somethingflushed = TRUE;
						couldremove = TRUE;
						break;
					}

					node = nextnode;
				}
			}

			if (!couldremove)
			{
				break;
			}
		}
	}



	if (flushmode != 2)
	{
		while (flushmode == 1 || BufferFull(llist, bufferpercent))
		{
			BOOL couldremove = FALSE;

			if (node = llist->list->lh_TailPred)
			{
				while (nextnode = node->ln_Pred)
				{
					struct LoadedNode *lnode = (struct LoadedNode *) node;

					if (lnode->lockcount == 0)
					{
						DB(kprintf("!>> flushing (memory): %s at pri %ld\n", lnode->fullname, lnode->node.ln_Pri));

						somethingflushed = TRUE;
						couldremove = TRUE;

						Remove(node);
						llist->numbytes -= lnode->numbytes;
						llist->numentries--;
						DeleteLoadedNode(lnode);

						break;
					}

					node = nextnode;
				}
			}


			if (!couldremove)
			{
				break;
			}
		}
	}


	ReleaseSemaphore(&llist->locksemaphore);


	return somethingflushed;
}




/*--------------------------------------------------------------------

	loadclosefunc

--------------------------------------------------------------------*/

void loadclosefunc(struct PicHandler *handler)
{
	assert(handler);

	DeleteLoadedList(handler->loadedlist);
	handler->loadedlist = NULL;

	DB(kprintf("*** loader closing down.\n"));
}



/*--------------------------------------------------------------------

	LowerLoadedListPriority(tlist)

	lower loadedlist's priorities

--------------------------------------------------------------------*/

void LowerLoadedListPriority(struct LoadedList *llist)
{
	struct Node *node, *nextnode;

	assert (llist);

	ObtainSemaphore(&llist->locksemaphore);

	if (node = llist->list->lh_TailPred)
	{
		while (nextnode = node->ln_Pred)
		{
			struct LoadedNode *lnode = (struct LoadedNode *) node;

			lnode->pri *= LOADEDFACTOR;
			node->ln_Pri = (int)(lnode->pri * 127.0f);

			node = nextnode;
		}
	}

	ReleaseSemaphore(&llist->locksemaphore);
}




/*--------------------------------------------------------------------

	loadinitfunc

--------------------------------------------------------------------*/

BOOL SAVEDS loadinitfunc(struct subtask *data)
{
	struct PicHandler *handler = (struct PicHandler *) data;

	assert(handler->loadedlist == NULL);

	if (handler->loadedlist = CreateLoadedList())
	{
		return TRUE;
	}

	loadclosefunc(handler);
	return FALSE;
}


/*--------------------------------------------------------------------

	loadfunc

--------------------------------------------------------------------*/

LONG SAVEDS loadfunc(struct subtask *subtask, BYTE abortsignal)
{

	struct PicHandler *handler;
	ULONG signals;
	ULONG abortsignals;

	struct ToLoadNode *tlnode;
	BOOL loading = FALSE;




	handler = (struct PicHandler *) ObtainData(subtask);

	abortsignals = 1 << abortsignal;



	do
	{
		Delay(4);

		if (SetSignal(0, 0) & abortsignals) break;

		tlnode = NULL;


		//	remove a node from the toloadlist

		ObtainSemaphore(&handler->toloadlist->locksemaphore);

		if (!IsListEmpty(handler->toloadlist->list))
		{
			if (tlnode = (struct ToLoadNode *) RemHead(handler->toloadlist->list))
			{
				handler->toloadlist->numentries--;
			}
		}

		ReleaseSemaphore(&handler->toloadlist->locksemaphore);



		//	process this node

		if (tlnode)
		{

			struct Node *node, *nextnode;
			struct LoadedNode *loadednode = NULL;



			//	check if the current node's ID is already in the loadedlist
			//	and unlink it from the list

			ObtainSemaphore(&handler->loadedlist->locksemaphore);

			if (node = handler->loadedlist->list->lh_Head)
			{
				while (nextnode = node->ln_Succ)
				{
					struct LoadedNode *lnode = (struct LoadedNode *) node;
					if (lnode->picID == tlnode->picID)
					{
						loadednode = lnode;

						Remove((struct Node *) lnode);
						handler->loadedlist->numentries--;
						handler->loadedlist->numbytes -= lnode->numbytes;

						break;
					}
					node = nextnode;
				}
			}

			ReleaseSemaphore(&handler->loadedlist->locksemaphore);




			//	insert already loaded node to the loadedlist

			if (loadednode)
			{
				ObtainSemaphore(&handler->loadedlist->locksemaphore);

		//		if (tlnode->primary)
		//		{
		//			loadednode->pri = 1.0f;
		//			loadednode->node.ln_Pri = 127;
		//		}
		//		else
		//		{
					loadednode->pri = MIN(loadednode->pri + tlnode->pri, 1.0f);
					loadednode->node.ln_Pri = (int)(loadednode->pri * 127.0f);
		//		}

				Enqueue(handler->loadedlist->list, (struct Node *) loadednode);
				handler->loadedlist->numentries++;
				handler->loadedlist->numbytes += loadednode->numbytes;

				ReleaseSemaphore(&handler->loadedlist->locksemaphore);


				if (tlnode->primary)
				{
					PostPHMessage(handler, PHNOTIFY_PIC_AVAILABLE, loadednode->picID);
				}
				PostPHMessage(handler, PHNOTIFY_ID_AVAILABLE, loadednode->picID);
			}


			else	//!


			//	if no node was found, load the picture and create a new node

		//!	if (!loadednode)
			{

				if (!tlnode->primary && BufferFull(handler->loadedlist, handler->bufferpercent))
				{
					FlushLoadedList(handler->loadedlist, 2, handler->bufferpercent);
				}


				if (!tlnode->primary && BufferFull(handler->loadedlist, handler->bufferpercent))
				{
			//		DB(kprintf("NOT YET loading %s.\n", tlnode->fullname));
				}
				else
				{
					clock_t t1, t2;
					float loadtime;

					APTR picture;
					LONG error = 0;

					//	flush loadedlist

					FlushLoadedList(handler->loadedlist, 0, handler->bufferpercent);


					if (!loading)
					{
						loading = TRUE;
						PostPHMessage(handler, PHNOTIFY_LOADING, TRUE);
					}



					//	load picture.

					if (tlnode->isdirectory)
					{
						t1 = clock();
						picture = MakePicture(NULL, 64, 64, NULL);
						t2 = clock();
						loadtime = (float) (t2 - t1) / (float) CLOCKS_PER_SEC;
					}
					else
					{
						DB(kprintf("loading...\n"));

						PostPHMessage(handler, PHNOTIFY_ID_LOADING, tlnode->picID, TRUE);

						t1 = clock();
						picture = LoadPicture(tlnode->fullname, GGFX_ErrorCode, &error, TAG_DONE);
						t2 = clock();
						loadtime = (float) (t2 - t1) / (float) CLOCKS_PER_SEC;

						PostPHMessage(handler, PHNOTIFY_ID_LOADING, tlnode->picID, FALSE);

						DB(kprintf("loading ready\n"));
					}


					if (!picture /*&& !tlnode->isdirectory && (!SetSignal(0, 0) & abortsignals)*/)
					{
						if (error == ERROR_NO_FREE_STORE)
						{
							DB(kprintf("NO FREE STORE... flushing\n"));
							if (FlushLoadedList(handler->loadedlist, 0, handler->bufferpercent) ||
									FlushLoadedList(handler->loadedlist, 1, handler->bufferpercent))
							{
								if (tlnode->primary)
								{
									PostPHMessage(handler, PHNOTIFY_ID_LOADING, tlnode->picID, TRUE);

									error = 0;

							DB(kprintf("retrying...\n"));

									t1 = clock();
									picture = LoadPicture(tlnode->fullname, GGFX_ErrorCode, &error, TAG_DONE);
									t2 = clock();
									loadtime = (float) (t2 - t1) / (float) CLOCKS_PER_SEC;

									PostPHMessage(handler, PHNOTIFY_ID_LOADING, tlnode->picID, FALSE);
								}
							}

							if (!picture)
							{
								/*
								**	OUT OF MEMORY
								**
								*/
								PostPHMessage(handler, PHNOTIFY_ID_ERROR_LOADING, tlnode->picID, error);
							}
						}
						else
						{
							/*
							**	OTHER/UKNOWN ERROR
							**
							*/

							PostPHMessage(handler, PHNOTIFY_ID_ERROR_LOADING, tlnode->picID, error);
						}
					}


					if (picture)
					{
						if (handler->autocrop)
						{
							DoPictureMethod(picture, PICMTHD_AUTOCROP, NULL);
						}

						DB(kprintf(">> creating loadednode for %lx: %s\n", tlnode->picID, tlnode->fullname));

						if (loadednode = CreateLoadedNode(tlnode->picID, picture, tlnode->fullname,
							tlnode->primary ? 1.0f : tlnode->pri, loadtime))
						{

						}
						else
						{
							DeletePicture(picture);
						}
					}



					//	insert newly loaded node to the loadedlist

					if (loadednode)
					{
						ObtainSemaphore(&handler->loadedlist->locksemaphore);

						Enqueue(handler->loadedlist->list, (struct Node *) loadednode);
						handler->loadedlist->numentries++;
						handler->loadedlist->numbytes += loadednode->numbytes;

						ReleaseSemaphore(&handler->loadedlist->locksemaphore);

						if (tlnode->primary)
						{
							PostPHMessage(handler, PHNOTIFY_PIC_AVAILABLE, loadednode->picID);
						}
						PostPHMessage(handler, PHNOTIFY_ID_AVAILABLE, loadednode->picID);
					}



					ObtainSemaphore(&handler->toloadlist->locksemaphore);
					if (/*handler->toloadlist->numentries == 0 &&*/ loading)
					{
						loading = FALSE;
						PostPHMessage(handler, PHNOTIFY_LOADING, FALSE);
					}
					ReleaseSemaphore(&handler->toloadlist->locksemaphore);

				}
			}

			DeleteToLoadNode(tlnode);

			LowerLoadedListPriority(handler->loadedlist);

		}

		signals = SetSignal(0, abortsignals);


	} while (!(signals & abortsignals));


	loadclosefunc(handler);


	ReleaseData(subtask);

	return NULL;
}

/*<<*****************************************************************/






/*********************************************************************
----------------------------------------------------------------------

	PicHandler_FreePicInfo(picinfo);

----------------------------------------------------------------------
*********************************************************************/

void PicHandler_FreePicInfo(PICINFO *picinfo)
{
	if (picinfo)
	{
		Free(picinfo->fullname);
		Free(picinfo->formatname);
		Free(picinfo);
	}
}


/*********************************************************************
----------------------------------------------------------------------

	picinfo = PicHandler_GetPicInfo(pichandler, ID);

	get picture info block from an ID

----------------------------------------------------------------------
*********************************************************************/

PICINFO *PicHandler_GetPicInfo(struct PicHandler *pichandler, int id)
{
	PICINFO *picinfo = NULL;

	if (pichandler && id != PICID_UNDEFINED)
	{
		if (pichandler->filelist)
		{
			struct Node *node, *nextnode;
			struct FileListNode *fnode;
			int listindex = 0;
			BOOL found = FALSE;

			ObtainSemaphore(&pichandler->filelist->locksemaphore);

			node = pichandler->filelist->list->lh_Head;

			while (nextnode = node->ln_Succ)
			{
				fnode = (struct FileListNode *) node;
				if (fnode->picID == id)
				{
					found = TRUE;
					break;
				}
				node = nextnode;
				listindex++;
			}

			if (found)
			{
				if (picinfo = Malloclear(sizeof(PICINFO)))
				{
					picinfo->listindex = listindex;
					picinfo->fullname = _StrDup(fnode->fullname);
					picinfo->isdirectory = fnode->isdirectory;
					picinfo->formatname = _StrDup(fnode->formatID);
				}
			}

			ReleaseSemaphore(&pichandler->filelist->locksemaphore);
		}
	}

	return picinfo;
}


/*********************************************************************
----------------------------------------------------------------------

	PicHandler_GetName(pichandler, ID);

	get filename of an ID

----------------------------------------------------------------------
*********************************************************************/

char *PicHandler_GetName(struct PicHandler *pichandler, int id)
{
	char *fullname = NULL;

	if (pichandler && id != PICID_UNDEFINED)
	{
		if (pichandler->filelist)
		{
			struct FileListNode *fnode;

			ObtainSemaphore(&pichandler->filelist->locksemaphore);

			fnode = FindFileListNode(pichandler->filelist, id);

			if (fnode)
			{
				fullname = _StrDup(fnode->fullname);
			}

			ReleaseSemaphore(&pichandler->filelist->locksemaphore);
		}
	}

	return fullname;
}


/*********************************************************************
----------------------------------------------------------------------

	PicHandler_GetListIndex(pichandler, ID);

	get an ID's list index (this sucks)

----------------------------------------------------------------------
*********************************************************************/

LONG PicHandler_GetListIndex(struct PicHandler *pichandler, int id)
{
	LONG index = -1;

	if (pichandler && id != PICID_UNDEFINED)
	{
		if (pichandler->filelist)
		{
			struct Node *node = pichandler->filelist->list->lh_Head, *nextnode;

			ObtainSemaphore(&pichandler->filelist->locksemaphore);

			index = 0;

			while (nextnode = node->ln_Succ)
			{
				if (((struct FileListNode *) node)->picID == id)
				{
					break;
				}
				node = nextnode;
				index++;
			}

			ReleaseSemaphore(&pichandler->filelist->locksemaphore);
		}
	}

	return index;
}




void PicHandler_LockFileList(struct PicHandler *ph)
{
	ObtainSemaphore(&ph->filelist->locksemaphore);
}

void PicHandler_UnLockFileList(struct PicHandler *ph)
{
	ReleaseSemaphore(&ph->filelist->locksemaphore);
}


/*********************************************************************
----------------------------------------------------------------------

	BOOL PicHandler_Rename(pichandler, ID, fullname);
	rename a picture with given ID

----------------------------------------------------------------------
*********************************************************************/

BOOL PicHandler_Rename(struct PicHandler *pichandler, int id, char *fullname)
{
	BOOL success = FALSE;

	if (pichandler && id != PICID_UNDEFINED && fullname)
	{
		if (pichandler->filelist && pichandler->toloadlist)
		{
			struct FileListNode *fnode;
			struct ToLoadNode *tlnode;
			char *newname1 = _StrDup(fullname);
			char *newname2 = _StrDup(fullname);

			if (newname1 && newname2)
			{
				ObtainSemaphore(&pichandler->filelist->locksemaphore);
				ObtainSemaphore(&pichandler->toloadlist->locksemaphore);

				fnode = FindFileListNode(pichandler->filelist, id);
				tlnode = FindToLoadNode(pichandler->toloadlist, id);

				if (fnode)
				{
					Free(fnode->fullname);
					fnode->fullname = newname1;
					newname1 = NULL;

					success = TRUE;

					if (tlnode)
					{
						Free(tlnode->fullname);
						tlnode->fullname = newname2;
						newname2 = NULL;
					}
				}

				ReleaseSemaphore(&pichandler->toloadlist->locksemaphore);
				ReleaseSemaphore(&pichandler->filelist->locksemaphore);

				if (newname1) Free(newname1);
				if (newname2) Free(newname2);
			}
		}
	}

	return success;
}


/*********************************************************************
----------------------------------------------------------------------

	BOOL PicHandler_Remove(pichandler, ID);
	remove a picture with given ID

----------------------------------------------------------------------
*********************************************************************/

BOOL PicHandler_Remove(struct PicHandler *pichandler, int id)
{
	BOOL success = FALSE;

	if (pichandler && id != PICID_UNDEFINED)
	{
		if (pichandler->filelist && pichandler->toloadlist)
		{
			struct FileListNode *fnode;
			struct ToLoadNode *tlnode;

			ObtainSemaphore(&pichandler->filelist->locksemaphore);
			ObtainSemaphore(&pichandler->toloadlist->locksemaphore);

			fnode = FindFileListNode(pichandler->filelist, id);
			tlnode = FindToLoadNode(pichandler->toloadlist, id);

			if (fnode)
			{
				RemFileListNode(pichandler->filelist, fnode);
				DeleteFileListNode(fnode);
				pichandler->filelist->numentries--;

				success = TRUE;

				if (tlnode)
				{
					Remove(&tlnode->node);
					DeleteToLoadNode(tlnode);
					pichandler->toloadlist->numentries--;
				}
			}

			ReleaseSemaphore(&pichandler->toloadlist->locksemaphore);
			ReleaseSemaphore(&pichandler->filelist->locksemaphore);
		}
	}

	return success;
}




/*********************************************************************
----------------------------------------------------------------------

	success = PicHandler_AppendScanList(filepatternlist, tags)

	append a filepatternlist to a running pichandler from outside

	tags:
		PICH_Recursive		(override pichandler default)
		PICH_Reject			(override pichandler default)
		PICH_IncludeDirs	(override pichandler default)
		PICH_SimpleScanning	(override pichandler default)

----------------------------------------------------------------------
*********************************************************************/

BOOL STDARGS PicHandler_AppendScanList(
	char **patternlist,
	...)
{
	BOOL result = FALSE;

	if (patternlist)
	{
		struct SignalSemaphore *scanlist_semaphore;
		BOOL locked = FALSE;
		int retrycount = 20;

		do
		{
			Forbid();

			scanlist_semaphore = FindSemaphore(PICLIST_SEMAPHORE_NAME);

			if (scanlist_semaphore)
			{
				locked = AttemptSemaphore(scanlist_semaphore);
			}

			Permit();

			if (locked || !scanlist_semaphore)
			{
				break;
			}

			Delay(5);

		} while (--retrycount);


		if (locked)
		{
			struct ScanList *scanlist = (struct ScanList *) scanlist_semaphore;
			va_list va;
			struct TagItem *tags;

			char *rejectpattern;
			BOOL recursive, includedirs, simplescanning;

			char *filepattern;

			va_start(va, patternlist);
#ifdef __MORPHOS__
			tags = (struct TagItem *) va->overflow_arg_area;
#else
			tags = (struct TagItem *) va;
#endif

			recursive = (BOOL) GetTagData(PICH_Recursive, TRUE, tags);
			includedirs = (BOOL) GetTagData(PICH_IncludeDirs, FALSE, tags);
			simplescanning = (BOOL) GetTagData(PICH_SimpleScanning, FALSE, tags);
			rejectpattern = (char *) GetTagData(PICH_Reject, NULL, tags);


			result = TRUE;

			while (result && (filepattern = *patternlist++))
			{
				result = !!ScanList_AddPattern(scanlist,
					filepattern, rejectpattern, recursive, includedirs, simplescanning);
			}

			ReleaseSemaphore(scanlist_semaphore);
		}

	}

	return result;
}


/*********************************************************************
----------------------------------------------------------------------

	stringlist = PicHandler_CreateFileList(pichandler)

----------------------------------------------------------------------
*********************************************************************/

char **PicHandler_CreateFileList(struct PicHandler *ph)
{
	BOOL success = FALSE;
	char **list = NULL;

	if (ph->filelist)
	{
		int n;

		ObtainSemaphore(&ph->filelist->locksemaphore);

		n = PicHandler_GetNumber(ph);

		if (n > 0)
		{
			if (list = CreateStringList(n))
			{
				struct Node *node = ph->filelist->list->lh_Head, *nextnode;
				struct FileListNode *fnode;
				int i = 0;

				success = TRUE;

				while (nextnode = node->ln_Succ)
				{
					fnode = (struct FileListNode *) node;

					list[i] = _StrDup(fnode->fullname);

					if (!list[i])
					{
						success = FALSE;
						break;
					}
					i++;
					node = nextnode;
				}

				if (success) assert(i == n);
			}
		}

		ReleaseSemaphore(&ph->filelist->locksemaphore);
	}

	if (!success)
	{
		DeleteStringList(list);
		list = NULL;
	}

	return list;
}

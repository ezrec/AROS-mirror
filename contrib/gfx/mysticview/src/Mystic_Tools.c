/*********************************************************************
----------------------------------------------------------------------

	MysticView
	Toolbox

----------------------------------------------------------------------
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <intuition/intuition.h>
#include <libraries/asl.h>

#include <proto/asl.h>
#include <proto/graphics.h>
#include <proto/dos.h>
#include <proto/guigfx.h>
#include <clib/alib_protos.h>

#include "Mystic_Global.h"
#include "Mystic_Window.h"
#include "Mystic_Tools.h"
#include "Mystic_Settings.h"
#include "Mystic_Subtask.h"


#ifdef __MORPHOS__
FUNC_HOOK(ULONG, FileReqFilter, struct Hook *, hook, struct FileRequester *, fr, struct AnchorPath *, ap)
#else
ULONG ASM SAVEDS FileReqFilterFunc(
	REG(a0) struct Hook *hook,
	REG(a2) struct FileRequester *fr,
	REG(a1) struct AnchorPath *ap)
#endif
{
	BOOL include = TRUE;

	if (ap && fr && hook)
	{
		struct FileInfoBlock *fib = &ap->ap_Info;

		if (fib)
		{
			if (fib->fib_DirEntryType < 0)
			{
				char *fullname;
				if (fullname = FullName(fr->fr_Drawer, fib->fib_FileName))
				{
					include = IsPicture(fullname, TAG_DONE);
					Free(fullname);
				}
			}
		}
	}

	return (ULONG) include;
}




/*--------------------------------------------------------------------

	len = StrLen(string)

	also handles NULL strings

--------------------------------------------------------------------*/

int StrLen(char *s)
{
	int l = 0;

	if (s)
	{
		l = strlen(s);
	}

	return l;
}



/*--------------------------------------------------------------------

	exists = Exists(filename)

--------------------------------------------------------------------*/

BOOL Exists(char *filename)
{
	BOOL exists = FALSE;

	if (filename)
	{
		BPTR lock;
		if (lock = Lock(filename, ACCESS_READ))
		{
			exists = TRUE;
			UnLock(lock);
		}
	}

	return exists;
}


/*--------------------------------------------------------------------

	list = CreateList(void)

	create a double linked list

--------------------------------------------------------------------*/

struct List *CreateList(void)
{
	struct List *list;

	if (list = Malloc(sizeof(struct List)))
	{
		NewList(list);
	}

	return list;
}



/*--------------------------------------------------------------------

	DeleteNode(node)

	free a node

--------------------------------------------------------------------*/

void DeleteNode(struct Node *node)
{
	if (node)
	{
		Free(node->ln_Name);
		Free(node);
	}
}



/*--------------------------------------------------------------------

	DeleteList(list)

	free a list

--------------------------------------------------------------------*/

void DeleteList(struct List *list)
{
	if (list)
	{
		struct Node *node;
		while (!IsListEmpty(list))
		{
			if (node = list->lh_Head)
			{
				Remove(node);
				DeleteNode(node);
			}
		}
		Free(list);
	}
}


/*--------------------------------------------------------------------

	count = CountListEntries(list)

	count number of entries in a list

--------------------------------------------------------------------*/

int CountListEntries(struct List *list)
{
	int count = 0;

	if (list)
	{
		if (!IsListEmpty(list))
		{
			struct Node *node;
			struct Node *nextnode;

			node = list->lh_Head;
			while (nextnode = node->ln_Succ)
			{
				count++;
				node = nextnode;
			}
		}
	}

	return count;
}



/*--------------------------------------------------------------------

	array = CreateArrayFromList(list)

	create an array from a double linked list

--------------------------------------------------------------------*/

char **CreateArrayFromList(struct List *list)
{
	char **array = NULL;

	int count;

	if (count = CountListEntries(list))
	{
		if (array = Malloclear((count + 1) * sizeof(char *)))
		{
			struct Node *node;
			struct Node *nextnode;
			int i = 0;
			BOOL error = FALSE;

			node = list->lh_Head;
			while ((nextnode = node->ln_Succ) && !error)
			{
				error = !(array[i++] = _StrDup(node->ln_Name));
				node = nextnode;
			}

			if (error)
			{
				char **p = array;
				while (*p)
				{
					Free(*p++);
				}
				Free(array);
				array = NULL;
			}
		}
	}

	return array;
}


/*--------------------------------------------------------------------

	list = CreateListFromArray(array)

	create a double linked list from an array

--------------------------------------------------------------------*/

struct List *CreateListFromArray(char **array)
{
	struct List *list;

	if (list = CreateList())
	{
		char **t;

		if (t = array)
		{
			BOOL error = FALSE;
			struct Node *node;

			while (*t && !error)
			{
				error = TRUE;
				if (node = Malloclear(sizeof(struct Node)))
				{
					if (node->ln_Name = _StrDup(*t))
					{
						AddTail(list, node);
						error = FALSE;
					}
					else
					{
						Free(node);
					}
				}
				t++;
			}

			if (error)
			{
				DeleteList(list);
				list = NULL;
			}
		}
	}

	return list;
}


/*--------------------------------------------------------------------

	path = DupPath(filename)

	create pathpart from a filename

--------------------------------------------------------------------*/

char *DupPath(char *filename)
{
	char *path = NULL;

	if (filename)
	{
		char c, *s;

		if (s = PathPart(filename))
		{
			c = *s;
			*s = 0;
			path = _StrDup(filename);
			*s = c;
		}
	}

	return path;
}

/*--------------------------------------------------------------------

	trimmedstring = _StrDupTrim(string)

	create a duplicate of a string, but trim leading and
	trailing blank characters

--------------------------------------------------------------------*/

char *_StrDupTrim(char *string)
{
	char *trimmed = NULL;
	int len;

	if (string)
	{
		if (len = strlen(string))
		{
			char *a, *b;
			char c;

			a = string;
			while (c = *a)
			{
				if (c == 32 || c == 9 || c == 13 || c == 10)
				{
					a++;
					len--;
				}
				else
				{
					break;
				}
			}

			if (len)
			{
				if (len > 1)
				{
					b = a + len - 1;
					while (c = *b)
					{
						if (c == 32 || c == 9 || c == 13 || c == 10)
						{
							b--;
							len--;
						}
						else
						{
							break;
						}
					}
				}

				if (len)
				{
					if (trimmed = Malloc(len+1))
					{
						strncpy(trimmed, a, len);
						trimmed[len] = (char) 0;
					}
				}
			}
		}
	}

	return trimmed;
}


/*--------------------------------------------------------------------

	list = CreateStringList(entries)
	create a list of strings with a given number of entries

--------------------------------------------------------------------*/

char **CreateStringList(int num)
{
	return Malloclear((num + 1) * sizeof(char *));
}


/*--------------------------------------------------------------------

	DeleteStringList(list)
	delete a list of strings

--------------------------------------------------------------------*/

void DeleteStringList(char **list)
{
	if (list)
	{
		char **p = list;
		while(*p)
		{
			Free(*p++);
		}
		Free(list);
	}
}


/*--------------------------------------------------------------------

	list = DupStringList(list)
	create a duplicate of a list of strings

--------------------------------------------------------------------*/

char **DupStringList(char **list)
{
	char **newlist = NULL;
	if (list)
	{
		int i, num = 0;
		char **p = list;

		while (*p++) num++;

	//	if (num > 0)
	//	{
			if (newlist = CreateStringList(num))
			{
				for (i = 0; i < num; ++i)
				{
					if (!(newlist[i] = _StrDup(list[i])))
					{
						DeleteStringList(newlist);
						newlist = NULL;
						break;
					}
				}
			}
	//	}
	}

	return newlist;
}


/*--------------------------------------------------------------------

	list = CreateFilePatternList(firstwbarg, numargs)

	create an array of filenames or filename
	patterns from an array of wbargs

--------------------------------------------------------------------*/

char **CreateFilePatternList(struct WBArg *wbargs, int numargs)
{
	char **wbargarray = NULL;

	if (wbargs && numargs > 0)
	{
		if (wbargarray = CreateStringList(numargs))
		{
			char filenamebuffer[MAXFULLNAMELEN];
			int i;

			for (i = 0; i < numargs; ++i)
			{
				if (NameFromLock(wbargs->wa_Lock, filenamebuffer, MAXFULLNAMELEN))
				{
					if (AddPart(filenamebuffer, wbargs->wa_Name, MAXFULLNAMELEN))
					{
						if (!(wbargarray[i] = _StrDup(filenamebuffer)))
						{
							DeleteStringList(wbargarray);
							break;
						}
					}
				}
				wbargs++;
			}
		}
	}

	return wbargarray;
}


/*--------------------------------------------------------------------

	DeleteFilePatternList(list)

	delete a file pattern list

--------------------------------------------------------------------*/

void DeleteFilePatternList(char **list)
{
	DeleteStringList(list);
}


/*--------------------------------------------------------------------

	identical = SameFile(char *file1, char *file2)

	check if 2 filenames reference the same physical file

--------------------------------------------------------------------*/

BOOL SameFile(char *f1, char *f2)
{
	BOOL identical = FALSE;

	BPTR lock1, lock2;

	lock1 = Lock(f1, ACCESS_READ);
	lock2 = Lock(f2, ACCESS_READ);

	if (lock1 && lock2)
	{
		identical = (SameLock(lock1, lock2) == LOCK_SAME);
	}

	UnLock(lock2);
	UnLock(lock1);

	return identical;
}


/*--------------------------------------------------------------------

	datestamp = GetFileDate(filename)

--------------------------------------------------------------------*/

struct DateStamp *GetFileDate(char *filename)
{
	struct DateStamp *d = NULL;
	BPTR lock;

	if (lock = Lock(filename, ACCESS_READ))
	{
		struct FileInfoBlock *fib;

		if (fib = AllocDosObject(DOS_FIB, NULL))
		{
			if (Examine(lock, fib))
			{
				if (d = Malloc(sizeof(struct DateStamp)))
				{
					memcpy(d, &fib->fib_Date, sizeof(struct DateStamp));
				}
			}
			FreeDosObject(DOS_FIB, fib);
		}

		UnLock(lock);
	}
	return d;
}




/*********************************************************************
----------------------------------------------------------------------

	fullname = FullName(pathname, filename)

	concatenate path and filename

----------------------------------------------------------------------
*********************************************************************/

char *FullName(char *pathname, char *filename)
{
	char *fullname = NULL;

	if (pathname && filename)
	{
		BOOL success = FALSE;
		int fulllen = strlen(pathname) + strlen(filename) + 20;

		if (fullname = Malloc(fulllen+1))
		{
			strcpy(fullname, pathname);
			if (AddPart(fullname, filename, fulllen))
			{
				success = TRUE;
			}
		}

		if (!success)
		{
			Free(fullname);
			fullname = NULL;
		}
	}

	return fullname;
}


/*********************************************************************
----------------------------------------------------------------------

	char **LoadStringList(char *filename)

	read a list file

----------------------------------------------------------------------
*********************************************************************/

char **LoadStringList(char *filename)
{
	char *linebuffer, *line;
	FILE *fp;
	char **picturelist = NULL;
	struct List list;
	struct Node *node;
	BOOL error = TRUE;
	int count = 0;

	NewList(&list);


	if (fp = fopen(filename, "r"))
	{
		if (linebuffer = Malloc(2048))
		{
			error = FALSE;

			while (fgets(linebuffer, 2048, fp) && !error)
			{
				if (line = _StrDupTrim(linebuffer))
				{
					if (node = Malloclear(sizeof(struct Node)))
					{
						node->ln_Name = line;
						AddTail(&list, node);
						count++;
					}
					else
					{
						Free(line);
						error = TRUE;
					}
				}
			}

			Free(linebuffer);
		}
		fclose(fp);
	}

	if (!error && count)
	{
		if (picturelist = Malloc(sizeof(char *) * (count + 1)))
		{
			picturelist[count] = NULL;
		}
	}

	if (count)
	{
		int i = 0;

		while (!IsListEmpty(&list))
		{
			if (node = list.lh_Head)
			{
				if (picturelist)
				{
					picturelist[i++] = _StrDup(node->ln_Name);
				}
				Remove(node);
				Free(node->ln_Name);
				Free(node);
			}
		}
	}

	return picturelist;
}


/*********************************************************************
----------------------------------------------------------------------

	success = ModeRequest(req, win, settings, title)

----------------------------------------------------------------------
*********************************************************************/

BOOL ModeRequest(struct ScreenModeRequester *sr, struct Window *win, struct mainsettings *settings, STRPTR title)
{
	BOOL success = FALSE;

	if (sr && settings)
	{
		//LOCK(settings);

		if (AslRequestTags(sr,
				ASLSM_TitleText, title,
				ASLSM_Window, win,
				ASLSM_SleepWindow, TRUE,

				ASLSM_InitialDisplayID, settings->modeID,
				ASLSM_InitialDisplayWidth, settings->scrwidth,
				ASLSM_InitialDisplayHeight, settings->scrheight,
				ASLSM_InitialDisplayDepth, settings->depth,
				ASLSM_DoWidth, TRUE,
				ASLSM_DoHeight, TRUE,
				ASLSM_DoDepth, TRUE,
				ASLSM_DoOverscanType, TRUE,
				ASLSM_PropertyFlags, 0,
				ASLSM_PropertyMask, DIPF_IS_EXTRAHALFBRITE | DIPF_IS_DUALPF | DIPF_IS_PF2PRI,
				TAG_DONE))
		{
			settings->modeID = sr->sm_DisplayID;
			settings->scrwidth = sr->sm_DisplayWidth;
			settings->scrheight = sr->sm_DisplayHeight;
			settings->depth = sr->sm_DisplayDepth;
			success = TRUE;
		}

		//UNLOCK(settings);
	}

	return success;
}



/*********************************************************************
----------------------------------------------------------------------

	PathRequest(req, title, pathname, *newpathname)

----------------------------------------------------------------------
*********************************************************************/

BOOL PathRequest(struct FileRequester *fr, struct Window *win, STRPTR title,
	char *pathname,	char **newpathname)
{
	BOOL result = FALSE;

	if (AslRequestTags(fr,
			ASLFR_TitleText, title,
			ASLFR_RejectIcons, TRUE,
			ASLFR_Window, win,
			ASLFR_SleepWindow, TRUE,
			ASLFR_InitialDrawer, pathname ? pathname : "",
			ASLFR_DrawersOnly, TRUE,
			TAG_DONE))
	{
		char *newp;

		if (newp = _StrDup(fr->fr_Drawer))
		{
			*newpathname = newp;
			result = TRUE;
		}
	}

	return result;
}


/*********************************************************************
----------------------------------------------------------------------

	FileRequest(req, title, pathname, filename, *newpathname, *newfilename)

----------------------------------------------------------------------
*********************************************************************/

BOOL FileRequest(struct FileRequester *fr, struct Window *win, STRPTR title,
	char *pathname, char *filename,
	char **newpathname, char **newfilename)
{
	BOOL result = FALSE;

	if (AslRequestTags(fr,
			ASLFR_TitleText, title,
			ASLFR_RejectIcons, TRUE,
			ASLFR_Window, win,
			ASLFR_SleepWindow, TRUE,
			ASLFR_InitialFile, filename ? filename : "",
			ASLFR_InitialDrawer, pathname ? pathname : "",
			TAG_DONE))
	{
		char *newp, *newf;

		if (newp = _StrDup(fr->fr_Drawer))
		{
			if (newf = _StrDup(fr->fr_File))
			{
				*newpathname = newp;
				*newfilename = newf;
				result = TRUE;
			}
			else
			{
				Free(newp);
			}
		}
	}

	return result;
}

/*********************************************************************
----------------------------------------------------------------------

	SaveRequest(req, title, pathname, filename, *newpathname, *newfilename)

----------------------------------------------------------------------
*********************************************************************/

BOOL SaveRequest(struct FileRequester *fr, struct Window *win, STRPTR title,
	char *pathname, char *filename,
	char **newpathname, char **newfilename)
{
	BOOL result = FALSE;

	if (AslRequestTags(fr,
			ASLFR_TitleText, title,
			ASLFR_RejectIcons, TRUE,
			ASLFR_Window, win,
			ASLFR_SleepWindow, TRUE,
			ASLFR_DoSaveMode, TRUE,
			ASLFR_InitialFile, filename ? filename : "",
			ASLFR_InitialDrawer, pathname ? pathname : "",
			TAG_DONE))
	{
		char *newp, *newf;

		if (newp = _StrDup(fr->fr_Drawer))
		{
			if (newf = _StrDup(fr->fr_File))
			{
				*newpathname = newp;
				*newfilename = newf;
				result = TRUE;
			}
			else
			{
				Free(newp);
			}
		}
	}

	return result;
}


/*********************************************************************
----------------------------------------------------------------------

	filepatternlist = MultiFileRequest(req, win, title,
		pathname, filename, *newpathname, *newfilename)

----------------------------------------------------------------------
*********************************************************************/

char **MultiFileRequest(struct FileRequester *fr, struct Window *win,
	char *title, char *pathname, char *filename,
	char **newpathname, char **newfilename, BOOL onlypictures, char *rejectpattern)
{
	char **filepatternlist = NULL;
	char *rejpat = NULL;
	struct Hook filereqfilterhook;

	char nonemptystring[] = "#?";

	if (rejectpattern)
	{
		int l = StrLen(rejectpattern);
		if (l > 0)
		{
			l = l * 2 + 2;
			if (rejpat = Malloc(l))
			{
				if (ParsePatternNoCase(rejectpattern, rejpat, l) == -1)
				{
					Free(rejpat);
					rejpat = NULL;
				}
			}
		}
	}


#ifdef __MORPHOS__
	filereqfilterhook.h_Entry = (HOOKFUNC) &FileReqFilter;
#else
	filereqfilterhook.h_Entry = (HOOKFUNC) FileReqFilterFunc;
#endif
	filereqfilterhook.h_SubEntry = (HOOKFUNC) NULL;
	filereqfilterhook.h_Data = NULL;

	if(AslRequestTags(fr,ASLFR_TitleText,title,
			ASLFR_FilterFunc, onlypictures ? &filereqfilterhook : NULL,
			ASLFR_RejectIcons, TRUE,
			ASLFR_RejectPattern, rejpat,
			ASLFR_Window, win,
			ASLFR_SleepWindow, TRUE,
			ASLFR_InitialFile, filename ? filename : nonemptystring,
			ASLFR_InitialDrawer, pathname ? pathname : "",
			ASLFR_DoMultiSelect, TRUE,
			ASLFR_DoPatterns, TRUE,
			TAG_DONE))
	{
		char *newp, *newf;

		if (newp = _StrDup(fr->fr_Drawer))
		{
			*newpathname = newp;
		}

		if (newf = _StrDup(fr->fr_File))
		{
			*newfilename = newf;
		}

		if (fr->fr_NumArgs)
		{
			filepatternlist = CreateFilePatternList(fr->fr_ArgList, fr->fr_NumArgs);
		}
	}

	Free(rejpat);

	return filepatternlist;
}


/*********************************************************************
----------------------------------------------------------------------

	SetAlternateWindowPosition(screen, window)

----------------------------------------------------------------------
*********************************************************************/

void SetAlternateWindowPosition(struct mvscreen *scr, struct mvwindow *win)
{
	UWORD visibleWidth, visibleHeight;
	UWORD visibleLeft, visibleTop;
	ULONG modeID;


	LOCK(win);
	LOCK(scr);

	visibleWidth = scr->screen->Width;
	visibleHeight = scr->screen->Height;

	if ((modeID = GetVPModeID(&scr->screen->ViewPort)) != INVALID_ID)
	{
		DisplayInfoHandle dih;

		if(dih = FindDisplayInfo(modeID))
		{
			struct DimensionInfo di;

			if(GetDisplayInfoData(dih, (UBYTE*) &di, sizeof(di), DTAG_DIMS, modeID))
			{
				visibleWidth = di.TxtOScan.MaxX - di.TxtOScan.MinX;
				visibleHeight = di.TxtOScan.MaxY - di.TxtOScan.MinY;
			}
		}
	}

	visibleLeft = scr->screen->ViewPort.DxOffset;
	visibleTop = scr->screen->ViewPort.DyOffset;

	win->otherwinpos[0] = visibleLeft;
	win->otherwinpos[1] = visibleTop;
	win->otherwinpos[2] = visibleWidth;
	win->otherwinpos[3] = visibleHeight;

	UNLOCK(scr);
	UNLOCK(win);
}

/*--------------------------------------------------------------------

	size = FileSize(filename);

--------------------------------------------------------------------*/

long FileSize(char *filename)
{
	long filesize = -1;

	if (filename)
	{
		BPTR lock;
		if (lock = Lock(filename, ACCESS_READ))
		{
			struct FileInfoBlock *fib;

			if (fib = AllocDosObject(DOS_FIB, NULL))
			{
				if (Examine(lock, fib))
				{
					if (fib->fib_DirEntryType < 0)
					{
						filesize = fib->fib_Size;
					}
				}

				FreeDosObject(DOS_FIB, fib);
			}

			UnLock(lock);
		}
	}

	return filesize;
}



/*--------------------------------------------------------------------

	success = CallSubTask(LONG (* function)(APTR data), APTR data, LONG *error)

	call a function as a subtask. the result should be != 0,
	since 0 is used as an indicator for failure.

--------------------------------------------------------------------*/

struct calldata
{
	LONG (* function)(APTR data);
	APTR userdata;
} *data;

LONG SAVEDS callsubfunc(struct subtask *subtask, BYTE abortsignal)
{
	LONG result = 0;

	if (data = ObtainData(subtask))
	{
		result = data->function(data->userdata);
		ReleaseData(subtask);
	}

	return result;
}

BOOL CallSubTask(LONG (* function)(APTR data), APTR data, int prio, int stack, LONG *result)
{
	BOOL success = FALSE;
	APTR subtask;
	struct calldata calldata;
	char name[] = "MysticView Subtask (%x)";

	calldata.function = function;
	calldata.userdata = data;

	subtask = SubTask(callsubfunc, (APTR) &calldata, stack, prio, name, NULL, FALSE);
	if (subtask)
	{
		LONG ret;
		ret = CloseSubTask(subtask);
		if (result) *result = ret;
		success = TRUE;
	}

	return success;
}



STRPTR STDARGS _StrDupCat(ULONG dummy, ...)
{
	va_list va;
	STRPTR *stringlist;
	int numargs = 0;
	int len = 0;
	STRPTR newstring = NULL;
	
	va_start(va, dummy);
#ifdef __MORPHOS__
	stringlist = (STRPTR *) va->overflow_arg_area;
#else
	stringlist = (STRPTR *) va;
#endif

	while (stringlist[numargs])
	{
		len += strlen((char *) stringlist[numargs++]);
	}
	
	if (numargs > 0)
	{
		newstring = Malloc(len + 1);
		if (newstring)
		{
			*newstring = 0;
			while (*stringlist)
			{
				strcat((char *) newstring, (char *) *stringlist++);
			}
		}
	}
		
	return newstring;
}

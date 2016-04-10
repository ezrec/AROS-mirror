/*--------------------------------------------------------------------------

 minad - MINimalistic AutoDoc

 Copyright (C) 2005-2012 Rupert Hausberger <naTmeg@gmx.net>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

--------------------------------------------------------------------------*/

#include "include.h"

/*------------------------------------------------------------------------*/

void RemPart(UBYTE *path)
{
	int i = _strlen(path) - 1;

	if (path[i] == ':')
		return;

	for (; i >= 0; i--) {
		if (path[i] == '/') {
			path[i] = 0; return;
		}
		else if (path[i] == ':') {
			path[i + 1] = 0; return;
		}
	}
	path[0] = 0;
}

/*------------------------------------------------------------------------*/

/* Returns the number of entrys in the given List */
LONG ListSize(struct List *list)
{
	struct Node *node;
	LONG size = 0L;

	if (!IsListEmpty(list))
		for (node = list->lh_Head; node->ln_Succ; node = node->ln_Succ)
			size++;

	return size;
}

/* Get a Node by it's number from the given List */
struct Node *GetNode(struct List *list, LONG num)
{
	struct Node *node;
	LONG count = 0L;

	if (!IsListEmpty(list))
		for (node = list->lh_Head; node->ln_Succ; node = node->ln_Succ)
			if (count++ == num)
				return node;

	return NULL;
}

/*------------------------------------------------------------------------*/

static void bcplcpy(UBYTE *out, BSTR in)
{
	UBYTE *ptr = (UBYTE *)BADDR(in);
	ULONG len = (ULONG)*ptr++;

	if (len) {
		_strncpy(out, ptr, len); out[len] = ':'; out[len+1] = '\0';
	} else
		*out = '\0';
}

/* Checks whether a given Assign is avail and returns the real path of it.
	ie. assign = "devs:", realpath will be 'morphos:devs' */
BOOL AssignAvail(UBYTE *assign, UBYTE *realpath)
{
	struct DosList *dol = LockDosList(LDF_READ | LDF_ASSIGNS);

	while ((dol = NextDosEntry(dol, LDF_ASSIGNS)))
   {
		UBYTE tmp[SIZE_NAME];

      bcplcpy(tmp, dol->dol_Name);
		if (!_stricmp(tmp, assign)) {
			if (dol->dol_Lock)
				NameFromLock(dol->dol_Lock, realpath, SIZE_PATH);
			
         UnLockDosList(LDF_READ | LDF_ASSIGNS); return TRUE;
		}
	}
	UnLockDosList(LDF_READ | LDF_ASSIGNS); return FALSE;
}

/*------------------------------------------------------------------------*/

UBYTE *ReadFileMagic(UBYTE *filename, LONG *size, UBYTE *magic_id, LONG magic_len)
{
	BPTR fh;

	*size = 0l;
	if ((fh = Open(filename, MODE_OLDFILE)))
	{
		struct FileInfoBlock fib;

		if (ExamineFH(fh, &fib) && fib.fib_Size > magic_len)
		{
			UBYTE *data = NULL;

			if ((data = (UBYTE *)myAlloc(fib.fib_Size + 1)))
			{
				if (Read(fh, data, fib.fib_Size) == fib.fib_Size)
				{
					if (_memcmp(data, magic_id, magic_len) == 0)
					{
						*(data + fib.fib_Size) = '\0';
						*size	= fib.fib_Size;
						return data;
					}
				}
				myFree(data);
			}
		}
		Close(fh);
	}
	return NULL;
}


/*------------------------------------------------------------------------*/

UBYTE *LowerString(UBYTE *string, UBYTE *buffer)
{
	register UBYTE *ptr1 = string, *ptr2 = buffer;
	register UBYTE c;

	while ((c = *ptr1++))
		*ptr2++ = ((c >= 'A') && (c <= 'Z')) ? (c + 32) : c;
	
	*ptr2 = 0; return buffer;
}

/*------------------------------------------------------------------------*/

void ErrorMsg(UBYTE *fmt, ...)
{
	struct Process *proc = (struct Process *)FindTask(NULL);
	UBYTE text[1024];
	va_list args;

	va_start(args, fmt);
	VSPrintf(text, fmt, args);
	va_end(args);

	if (proc->pr_CLI) {
		PutStr(text); PutStr("\n");
	} else {
		if ((IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 39))) {
			struct EasyStruct params = { sizeof(struct EasyStruct), 0, APP_PROGNAME" · Error", text, "Ok" };
			EasyRequestArgs(NULL, &params, NULL, NULL);
			CloseLibrary((struct Library *)IntuitionBase);
		}
	}
}

/*------------------------------------------------------------------------*/     


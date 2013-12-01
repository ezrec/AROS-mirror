/* OpenDrawer.c: rewritten in C by Matthias Rustler
 * OpenDrawer.e E source by: Jean-Marie COAT <agalliance@free.fr>
 * This version need at least: Scalos.library 40.34
 * Original source (version: 0.1) by: Eric Sauvageau
*/

/*
 * Arguments:
 *
 * PATH		The path to open. 	NOTE: Since version 40.36, if a file is specified as path, its parent dir will be used.
 * VIEWMODE	The viewing mode: 	0 = Default, 1 = Icon, 2 = Name, 3 = Size, 4 = Date, 5 = Time, 6 = Comment, 7 = Access.
 * SALL		Use if you want to display all files or as default or only icons.
 * ICONIFY	Switch, the window(argument PATH) will be iconified.
 *
*/

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/scalos.h>

#include <defs.h>

#include <stdlib.h>

// Switch: Debug output line.
#define d1(x)
#define d2(x)	x

#define TEMPLATE "PATH/A,VM=VIEWMODE/K/N,SALL/K/N,I/S"

static void clean_exit(CONST_STRPTR str);

static struct FileInfoBlock *info;
static struct RDArgs *rdargs;

struct ScalosBase *ScalosBase;

#ifdef __amigaos4__
struct ScalosIFace *IScalos = NULL;
#endif

const char *vers = "$VER: OpenDrawer 42.0 (20.08.2011) © 2002-2011 - The Scalos team";

int main(void)
{
	IPTR			argarray[] = {0,0,0,0};
	BYTE			path[512];
	BPTR			lock_Path = (BPTR) NULL;
	BPTR			lock_Parent = (BPTR) NULL;

	LONG viewmodes = 0;
	LONG showall = 0;
	BOOL iconify = FALSE;

	if (NULL == (rdargs = AllocDosObject(DOS_RDARGS, NULL)))
		clean_exit("Can't allocate RDArgs");

	rdargs->RDA_ExtHelp =
		"ViewModes: VIEWBYDEFAULT=0 / ICON=1 / NAME=2 / SIZE=3 / DATE=4 / TIME=5 / COMMENT=6 / ACCESS=7\n"
		"ShowAllMode=SALL: SHOWDEFAULT=0 / SHOWONLYICONS=1 / SHOWALL=2\n"
		"ICONIFY=I";

	if (NULL == ReadArgs(TEMPLATE, argarray, rdargs))
		clean_exit("Invalid arguments");

	if (NULL == (info = AllocDosObject(DOS_FIB, NULL)))
		clean_exit("AllocDosObject failed");

	// Open Scalos.library
	if (NULL == (ScalosBase = (struct ScalosBase *)OpenLibrary(SCALOSNAME, 40)))
		clean_exit("Can't open scalos.library v40");
#ifdef __amigaos4__
	IScalos = (struct ScalosIFace *)GetInterface((struct Library *)ScalosBase, "main", 1, NULL);
	if (NULL == IScalos)
		clean_exit("Failed to open scalos interface.");
#endif /* __amigaos4__ */

	if (argarray[1])
		{
		viewmodes = *((LONG *)argarray[1]);
		if (viewmodes > 7 || viewmodes < 0)
			clean_exit("Argument VIEWMODE must be between 0 and 7");
		}
	if (argarray[2])
		{
		showall = *((LONG *)argarray[2]);
		if (showall > 2 || showall < 0)
			clean_exit("Argument SALL must be between 0 and 2");
		}

	iconify = argarray[3] ? TRUE : FALSE;

	// If first argument isn't a drawer or a volume, opendrawer will use its parent path. 
	if (lock_Path = Lock((STRPTR)argarray[0], SHARED_LOCK))
		{
		Examine(lock_Path, info);
		if (info->fib_DirEntryType < 0)
			{
			d1(kprintf('Path is not drawer or volume:\n<\s>\nType = \d\n', argarray[0, info.direntrytype]));

			lock_Parent = ParentDir(lock_Path);
			NameFromLock(lock_Parent, path, 512);
			argarray[0] = (IPTR)path;

			d1(kprintf('Lock to its parent:\n<\s>\n', path));

			UnLock(lock_Parent);
			}
		UnLock(lock_Path);
		}


	if (SCA_OpenIconWindowTags(SCA_Path, argarray[0],
			SCA_ViewModes,	viewmodes,
			SCA_ShowAllMode,showall,
			SCA_Iconify,	iconify,
			TAG_END) == 0)
		clean_exit("Can't open Scalos window");

	clean_exit(NULL);
	return 0;
}


static void clean_exit(CONST_STRPTR str)
{
#ifdef __amigaos4__
	if (IScalos)
		{
		DropInterface((struct Interface *)IScalos);
		IScalos = NULL;
		}
#endif /* __amigaos4__ */
	if (ScalosBase)
		{
		CloseLibrary((struct Library *)ScalosBase);
		ScalosBase = NULL;
		}

	if (rdargs)
		{
		FreeArgs(rdargs);
		FreeDosObject(DOS_RDARGS, rdargs);
		}

	if (info)
		FreeDosObject(DOS_FIB, info);

	if (str)
		{
		PutStr(str);
		FPutC(Output(), '\n');
		exit(20);
		}
}

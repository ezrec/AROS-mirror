/*
**	Shutdown 2.0 package, LibMain.c module
**
**	Copyright © 1992 by Olaf `Olsen' Barthel
**		All Rights Reserved
*/

#include "ShutdownGlobal.h"

#include "shutdown.library.h"

STATIC STRPTR VersTag = VERSTAG;

	/* The structure expected by the library loader (auto-init). */

struct InitTable
{
	ULONG	 it_DataSize;	/* Data size to allocate. */
	APTR	*it_FuncTable;	/* Pointer to function table. */
	APTR	 it_DataInit;	/* Pointer to data initializers (remember InitStruct?). */
	APTR	 it_InitFunc;	/* The real library init function. */
};

	/* Protos for this module. */

struct ShutdownBase * __saveds __asm		LibInit(register __d0 struct ShutdownBase *ShutdownBase,register __a0 BPTR SegList);
struct ShutdownBase * __saveds __asm		LibOpen(register __a6 struct ShutdownBase *ShutdownBase);
BPTR __saveds __asm				LibClose(register __a6 struct ShutdownBase *ShutdownBase);
BPTR __saveds __asm				LibExpunge(register __a6 struct ShutdownBase *ShutdownBase);
LONG __saveds					LibNull(VOID);

	/* Pointer to library segment list. */

STATIC BPTR	LibSegList;

	/* ASCII library ID. */

UBYTE __aligned	LibName[]	= "shutdown.library";
UBYTE __aligned	LibID[]		= VSTRING;

	/* File version string. */

STATIC UBYTE	VersionTag[]	= VERSTAG;

	/* The list of library functions. */

APTR __aligned	LibFuncTab[] =
{
	LibOpen,		/* Standard library routines. */
	LibClose,
	LibExpunge,
	LibNull,

	RexxDispatch,
	AddShutdownInfoTagList,
	RemShutdownInfo,
	Shutdown,

	(APTR)-1		/* End marker. */
};

	/* The romtag needs this. */

struct InitTable LibInitTab =
{
	sizeof(struct ShutdownBase),	/* Lib base. */
	LibFuncTab,			/* Function table. */
	NULL,				/* No data init table (we'll do autoinit). */
	LibInit				/* Lib init routine. */
};

	/* LibInit(ShutdownBase,SegList):
	 *
	 *	Does the main library initialization, expects
	 *	all arguments in registers.
	 */

struct ShutdownBase * __saveds __asm
LibInit(register __d0 struct ShutdownBase *ShutdownBase,register __a0 BPTR SegList)
{
#warning This would kill AROS!!!
#ifndef _AROS
	SysBase = *(struct ExecBase **)4;
#endif

	if(DOSBase = (struct DosLibrary *)OpenLibrary("dos.library",37))
	{
		if(UtilityBase = OpenLibrary("utility.library",37))
		{
			if(IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",37))
			{
				if(GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",37))
				{
					if(GadToolsBase = OpenLibrary("gadtools.library",37))
					{
						RexxSysBase = (struct RxsLib *)OpenLibrary(RXSNAME,0);

							/* Remember segment list. */

						LibSegList = SegList;

							/* Fill in the library node head. */

						ShutdownBase -> LibNode . lib_Node . ln_Type	= NT_LIBRARY;
						ShutdownBase -> LibNode . lib_Node . ln_Name	= LibName;

							/* Set the remaining flags. */

						ShutdownBase -> LibNode . lib_Flags		= LIBF_SUMUSED | LIBF_CHANGED;
						ShutdownBase -> LibNode . lib_Version		= VERSION;
						ShutdownBase -> LibNode . lib_Revision		= REVISION;
						ShutdownBase -> LibNode . lib_IdString		= (APTR)LibID;

						InitSemaphore(&ShutdownBase -> BlockLock);
						InitSemaphore(&ShutdownBase -> DevBlockLock);
						InitSemaphore(&ShutdownBase -> AccessLock);
						InitSemaphore(&ShutdownBase -> ShutdownLock);

						NewList((struct List *)&ShutdownBase -> AccessList);
						NewList((struct List *)&ShutdownBase -> ShutdownList);

						ShutdownBase -> Main = (APTR)Main;

						GlobalBase = ShutdownBase;

							/* Return the result (surprise!). */

						return(ShutdownBase);
					}

					CloseLibrary(GfxBase);
				}

				CloseLibrary(IntuitionBase);
			}

			CloseLibrary(UtilityBase);
		}

		CloseLibrary(DOSBase);
	}

	return(NULL);
}

	/* LibOpen():
	 *
	 *	Library open routine.
	 */

struct ShutdownBase * __saveds __asm
LibOpen(register __a6 struct ShutdownBase *ShutdownBase)
{
		/* Increment open count and prevent delayed
		 * expunges.
		 */

	ShutdownBase -> LibNode . lib_OpenCnt++;
	ShutdownBase -> LibNode . lib_Flags &= ~LIBF_DELEXP;

		/* Return base pointer. */

	return(ShutdownBase);
}

	/* LibClose():
	 *
	 *	Closes the library.
	 */

BPTR __saveds __asm
LibClose(register __a6 struct ShutdownBase *ShutdownBase)
{
	BPTR SegList = NULL;

		/* Is the library user count ok? */

	if(ShutdownBase -> LibNode . lib_OpenCnt)
	{
			/* Decrement user count. */

		ShutdownBase -> LibNode . lib_OpenCnt--;

			/* Try the expunge. */

		SegList = LibExpunge(ShutdownBase);
	}

		/* Return the segment list, ramlib will know
		 * what to do with it.
		 */

	return(SegList);
}

	/* LibExpunge(ShutdownBase):
	 *
	 *	Expunge library, careful: this can be called by
	 *	ramlib without the rest of the library knowing
	 *	about it.
	 */

BPTR __saveds __asm
LibExpunge(register __a6 struct ShutdownBase *ShutdownBase)
{
	BPTR SegList = NULL;

		/* Is the user count zero, the delayed expunge flag
		 * set and do we have a valid segment list?
		 */

	if(!ShutdownBase -> LibNode . lib_OpenCnt && (ShutdownBase -> LibNode . lib_Flags & LIBF_DELEXP) && LibSegList && !ShutdownBase -> Running)
	{
			/* Remember segment list. */

		SegList = LibSegList;

			/* Set real segment list to zero which will
			 * hopefully keep us from getting expunged
			 * twice.
			 */

		LibSegList = NULL;

		if(RexxSysBase)
			CloseLibrary(RexxSysBase);

		if(GadToolsBase)
			CloseLibrary(GadToolsBase);

		if(GfxBase)
			CloseLibrary(GfxBase);

		if(IntuitionBase)
			CloseLibrary(IntuitionBase);

		if(UtilityBase)
			CloseLibrary(UtilityBase);

		if(DOSBase)
			CloseLibrary(DOSBase);

			/* Remove library from lib list. */

		Remove(&ShutdownBase -> LibNode . lib_Node);

			/* Free library/jump table memory. */

		FreeMem((BYTE *)((ULONG)ShutdownBase - ShutdownBase -> LibNode . lib_NegSize),ShutdownBase -> LibNode . lib_NegSize + ShutdownBase -> LibNode . lib_PosSize);
	}
	else
	{
		/* In any other case we'll set the delayed
		 * expunge flag (so next expunge call will
		 * hopefully wipe us from the lib list).
		 */

		ShutdownBase -> LibNode . lib_Flags |= LIBF_DELEXP;
	}

		/* Return segment list pointer. */

	return(SegList);
}

	/* LibNull():
	 *
	 *	Null function: do nothing, return NULL.
	 */

LONG __saveds
LibNull()
{
	return(NULL);
}


#include "mystic_global.h"

#ifdef __MORPHOS__
APTR CreateView_GATE(void);
void DeleteView_GATE(void);
void SetViewAttrs_GATE(void);
void SetViewPicture_GATE(void);
BOOL DrawOn_GATE(void);
void DrawOff_GATE(void);
void RefreshView_GATE(void);
void GetViewAttrs_GATE(void);
void SetViewRelative_GATE(void);
void SetViewStart_GATE(void);
void LockView_GATE(void);
#endif

/*****************************************************************************/

/* Reserved library-function and dummy entrypoint */
LONG LibNull()
{
	return(NULL);
}

/*****************************************************************************/

//#define SysBase MysticBase->ExecBase
struct ExecBase *SysBase = NULL;
struct MysticBase *MysticBase = NULL;

/*****************************************************************************/

ULONG LibVectors[] =
{
#ifdef __MORPHOS__
	FUNCARRAY_32BIT_NATIVE,
#endif
	(ULONG) &LibOpen,
	(ULONG) &LibClose,
#ifdef __MORPHOS__
	(ULONG) &LIBExpunge,
#else
	(ULONG) &LibExpunge,
#endif
	(ULONG) &LibNull,

#ifdef __MORPHOS__
	(ULONG) &CreateView_GATE,
	(ULONG) &DeleteView_GATE,
	(ULONG) &SetViewAttrs_GATE,
	(ULONG) &DrawOn_GATE,
	(ULONG) &DrawOff_GATE,
	(ULONG) &RefreshView_GATE,
	(ULONG) &GetViewAttrs_GATE,
	(ULONG) &SetViewStart_GATE,
	(ULONG) &SetViewRelative_GATE,
	(ULONG) &LockView_GATE,
#else
	(ULONG) &CreateView,
	(ULONG) &DeleteView,
	(ULONG) &SetViewAttrs,
	(ULONG) &DrawOn,
	(ULONG) &DrawOff,
	(ULONG) &RefreshView,
	(ULONG) &GetViewAttrs,
	(ULONG) &SetViewStart,
	(ULONG) &SetViewRelative,
	(ULONG) &LockView,
#endif

	0xFFFFFFFF
};

struct LibInitStruct
{
	ULONG	LibSize;
	void	*FuncTable;
	void	*DataTable;
	void	(*InitFunc)(void);
};

struct LibInitStruct LibInitStruct = {
	sizeof(struct MysticBase),
	LibVectors,
	NULL,
	(void (*)(void)) &LibInit
};

static const char LibVersion[] = "$VER: " LIBNAME " " LIBVER " " LIBDATE LIBTYPE;

static const struct Resident RomTag = {
	RTC_MATCHWORD,
	&RomTag,
	&RomTag + 1,
#ifdef __MORPHOS__
	RTF_PPC | RTF_AUTOINIT,
#else
	RTF_AUTOINIT,
#endif
	VERSION,
	NT_LIBRARY,
	0,
	LIBNAME,
	&LibVersion[6],
	&LibInitStruct
};

#ifdef __MORPHOS__
ULONG __amigappc__ = 1;
#endif

/*****************************************************************************/

struct Library * LIBENT LibInit(
		/* DO NOT change the order of these arguments! */
		REG(d0) struct Library *LibBase,
		REG(a0) BPTR Segment,
		REG(a6) struct ExecBase *ExecBase)
{
	SysBase = ExecBase;
	MysticBase = (struct MysticBase *) LibBase;

#ifndef __MORPHOS__
	if(ExecBase->LibNode.lib_Version < 39 ||
		!(ExecBase->AttnFlags & AFF_68020))
	{
		return(NULL);
	}
#endif

	MysticBase->LibNode.lib_Revision = REVISION;
	MysticBase->LibSegment = Segment;
	MysticBase->ExecBase = ExecBase;

	InitSemaphore(&MysticBase->LockSemaphore);

	return(LibBase);
}

/*****************************************************************************/

struct Library * LIBENT LibOpen(
#ifdef __MORPHOS__
	void)
{
	struct Library *LibBase = (struct Library *)REG_A6;
#else
	REG(a6) struct Library *LibBase)
{
#endif
	MysticBase->LibNode.lib_Flags &= ~LIBF_DELEXP;
	MysticBase->LibNode.lib_OpenCnt++;

	ObtainSemaphore(&MysticBase->LockSemaphore);

	if(MysticBase->LibNode.lib_OpenCnt == 1)
	{
		if(!MYSTIC_Init())
		{
			MYSTIC_Exit();

			MysticBase->LibNode.lib_OpenCnt--;

			LibBase = NULL;
		}
	}

	ReleaseSemaphore(&MysticBase->LockSemaphore);

	return(LibBase);
}

/*****************************************************************************/

#ifdef __MORPHOS__
BPTR LIBExpunge(void)
{
	struct Library *LibBase = (struct Library *)REG_A6;

	return LibExpunge(LibBase);
}
#endif

BPTR LIBENT LibExpunge(
	REG(a6) struct Library *LibBase)
{
	if(MysticBase->LibNode.lib_OpenCnt == 0 && 
		MysticBase->LibSegment != NULL)
	{
		BPTR TempSegment = MysticBase->LibSegment;

		Remove((struct Node *)MysticBase);

		FreeMem((APTR)((ULONG)(MysticBase) - (ULONG)(MysticBase->LibNode.lib_NegSize)),
			MysticBase->LibNode.lib_NegSize + MysticBase->LibNode.lib_PosSize);

		MysticBase = NULL;

		return(TempSegment);
	}
	else
	{
		MysticBase->LibNode.lib_Flags |= LIBF_DELEXP;

		return(NULL);
	}
}

/*****************************************************************************/

BPTR LIBENT LibClose(
#ifdef __MORPHOS__
	void)
{
	struct Library *LibBase = (struct Library *)REG_A6;
#else
	REG(a6) struct Library *LibBase)
{
#endif
	ObtainSemaphore(&MysticBase->LockSemaphore);

	if(MysticBase->LibNode.lib_OpenCnt > 0)
	{
		if(!(--MysticBase->LibNode.lib_OpenCnt))
			MYSTIC_Exit();
	}

	ReleaseSemaphore(&MysticBase->LockSemaphore);

	if(MysticBase->LibNode.lib_OpenCnt == 0 && 
		(MysticBase->LibNode.lib_Flags & LIBF_DELEXP))
	{
		return(LibExpunge(LibBase));
	}
	else
	{
		return(NULL);
	}
}

/*****************************************************************************/

#ifdef __MORPHOS__
APTR CreateView_GATE(void)
{
	struct Screen *scr = (struct Screen *)REG_A0;
	struct RastPort *rp = (struct RastPort *)REG_A1;
	TAGLIST tags = (TAGLIST)REG_A2;

	return CreateView(scr, rp, tags);
}

void DeleteView_GATE(void)
{
	APTR mview = (APTR)REG_A0;

	DeleteView(mview);
}

void SetViewAttrs_GATE(void)
{
	APTR mview = (APTR)REG_A0;
	TAGLIST tags = (TAGLIST)REG_A1;

	SetViewAttrs(mview, tags);
}

void SetViewPicture_GATE(void)
{
	APTR mview = (APTR)REG_A0;
	PICTURE *pic = (PICTURE *)REG_A1;

	SetViewPicture(mview, pic);
}

BOOL DrawOn_GATE(void)
{
	APTR mview = (APTR)REG_A0;

	return DrawOn(mview);
}

void DrawOff_GATE(void)
{
	APTR mview = (APTR)REG_A0;

	DrawOff(mview);
}

void RefreshView_GATE(void)
{
	APTR mview = (APTR)REG_A0;

	RefreshView(mview);
}

void GetViewAttrs_GATE(void)
{
	APTR mview = (APTR)REG_A0;
	TAGLIST tags = (TAGLIST)REG_A1;

	GetViewAttrs(mview, tags);
}

void SetViewRelative_GATE(void)
{
	APTR mview = (APTR)REG_A0;
	LONG x = (LONG)REG_D0;
	LONG y = (LONG)REG_D1;

	SetViewRelative(mview, x, y);
}

void SetViewStart_GATE(void)
{
	APTR mview = (APTR)REG_A0;
	LONG startx = (LONG)REG_D0;
	LONG starty = (LONG)REG_D1;

	SetViewStart(mview, startx, starty);
}

void LockView_GATE(void)
{
	APTR mview = (APTR)REG_A0;
	BOOL lock = (BOOL)REG_D0;

	LockView(mview, lock);
}

#endif


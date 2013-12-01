// ToolTip.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <graphics/rastport.h>
#include <graphics/gfxmacros.h>
#include <datatypes/pictureclass.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <utility/hooks.h>
#include <libraries/gadtools.h>
#include <prefs/pointer.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <dos/dostags.h>
#include <dos/datetime.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/locale.h>
#include <proto/diskfont.h>
#include <proto/cybergraphics.h>
#include <proto/iconobject.h>
#include <proto/timer.h>
#include "debug.h"
#include <proto/scalosgfx.h>
#include <proto/scalos.h>
#include <proto/scalosfiletypeplugin.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "scalos_structures.h"
#include "FsAbstraction.h"
#include "functions.h"
#include "locale.h"
#include "int64.h"
#include "Variables.h"
#include "DefIcons.h"
#include "TTLayout.h"

#include <math.h>

//----------------------------------------------------------------------------

#define	TOOLTIP_WINDOW_BORDER_HORIZ	(2 * TT_HORIZ_SPACE)
#define	TOOLTIP_WINDOW_BORDER_VERT	(4 * TT_VERT_SPACE)

#define	ELLIPSE_SEGMENT_TOPLEFT		8
#define	ELLIPSE_SEGMENT_TOPRIGHT	2
#define	ELLIPSE_SEGMENT_BOTTOMLEFT      4
#define	ELLIPSE_SEGMENT_BOTTOMRIGHT  	1

#define	COMBINE_RGB(r,b,g)      (0xff000000 | \
	(((r) & 0xff) << 16) | \
	(((g) & 0xff) << 8) | \
        ((b) & 0xff))
#define	GET_R_BYTE(color)	((UBYTE) ((color) >> 16))
#define	GET_G_BYTE(color)	((UBYTE) ((color) >> 8))
#define	GET_B_BYTE(color)	((UBYTE) (color))

//----------------------------------------------------------------------------

// local data structures


#define	TOOLTIP_PROCESSNAME	"Scalos_ToolTipProcess"

#define	TOOLTIP_PLUGINPATH	"Scalos:Plugins/FileTypes/"


struct ToolTipStart
	{
	struct internalScaWindowTask *tts_WindowTask;
	struct ttDef *tts_ttDef;
	struct IBox tts_WindowBox;	// valid Window box for tts_ttDef
	WORD tts_MouseX;		// x/y position of mouse pointer on tooltip start
	WORD tts_MouseY;
	};

struct ToolTipCmdFunc
	{
	CONST_STRPTR ttcf_CommandName;		// Name of the command.

	// Pointer to the function which implements this command.
	STRPTR (*ttcf_Function)(struct ScaToolTipInfoHookData *, CONST_STRPTR);
	};

struct ToolTipHideFunc
	{
	CONST_STRPTR tthf_CommandName;		// Name of the command.

	// Pointer to the function which implements this command.
	ULONG (*tthf_Function)(struct ttDef *, struct ScaToolTipInfoHookData *, CONST_STRPTR);
	};

struct ToolTipSignalData
	{
	struct Interrupt ttsd_Int; // The interrupt structure
	struct Process *ttsd_Process;	// the IconToolTipProcess
	};

struct ttInfo
	{
	struct IBox tti_WindowBox;
	struct ttDef *tti_Def;
	struct Window *tti_Window;
	struct TextFont *tti_Font;
	struct TTFontFamily tti_TTFont;
	struct BitMap *tti_BackgroundBitMap;

	BPTR tti_FileLock;

	WORD tti_PointerX;		// x/y position of mouse pointer on tooltip start
	WORD tti_PointerY;

	BOOL tti_AbovePointer;		// Flag: TRUE if tooltip window if above mouse pointer

	WORD tti_XRenderOffset;
	WORD tti_YRenderOffset;

	WORD tti_yTop;			// top of tooltip area
	WORD tti_yBottom;		// bottom of tooltip area
	};

#define	TT_RADIUS	10
#define	TT_PEAKHEIGHT	15
#define	TT_PEAKWIDTH	10
#define	AREAMAX		100

//----------------------------------------------------------------------------

// Tags for DisplayToolTip()

#define	DITT_FirstTag		(TAG_USER+17390)

#define	DITT_IconNode		(DITT_FirstTag+1)	// struct ScaIconNode *
#define	DITT_GadgetID		(DITT_FirstTag+2)	// enum sgttGadgetIDs
#define DITT_GadgetTextHook     (DITT_FirstTag+3)	// struct Hook *

//----------------------------------------------------------------------------

// local functions

static void DisplayToolTip(struct internalScaWindowTask *iwt, ULONG FirstTag, ...);
static SAVEDS(CONST_STRPTR) GetStringFunc(struct Hook *hook, ULONG dummy, ULONG *args);
static struct ttDef *CreateIconTooltip(struct internalScaWindowTask *iwt,
	struct ScaIconNode *in, BPTR fileLock);
static struct ttDef *CreateDiskTooltip(struct internalScaWindowTask *iwt, const struct ScaIconNode *in);
static struct ttDef *CreateAppIconTooltip(struct internalScaWindowTask *iwt, struct ScaIconNode *in);
static struct ttDef *CreateToolTipMsg(CONST_STRPTR Name, ULONG MsgID);
static struct ttDef *CreateGadgetTooltip(struct internalScaWindowTask *iwt,
	enum sgttGadgetIDs GadgetID, struct Hook *GadgetTextHook);
static SAVEDS(void) INTERRUPT IconToolTipProcess(struct ToolTipStart *startArg);
M68KFUNC_P2_PROTO(struct InputEvent *, ttInputHandler,
	A0, struct InputEvent *, eventList,
	A1, APTR, isData);
static struct BitMap *SaveTTWindowBackground(const struct IBox *ttWindowBox);
static void TTRenderWindow(struct ttInfo *tti);
static void DrawTooltipShapeBorder(struct ttInfo *tti, WORD PeakLeft, WORD PeakRight, WORD PeakPoint);
static void outline_ellipse(struct RastPort *rp, WORD center_x, WORD center_y, 
	WORD rx, WORD ry, WORD Segment, UBYTE color1, UBYTE color2);
static void ToolTipDraw(struct RastPort *rp, WORD Left, WORD Top);
static void ToolTopDrawEllipse(struct RastPort *rp, WORD center_x, WORD center_y,
	WORD rx, WORD ry, WORD Segment, UBYTE color1, UBYTE color2);
static void TTLayoutWindow(struct ttInfo *tti);
static SAVEDS(ULONG) ToolTip_BackFillFunc(struct Hook *bfHook, struct RastPort *rp, struct BackFillMsg *msg);
static STRPTR TooltipTTUserHookFunc(struct Hook *tooltipHook, struct ttDef *ttd, CONST_STRPTR defString);
static const struct ToolTipCmdFunc *ParseToolTipCmd(CONST_STRPTR *Line);
static const struct ToolTipHideFunc *ParseToolTipHide(CONST_STRPTR *Line);

static STRPTR Tooltip_VersionString(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static STRPTR Tooltip_IconName(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static STRPTR Tooltip_FileType(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static STRPTR Tooltip_FileSize(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static STRPTR Tooltip_Protection(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static STRPTR Tooltip_FileDate(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static STRPTR Tooltip_FileTime(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static STRPTR Tooltip_FileComment(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static STRPTR Tooltip_FibFileName(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static STRPTR Tooltip_DiskState(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static STRPTR Tooltip_VolumeCreatedDate(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static STRPTR Tooltip_VolumeCreatedTime(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static STRPTR Tooltip_DiskUsedCount(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static STRPTR Tooltip_DiskUsedPercent(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static STRPTR Tooltip_DiskUsedInUse(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static STRPTR Tooltip_DiskUsedFree(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static STRPTR Tooltip_RunPlugin(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static STRPTR Tooltip_LinkTarget(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);

static ULONG Tooltip_Hide_IsEmpty(struct ttDef *ttd, 
	struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);
static ULONG Tooltip_Hide_NoVolumeNode(struct ttDef *ttd, 
	struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args);

static struct ttDef *CloneToolTipDef(const struct ttDef *ttdOrig);
static void BlitBackgroundTransparent(struct ttInfo *tti, struct BitMap *MaskBitMap);
static void DrawWuLine(struct RastPort *rp, int X1, int Y1, UWORD LinePen);
static void outline_ellipse_antialias(struct RastPort *rp,
	WORD xc, WORD yc, WORD rx, WORD ry,
	WORD Segment, UBYTE color1, UBYTE color2);

static BOOL GetFileTypeFromTypeNode(struct ScaToolTipInfoHookData *ttshd);

//----------------------------------------------------------------------------

// local data items

static const struct ToolTipCmdFunc ToolTipCommandTable[] =
{
	{ "diskstate",		Tooltip_DiskState		},
	{ "diskusage",		Tooltip_DiskUsedCount		},
	{ "diskusagefree",	Tooltip_DiskUsedFree		},
	{ "diskusageinuse",	Tooltip_DiskUsedInUse		},
	{ "diskusagepercent",	Tooltip_DiskUsedPercent		},
	{ "fibfilename",	Tooltip_FibFileName		},
	{ "filecomment",	Tooltip_FileComment		},
	{ "filedate",		Tooltip_FileDate		},
	{ "fileprotection",	Tooltip_Protection		},
	{ "filesize",		Tooltip_FileSize		},
	{ "filetime",		Tooltip_FileTime		},
	{ "filetypestring",	Tooltip_FileType		},
	{ "iconname",		Tooltip_IconName		},
	{ "linktarget",		Tooltip_LinkTarget		},
	{ "plugin",		Tooltip_RunPlugin		},
	{ "versionstring",	Tooltip_VersionString		},
	{ "volumecreateddate",	Tooltip_VolumeCreatedDate	},
	{ "volumecreatedtime",	Tooltip_VolumeCreatedTime	},
	{ "volumeordevicename",	Tooltip_FibFileName		},
	{ NULL,			NULL },
};

static const struct ToolTipHideFunc ToolTipHideFunctionTable[] =
{
	{ "novolumenode",	Tooltip_Hide_NoVolumeNode	},
	{ "isempty",		Tooltip_Hide_IsEmpty		},
	{ NULL,			NULL },
};

static struct Hook ToolTipBackFillHook;
static struct DatatypesImage *ToolTipBackground;

//----------------------------------------------------------------------------

ULONG IconWinShowIconToolTip(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	if (CurrentPrefs.pref_EnableTooltips)
		{
		DisplayToolTip(iwt, 
			DITT_IconNode, in,
			TAG_END);
		}

	return 0;
}


ULONG IconWinShowGadgetToolTip(struct internalScaWindowTask *iwt, ULONG GadgetID, struct Hook *GadgetTextHook)
{
	d1(KPrintF("%s/%s/%ld: GadgetID=%ld\n", __FILE__, __FUNC__, __LINE__, GadgetID));

	if (CurrentPrefs.pref_EnableTooltips)
		{
		DisplayToolTip(iwt, 
			DITT_GadgetID, GadgetID,
			GadgetTextHook ? DITT_GadgetTextHook : TAG_IGNORE, GadgetTextHook,
			TAG_END);
		}

	return 0;
}


void ResetToolTips(struct internalScaWindowTask *iwt)
{
	struct internalScaWindowTask *iwtUnderPointer;
	struct ScaIconNode *inUnderPointer;
	struct Window *win;
	struct Task *ToolTipTask;

	// If a tooltip is currently being displayed, signal task to close it.
	Forbid();
	ToolTipTask = FindTask(TOOLTIP_PROCESSNAME);
	if (ToolTipTask)
		Signal(ToolTipTask, SIGBREAKF_CTRL_C);
	Permit();

	QueryObjectUnderPointer(&iwtUnderPointer, &inUnderPointer, NULL, &win);

	if (iwtUnderPointer)
		{
		if (inUnderPointer)
			ScalosUnLockIconList(iwtUnderPointer);
		SCA_UnLockWindowList();
		}

	MainWindowTask->miwt_LastIconUnderPtr = inUnderPointer;
	MainWindowTask->miwt_IconUnderPtrCount = 9999;
}


static void DisplayToolTip(struct internalScaWindowTask *iwt, ULONG FirstTag, ...)
{
	struct ToolTipStart startArg;
	struct TagItem *TagList;
	struct TagItem *ti;
	ULONG IconType;
	struct ttDef *ttd = NULL;
	WORD MouseX, MouseY;
	STRPTR tt;
	CONST_STRPTR fName = "";
	CONST_STRPTR Comment = "";
	BPTR fLock = NOT_A_LOCK;
	struct ScaIconNode *in = NULL;
	T_ExamineData *fib = NULL;
	LONG DirEntryType = 0;
	ULONG64 fSize;
	ULONG Protection = 0;
	struct DateStamp fDate = { 0, 0, 0 };
	va_list args;

	va_start(args, FirstTag);

	fSize = MakeU64(0);

	MouseX = iInfos.xii_iinfos.ii_Screen->MouseX;
	MouseY = iInfos.xii_iinfos.ii_Screen->MouseY;

	TagList = ScalosVTagList(FirstTag, args);
	if (TagList)
		{
		ti = FindTagItem(DITT_IconNode, TagList);
		if (ti)
			{
			do	{
				in = (struct ScaIconNode *) ti->ti_Data;

				if (NULL == in)
					break;

				tt = NULL;
				if (DoMethod(in->in_Icon, IDTM_FindToolType, "SCALOS_NOTOOLTIPS", &tt))
					{
					return;
					}

				GetAttr(IDTA_Type, in->in_Icon, &IconType);

				if (WBAPPICON == IconType)
					{
					ttd = CreateAppIconTooltip(iwt, in);
					}
				else
					{
					if (in->in_DeviceIcon)
						{
						ttd = CreateDiskTooltip(iwt, in);
						fLock = DiskInfoLock(in);
						if ((BPTR)NULL == fLock)
							break;

						if (!ScalosExamineBegin(&fib))
							break;

						if (!ScalosExamineLock(fLock, &fib))
							break;

						DirEntryType = ScalosExamineGetDirEntryTypeRoot(fib, fLock);
						fName = ScalosExamineGetName(fib);
						fSize = ScalosExamineGetSize(fib);
						Protection = ScalosExamineGetProtection(fib);
						fDate = *ScalosExamineGetDate(fib);
						Comment = ScalosExamineGetComment(fib);
						}
					else
						{
						BPTR oldDir = CurrentDir(in->in_Lock ? in->in_Lock : iwt->iwt_WindowTask.mt_WindowStruct->ws_Lock);

						do	{
							if (NULL == in->in_Name)
								break;

							fLock = Lock(in->in_Name, ACCESS_READ);
							if ((BPTR)NULL == fLock)
								{
								LONG LockError = IoErr();

								switch (LockError)
									{
								case ERROR_OBJECT_NOT_FOUND:
									ttd = CreateToolTipMsg(in->in_Name, MSGID_ICON_WITHOUT_FILE);
									break;
									}
								break;
								}

							if (!ScalosExamineBegin(&fib))
								break;

							if (!ScalosExamineLock(fLock, &fib))
								break;

							fSize = ScalosExamineGetSize(fib);
							Protection = ScalosExamineGetProtection(fib);
							fDate = *ScalosExamineGetDate(fib);
							Comment = ScalosExamineGetComment(fib);

							if (IsSoftLink(in->in_Name))
								{
								DirEntryType = ST_SOFTLINK;

								// Examine() returns the name of the link target
								// copy the original name of the link in place.
								fName = in->in_Name;
								}
							else
								{
								DirEntryType = ScalosExamineGetDirEntryTypeRoot(fib, fLock);
								fName = ScalosExamineGetName(fib);
								}

							ttd = CreateIconTooltip(iwt, in, fLock);

							if (NULL == ttd)
								break;

							} while (0);

						CurrentDir(oldDir);
						}
					}
				} while (0);
			}

		ti = FindTagItem(DITT_GadgetID, TagList);
		if (ti)
			{
			ttd = CreateGadgetTooltip(iwt, ti->ti_Data,
				(struct Hook *) GetTagData(DITT_GadgetTextHook,
					(ULONG) iInfos.xii_GlobalGadgetUnderPointer.ggd_GadgetTextHook, TagList));
			}
		}

	if (ttd)
		{
		char Buffer[128];
		struct RastPort rp;
		struct ScaToolTipInfoHookData ttu;
		struct Hook userHook;

		Scalos_InitRastPort(&rp);

		Scalos_SetFont(&rp, iInfos.xii_iinfos.ii_Screen->RastPort.Font, &ScreenTTFont);

		ttu.ttshd_IconNode = in;
		ttu.ttshd_FileLock = IS_VALID_LOCK(fLock) ? fLock : (BPTR)NULL;
		ttu.ttshd_FileName = fName;
		ttu.ttshd_Comment = Comment;
		ttu.ttshd_DirEntryType = DirEntryType;
		ttu.ttshd_FSize = fSize;
		ttu.ttshd_Date = fDate;
		ttu.ttshd_Protection = Protection;
		ttu.ttshd_Buffer = Buffer;
		ttu.ttshd_BuffLen = sizeof(Buffer);

		userHook.h_Data = &ttu;
		SETHOOKFUNC(userHook, TooltipTTUserHookFunc);

		TTLayout(&rp, ttd, &startArg.tts_WindowBox, TTL_Vertical, &userHook);

		startArg.tts_WindowTask = iwt;
		startArg.tts_ttDef = ttd;
		startArg.tts_MouseX = MouseX;
		startArg.tts_MouseY = MouseY;

		d1(kprintf("%s/%s/%ld: startArg=%08lx  iwt=%08lx  ttd=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, &startArg, iwt, ttd));

		if (MouseX == iInfos.xii_iinfos.ii_Screen->MouseX && MouseY == iInfos.xii_iinfos.ii_Screen->MouseY)
			{
			DoMethod(iwt->iwt_WindowTask.mt_MainObject, SCCM_RunProcess, 
				IconToolTipProcess,
				&startArg, sizeof(startArg),
				SCCV_RunProcess_NoReply);

			ttd = NULL;
			}

		Scalos_DoneRastPort(&rp);
		}

	va_end(args);

	if (TagList)
		FreeTagItems(TagList);
	if (ttd)
		TTDisposeItem(ttd);
	if (IS_VALID_LOCK(fLock))
		UnLock(fLock);
	ScalosExamineEnd(&fib);
}


static SAVEDS(CONST_STRPTR) GetStringFunc(struct Hook *theHook, ULONG dummy, ULONG *args)
{
	return GetLocString(*args);
}


void GetProtectionString(ULONG Protection, STRPTR String, size_t MaxLen)
{
	if (--MaxLen > 1)
		*String++ = (Protection & 0x80)		? 'H' : '-';
	if (--MaxLen > 1)
		*String++ = (Protection & FIBF_SCRIPT)	? 'S' : '-';
	if (--MaxLen > 1)
		*String++ = (Protection & FIBF_PURE)	? 'P' : '-';
	if (--MaxLen > 1)
		*String++ = (Protection & FIBF_ARCHIVE)	? 'A' : '-';
	if (--MaxLen > 1)
		*String++ = (Protection & FIBF_READ)	? '-' : 'R';
	if (--MaxLen > 1)
		*String++ = (Protection & FIBF_WRITE)	? '-' : 'W';
	if (--MaxLen > 1)
		*String++ = (Protection & FIBF_EXECUTE)	? '-' : 'E';
	if (--MaxLen > 1)
		*String++ = (Protection & FIBF_DELETE)	? '-' : 'D';

	*String = '\0';
}


static struct ttDef *CreateIconTooltip(struct internalScaWindowTask *iwt,
	struct ScaIconNode *in, BPTR fileLock)
{
	struct ttDef *ttd = NULL;
	struct FileTypeDef *ftd;

	ftd = FindFileType(iwt, in);
	d1(KPrintF("%s/%s/%ld:  ftd=%08lx  ttd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd, ftd ? ftd->ftd_ToolTipDef : NULL));
	if (ftd && ftd->ftd_ToolTipDef)
		ttd = CloneToolTipDef(ftd->ftd_ToolTipDef);

	ReleaseFileType(ftd);

	d1(KPrintF("%s/%s/%ld:  ttd=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd));
	if (NULL == ttd)
		{
		// Fallback if no filetype definition available
		ttd = TT_CreateItem(
			TTHoriz,
				TTMembers,
				TTTitleHook("fibfilename"), TT_TextStyle, FSF_BOLD, 
					TT_HAlign, GTJ_CENTER, TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
				TTTitleHook("filetypestring"), TT_HAlign, GTJ_CENTER, TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
				End,
			End,
			TT_Item, TT_CreateItem(TT_WideTitleBar, TRUE, TAG_DONE),

			TTTitleHook("versionstring"), TT_HiddenHook, "isempty versionstring", TT_HAlign, GTJ_CENTER, TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,

			TTHoriz,
				TTMembers,
					TTVert,
						TTMembers,
						TTItemID(MSGID_TOOLTIP_LASTCHANGE), TT_HAlign, GTJ_RIGHT, TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
						TTItemID(MSGID_TOOLTIP_SIZE), TT_HiddenHook, "isempty filesize", TT_HAlign, GTJ_RIGHT, TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
						TTItemID(MSGID_TOOLTIP_PROTECTION), TT_HAlign, GTJ_RIGHT, TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
						End,
					End,
					TTVert,
						TTMembers,
						TTHoriz,
							TTMembers,
							TTTitleHook("filedate"), TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
							TTTitleHook("filetime"), TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
							End,
						End,
						TTTitleHook("filesize"), TT_HiddenHook, "isempty filesize", TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
						TTTitleHook("fileprotection"), TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
						End,
					End,
				End,
			End,

			TTTitleHook("filecomment"), TT_HiddenHook, "isempty filecomment", TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
			End;
		}

	return ttd;
}


static struct ttDef *CreateDiskTooltip(struct internalScaWindowTask *iwt, const struct ScaIconNode *in)
{
	struct ttDef *ttd = NULL;
	struct FileTypeDef *ftd;

	ftd = FindFileType(iwt, in);
	d1(KPrintF("%s/%s/%ld:  ftd=%08lx  ttd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd, ftd ? ftd->ftd_ToolTipDef : NULL));
	if (ftd && ftd->ftd_ToolTipDef)
		ttd = CloneToolTipDef(ftd->ftd_ToolTipDef);

	ReleaseFileType(ftd);

	d1(KPrintF("%s/%s/%ld:  ttd=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd));
	if (NULL == ttd)
		{
		// Fallback if no filetype definition available
		ttd = TT_CreateTooltip(
			TTHoriz,
				TTMembers,
					TTTitleHook("fibfilename"), TT_TextStyle, FSF_BOLD, 
						TT_HAlign, GTJ_CENTER, TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
					TTItemID(MSGID_TOOLTIP_DISK), TT_HAlign, GTJ_CENTER, TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
				End,
			End,
			TT_Item, TT_CreateItem(TT_WideTitleBar, TRUE, TAG_DONE),

			TTHoriz,
				TTMembers,
					TTVert,
						TTMembers,
							TTItemID(MSGID_TOOLTIP_CREATED), TT_HiddenHook, "novolumenode", TT_HAlign, GTJ_RIGHT, TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
							TTItemID(MSGID_TOOLTIP_STATE), TT_HAlign, GTJ_RIGHT, TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
							TTItemID(MSGID_TOOLTIP_USED), TT_HAlign, GTJ_RIGHT, TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
						End,
					End,
					TTVert,
						TTMembers,
							TTHoriz,
								TT_HiddenHook, "novolumenode",
								TTMembers,
									TTTitleHook("volumecreateddate"), TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
									TTTitleHook("volumecreatedtime"), TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
								End,
							End,
							TTTitleHook("diskstate"), TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
							TTTitleHook("diskusage"), TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
						End,
					End,
				End,
			End,
			End;
		}

	return ttd;
}


static struct ttDef *CreateAppIconTooltip(struct internalScaWindowTask *iwt, struct ScaIconNode *in)
{
	struct ttDef *ttd = NULL;
	struct FileTypeDef *ftd;

	ftd = FindFileType(iwt, in);
	d1(KPrintF("%s/%s/%ld:  ftd=%08lx  ttd=%08lx\n", __FILE__, __FUNC__, __LINE__, ftd, ftd ? ftd->ftd_ToolTipDef : NULL));
	if (ftd && ftd->ftd_ToolTipDef)
		ttd = CloneToolTipDef(ftd->ftd_ToolTipDef);

	ReleaseFileType(ftd);

	d1(KPrintF("%s/%s/%ld:  ttd=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd));
	if (NULL == ttd)
		{
		// Fallback if no filetype definition available
		ttd = TT_CreateTooltip(
			TTHoriz,
				TTMembers,
				TTTitleHook("iconname"), TT_HiddenHook, "isempty iconname", 
					TT_TextStyle, FSF_BOLD, TT_HAlign, GTJ_CENTER, TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
				TTTitleHook("filetypestring"), TT_HAlign, GTJ_CENTER, TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
				End,
			End,
			End;
		}

	return ttd;
}


static struct ttDef *CreateToolTipMsg(CONST_STRPTR Name, ULONG MsgID)
{
	struct ttDef *ttd;

	ttd = TT_CreateTooltip(
		TTHoriz,
			TTMembers,
			TTobjName(Name), End,
			TTItemID(MsgID), TT_HAlign, GTJ_CENTER, TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
			End,
		End,
		End;

	return ttd;
}


static struct ttDef *CreateGadgetTooltip(struct internalScaWindowTask *iwt,
	enum sgttGadgetIDs GadgetID, struct Hook *GadgetTextHook)
{
	CONST_STRPTR ToolTipText;
	struct ttDef *ttd = NULL;

	d1(KPrintF("%s/%s/%ld: GadgetTextHook=%08lx\n", __FILE__, __FUNC__, __LINE__, GadgetTextHook));

	if (GadgetTextHook)
		{
		// fetch Tooltip text from hook
		ToolTipText = (CONST_STRPTR) CallHookPkt(GadgetTextHook, (APTR) iwt, (APTR) GadgetID);
		}
	else
		{
		// use built-in tooltip text according to gadget ID
		d1(KPrintF("%s/%s/%ld: GadgetID=%ld\n", __FILE__, __FUNC__, __LINE__, GadgetID));

		switch (GadgetID)
			{
		case SGTT_GADGETID_RightScroller:
			ToolTipText = GetLocString(MSGID_TOOLTIP_GADGET_RIGHTSCROLLER);
			break;
		case SGTT_GADGETID_BottomScroller:
			ToolTipText = GetLocString(MSGID_TOOLTIP_GADGET_BOTTOMSCROLLER);
			break;
		case SGTT_GADGETID_UpArrow:
			ToolTipText = GetLocString(MSGID_TOOLTIP_GADGET_UPARROW);
			break;
		case SGTT_GADGETID_DownArrow:
			ToolTipText = GetLocString(MSGID_TOOLTIP_GADGET_DOWNARROW);
			break;
		case SGTT_GADGETID_RightArrow:
			ToolTipText = GetLocString(MSGID_TOOLTIP_GADGET_RIGHTARROW);
			break;
		case SGTT_GADGETID_LeftArrow:
			ToolTipText = GetLocString(MSGID_TOOLTIP_GADGET_LEFTARROW);
			break;
		case SGTT_GADGETID_Iconify:
			ToolTipText = GetLocString(MSGID_TOOLTIP_GADGET_ICONIFY);
			break;
		case SGTT_GADGETID_StatusBar_Text:
			ToolTipText = GetLocString(MSGID_TOOLTIP_GADGET_STATUSBAR_TEXT);
			break;
		case SGTT_GADGETID_StatusBar_ReadOnly:
			ToolTipText = GetLocString(MSGID_TOOLTIP_GADGET_STATUSBAR_READONLY);
			break;
		case SGTT_GADGETID_StatusBar_Reading:
			ToolTipText = GetLocString(MSGID_TOOLTIP_GADGET_STATUSBAR_READING);
			break;
		case SGTT_GADGETID_StatusBar_Typing:
			ToolTipText = GetLocString(MSGID_TOOLTIP_GADGET_STATUSBAR_TYPING);
			break;
		case SGTT_GADGETID_StatusBar_ShowAll:
			ToolTipText = GetLocString(MSGID_TOOLTIP_GADGET_STATUSBAR_SHOWALL);
			break;
		case SGTT_GADGETID_StatusBar_ThumbnailsAlways:
			ToolTipText = GetLocString(MSGID_TOOLTIP_GADGET_STATUSBAR_THUMBNAILS_ALWAYS);
			break;
		case SGTT_GADGETID_StatusBar_ThumbnailsAsDefault:
			ToolTipText = GetLocString(MSGID_TOOLTIP_GADGET_STATUSBAR_THUMBNAILS_ASDEFAULT);
			break;
		case SGTT_GADGETID_StatusBar_ThumbnailsGenerate:
			ToolTipText = GetLocString(MSGID_TOOLTIP_GADGET_STATUSBAR_THUMBNAILS_GENERATE);
			break;
		case SGTT_GADGETID_ControlBar:
			if (iInfos.xii_GlobalGadgetUnderPointer.ggd_cgy)
				ToolTipText = iInfos.xii_GlobalGadgetUnderPointer.ggd_cgy->cgy_HelpText;
			else
				ToolTipText = NULL;
			break;
		default:
			ToolTipText = NULL;
			break;
			}
		}

	d1(KPrintF("%s/%s/%ld: ToolTipText=%08lx\n", __FILE__, __FUNC__, __LINE__, ToolTipText));

	if (ToolTipText && *ToolTipText)
		{
		d1(kprintf("%s/%s/%ld: ToolTipText=<%s>\n", __FILE__, __FUNC__, __LINE__, ToolTipText));

		ttd = TT_CreateTooltip(
			TTHoriz,
				TTMembers,
					TTVert,
						TTMembers,
						TTItem(ToolTipText), TT_HAlign, GTJ_LEFT, TT_TextPen, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT], End,
						End,
					End,
				End,
			End,
			End;
		}
	
	return ttd;
}


static SAVEDS(void) INTERRUPT IconToolTipProcess(struct ToolTipStart *startArg)
{
	struct MsgPort   *inputPort;
	struct IOStdReq  *inputRequest = NULL;
	BOOL inputOpen = FALSE;
	BOOL handlerAdded = FALSE;
	static struct Hook GetStringHook;
	STATIC_PATCHFUNC(ttInputHandler)
	struct ToolTipSignalData *sData = NULL;
	struct ttInfo tti;

	memset(&tti, 0, sizeof(tti));

	SETHOOKFUNC(GetStringHook, GetStringFunc);

	d1(kprintf("%s/%s/%ld: startArg=%08lx  iwt=%08lx  ttd=%08lx\n", \
		__FILE__, __FUNC__, __LINE__, startArg, startArg->tts_WindowTask, startArg->tts_ttDef));

	do	{
		tti.tti_Font = iInfos.xii_iinfos.ii_Screen->RastPort.Font;
		tti.tti_TTFont = ScreenTTFont;
		tti.tti_Def = startArg->tts_ttDef;
		tti.tti_WindowBox = startArg->tts_WindowBox;

		tti.tti_PointerX = startArg->tts_MouseX;
		tti.tti_PointerY = startArg->tts_MouseY;

		inputPort = CreateMsgPort();
		if (NULL == inputPort)
			break;

		sData = ScalosAlloc(sizeof(struct ToolTipSignalData));
		if (NULL == sData)
			break;

		memset(sData, 0, sizeof(struct ToolTipSignalData));

		SetProgramName((STRPTR)TOOLTIP_PROCESSNAME);
		sData->ttsd_Process = (struct Process *) FindTask(NULL);
		sData->ttsd_Process->pr_Task.tc_Node.ln_Name = (STRPTR) TOOLTIP_PROCESSNAME;

		inputRequest = (struct IOStdReq *) CreateExtIO(inputPort, sizeof(struct IOStdReq));
		if (NULL == inputRequest)
			break;

		if (0 == OpenDevice("input.device", 0, (struct IORequest *) inputRequest, 0))
			inputOpen = TRUE;

		if (!inputOpen)
			break;

		ToolTipBackground = CreateDatatypesImage("THEME:ToolTipBackground", 0);

		SETHOOKFUNC(ToolTipBackFillHook, ToolTip_BackFillFunc);
		ToolTipBackFillHook.h_Data = NULL;

		sData->ttsd_Int.is_Code = (VOID (*)()) PATCH_NEWFUNC(ttInputHandler);
		sData->ttsd_Int.is_Data = sData;
		sData->ttsd_Int.is_Node.ln_Pri = 100;
#ifdef __amigaos4__
	    sData->ttsd_Int.is_Node.ln_Type = NT_EXTINTERRUPT;
#else
	    sData->ttsd_Int.is_Node.ln_Type = NT_INTERRUPT;
#endif
		sData->ttsd_Int.is_Node.ln_Name = (STRPTR) "Scalos Tooltip Input Handler";

		d1(kprintf("%s/%s/%ld: sData = %ld, is_Data = %ld\n", __FILE__, __FUNC__, __LINE__,
			(LONG)sData, (LONG)sData->ttsd_Int.is_Data));

		inputRequest->io_Data = (APTR) sData;
		inputRequest->io_Command = IND_ADDHANDLER;

		DoIO((struct IORequest *) inputRequest);
		handlerAdded = TRUE;

		d1(kprintf("%s/%s/%ld: sData = %ld, is_Data = %ld\n", __FILE__, __FUNC__, __LINE__,
			(LONG)sData, (LONG)sData->ttsd_Int.is_Data));

		TTLayoutWindow(&tti);

		d1(kprintf("%s/%s/%ld: Width=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, \
			tti.tti_WindowBox.Width, tti.tti_WindowBox.Height));

		tti.tti_BackgroundBitMap = SaveTTWindowBackground(&tti.tti_WindowBox);

		tti.tti_Window = LockedOpenWindowTags(NULL,
			WA_Left, tti.tti_WindowBox.Left,
			WA_Top, tti.tti_WindowBox.Top,
			WA_Width, tti.tti_WindowBox.Width,
			WA_Height, tti.tti_WindowBox.Height,
			WA_BackFill, &ToolTipBackFillHook,
			WA_SizeGadget, FALSE,
			WA_DragBar, FALSE,
			WA_CloseGadget, FALSE,
			WA_DepthGadget, FALSE,
			WA_NoCareRefresh, TRUE,
			WA_Borderless, TRUE,
			WA_Activate, FALSE,
			WA_SmartRefresh, TRUE,
			WA_AutoAdjust, TRUE,
#if defined(WA_FrontWindow)
			WA_FrontWindow, TRUE,
			WA_ToolbarWindow, TRUE,
#elif defined(WA_StayTop)
			WA_StayTop, TRUE,
#endif //defined(WA_StayTop)
			WA_SCA_Opaqueness, SCALOS_OPAQUENESS(0),
			WA_PubScreen, iInfos.xii_iinfos.ii_Screen,
			TAG_END);

		if (NULL == tti.tti_Window)
			break;

		TTRenderWindow(&tti);
		WindowFadeIn(tti.tti_Window);

		// wait for signal from ttInputHandler()
		Wait(SIGBREAKF_CTRL_C);

		WindowFadeOut(tti.tti_Window);
		} while (0);

	d1(kprintf("%s/%s/%ld: sData = %ld, is_Data = %ld\n", __FILE__, __FUNC__, __LINE__,
		(LONG)sData, (LONG)sData->ttsd_Int.is_Data));

	if (tti.tti_Window)
		LockedCloseWindow(tti.tti_Window);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	DisposeDatatypesImage(&ToolTipBackground);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (inputOpen)
		{
		if (handlerAdded)
			{
			inputRequest->io_Data = (APTR) sData;
			inputRequest->io_Command = IND_REMHANDLER;

			DoIO((struct IORequest *) inputRequest);
			}

		CloseDevice((struct IORequest *) inputRequest);
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (tti.tti_BackgroundBitMap)
		FreeBitMap(tti.tti_BackgroundBitMap);
	if (sData)
		ScalosFree(sData);
	if (inputRequest)
		DeleteExtIO((struct IORequest *) inputRequest);
	if (inputPort)
		DeleteMsgPort(inputPort);

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	TTDisposeItem(tti.tti_Def);

	d1(kprintf("%s/%s/%ld:  END\n", __FILE__, __FUNC__, __LINE__));
}


/*
* The event list gets passed to you in  a0.
* The is_Data field is passed to you in a1.
*
* On exit you must return the event list in d0.  In this way
* you could add or remove items from the event list.
*/
M68KFUNC_P2(struct InputEvent *, ttInputHandler,
	A0, struct InputEvent *, eventList,
	A1, APTR, isData)
{
	struct ToolTipSignalData *ttsData = (struct ToolTipSignalData *) isData;
	struct InputEvent *ie;
	BOOL Found = FALSE;

	d1(kprintf("%s/%s/%ld: isData = %ld\n", __FILE__, __FUNC__, __LINE__, (LONG)isData));

	for (ie = eventList; ie && !Found; ie=ie->ie_NextEvent)
		{
		switch (ie->ie_Class)
			{
		case IECLASS_RAWKEY:
		case IECLASS_RAWMOUSE:
		case IECLASS_POINTERPOS:
		case IECLASS_GADGETDOWN:
		case IECLASS_GADGETUP:
		    Signal(&ttsData->ttsd_Process->pr_Task, SIGBREAKF_CTRL_C);
			Found = TRUE;
			break;
			}
		}

	return eventList;
}
M68KFUNC_END


static struct BitMap *SaveTTWindowBackground(const struct IBox *ttWindowBox)
{
	struct RastPort rp;

	Scalos_InitRastPort(&rp);

	rp.BitMap = AllocBitMap(ttWindowBox->Width, ttWindowBox->Height, 
		GetBitMapAttr(iInfos.xii_iinfos.ii_Screen->RastPort.BitMap, BMA_DEPTH), 
		BMF_MINPLANES,
		iInfos.xii_iinfos.ii_Screen->RastPort.BitMap);

	if (rp.BitMap)
		{
		ClipBlit(&iInfos.xii_iinfos.ii_Screen->RastPort,
			ttWindowBox->Left, ttWindowBox->Top,
			&rp, 0, 0, 
			ttWindowBox->Width, ttWindowBox->Height,
			ABC | ABNC);
		}

	Scalos_DoneRastPort(&rp);

	return rp.BitMap;
}


static void TTRenderWindow(struct ttInfo *tti)
{
	struct BitMap *MaskBitMap;
	WORD AreaBuffer[AREAMAX];
	struct AreaInfo AInfo;
	PLANEPTR myPlanePtr = NULL;
	struct TmpRas myTmpRas;
	ULONG AllocRastWidth = 0, AllocRastHeight = 0;

	do	{
		struct RastPort MaskRp;
		WORD PeakLeft, PeakRight, PeakPoint;
		LONG bmWidth;
		ULONG bmFlags, bmDepth;
		ULONG ScreenDepth;

		ScreenDepth = GetBitMapAttr(iInfos.xii_iinfos.ii_Screen->RastPort.BitMap, BMA_DEPTH);

		bmWidth = (LONG) GetBitMapAttr(tti->tti_BackgroundBitMap, BMA_WIDTH);
		bmDepth = GetBitMapAttr(tti->tti_BackgroundBitMap, BMA_DEPTH);
		bmFlags = GetBitMapAttr(tti->tti_BackgroundBitMap, BMA_FLAGS);

		if (bmFlags & BMF_INTERLEAVED)
			bmWidth *= bmDepth;

		if (tti->tti_PointerX <= tti->tti_Window->LeftEdge)
			{
			PeakLeft = TT_RADIUS + TT_PEAKHEIGHT - TT_PEAKWIDTH;
			PeakRight = PeakLeft + 2 * TT_PEAKWIDTH;
			PeakPoint = 0;
			}
		else
			{
			PeakLeft = tti->tti_Window->Width - TT_RADIUS - TT_PEAKHEIGHT - TT_PEAKWIDTH;
			PeakRight = PeakLeft + 2 * TT_PEAKWIDTH;
			PeakPoint = tti->tti_Window->Width - 1;
			}

		memset(&AInfo, 0, sizeof(AInfo));
		Scalos_InitRastPort(&MaskRp);

		d1(kprintf("%s/%s/%ld: yTop=%ld  yBottom=%ld\n", __FILE__, __FUNC__, __LINE__, tti->tti_yTop, tti->tti_yBottom));

		MaskRp.BitMap = MaskBitMap = AllocBitMap(bmWidth, tti->tti_Window->Height, 1, 
			BMF_INTERLEAVED | BMF_MINPLANES, NULL);
		if (NULL == MaskBitMap)
			break;

		d1(kprintf("%s/%s/%ld: bmWidth=%ld  Height=%ld\n", __FILE__, __FUNC__, __LINE__, bmWidth, tti->tti_Window->Height));
		d1(kprintf("%s/%s/%ld: MaskBitMap=%08lx BytesPerRow=%ld  Rows=%ld\n", \
			__FILE__, __FUNC__, __LINE__, MaskBitMap, MaskBitMap->BytesPerRow, MaskBitMap->Rows));

		AllocRastWidth = bmWidth;
		AllocRastHeight = tti->tti_Window->Height;

		myPlanePtr = AllocRaster(AllocRastWidth, AllocRastHeight);
		if (myPlanePtr == NULL)
			break;

		InitTmpRas(&myTmpRas, myPlanePtr, RASSIZE(bmWidth, tti->tti_Window->Height));
		MaskRp.TmpRas = &myTmpRas;

		InitArea(&AInfo, AreaBuffer, (AREAMAX * sizeof(WORD)) / 5);
		MaskRp.AreaInfo = &AInfo;

		//  1-----2		tti->tti_yTop
		// /       \            .
		// 8       3            .
		// |       |            .
		// 7       4            .
		//  \     /             .
		//   6---5		tti->tti_yBottom

		SetRast(&MaskRp, 1);

		SetABPenDrMd(&MaskRp, 0, 0, JAM1);

		AreaMove(&MaskRp, TT_RADIUS, tti->tti_yTop);	// 1

		if (!tti->tti_AbovePointer)
			{
			AreaDraw(&MaskRp, PeakLeft, tti->tti_yTop);
			d1(kprintf("%s/%s/%ld: BelowPointer ptrX=%ld  ptrY=%ld\n", __FILE__, __FUNC__, __LINE__, \
				tti->tti_PointerX - tti->tti_Window->LeftEdge, \
				tti->tti_PointerY - tti->tti_Window->TopEdge));
			AreaDraw(&MaskRp, PeakPoint, 0);
			AreaDraw(&MaskRp, PeakRight, tti->tti_yTop);
			}

		AreaDraw(&MaskRp, tti->tti_Window->Width - TT_RADIUS - 1, tti->tti_yTop);	// -> 2
		AreaDraw(&MaskRp, tti->tti_Window->Width - 1, tti->tti_yTop + TT_RADIUS);	// -> 3
		AreaDraw(&MaskRp, tti->tti_Window->Width - 1, tti->tti_yBottom - TT_RADIUS);	// -> 4
		AreaDraw(&MaskRp, tti->tti_Window->Width - TT_RADIUS - 1, tti->tti_yBottom);	// -> 5

		if (tti->tti_AbovePointer)
			{
			AreaDraw(&MaskRp, PeakRight, tti->tti_yBottom);
			d1(kprintf("%s/%s/%ld: AbovePointer ptrX=%ld  ptrY=%ld\n", __FILE__, __FUNC__, __LINE__, \
				tti->tti_PointerX - tti->tti_Window->LeftEdge, \
				tti->tti_PointerY - tti->tti_Window->TopEdge));
			AreaDraw(&MaskRp, PeakPoint, tti->tti_Window->Height - 1);
			AreaDraw(&MaskRp, PeakLeft, tti->tti_yBottom);
			}

		AreaDraw(&MaskRp, TT_RADIUS, tti->tti_yBottom);		// -> 6
		AreaDraw(&MaskRp, 0, tti->tti_yBottom - TT_RADIUS);	// -> 7
		AreaDraw(&MaskRp, 0, tti->tti_yTop + TT_RADIUS);	// -> 8

		AreaEnd(&MaskRp);

		AreaCircle(&MaskRp, TT_RADIUS, tti->tti_yTop + TT_RADIUS, TT_RADIUS);							// top left
		AreaCircle(&MaskRp, tti->tti_Window->Width - 1 - TT_RADIUS, tti->tti_yTop + TT_RADIUS, TT_RADIUS);				// top right
		AreaCircle(&MaskRp, TT_RADIUS, tti->tti_yBottom - TT_RADIUS, TT_RADIUS);					// bottom left
		AreaCircle(&MaskRp, tti->tti_Window->Width - 1 - TT_RADIUS, tti->tti_yBottom - TT_RADIUS, TT_RADIUS);	// bottom right

		AreaEnd(&MaskRp);

		d1(KPrintF("%s/%s/%ld: pref_TooltipsTransparency=%ld\n", __FILE__, __FUNC__, __LINE__, (ULONG) CurrentPrefs.pref_TooltipsTransparency));

		if (CyberGfxBase && ScreenDepth > 8 && CurrentPrefs.pref_TooltipsTransparency < PREFS_TRANSPARENCY_OPAQUE)
			{
			BlitBackgroundTransparent(tti, MaskBitMap);
			}
		else
			{
			BltMaskBitMapRastPort(tti->tti_BackgroundBitMap, 0, 0,
				tti->tti_Window->RPort, 
				0, 0, tti->tti_Window->Width, tti->tti_Window->Height,
				ABC | ABNC | ANBC,
				MaskBitMap->Planes[0]);
			}

		SetAPen(tti->tti_Window->RPort, PalettePrefs.pal_PensList[PENIDX_TOOLTIP_TEXT]);

		// Draw border around tooltip shape
		DrawTooltipShapeBorder(tti, PeakLeft, PeakRight, PeakPoint);

		Scalos_SetFont(tti->tti_Window->RPort, tti->tti_Font, &tti->tti_TTFont);
		SetDrMd(tti->tti_Window->RPort, JAM1);

		TTRender(tti->tti_Window->RPort, tti->tti_Def, 
			tti->tti_XRenderOffset, tti->tti_YRenderOffset + tti->tti_yTop);

		Scalos_DoneRastPort(&MaskRp);
		} while (0);

	if (myPlanePtr)
		FreeRaster(myPlanePtr, AllocRastWidth, AllocRastHeight);
	if (MaskBitMap)
		FreeBitMap(MaskBitMap);
}


static void DrawTooltipShapeBorder(struct ttInfo *tti, WORD PeakLeft, WORD PeakRight, WORD PeakPoint)
{
	SetAPen(tti->tti_Window->RPort, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHINEPEN]);

	Move(tti->tti_Window->RPort, TT_RADIUS, tti->tti_yTop);					// 1

	if (!tti->tti_AbovePointer)
		{
		ToolTipDraw(tti->tti_Window->RPort, PeakLeft, tti->tti_yTop);
		ToolTipDraw(tti->tti_Window->RPort, PeakPoint, 0);
		SetAPen(tti->tti_Window->RPort, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHADOWPEN]);
		ToolTipDraw(tti->tti_Window->RPort, PeakRight, tti->tti_yTop);
		}

	SetAPen(tti->tti_Window->RPort, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHINEPEN]);
	ToolTipDraw(tti->tti_Window->RPort, tti->tti_Window->Width - TT_RADIUS - 1, tti->tti_yTop);    // -> 2

	ToolTopDrawEllipse(tti->tti_Window->RPort,
		tti->tti_Window->Width - 1 - TT_RADIUS, tti->tti_yTop + TT_RADIUS,
		TT_RADIUS, TT_RADIUS, ELLIPSE_SEGMENT_TOPRIGHT,
		iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHINEPEN],
		iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHADOWPEN]);					// top right -> 3
	SetAPen(tti->tti_Window->RPort, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHADOWPEN]);
	Move(tti->tti_Window->RPort, tti->tti_Window->Width - 1, tti->tti_yTop + TT_RADIUS);	// -> 3
	ToolTipDraw(tti->tti_Window->RPort, tti->tti_Window->Width - 1, tti->tti_yBottom - TT_RADIUS); // -> 4

	ToolTopDrawEllipse(tti->tti_Window->RPort,
		tti->tti_Window->Width - 1 - TT_RADIUS, tti->tti_yBottom - TT_RADIUS,
		TT_RADIUS, TT_RADIUS, ELLIPSE_SEGMENT_BOTTOMRIGHT,
		iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHADOWPEN],
		iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHADOWPEN]);					// bottom right -> 5
	Move(tti->tti_Window->RPort, tti->tti_Window->Width - TT_RADIUS - 1, tti->tti_yBottom);	// -> 5

	if (tti->tti_AbovePointer)
		{
		ToolTipDraw(tti->tti_Window->RPort, PeakRight, tti->tti_yBottom);
		ToolTipDraw(tti->tti_Window->RPort, PeakPoint, tti->tti_Window->Height - 1);
		SetAPen(tti->tti_Window->RPort, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHINEPEN]);
		ToolTipDraw(tti->tti_Window->RPort, PeakLeft, tti->tti_yBottom);
		}

	SetAPen(tti->tti_Window->RPort, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHADOWPEN]);
	ToolTipDraw(tti->tti_Window->RPort, TT_RADIUS, tti->tti_yBottom);			       // -> 6

	ToolTopDrawEllipse(tti->tti_Window->RPort,
		TT_RADIUS, tti->tti_yBottom - TT_RADIUS,
		TT_RADIUS, TT_RADIUS, ELLIPSE_SEGMENT_BOTTOMLEFT,
		iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHADOWPEN],
		iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHINEPEN]);					// bottom left -> 7
	SetAPen(tti->tti_Window->RPort, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHINEPEN]);
	Move(tti->tti_Window->RPort, 0, tti->tti_yBottom - TT_RADIUS);				// -> 7
	ToolTipDraw(tti->tti_Window->RPort, 0, tti->tti_yTop + TT_RADIUS);			       // -> 8

	ToolTopDrawEllipse(tti->tti_Window->RPort,
		TT_RADIUS, tti->tti_yTop + TT_RADIUS,
		TT_RADIUS, TT_RADIUS, ELLIPSE_SEGMENT_TOPLEFT,
		iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHINEPEN],
		iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[SHINEPEN]);					// top left -> 1
}


static void outline_ellipse(struct RastPort *rp, WORD center_x, WORD center_y, 
	WORD rx, WORD ry, WORD Segment, UBYTE color1, UBYTE color2)
{
	/* intermediate terms to speed up loop */
	long t1 = rx*rx, t2 = t1<<1, t3 = t2<<1;
	long t4 = ry*ry, t5 = t4<<1, t6 = t5<<1;
	long t7 = rx*t5, t8 = t7<<1, t9 = 0L;
	long d1 = t2 - t7 + (t4>>1);	/* error terms */
	long d2 = (t1>>1) - t8 + t5;

	register int x = rx, y = 0;	/* ellipse points */

	SetAPen(rp, color2);

	while (d2 < 0)	/* til slope = -1 */
		{
		/* draw 4 points using symmetry */
		if (Segment & ELLIPSE_SEGMENT_BOTTOMRIGHT)
		    	WritePixel(rp, center_x + x, center_y + y);
		if (Segment & ELLIPSE_SEGMENT_TOPRIGHT)
		    	WritePixel(rp, center_x + x, center_y - y);
		if (Segment & ELLIPSE_SEGMENT_BOTTOMLEFT)
		    	WritePixel(rp, center_x - x, center_y + y);
		if (Segment & ELLIPSE_SEGMENT_TOPLEFT)
		    	WritePixel(rp, center_x - x, center_y - y);

		y++;	/* always move up here */
		t9 += t3;	
		if (d1 < 0)	/* move straight up */
			{
			d1 += t9 + t2;
			d2 += t9;
			}
		else	/* move up and left */
			{
			x--;
			t8 -= t6;
			d1 += t9 + t2 - t8;
			d2 += t9 + t5 - t8;
			}
		}

	SetAPen(rp, color1);

	do 	/* rest of top right quadrant */
		{
		/* draw 4 points using symmetry */
		if (Segment & ELLIPSE_SEGMENT_BOTTOMRIGHT)
		    	WritePixel(rp, center_x + x, center_y + y);
		if (Segment & ELLIPSE_SEGMENT_TOPRIGHT)
		    	WritePixel(rp, center_x + x, center_y - y);
		if (Segment & ELLIPSE_SEGMENT_BOTTOMLEFT)
		    	WritePixel(rp, center_x - x, center_y + y);
		if (Segment & ELLIPSE_SEGMENT_TOPLEFT)
	  	  	WritePixel(rp, center_x - x, center_y - y);

		x--;	/* always move left here */
		t8 -= t6;	
		if (d2 < 0)	/* move up and left */
			{
			y++;
			t9 += t3;
			d2 += t9 + t5 - t8;
			}
		else	/* move straight left */
			d2 += t5 - t8;
		} while (x>=0);
}


static void ToolTipDraw(struct RastPort *rp, WORD Left, WORD Top)
{
	ULONG ScreenDepth = GetBitMapAttr(iInfos.xii_iinfos.ii_Screen->RastPort.BitMap, BMA_DEPTH);

	if (CyberGfxBase && ScreenDepth > 8)
		{
		DrawWuLine(rp, Left, Top, GetAPen(rp));
		Move(rp, Left, Top);
		}
	else
		{
		Draw(rp, Left, Top);
		}
}


static void ToolTopDrawEllipse(struct RastPort *rp, WORD center_x, WORD center_y,
	WORD rx, WORD ry, WORD Segment, UBYTE color1, UBYTE color2)
{
	ULONG ScreenDepth = GetBitMapAttr(iInfos.xii_iinfos.ii_Screen->RastPort.BitMap, BMA_DEPTH);

	if (CyberGfxBase && ScreenDepth > 8)
		{
		outline_ellipse_antialias(rp, center_x, center_y,
			rx, ry, Segment, color1, color2);
		}
	else
		{
		outline_ellipse(rp, center_x, center_y,
			rx, ry, Segment, color1, color2);
		}
}


static void TTLayoutWindow(struct ttInfo *tti)
{
	tti->tti_XRenderOffset = TOOLTIP_WINDOW_BORDER_HORIZ;
	tti->tti_YRenderOffset = TOOLTIP_WINDOW_BORDER_VERT;

	tti->tti_WindowBox.Width += 2 * TOOLTIP_WINDOW_BORDER_HORIZ;
	tti->tti_WindowBox.Height += 2 * TOOLTIP_WINDOW_BORDER_VERT;

	tti->tti_WindowBox.Width = max(tti->tti_WindowBox.Width, 3 * TT_RADIUS + TT_PEAKHEIGHT + TT_PEAKWIDTH);

	if (tti->tti_WindowBox.Height < 3 * TT_RADIUS)
		{
		tti->tti_YRenderOffset += ((3 * TT_RADIUS) - tti->tti_WindowBox.Height) / 2;
		tti->tti_WindowBox.Height = 3 * TT_RADIUS;
		}

	// leave room for peak above/below bubble
	tti->tti_WindowBox.Height += TT_PEAKHEIGHT;

	d1(kprintf("%s/%s/%ld: ttWindowBox Width=%ld  Height=%ld\n", \
		__FILE__, __FUNC__, __LINE__, tti->tti_WindowBox.Width, tti->tti_WindowBox.Height));

	tti->tti_WindowBox.Left = tti->tti_PointerX;

	if ((tti->tti_WindowBox.Left + tti->tti_WindowBox.Width) >= iInfos.xii_iinfos.ii_Screen->Width)
		tti->tti_WindowBox.Left -= tti->tti_WindowBox.Width;

	// position tooltip above mouse pointer if enough space available
	if (tti->tti_PointerY > tti->tti_WindowBox.Height + 5)
		{
		tti->tti_AbovePointer = TRUE;
		tti->tti_WindowBox.Top = tti->tti_PointerY - tti->tti_WindowBox.Height;

		tti->tti_yTop = 0;
		tti->tti_yBottom = tti->tti_WindowBox.Height - TT_PEAKHEIGHT;
		}
	else
		{
		tti->tti_AbovePointer = FALSE;
		tti->tti_WindowBox.Top = tti->tti_PointerY;

		tti->tti_yTop = TT_PEAKHEIGHT;
		tti->tti_yBottom = tti->tti_WindowBox.Height - 1;
		}

	// make sure that window will fit on screen
	// w/o having to be shifted by intuition
	if (tti->tti_WindowBox.Left < 0)
		tti->tti_WindowBox.Left = 0;
	if (tti->tti_WindowBox.Left + tti->tti_WindowBox.Width > iInfos.xii_iinfos.ii_Screen->Width)
		tti->tti_WindowBox.Left = iInfos.xii_iinfos.ii_Screen->Width - tti->tti_WindowBox.Width;

	d1(kprintf("%s/%s/%ld: yTop=%ld  yBottom=%ld\n", __FILE__, __FUNC__, __LINE__, tti->tti_yTop, tti->tti_yBottom));
}


// object == (struct RastPort *) result->RastPort
// message == [ (Layer *) layer, (struct Rectangle) bounds,
//              (LONG) offsetx, (LONG) offsety ]

static SAVEDS(ULONG) ToolTip_BackFillFunc(struct Hook *bfHook, struct RastPort *rp, struct BackFillMsg *msg)
{
	struct RastPort rpCopy;

	d1(kprintf("%s/%s/%ld: RastPort=%08lx\n", __FILE__, __FUNC__, __LINE__, rp));
	d1(kprintf("%s/%s/%ld: Rect=%ld %ld %ld %ld\n", __FILE__, __FUNC__, __LINE__, \
		msg->bfm_Rect.MinX, msg->bfm_Rect.MinY, msg->bfm_Rect.MaxX, msg->bfm_Rect.MaxY));
	d1(kprintf("%s/%s/%ld: Layer=%08lx  OffsetX=%ld  OffsetY=%ld\n", __FILE__, __FUNC__, __LINE__, \
		msg->bfm_Layer, msg->bfm_OffsetX, msg->bfm_OffsetY));

	rpCopy = *rp;
	rpCopy.Layer = NULL;

	if (ToolTipBackground)
		{
		WindowBackFill(&rpCopy, msg, ToolTipBackground->dti_BitMap,
			ToolTipBackground->dti_BitMapHeader->bmh_Width,
			ToolTipBackground->dti_BitMapHeader->bmh_Height,
			PalettePrefs.pal_PensList[PENIDX_TOOLTIP_BG],
			0, 0,
			NULL);
		}
	else
		{
		WindowBackFill(&rpCopy, msg, NULL,
			0, 0,
			PalettePrefs.pal_PensList[PENIDX_TOOLTIP_BG],
			0, 0,
			NULL);
		}

	return 0;
}

//----------------------------------------------------------------------------

static STRPTR TooltipTTUserHookFunc(struct Hook *ttHook, struct ttDef *ttd, CONST_STRPTR defString)
{
	struct ScaToolTipInfoHookData *ttshd = (struct ScaToolTipInfoHookData *) ttHook->h_Data;
	const struct ToolTipCmdFunc *cmd;
	CONST_STRPTR lp;
	STRPTR ResultString = (STRPTR) "***UNKNOWN***";

	d1(kprintf("%s/%s/%ld: hook=%08lx  ttd=%08lx defString=<%s>\n", __FILE__, __FUNC__, __LINE__, \
		ttHook, ttd, defString));

	lp = defString;
	cmd = ParseToolTipCmd(&lp);
	d1(kprintf("%s/%s/%ld: cmd=%08lx\n", __FILE__, __FUNC__, __LINE__, cmd));
	if (cmd)
		{
		strcpy(ttshd->ttshd_Buffer, "");

		ResultString = (cmd->ttcf_Function)(ttshd, lp);
		}
	else
		{
		const struct ToolTipHideFunc *hide;

		lp = defString;

		hide = ParseToolTipHide(&lp);
		if (hide)
			{
			d1(kprintf("%s/%s/%ld: hide CommandName=<%s>\n", __FILE__, __FUNC__, __LINE__, hide->tthf_CommandName));

			ResultString = (STRPTR) (hide->tthf_Function)(ttd, ttshd, lp);
			}
		}

	d1(kprintf("%s/%s/%ld: ResultString=<%s>\n", __FILE__, __FUNC__, __LINE__, ResultString));

	return ResultString;
}


static const struct ToolTipCmdFunc *ParseToolTipCmd(CONST_STRPTR *Line)
{
	short i;
	size_t Len;
	CONST_STRPTR Command = *Line;
	const struct ToolTipCmdFunc *Table;

	/* Skip leading blanks. */

	while(*Command == ' ' || *Command == '\t' || '\n' == *Command)
		Command++;

	/* Now determine the length of the command. */

	Len = 0;

	while (Command[Len])
		{
		if ('\n' == Command[Len] || Command[Len] == '\t' || Command[Len] == ' ')
			break;
		else
			Len++;
		}

	*Line = Command + Len;

	/* Now we need to check if the command we received
	 * can be handled by one of the routines in the
	 * command table.
	 */

	Table = ToolTipCommandTable;

	d1(kprintf("%s/%s/%ld: cmd=<%s>\n", __FILE__, __FUNC__, __LINE__, Command);)

	for(i = 0 ; Table[i].ttcf_CommandName != NULL ; i++)
		{
		if((strlen(Table[i].ttcf_CommandName) == Len) 
				&& (Strnicmp((STRPTR) Command, (STRPTR) Table[i].ttcf_CommandName, Len) == 0))
			{
			return &Table[i];
			}
		}

	return NULL;
}


static const struct ToolTipHideFunc *ParseToolTipHide(CONST_STRPTR *Line)
{
	short i;
	size_t Len;
	CONST_STRPTR Command = *Line;
	const struct ToolTipHideFunc *Table;

	/* Skip leading blanks. */

	while(*Command == ' ' || *Command == '\t' || '\n' == *Command)
		Command++;

	/* Now determine the length of the command. */

	Len = 0;

	while (Command[Len])
		{
		if ('\n' == Command[Len] || Command[Len] == '\t' || Command[Len] == ' ')
			break;
		else
			Len++;
		}

	*Line = Command + Len;

	/* Now we need to check if the command we received
	 * can be handled by one of the routines in the
	 * command table.
	 */

	Table = ToolTipHideFunctionTable;

	d1(kprintf("%s/%s/%ld: cmd=<%s>\n", __FILE__, __FUNC__, __LINE__, Command);)

	for(i = 0 ; Table[i].tthf_CommandName != NULL ; i++)
		{
		if((strlen(Table[i].tthf_CommandName) == Len) 
				&& (Strnicmp((STRPTR) Command, (STRPTR) Table[i].tthf_CommandName, Len) == 0))
			{
			return &Table[i];
			}
		}

	return NULL;
}

//----------------------------------------------------------------------------

static STRPTR Tooltip_VersionString(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	char VersMask[10];
	BPTR fh = (BPTR)NULL;
	UBYTE *Buffer;
	const size_t BuffLength = 1024;
	T_TIMEVAL StartTime, currentTime;
	STRPTR VersionString = ttshd->ttshd_Buffer;
	size_t MaxLen= ttshd->ttshd_BuffLen;
	BPTR fLock = NOT_A_LOCK;

	(void) Args;

	// do not use statically initalized "$VER" variable here 
	// since it might confuse the "version" command.
	// skip the leading "\0" in versTag
	stccpy(VersMask, versTag + 1, 6 + 1);

	switch (ttshd->ttshd_DirEntryType)
		{
	case ST_FILE:
	case ST_LINKFILE:
	case ST_PIPEFILE:
		break;
	default:
		return ttshd->ttshd_Buffer;
		break;
		}

	do	{
		ULONG TotalLen = 0;
		LONG ActLen;
		ULONG MaskNdx = 0;
		BOOL Finished = FALSE;
		BOOL Found = FALSE;

		Buffer = ScalosAlloc(BuffLength);
		if (NULL == Buffer)
			break;

		GetSysTime(&StartTime);

		fLock = DupLock(ttshd->ttshd_FileLock);
		if ((BPTR)NULL == fLock)
			{
			fLock = NOT_A_LOCK;
			break;
			}

		fh = OpenFromLock(fLock);
		if ((BPTR)NULL == fh)
			break;

		fLock = NOT_A_LOCK;

		do	{
			ActLen = Read(fh, Buffer, BuffLength);

			if (ActLen > 0)
				{
				ULONG n;
				UBYTE *BufPtr = Buffer;

				TotalLen += ActLen;

				for (n=0; !Finished && n<ActLen; n++)
					{
					if (Found)
						{
						if (MaxLen > 1 && *BufPtr && *BufPtr >= ' ' && *BufPtr <= 0x7f)
							{
							*VersionString++ = *BufPtr++;
							MaxLen--;
							}
						else
							Finished = TRUE;
						}
					else
						{
						if (*BufPtr++ == VersMask[MaskNdx])
							{
							MaskNdx++;

							if (MaskNdx >= strlen(VersMask))
								{
								Found = TRUE;
								}
							}
						else
							{
							MaskNdx = 0;
							}
						}
					}
				}

			GetSysTime(&currentTime);
			if ((currentTime.tv_secs - StartTime.tv_secs) > 2)
				break;
			} while (ActLen > 0 && TotalLen < 0x100000 && !Finished);
		} while (0);

	*VersionString = '\0';

	if (Buffer)
		ScalosFree(Buffer);
	if (fh)
		Close(fh);

	if (IS_VALID_LOCK(fLock))
		UnLock(fLock);

	return ttshd->ttshd_Buffer;
}


static STRPTR Tooltip_IconName(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	CONST_STRPTR lp = NULL;

	(void) Args;

	if (ttshd->ttshd_IconNode && ttshd->ttshd_IconNode->in_Icon)
		GetAttr(IDTA_Text, ttshd->ttshd_IconNode->in_Icon, (APTR) &lp);

	if (lp)
		stccpy(ttshd->ttshd_Buffer, lp, ttshd->ttshd_BuffLen);

	return ttshd->ttshd_Buffer;
}


static STRPTR Tooltip_FileType(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	(void) Args;

	d1(KPrintF("%s/%s/%ld: <%s>  in_FileType=%08lx\n", __FILE__, __FUNC__, __LINE__, \
		GetIconName(ttshd->ttshd_IconNode), ttshd->ttshd_IconNode->in_FileType));

	switch ((ULONG) ttshd->ttshd_IconNode->in_FileType)
		{
	case WBAPPICON:
		{
		struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_AppWindowStruct->ws_WindowTask;
		const struct AppObject *appo, *appoFound = NULL;
		ULONG TypeMsgID = MSGID_TOOLTIP_APPICON;

		ScalosObtainSemaphoreShared(iwt->iwt_AppListSemaphore);

		for (appo=iwt->iwt_AppList; NULL == appoFound && appo; appo = (struct AppObject *) appo->appo_Node.mln_Succ)
			{
			if (APPTYPE_AppIcon == appo->appo_type
				&& appo->appo_object.appoo_IconObject == ttshd->ttshd_IconNode->in_Icon)
				appoFound = appo;
			}

		if (appoFound)
			{
			if (MAKE_ID('I','C','F','Y') == appoFound->appo_id)
				{
				// this is an iconified scalos window
//				struct ScaWindowStruct *ws = (struct ScaWindowStruct *) appoFound->appo_userdata;

				TypeMsgID = MSGID_ICONIFIED_WINDOW;
				}
			}

		ScalosReleaseSemaphore(iwt->iwt_AppListSemaphore);

		stccpy(ttshd->ttshd_Buffer, GetLocString(TypeMsgID), ttshd->ttshd_BuffLen);
		}
		break;

	case WBKICK:
		stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_KICKSTART), ttshd->ttshd_BuffLen);
		break;

	case WBDISK:
		stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_DISK), ttshd->ttshd_BuffLen);
		break;

	case WBGARBAGE:
		stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_FILETYPE_WBGARBAGE), ttshd->ttshd_BuffLen);
		break;

	default:
		switch (ttshd->ttshd_DirEntryType)
			{
		case ST_ROOT:
			if (!GetFileTypeFromTypeNode(ttshd))
				{
				stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_ROOTDIR), ttshd->ttshd_BuffLen);
				}
			break;
		case ST_USERDIR:
			stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_USERDIR), ttshd->ttshd_BuffLen);
			break;
		case ST_SOFTLINK:
			stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_SOFTLINK), ttshd->ttshd_BuffLen);
			break;
		case ST_LINKDIR:
			stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_HARDLINKDIR), ttshd->ttshd_BuffLen);
			break;
		case ST_FILE:
			switch ((ULONG) ttshd->ttshd_IconNode->in_FileType)
				{
			case WBDISK:
				stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_FILETYPE_WBDISK), ttshd->ttshd_BuffLen);
				break;
			case WBDRAWER:
			case WB_TEXTICON_DRAWER:
				stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_USERDIR), ttshd->ttshd_BuffLen);
				break;
			case WBTOOL:
			case WB_TEXTICON_TOOL:
				stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_FILETYPE_WBTOOL), ttshd->ttshd_BuffLen);
				break;
			case WBPROJECT:
				stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_FILETYPE_WBPROJECT), ttshd->ttshd_BuffLen);
				break;
			case WBGARBAGE:
				stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_FILETYPE_WBGARBAGE), ttshd->ttshd_BuffLen);
				break;
			case WBDEVICE:
				stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_FILETYPE_WBDEVICE), ttshd->ttshd_BuffLen);
				break;
			case WBKICK:
				stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_FILETYPE_WBKICK), ttshd->ttshd_BuffLen);
				break;
			case 0:		// should not happen
				stccpy(ttshd->ttshd_Buffer, "**NULL**", ttshd->ttshd_BuffLen);
				break;
			default:	// a valid TypeNode
				if (!GetFileTypeFromTypeNode(ttshd))
					{
					stccpy(ttshd->ttshd_Buffer, "**INVALID**", ttshd->ttshd_BuffLen);
					}
				break;
				}
			if (0 == strlen(ttshd->ttshd_Buffer))
				stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_FILE), ttshd->ttshd_BuffLen);
			break;
		case ST_LINKFILE:
			stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_HARDLINKFILE), ttshd->ttshd_BuffLen);
			break;
		case ST_PIPEFILE:
			stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_PIPEFILE), ttshd->ttshd_BuffLen);
			break;
		default:
			stccpy(ttshd->ttshd_Buffer, GetLocString(MSGID_TOOLTIP_UNKNOWNTYPE), ttshd->ttshd_BuffLen);
			break;
			}
		break;
		}

	return ttshd->ttshd_Buffer;
}


static STRPTR Tooltip_FileSize(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	(void) Args;

	if (ST_FILE == ttshd->ttshd_DirEntryType)
		{
		char NumBuff[40];

		Convert64(ScalosLocale, ttshd->ttshd_FSize, NumBuff, sizeof(NumBuff));

		ScaFormatStringMaxLength(ttshd->ttshd_Buffer,
			ttshd->ttshd_BuffLen,
			GetLocString(MSGID_TOOLTIP_FILESIZEFMT),
			(ULONG) NumBuff);
		}

	return ttshd->ttshd_Buffer;
}


static STRPTR Tooltip_Protection(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	(void) Args;

	GetProtectionString(ttshd->ttshd_Protection, ttshd->ttshd_Buffer, ttshd->ttshd_BuffLen);

	return ttshd->ttshd_Buffer;
}


static STRPTR Tooltip_FileDate(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	struct DateTime dtm;

	(void) Args;

	dtm.dat_Stamp = ttshd->ttshd_Date;
	dtm.dat_Format = FORMAT_DOS;
	dtm.dat_Flags = DTF_SUBST;
	dtm.dat_StrDay = NULL;
	dtm.dat_StrDate = ttshd->ttshd_Buffer;
	dtm.dat_StrTime = NULL;
	ClassFormatDate(&dtm, ttshd->ttshd_BuffLen, 0);

	return ttshd->ttshd_Buffer;
}


static STRPTR Tooltip_FileTime(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	struct DateTime dtm;

	(void) Args;

	dtm.dat_Stamp = ttshd->ttshd_Date;
	dtm.dat_Format = FORMAT_DOS;
	dtm.dat_Flags = DTF_SUBST;
	dtm.dat_StrDay = NULL;
	dtm.dat_StrDate = NULL;
	dtm.dat_StrTime = ttshd->ttshd_Buffer;
	ClassFormatDate(&dtm, 0, ttshd->ttshd_BuffLen);

	return ttshd->ttshd_Buffer;
}


static STRPTR Tooltip_FileComment(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	(void) Args;

	stccpy(ttshd->ttshd_Buffer, ttshd->ttshd_Comment, ttshd->ttshd_BuffLen);

	return ttshd->ttshd_Buffer;
}


static STRPTR Tooltip_FibFileName(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	(void) Args;

	if (ttshd->ttshd_IconNode && ttshd->ttshd_IconNode->in_DeviceIcon)
		{
		struct ScaDeviceIcon *di = ttshd->ttshd_IconNode->in_DeviceIcon;

		d1(kprintf("%s/%s/%ld: di=%08lx\n", __FILE__, __FUNC__, __LINE__, di));

		stccpy(ttshd->ttshd_Buffer, di->di_Volume ? di->di_Volume : di->di_Device, ttshd->ttshd_BuffLen);
		}
	else
		{
		stccpy(ttshd->ttshd_Buffer, ttshd->ttshd_FileName, ttshd->ttshd_BuffLen);
		}

	return ttshd->ttshd_Buffer;
}


static STRPTR Tooltip_DiskState(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	(void) Args;

	if (ttshd->ttshd_IconNode)
		{
		struct ScaDeviceIcon *di = ttshd->ttshd_IconNode->in_DeviceIcon;

		if (di)
			{
			ULONG State;

			switch (di->di_Info->id_DiskState)
				{
			case ID_WRITE_PROTECTED:
				State = MSGID_TOOLTIP_STATE_WRITEPROTECT;
				break;
			case ID_VALIDATING:
				State = MSGID_TOOLTIP_STATE_VALIDATING;
				break;
			case ID_VALIDATED:
				State = MSGID_TOOLTIP_STATE_READWRITE;
				break;
			default:
				State = MSGID_TOOLTIP_STATE_UNKNOWN;
				break;
				}

			stccpy(ttshd->ttshd_Buffer, GetLocString(State), ttshd->ttshd_BuffLen);
			}
		}

	return ttshd->ttshd_Buffer;
}


static STRPTR Tooltip_VolumeCreatedDate(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	(void) Args;

	if (ttshd->ttshd_IconNode)
		{
		struct ScaDeviceIcon *di = ttshd->ttshd_IconNode->in_DeviceIcon;

		if (di)
			{
			struct DosList *VolumeNode;

			VolumeNode = BADDR(di->di_Info->id_VolumeNode);

			if (VolumeNode)
				{
				struct DateTime dtm;

				dtm.dat_Stamp = VolumeNode->dol_misc.dol_volume.dol_VolumeDate;
				dtm.dat_Format = FORMAT_DOS;
				dtm.dat_Flags = DTF_SUBST;
				dtm.dat_StrDay = NULL;
				dtm.dat_StrDate = ttshd->ttshd_Buffer;
				dtm.dat_StrTime = NULL;

				ClassFormatDate(&dtm, ttshd->ttshd_BuffLen, 0);
				}
			}
		}

	return ttshd->ttshd_Buffer;
}


static STRPTR Tooltip_VolumeCreatedTime(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	(void) Args;

	if (ttshd->ttshd_IconNode)
		{
		struct ScaDeviceIcon *di = ttshd->ttshd_IconNode->in_DeviceIcon;

		if (di)
			{
			struct DosList *VolumeNode;

			VolumeNode = BADDR(di->di_Info->id_VolumeNode);

			if (VolumeNode)
				{
				struct DateTime dtm;

				dtm.dat_Stamp = VolumeNode->dol_misc.dol_volume.dol_VolumeDate;
				dtm.dat_Format = FORMAT_DOS;
				dtm.dat_Flags = DTF_SUBST;
				dtm.dat_StrDay = NULL;
				dtm.dat_StrDate = NULL;
				dtm.dat_StrTime = ttshd->ttshd_Buffer;

				ClassFormatDate(&dtm, 0, ttshd->ttshd_BuffLen);
				}
			}
		}

	return ttshd->ttshd_Buffer;
}


static STRPTR Tooltip_DiskUsedCount(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	(void) Args;

	if (ttshd->ttshd_IconNode)
		{
		struct ScaDeviceIcon *di = ttshd->ttshd_IconNode->in_DeviceIcon;

		if (di)
			{
			struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_AppWindowStruct->ws_WindowTask;
			struct msg_Translate ttm;
			struct MsgPort *oldFsTask;
			char SizePercent[15], SizeUsed[20], SizeFree[20];

			oldFsTask = SetFileSysTask(di->di_Handler);

			// collect disk size information
			// use TitleClass to take advantage of its 64bit arithmetic and scaling capabilities.

			ttm.mxl_MethodID = SCCM_Title_Translate;

			ttm.mxl_Buffer = SizePercent;
			ttm.mxl_TitleFormat = (STRPTR) "";
			ttm.mxl_ParseID = 0x6470;	// "dp"
			ttm.mxl_BuffLen = sizeof(SizePercent);

			DoMethodA(iwt->iwt_TitleObject, (Msg)(APTR) &ttm);
			ttm.mxl_Buffer[0] = '\0';

			ttm.mxl_Buffer = SizeFree;
			ttm.mxl_TitleFormat = (STRPTR) "";
			ttm.mxl_ParseID = 0x4446;	// "DF"
			ttm.mxl_BuffLen = sizeof(SizeFree);

			DoMethodA(iwt->iwt_TitleObject, (Msg)(APTR) &ttm);
			ttm.mxl_Buffer[0] = '\0';

			ttm.mxl_Buffer = SizeUsed;
			ttm.mxl_TitleFormat = (STRPTR) "";
			ttm.mxl_ParseID = 0x4455;	// "DU"
			ttm.mxl_BuffLen = sizeof(SizeUsed);

			DoMethodA(iwt->iwt_TitleObject, (Msg)(APTR) &ttm);
			ttm.mxl_Buffer[0] = '\0';

			SetFileSysTask(oldFsTask);

			ScaFormatStringMaxLength(ttshd->ttshd_Buffer, ttshd->ttshd_BuffLen,
				GetLocString(MSGID_TOOLTIP_USEDCOUNTFMT),
				(ULONG) SizePercent, (ULONG) SizeFree, (ULONG) SizeUsed);
			}
		}

	return ttshd->ttshd_Buffer;
}


static STRPTR Tooltip_DiskUsedPercent(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	(void) Args;

	if (ttshd->ttshd_IconNode)
		{
		struct ScaDeviceIcon *di = ttshd->ttshd_IconNode->in_DeviceIcon;

		if (di)
			{
			struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_AppWindowStruct->ws_WindowTask;
			struct msg_Translate ttm;
			struct MsgPort *oldFsTask;
			char SizePercent[15];

			oldFsTask = SetFileSysTask(di->di_Handler);

			// collect disk size information
			// use TitleClass to take advantage of its 64bit arithmetic and scaling capabilities.

			ttm.mxl_MethodID = SCCM_Title_Translate;

			ttm.mxl_Buffer = SizePercent;
			ttm.mxl_TitleFormat = (STRPTR) "";
			ttm.mxl_ParseID = 0x6470;	// "dp"
			ttm.mxl_BuffLen = sizeof(SizePercent);

			DoMethodA(iwt->iwt_TitleObject, (Msg)(APTR) &ttm);
			ttm.mxl_Buffer[0] = '\0';

			SetFileSysTask(oldFsTask);

			ScaFormatStringMaxLength(ttshd->ttshd_Buffer, ttshd->ttshd_BuffLen,
				GetLocString(MSGID_TOOLTIP_USEDCOUNTFMT_FULL),
								(ULONG) SizePercent);
			}
		}

	return ttshd->ttshd_Buffer;
}


static STRPTR Tooltip_DiskUsedInUse(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	(void) Args;

	if (ttshd->ttshd_IconNode)
		{
		struct ScaDeviceIcon *di = ttshd->ttshd_IconNode->in_DeviceIcon;

		if (di)
			{
			struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_AppWindowStruct->ws_WindowTask;
			struct msg_Translate ttm;
			struct MsgPort *oldFsTask;
			char SizeUsed[20];

			oldFsTask = SetFileSysTask(di->di_Handler);

			// collect disk size information
			// use TitleClass to take advantage of its 64bit arithmetic and scaling capabilities.

			ttm.mxl_MethodID = SCCM_Title_Translate;

			ttm.mxl_Buffer = SizeUsed;
			ttm.mxl_TitleFormat = (STRPTR) "";
			ttm.mxl_ParseID = 0x4455;	// "DU"
			ttm.mxl_BuffLen = sizeof(SizeUsed);

			DoMethodA(iwt->iwt_TitleObject, (Msg)(APTR) &ttm);
			ttm.mxl_Buffer[0] = '\0';

			SetFileSysTask(oldFsTask);

			ScaFormatStringMaxLength(ttshd->ttshd_Buffer, ttshd->ttshd_BuffLen,
				GetLocString(MSGID_TOOLTIP_USEDCOUNTFMT_INUSE),
								(ULONG) SizeUsed);
			}
		}

	return ttshd->ttshd_Buffer;
}


static STRPTR Tooltip_DiskUsedFree(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	(void) Args;

	if (ttshd->ttshd_IconNode)
		{
		struct ScaDeviceIcon *di = ttshd->ttshd_IconNode->in_DeviceIcon;

		if (di)
			{
			struct internalScaWindowTask *iwt = (struct internalScaWindowTask *) iInfos.xii_iinfos.ii_AppWindowStruct->ws_WindowTask;
			struct msg_Translate ttm;
			struct MsgPort *oldFsTask;
			char SizeFree[20];

			oldFsTask = SetFileSysTask(di->di_Handler);

			// collect disk size information
			// use TitleClass to take advantage of its 64bit arithmetic and scaling capabilities.

			ttm.mxl_MethodID = SCCM_Title_Translate;

			ttm.mxl_Buffer = SizeFree;
			ttm.mxl_TitleFormat = (STRPTR) "";
			ttm.mxl_ParseID = 0x4446;	// "DF"
			ttm.mxl_BuffLen = sizeof(SizeFree);

			DoMethodA(iwt->iwt_TitleObject, (Msg)(APTR) &ttm);
			ttm.mxl_Buffer[0] = '\0';

			SetFileSysTask(oldFsTask);

			ScaFormatStringMaxLength(ttshd->ttshd_Buffer, ttshd->ttshd_BuffLen,
				GetLocString(MSGID_TOOLTIP_USEDCOUNTFMT_FREE),
				(ULONG) SizeFree);
			}
		}

	return ttshd->ttshd_Buffer;
}


static STRPTR Tooltip_RunPlugin(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	STRPTR PathBuffer = NULL, NameBuffer;
	STRPTR lp;
	struct Library *ScalosFileTypePluginBase = NULL;
#ifdef __amigaos4__
	struct ScalosFileTypePluginIFace *IScalosFileTypePlugin = NULL;
#endif
	CONST_STRPTR PluginName = stpblk(Args);

	do	{
		CONST_STRPTR PluginArgs = PluginName;

		NameBuffer = AllocPathBuffer();
		if (NULL == NameBuffer)
			break;

		PathBuffer = AllocPathBuffer();
		if (NULL == PathBuffer)
			break;

		lp = NameBuffer;
		while (*PluginArgs && !isspace(*PluginArgs))
			{
			*lp++ = *PluginArgs++;
			}
		*lp = '\0';

		stccpy(PathBuffer, TOOLTIP_PLUGINPATH, Max_PathLen);
		AddPart(PathBuffer, NameBuffer, Max_PathLen);

		ScalosFileTypePluginBase = OpenLibrary(PathBuffer, 0);

		d1(KPrintF("%s/%s/%ld: PathBuffer=<%s> Base=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, PathBuffer, ScalosFileTypePluginBase));

#ifdef __amigaos4__
		if (ScalosFileTypePluginBase)
			{
			IScalosFileTypePlugin = (struct ScalosFileTypePluginIFace *)GetInterface(ScalosFileTypePluginBase, "main", 1, NULL);
			}
#endif

		if (NULL == ScalosFileTypePluginBase)
			{
			stccpy(ttshd->ttshd_Buffer, "**MISSING PLUGIN**", ttshd->ttshd_BuffLen);
			d1(kprintf("%s/%s/%ld: Buffer=<%s>\n", __FILE__, __FUNC__, __LINE__, ttshd->ttshd_Buffer));
			break;
			}

		PluginArgs = stpblk(PluginArgs);
		d1(kprintf("%s/%s/%ld: PluginArgs=<%s>\n", __FILE__, __FUNC__, __LINE__, PluginArgs));

		SCAToolTipInfoString(ttshd, PluginArgs);

		d1(kprintf("%s/%s/%ld: Buffer=<%s>\n", __FILE__, __FUNC__, __LINE__, ttshd->ttshd_Buffer));
		} while (0);

#ifdef __amigaos4__
	if (IScalosFileTypePlugin)
		{
		DropInterface((struct Interface *)IScalosFileTypePlugin);
		}
#endif
	if (ScalosFileTypePluginBase)
		{
		CloseLibrary(ScalosFileTypePluginBase);
		}
	if (NameBuffer)
		FreePathBuffer(NameBuffer);
	if (PathBuffer)
		FreePathBuffer(PathBuffer);

	d1(kprintf("%s/%s/%ld: Buffer=<%s>\n", __FILE__, __FUNC__, __LINE__, ttshd->ttshd_Buffer));

	return ttshd->ttshd_Buffer;
}


static STRPTR Tooltip_LinkTarget(struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	STRPTR NameBuffer = NULL;

	(void) Args;

	d1(kprintf("%s/%s/%ld: IconNode=%08lx\n", __FILE__, __FUNC__, __LINE__, ttshd->ttshd_IconNode));

	do	{
		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		if (ST_SOFTLINK != ttshd->ttshd_DirEntryType)
			break;

		NameBuffer = AllocPathBuffer();

		d1(kprintf("%s/%s/%ld: NameBuffer=%08lx\n", __FILE__, __FUNC__, __LINE__, NameBuffer));
		if (NULL == NameBuffer)
			break;

		if (!NameFromLock(ttshd->ttshd_FileLock, NameBuffer, Max_PathLen))
			break;

		d1(kprintf("%s/%s/%ld: NameBuffer=<%s>\n", __FILE__, __FUNC__, __LINE__, NameBuffer));

		stccpy(ttshd->ttshd_Buffer, NameBuffer, ttshd->ttshd_BuffLen);
		} while (0);

	if (NameBuffer)
		FreePathBuffer(NameBuffer);

	return ttshd->ttshd_Buffer;
}

//----------------------------------------------------------------------------

static ULONG Tooltip_Hide_IsEmpty(struct ttDef *ttd, 
	struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	struct Hook testHook;
	STRPTR TestString;
	ULONG hidden;

	d1(kprintf("%s/%s/%ld: Args=<%s>\n", __FILE__, __FUNC__, __LINE__, Args);)

	testHook.h_Data = ttshd;
	SETHOOKFUNC(testHook, TooltipTTUserHookFunc);

	TestString = (STRPTR) CallHookPkt(&testHook, ttd, stpblk(Args));

	d1(kprintf("%s/%s/%ld: TestString=<%s>\n", __FILE__, __FUNC__, __LINE__, TestString);)

	hidden = NULL == TestString || 0 == strlen(TestString);

	d1(kprintf("%s/%s/%ld: hidden=%ld\n", __FILE__, __FUNC__, __LINE__, hidden);)

	return hidden;
}


static ULONG Tooltip_Hide_NoVolumeNode(struct ttDef *ttd, 
	struct ScaToolTipInfoHookData *ttshd, CONST_STRPTR Args)
{
	ULONG hidden;
	struct DosList *VolumeNode = NULL;

	if (ttshd->ttshd_IconNode && ttshd->ttshd_IconNode->in_DeviceIcon)
		VolumeNode = BADDR(ttshd->ttshd_IconNode->in_DeviceIcon->di_Info->id_VolumeNode);

	hidden = NULL == VolumeNode;

	d1(kprintf("%s/%s/%ld: hidden=%ld\n", __FILE__, __FUNC__, __LINE__, hidden);)

	return hidden;
}


static struct ttDef *CloneToolTipDef(const struct ttDef *ttdOrig)
{
	struct ttDef *ttdNew = NULL;
	struct ttDef **ttdLast = &ttdNew;
	struct ttDef *ttd;

	d1(kprintf("%s/%s/%ld: ttdOrig=%08lx\n", __FILE__, __FUNC__, __LINE__, ttdOrig);)

	for (; ttdOrig; ttdOrig = ttdOrig->ttd_Next)
		{
		ttd = *ttdLast = TT_CreateItem(TAG_END);
		if (NULL == ttd)
			break;

		*ttd = *ttdOrig;

		switch (ttd->ttd_Type)
			{
		case TTTYPE_Text:
		case TTTYPE_TextHook:
			if (ttdOrig->ttd_Contents.ttc_Text)
				{
				ttd->ttd_Contents.ttc_Text = ScalosAlloc(1 + strlen(ttdOrig->ttd_Contents.ttc_Text));
				d1(kprintf("%s/%s/%ld: ALLOC ttc_Text=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_Contents.ttc_Text));
				if (ttd->ttd_Contents.ttc_Text)
					strcpy(ttd->ttd_Contents.ttc_Text, ttdOrig->ttd_Contents.ttc_Text);
				}
			break;
		case TTTYPE_DTImage:
			if (ttdOrig->ttd_Contents.ttc_Image)
				{
				ttd->ttd_Contents.ttc_Image = ScalosAlloc(sizeof(struct DatatypesImage));
				d1(kprintf("%s/%s/%ld: ALLOC ttc_Image=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_Contents.ttc_Image));

				if (ttd->ttd_Contents.ttc_Image)
					{
					memset(ttd->ttd_Contents.ttc_Image, 0, sizeof(struct DatatypesImage));
					ttd->ttd_Contents.ttc_Image = CreateDatatypesImage(ttdOrig->ttd_Contents.ttc_Image->dti_Filename, 0);
					}
				}
			break;
		default:
			break;
			}
		ttd->ttd_Sub = CloneToolTipDef(ttdOrig->ttd_Sub);

		if (ttdOrig->ttd_HiddenHookText)
			{
			ttd->ttd_HiddenHookText = ScalosAlloc(1 + strlen(ttdOrig->ttd_HiddenHookText));
			d1(kprintf("%s/%s/%ld: ALLOC ttd_HiddenHookText=%08lx\n", __FILE__, __FUNC__, __LINE__, ttd->ttd_HiddenHookText));
			if (ttd->ttd_HiddenHookText)
				strcpy(ttd->ttd_HiddenHookText, ttdOrig->ttd_HiddenHookText);
			}

		// Clone TrueType font info
		if (ttdOrig->ttd_TTFontSpec)
			{
			ttd->ttd_TTFontSpec = AllocCopyString(ttdOrig->ttd_TTFontSpec);
			Scalos_OpenTTFont(ttd->ttd_TTFontSpec, &ttd->ttd_TTFont);
			}

		// Clone standard font info
		if (ttdOrig->ttd_TextAttr.ta_Name)
			ttd->ttd_TextAttr.ta_Name = AllocCopyString(ttdOrig->ttd_TextAttr.ta_Name);
		if (ttdOrig->ttd_Font)
			{
			ttd->ttd_Font = OpenDiskFont(&ttd->ttd_TextAttr);
			}

		ttdLast = &ttd->ttd_Next;
		}

	d1(kprintf("%s/%s/%ld: ttdNew=%08lx\n", __FILE__, __FUNC__, __LINE__, ttdNew);)

	return ttdNew;
}


static void BlitBackgroundTransparent(struct ttInfo *tti, struct BitMap *MaskBitMap)
{
	struct ARGB *SrcARGB;
	struct ARGB *DestARGB = NULL;

	do	{
		struct RastPort rp;

		InitRastPort(&rp);
		rp.BitMap = tti->tti_BackgroundBitMap;

		SrcARGB	= ScalosGfxCreateARGB(tti->tti_Window->Width,
			tti->tti_Window->Height,
			NULL);
		if (NULL == SrcARGB)
			break;

		DestARGB = ScalosGfxCreateARGB(tti->tti_Window->Width,
			tti->tti_Window->Height,
			NULL);
		if (NULL == DestARGB)
			break;

		// read screen background into DestARGB
		ScalosReadPixelArray(DestARGB,
			tti->tti_Window->Width * sizeof(struct ARGB),
			&rp,
			0, 0,
			tti->tti_Window->Width,
	                tti->tti_Window->Height);

		// Blur background from DestARGB to SrcARGB
		ScalosBlurPixelArray(SrcARGB, DestARGB,
			tti->tti_Window->Width,
			tti->tti_Window->Height,
			MaskBitMap->Planes[0]);

		// read window contents into DestARGB
		ScalosReadPixelArray(DestARGB,
			tti->tti_Window->Width * sizeof(struct ARGB),
			tti->tti_Window->RPort,
			0, 0,
			tti->tti_Window->Width,
	                tti->tti_Window->Height);

		// Merge window contents and background
		BlitARGB(tti->tti_Window->Width,
			tti->tti_Window->Height,
			SrcARGB,
			0, 0,
                        tti->tti_Window->Width,
			DestARGB,
			0, 0,
			255 - (CurrentPrefs.pref_TooltipsTransparency * 255) / 100);

		d1(KPrintF("%s/%s/%ld: AlphaChannel=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, bob->scbob_Bob.scbob_Bob2.scbob2_AlphaChannel));

		BlitARGBMask(tti->tti_Window->Width,
			tti->tti_Window->Height,
			SrcARGB,
	                0, 0,
			tti->tti_Window->Width,
			DestARGB,
	                0, 0,
			MaskBitMap,
			255);

		// write DestARGB to Window RastPort
		ScalosWritePixelArray(DestARGB,
			tti->tti_Window->Width * sizeof(struct ARGB),
			tti->tti_Window->RPort,
			0, 0,
			tti->tti_Window->Width,
			tti->tti_Window->Height);
		} while (0);

	if (DestARGB)
		ScalosGfxFreeARGB(&DestARGB);
	if (SrcARGB)
		ScalosGfxFreeARGB(&SrcARGB);
}


static void DrawWuLine(struct RastPort *rp, int X1, int Y1, UWORD LinePen)
{
	struct ARGB FgColor;
	ULONG LinePenRGB[3];
	int X0, Y0;

	X0 = rp->cp_x;
	Y0 = rp->cp_y;

	GetRGB32(iInfos.xii_iinfos.ii_Screen->ViewPort.ColorMap, LinePen, 1, LinePenRGB);

	FgColor.Red   = LinePenRGB[0] >> 24;
	FgColor.Green = LinePenRGB[1] >> 24;
	FgColor.Blue  = LinePenRGB[2] >> 24;
	FgColor.Alpha = (UBYTE) ~0;

	ScalosGfxDrawLineRastPort(rp, X0, Y0, X1, Y1, &FgColor);
}


// Draw an antialiased ellipse
static void outline_ellipse_antialias(struct RastPort *rp,
	WORD xc, WORD yc, WORD rx, WORD ry,
	WORD Segment, UBYTE color1, UBYTE color2)
{
	struct ARGB rgbColor1, rgbColor2;
	ULONG LinePenRGB[3];

	GetRGB32(iInfos.xii_iinfos.ii_Screen->ViewPort.ColorMap, color1, 1, LinePenRGB);

	rgbColor1.Red   = LinePenRGB[0] >> 24;
	rgbColor1.Green = LinePenRGB[1] >> 24;
	rgbColor1.Blue  = LinePenRGB[2] >> 24;
	rgbColor1.Alpha = (UBYTE) ~0;

	GetRGB32(iInfos.xii_iinfos.ii_Screen->ViewPort.ColorMap, color2, 1, LinePenRGB);

	rgbColor2.Red   = LinePenRGB[0] >> 24;
	rgbColor2.Green = LinePenRGB[1] >> 24;
	rgbColor2.Blue  = LinePenRGB[2] >> 24;
	rgbColor2.Alpha = (UBYTE) ~0;

	ScalosGfxDrawEllipseRastPort(rp, xc, yc, rx, ry, Segment, &rgbColor1, &rgbColor2);
}


static BOOL GetFileTypeFromTypeNode(struct ScaToolTipInfoHookData *ttshd)
{
	BOOL Success = FALSE;

	if (IS_TYPENODE(ttshd->ttshd_IconNode->in_FileType))
		{
		const struct TypeNode *tn = ttshd->ttshd_IconNode->in_FileType;

		ScaFormatStringMaxLength(ttshd->ttshd_Buffer, ttshd->ttshd_BuffLen,
			"[%s]", (ULONG) tn->tn_Name);

		tn = tn->tn_Parent;
		while (tn)
			{
			if (strlen(ttshd->ttshd_Buffer) + strlen(tn->tn_Name) + 2 < ttshd->ttshd_BuffLen)
				{
				strins(ttshd->ttshd_Buffer + 1, tn->tn_Name);
				strins(ttshd->ttshd_Buffer + 1 + strlen(tn->tn_Name), ".");
				}
			tn = tn->tn_Parent;
			}

		Success = TRUE;
		}

	return Success;
}




// ScalosInit.c
// $Date$
// $Revision$


#include <exec/types.h>
#include <exec/initializers.h>
#include <exec/execbase.h>
#include <graphics/gels.h>
#include <graphics/rastport.h>
#include <graphics/gfxbase.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <intuition/intuitionbase.h>
#include <utility/utility.h>
#include <utility/hooks.h>
#include <libraries/locale.h>
#include <libraries/ttengine.h>
#include <gadgets/colorwheel.h>
#include <workbench/workbench.h>
#include <workbench/startup.h>
#include <workbench/icon.h>
#include <cybergraphx/cybergraphics.h>
#include <datatypes/pictureclass.h>
#include <devices/conunit.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <dos/exall.h>

#define	__USE_SYSBASE

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/layers.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/iconobject.h>
#include <proto/utility.h>
#include <proto/gadtools.h>
#include <proto/datatypes.h>
#include <proto/cybergraphics.h>
#include <proto/wb.h>
#include <proto/locale.h>
#include <proto/diskfont.h>
#include <proto/icon.h>
#include <proto/guigfx.h>
#include "debug.h"
#include <proto/scalos.h>
#include <proto/scalosgfx.h>
#include <proto/scalosplugin.h>

#include <clib/alib_protos.h>

#include <defs.h>
#include <Year.h> // +jmc+

#include <datatypes/iconobject.h>
#include <scalos/scalos.h>

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

#include "scalos_structures.h"
#include "Scalos_rev.h"
#include "locale.h"
#include "functions.h"
#include "Variables.h"

//----------------------------------------------------------------------------

// local data structures

#define	LINE_TRACE	d1(KPrintF("%s/%s/%ld: tc_SigAlloc=%08lx\n", __FILE__, __FUNC__, __LINE__, FindTask(NULL)->tc_SigAlloc));\
			TRACE_AMITHLON(__LINE__);

struct ScrColorList
	{
	UWORD MaxPens;
	UWORD FirstPen;
	struct ColorWheelRGB Colors[1];
	};

struct ScalosClassList
	{
	CONST_STRPTR scl_ClassName;
	CONST_STRPTR scl_SuperclassName;
	ULONG scl_Priority;

	struct ScalosClass * (*scl_ClassInit)(const struct PluginClass *);
	};

struct LibListEntry
	{
	CONST_STRPTR lle_LibName;
	struct Library **lle_LibBase;
	WORD lle_LibVersion;
	ULONG lle_Flags;
#ifdef __amigaos4__
	CONST_STRPTR lle_IFaceName;
	struct Interface **lle_IFace;
	WORD lle_IFaceVersion;
#endif
	};

#ifdef __amigaos4__
#define LLE(libname, baseptr, version, flags, ifacename, ifaceptr, ifaceversion) \
	{libname, baseptr, version, flags, ifacename, ifaceptr, ifaceversion}
#else
#define LLE(libname, baseptr, version, flags, ifacename, ifaceptr, ifaceversion) \
	{libname, baseptr, version, flags}
#endif

// Values in lle_Flags
#define	LLEFLAGF_Optional	0x00000001

//----------------------------------------------------------------------------

// local functions

static void InitDiskPlugins(void);
static BOOL ReadDiskPluginData(struct PluginClass *plugDef);
static LONG LookupPenInDriPens(LONG Pen);
static void SetupICandy(void);
static void SetupTheme(void);
static BOOL AddInternalClasses(void);
static ULONG OpenLibraries(void);
static void CloseLibraries(void);
static void InitPrefsNotification(struct MainTask *mainTask);
static void QuitPrefsNotification(struct MainTask *mainTask);
static ULONG InitMainWindow(void);
static BOOL OpenConsoleDevice(void);
static void CloseConsoleDevice(void);
static SAVEDS(LONG) CompareRevPriFunc(struct Hook *hook, const struct PluginClass *pl2,
	const struct PluginClass *pl1);
static M68KFUNC_P3_PROTO(ULONG, ReadDiskPlugin,
	A0, Class *, cl,
	A2, Object *, o,
	A1, Msg, msg);
static void CleanupScalosClasses(void);
static void ScalosMain(LONG *ArgArray);

//----------------------------------------------------------------------------

// public Data

T_CONSOLEDEVICE	ConsoleDevice;

#if !defined(__amigaos4__) && !defined(__AROS__)
#include <dos.h>

// No options for newlib.library on OS4

#ifdef __GNUC__
// Options for libnix
char *__procname = "Workbench";
int __priority = 5;
unsigned long __stack = 16384;			// Stack requirement
unsigned int __stack_size = 16384;
#endif

#ifdef	__SASC
// Declarations for CBACK
extern BPTR __Backstdout;			// standard output when run in background
long __BackGroundIO = FALSE;			// Flag to tell it we don't want to do I/O
long __stack = 16384;				// Amount of stack space our task needs  
char *__procname = "Workbench";			// The name of the task to create        
long __priority = 5;				// The priority to run us at
#endif
#endif // !__amigaos4__

#if defined(__GNUC__) && !defined(__AROS__)
struct ExecBase *SysBase;
#endif

struct Process *ScalosMainTask;

SCALOSSEMAPHORE	WinListSemaphore;		// Semaphore for window list arbitration
struct ScaWindowList winlist = { NULL, NULL, NULL };

UWORD ScaLibPluginOpenCount;			// +jl+ 20010710 Scalos library open count by plugins
UBYTE fInitializingPlugins;			// Flag: set during OOP plugin initialization

ULONG HelpGroupID;				// common ID for window help group

// Library bases
struct Library *AslBase;
struct Library *CxBase;
struct Library *CyberGfxBase;
struct Library *DataTypesBase;
struct Library *DiskfontBase;
#ifndef	__SASC
struct DosLibrary *DOSBase;
#endif /* __SASC */
struct GfxBase *GfxBase;
struct Library *GuiGFXBase;
struct Library *GadToolsBase;
struct Library *IconBase;
struct Library *IconobjectBase;
struct Library *IFFParseBase;
struct IntuitionBase *IntuitionBase;
struct Library *LayersBase;
T_LOCALEBASE LocaleBase;
struct PopupMenuBase *PopupMenuBase;
struct Library *PreferencesBase;
T_REXXSYSBASE RexxSysBase;
struct ScalosBase *ScalosBase;
T_TIMERBASE TimerBase;
struct Library *WorkbenchBase;
T_UTILITYBASE UtilityBase;
struct Library *TitlebarImageBase;
struct Library *TTEngineBase;
struct Library *SQLite3Base;
struct ScalosGfxBase *ScalosGfxBase;

#ifdef __amigaos4__
// Library interfaces
struct AslIFace *IAsl;
struct CommoditiesIFace *ICommodities;
struct ConsoleIFace *IConsole;
struct CyberGfxIFace *ICyberGfx;
struct DataTypesIFace *IDataTypes;
struct DiskfontIFace *IDiskfont;
struct DOSIFace *IDOS;
struct GraphicsIFace *IGraphics;
struct GuiGFXIFace *IGuiGFX;
struct GadToolsIFace *IGadTools;
struct IconIFace *IIcon;
struct IconobjectIFace *IIconobject;
struct IFFParseIFace *IIFFParse;
struct IntuitionIFace *IIntuition;
struct LayersIFace *ILayers;
struct LocaleIFace *ILocale;
struct PopupMenuIFace *IPopupMenu;
struct PreferencesIFace *IPreferences;
struct RexxSysIFace *IRexxSys;
struct ScalosIFace *IScalos;
struct ScalosGfxIFace *IScalosGfx;
struct TimerIFace *ITimer;
struct WorkbenchIFace *IWorkbench;
struct UtilityIFace *IUtility;
//struct Library *ITitlebarImage;
struct TTEngineIFace *ITTEngine;
struct SQLite3Interface *ISQLite3;
#endif

//----------------------------------------------------------------------------

// local Data

static struct MsgPort *ioConsolePort;		// MsgPort for ioConsole
static struct IOStdReq *ioConsole;		// IORequest to open console.device

static struct Hook CompareRevPriHook =
	{
	{ NULL, NULL },
	HOOKFUNC_DEF(CompareRevPriFunc),	// h_Entry + h_SubEntry
	NULL,						// h_Data
	};

// All classes whose names don't end with ".sca" are inherited from
// standard BOOPSI classes instead of other Scalos classes!
struct ScalosClassList InternalScalosClasses[] =
	{
	{ "IconWindow.sca",	"Root.sca",		-80, initIconWindowClass },
	{ "TextWindow.sca",	"IconWindow.sca",	-60, initTextWindowClass },
	{ "DeviceWindow.sca",	"IconWindow.sca",	-60, initDeviceWindowClass },
	{ "Title.sca",		"Root.sca",		-80, initTitleClass },
	{ "Window.sca",		"Root.sca",		-90, initWindowClass },
	{ "DeviceList.sca",	"Root.sca",		-80, initDevListClass },
	{ "FileTransfer.sca",	"Root.sca",		-80, initFileTransClass },
	{ "GadgetBar.sca",	GADGETCLASS,		-80, initGadgetBarClass },
	{ "GadgetBarImage.sca",	GADGETCLASS,		-80, initGadgetBarImageClass },
	{ "GadgetBarText.sca",	GADGETCLASS,		-80, initGadgetBarTextClass },
	{ "Gauge.sca",		GADGETCLASS,		-80, initGaugeGadgetClass },
	{ "ButtonGadget.sca",	BUTTONGCLASS,		-80, initButtonGadgetClass },
	{ "SeparatorGadget.sca", BUTTONGCLASS,		-80, initSeparatorGadgetClass },
	{ "StringGadget.sca",	GADGETCLASS,		-80, initStringGadgetClass },
	{ "CycleGadget.sca",	GADGETCLASS,		-80, initCycleGadgetClass },
	{ "HistoryGadget.sca",	GADGETCLASS,		-80, initHistoryGadgetClass },
	{ "FrameImage.sca",	IMAGECLASS,		-80, initFrameImageClass },

	{ NULL, NULL, },
	};

static struct LibListEntry LibList[] =
	{
	// !! "dos.library" and "intuition.library" should remain the very first entries !!
	LLE(DOSNAME,	      (struct Library **)(APTR) &DOSBase,             	39, 0L, "main", (struct Interface **)(APTR) &IDOS, 1),
	LLE("intuition.library", (struct Library **)(APTR) &IntuitionBase,    	39, 0L, "main", (struct Interface **)(APTR) &IIntuition, 1),

	LLE("cybergraphics.library",      	&CyberGfxBase,			40, LLEFLAGF_Optional, "main", (struct Interface **)(APTR) &ICyberGfx, 1),
	LLE("datatypes.library",	      	&DataTypesBase,			39, 0L, "main", (struct Interface **)(APTR) &IDataTypes, 1),
	LLE(GRAPHICSNAME,   (struct Library **)(APTR) &GfxBase,       	      	39, 0L, "main", (struct Interface **)(APTR) &IGraphics, 1),
	LLE("layers.library",	  		&LayersBase,	        	39, 0L, "main", (struct Interface **)(APTR) &ILayers, 1),
	LLE("gadtools.library",	      		&GadToolsBase,			39, 0L, "main", (struct Interface **)(APTR) &IGadTools, 1),
	LLE(ICONNAME,	      			&IconBase,	        	39, 0L, "main", (struct Interface **)(APTR) &IIcon, 1),
	LLE("diskfont.library",	      		&DiskfontBase,			37, 0L, "main", (struct Interface **)(APTR) &IDiskfont, 1),
	LLE(UTILITYNAME,    (struct Library **)(APTR) &UtilityBase,   	      	37, 0L, "main", (struct Interface **)(APTR) &IUtility, 1),
	LLE("iffparse.library",	      		&IFFParseBase,			39, 0L, "main", (struct Interface **)(APTR) &IIFFParse, 1),
	LLE(ICONOBJECTNAME,		  	&IconobjectBase,        	39, 0L, "main", (struct Interface **)(APTR) &IIconobject, 1),
	LLE(WORKBENCH_NAME,		  	&WorkbenchBase,	        	39, 0L, "main", (struct Interface **)(APTR) &IWorkbench, 1),
	LLE("rexxsyslib.library", (struct Library **)(APTR) &RexxSysBase,     	36, 0L, "main", (struct Interface **)(APTR) &IRexxSys, 1),
	LLE("preferences.library",	      	&PreferencesBase,		39, 0L, "main", (struct Interface **)(APTR) &IPreferences, 1),
	LLE("popupmenu.library", (struct Library **)(APTR) &PopupMenuBase,     	 3, LLEFLAGF_Optional, "main", (struct Interface **)(APTR) &IPopupMenu, 1),
	LLE("guigfx.library",	  		&GuiGFXBase,	          	 1, LLEFLAGF_Optional, "main", (struct Interface **)(APTR) &IGuiGFX, 1),
	LLE("locale.library",	  (struct Library **)(APTR) &LocaleBase,      	38, LLEFLAGF_Optional, "main", (struct Interface **)(APTR) &ILocale, 1),
	LLE("commodities.library",	      	&CxBase,	        	39, 0L, "main", (struct Interface **)(APTR) &ICommodities, 1),
	LLE("asl.library",		  	&AslBase,	        	39, 0L, "main", (struct Interface **)(APTR) &IAsl, 1),
	LLE(TTENGINENAME,		  	&TTEngineBase,	        	 6, LLEFLAGF_Optional, "main", (struct Interface **)(APTR) &ITTEngine, 1),
	LLE(SCALOSGFXNAME,  (struct Library **)(APTR) &ScalosGfxBase,		42, 0L, "main", (struct Interface **)(APTR)  &IScalosGfx, 1),
	LLE("sqlite3.library",			&SQLite3Base,			43, LLEFLAGF_Optional, "main", (struct Interface **)(APTR) &ISQLite3, 1),
	LLE("titlebar.image",	  		&TitlebarImageBase,     	40, LLEFLAGF_Optional, NULL, NULL, 0),
	LLE("Images/titlebar.image",      	&TitlebarImageBase,     	40, LLEFLAGF_Optional, NULL, NULL, 0),
	LLE("Classes/Images/titlebar.image",  	&TitlebarImageBase,		40, LLEFLAGF_Optional, NULL, NULL, 0),
	LLE(NULL, 		      		NULL, 		        	 0, 0L, NULL, NULL, 0),
	};

static struct NotifyTab ScalosNotifyTable[] =
	{
	{ "ENV:Scalos/Pattern.prefs", 	NewPatternPrefs	},
	{ "ENV:Scalos/Menu.prefs",	NewMenuPrefs	},
	{ "ENV:Scalos/Menu13.prefs",	NewMenuPrefs	},
	{ "ENV:sys/font.prefs",		NewFontPrefs	},
	{ "ENV:Scalos/Scalos.prefs",	NewMainPrefs	},
	{ "ENV:Scalos/Palette.prefs",	NewPalettePrefs	},
	{ "ENV:Scalos/Palette13.prefs",	NewPalettePrefs	},
	{ "ENV:deficons.prefs",		NewDefIconsPrefs },
	{ "ENV:sys/locale.prefs",	NewLocalePrefs	},
	{ "ENV:Scalos/FileTypes",	NewFileTypesPrefs },
	{ NULL, NULL },
	};

//----------------------------------------------------------------------------

static void InitDiskPlugins(void)
{
///
	struct PluginClass *plug, *plugNext;

	LINE_TRACE;

	for (plug=ScalosPluginList; plug; plug=plugNext)
		{
		plugNext = (struct PluginClass *) plug->plug_Node.mln_Succ;

		if (NULL == plug->plug_classhook && NULL == plug->plug_ClassInit)
			{
			if (!ReadDiskPluginData(plug))
				{
				// Reading plugin data failed, remove it from list
				SCA_FreeNode((struct ScalosNodeList *)(APTR) &ScalosPluginList, &plug->plug_Node);
				}
			LINE_TRACE;
			}
		}

	LINE_TRACE;

	// sort classes by priority
	SCA_SortNodes((struct ScalosNodeList *)(APTR) &ScalosPluginList, &CompareRevPriHook, SCA_SortType_Best);

	LINE_TRACE;

	for (plug=ScalosPluginList; plug; plug = (struct PluginClass *) plug->plug_Node.mln_Succ)
		{
		d1(KPrintF("%s/%s/%ld: ClassName=<%s>  Priority=%ld\n", __FILE__, __FUNC__, __LINE__, \
			plug->plug_classname, plug->plug_priority));

		if (plug->plug_ClassInit)
			{
			plug->plug_class = (*plug->plug_ClassInit)(plug);

			if (plug->plug_class)
				{
				plug->plug_instsize = plug->plug_class->sccl_class->cl_InstSize;
				plug->plug_classhook = plug->plug_class->sccl_class->cl_Dispatcher.h_SubEntry;
				}
			}
		else
			{
			plug->plug_class = SCA_MakeScalosClass(plug->plug_classname, plug->plug_superclassname,
				plug->plug_instsize,
				plug->plug_classhook ? plug->plug_classhook : (HOOKFUNC) ReadDiskPlugin);
			}

		if (plug->plug_class)
			plug->plug_class->sccl_plugin = plug;
		}

	LINE_TRACE;
///
}


static BOOL ReadDiskPluginData(struct PluginClass *plugDef)
{
///
	BOOL Success = FALSE;
	struct ScaOOPPluginBase *ScalosPluginBase;
#ifdef __amigaos4__
	struct ScalosPluginIFace *IScalosPlugin = NULL;
#endif
	const struct ScaClassInfo *ci;

	d1(KPrintF("%s/%s/%ld: Plugin FileName=<%s>\n", __FILE__, __FUNC__, __LINE__, plugDef->plug_filename));

	do	{
		fInitializingPlugins = TRUE;

		ScalosPluginBase = (struct ScaOOPPluginBase *) OpenLibrary(plugDef->plug_filename, 0);
#ifdef __amigaos4__
		if (NULL != ScalosPluginBase)
			IScalosPlugin = (struct ScalosPluginIFace *) GetInterface((struct Library *)ScalosPluginBase, "main", 1, NULL);
#endif

		d1(KPrintF("%s/%s/%ld: ScalosPluginBase=%08lx\n", __FILE__, __FUNC__, __LINE__, ScalosPluginBase));
		if (NULL == ScalosPluginBase)
			break;

		if (MAKE_ID('P','L','U','G') != ScalosPluginBase->soob_Signature)
			{
			d1(KPrintF("%s/%s/%ld: Wrong Signature(%08lx)\n", __FILE__, __FUNC__, __LINE__,
				ScalosPluginBase->soob_Signature));
			break;
			}

		ci = SCAGetClassInfo();
		if (NULL == ci)
			{
			d1(KPrintF("%s/%s/%ld: Could not get classinfo\n", __FILE__, __FUNC__, __LINE__));
			break;
			}

		d1(KPrintF("%s/%s/%ld: ClassInfo=%08lx  NeededVersion=%ld\n", __FILE__, __FUNC__, __LINE__, ci, ci->ci_neededversion));

		if (ci->ci_neededversion > ScalosBase->scb_LibNode.lib_Version)
			break;

		plugDef->plug_priority = ci->ci_priority;
		d1(kprintf("%s/%s/%ld: Priority=%ld\n", __FILE__, __FUNC__, __LINE__, plugDef->plug_priority));

		plugDef->plug_instsize = ci->ci_instsize;
		d1(kprintf("%s/%s/%ld: InstSize=%ld\n", __FILE__, __FUNC__, __LINE__, plugDef->plug_instsize));

		plugDef->plug_classname = AllocCopyString(ci->ci_classname);
		if (NULL == plugDef->plug_classname)
			break;

		d1(kprintf("%s/%s/%ld: ClassName=<%s>\n", __FILE__, __FUNC__, __LINE__, plugDef->plug_classname));

		plugDef->plug_superclassname = AllocCopyString(ci->ci_superclassname);
		if (NULL == plugDef->plug_superclassname)
			break;

		d1(kprintf("%s/%s/%ld: SuperclassName=<%s>\n", __FILE__, __FUNC__, __LINE__, plugDef->plug_superclassname));

		Success = TRUE;
		} while (0);

	d1(kprintf("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	fInitializingPlugins = FALSE;

	if (ScalosPluginBase)
		{
#ifdef __amigaos4__
		if (IScalosPlugin)
			DropInterface((struct Interface *)IScalosPlugin);
#endif
		CloseLibrary((struct Library *) ScalosPluginBase);
		}

	return Success;
///
}


static void SetupICandy(void)
{
	char Buffer[20];

	if (GetVar("scalos/icandy", Buffer, sizeof(Buffer), GVF_GLOBAL_ONLY) <= 0)
		{
		d1(KPrintF("%s/%s/%ld: Scalos/icandy NOT FOUND\n", __FILE__, __FUNC__, __LINE__));
		ScaFormatStringMaxLength(Buffer, sizeof(Buffer), "%lu", CurrentPrefs.pref_iCandy);

		SetVar("scalos/icandy", Buffer, -1, GVF_GLOBAL_ONLY);
		}
	else
		{
		d1(KPrintF("%s/%s/%ld: Scalos/iCandy=<%s>\n", __FILE__, __FUNC__, __LINE__, Buffer));
		StrToLong(Buffer, (LONG *) &CurrentPrefs.pref_iCandy);
		}
}


static void SetupTheme(void)
{
	if (!ExistsAssign("THEME"))
		AssignPath("THEME", "Scalos:themes/default");
}


static BOOL OpenConsoleDevice(void)
{
///
	ioConsolePort = CreateMsgPort();
	ioConsole = (struct IOStdReq *)CreateIORequest(ioConsolePort, sizeof(struct IOStdReq));
	if (NULL == ioConsole)
		return FALSE;

	if (0 != OpenDevice("console.device", CONU_LIBRARY, (struct IORequest *) ioConsole, CONFLAG_DEFAULT))
		return FALSE;

	ConsoleDevice = (T_CONSOLEDEVICE) ioConsole->io_Device;
#ifdef __amigaos4__
	IConsole = (struct ConsoleIFace *)GetInterface((struct Library *)ConsoleDevice, "main", 1, NULL);
	if (NULL == IConsole)
		{
		CloseDevice((struct IORequest *)ioConsole);
		ConsoleDevice = NULL;
		return FALSE;
		}
#endif

	return TRUE;
///
}


static void CloseConsoleDevice(void)
{
///
	if (ConsoleDevice)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)IConsole);
#endif
		ConsoleDevice = NULL;
		CloseDevice((struct IORequest *) ioConsole);
		}
	if (ioConsole)
		{
		DeleteIORequest((struct IORequest *) ioConsole);
		ioConsole = NULL;
		}
	if (ioConsolePort)
		{
		DeleteMsgPort(ioConsolePort);
		ioConsolePort = NULL;
		}
///
}


static SAVEDS(LONG) CompareRevPriFunc(struct Hook *hook, const struct PluginClass *pl2,
	const struct PluginClass *pl1)
{
	d1(KPrintF("%s/%s/%ld: pl1=%08lx  pl2=%08lx\n", __FILE__, __FUNC__, __LINE__, pl1, pl2));

	(void) hook;

	return (pl2->plug_priority - pl1->plug_priority);
}


static M68KFUNC_P3(ULONG, ReadDiskPlugin,
	A0, Class *, cl,
	A2, Object *, o,
	A1, Msg, msg)
{
///
	struct ScalosClass *sccl = (struct ScalosClass *) cl->cl_UserData;
	BOOL Success = FALSE;

	d1(kprintf("%s/%s/%ld: sccl=%08lx\n", __FILE__, __FUNC__, __LINE__, sccl));

	if (sccl)
		{
		struct PluginClass *plug = sccl->sccl_plugin;
		struct ScaOOPPluginBase *ScalosPluginBase;
#ifdef __amigaos4__
		struct ScalosPluginIFace *IScalosPlugin;
#endif

		fInitializingPlugins = TRUE;

		d1(kprintf("%s/%s/%ld: plug=%08lx  plug_base=%08lx\n", __FILE__, __FUNC__, __LINE__, plug, plug->plug_base));

		if (NULL == plug->plug_base)
			{
			plug->plug_base = OpenLibrary(plug->plug_filename, 0);
#ifdef __amigaos4__
		    if (plug->plug_base)
				plug->plug_iface = GetInterface(plug->plug_base, "main", 1, NULL);
#endif
			}

		d1(kprintf("%s/%s/%ld: plug=%08lx  plug_base=%08lx\n", __FILE__, __FUNC__, __LINE__, plug, plug->plug_base));

		ScalosPluginBase = (struct ScaOOPPluginBase *) plug->plug_base;
#ifdef __amigaos4__
		IScalosPlugin = (struct ScalosPluginIFace *) plug->plug_iface;
#endif

		do	{
			const struct ScaClassInfo *ci;

			d1(kprintf("%s/%s/%ld: plug=%08lx  plug_base=%08lx\n", __FILE__, __FUNC__, __LINE__, plug, plug->plug_base));

			if (NULL == ScalosPluginBase)
				break;

			d1(kprintf("%s/%s/%ld: plug=%08lx  plug_base=%08lx\n", __FILE__, __FUNC__, __LINE__, plug, plug->plug_base));

			if (MAKE_ID('P','L','U','G') != ScalosPluginBase->soob_Signature)
				break;

			d1(kprintf("%s/%s/%ld: plug=%08lx  plug_base=%08lx\n", __FILE__, __FUNC__, __LINE__, plug, plug->plug_base));

			ci = SCAGetClassInfo();
			d1(kprintf("%s/%s/%ld: ci=%08lx\n", __FILE__, __FUNC__, __LINE__, ci));
			if (NULL == ci)
				break;

			d1(kprintf("%s/%s/%ld: ci=%08lx  Hook=%08lx\n", __FILE__, __FUNC__, __LINE__, ci, ci->ci_Hook));

			cl->cl_Dispatcher = ci->ci_Hook;

			Success = TRUE;
			} while (0);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		fInitializingPlugins = FALSE;
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	if (!Success)
		{
		cl->cl_UserData = (ULONG)NULL;
		return DoSuperMethodA(cl, o, msg);
		}

	d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

	return CoerceMethodA(cl, o, msg);
///
}
M68KFUNC_END


static LONG LookupPenInDriPens(LONG Pen)
{
///
	LONG n;

	for (n=0; n < sizeof(PalettePrefs.pal_driPens)/sizeof(PalettePrefs.pal_driPens[0]); n++)
		{
		if (NO_PEN == PalettePrefs.pal_driPens[n])
			break;

		if (PalettePrefs.pal_driPens[n] == Pen)
			return n;
		}

	return NO_PEN;
///
}


SAVEDS(ULONG) LockScalosPens(void)
{
///
	ULONG UnAllocatedPens = 0;

	memset(PalettePrefs.pal_AllocatedPens, NO_PEN, sizeof(PalettePrefs.pal_AllocatedPens));

	if (PalettePrefs.pal_ScreenColorList && iInfos.xii_iinfos.ii_Screen)
		{
		struct ScrColorList *cList = (struct ScrColorList *) PalettePrefs.pal_ScreenColorList;
		ULONG n, Pen;

		d1(for (n=0; n < sizeof(PalettePrefs.pal_driPens)/sizeof(PalettePrefs.pal_driPens[0]); n++)\
			{\
			kprintf("%s/%s/%ld: driPens[%ld]=%ld \n", \
				__FILE__, __FUNC__, __LINE__, n, PalettePrefs.pal_driPens[n]);\
			});

		d1(kprintf("%s/%s/%ld: FirstPen=%ld  MaxPens=%ld\n", __FILE__, __FUNC__, __LINE__, cList->FirstPen, cList->MaxPens));

		for (n=0, Pen=cList->FirstPen; n<cList->MaxPens && n < PENIDX_MAX; n++, Pen++)
			{
			LONG gotPen;
			LONG driPen;

			d1(kprintf("%s/%s/%ld: Pen=%ld  r=%08lx g=%08lx b=%08lx\n", __FILE__, __FUNC__, __LINE__, Pen, cList->Colors[n].cw_Red, cList->Colors[n].cw_Green, cList->Colors[n].cw_Blue));

			driPen = LookupPenInDriPens(Pen);

			if (NO_PEN == driPen)
				{
				PalettePrefs.pal_AllocatedPens[n] = gotPen = ObtainPen(iInfos.xii_iinfos.ii_Screen->ViewPort.ColorMap,
					Pen, cList->Colors[n].cw_Red, cList->Colors[n].cw_Green, cList->Colors[n].cw_Blue, 0);

				d1(kprintf("%s/%s/%ld: gotPen=%ld\n", __FILE__, __FUNC__, __LINE__, gotPen));

				if (NO_PEN == gotPen)
					{
					PalettePrefs.pal_AllocatedPens[n] = gotPen = ObtainPen(iInfos.xii_iinfos.ii_Screen->ViewPort.ColorMap, NO_PEN, cList->Colors[n].cw_Red, cList->Colors[n].cw_Green, cList->Colors[n].cw_Blue, 0);

					d1(kprintf("%s/%s/%ld: gotPen=%ld\n", __FILE__, __FUNC__, __LINE__, gotPen));
					}

				if (NO_PEN == gotPen)
					UnAllocatedPens++;
				}
			else
				{
				PalettePrefs.pal_AllocatedPens[n] = ObtainBestPen(iInfos.xii_iinfos.ii_Screen->ViewPort.ColorMap,
					cList->Colors[n].cw_Red,
					cList->Colors[n].cw_Green,
					cList->Colors[n].cw_Blue,
					OBP_Precision, PRECISION_GUI,
					OBP_FailIfBad, FALSE,
					TAG_END);
				d1(KPrintF("%s/%s/%ld: pal_AllocatedPens[%ld]=%ld\n", \
					__FILE__, __FUNC__, __LINE__, n, PalettePrefs.pal_AllocatedPens[n]));
				}
			}

		for (n=0; n < PENIDX_MAX; n++)
			{
			PalettePrefs.pal_PensList[n] = PalettePrefs.pal_AllocatedPens[PalettePrefs.pal_origPensList[n] - cList->FirstPen];

			d1(kprintf("%s/%s/%ld: prefPens[%ld]=%ld  origPens[%ld]=%ld\n", \
				__FILE__, __FUNC__, __LINE__, n, PalettePrefs.pal_PensList[n], \
                                n, PalettePrefs.pal_origPensList[n]));
			}
		}

	return UnAllocatedPens;
///
}


SAVEDS(void) UnlockScalosPens(void)
{
///
	if (PalettePrefs.pal_ScreenColorList && iInfos.xii_iinfos.ii_Screen)
		{
		ULONG n;

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		for (n=0; n < PENIDX_MAX; n++)
			{
			if (NO_PEN != PalettePrefs.pal_AllocatedPens[n])
				{
				ReleasePen(iInfos.xii_iinfos.ii_Screen->ViewPort.ColorMap, PalettePrefs.pal_AllocatedPens[n]);
				PalettePrefs.pal_AllocatedPens[n] = NO_PEN;
				}
			}
		}
///
}


static BOOL AddInternalClasses(void)
{
///
	short n;

	for (n=0; InternalScalosClasses[n].scl_ClassName; n++)
		{
		struct PluginClass *plg = (struct PluginClass *) SCA_AllocStdNode((struct ScalosNodeList *)(APTR) &ScalosPluginList, NTYP_PluginNode);

		d1(kprintf("%s/%s/%ld: ClassName=<%s>  plg=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, InternalScalosClasses[n].scl_ClassName, plg));

		if (NULL == plg)
			return FALSE;

		plg->plug_classname = InternalScalosClasses[n].scl_ClassName;
		plg->plug_superclassname = InternalScalosClasses[n].scl_SuperclassName;
		plg->plug_priority = InternalScalosClasses[n].scl_Priority;
		plg->plug_ClassInit = InternalScalosClasses[n].scl_ClassInit;
		}

	return TRUE;
///
}


static ULONG OpenLibraries(void)
{
///
	struct LibListEntry *lle;

	for (lle=LibList; lle->lle_LibName; lle++)
		{
		if (NULL == *lle->lle_LibBase)
			{
			*lle->lle_LibBase = OpenLibrary((STRPTR) lle->lle_LibName, lle->lle_LibVersion);
#ifdef __amigaos4__
			if (*lle->lle_LibBase && lle->lle_IFace != NULL)
				{
				*lle->lle_IFace = GetInterface(*lle->lle_LibBase, lle->lle_IFaceName, lle->lle_IFaceVersion, NULL);
				if (!(*lle->lle_IFace))
					{
					CloseLibrary(*lle->lle_LibBase);
					*lle->lle_LibBase = NULL;
					}
				}
#endif

			if (NULL == *lle->lle_LibBase &&
				!(lle->lle_Flags & LLEFLAGF_Optional))
				{
				if (IntuitionBase)
					{
					UseRequestArgs(NULL, MSGID_LIBERRORNAME, MSGID_OKNAME, 
						2,
						lle->lle_LibName,
						lle->lle_LibVersion);
					}

				return FALSE;
				}
			}
		}

	return TRUE;
///
}


static void CloseLibraries(void)
{
///
	struct LibListEntry *lle;

	for (lle=LibList; lle->lle_LibName; lle++)
		{
		if (*lle->lle_LibBase)
			{
			Forbid();
			RemLibrary(*lle->lle_LibBase);
			CloseLibrary(*lle->lle_LibBase);
			Permit();

			// do not clear DOSBase here!
			if (((struct Library **)(APTR) &DOSBase) != lle->lle_LibBase)
				*lle->lle_LibBase = NULL;
			}

		}
///
}


static void InitPrefsNotification(struct MainTask *mainTask)
{
///
	ULONG n;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	for (n=0; ScalosNotifyTable[n].nft_FileName; n++)
		{
		BPTR fLock;

		fLock = Lock((STRPTR) ScalosNotifyTable[n].nft_FileName, ACCESS_READ);
		if (fLock)
			{
			UnLock(fLock);

			d1(kprintf("%s/%s/%ld: n=%ld  <%s>\n", __FILE__, __FUNC__, __LINE__, n, ScalosNotifyTable[n].nft_FileName));
			AddToMainNotifyList(&ScalosNotifyTable[n], NRF_WAIT_REPLY);
			}
		}

	d1(kprintf("%s/%s/%ld: END\n", __FILE__, __FUNC__, __LINE__));
///
}


static void QuitPrefsNotification(struct MainTask *mainTask)
{
///
	while (mainTask->mainnotifylist)
		RemFromMainNotifyList(mainTask->mainnotifylist);
///
}


static ULONG InitMainWindow(void)
{
///
	struct ScaWindowStruct *NodeList = NULL;
	ULONG Success = FALSE;
	struct SM_StartWindow *smsw = NULL;

	d1(kprintf("%s/%s/%ld: START\n", __FILE__, __FUNC__, __LINE__));

	SplashDisplayProgress(GetLocString(MSGID_PROGRESS_MAINWINDOW), 0);

	do	{
		STATIC_PATCHFUNC(WindowTask)
		struct ScaWindowStruct *wsMain;

		wsMain = (struct ScaWindowStruct *) SCA_AllocStdNode((struct ScalosNodeList *)(APTR) &NodeList, NTYP_WindowStruct);
		if (NULL == wsMain)
			break;

		d1(kprintf("%s/%s/%ld: wsMain=%08lx\n", __FILE__, __FUNC__, __LINE__, wsMain));

		wsMain->ws_Left = CurrentPrefs.pref_WBWindowBox.Left;
		wsMain->ws_Top = CurrentPrefs.pref_WBWindowBox.Top;
		wsMain->ws_Width = CurrentPrefs.pref_WBWindowBox.Width;
		wsMain->ws_Height = CurrentPrefs.pref_WBWindowBox.Height;

		wsMain->ws_WindowOpacityActive = CurrentPrefs.pref_ActiveWindowTransparency;
		wsMain->ws_WindowOpacityInactive = CurrentPrefs.pref_InactiveWindowTransparency;

		wsMain->ws_IconSizeConstraints = CurrentPrefs.pref_IconSizeConstraints;
		wsMain->ws_IconScaleFactor = CurrentPrefs.pref_IconScaleFactor;

		wsMain->ws_xoffset = CurrentPrefs.pref_WBWindowXOffset;
		wsMain->ws_yoffset = CurrentPrefs.pref_WBWindowYOffset;

		d1(KPrintF("%s/%s/%ld:  pref_WBBackdrop=%ld\n", __FILE__, __FUNC__, __LINE__, CurrentPrefs.pref_WBBackdrop));
		if (CurrentPrefs.pref_WBBackdrop)
			wsMain->ws_Flags |= WSV_FlagF_Backdrop;

		wsMain->ws_Flags |= WSV_FlagF_ShowAllFiles;
		wsMain->ws_WindowType = WSV_Type_DeviceWindow;

		wsMain->ws_PatternNumber = PATTERNNR_WorkbenchDefault;
		wsMain->ws_PatternNode = GetPatternNode(wsMain->ws_PatternNumber, NULL);
		d1(kprintf("%s/%s/%ld: ws_PatternNode=%08lx\n", __FILE__, __FUNC__, __LINE__, wsMain->ws_PatternNode));

		wsMain->ms_ClassName = (STRPTR) "DeviceWindow.sca";

		smsw = (struct SM_StartWindow *) SCA_AllocMessage(MTYP_StartWindow, 0);
		d1(kprintf("%s/%s/%ld: smsw=%08lx\n", __FILE__, __FUNC__, __LINE__, smsw));
		if (NULL == smsw)
			break;

		smsw->ScalosMessage.sm_Message.mn_ReplyPort = iInfos.xii_iinfos.ii_MainMsgPort;
		smsw->WindowStruct = wsMain;

		iInfos.xii_iinfos.ii_MainWindowStruct = iInfos.xii_iinfos.ii_AppWindowStruct = wsMain;

		SplashDisplayProgress(GetLocString(MSGID_PROGRESS_STARTWINDOWPROC), 0);

		wsMain->ws_Task = CreateNewProcTags(NP_WindowPtr, ~0,
			NP_StackSize, CurrentPrefs.pref_DefaultStackSize,
			NP_Priority, 1,
			NP_Cli, TRUE,
			NP_CommandName, (ULONG) "Scalos_Window_Task_Main",
			NP_Name, (ULONG) "Scalos_Window_Task_Main",
			NP_Entry, (ULONG) PATCH_NEWFUNC(WindowTask),
			NP_CurrentDir, wsMain->ws_Lock,
			TAG_END);

		d1(kprintf("%s/%s/%ld: ws_Task=%08lx\n", __FILE__, __FUNC__, __LINE__, wsMain->ws_Task));
		if (NULL == wsMain->ws_Task)
			break;

		PutMsg(&wsMain->ws_Task->pr_MsgPort, &smsw->ScalosMessage.sm_Message);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		WaitReply(iInfos.xii_iinfos.ii_MainMsgPort, &MainWindowTask->mwt, MTYP_StartWindow);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		Success = TRUE;
		} while (0);

	d1(kprintf("%s/%s/%ld: Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	if (!Success)
		{
		SCA_FreeAllNodes((struct ScalosNodeList *)(APTR) &NodeList);
		if (smsw)
			SCA_FreeMessage(&smsw->ScalosMessage);
		}

	d1(kprintf("%s/%s/%ld: Finished  Success=%ld\n", __FILE__, __FUNC__, __LINE__, Success));

	return Success;
///
}


static void CleanupScalosClasses(void)
{
///
	if (ScalosBase)
		{
		struct PluginClass *plug, *plugNext;

		ScalosObtainSemaphore(&ClassListSemaphore);

		// sort classes by priority
		SCA_SortNodes((struct ScalosNodeList *)(APTR) &ScalosPluginList, &CompareRevPriHook, SCA_SortType_Best);

		for (plug=ScalosPluginList; plug; plug=plugNext)
			{
			plugNext = (struct PluginClass *) plug->plug_Node.mln_Succ;

			SCA_FreeScalosClass(plug->plug_class);

			if (plug->plug_base)
				{
				Forbid();
#ifdef __amigaos4__
				if (plug->plug_iface)
					{
					DropInterface(plug->plug_iface);
					plug->plug_iface = NULL;
					}
#endif
				CloseLibrary(plug->plug_base);
				RemLibrary(plug->plug_base);
				Permit();

				plug->plug_base = NULL;
				}
			if (plug->plug_filename)
				{
				FreeCopyString(plug->plug_filename);
				FreeCopyString((APTR) plug->plug_classname);
				FreeCopyString((APTR) plug->plug_superclassname);
				}

			SCA_FreeNode((struct ScalosNodeList *)(APTR) &ScalosPluginList, &plug->plug_Node);
			}

		while (ScalosClassList)
			{
			SCA_FreeScalosClass(ScalosClassList);
			}

		ScalosReleaseSemaphore(&ClassListSemaphore);
		}
///
}


int main(int argc, char **argv)
{
///
	static CONST_STRPTR WbProcName = "Workbench";
	struct CommandLineInterface *myCli;
	struct RDArgs *argsPtr = NULL;
	BPTR origCommandName = (BPTR)NULL;
	BSTR origCommandFile = (BSTR)NULL;
	LONG argArray[5];
	struct Task *MyTask;
	STRPTR OrigTaskName;

	d1(kprintf("%s/%s/%ld: argc=%ld\n", __FILE__, __FUNC__, __LINE__, argc));

	memset(argArray, 0, sizeof(argArray));

	DOSBase = (struct DosLibrary *) OpenLibrary(DOSNAME, 0);
	if (NULL == DOSBase)
		return RETURN_ERROR;

	if (argc > 0)
		{
		argsPtr = ReadArgs("EMU=EMULATIONMODE/S,NOSPLASH/S,FORCEPALETTE/S", argArray, NULL);
		if (argsPtr == NULL)
			{
			PrintFault(IoErr(), NULL);	/* prints the appropriate err message */
			return RETURN_ERROR;
			}
		}

	d1(KPrintF("%s/%s/%ld: EMULATIONMODE=%ld  NOSPLASH=%ld  FORCEPALETTE=%ld\n", \
		__FILE__, __FUNC__, __LINE__, argArray[0], argArray[1], argArray[2]));

	myCli = Cli();
	if (myCli)
		{
		// allocate own buffer for cli_CommandName
		// to avoid trashing of very small (just enough for "Scalos") buffer 
		// allocated by SAS/C cback.o startup code.
		origCommandName = myCli->cli_CommandName;
		myCli->cli_CommandName = MKBADDR(AllocVec(100, MEMF_PUBLIC | MEMF_CLEAR));

		origCommandFile = myCli->cli_CommandFile;
		myCli->cli_CommandFile = MKBADDR("");
		}

	SetProgramName(WbProcName);

	MyTask = FindTask(NULL);

	// save original task name
	OrigTaskName = MyTask->tc_Node.ln_Name;

	// set new task name
	MyTask->tc_Node.ln_Name = (char *) WbProcName;

#if defined(__GNUC__) && !defined(__MORPHOS__) && !defined(__amigaos4__) && !defined(__AROS__)
	__exitstdio();
#endif

	LINE_TRACE;

	ScalosMain(argArray);

	LINE_TRACE;

	if (myCli)
		{
		// restore original cli_CommandName
		FreeVec(BADDR(myCli->cli_CommandName));
		myCli->cli_CommandName = origCommandName;

		// restore original cli_CommandFile
		myCli->cli_CommandFile = origCommandFile;
		}

	LINE_TRACE;

	if (argsPtr)
		FreeArgs(argsPtr);

	CloseLibrary((struct Library *) DOSBase);

	LINE_TRACE;

	// Restore original task name
	MyTask->tc_Node.ln_Name = (char *) OrigTaskName;

	return 0;
///
}


static void ScalosMain(LONG *ArgArray)
{
///
	BOOL ArgsDisableSplash = FALSE;
	BOOL ArgsForcePalette = FALSE;
	Class *iconImageClass = NULL;

	d1(KPrintF("%s/%s/%ld: START ArgArray=%08lx\n", __FILE__, __FUNC__, __LINE__, ArgArray));
	TRACE_AMITHLON(__LINE__);

	do	{
		ULONG rc;
		ULONG UnAllocatedPens;
		struct PatternNode *patNode;
		struct Node *libNode;
		struct ScalosSemaphoreList *sspl;
		struct ScalosBase *NewScalosBase;

		NewList(&globalRedoList);
		NewList(&globalUndoList);
		NewList(&globalCopyClipBoard);
		NewList(&WBStartList);
		NewList(&globalCloseWBHookList);
		NewList(&ControlBarGadgetListNormal);
		NewList(&ControlBarGadgetListBrowser);

		TRACE_AMITHLON(__LINE__);

		// Init. all global semaphores
		for (sspl = GlobalSemaphoreList; sspl->sspl_Semaphore; sspl++)
		{
			ScalosInitSemaphore(sspl->sspl_Semaphore);
		}

		LINE_TRACE;

		if (!MemoryInit())
			break;

		TRACE_AMITHLON(__LINE__);

		MainWindowTask = ScalosAlloc(sizeof(struct MainTask));

		d1(kprintf("%s/%s/%ld: MainWindowTask=%08lx\n", __FILE__, __FUNC__, __LINE__, MainWindowTask));
		if (NULL == MainWindowTask)
			break;

		TRACE_AMITHLON(__LINE__);

		memset(MainWindowTask, 0, sizeof(struct MainTask));

		MainTask = (struct Process *) FindTask(NULL);

		d1(kprintf("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		ScalosInitSemaphore(&MainWindowTask->mwt.iwt_UpdateSemaphore);
		ScalosInitSemaphore(&MainWindowTask->mwt.iwt_ScanDirSemaphore);
		ScalosInitSemaphore(&MainWindowTask->mwt.iwt_ChildProcessSemaphore);
		ScalosInitSemaphore(&MainWindowTask->mwt.iwt_ThumbnailIconSemaphore);
		ScalosInitSemaphore(&MainWindowTask->mwt.iwt_ThumbGenerateSemaphore);

		ScalosInitSemaphore(&MainWindowTask->mwt_MainNotifyListSema);

		NewList(&MainWindowTask->mwt.iwt_ThumbnailIconList);

		MainWindowTask->mwt.iwt_WindowTask.wt_IconSemaphore = ScalosCreateSemaphore();
		if (NULL == MainWindowTask->mwt.iwt_WindowTask.wt_IconSemaphore)
			break;

		LINE_TRACE;

		if (!OpenLibraries())
			break;

		libNode = FindName(&SysBase->LibList, SCALOSNAME);

		d1(kprintf("%s/%s/%ld: another Scalos instance .. libNode=%08lx\n", __FILE__, __FUNC__, __LINE__, libNode));
		if (libNode)
			{
			// abort if Scalos already running
			(void) UseRequest(NULL, MSGID_REQUEST_SCALOS_ALREADY_RUNNING, MSGID_OKNAME, NULL);
			break;
			}

		LINE_TRACE;

		ScalosMainTask = (struct Process *) FindTask(NULL);

		TRACE_AMITHLON(__LINE__);
		FBlitPort = FindPort("FBlit");

		d1(kprintf("%s/%s/%ld: FBlitPort=%08lx\n", __FILE__, __FUNC__, __LINE__, FBlitPort));

		// Create an assignment "Scalos:" if there isn't one.
		if (!ExistsAssign("Scalos"))
			AssignPath("Scalos", "PROGDIR:");

		LINE_TRACE;

		TRACE_AMITHLON(__LINE__);
		if (!OpenConsoleDevice())
			break;

		LINE_TRACE;

		wbPort = CreateMsgPort();
		if (NULL == wbPort)
			break;
		iInfos.xii_iinfos.ii_MainMsgPort = CreateMsgPort();
		if (NULL == iInfos.xii_iinfos.ii_MainMsgPort)
			break;

		iInfos.xii_iinfos.ii_MainMsgPort->mp_Node.ln_Name = (STRPTR) "SCALOS";
		iInfos.xii_iinfos.ii_MainMsgPort->mp_Node.ln_Pri = 0;
		AddPort(iInfos.xii_iinfos.ii_MainMsgPort);

		LINE_TRACE;

		MainWindowTask->iorequest = (T_TIMEREQUEST *)CreateIORequest(iInfos.xii_iinfos.ii_MainMsgPort, sizeof(T_TIMEREQUEST));
		if (NULL == MainWindowTask->iorequest)
			break;

		LINE_TRACE;

		OpenDevice("timer.device", UNIT_VBLANK, &MainWindowTask->iorequest->tr_node, 0);
		TimerBase = (T_TIMERBASE) MainWindowTask->iorequest->tr_node.io_Device;
		if (NULL == TimerBase)
			break;
#ifdef __amigaos4__
		ITimer = (struct TimerIFace *)GetInterface((struct Library *)TimerBase, "main", 1, NULL);
		if (NULL == ITimer)
			break;
#endif

		RandomSeed = IntuitionBase->Micros / 33333;

		LINE_TRACE;

		if (ArgArray)
			{
			if (ArgArray[0])
				MainWindowTask->emulation = TRUE;
			if (ArgArray[1])
				ArgsDisableSplash = TRUE;
			if (ArgArray[2])
				ArgsForcePalette = TRUE;
			}

		LINE_TRACE;

		iconImageClass = initIconImageClass();

		TRACE_AMITHLON(__LINE__);

		AppMenu_Init();

		LINE_TRACE;
	
		NewScalosBase = sca_MakeLibrary();

		TRACE_AMITHLON(__LINE__);

		if (NULL == NewScalosBase)
			{
			UseRequest(NULL, MSGID_SCALIBERRORNAME, MSGID_OKNAME, NULL);
			break;
			}
		d1(kprintf("%s/%s/%ld: NewScalosBase=%08lx\n", __FILE__, __FUNC__, __LINE__, NewScalosBase));
		TRACE_AMITHLON(__LINE__);

		AddLibrary(&NewScalosBase->scb_LibNode);

		LINE_TRACE;

		d1(KPrintF("%s/%s/%ld: ln_Name=<%s>  version=%ld\n", \
			__FILE__, __FUNC__, __LINE__, NewScalosBase->scb_LibNode.lib_Node.ln_Name, NewScalosBase->scb_LibNode.lib_Version));

		ScalosBase = (struct ScalosBase *) OpenLibrary(NewScalosBase->scb_LibNode.lib_Node.ln_Name, NewScalosBase->scb_LibNode.lib_Version);
		d1(KPrintF("%s/%s/%ld: ScalosBase=%08lx version=%ld\n", __FILE__, __FUNC__,
			__LINE__, ScalosBase, ScalosBase->scb_LibNode.lib_Version));
		if (NULL == ScalosBase)
			break;	// Should not happen...

		LINE_TRACE;

		InitScalosPrefs();
		ReadScalosPrefs();

		if (!InitDataTypesImage())
			break;

		LINE_TRACE;

		FontUtilInit();

		LINE_TRACE;

		initRootClass();

		TRACE_AMITHLON(__LINE__);

		AddInternalClasses();

		LINE_TRACE;

		InitLocale();
#if 0
		d1(KPrintF("%s/%s/%ld: Delay START\n", __FILE__, __FUNC__, __LINE__));
		Delay(3 * 50);
		d1(KPrintF("%s/%s/%ld: Delay END\n", __FILE__, __FUNC__, __LINE__));
#endif

		LINE_TRACE;

		DtImageClass = initDtImageClass();
		TextIconClass = initTextIconClass();
		TextIconHighlightClass = initTextIconHighlightClass();
		IconifyImageClass = initIconifyClass();

		LINE_TRACE;

		TRACE_AMITHLON(__LINE__);

		FileTypeInit();

		ReadWBConfig();
		ReadPalettePrefs();
//		ReadScalosPrefs();
		ReadFontPrefs();
		ShortcutReadPrefs();

		LINE_TRACE;

		if (!InitInputHandler())
			break;

		LINE_TRACE;

		if (!CurrentPrefs.pref_FullBenchFlag)
			CurrentPrefs.pref_PopTitleFlag = FALSE;

		InitCx();

		LINE_TRACE;

		SetupTheme();

		InitSplash(iInfos.xii_iinfos.ii_MainMsgPort);

		LINE_TRACE;

		SetAllPatches(MainWindowTask);

		LINE_TRACE;

		InitDiskPlugins();

		TRACE_AMITHLON(__LINE__);

		HelpGroupID = GetUniqueID();

		TRACE_AMITHLON(__LINE__);

		LastScreenTitleBuffer = ScalosAlloc(CurrentPrefs.pref_ScreenTitleSize);
		d1(KPrintF("%s/%s/%ld: LastScreenTitleBuffer=%08lx\n", __FILE__, __FUNC__, __LINE__, LastScreenTitleBuffer));

		if (NULL == LastScreenTitleBuffer)
			break;

		ScreenTitleObject = SCA_NewScalosObjectTags("Title.sca",
			SCCA_Title_Format, (ULONG) CurrentPrefs.pref_ScreenTitle,
			TAG_END);

		d1(kprintf("%s/%s/%ld: ScreenTitleObject=%08lx\n", __FILE__, __FUNC__, __LINE__, ScreenTitleObject));
		if (NULL == ScreenTitleObject)
			break;

		TRACE_AMITHLON(__LINE__);

		SetupICandy();

		if (ArgsDisableSplash || CurrentPrefs.pref_iCandy < 2)
			CurrentPrefs.pref_EnableSplash = FALSE;	      // no splash window if disabled via CLI argument

		LINE_TRACE;

		ScreenTitleBuffer = (STRPTR) DoMethod(ScreenTitleObject, SCCM_Title_Generate);
		d1(KPrintF("%s/%s/%ld: ScreenTitleBuffer=%08lx\n", __FILE__, __FUNC__, __LINE__, ScreenTitleBuffer));
		if (NULL == ScreenTitleBuffer)
			break;

		SCA_ScreenTitleMsg("Copyright © 2000" CURRENTYEAR " The Scalos Team - All Rights Reserved", NULL);

		LINE_TRACE;

		if (MainWindowTask->emulation)
			{
#if !defined(__MORPHOS__)
			((char *) WorkbenchBase)[45] |= 0x02;	// set flag "Wb opened"
#endif /* __MORPHOS__ */

			iInfos.xii_iinfos.ii_Screen = LockPubScreen("Workbench");
			d1(KPrintF("%s/%s/%ld: ii_Screen=%08lx\n", __FILE__, __FUNC__, __LINE__, iInfos.xii_iinfos.ii_Screen));
			if (NULL == iInfos.xii_iinfos.ii_Screen)
				break;

			// Adjust colors for existing Workbench screen
			if (ArgsForcePalette && PalettePrefs.pal_ScreenColorList)
				{
				LoadRGB32(&iInfos.xii_iinfos.ii_Screen->ViewPort, PalettePrefs.pal_ScreenColorList);
				SetRast(&iInfos.xii_iinfos.ii_Screen->RastPort, 0);
				}

			if (IconBase->lib_Version >= 44)
				{
				IconControl(NULL,
					ICONCTRLA_SetGlobalScreen, (ULONG) iInfos.xii_iinfos.ii_Screen,
					TAG_END);
				}
			}
		else
			{
			iInfos.xii_iinfos.ii_Screen = OpenScreenTags(NULL,
				CurrentPrefs.pref_FullBenchFlag ? SA_ShowTitle : TAG_IGNORE, FALSE,
//				  SA_Pens, (ULONG) PalettePrefs.pal_driPens,
				SA_SharePens, TRUE,
				SA_LikeWorkbench, TRUE,
//				  PalettePrefs.pal_ScreenColorList ? SA_Colors32 : TAG_IGNORE, (ULONG) PalettePrefs.pal_ScreenColorList,
				SA_PubName, (ULONG) "Scalos",
				SA_Type, PUBLICSCREEN,
				SA_Title, (ULONG) "Scalos Screen",
				TAG_END);

			if (NULL == iInfos.xii_iinfos.ii_Screen)
				break;
			}

#if defined(__MORPHOS__)
#if defined(SA_OpacitySupport)
		if (IntuitionBase->LibNode.lib_Version >= 51)
			{
			LONG attr;

			GetAttr(SA_OpacitySupport, iInfos.xii_iinfos.ii_Screen, &attr);
			d1(KPrintF("%s/%s/%ld: SA_OpacitySupport=%ld\n", __FILE__, __FUNC__, __LINE__, attr));
			iInfos.xii_Layers3D = attr > SAOS_OpacitySupport_OnOff;
			}
		else
#endif //defined(SA_OpacitySupport)
			{
			iInfos.xii_Layers3D = NULL != FindTask("« LayerInfoTask »");
			}
#endif //defined(__MORPHOS__)

		d1(KPrintF("%s/%s/%ld: ii_Screen=%08lx\n", __FILE__, __FUNC__, __LINE__, iInfos.xii_iinfos.ii_Screen));

		iInfos.xii_iinfos.ii_DrawInfo = GetScreenDrawInfo(iInfos.xii_iinfos.ii_Screen);
		UnAllocatedPens = LockScalosPens();

		d1(kprintf("%s/%s/%ld: DrawInfo=%08lx  dri_Pens=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, iInfos.xii_iinfos.ii_DrawInfo, iInfos.xii_iinfos.ii_DrawInfo->dri_Pens));

		LINE_TRACE;

		if (CurrentPrefs.pref_UseScreenTTFont)
			Scalos_OpenTTFont(CurrentPrefs.pref_ScreenTTFontDescriptor, &ScreenTTFont);

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		LINE_TRACE;

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		SplashAddUser();
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));
		SplashDisplayProgress(GetLocString(MSGID_PROGRESS_SETUPSCREEN), 0);
		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		TRACE_AMITHLON(__LINE__);

		InitDefIcons();

		TRACE_AMITHLON(__LINE__);

		// TimerBase must be initialized now!
		if (!ThumbnailCacheInit())
			break;

		LINE_TRACE;

		OpenIconWindowFont();
		d1(kprintf("%s/%s/%ld: IconWindowTTFont=%08lx\n", __FILE__, __FUNC__, __LINE__, IconWindowTTFont));

		OpenTextWindowFont();
		d1(kprintf("%s/%s/%ld: TextWindowTTFont=%08lx\n", __FILE__, __FUNC__, __LINE__, TextWindowTTFont));

		TRACE_AMITHLON(__LINE__);

		d1(kprintf("%s/%s/%ld: iInfos.xii_iinfos.ii_Screen=%08lx\n", __FILE__, __FUNC__, __LINE__, iInfos.xii_iinfos.ii_Screen));
		if (NULL == iInfos.xii_iinfos.ii_Screen)
			break;

		TRACE_AMITHLON(__LINE__);

		d1({ short n;\
			for (n=0; n < NUMDRIPENS; n++)\
			{\
			kprintf("%s/%s/%ld: dri_Pen[%ld]=%ld \n", \
				__FILE__, __FUNC__, __LINE__, n, (LONG) iInfos.xii_iinfos.ii_DrawInfo->dri_Pens[n]);\
			} });

		if (GuiGFXBase)
			{
			PenShareMap = CreatePenShareMapA(NULL);
			}

		SplashDisplayProgress(GetLocString(MSGID_PROGRESS_READINGPREFS), 0);

		LINE_TRACE;

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		ReadPatternPrefs();

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		rc = ReadMenuPrefs();
		if (RETURN_OK != rc)
			{
			char ErrorText[100];

			(void) Fault(rc, (STRPTR) "", ErrorText, sizeof(ErrorText));

			(void) UseRequestArgs(NULL, MSGID_READMENUPREFS_FAIL, MSGID_OKNAME,
				1, ErrorText);

			}

		d1(KPrintF("%s/%s/%ld: \n", __FILE__, __FUNC__, __LINE__));

		SplashDisplayProgress(GetLocString(MSGID_PROGRESS_SETUPSCRPATTERN), 0);

		LINE_TRACE;

		patNode = GetPatternNode(PatternPrefs.patt_DefScreenPatternNr, NULL);
		if (patNode)
			{
			MainWindowTask->mwt.iwt_WindowTask.wt_PatternInfo.ptinf_width = iInfos.xii_iinfos.ii_Screen->Width;
			MainWindowTask->mwt.iwt_WindowTask.wt_PatternInfo.ptinf_height = iInfos.xii_iinfos.ii_Screen->Height;
			MainWindowTask->mwt.iwt_WindowTask.wt_PatternInfo.ptinf_hook.h_Data = MainWindowTask;

			if (SetBackFill(&MainWindowTask->mwt, patNode, 
				&MainWindowTask->mwt.iwt_WindowTask.wt_PatternInfo, 1, iInfos.xii_iinfos.ii_Screen))
				{
				MainWindowTask->screenbackfill = TRUE;
				SetScreenBackfillHook(&MainWindowTask->mwt.iwt_WindowTask.wt_PatternInfo.ptinf_hook);
				}
			}

		LINE_TRACE;

		SplashDisplayProgress(GetLocString(MSGID_PROGRESS_INITMENU), 0);

		iInfos.xii_iinfos.ii_visualinfo = GetVisualInfoA(iInfos.xii_iinfos.ii_Screen, NULL);

		CreateScalosMenu();

		SplashDisplayProgress(GetLocString(MSGID_PROGRESS_INITTIMER), 0);

		LINE_TRACE;

		StartMainTaskTimer(MainWindowTask);

		LINE_TRACE;

		if (!InitMainWindow())
			break;

		LINE_TRACE;

		if (MainWindowTask->emulation && !MainWindowTask->restartflag)
			{
			SplashDisplayProgress(GetLocString(MSGID_PROGRESS_WBSTARTUP), 0);
			SplashAddUser();

			RunProcess(&MainWindowTask->mwt.iwt_WindowTask, 
				(RUNPROCFUNC) WBStartup, 0, NULL, iInfos.xii_iinfos.ii_MainMsgPort);
			}
		else
			{
			WBStartupFinished();
			}

		LINE_TRACE;

		PubScreenStatus(iInfos.xii_iinfos.ii_Screen, 0);
		SetDefaultPubScreen(MainWindowTask->emulation ? "Workbench" : "Scalos");

		SplashDisplayProgress(GetLocString(MSGID_PROGRESS_INITNOTIFY), 0);

		LINE_TRACE;

		InitPrefsNotification(MainWindowTask);

		SplashRemoveUser();

		LINE_TRACE;

		if (UnAllocatedPens)
			{
			// Inform user that not all requested pens could be allocated.
			UseRequestArgs(NULL, MSGID_ALLOCPENSREQ_CONTENTS, MSGID_ALLOCPENSREQ_GADGETSNAME,
				1,
				UnAllocatedPens);
			}

		LINE_TRACE;

		do	{
			MainWait(MainWindowTask);	// run until Scalos finished

			LINE_TRACE;
			
			MainWindowTask->restartflag = TRUE;
			} while (!RemoveAllPatches(MainWindowTask));

		LINE_TRACE;

		PubScreenStatus(iInfos.xii_iinfos.ii_Screen, PSNF_PRIVATE);

		LINE_TRACE;
		} while (0);

	LINE_TRACE;

	if (MainWindowTask && ScalosBase)
		{
		struct ScaWindowStruct *ws, *wsNext;

		SCA_LockWindowList(SCA_LockWindowList_Exclusiv);

		for (ws=winlist.wl_WindowStruct; ws; ws=wsNext)
			{
			struct SM_CloseWindow *msg;

			msg = (struct SM_CloseWindow *) SCA_AllocMessage(MTYP_CloseWindow, 0);
			if (msg)
				{
				msg->ScalosMessage.sm_Message.mn_ReplyPort = iInfos.xii_iinfos.ii_MainMsgPort;
				PutMsg(ws->ws_MessagePort, &msg->ScalosMessage.sm_Message);

				wsNext = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ;

				SCA_UnLockWindowList();
				SCA_LockWindowList(SCA_LockWindowList_Shared);

				WaitReply(iInfos.xii_iinfos.ii_MainMsgPort, &MainWindowTask->mwt, MTYP_CloseWindow);
				}
			else
				wsNext = (struct ScaWindowStruct *) ws->ws_Node.mln_Succ;
			}

		SCA_UnLockWindowList();
		}

	LINE_TRACE;

	CleanupDefIcons();

	LINE_TRACE;

	if (MainWindowTask && MainWindowTask->mwt.iwt_WindowTask.mt_MainObject)
		{
		d1(KPrintF("%s/%s/%ld: mt_MainObject=%08lx\n", \
			__FILE__, __FUNC__, __LINE__, MainWindowTask->mwt.iwt_WindowTask.mt_MainObject));
		DoMethod(MainWindowTask->mwt.iwt_WindowTask.mt_MainObject, SCCM_IconWin_CleanUp);
		DoMethod(MainWindowTask->mwt.iwt_WindowTask.mt_MainObject, SCCM_ClearClipboard);
		}

	LINE_TRACE;

	// wait for QuitSemaphore until every Scalos windowtask has finished
	ScalosObtainSemaphore(&QuitSemaphore);
	ScalosReleaseSemaphore(&QuitSemaphore);

	LINE_TRACE;

	if (MainWindowTask && ScalosBase)
		QuitPrefsNotification(MainWindowTask);

	LINE_TRACE;

	if (MainMenu)
		{
		FreeMenus(MainMenu);
		MainMenu = NULL;
		}
	if (iInfos.xii_iinfos.ii_visualinfo)
		{
		FreeVisualInfo(iInfos.xii_iinfos.ii_visualinfo);
		iInfos.xii_iinfos.ii_visualinfo = NULL;
		}

	LINE_TRACE;

	if (MainWindowTask && iInfos.xii_iinfos.ii_MainMsgPort && ScalosBase)
		PatternsOff(MainWindowTask, iInfos.xii_iinfos.ii_MainMsgPort);

	ShortcutFreePrefs();
	FreeFontPrefs();
	FreePatternPrefs();
	FreeMenuPrefs();
	FreePalettePrefs();
	FreeScalosPrefs();

	FileTypeCleanup();

	if (GuiGFXBase)
		{
		DeletePenShareMap(PenShareMap);
		PenShareMap = NULL;
		}

	LINE_TRACE;

	UndoCleanup();

	LINE_TRACE;

	CleanupThumbnailCache();

	LINE_TRACE;

	CleanupInputHandler();

	LINE_TRACE;

	CleanupCx();

	LINE_TRACE;

	if (iInfos.xii_iinfos.ii_Screen)
		{
		if (MainWindowTask && MainWindowTask->emulation)
			{
			UnlockPubScreen(NULL, iInfos.xii_iinfos.ii_Screen);
			}
		else
			{
			if (!CloseScreen(iInfos.xii_iinfos.ii_Screen))
				{
				Delay(20);

				while (!CloseScreen(iInfos.xii_iinfos.ii_Screen))
					{
					UseRequest(NULL, MSGID_CANNOT_QUIT_FOREIGNWINDOWS, MSGID_GADGETSNAME, NULL);
					}
				}
			}

		iInfos.xii_iinfos.ii_Screen = NULL;
		}

	LINE_TRACE;

	if (MainWindowTask && MainWindowTask->iorequest)
		{
		AbortIO(&MainWindowTask->iorequest->tr_node);
		WaitIO(&MainWindowTask->iorequest->tr_node);
		}

	if (MainWindowTask && MainWindowTask->iorequest)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)ITimer);
#endif
		CloseDevice(&MainWindowTask->iorequest->tr_node);
		DeleteIORequest(&MainWindowTask->iorequest->tr_node);

		MainWindowTask->iorequest = NULL;
		}

	if (iInfos.xii_iinfos.ii_MainMsgPort)
		{
		Forbid();
		RemPort(iInfos.xii_iinfos.ii_MainMsgPort);
		DeleteMsgPort(iInfos.xii_iinfos.ii_MainMsgPort);
		Permit();
		}

	if (wbPort)
		{
		DeleteMsgPort(wbPort);
		wbPort = NULL;
		}

	LINE_TRACE;

	if (TextIconHighlightClass)
		{
		FreeClass(TextIconHighlightClass);
		TextIconHighlightClass = NULL;
		}
	if (TextIconClass)
		{
		FreeClass(TextIconClass);
		TextIconClass = NULL;
		}
	if (IconifyImageClass)
		{
		FreeClass(IconifyImageClass);
		IconifyImageClass = NULL;
		}
	if (DtImageClass)
		{
		FreeClass(DtImageClass);
		DtImageClass = NULL;
		}

	Scalos_CloseFont(&IconWindowFont, &IconWindowTTFont);
	Scalos_CloseFont(&TextWindowFont, &TextWindowTTFont);
	Scalos_CloseFont(NULL, &ScreenTTFont);

	LINE_TRACE;

	CleanupLocale();

	if (ScreenTitleObject)
		{
		SCA_DisposeScalosObject(ScreenTitleObject);
		ScreenTitleObject = NULL;
		}

	LINE_TRACE;

	if (LastScreenTitleBuffer)
		{
		ScalosFree(LastScreenTitleBuffer);
		LastScreenTitleBuffer = NULL;
		}

	// free internal and plugin classes
	CleanupScalosClasses();

	LINE_TRACE;

	freeIconImageClass(iconImageClass);

	LINE_TRACE;

	if (MainWindowTask)
		{
		if (MainWindowTask->mwt.iwt_WindowTask.wt_IconList)
			FreeIconList(&MainWindowTask->mwt, &MainWindowTask->mwt.iwt_WindowTask.wt_IconList);
		if (MainWindowTask->mwt.iwt_WindowTask.wt_LateIconList)
			FreeIconList(&MainWindowTask->mwt, &MainWindowTask->mwt.iwt_WindowTask.wt_LateIconList);

		if (MainWindowTask->mwt.iwt_WindowTask.wt_IconSemaphore)
			{
			ScalosDeleteSemaphore(MainWindowTask->mwt.iwt_WindowTask.wt_IconSemaphore);
			MainWindowTask->mwt.iwt_WindowTask.wt_IconSemaphore = NULL;
			}

		ScalosFree(MainWindowTask);
		MainWindowTask = NULL;
		}

	LINE_TRACE;

	if (ScalosBase)
		{
		d1(kprintf("%s/%s/%ld: ScalosBase=%08lx  OpenCount=%ld\n", __FILE__, __FUNC__, __LINE__, ScalosBase, ScalosBase->scb_LibNode.lib_OpenCnt));
		CloseLibrary(&ScalosBase->scb_LibNode);

		d1(kprintf("%s/%s/%ld: ScalosBase=%08lx  OpenCount=%ld\n", __FILE__, __FUNC__, __LINE__, ScalosBase, ScalosBase->scb_LibNode.lib_OpenCnt));
		RemLibrary(&ScalosBase->scb_LibNode);
		ScalosBase = NULL;
		}

	LINE_TRACE;

	CleanupDataTypesImage();

	LINE_TRACE;

	CloseConsoleDevice();

	LINE_TRACE;

	CloseLibraries();

	MemoryCleanup();

	d1(KPrintF("%s/%s/%ld: Finished.\n", __FILE__, __FUNC__, __LINE__));
///
}


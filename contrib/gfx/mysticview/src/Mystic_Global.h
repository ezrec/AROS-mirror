#ifndef MYSTIC_GLOBAL_H
#define	MYSTIC_GLOBAL_H 1

/*********************************************************************
----------------------------------------------------------------------

	MysticView 
	global data definitions

------------------------------------------------------ tabsize = 4 ---
*********************************************************************/


#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/semaphores.h>
#include <exec/memory.h>

#include <guigfx/guigfx.h>
#include <graphics/view.h>
#include <libraries/gadtools.h>

#include <proto/exec.h>


/*--------------------------------------------------------------------

	constants

--------------------------------------------------------------------*/

#if defined(__MORPHOS__)
	#define FAR
	#define ASM
	#define REG(x)
	#define SAVEDS
	#define ALIGNED __attribute__((aligned(4)))
	#define STDARGS __attribute__((varargs68k))
#elif defined(__AROS__)
	#define FAR
	#define ASM
	#define REG(x)
	#define SAVEDS
	#define ALIGNED
	#define STDARGS
#elif defined(__STORM__)
	#define FAR		__far
	#define	ASM
	#define REG(x)	register __ ## x
	#define SAVEDS
	#define ALIGNED
	#define STDARGS
#else
	#define FAR		__far
	#define ASM		__asm
	#define REG(x)	register __ ## x
	#define SAVEDS	__saveds
	#define ALIGNED	__aligned
	#define STDARGS	__stdargs
#endif

#define	POOLPUDSIZE			32768
#define	POOLTHRESSIZE		1024


#define	USEPOOLS				1
//#define TRACKMEMORY				1



#define	PROGVERSION				" 1.07"

#define PROGNAME				"MysticView"  

#define	MAXPRIORITY				4

#define MAINSTACK				20000
#define LOADSTACK				20000
#define SCANSTACK				9000
#define PICHANDLESTACK			1000
#define FILEHANDLESTACK			4096
#define TRIGGERSTACK			600
#define	ABOUTSTACK				1000





#define	LOGOWIDTH				160
#define	LOGOHEIGHT				128
#define	LOGONUMCOLORS			256

#define	BUTTONSWIDTH			200
#define	BUTTONSHEIGHT			72
#define	BUTTONSNUMCOLORS		8

#define	ANIMWIDTH				20
#define	ANIMHEIGHT				14
#define	ANIMFRAMES				20
#define	ANIMNUMCOLORS			32


#define MYSTIC_VERSION			5
#define GUIGFX_VERSION			19
#define RENDER_VERSION			30
#define NEURALNET_VERSION		5

#define	MINSCREENWIDTH			320
#define	MINSCREENHEIGHT			200
#define	MINSCREENDEPTH			2


#define	MAXFILENAMELEN			110
#define	MAXPATHNAMELEN			2000
#define	MAXFULLNAMELEN			(MAXFILENAMELEN + MAXPATHNAMELEN)


#define MINDELAY				1

#define	BORDERREFRESHDELAY		15

#define	THUMBWIDTH				100
#define	THUMBHEIGHT				100


#define EVT_HOTKEY 				1L			// for hotkey handler


/*--------------------------------------------------------------------

	menu ids

--------------------------------------------------------------------*/

enum MENUITEMS
{
	MITEM_DUMMY = 1,
	MITEM_OPEN, MITEM_OPENLIST, MITEM_ABOUT, MITEM_QUIT, MITEM_NEXT, MITEM_PREV, MITEM_SLIDE,
	MITEM_DITHERON, MITEM_DITHEROFF, MITEM_DITHERAUTO,
	MITEM_FSDITHER, MITEM_EDDDITHER,
	MITEM_OPAQUE, MITEM_KEEPIMAGE, 
	MITEM_KEEPASPECT1, MITEM_KEEPASPECT2, MITEM_ONEPIXEL, MITEM_IGNOREASPECT, MITEM_FIT,
	MITEM_RENDERHIGH, MITEM_STATIC, MITEM_BACKDROP, MITEM_SIMPLEREFRESH, MITEM_SMARTREFRESH,
	MITEM_CUSTOMSCREEN, MITEM_GUI, MITEM_ICON, MITEM_IMAGE, MITEM_EXACT,
	MITEM_ZOOMIN, MITEM_ZOOMOUT, MITEM_RESETZOOM, MITEM_RESETPOS, MITEM_ZOOMFRAME,
	MITEM_SMALLWINDOW, MITEM_HALFWINDOW, MITEM_LARGEWINDOW, MITEM_VISIBLEWINDOW,
	MITEM_FULLWINDOW, MITEM_FIXEDWINDOWSIZE, MITEM_SNAPWINDOW, MITEM_REQUESTFILE,
	MITEM_CENTERWINDOW, MITEM_MOUSEWINDOW, MITEM_FIXEDWINDOWPOS, 
	MITEM_SCREENMODE,
	MITEM_GRID, MITEM_NOREFRESH, MITEM_ZIPWINDOW,
	MITEM_RESETSETTINGS, MITEM_LASTSETTINGS, MITEM_RESTORESETTINGS, MITEM_AUTOSAVESETTINGS, MITEM_SAVESETTINGS,
	MITEM_COPYFILEREQ, MITEM_MOVEFILEREQ, MITEM_COPYTOPATH, MITEM_MOVETOPATH, MITEM_SETPATH, MITEM_DELETE,
	MITEM_PICINFO, MITEM_PRELOAD, MITEM_CREATETHUMB,
	MITEM_FLIPX, MITEM_FLIPY, MITEM_SLIDEDELAY_0, MITEM_SLIDEDELAY_04,
	MITEM_SLIDEDELAY_1, MITEM_SLIDEDELAY_2, MITEM_SLIDEDELAY_5, MITEM_SLIDEDELAY_10,
	MITEM_SLIDEDELAY_30, MITEM_SLIDEDELAY_60, MITEM_SLIDEDELAY_120, MITEM_SLIDEDELAY_300,
	MITEM_SLIDEDELAY_600, MITEM_SLIDEDELAY_CUSTOM,
	MITEM_SAVEPRESET, MITEM_LOADPRESET,
	MITEM_LOOP, MITEM_AUTOQUIT, MITEM_STARTPIC,
	MITEM_SORT_NONE, MITEM_SORT_ALPHA_FILE, MITEM_SORT_RANDOM,
	MITEM_USEPRESET, MITEM_MARKCENTER, MITEM_HIDE, MITEM_AUTOHIDE,
	MITEM_ROTATELEFT, MITEM_ROTATERIGHT,
	MITEM_PUBLICSCREEN, MITEM_DEFAULTSCREEN, MITEM_SELECTPUBSCREEN, MITEM_RESETROT,
	MITEM_HIDEWITHPICTURE, MITEM_RESTART,
	MITEM_ICONHIDE, MITEM_ITEMHIDE, MITEM_INVISIBLEHIDE,
	MITEM_SAVEJPEG, MITEM_NEGATIVE, MITEM_COPYTO, MITEM_MOVETO, MITEM_RESETDISPLAYSETTINGS,
	MITEM_CREATETHUMBNAILS,
	MITEM_LMB_DRAG, MITEM_LMB_NEXT, MITEM_RENAME, MITEM_AUTODISPLAYMODE, MITEM_AUTOCROP,
	MITEM_APPEND,
	MITEM_SORT_FILESIZE, MITEM_SORT_DATE, MITEM_SORT_ALPHA_FULL, MITEM_SORT_REVERSE,
	MITEM_ASYNCSCANNING,
	MITEM_CACHE_OFF, MITEM_CACHE_SMALL, MITEM_CACHE_MEDIUM, MITEM_CACHE_LARGE, MITEM_CACHE_HUGE,
	MITEM_FILTERPICTURES, MITEM_HIDEONCLOSE, MITEM_CLEAR,
	MITEM_AUTOSTOP, MITEM_AUTOCLEAR, MITEM_SHOWARROWS, MITEM_HIDEONESCAPE,
	MITEM_SHOWPIP, MITEM_SHOWBUTTONS, MITEM_REMOVE, MITEM_SAVELIST,
	MITEM_APPENDLIST, MITEM_RESETALL, MITEM_APPENDICONS
};

#define 	MITEM_COPY1			(0x1000	+	0)
#define		MITEM_MOVE1			(0x1000 +	10)
#define		MITEM_PRESET1		(0x1000 +	20)
#define		MITEM_PUBSCREEN1	(0x1000 +	30)


#define CLEARMENUITEM(menu, a) ToggleMenuFlags(menu,a,NULL,CHECKED);
#define SETMENUITEM(menu, a) ToggleMenuFlags(menu,a,CHECKED+CHECKIT,NULL);


extern void ToggleMenuFlags(struct Menu *menu, ULONG id, UWORD set, UWORD clear);
extern struct MenuItem *FindMenuItem(struct Menu *menu, ULONG id);
extern ULONG GetMenuFlag(struct Menu *menu, ULONG id, int true, int false);
#define GetItemFlag(mi, t, f) (((mi)->Flags & CHECKED) ? t : f)


/*--------------------------------------------------------------------

	types, flags, tags

--------------------------------------------------------------------*/

enum WINOPENMODES
	{WOPENMODE_NONE, WOPENMODE_CENTER, WOPENMODE_MOUSE};

enum WINSIZEMODES
	{WSIZEMODE_NONE, WSIZEMODE_HALF, WSIZEMODE_VISIBLE, WSIZEMODE_66PERCENT, 
	WSIZEMODE_FULL, WSIZEMODE_33PERCENT};

enum SCROPENMODES
	{SCROPENMODE_NONE, SCROPENMODE_CUSTOM, SCROPENMODE_PUBLIC, SCROPENMODE_FRONTMOST};

enum CONFIRMLEVELS
	{CONFIRM_NEVER, CONFIRM_ALWAYS};

enum ICONIFYMODES
	{ICONIFY_APPICON, ICONIFY_APPITEM, ICONIFY_INVISIBLE};

enum MOUSEACTIONS
	{MOUSEACTION_NONE, MOUSEACTION_DRAG, MOUSEACTION_NEXT};

enum PRIORITIES
	{PRIORITY_LOW=0, PRIORITY_NICE, PRIORITY_NORMAL};


/*--------------------------------------------------------------------

	command line

--------------------------------------------------------------------*/

#define	RDARGTEMPLATE 		\
	"FILENAME/M," 			\
	"DEFAULT/S," 			\
	"LISTFILE/K," 			\
	"PRESETFILE/K," 		\
	"APPEND/S," 			\
	"ANIMPIC/K,"			\
	"APPENDICONS/K,"		\
	"ASYNCSCANNING/K," 		\
	"AUTOCLEAR/K," 			\
	"AUTOCROP/K," 			\
	"AUTODISPLAYMODE/K," 	\
	"AUTOHIDE/K," 			\
	"AUTOQUIT/K," 			\
	"AUTOSAVESETTINGS/K," 	\
	"AUTOSTOP/K," 			\
	"BACKDROP/K," 			\
	"BGCOLOR/K," 			\
	"BUFFERPERCENT/N," 		\
	"BUTTONPIC/K,"			\
	"CONFIRMLEVEL/K," 		\
	"COPYPATH/K," 			\
	"CREATETHUMBNAILS/K," 	\
	"CX_POPKEY/K," 			\
	"CX_POPUP/K," 			\
	"CX_PRIORITY/N," 		\
	"DEFAULTTOOL/K,"		\
	"DEPTH/N," 				\
	"DISPLAYMODE/K," 		\
	"DITHERING/K," 			\
	"DITHERMODE/K," 		\
	"FILTERPICTURES/K," 	\
	"FONTSPEC/K," 			\
	"HAM/K," 				\
	"HIDEONCLOSE/K," 		\
	"HIDEONESCAPE/K," 		\
	"ICONIFY/K," 			\
	"LISTPATH/K," 			\
	"LMBACTION/K," 			\
	"LOOP/K," 				\
	"MARKCOLOR/K,"			\
	"MODEID/K," 			\
	"PICINFO/K," 			\
	"PICFORMAT/K," 			\
	"PRECISION/K," 			\
	"PRESETPATH/K," 		\
	"PREVIEWMODE/K," 		\
	"PUBSCREEN/K," 			\
	"RECURSE/K," 			\
	"REFRESHMODE/K," 		\
	"REJECTPATTERN/K," 		\
	"RENDERQUALITY/K," 		\
	"REQUESTFILE/K," 		\
	"RESETDISPLAY/K," 		\
	"REVERSE/K," 			\
	"ROTATESTEP/N," 		\
	"SAVEPATH/K," 			\
	"SCREENASPECTX/N," 		\
	"SCREENASPECTY/N," 		\
	"SCRHEIGHT/N," 			\
	"SCROPENMODE/K," 		\
	"SCRWIDTH/N," 			\
	"SHOWARROWS/K," 		\
	"SHOWBUTTONS/K," 		\
	"SHOWCURSOR/K," 		\
	"SHOWPIP/K," 			\
	"SIMPLESCANNING/K,"		\
	"SLIDEDELAY/N," 		\
	"SLIDESHOW/K," 			\
	"SMOOTHDISPLAY/K,"		\
	"SORTMODE/K," 			\
	"STARTPATH/K," 			\
	"STARTPIC/K," 			\
	"STATICPALETTE/K," 		\
	"TEXTCOLOR/K,"			\
	"THUMBCOLORS/N," 		\
	"THUMBSIZE/N," 			\
	"WINHEIGHT/N," 			\
	"WINLEFT/N," 			\
	"WINOPENMODE/K," 		\
	"WINSIZEMODE/K," 		\
	"WINTOP/N," 			\
	"WINWIDTH/N," 			\
	"ZOOMSTEP/K"
	
#define	RDNUMARGS		83


//	commandline-only arguments:

#define	ARG_DEFAULT		1
#define	ARG_LISTFILE	2
#define	ARG_PRESETFILE	3
#define ARG_APPEND		4




/*--------------------------------------------------------------------

	defaults

--------------------------------------------------------------------*/

#define	DEFAULT_ANIMPIC				NULL
#define	DEFAULT_AUTOCROP			FALSE
#define	DEFAULT_AUTODISPLAYMODE		FALSE
#define	DEFAULT_AUTOHIDE			FALSE
#define	DEFAULT_AUTOSAVESETTINGS	FALSE
#define	DEFAULT_BUFFERPERCENT		15
#define	DEFAULT_CONFIRMLEVEL		CONFIRM_ALWAYS
#define	DEFAULT_CREATETHUMBNAILS	FALSE
#define	DEFAULT_CX_POPUP			TRUE
#define	DEFAULT_CX_PRIORITY			0
#define	DEFAULT_DEFAULTTOOL			"mysticview"
#define	DEFAULT_DEPTH				0
#define	DEFAULT_DITHER				MVDITHERMODE_AUTO
#define	DEFAULT_HIDEWITHPICTURE		FALSE
#define	DEFAULT_HOTKEY				"control m"
#define	DEFAULT_ICONIFYMODE			ICONIFY_APPICON
#define	DEFAULT_LISTPATH			NULL
#define	DEFAULT_MAXHEIGHT			-1
#define	DEFAULT_MAXWIDTH			-1
#define	DEFAULT_MINHEIGHT			160
#define	DEFAULT_MINWIDTH			160
#define	DEFAULT_MODEID				INVALID_ID
#define	DEFAULT_PICINFO				TRUE
#define	DEFAULT_PICFORMAT			"%f - %w×%h×%d"
#define	DEFAULT_PRESETPATH			"sys:prefs/presets"
#define	DEFAULT_PRIORITY			PRIORITY_NORMAL
#define	DEFAULT_RECURSIVE			TRUE
#define	DEFAULT_REJECTPATTERN		"#?.info"
#define	DEFAULT_RESETDISPLAYSETTINGS	TRUE
#define	DEFAULT_ROTATESTEP			30
#define	DEFAULT_SCREENTITLE			PROGNAME
#define	DEFAULT_SCRHEIGHT			0
#define	DEFAULT_SCROPENMODE			SCROPENMODE_NONE
#define	DEFAULT_SCRWIDTH			0
#define	DEFAULT_SLIDESHOW			FALSE
#define	DEFAULT_STARTPIC			NULL
#define	DEFAULT_BUTTONPIC			NULL
#define	DEFAULT_STATICPALETTE		TRUE
#define	DEFAULT_THUMBNUMCOLORS		32
#define	DEFAULT_THUMBSIZE			50
#define	DEFAULT_WINHEIGHT			0
#define	DEFAULT_WINLEFT				-1
#define	DEFAULT_WINTITLE			PROGNAME PROGVERSION
#define	DEFAULT_WINTOP				-1
#define	DEFAULT_WINWIDTH			0
#define	DEFAULT_ZOOMSTEP			(0.25)
#define DEFAULT_ASYNCSCANNING		FALSE
#define DEFAULT_AUTOCLEAR			FALSE
#define DEFAULT_AUTOQUIT			FALSE
#define DEFAULT_AUTOSTOP			TRUE
#define DEFAULT_BGCOLOR				(0x103020)
#define DEFAULT_TEXTCOLOR			(0xeeffdd)
#define DEFAULT_MARKCOLOR			(0x8ce43c)
#define DEFAULT_BORDERLESS			FALSE
#define DEFAULT_COPYPATH			NULL
#define DEFAULT_DISPLAYMODE			MVDISPMODE_KEEPASPECT_MIN
#define DEFAULT_DITHERMODE			DITHERMODE_EDD
#define DEFAULT_FILTERPICTURES		FALSE
#define DEFAULT_HAMSCREEN			FALSE
#define DEFAULT_HIDEONCLOSE			FALSE
#define DEFAULT_HIDEONESCAPE		FALSE
#define DEFAULT_HSTYPE				HSTYPE_12BIT_TURBO
#define DEFAULT_KEEPIMAGE			FALSE
#define DEFAULT_LMBACTION			MOUSEACTION_DRAG
#define DEFAULT_LOOP				FALSE
#define DEFAULT_MARKCENTER			FALSE
#define DEFAULT_PRECISION			PRECISION_IMAGE
#define DEFAULT_PRELOAD				FALSE
#define DEFAULT_PREVIEWMODE			MVPREVMODE_NONE
#define DEFAULT_REFRESHMODE			WFLG_SMART_REFRESH
#define DEFAULT_REQUESTFILE			FALSE
#define DEFAULT_REVERSE				FALSE
#define DEFAULT_SCREENASPECTX		0
#define DEFAULT_SCREENASPECTY		0
#define DEFAULT_SHOWARROWS			TRUE
#define DEFAULT_SHOWPIP				FALSE
#define DEFAULT_SLIDEDELAY			50
#define DEFAULT_SORTMODE			SORTMODE_NONE
#define DEFAULT_WINOPENMODE			WOPENMODE_CENTER
#define DEFAULT_WINSIZEMODE			WSIZEMODE_66PERCENT
#define DEFAULT_SMOOTHDISPLAY		FALSE
#define DEFAULT_SIMPLESCANNING		FALSE
#define DEFAULT_SHOWBUTTONS			TRUE
#define DEFAULT_APPENDICONS			FALSE


#define DEFAULT_BROWSEMODE			FALSE

/*--------------------------------------------------------------------

	makros

--------------------------------------------------------------------*/

#ifdef __MORPHOS__
#include <emul/emulinterface.h>
#include <emul/emulregs.h>

#define FUNC_HOOK(ret, name, htype, hook, dtype, data, mtype, msg) \
	static ret name##_GATE(void); \
	static ret name##func(htype hook, dtype data, mtype msg); \
	struct EmulLibEntry name = { TRAP_LIB, 0, (void (*)(void))name##_GATE }; \
	static ret name##_GATE(void) { return (name##func((htype)REG_A0, (dtype)REG_A2, (mtype)REG_A1)); } \
	static ret name##func(htype hook, dtype data, mtype msg)
#endif

#define RNG(b,x,t) ((x) < (b) ? (b) : ((x) > (t) ? (t) : (x)))

#define inserttag(x,t,d) {(x)->ti_Tag=(t);((x)++)->ti_Data=(ULONG)(d);}

#define	LOCK(a) {if(a){ObtainSemaphore((struct SignalSemaphore *)(a));}}
#define	LOCKREAD(a) {if(a){ObtainSemaphoreShared((struct SignalSemaphore *)(a));}}
#define	UNLOCK(a) {if(a){ReleaseSemaphore((struct SignalSemaphore *)(a));}}

#define RED_RGB32(a) ((((a) & 0xff0000)<<8) + ((a) & 0xff0000) + (((a) & 0xff0000)>>8) + (((a) & 0xff0000)>>16))
#define GREEN_RGB32(a) ((((a) & 0x00ff00)<<16) + (((a) & 0x00ff00)<<8) + ((a) & 0x00ff00) + (((a) & 0x00ff00)>>8))
#define BLUE_RGB32(a) ((((a) & 0x0000ff)<<24) + (((a) & 0x0000ff)<<16) + (((a) & 0x0000ff)<<8) + ((a) & 0x0000ff))


/*--------------------------------------------------------------------

	global data

--------------------------------------------------------------------*/

extern struct Library *DiskFontBase;
extern struct Library *MysticBase;
extern struct Library *GuiGFXBase;
extern struct Library *RenderBase;
extern struct Library *CyberGfxBase;
extern struct Library *IconBase;
extern struct IntuitionBase *IntuitionBase;
extern struct NewIconBase *NewIconBase;
extern struct Library *NeuralBase;

extern UBYTE FAR MysticLogo[LOGOWIDTH*LOGOHEIGHT];
extern ULONG MysticLogoPalette[LOGONUMCOLORS];
extern UBYTE FAR MysticButtons[BUTTONSWIDTH*BUTTONSHEIGHT];
extern ULONG MysticButtonsPalette[BUTTONSNUMCOLORS];
extern UBYTE FAR MysticAnim[ANIMWIDTH*ANIMHEIGHT*ANIMFRAMES];
extern ULONG MysticAnimPalette[ANIMNUMCOLORS];

extern struct NewMenu mainmenu[];

extern APTR mainpool;
extern int globalpriority;

extern char mypersonalID[30];

extern struct EasyStruct newiconreq;
extern struct EasyStruct nonewiconreq;
extern struct EasyStruct registerreq;
extern struct EasyStruct aboutreq;
extern struct EasyStruct aboutregisteredreq;
extern struct EasyStruct saveerrorreq;
extern struct EasyStruct nopicreq;
extern struct EasyStruct copysamefilereq;
extern struct EasyStruct renamesamefilereq;
extern struct EasyStruct noneuralreq;
extern struct EasyStruct errorsavinglistreq;
extern struct EasyStruct nobuttonsreq;
extern struct EasyStruct noviewreq;
extern struct EasyStruct nopichandlerreq;

/*--------------------------------------------------------------------

	global prototypes

--------------------------------------------------------------------*/

extern BOOL InitGlobal(void);
extern void CloseGlobal(void);

extern char *_StrDup(char *s);

#ifdef USEPOOLS
	extern void *Malloc(unsigned long size);
	extern void *Malloclear(unsigned long size);
	extern void Free(void *mem);
#else
	#ifdef TRACKMEMORY
		extern void *Malloc(unsigned long size);
		extern void *Malloclear(unsigned long size);
		extern void Free(void *mem);
	#else
		#define Malloc(s) AllocVec((s), MEMF_PUBLIC)
		#define Malloclear(s) AllocVec((s), MEMF_PUBLIC + MEMF_CLEAR)
		#define Free(s) FreeVec((s))
	#endif
#endif

extern void MemStats(LONG *numalloc, LONG *numbytes);
extern LONG _Stricmp(char *s1, char *s2);
extern unsigned char *_FilePart(char *p);
extern int getrandom(int min, int max);

#endif

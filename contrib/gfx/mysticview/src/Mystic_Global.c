/*********************************************************************
----------------------------------------------------------------------

	MysticView
	global functions

------------------------------------------------------ tabsize = 4 ---
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#ifdef MATH_68882
	#include <m68881.h>
#endif

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/render.h>

#include <exec/memory.h>
#include <libraries/commodities.h>

#include "Mystic_Texts.h"


/*------------------------------------------------------------------*/

	static char versionstring[] = "$VER: " PROGNAME PROGVERSION "";

/*------------------------------------------------------------------*/

struct Library *DiskFontBase = NULL;
struct Library *MysticBase = NULL;
struct Library *GuiGFXBase = NULL;
struct Library *RenderBase = NULL;
struct Library *CyberGfxBase = NULL;
struct Library *IconBase = NULL;
struct Library *CxBase = NULL;
struct NewIconBase *NewIconBase = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct Library *NeuralBase = NULL;

#ifdef USEPOOLS
	APTR mainpool = NULL;
	struct SignalSemaphore memsemaphore;
#endif

char mypersonalID[30];

int	globalpriority;

/*------------------------------------------------------------------*/

ULONG MysticLogoPalette[LOGONUMCOLORS] =
{
	0x00410041,0x00181818,0x00181820,0x00202020,0x00202028,0x00282828,0x00282831,0x00312439,0x002D2841,0x00312849,0x00392841,0x00412841,0x00313139,0x00393939,0x00283549,0x00313545,
	0x00393941,0x00393949,0x00314149,0x00413941,0x00413149,0x00493549,0x00101014,0x00414149,0x00494141,0x00494149,0x00414941,0x00414949,0x00514149,0x00393151,0x00393955,0x00354151,
	0x00413959,0x00454151,0x00454159,0x00494161,0x00494169,0x00394955,0x00494951,0x00495151,0x00414959,0x00494959,0x00395161,0x00494D65,0x00495959,0x00415961,0x00495961,0x00513951,
	0x00593955,0x00614159,0x00613961,0x00515155,0x00554961,0x00514969,0x00515161,0x00595169,0x00595969,0x00614965,0x00615169,0x00615969,0x00515171,0x00595179,0x00595975,0x00615171,
	0x00615971,0x00615986,0x00416169,0x004D6159,0x00516161,0x00516965,0x00597169,0x00496D75,0x00517175,0x00596171,0x00616171,0x00616179,0x00616186,0x00597971,0x00517979,0x00597979,
	0x00598679,0x00617971,0x00618679,0x00618E79,0x00597986,0x00518686,0x00598686,0x00598E86,0x00618E86,0x00694161,0x00714161,0x006D4961,0x00694969,0x00714169,0x00714969,0x006D5169,
	0x00794D69,0x00794971,0x00795171,0x00795179,0x00695971,0x00696179,0x00715D71,0x00715979,0x00696586,0x00795979,0x00796179,0x00796186,0x00797982,0x00698E79,0x00698E86,0x00699686,
	0x00864971,0x00865171,0x008E5171,0x008E5179,0x00865979,0x00866186,0x008E5979,0x00965179,0x00965979,0x00965986,0x009E5986,0x00966186,0x009E6186,0x0061618E,0x0069618E,0x0069698E,
	0x00696996,0x00716996,0x00757592,0x00716D9E,0x0079719E,0x007971A6,0x007979A2,0x0059868E,0x00598E8E,0x00618E8E,0x0059968E,0x0061968E,0x0069968E,0x00619696,0x00619E96,0x00699E8E,
	0x00699E96,0x0069A696,0x00659E9E,0x0069A69E,0x0071A69E,0x009E598E,0x00A6598E,0x009E618E,0x00A6618E,0x008E8E92,0x009A9A9A,0x008679A6,0x008679AE,0x008A86A6,0x008686AE,0x008E86AE,
	0x008E8EAA,0x008E86B6,0x008E8EB6,0x00968EBA,0x009696BA,0x009E96BE,0x009A92C6,0x009E9EC6,0x00A6A2CE,0x00A6A6D6,0x0069AE9E,0x0071AE9E,0x0071AEA6,0x0071B6A6,0x0079B6A6,0x0079B6AE,
	0x0079BEAE,0x0086BEAE,0x0079C6B2,0x0086C6AE,0x0086C6B6,0x0086CEB6,0x0086CEBE,0x008ECEBE,0x008ED6BE,0x008ED6C6,0x008EDFC6,0x0096DFC6,0x0096DFCE,0x0096E7CE,0x0096EFCE,0x009EEFD6,
	0x009EF7D6,0x009EF7DF,0x00A6F7DF,0x00A6FFDF,0x00A6FFE7,0x00AE598E,0x00AE618E,0x00AE6196,0x00AE6996,0x00B66996,0x00B6699E,0x00BE699E,0x00BE719E,0x00BE71A6,0x00C6719E,0x00C671A6,
	0x00CE71A6,0x00CE79A6,0x00CE79AE,0x00AEA6D6,0x00D679AE,0x00DF79B2,0x00DF86B6,0x00E786B6,0x00E786BE,0x00EF86BE,0x00EF8EBE,0x00F78EC6,0x00FF8EC6,0x00FF96C6,0x00FF96CE,0x00FF96D6,
	0x00FF9ED6,0x00FF9EDF,0x00FFA6DF,0x00FFA6E7,0x00B2B2B6,0x00AEAED6,0x00B6AEDB,0x00BEB6D6,0x00BAB2E7,0x00BEBEE7,0x00D6D6DF,0x00C2BAEF,0x00CAC6F7,0x00CECAFF,0x00DBD2FF,0x00FFAEE7,
	0x00FFAEEF,0x00AEFFE7,0x00AEFFEF,0x00B6FFEF,0x00B6FFF7,0x00B6FFFF,0x00BEFFFF,0x00C6FFFF,0x00E3E3E7,0x00DFDFFF,0x00E7DFFF,0x00EBE7FF,0x00F3EFFF,0x00F7F7FF,0x00FFF7FF,0x00FFFFFF,
};

ULONG MysticButtonsPalette[BUTTONSNUMCOLORS] =
{
		0x00598E65,0x00003900,0x003D6951,0x00000000,0x00418655,0x0069B282,0x00C2FFDF,0x009AFF18,
};

ULONG MysticAnimPalette[ANIMNUMCOLORS] =
{
		0x00000000,0x00241831,0x00312D35,0x00313535,0x00598E65,0x00493D41,0x0028394D,0x00493D4D,0x00414D51,0x00554D55,0x0049615D,0x0051616D,0x00557979,0x00714D65,0x00925579,0x00717182,
		0x00A66586,0x00615D86,0x00717D8E,0x00659E92,0x0075B6A6,0x00A6658E,0x0096869E,0x0086B2AA,0x00AEAAC2,0x0086DBC2,0x00A2FFEB,0x00CE79AA,0x00F386BE,0x00CAB6D6,0x00FBAADB,0x00DFEBFB,
};

/*------------------------------------------------------------------*/

struct NewMenu mainmenu[] =
{
    {NM_TITLE,	"Project",					0, 0, 0, 0},
    //--------------------------------------------//
	{NM_ITEM,	"Clear List",				0, 0, 0, (APTR) MITEM_CLEAR},
	{NM_ITEM,	NM_BARLABEL,				0, 0, 0, 0},
	{NM_ITEM,	"Open Pictures...",			"O", 0, 0, (APTR) MITEM_OPEN},
	{NM_ITEM,	"Open List File...",		"L", 0, 0, (APTR) MITEM_OPENLIST},
	{NM_ITEM,	NM_BARLABEL,				0, 0, 0, 0},
	{NM_ITEM,	"Add Pictures...",			"A", 0, 0, (APTR) MITEM_APPEND},
	{NM_ITEM,	"Add List File...",			0, 0, 0, (APTR) MITEM_APPENDLIST},
	{NM_ITEM,	NM_BARLABEL,				0, 0, 0, 0},
	{NM_ITEM,	"Save List as...",			"S", 0, 0, (APTR) MITEM_SAVELIST},
	{NM_ITEM,	NM_BARLABEL,				0, 0, 0, 0},
	{NM_ITEM,	"Copy Picture to...",		0, 0, 0, (APTR) MITEM_COPYTOPATH},
	{NM_ITEM,	"Move Picture to...",		0, 0, 0, (APTR) MITEM_MOVETOPATH},
	{NM_ITEM,	NM_BARLABEL,				0, 0, 0, 0},
	{NM_ITEM,	"Delete Picture",			"D", 0, 0, (APTR) MITEM_DELETE},
	{NM_ITEM,	NM_BARLABEL,				0, 0, 0, 0},
	{NM_ITEM,	"About...",					0, 0, 0, (APTR) MITEM_ABOUT},
	{NM_ITEM,	NM_BARLABEL,				0, 0, 0, 0},
	{NM_ITEM,	"Hide",						"H", 0, 0, (APTR) MITEM_HIDE},
	{NM_ITEM,	"Quit",						"Q", 0, 0, (APTR) MITEM_QUIT},


    {NM_TITLE,	"Picture",				0, 0, 0, 0},
    //--------------------------------------------//
	{NM_ITEM,	"Next",					"ENTER", NM_COMMANDSTRING, 0, (APTR) MITEM_NEXT},
	{NM_ITEM,	"Previous",				".", NM_COMMANDSTRING, 0, (APTR) MITEM_PREV},
	{NM_ITEM,	"First",				"BCKSPC", NM_COMMANDSTRING, 0, (APTR) MITEM_RESTART},
	{NM_ITEM,	NM_BARLABEL,			0, 0, 0, 0},
	{NM_ITEM,	"Remove",				"DEL", NM_COMMANDSTRING, 0, (APTR) MITEM_REMOVE},
	{NM_ITEM,	NM_BARLABEL,			0, 0, 0, 0},
	{NM_ITEM,	"Scroll...",			0, 0, 0, 0},
		{NM_SUB,	"Left",					"4", NM_COMMANDSTRING, 0, (APTR) 0},
		{NM_SUB,	"Right",				"6", NM_COMMANDSTRING, 0, (APTR) 0},
		{NM_SUB,	"Up",					"8", NM_COMMANDSTRING, 0, (APTR) 0},
		{NM_SUB,	"Down",					"2", NM_COMMANDSTRING, 0, (APTR) 0},
		{NM_SUB,	NM_BARLABEL,			0, 0, 0, 0},
		{NM_SUB,	"Center",				"5", NM_COMMANDSTRING, 0, (APTR) MITEM_RESETPOS},
	{NM_ITEM,	"Zoom...",				0, 0, 0, 0},
		{NM_SUB,	"Zoom in",				"+", NM_COMMANDSTRING, 0, (APTR) MITEM_ZOOMIN},
		{NM_SUB,	"Zoom out",				"-", NM_COMMANDSTRING, 0, (APTR) MITEM_ZOOMOUT},
		{NM_SUB,	NM_BARLABEL,			0, 0, 0, 0},
		{NM_SUB,	"Reset Zoom",			"*", NM_COMMANDSTRING, 0, (APTR) MITEM_RESETZOOM},
	{NM_ITEM,	"Rotate...",			0, 0, 0, 0},
		{NM_SUB,	"Rotate Left",			"(", NM_COMMANDSTRING, 0, (APTR) MITEM_ROTATELEFT},
		{NM_SUB,	"Rotate Right",			")", NM_COMMANDSTRING, 0, (APTR) MITEM_ROTATERIGHT},
		{NM_SUB,	NM_BARLABEL,			0, 0, 0, 0},
		{NM_SUB,	"Reset Rotation",		"/", NM_COMMANDSTRING, 0, (APTR) MITEM_RESETROT},
	{NM_ITEM,	"Reset All",			"HELP", NM_COMMANDSTRING, 0, (APTR) MITEM_RESETALL},
	{NM_ITEM,	NM_BARLABEL,			0, 0, 0, 0},
	{NM_ITEM,	"Flip Horizontally",	"X", NM_COMMANDSTRING, 0, (APTR) MITEM_FLIPX},
	{NM_ITEM,	"Flip Vertically",		"Y", NM_COMMANDSTRING, 0, (APTR) MITEM_FLIPY},
	{NM_ITEM,	"Negative",				"~", NM_COMMANDSTRING, 0, (APTR) MITEM_NEGATIVE},


    {NM_TITLE,	"Options",				0, 0, 0, 0},
    //--------------------------------------------//
	{NM_ITEM,	"Slideshow",			"S", CHECKIT | MENUTOGGLE | NM_COMMANDSTRING, 0, (APTR) MITEM_SLIDE},
	{NM_ITEM,	"Loop",					0, CHECKIT | MENUTOGGLE, 0, (APTR) MITEM_LOOP},
	{NM_ITEM,	NM_BARLABEL,			0, 0, 0, 0},
	{NM_ITEM,	"Scale with Aspect",	"1", CHECKIT | NM_COMMANDSTRING, 16, (APTR) MITEM_KEEPASPECT1},
	{NM_ITEM,	"Display 1:1 Pixel",	"3", CHECKIT | NM_COMMANDSTRING, 8, (APTR) MITEM_IGNOREASPECT},
	{NM_ITEM,	"Learn",				0, CHECKIT | MENUTOGGLE, 0, (APTR) MITEM_AUTODISPLAYMODE},
	{NM_ITEM,	NM_BARLABEL,			0, 0, 0, 0},
	{NM_ITEM,	"Show Buttons",			"B", CHECKIT | MENUTOGGLE | NM_COMMANDSTRING, 0, (APTR) MITEM_SHOWBUTTONS},
	{NM_ITEM,	"Show Picture Info",	"I", CHECKIT | MENUTOGGLE | NM_COMMANDSTRING, 0, (APTR) MITEM_PICINFO},
	{NM_ITEM,	"Show Arrows",			"A", CHECKIT | MENUTOGGLE | NM_COMMANDSTRING, 0, (APTR) MITEM_SHOWARROWS},
	{NM_ITEM,	"Show PIP",				"P", CHECKIT | MENUTOGGLE | NM_COMMANDSTRING, 0, (APTR) MITEM_SHOWPIP},
	{NM_ITEM,	"Show Cursor",			"C", CHECKIT | MENUTOGGLE | NM_COMMANDSTRING, 0, (APTR) MITEM_MARKCENTER},
	{NM_ITEM,	NM_BARLABEL,			0, 0, 0, 0},
	{NM_ITEM,	"Mouse Drag Picture",	0, CHECKIT | MENUTOGGLE, 0, (APTR) MITEM_LMB_DRAG},
	{NM_ITEM,	"Reset Display",		0, CHECKIT | MENUTOGGLE, 0, (APTR) MITEM_RESETDISPLAYSETTINGS},
	{NM_ITEM,	"Auto File Requester",	0, CHECKIT | MENUTOGGLE, 0, (APTR) MITEM_REQUESTFILE},
	{NM_ITEM,	"Show Pictures Only",	0, CHECKIT | MENUTOGGLE, 0, (APTR) MITEM_FILTERPICTURES},
	{NM_ITEM,	"Autocrop Pictures",	0, CHECKIT | MENUTOGGLE, 0, (APTR) MITEM_AUTOCROP},
	{NM_ITEM,	"Scan Asynchronously",	0, CHECKIT | MENUTOGGLE, 0, (APTR) MITEM_ASYNCSCANNING},
	{NM_ITEM,	"Create Thumbnails",	0, CHECKIT | MENUTOGGLE, 0, (APTR) MITEM_CREATETHUMBNAILS},
	{NM_ITEM,	"Append Drag'n'Drop",	0, CHECKIT | MENUTOGGLE, 0, (APTR) MITEM_APPENDICONS},


    {NM_TITLE,	"Settings",				0, 0, 0, 0},
    //--------------------------------------------//
    {NM_ITEM,	"Screen...",			0, 0, 0, (APTR) MITEM_SELECTPUBSCREEN},
    {NM_ITEM,	"Window...",			0, 0, 0, 0},
		{NM_SUB,	"Small",				0, CHECKIT, 2+4+8+16+32, (APTR) MITEM_SMALLWINDOW},
		{NM_SUB,	"Medium",				0, CHECKIT, 1+4+8+16+32, (APTR) MITEM_HALFWINDOW},
		{NM_SUB,	"Large",				0, CHECKIT, 1+2+8+16+32, (APTR) MITEM_LARGEWINDOW},
		{NM_SUB,	"Visible",				0, CHECKIT, 1+2+4+16+32, (APTR) MITEM_VISIBLEWINDOW},
		{NM_SUB,	"Full",					0, CHECKIT, 1+2+4+8+32, (APTR) MITEM_FULLWINDOW},
		{NM_SUB,	"Fixed Size",			0, CHECKIT, 1+2+4+8+16, (APTR) MITEM_FIXEDWINDOWSIZE},
		{NM_SUB,	NM_BARLABEL,			0, 0, 0, 0},
		{NM_SUB,	"Centered",				0, CHECKIT, 256+512, (APTR) MITEM_CENTERWINDOW},
		{NM_SUB,	"Under Mouse",			0, CHECKIT, 128+512, (APTR) MITEM_MOUSEWINDOW},
		{NM_SUB,	"Fixed Pos.",			0, CHECKIT, 128+256, (APTR) MITEM_FIXEDWINDOWPOS},
		{NM_SUB,	NM_BARLABEL,			0, 0, 0, 0},
		{NM_SUB,	"Backdrop",				0, CHECKIT | MENUTOGGLE, 0, (APTR) MITEM_BACKDROP},
		{NM_SUB,	NM_BARLABEL,			0, 0, 0, 0},
		{NM_SUB,	"Snap Window",			0, 0, 0, (APTR) MITEM_SNAPWINDOW},
	{NM_ITEM,	NM_BARLABEL,			0, 0, 0, 0},
	{NM_ITEM,	"Slideshow Delay...",	0, 0, 0, 0},
		{NM_SUB,	"none",						0, CHECKIT, 2+4+8+16+32+64+128+256+512+1024, (APTR) MITEM_SLIDEDELAY_0},
		{NM_SUB,	"0.4 seconds",				0, CHECKIT, 1+4+8+16+32+64+128+256+512+1024, (APTR) MITEM_SLIDEDELAY_04},
		{NM_SUB,	"1 second",					0, CHECKIT, 1+2+8+16+32+64+128+256+512+1024, (APTR) MITEM_SLIDEDELAY_1},
		{NM_SUB,	"2 seconds",				0, CHECKIT, 1+2+4+16+32+64+128+256+512+1024, (APTR) MITEM_SLIDEDELAY_2},
		{NM_SUB,	"5 seconds",				0, CHECKIT, 1+2+4+8+32+64+128+256+512+1024, (APTR) MITEM_SLIDEDELAY_5},
		{NM_SUB,	"10 seconds",				0, CHECKIT, 1+2+4+8+16+64+128+256+512+1024, (APTR) MITEM_SLIDEDELAY_10},
		{NM_SUB,	"30 seconds",				0, CHECKIT, 1+2+4+8+16+32+128+256+512+1024, (APTR) MITEM_SLIDEDELAY_30},
		{NM_SUB,	"1 minute",					0, CHECKIT, 1+2+4+8+16+32+64+256+512+1024, (APTR) MITEM_SLIDEDELAY_60},
		{NM_SUB,	"2 minutes",				0, CHECKIT, 1+2+4+8+16+32+64+128+512+1024, (APTR) MITEM_SLIDEDELAY_120},
		{NM_SUB,	"5 minutes",				0, CHECKIT, 1+2+4+8+16+32+64+128+256+1024, (APTR) MITEM_SLIDEDELAY_300},
		{NM_SUB,	"10 minutes",				0, CHECKIT, 1+2+4+8+16+32+64+128+256+512, (APTR) MITEM_SLIDEDELAY_600},
	{NM_ITEM,	"Sort Pictures by...",		0, 0, 0, 0},
		{NM_SUB,	"Scan Order",				0, CHECKIT, 2+4+8+16+32, (APTR) MITEM_SORT_NONE},
		{NM_SUB,	"File Name",				0, CHECKIT, 1+4+8+16+32, (APTR) MITEM_SORT_ALPHA_FILE},
		{NM_SUB,	"Path, File Name",			0, CHECKIT, 1+2+8+16+32, (APTR) MITEM_SORT_ALPHA_FULL},
		{NM_SUB,	"Random",					0, CHECKIT, 1+2+4+16+32, (APTR) MITEM_SORT_RANDOM},
		{NM_SUB,	"File Date",				0, CHECKIT, 1+2+4+8+32, (APTR) MITEM_SORT_DATE},
		{NM_SUB,	"File Size",				0, CHECKIT, 1+2+4+8+16, (APTR) MITEM_SORT_FILESIZE},
		{NM_SUB,	NM_BARLABEL,				0, 0, 0, 0},
		{NM_SUB,	"Reverse Order",			0, CHECKIT | MENUTOGGLE, 0, (APTR) MITEM_SORT_REVERSE},
	{NM_ITEM,	"After Last Picture...",	0, 0, 0, 0},
		{NM_SUB,	"Hide",					0, CHECKIT | MENUTOGGLE, 2, (APTR) MITEM_AUTOHIDE},
		{NM_SUB,	"Quit",					0, CHECKIT | MENUTOGGLE, 1, (APTR) MITEM_AUTOQUIT},
		{NM_SUB,	NM_BARLABEL,			0, 0, 0, 0},
		{NM_SUB,	"Stop Slideshow",		0, CHECKIT | MENUTOGGLE, 0, (APTR) MITEM_AUTOSTOP},
		{NM_SUB,	"Clear List",			0, CHECKIT | MENUTOGGLE, 0, (APTR) MITEM_AUTOCLEAR},
	{NM_ITEM,	"Picture Cache...",		0, 0, 0, 0},
		{NM_SUB,	"Off",						0, CHECKIT, ~1 & 31, (APTR) MITEM_CACHE_OFF},
		{NM_SUB,	"Small",					0, CHECKIT, ~2 & 31, (APTR) MITEM_CACHE_SMALL},
		{NM_SUB,	"Medium",					0, CHECKIT, ~4 & 31, (APTR) MITEM_CACHE_MEDIUM},
		{NM_SUB,	"Large",					0, CHECKIT, ~8 & 31, (APTR) MITEM_CACHE_LARGE},
		{NM_SUB,	"Huge",						0, CHECKIT, ~16 & 31, (APTR) MITEM_CACHE_HUGE},
	{NM_ITEM,	"Rendering...",			0, 0, 0, 0},
		{NM_SUB,	"Static Palette",		0, CHECKIT | MENUTOGGLE, 0, (APTR) MITEM_STATIC},
		{NM_SUB,	NM_BARLABEL,			0, 0, 0, 0},
		{NM_SUB,	"Grid Preview",			0, CHECKIT | MENUTOGGLE, 8, (APTR) MITEM_GRID},
		{NM_SUB,	"Opaque Refresh",		0, CHECKIT | MENUTOGGLE, 4, (APTR) MITEM_OPAQUE},
		{NM_SUB,	NM_BARLABEL,			0, 0, 0, 0},
		{NM_SUB,	"Auto Dither",			0, CHECKIT | MENUTOGGLE, 64, (APTR) MITEM_DITHERAUTO},
		{NM_SUB,	"Always Dither",		0, CHECKIT | MENUTOGGLE, 32, (APTR) MITEM_DITHERON},
	{NM_ITEM,	NM_BARLABEL,			0, 0, 0, 0},
	{NM_ITEM,	"Load Preset...",		0, 0, 0, (APTR) MITEM_LOADPRESET},
	{NM_ITEM,	"Save as Preset...",	0, 0, 0, (APTR) MITEM_SAVEPRESET},
	{NM_ITEM,	"Use Preset...",		0, 0, 0, (APTR) MITEM_USEPRESET},
	{NM_ITEM,	NM_BARLABEL,			0, 0, 0, 0},
	{NM_ITEM,	"Reset to Defaults",	0, 0, 0, (APTR) MITEM_RESETSETTINGS},
	{NM_ITEM,	"Last Saved",			0, 0, 0, (APTR) MITEM_LASTSETTINGS},
	{NM_ITEM,	"Restore",				0, 0, 0, (APTR) MITEM_RESTORESETTINGS},
	{NM_ITEM,	NM_BARLABEL,			0, 0, 0, 0},
	{NM_ITEM,	"Auto Save Settings",	0, CHECKIT | MENUTOGGLE, 0, (APTR) MITEM_AUTOSAVESETTINGS},
	{NM_ITEM,	"Save Settings",		"!", 0, 0, (APTR) MITEM_SAVESETTINGS},

    {NM_END, NULL, 0, 0, 0, 0,}
};


/*--------------------------------------------------------------------

	Requesters etc.

--------------------------------------------------------------------*/

struct EasyStruct nopichandlerreq =
{
	sizeof(struct EasyStruct), 0,
	MVTEXT_PROBLEM,
	MVREQ_ERROR_CREATING_PICHANDLER,
	MVREQ_OKAY
};

struct EasyStruct noviewreq =
{
	sizeof(struct EasyStruct), 0,
	MVTEXT_PROBLEM,
	MVREQ_ERROR_CREATING_VIEW,
	MVREQ_OKAY
};

struct EasyStruct nobuttonsreq =
{
	sizeof(struct EasyStruct), 0,
	MVTEXT_PROBLEM,
	MVREQ_ERROR_CREATING_BUTTONS,
	MVREQ_OKAY
};


struct EasyStruct newiconreq =
{
	sizeof(struct EasyStruct), 0,
	MVTEXT_PROBLEM,
	MVREQ_ERROR_CREATING_THUMBNAIL,
	MVREQ_OKAY
};

struct EasyStruct nonewiconreq =
{
	sizeof(struct EasyStruct), 0,
	MVTEXT_NOTE,
	MVREQ_ERROR_NO_NEWICON_LIB,
	MVREQ_OKAY
};

struct EasyStruct aboutreq =
{
	sizeof(struct EasyStruct), 0,
	MVTEXT_ABOUT,
	MVREQ_ABOUT_UNREGISTERED,
	MVREQ_OKAY

};

struct EasyStruct aboutregisteredreq =
{
	sizeof(struct EasyStruct), 0,
	MVTEXT_ABOUT,
	MVREQ_ABOUT_REGISTERED,
	MVREQ_OKAY
};

struct EasyStruct saveerrorreq =
{
	sizeof(struct EasyStruct), 0,
	MVTEXT_PROBLEM,
	MVREQ_ERROR_SAVING,
	MVREQ_OKAY
};

struct EasyStruct nopicreq =
{
	sizeof(struct EasyStruct), 0,
	MVTEXT_NOTE,
	MVREQ_NOPICTURE,
	MVREQ_OKAY
};

struct EasyStruct copysamefilereq =
{
	sizeof(struct EasyStruct), 0,
	MVTEXT_PROBLEM,
	MVREQ_ERROR_COPYSAMEFILE,
	MVREQ_OKAY
};

struct EasyStruct renamesamefilereq =
{
	sizeof(struct EasyStruct), 0,
	MVTEXT_PROBLEM,
	MVREQ_ERROR_RENAMESAMEFILE,
	MVREQ_OKAY
};

struct EasyStruct noneuralreq =
{
	sizeof(struct EasyStruct), 0,
	MVTEXT_PROBLEM,
	MVREQ_ERROR_NONEURALLIB,
	MVREQ_OKAY
};

struct EasyStruct errorsavinglistreq =
{
	sizeof(struct EasyStruct), 0,
	MVTEXT_PROBLEM,
	MVREQ_ERROR_SAVINGLIST,
	MVREQ_OKAY
};


/*--------------------------------------------------------------------

	ToggleMenuFlags(menu,identifier,setmask,clearmask)

--------------------------------------------------------------------*/

void ToggleMenuFlags(struct Menu *menu, ULONG id, UWORD set, UWORD clear)
{
	do
	{
		struct MenuItem *mi = menu->FirstItem;
		do
		{
			if (mi->SubItem)
			{
				struct MenuItem *si = mi->SubItem;
				do
				{
					if (GTMENUITEM_USERDATA(si) == (APTR)id)
					{
						si->Flags &= ~clear;
						si->Flags |= set;
					}
				} while (si = si->NextItem);
			}

			if (GTMENUITEM_USERDATA(mi) == (APTR)id)
			{
				mi->Flags &= ~clear;
				mi->Flags |= set;
			}

		} while (mi = mi->NextItem);

	} while (menu = menu->NextMenu);

}



/*--------------------------------------------------------------------

	value = GetMenuFlag(menu, identifier, true, false)

--------------------------------------------------------------------*/

ULONG GetMenuFlag(struct Menu *menu, ULONG id, int true, int false)
{
	do
	{
		struct MenuItem *mi = menu->FirstItem;
		do
		{
			if (mi->SubItem)
			{
				struct MenuItem *si = mi->SubItem;
				do
				{
					if (GTMENUITEM_USERDATA(si) == (APTR)id)
					{
						return (ULONG)((si->Flags & CHECKED) ? true : false);
					}
				} while (si = si->NextItem);
			}

			if (GTMENUITEM_USERDATA(mi) == (APTR)id)
			{
				return (ULONG)((mi->Flags & CHECKED) ? true : false);
			}

		} while (mi = mi->NextItem);

	} while (menu = menu->NextMenu);

	return 0;
}


/*--------------------------------------------------------------------

	menuitem = FindMenuItem(menu, identifier)

--------------------------------------------------------------------*/

struct MenuItem *FindMenuItem(struct Menu *menu, ULONG id)
{
	do
	{
		struct MenuItem *mi = menu->FirstItem;
		do
		{
			if (mi->SubItem)
			{
				struct MenuItem *si = mi->SubItem;
				do
				{
					if (GTMENUITEM_USERDATA(si) == (APTR)id)
					{
						return si;
					}
				} while (si = si->NextItem);
			}

			if (GTMENUITEM_USERDATA(mi) == (APTR)id)
			{
				return mi;
			}

		} while (mi = mi->NextItem);

	} while (menu = menu->NextMenu);

	return NULL;
}


/*--------------------------------------------------------------------

	Malloc
	Free

--------------------------------------------------------------------*/

#ifdef TRACKMEMORY
	static LONG alloccount = 0;
	static LONG allocbytes = 0;
#endif

void *Malloc(unsigned long size)
{
	#ifndef USEPOOLS

		void *buf;
		if (buf = AllocVec(size, MEMF_PUBLIC))
		{
			#ifdef TRACKMEMORY
				alloccount++;
			#endif
		}
		return buf;

	#else

		ULONG *buf;
		
		ObtainSemaphore(&memsemaphore);

		if (buf = AllocPooled(mainpool, size + sizeof(ULONG)))
		{
			*buf++ = size;

			#ifdef TRACKMEMORY
				alloccount++;
				allocbytes += size + sizeof(ULONG);
			#endif
		}

		ReleaseSemaphore(&memsemaphore);

		return (void *) buf;

	#endif
}

void *Malloclear(unsigned long size)
{
	#ifndef USEPOOLS

		void *buf;
		if (buf = AllocVec(size, MEMF_PUBLIC + MEMF_CLEAR))
		{
			#ifdef TRACKMEMORY
				alloccount++;
			#endif
		}
		return buf;

	#else

		ULONG *buf;

		ObtainSemaphore(&memsemaphore);

		if (buf = AllocPooled(mainpool, size + sizeof(ULONG)))
		{
			*buf++ = size;

			memset(buf, 0, size);

			#ifdef TRACKMEMORY
				alloccount++;
				allocbytes += size + sizeof(ULONG);
			#endif
		}

		ReleaseSemaphore(&memsemaphore);

		return (void *) buf;

	#endif
}


void Free(void *mem)
{
	#ifndef USEPOOLS

		if (mem)
		{
			alloccount--;
			FreeVec(mem);
		}

	#else

		if (mem)
		{
			ULONG *buf = (ULONG *) mem;

			ObtainSemaphore(&memsemaphore);

			--buf;
			
			#ifdef TRACKMEMORY
				alloccount--;
				allocbytes -= *buf + sizeof(ULONG);
			#endif

			FreePooled(mainpool, buf, *buf + sizeof(ULONG));

			ReleaseSemaphore(&memsemaphore);
		}

	#endif
}



/*--------------------------------------------------------------------

	s2 = _StrDup(s)

--------------------------------------------------------------------*/

char *_StrDup(char *s)
{
	char *s2 = NULL;

	if (s)
	{
		int l = strlen(s);
		if (s2 = Malloc(l + 1))
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


/*------------------------------------------------------------------*/

LONG _Stricmp(char *s1, char *s2)
{
	if (s1 && s2)
	{
		return Stricmp(s1, s2);
	}

	if (s1 == NULL && s2 == NULL)
	{
		return 0;
	}

	return -1;
}

/*------------------------------------------------------------------*/

unsigned char *_FilePart(char *p)
{
	if (p)
	{
		return FilePart(p);
	}

	return NULL;
}


/*------------------------------------------------------------------*/

//unsigned short randomseed[3] = {0,0,0x330e};

int seed;

int getrandom(int min, int max)
{
	int r;
	r = min + rand() % (max - min + 1);
//	r = min + nrand48(randomseed) % (max-min+1);
	return r;
}

/*------------------------------------------------------------------*/

BOOL InitGlobal(void)
{
	sprintf(mypersonalID, "CBF%x", GetUniqueID());

	MysticBase = OpenLibrary("mysticview.library", MYSTIC_VERSION);
	if (!MysticBase)
	{
		MysticBase = OpenLibrary("libs/mysticview.library", MYSTIC_VERSION);
	}

	RenderBase = OpenLibrary("render.library", RENDER_VERSION);
	if (!RenderBase)
	{
		RenderBase = OpenLibrary("libs/render.library", RENDER_VERSION);
	}

	GuiGFXBase = OpenLibrary("guigfx.library", GUIGFX_VERSION);
	if (!GuiGFXBase)
	{
		GuiGFXBase = OpenLibrary("libs/guigfx.library", GUIGFX_VERSION);
	}

	NeuralBase = OpenLibrary("neuralnet.library", NEURALNET_VERSION);
	DiskFontBase = OpenLibrary("diskfont.library", 0);
	CyberGfxBase = OpenLibrary("cybergraphics.library", 0);
	IconBase = OpenLibrary("icon.library", 0);
	NewIconBase = (struct NewIconBase *) OpenLibrary("newicon.library", 40);
	CxBase = OpenLibrary("commodities.library", 37);
	IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 39);

	#ifdef USEPOOLS
	mainpool = CreatePool(MEMF_PUBLIC, POOLPUDSIZE, POOLTHRESSIZE);

	InitSemaphore(&memsemaphore);

	if (mainpool)
	{

	#endif

		if (GuiGFXBase && IconBase && CxBase && IntuitionBase && MysticBase)
		{
			time_t timeval;
			time(&timeval);
			srand((int) timeval);

			return TRUE;
		}

	#ifdef USEPOOLS
	}
	#endif


	if (!CxBase)
	{
		printf("couldn't open commodities library.\n");
	}

	if (!RenderBase)
	{
		printf("couldn't open render.library v%d.\n", RENDER_VERSION);
	}

	if (!GuiGFXBase)
	{
		printf("couldn't open guigfx.library v%d.\n", GUIGFX_VERSION);
	}

	if (!MysticBase)
	{
		printf("couldn't open mysticview.library v%d.\n", MYSTIC_VERSION);
	}

	CloseGlobal();


	return FALSE;

}


void CloseGlobal(void)
{
	CloseLibrary(NeuralBase);
	NeuralBase = NULL;

	CloseLibrary(MysticBase);
	MysticBase = NULL;

	CloseLibrary((struct Library *) NewIconBase);
	NewIconBase = NULL;

	CloseLibrary((struct Library *) IntuitionBase);
	IntuitionBase = NULL;

	CloseLibrary(CxBase);
	CxBase = NULL;

	CloseLibrary(RenderBase);
	RenderBase = NULL;

	CloseLibrary(CyberGfxBase);
	CyberGfxBase = NULL;

	CloseLibrary(DiskFontBase);
	DiskFontBase = NULL;

	CloseLibrary(IconBase);
	IconBase = NULL;

	CloseLibrary(GuiGFXBase);
	GuiGFXBase = NULL;

	#ifdef USEPOOLS
	if (mainpool)
	{
		#ifdef TRACKMEMORY
		if (alloccount || allocbytes)
		{
			printf("*** memory leak detected - %ld allocations (%ld bytes) pending.\n", alloccount, allocbytes);
			printf("*** the memory has been returned to the system.\n");
		}
		#endif

		DeletePool(mainpool);
		mainpool = NULL;
	}

	#else

	#ifdef TRACKMEMORY
	if (alloccount)
	{
		printf("*** memory leak detected - %ld allocations pending.\n", alloccount);
	}
	#endif

	#endif
}


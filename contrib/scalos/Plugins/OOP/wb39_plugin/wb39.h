// wb39.h
// $Date$
// $Revision$


#ifndef	WB39_H_INCLUDED
#define	WB39_H_INCLUDED

//----------------------------------------------------------------------------

#include <defs.h>
#include <scalos/scalos.h>

//----------------------------------------------------------------------------

#define	d(x)	;
#define	d1(x)	;
#define	d2(x)	x;

#define	debugLock_d1(LockName) ;
#define	debugLock_d2(LockName) \
	{\
	char xxName[200];\
	strcpy(xxName, "");\
	NameFromLock((LockName), xxName, sizeof(xxName));\
	KPrintF("%s/%s/%ld: " #LockName "=%08lx <%s>\n", __FILE__, __FUNC__, __LINE__, LockName, xxName);\
	}

//----------------------------------------------------------------------------

struct myAppIcon
	{
	struct ScaAppObjNode mai_AppIconClone;	// Copy of OrigAppIcon
	struct TagItem *mai_TagListClone;	// AddAppIcon() Taglist cloned by CloneTagItems();
	struct ScaAppObjNode *mai_OrigAppIcon;
	};

struct myAppWindow
	{
	struct Node maw_Node;

	ULONG maw_Magic;			// Magic ID = "MAWX"

	struct ScaAppObjNode *maw_AppWindow;

	struct MsgPort *maw_origMsgPort;	// original AddAppWindowA MsgPort
	ULONG maw_origUserData;			// original AddAppWindowA UserData

	struct List maw_DropZoneList;
	struct SignalSemaphore maw_Sema;	// Semaphore for maw_DropZoneList

	struct myAppWindowDropZone *maw_ActiveDropZone;
	};

struct myAppWindowDropZone
	{
	struct Node awz_Node;

	struct myAppWindow *awz_AppWindow;

	struct IBox awz_DropBox;
	struct IBox awz_MeasureBox;	// original dimensions, may be <0 if relative

	BOOL awz_RelBottom;
	BOOL awz_RelRight;
	BOOL awz_RelWidth;
	BOOL awz_RelHeight;

	ULONG awz_ID;
	ULONG awz_UserData;

	struct Hook *awz_Hook;
	};


// aus debug.lib
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);


// node type for workbench lists
#define	NT_WB	0x67


// Hunk IDs in Worbench prefs file

#define	ID_PREF	MAKE_ID('P','R','E','F')
#define	ID_WBNC	MAKE_ID('W','B','N','C')
#define ID_WBHD MAKE_ID('W','B','H','D')

// Offsets of several WB prefs settings in WBNC hunk
enum WBPrefsItems
	{
	wpi_DefaultStackSize_Hi = 0,	// ULONG : 0,1
	wpi_DefaultStackSize_Lo = 1,	// ULONG : 0,1
	wpi_KeyboardRestart = 3,	// keyboard restart delay in seconds
	wpi_UseNewIcons = 13,
	wpi_NoColorIcons = 14,
	wpi_SetMWBColor = 17,
	wpi_NoTitleBar = 18,
	wpi_NoVolumeGauge = 19,
	};


// Instance structure for overriden window.sca class
struct WB39inst
	{
	Object *wb39i_VolumeGaugeObject;
	};

//----------------------------------------------------------------------------

#endif	/* WB39_H_INCLUDED */

//
// PopupMenu
// ©1996-2000 Henrik Isaksson
//
// Library init & cleanup
//
// $Date$
// $Revision$
//

#define INIT 1

#include "pmpriv.h"


//
// Libraries we need
//

#ifdef __amigaos4__
#define UTILITYBASE_T struct UtilityBase
#else
#define UTILITYBASE_T struct Library
#endif

UTILITYBASE_T 		*UtilityBase=NULL;
struct IntuitionBase	*IntuitionBase=NULL;
struct GfxBase		*GfxBase=NULL;
struct DosLibrary	*DOSBase=NULL;
struct Library		*CxBase=NULL;
struct Library		*LayersBase=NULL;
struct Library		*CyberGfxBase=NULL;
struct Library 		*PreferencesBase = NULL;

struct ExecBase	 *SysBase;

#ifdef __amigaos4__
struct Library 		*NewlibBase;
struct ExecIFace	*IExec;
struct Interface 	*INewlib;
struct UtilityIFace	*IUtility;
struct GraphicsIFace	*IGraphics;
struct IntuitionIFace	*IIntuition;
struct DOSIFace		*IDOS;
struct CyberGfxIFace	*ICyberGfx;
struct LayersIFace	*ILayers;
struct CommoditiesIFace	*ICommodities;
struct PreferencesIFace *IPreferences;
#endif

APTR			MemPool = NULL;
struct SignalSemaphore	MemPoolSemaphore;

BOOL V40Gfx=FALSE;
BOOL CyberGfx=FALSE;

void CloseLibs(void)
{
	if (MemPool)		 
		DeletePool(MemPool);

#ifdef __amigaos4__
	if (IPreferences)
		DropInterface((struct Interface *) IPreferences);
	if (IUtility)
		DropInterface((struct Interface *)IUtility);
	if (IGraphics)		 
		DropInterface((struct Interface *)IGraphics);
	if (IIntuition)		 
		DropInterface((struct Interface *)IIntuition);
	if (IDOS)		 
		DropInterface((struct Interface *)IDOS);
	if (ICommodities)	 
		DropInterface((struct Interface *)ICommodities);
	if (ILayers)		 
		DropInterface((struct Interface *)ILayers);
	if (ICyberGfx)		 
		DropInterface((struct Interface *)ICyberGfx);
	if (INewlib)		 
		DropInterface((struct Interface *)INewlib);
	if (NewlibBase)		 
		CloseLibrary((struct Library *)NewlibBase);

	IPreferences = NULL;
	IUtility = NULL;
	IGraphics = NULL;
	IIntuition = NULL;
	IDOS = NULL;
	ICommodities = NULL;
	ILayers = NULL;
	ICyberGfx = NULL;
	INewlib = NULL;
	NewlibBase = NULL;
#endif

	if (PreferencesBase)
		CloseLibrary(PreferencesBase);
	if (UtilityBase)	 
		CloseLibrary((struct Library *)UtilityBase);
	if (GfxBase)		 
		CloseLibrary((struct Library *)GfxBase);
	if (IntuitionBase)	 
		CloseLibrary((struct Library *)IntuitionBase);
	if (DOSBase)		 
		CloseLibrary((struct Library *)DOSBase);
	if (CxBase)		 
		CloseLibrary((struct Library *)CxBase);
	if (LayersBase)		 
		CloseLibrary((struct Library *)LayersBase);
	if (CyberGfxBase)	 
		CloseLibrary((struct Library *)CyberGfxBase);

	MemPool = NULL;

	PreferencesBase = NULL;
	UtilityBase=NULL;
	GfxBase=NULL;
	IntuitionBase=NULL;
	DOSBase=NULL;
	CxBase=NULL;
	LayersBase=NULL;
	CyberGfxBase=NULL;
}

BOOL OpenLibs(struct PopupMenuBase *l)
{
	BOOL Success = FALSE;

	if (UtilityBase)
		return TRUE;

	l->pmb_ExecBase = (struct Library *) SysBase;

	do	{
#ifdef __amigaos4__
		NewlibBase = OpenLibrary("newlib.library", 0);
		if (NULL == NewlibBase)
			break;;
		INewlib = GetInterface(NewlibBase, "main", 1, NULL);
		if (NULL == INewlib)
			break;;
#endif

		l->pmb_UtilityBase = OpenLibrary("utility.library",37L);
		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  pmb_UtilityBase=%08lx\n", __LINE__, l->pmb_UtilityBase));
		if (NULL == l->pmb_UtilityBase)
			break;;

		UtilityBase = (UTILITYBASE_T *)l->pmb_UtilityBase;
#ifdef __amigaos4__
		IUtility = (struct UtilityIFace *)GetInterface(l->pmb_UtilityBase, "main", 1, NULL);
		if (NULL == IUtility)
			break;
#endif
		l->pmb_GfxBase = OpenLibrary("graphics.library",40L);
		if (!l->pmb_GfxBase)
			l->pmb_GfxBase = OpenLibrary("graphics.library",37L);
		else
			V40Gfx = TRUE;

		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  pmb_GfxBase=%08lx\n", __LINE__, l->pmb_GfxBase));
		if (NULL == l->pmb_GfxBase)
			break;

		GfxBase = (struct GfxBase *)l->pmb_GfxBase;
#ifdef __amigaos4__
		IGraphics = (struct GraphicsIFace *)GetInterface(l->pmb_GfxBase, "main", 1, NULL);
		if (NULL == IGraphics)
			break;
#endif
		l->pmb_IntuitionBase = OpenLibrary("intuition.library",37L);
		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  pmb_IntuitionBase=%08lx\n", __LINE__, l->pmb_IntuitionBase));
		if (NULL == l->pmb_IntuitionBase)
			break;

		IntuitionBase = (struct IntuitionBase *)l->pmb_IntuitionBase;
#ifdef __amigaos4__
		IIntuition = (struct IntuitionIFace *)GetInterface(l->pmb_IntuitionBase, "main", 1, NULL);
		if (NULL == IIntuition)
			break;
#endif
		l->pmb_DOSBase = OpenLibrary("dos.library",0L);
		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  pmb_DOSBase=%08lx\n", __LINE__, l->pmb_DOSBase));
		if (NULL == l->pmb_DOSBase)
			break;

		DOSBase = (struct DosLibrary *)l->pmb_DOSBase;
#ifdef __amigaos4__
		IDOS = (struct DOSIFace *)GetInterface(l->pmb_DOSBase, "main", 1, NULL);
		if (NULL == IDOS)
			break;
#endif
		l->pmb_CxBase = OpenLibrary("commodities.library",37L);
		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  pmb_CxBase=%08lx\n", __LINE__, l->pmb_CxBase));
		if (NULL == l->pmb_CxBase)
			break;

		CxBase=l->pmb_CxBase;
#ifdef __amigaos4__
		ICommodities = (struct CommoditiesIFace *)GetInterface(l->pmb_CxBase, "main", 1, NULL);
		if (NULL == ICommodities)
			break;
#endif
		LayersBase = OpenLibrary("layers.library",0);
		if (NULL == LayersBase)
			break;
		l->pmb_LayersBase = LayersBase;
		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  pmb_LayersBase=%08lx\n", __LINE__, l->pmb_LayersBase));
#ifdef __amigaos4__
		ILayers = (struct LayersIFace *)GetInterface(l->pmb_LayersBase, "main", 1, NULL);
		if (NULL == ILayers)
			break;
#endif
		PreferencesBase	= OpenLibrary("preferences.library", 39);
		if (NULL == PreferencesBase)
			break;
		l->pmb_PreferencesBase = PreferencesBase;
		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  pmb_PreferencesBase=%08lx\n", __LINE__, l->pmb_PreferencesBase));
#ifdef __amigaos4__
		IPreferences = (struct PreferencesIFace *)GetInterface(l->pmb_PreferencesBase, "main", 1, NULL);
		if (NULL == IPreferences)
			break;
#endif
		
		CyberGfxBase = OpenLibrary("cybergraphics.library",39L);
		if (CyberGfxBase)
			{
#ifdef __amigaos4__
			ICyberGfx = (struct CyberGfxIFace *)GetInterface(CyberGfxBase, "main", 1, NULL);
			if (NULL == ICyberGfx)
				break;
#endif
			CyberGfx=TRUE;
			}
		l->pmb_CyberGfxBase = CyberGfxBase;
		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  pmb_CyberGfxBase=%08lx\n", __LINE__, l->pmb_CyberGfxBase));

		InitSemaphore(&MemPoolSemaphore);

		MemPool = CreatePool(MEMF_PUBLIC | MEMF_CLEAR, 10240L, 10240L);
		if (NULL == MemPool)
			break;

		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  CreatePool OK\n", __LINE__));
		
		PM_Prefs_Load(PMP_PATH_OLD, PMP_PATH_NEW);

		Success = TRUE;
		} while (0);

	return Success;
}

//
// Library initializtion
//

int PMLibInit(struct PopupMenuBase *PopupMenuBase)
{
	d1(KPrintF(__FUNC__ "/%ld: START PopupMenuBase=%08lx\n", __LINE__, PopupMenuBase));

	if (OpenLibs(PopupMenuBase))
		{
		d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  success\n", __LINE__));
		return -1;
		}

	d1(KPrintF(__FILE__ "/" __FUNC__ "/%ld  fail\n", __LINE__));

	return 0;
}

void PMLibCleanup(struct PopupMenuBase *l)
{

	//kprintf("UserLibCleanUp, pmbase = %08lx\n", l);

	//PM_FreeAllImages();

	PM_Prefs_Free();

	//CloseLibs();

	if (MemPool)		 
		DeletePool(MemPool);

	if (PreferencesBase)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)IPreferences);
#endif
		CloseLibrary(PreferencesBase);
		}
	if (UtilityBase)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)IUtility);
#endif
		CloseLibrary((struct Library *)UtilityBase);
		}
	if (GfxBase)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)IGraphics);
#endif
		CloseLibrary((struct Library *)GfxBase);
		}
	if (IntuitionBase)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)IUtility);
#endif
		CloseLibrary((struct Library *)UtilityBase);
		}
	if (l->pmb_DOSBase)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)IDOS);
#endif
		CloseLibrary((struct Library *)l->pmb_DOSBase);
		}
	if (CxBase)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)ICommodities);
#endif
		CloseLibrary((struct Library *)CxBase);
		}
	if (LayersBase)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)ILayers);
#endif
		CloseLibrary((struct Library *)LayersBase);
		}
	if (CyberGfxBase)
		{
#ifdef __amigaos4__
		DropInterface((struct Interface *)ICyberGfx);
#endif
		CloseLibrary((struct Library *)CyberGfxBase);
		}

	//kprintf("UserLibCleanUp done.\n");
}

#ifdef __AROS__
#include <aros/symbolsets.h>
#include <aros/debug.h>

AROS_SET_LIBFUNC(AROS__UserLibInit, struct PopupMenuBase, PopupMenuBase)
{
    AROS_SET_LIBFUNC_INIT

    return (__UserLibInit(PopupMenuBase) == 0) ? TRUE : FALSE;
    
    AROS_SET_LIBFUNC_EXIT
}

AROS_SET_LIBFUNC(AROS__UserLibCleanup, struct PopupMenuBase, PopupMenuBase)
{
    AROS_SET_LIBFUNC_INIT
    
    __UserLibCleanup(PopupMenuBase);
    
    return TRUE;
    
    AROS_SET_LIBFUNC_EXIT
}

ADD2INITLIB(AROS__UserLibInit, 0);
ADD2EXPUNGELIB(AROS__UserLibCleanup, 0);
#endif


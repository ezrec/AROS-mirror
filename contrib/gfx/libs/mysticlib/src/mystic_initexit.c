/*********************************************************************
----------------------------------------------------------------------

	$VER: mystic_initexit.c 1.0
	©1997 Captain Bifat / TEK neoscientists

----------------------------------------------------------------------
*********************************************************************/

#include "mystic_global.h"

#include <proto/render.h>
#include <render/render.h>
#include <graphics/gfxbase.h>

/*********************************************************************

	MYSTIC_Init
	MYSTIC_Exit

*********************************************************************/

BOOL LIBENT MYSTIC_Init(void)
{

	GadToolsBase = OpenLibrary("gadtools.library", 36);
#ifdef __AROS__
	UtilityBase = (struct UtilityBase *)OpenLibrary("utility.library", 37);
#else
	UtilityBase = OpenLibrary("utility.library", 37);
#endif
	GfxBase = (struct GfxBase *) OpenLibrary("graphics.library", 39);
	DataTypesBase = OpenLibrary("datatypes.library", DATATYPES_VERSION);
	CyberGfxBase = OpenLibrary("cybergraphics.library", CYBERGFX_VERSION);
	DOSBase = (struct DosLibrary *) OpenLibrary("dos.library", 0);
//	MathTransBase = OpenLibrary("mathtrans.library", 0);
	IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 0);

	LayersBase = OpenLibrary("layers.library", 0);

	GuiGFXBase = OpenLibrary("guigfx.library", GUIGFX_VERSION);
	if (!GuiGFXBase)
	{
		GuiGFXBase = OpenLibrary("libs/guigfx.library", GUIGFX_VERSION);
	}

	#ifdef USEPOOLS
		mainpool = CreatePool(MEMF_ANY, 4096, 4096);
		InitSemaphore(&memsemaphore);

		if (mainpool)
		{
	#endif

			if (UtilityBase && GfxBase && GuiGFXBase && GadToolsBase &&
				DOSBase && IntuitionBase && LayersBase) // && MathTransBase
			{
				return TRUE;
			}

	#ifdef USEPOOLS
		}
	#endif
		

	MYSTIC_Exit();
	return FALSE;
}

void LIBENT MYSTIC_Exit(void)
{
	if (mainpool)
	{
		DeletePool(mainpool);
		mainpool = NULL;
	}

	CloseLibrary(LayersBase);
	LayersBase = NULL;

	CloseLibrary(GuiGFXBase);
	GuiGFXBase = NULL;

	CloseLibrary((struct Library *) IntuitionBase);
	IntuitionBase = NULL;

//	CloseLibrary(MathTransBase);
//	MathTransBase = NULL;

	CloseLibrary((struct Library *) DOSBase);
	DOSBase = NULL;

	CloseLibrary(CyberGfxBase);
	CyberGfxBase = NULL;

	CloseLibrary(DataTypesBase);
	DataTypesBase = NULL;

	CloseLibrary((struct Library *) GfxBase);
	GfxBase = NULL;

	CloseLibrary((struct Library *)UtilityBase);
	UtilityBase = NULL;

	CloseLibrary(GadToolsBase);
	GadToolsBase = NULL;
}

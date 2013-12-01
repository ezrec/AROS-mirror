// PalettePrefs.h
// $Date$
// $Revision$


#ifndef SCALOS_PALETTEPREFS_H
#define	SCALOS_PALETTEPREFS_H

enum HookIndex
{
	HOOKNDX_WBPensListConstruct,
	HOOKNDX_WBPensListDestruct,
	HOOKNDX_WBPensListDisplay,
	HOOKNDX_WBPensListCompare,

	HOOKNDX_AllocatedPensListConstruct,
	HOOKNDX_AllocatedPensListDestruct,
	HOOKNDX_AllocatedPensListDisplay,
	HOOKNDX_AllocatedPensListCompare,

	HOOKNDX_ScalosPenListConstruct,
	HOOKNDX_ScalosPenListDestruct,
	HOOKNDX_ScalosPenListDisplay,
	HOOKNDX_ScalosPenListCompare,

	HOOKNDX_GetWBColors,

	HOOKNDX_AddPen,
	HOOKNDX_SelectPalettePen,
	HOOKNDX_SelectScalosPen,
	HOOKNDX_ChangePenColor,
	HOOKNDX_ResetToDefaults,
	HOOKNDX_About,
	HOOKNDX_Open,
	HOOKNDX_LastSaved,
	HOOKNDX_Restore,
	HOOKNDX_SaveAs,
	HOOKNDX_DeletePen,
	HOOKNDX_ContextMenuTrigger,
	HOOKNDX_AslIntuiMsg,

	HOOKNDX_MAX
};

enum ObjectIndex
{
	OBJNDX_APP_Main,
	OBJNDX_WIN_Main,

	OBJNDX_NewButton,
	OBJNDX_DeleteButton,
	OBJNDX_AllocatedPensList,
	OBJNDX_ScalosPenList,
	OBJNDX_ColorAdjust,
	OBJNDX_WBColorsList,
	OBJNDX_AllocatedPensListTitleObj,
	OBJNDX_Lamp_Changed,

	OBJNDX_ContextMenu,

	OBJNDX_Group_Main,

	OBJNDX_MAX
};

struct PalettePrefsInst
{
	ULONG	ppb_fCreateIcons;
	ULONG	ppb_fPreview;
	ULONG	ppb_fAutoPreview;
	ULONG	ppb_fDontChange;

	BYTE	ppb_PalettePrecision;
	ULONG	ppb_PreviewWeight;

	BOOL	ppb_Changed;
	struct Screen *ppb_WBScreen;

	ULONG	ppb_AllocatedPensListImageIndex;

	CONST_STRPTR	ppb_ProgramName;

	struct	Hook ppb_Hooks[HOOKNDX_MAX];
	Object 	*ppb_Objects[OBJNDX_MAX];

	struct FileRequester *ppb_LoadReq;
	struct FileRequester *ppb_SaveReq;

	char ppb_PalettePenListTitle[100];
};

//----------------------------------------------------------------------------

#define	d1(x)	;
#define	d2(x)	x;

// from debug.lib
extern int kprintf(const char *fmt, ...);
extern int KPrintF(const char *fmt, ...);

//----------------------------------------------------------------------------

struct ScalosPalette_LocaleInfo
{
	APTR li_LocaleBase;
	APTR li_Catalog;
	struct LocaleIFace *li_ILocale;
};

#endif /* SCALOS_PALETTEPREFS_H */

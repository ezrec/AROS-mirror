// PatternPrefs.h
// $Date$
// $Revision$


#ifndef SCALOS_PATTERNPREFS_H
#define	SCALOS_PATTERNPREFS_H

#define	Sizeof(array)	(sizeof(array) / sizeof((array)[0]))

enum HookIndex
{
	HOOKNDX_ListDisplay,
	HOOKNDX_ListConstruct,
	HOOKNDX_ListDestruct,
	HOOKNDX_ListCompare,
	HOOKNDX_ListSelectEntry,
	HOOKNDX_ListChangeEntry,
	HOOKNDX_ListChangeEntry2,
	HOOKNDX_ListChangeEntry3,
	HOOKNDX_AddNewEntry,
	HOOKNDX_RemEntry,
	HOOKNDX_LastSaved,
	HOOKNDX_Restore,
	HOOKNDX_ResetToDefaults,
	HOOKNDX_About,
	HOOKNDX_Open,
	HOOKNDX_SaveAs,
	HOOKNDX_PreviewSelect,
	HOOKNDX_ContextMenuTrigger,
	HOOKNDX_ShowPreview,
	HOOKNDX_AutoPreview,
	HOOKNDX_AppMsgInMainList,
	HOOKNDX_SettingsChanged,
	HOOKNDX_Thumbnails,
	HOOKNDX_DesktopPatternChanged,
	HOOKNDX_ScreenPatternChanged,
	HOOKNDX_IconWindowPatternChanged,
	HOOKNDX_TextWindowPatternChanged,
	HOOKNDX_AslIntuiMsg,
	HOOKNDX_ReloadThumbnails,

	HOOKNDX_MAX
};

enum ObjectIndex
{
	OBJNDX_APP_Main,
	OBJNDX_WIN_Main,
	OBJNDX_WIN_Message,
	OBJNDX_MainListView,
	OBJNDX_MainList,
	OBJNDX_NewButton,
	OBJNDX_DelButton,
	OBJNDX_NumButton,
	OBJNDX_Popasl,
	OBJNDX_CyclePatternType,
	OBJNDX_CheckmarkGad,
	OBJNDX_CheckmarkEnh,
	OBJNDX_PrecSlider,
	OBJNDX_CheckmarkAutoDither,
	OBJNDX_PreviewImage,
	OBJNDX_SliderColours,
	OBJNDX_CycleDitherMode,
	OBJNDX_SliderDitherAmount,
	OBJNDX_CheckmarkAsync,
	OBJNDX_CheckmarkFriend,
	OBJNDX_CheckmarkRelayout,
	OBJNDX_CheckmarkRandom,
	OBJNDX_MsgText1,
	OBJNDX_MsgText2,
	OBJNDX_MsgGauge,
	OBJNDX_Group_Enhanced,
	OBJNDX_Group_Preview,
	OBJNDX_Group_String,
	OBJNDX_Group_Slider,
	OBJNDX_Group_MainList,
	OBJNDX_Group_Main,
	OBJNDX_Group_PatternSliders,
	OBJNDX_Group_PatternPreviews,
	OBJNDX_SliderTaskPri,
	OBJNDX_STR_PopAsl,
	OBJNDX_SliderWB,
	OBJNDX_SliderScreen,
	OBJNDX_SliderWin,
	OBJNDX_SliderText,
	OBJNDX_Balance,
	OBJNDX_Menu_ShowPreview,
	OBJNDX_Menu_AutoPreview,
	OBJNDX_Menu_Thumbnails,
	OBJNDX_Menu_ReloadThumbnails,
	OBJNDX_Lamp_Changed,
	OBJNDX_Image_DesktopPattern,
	OBJNDX_Image_ScreenPattern,
	OBJNDX_Image_IconWindowPattern,
	OBJNDX_Image_TextWindowPattern,

	OBJNDX_CycleBackgroundType,
	OBJNDX_ColorAdjustBgColor1,
	OBJNDX_ColorAdjustBgColor2,

	OBJNDX_EmptyThumbnailBitmap,

	OBJNDX_ContextMenu,

	OBJNDX_MAX
};

struct PatternPrefsInst
{
	ULONG	ppb_fCreateIcons;
	ULONG	ppb_fPreview;
	ULONG	ppb_fAutoPreview;
	ULONG	ppb_fDontChange;

	BYTE	ppb_patternPrecision;
	ULONG	ppb_PreviewWeight;

	CONST_STRPTR	ppb_ProgramName;

	BOOL    ppb_UseSplashWindow;
	BOOL	ppb_UseThumbNails;
	BOOL 	ppb_Changed;
	ULONG	ppb_BitMapsRead;

	ULONG	ppb_ThumbnailWidth;	// Dimensions of thumbnails
	ULONG	ppb_ThumbnailHeight;

	ULONG 	ppb_ThumbnailImageNumber;

	struct 	Screen *ppb_WBScreen;

	struct	Hook ppb_Hooks[HOOKNDX_MAX];
	Object 	*ppb_Objects[OBJNDX_MAX];

	struct	MUI_NListtree_TreeNode *ppb_MainMenuNode;
	struct	MUI_NListtree_TreeNode *ppb_PopMenuNode[6];

	struct FileRequester *ppb_LoadReq;
	struct FileRequester *ppb_SaveReq;

	Object	*ppb_SubWindows[2];
};

//----------------------------------------------------------------------------

struct ScalosPattern_LocaleInfo
{
	APTR li_LocaleBase;
	APTR li_Catalog;
	struct LocaleIFace *li_ILocale;
};

#endif /* SCALOS_PATTERNPREFS_H */

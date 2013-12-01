// Hooks.h
// $Date$
// $Revision$


#ifndef HOOKS_H
#define	HOOKS_H

SAVEDS(void) INTERRUPT StartFontPrefs_Func(struct Hook *hook, Object *pPrefsWindow, Msg msg);
SAVEDS(void) INTERRUPT StartWorkbenchPrefs_Func(struct Hook *hook, Object *pPrefsWindow, Msg msg);
SAVEDS(void) INTERRUPT CheckPopTitleBarBox(struct Hook *hook, Object *pPrefsWindow, Msg msg);
SAVEDS(void) INTERRUPT ControlBarGadgetListBrowserActiveHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT ControlBarGadgetListNormalActiveHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT AddPlugin_Func(struct Hook *hook, Object *pPluginList, Msg msg);
SAVEDS(void) INTERRUPT PluginActive_Func(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT OpenHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT LastSavedFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT SaveAsFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT ResetToDefaultsFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT RestoreFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT OpenAboutFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT OpenAboutMorpOSFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT OpenAboutMUIFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT RemovePluginFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT ModulesPrefsPopupHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT DesktopHiddenDevicesHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT IconsSelectBobRoutineHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT CreateIconsHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT PageChangeHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT PluginListAppMsgHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT ScreenTtfPopOpenHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT ScreenTtfPopCloseHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT IconTtfPopOpenHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT IconTtfPopCloseHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT TextWindowTtfPopOpenHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT TextWindowTtfPopCloseHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT IconDragTransparencyHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT IconsMinSizeHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT IconsMaxSizeHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT IconFrameHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT ThumbnailFrameHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT AslIntuiMsgHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(APTR) INTERRUPT BrowserCmdSelectedHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(APTR) INTERRUPT NormalCmdSelectedHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(APTR) INTERRUPT ControlBarGadgetBrowserChangedHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(APTR) INTERRUPT ControlBarGadgetNormalChangedHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT CalculateMaxRadiusHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT UpdateSelectMarkerSampleHookFunc(struct Hook *hook, Object *o, Msg msg);
SAVEDS(void) INTERRUPT ChangeThumbnailSizeHookFunc(struct Hook *hook, Object *o, Msg msg);
#endif /* HOOKS_H */

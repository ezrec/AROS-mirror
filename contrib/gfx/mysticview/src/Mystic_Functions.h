/*********************************************************************
----------------------------------------------------------------------

	MysticView
	functions

----------------------------------------------------------------------
*********************************************************************/

#ifndef MYSTIC_FUNC_H
#define	MYSTIC_FUNC_H 1

#include	"Mystic_Global.h"
#include	"MysticView.h"
#include	"Mystic_Settings.h"
#include	"Mystic_Window.h"

extern BOOL MVFunction_CreateThumbnail(struct mvwindow *win, struct mainsettings *settings, struct viewdata *viewdata);
extern void MVFunction_About(struct mview *mv);
extern BOOL MVFunction_UsePreset(struct mview *mv, struct mainsettings **settings, struct pathsettings *pathsettings, struct viewdata *viewdata, int slot, BOOL immediately);
extern void MVFunction_CopyMoveToPath(struct mview *mv, struct mainsettings *settings, struct pathsettings *pathsettings, struct viewdata *viewdata, int slot, int action, BOOL immediately, struct MVButtons *buttons);
extern void MVFunction_SetHideMode(struct mview *mv, struct mainsettings *settings, PICTURE *picture);
extern BOOL MVFunction_SaveJPEG(struct mview *mv, struct viewdata *viewdata, struct pathsettings *settings);
extern void MVFunction_CopyMoveTo(struct mview *mv, struct mainsettings *settings, struct pathsettings *pathsettings, struct viewdata *viewdata, int action, BOOL immediately, struct MVButtons *buttons);
extern BOOL MVFunction_CreateAutoThumbnail(struct mvwindow *win, struct mainsettings *settings, struct viewdata *viewdata);

#endif



/*********************************************************************
----------------------------------------------------------------------

	MysticView
	neuromancer

	artificial intelligence routines

----------------------------------------------------------------------
*********************************************************************/

#ifndef MYSTIC_NEURO
#define MYSTIC_NEURO	1

#include "MysticView.h"
#include "Mystic_Screen.h"
#include "Mystic_Window.h"
#include "Mystic_Settings.h"
#include <guigfx/guigfx.h>

extern void MVNeuro_DeleteNet(void);
extern void MVNeuro_CreateNet(void);
extern float MVNeuro_Train(struct mvscreen *scr, struct mvwindow *win, struct mainsettings *settings, struct viewdata *viewdata, PICTURE *pic);
extern BOOL MVNeuro_RecallDisplayMode(int *displaymode, struct mvscreen *scr, struct mvwindow *win, struct mainsettings *settings, struct viewdata *viewdata, PICTURE *pic);


#endif

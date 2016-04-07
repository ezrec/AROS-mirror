/***************************************************************************

 Toolbar MCP - MUI Custom Class Preferences for Toolbar handling

 Copyright (C) 1997-2004 by Benny Kjær Nielsen <floyd@amiga.dk>
                            Darius Brewka <d.brewka@freenet.de>
                            Jens Langner <Jens.Langner@light-speed.de>

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 $Id$

***************************************************************************/

extern STRPTR GetString(APTR, struct Catalog *);

/******************************************************************************/

#define LOCALE(a,b) GetString(a, data->catalog)

extern const APTR _MSG_ABOUT;
#define MSG_ABOUT ((APTR) &_MSG_ABOUT)
extern const APTR _MSG_ADJUST;
#define MSG_ADJUST ((APTR) &_MSG_ADJUST)
extern const APTR _MSG_ASL_ERROR;
#define MSG_ASL_ERROR ((APTR) &_MSG_ASL_ERROR)
extern const APTR _MSG_COLOR;
#define MSG_COLOR ((APTR) &_MSG_COLOR)
extern const APTR _MSG_COLOR_HELP;
#define MSG_COLOR_HELP ((APTR) &_MSG_COLOR_HELP)
extern const APTR _MSG_DIMMMED;
#define MSG_DIMMMED ((APTR) &_MSG_DIMMMED)
extern const APTR _MSG_EXACT;
#define MSG_EXACT ((APTR) &_MSG_EXACT)
extern const APTR _MSG_FONT;
#define MSG_FONT ((APTR) &_MSG_FONT)
extern const APTR _MSG_GENERAL;
#define MSG_GENERAL ((APTR) &_MSG_GENERAL)
extern const APTR _MSG_GHOSTEFFECT;
#define MSG_GHOSTEFFECT ((APTR) &_MSG_GHOSTEFFECT)
extern const APTR _MSG_GROUPSPACE;
#define MSG_GROUPSPACE ((APTR) &_MSG_GROUPSPACE)
extern const APTR _MSG_GUI;
#define MSG_GUI ((APTR) &_MSG_GUI)
extern const APTR _MSG_HEAVY;
#define MSG_HEAVY ((APTR) &_MSG_HEAVY)
extern const APTR _MSG_ICON;
#define MSG_ICON ((APTR) &_MSG_ICON)
extern const APTR _MSG_IMAGE;
#define MSG_IMAGE ((APTR) &_MSG_IMAGE)
extern const APTR _MSG_IMAGEONLY;
#define MSG_IMAGEONLY ((APTR) &_MSG_IMAGEONLY)
extern const APTR _MSG_IMAGETEXT;
#define MSG_IMAGETEXT ((APTR) &_MSG_IMAGETEXT)
extern const APTR _MSG_INNERSPACE;
#define MSG_INNERSPACE ((APTR) &_MSG_INNERSPACE)
extern const APTR _MSG_INNERSPACE_WITH;
#define MSG_INNERSPACE_WITH ((APTR) &_MSG_INNERSPACE_WITH)
extern const APTR _MSG_INNERSPACE_WITHOUT;
#define MSG_INNERSPACE_WITHOUT ((APTR) &_MSG_INNERSPACE_WITHOUT)
extern const APTR _MSG_INSIDE;
#define MSG_INSIDE ((APTR) &_MSG_INSIDE)
extern const APTR _MSG_LIGHT;
#define MSG_LIGHT ((APTR) &_MSG_LIGHT)
extern const APTR _MSG_OK;
#define MSG_OK ((APTR) &_MSG_OK)
extern const APTR _MSG_OUTSIDE;
#define MSG_OUTSIDE ((APTR) &_MSG_OUTSIDE)
extern const APTR _MSG_PLACEMENT;
#define MSG_PLACEMENT ((APTR) &_MSG_PLACEMENT)
extern const APTR _MSG_REMAP;
#define MSG_REMAP ((APTR) &_MSG_REMAP)
extern const APTR _MSG_SELECT;
#define MSG_SELECT ((APTR) &_MSG_SELECT)
extern const APTR _MSG_SPACING;
#define MSG_SPACING ((APTR) &_MSG_SPACING)
extern const APTR _MSG_SUPERLIGHT;
#define MSG_SUPERLIGHT ((APTR) &_MSG_SUPERLIGHT)
extern const APTR _MSG_TEXT;
#define MSG_TEXT ((APTR) &_MSG_TEXT)
extern const APTR _MSG_TEXTONLY;
#define MSG_TEXTONLY ((APTR) &_MSG_TEXTONLY)
extern const APTR _MSG_TOOLSPACE;
#define MSG_TOOLSPACE ((APTR) &_MSG_TOOLSPACE)
extern const APTR _MSG_USEIMAGES1;
#define MSG_USEIMAGES1 ((APTR) &_MSG_USEIMAGES1)
extern const APTR _MSG_USEIMAGES2;
#define MSG_USEIMAGES2 ((APTR) &_MSG_USEIMAGES2)
extern const APTR _MSG_BORDERTYPE;
#define MSG_BORDERTYPE ((APTR) &_MSG_BORDERTYPE)
extern const APTR _MSG_BORDERTYPE_OLD;
#define MSG_BORDERTYPE_OLD ((APTR) &_MSG_BORDERTYPE_OLD)
extern const APTR _MSG_BORDERTYPE_OFF;
#define MSG_BORDERTYPE_OFF ((APTR) &_MSG_BORDERTYPE_OFF)
extern const APTR _MSG_BORDERTYPE_NEW;
#define MSG_BORDERTYPE_NEW ((APTR) &_MSG_BORDERTYPE_NEW)
extern const APTR _MSG_SELECTIONMODE;
#define MSG_SELECTIONMODE ((APTR) &_MSG_SELECTIONMODE)
extern const APTR _MSG_SELECTIONMODE_OLD;
#define MSG_SELECTIONMODE_OLD ((APTR) &_MSG_SELECTIONMODE_OLD)
extern const APTR _MSG_SELECTIONMODE_OFF;
#define MSG_SELECTIONMODE_OFF ((APTR) &_MSG_SELECTIONMODE_OFF)
extern const APTR _MSG_SELECTIONMODE_NEW;
#define MSG_SELECTIONMODE_NEW ((APTR) &_MSG_SELECTIONMODE_NEW)
extern const APTR _MSG_AUTOACTIVE;
#define MSG_AUTOACTIVE ((APTR) &_MSG_AUTOACTIVE)

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

#include <libraries/locale.h>
#include <proto/locale.h>

struct LocStr
{
    LONG   ID;
    STRPTR Str;
};

const struct LocStr _MSG_ABOUT = {0,"\033cToolbar.mcc Version %s (%s)\n%s"};
const struct LocStr _MSG_ADJUST = {1,"Adjust Text Color"};
const struct LocStr _MSG_ASL_ERROR = {2,"Cannot quit now, still\nan asl popup opened.\nPlease close it now."};
const struct LocStr _MSG_COLOR = {3,"Text Color"};
const struct LocStr _MSG_COLOR_HELP = {4,"Color of the tool texts."};
const struct LocStr _MSG_DIMMMED = {5,"Dimmed"};
const struct LocStr _MSG_EXACT = {6,"Exact"};
const struct LocStr _MSG_FONT = {7,"Text Font"};
const struct LocStr _MSG_GENERAL = {8,"General"};
const struct LocStr _MSG_GHOSTEFFECT = {9,"Ghosting effect"};
const struct LocStr _MSG_GROUPSPACE = {10,"Group Space"};
const struct LocStr _MSG_GUI = {11,"GUI"};
const struct LocStr _MSG_HEAVY = {12,"Heavy Grid"};
const struct LocStr _MSG_ICON = {13,"Icon"};
const struct LocStr _MSG_IMAGE = {14,"Image"};
const struct LocStr _MSG_IMAGEONLY = {15,"Image only"};
const struct LocStr _MSG_IMAGETEXT = {16,"Image and Text"};
const struct LocStr _MSG_INNERSPACE = {17,"\33cInner Space"};
const struct LocStr _MSG_INNERSPACE_WITH = {18,"\33c(with tooltext)"};
const struct LocStr _MSG_INNERSPACE_WITHOUT = {19,"\33c(without tooltext)"};
const struct LocStr _MSG_INSIDE = {20,"Inside"};
const struct LocStr _MSG_LIGHT = {21,"Light Grid"};
const struct LocStr _MSG_OK = {22,"OK"};
const struct LocStr _MSG_OUTSIDE = {23,"Outside"};
const struct LocStr _MSG_PLACEMENT = {24,"Placement"};
const struct LocStr _MSG_REMAP = {25,"Remap Precision"};
const struct LocStr _MSG_SELECT = {26,"Please select a text font..."};
const struct LocStr _MSG_SPACING = {27,"Spacing"};
const struct LocStr _MSG_SUPERLIGHT = {28,"Superlight Grid"};
const struct LocStr _MSG_TEXT = {29,"Text"};
const struct LocStr _MSG_TEXTONLY = {30,"Text only"};
const struct LocStr _MSG_TOOLSPACE = {31,"Tool Space"};
const struct LocStr _MSG_USEIMAGES1 = {32,"\33cUse ghosted/selected images"};
const struct LocStr _MSG_USEIMAGES2 = {33,"\33c(if they are available)"};
const struct LocStr _MSG_BORDERTYPE = {34, "Border Type"};
const struct LocStr _MSG_BORDERTYPE_OLD = {35, "Old"};
const struct LocStr _MSG_BORDERTYPE_OFF = {36, "Off"};
const struct LocStr _MSG_BORDERTYPE_NEW = {37, "New"};
const struct LocStr _MSG_SELECTIONMODE= {38, "Selection Border"};
const struct LocStr _MSG_SELECTIONMODE_OLD = {39, "Old"};
const struct LocStr _MSG_SELECTIONMODE_OFF = {40, "Off"};
const struct LocStr _MSG_SELECTIONMODE_NEW = {41, "New"};
const struct LocStr _MSG_AUTOACTIVE = {42, "Show Active on Mousehit"};

STRPTR GetString(APTR CatStr, struct Catalog *catalog)
{
    STRPTR DefStr;
    LONG StrNum;

    StrNum=((struct LocStr *)CatStr)->ID;
    DefStr=((struct LocStr *) CatStr)->Str;

    return((STRPTR)GetCatalogStr(catalog,StrNum,DefStr));
}

/*

Directory Opus 4
Original GPL release version 4.12
Copyright 1993-2000 Jonathan Potter

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

All users of Directory Opus 4 (including versions distributed
under the GPL) are entitled to upgrade to the latest version of
Directory Opus version 5 at a reduced price. Please see
http://www.gpsoft.com.au for more information.

The release of Directory Opus 4 under the GPL in NO WAY affects
the existing commercial status of Directory Opus 5.

*/

#include "config.h"
#include "dopusiff.h"

#define STRING_VERSION 1

char **cfg_string;

struct DefaultString
	default_strings[]={
		{STR_REALLY_DISCARD_CONFIG,
		"Do you really want to discard the\ncurrent configuration settings?"},

		{STR_EXIT_WITHOUT_SAVING,
		"Do you really want to exit without\nsaving the current configuration settings?"},

		{STR_OKAY_TO_QUIT,
		"Okay"},

		{STR_CONFIGURATION_MAIN_SCREEN,
		"Configuration main screen"},

		{STR_OPERATION_SCREEN,
		"Operation configuration screen"},

		{STR_MENU_SCREEN,
		"Menu configuration screen"},

		{STR_DRIVE_SCREEN,
		"Drive button configuration screen"},

		{STR_FILETYPE_SCREEN,
		"Filetype configuration screen"},

		{STR_SCREEN_SCREEN,
		"Screen configuration screen"},

		{STR_SYSTEM_SCREEN,
		"System configuration screen"},

		{STR_HOTKEYS_SCREEN,
		"Global hotkeys configuration screen"},

		{STR_MAIN_MENU,
		"Main Menu"},

		{STR_LEFT_MOUSE_BUTTON,
		"Left mouse button"},

		{STR_RIGHT_MOUSE_BUTTON,
		"Right mouse button"},

		{STR_OKAY,
		"Okay"},

		{STR_CANCEL,
		"Cancel"},

		{STR_AVAILABLE_DISPLAY_ITEMS,
		"Available display items..."},

		{STR_SELECTED_DISPLAY_ITEMS,
		"Selected display items..."},

		{STR_DISPLAYED_LENGTHS,
		"Displayed lengths..."},

		{STR_CHECK_INDICATES_SORT,
		"(  Check indicates sort item)"},

		{STR_ENTRY_SEPARATION,
		"Entry separation..."},

		{STR_UNICONIFY_HOTKEY,
		"Uniconify/activate hotkey"},

		{STR_PROJECTS_DEFAULT_TOOL,
		"Project's Default Tool..."},

		{STR_SELECT_DESIRED_FILE,
		"Select the desired file"},

		{STR_SELECT_DESIRED_DIRECTORY,
		"Select the desired directory"},

		{STR_SELECT_DESIRED_FONT,
		"Select the desired font and size"},

		{STR_NEW_GADGET_BANKS_APPENDED,
		"New button banks appended to existing ones."},

		{STR_DEFAULT_GADGET_BANKS_APPENDED,
		"Default button banks appended to existing ones."},

		{STR_NO_GADGET_BANKS_CONFIGURED,
		"There are no button banks configured!"},

		{STR_GADGET_DELETED,
		"Button deleted."},

		{STR_SELECT_GADGET_TO_COPY,
		"Select the button position to copy the original button to."},

		{STR_SELECT_GADGET_TO_SWAP,
		"Select the button to swap with the first button."},

		{STR_SELECT_A_GADGET_TO_COPY,
		"Select a button to copy."},

		{STR_SELECT_FIRST_GADGET_TO_SWAP,
		"Select the first button to swap."},

		{STR_GADGET_EDIT_SCREEN,
		"Button edit screen"},

		{STR_ENTER_BANK_TO_COPY,
		"Enter bank number to copy this bank to.\n"
		"If this bank does not exist a new bank will be created."},

		{STR_BANK_ALREADY_EXISTS,
		"Selected bank already exists! Do you really\n"
		"want to overwrite this bank?"},

		{STR_GADGET_BANK_COPIED,
		"Button bank copied."},

		{STR_ENTER_BANK_TO_SWAP,
		"Enter bank number to swap this bank with."},

		{STR_BANK_DOES_NOT_EXIST,
		"Selected button bank does not exist."},

		{STR_BANKS_SWAPPED,
		"Button banks swapped."},

		{STR_ONLY_BANK_CONFIGURED,
		"This is the only button bank configured!\n"
		"Do you really want to delete this button bank?"},

		{STR_REALLY_DELETE_BANK,
		"Do you really want to delete this button bank?"},

		{STR_BANK_DELETED,
		"Button bank deleted."},

		{STR_SELECT_GADGET_TO_DELETE,
		"Select a button to delete."},

		{STR_GADGET_BANK_NUMBER,
		"Button bank number"},

		{STR_GADGET_SCREEN_NO_BANKS,
		"Button configuration screen - no banks configured"},

		{STR_NO_MEMORY_FOR_NEW_BANK,
		"Unable to allocate memory for new bank!"},

		{STR_NEW_BANK_CREATED,
		"New button bank created."},

		{STR_NEW_MENUS_LOADED,
		"New menus loaded."},

		{STR_DEFAULT_MENUS_LOADED,
		"Default menus loaded."},

		{STR_ENTER_MENU_TITLE,
		"Enter menu title."},

		{STR_MENU_ITEM_DELETED,
		"Menu item deleted."},

		{STR_MENU_ITEM_INSERTED,
		"Menu item inserted."},

		{STR_REALLY_DELETE_THIS_MENU,
		"Do you really want to delete this menu?"},

		{STR_MENU_DELETED,
		"Menu deleted."},

		{STR_MENU_SORTED,
		"Menu sorted."},

		{STR_SELECT_MENU_TO_COPY,
		"Select an item in the menu to the copy original menu to."},

		{STR_SELECT_MENU_TO_SWAP,
		"Select an item in the menu to swap with the first."},

		{STR_SELECT_ITEM_TO_COPY,
		"Select item position to copy the original menu item to."},

		{STR_SELECT_ITEM_TO_SWAP,
		"Select menu item to swap with the first item."},

		{STR_MENU_ITEM_EDIT_SCREEN,
		"Menu item edit screen"},

		{STR_SELECT_ITEM_IN_MENU_TO_COPY,
		"Select an item in the menu to copy."},

		{STR_SELECT_ITEM_IN_MENU_TO_SWAP,
		"Select an item in the first menu to swap."},

		{STR_SELECT_ITEM_IN_MENU_TO_DELETE,
		"Select an item in the menu to delete."},

		{STR_SELECT_ITEM_IN_MENU_TO_SORT,
		"Select an item in the menu to sort."},

		{STR_SELECT_ITEM_TO_INSERT_BEFORE,
		"Select the menu item to insert the new item BEFORE."},

		{STR_SELECT_MENU_ITEM_TO_COPY,
		"Select a menu item to copy."},

		{STR_SELECT_MENU_ITEM_TO_SWAP,
		"Select the first menu item to swap."},

		{STR_SELECT_MENU_ITEM_TO_DELETE,
		"Select a menu item to delete."},

		{STR_EDIT,
		"Edit"},

		{STR_EDIT_MENU,
		"Edit"},

		{STR_SHORTCUT_KEY,
		"Shortcut _key"},

		{STR_HOTKEY,
		"Hotkey"},

		{STR_HOTKEY_USCORE,
		"Hot_key"},

		{STR_ARGUMENT_CONTROL_SEQUENCES,
		"List of available functions..."},

		{STR_AVAILABLE_COMMANDS,
		"List of available commands..."},

		{STR_FOREGROUND,
		"Foreground"},

		{STR_BACKGROUND,
		"Background"},

		{STR_DROP_A_TOOL_HERE,
		"Drop a tool here!"},

		{STR_NEW_DRIVE_BANKS_LOADED,
		"New drive banks loaded."},

		{STR_DEFAULT_DRIVE_BANKS_RESET,
		"Default drive banks reset."},

		{STR_DRIVE_GADGET_DELETED,
		"Drive button deleted."},

		{STR_SELECT_DRIVE_TO_COPY_TO,
		"Select the drive position to copy the original drive button to."},

		{STR_SELECT_DRIVE_TO_SWAP_WITH_FIRST,
		"Select the drive button to swap with the first button."},

		{STR_SELECT_DRIVE_BANK_TO_COPY_TO,
		"Select a drive button in the bank to copy the original bank to."},

		{STR_SELECT_DRIVE_BANK_TO_SWAP_WITH_FIRST,
		"Select a drive button in the bank to swap with the first bank."},

		{STR_REALLY_DELETE_DRIVE_BANK,
		"Do you really want to delete this drive bank?"},

		{STR_PATH,
		"_Path"},

		{STR_SYSTEM_DEVICES_LISTED,
		"System devices listed."},

		{STR_SELECT_DRIVE_BANK_TO_COPY,
		"Select a drive button in the bank to copy."},

		{STR_SELECT_DRIVE_BANK_TO_SWAP,
		"Select a drive button in the first bank to swap."},

		{STR_SELECT_DRIVE_BANK_TO_DELETE,
		"Select a drive button in the bank to delete."},

		{STR_SELECT_DRIVE_BANK_TO_SORT,
		"Select a drive button in the bank to sort."},

		{STR_SELECT_DRIVE_GADGET_TO_COPY,
		"Select a drive button to copy."},

		{STR_SELECT_DRIVE_GADGET_TO_SWAP,
		"Select the first drive button to swap."},

		{STR_SELECT_DRIVE_GADGET_TO_DELETE,
		"Select a drive button to delete."},

		{STR_NEW_FILETYPES_MERGED,
		"New filetypes merged with existing ones."},

		{STR_DEFAULT_FILETYPES_MERGED,
		"Default filetypes merged with existing ones."},

		{STR_REALLY_DELETE_THIS_CLASS,
		"Do you really want to delete this class?"},

		{STR_FILE_CLASS_DELETED,
		"File class deleted."},

		{STR_SELECT_CLASS_FILE_TO_LOAD,
		"Select the class file to load"},

		{STR_ENTER_NAME_TO_SAVE_CLASSES,
		"Enter a name to save the file classes"},

		{STR_SELECT_CLASSES_TO_IMPORT,
		"Select file classes to import"},

		{STR_ERASE_ALL_CLASSES,
		"Do you really wish to erase all file classes?"},

		{STR_EXIT_WITHOUT_SAVING_CLASSES,
		"Do you really wish to exit this section without\nsaving changes made to the file classes?"},

		{STR_REDEFINE_EXISTING_CLASS_ACTION,
		"That file class already has an action defined\nfor it. Do you wish to continue?"},

		{STR_DEFAULT_ACTION_CAN_NOT_BE_MOVED,
		"The default action can not be moved."},

		{STR_CLASS_ACTIONS_SWAPPED,
		"Class actions swapped."},

		{STR_SELECT_ACTION_TO_SWAP,
		"Select the first action to swap."},

		{STR_SELECT_ACTION_TO_SWAP_WITH_FIRST,
		"Select the second action to swap with the first."},

		{STR_SELECT_ACTION_TO_DELETE,
		"Select an action to delete."},

		{STR_REALLY_DELETE_THIS_ACTION,
		"Do you really want to delete this action?"},

		{STR_ACTION_DELETED,
		"Action deleted."},

		{STR_FILETYPE_CLASS,
		"Filetype class"},

		{STR_EDIT_CLASS,
		"Edit class"},

		{STR_FILE_CLASS_EDIT_SCREEN,
		"File class edit screen"},

		{STR_OPEN_FAILED,
		"Open failed - DOS error code %ld\nTry again?"},

		{STR_SAVE_FAILED,
		"Save failed - DOS error code %ld\nTry again?"},

		{STR_LIST_OF_FILETYPE_COMMANDS,
		"List of file identification commands..."},

		{STR_POSITION,
		"Position"},

		{STR_OFFSET,
		"Offset"},

		{STR_FILE_NOT_FOUND,
		"File not found!"},

		{STR_EXAMPLE,
		"Example"},

		{STR_DISPLAY_MODE_DESCRIPTION,
		"Display mode description..."},

		{STR_DISPLAY_ITEM,
		"Display item..."},

		{STR_FONT,
		"Font..."},

		{STR_FONT_SIZE,
		"Size..."},

		{STR_MODIFY_SIZE,
		"Adjust size..."},

		{STR_USE_MMB,
		"Use middle mouse button"},

		{STR_MINIMUM_SIZE,
		"Minimum size"},

		{STR_MAXIMUM_SIZE,
		"Maximum size"},

		{STR_DEFAULT_SIZE,
		"Default size"},

		{STR_MAXIMUM_COLORS,
		"Maximum colors"},

		{STR_BY,
		"by"},

		{STR_LOADING_FONT,
		"Loading font..."},

		{STR_FONT_COULD_NOT_BE_LOADED,
		"Font could not be loaded!"},

		{STR_ENTER_CONFIGURATION_FILENAME,
		"Enter configuration filename"},

		{STR_SELECT_CONFIGURATION_TO_LOAD,
		"Select configuration file to load"},

		{STR_FILE_NOT_VALID_CONFIGURATION,
		"File is not a valid configuration file!\nTry again?"},

		{STR_SELECT_MENUS_TO_IMPORT,
		"Select menus to import"},

		{STR_SELECT_FILETYPES_TO_IMPORT,
		"Select filetypes to import"},

		{STR_SELECT_HOTKEYS_TO_IMPORT,
		"Select hotkeys to import"},

		{STR_SELECT_FUNCTION_TO_PASTE,
		"Select function to paste"},

		{STR_NEW_HOTKEYS_MERGED,
		"New hotkeys merged with existing ones."},

		{STR_DEFAULT_HOTKEYS_MERGED,
		"Default hotkeys merged with existing ones."},

		{STR_SELECT_HOTKEY_TO_DELETE,
		"Select hotkey to delete."},

		{STR_SELECT_HOTKEY_TO_DUPLICATE,
		"Select hotkey to duplicate."},

		{STR_REALLY_DELETE_HOTKEY,
		"Do you really want to delete this hotkey?"},

		{STR_HOTKEY_DELETED,
		"Hotkey deleted."},

		{STR_HOTKEY_EDIT_SCREEN,
		"Hotkey edit screen"},

		{STR_NEW_BANK,
		"New bank"},

		{STR_INSERT_BANK,
		"Insert bank"},

		{STR_NEXT_BANK,
		"Next bank"},

		{STR_LAST_BANK,
		"Prev bank"},

		{STR_CLEAR,
		"Clear"},

		{STR_CLEAR_LAST,
		"Clear last"},

		{STR_MENU_DEFAULT,
		"Default"},

		{STR_MENU_OPEN,
		"Open..."},

		{STR_MENU_SAVE,
		"Save"},

		{STR_MENU_SAVE_AS,
		"Save As..."},

		{STR_MENU_CUT,
		"Cut"},

		{STR_MENU_COPY,
		"Copy"},

		{STR_MENU_PASTE,
		"Paste..."},

		{STR_MENU_ERASE,
		"Erase"},

		{STR_MENU_NEW,
		"New..."},

		{STR_MENU_EDIT,
		"Edit..."},

		{STR_MENU_DUPLICATE,
		"Duplicate..."},

		{STR_MENU_DELETE,
		"Delete..."},

		{STR_GADGETROWS_NONE,
		"No buttons"},

		{STR_GADGETROWS_ONE,
		"One row"},

		{STR_GADGETROWS_TWO,
		"Two rows"},

		{STR_GADGETROWS_THREE,
		"Three rows"},

		{STR_GADGETROWS_SIX,
		"Six rows"},

		{STR_MENUNAME_CONFIGURE,
		"Configure"},

		{STR_MENUNAME_GADGETROWS,
		"Button rows"},

		{STR_MENUNAME_CLASSES,
		"Classes"},

		{STR_MENUNAME_NEATSTUFF,
		"Tools"},

		{STR_FILECLASS_DEFINE,
		"Define an action for class..."},

		{STR_FILECLASS_EDIT,
		"Select class to edit..."},

		{STR_FILECLASS_DELETE,
		"Select class to delete..."},

		{STR_FILECLASS_DUPLICATE,
		"Select class to duplicate..."},

		{STR_FONTPLACE_CLOCK,
		"Clock/Memory"},

		{STR_FONTPLACE_GADGETS,
		"Custom buttons"},

		{STR_FONTPLACE_MENUS,
		"Custom menus"},

		{STR_FONTPLACE_DISKNAMES,
		"Disk names"},

		{STR_FONTPLACE_WINDOW,
		"Directory window"},

		{STR_FONTPLACE_GENERAL,
		"General (8 point)"},

		{STR_FONTPLACE_ICONIFY,
		"Iconify to buttons"},

		{STR_FONTPLACE_PATHS,
		"Path names"},

		{STR_FONTPLACE_REQUESTERS,
		"Requesters"},

		{STR_FONTPLACE_SCREEN,
		"Screen font"},

		{STR_FONTPLACE_STATUS,
		"Status bar"},

		{STR_FONTPLACE_TEXTVIEWER,
		"Text viewer"},

		{STR_PALETTE_PRESETS,
		"Presets..."},

		{STR_ICONLIST_TITLE,
		"AddIcon's icons..."},

		{STR_FORMAT_NAME,
		"File name"},

		{STR_FORMAT_SIZE,
		"File size"},

		{STR_FORMAT_BITS,
		"Protection bits"},

		{STR_FORMAT_DATE,
		"Creation date"},

		{STR_FORMAT_COMMENT,
		"File comment"},

		{STR_FORMAT_TYPE,
		"File type"},

		{STR_FORMAT_OWNER,
		"Owner"},

		{STR_FORMAT_GROUP,
		"Group"},

		{STR_FORMAT_GROUPBITS,
		"Net protection bits"},

		{STR_ICONS_DRAWER,
		"   Drawer icon"},

		{STR_ICONS_TOOL,
		"   Default Tool icon"},

		{STR_ICONS_PROJECT,
		"   Default Project icon"},

		{STR_MAINMENU_GADGETS,
		"Buttons"},

		{STR_MAINMENU_DRIVES,
		"Drives"},

		{STR_MAINMENU_FILETYPES,
		"Filetypes"},

		{STR_MAINMENU_HOTKEYS,
		"Hotkeys"},

		{STR_MAINMENU_MENUS,
		"Menus"},

		{STR_MAINMENU_OPERATION,
		"Operation"},

		{STR_MAINMENU_SCREEN,
		"Screen"},

		{STR_MAINMENU_SYSTEM,
		"System"},

		{STR_ALL,
		"All"},

		{STR_OPERATION_DATEFORMAT,
		"Date format"},

		{STR_OPERATION_DELETE,
		"Delete"},

		{STR_OPERATION_ERRORCHECK,
		"Error check"},

		{STR_OPERATION_GENERAL,
		"General"},

		{STR_OPERATION_ICONS,
		"Icons"},

		{STR_OPERATION_LISTFORMAT,
		"List format"},

		{STR_OPERATION_UPDATE,
		"Update"},

		{STR_SYSTEM_AMIGADOS,
		"AmigaDOS"},

		{STR_SYSTEM_CLOCKS,
		"Clocks"},

		{STR_SYSTEM_DIRECTORIES,
		"Directories"},

		{STR_SYSTEM_ICONS,
		"Icons"},

		{STR_SYSTEM_MODULES,
		"Modules"},

		{STR_SYSTEM_SHOWPATTERN,
		"Show pattern"},

		{STR_SYSTEM_STARTUP,
		"Startup"},

		{STR_SYSTEM_VIEWPLAY,
		"View & Play"},

		{STR_GADGET_COPYBANK,
		"Copy bank"},

		{STR_GADGET_SWAPBANK,
		"Swap bank"},

		{STR_GADGET_DELETEBANK,
		"Delete bank"},

		{STR_GADGET_COPYGADGET,
		"Copy button"},

		{STR_GADGET_SWAPGADGET,
		"Swap button"},

		{STR_GADGET_DELETEGADGET,
		"Del. button"},

		{STR_FLAG_AUTOICONIFY,
		"Auto iconify"},

		{STR_FLAG_CDDESTINATION,
		"CD destination"},

		{STR_FLAG_CDSOURCE,
		"CD source"},

		{STR_FLAG_DOPUSTOFRONT,
		"Directory Opus to front"},

		{STR_FLAG_DOALLFILES,
		"Do all files"},

		{STR_FLAG_INCLUDEDOPUS,
		"Include DOpus-Startup"},

		{STR_FLAG_INCLUDESHELL,
		"Include Shell-Startup"},

		{STR_FLAG_NOQUOTE,
		"No filename quote"},

		{STR_FLAG_OUTPUTFILE,
		"Output to file"},

		{STR_FLAG_OUTPUTWINDOW,
		"Output window"},

		{STR_FLAG_RECURSIVE,
		"Recursive directories"},

		{STR_FLAG_RELOAD,
		"Reload each file"},

		{STR_FLAG_RESCANDEST,
		"Rescan destination"},

		{STR_FLAG_RESCANSOURCE,
		"Rescan source"},

		{STR_FLAG_RUNASYNC,
		"Run asynchronously"},

		{STR_FLAG_WORKBENCHTOFRONT,
		"Workbench to front"},

		{STR_EDIT_NAME,
		"_Name"},

		{STR_EDIT_SAMPLE,
		"Sample"},

		{STR_EDIT_NEWENTRY,
		"New entry"},

		{STR_EDIT_DUPLICATE,
		"Duplicate"},

		{STR_EDIT_SWAP,
		"Swap"},

		{STR_EDIT_STACKSIZE,
		"_Stack size"},

		{STR_EDIT_PRIORITY,
		"_Priority"},

		{STR_EDIT_CLOSEDELAY,
		"_Close delay"},

		{STR_EDIT_ACTION,
		"_Event description"},

		{STR_EDITCLASS_FILECLASS,
		"_File class"},

		{STR_EDITCLASS_CLASSID,
		"Class _ID"},

		{STR_EDITCLASS_FILEVIEWER,
		"File _viewer"},

		{STR_MENU_COPYMENU,
		"Copy menu"},

		{STR_MENU_SWAPMENU,
		"Swap menu"},

		{STR_MENU_DELETEMENU,
		"Delete menu"},

		{STR_MENU_SORTMENU,
		"Sort menu"},

		{STR_MENU_INSERTITEM,
		"Insert item"},

		{STR_MENU_COPYITEM,
		"Copy item"},

		{STR_MENU_SWAPITEM,
		"Swap item"},

		{STR_MENU_DELETEITEM,
		"Delete item"},

		{STR_DRIVES_GETDRIVES,
		"Get drives"},

		{STR_DRIVES_SORTBANK,
		"Sort bank"},

		{STR_DRIVES_COPYDRIVE,
		"Copy drive"},

		{STR_DRIVES_SWAPDRIVE,
		"Swap drive"},

		{STR_DRIVES_DELETEDRIVE,
		"Delete drive"},

		{STR_HOTKEYS_NEWHOTKEY,
		"New hotkey"},

		{STR_SCREEN_COLORS,
		"Colors"},

		{STR_SCREEN_FONTS,
		"Fonts"},

		{STR_SCREEN_GENERAL,
		"General"},

		{STR_SCREEN_PALETTE,
		"Palette"},

		{STR_SCREEN_SCREENMODE,
		"Screen mode"},

		{STR_SCREEN_SLIDERS,
		"Sliders"},

		{STR_PALETTE_RED,
		"_Red"},

		{STR_PALETTE_GREEN,
		"_Green"},

		{STR_PALETTE_BLUE,
		"_Blue"},

		{STR_COLOR_STATUS,
		"Status bar"},

		{STR_COLOR_SELDISK,
		"Selected disk name"},

		{STR_COLOR_UNSELDISK,
		"Unselected disk name"},

		{STR_COLOR_SELDIR,
		"Selected directory"},

		{STR_COLOR_UNSELDIR,
		"Unselected directory"},

		{STR_COLOR_SELFILE,
		"Selected file"},

		{STR_COLOR_UNSELFILE,
		"Unselected file"},

		{STR_COLOR_SLIDERS,
		"Sliders"},

		{STR_COLOR_ARROWS,
		"Arrows"},

		{STR_COLOR_TINY,
		"Tiny buttons"},

		{STR_COLOR_CLOCK,
		"Clock/memory bar"},

		{STR_COLOR_REQUESTER,
		"Requester text"},

		{STR_COLOR_BOXES,
		"3D boxes"},

		{STR_COLOR_PATHNAME,
		"Path name"},

		{STR_COLOR_SELPATHNAME,
		"Selected path name"},

		{STR_SCREENMODE_WIDTH,
		"_Width"},

		{STR_SCREENMODE_HEIGHT,
		"_Height"},

		{STR_SCREENMODE_COLORS,
		"_Colors"},

		{STR_SCREENMODE_DEFAULT,
		"Use default"},

		{STR_SCREENMODE_HALFHEIGHT,
		"Half-height screen"},

		{STR_LISTFORMAT_TITLE,
		"Format of the list in the"},

		{STR_LISTFORMAT_RESET,
		"Reset"},

		{STR_LISTFORMAT_NAME,
		"File name"},

		{STR_LISTFORMAT_COMMENT,
		"File comment"},

		{STR_LISTFORMAT_REVERSE,
		"Reverse sorting"},

		{STR_ARROWS_INSIDE,
		"Arrows on the inside"},

		{STR_ARROWS_OUTSIDE,
		"Arrows on the outside"},

		{STR_ARROWS_EACH,
		"One arrow at each end of the button"},

		{STR_ARROWTYPE_UPDOWN,
		"Scroll up/down"},

		{STR_ARROWTYPE_LEFTRIGHT,
		"Scroll left/right"},

		{STR_ARROWTYPE_PREVNEXT,
		"Buffer previous/next"},

		{STR_NEW,
		"New"},

		{STR_MISC_FLAGS,
		"Misc. flags..."},

		{STR_OP_COPY_WHENCOPYING,
		"When copying files and directories..."},

		{STR_OP_COPY_CHECKDEST,
		"Check destination's free space before starting"},

		{STR_OP_COPY_SETARCHIVE,
		"Set archive bit of source after finishing"},

		{STR_OP_COPY_ALSOCOPY,
		"Also copy source's..."},

		{STR_OP_COPY_DATESTAMP,
		"datestamp"},

		{STR_OP_COPY_BITS,
		"protection bits"},

		{STR_OP_COPY_COMMENT,
		"comment"},

		{STR_OP_COPY_IFFILEEXISTS,
		"What to do when a file already exists..."},

		{STR_OP_COPY_ALWAYSREPLACE,
		"Always replace files"},

		{STR_OP_COPY_NEVERREPLACE,
		"Never replace files"},

		{STR_OP_COPY_ONLYOLDER,
		"Replace only older files"},

		{STR_OP_COPY_ASKBEFORE,
		"Ask before replacing"},

		{STR_OP_DATE_DATEFORMAT,
		"Date format..."},

		{STR_OP_DATE_DDMMMYY,
		"DD-MMM-YY   (22-Sep-92)"},

		{STR_OP_DATE_YYMMDD,
		"YY-MM-DD    (92-09-22)"},

		{STR_OP_DATE_MMDDYY,
		"MM-DD-YY    (09-22-92)"},

		{STR_OP_DATE_DDMMYY,
		"DD-MM-YY    (22-09-92)"},

		{STR_OP_DATE_NAMESUB,
		"Name substitution (Today, Tomorrow, etc)"},

		{STR_OP_DATE_12HOUR,
		"12 hour clock"},

		{STR_OP_DEL_ASKBEFORE,
		"Ask before..."},

		{STR_OP_DEL_COMMENCING,
		"Commencing delete"},

		{STR_OP_DEL_DELETEFILES,
		"Deleting files"},

		{STR_OP_DEL_DELETEDIRS,
		"Deleting non-empty directories"},

		{STR_OP_DEL_IGNOREPROT,
		"Ignore the delete protection bit"},

		{STR_OP_ERROR_ENABLE,
		"Enable..."},

		{STR_OP_ERROR_DOSREQ,
		"DOS requesters"},

		{STR_OP_ERROR_OPUSREQ,
		"Opus error requesters"},

		{STR_OP_GENERAL_DRAG,
		"Click-M-Click drag"},

		{STR_OP_GENERAL_DISPLAYINFO,
		"Display info"},

		{STR_OP_GENERAL_DOUBLECLICK,
		"File double-click"},

		{STR_OP_GENERAL_SLIDERACTIVE,
		"Window slider activate"},

		{STR_OP_ICON_CREATEWITHDIR,
		"Create icon with directories"},

		{STR_OP_ICON_DOUNTOICONS,
		"Perform all actions on icons as well"},

		{STR_OP_ICON_SELECTAUTO,
		"Select icons automatically"},

		{STR_OP_UPDATE_PROGRESS,
		"Progress indicator..."},

		{STR_OP_UPDATE_PROGRESSIND,
		"Operation progress indicator"},

		{STR_OP_UPDATE_PROGRESSIND_COPY,
		"Current file progress indicator (Copy)"},

		{STR_OP_UPDATE_WHENPROCESSING,
		"When processing files..."},

		{STR_OP_UPDATE_LEFTJUSTIFY,
		"Left-justify filename in status bar"},

		{STR_OP_UPDATE_SCROLLTOFOLLOW,
		"Scroll directory window to follow operations"},

		{STR_OP_UPDATE_UPDATEFREE,
		"Update free disk space"},

		{STR_OP_UPDATE_STARTNOTIFY,
		"Directory refresh using StartNotify()"},

		{STR_OP_UPDATE_REDRAWMORE,
		"Redraw more than a quarter of a page"},

		{STR_SYS_AMIGADOS_TITLE,
		"Running AmigaDOS applications..."},

		{STR_SYS_AMIGADOS_SHELL,
		"Shell"},

		{STR_SYS_AMIGADOS_CONSOLE,
		"Console"},

		{STR_SYS_AMIGADOS_STARTUP,
		"Startup script"},

		{STR_SYS_AMIGADOS_PRIORITY,
		"Priority"},

		{STR_SYS_CLOCK_TITLE,
		"Clock/memory bar..."},

		{STR_SYS_CLOCK_MEMORY,
		"Memory monitor"},

		{STR_SYS_CLOCK_CPUMONITOR,
		"CPU monitor"},

		{STR_SYS_CLOCK_DATE,
		"Date"},

		{STR_SYS_CLOCK_TIME,
		"Time"},

		{STR_SYS_CLOCK_WHENICONIFIED,
		"When iconified..."},

		{STR_SYS_CLOCK_WINDOW,
		"Window"},

		{STR_SYS_CLOCK_NOWINDOW,
		"No window"},

		{STR_SYS_CLOCK_APPICON,
		"An AppIcon"},

		{STR_SYS_CLOCK_SHOWFREEAS,
		"Show free space as..."},

		{STR_SYS_CLOCK_BYTES,
		"Bytes free"},

		{STR_SYS_CLOCK_KILOBYTES,
		"Kilo/Megabytes free"},

		{STR_SYS_CLOCK_TEXTFORMAT,
		"Text format..."},

		{STR_SYS_CLOCK_CHIPANDFAST,
		"CHIP: and FAST:"},

		{STR_SYS_CLOCK_CANDF,
		"C: and F:"},

		{STR_SYS_DIR_CACHES,
		"Directory caches..."},

		{STR_SYS_DIR_NUMBUFFERS,
		"Buffers per window"},

		{STR_SYS_DIR_ALWAYSEMPTY,
		"Always move to an empty buffer"},

		{STR_SYS_DIR_REREADINCOMPLETE,
		"Re-read changed buffers"},

		{STR_SYS_DIR_SEARCHBUFFERS,
		"Search buffers on..."},

		{STR_SYS_DIR_SEARCHPARENT,
		"Parent/Root operation"},

		{STR_SYS_DIR_DIRREAD,
		"Double-click/Click-m-click directory read"},

		{STR_SYS_DIR_AUTODISKCHANGE,
		"Auto diskchange"},

		{STR_SYS_DIR_AUTODISKLOAD,
		"Auto disk load"},

		{STR_SYS_DIR_EXPANDPATHS,
		"Expand pathnames"},

		{STR_SYS_DIR_USEEXALL,
		"Use ExAll()"},

		{STR_SYS_DIR_BLOCKSFREE,
		"Blocks free"},

		{STR_SYS_DIR_PERCENTAGE,
		"Percentage of space free"},

		{STR_SYS_SHOWPATTERN_TITLE,
		"Directory list pattern..."},

		{STR_SYS_SHOWPATTERN_HIDDENBIT,
		"Hide files with hidden bit set"},

		{STR_SYS_SHOWPATTERN_SHOW,
		"Show"},

		{STR_SYS_SHOWPATTERN_HIDE,
		"Hide"},

		{STR_SYS_STARTUP_TITLE,
		"Directories to read on startup..."},

		{STR_SYS_STARTUP_LEFT,
		"Left"},

		{STR_SYS_STARTUP_RIGHT,
		"Right"},

		{STR_SYS_STARTUP_AREXX,
		"ARexx scripts to execute on..."},

		{STR_SYS_STARTUP_STARTUP,
		"Startup"},

		{STR_SYS_STARTUP_UNICONIFY,
		"Uniconify"},

		{STR_SYS_VIEWPLAY_TITLE,
		"Picture/animation viewer (IFF ILBM/ANIM)..."},

		{STR_SYS_VIEWPLAY_BLACK,
		"Black background between pictures"},

		{STR_SYS_VIEWPLAY_PAUSED,
		"Start animation paused"},

		{STR_SYS_VIEWPLAY_8BITCOL,
		"8-bits per gun color"},

		{STR_SYS_VIEWPLAY_BESTMODEID,
		"Pick best screen-mode"},

		{STR_SYS_VIEWPLAY_SHOWDELAY,
		"Show delay"},

		{STR_SYS_VIEWPLAY_FADEDELAY,
		"Fade delay"},

		{STR_SYS_VIEWPLAY_SOUNDPLAYER,
		"Sound player (IFF 8SVX)..."},

		{STR_SYS_VIEWPLAY_FILTER,
		"Turn audio filter off for play"},

		{STR_SYS_VIEWPLAY_LOOP,
		"Loop on double-click"},

		{STR_SYS_VIEWPLAY_TEXTVIEW,
		"Text viewer..."},

		{STR_SYS_VIEWPLAY_BORDERS,
		"Text viewer borders"},

		{STR_SYS_VIEWPLAY_TABSIZE,
		"Tab size"},

		{STR_SCREEN_GENERAL_TINYGADS,
		"Bottom-right tiny buttons"},

		{STR_SCREEN_GENERAL_GADGETSLIDERS,
		"Button banks sliders"},

		{STR_SCREEN_GENERAL_WINDOWBORDERS,
		"Custom screen window borders"},

		{STR_SCREEN_GENERAL_TITLEBARSTATUS,
		"Display status text in title bar"},

		{STR_SCREEN_GENERAL_DRAGREQUESTERS,
		"Draggable requesters"},

		{STR_SCREEN_GENERAL_INDICATERMB,
		"Indicate button has RMB function"},

		{STR_SCREEN_GENERAL_NEWLOOKMENUS,
		"New-look menus"},

		{STR_SCREEN_GENERAL_NEWLOOKSLIDERS,
		"New-look sliders"},

		{STR_EDITLIST_TITLE1,
		"Flags..."},

		{STR_EDITLIST_TITLE2,
		"Events..."},

		{STR_FILETYPEACTIONLIST_TITLE,
		"Defined filetype actions..."},

		{STR_HOTKEYSLIST_TITLE,
		"Global hotkeys defined..."},

		{STR_SCREENMODELIST_TITLE,
		"Display mode..."},

		{STR_LEFT_WINDOW,
		"Left window"},

		{STR_RIGHT_WINDOW,
		"Right window"},

		{STR_SEP_MIX_FILES,
		"Mix files/directories"},

		{STR_SEP_DIRS_FIRST,
		"Directories first"},

		{STR_SEP_FILES_FIRST,
		"Files first"},

		{STR_FUNCTIONLIST_ABOUT,
		"About         -      Display program info"},

		{STR_FUNCTIONLIST_ADDICON,
		"AddIcon       -      Add icons to entries"},

		{STR_FUNCTIONLIST_ALARM,
		"Alarm         -            Sound an alarm"},

		{STR_FUNCTIONLIST_ALL,
		"All           -        Select all entries"},

		{STR_FUNCTIONLIST_ANSIREAD,
		"AnsiRead      -          ANSI text viewer"},

		{STR_FUNCTIONLIST_AREXX,
		"ARexx         -      Run an ARexx command"},

		{STR_FUNCTIONLIST_ASSIGN,
		"Assign        -    Assign logical devices"},

		{STR_FUNCTIONLIST_BEEP,
		"Beep          -              Sound a beep"},

		{STR_FUNCTIONLIST_BUFFERLIST,
		"BufferList    - List buffered directories"},

		{STR_FUNCTIONLIST_BUTTONICONIFY,
		"ButtonIconify -   Iconify to button strip"},

		{STR_FUNCTIONLIST_CD,
		"CD            -  Change current directory"},

		{STR_FUNCTIONLIST_CHECKFIT,
		"CheckFit      - Check if entries will fit"},

		{STR_FUNCTIONLIST_CLEARBUFFERS,
		"ClearBuffers  -       Clear buffered dirs"},

		{STR_FUNCTIONLIST_CLEARSIZES,
		"ClearSizes    -    Remove dir byte counts"},

		{STR_FUNCTIONLIST_CLEARWIN,
		"ClearWin      -  Clear the current window"},

		{STR_FUNCTIONLIST_CLONE,
		"Clone         -    Clone selected entries"},

		{STR_FUNCTIONLIST_COMMENT,
		"Comment       - Give filenotes to entries"},

		{STR_FUNCTIONLIST_CONFIGURE,
		"Configure     -      Launch config editor"},

		{STR_FUNCTIONLIST_CONTST,
		"ContST        -  Restart paused ST module"},

		{STR_FUNCTIONLIST_COPY,
		"Copy          -     Copy selected entries"},

		{STR_FUNCTIONLIST_COPYAS,
		"CopyAs        -       Copy with new names"},

		{STR_FUNCTIONLIST_COPYWINDOW,
		"CopyWindow    -  Copy one window to other"},

		{STR_FUNCTIONLIST_DATESTAMP,
		"DateStamp     -    Modify file datestamps"},

		{STR_FUNCTIONLIST_DEFAULTS,
		"Defaults      - Set default configuration"},

		{STR_FUNCTIONLIST_DELETE,
		"Delete        -   Delete selected entries"},

		{STR_FUNCTIONLIST_DIRTREE,
		"DirTree       -     Create directory tree"},

		{STR_FUNCTIONLIST_DISKCOPY,
		"Diskcopy      -         Copy floppy disks"},

		{STR_FUNCTIONLIST_DISKCOPYBG,
		"DiskcopyBG    -    Diskcopy in background"},

		{STR_FUNCTIONLIST_DISKINFO,
		"DiskInfo      -    Display info on a disk"},

		{STR_FUNCTIONLIST_DISPLAYDIR,
		"DisplayDir    -  Update directory display"},

		{STR_FUNCTIONLIST_DOPUSTOBACK,
		"DopusToBack   -   Move screen to the back"},

		{STR_FUNCTIONLIST_DOPUSTOFRONT,
		"DopusToFront  -  Move screen to the front"},

		{STR_FUNCTIONLIST_ENCRYPT,
		"Encrypt       -    Encrypt selected files"},

		{STR_FUNCTIONLIST_ERRORHELP,
		"ErrorHelp     -      Help with DOS errors"},

		{STR_FUNCTIONLIST_EXECUTE,
		"Execute       -       Execute batch files"},

		{STR_FUNCTIONLIST_ENDFUNCTION,
		"FinishSection -     Finish above commands"},

		{STR_FUNCTIONLIST_FORMAT,
		"Format        -       Format floppy disks"},

		{STR_FUNCTIONLIST_FORMATBG,
		"FormatBG      -      Format in background"},

		{STR_FUNCTIONLIST_GETDEVICES,
		"GetDevices    -       Display device list"},

		{STR_FUNCTIONLIST_GETSIZES,
		"GetSizes      - Calculate total dir sizes"},

		{STR_FUNCTIONLIST_HELP,
		"Help          -  Turn help mode on or off"},

		{STR_FUNCTIONLIST_HEXREAD,
		"HexRead       -   Hexadecimal text viewer"},

		{STR_FUNCTIONLIST_HUNT,
		"Hunt          -      Search for filenames"},

		{STR_FUNCTIONLIST_ICONIFY,
		"Iconify       -           Iconify program"},

		{STR_FUNCTIONLIST_ICONINFO,
		"IconInfo      - Give information on icons"},

		{STR_FUNCTIONLIST_INSTALL,
		"Install       -       Make disks bootable"},

		{STR_FUNCTIONLIST_INSTALLBG,
		"InstallBG     -     Install in background"},

		{STR_FUNCTIONLIST_LASTSAVED,
		"LastSaved     -    Read last-saved config"},

		{STR_FUNCTIONLIST_LOADCONFIG,
		"LoadConfig    -     Load nominated config"},

		{STR_FUNCTIONLIST_LOADSTRINGS,
		"LoadStrings   -    Load a new string file"},

		{STR_FUNCTIONLIST_LOOPPLAY,
		"LoopPlay      -     Loop-play sound files"},

		{STR_FUNCTIONLIST_MAKEDIR,
		"Makedir       -    Create a new directory"},

		{STR_FUNCTIONLIST_MODIFY,
		"Modify        -      Change configuration"},

		{STR_FUNCTIONLIST_MOVE,
		"Move          -     Move selected entries"},

		{STR_FUNCTIONLIST_MOVEAS,
		"MoveAs        -       Move with new names"},

		{STR_FUNCTIONLIST_NEWCLI,
		"NewCLI        -     Open a new CLI window"},

		{STR_FUNCTIONLIST_NONE,
		"None          -      Deselect all entries"},

		{STR_FUNCTIONLIST_NOTIFY,
		"Notify        -    Notification requester"},

		{STR_FUNCTIONLIST_OTHERWINDOW,
		"OtherWindow   -     Activate other window"},

		{STR_FUNCTIONLIST_PARENT,
		"Parent        -     Read parent directory"},

		{STR_FUNCTIONLIST_PARENTLIST,
		"ParentList    -   List parent directories"},

		{STR_FUNCTIONLIST_PLAY,
		"Play          - Play selected sound files"},

		{STR_FUNCTIONLIST_PLAYST,
		"PlayST        -      Play ST modules only"},

		{STR_FUNCTIONLIST_PRINT,
		"Print         -      Print selected files"},

		{STR_FUNCTIONLIST_PRINTDIR,
		"PrintDir      - Print displayed directory"},

		{STR_FUNCTIONLIST_PROTECT,
		"Protect       -    Modify protection bits"},

		{STR_FUNCTIONLIST_QUIT,
		"Quit          -       Quit Directory Opus"},

		{STR_FUNCTIONLIST_READ,
		"Read          -         Plain text viewer"},

		{STR_FUNCTIONLIST_REDRAW,
		"Redraw        -        Redraw main screen"},

		{STR_FUNCTIONLIST_RELABEL,
		"Relabel       -         Change disk names"},

		{STR_FUNCTIONLIST_REMEMBER,
		"Remember      -     Remember config state"},

		{STR_FUNCTIONLIST_RENAME,
		"Rename        -   Rename selected entries"},

		{STR_FUNCTIONLIST_RESCAN,
		"Rescan        -  Reload current directory"},

		{STR_FUNCTIONLIST_RESELECT,
		"Reselect      -          Reselect entries"},

		{STR_FUNCTIONLIST_RESTORE,
		"Restore       -   Restore remembered cfg."},

		{STR_FUNCTIONLIST_ROOT,
		"Root          -       Read root directory"},

		{STR_FUNCTIONLIST_RUN,
		"Run           -             Run a program"},

		{STR_FUNCTIONLIST_SAVECONFIG,
		"SaveConfig    -       Save current config"},

		{STR_FUNCTIONLIST_SCANDIR,
		"ScanDir       -  Load nominated directory"},

		{STR_FUNCTIONLIST_SEARCH,
		"Search        - Search files for a string"},

		{STR_FUNCTIONLIST_SELECT,
		"Select        -   Wildcard-select entries"},

		{STR_FUNCTIONLIST_SETVAR,
		"SetVar        -  Set an internal variable"},

		{STR_FUNCTIONLIST_SHOW,
		"Show          -    Display graphic images"},

		{STR_FUNCTIONLIST_SMARTREAD,
		"SmartRead     -   Auto Hex or Text viewer"},

		{STR_FUNCTIONLIST_STOPST,
		"StopST        -    Stop ST module playing"},

		{STR_FUNCTIONLIST_SWAPWINDOW,
		"SwapWindow    -  Swap contents of windows"},

		{STR_FUNCTIONLIST_TOGGLE,
		"Toggle        -     Toggle all selections"},

		{STR_FUNCTIONLIST_UNICONIFY,
		"UnIconify     -        Un-iconify program"},

		{STR_FUNCTIONLIST_USER_FTYPE1,
		"User1         -  Custom filetype action 1"},

		{STR_FUNCTIONLIST_USER_FTYPE2,
		"User2         -  Custom filetype action 2"},

		{STR_FUNCTIONLIST_USER_FTYPE3,
		"User3         -  Custom filetype action 3"},

		{STR_FUNCTIONLIST_USER_FTYPE4,
		"User4         -  Custom filetype action 4"},

		{STR_FUNCTIONLIST_VERIFY,
		"Verify        -           Verify function"},

		{STR_FUNCTIONLIST_VERSION,
		"Version       -   Display version numbers"},

		{STR_COMMANDSEQ_DESTINATION,
		"{d}  -         Destination directory name"},

		{STR_COMMANDSEQ_FIRSTFILE,
		"{f}  -   First selected entry (with path)"},

		{STR_COMMANDSEQ_ALLFILES,
		"{F}  -  All selected entries (with paths)"},

		{STR_COMMANDSEQ_FIRSTFILEUNSEL,
		"{fu} -   First entry (path ; no unselect)"},

		{STR_COMMANDSEQ_ALLFILESUNSEL,
		"{Fu} -   All entries (path ; no unselect)"},

		{STR_COMMANDSEQ_FIRSTFILENOPATH,
		"{o}  -   First selected entry (name only)"},

		{STR_COMMANDSEQ_ALLFILESNOPATH,
		"{O}  -  All selected entries (names only)"},

		{STR_COMMANDSEQ_FIRSTFILENOPATHUNSEL,
		"{ou} -   First entry (name ; no unselect)"},

		{STR_COMMANDSEQ_ALLFILESNOPATHUNSEL,
		"{Ou} -   All entries (name ; no unselect)"},

		{STR_COMMANDSEQ_PORTNAME,
		"{Q}  -       Query the value of something"},

		{STR_COMMANDSEQ_DIRREQ,
		"{Rd} -                Directory requester"},

		{STR_COMMANDSEQ_FILEREQ,
		"{Rf} -                     File requester"},

		{STR_COMMANDSEQ_MULTIFILEREQ,
		"{RF} -        Multi-select file requester"},

		{STR_COMMANDSEQ_FONTREQ,
		"{Ro} -                     Font requester"},

		{STR_COMMANDSEQ_ARGUMENTS,
		"{Rs} -                   String requester"},

		{STR_COMMANDSEQ_SOURCE,
		"{s}  -              Source directory name"},

		{STR_COMMANDSEQ_VARIABLE,
		"{v}  -               Pre-defined variable"},

		{STR_CLASSOPS_AND,
		"And          - start of another group"},

		{STR_CLASSOPS_MATCH,
		"Match        - text or $hex"},

		{STR_CLASSOPS_MATCHBITS,
		"MatchBits    - HSPARWED with + and -"},

		{STR_CLASSOPS_MATCHCOMMENT,
		"MatchComment - text"},

		{STR_CLASSOPS_MATCHDATE,
		"MatchDate    - date string or range"},

		{STR_CLASSOPS_MATCHNAME,
		"MatchName    - filename"},

		{STR_CLASSOPS_MATCHSIZE,
		"MatchSize    - value with >, < and ="},

		{STR_CLASSOPS_MOVE,
		"Move         - byte offset from here"},

		{STR_CLASSOPS_MOVETO,
		"MoveTo       - absolute location"},

		{STR_CLASSOPS_OR,
		"Or           - start of another group"},

		{STR_CLASSOPS_SEARCHFOR,
		"SearchFor    - text or $hex"},

		{STR_SELECT_HOTKEY_TO_SWAP,
		"Select hotkey to swap."},

		{STR_SELECT_SECOND_HOTKEY_TO_SWAP,
		"Select second hotkey to swap with first."},

		{STR_MODULE_LIST_TITLE,
		"External support modules..."},

		{STR_LANGUAGE_LIST_TITLE,
		"Language..."},

		{STR_MODULE_LIST_CHECK_STRING,
		"(  Check indicates a module to be pre-loaded)"},

		{STR_LANGUAGE_LIST_CHECK_STRING,
		"(  Check indicates the language to use)"},

		{STR_SYS_CLOCK_KILOBYTES_FREE,
		"Kilobytes free"},

		{STR_SYS_STARTUP_CONFIG,
		"Cfg return"},

		{STR_MENU_CLEARCLIPS,
		"Clear clips"},

		{STR_REALLY_CLEAR_CLIPS,
		"Really clear all clipped functions?"},

		{STR_PALETTE_RESET,
		"Reset"},

		{STR_PALETTE_DOPUS_DEFAULT,
		"Directory Opus default"},

		{STR_PALETTE_WB_DEFAULT,
		"Workbench default"},

		{STR_PALETTE_WB_CURRENT,
		"Workbench current"},

		{STR_PALETTE_TINT,
		"Tint"},

		{STR_PALETTE_PHARAOH,
		"Pharaoh"},

		{STR_PALETTE_SUNSET,
		"Sunset"},

		{STR_PALETTE_OCEAN,
		"Ocean"},

		{STR_PALETTE_STEEL,
		"Steel"},

		{STR_PALETTE_CHOCOLATE,
		"Chocolate"},

		{STR_PALETTE_PEWTER,
		"Pewter"},

		{STR_PALETTE_WINE,
		"Wine"},

		{STR_PALETTE_A2024,
		"A2024"},

		{STR_FTYPE_ACTION,
		"Action"},

		{STR_FTYPE_COMMAND,
		"Command"},

		{STR_FTYPE_CLICKMCLICK,
		"Click-m-click"},

		{STR_FTYPE_DOUBLECLICK,
		"Double-click"},

		{STR_FILEVIEW_HEX,
		"Hex"},

		{STR_FILEVIEW_DEC,
		"Decimal"},

		{STR_FUNCTION_COMMAND,
		"Command"},

		{STR_FUNCTION_AMIGADOS,
		"AmigaDOS"},

		{STR_FUNCTION_WORKBENCH,
		"Workbench"},

		{STR_FUNCTION_BATCH,
		"Batch"},

		{STR_FUNCTION_AREXX,
		"ARexx"},

		{STR_MODE_WORKBENCH_CLONE,
		"Workbench:Clone"},

		{STR_MODE_WORKBENCH_USE,
		"Workbench:Use"},

		{STR_SCREEN_MODE_USE,
		"Use"},

		{STR_SCREEN_MODE_CLONE,
		"Clone"},

		{STR_COLOURS_TINY_GADS,
		"BRSA?EFCIQ"},

		{STR_NEAT_PAINT_MODE,
		"Paint mode"},

		{STR_PAINT_SELECT_COLOURS,
		"Select paint colors"},

		{STR_PAINT_STATE,
		"  (Paint mode active)"},

		{STR_STRING_COUNT,NULL}};

#ifndef STRINGS_ONLY
void read_strings()
{
	char stringfile[80],buf[40];

	lsprintf(buf,"CO_%s.STR",config->language);
	if (((!config->language[0] ||
		!(FindSystemFile(buf,stringfile,80,SYSFILE_DATA))) &&
		!(FindSystemFile("ConfigOpus.STR",stringfile,80,SYSFILE_DATA))))
		stringfile[0]=0;

	stringdata.default_table=default_strings;
	stringdata.string_count=STR_STRING_COUNT;
	stringdata.min_version=STRING_VERSION;
	if (!(ReadStringFile(&stringdata,stringfile))) quit();
	cfg_string=stringdata.string_table;
	init_strings();
}
#else
char string_type[]="Config Opus Strings";
char *string_save="CO_%s.STR";
#endif

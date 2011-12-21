/****************************************************************

   This file was created automatically by `FlexCat 2.5'
   from "../catalogs/DOpus4.cd".

   Do NOT edit by hand!

****************************************************************/

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

#include "dopus.h"

char **globstring;

static const struct DefaultString
	default_strings[]={
    	{STR_DIRS_FILES_BYTES_COUNT,"Dirs : %03ld/%03ld  Files : %03ld/%03ld  Bytes : %7s/%7s"},
    	{STR_DOPUS_ALREADY_RUNNING,"Directory Opus is already resident in memory.\nDo you want to run another copy?"},
    	{STR_UNABLE_TO_OPEN_WINDOW,"Unable to open screen/window!"},
    	{STR_WELCOME_TO_DOPUS,"Welcome to Directory Opus!"},
    	{STR_WELCOME_BACK_TO_DOPUS,"Welcome back to Directory Opus!"},
    	{STR_INTERROGATING_FILE,"Interrogating file..."},
    	{STR_READING_DIRECTORY,"Reading directory..."},
    	{STR_ENTER_ARGUMENTS_FOR,"Enter arguments for \"%s\""},
    	{STR_ENTER_DIRECTORY_NAME,"Enter directory name"},
    	{STR_FILE_ALREADY_EXISTS,"File or directory already exists!"},
    	{STR_DIRECTORY_CREATED,"Directory created."},
    	{STR_PRINTING_DIRECTORY,"Printing directory..."},
    	{STR_DIRECTORY,"Directory "},
    	{STR_DEVICE_LIST,"Device list\n\n"},
    	{STR_NO_CHIP_FOR_ICON,"Not enough chip memory to display icon"},
    	{STR_FILE_NOT_ILBM,"File is not ILBM"},
    	{STR_ERROR_IN_IFF,"Error in IFF structure"},
    	{STR_NO_CHIP_FOR_PICTURE,"Not enough chip memory to display picture"},
    	{STR_CANT_ALLOCATE_AUDIO,"Can't allocate audio channels"},
    	{STR_NOT_ST_MOD,"File is not a supported music module"},
    	{STR_FILE_EXISTS_REPLACE,"\"%s\" already exists! Replace?"},
    	{STR_SELECT_UNPROTECT,"\nSelect Unprotect to set the deletion bit."},
    	{STR_ENTER_TIME,"Enter time in the form HH:MM:SS"},
    	{STR_AREXX_NOT_RUNNING,"ARexx server not found!"},
    	{STR_ENTER_AREXX_COMMAND,"Enter ARexx command"},
    	{STR_ENTER_NEW_CD,"Enter new current directory"},
    	{STR_SCANNING_DEVICE_LIST,"Scanning device list..."},
    	{STR_FOUND_A_MATCH,"Found the file \"%s\" in the directory\n\"%s\"\nShall I go there?"},
    	{STR_BUFFERS_CLEARED,"Buffers not currently displayed have been cleared."},
    	{STR_UNREADABLE_DISK,"Unreadable disk"},
    	{STR_NOT_A_DOS_DISK,"Not a DOS disk"},
    	{STR_KICKSTART_DISK,"Kickstart disk"},
    	{STR_VALIDATING,"Validating"},
    	{STR_WRITE_PROTECTED,"Write protected"},
    	{STR_READ_WRITE,"Read/Write"},
    	{STR_DEVICE_NOT_MOUNTED,"Device or volume not mounted"},
    	{STR_NO_DISK_IN_DRIVE,"No disk in drive"},
    	{STR_DOS_ERROR_CODE,"DOS error code %ld"},
    	{STR_ENTER_NEW_DISK_NAME,"Enter new disk name"},
    	{STR_ENTER_ASSIGN_NAME,"Enter directory to re-assign device"},
    	{STR_CANT_CANCEL_ASSIGN,"Can't cancel assign."},
    	{STR_ENTER_DEVICE_NAME,"Enter device name for this directory"},
    	{STR_ASSIGN_FAILED,"Assign failed"},
    	{STR_NO_DESTINATION_SELECTED,"No destination directory selected!"},
    	{STR_CANT_COPY_DIR_TO_ITSELF,"You can't copy a directory into itself!"},
    	{STR_SHOWING_FONT,"Showing selected font..."},
    	{STR_RUNNING_FILE,"Running selected file..."},
    	{STR_PLAYING_FILE,"Playing selected file..."},
    	{STR_SHOWING_FILE,"Showing selected file..."},
    	{STR_PLAYING_ST_MOD,"Playing music module..."},
    	{STR_READING_SELECTED_FILE,"Reading selected file..."},
    	{STR_FOUND_A_MATCH_READ,"Found a match in the file\n\"%s\"\nShall I read the file?"},
    	{STR_CONFIG_CHANGED_QUIT,"You have modified the configuration.\nDo you really want to quit?"},
    	{STR_SAVING_CONFIG,"Saving configuration..."},
    	{STR_READING_CONFIG,"Reading configuration..."},
    	{STR_CONFIG_CHANGED_LASTSAVED,"You have modified the configuration.\nReally load the last-saved settings?"},
    	{STR_CONFIG_CHANGED_DEFAULTS,"You have modified the configuration.\nSave before restoring defaults?"},
    	{STR_HELP_NOT_AVAILABLE,"Help not available for that function!"},
    	{STR_SCANNING_TREE,"Scanning directory tree..."},
    	{STR_ENTER_A_STRING,"Enter a string"},
    	{STR_PLEASE_CHOOSE,"Please choose"},
    	{STR_COMMENTING,"commenting"},
    	{STR_PROTECTING,"protecting"},
    	{STR_DATESTAMPING,"datestamping"},
    	{STR_DELETING,"deleting"},
    	{STR_COPYING,"copying"},
    	{STR_CREATING,"creating"},
    	{STR_CANT_OVERCOPY_FILES,"You can't copy these files over themselves!"},
    	{STR_INTERROGATING_FILES,"Interrogating files..."},
    	{STR_NOT_IDENTIFIED,"%s could not be identified."},
    	{STR_LOADING_CONFIG,"Loading configuration program..."},
    	{STR_WAITING_FOR_PORT,"Waiting for configuration port..."},
    	{STR_CONFIG_NOT_FOUND,"Configuration program not found/could not be loaded."},
    	{STR_ALIEN_WINDOWS,"There are alien windows open on the Directory Opus screen.\nSelect Cancel and then close them yourself, or select Close\nand Directory Opus will close them for you (DANGER!)"},
    	{STR_REALLY_DELETE,"Do you really wish to delete selected entries?"},
    	{STR_WISH_TO_DELETE,"Do you really wish to delete \"%s\"?"},
    	{STR_NOT_EMPTY,"\"%s\" is not empty! Delete?"},
    	{STR_ENTER_NEW_NAME,"Enter new name"},
    	{STR_RENAMING,"renaming"},
    	{STR_ENTER_NEW_NAME_MOVE,"Enter new name for moved file"},
    	{STR_MOVING,"moving"},
    	{STR_ENTER_NEW_NAME_CLONE,"Enter name for cloned entry"},
    	{STR_CLONING,"cloning"},
    	{STR_SIZE_IS_NOT_KNOWN,"Size is not known for the selected directory.\nPerform a byte count on this directory?"},
    	{STR_FILE_MAY_NOT_FIT,"Selected file will probably not fit in destination.\nContinue with the copy?"},
    	{STR_SIZES_NOT_KNOWN,"Sizes are not known for some selected directories.\nPerform a byte count on these directories?"},
    	{STR_ENTRIES_MAY_NOT_FIT,"Selected entries will probably not fit in destination.\nContinue with the copy?"},
    	{STR_ENTER_NEW_NAME_COPY,"Enter new name for copied file"},
    	{STR_ENTER_HUNT_PATTERN,"Enter file pattern to hunt"},
    	{STR_COULD_NOT_FIND_FILE,"Sorry! File could not be found!"},
    	{STR_FOUND_MATCHING_FILES,"Found %ld matching files."},
    	{STR_ADDING_ICON,"adding an icon to"},
    	{STR_SELECT_COMMENT,"Comment"},
    	{STR_ENTER_COMMENT,"Enter comment"},
    	{STR_CANNOT_CLOSE_SCREEN,"I cannot close the screen because\nthere are alien windows opened on it.\nI'll wait quietly until they'll be closed"},
    	{STR_ENTER_PASSWORD,"Enter password"},
    	{STR_ENTER_DATE_AND_TIME,"Enter date and time (blank for current)"},
    	{STR_ENTER_SEARCH_STRING,"Enter search string"},
    	{STR_RENAME_REQ,"Rena_me"},
    	{STR_STRING_NOT_FOUND,"Sorry! String could not be found!"},
    	{STR_MATCHED_FILES,"Matched in %ld files."},
    	{STR_NO_SOURCE_SELECTED,"No source directory selected!"},
    	{STR_TEMPORARY_OUTPUT_FILE,"Temporary output file"},
    	{STR_ENTER_ARGUMENTS,"Enter arguments"},
    	{STR_SELECT_FUNCTION,"Select function you need help with, or Help again to cancel."},
    	{STR_CONFIG_CHANGED_LOAD,"You have modified the configuration.\nReally load a new configuration file?"},
    	{STR_ABORTED,"Aborted."},
    	{STR_ENTER_DOS_ERROR_CODE,"Enter DOS error code"},
    	{STR_NO_HELP_FOR_ERROR,"Help not available for that error code!"},
    	{STR_ERROR_OCCURED,"An error occured %s \"%s\"\n%s"},
    	{STR_RENAME,"Rename"},
    	{STR_AS,"As"},
    	{STR_ENTER_SELECT_PATTERN,"Enter select pattern"},
    	{STR_FILE_ENCRYPTED,"\nFile is encrypted!\n\n"},
    	{STR_READING_FILE,"Reading file..."},
    	{STR_NO_MEMORY_TO_DECRUNCH,"Not enough memory to load/decrunch complete file!"},
    	{STR_BAD_PASSWORD,"Incorrect password given!"},
    	{STR_COUNTING_LINES,"Counting lines..."},
    	{STR_JUMP_TO_LINE,"Jump to which line?"},
    	{STR_JUMP_TO_PERCENTAGE,"Jump to what percentage?"},
    	{STR_CANT_OPEN_PRINTER,"Can't open printer!"},
    	{STR_ERROR_PRINTING_FILE,"Error printing file! IoErr = %ld\n"},
    	{STR_READY_PRINTER,"Ready printer, and choose OKAY to continue"},
    	{STR_CANT_FIND_ICON,"Unable to find or load an icon for that file."},
    	{STR_ENCRYPTING,"encrypting"},
    	{STR_NEW_DISK_INSERTED,"A new disk has been inserted in drive DF%ld:\nDo you want to read this disk?"},
    	{STR_REPLACE_ALL,"Replace _all"},
    	{STR_ENTER_DATE_1,"Enter date in the form DD-MMM-YY"},
    	{STR_ENTER_DATE_2,"Enter date in the form YY-MM-DD"},
    	{STR_ENTER_DATE_3,"Enter date in the form MM-DD-YY"},
    	{STR_ENTER_DATE_4,"Enter date in the form DD-MM-YY"},
    	{STR_CONTINUE_WITH_SEARCH,"Do you wish to continue with the search?"},
    	{STR_REALLY_QUIT,"Do you really want to quit Directory Opus?"},
    	{STR_REALLY_PROCEED,"Do you really wish to proceed with this operation?"},
    	{STR_SCREEN_MODE_UNAVAILABLE,"Screen mode not available."},
    	{STR_ERROR_INITIALISING_PRINTER,"Error initialising printer.\nCheck cable and connections."},
    	{STR_REALLY_ABORT_PRINT,"Really abort the print?"},
    	{STR_SELECT_SOURCE_DIR,"Select the source directory"},
    	{STR_SELECT_DESTINATION_DIR,"Select the destination directory"},
    	{STR_SELECT_A_FILE,"Select a file"},
    	{STR_SELECT_A_DIRECTORY,"Select a directory"},
    	{STR_LEFT_WINDOW,"Left window"},
    	{STR_RIGHT_WINDOW,"Right window"},
    	{STR_DEVICE_LIST_TITLE,"<Device list>"},
    	{STR_DIR_NOT_AVAILABLE_TITLE,"<Directory not available>"},
    	{STR_BUFFER_LIST_TITLE,"<Buffer list>"},
    	{STR_UNPROTECT,"Unprotect"},
    	{STR_RUN,"_Run"},
    	{STR_TRY_AGAIN,"_Try Again"},
    	{STR_RETRY,"Retry"},
    	{STR_ABORT,"A_bort"},
    	{STR_REPLACE,"_Replace"},
    	{STR_SKIP,"_Skip"},
    	{STR_ALL,"_All"},
    	{STR_REMOVE,"Remove"},
    	{STR_QUIT,"_Quit"},
    	{STR_SAVE_QUIT,"_Save & quit"},
    	{STR_LOAD,"Load"},
    	{STR_SAVE,"Save"},
    	{STR_DEFAULTS,"Defaults"},
    	{STR_CLOSE,"Close"},
    	{STR_DELETE,"_Delete"},
    	{STR_LEAVE,"Leave"},
    	{STR_YES,"Yes"},
    	{STR_NO,"No"},
    	{STR_ICONIFY,"_Iconify"},
    	{STR_CONTINUE,"C_ontinue"},
    	{STR_OKAY,"_Okay"},
    	{STR_CANCEL,"_Cancel"},
    	{STR_OKAY_TITLE,"OK"},
    	{STR_DESCRIPTION,"Description"},
    	{STR_PRINT_CONTROL,"Print control"},
    	{STR_FILE,"File"},
    	{STR_IMAGE_SIZE,"Image size"},
    	{STR_PAGE_SIZE,"Page size"},
    	{STR_SCREEN_SIZE,"Screen size"},
    	{STR_DEPTH,"Depth"},
    	{STR_COLOURS,"Colors"},
    	{STR_SCREEN_MODES,"Screen modes"},
    	{STR_ASPECT,"_Aspect:"},
    	{STR_IMAGE,"_Image:"},
    	{STR_SHADE,"_Shade:"},
    	{STR_PLACEMENT,"_Placement:"},
    	{STR_FORMFEED,"_Form feed"},
    	{STR_PRINT,"Print"},
    	{STR_ABORT_PRINT,"Abort print"},
    	{STR_PORTRAIT,"Portrait"},
    	{STR_LANDSCAPE,"Landscape"},
    	{STR_POSITIVE,"Positive"},
    	{STR_NEGATIVE,"Negative"},
    	{STR_BLACK_WHITE,"Black&White"},
    	{STR_GRAY_SCALE,"Gray scale"},
    	{STR_COLOUR,"Color"},
    	{STR_CENTER,"Center"},
    	{STR_FULL_LEFT,"Full left"},
    	{STR_FONT,"Font"},
    	{STR_FONT_SIZE,"Font size"},
    	{STR_NUM_CHARS,"Characters"},
    	{STR_FONT_STYLE,"Font style"},
    	{STR_FONT_FLAGS,"Font flags"},
    	{STR_ICON,"Icon"},
    	{STR_ICON_TYPE,"Icon type"},
    	{STR_ICON_ALTERNATE,"Alt. image"},
    	{STR_ICON_DEFAULTTOOL,"Default tool"},
    	{STR_NUM_FRAMES,"Frame"},
    	{STR_PLAYING_ANIM,"Playing selected animation..."},
    	{STR_OLD_NEW_FILE_REPLACE,"\"%s\" already exists! Replace?\n\n"\
	"New  -  Size : %7ld  Date : %19s\n"\
	"Old  -  Size : %7ld  Date : %19s"},
    	{STR_REMAINING,"%ld of %ld items"},
    	{STR_CUSTOM_LIST,"Custom list"},
    	{STR_ENTRIES_IN_TREE,"%ld entries in tree"},
    	{STR_DIRS_IN_BUFFERS,"%ld directories in buffers"},
    	{STR_USER_ENTRIES,"%ld user-defined entries"},
    	{STR_COMPLETED,"Completed"},
    	{STR_UNICONIFY,"Uniconify"},
    	{STR_CUSTPORT_NOT_FOUND,"User-specified custom handler port\n\"%s\"\ncould not be found!"},
    	{STR_CHECKFIT_STRING,"Bytes needed : %11qd   Bytes free : %11qd   Fit : %.1f%%"},
    	{STR_DIR_TREE,"<Directory tree>"},
    	{STR_BUILDING_TREE,"Generating tree structure..."},
    	{STR_UNPROTECT_ALL,"Unprotect all"},
    	{STR_READING_CHANGED_DIRECTORY,"Buffered directory changed - re-reading"},
    	{STR_CREATING_DIRECTORY,"creating directory"},
    	{STR_STARTING_DISK_MODULE,"Starting disk module..."},
    	{STR_UNABLE_TO_LOAD_MODULE,"Unable to load external module"},
    	{STR_SELECT_ALL_ENTRIES,"All entries"},
    	{STR_SELECT_ONLY_FILES,"Only files"},
    	{STR_SELECT_ONLY_DIRS,"Only dirs"},
    	{STR_SELECT_NAME,"Name"},
    	{STR_SELECT_DATE,"Date"},
    	{STR_SELECT_BITS,"Bits"},
    	{STR_STARTING_PRINT_MODULE,"Starting print module..."},
    	{STR_ABOUT,"%s\nBased on GPL release version 4.12\nmodified by %s\n\n"\
	"This program is free software; you can redistribute it and/or\n"\
	"modify it under the terms of the GNU General Public License\n"\
	"as published by the Free Software Foundation; either version 2\n"\
	"of the License, or (at your option) any later version.\n\n"\
	"This program is distributed in the hope that it will be useful,\n"\
	"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"\
	"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"\
	"GNU General Public License for more details.\n\n"\
	"All users of Directory Opus 4 (including versions distributed\n"\
	"under the GPL) are entitled to upgrade to the latest version of\n"\
	"Directory Opus version 5 at a reduced price. Please see\n"\
	"http://www.gpsoft.com.au for more information."},
    	{STR_PRINTTITLE,"Print _Title"},
    	{STR_SEARCH_NO_CASE,"Treat upper and lower case as the same"},
    	{STR_SEARCH_WILD,"Recognise ? as a wildcard character"},
    	{STR_SEARCH_ONLYWORD,"Only match on whole, unbroken words"},
    	{STR_ENTER_FILTER,"Enter file filter"},
    	{STR_SELECT_PROTECTION_BITS,"Select new protection bits"},
    	{STR_DIRECTORY_OPUS_REQUEST,"Directory Opus Request"},
    	{STR_CLOCK_CHIP,"CHIP:"},
    	{STR_CLOCK_FAST,"FAST:"},
    	{STR_CLOCK_TOTAL,"TOTAL:"},
    	{STR_CLOCK_MEM,"MEM:"},
    	{STR_KEY,"Key :"},
    	{STR_TINY_BUTTONS,"BRSA?EFCIQ"},
    	{STR_VIEW_BUTTONS,"UDTBSPQ"},
    	{STR_DISKINFO_DEVICE,"Device"},
    	{STR_DISKINFO_NAME,"Name"},
    	{STR_DISKINFO_SIZE,"Size"},
    	{STR_DISKINFO_USED,"Used"},
    	{STR_DISKINFO_FREE,"Free"},
    	{STR_DISKINFO_PERCENT,"Percent"},
    	{STR_DISKINFO_DENSITY,"Density"},
    	{STR_DISKINFO_ERRORS,"Errors"},
    	{STR_DISKINFO_STATUS,"Status"},
    	{STR_DISKINFO_DATE,"Date"},
    	{STR_DISKINFO_BYTES,"bytes"},
    	{STR_DISKINFO_BLOCKS,"blocks"},
    	{STR_DISKINFO_FULL,"full"},
    	{STR_DISKINFO_SFREE,"free"},
    	{STR_DISKINFO_BYTESPERBLOCK,"bytes/block"},
    	{STR_TEXT_VIEWER_TITLE,"Directory Opus Text Viewer"},
    	{STR_PROTECT_ALL_BITS,"All bits"},
    	{STR_PROTECT_NONE,"None"},
    	{STR_PROTECT_TOGGLE,"Toggle"},
    	{STR_PROTECT_REVERT,"Revert"},
    	{STR_PROTECT_OLD,"Old :"},
    	{STR_PROTECT_NEW,"New :"},
    	{STR_PROTECT_MASK,"Mask:"},
    	{STR_PROTECT_HIDDEN,"_Hidden"},
    	{STR_PROTECT_SCRIPT,"_Script"},
    	{STR_PROTECT_PURE,"_Pure"},
    	{STR_PROTECT_ARCHIVE,"_Archive"},
    	{STR_PROTECT_READ,"_Read"},
    	{STR_PROTECT_WRITE,"_Write"},
    	{STR_PROTECT_EXECUTED,"_Execute"},
    	{STR_PROTECT_DELETED,"_Delete"},
    	{STR_FILE_REQUEST,"File request"},
    	{STR_OF,"of"},
    	{STR_FRAME,"Frame"},
    	{STR_DISK_OP_TITLE,"Directory Opus Disk Operations"},
    	{STR_BUTTON_STRIP,"Directory Opus Button Strip"},
    	{STR_NO_FIT," N"},
    	{STR_YES_FIT," Y"},
    	{STR_FTYPE_DEFAULT,"Default"},
    	{STR_FTYPE_UNKNOWN,"Unknown file type"},
    	{STR_MULTI_DRAG,"Multi-drag - %ld files selected"},
    	{STR_PRESS_MOUSE_BUTTON,"Press left mouse button to continue..."},
    	{STR_NETWORK_NO_OWNER,"No owner"},
    	{STR_NETWORK_NO_GROUP,"No group"},
    	{STR_ERROR_ERROR_HELP,"Error help"},
    	{STR_SKIP_ALL,"S_kip all"},
    	{STR_PARENT_MULTI,"Parents and Multi-Assigns..."},
    	{STR_DISKINFO_FILESYSTEM,"F.System"},
    	{STR_ANIM_TYPE,"Anim type"},
    	{STR_GFX_NATIVE,"Native"},
    	{STR_OPENING_ARCHIVE,"Opening archive..."},
    	{STR_LISTER_MENU,"Lister options"},
    	{STR_FILE_NAME,"Name"},
    	{STR_FILE_SIZE,"Size"},
    	{STR_PROTECTION_BITS,"Protection"},
    	{STR_CREATION_DATE,"Date"},
    	{STR_FILE_COMMENT,"Comment"},
    	{STR_FILE_TYPE,"File type"},
    	{STR_OWNER,"Owner"},
    	{STR_GROUP,"Group"},
    	{STR_NET_PROTECTION_BITS,"Net protection bits"},
    	{STR_REVERSE_ORDER,"Reverse order"},
    	{STR_SIZE_AS_KMG,"Sizes in K/M/G format"},
    	{STR_BRING_TO_FRONT,"_Bring to front"},
    	{STR_REPLACE_DIR_WITH_FILE,"\"%s\" already exists! Replace?\n\n"\
	"New  -  Size : %7ld  Date : %19s\n"\
	"Old  -                  Date : %19s"},
    	{STR_REPLACE_FILE_WITH_DIR,"\"%s\" already exists! Replace?\n\n"\
	"New  -                  Date : %19s\n"\
	"Old  -  Size : %7ld  Date : %19s"},
    	{STR_OPERATION_NOT_SUPPORTED,"Operation not supported"},
    	{STR_QUERY_REMOVE_ASSIGN,"Do you really wish to remove \"%s\" assign?"},
    	{STR_VERSION_CONTENTS,"%-42s\n\n"\
	"  Directory Opus%24s  \n"\
	"  DOpus Library%25s  \n"\
	"  Compile time%26s  \n"\
	"  Compile date%26s  \n"\
	"  Port/screen name%22s  \n\n"\
	"  Kickstart%29s  \n"\
	"  Workbench%29s  \n"\
	"  Processor%29s  \n"\
	"  Math co-processor%21s  \n"\
	"  Graphics chipset%22s  \n"\
	"  Video system%26s  \000"},
    	{STR_VERSION_HEADER,"Version information..."},
    	{STR_DIRSFIRST,"Show dirs first"},
    	{STR_FILESFIRST,"Show files first"},
    	{STR_MIXDIRSFILES,"Mix files and dirs"},
    	{STR_SORTALPHA,"Plain alphanumeric"},
    	{STR_SORTDEC,"Recognize decimals"},
    	{STR_SORTHEX,"Recognize hexadecimals"},
    	{STR_SORTBY,"Sort by"},
    	{STR_NAMESORT_MENU,"Name sorting"},
    	{STR_FILE_EXTENSION,"Extension"},
    	{STR_PROTECT_OKAY,"_Okay"},
    	{STR_PROTECT_ALL,"A_ll"},
    	{STR_ENTER_MAKELINK_PARAMS,"Enter link parameters"},
    	{STR_MAKELINK_TYPE_SOFT,"Soft"},
    	{STR_MAKELINK_TYPE_HARD,"Hard"},
    	{STR_MAKELINK_TYPE,"Type"},
    	{STR_MAKELINK_NAME,"Link name"},
    	{STR_MAKELINK_DESTINATION,"Link destination"},
    	{STR_PROTECT_CANCEL,"_Cancel"},
        {STR_STRING_COUNT,NULL}};

char str_okaystring[30];
char str_cancelstring[30];

void readstrings(file)
char *file;
{
//    int a;
    stringdata.default_table=default_strings;
    stringdata.string_count=STR_STRING_COUNT;
    stringdata.min_version=0;
//D(bug("readstrings(%s)\n",file);Delay(50);)

//D(bug("STR_STRING_COUNT=%ld\n",STR_STRING_COUNT));
//D(for(a=0;a<STR_STRING_COUNT;a++) bug("%ld\t%s\n",default_strings[a].string_id,default_strings[a].string));
    if (!(ReadStringFile(&stringdata,file))) quit();
    globstring=stringdata.string_table;
/*
    for (a=0;a<12;a++) {
        date_months[a]=globstring[STR_MONTH_JANUARY+a];
        date_shortmonths[a]=globstring[STR_MONTH_JAN+a];
    }
    date_weekdays[0]=globstring[STR_DAY_SUNDAY];
    date_shortweekdays[0]=globstring[STR_DAY_SUN];
    for (a=1;a<7;a++) {
        date_weekdays[a]=globstring[(STR_DAY_MONDAY-1)+a];
        date_shortweekdays[a]=globstring[(STR_DAY_MON-1)+a];
    }
    for (a=0;a<4;a++) date_special[a]=globstring[STR_DAY_YESTERDAY+a];
*/
    strcpy(str_okaystring,globstring[STR_OKAY]);
    strcpy(str_cancelstring,globstring[STR_CANCEL]);
}

int getkeyshortcut(const char *str)
{
    char *c;

    if ((c = strchr(str,'_'))) return ToLower(c[1]);
    else return 0;
}


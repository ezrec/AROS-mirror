// Delete.module_strings.h
// $Date$
// $Revision$

//     ___       ___
//   _/  /_______\  \_     ___ ___ __ _                       _ __ ___ ___
//__//  / _______ \  \\___/                                               \___
//_/ | '  \__ __/  ` | \_/        © Copyright 1999, Christopher Page       \__
// \ | |    | |__  | | / \   Released as Free Software under the GNU GPL   /
//  >| .    |  _/  . |<   >--- --- -- -                       - -- --- ---<
// / \  \   | |   /  / \ /   This file is part of the ScalosDelete code    \
// \  \  \_/   \_/  /  / \  and it is released under the GNU GPL. Please   /
//  \  \           /  /   \   read the "COPYING" file which should have   /
// //\  \_________/  /\\ //\    been included in the distribution arc.   /
//- --\   _______   /-- - --\      for full details of the license      /-----
//-----\_/       \_/---------\   ___________________________________   /------
//                            \_/                                   \_/
//
// Description:
//
//  Default locale strings (English)
//
// Modification History:
//
//  [02 June 1999, Chris <chris@worldfoundry.demon.co.uk>]
//
//  Converted to GNU GPL license, finally used the correct numbers for the
//  ID_s!
//
//

#define ID_APP_TITLE        1
#define MSG_APP_TITLE       "Delete.module"

#define ID_APP_DESC         2
#define MSG_APP_DESC        "Replacement for Scalos Delete.module"

#define ID_WIN_TITLE        3
#define MSG_WIN_TITLE       "Delete"

#define ID_ASL_TITLE        4
#define MSG_ASL_TITLE       "Select Trashcan directory"

#define ID_PAGES_FILES      5
#define MSG_PAGES_FILES     "Files"

#define ID_PAGES_PREFS      6
#define MSG_PAGES_PREFS     "Preferences"

#define ID_LABEL_TRASH      7
#define MSG_LABEL_TRASH     "Use trashcan?"

#define ID_LABEL_DIRS       8
#define MSG_LABEL_DIRS      "Confirm dir delete?"

#define ID_LABEL_FILES      9
#define MSG_LABEL_FILES     "Confirm file delete?"

#define ID_LABEL_QUIET      10
#define MSG_LABEL_QUIET     "Quiet start"

#define ID_LABEL_TLOC       11
#define MSG_LABEL_TLOC      "Trashcan Location"

#define ID_LABEL_INFO       12
#define MSG_LABEL_INFO      "Done %ld%%"

#define ID_LABEL_PROG       13
#define MSG_LABEL_PROG      "Selected: %ld files [%ld bytes], %ld directories"

#define ID_LABEL_VERS       14
#define MSG_LABEL_VERS      "Version:"

#define ID_LABEL_COMP       15
#define MSG_LABEL_COMP      "Compiled:"

#define ID_BUTTON_FULL      16
#define MSG_BUTTON_FULL     "Details"

#define ID_BUTTON_DEL       17
#define MSG_BUTTON_DEL      "Delete"

#define ID_BUTTON_ABT       18
#define MSG_BUTTON_ABT      "About"

#define ID_BUTTON_CAN       19
#define MSG_BUTTON_CAN      "Cancel"

#define ID_BUTTON_STOP      20
#define MSG_BUTTON_STOP     "STOP!"

#define ID_BUTTON_AMUI      21
#define MSG_BUTTON_AMUI     "About MUI"

#define ID_BUTTON_SAVE      22
#define MSG_BUTTON_SAVE     "Save"

#define ID_BUTTON_USE       23
#define MSG_BUTTON_USE      "Use"

#define ID_BUTTON_REST      24
#define MSG_BUTTON_REST     "Restore"

#define ID_KEY_DETAILS      25
#define MSG_KEY_DETAILS     "e"

#define ID_KEY_TRASH        26
#define MSG_KEY_TRASH       "t"

#define ID_KEY_DIRS         27
#define MSG_KEY_DIRS        "o"

#define ID_KEY_FILES        28
#define MSG_KEY_FILES       "n"

#define ID_KEY_QUIET        29
#define MSG_KEY_QUIET       "q"

#define ID_KEY_TLOC         30
#define MSG_KEY_TLOC        "l"

#define ID_KEY_SAVE         31
#define MSG_KEY_SAVE        "v"

#define ID_KEY_USE          32
#define MSG_KEY_USE         "u"

#define ID_KEY_REST         33
#define MSG_KEY_REST        "r"

#define ID_KEY_DELETE       34
#define MSG_KEY_DELETE      "d"

#define ID_KEY_ABOUT        35
#define MSG_KEY_ABOUT       "a"

#define ID_KEY_CANCEL       36
#define MSG_KEY_CANCEL      "c"

#define ID_KEY_STOP         37
#define MSG_KEY_STOP        "s"

#define ID_KEY_AMUI         38
#define MSG_KEY_AMUI        "m"

#define ID_HELP_TRASH       39
#define MSG_HELP_TRASH      "\33cFiles are moved to\nSYS:Trashcan\nrather than being\nremoved completely"

#define ID_HELP_DIRS        40
#define MSG_HELP_DIRS       "\33cIf set, you must confirm that\na directory should be deleted"

#define ID_HELP_FILES       41
#define MSG_HELP_FILES      "\33cIf set, you must confirm every\nfile delete operation"

#define ID_HELP_QUIET       42
#define MSG_HELP_QUIET      "\33cIf set, the about window is not\nshown when the program starts"

#define ID_HELP_TLOC        43
#define MSG_HELP_TLOC       "\33cLocation of the trashcan used\nif \"Use Trashcan\" is set"

#define ID_LVT_NAME         44
#define MSG_LVT_NAME        "\33bName\33n"

#define ID_LVT_SIZE         45
#define MSG_LVT_SIZE        "Size"

#define ID_LVT_DATE         46
#define MSG_LVT_DATE        "Date"

#define ID_LVT_TIME         47
#define MSG_LVT_TIME        "Time"

#define ID_ERROR_TITLE      48
#define MSG_ERROR_TITLE     "Delete module error"

#define ID_WARN_TITLE       49
#define MSG_WARN_TITLE      "Delete module warning"

#define ID_REQ_TITLE        50
#define MSG_REQ_TITLE       "Delete module request"

#define ID_GAD_OK           51
#define MSG_GAD_OK          "*_Ok"

#define ID_GAD_NOYES        52
#define MSG_GAD_NOYES       "_Yes|*_No"

#define ID_GAD_PROTECT      53
#define MSG_GAD_PROTECT     "_Yes|_All files|*_No"

#define ID_BODY_NOFILES     54
#define MSG_BODY_NOFILES    "\33cNo files have been selected!\n\nOperation aborted"

#define ID_BODY_NOLIST      55
#define MSG_BODY_NOLIST     "\33cUnable to create internal lists\n\nOperation aborted"

#define ID_BODY_VOLUME      56
#define MSG_BODY_VOLUME     "\33cDetected an attempt to delete volume\n\33b%s\33n\nAre you \33bsure\33n you want to do this?!?"

#define ID_BODY_CFILE       57
#define MSG_BODY_CFILE      "\33cAre you sure you want to delete file\n%s"

#define ID_BODY_BUFERR      58
#define MSG_BODY_BUFERR     "\33cUnable to copy\n%s\nto trashcan\nOut of buffer space.\nContinue to delete the file?"

#define ID_BODY_COPYFAIL    59
#define MSG_BODY_COPYFAIL   "\33cUnable to copy\n%s\nto trashcan\nError writing to trashcan\nContinue to delete the file?"

#define ID_BODY_NOICON      60
#define MSG_BODY_NOICON     "\33cUnable to delete icon\n%s"

#define ID_BODY_CDIR        61
#define MSG_BODY_CDIR       "\33cAre you sure you want to delete directory\n%s"

#define ID_BODY_BADTD       62
#define MSG_BODY_BADTD      "\33cUnable to create directory\nin Trashcan.\nContinue?"

#define ID_BODY_NOSCAN      63
#define MSG_BODY_NOSCAN     "\33cUnable to scan directory\nDirectory aborted"

#define ID_BODY_PROTECT     64
#define MSG_BODY_PROTECT    "\33c%s\nis protected from deletion\nRemove the protection?"

#define ID_BODY_NODEL       65
#define MSG_BODY_NODEL      "\33c%s\nnot deleted"

#define ID_BODY_COPYING     66
#define MSG_BODY_COPYING    "Copying %s to trashcan"

#define ID_BODY_REMOVE      67
#define MSG_BODY_REMOVE     "Removing %s"

#define ID_BODY_SCAN        68
#define MSG_BODY_SCAN       "Scanning %s"

#define ID_BODY_DETAIL      69
#define MSG_BODY_DETAIL     "\33c\0338WARNING\0330\nThis operation may take a long time\nto complete and may not be\naborted. Continue?"


// The following lines *MAY NOT* be modified. They are the copyright notice
// and GNU GPL notice. Removing these violates the GNU GPL agreemenT!
#define ID_ABOUT_HEAD       70
#define MSG_ABOUT_HEAD      "\n\33b\33c\0338ScalosDelete\0330\33n\n"\
"  Copyright 1999 Chris Page,  \n"\
"  The World Foundry LLC \n\n"\
"  Contact: \33ichris <chris@worldfoundry.demon.co.uk>  \n"

#define ID_ABOUT_BODY       71
#define MSG_ABOUT_BODY      "\n\33cThis program is free software; you can\n"\
" redistribute it and/or modify it under the \n"\
"terms of the GNU General Public License as\n"\
"published by the Free Software Foundation;\n"\
"either version 2 of the License, or at\n"\
"your option) any later version.\n\n"\
"This program is distributed in the hope\n"\
"that it will be useful,but WITHOUT ANY\n"\
"WARRANTY; without even the implied\n"\
"    warranty of MERCHANTABILITY or FITNESS FOR    \n"\
"A PARTICULAR PURPOSE. See the GNU General\n"\
"Public License for more details.\n\n"\
"You should have received a copy of the GNU\n"\
"General Public License along with this\n"\
"program; if not, write to\n\n"\
"Free Software Foundation, Inc.,\n"\
"59 Temple Place,\n"\
"Suite 330,\n"\
"Boston, MA\n"\
"02111-1307\n"\
"USA\n\n"\
"Or email the original author"

//#define ID_                 72
//#define MSG_



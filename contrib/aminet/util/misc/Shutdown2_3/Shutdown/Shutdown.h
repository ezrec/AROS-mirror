#ifndef SHUTDOWN_H
#define SHUTDOWN_H


/****************************************************************************/


/* This file was created automatically by CatComp.
 * Do NOT edit by hand!
 */


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif


/****************************************************************************/


#define MSG_HEADER_TXT 0
#define MSG_HEADER_TXT_STR "%s © Copyright by Olaf Barthel, all rights reserved.\n"

#define MSG_REMOVING_TXT 1
#define MSG_REMOVING_TXT_STR "Removing monitoring tasks... "

#define MSG_INSTALLING_TXT 2
#define MSG_INSTALLING_TXT_STR "Installing monitoring tasks... "

#define MSG_TASKNAME_NAME 3
#define MSG_TASKNAME_NAME_STR "« File monitor »"

#define MSG_DEVTASKNAME_NAME 4
#define MSG_DEVTASKNAME_NAME_STR "« Device monitor »"

#define MSG_DONE_TXT 5
#define MSG_DONE_TXT_STR "done.\n"

#define MSG_FAILED_TXT 6
#define MSG_FAILED_TXT_STR "failed!\a\n"

#define MSG_OPENFAIL_TXT 7
#define MSG_OPENFAIL_TXT_STR "Could not open \"%s\"!\a\n"

#define MSG_TURNOFF_TXT 8
#define MSG_TURNOFF_TXT_STR "You may now turn off your Amiga"

#define MSG_WAIT_A_MINUTE_TXT 9
#define MSG_WAIT_A_MINUTE_TXT_STR "Wait a minute... devices are still busy"

#define MSG_RETURN_TXT 10
#define MSG_RETURN_TXT_STR "Return to _Workbench"

#define MSG_RESTART_TXT 11
#define MSG_RESTART_TXT_STR "_Restart the Amiga"

#define MSG_RETURN_SHORTCUT 12
#define MSG_RETURN_SHORTCUT_STR "W"

#define MSG_RESTART_SHORTCUT 13
#define MSG_RESTART_SHORTCUT_STR "R"

#define MSG_FILESOPEN1_TXT 14
#define MSG_FILESOPEN1_TXT_STR "Stop! This file is still open"

#define MSG_FILESOPEN2_TXT 15
#define MSG_FILESOPEN2_TXT_STR "Stop! These files are still open"

#define MSG_WORKING1_TXT 16
#define MSG_WORKING1_TXT_STR "Stop! This program is still busy"

#define MSG_WORKING2_TXT 17
#define MSG_WORKING2_TXT_STR "Stop! These programs are still busy"

#define MSG_RESET_TXT 18
#define MSG_RESET_TXT_STR "Restarting..."

#define MSG_BROKERNAME_TXT 19
#define MSG_BROKERNAME_TXT_STR "« Restart »"

#define MSG_BROKERTITLE_TXT 20
#define MSG_BROKERTITLE_TXT_STR "Restart"

#define MSG_BROKERDESCRIPTION_TXT 21
#define MSG_BROKERDESCRIPTION_TXT_STR "Restarts the Amiga"

#define MSG_PROCESS_CREATION_FAILED_TXT 22
#define MSG_PROCESS_CREATION_FAILED_TXT_STR "failed (could not install file monitor)!\a\n"

#define MSG_HOTKEYS_FAILED_TXT 23
#define MSG_HOTKEYS_FAILED_TXT_STR "failed (could not install keyboard handler, error code #%ld)!\a\n"


/****************************************************************************/


#ifdef STRINGARRAY

struct AppString
{
    LONG   as_ID;
    STRPTR as_Str;
};

struct AppString AppStrings[] =
{
    {MSG_HEADER_TXT,MSG_HEADER_TXT_STR},
    {MSG_REMOVING_TXT,MSG_REMOVING_TXT_STR},
    {MSG_INSTALLING_TXT,MSG_INSTALLING_TXT_STR},
    {MSG_TASKNAME_NAME,MSG_TASKNAME_NAME_STR},
    {MSG_DEVTASKNAME_NAME,MSG_DEVTASKNAME_NAME_STR},
    {MSG_DONE_TXT,MSG_DONE_TXT_STR},
    {MSG_FAILED_TXT,MSG_FAILED_TXT_STR},
    {MSG_OPENFAIL_TXT,MSG_OPENFAIL_TXT_STR},
    {MSG_TURNOFF_TXT,MSG_TURNOFF_TXT_STR},
    {MSG_WAIT_A_MINUTE_TXT,MSG_WAIT_A_MINUTE_TXT_STR},
    {MSG_RETURN_TXT,MSG_RETURN_TXT_STR},
    {MSG_RESTART_TXT,MSG_RESTART_TXT_STR},
    {MSG_RETURN_SHORTCUT,MSG_RETURN_SHORTCUT_STR},
    {MSG_RESTART_SHORTCUT,MSG_RESTART_SHORTCUT_STR},
    {MSG_FILESOPEN1_TXT,MSG_FILESOPEN1_TXT_STR},
    {MSG_FILESOPEN2_TXT,MSG_FILESOPEN2_TXT_STR},
    {MSG_WORKING1_TXT,MSG_WORKING1_TXT_STR},
    {MSG_WORKING2_TXT,MSG_WORKING2_TXT_STR},
    {MSG_RESET_TXT,MSG_RESET_TXT_STR},
    {MSG_BROKERNAME_TXT,MSG_BROKERNAME_TXT_STR},
    {MSG_BROKERTITLE_TXT,MSG_BROKERTITLE_TXT_STR},
    {MSG_BROKERDESCRIPTION_TXT,MSG_BROKERDESCRIPTION_TXT_STR},
    {MSG_PROCESS_CREATION_FAILED_TXT,MSG_PROCESS_CREATION_FAILED_TXT_STR},
    {MSG_HOTKEYS_FAILED_TXT,MSG_HOTKEYS_FAILED_TXT_STR},
};


#endif /* STRINGARRAY */


/****************************************************************************/


#endif /* SHUTDOWN_H */

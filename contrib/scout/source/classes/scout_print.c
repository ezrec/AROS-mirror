/**
 * Scout - The Amiga System Monitor
 *
 *------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *------------------------------------------------------------------
 *
 * @author Andreas Gelhausen
 * @author Richard Körber <rkoerber@gmx.de>
 */

#include "system_headers.h"

STATIC TEXT initialdrawer[TEXT_LENGTH];
STATIC TEXT initialfile[FILENAME_LENGTH] = "PRT:";
STATIC TEXT printfilename[FILENAME_LENGTH] = "PRT:";

ULONG VARARGS68K STDARGS MyRequest( STRPTR gadgets,
                                    STRPTR fmt, ... )
{
    ULONG result = 0;
    STRPTR buf;
    VA_LIST args;
    struct EasyStruct myreqES;
    struct Window *myreq_window;

    VA_START(args, fmt);
#if defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)
    if ((buf = VASPrintf(fmt, args)) != NULL) {
#else
    if ((buf = AllocVec(TMP_STRING_LENGTH, MEMF_ANY)) != NULL) {
        _vsnprintf(buf, TMP_STRING_LENGTH, fmt, VA_ARG(args, APTR));
#endif
        myreq_window = (struct Window *)xget(WI_Main, MUIA_Window_Window);

        myreqES.es_StructSize = sizeof(struct EasyStruct);
        myreqES.es_Flags = 0;
        myreqES.es_Title = txtRequestTitle;
        myreqES.es_TextFormat = buf;
        myreqES.es_GadgetFormat = gadgets;

        ApplicationSleep(TRUE);
        result = EasyRequest(myreq_window, &myreqES, NULL);
        ApplicationSleep(FALSE);

        FreeVec(buf);
    }

    VA_END(args);

    return result;
}

STATIC BOOL FileRequest (void)
{
    struct Window *aslfr_window;
    struct FileRequester *requester;
    BOOL result = FALSE;

    aslfr_window = (struct Window *)xget(WI_Main, MUIA_Window_Window);

    if (aslfr_window) {
        if ((requester = MUI_AllocAslRequestTags(ASL_FileRequest, ASLFR_Window, aslfr_window,
                                                          ASLFR_PrivateIDCMP, TRUE,
                                                          ASLFR_SleepWindow, TRUE,
                                                          TAG_DONE)) != NULL) {
            result = MUI_AslRequestTags(requester, ASLFR_TitleText, txtChooseFile,
                                               ASLFR_InitialDrawer, initialdrawer,
                                               ASLFR_InitialFile, initialfile,
                                               ASLFR_DoSaveMode, TRUE,
                                               TAG_DONE);

            if (result)
            {
                stccpy(initialdrawer, requester->fr_Drawer, sizeof(initialdrawer));
                stccpy(initialfile, requester->fr_File, sizeof(initialfile));
            }

            stccpy(printfilename, initialdrawer, sizeof(printfilename));
            AddPart(printfilename, initialfile, sizeof(printfilename));

            MUI_FreeAslRequest(requester);
        }
    }

    return result;
}

STATIC BPTR PrintHandle;
LONG PrintHandleMode = MODE_NEWFILE;

BPTR HandlePrintStart( STRPTR filename )
{
    PrintHandle = ZERO;

    if (AP_Scout) {
        if (filename == NULL && !FileRequest()) {
            return ZERO;
        }

        ApplicationSleep(TRUE);
    } else if (filename == NULL || filename[0] == '\0') {
        return Output();
    }

    if (filename) {
        stccpy(printfilename, filename, sizeof(printfilename));
    }

    if ((PrintHandle = Open(printfilename, PrintHandleMode)) != ZERO) {
        if (PrintHandleMode == MODE_OLDFILE) {
            PrintHandle = Open(printfilename, MODE_NEWFILE);
        }
        if (PrintHandle == ZERO) {
            MyRequest(msgErrorContinue, msgCouldNotOpenFile, printfilename);
        }
    }
    if (PrintHandle != ZERO) {
        Seek(PrintHandle, 0, OFFSET_END);
    }

    return PrintHandle;
}

void HandlePrintStop( void )
{
    if (AP_Scout) {
        ApplicationSleep(FALSE);
    }
    if (PrintHandle != ZERO && PrintHandle != Output()) {
        Close(PrintHandle);
    }
}


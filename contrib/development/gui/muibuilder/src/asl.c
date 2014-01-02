/***************************************************************************

  MUIBuilder - MUI interface builder
  Copyright (C) 1990-2009 by Eric Totel
  Copyright (C) 2010-2011 by MUIBuilder Open Source Team

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  MUIBuilder Support Site: http://sourceforge.net/projects/muibuilder/

  $Id$$

***************************************************************************/

#include "builder.h"

BOOL LoadRequester(CONST_STRPTR title, char *dir, char *file)
{
    struct FileRequester *req;
    struct Window *win = NULL;
    BOOL result = FALSE;

    if (req = MUI_AllocAslRequestTags(ASL_FileRequest, TAG_DONE))
    {
        get(WI_current, MUIA_Window_Window, &win);
        if (MUI_AslRequestTags(req,
                               ASLFO_PrivateIDCMP, TRUE,
                               ASLFO_TitleText, title,
                               ASLFR_InitialFile, file,
                               ASLFR_InitialDrawer, dir,
                               ASLFO_Window, win,
                               ASLFR_RejectIcons, TRUE, TAG_DONE))
        {
            strncpy(dir, req->fr_Drawer, 255);
            strncpy(file, req->fr_File, 255);
            result = TRUE;
        }
    }
    MUI_FreeAslRequest(req);
    return (result);
}

BOOL SaveRequester(CONST_STRPTR title, char *dir, char *file)
{
    struct FileRequester *req;
    struct Window *win = NULL;
    BOOL result = FALSE;

    if (req = MUI_AllocAslRequestTags(ASL_FileRequest, TAG_DONE))
    {
        get(WI_current, MUIA_Window_Window, &win);
        if (MUI_AslRequestTags(req,
                               ASLFO_PrivateIDCMP, TRUE,
                               ASLFO_TitleText, title,
                               ASLFR_InitialFile, file,
                               ASLFR_InitialDrawer, dir,
                               ASLFO_Window, win,
                               ASLFR_RejectIcons, TRUE,
                               ASLFR_DoSaveMode, TRUE, TAG_DONE))
        {
            strncpy(dir, req->fr_Drawer, 255);
            strncpy(file, req->fr_File, 255);
            result = TRUE;
        }
    }
    MUI_FreeAslRequest(req);
    return (result);
}

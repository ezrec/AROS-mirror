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
#include <exec/memory.h>

int nb_modules = 0;

BOOL RecordFileInfoBlock(struct FileInfoBlock *fib)
{
    char *aux;

    if (strncmp(fib->fib_FileName, "GenCode", 7) == 0)
    {
        aux =
            AllocVec(strlen(fib->fib_FileName) + 1,
                     MEMF_PUBLIC | MEMF_CLEAR);
        if (aux)
        {
            strcpy(aux, &((char *) fib->fib_FileName)[7]);
            PopModules[nb_modules++] = aux;
            PopModules[nb_modules] = NULL;
        }
    }
    return (TRUE);
}


static void ScanDir(BPTR DirLock)
{
    struct FileInfoBlock *fib;
    BOOL Ok = TRUE;
    BPTR OldDir;
    char Name[32], errbuf[80];

    if (!
        (fib =
         AllocMem(sizeof(struct FileInfoBlock), MEMF_PUBLIC | MEMF_CLEAR)))
        return;
    if (Examine(DirLock, fib))
    {
        strcpy(Name, fib->fib_FileName);
        OldDir = CurrentDir(DirLock);
        if (fib->fib_DirEntryType > 0)
        {
            while (ExNext(DirLock, fib)
                   && (Ok = RecordFileInfoBlock(fib))) ;
            if (Ok && IoErr() != ERROR_NO_MORE_ENTRIES)
            {
                Fault(IoErr(), NULL, errbuf, 79);
                ErrorMessageEasy(GetMUIBuilderString(MSG_ErrorReading));
            }
        }
        CurrentDir(OldDir);
    }
    FreeMem(fib, sizeof(struct FileInfoBlock));
}


void ScanAvailableModules(void)
{
    BPTR DirLock;

    if (DirLock = Lock("PROGDIR:Modules", ACCESS_READ))
    {
        ScanDir(DirLock);
        UnLock(DirLock);
    }
}

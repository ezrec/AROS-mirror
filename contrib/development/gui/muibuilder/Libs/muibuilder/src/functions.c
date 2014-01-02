/***************************************************************************

  muibuilder.library - Support library for MUIBuilder's code generators
  Copyright (C) 1990-2009 by Eric Totel
  Copyright (C) 2010-2011 by MUIBuilder Open Source Team

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  MUIBuilder Support Site: http://sourceforge.net/projects/muibuilder/

  $Id$

***************************************************************************/

/* Prototypes */
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>

/* ANSI */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "functions.h"
#include "lib.h"


struct Var
{
    char *Type;
    char *Name;
    char *Init;
    ULONG Size;
};

/* renseignements sur le fichier en memoire */
int length;
char *adr_declarations;

/* donnees dans le fichiers */
char *File = NULL;
char *Catalog = NULL;
char *GetString = NULL;
BOOL Code, Env, Locale, Declarations, Notifications, GenerateAppli;

/* localisation dans le fichier */
char *variables = NULL;
char *inits = NULL;
char *MUIcode = NULL;
char *MUIcodeBegin = NULL;
char *Notifycode = NULL;
char *NotifycodeBegin = NULL;
char *CodeEnd = NULL;
char *NotifyCodeEnd = NULL;
struct Var *Vars = NULL;
int varnum = 0;

/* TRUE if MB_Open was successful */
BOOL rights = FALSE;

struct DosLibrary *DOSBase;

#ifdef __AROS__
struct UtilityBase *UtilityBase = NULL;
#else
struct UtilityBase *UtilityBase = NULL;
struct UtilityBase *__UtilityBase = NULL; // required by clib2 & libnix
#endif

ULONG initBase(struct LibraryHeader *lib)
{
    BOOL retval = FALSE;
    DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 0);
    UtilityBase = (struct UtilityBase *)OpenLibrary("utility.library", 0);

    #if !defined(__NEWLIB__) && !defined(__AROS__)
    __UtilityBase = (APTR)UtilityBase;
    #endif

    if (DOSBase && UtilityBase)
    {
        retval = TRUE;
    }
    return retval;
}

ULONG freeBase(struct LibraryHeader * lib)
{
    CloseLibrary((struct Library *)DOSBase);
    CloseLibrary((struct Library *)UtilityBase);
    return TRUE;
}


LIBFUNC BOOL MB_Open(void)
{
    BPTR TMPfile;
    BOOL success = TRUE;
    struct FileInfoBlock *Info;
    char *adr_file;

    char *ptrvar, *ptrinit, *InitPtr;
    ULONG size;
    int i;

    Notifycode = NULL;
    MUIcode = NULL;
    adr_declarations = NULL;
    File = NULL;

    Info = AllocDosObjectTagList(DOS_FIB, NULL);
    if (!Info)
    {
        rights = FALSE;
        return FALSE;
    }

    /* File with options */
    if ((TMPfile = Open("T:MUIBuilder1.tmp", MODE_OLDFILE)) != BNULL)
    {
        ExamineFH(TMPfile, Info);
        length = Info->fib_Size;
        adr_file = AllocVec(length, MEMF_PUBLIC | MEMF_CLEAR);
        if (adr_file)
            Read(TMPfile, adr_file, length);
        Close(TMPfile);

        /* Initialisations */
        if (adr_file)
        {
            File = adr_file;
            GetString = File + strlen(File) + 1;
            Catalog = GetString + strlen(GetString) + 1;
            adr_file = Catalog + strlen(Catalog) + 1;
            Env = ('1' == *adr_file++);
            Declarations = ('1' == *adr_file++);
            Code = ('1' == *adr_file++);
            Locale = ('1' == *adr_file++);
            Notifications = ('1' == *adr_file++);
            GenerateAppli = ('1' == *adr_file++);
        } else
            success = FALSE;
    } else
        success = FALSE;

    /* File with declarations of variables */
    if ((TMPfile = Open("T:MUIBuilder2.tmp", MODE_OLDFILE)) != BNULL)
    {
        ExamineFH(TMPfile, Info);
        length = Info->fib_Size;
        adr_declarations = AllocVec(length, MEMF_PUBLIC | MEMF_CLEAR);
        if (adr_declarations)
            Read(TMPfile, adr_declarations, length);
        Close(TMPfile);

        /* Initialisations */
        if (adr_declarations)
        {
            adr_file = adr_declarations;
            varnum = atoi(adr_file);
            variables = adr_file + strlen(adr_file) + 1;
        } else
            success = FALSE;
    } else
        success = FALSE;

    /* File with initializations of variables */
    if ((TMPfile = Open("T:MUIBuilder3.tmp", MODE_OLDFILE)) != BNULL)
    {
        ExamineFH(TMPfile, Info);
        length = Info->fib_Size;
        inits = AllocVec(length, MEMF_PUBLIC | MEMF_CLEAR);
        if (inits)
            Read(TMPfile, inits, length);
        Close(TMPfile);
        success = (inits != NULL);
    } else
        success = FALSE;

    if (success)
    {
        Vars =
            AllocVec(varnum * sizeof(struct Var),
                     MEMF_PUBLIC | MEMF_CLEAR);
        ptrvar = variables;
        ptrinit = inits;
        for (i = 0; i < varnum; i++)
        {
            size = 0;
            InitPtr = ptrinit;
            switch (*ptrvar)
            {
                case TYPEVAR_TABSTRING:
                    do
                    {
                        size++;
                        ptrinit = ptrinit + strlen(ptrinit) + 1;
                    }
                    while (strlen(ptrinit) != 0);
                    ptrinit++;
                    break;
                case TYPEVAR_STRING:
                    size = strlen(ptrinit);
                default:
                    ptrinit = ptrinit + strlen(ptrinit) + 1;
                    break;
            }
            Vars[i].Name = ptrvar + 1;
            Vars[i].Size = size;
            Vars[i].Type = ptrvar++;
            Vars[i].Init = InitPtr;
            ptrvar = ptrvar + strlen(ptrvar) + 1;;
        }
    }

    /* File with MUI code */
    if ((TMPfile = Open("T:MUIBuilder4.tmp", MODE_OLDFILE)) != BNULL)
    {
        ExamineFH(TMPfile, Info);
        length = Info->fib_Size;
        MUIcode = AllocVec(length, MEMF_PUBLIC | MEMF_CLEAR);
        if (MUIcode)
            Read(TMPfile, MUIcode, length);
        Close(TMPfile);
        if (MUIcode)
        {
            CodeEnd = (char *) MUIcode + length;
            MUIcodeBegin = MUIcode;
        } else
            success = FALSE;
    } else
        success = FALSE;

    /* File with notifications */
    if ((TMPfile = Open("T:MUIBuilder5.tmp", MODE_OLDFILE)) != BNULL)
    {
        ExamineFH(TMPfile, Info);
        length = Info->fib_Size;
        Notifycode = AllocVec(length, MEMF_PUBLIC | MEMF_CLEAR);
        if (Notifycode)
            Read(TMPfile, Notifycode, length);
        Close(TMPfile);
        if (Notifycode)
        {
            NotifycodeBegin = Notifycode;
            NotifyCodeEnd = (char *) Notifycode + length;
        } else
        {
            Notifycode = 0;
            NotifyCodeEnd = 0;
        }
    }

    FreeDosObject(DOS_FIB, Info);

    rights = success;

    if (!success)
    {
        if (Notifycode)
            FreeVec(Notifycode);
        if (MUIcode)
            FreeVec(MUIcode);
        if (inits)
            FreeVec(inits);
        if (adr_declarations)
            FreeVec(adr_declarations);
        if (File)
            FreeVec(File);
    }
    return success;
}

LIBFUNC void MB_Close(void)
{
    if (rights)
    {
        if (File)
            FreeVec(File);
        if (adr_declarations)
            FreeVec(adr_declarations);
        if (inits)
            FreeVec(inits);
        if (MUIcodeBegin)
            FreeVec(MUIcodeBegin);
        if (NotifycodeBegin)
            FreeVec(NotifycodeBegin);
        if (Vars)
            FreeVec(Vars);
    }
}

LIBFUNC void MB_GetA(REG(a1, struct TagItem *TagList))
{
    BOOL *TagBool;
    char **TagString;
    ULONG *TagInt;

    if (!rights)
        return;

    TagBool = (BOOL *) GetTagData(MUIB_Code, 0, TagList);
    if (TagBool)
        *TagBool = Code;

    TagBool = (BOOL *) GetTagData(MUIB_Environment, 0, TagList);
    if (TagBool)
        *TagBool = Env;

    TagBool = (BOOL *) GetTagData(MUIB_Locale, 0, TagList);
    if (TagBool)
        *TagBool = Locale;

    TagBool = (BOOL *) GetTagData(MUIB_Declarations, 0, TagList);
    if (TagBool)
        *TagBool = Declarations;

    TagBool = (BOOL *) GetTagData(MUIB_Notifications, 0, TagList);
    if (TagBool)
        *TagBool = Notifications;

    TagBool = (BOOL *) GetTagData(MUIB_Application, 0, TagList);
    if (TagBool)
        *TagBool = GenerateAppli;

    TagString = (char **) GetTagData(MUIB_FileName, 0, TagList);
    if (TagString)
        *TagString = File;

    TagString = (char **) GetTagData(MUIB_CatalogName, 0, TagList);
    if (TagString)
        *TagString = Catalog;

    TagString = (char **) GetTagData(MUIB_GetStringName, 0, TagList);
    if (TagString)
        *TagString = GetString;

    TagInt = (ULONG *) GetTagData(MUIB_VarNumber, 0, TagList);
    if (TagInt)
        *TagInt = varnum;
}

LIBFUNC void MB_GetVarInfoA(REG(d0, ULONG varnb),
                            REG(a1, struct TagItem *TagList))
{
    char **TagString;
    LONG *TagInt;
    IPTR *TagPtr;

    if (!rights)
        return;

    /*
        Some #?.MUIB files create in T:MUIBuilder4.tmp references
        to unexisting variables. To avoid a crash we are returning
        a variable with a special name.
    */
    TagInt = (ULONG *) GetTagData(MUIB_VarType, 0, TagList);
    if (TagInt)
    {
        if (varnb < varnum)
        {
            *TagInt = (ULONG) * (Vars[varnb].Type);
        }
        else
        {
            *TagInt = 0;
        }
    }

    TagString = (char **) GetTagData(MUIB_VarName, 0, TagList);
    if (TagString)
    {
        if (varnb < varnum)
        {
            *TagString = Vars[varnb].Name;
        }
        else
        {
            *TagString = "ERROR_REFERENCE_TO_UNEXISTING_VARIABLE";
        }
    }

    TagInt = (ULONG *) GetTagData(MUIB_VarSize, 0, TagList);
    if (TagInt)
    {
        if (varnb < varnum)
        {
            *TagInt = Vars[varnb].Size;
        }
        else
        {
            *TagInt = 0;
        }
    }

    TagPtr = (IPTR *) GetTagData(MUIB_VarInitPtr, 0, TagList);
    if (TagPtr)
    {
        if (varnb < varnum)
        {
            *TagPtr = (IPTR) Vars[varnb].Init;
        }
        else
        {
            *TagPtr = 0;
        }
    }
}

LIBFUNC void MB_GetNextCode(REG(a0, ULONG * type), REG(a1, char **code))
{
    char *typecode;

    if (!rights)
        return;

    if (MUIcode >= CodeEnd)
    {
        *type = -1;
        *code = NULL;
    } else
    {
        typecode = MUIcode;
        MUIcode = (char *) MUIcode + 1;
        *type = (ULONG) * typecode;
        *code = MUIcode;
        MUIcode = (char *) MUIcode + strlen((char *) MUIcode) + 1;
    }
}

LIBFUNC void MB_GetNextNotify(REG(a0, ULONG * type), REG(a1, char **code))
{
    char *typecode;

    if (!rights)
        return;

    if (Notifycode >= NotifyCodeEnd)
    {
        *type = -1;
        *code = NULL;
    } else
    {
        typecode = Notifycode;
        Notifycode = (char *) Notifycode + 1;
        *type = *typecode;
        *code = Notifycode;
        Notifycode = (char *) Notifycode + strlen((char *) Notifycode) + 1;
    }
}

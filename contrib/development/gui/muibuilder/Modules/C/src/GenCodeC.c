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

  $Id$

***************************************************************************/

/* Prototypes */
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/muibuilder.h>

#include <libraries/muibuilder.h>

/* ANSI */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* File name utilities */
#include "file.h"

/* Compatibility macros */
#include "SDI_compiler.h"


char *version = "$VER: GenCodeC 2.3 (16.01.2011)";

struct Library *MUIBBase = NULL;
struct DosLibrary *DOSBase = NULL;

/* Global variables */
ULONG varnb;                    /* number of variables */

BOOL Code, Env;                 /* flags-options */
BOOL Locale, Declarations;
BOOL Notifications;
BOOL ExternalExist = FALSE;
char *FileName, *CatalogName;   /* Strings */
char *GetString;
char *GetMBString;

FILE *file;

char HeaderFile[512];
char GUIFile[512];
char MBDir[512];
char Externals[512];
char MainFile[512];

/* variable types */
char *STR_type[] = {
    "BOOL",             // TYPEVAR_BOOL
    "int",              // TYPEVAR_INT
    "CONST_STRPTR",     // TYPEVAR_STRING
    "CONST_STRPTR",     // TYPEVAR_TABSTRING
    "APTR",             // TYPEVAR_PTR
    "",                 // TYPEVAR_HOOK
    "",                 // TYPEVAR_IDENT
    "",                 // TYPEVAR_EXTERNAL
    "APTR",             // TYPEVAR_LOCAL_PTR
    "APTR"              // TYPEVAR_EXTERNAL_PTR
};

static void End(void)
{
    MB_Close();                 /* Free Memory and Close Temporary Files */
    if (MUIBBase)
        CloseLibrary(MUIBBase);
    if (DOSBase)
        CloseLibrary((struct Library *) DOSBase);
}

static void Indent(int nb)
{
    int i;

    for (i = 0; i < nb; i++)
        fprintf(file, "\t");
}

static void WriteParameters(void)
{
    int i;
    char *varname, *typename;
    ULONG type, size;
    BOOL comma = FALSE;

    typename = STR_type[TYPEVAR_EXTERNAL_PTR - 1];
    if (Notifications)
    {
        for (i = 0; i < varnb; i++)
        {
            MB_GetVarInfo(i,
                          MUIB_VarType, (IPTR) &type,
                          MUIB_VarName, (IPTR) &varname,
                          MUIB_VarSize, (IPTR) &size, TAG_END);
            if (type == TYPEVAR_EXTERNAL_PTR)
            {
                if (comma)
                    fprintf(file, ", ");
                comma = TRUE;
                fprintf(file, "%s %s", typename, varname);
            }
        }
    }
    if (!comma)
        fprintf(file, "void");
}

static void WriteDeclarations(int vartype)
{
    int i;
    char *varname;
    ULONG type, size;
    char *typename;
    int nb_ident = 1;
    char buffer[150];

    typename = STR_type[vartype - 1];   /* find the name 'BOOL ...' */
    buffer[0] = '\0';
    for (i = 0; i < varnb; i++)
    {
        MB_GetVarInfo(i,
                      MUIB_VarType, (IPTR) &type,
                      MUIB_VarName, (IPTR) &varname,
                      MUIB_VarSize, (IPTR) &size, TAG_END);
        if (type == vartype)
        {
            switch (type)
            {
                case TYPEVAR_TABSTRING:
                    fprintf
                        (file,
                         "\t%s\t%s[%u];\n", typename, varname, (unsigned int)size + 1);
                    break;
                case TYPEVAR_IDENT:
                    fprintf(file, "#define %s %d\n", varname, nb_ident++);
                    break;
                case TYPEVAR_LOCAL_PTR:
                    if (strlen(buffer) == 0)
                        sprintf(buffer, "\t%s\t%s", typename, varname);
                    else
                    {
                        strcat(buffer, ", ");
                        strcat(buffer, varname);
                    }
                    if (strlen(buffer) >= 70)
                    {
                        strcat(buffer, ";\n");
                        fprintf(file, "%s", buffer);
                        buffer[0] = '\0';
                    }
                    break;
                default:
                    fprintf(file, "\t%s\t%s;\n", typename, varname);
                    break;
            }
        }
    }
    if (strlen(buffer) > 0)
        fprintf(file, "%s;\n", buffer);
}

static void WriteInitialisations(int vartype)
{
    int i, j;
    ULONG type, size;
    char *inits, *name;
    BOOL enter = FALSE;

    for (i = 0; i < varnb; i++)
    {
        MB_GetVarInfo(i,
                      MUIB_VarType, (IPTR) &type,
                      MUIB_VarName, (IPTR) &name,
                      MUIB_VarSize, (IPTR) &size,
                      MUIB_VarInitPtr, (IPTR) &inits, TAG_END);
        if (type == vartype)
        {
            enter = TRUE;
            switch (type)
            {
                case TYPEVAR_TABSTRING:
                    for (j = 0; j < size; j++)
                    {
                        if (!Locale)
                            fprintf(file, "\tobject->%s[%d] = \"%s\";\n",
                                    name, j, inits);
                        else
                            fprintf(file, "\tobject->%s[%d] = %s(%s);\n",
                                    name, j, GetMBString, inits);
                        inits = inits + strlen(inits) + 1;
                    }
                    fprintf(file, "\tobject->%s[%d] = NULL;\n", name, j);
                    break;
                case TYPEVAR_STRING:
                    if (*inits != 0)
                    {
                        if (!Locale)
                            fprintf(file, "\tobject->%s = \"%s\";\n", name,
                                    inits);
                        else
                            fprintf(file, "\tobject->%s = %s(%s);\n", name,
                                    GetMBString, inits);
                    }
                    else
                        fprintf(file, "\tobject->%s = NULL;\n", name);
                    break;
                case TYPEVAR_HOOK:
                    /*
                        We can't use the SDI macros here because they create "static" functions
                        but for MUIBuilder we need external functions
                    */
                    fprintf(file,
                            "#if defined(__amigaos4__)\n");
                    fprintf(file,
                            "\tstatic const struct Hook %sHook = { { NULL,NULL }, (HOOKFUNC)%s, NULL, NULL };\n",
                            name, name);
                    fprintf(file,
                            "#else\n");
                    fprintf(file,
                            "\tstatic const struct Hook %sHook = { { NULL,NULL }, HookEntry, (HOOKFUNC)%s, NULL };\n",
                            name, name);
                    fprintf(file,
                            "#endif\n");
                    break;
                default:
                    break;
            }
        }
    }
    if (enter)
        fprintf(file, "\n");
}

static void WriteCode(void)
{
    ULONG type;
    char *code;
    BOOL InFunction = FALSE;
    BOOL IndentFunction = TRUE;
    BOOL obj_function = FALSE;
    BOOL InObj = FALSE;
    int nb_indent = 1;
    int nb_function = 0;
    int name;

    MB_GetNextCode(&type, &code);
    while (type != -1)
    {
        switch (type)
        {
            case TC_CREATEOBJ:
                name = atoi(code);
                fprintf(file, "%s,\n", MUIStrings[name]);
                nb_indent++;
                IndentFunction = TRUE;
                MB_GetNextCode(&type, &code);
                InObj = TRUE;
                break;
            case TC_ATTRIBUT:
                Indent(nb_indent);
                name = atoi(code);
                fprintf(file, "%s, ", MUIStrings[name]);
                IndentFunction = FALSE;
                MB_GetNextCode(&type, &code);
                break;
            case TC_END:
                nb_indent--;
                InObj = FALSE;
                Indent(nb_indent);
                name = atoi(code);
                fprintf(file, "%s", MUIStrings[name]);
                IndentFunction = TRUE;
                MB_GetNextCode(&type, &code);
                fprintf(file, ";\n\n");
                break;
            case TC_MUIARG_OBJFUNCTION:
                if (IndentFunction)
                    Indent(nb_indent);
                nb_function++;
                name = atoi(code);
                fprintf(file, "%s(", MUIStrings[name]);
                IndentFunction = FALSE;
                InFunction = TRUE;
                MB_GetNextCode(&type, &code);
                obj_function = TRUE;
                InFunction = TRUE;
                break;
            case TC_MUIARG_FUNCTION:
            case TC_FUNCTION:
                if (IndentFunction)
                    Indent(nb_indent);
                nb_function++;
                name = atoi(code);
                fprintf(file, "%s(", MUIStrings[name]);
                IndentFunction = FALSE;
                InFunction = TRUE;
                MB_GetNextCode(&type, &code);
                obj_function = FALSE;
                break;
            case TC_OBJFUNCTION:
                if (IndentFunction)
                    Indent(nb_indent);
                nb_function++;
                name = atoi(code);
                fprintf(file, "%s(", MUIStrings[name]);
                InFunction = TRUE;
                IndentFunction = FALSE;
                MB_GetNextCode(&type, &code);
                obj_function = TRUE;
                break;
            case TC_STRING:
                fprintf(file, "\"%s\"", code);
                MB_GetNextCode(&type, &code);
                IndentFunction = TRUE;
                if (InFunction)
                {
                    if (type != TC_END_FUNCTION)
                        fprintf(file, ", ");
                    IndentFunction = FALSE;
                }
                else
                    fprintf(file, ",\n");
                break;
            case TC_LOCALESTRING:
                fprintf(file, "%s(%s)", GetMBString, code);
                MB_GetNextCode(&type, &code);
                IndentFunction = TRUE;
                if (InFunction)
                {
                    if (type != TC_END_FUNCTION)
                        fprintf(file, ", ");
                    IndentFunction = FALSE;
                }
                else
                    fprintf(file, ",\n");
                break;
            case TC_LOCALECHAR:
                fprintf(file, "%s(%s)[0]", GetString, code);
                MB_GetNextCode(&type, &code);
                IndentFunction = TRUE;
                if (InFunction)
                {
                    if (type != TC_END_FUNCTION)
                        fprintf(file, ", ");
                    IndentFunction = FALSE;
                }
                else
                    fprintf(file, ",\n");
                break;
            case TC_INTEGER:
                fprintf(file, "%s", code);
                MB_GetNextCode(&type, &code);
                IndentFunction = TRUE;
                if (InFunction)
                {
                    if (type != TC_END_FUNCTION)
                        fprintf(file, ", ");
                    IndentFunction = FALSE;
                }
                else
                    fprintf(file, ",\n");
                break;
            case TC_CHAR:
                fprintf(file, "'%s'", code);
                MB_GetNextCode(&type, &code);
                IndentFunction = TRUE;
                if (InFunction)
                {
                    if (type != TC_END_FUNCTION)
                        fprintf(file, ", ");
                    IndentFunction = FALSE;
                }
                else
                    fprintf(file, ",\n");
                break;
            case TC_VAR_AFFECT:
                name = atoi(code);
                MB_GetVarInfo(name, MUIB_VarName, (IPTR) &code,
                              MUIB_VarType, (IPTR) &type, TAG_END);
                if (type == TYPEVAR_LOCAL_PTR)
                    fprintf(file, "\t%s = ", code);
                else
                    fprintf(file, "\tobject->%s = ", code);
                IndentFunction = FALSE;
                MB_GetNextCode(&type, &code);
                break;
            case TC_OBJ_ARG:
            case TC_VAR_ARG:
                name = atoi(code);
                MB_GetVarInfo(name, MUIB_VarName, (IPTR) &code,
                              MUIB_VarType, (IPTR) &type, TAG_END);
                if (type == TYPEVAR_LOCAL_PTR)
                    fprintf(file, "%s", code);
                else
                    fprintf(file, "object->%s", code);
                MB_GetNextCode(&type, &code);
                if ((InFunction) && (type != TC_END_FUNCTION))
                    fprintf(file, ", ");
                if (!InFunction)
                {
                    fprintf(file, ",\n");
                    IndentFunction = TRUE;
                }
                break;
            case TC_END_FUNCTION:
                MB_GetNextCode(&type, &code);
                if (nb_function > 1)
                {
                    if (type != TC_END_FUNCTION)
                        fprintf(file, "),");
                    else
                        fprintf(file, ")");
                }
                else
                {
                    if (obj_function)
                        fprintf(file, ");\n\n");
                    else
                        fprintf(file, "),\n");
                    IndentFunction = TRUE;
                    InFunction = FALSE;
                    obj_function = FALSE;
                }
                nb_function--;
                break;
            case TC_BOOL:
                if (*code == '0')
                    fprintf(file, "FALSE");
                else
                    fprintf(file, "TRUE");
                MB_GetNextCode(&type, &code);
                if (InFunction)
                {
                    if (type != TC_END_FUNCTION)
                    {
                        fprintf(file, ", ");
                        IndentFunction = FALSE;
                    }
                }
                else
                    fprintf(file, ",\n");
                break;
            case TC_MUIARG:
                if (IndentFunction)
                    Indent(nb_indent);
                name = atoi(code);
                fprintf(file, "%s", MUIStrings[name]);
                MB_GetNextCode(&type, &code);
                if (InFunction)
                {
                    if (type != TC_END_FUNCTION)
                    {
                        fprintf(file, ", ");
                        IndentFunction = FALSE;
                    }
                }
                else
                {
                    fprintf(file, ",\n");
                    IndentFunction = TRUE;
                }
                break;
            case TC_MUIARG_ATTRIBUT:
                if (IndentFunction)
                    Indent(nb_indent);
                name = atoi(code);
                MB_GetNextCode(&type, &code);
                if ((InObj))
                    fprintf(file, "%s,\n", MUIStrings[name]);
                else
                {
                    if (InFunction)
                    {
                        if (type != TC_END_FUNCTION)
                            fprintf(file, "%s,", MUIStrings[name]);
                        else
                            fprintf(file, "%s", MUIStrings[name]);
                    }
                    else
                    {
                        fprintf(file, "%s;\n\n", MUIStrings[name]);
                    }
                }
                break;
            case TC_MUIARG_OBJ:
                if (IndentFunction)
                    Indent(nb_indent);
                name = atoi(code);
                MB_GetNextCode(&type, &code);
                fprintf(file, "%s;\n\n", MUIStrings[name]);
                break;
            case TC_EXTERNAL_FUNCTION:
                fprintf(file, "&%sHook", code);
                MB_GetNextCode(&type, &code);
                if (InFunction)
                {
                    if (type != TC_END_FUNCTION)
                    {
                        fprintf(file, ", ");
                        IndentFunction = FALSE;
                    }
                }
                else
                {
                    fprintf(file, ",\n");
                    IndentFunction = TRUE;
                }
                break;
            default:
                printf("Type = %u\n", (unsigned int)type);
                printf
                    ("ERROR !!!!! THERE IS A PROBLEM WITH THIS FILE !!!\n");
                End();
                exit(1);
                break;
        }
    }
}

static void WriteNotify(void)
{
    ULONG type;
    char *code;
    int name;
    BOOL indent = FALSE;

    fprintf(file, "\n");
    MB_GetNextNotify(&type, &code);
    while (type != -1)
    {
        if (indent)
            fprintf(file, "\t\t");
        indent = TRUE;
        switch (type)
        {
            case TC_END_FUNCTION:
            case TC_END_NOTIFICATION:
                fprintf(file, ");\n\n");
                MB_GetNextNotify(&type, &code);
                indent = FALSE;
                break;
            case TC_BEGIN_NOTIFICATION:
                name = atoi(code);
                MB_GetVarInfo(name, MUIB_VarName, (IPTR) &code,
                              MUIB_VarType, (IPTR) &type, TAG_END);
                if (type == TYPEVAR_LOCAL_PTR)
                    fprintf(file, "\tDoMethod(%s,\n", code);
                else
                    fprintf(file, "\tDoMethod(object->%s,\n", code);
                MB_GetNextNotify(&type, &code);
                break;
            case TC_FUNCTION:
                name = atoi(code);
                fprintf(file, "\t%s(", MUIStrings[name]);
                MB_GetNextNotify(&type, &code);
                indent = FALSE;
                break;
            case TC_STRING:
                fprintf(file, "\"%s\"", code);
                MB_GetNextNotify(&type, &code);
                if ((type != TC_END_NOTIFICATION)
                    && (type != TC_END_FUNCTION))
                    fprintf(file, ",\n");
                else
                    fprintf(file, "\n");
                break;
            case TC_LOCALESTRING:
                fprintf(file, "%s(%s)", GetMBString, code);
                MB_GetNextNotify(&type, &code);
                if ((type != TC_END_NOTIFICATION)
                    && (type != TC_END_FUNCTION))
                    fprintf(file, ",\n");
                else
                    fprintf(file, "\n");
                break;
            case TC_LOCALECHAR:
                fprintf(file, "%s(%s)[0]\n", GetString, code);
                MB_GetNextNotify(&type, &code);
                if ((type != TC_END_NOTIFICATION)
                    && (type != TC_END_FUNCTION))
                    fprintf(file, ",\n");
                else
                    fprintf(file, "\n");
                break;
            case TC_INTEGER:
                fprintf(file, "%s", code);
                MB_GetNextNotify(&type, &code);
                if ((type != TC_END_NOTIFICATION)
                    && (type != TC_END_FUNCTION))
                    fprintf(file, ",\n");
                else
                    fprintf(file, "\n");
                break;
            case TC_CHAR:
                fprintf(file, "'%s'", code);
                MB_GetNextNotify(&type, &code);
                if ((type != TC_END_NOTIFICATION)
                    && (type != TC_END_FUNCTION))
                    fprintf(file, ",\n");
                else
                    fprintf(file, "\n");
                break;
            case TC_VAR_ARG:
                name = atoi(code);
                MB_GetVarInfo(name, MUIB_VarName, (IPTR) &code,
                              MUIB_VarType, (IPTR) &type, TAG_END);
                if ((type == TYPEVAR_LOCAL_PTR)
                    || (type == TYPEVAR_EXTERNAL_PTR))
                    fprintf(file, "%s", code);
                else
                    fprintf(file, "object->%s", code);
                MB_GetNextNotify(&type, &code);
                if ((type != TC_END_NOTIFICATION)
                    && (type != TC_END_FUNCTION))
                    fprintf(file, ",\n");
                else
                    fprintf(file, "\n");
                break;
            case TC_BOOL:
                if (*code == '0')
                    fprintf(file, "FALSE");
                else
                    fprintf(file, "TRUE");
                MB_GetNextNotify(&type, &code);
                if ((type != TC_END_NOTIFICATION)
                    && (type != TC_END_FUNCTION))
                    fprintf(file, ",\n");
                else
                    fprintf(file, "\n");
                break;
            case TC_MUIARG:
            case TC_MUIARG_OBJ:
                name = atoi(code);
                fprintf(file, "%s", MUIStrings[name]);
                MB_GetNextNotify(&type, &code);
                if ((type != TC_END_NOTIFICATION)
                    && (type != TC_END_FUNCTION))
                    fprintf(file, ", ");
                indent = FALSE;
                break;
            case TC_MUIARG_ATTRIBUT:
                name = atoi(code);
                fprintf(file, "%s", MUIStrings[name]);
                MB_GetNextNotify(&type, &code);
                if ((type != TC_END_NOTIFICATION)
                    && (type != TC_END_FUNCTION))
                    fprintf(file, ",\n");
                else
                    fprintf(file, "\n");
                break;
            case TC_EXTERNAL_CONSTANT:
                fprintf(file, "%s", code);
                MB_GetNextNotify(&type, &code);
                if ((type != TC_END_NOTIFICATION)
                    && (type != TC_END_FUNCTION))
                    fprintf(file, ",\n");
                else
                    fprintf(file, "\n");
                break;
            case TC_EXTERNAL_FUNCTION:
                fprintf(file, "&%sHook", code);
                MB_GetNextNotify(&type, &code);
                if ((type != TC_END_NOTIFICATION)
                    && (type != TC_END_FUNCTION))
                    fprintf(file, ",\n");
                else
                    fprintf(file, "\n");
                break;
            case TC_EXTERNAL_VARIABLE:
                fprintf(file, "%s", code);
                MB_GetNextNotify(&type, &code);
                if ((type != TC_END_NOTIFICATION)
                    && (type != TC_END_FUNCTION))
                    fprintf(file, ",\n");
                else
                    fprintf(file, "\n");
                break;
            default:
                printf("Type = %u\n", (unsigned int)type);
                printf
                    ("ERROR !!!!! THERE IS A PROBLEM WITH THIS FILE !!!\n");
                End();
                exit(1);
                break;
        }
    }
}

static void Init(void)
{
    BPTR lock;

    /* Get all needed variables */
    MB_Get
        (MUIB_VarNumber, (IPTR) &varnb,
         MUIB_Code, (IPTR) &Code,
         MUIB_Environment, (IPTR) &Env,
         MUIB_Locale, (IPTR) &Locale,
         MUIB_Notifications, (IPTR) &Notifications,
         MUIB_Declarations, (IPTR) &Declarations,
         MUIB_FileName, (IPTR) &FileName,
         MUIB_CatalogName, (IPTR) &CatalogName,
         MUIB_GetStringName, (IPTR) &GetString, TAG_END);

    /* Create 'GetMBString' name */
    if (strcmp(GetString, "GetMBString") == 0)
        GetMBString = "GetMBString2";
    else
        GetMBString = "GetMBString";

    /* Create File Names */
    remove_extend(FileName);
    strncpy(GUIFile, FileName, 512);
    add_extend(GUIFile, ".c");
    strncpy(HeaderFile, FileName, 512);
    add_extend(HeaderFile, ".h");
    strncpy(Externals, FileName, 512);
    strncat(Externals, "Extern", 512);
    add_extend(Externals, ".h");
    strncpy(MainFile, FileName, 512);
    strncat(MainFile, "Main", 512);
    add_extend(MainFile, ".c");

    /* Get Current Directory Name */
    lock = Lock("PROGDIR:", ACCESS_READ);
    NameFromLock(lock, MBDir, 512);
    UnLock(lock);
}

static void WriteHeaderFile(void)
{
    char *name;
    char buffer[600];
    char buffer2[600];

    if (Env)
    {
        strncpy(buffer, MBDir, 600);
        AddPart(buffer, "H-Header", 512);
        sprintf(buffer2, "copy \"%s\" \"%s\"", buffer, HeaderFile);
        Execute(buffer2, 0, 0);
    }
    else
        DeleteFile(HeaderFile);
    file = fopen(HeaderFile, "a+");
    if (file)
    {
        MB_GetVarInfo(0, MUIB_VarName, (IPTR) &name, TAG_END);
        fprintf(file, "struct Obj%s\n{\n", name);
        WriteDeclarations(TYPEVAR_PTR);
        WriteDeclarations(TYPEVAR_BOOL);
        WriteDeclarations(TYPEVAR_INT);
        WriteDeclarations(TYPEVAR_STRING);
        WriteDeclarations(TYPEVAR_TABSTRING);
        fprintf(file, "};\n\n");
        if (Notifications)
        {
            WriteDeclarations(TYPEVAR_IDENT);
            fprintf(file, "\n");
        }
        if (Env)
        {
            fprintf(file, "extern struct Obj%s * Create%s(void);\n", name,
                    name);
            fprintf(file, "extern void Dispose%s(struct Obj%s *);\n", name,
                    name);
        }
        fclose(file);
    }
}

static void WriteGUIFile(void)
{
    char buffer[600];
    char buffer2[600];
    char *name;

    if (Env)
    {
        strncpy(buffer, MBDir, 600);
        AddPart(buffer, "C-Header", 512);
        sprintf(buffer2, "copy \"%s\" \"%s\"", buffer, GUIFile);
        Execute(buffer2, 0, 0);
    }
    else
        DeleteFile(GUIFile);
    if ((file = fopen(GUIFile, "a+")) != NULL)
    {
        if (Env)
        {
            MB_GetVarInfo(0, MUIB_VarName, (IPTR) &name, TAG_END);
            if (ExternalExist)
            {
                fprintf(file, "#include \"%s\"\n", FilePart(Externals));
            }
            fprintf(file, "#include \"%s\"\n\n", FilePart(HeaderFile));
            if (Locale)
            {
                remove_extend(CatalogName);
                fprintf(file, "#include \"%s_cat.h\"\n\n",
                        FilePart(CatalogName));
                fprintf(file, "static CONST_STRPTR GetMBString(CONST_STRPTR ref)\n");
                fprintf(file, "{\n");
                fprintf(file, "\tif (ref[1] == '\\0')\n");
                fprintf(file, "\t\treturn &ref[2];\n");
                fprintf(file, "\telse\n");
                fprintf(file, "\t\treturn ref;\n");
                fprintf(file, "}\n");
            }
            fprintf(file, "\nstruct Obj%s * Create%s(", name, name);
            WriteParameters();
            fprintf(file, ")\n");
            fprintf(file, "{\n\tstruct Obj%s * object;\n\n", name);
        }
        if (Declarations)
        {
            WriteDeclarations(TYPEVAR_LOCAL_PTR);
            WriteInitialisations(TYPEVAR_HOOK);
        }
        if (Env)
            fprintf(file,
                    "\n\tif (!(object = AllocVec(sizeof(struct Obj%s), MEMF_PUBLIC|MEMF_CLEAR)))\n\t\treturn NULL;\n",
                    name);
        if (Declarations)
        {
            WriteInitialisations(TYPEVAR_PTR);
            WriteInitialisations(TYPEVAR_BOOL);
            WriteInitialisations(TYPEVAR_INT);
            WriteInitialisations(TYPEVAR_STRING);
            WriteInitialisations(TYPEVAR_TABSTRING);
        }
        if (Code)
            WriteCode();
        if (Env)
        {
            fprintf(file, "\n\tif (!object->%s)\n", name);
            fprintf(file, "\t{\n");
            fprintf(file, "\t\tFreeVec(object);\n");
            fprintf(file, "\t\treturn NULL;\n");
            fprintf(file, "\t}\n");
        }
        if (Notifications)
        {
            WriteNotify();
        }
        if (Env)
        {
            fprintf(file, "\n\treturn object;\n");
            fprintf(file, "}\n");
            fprintf(file, "\nvoid Dispose%s(struct Obj%s * object)\n", name, name);
            fprintf(file, "{\n");
            fprintf(file, "\tif (object)\n");
            fprintf(file, "\t{\n");
            fprintf(file, "\t\tMUI_DisposeObject(object->%s);\n", name);
            fprintf(file, "\t\tFreeVec(object);\n");
            fprintf(file, "\t\tobject = NULL;\n");
            fprintf(file, "\t}\n");
            fprintf(file, "}\n");
        }
        fclose(file);
    }
    else
        printf("Unable to open GUI-File !\n");
}

/* Create a file where are the external variables and functions declarations */
static BOOL WriteExternalFile(void)
{
    int i;
    ULONG length, type;
    BPTR TMPfile;
    char *adr_file = NULL;
    struct FileInfoBlock *Info;
    BOOL bool_aux = FALSE;
    char *varname;
    BOOL result = FALSE;

    Info = AllocDosObjectTagList(DOS_FIB, NULL);
    if (!Info)
    {
        return FALSE;
    }

    /* If the file already exists, we load it in memory */
    if ((TMPfile = Open(Externals, MODE_OLDFILE)) != BNULL)
    {
        ExamineFH(TMPfile, Info);
        length = Info->fib_Size;
        adr_file = AllocVec(length + 1, MEMF_PUBLIC | MEMF_CLEAR);
        Read(TMPfile, adr_file, length);
        adr_file[length] = '\0';
        Close(TMPfile);
    }
    if ((file = fopen(Externals, "a+")) != NULL)
    {
        for (i = 0; i < varnb; i++)
        {
            MB_GetVarInfo(i,
                          MUIB_VarType, (IPTR) &type,
                          MUIB_VarName, (IPTR) &varname, TAG_END);
            switch (type)       /* if the declaration doesn't exist, we generate it */
            {
                case TYPEVAR_EXTERNAL:
                    if (adr_file)
                        bool_aux = (strstr(adr_file, varname) != NULL);
                    if (!bool_aux)
                        fprintf(file, "extern int %s;\n", varname);
                    break;
                case TYPEVAR_HOOK:
                    if (adr_file)
                        bool_aux = (strstr(adr_file, varname) != NULL);
                    if (!bool_aux)
                        fprintf(file,
                                "void %s(struct Hook *h, Object *o);\n",
                                varname);
                    break;
            }
        }
        fclose(file);
    }
    if (adr_file)
        FreeVec(adr_file);
    
    /* if the file is 0 bytes long we remove it */
    if ((TMPfile = Open(Externals, MODE_OLDFILE)) != BNULL)
    {
        ExamineFH(TMPfile, Info);
        Close(TMPfile);
        length = Info->fib_Size;
        if (length == 0)
            DeleteFile(Externals);
        else
            result = TRUE;
    }

    FreeDosObject(DOS_FIB, Info);

    return result;
}

static void WriteMainFile(void)
{
    char buffer[600];
    char buffer2[600];

    if (Env)
    {
        strncpy(buffer, MBDir, 600);
        AddPart(buffer, "C-Header", 512);
        sprintf(buffer2, "copy \"%s\" \"%s\"", buffer, MainFile);
        Execute(buffer2, 0, 0);
    }
    else
        DeleteFile(MainFile);

    file = fopen(MainFile, "a+");
    if (file)
    {
        fprintf(file, "\n#include \"%s\"\n\n", FilePart(HeaderFile));
        fprintf(file, "struct Library *MUIMasterBase;\n\n");
        fprintf(file, "void CleanExit(CONST_STRPTR s)\n");
        fprintf(file, "{\n");
        fprintf(file, "\tif (s)\n");
        fprintf(file, "\t{\n");
        fprintf(file, "\t\tPutStr(s);\n");
        fprintf(file, "\t}\n");
        fprintf(file, "\tMyCleanExit();\n");
        fprintf(file, "\tCloseLibrary(MUIMasterBase);\n");
        fprintf(file, "}\n\n");
        fprintf(file, "BOOL InitApp(int argc, char **argv)\n");
        fprintf(file, "{\n");
        fprintf(file, "\tif ((MUIMasterBase = OpenLibrary(\"muimaster.library\", 19)) == NULL)\n");
        fprintf(file, "\t{\n");
        fprintf(file, "\t\tCleanExit(\"Can't open muimaster.library v19\\n\");\n");
        fprintf(file, "\t}\n");
        fprintf(file, "\tif (!MyInitApp(argc, argv))\n");
        fprintf(file, " \t{\n");
        fprintf(file, "\t\tCleanExit(\"Can't initialize application\\n\");\n");
        fprintf(file, "\t}\n");
        fprintf(file, "\treturn TRUE;\n");
        fprintf(file, "}\n\n");
        fprintf(file, "int main(int argc, char **argv)\n");
        fprintf(file, "{\n");
        fprintf(file, "\tInitApp(argc, argv);\n");
        fprintf(file, "\tULONG sigs = 0;\n");
        fprintf(file, "\tstruct ObjApp * obj = CreateApp();\n");
        fprintf(file, "\tif (obj)\n");
        fprintf(file, "\t{\n");
        fprintf(file, "\t\twhile (DoMethod(obj->App, MUIM_Application_NewInput, (IPTR)&sigs)\n");
        fprintf(file, "\t\t\t!= MUIV_Application_ReturnID_Quit)\n");
        fprintf(file, "\t\t{\n");
        fprintf(file, "\t\t\tif (sigs)\n");
        fprintf(file, "\t\t\t{\n");
        fprintf(file, "\t\t\t\tsigs = Wait(sigs | SIGBREAKF_CTRL_C);\n");
        fprintf(file, "\t\t\t\tif (sigs & SIGBREAKF_CTRL_C)\n");
        fprintf(file, "\t\t\t\t\tbreak;\n");
        fprintf(file, "\t\t\t}\n");
        fprintf(file, "\t\t}\n");
        fprintf(file, "\t\tDisposeApp(obj);\n");
        fprintf(file, "\t}\n");
        fprintf(file, "\telse\n");
        fprintf(file, "\t{\n");
        fprintf(file, "\t\tCleanExit(\"Can't create application\\n\");\n");
        fprintf(file, "\t}\n");
        fprintf(file, "\tCleanExit(NULL);\n");
        fprintf(file, "\treturn 0;\n");
        fprintf(file, "}\n");

        fclose(file);
    }
}

int main(void)
{
    /* Open MUIBuilder Library */
    MUIBBase = OpenLibrary("muibuilder.library", 0);

    /* Open Dos Library */
    DOSBase = (struct DosLibrary *) OpenLibrary("dos.library", 0);

    /* exit if it can't open */
    if ((!MUIBBase) || (!DOSBase))
    {
        printf("Unable to open a library\n");
        return 20;
    }

    /* exit if we can't init the Library */
    if (!MB_Open())
    {
        printf("Unable to get temporary files !\n");
        End();
        return 20;
    }

    Init();

    if (Declarations)
        WriteHeaderFile();
    if (Env)
        ExternalExist = WriteExternalFile();

    WriteGUIFile();
    WriteMainFile();

    End();

    return 0;
}

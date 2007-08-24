#ifndef FEELIN_DOCUMENT_H
#define FEELIN_DOCUMENT_H

/*
**  feelin/document.h
**
**  © 2001-2005 Olivier LAVIALE (gofromiel@gofromiel.com)
**
********************************************************************************

$VER: 02.00 (2005/06/13)

    OLAV: Added type for FM_Document_Resolve 'ok' value.

*/

#ifdef __MORPHOS__
#if defined(__GNUC__)
#pragma pack(2)
#endif
#endif /* __MORPHOS__ */

#define FC_Document                          "Document"
#define DocumentObject                       F_NewObj(FC_Document

enum    {

        FV_DOCUMENT_ID_DUMMY = 11

        };

enum    {

        FV_Document_SourceType_Unknown,
        FV_Document_SourceType_File,
        FV_Document_SourceType_Memory

        };

/****************************************************************************
*** Types *******************************************************************
****************************************************************************/

typedef struct FeelinDocumentID
{
    STRPTR                          Name;
    uint16                          Length;
    uint16                          Value;
}
FDOCID;

typedef struct FeelinDocumentName
{
    struct FeelinDocumentName      *Next;
    STRPTR                          Key;
    uint32                          KeyLength;
    uint32                          ID;
}
FDOCName;

typedef struct FeelinDocumentValue
{
    STRPTR                          Name;
    uint32                          Value;
}
FDOCValue;

/****************************************************************************
*** Messages ****************************************************************
****************************************************************************/

struct  FS_Document_Read                     { STRPTR Source; uint32 Type; APTR Pool; };
struct  FS_Document_Parse                    { STRPTR Source; APTR Pool; };
struct  FS_Document_AddIDs                   { FDOCID *Table; };
struct  FS_Document_Resolve                  { STRPTR Data; uint32 Type; FDOCValue *Values; int32 *Done; };
struct  FS_Document_FindName                 { STRPTR Key; uint32 KeyLength; };
struct  FS_Document_ObtainName               { STRPTR Key; uint32 KeyLength; };
struct  FS_Document_Log                      { uint32 Line; STRPTR Source; STRPTR Fmt; };

#ifdef __MORPHOS__
#if defined(__GNUC__)
# pragma pack()
#endif
#endif /* __MORPHOS__ */

#endif

#ifndef LOC_H
#define LOC_H


/****************************************************************************/


/* This file was created automatically by CatComp.
 * Do NOT edit by hand!
 */


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifdef CATCOMP_ARRAY
#undef CATCOMP_NUMBERS
#undef CATCOMP_STRINGS
#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#endif

#ifdef CATCOMP_BLOCK
#undef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif


/****************************************************************************/


#ifdef CATCOMP_NUMBERS

#define MCCMSG_BWin 1
#define MCCMSG_ToFront 2
#define MCCMSG_ToBack 3
#define MCCMSG_HideWin 4
#define MCCMSG_CloseWin 5
#define MCCMSG_DragBarTop 6
#define MCCMSG_AutoShow 7
#define MCCMSG_Close 8
#define MCCMSG_DragBar 9
#define MCCMSG_Size 10
#define MCCMSG_Snapshot 11
#define MCCMSG_Unsnapshot 12
#define MCCMSG_Settings 13
#define MCCMSG_User 14

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define MCCMSG_BWin_STR "BWin"
#define MCCMSG_ToFront_STR "Bring to front"
#define MCCMSG_ToBack_STR "Send to back"
#define MCCMSG_HideWin_STR "Hide"
#define MCCMSG_CloseWin_STR "Close"
#define MCCMSG_DragBarTop_STR "DragBar at top"
#define MCCMSG_AutoShow_STR "Auto"
#define MCCMSG_Close_STR "Close"
#define MCCMSG_DragBar_STR "DragBar"
#define MCCMSG_Size_STR "Size"
#define MCCMSG_Snapshot_STR "Snapshot"
#define MCCMSG_Unsnapshot_STR "Unsnapshot"
#define MCCMSG_Settings_STR "MUI settings..."
#define MCCMSG_User_STR "User..."

#endif /* CATCOMP_STRINGS */


/****************************************************************************/


#ifdef CATCOMP_ARRAY

struct CatCompArrayType
{
    LONG   cca_ID;
    CONST_STRPTR cca_Str;
};

static const struct CatCompArrayType CatCompArray[] =
{
    {MCCMSG_BWin,(STRPTR)MCCMSG_BWin_STR},
    {MCCMSG_ToFront,(STRPTR)MCCMSG_ToFront_STR},
    {MCCMSG_ToBack,(STRPTR)MCCMSG_ToBack_STR},
    {MCCMSG_HideWin,(STRPTR)MCCMSG_HideWin_STR},
    {MCCMSG_CloseWin,(STRPTR)MCCMSG_CloseWin_STR},
    {MCCMSG_DragBarTop,(STRPTR)MCCMSG_DragBarTop_STR},
    {MCCMSG_AutoShow,(STRPTR)MCCMSG_AutoShow_STR},
    {MCCMSG_Close,(STRPTR)MCCMSG_Close_STR},
    {MCCMSG_DragBar,(STRPTR)MCCMSG_DragBar_STR},
    {MCCMSG_Size,(STRPTR)MCCMSG_Size_STR},
    {MCCMSG_Snapshot,(STRPTR)MCCMSG_Snapshot_STR},
    {MCCMSG_Unsnapshot,(STRPTR)MCCMSG_Unsnapshot_STR},
    {MCCMSG_Settings,(STRPTR)MCCMSG_Settings_STR},
    {MCCMSG_User,(STRPTR)MCCMSG_User_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

static const char CatCompBlock[] =
{
    "\x00\x00\x00\x01\x00\x06"
    MCCMSG_BWin_STR "\x00\x00"
    "\x00\x00\x00\x02\x00\x10"
    MCCMSG_ToFront_STR "\x00\x00"
    "\x00\x00\x00\x03\x00\x0E"
    MCCMSG_ToBack_STR "\x00\x00"
    "\x00\x00\x00\x04\x00\x06"
    MCCMSG_HideWin_STR "\x00\x00"
    "\x00\x00\x00\x05\x00\x06"
    MCCMSG_CloseWin_STR "\x00"
    "\x00\x00\x00\x06\x00\x10"
    MCCMSG_DragBarTop_STR "\x00\x00"
    "\x00\x00\x00\x07\x00\x06"
    MCCMSG_AutoShow_STR "\x00\x00"
    "\x00\x00\x00\x08\x00\x06"
    MCCMSG_Close_STR "\x00"
    "\x00\x00\x00\x09\x00\x08"
    MCCMSG_DragBar_STR "\x00"
    "\x00\x00\x00\x0A\x00\x06"
    MCCMSG_Size_STR "\x00\x00"
    "\x00\x00\x00\x0B\x00\x0A"
    MCCMSG_Snapshot_STR "\x00\x00"
    "\x00\x00\x00\x0C\x00\x0C"
    MCCMSG_Unsnapshot_STR "\x00\x00"
    "\x00\x00\x00\x0D\x00\x10"
    MCCMSG_Settings_STR "\x00"
    "\x00\x00\x00\x0E\x00\x08"
    MCCMSG_User_STR "\x00"
};

#endif /* CATCOMP_BLOCK */


/****************************************************************************/


struct LocaleInfo
{
    APTR li_LocaleBase;
    APTR li_Catalog;
};


#ifdef CATCOMP_CODE

STRPTR GetString(struct LocaleInfo *li, LONG stringNum)
{
LONG   *l;
UWORD  *w;
STRPTR  builtIn;

    l = (LONG *)CatCompBlock;

    while (*l != stringNum)
    {
        w = (UWORD *)((ULONG)l + 4);
        l = (LONG *)((ULONG)l + (ULONG)*w + 6);
    }
    builtIn = (STRPTR)((ULONG)l + 6);

#undef LocaleBase
#define LocaleBase li->li_LocaleBase
    
    if (LocaleBase)
        return(GetCatalogStr(li->li_Catalog,stringNum,builtIn));
#undef LocaleBase

    return(builtIn);
}


#endif /* CATCOMP_CODE */


/****************************************************************************/


#endif /* LOC_H */

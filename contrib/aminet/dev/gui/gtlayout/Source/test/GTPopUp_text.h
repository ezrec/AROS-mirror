#ifndef GTPOPUP_TEXT_H
#define GTPOPUP_TEXT_H


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

#define GTLAYOUT_CONTEXT_ERROR 0
#define OPEN_GADTOOLS_ERROR 1
#define OPEN_GTLAYOUT_ERROR 2
#define EXAMINE_ERROR 3
#define LOCK_MAILDIR_ERROR 4
#define TITLE_GTPOPUP 5
#define TITLE_MESSAGE 6
#define TITLE_SELECT_FILE_TO_SAVE_TO 7
#define TITLE_SELECT_FILE_TO_OPEN 8
#define BUTTON_NEWMESSAGE_TEXT 9
#define BUTTON_DELMESSAGE_TEXT 10
#define BUTTON_PREVMESSAGE_TEXT 11
#define BUTTON_NEXTMESSAGE_TEXT 12
#define BOX_OVERVIEW_TEXT 13
#define BOX_FROM_TEXT 14
#define RADIO_TOWHOM_COMPUTER_TEXT 15
#define RADIO_TOWHOM_WORKGROUP_TEXT 16
#define BUTTON_SENDMESSAGE_TEXT 17
#define BUTTON_CANCEL_TEXT 18
#define FRAME_MESSAGE_TEXT 19
#define STRING_MESSAGE_LABEL_TEXT 20

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define GTLAYOUT_CONTEXT_ERROR_STR "Can't build context"
#define OPEN_GADTOOLS_ERROR_STR "Can't open gadtools.library v39 or higher"
#define OPEN_GTLAYOUT_ERROR_STR "Can't open gtlayout.library v47 or higher"
#define EXAMINE_ERROR_STR "can't examine"
#define LOCK_MAILDIR_ERROR_STR "can't lock the message directory"
#define TITLE_GTPOPUP_STR "GTPopUp v0.1"
#define TITLE_MESSAGE_STR "Message"
#define TITLE_SELECT_FILE_TO_SAVE_TO_STR "Select File to Save to"
#define TITLE_SELECT_FILE_TO_OPEN_STR "Select a Picture to open"
#define BUTTON_NEWMESSAGE_TEXT_STR "New Message"
#define BUTTON_DELMESSAGE_TEXT_STR "Delete Message"
#define BUTTON_PREVMESSAGE_TEXT_STR "Previous Message"
#define BUTTON_NEXTMESSAGE_TEXT_STR "Next Message"
#define BOX_OVERVIEW_TEXT_STR "current message %ld - count of messages %ld"
#define BOX_FROM_TEXT_STR "message from %s (%s) to %s on %date"
#define RADIO_TOWHOM_COMPUTER_TEXT_STR "User or Computer"
#define RADIO_TOWHOM_WORKGROUP_TEXT_STR "Workgroup"
#define BUTTON_SENDMESSAGE_TEXT_STR "send message"
#define BUTTON_CANCEL_TEXT_STR "Cancel"
#define FRAME_MESSAGE_TEXT_STR "To:"
#define STRING_MESSAGE_LABEL_TEXT_STR "message:"

#endif /* CATCOMP_STRINGS */


/****************************************************************************/


#ifdef CATCOMP_ARRAY

struct CatCompArrayType
{
    LONG   cca_ID;
    STRPTR cca_Str;
};

static const struct CatCompArrayType CatCompArray[] =
{
    {GTLAYOUT_CONTEXT_ERROR,(STRPTR)GTLAYOUT_CONTEXT_ERROR_STR},
    {OPEN_GADTOOLS_ERROR,(STRPTR)OPEN_GADTOOLS_ERROR_STR},
    {OPEN_GTLAYOUT_ERROR,(STRPTR)OPEN_GTLAYOUT_ERROR_STR},
    {EXAMINE_ERROR,(STRPTR)EXAMINE_ERROR_STR},
    {LOCK_MAILDIR_ERROR,(STRPTR)LOCK_MAILDIR_ERROR_STR},
    {TITLE_GTPOPUP,(STRPTR)TITLE_GTPOPUP_STR},
    {TITLE_MESSAGE,(STRPTR)TITLE_MESSAGE_STR},
    {TITLE_SELECT_FILE_TO_SAVE_TO,(STRPTR)TITLE_SELECT_FILE_TO_SAVE_TO_STR},
    {TITLE_SELECT_FILE_TO_OPEN,(STRPTR)TITLE_SELECT_FILE_TO_OPEN_STR},
    {BUTTON_NEWMESSAGE_TEXT,(STRPTR)BUTTON_NEWMESSAGE_TEXT_STR},
    {BUTTON_DELMESSAGE_TEXT,(STRPTR)BUTTON_DELMESSAGE_TEXT_STR},
    {BUTTON_PREVMESSAGE_TEXT,(STRPTR)BUTTON_PREVMESSAGE_TEXT_STR},
    {BUTTON_NEXTMESSAGE_TEXT,(STRPTR)BUTTON_NEXTMESSAGE_TEXT_STR},
    {BOX_OVERVIEW_TEXT,(STRPTR)BOX_OVERVIEW_TEXT_STR},
    {BOX_FROM_TEXT,(STRPTR)BOX_FROM_TEXT_STR},
    {RADIO_TOWHOM_COMPUTER_TEXT,(STRPTR)RADIO_TOWHOM_COMPUTER_TEXT_STR},
    {RADIO_TOWHOM_WORKGROUP_TEXT,(STRPTR)RADIO_TOWHOM_WORKGROUP_TEXT_STR},
    {BUTTON_SENDMESSAGE_TEXT,(STRPTR)BUTTON_SENDMESSAGE_TEXT_STR},
    {BUTTON_CANCEL_TEXT,(STRPTR)BUTTON_CANCEL_TEXT_STR},
    {FRAME_MESSAGE_TEXT,(STRPTR)FRAME_MESSAGE_TEXT_STR},
    {STRING_MESSAGE_LABEL_TEXT,(STRPTR)STRING_MESSAGE_LABEL_TEXT_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

static const char CatCompBlock[] =
{
    "\x00\x00\x00\x00\x00\x14"
    GTLAYOUT_CONTEXT_ERROR_STR "\x00"
    "\x00\x00\x00\x01\x00\x2A"
    OPEN_GADTOOLS_ERROR_STR "\x00"
    "\x00\x00\x00\x02\x00\x2A"
    OPEN_GTLAYOUT_ERROR_STR "\x00"
    "\x00\x00\x00\x03\x00\x0E"
    EXAMINE_ERROR_STR "\x00"
    "\x00\x00\x00\x04\x00\x22"
    LOCK_MAILDIR_ERROR_STR "\x00\x00"
    "\x00\x00\x00\x05\x00\x0E"
    TITLE_GTPOPUP_STR "\x00\x00"
    "\x00\x00\x00\x06\x00\x08"
    TITLE_MESSAGE_STR "\x00"
    "\x00\x00\x00\x07\x00\x18"
    TITLE_SELECT_FILE_TO_SAVE_TO_STR "\x00\x00"
    "\x00\x00\x00\x08\x00\x1A"
    TITLE_SELECT_FILE_TO_OPEN_STR "\x00\x00"
    "\x00\x00\x00\x09\x00\x0C"
    BUTTON_NEWMESSAGE_TEXT_STR "\x00"
    "\x00\x00\x00\x0A\x00\x10"
    BUTTON_DELMESSAGE_TEXT_STR "\x00\x00"
    "\x00\x00\x00\x0B\x00\x12"
    BUTTON_PREVMESSAGE_TEXT_STR "\x00\x00"
    "\x00\x00\x00\x0C\x00\x0E"
    BUTTON_NEXTMESSAGE_TEXT_STR "\x00\x00"
    "\x00\x00\x00\x0D\x00\x2C"
    BOX_OVERVIEW_TEXT_STR "\x00"
    "\x00\x00\x00\x0E\x00\x24"
    BOX_FROM_TEXT_STR "\x00"
    "\x00\x00\x00\x0F\x00\x12"
    RADIO_TOWHOM_COMPUTER_TEXT_STR "\x00\x00"
    "\x00\x00\x00\x10\x00\x0A"
    RADIO_TOWHOM_WORKGROUP_TEXT_STR "\x00"
    "\x00\x00\x00\x11\x00\x0E"
    BUTTON_SENDMESSAGE_TEXT_STR "\x00\x00"
    "\x00\x00\x00\x12\x00\x08"
    BUTTON_CANCEL_TEXT_STR "\x00\x00"
    "\x00\x00\x00\x13\x00\x04"
    FRAME_MESSAGE_TEXT_STR "\x00"
    "\x00\x00\x00\x14\x00\x0A"
    STRING_MESSAGE_LABEL_TEXT_STR "\x00\x00"
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


#endif /* GTPOPUP_TEXT_H */

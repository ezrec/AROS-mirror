#ifndef TEXTINPUT_CAT_H
#define TEXTINPUT_CAT_H


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

#define MSG_PREFS_COLORS 0
#define MSG_PREFS_FOREGROUND 1
#define MSG_PREFS_BACKGROUND 2
#define MSG_PREFS_INACTIVE 3
#define MSG_PREFS_ACTIVE 4
#define MSG_PREFS_MARKED 5
#define MSG_PREFS_CURSOR 6
#define MSG_PREFS_STYLE 7
#define MSG_PREFS_GRAB_GAD 8
#define MSG_PREFS_ADD 9
#define MSG_PREFS_DEL 10
#define MSG_PREFS_L1 11
#define MSG_PREFS_L2 12
#define MSG_PREFS_SYNC 13
#define MSG_PREFS_EDITCALL 14
#define MSG_MINVER 15

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define MSG_PREFS_COLORS_STR "Colors"
#define MSG_PREFS_FOREGROUND_STR "Foreground"
#define MSG_PREFS_BACKGROUND_STR "Background"
#define MSG_PREFS_INACTIVE_STR "Inactive:"
#define MSG_PREFS_ACTIVE_STR "Active:"
#define MSG_PREFS_MARKED_STR "Marked:"
#define MSG_PREFS_CURSOR_STR "Cursor:"
#define MSG_PREFS_STYLE_STR "Style:"
#define MSG_PREFS_GRAB_GAD_STR "_Grab old string gadget colours"
#define MSG_PREFS_ADD_STR "Add"
#define MSG_PREFS_DEL_STR "Del"
#define MSG_PREFS_L1_STR "\033uKey"
#define MSG_PREFS_L2_STR "\033uAction"
#define MSG_PREFS_SYNC_STR "Call editor synchronously?"
#define MSG_PREFS_EDITCALL_STR "Call:"
#define MSG_MINVER_STR "Application requests version %ld\nof Textinput.mcc, but you only\nhave version %ld installed!"

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
    {MSG_PREFS_COLORS,(STRPTR)MSG_PREFS_COLORS_STR},
    {MSG_PREFS_FOREGROUND,(STRPTR)MSG_PREFS_FOREGROUND_STR},
    {MSG_PREFS_BACKGROUND,(STRPTR)MSG_PREFS_BACKGROUND_STR},
    {MSG_PREFS_INACTIVE,(STRPTR)MSG_PREFS_INACTIVE_STR},
    {MSG_PREFS_ACTIVE,(STRPTR)MSG_PREFS_ACTIVE_STR},
    {MSG_PREFS_MARKED,(STRPTR)MSG_PREFS_MARKED_STR},
    {MSG_PREFS_CURSOR,(STRPTR)MSG_PREFS_CURSOR_STR},
    {MSG_PREFS_STYLE,(STRPTR)MSG_PREFS_STYLE_STR},
    {MSG_PREFS_GRAB_GAD,(STRPTR)MSG_PREFS_GRAB_GAD_STR},
    {MSG_PREFS_ADD,(STRPTR)MSG_PREFS_ADD_STR},
    {MSG_PREFS_DEL,(STRPTR)MSG_PREFS_DEL_STR},
    {MSG_PREFS_L1,(STRPTR)MSG_PREFS_L1_STR},
    {MSG_PREFS_L2,(STRPTR)MSG_PREFS_L2_STR},
    {MSG_PREFS_SYNC,(STRPTR)MSG_PREFS_SYNC_STR},
    {MSG_PREFS_EDITCALL,(STRPTR)MSG_PREFS_EDITCALL_STR},
    {MSG_MINVER,(STRPTR)MSG_MINVER_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

static const char CatCompBlock[] =
{
    "\x00\x00\x00\x00\x00\x08"
    MSG_PREFS_COLORS_STR "\x00\x00"
    "\x00\x00\x00\x01\x00\x0C"
    MSG_PREFS_FOREGROUND_STR "\x00\x00"
    "\x00\x00\x00\x02\x00\x0C"
    MSG_PREFS_BACKGROUND_STR "\x00\x00"
    "\x00\x00\x00\x03\x00\x0A"
    MSG_PREFS_INACTIVE_STR "\x00"
    "\x00\x00\x00\x04\x00\x08"
    MSG_PREFS_ACTIVE_STR "\x00"
    "\x00\x00\x00\x05\x00\x08"
    MSG_PREFS_MARKED_STR "\x00"
    "\x00\x00\x00\x06\x00\x08"
    MSG_PREFS_CURSOR_STR "\x00"
    "\x00\x00\x00\x07\x00\x08"
    MSG_PREFS_STYLE_STR "\x00\x00"
    "\x00\x00\x00\x08\x00\x20"
    MSG_PREFS_GRAB_GAD_STR "\x00"
    "\x00\x00\x00\x09\x00\x04"
    MSG_PREFS_ADD_STR "\x00"
    "\x00\x00\x00\x0A\x00\x04"
    MSG_PREFS_DEL_STR "\x00"
    "\x00\x00\x00\x0B\x00\x06"
    MSG_PREFS_L1_STR "\x00"
    "\x00\x00\x00\x0C\x00\x0A"
    MSG_PREFS_L2_STR "\x00\x00"
    "\x00\x00\x00\x0D\x00\x1C"
    MSG_PREFS_SYNC_STR "\x00\x00"
    "\x00\x00\x00\x0E\x00\x06"
    MSG_PREFS_EDITCALL_STR "\x00"
    "\x00\x00\x00\x0F\x00\x5C"
    MSG_MINVER_STR "\x00"
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

#define XLocaleBase LocaleBase
#define LocaleBase li->li_LocaleBase
    
    if (LocaleBase)
        return(GetCatalogStr(li->li_Catalog,stringNum,builtIn));
#define LocaleBase XLocaleBase
#undef XLocaleBase

    return(builtIn);
}


#endif /* CATCOMP_CODE */


/****************************************************************************/


#endif /* TEXTINPUT_CAT_H */

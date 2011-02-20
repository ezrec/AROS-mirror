#ifndef BGUILIB_LOCALE_H
#define BGUILIB_LOCALE_H


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

#define MSG_BGUI_REQUESTA_TITLE 0
#define MSG_BGUI_HELP_CONTINUE 1
#define MSG_BGUI_HELP_TITLE 2

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define MSG_BGUI_REQUESTA_TITLE_STR "BGUI Request"
#define MSG_BGUI_HELP_CONTINUE_STR "Continue"
#define MSG_BGUI_HELP_TITLE_STR "BGUI Help..."

#endif /* CATCOMP_STRINGS */


/****************************************************************************/


#ifdef CATCOMP_ARRAY

struct CatCompArrayType
{
    LONG   cca_ID;
    CONST_STRPTR cca_Str;
};

 struct CatCompArrayType CatCompArray[] =
{
    {MSG_BGUI_REQUESTA_TITLE,(STRPTR)MSG_BGUI_REQUESTA_TITLE_STR},
    {MSG_BGUI_HELP_CONTINUE,(STRPTR)MSG_BGUI_HELP_CONTINUE_STR},
    {MSG_BGUI_HELP_TITLE,(STRPTR)MSG_BGUI_HELP_TITLE_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

 char CatCompBlock[] =
{
    "\x00\x00\x00\x00\x00\x0E"
    MSG_BGUI_REQUESTA_TITLE_STR "\x00\x00"
    "\x00\x00\x00\x01\x00\x0A"
    MSG_BGUI_HELP_CONTINUE_STR "\x00\x00"
    "\x00\x00\x00\x02\x00\x0E"
    MSG_BGUI_HELP_TITLE_STR "\x00\x00"
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


#endif /* BGUILIB_LOCALE_H */

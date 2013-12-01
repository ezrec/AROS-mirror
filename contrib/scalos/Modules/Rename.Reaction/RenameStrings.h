#ifndef RENAMESTRINGS_H
#define RENAMESTRINGS_H


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

#define MSGID_WINTITLE_RENAME 256
#define MSGID_TEXTLINE_ENTERNEWNAME 257
#define MSGID_NEWNAME_DEFAULT 258
#define MSGID_LABEL_NEWNAME 259
#define MSGID_BUTTON_OK 261
#define MSGID_BUTTON_CANCEL 262
#define MSGID_TITLE_ERRORREQ 263
#define MSGID_ERRORREQ_BODYTEXT 264
#define MSGID_REQ_OK 265
#define MSGID_TEXTLINE_PATH 266
#define MSGID_BUTTON_SKIP 267

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define MSGID_WINTITLE_RENAME_STR "Rename"
#define MSGID_TEXTLINE_ENTERNEWNAME_STR "Enter a new Name for '%s'"
#define MSGID_NEWNAME_DEFAULT_STR "New_Object_Name"
#define MSGID_LABEL_NEWNAME_STR "New Name"
#define MSGID_BUTTON_OK_STR "_Ok"
#define MSGID_BUTTON_CANCEL_STR "_Cancel"
#define MSGID_TITLE_ERRORREQ_STR "Rename Error"
#define MSGID_ERRORREQ_BODYTEXT_STR "Could not rename\n'%s' to '%s'\n%s"
#define MSGID_REQ_OK_STR " _Ok"
#define MSGID_TEXTLINE_PATH_STR "Path: '%s'"
#define MSGID_BUTTON_SKIP_STR "_Skip"

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
    {MSGID_WINTITLE_RENAME,(STRPTR)MSGID_WINTITLE_RENAME_STR},
    {MSGID_TEXTLINE_ENTERNEWNAME,(STRPTR)MSGID_TEXTLINE_ENTERNEWNAME_STR},
    {MSGID_NEWNAME_DEFAULT,(STRPTR)MSGID_NEWNAME_DEFAULT_STR},
    {MSGID_LABEL_NEWNAME,(STRPTR)MSGID_LABEL_NEWNAME_STR},
    {MSGID_BUTTON_OK,(STRPTR)MSGID_BUTTON_OK_STR},
    {MSGID_BUTTON_CANCEL,(STRPTR)MSGID_BUTTON_CANCEL_STR},
    {MSGID_TITLE_ERRORREQ,(STRPTR)MSGID_TITLE_ERRORREQ_STR},
    {MSGID_ERRORREQ_BODYTEXT,(STRPTR)MSGID_ERRORREQ_BODYTEXT_STR},
    {MSGID_REQ_OK,(STRPTR)MSGID_REQ_OK_STR},
    {MSGID_TEXTLINE_PATH,(STRPTR)MSGID_TEXTLINE_PATH_STR},
    {MSGID_BUTTON_SKIP,(STRPTR)MSGID_BUTTON_SKIP_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/


#ifdef CATCOMP_BLOCK

static const char CatCompBlock[] =
{
    "\x00\x00\x01\x00\x00\x08"
    MSGID_WINTITLE_RENAME_STR "\x00\x00"
    "\x00\x00\x01\x01\x00\x1A"
    MSGID_TEXTLINE_ENTERNEWNAME_STR "\x00"
    "\x00\x00\x01\x02\x00\x10"
    MSGID_NEWNAME_DEFAULT_STR "\x00"
    "\x00\x00\x01\x03\x00\x0A"
    MSGID_LABEL_NEWNAME_STR "\x00\x00"
    "\x00\x00\x01\x05\x00\x04"
    MSGID_BUTTON_OK_STR "\x00"
    "\x00\x00\x01\x06\x00\x08"
    MSGID_BUTTON_CANCEL_STR "\x00"
    "\x00\x00\x01\x07\x00\x0E"
    MSGID_TITLE_ERRORREQ_STR "\x00\x00"
    "\x00\x00\x01\x08\x00\x22"
    MSGID_ERRORREQ_BODYTEXT_STR "\x00\x00"
    "\x00\x00\x01\x09\x00\x06"
    MSGID_REQ_OK_STR "\x00\x00"
    "\x00\x00\x01\x0A\x00\x0C"
    MSGID_TEXTLINE_PATH_STR "\x00\x00"
    "\x00\x00\x01\x0B\x00\x06"
    MSGID_BUTTON_SKIP_STR "\x00"
};

#endif /* CATCOMP_BLOCK */


/****************************************************************************/


struct LocaleInfo
{
    APTR li_LocaleBase;
    APTR li_Catalog;
};



#endif /* RENAMESTRINGS_H */

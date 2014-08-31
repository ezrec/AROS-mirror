
#define SYSTEM_PRIVATE
#define AROS_ALMOST_COMPATIBLE
#include <exec/lists.h>
#include <proto/alib.h>
#include <private/vapor/vapor.h>

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <proto/dos.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H
#include FT_SFNT_NAMES_H
#include "system/directory.h"
#include "system/functions.h"
#include "freetype/ttnameid.h"

#include "fcstr.c"

#include "fontcache.h"

#define FC_DBG_SCANV 1

static FT_Library ftLibrary;

void kprintf(char *fmt,...);


#define D(x) x
//#define FcDebug() 0xffffffffUL
#define FcDebug() 0
#define FcResultMatch TRUE


struct fontlangset
{
};

struct fontcharset
{
};

/* return an id for next insertion. if not found returns 0 */

static int fcPatternCheckEntry(struct fontpattern *pat, int element)
{	
	struct patternentry *pe;
	
	ITERATELIST(pe, &pat->entries)
	{
		if (pe->element == element)
			return pe->id + 1; /* elements with high ids are at the begining of the list */
	}
    	
	return 0;
}


static int fcPatternAddEntry(struct fontpattern *pat, int element, int type, union patternvalue *value, int excluding)
{
	struct patternentry *pe = calloc(1, sizeof(*pe));
	if (pe == NULL)
		return FALSE;
		
	pe->element = element;
	pe->type = type;
	pe->id = fcPatternCheckEntry(pat, element);
	pe->excluding = excluding;
		
	if (type == FC_ETYPE_STRING)
	{
		if (FcDebug () & FC_DBG_SCANV)
			kprintf("adding string entry to pattern. elt:%d. value:%s, id:%d, excluding:%d\n",  element, value->s, pe->id, excluding);
		pe->value.s = value->s != NULL ? strdup(value->s) : NULL;
	}
	else
	{
		if (FcDebug () & FC_DBG_SCANV)
			kprintf("adding int entry to pattern. elt:%d. value:%d, id:%d, excluding:%d\n",  element, value->i, pe->id, excluding);
		pe->value.p = value->p;
	}
	
	ADDHEAD(&pat->entries, pe);	/* add to head so elements with high ids are first. this is 'normal' mode */
	
	return TRUE;
}

static int fcPatternAddEntryRaw(struct fontpattern *pat, int element, int type, union patternvalue *value, int id)
{
	struct patternentry *pe = calloc(1, sizeof(*pe));
	if (pe == NULL)
		return FALSE;
		
	pe->element = element;
	pe->type = type;
	pe->id = id;
	pe->value.p = value->p; /* no duplication. string is preallocated */
	
	ADDTAIL(&pat->entries, pe);	/* when loading from cache we put entries in the order they were written in */
	
	return TRUE;
}



int fcPatternAddString(struct fontpattern *pat, int element, char *value)
{
	union patternvalue v;
	v.s = value;
	return fcPatternAddEntry(pat, element, FC_ETYPE_STRING, &v, FALSE); /* value is copied */
}

int fcPatternAddInteger(struct fontpattern *pat, int element, int value)
{
	union patternvalue v;
	v.i = value;
	return fcPatternAddEntry(pat, element, FC_ETYPE_INTEGER, &v, FALSE);
}

int fcPatternAddExcludingInteger(struct fontpattern *pat, int element, int value)
{
	union patternvalue v;
	v.i = value;
	return fcPatternAddEntry(pat, element, FC_ETYPE_INTEGER, &v, TRUE);
}


/* if elt is found but is not string then return NULL! */

char *fcPatternGetString(struct fontpattern *pat, int element, int id)
{	
	struct patternentry *pe;
	
	ITERATELIST(pe, &pat->entries)
	{
		if (pe->element == element && pe->id == id)
		{
			if (pe->type != FC_ETYPE_STRING)
				kprintf("***ERROR*** Getting element %d as string!\n", element);
				
			return pe->value.s;
		}
	}
    	
	return NULL;	
}

int fcPatternGetInteger(struct fontpattern *pat, int element, int id)
{	
	struct patternentry *pe;
	
	ITERATELIST(pe, &pat->entries)
	{
		if (pe->element == element && pe->id == id)
		{
			if (pe->type != FC_ETYPE_INTEGER)
				kprintf("***ERROR*** Getting element %d as integer!\n", element);
				
			return pe->value.i;
		}
	}
    	
	return 0;	
}

struct patternentry *fcPatternGetEntry(struct fontpattern *pat, int element, int id)
{
	struct patternentry *pe;
	
	ITERATELIST(pe, &pat->entries)
	{
		if (pe->element == element && pe->id == id)
		{
			return pe;
		}
	}
    	
	return NULL;	
}


/* */

typedef struct {
    const unsigned short platform_id;
    const unsigned short encoding_id;
    const char	fromcode[12];
} FcFtEncoding;

typedef struct _FcCharEnt {
    unsigned short bmp;
    unsigned char encode;
} FcCharEnt;

typedef struct _FcCharMap {
    const FcCharEnt *ent;
    int nent;
} FcCharMap;

typedef struct _FcFontDecode {
    FT_Encoding	    encoding;
    const FcCharMap *map;
    FcChar32	    max;
} FcFontDecode;

/* Keep Han languages separated by eliminating languages that the codePageRange bits says aren't supported */

static const struct {
    char    	    bit;
    const FcChar8   lang[6];
} FcCodePageRange[] = {
    { 17,	"ja" },
    { 18,	"zh-cn" },
    { 19,	"ko" },
    { 20,	"zh-tw" },
};

#define NUM_CODE_PAGE_RANGE (int) (sizeof FcCodePageRange / sizeof FcCodePageRange[0])

#if 0
FcBool FcFreeTypeIsExclusiveLang (const FcChar8  *lang)
{
	int i;

    for (i = 0; i < NUM_CODE_PAGE_RANGE; i++)
    {
		if (FcLangCompare (lang, FcCodePageRange[i].lang) == FcLangEqual)
			return FcTrue;
    }
    return FcFalse;
}
#endif

#define TT_ENCODING_DONT_CARE	0xffff
#define FC_ENCODING_MAC_ROMAN	"MACINTOSH"

static const FcFtEncoding   fcFtEncoding[] = {
 {  TT_PLATFORM_APPLE_UNICODE,	TT_ENCODING_DONT_CARE,	"UCS-2BE" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_ID_ROMAN,	"MACINTOSH" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_ID_JAPANESE,	"SJIS" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_ID_UNICODE_CS,	"UTF-16BE" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_ID_SJIS,		"SJIS-WIN" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_ID_GB2312,	"GB2312" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_ID_BIG_5,		"BIG-5" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_ID_WANSUNG,	"Wansung" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_ID_JOHAB,		"Johab" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_ID_UCS_4,		"UCS-2BE" },
 {  TT_PLATFORM_ISO,		TT_ISO_ID_7BIT_ASCII,	"ASCII" },
 {  TT_PLATFORM_ISO,		TT_ISO_ID_10646,	"UCS-2BE" },
 {  TT_PLATFORM_ISO,		TT_ISO_ID_8859_1,	"ISO-8859-1" },
};

#define NUM_FC_FT_ENCODING  (int) (sizeof (fcFtEncoding) / sizeof (fcFtEncoding[0]))

typedef struct {
    const FT_UShort	platform_id;
    const FT_UShort	language_id;
    const char	lang[8];
} FcFtLanguage;

#define TT_LANGUAGE_DONT_CARE	0xffff

static const FcFtLanguage   fcFtLanguage[] = {
 {  TT_PLATFORM_APPLE_UNICODE,	TT_LANGUAGE_DONT_CARE,		    "" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ENGLISH,		    "en" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_FRENCH,		    "fr" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GERMAN,		    "de" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ITALIAN,		    "it" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_DUTCH,		    "nl" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SWEDISH,		    "sv" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SPANISH,		    "es" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_DANISH,		    "da" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_PORTUGUESE,	    "pt" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_NORWEGIAN,	    "no" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_HEBREW,		    "he" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_JAPANESE,		    "ja" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ARABIC,		    "ar" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_FINNISH,		    "fi" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GREEK,		    "el" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ICELANDIC,	    "is" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MALTESE,		    "mt" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TURKISH,		    "tr" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_CROATIAN,		    "hr" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_CHINESE_TRADITIONAL,  "zh-tw" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_URDU,		    "ur" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_HINDI,		    "hi" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_THAI,		    "th" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_KOREAN,		    "ko" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_LITHUANIAN,	    "lt" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_POLISH,		    "pl" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_HUNGARIAN,	    "hu" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ESTONIAN,		    "et" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_LETTISH,		    "lv" },
/* {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SAAMISK, ??? */
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_FAEROESE,		    "fo" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_FARSI,		    "fa" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_RUSSIAN,		    "ru" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_CHINESE_SIMPLIFIED,   "zh-cn" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_FLEMISH,		    "nl" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_IRISH,		    "ga" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ALBANIAN,		    "sq" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ROMANIAN,		    "ro" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_CZECH,		    "cs" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SLOVAK,		    "sk" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SLOVENIAN,	    "sl" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_YIDDISH,		    "yi" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SERBIAN,		    "sr" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MACEDONIAN,	    "mk" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_BULGARIAN,	    "bg" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_UKRAINIAN,	    "uk" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_BYELORUSSIAN,	    "be" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_UZBEK,		    "uz" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_KAZAKH,		    "kk" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_AZERBAIJANI,	    "az" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_AZERBAIJANI_CYRILLIC_SCRIPT, "az" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_AZERBAIJANI_ARABIC_SCRIPT,    "ar" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ARMENIAN,		    "hy" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GEORGIAN,		    "ka" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MOLDAVIAN,	    "mo" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_KIRGHIZ,		    "ky" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TAJIKI,		    "tg" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TURKMEN,		    "tk" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MONGOLIAN,	    "mo" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MONGOLIAN_MONGOLIAN_SCRIPT,"mo" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MONGOLIAN_CYRILLIC_SCRIPT, "mo" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_PASHTO,		    "ps" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_KURDISH,		    "ku" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_KASHMIRI,		    "ks" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SINDHI,		    "sd" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TIBETAN,		    "bo" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_NEPALI,		    "ne" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SANSKRIT,		    "sa" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MARATHI,		    "mr" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_BENGALI,		    "bn" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ASSAMESE,		    "as" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GUJARATI,		    "gu" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_PUNJABI,		    "pa" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ORIYA,		    "or" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MALAYALAM,	    "ml" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_KANNADA,		    "kn" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TAMIL,		    "ta" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TELUGU,		    "te" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SINHALESE,	    "si" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_BURMESE,		    "my" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_KHMER,		    "km" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_LAO,		    "lo" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_VIETNAMESE,	    "vi" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_INDONESIAN,	    "id" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TAGALOG,		    "tl" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MALAY_ROMAN_SCRIPT,   "ms" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MALAY_ARABIC_SCRIPT,  "ms" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_AMHARIC,		    "am" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TIGRINYA,		    "ti" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GALLA,		    "om" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SOMALI,		    "so" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SWAHILI,		    "sw" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_RUANDA,		    "rw" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_RUNDI,		    "rn" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_CHEWA,		    "ny" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MALAGASY,		    "mg" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_ESPERANTO,	    "eo" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_WELSH,		    "cy" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_BASQUE,		    "eu" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_CATALAN,		    "ca" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_LATIN,		    "la" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_QUECHUA,		    "qu" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GUARANI,		    "gn" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_AYMARA,		    "ay" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TATAR,		    "tt" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_UIGHUR,		    "ug" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_DZONGKHA,		    "dz" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_JAVANESE,		    "jw" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SUNDANESE,	    "su" },
    
#if 0  /* these seem to be errors that have been dropped */

 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SCOTTISH_GAELIC },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_IRISH_GAELIC },

#endif
    
  /* The following codes are new as of 2000-03-10 */
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GALICIAN,		    "gl" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_AFRIKAANS,	    "af" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_BRETON,		    "br" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_INUKTITUT,	    "iu" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_SCOTTISH_GAELIC,	    "gd" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_MANX_GAELIC,	    "gv" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_IRISH_GAELIC,	    "ga" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_TONGAN,		    "to" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GREEK_POLYTONIC,	    "el" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_GREELANDIC,	    "ik" },
 {  TT_PLATFORM_MACINTOSH,	TT_MAC_LANGID_AZERBAIJANI_ROMAN_SCRIPT,"az" },

 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_SAUDI_ARABIA,	"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_IRAQ,		"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_EGYPT,		"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_LIBYA,		"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_ALGERIA,		"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_MOROCCO,		"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_TUNISIA,		"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_OMAN,		"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_YEMEN,		"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_SYRIA,		"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_JORDAN,		"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_LEBANON,		"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_KUWAIT,		"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_UAE,		"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_BAHRAIN,		"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_QATAR,		"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_BULGARIAN_BULGARIA,	"bg" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CATALAN_SPAIN,		"ca" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CHINESE_TAIWAN,		"zh-tw" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CHINESE_PRC,		"zh-cn" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CHINESE_HONG_KONG,		"zh-hk" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CHINESE_SINGAPORE,		"zh-sg" },

 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CHINESE_MACAU,		"zh-mo" },

 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CZECH_CZECH_REPUBLIC,	"cs" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_DANISH_DENMARK,		"da" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GERMAN_GERMANY,		"de" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GERMAN_SWITZERLAND,	"de" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GERMAN_AUSTRIA,		"de" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GERMAN_LUXEMBOURG,		"de" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GERMAN_LIECHTENSTEI,	"de" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GREEK_GREECE,		"el" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_UNITED_STATES,	"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_UNITED_KINGDOM,	"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_AUSTRALIA,		"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_CANADA,		"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_NEW_ZEALAND,	"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_IRELAND,		"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_SOUTH_AFRICA,	"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_JAMAICA,		"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_CARIBBEAN,		"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_BELIZE,		"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_TRINIDAD,		"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_ZIMBABWE,		"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_PHILIPPINES,	"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_SPAIN_TRADITIONAL_SORT,"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_MEXICO,		"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_SPAIN_INTERNATIONAL_SORT,"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_GUATEMALA,		"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_COSTA_RICA,	"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_PANAMA,		"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_DOMINICAN_REPUBLIC,"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_VENEZUELA,		"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_COLOMBIA,		"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_PERU,		"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_ARGENTINA,		"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_ECUADOR,		"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_CHILE,		"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_URUGUAY,		"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_PARAGUAY,		"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_BOLIVIA,		"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_EL_SALVADOR,	"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_HONDURAS,		"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_NICARAGUA,		"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_PUERTO_RICO,	"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FINNISH_FINLAND,		"fi" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_FRANCE,		"fr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_BELGIUM,		"fr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_CANADA,		"fr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_SWITZERLAND,	"fr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_LUXEMBOURG,		"fr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_MONACO,		"fr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_HEBREW_ISRAEL,		"he" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_HUNGARIAN_HUNGARY,		"hu" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ICELANDIC_ICELAND,		"is" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ITALIAN_ITALY,		"it" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ITALIAN_SWITZERLAND,	"it" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_JAPANESE_JAPAN,		"ja" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KOREAN_EXTENDED_WANSUNG_KOREA,"ko" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KOREAN_JOHAB_KOREA,	"ko" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_DUTCH_NETHERLANDS,		"nl" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_DUTCH_BELGIUM,		"nl" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_NORWEGIAN_NORWAY_BOKMAL,	"no" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_NORWEGIAN_NORWAY_NYNORSK,	"nn" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_POLISH_POLAND,		"pl" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_PORTUGUESE_BRAZIL,		"pt" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_PORTUGUESE_PORTUGAL,	"pt" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_RHAETO_ROMANIC_SWITZERLAND,"rm" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ROMANIAN_ROMANIA,		"ro" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MOLDAVIAN_MOLDAVIA,	"mo" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_RUSSIAN_RUSSIA,		"ru" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_RUSSIAN_MOLDAVIA,		"ru" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CROATIAN_CROATIA,		"hr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SERBIAN_SERBIA_LATIN,	"sr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SERBIAN_SERBIA_CYRILLIC,	"sr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SLOVAK_SLOVAKIA,		"sk" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ALBANIAN_ALBANIA,		"sq" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SWEDISH_SWEDEN,		"sv" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SWEDISH_FINLAND,		"sv" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_THAI_THAILAND,		"th" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TURKISH_TURKEY,		"tr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_URDU_PAKISTAN,		"ur" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_INDONESIAN_INDONESIA,	"id" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_UKRAINIAN_UKRAINE,		"uk" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_BELARUSIAN_BELARUS,	"be" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SLOVENE_SLOVENIA,		"sl" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ESTONIAN_ESTONIA,		"et" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_LATVIAN_LATVIA,		"lv" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_LITHUANIAN_LITHUANIA,	"lt" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CLASSIC_LITHUANIAN_LITHUANIA,"lt" },

#ifdef TT_MS_LANGID_MAORI_NEW_ZELAND
    /* this seems to be an error that have been dropped */
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MAORI_NEW_ZEALAND,		"mi" },
#endif

 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FARSI_IRAN,		"fa" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_VIETNAMESE_VIET_NAM,	"vi" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARMENIAN_ARMENIA,		"hy" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_AZERI_AZERBAIJAN_LATIN,	"az" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_AZERI_AZERBAIJAN_CYRILLIC,	"az" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_BASQUE_SPAIN,		"eu" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SORBIAN_GERMANY,		"wen" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MACEDONIAN_MACEDONIA,	"mk" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SUTU_SOUTH_AFRICA,		"st" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TSONGA_SOUTH_AFRICA,	"ts" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TSWANA_SOUTH_AFRICA,	"tn" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_VENDA_SOUTH_AFRICA,	"ven" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_XHOSA_SOUTH_AFRICA,	"xh" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ZULU_SOUTH_AFRICA,		"zu" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_AFRIKAANS_SOUTH_AFRICA,	"af" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GEORGIAN_GEORGIA,		"ka" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FAEROESE_FAEROE_ISLANDS,	"fo" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_HINDI_INDIA,		"hi" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MALTESE_MALTA,		"mt" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SAAMI_LAPONIA,		"se" },

 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SCOTTISH_GAELIC_UNITED_KINGDOM,"gd" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_IRISH_GAELIC_IRELAND,	"ga" },

 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MALAY_MALAYSIA,		"ms" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MALAY_BRUNEI_DARUSSALAM,	"ms" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KAZAK_KAZAKSTAN,		"kk" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SWAHILI_KENYA,		"sw" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_UZBEK_UZBEKISTAN_LATIN,	"uz" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_UZBEK_UZBEKISTAN_CYRILLIC,	"uz" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TATAR_TATARSTAN,		"tt" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_BENGALI_INDIA,		"bn" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_PUNJABI_INDIA,		"pa" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GUJARATI_INDIA,		"gu" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ORIYA_INDIA,		"or" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TAMIL_INDIA,		"ta" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TELUGU_INDIA,		"te" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KANNADA_INDIA,		"kn" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MALAYALAM_INDIA,		"ml" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ASSAMESE_INDIA,		"as" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MARATHI_INDIA,		"mr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SANSKRIT_INDIA,		"sa" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KONKANI_INDIA,		"kok" },

  /* new as of 2001-01-01 */
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ARABIC_GENERAL,		"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CHINESE_GENERAL,		"zh" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_GENERAL,		"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_WEST_INDIES,	"fr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_REUNION,		"fr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_CONGO,		"fr" },

 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_SENEGAL,		"fr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_CAMEROON,		"fr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_COTE_D_IVOIRE,	"fr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_MALI,		"fr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_BOSNIAN_BOSNIA_HERZEGOVINA,"bs" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_URDU_INDIA,		"ur" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TAJIK_TAJIKISTAN,		"tg" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_YIDDISH_GERMANY,		"yi" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KIRGHIZ_KIRGHIZSTAN,	"ky" },

 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TURKMEN_TURKMENISTAN,	"tk" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MONGOLIAN_MONGOLIA,	"mn" },

  /* the following seems to be inconsistent;
     here is the current "official" way: */
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TIBETAN_BHUTAN,		"bo" },
  /* and here is what is used by Passport SDK */
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TIBETAN_CHINA,		"bo" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_DZONGHKA_BHUTAN,		"dz" },
  /* end of inconsistency */

 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_WELSH_WALES,		"cy" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KHMER_CAMBODIA,		"km" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_LAO_LAOS,			"lo" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_BURMESE_MYANMAR,		"my" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GALICIAN_SPAIN,		"gl" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MANIPURI_INDIA,		"mni" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SINDHI_INDIA,		"sd" },
  /* the following one is only encountered in Microsoft RTF specification */
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KASHMIRI_PAKISTAN,		"ks" },
  /* the following one is not in the Passport list, looks like an omission */
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KASHMIRI_INDIA,		"ks" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_NEPALI_NEPAL,		"ne" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_NEPALI_INDIA,		"ne" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRISIAN_NETHERLANDS,	"fy" },

  /* new as of 2001-03-01 (from Office Xp) */
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_HONG_KONG,		"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_INDIA,		"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_MALAYSIA,		"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_ENGLISH_SINGAPORE,		"en" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SYRIAC_SYRIA,		"syr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SINHALESE_SRI_LANKA,	"si" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_CHEROKEE_UNITED_STATES,	"chr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_INUKTITUT_CANADA,		"iu" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_AMHARIC_ETHIOPIA,		"am" },
#if 0
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TAMAZIGHT_MOROCCO },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TAMAZIGHT_MOROCCO_LATIN },
#endif
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_PASHTO_AFGHANISTAN,	"ps" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FILIPINO_PHILIPPINES,	"phi" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_DHIVEHI_MALDIVES,		"div" },
    
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_OROMO_ETHIOPIA,		"om" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TIGRIGNA_ETHIOPIA,		"ti" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_TIGRIGNA_ERYTHREA,		"ti" },

  /* New additions from Windows Xp/Passport SDK 2001-11-10. */

  /* don't ask what this one means... It is commented out currently. */
#if 0
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GREEK_GREECE2 },
#endif

 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_UNITED_STATES,	"es" },
  /* The following two IDs blatantly violate MS specs by using a */
  /* sublanguage >,.                                         */
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SPANISH_LATIN_AMERICA,	"es" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_NORTH_AFRICA,	"fr" },

 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_MOROCCO,		"fr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FRENCH_HAITI,		"fr" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_BENGALI_BANGLADESH,	"bn" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_PUNJABI_ARABIC_PAKISTAN,	"ar" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_MONGOLIAN_MONGOLIA_MONGOLIAN,"mn" },
#if 0
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_EDO_NIGERIA },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_FULFULDE_NIGERIA },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_IBIBIO_NIGERIA },
#endif
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_HAUSA_NIGERIA,		"ha" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_YORUBA_NIGERIA,		"yo" },
  /* language codes from, to, are (still) unknown. */
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_IGBO_NIGERIA,		"ibo" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_KANURI_NIGERIA,		"kau" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_GUARANI_PARAGUAY,		"gn" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_HAWAIIAN_UNITED_STATES,	"haw" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_LATIN,			"la" },
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_SOMALI_SOMALIA,		"so" },
#if 0
  /* Note: Yi does not have a (proper) ISO 639-2 code, since it is mostly */
  /*       not written (but OTOH the peculiar writing system is worth     */
  /*       studying).                                                     */
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_YI_CHINA },
#endif
 {  TT_PLATFORM_MICROSOFT,	TT_MS_LANGID_PAPIAMENTU_NETHERLANDS_ANTILLES,"pap" },
};

#define NUM_FC_FT_LANGUAGE  (int) (sizeof (fcFtLanguage) / sizeof (fcFtLanguage[0]))


static const FT_UShort platform_order[] = {
    TT_PLATFORM_MICROSOFT,
    TT_PLATFORM_APPLE_UNICODE,
    TT_PLATFORM_MACINTOSH,
};
#define NUM_PLATFORM_ORDER (sizeof (platform_order) / sizeof (platform_order[0]))

static const FT_UShort nameid_order[] = {
#ifdef TT_NAME_ID_WWS_FAMILY
    TT_NAME_ID_WWS_FAMILY,
#endif
    TT_NAME_ID_PREFERRED_FAMILY,
    TT_NAME_ID_FONT_FAMILY,
    TT_NAME_ID_MAC_FULL_NAME,
    TT_NAME_ID_FULL_NAME,
#ifdef TT_NAME_ID_WWS_SUBFAMILY
    TT_NAME_ID_WWS_SUBFAMILY,
#endif
    TT_NAME_ID_PREFERRED_SUBFAMILY,
    TT_NAME_ID_FONT_SUBFAMILY,
    TT_NAME_ID_TRADEMARK,
    TT_NAME_ID_MANUFACTURER,
};

#define NUM_NAMEID_ORDER  (sizeof (nameid_order) / sizeof (nameid_order[0]))

typedef struct {
    unsigned short language_id;
    char fromcode[12];
} FcMacRomanFake;

static const FcMacRomanFake fcMacRomanFake[] = {
 {  TT_MS_LANGID_JAPANESE_JAPAN,	"SJIS-WIN" },
 {  TT_MS_LANGID_ENGLISH_UNITED_STATES,	"ASCII" },
};

#define NUM_FC_MAC_ROMAN_FAKE	(int) (sizeof (fcMacRomanFake) / sizeof (fcMacRomanFake[0]))


typedef struct _FcStringConst {
    const FcChar8   *name;
    int		    value;
} FcStringConst;

static int FcStringContainsConst (const FcChar8	    *string, const FcStringConst  *c, int		    nc)
{
    int	i;

    for (i = 0; i < nc; i++)
    {
	if (c[i].name[0] == '<')
	{
	    if (FcStrContainsWord (string, c[i].name + 1))
		return c[i].value;
	}
	else
	{
	    if (FcStrContainsIgnoreBlanksAndCase (string, c[i].name))
		return c[i].value;
	}
    }
    return -1;
}

typedef FcChar8 *FC8;

static const FcStringConst  weightConsts[] = {
    { (FC8) "thin",		FC_WEIGHT_THIN },
    { (FC8) "extralight",	FC_WEIGHT_EXTRALIGHT },
    { (FC8) "ultralight",	FC_WEIGHT_ULTRALIGHT },
    { (FC8) "light",		FC_WEIGHT_LIGHT },
    { (FC8) "book",		FC_WEIGHT_BOOK },
    { (FC8) "regular",		FC_WEIGHT_REGULAR },
    { (FC8) "normal",		FC_WEIGHT_NORMAL },
    { (FC8) "medium",		FC_WEIGHT_MEDIUM },
    { (FC8) "demibold",		FC_WEIGHT_DEMIBOLD },
    { (FC8) "demi",		FC_WEIGHT_DEMIBOLD },
    { (FC8) "semibold",		FC_WEIGHT_SEMIBOLD },
    { (FC8) "extrabold",	FC_WEIGHT_EXTRABOLD },
    { (FC8) "superbold",	FC_WEIGHT_EXTRABOLD },
    { (FC8) "ultrabold",	FC_WEIGHT_ULTRABOLD },
    { (FC8) "bold",		FC_WEIGHT_BOLD },
    { (FC8) "ultrablack",	FC_WEIGHT_ULTRABLACK },
    { (FC8) "superblack",	FC_WEIGHT_EXTRABLACK },
    { (FC8) "extrablack",	FC_WEIGHT_EXTRABLACK },
    { (FC8) "<ultra",		FC_WEIGHT_ULTRABOLD }, /* only if a word */
    { (FC8) "black",		FC_WEIGHT_BLACK },
    { (FC8) "heavy",		FC_WEIGHT_HEAVY },
};

#define NUM_WEIGHT_CONSTS  (int) (sizeof (weightConsts) / sizeof (weightConsts[0]))

#define FcIsWeight(s)	    FcStringIsConst(s,weightConsts,NUM_WEIGHT_CONSTS)
#define FcContainsWeight(s) FcStringContainsConst (s,weightConsts,NUM_WEIGHT_CONSTS)

static const FcStringConst  widthConsts[] = {
    { (FC8) "ultracondensed",	FC_WIDTH_ULTRACONDENSED },
    { (FC8) "extracondensed",	FC_WIDTH_EXTRACONDENSED },
    { (FC8) "semicondensed",	FC_WIDTH_SEMICONDENSED },
    { (FC8) "condensed",	FC_WIDTH_CONDENSED },	/* must be after *condensed */
    { (FC8) "normal",		FC_WIDTH_NORMAL },
    { (FC8) "semiexpanded",	FC_WIDTH_SEMIEXPANDED },
    { (FC8) "extraexpanded",	FC_WIDTH_EXTRAEXPANDED },
    { (FC8) "ultraexpanded",	FC_WIDTH_ULTRAEXPANDED },
    { (FC8) "expanded",		FC_WIDTH_EXPANDED },	/* must be after *expanded */
    { (FC8) "extended",		FC_WIDTH_EXPANDED },
};

#define NUM_WIDTH_CONSTS    (int) (sizeof (widthConsts) / sizeof (widthConsts[0]))

#define FcIsWidth(s)	    FcStringIsConst(s,widthConsts,NUM_WIDTH_CONSTS)
#define FcContainsWidth(s)  FcStringContainsConst (s,widthConsts,NUM_WIDTH_CONSTS)

static const FcStringConst  slantConsts[] = {
    { (FC8) "italic",		FC_SLANT_ITALIC },
    { (FC8) "kursiv",		FC_SLANT_ITALIC },
    { (FC8) "oblique",		FC_SLANT_OBLIQUE },
};

#define NUM_SLANT_CONSTS    (int) (sizeof (slantConsts) / sizeof (slantConsts[0]))

#define FcIsSlant(s)	    FcStringIsConst(s,slantConsts,NUM_SLANT_CONSTS)
#define FcContainsSlant(s)  FcStringContainsConst (s,slantConsts,NUM_SLANT_CONSTS)   


struct fontpattern *fcPatternAlloc(void)
{
	struct fontpattern *pat = calloc(1, sizeof(struct fontpattern));
	
	if (pat != NULL)
	{
		NewList(&pat->entries);
	}
	
	return pat;
}

void fcPatternDestroy(struct fontpattern *pat)
{
	if (pat != NULL)
	{
		struct patternentry *pe, *pen;

		ITERATELISTSAFE(pe, pen, &pat->entries)
		{
			if (pe->type == FC_ETYPE_STRING)
			{
				if (pe->value.s != NULL)
					free(pe->value.s);
					
			}
			
			REMOVE(pe);
			free(pe);			
		}
	
		free(pat);
	}
}

#if 0

struct fontpattern *fcQueryFace(const FT_Face face, char *fname, int id)
{
}

#else

static const FcCharEnt AppleRomanEnt[] = {
    { 0x0020, 0x20 }, /* SPACE */
    { 0x0021, 0x21 }, /* EXCLAMATION MARK */
    { 0x0022, 0x22 }, /* QUOTATION MARK */
    { 0x0023, 0x23 }, /* NUMBER SIGN */
    { 0x0024, 0x24 }, /* DOLLAR SIGN */
    { 0x0025, 0x25 }, /* PERCENT SIGN */
    { 0x0026, 0x26 }, /* AMPERSAND */
    { 0x0027, 0x27 }, /* APOSTROPHE */
    { 0x0028, 0x28 }, /* LEFT PARENTHESIS */
    { 0x0029, 0x29 }, /* RIGHT PARENTHESIS */
    { 0x002A, 0x2A }, /* ASTERISK */
    { 0x002B, 0x2B }, /* PLUS SIGN */
    { 0x002C, 0x2C }, /* COMMA */
    { 0x002D, 0x2D }, /* HYPHEN-MINUS */
    { 0x002E, 0x2E }, /* FULL STOP */
    { 0x002F, 0x2F }, /* SOLIDUS */
    { 0x0030, 0x30 }, /* DIGIT ZERO */
    { 0x0031, 0x31 }, /* DIGIT ONE */
    { 0x0032, 0x32 }, /* DIGIT TWO */
    { 0x0033, 0x33 }, /* DIGIT THREE */
    { 0x0034, 0x34 }, /* DIGIT FOUR */
    { 0x0035, 0x35 }, /* DIGIT FIVE */
    { 0x0036, 0x36 }, /* DIGIT SIX */
    { 0x0037, 0x37 }, /* DIGIT SEVEN */
    { 0x0038, 0x38 }, /* DIGIT EIGHT */
    { 0x0039, 0x39 }, /* DIGIT NINE */
    { 0x003A, 0x3A }, /* COLON */
    { 0x003B, 0x3B }, /* SEMICOLON */
    { 0x003C, 0x3C }, /* LESS-THAN SIGN */
    { 0x003D, 0x3D }, /* EQUALS SIGN */
    { 0x003E, 0x3E }, /* GREATER-THAN SIGN */
    { 0x003F, 0x3F }, /* QUESTION MARK */
    { 0x0040, 0x40 }, /* COMMERCIAL AT */
    { 0x0041, 0x41 }, /* LATIN CAPITAL LETTER A */
    { 0x0042, 0x42 }, /* LATIN CAPITAL LETTER B */
    { 0x0043, 0x43 }, /* LATIN CAPITAL LETTER C */
    { 0x0044, 0x44 }, /* LATIN CAPITAL LETTER D */
    { 0x0045, 0x45 }, /* LATIN CAPITAL LETTER E */
    { 0x0046, 0x46 }, /* LATIN CAPITAL LETTER F */
    { 0x0047, 0x47 }, /* LATIN CAPITAL LETTER G */
    { 0x0048, 0x48 }, /* LATIN CAPITAL LETTER H */
    { 0x0049, 0x49 }, /* LATIN CAPITAL LETTER I */
    { 0x004A, 0x4A }, /* LATIN CAPITAL LETTER J */
    { 0x004B, 0x4B }, /* LATIN CAPITAL LETTER K */
    { 0x004C, 0x4C }, /* LATIN CAPITAL LETTER L */
    { 0x004D, 0x4D }, /* LATIN CAPITAL LETTER M */
    { 0x004E, 0x4E }, /* LATIN CAPITAL LETTER N */
    { 0x004F, 0x4F }, /* LATIN CAPITAL LETTER O */
    { 0x0050, 0x50 }, /* LATIN CAPITAL LETTER P */
    { 0x0051, 0x51 }, /* LATIN CAPITAL LETTER Q */
    { 0x0052, 0x52 }, /* LATIN CAPITAL LETTER R */
    { 0x0053, 0x53 }, /* LATIN CAPITAL LETTER S */
    { 0x0054, 0x54 }, /* LATIN CAPITAL LETTER T */
    { 0x0055, 0x55 }, /* LATIN CAPITAL LETTER U */
    { 0x0056, 0x56 }, /* LATIN CAPITAL LETTER V */
    { 0x0057, 0x57 }, /* LATIN CAPITAL LETTER W */
    { 0x0058, 0x58 }, /* LATIN CAPITAL LETTER X */
    { 0x0059, 0x59 }, /* LATIN CAPITAL LETTER Y */
    { 0x005A, 0x5A }, /* LATIN CAPITAL LETTER Z */
    { 0x005B, 0x5B }, /* LEFT SQUARE BRACKET */
    { 0x005C, 0x5C }, /* REVERSE SOLIDUS */
    { 0x005D, 0x5D }, /* RIGHT SQUARE BRACKET */
    { 0x005E, 0x5E }, /* CIRCUMFLEX ACCENT */
    { 0x005F, 0x5F }, /* LOW LINE */
    { 0x0060, 0x60 }, /* GRAVE ACCENT */
    { 0x0061, 0x61 }, /* LATIN SMALL LETTER A */
    { 0x0062, 0x62 }, /* LATIN SMALL LETTER B */
    { 0x0063, 0x63 }, /* LATIN SMALL LETTER C */
    { 0x0064, 0x64 }, /* LATIN SMALL LETTER D */
    { 0x0065, 0x65 }, /* LATIN SMALL LETTER E */
    { 0x0066, 0x66 }, /* LATIN SMALL LETTER F */
    { 0x0067, 0x67 }, /* LATIN SMALL LETTER G */
    { 0x0068, 0x68 }, /* LATIN SMALL LETTER H */
    { 0x0069, 0x69 }, /* LATIN SMALL LETTER I */
    { 0x006A, 0x6A }, /* LATIN SMALL LETTER J */
    { 0x006B, 0x6B }, /* LATIN SMALL LETTER K */
    { 0x006C, 0x6C }, /* LATIN SMALL LETTER L */
    { 0x006D, 0x6D }, /* LATIN SMALL LETTER M */
    { 0x006E, 0x6E }, /* LATIN SMALL LETTER N */
    { 0x006F, 0x6F }, /* LATIN SMALL LETTER O */
    { 0x0070, 0x70 }, /* LATIN SMALL LETTER P */
    { 0x0071, 0x71 }, /* LATIN SMALL LETTER Q */
    { 0x0072, 0x72 }, /* LATIN SMALL LETTER R */
    { 0x0073, 0x73 }, /* LATIN SMALL LETTER S */
    { 0x0074, 0x74 }, /* LATIN SMALL LETTER T */
    { 0x0075, 0x75 }, /* LATIN SMALL LETTER U */
    { 0x0076, 0x76 }, /* LATIN SMALL LETTER V */
    { 0x0077, 0x77 }, /* LATIN SMALL LETTER W */
    { 0x0078, 0x78 }, /* LATIN SMALL LETTER X */
    { 0x0079, 0x79 }, /* LATIN SMALL LETTER Y */
    { 0x007A, 0x7A }, /* LATIN SMALL LETTER Z */
    { 0x007B, 0x7B }, /* LEFT CURLY BRACKET */
    { 0x007C, 0x7C }, /* VERTICAL LINE */
    { 0x007D, 0x7D }, /* RIGHT CURLY BRACKET */
    { 0x007E, 0x7E }, /* TILDE */
    { 0x00A0, 0xCA }, /* NO-BREAK SPACE */
    { 0x00A1, 0xC1 }, /* INVERTED EXCLAMATION MARK */
    { 0x00A2, 0xA2 }, /* CENT SIGN */
    { 0x00A3, 0xA3 }, /* POUND SIGN */
    { 0x00A5, 0xB4 }, /* YEN SIGN */
    { 0x00A7, 0xA4 }, /* SECTION SIGN */
    { 0x00A8, 0xAC }, /* DIAERESIS */
    { 0x00A9, 0xA9 }, /* COPYRIGHT SIGN */
    { 0x00AA, 0xBB }, /* FEMININE ORDINAL INDICATOR */
    { 0x00AB, 0xC7 }, /* LEFT-POINTING DOUBLE ANGLE QUOTATION MARK */
    { 0x00AC, 0xC2 }, /* NOT SIGN */
    { 0x00AE, 0xA8 }, /* REGISTERED SIGN */
    { 0x00AF, 0xF8 }, /* MACRON */
    { 0x00B0, 0xA1 }, /* DEGREE SIGN */
    { 0x00B1, 0xB1 }, /* PLUS-MINUS SIGN */
    { 0x00B4, 0xAB }, /* ACUTE ACCENT */
    { 0x00B5, 0xB5 }, /* MICRO SIGN */
    { 0x00B6, 0xA6 }, /* PILCROW SIGN */
    { 0x00B7, 0xE1 }, /* MIDDLE DOT */
    { 0x00B8, 0xFC }, /* CEDILLA */
    { 0x00BA, 0xBC }, /* MASCULINE ORDINAL INDICATOR */
    { 0x00BB, 0xC8 }, /* RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK */
    { 0x00BF, 0xC0 }, /* INVERTED QUESTION MARK */
    { 0x00C0, 0xCB }, /* LATIN CAPITAL LETTER A WITH GRAVE */
    { 0x00C1, 0xE7 }, /* LATIN CAPITAL LETTER A WITH ACUTE */
    { 0x00C2, 0xE5 }, /* LATIN CAPITAL LETTER A WITH CIRCUMFLEX */
    { 0x00C3, 0xCC }, /* LATIN CAPITAL LETTER A WITH TILDE */
    { 0x00C4, 0x80 }, /* LATIN CAPITAL LETTER A WITH DIAERESIS */
    { 0x00C5, 0x81 }, /* LATIN CAPITAL LETTER A WITH RING ABOVE */
    { 0x00C6, 0xAE }, /* LATIN CAPITAL LETTER AE */
    { 0x00C7, 0x82 }, /* LATIN CAPITAL LETTER C WITH CEDILLA */
    { 0x00C8, 0xE9 }, /* LATIN CAPITAL LETTER E WITH GRAVE */
    { 0x00C9, 0x83 }, /* LATIN CAPITAL LETTER E WITH ACUTE */
    { 0x00CA, 0xE6 }, /* LATIN CAPITAL LETTER E WITH CIRCUMFLEX */
    { 0x00CB, 0xE8 }, /* LATIN CAPITAL LETTER E WITH DIAERESIS */
    { 0x00CC, 0xED }, /* LATIN CAPITAL LETTER I WITH GRAVE */
    { 0x00CD, 0xEA }, /* LATIN CAPITAL LETTER I WITH ACUTE */
    { 0x00CE, 0xEB }, /* LATIN CAPITAL LETTER I WITH CIRCUMFLEX */
    { 0x00CF, 0xEC }, /* LATIN CAPITAL LETTER I WITH DIAERESIS */
    { 0x00D1, 0x84 }, /* LATIN CAPITAL LETTER N WITH TILDE */
    { 0x00D2, 0xF1 }, /* LATIN CAPITAL LETTER O WITH GRAVE */
    { 0x00D3, 0xEE }, /* LATIN CAPITAL LETTER O WITH ACUTE */
    { 0x00D4, 0xEF }, /* LATIN CAPITAL LETTER O WITH CIRCUMFLEX */
    { 0x00D5, 0xCD }, /* LATIN CAPITAL LETTER O WITH TILDE */
    { 0x00D6, 0x85 }, /* LATIN CAPITAL LETTER O WITH DIAERESIS */
    { 0x00D8, 0xAF }, /* LATIN CAPITAL LETTER O WITH STROKE */
    { 0x00D9, 0xF4 }, /* LATIN CAPITAL LETTER U WITH GRAVE */
    { 0x00DA, 0xF2 }, /* LATIN CAPITAL LETTER U WITH ACUTE */
    { 0x00DB, 0xF3 }, /* LATIN CAPITAL LETTER U WITH CIRCUMFLEX */
    { 0x00DC, 0x86 }, /* LATIN CAPITAL LETTER U WITH DIAERESIS */
    { 0x00DF, 0xA7 }, /* LATIN SMALL LETTER SHARP S */
    { 0x00E0, 0x88 }, /* LATIN SMALL LETTER A WITH GRAVE */
    { 0x00E1, 0x87 }, /* LATIN SMALL LETTER A WITH ACUTE */
    { 0x00E2, 0x89 }, /* LATIN SMALL LETTER A WITH CIRCUMFLEX */
    { 0x00E3, 0x8B }, /* LATIN SMALL LETTER A WITH TILDE */
    { 0x00E4, 0x8A }, /* LATIN SMALL LETTER A WITH DIAERESIS */
    { 0x00E5, 0x8C }, /* LATIN SMALL LETTER A WITH RING ABOVE */
    { 0x00E6, 0xBE }, /* LATIN SMALL LETTER AE */
    { 0x00E7, 0x8D }, /* LATIN SMALL LETTER C WITH CEDILLA */
    { 0x00E8, 0x8F }, /* LATIN SMALL LETTER E WITH GRAVE */
    { 0x00E9, 0x8E }, /* LATIN SMALL LETTER E WITH ACUTE */
    { 0x00EA, 0x90 }, /* LATIN SMALL LETTER E WITH CIRCUMFLEX */
    { 0x00EB, 0x91 }, /* LATIN SMALL LETTER E WITH DIAERESIS */
    { 0x00EC, 0x93 }, /* LATIN SMALL LETTER I WITH GRAVE */
    { 0x00ED, 0x92 }, /* LATIN SMALL LETTER I WITH ACUTE */
    { 0x00EE, 0x94 }, /* LATIN SMALL LETTER I WITH CIRCUMFLEX */
    { 0x00EF, 0x95 }, /* LATIN SMALL LETTER I WITH DIAERESIS */
    { 0x00F1, 0x96 }, /* LATIN SMALL LETTER N WITH TILDE */
    { 0x00F2, 0x98 }, /* LATIN SMALL LETTER O WITH GRAVE */
    { 0x00F3, 0x97 }, /* LATIN SMALL LETTER O WITH ACUTE */
    { 0x00F4, 0x99 }, /* LATIN SMALL LETTER O WITH CIRCUMFLEX */
    { 0x00F5, 0x9B }, /* LATIN SMALL LETTER O WITH TILDE */
    { 0x00F6, 0x9A }, /* LATIN SMALL LETTER O WITH DIAERESIS */
    { 0x00F7, 0xD6 }, /* DIVISION SIGN */
    { 0x00F8, 0xBF }, /* LATIN SMALL LETTER O WITH STROKE */
    { 0x00F9, 0x9D }, /* LATIN SMALL LETTER U WITH GRAVE */
    { 0x00FA, 0x9C }, /* LATIN SMALL LETTER U WITH ACUTE */
    { 0x00FB, 0x9E }, /* LATIN SMALL LETTER U WITH CIRCUMFLEX */
    { 0x00FC, 0x9F }, /* LATIN SMALL LETTER U WITH DIAERESIS */
    { 0x00FF, 0xD8 }, /* LATIN SMALL LETTER Y WITH DIAERESIS */
    { 0x0131, 0xF5 }, /* LATIN SMALL LETTER DOTLESS I */
    { 0x0152, 0xCE }, /* LATIN CAPITAL LIGATURE OE */
    { 0x0153, 0xCF }, /* LATIN SMALL LIGATURE OE */
    { 0x0178, 0xD9 }, /* LATIN CAPITAL LETTER Y WITH DIAERESIS */
    { 0x0192, 0xC4 }, /* LATIN SMALL LETTER F WITH HOOK */
    { 0x02C6, 0xF6 }, /* MODIFIER LETTER CIRCUMFLEX ACCENT */
    { 0x02C7, 0xFF }, /* CARON */
    { 0x02D8, 0xF9 }, /* BREVE */
    { 0x02D9, 0xFA }, /* DOT ABOVE */
    { 0x02DA, 0xFB }, /* RING ABOVE */
    { 0x02DB, 0xFE }, /* OGONEK */
    { 0x02DC, 0xF7 }, /* SMALL TILDE */
    { 0x02DD, 0xFD }, /* DOUBLE ACUTE ACCENT */
    { 0x03A9, 0xBD }, /* GREEK CAPITAL LETTER OMEGA */
    { 0x03C0, 0xB9 }, /* GREEK SMALL LETTER PI */
    { 0x2013, 0xD0 }, /* EN DASH */
    { 0x2014, 0xD1 }, /* EM DASH */
    { 0x2018, 0xD4 }, /* LEFT SINGLE QUOTATION MARK */
    { 0x2019, 0xD5 }, /* RIGHT SINGLE QUOTATION MARK */
    { 0x201A, 0xE2 }, /* SINGLE LOW-9 QUOTATION MARK */
    { 0x201C, 0xD2 }, /* LEFT DOUBLE QUOTATION MARK */
    { 0x201D, 0xD3 }, /* RIGHT DOUBLE QUOTATION MARK */
    { 0x201E, 0xE3 }, /* DOUBLE LOW-9 QUOTATION MARK */
    { 0x2020, 0xA0 }, /* DAGGER */
    { 0x2021, 0xE0 }, /* DOUBLE DAGGER */
    { 0x2022, 0xA5 }, /* BULLET */
    { 0x2026, 0xC9 }, /* HORIZONTAL ELLIPSIS */
    { 0x2030, 0xE4 }, /* PER MILLE SIGN */
    { 0x2039, 0xDC }, /* SINGLE LEFT-POINTING ANGLE QUOTATION MARK */
    { 0x203A, 0xDD }, /* SINGLE RIGHT-POINTING ANGLE QUOTATION MARK */
    { 0x2044, 0xDA }, /* FRACTION SLASH */
    { 0x20AC, 0xDB }, /* EURO SIGN */
    { 0x2122, 0xAA }, /* TRADE MARK SIGN */
    { 0x2202, 0xB6 }, /* PARTIAL DIFFERENTIAL */
    { 0x2206, 0xC6 }, /* INCREMENT */
    { 0x220F, 0xB8 }, /* N-ARY PRODUCT */
    { 0x2211, 0xB7 }, /* N-ARY SUMMATION */
    { 0x221A, 0xC3 }, /* SQUARE ROOT */
    { 0x221E, 0xB0 }, /* INFINITY */
    { 0x222B, 0xBA }, /* INTEGRAL */
    { 0x2248, 0xC5 }, /* ALMOST EQUAL TO */
    { 0x2260, 0xAD }, /* NOT EQUAL TO */
    { 0x2264, 0xB2 }, /* LESS-THAN OR EQUAL TO */
    { 0x2265, 0xB3 }, /* GREATER-THAN OR EQUAL TO */
    { 0x25CA, 0xD7 }, /* LOZENGE */
    { 0xF8FF, 0xF0 }, /* Apple logo */
    { 0xFB01, 0xDE }, /* LATIN SMALL LIGATURE FI */
    { 0xFB02, 0xDF }, /* LATIN SMALL LIGATURE FL */
};

static const FcCharMap AppleRoman = {
    AppleRomanEnt,
    sizeof (AppleRomanEnt) / sizeof (AppleRomanEnt[0])
};

/// AdobeSymboEnt

static const FcCharEnt AdobeSymbolEnt[] = {
    { 0x0020, 0x20 }, /* SPACE	# space */
    { 0x0021, 0x21 }, /* EXCLAMATION MARK	# exclam */
    { 0x0023, 0x23 }, /* NUMBER SIGN	# numbersign */
    { 0x0025, 0x25 }, /* PERCENT SIGN	# percent */
    { 0x0026, 0x26 }, /* AMPERSAND	# ampersand */
    { 0x0028, 0x28 }, /* LEFT PARENTHESIS	# parenleft */
    { 0x0029, 0x29 }, /* RIGHT PARENTHESIS	# parenright */
    { 0x002B, 0x2B }, /* PLUS SIGN	# plus */
    { 0x002C, 0x2C }, /* COMMA	# comma */
    { 0x002E, 0x2E }, /* FULL STOP	# period */
    { 0x002F, 0x2F }, /* SOLIDUS	# slash */
    { 0x0030, 0x30 }, /* DIGIT ZERO	# zero */
    { 0x0031, 0x31 }, /* DIGIT ONE	# one */
    { 0x0032, 0x32 }, /* DIGIT TWO	# two */
    { 0x0033, 0x33 }, /* DIGIT THREE	# three */
    { 0x0034, 0x34 }, /* DIGIT FOUR	# four */
    { 0x0035, 0x35 }, /* DIGIT FIVE	# five */
    { 0x0036, 0x36 }, /* DIGIT SIX	# six */
    { 0x0037, 0x37 }, /* DIGIT SEVEN	# seven */
    { 0x0038, 0x38 }, /* DIGIT EIGHT	# eight */
    { 0x0039, 0x39 }, /* DIGIT NINE	# nine */
    { 0x003A, 0x3A }, /* COLON	# colon */
    { 0x003B, 0x3B }, /* SEMICOLON	# semicolon */
    { 0x003C, 0x3C }, /* LESS-THAN SIGN	# less */
    { 0x003D, 0x3D }, /* EQUALS SIGN	# equal */
    { 0x003E, 0x3E }, /* GREATER-THAN SIGN	# greater */
    { 0x003F, 0x3F }, /* QUESTION MARK	# question */
    { 0x005B, 0x5B }, /* LEFT SQUARE BRACKET	# bracketleft */
    { 0x005D, 0x5D }, /* RIGHT SQUARE BRACKET	# bracketright */
    { 0x005F, 0x5F }, /* LOW LINE	# underscore */
    { 0x007B, 0x7B }, /* LEFT CURLY BRACKET	# braceleft */
    { 0x007C, 0x7C }, /* VERTICAL LINE	# bar */
    { 0x007D, 0x7D }, /* RIGHT CURLY BRACKET	# braceright */
    { 0x00A0, 0x20 }, /* NO-BREAK SPACE	# space */
    { 0x00AC, 0xD8 }, /* NOT SIGN	# logicalnot */
    { 0x00B0, 0xB0 }, /* DEGREE SIGN	# degree */
    { 0x00B1, 0xB1 }, /* PLUS-MINUS SIGN	# plusminus */
    { 0x00B5, 0x6D }, /* MICRO SIGN	# mu */
    { 0x00D7, 0xB4 }, /* MULTIPLICATION SIGN	# multiply */
    { 0x00F7, 0xB8 }, /* DIVISION SIGN	# divide */
    { 0x0192, 0xA6 }, /* LATIN SMALL LETTER F WITH HOOK	# florin */
    { 0x0391, 0x41 }, /* GREEK CAPITAL LETTER ALPHA	# Alpha */
    { 0x0392, 0x42 }, /* GREEK CAPITAL LETTER BETA	# Beta */
    { 0x0393, 0x47 }, /* GREEK CAPITAL LETTER GAMMA	# Gamma */
    { 0x0394, 0x44 }, /* GREEK CAPITAL LETTER DELTA	# Delta */
    { 0x0395, 0x45 }, /* GREEK CAPITAL LETTER EPSILON	# Epsilon */
    { 0x0396, 0x5A }, /* GREEK CAPITAL LETTER ZETA	# Zeta */
    { 0x0397, 0x48 }, /* GREEK CAPITAL LETTER ETA	# Eta */
    { 0x0398, 0x51 }, /* GREEK CAPITAL LETTER THETA	# Theta */
    { 0x0399, 0x49 }, /* GREEK CAPITAL LETTER IOTA	# Iota */
    { 0x039A, 0x4B }, /* GREEK CAPITAL LETTER KAPPA	# Kappa */
    { 0x039B, 0x4C }, /* GREEK CAPITAL LETTER LAMDA	# Lambda */
    { 0x039C, 0x4D }, /* GREEK CAPITAL LETTER MU	# Mu */
    { 0x039D, 0x4E }, /* GREEK CAPITAL LETTER NU	# Nu */
    { 0x039E, 0x58 }, /* GREEK CAPITAL LETTER XI	# Xi */
    { 0x039F, 0x4F }, /* GREEK CAPITAL LETTER OMICRON	# Omicron */
    { 0x03A0, 0x50 }, /* GREEK CAPITAL LETTER PI	# Pi */
    { 0x03A1, 0x52 }, /* GREEK CAPITAL LETTER RHO	# Rho */
    { 0x03A3, 0x53 }, /* GREEK CAPITAL LETTER SIGMA	# Sigma */
    { 0x03A4, 0x54 }, /* GREEK CAPITAL LETTER TAU	# Tau */
    { 0x03A5, 0x55 }, /* GREEK CAPITAL LETTER UPSILON	# Upsilon */
    { 0x03A6, 0x46 }, /* GREEK CAPITAL LETTER PHI	# Phi */
    { 0x03A7, 0x43 }, /* GREEK CAPITAL LETTER CHI	# Chi */
    { 0x03A8, 0x59 }, /* GREEK CAPITAL LETTER PSI	# Psi */
    { 0x03A9, 0x57 }, /* GREEK CAPITAL LETTER OMEGA	# Omega */
    { 0x03B1, 0x61 }, /* GREEK SMALL LETTER ALPHA	# alpha */
    { 0x03B2, 0x62 }, /* GREEK SMALL LETTER BETA	# beta */
    { 0x03B3, 0x67 }, /* GREEK SMALL LETTER GAMMA	# gamma */
    { 0x03B4, 0x64 }, /* GREEK SMALL LETTER DELTA	# delta */
    { 0x03B5, 0x65 }, /* GREEK SMALL LETTER EPSILON	# epsilon */
    { 0x03B6, 0x7A }, /* GREEK SMALL LETTER ZETA	# zeta */
    { 0x03B7, 0x68 }, /* GREEK SMALL LETTER ETA	# eta */
    { 0x03B8, 0x71 }, /* GREEK SMALL LETTER THETA	# theta */
    { 0x03B9, 0x69 }, /* GREEK SMALL LETTER IOTA	# iota */
    { 0x03BA, 0x6B }, /* GREEK SMALL LETTER KAPPA	# kappa */
    { 0x03BB, 0x6C }, /* GREEK SMALL LETTER LAMDA	# lambda */
    { 0x03BC, 0x6D }, /* GREEK SMALL LETTER MU	# mu */
    { 0x03BD, 0x6E }, /* GREEK SMALL LETTER NU	# nu */
    { 0x03BE, 0x78 }, /* GREEK SMALL LETTER XI	# xi */
    { 0x03BF, 0x6F }, /* GREEK SMALL LETTER OMICRON	# omicron */
    { 0x03C0, 0x70 }, /* GREEK SMALL LETTER PI	# pi */
    { 0x03C1, 0x72 }, /* GREEK SMALL LETTER RHO	# rho */
    { 0x03C2, 0x56 }, /* GREEK SMALL LETTER FINAL SIGMA	# sigma1 */
    { 0x03C3, 0x73 }, /* GREEK SMALL LETTER SIGMA	# sigma */
    { 0x03C4, 0x74 }, /* GREEK SMALL LETTER TAU	# tau */
    { 0x03C5, 0x75 }, /* GREEK SMALL LETTER UPSILON	# upsilon */
    { 0x03C6, 0x66 }, /* GREEK SMALL LETTER PHI	# phi */
    { 0x03C7, 0x63 }, /* GREEK SMALL LETTER CHI	# chi */
    { 0x03C8, 0x79 }, /* GREEK SMALL LETTER PSI	# psi */
    { 0x03C9, 0x77 }, /* GREEK SMALL LETTER OMEGA	# omega */
    { 0x03D1, 0x4A }, /* GREEK THETA SYMBOL	# theta1 */
    { 0x03D2, 0xA1 }, /* GREEK UPSILON WITH HOOK SYMBOL	# Upsilon1 */
    { 0x03D5, 0x6A }, /* GREEK PHI SYMBOL	# phi1 */
    { 0x03D6, 0x76 }, /* GREEK PI SYMBOL	# omega1 */
    { 0x2022, 0xB7 }, /* BULLET	# bullet */
    { 0x2026, 0xBC }, /* HORIZONTAL ELLIPSIS	# ellipsis */
    { 0x2032, 0xA2 }, /* PRIME	# minute */
    { 0x2033, 0xB2 }, /* DOUBLE PRIME	# second */
    { 0x2044, 0xA4 }, /* FRACTION SLASH	# fraction */
    { 0x20AC, 0xA0 }, /* EURO SIGN	# Euro */
    { 0x2111, 0xC1 }, /* BLACK-LETTER CAPITAL I	# Ifraktur */
    { 0x2118, 0xC3 }, /* SCRIPT CAPITAL P	# weierstrass */
    { 0x211C, 0xC2 }, /* BLACK-LETTER CAPITAL R	# Rfraktur */
    { 0x2126, 0x57 }, /* OHM SIGN	# Omega */
    { 0x2135, 0xC0 }, /* ALEF SYMBOL	# aleph */
    { 0x2190, 0xAC }, /* LEFTWARDS ARROW	# arrowleft */
    { 0x2191, 0xAD }, /* UPWARDS ARROW	# arrowup */
    { 0x2192, 0xAE }, /* RIGHTWARDS ARROW	# arrowright */
    { 0x2193, 0xAF }, /* DOWNWARDS ARROW	# arrowdown */
    { 0x2194, 0xAB }, /* LEFT RIGHT ARROW	# arrowboth */
    { 0x21B5, 0xBF }, /* DOWNWARDS ARROW WITH CORNER LEFTWARDS	# carriagereturn */
    { 0x21D0, 0xDC }, /* LEFTWARDS DOUBLE ARROW	# arrowdblleft */
    { 0x21D1, 0xDD }, /* UPWARDS DOUBLE ARROW	# arrowdblup */
    { 0x21D2, 0xDE }, /* RIGHTWARDS DOUBLE ARROW	# arrowdblright */
    { 0x21D3, 0xDF }, /* DOWNWARDS DOUBLE ARROW	# arrowdbldown */
    { 0x21D4, 0xDB }, /* LEFT RIGHT DOUBLE ARROW	# arrowdblboth */
    { 0x2200, 0x22 }, /* FOR ALL	# universal */
    { 0x2202, 0xB6 }, /* PARTIAL DIFFERENTIAL	# partialdiff */
    { 0x2203, 0x24 }, /* THERE EXISTS	# existential */
    { 0x2205, 0xC6 }, /* EMPTY SET	# emptyset */
    { 0x2206, 0x44 }, /* INCREMENT	# Delta */
    { 0x2207, 0xD1 }, /* NABLA	# gradient */
    { 0x2208, 0xCE }, /* ELEMENT OF	# element */
    { 0x2209, 0xCF }, /* NOT AN ELEMENT OF	# notelement */
    { 0x220B, 0x27 }, /* CONTAINS AS MEMBER	# suchthat */
    { 0x220F, 0xD5 }, /* N-ARY PRODUCT	# product */
    { 0x2211, 0xE5 }, /* N-ARY SUMMATION	# summation */
    { 0x2212, 0x2D }, /* MINUS SIGN	# minus */
    { 0x2215, 0xA4 }, /* DIVISION SLASH	# fraction */
    { 0x2217, 0x2A }, /* ASTERISK OPERATOR	# asteriskmath */
    { 0x221A, 0xD6 }, /* SQUARE ROOT	# radical */
    { 0x221D, 0xB5 }, /* PROPORTIONAL TO	# proportional */
    { 0x221E, 0xA5 }, /* INFINITY	# infinity */
    { 0x2220, 0xD0 }, /* ANGLE	# angle */
    { 0x2227, 0xD9 }, /* LOGICAL AND	# logicaland */
    { 0x2228, 0xDA }, /* LOGICAL OR	# logicalor */
    { 0x2229, 0xC7 }, /* INTERSECTION	# intersection */
    { 0x222A, 0xC8 }, /* UNION	# union */
    { 0x222B, 0xF2 }, /* INTEGRAL	# integral */
    { 0x2234, 0x5C }, /* THEREFORE	# therefore */
    { 0x223C, 0x7E }, /* TILDE OPERATOR	# similar */
    { 0x2245, 0x40 }, /* APPROXIMATELY EQUAL TO	# congruent */
    { 0x2248, 0xBB }, /* ALMOST EQUAL TO	# approxequal */
    { 0x2260, 0xB9 }, /* NOT EQUAL TO	# notequal */
    { 0x2261, 0xBA }, /* IDENTICAL TO	# equivalence */
    { 0x2264, 0xA3 }, /* LESS-THAN OR EQUAL TO	# lessequal */
    { 0x2265, 0xB3 }, /* GREATER-THAN OR EQUAL TO	# greaterequal */
    { 0x2282, 0xCC }, /* SUBSET OF	# propersubset */
    { 0x2283, 0xC9 }, /* SUPERSET OF	# propersuperset */
    { 0x2284, 0xCB }, /* NOT A SUBSET OF	# notsubset */
    { 0x2286, 0xCD }, /* SUBSET OF OR EQUAL TO	# reflexsubset */
    { 0x2287, 0xCA }, /* SUPERSET OF OR EQUAL TO	# reflexsuperset */
    { 0x2295, 0xC5 }, /* CIRCLED PLUS	# circleplus */
    { 0x2297, 0xC4 }, /* CIRCLED TIMES	# circlemultiply */
    { 0x22A5, 0x5E }, /* UP TACK	# perpendicular */
    { 0x22C5, 0xD7 }, /* DOT OPERATOR	# dotmath */
    { 0x2320, 0xF3 }, /* TOP HALF INTEGRAL	# integraltp */
    { 0x2321, 0xF5 }, /* BOTTOM HALF INTEGRAL	# integralbt */
    { 0x2329, 0xE1 }, /* LEFT-POINTING ANGLE BRACKET	# angleleft */
    { 0x232A, 0xF1 }, /* RIGHT-POINTING ANGLE BRACKET	# angleright */
    { 0x25CA, 0xE0 }, /* LOZENGE	# lozenge */
    { 0x2660, 0xAA }, /* BLACK SPADE SUIT	# spade */
    { 0x2663, 0xA7 }, /* BLACK CLUB SUIT	# club */
    { 0x2665, 0xA9 }, /* BLACK HEART SUIT	# heart */
    { 0x2666, 0xA8 }, /* BLACK DIAMOND SUIT	# diamond */
    { 0xF6D9, 0xD3 }, /* COPYRIGHT SIGN SERIF	# copyrightserif (CUS) */
    { 0xF6DA, 0xD2 }, /* REGISTERED SIGN SERIF	# registerserif (CUS) */
    { 0xF6DB, 0xD4 }, /* TRADE MARK SIGN SERIF	# trademarkserif (CUS) */
    { 0xF8E5, 0x60 }, /* RADICAL EXTENDER	# radicalex (CUS) */
    { 0xF8E6, 0xBD }, /* VERTICAL ARROW EXTENDER	# arrowvertex (CUS) */
    { 0xF8E7, 0xBE }, /* HORIZONTAL ARROW EXTENDER	# arrowhorizex (CUS) */
    { 0xF8E8, 0xE2 }, /* REGISTERED SIGN SANS SERIF	# registersans (CUS) */
    { 0xF8E9, 0xE3 }, /* COPYRIGHT SIGN SANS SERIF	# copyrightsans (CUS) */
    { 0xF8EA, 0xE4 }, /* TRADE MARK SIGN SANS SERIF	# trademarksans (CUS) */
    { 0xF8EB, 0xE6 }, /* LEFT PAREN TOP	# parenlefttp (CUS) */
    { 0xF8EC, 0xE7 }, /* LEFT PAREN EXTENDER	# parenleftex (CUS) */
    { 0xF8ED, 0xE8 }, /* LEFT PAREN BOTTOM	# parenleftbt (CUS) */
    { 0xF8EE, 0xE9 }, /* LEFT SQUARE BRACKET TOP	# bracketlefttp (CUS) */
    { 0xF8EF, 0xEA }, /* LEFT SQUARE BRACKET EXTENDER	# bracketleftex (CUS) */
    { 0xF8F0, 0xEB }, /* LEFT SQUARE BRACKET BOTTOM	# bracketleftbt (CUS) */
    { 0xF8F1, 0xEC }, /* LEFT CURLY BRACKET TOP	# bracelefttp (CUS) */
    { 0xF8F2, 0xED }, /* LEFT CURLY BRACKET MID	# braceleftmid (CUS) */
    { 0xF8F3, 0xEE }, /* LEFT CURLY BRACKET BOTTOM	# braceleftbt (CUS) */
    { 0xF8F4, 0xEF }, /* CURLY BRACKET EXTENDER	# braceex (CUS) */
    { 0xF8F5, 0xF4 }, /* INTEGRAL EXTENDER	# integralex (CUS) */
    { 0xF8F6, 0xF6 }, /* RIGHT PAREN TOP	# parenrighttp (CUS) */
    { 0xF8F7, 0xF7 }, /* RIGHT PAREN EXTENDER	# parenrightex (CUS) */
    { 0xF8F8, 0xF8 }, /* RIGHT PAREN BOTTOM	# parenrightbt (CUS) */
    { 0xF8F9, 0xF9 }, /* RIGHT SQUARE BRACKET TOP	# bracketrighttp (CUS) */
    { 0xF8FA, 0xFA }, /* RIGHT SQUARE BRACKET EXTENDER	# bracketrightex (CUS) */
    { 0xF8FB, 0xFB }, /* RIGHT SQUARE BRACKET BOTTOM	# bracketrightbt (CUS) */
    { 0xF8FC, 0xFC }, /* RIGHT CURLY BRACKET TOP	# bracerighttp (CUS) */
    { 0xF8FD, 0xFD }, /* RIGHT CURLY BRACKET MID	# bracerightmid (CUS) */
    { 0xF8FE, 0xFE }, /* RIGHT CURLY BRACKET BOTTOM	# bracerightbt (CUS) */
};

////

static const FcCharMap AdobeSymbol = {
    AdobeSymbolEnt,
    sizeof (AdobeSymbolEnt) / sizeof (AdobeSymbolEnt[0]),
};
    

static const FcFontDecode fcFontDecoders[] = {
    { ft_encoding_unicode,	0,		(1 << 21) - 1 },
    { ft_encoding_symbol,	&AdobeSymbol,	(1 << 16) - 1 },
    { ft_encoding_apple_roman,	&AppleRoman,	(1 << 16) - 1 },
};

#define NUM_DECODE  (int) (sizeof (fcFontDecoders) / sizeof (fcFontDecoders[0]))

static FcChar32 FcFreeTypePrivateToUcs4 (FcChar32 private, const FcCharMap *map);

const FcCharMap *FcFreeTypeGetPrivateMap(FT_Encoding encoding)
{
    int	i;

    for (i = 0; i < NUM_DECODE; i++)
	if (fcFontDecoders[i].encoding == encoding)
	    return fcFontDecoders[i].map;
    return 0;
}

/* A shift-JIS will have many high bits turned on */
static FcBool FcLooksLikeSJIS (FcChar8 *string, int len)
{
	int nhigh = 0, nlow = 0;

    while (len-- > 0)
    {
		if (*string++ & 0x80)
			nhigh++;
		else
			nlow++;
    }
	/* Heuristic -- if more than 1/3 of the bytes have the high-bit set, this is likely to be SJIS and not ROMAN */
    if (nhigh * 2 > nlow)
		return FcTrue;

    return FcFalse;
}

static char *fcSfntNameTranscode(FT_SfntName *sname)
{
    int	i;
    const char *fromcode;
    char *utf8;

    for (i = 0; i < NUM_FC_FT_ENCODING; i++)
		if (fcFtEncoding[i].platform_id == sname->platform_id &&
			(fcFtEncoding[i].encoding_id == TT_ENCODING_DONT_CARE ||
			fcFtEncoding[i].encoding_id == sname->encoding_id))
			break;
			
    if (i == NUM_FC_FT_ENCODING)
		return 0;
    fromcode = fcFtEncoding[i].fromcode;

    /* Many names encoded for TT_PLATFORM_MACINTOSH are broken in various ways. Kludge around them. */
     
    if (!strcmp (fromcode, FC_ENCODING_MAC_ROMAN))
    {
		if (sname->language_id == TT_MAC_LANGID_ENGLISH &&
			FcLooksLikeSJIS (sname->string, sname->string_len))
		{
			fromcode = "SJIS";
		}
		else if (sname->language_id >= 0x100)
		{
			/* "real" Mac language IDs are all less than 150.
			 * Names using one of the MS language IDs are assumed
			 * to use an associated encoding (Yes, this is a kludge)
			 */
			int	f;

			fromcode = NULL;
			for (f = 0; f < NUM_FC_MAC_ROMAN_FAKE; f++)
			if (fcMacRomanFake[f].language_id == sname->language_id)
			{
				fromcode = fcMacRomanFake[f].fromcode;
				break;
			}
			if (!fromcode)
				return 0;
		}
    }
    if (!strcmp (fromcode, "UCS-2BE") || !strcmp (fromcode, "UTF-16BE"))
    {
		char *src = sname->string;
		int	src_len = sname->string_len;
		int	len;
		int	wchar;
		int	ilen, olen;
		char *u8;
		unsigned int ucs4;
	
		/* Convert Utf16 to Utf8 */

		if (!FcUtf16Len(src, FcEndianBig, src_len, &len, &wchar))
			return 0;

		/* Allocate plenty of space.  Freed below */
		utf8 = malloc (len * FC_UTF8_MAX_LEN + 1);
		if (!utf8)
			return 0;

		u8 = utf8;

		while ((ilen = FcUtf16ToUcs4 (src, FcEndianBig, &ucs4, src_len)) > 0)
		{
			src_len -= ilen;
			src += ilen;
			olen = FcUcs4ToUtf8 (ucs4, u8);
			u8 += olen;
		}

		*u8 = '\0';
		goto done;
    }
    if (!strcmp (fromcode, "ASCII") || !strcmp (fromcode, "ISO-8859-1"))
    {
		FcChar8	    *src = sname->string;
		int	    src_len = sname->string_len;
		int	    olen;
		FcChar8	    *u8;
		FcChar32    ucs4;
	
		/* Convert Latin1 to Utf8. Freed below */
		utf8 = malloc (src_len * 2 + 1);
		if (!utf8)
			return 0;

		u8 = utf8;
		while (src_len > 0)
		{
			ucs4 = *src++;
			src_len--;
			olen = FcUcs4ToUtf8 (ucs4, u8);
			u8 += olen;
		}
		*u8 = '\0';
		goto done;
	}
    if (!strcmp (fromcode, FC_ENCODING_MAC_ROMAN))
    {
		FcChar8 *u8;
		const FcCharMap	*map = FcFreeTypeGetPrivateMap (ft_encoding_apple_roman);
		FcChar8 *src = (FcChar8 *) sname->string;
		int src_len = sname->string_len;
	
		/* Convert AppleRoman to Utf8 */
		if (!map)
			return 0;

		utf8 = malloc (sname->string_len * 3 + 1);
		if (!utf8)
			return 0;

		u8 = utf8;
		while (src_len > 0)
		{
			FcChar32 ucs4 = FcFreeTypePrivateToUcs4 (*src++, map);
			int olen = FcUcs4ToUtf8 (ucs4, u8);
			src_len--;
			u8 += olen;
		}
		*u8 = '\0';
		goto done;
    }

    return 0;
done:
    if (FcStrCmpIgnoreBlanksAndCase (utf8, (FcChar8 *) "") == 0)
    {
		free (utf8);
		return 0;
    }
    return utf8;
}

static const char *fcSfntNameLanguage(FT_SfntName *sname)
{
    int i;
    FT_UShort platform_id = sname->platform_id;
    FT_UShort language_id = sname->language_id;

    /* Many names encoded for TT_PLATFORM_MACINTOSH are broken in various ways. Kludge around them. */
    if (platform_id == TT_PLATFORM_MACINTOSH &&	sname->encoding_id == TT_MAC_ID_ROMAN && FcLooksLikeSJIS (sname->string, sname->string_len))
    {
		language_id = TT_MAC_LANGID_JAPANESE;
    }
    
    for (i = 0; i < NUM_FC_FT_LANGUAGE; i++)
	if (fcFtLanguage[i].platform_id == platform_id && (fcFtLanguage[i].language_id == TT_LANGUAGE_DONT_CARE || fcFtLanguage[i].language_id == language_id))
	{
	    if (fcFtLanguage[i].lang[0] == '\0')
	      return NULL;
	    else
	      return (char *)fcFtLanguage[i].lang;
	}
    return 0;
}

static int fcStringInPatternElement(struct fontpattern *pat, const int elt, char *string)
{
    int	e;
    for (e = 0;; e++)
    {
		char *old = fcPatternGetString(pat, elt, e);
		if (old != NULL && !FcStrCmpIgnoreBlanksAndCase(old, string))
			return FcTrue;
			
		if (old != NULL)
			e++; /* try next element, otherwise bail out */
		else
			break;
	}
		
    return FcFalse;
}



struct fontpattern *fcQueryFace(const FT_Face face, char *fname, int id)
{
	struct fontpattern *pat = NULL;
	int slant = -1;
	int weight = -1;
	int width = -1;
	int serif = -1;
	int	i;
	struct fontlangset *ls;
	struct fontcharset *cs;
    int		    spacing;
    TT_OS2	    *os2;
#if HAVE_FT_GET_PS_FONT_INFO
    PS_FontInfoRec  psfontinfo;
#endif
#if HAVE_FT_GET_BDF_PROPERTY
    BDF_PropertyRec prop;
#endif
	TT_Header	*head;
	char		*exclusiveLang = 0;
	FT_SfntName	sname;
	unsigned int snamei, snamec;

    int		    nfamily = 0;
    int		    nstyle = 0;
    int		    nfullname = 0;
    int		    p, platform;
    int		    n, nameid;

	char		*style = 0;
    int		    st;

	pat = fcPatternAlloc();
	if (pat == NULL)
		return NULL;

	/* Get the OS/2 table */
	os2 = (TT_OS2 *)FT_Get_Sfnt_Table(face, ft_sfnt_os2);

	/* Grub through the name table looking for family and style names. FreeType makes quite a hash of them */
	snamec = FT_Get_Sfnt_Name_Count(face);
    for (p = 0; p <= NUM_PLATFORM_ORDER; p++)
    {
		if (p < NUM_PLATFORM_ORDER)
		    platform = platform_order[p];
		else
		    platform = 0xffff;

		/* Order nameids so preferred names appear first in the resulting list */
		for (n = 0; n < NUM_NAMEID_ORDER; n++)
		{
		    nameid = nameid_order[n];

		    for (snamei = 0; snamei < snamec; snamei++)
		    {
				char *utf8;
				int elt = 0;

				if (FT_Get_Sfnt_Name (face, snamei, &sname) != 0)
					continue;
				if (sname.name_id != nameid)
					continue;

				/* Sort platforms in preference order, accepting all other platforms last */
				if (p < NUM_PLATFORM_ORDER)
				{
					if (sname.platform_id != platform)
					continue;
				}
				else
				{
					int sp;
	
					for (sp = 0; sp < NUM_PLATFORM_ORDER; sp++)
						if (sname.platform_id == platform_order[sp])
							break;
							
					if (sp != NUM_PLATFORM_ORDER)
						continue;
				}
				utf8 = fcSfntNameTranscode(&sname);

				if (utf8 == NULL)
					continue;

				switch (sname.name_id) {
	#ifdef TT_NAME_ID_WWS_FAMILY
				case TT_NAME_ID_WWS_FAMILY:
	#endif
				case TT_NAME_ID_PREFERRED_FAMILY:
				case TT_NAME_ID_FONT_FAMILY:
	#if 0
				case TT_NAME_ID_PS_NAME:
				case TT_NAME_ID_UNIQUE_ID:
	#endif

					if (FcDebug () & FC_DBG_SCANV)
						kprintf ("found family (n %2d p %d e %d l 0x%04x) %s\n",
							sname.name_id, sname.platform_id,
							sname.encoding_id, sname.language_id,
							utf8);

					elt = FC_FAMILY;
					break;
				case TT_NAME_ID_MAC_FULL_NAME:
				case TT_NAME_ID_FULL_NAME:

					if (FcDebug () & FC_DBG_SCANV)
						kprintf ("found full   (n %2d p %d e %d l 0x%04x) %s\n",
						sname.name_id, sname.platform_id,
						sname.encoding_id, sname.language_id,
						utf8);


					elt = FC_FULLNAME;
					break;
	#ifdef TT_NAME_ID_WWS_SUBFAMILY
				case TT_NAME_ID_WWS_SUBFAMILY:
	#endif
				case TT_NAME_ID_PREFERRED_SUBFAMILY:
				case TT_NAME_ID_FONT_SUBFAMILY:

					if (FcDebug () & FC_DBG_SCANV)
						kprintf ("found style  (n %2d p %d e %d l 0x%04x) %s\n",
						sname.name_id, sname.platform_id,
						sname.encoding_id, sname.language_id,
						utf8);


					elt = FC_STYLE;
					break;
				}
				
				if (elt)
				{
					if (fcStringInPatternElement(pat, elt, utf8))
					{
						free(utf8);
						continue;
					}
	
					/* add new element */
					if (!fcPatternAddString (pat, elt, utf8))
					{
						free(utf8);
						goto bail1;
					}
					free(utf8);
				}
				else
					free (utf8);
					
			}
		}
    }

    if (!nfamily && face->family_name && FcStrCmpIgnoreBlanksAndCase ((FcChar8 *) face->family_name, (FcChar8 *) "") != 0)
    {

		if (FcDebug () & FC_DBG_SCANV)
			kprintf ("using FreeType family \"%s\"\n", face->family_name);

		if (!fcPatternAddString (pat, FC_FAMILY, (FcChar8 *) face->family_name))
			goto bail1;
		++nfamily;
    }

    if (!nstyle && face->style_name && FcStrCmpIgnoreBlanksAndCase ((FcChar8 *) face->style_name, (FcChar8 *) "") != 0)
    {
		if (FcDebug () & FC_DBG_SCANV)
			kprintf ("using FreeType style \"%s\"\n", face->style_name);

		if (!fcPatternAddString (pat, FC_STYLE, (FcChar8 *) face->style_name))
			goto bail1;
		++nstyle;
    }

    if (!nfamily)
    {
		FcChar8	*start, *end;
		FcChar8	*family;

		start = (FcChar8 *) strrchr ((char *) fname, '/');
		if (start)
			start++;
		else
			start = (FcChar8 *) fname;
		end = (FcChar8 *) strrchr ((char *) start, '.');
		if (!end)
			end = start + strlen ((char *) start);
		/* freed below */
		family = malloc (end - start + 1);
		strncpy ((char *) family, (char *) start, end - start);
		family[end - start] = '\0';

		if (FcDebug () & FC_DBG_SCANV)
			kprintf ("using filename for family %s\n", family);

		if (!fcPatternAddString (pat, FC_FAMILY, family))
		{
			free (family);
			goto bail1;
		}
		free (family);
		++nfamily;
    }

	if (!fcPatternAddString (pat, FC_FILE, fname))
		goto bail1;

    if (!fcPatternAddInteger (pat, FC_INDEX, id))
		goto bail1;

    if (os2 && os2->version >= 0x0001 && os2->version != 0xffff)
    {
		for (i = 0; i < NUM_CODE_PAGE_RANGE; i++)
		{
			FT_ULong	bits;
			int		bit;
			if (FcCodePageRange[i].bit < 32)
			{
				bits = os2->ulCodePageRange1;
				bit = FcCodePageRange[i].bit;
			}
			else
			{
				bits = os2->ulCodePageRange2;
				bit = FcCodePageRange[i].bit - 32;
			}
			if (bits & (1 << bit))
			{
				/* If the font advertises support for multiple
				 * "exclusive" languages, then include support
				 * for any language found to have coverage
				 */

				if (exclusiveLang)
				{
					exclusiveLang = 0;
					break;
				}
				exclusiveLang = FcCodePageRange[i].lang;
			}
		}
    }

	if (os2 && os2 && (unsigned)(((os2->sFamilyClass >> 8) & 0xff) - 1) < 5)	/* kiero: from ftmanager source. too lazy to google for it... */
	{
		fcPatternAddInteger(pat, FC_SERIF, TRUE);
	}
		

    if (os2 && os2->version != 0xffff)
    {
		if (os2->usWeightClass == 0)
			;
		else if (os2->usWeightClass < 150)
			weight = FC_WEIGHT_THIN;
		else if (os2->usWeightClass < 250)
			weight = FC_WEIGHT_EXTRALIGHT;
		else if (os2->usWeightClass < 350)
			weight = FC_WEIGHT_LIGHT;
		else if (os2->usWeightClass < 450)
			weight = FC_WEIGHT_REGULAR;
		else if (os2->usWeightClass < 550)
			weight = FC_WEIGHT_MEDIUM;
		else if (os2->usWeightClass < 650)
			weight = FC_WEIGHT_SEMIBOLD;
		else if (os2->usWeightClass < 750)
			weight = FC_WEIGHT_BOLD;
		else if (os2->usWeightClass < 850)
			weight = FC_WEIGHT_EXTRABOLD;
		else if (os2->usWeightClass < 925)
			weight = FC_WEIGHT_BLACK;
		else if (os2->usWeightClass < 1000)
			weight = FC_WEIGHT_EXTRABLACK;

		if ((FcDebug() & FC_DBG_SCANV) && weight != -1)
			kprintf ("\tos2 weight class %d maps to weight %d\n",
				os2->usWeightClass, weight);


		switch (os2->usWidthClass) {
		case 1:	width = FC_WIDTH_ULTRACONDENSED; break;
		case 2:	width = FC_WIDTH_EXTRACONDENSED; break;
		case 3:	width = FC_WIDTH_CONDENSED; break;
		case 4:	width = FC_WIDTH_SEMICONDENSED; break;
		case 5:	width = FC_WIDTH_NORMAL; break;
		case 6:	width = FC_WIDTH_SEMIEXPANDED; break;
		case 7:	width = FC_WIDTH_EXPANDED; break;
		case 8:	width = FC_WIDTH_EXTRAEXPANDED; break;
		case 9:	width = FC_WIDTH_ULTRAEXPANDED; break;
		}

		if ((FcDebug() & FC_DBG_SCANV) && width != -1)
			kprintf ("\tos2 width class %d maps to width %d\n",
				os2->usWidthClass, width);

		}

		/* Type 1: Check for FontInfo dictionary information Code from g2@magestudios.net (Gerard Escalante) */

#if HAVE_FT_GET_PS_FONT_INFO
		if (FT_Get_PS_Font_Info(face, &psfontinfo) == 0)
		{
			if (weight == -1 && psfontinfo.weight)
			{
				weight = FcIsWeight ((FcChar8 *) psfontinfo.weight);
				if (FcDebug() & FC_DBG_SCANV)
					kprintf ("\tType1 weight %s maps to %d\n",
					psfontinfo.weight, weight);

			}


		}
#endif /* HAVE_FT_GET_PS_FONT_INFO */

#if HAVE_FT_GET_BDF_PROPERTY

		if (width == -1)
		{
			if (FT_Get_BDF_Property(face, "RELATIVE_SETWIDTH", &prop) == 0 &&
				(prop.type == BDF_PROPERTY_TYPE_INTEGER ||
				 prop.type == BDF_PROPERTY_TYPE_CARDINAL))
			{
				FT_Int32	value;

				if (prop.type == BDF_PROPERTY_TYPE_INTEGER)
				value = prop.u.integer;
				else
				value = (FT_Int32) prop.u.cardinal;
				switch ((value + 5) / 10) {
				case 1: width = FC_WIDTH_ULTRACONDENSED; break;
				case 2: width = FC_WIDTH_EXTRACONDENSED; break;
				case 3: width = FC_WIDTH_CONDENSED; break;
				case 4: width = FC_WIDTH_SEMICONDENSED; break;
				case 5: width = FC_WIDTH_NORMAL; break;
				case 6: width = FC_WIDTH_SEMIEXPANDED; break;
				case 7: width = FC_WIDTH_EXPANDED; break;
				case 8: width = FC_WIDTH_EXTRAEXPANDED; break;
				case 9: width = FC_WIDTH_ULTRAEXPANDED; break;
				}
			}
			if (width == -1 && FT_Get_BDF_Property (face, "SETWIDTH_NAME", &prop) == 0 && prop.type == BDF_PROPERTY_TYPE_ATOM)
			{
				width = FcIsWidth ((FcChar8 *) prop.u.atom);
				if (FcDebug () & FC_DBG_SCANV)
					kprintf ("\tsetwidth %s maps to %d\n", prop.u.atom, width);
			}
		}
#endif

    /* Look for weight, width and slant names in the style value */
    for (st = 0;; st++)
    {
		style = fcPatternGetString (pat, FC_STYLE, st);
		if (style == NULL)
			break;
    
		if (weight == -1)
		{
			weight = FcContainsWeight (style);

			if (FcDebug() & FC_DBG_SCANV)
				kprintf ("\tStyle %s maps to weight %d\n", style, weight);

		}
		if (width == -1)
		{
			width = FcContainsWidth (style);
			if (FcDebug() & FC_DBG_SCANV)
				kprintf ("\tStyle %s maps to width %d\n", style, width);

		}
		if (slant == -1)
		{
			slant = FcContainsSlant (style);

			if (FcDebug() & FC_DBG_SCANV)
				kprintf ("\tStyle %s maps to slant %d\n", style, slant);

		}
    }
    /* Pull default values from the FreeType flags if more specific values not found above */
    if (slant == -1)
    {
		slant = FC_SLANT_ROMAN;
		if (face->style_flags & FT_STYLE_FLAG_ITALIC)
			slant = FC_SLANT_ITALIC;
    }

    if (weight == -1)
    {
		weight = FC_WEIGHT_MEDIUM;
		if (face->style_flags & FT_STYLE_FLAG_BOLD)
			weight = FC_WEIGHT_BOLD;
    }

    if (width == -1)
	width = FC_WIDTH_NORMAL;

  
    if (!fcPatternAddInteger (pat, FC_SLANT, slant))
		goto bail1;

    if (!fcPatternAddInteger (pat, FC_WEIGHT, weight))
		goto bail1;

    if (!fcPatternAddInteger (pat, FC_WIDTH, width))
		goto bail1;

	/* Compute the unicode coverage for the font */
#if 0
	cs = FcFreeTypeCharSetAndSpacing (face, blanks, &spacing);
	if (!cs)
		goto bail1;


#if HAVE_FT_GET_BDF_PROPERTY
    /* For PCF fonts, override the computed spacing with the one from the property */
    if(FT_Get_BDF_Property(face, "SPACING", &prop) == 0 && prop.type == BDF_PROPERTY_TYPE_ATOM)
    {
		if(!strcmp(prop.u.atom, "c") || !strcmp(prop.u.atom, "C"))
            spacing = FC_CHARCELL;
        else if(!strcmp(prop.u.atom, "m") || !strcmp(prop.u.atom, "M"))
            spacing = FC_MONO;
        else if(!strcmp(prop.u.atom, "p") || !strcmp(prop.u.atom, "P"))
            spacing = FC_PROPORTIONAL;
    }
#endif

    if (spacing != FC_PROPORTIONAL)
		if (!fcPatternAddInteger (pat, FC_SPACING, spacing))
			goto bail2;

#endif

    /* Skip over PCF fonts that have no encoded characters; they're
     * usually just Unicode fonts transcoded to some legacy encoding
     * FT forces us to approximate whether a font is a PCF font
     * or not by whether it has any BDF properties.  Try PIXEL_SIZE;
     * I don't know how to get a list of BDF properties on the font. -PL
     */
#if 0
    if (FcCharSetCount (cs) == 0)
    {
#if HAVE_FT_GET_BDF_PROPERTY
		if(FT_Get_BDF_Property(face, "PIXEL_SIZE", &prop) == 0)
			goto bail2;
#endif
    }

    ls = FcFreeTypeLangSet (cs, exclusiveLang);
    if (!ls)
		goto bail2;
	
    if (!fcPatternAddLangSet (pat, FC_LANG, ls))
    {
		FcLangSetDestroy (ls);
		goto bail2;
    }

    FcLangSetDestroy (ls);


    /* Drop our reference to the charset */
    FcCharSetDestroy (cs);
#endif

    return pat;

bail2:

#if 0
    FcCharSetDestroy (cs);
#endif
bail1:

    fcPatternDestroy (pat);
bail0:

    return NULL;
}

struct fontcache 
{
	int valid;
	struct List entries;
};



static struct fontpattern *fcQueryFont(char *fname, int id, int *count)
{
	FT_Face face;
	struct fontpattern *pat = NULL;
	
	if (FT_New_Face(ftLibrary, fname, id, &face))
	{
		D(kprintf("failed to open tt font:%s, %d\n", fname, id));
		goto bail;
	}
    *count = face->num_faces;

	pat = fcQueryFace(face, fname, id);

	FT_Done_Face(face);
bail:
    
	return pat;
}


#endif


static DirectoryTraverseResult callback(struct FileInfoBlock *fib, char *path, int level, void *data)
{
	struct fontcache *cache = (struct fontcache*)data;
	int ccount;

	if (strMatch(path, "#?.(ttf|pfb)"))
	{
		struct fontpattern *pat;
		if (FcDebug () & FC_DBG_SCANV)
			kprintf("scanning:%s\n", path);
		pat = fcQueryFont(path, 0, &ccount);
		if (FcDebug () & FC_DBG_SCANV)
			kprintf("scanning:%s done. pattern:%p\n", path, pat);
		
		if (pat != NULL)
			ADDTAIL(&cache->entries, pat);
		//else (don't abort scanning. ikn future add different error codes to distinguish between lack of memory and broken font files)
		//	return DIRECTORY_TRAVERSE_ABORT;
	}

	return DIRECTORY_TRAVERSE_CONTINUE;
}

void fcDestroy(struct fontcache *cache)
{
	/* TODO. not really high priority as it will be dispose anyway */
}

struct fontcache *fcCreate(char *directories[])
{
	struct fontcache *cache = calloc(1, sizeof(*cache));
	NewList(&cache->entries);
	
	if (FcDebug () & FC_DBG_SCANV)
		kprintf("FCCache: Initialize cache system...\n");
	
	if (!FT_Init_FreeType(&ftLibrary))
	{
		while(*directories)
		{
			char *dir = *directories++;
			BPTR dirlock = Lock(dir, ACCESS_READ);

			if (dirlock != NULL)
			{
				int result;

				UnLock(dirlock);
				result = directoryTraverse(dir, DIRECTORY_SCAN_RECURSIVE, callback, cache);

				if (result != DIRECTORY_TRAVERSE_OK)
				{
					fcDestroy(cache);
					cache = NULL;
					break;
				}
			}
		}
		
		FT_Done_FreeType(ftLibrary);
	}
	else
	{
		D(kprintf("**********ERROR: Failed to initiclize freetype library\n"));
	}
	return cache;
}

struct fontpattern *fcMatch(struct fontcache *cache, struct fontpattern *pat, int *matchingcriteria)
{
	struct fontpattern *cpat;
	struct fontpattern *bestmatch = NULL;
	int bestmatchweight = 0;
	
	/* Find best match by calcularing weighted compatibility value */
		
	ITERATELIST(cpat, &cache->entries)
	{
		struct patternentry *cpe, *pe;
		int cmatchingcriteria[FC_LAST_CRITERIA] = {0};
		int weight = 0;
		
		ITERATELIST(pe, &pat->entries)	/* pe - for each entry in criteria pattern. assume that caller is not stupid and doesn't pass elements with 0-weight */
		{
			ITERATELIST(cpe, &cpat->entries)	/* cpe - for each entry in pattern in cache */
			{
				int match = FALSE;
				
				if (cpe->element == pe->element)
				{
					/* first go special elements */
					
					if (cpe->element == FC_FILE)
					{
						char *filepart = FilePart(cpe->value.s);
						if (filepart != NULL) /* paranoid? */
						{
							if (0 == stricmp(filepart, pe->value.s))
								match = TRUE;
						}
					}
					else /* then generic attributes */
					{
											
						if (cpe->type == FC_ETYPE_STRING)
						{
							/* TODO: make all strings uppercase and kill blanks */
							if (0 == FcStrCmpIgnoreBlanksAndCase(cpe->value.s, pe->value.s))
							{
								match = TRUE;
							}
						}
						else if (cpe->type == FC_ETYPE_INTEGER)
						{
							if (cpe->value.i == pe->value.i)
							{
								match = TRUE;
							}
						}
					}
				}
				
				if (match)
				{
					if (pe->excluding) /* if the match was found and the criteria is excluding we lower the score */
					{
						/* if any attribute did match earlier then we don't go below 1 as it still matches better than font with zero matches */
						weight -= fcelementpriority[cpe->element];	
					}
					else
					{
						/* if only excluding attributes were found earlier (<0) then we automaticly bump it to be >0 */
						weight += fcelementpriority[cpe->element];
						if (weight < 0)
							weight = 1;
							
						cmatchingcriteria[cpe->element] = TRUE;
					}
						
					break;
				}
			}
		}	
		
		if (weight > bestmatchweight)
		{
			bestmatch = cpat;
			bestmatchweight = weight;
			if (matchingcriteria != NULL)
				memcpy(matchingcriteria, cmatchingcriteria, sizeof(cmatchingcriteria));
		}
	}
	
	/* we can still land with no font matching! do we need some fallback? */
	
	if (bestmatch == NULL)
	{
		if (FcDebug () & FC_DBG_SCANV)
			kprintf("*****ERROR: No match for pattern\n");
	}
	
	return bestmatch;	
}

static int writeint(FILE *f, int val)
{
	return fwrite(&val, sizeof(val), 1, f);
}

static int writestring(FILE *f, char *str)
{
	int l = strlen(str);
	int rc = writeint(f, l + 1);
	if (rc)
		rc = fwrite(str, l + 1, 1, f);
		
	return rc;
}

int fcSave(struct fontcache *fontcache, char *fname)
{
	struct fontpattern *pat;
	FILE *f = fopen(fname, "w");
	
	if (f == NULL)
		return FALSE;
	
	D(kprintf("writting fontcache to %s\n", fname));
		
	/* write some header + version number */
	
	fprintf(f, "FCDTVER1");
	
	/* now for each pattern */	 
	
	ITERATELIST(pat, &fontcache->entries)
	{
		struct patternentry *pe;
		int elements = 0;
		
		/* what we do here:
		 * write number of entries for pattern
		 * for each pattern entry:
		 *  write element
		 *  write id
		 *  write type
		 *   for int: write value
		 *   for string: write lenght including null termination
		 *               write string contents including null termination
		 */
		 
		ListLength(&pat->entries, elements);
		writeint(f, elements);
		
		ITERATELIST(pe, &pat->entries)
		{
			writeint(f, pe->element);
			writeint(f, pe->id);
			writeint(f, pe->type);
			if (pe->type == FC_ETYPE_STRING)
				writestring(f, pe->value.s);
			else
				writeint(f, pe->value.i);
		}
		
	}
	
	
	fclose(f);
	return TRUE;	
}

static int readint(FILE *f)
{
	int val = 0;
	fread(&val, sizeof(val), 1, f);
	return val;	
}

static char *readstring(FILE *f)
{
	int l = readint(f);
	char *val;
	
	assert(l > 0);
	
	val = malloc(l);
	if (val == NULL)
		return NULL;
		
	fread(val, l, 1, f);
	return val;
}

struct fontcache *fcLoad(char *fname)
{
	FILE *f;
	char header[8];
	struct fontcache *fontcache;
		
	f = fopen(fname, "r");	
	if (f == NULL)
		return NULL;
		
	fontcache = calloc(1, sizeof(*fontcache));
	
	if (fontcache == NULL)
	{	
		fclose(f);
		return NULL;
	}
	
	NewList(&fontcache->entries);
	fread(header, sizeof(header), 1, f);
	
	if (memcmp(header, "FCDTVER1", sizeof(header)))
	{
		fclose(f);
		fcDestroy(fontcache);
		return NULL;
	}
	
	D(kprintf("loading font cache from %s\n", fname));
	
	for(;;)
	{
		struct fontpattern *pat;
		int entries = readint(f);
		if (entries == 0) /* this means end of stream here */
			break;
			
		pat = fcPatternAlloc();
		if (pat != NULL)
		{
			int i;
			for(i=0; i<entries; i++)
			{
				int element = readint(f);
				int id = readint(f);
				int type = readint(f);
				union patternvalue val;
				if (type == FC_ETYPE_STRING)
					val.s = readstring(f);
				else
					val.i = readint(f);
					
				fcPatternAddEntryRaw(pat, element, type, &val, id);
			}
			
			ADDTAIL(&fontcache->entries, pat);
		}
		else
		{
			break;
		}
			
	}
	
	fclose(f);
	return fontcache;
}

int fcAddPattern(struct fontcache *fontcache, struct fontpattern *pat)
{
	if (fontcache != NULL && pat != NULL)
	{
		ADDTAIL(&fontcache->entries, pat);
		return TRUE;
	}
	return FALSE;
}


#ifdef STANDALONE
int main(void)
{
	{
		struct Task *this = FindTask(NULL);
		this->tc_ETask->MaxHits = 1;
	}    


	
	{

		/* scan all fonts installed in any given user directory */
		
		char *dirs[] = {"sys:fonts", "mossys:fonts", NULL};
		
		struct fontcache *cache = fcCreate(dirs);
		
		fcSave(cache, "ram:cache1");
		cache = fcLoad("ram:cache1");
		fcSave(cache, "ram:cache2");
		
		/* try to match simple slanted font */
		
		if (cache != NULL)
		{
			struct fontpattern *pat = fcPatternAlloc();
			struct fontpattern *cpat;
			
			fcPatternAddString(pat, FC_FAMILY, "Times");
			//fcPatternAddString(pat, FC_FILE, "n019003l.pfb");
			
			cpat = fcMatch(cache, pat, NULL);
			if (cpat != NULL)
			{
				char *name = fcPatternGetString(cpat, FC_FILE, 0);
				char *family = fcPatternGetString(cpat, FC_FAMILY, 0);
			}
			else
			{
			}
		}
		
		if (cache != NULL)
		{
			struct fontpattern *pat = fcPatternAlloc();
			struct fontpattern *cpat;
			
			//fcPatternAddString(pat, FC_FAMILY, "Helvetica");
			fcPatternAddString(pat, FC_FILE, "n019003l.pfb");
			
			cpat = fcMatch(cache, pat, NULL);
			if (cpat != NULL)
			{
				char *name = fcPatternGetString(cpat, FC_FILE, 0);
				char *family = fcPatternGetString(cpat, FC_FAMILY, 0);
			}
			else
			{

			}
		}
		
		
	}

	
	

	return 0;
}
#endif


static FcChar32 FcFreeTypePrivateToUcs4 (FcChar32 private, const FcCharMap *map)
{
    int	i;

    for (i = 0; i < map->nent; i++)
	if (map->ent[i].encode == private)
	    return (FcChar32) map->ent[i].bmp;
    return ~0;
}



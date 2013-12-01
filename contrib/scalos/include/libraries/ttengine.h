#ifndef LIBRARIES_TTENGINE_H
#define LIBRARIES_TTENGINE_H

/* $VER: ttengine.h 7.2 (5.4.2005) (c) by Grzegorz Kraszewski 2002 - 2005. */

#define TTENGINENAME "ttengine.library"
#define TTENGINEVERSION 7
#define TTENGINEMINVERSION 6

/* Tags */

/* Tags applicability legend: */
/* O - TT_OpenFont() */
/* G - TT_GetAttrs() */
/* S - TT_SetAttrs() */
/* P - TT_GetPixmap() */

/* ---- name -------------------- value ----- applicability */

#define TT_FontFile             0x6EDA0000    /* OG.. */

#define TT_FontStyle            0x6EDA0001    /* OG.. */

#define TT_FontStyle_Regular    0
#define TT_FontStyle_Italic     1

#define TT_FamilyTable          0x6EDA0002    /* O... */
#define TT_FontSize             0x6EDA0003    /* OG.. */

#define TT_FontWeight           0x6EDA0004    /* OG.. */

#define TT_FontWeight_Normal    400
#define TT_FontWeight_Bold      700

#define TT_ColorMap             0x6EDA0005    /* O... */
#define TT_Screen               0x6EDA0006    /* O... */
#define TT_Window               0x6EDA0007    /* O... */
#define TT_FontAscender         0x6EDA0008    /* .G.. */
#define TT_FontDescender        0x6EDA0009    /* .G.. */

#define TT_Antialias            0x6EDA000F    /* .GSP */

#define TT_Antialias_Auto       0
#define TT_Antialias_Off        1
#define TT_Antialias_On         2

#define TT_Encoding             0x6EDA0010    /* .GSP */

/* All encoding numbers (excluding TT_Encoding_Default) are equal to IANA */
/* registered encoding numbers                                            */

#define TT_Encoding_Default     0             /* use ENV:ttfcodepage or ISO-8859-1 if not found */
#define TT_Encoding_ISO8859_1   4             /* Western Europe and US */
#define TT_Encoding_ISO8859_2   5             /* Eastern Europe */
#define TT_Encoding_ISO8859_3   6
#define TT_Encoding_ISO8859_4   7
#define TT_Encoding_ISO8859_5   8
#define TT_Encoding_ISO8859_6   9
#define TT_Encoding_ISO8859_7   10
#define TT_Encoding_ISO8859_8   11
#define TT_Encoding_ISO8859_9   12
#define TT_Encoding_ISO8859_10  13
#define TT_Encoding_ISO8859_11  14
#define TT_Encoding_ISO8859_13  109
#define TT_Encoding_ISO8859_14  110
#define TT_Encoding_ISO8859_15  111
#define TT_Encoding_ISO8859_16  112
#define TT_Encoding_UTF16_BE    1013
#define TT_Encoding_UTF32_BE    1018
#define TT_Encoding_UTF8        106
#define TT_Encoding_UTF16_LE    1014
#define TT_Encoding_UTF32_LE    1019
#define TT_Encoding_UTF16       1015
#define TT_Encoding_UTF32       1017

#define TT_FontName             0x6EDA0011    /* .G.. */
#define TT_FamilyName           0x6EDA0012    /* .G.. */
#define TT_SubfamilyName        0x6EDA0013    /* .G.. */
#define TT_Transparency         0x6EDA0014    /* .GS.  from 0 to 255 */
#define TT_ScaleX               0x6EDA0015    /* O.SP  16.16 scaled integer */

#define TT_SoftStyle            0x6EDA0017    /* ..SP (V5) */

#define TT_SoftStyle_None           0x0000
#define TT_SoftStyle_Underlined     0x0001
#define TT_SoftStyle_DblUnderlined  0x0002
#define TT_SoftStyle_Overstriked    0x0004
#define TT_SoftStyle_DblOverstriked 0x0008

#define TT_Foreground           0x6EDA0018    /* ..S.  foreground RGB value*/

#define TT_Foreground_UseRastPort  0xFFFFFFFF

#define TT_Background           0x6EDA0019    /* ..S.  background RGB value*/

#define TT_Background_UseRastPort  0xFFFFFFFF

#define TT_FontMaxTop           0x6EDA001E    /* .G.. */
#define TT_FontMaxBottom        0x6EDA001F    /* .G.. */
#define TT_FontDesignHeight     0x6EDA0020    /* .G.. */
#define TT_FontRealAscender     0x6EDA0021    /* .G.. */
#define TT_FontRealDescender    0x6EDA0022    /* .G.. */
#define TT_FontAccentedAscender 0x6EDA0023    /* .G.. */
#define TT_CustomEncoding       0x6EDA0024    /* ..SP */
#define TT_Gamma                0x6EDA0025    /* .GS. */  /* gettable from V7.2 */
#define TT_FontBaseline         TT_FontMaxTop             /* V6.7 */
#define TT_FontFixedWidth       0x6EDA0026    /* OG.. */  /* V6.7 */
#define TT_FontHeight           0x6EDA0027    /* .G.. */  /* V6.7 */
#define TT_FontWidth            0x6EDA0028    /* .G.. */  /* V6.7 */
#define TT_DiskFontMetrics      0x6EDA0029    /* ..SP */  /* V6.7 */
#define TT_ForceFixedWidth      0x6EDA0030    /* ..SP */  /* V7.2 */

/* Structure returned by TT_GetPixmap() (V5)*/

struct TT_Pixmap
{
	ULONG     ttp_Size;       /* size of the structure inculdung this field */
	ULONG     ttp_Width;      /* also equal to bytes per row */
	ULONG     ttp_Height;     /* number of rows */
	UBYTE    *ttp_Data;       /* grayscale pixmap data */
};

/* font requester attributes (V6) */

                                            /* type,             default value     */

#define TTRQ_Window             0x6EDA2000  /* struct Window*,   NULL              */
#define TTRQ_PubScreenName      0x6EDA2001  /* STRPTR,           NULL [Workbench]  */
#define TTRQ_Screen             0x6EDA2002  /* struct Screen*,   NULL              */
#define TTRQ_SleepWindow        0x6EDA2003  /* BOOL,             FALSE             */
#define TTRQ_TitleText          0x6EDA2004  /* STRPTR,           "Select TrueType font" or localized */
#define TTRQ_PositiveText       0x6EDA2005  /* STRPTR,           "OK" or localized */
#define TTRQ_NegativeText       0x6EDA2006  /* STRPTR,           "Cancel" or localized */
#define TTRQ_InitialLeftEdge    0x6EDA2007  /* WORD,             centered on screen */
#define TTRQ_InitialTopEdge     0x6EDA2008  /* WORD,             centered on screen */
#define TTRQ_InitialWidth       0x6EDA2009  /* WORD,             max(200, 25% of sceeen width) */
#define TTRQ_InitialHeight      0x6EDA200A  /* WORD,             max(200, 50% of screen height) */
#define TTRQ_DoSizes            0x6EDA200B  /* BOOL,             TRUE              */
#define TTRQ_DoWeight           0x6EDA200C  /* BOOL,             FALSE             */
#define TTRQ_DoStyle            0x6EDA200D  /* BOOL,             FALSE             */
#define TTRQ_Activate           0x6EDA200E  /* BOOL,             TRUE              */
#define TTRQ_InitialSize        0x6EDA200F  /* LONG,             0 [no size]       */
#define TTRQ_InitialName        0x6EDA2010  /* STRPTR,           NULL [no name]    */
#define TTRQ_InitialStyle       0x6EDA2011  /* LONG,             TT_FontStyle_Regular */
#define TTRQ_DoPreview          0x6EDA2012  /* BOOL,             FALSE             */
#define TTRQ_FixedWidthOnly     0x6EDA2013  /* BOOL,             FALSE             */

#endif /* LIBRARIES_TTENGINE_H */


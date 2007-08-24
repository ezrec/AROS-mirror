/****************************************************************************
*** Includes ****************************************************************
****************************************************************************/

#include <string.h>

#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/feelin.h>

#include <exec/memory.h>
#include <utility/tagitem.h>
#include <libraries/feelin.h>
#include <feelin/xmldocument.h>

#define DOSBase                     FeelinBase -> DOS
#define UtilityBase                 FeelinBase -> Utility

/****************************************************************************
*** Attributes * Methods * Values *******************************************
****************************************************************************/

enum    {                                       // attributes

        FA_XMLDocument_Markups,
        FA_XMLDocument_Attributes

        };
        
enum    {                                       // methods

        FM_XMLDocument_Parse,

        };

enum    {                                       // resolved

        FA_Document_Pool,
        FA_Document_Source,
        FA_Document_SourceType,
        FA_Document_Version,
        FA_Document_Revision,

        FM_Document_Read,
        FM_Document_AddIDs,
        FM_Document_Resolve,
        FM_Document_ObtainName,
        FM_Document_Log

        };

enum    {

        FV_XML_ERROR_NONE,
        FV_XML_ERROR_NULL,
        FV_XML_ERROR_MEMORY,
        FV_XML_ERROR_SYNTAX,
        FV_XML_ERROR_HEADER,
        FV_XML_ERROR_OPEN,
        FV_XML_ERROR_CLOSE,
        FV_XML_ERROR_INTEROGATION,
        FV_XML_ERROR_EQUAL,
        FV_XML_ERROR_QUOTE,
        FV_XML_ERROR_NAME,
        FV_XML_ERROR_ATTRIBUTE

        };

/****************************************************************************
*** Structures **************************************************************
****************************************************************************/

struct LocalObjectData
{
    FList                           AttributesList;
    FList                           MarkupsList;
    APTR                            Pool;
    uint32                          line;
};

#define QUOTE_SINGLE                            39
#define QUOTE_DOUBLE                            34

#define F_SKIPWHITE(str)                        while (*str && (*str == ' ' || *str == '\t' || *str == '\n')) { if (*str == '\n') { LOD -> line++; /*F_Log(0,"line %ld (%16.16s)",LOD->line,str-16);*/ } str++; }
#define F_NEXTWHITE(str)                        while (*str && (*str != ' ' && *str != '\t' && *str != '\n')) str++

#define F_ERROR(n)                              { er = FV_XML_ERROR_ ## n; goto __error; }


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
#include <feelin/htmldocument.h>

#define DOSBase                     FeelinBase -> DOS

/****************************************************************************
*** Attributes, Methods, Values *********************************************
****************************************************************************/

enum    {

        FA_HTMLDocument_Nodes,
        FA_HTMLDocument_Attributes

        };

enum    {

        FM_HTMLDocument_Parse

        };


enum    {                                       // resolve

        FM_Document_Read,
        FM_Document_Resolve,
        FM_Document_ObtainName,
        FM_Document_Log

        };

enum    {

        FV_PARSE_ERROR_NONE,
        FV_PARSE_ERROR_NULL,
        FV_PARSE_ERROR_MEMORY,
        FV_PARSE_ERROR_SYNTAX,
        FV_PARSE_ERROR_HEADER,
        FV_PARSE_ERROR_OPEN,
        FV_PARSE_ERROR_CLOSE,
        FV_PARSE_ERROR_EQUAL,
        FV_PARSE_ERROR_QUOTE,
        FV_PARSE_ERROR_NAME,
        FV_PARSE_ERROR_ATTRIBUTE

        };

/****************************************************************************
*** Structures **************************************************************
****************************************************************************/

struct LocalObjectData
{
    APTR                            Pool;
    FList                           AttributesList;
    FList                           NodesList;

    uint32                          line;
};

#define QUOTE_SINGLE                            39
#define QUOTE_DOUBLE                            34

#define F_SKIPWHITE(str)                        while (*str && (*str == ' ' || *str == '\t' || *str == '\n')) { if (*str == '\n') { LOD -> line++; /*F_Log(0,"line %ld (%16.16s)",LOD->line,str-16);*/ } str++; }
#define F_NEXTWHITE(str)                        while (*str && (*str != ' ' && *str != '\t' && *str != '\n')) str++

#define F_ERROR(n)                              { er = FV_PARSE_ERROR_ ## n; goto __error; }


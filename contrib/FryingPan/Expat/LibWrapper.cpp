#include <Generic/LibrarySpool.h>
#include <LibC/LibC.h>
#include <GenericLib/Library.h>
#include "expat.h"
#include <Startup/Startup.h>

uint32 StartupFlags = 0;

#define str(x) #x
int main()
{
   return 0;
}

   LIBRARY("expat2.library", "$VER: expat2.library 2.0 by Tomasz Wiszkowski", 2);

   GATE1(XML_Parser,       XML_ParserCreate,                      const XML_Char *, a0);
   GATE2(XML_Parser,       XML_ParserCreateNS,                    const XML_Char *, a0, XML_Char, d0);
   GATE3(XML_Parser,       XML_ParserCreate_MM,                   const XML_Char *, a0, const XML_Memory_Handling_Suite *, a1, const XML_Char *, a2);
   GATE2(void,             XML_SetUserData,                       XML_Parser, a0, void*, a1);
   GATE2(void,             XML_SetElementDeclHandler,             XML_Parser, a0, XML_ElementDeclHandler, a1);
   GATE2(void,             XML_SetAttlistDeclHandler,             XML_Parser, a0, XML_AttlistDeclHandler, a1);
   GATE2(void,             XML_SetXmlDeclHandler,                 XML_Parser, a0, XML_XmlDeclHandler, a1);
   GATE2(XML_Bool,         XML_ParserReset,                       XML_Parser, a0, const XML_Char*, a1);
   GATE2(void,             XML_SetEntityDeclHandler,              XML_Parser, a0, XML_EntityDeclHandler, a1);
   GATE3(void,             XML_SetElementHandler,                 XML_Parser, a0, XML_StartElementHandler, a1, XML_EndElementHandler, a2);
   GATE2(void,             XML_SetStartElementHandler,            XML_Parser, a0, XML_StartElementHandler, a1);
   GATE2(void,             XML_SetEndElementHandler,              XML_Parser, a0, XML_EndElementHandler, a1);
   GATE2(void,             XML_SetCharacterDataHandler,           XML_Parser, a0, XML_CharacterDataHandler, a1);
   GATE2(void,             XML_SetProcessingInstructionHandler,   XML_Parser, a0, XML_ProcessingInstructionHandler, a1);
   GATE2(void,             XML_SetCommentHandler,                 XML_Parser, a0, XML_CommentHandler, a1);
   GATE3(void,             XML_SetCdataSectionHandler,            XML_Parser, a0, XML_StartCdataSectionHandler, a1, XML_EndCdataSectionHandler, a2);
   GATE2(void,             XML_SetStartCdataSectionHandler,       XML_Parser, a0, XML_StartCdataSectionHandler, a1);
   GATE2(void,             XML_SetEndCdataSectionHandler,         XML_Parser, a0, XML_EndCdataSectionHandler, a1);
   GATE2(void,             XML_SetDefaultHandler,                 XML_Parser, a0, XML_DefaultHandler, a1);
   GATE2(void,             XML_SetDefaultHandlerExpand,           XML_Parser, a0, XML_DefaultHandler, a1);
   GATE3(void,             XML_SetDoctypeDeclHandler,             XML_Parser, a0, XML_StartDoctypeDeclHandler, a1, XML_EndDoctypeDeclHandler, a2);
   GATE2(void,             XML_SetStartDoctypeDeclHandler,        XML_Parser, a0, XML_StartDoctypeDeclHandler, a1);
   GATE2(void,             XML_SetEndDoctypeDeclHandler,          XML_Parser, a0, XML_EndDoctypeDeclHandler, a1);
   GATE2(void,             XML_SetUnparsedEntityDeclHandler,      XML_Parser, a0, XML_UnparsedEntityDeclHandler, a1);
   GATE2(void,             XML_SetNotationDeclHandler,            XML_Parser, a0, XML_NotationDeclHandler, a1);
   GATE3(void,             XML_SetNamespaceDeclHandler,           XML_Parser, a0, XML_StartNamespaceDeclHandler, a1, XML_EndNamespaceDeclHandler, a2);
   GATE2(void,             XML_SetStartNamespaceDeclHandler,      XML_Parser, a0, XML_StartNamespaceDeclHandler, a1);
   GATE2(void,             XML_SetEndNamespaceDeclHandler,        XML_Parser, a0, XML_EndNamespaceDeclHandler, a1);
   GATE2(void,             XML_SetNotStandaloneHandler,           XML_Parser, a0, XML_NotStandaloneHandler, a1);
   GATE2(void,             XML_SetExternalEntityRefHandler,       XML_Parser, a0, XML_ExternalEntityRefHandler, a1);
   GATE2(void,             XML_SetExternalEntityRefHandlerArg,    XML_Parser, a0, void *, a1);
   GATE2(void,             XML_SetSkippedEntityHandler,           XML_Parser, a0, XML_SkippedEntityHandler, a1);
   GATE3(void,             XML_SetUnknownEncodingHandler,         XML_Parser, a0, XML_UnknownEncodingHandler, a1, void *, a2);
   GATE1(void,             XML_DefaultCurrent,                    XML_Parser, a0);
   GATE2(void,             XML_SetReturnNSTriplet,                XML_Parser, a0, int, d0);
   GATE2(XML_Status,       XML_SetEncoding,                       XML_Parser, a0, const XML_Char *, a1);
   GATE1(void,             XML_UseParserAsHandlerArg,             XML_Parser, a0);
   GATE2(XML_Error,        XML_UseForeignDTD,                     XML_Parser, a0, XML_Bool, a1);
   GATE2(XML_Status,       XML_SetBase,                           XML_Parser, a0, const XML_Char *, a1);
   GATE1(const XML_Char*,  XML_GetBase,                           XML_Parser, a0);
   GATE1(int,              XML_GetSpecifiedAttributeCount,        XML_Parser, a0); 
   GATE1(int,              XML_GetIdAttributeIndex,               XML_Parser, a0);
   GATE4(XML_Status,       XML_Parse,                             XML_Parser, a0, const char *, a1, int, d0, int, d1);
   GATE2(void *,           XML_GetBuffer,                         XML_Parser, a0, int, d0);
   GATE3(XML_Status,       XML_ParseBuffer,                       XML_Parser, a0, int, d0, int, d1);
   GATE2(XML_Status,       XML_StopParser,                        XML_Parser, a0, XML_Bool, d0);
   GATE1(XML_Status,       XML_ResumeParser,                      XML_Parser, a0);
   GATE2(void,             XML_GetParsingStatus,                  XML_Parser, a0, XML_ParsingStatus *, a1);
   GATE3(XML_Parser,       XML_ExternalEntityParserCreate,        XML_Parser, a0, const XML_Char *, a1, const XML_Char *, a2);
   GATE2(int,              XML_SetParamEntityParsing,             XML_Parser, a0, XML_ParamEntityParsing, a1);
   GATE1(XML_Error,        XML_GetErrorCode,                      XML_Parser, a0);
   GATE1(XML_Size,         XML_GetCurrentLineNumber,              XML_Parser, a0);
   GATE1(XML_Size,         XML_GetCurrentColumnNumber,            XML_Parser, a0);
   GATE1(XML_Index,        XML_GetCurrentByteIndex,               XML_Parser, a0);
   GATE1(int,              XML_GetCurrentByteCount,               XML_Parser, a0);
   GATE3(const char *,     XML_GetInputContext,                   XML_Parser, a0, int *, a1, int *, a2);
   GATE2(void,             XML_FreeContentModel,                  XML_Parser, a0, XML_Content *, a1);
   GATE2(void *,           XML_MemMalloc,                         XML_Parser, a0, size_t, d0);
   GATE3(void *,           XML_MemRealloc,                        XML_Parser, a0, void *, a1, size_t, d0);
   GATE2(void,             XML_MemFree,                           XML_Parser, a0, void *, a1);
   GATE1(void,             XML_ParserFree,                        XML_Parser, a0);
   GATE1(const XML_LChar *,XML_ErrorString,                       XML_Error, d0);
   GATE0(const XML_LChar *,XML_ExpatVersion                       );
   GATE0(XML_Expat_Version, XML_ExpatVersionInfo                  );
   GATE0(const XML_Feature *,XML_GetFeatureList                   );


   
   LIB_FT_Begin
      LIB_FT_Function(XML_ParserCreate)
      LIB_FT_Function(XML_ParserCreateNS)
      LIB_FT_Function(XML_ParserCreate_MM)
      LIB_FT_Function(XML_SetUserData)
      LIB_FT_Function(XML_SetElementDeclHandler)
      LIB_FT_Function(XML_SetAttlistDeclHandler)
      LIB_FT_Function(XML_SetXmlDeclHandler)
      LIB_FT_Function(XML_ParserReset)
      LIB_FT_Function(XML_SetEntityDeclHandler)
      LIB_FT_Function(XML_SetElementHandler)
      LIB_FT_Function(XML_SetStartElementHandler)
      LIB_FT_Function(XML_SetEndElementHandler)
      LIB_FT_Function(XML_SetCharacterDataHandler)
      LIB_FT_Function(XML_SetProcessingInstructionHandler)
      LIB_FT_Function(XML_SetCommentHandler)
      LIB_FT_Function(XML_SetCdataSectionHandler)
      LIB_FT_Function(XML_SetStartCdataSectionHandler)
      LIB_FT_Function(XML_SetEndCdataSectionHandler)
      LIB_FT_Function(XML_SetDefaultHandler)
      LIB_FT_Function(XML_SetDefaultHandlerExpand)
      LIB_FT_Function(XML_SetDoctypeDeclHandler)
      LIB_FT_Function(XML_SetStartDoctypeDeclHandler)
      LIB_FT_Function(XML_SetEndDoctypeDeclHandler)
      LIB_FT_Function(XML_SetUnparsedEntityDeclHandler)
      LIB_FT_Function(XML_SetNotationDeclHandler)
      LIB_FT_Function(XML_SetNamespaceDeclHandler)
      LIB_FT_Function(XML_SetStartNamespaceDeclHandler)
      LIB_FT_Function(XML_SetEndNamespaceDeclHandler)
      LIB_FT_Function(XML_SetNotStandaloneHandler)
      LIB_FT_Function(XML_SetExternalEntityRefHandler)
      LIB_FT_Function(XML_SetExternalEntityRefHandlerArg)
      LIB_FT_Function(XML_SetSkippedEntityHandler)
      LIB_FT_Function(XML_SetUnknownEncodingHandler)
      LIB_FT_Function(XML_DefaultCurrent)
      LIB_FT_Function(XML_SetReturnNSTriplet)
      LIB_FT_Function(XML_SetEncoding)
      LIB_FT_Function(XML_UseParserAsHandlerArg)
      LIB_FT_Function(XML_UseForeignDTD)
      LIB_FT_Function(XML_SetBase)
      LIB_FT_Function(XML_GetBase)
      LIB_FT_Function(XML_GetSpecifiedAttributeCount)
      LIB_FT_Function(XML_GetIdAttributeIndex)
      LIB_FT_Function(XML_Parse)
      LIB_FT_Function(XML_GetBuffer)
      LIB_FT_Function(XML_ParseBuffer)
      LIB_FT_Function(XML_StopParser)
      LIB_FT_Function(XML_ResumeParser)
      LIB_FT_Function(XML_GetParsingStatus)
      LIB_FT_Function(XML_ExternalEntityParserCreate)
      LIB_FT_Function(XML_SetParamEntityParsing)
      LIB_FT_Function(XML_GetErrorCode)
      LIB_FT_Function(XML_GetCurrentLineNumber)
      LIB_FT_Function(XML_GetCurrentColumnNumber)
      LIB_FT_Function(XML_GetCurrentByteIndex)
      LIB_FT_Function(XML_GetCurrentByteCount)
      LIB_FT_Function(XML_GetInputContext)
      LIB_FT_Function(XML_FreeContentModel)
      LIB_FT_Function(XML_MemMalloc)
      LIB_FT_Function(XML_MemRealloc)
      LIB_FT_Function(XML_MemFree)
      LIB_FT_Function(XML_ParserFree)
      LIB_FT_Function(XML_ErrorString)
      LIB_FT_Function(XML_ExpatVersion)
      LIB_FT_Function(XML_ExpatVersionInfo)
      LIB_FT_Function(XML_GetFeatureList)
   LIB_FT_End
   
   
bool Lib_SetUp()
{
   __setup();
   return true;
}

void Lib_CleanUp()
{
   __cleanup();
}

bool Lib_Acquire()
{
   return true;
}

void Lib_Release()
{
}


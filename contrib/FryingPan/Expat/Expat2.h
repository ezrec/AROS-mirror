#ifndef EXPAT2_H_
#define EXPAT2_H_

#include "expat.h"
#include <GenericLib/Calls.h>

   BEGINDECL(Expat2, "expat2.library")

      FUNC1(XML_Parser,       XML_ParserCreate,                      5, const XML_Char *, encoding, a0);
      FUNC2(XML_Parser,       XML_ParserCreateNS,                    6, const XML_Char *, encoding, a0, XML_Char, workspaceSeparator, d0);
      FUNC3(XML_Parser,       XML_ParserCreate_MM,                   7, const XML_Char *, encoding, a0, const XML_Memory_Handling_Suite *, memSuite, a1, const XML_Char *, workspaceSeparator, a2);
      PROC2(                  XML_SetUserData,                       8, XML_Parser, parser, a0, void*, userData, a1);
      PROC2(                  XML_SetElementDeclHandler,             9, XML_Parser, parser, a0, XML_ElementDeclHandler, eldecl, a1);
      PROC2(                  XML_SetAttlistDeclHandler,            10, XML_Parser, parser, a0, XML_AttlistDeclHandler, attdecl, a1);
      PROC2(                  XML_SetXmlDeclHandler,                11, XML_Parser, parser, a0, XML_XmlDeclHandler, xmldecl, a1);
      FUNC2(XML_Bool,         XML_ParserReset,                      12, XML_Parser, parser, a0, const XML_Char*, encoding, a1);
      PROC2(                  XML_SetEntityDeclHandler,             13, XML_Parser, parser, a0, XML_EntityDeclHandler, handler, a1);
      PROC3(                  XML_SetElementHandler,                14, XML_Parser, parser, a0, XML_StartElementHandler, start, a1, XML_EndElementHandler, end, a2);
      PROC2(                  XML_SetStartElementHandler,           15, XML_Parser, parser, a0, XML_StartElementHandler, handler, a1);
      PROC2(                  XML_SetEndElementHandler,             16, XML_Parser, parser, a0, XML_EndElementHandler, handler, a1);
      PROC2(                  XML_SetCharacterDataHandler,          17, XML_Parser, parser, a0, XML_CharacterDataHandler, handler, a1);
      PROC2(                  XML_SetProcessingInstructionHandler,  18, XML_Parser, parser, a0, XML_ProcessingInstructionHandler, handler, a1);
      PROC2(                  XML_SetCommentHandler,                19, XML_Parser, parser, a0, XML_CommentHandler, handler, a1);
      PROC3(                  XML_SetCdataSectionHandler,           20, XML_Parser, parser, a0, XML_StartCdataSectionHandler, start, a1, XML_EndCdataSectionHandler, end, a2);
      PROC2(                  XML_SetStartCdataSectionHandler,      21, XML_Parser, parser, a0, XML_StartCdataSectionHandler, handler, a1);
      PROC2(                  XML_SetEndCdataSectionHandler,        22, XML_Parser, parser, a0, XML_EndCdataSectionHandler, handler, a1);
      PROC2(                  XML_SetDefaultHandler,                23, XML_Parser, parser, a0, XML_DefaultHandler, handler, a1);
      PROC2(                  XML_SetDefaultHandlerExpand,          24, XML_Parser, parser, a0, XML_DefaultHandler, handler, a1);
      PROC3(                  XML_SetDoctypeDeclHandler,            25, XML_Parser, parser, a0, XML_StartDoctypeDeclHandler, start, a1, XML_EndDoctypeDeclHandler, end, a2);
      PROC2(                  XML_SetStartDoctypeDeclHandler,       26, XML_Parser, parser, a0, XML_StartDoctypeDeclHandler, handler, a1);
      PROC2(                  XML_SetEndDoctypeDeclHandler,         27, XML_Parser, parser, a0, XML_EndDoctypeDeclHandler, handler, a1);
      PROC2(                  XML_SetUnparsedEntityDeclHandler,     28, XML_Parser, parser, a0, XML_UnparsedEntityDeclHandler, handler, a1);
      PROC2(                  XML_SetNotationDeclHandler,           29, XML_Parser, parser, a0, XML_NotationDeclHandler, handler, a1);
      PROC3(                  XML_SetNamespaceDeclHandler,          30, XML_Parser, parser, a0, XML_StartNamespaceDeclHandler, start, a1, XML_EndNamespaceDeclHandler, end, a2);
      PROC2(                  XML_SetStartNamespaceDeclHandler,     31, XML_Parser, parser, a0, XML_StartNamespaceDeclHandler, handler, a1);
      PROC2(                  XML_SetEndNamespaceDeclHandler,       32, XML_Parser, parser, a0, XML_EndNamespaceDeclHandler, handler, a1);
      PROC2(                  XML_SetNotStandaloneHandler,          33, XML_Parser, parser, a0, XML_NotStandaloneHandler, handler, a1);
      PROC2(                  XML_SetExternalEntityRefHandler,      34, XML_Parser, parser, a0, XML_ExternalEntityRefHandler, handler, a1);
      PROC2(                  XML_SetExternalEntityRefHandlerArg,   35, XML_Parser, parser, a0, void *, handlerArg, a1);
      PROC2(                  XML_SetSkippedEntityHandler,          36, XML_Parser, parser, a0, XML_SkippedEntityHandler, handler, a1);
      PROC3(                  XML_SetUnknownEncodingHandler,        37, XML_Parser, parser, a0, XML_UnknownEncodingHandler, handler, a1, void *, handlerData, a2);
      PROC1(                  XML_DefaultCurrent,                   38, XML_Parser, parser, a0);
      PROC2(                  XML_SetReturnNSTriplet,               39, XML_Parser, parser, a0, int, doNst, d0);
      FUNC2(XML_Status,       XML_SetEncoding,                      40, XML_Parser, parser, a0, const XML_Char *, encoding, a1);
      PROC1(                  XML_UseParserAsHandlerArg,            41, XML_Parser, parser, a0);
      FUNC2(XML_Error,        XML_UseForeignDTD,                    42, XML_Parser, parser, a0, XML_Bool, useForeign, a1);
      FUNC2(XML_Status,       XML_SetBase,                          43, XML_Parser, parser, a0, const XML_Char *, base, a1);
      FUNC1(const XML_Char*,  XML_GetBase,                          44, XML_Parser, parser, a0);
      FUNC1(int,              XML_GetSpecifiedAttributeCount,       45, XML_Parser, parser, a0); 
      FUNC1(int,              XML_GetIdAttributeIndex,              46, XML_Parser, parser, a0);
      FUNC4(XML_Status,       XML_Parse,                            47, XML_Parser, parser, a0, const char *, s, a1, int, len, d0, int, isFinal, d1);
      FUNC2(void *,           XML_GetBuffer,                        48, XML_Parser, parser, a0, int, len, d0);
      FUNC3(XML_Status,       XML_ParseBuffer,                      49, XML_Parser, parser, a0, int, len, d0, int, isFinal, d1);
      FUNC2(XML_Status,       XML_StopParser,                       50, XML_Parser, parser, a0, XML_Bool, resumable, d0);
      FUNC1(XML_Status,       XML_ResumeParser,                     51, XML_Parser, parser, a0);
      PROC2(                  XML_GetParsingStatus,                 52, XML_Parser, parser, a0, XML_ParsingStatus *, status, a1);
      FUNC3(XML_Parser,       XML_ExternalEntityParserCreate,       53, XML_Parser, parser, a0, const XML_Char *, context, a1, const XML_Char *, encoding, a2);
      FUNC2(int,              XML_SetParamEntityParsing,            54, XML_Parser, parser, a0, XML_ParamEntityParsing, parsing, a1);
      FUNC1(XML_Error,        XML_GetErrorCode,                     55, XML_Parser, parser, a0);
      FUNC1(XML_Size,         XML_GetCurrentLineNumber,             56, XML_Parser, parser, a0);
      FUNC1(XML_Size,         XML_GetCurrentColumnNumber,           57, XML_Parser, parser, a0);
      FUNC1(XML_Index,        XML_GetCurrentByteIndex,              58, XML_Parser, parser, a0);
      FUNC1(int,              XML_GetCurrentByteCount,              59, XML_Parser, parser, a0);
      FUNC3(const char *,     XML_GetInputContext,                  60, XML_Parser, parser, a0, int *, offset, a1, int *, size, a2);
      PROC2(                  XML_FreeContentModel,                 61, XML_Parser, parser, a0, XML_Content *, model, a1);
      FUNC2(void *,           XML_MemMalloc,                        62, XML_Parser, parser, a0, size_t, size, d0);
      FUNC3(void *,           XML_MemRealloc,                       63, XML_Parser, parser, a0, void *, ptr, a1, size_t, size, d0);
      PROC2(                  XML_MemFree,                          64, XML_Parser, parser, a0, void *, ptr, a1);
      PROC1(                  XML_ParserFree,                       65, XML_Parser, parser, a0);
      FUNC1(const XML_LChar *,XML_ErrorString,                      66, XML_Error,  error, d0);
      FUNC0(const XML_LChar *,XML_ExpatVersion,                     67);
      FUNC0(XML_Expat_Version*,XML_ExpatVersionInfo,                68);
      FUNC0(const XML_Feature*,XML_GetFeatureList,                  69);

   ENDDECL

#endif /*EXPAT2_H_*/

#include "lua.h"
#include "lauxlib.h"

#include <libxml/xmlerror.h>
/* TODO: wrap the error object so user code can retrieve and query the last 
 * error */
/* Mostly ripped from code in parser.c and error.c in libxml. Not ideal, but 
 * it seems impossible to hook into libxml's error string generation */
int xml_push_error(lua_State *L, xmlErrorPtr err)
{
  if (err==NULL)
    luaL_error(L, "a NULL error was raised. Should not happen");

  char *file = err->file;
  int line = err->line;
/*  int code = err->code; */
  int domain = err->domain;
  xmlErrorLevel level = err->level;
  const xmlChar *name = NULL;
  xmlNodePtr node = err->node;

  luaL_Buffer buf;
  luaL_buffinit(L, &buf);
  luaL_Buffer *B = &buf;

  if ((node != NULL) && (node->type == XML_ELEMENT_NODE))
    name = node->name;

  if (file != NULL)
  {
    lua_pushfstring(L, "%s:%d: ", file, line);
    luaL_addvalue(B);
  }
  else if ((line != 0) && (domain == XML_FROM_PARSER))
  {
    lua_pushfstring(L, "Entity: line %d: ", line);
    luaL_addvalue(B);
  }
  if (name != NULL)
  {
    lua_pushfstring(L, "element %s: ", name);
    luaL_addvalue(B);
  }
  switch (domain)
  {
    case XML_FROM_PARSER:
      luaL_addstring(B, "parser ");
      break;
    case XML_FROM_NAMESPACE:
      luaL_addstring(B, "namespace ");
      break;
    case XML_FROM_DTD:
    case XML_FROM_VALID:
      luaL_addstring(B, "validity ");
      break;
    case XML_FROM_HTML:
      luaL_addstring(B, "HTML parser ");
      break;
    case XML_FROM_MEMORY:
      luaL_addstring(B, "memory ");
      break;
    case XML_FROM_OUTPUT:
      luaL_addstring(B, "output ");
      break;
    case XML_FROM_IO:
      luaL_addstring(B, "I/O ");
      break;
    case XML_FROM_XINCLUDE:
      luaL_addstring(B, "XInclude ");
      break;
    case XML_FROM_XPATH:
      luaL_addstring(B, "XPath ");
      break;
    case XML_FROM_XPOINTER:
      luaL_addstring(B, "parser ");
      break;
    case XML_FROM_REGEXP:
      luaL_addstring(B, "regexp ");
      break;
    case XML_FROM_MODULE:
      luaL_addstring(B, "module ");
      break;
    case XML_FROM_SCHEMASV:
      luaL_addstring(B, "Schemas validity ");
      break;
    case XML_FROM_SCHEMASP:
      luaL_addstring(B, "Schemas parser ");
      break;
    case XML_FROM_RELAXNGP:
      luaL_addstring(B, "Relax-NG parser ");
      break;
    case XML_FROM_RELAXNGV:
      luaL_addstring(B, "Relax-NG validity ");
      break;
    case XML_FROM_CATALOG:
      luaL_addstring(B, "Catalog ");
      break;
    case XML_FROM_C14N:
      luaL_addstring(B, "C14N ");
      break;
    case XML_FROM_XSLT:
      luaL_addstring(B, "XSLT ");
      break;
    case XML_FROM_I18N:
      luaL_addstring(B, "encoding ");
      break;
    default:
      break;
  }
  switch (level) {
    case XML_ERR_NONE:
      luaL_addstring(B, ": ");
      break;
    case XML_ERR_WARNING:
      luaL_addstring(B, "warning : ");
      break;
    case XML_ERR_ERROR:
      luaL_addstring(B, "error : ");
      break;
    case XML_ERR_FATAL:
      luaL_addstring(B, "error : ");
      break;
  }

  if (err->message)
    luaL_addstring(B, err->message);

  luaL_pushresult(B);
  return 1;
}

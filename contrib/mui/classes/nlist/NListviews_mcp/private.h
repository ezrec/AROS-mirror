/*
  NListviews.mcp (c) Copyright 1996 by Gilles Masson
  Registered MUI class, Serial Number: 1d51                            0x9d510001 to 0x9d51001F
                                                                   and 0x9d510101 to 0x9d51013F
  *** use only YOUR OWN Serial Number for your public custom class ***
  NListviews_priv_mcp.h
*/

#ifndef MUI_NLISTVIEWS_priv_MCP_H
#define MUI_NLISTVIEWS_priv_MCP_H


extern UWORD LIBVER;
#define LIBVER(lib)          ((struct Library *)lib)->lib_Version

#ifndef MUI_MUI_H
/* #include <mui/mui.h> */
#include <mui.h>
#endif

#include "NListviews_mcp.h"

#include <mcc_common.h>
#include <mcc_debug.h>

#ifdef MORPHOS
#define DoMethod(MyObject, tags...) \
	({ULONG _tags[] = { tags }; DoMethodA((MyObject), (APTR)_tags);})
#define DoSuperMethod(MyClass, MyObject, tags...) \
	({ULONG _tags[] = { tags }; DoSuperMethodA((MyClass), (MyObject), (APTR)_tags);})
#define CoerceMethod(MyClass, MyObject, tags...) \
	({ULONG _tags[] = { tags }; CoerceMethodA((MyClass), (MyObject), (APTR)_tags);})
#endif


#define MCPMAXRAWBUF 64


struct NListviews_MCP_Data
{
  APTR mcp_group;

  APTR mcp_list1;
  APTR mcp_list2;

  APTR mcp_PenTitle;
  APTR mcp_PenList;
  APTR mcp_PenSelect;
  APTR mcp_PenCursor;
  APTR mcp_PenUnselCur;
  APTR mcp_BG_Title;
  APTR mcp_BG_List;
  APTR mcp_BG_Select;
  APTR mcp_BG_Cursor;
  APTR mcp_BG_UnselCur;

  APTR mcp_R_Multi;
  APTR mcp_B_MultiMMB;
  APTR mcp_R_Drag;
  APTR mcp_ST_DragQualifier;
  APTR mcp_SL_VertInc;
  APTR mcp_R_HSB;
  APTR mcp_R_VSB;
  APTR mcp_B_Smooth;

  APTR mcp_Font;
  APTR mcp_Font_Little;
  APTR mcp_Font_Fixed;

  APTR mcp_ForcePen;
  APTR mcp_StackCheck;
  APTR mcp_ColWidthDrag;
  APTR mcp_PartialCol;
  APTR mcp_List_Select;
  APTR mcp_NList_Menu;
  APTR mcp_PartialChar;
  APTR mcp_PointerColor;
  APTR mcp_SerMouseFix;
  APTR mcp_DragLines;

  APTR mcp_WheelStep;
  APTR mcp_WheelFast;
  APTR mcp_WheelMMB;

  APTR mcp_listkeys;
  APTR mcp_stringkey;
  APTR mcp_snoopkey;
  APTR mcp_insertkey;
  APTR mcp_removekey;
  APTR mcp_updatekeys;
  APTR mcp_defaultkeys;
  APTR mcp_txtfct;
  APTR mcp_popstrfct;
  APTR mcp_poplistfct;

  struct KeyBinding *nlkeys;
  LONG nlkeys_size;
  char QualifierString[192];

  char rawtext[MCPMAXRAWBUF];

  struct InputEvent ievent;

  WORD pad1,pad2;
};

#endif /* MUI_NLISTVIEWS_priv_MCP_H */


#ifndef _INCLUDE_PRAGMA_TTENGINE_LIB_H
#define _INCLUDE_PRAGMA_TTENGINE_LIB_H

#ifndef CLIB_TTENGINE_PROTOS_H
#include <clib/ttengine_protos.h>
#endif

#if defined(AZTEC_C) || defined(__MAXON__) || defined(__STORM__)
#pragma amicall(TTEngineBase,0x01e,TT_OpenFontA(a0))
#pragma amicall(TTEngineBase,0x024,TT_SetFont(a1,a0))
#pragma amicall(TTEngineBase,0x02a,TT_CloseFont(a0))
#pragma amicall(TTEngineBase,0x030,TT_Text(a1,a0,d0))
#pragma amicall(TTEngineBase,0x036,TT_SetAttrsA(a1,a0))
#pragma amicall(TTEngineBase,0x03c,TT_GetAttrsA(a1,a0))
#pragma amicall(TTEngineBase,0x042,TT_TextLength(a1,a0,d0))
#pragma amicall(TTEngineBase,0x048,TT_TextExtent(a1,a0,d0,a2))
#pragma amicall(TTEngineBase,0x04e,TT_TextFit(a1,a0,d0,a2,a3,d1,d2,d3))
#pragma amicall(TTEngineBase,0x054,TT_GetPixmapA(a1,a2,d0,a0))
#pragma amicall(TTEngineBase,0x05a,TT_FreePixmap(a0))
#pragma amicall(TTEngineBase,0x060,TT_DoneRastPort(a1))
#pragma amicall(TTEngineBase,0x066,TT_AllocRequest())
#pragma amicall(TTEngineBase,0x06c,TT_RequestA(a0,a1))
#pragma amicall(TTEngineBase,0x072,TT_FreeRequest(a0))
#pragma amicall(TTEngineBase,0x078,TT_ObtainFamilyListA(a0))
#pragma amicall(TTEngineBase,0x07e,TT_FreeFamilyList(a0))
#endif
#if defined(_DCC) || defined(__SASC)
#pragma  libcall TTEngineBase TT_OpenFontA           01e 801
#pragma  libcall TTEngineBase TT_SetFont             024 8902
#pragma  libcall TTEngineBase TT_CloseFont           02a 801
#pragma  libcall TTEngineBase TT_Text                030 08903
#pragma  libcall TTEngineBase TT_SetAttrsA           036 8902
#pragma  libcall TTEngineBase TT_GetAttrsA           03c 8902
#pragma  libcall TTEngineBase TT_TextLength          042 08903
#pragma  libcall TTEngineBase TT_TextExtent          048 a08904
#pragma  libcall TTEngineBase TT_TextFit             04e 321ba08908
#pragma  libcall TTEngineBase TT_GetPixmapA          054 80a904
#pragma  libcall TTEngineBase TT_FreePixmap          05a 801
#pragma  libcall TTEngineBase TT_DoneRastPort        060 901
#pragma  libcall TTEngineBase TT_AllocRequest        066 00
#pragma  libcall TTEngineBase TT_RequestA            06c 9802
#pragma  libcall TTEngineBase TT_FreeRequest         072 801
#pragma  libcall TTEngineBase TT_ObtainFamilyListA   078 801
#pragma  libcall TTEngineBase TT_FreeFamilyList      07e 801
#endif
#ifdef __STORM__
#pragma tagcall(TTEngineBase,0x01e,TT_OpenFont(a0))
#pragma tagcall(TTEngineBase,0x036,TT_SetAttrs(a1,a0))
#pragma tagcall(TTEngineBase,0x03c,TT_GetAttrs(a1,a0))
#pragma tagcall(TTEngineBase,0x054,TT_GetPixmap(a1,a2,d0,a0))
#pragma tagcall(TTEngineBase,0x06c,TT_Request(a0,a1))
#pragma tagcall(TTEngineBase,0x078,TT_ObtainFamilyList(a0))
#endif
#ifdef __SASC_60
#pragma  tagcall TTEngineBase TT_OpenFont            01e 801
#pragma  tagcall TTEngineBase TT_SetAttrs            036 8902
#pragma  tagcall TTEngineBase TT_GetAttrs            03c 8902
#pragma  tagcall TTEngineBase TT_GetPixmap           054 80a904
#pragma  tagcall TTEngineBase TT_Request             06c 9802
#pragma  tagcall TTEngineBase TT_ObtainFamilyList    078 801
#endif

#endif	/*  _INCLUDE_PRAGMA_TTENGINE_LIB_H  */

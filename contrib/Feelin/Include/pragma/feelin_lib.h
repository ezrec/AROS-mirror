#ifndef _PRAGMA_FEELIN_LIB_H
#define _PRAGMA_FEELIN_LIB_H

/*
**    $VER: <pragma/feelin_lib.h> 8.0 (2004/12/18)
**
**    C pragmas definitions
**
**    © 2001-2004 Olivier LAVIALE (gofromiel@gofromiel.com)
*/

#ifndef CLIB_FEELIN_PROTOS_H
#include <clib/feelin_protos.h>
#endif

#if defined(AZTEC_C) || defined(__MAXON__) || defined(__STORM__)
#pragma amicall(FeelinBase,0x01e,F_CreatePoolA(d0,a0))
#pragma amicall(FeelinBase,0x024,F_DeletePool(a0))
#pragma amicall(FeelinBase,0x048,F_New(d0))
#pragma amicall(FeelinBase,0x04e,F_NewP(a0,d0))
#pragma amicall(FeelinBase,0x054,F_Dispose(a1))
#pragma amicall(FeelinBase,0x060,F_OPool(a0))
#pragma amicall(FeelinBase,0x066,F_SPool(a0))
#pragma amicall(FeelinBase,0x06c,F_RPool(a0))
#pragma amicall(FeelinBase,0x078,F_LinkTail(a0,a1))
#pragma amicall(FeelinBase,0x07e,F_LinkHead(a0,a1))
#pragma amicall(FeelinBase,0x084,F_LinkInsert(a0,a1,a2))
#pragma amicall(FeelinBase,0x08a,F_LinkMove(a0,a1,a2))
#pragma amicall(FeelinBase,0x090,F_LinkRemove(a0,a1))
#pragma amicall(FeelinBase,0x096,F_LinkMember(a0,a1))
#pragma amicall(FeelinBase,0x09c,F_NextNode(a0))
#pragma amicall(FeelinBase,0x0a2,F_StrFmtA(a0,a1,a2))
#pragma amicall(FeelinBase,0x0a8,F_StrNewA(a0,a1,a2))
#pragma amicall(FeelinBase,0x0ae,F_StrLen(a0))
#pragma amicall(FeelinBase,0x0b4,F_StrCmp(a0,a1,d1))
#pragma amicall(FeelinBase,0x0ba,F_LogA(d0,a0,a1))
#pragma amicall(FeelinBase,0x0c0,F_AlertA(a0,a1,a2))
#pragma amicall(FeelinBase,0x0c6,F_HashCreate(d0))
#pragma amicall(FeelinBase,0x0cc,F_HashDelete(a0))
#pragma amicall(FeelinBase,0x0d2,F_HashFind(a0,a1,d1,a2))
#pragma amicall(FeelinBase,0x0d8,F_HashAddLink(a0,a1))
#pragma amicall(FeelinBase,0x0de,F_HashRemLink(a0,a1))
#pragma amicall(FeelinBase,0x0e4,F_HashAdd(a0,a1,d0,a2))
#pragma amicall(FeelinBase,0x0ea,F_HashRem(a0,a1,d0))
#pragma amicall(FeelinBase,0x0f0,F_DynamicCreate(a0))
#pragma amicall(FeelinBase,0x0f6,F_DynamicDelete(a0))
#pragma amicall(FeelinBase,0x0fc,F_DynamicFindAttribute(a0,a1,a2))
#pragma amicall(FeelinBase,0x102,F_DynamicFindMethod(a0,a1,a2))
#pragma amicall(FeelinBase,0x108,F_DynamicFindID(a0))
#pragma amicall(FeelinBase,0x10e,F_DynamicResolveTable(a0))
#pragma amicall(FeelinBase,0x114,F_DynamicAddAutoTable(a0))
#pragma amicall(FeelinBase,0x11a,F_DynamicRemAutoTable(a0))
#pragma amicall(FeelinBase,0x120,F_DynamicNTI(a0,a1,a2))
#pragma amicall(FeelinBase,0x126,F_DynamicFTI(d0,a0))
#pragma amicall(FeelinBase,0x12c,F_DynamicGTD(d0,d1,a0))
#pragma amicall(FeelinBase,0x132,F_FindClass(a0))
#pragma amicall(FeelinBase,0x138,F_OpenClass(a0))
#pragma amicall(FeelinBase,0x13e,F_CloseClass(a0))
#pragma amicall(FeelinBase,0x144,F_CreateClassA(a0,a1))
#pragma amicall(FeelinBase,0x14a,F_DeleteClass(a0))
#pragma amicall(FeelinBase,0x150,F_DoA(a0,d0,a1))
#pragma amicall(FeelinBase,0x156,F_ClassDoA(a2,a0,d0,a1))
#pragma amicall(FeelinBase,0x15c,F_SuperDoA(a2,a0,d0,a1))
#pragma amicall(FeelinBase,0x162,F_NewObjA(a0,a1))
#pragma amicall(FeelinBase,0x168,F_MakeObjA(d0,a0))
#pragma amicall(FeelinBase,0x16e,F_DisposeObj(a0))
#pragma amicall(FeelinBase,0x174,F_Get(a0,d1))
#pragma amicall(FeelinBase,0x17a,F_Set(a0,d1,d2))
#pragma amicall(FeelinBase,0x180,F_Draw(a0,d0))
#pragma amicall(FeelinBase,0x186,F_Layout(a0,d0,d1,d2,d3,d4))
#pragma amicall(FeelinBase,0x18c,F_Erase(a0,d0,d1,d2,d3,d4))
#pragma amicall(FeelinBase,0x192,F_SharedFind(a0))
#pragma amicall(FeelinBase,0x198,F_SharedCreate(a0,a1))
#pragma amicall(FeelinBase,0x19e,F_SharedDelete(a0))
#pragma amicall(FeelinBase,0x1a4,F_SharedOpen(a0))
#pragma amicall(FeelinBase,0x1aa,F_SharedClose(a0))
#endif
#if defined(_DCC) || defined(__SASC)
#pragma  libcall FeelinBase F_CreatePoolA          01e 8002
#pragma  libcall FeelinBase F_DeletePool           024 801
#pragma  libcall FeelinBase F_New                  048 001
#pragma  libcall FeelinBase F_NewP                 04e 0802
#pragma  libcall FeelinBase F_Dispose              054 901
#pragma  libcall FeelinBase F_OPool                060 801
#pragma  libcall FeelinBase F_SPool                066 801
#pragma  libcall FeelinBase F_RPool                06c 801
#pragma  libcall FeelinBase F_LinkTail             078 9802
#pragma  libcall FeelinBase F_LinkHead             07e 9802
#pragma  libcall FeelinBase F_LinkInsert           084 a9803
#pragma  libcall FeelinBase F_LinkMove             08a a9803
#pragma  libcall FeelinBase F_LinkRemove           090 9802
#pragma  libcall FeelinBase F_LinkMember           096 9802
#pragma  libcall FeelinBase F_NextNode             09c 801
#pragma  libcall FeelinBase F_StrFmtA              0a2 a9803
#pragma  libcall FeelinBase F_StrNewA              0a8 a9803
#pragma  libcall FeelinBase F_StrLen               0ae 801
#pragma  libcall FeelinBase F_StrCmp               0b4 19803
#pragma  libcall FeelinBase F_LogA                 0ba 98003
#pragma  libcall FeelinBase F_AlertA               0c0 a9803
#pragma  libcall FeelinBase F_HashCreate           0c6 001
#pragma  libcall FeelinBase F_HashDelete           0cc 801
#pragma  libcall FeelinBase F_HashFind             0d2 a19804
#pragma  libcall FeelinBase F_HashAddLink          0d8 9802
#pragma  libcall FeelinBase F_HashRemLink          0de 9802
#pragma  libcall FeelinBase F_HashAdd              0e4 a09804
#pragma  libcall FeelinBase F_HashRem              0ea 09803
#pragma  libcall FeelinBase F_DynamicCreate        0f0 801
#pragma  libcall FeelinBase F_DynamicDelete        0f6 801
#pragma  libcall FeelinBase F_DynamicFindAttribute 0fc a9803
#pragma  libcall FeelinBase F_DynamicFindMethod    102 a9803
#pragma  libcall FeelinBase F_DynamicFindID        108 801
#pragma  libcall FeelinBase F_DynamicResolveTable  10e 801
#pragma  libcall FeelinBase F_DynamicAddAutoTable  114 801
#pragma  libcall FeelinBase F_DynamicRemAutoTable  11a 801
#pragma  libcall FeelinBase F_DynamicNTI           120 a9803
#pragma  libcall FeelinBase F_DynamicFTI           126 8002
#pragma  libcall FeelinBase F_DynamicGTD           12c 81003
#pragma  libcall FeelinBase F_FindClass            132 801
#pragma  libcall FeelinBase F_OpenClass            138 801
#pragma  libcall FeelinBase F_CloseClass           13e 801
#pragma  libcall FeelinBase F_CreateClassA         144 9802
#pragma  libcall FeelinBase F_DeleteClass          14a 801
#pragma  libcall FeelinBase F_DoA                  150 90803
#pragma  libcall FeelinBase F_ClassDoA             156 908a04
#pragma  libcall FeelinBase F_SuperDoA             15c 908a04
#pragma  libcall FeelinBase F_NewObjA              162 9802
#pragma  libcall FeelinBase F_MakeObjA             168 8002
#pragma  libcall FeelinBase F_DisposeObj           16e 801
#pragma  libcall FeelinBase F_Get                  174 1802
#pragma  libcall FeelinBase F_Set                  17a 21803
#pragma  libcall FeelinBase F_Draw                 180 0802
#pragma  libcall FeelinBase F_Layout               186 43210806
#pragma  libcall FeelinBase F_Erase                18c 43210806
#pragma  libcall FeelinBase F_SharedFind           192 801
#pragma  libcall FeelinBase F_SharedCreate         198 9802
#pragma  libcall FeelinBase F_SharedDelete         19e 801
#pragma  libcall FeelinBase F_SharedOpen           1a4 801
#pragma  libcall FeelinBase F_SharedClose          1aa 801
#endif
#ifdef __STORM__
#pragma tagcall(FeelinBase,0x01e,F_CreatePool(d0,a0))
#pragma tagcall(FeelinBase,0x0a2,F_StrFmt(a0,a1,a2))
#pragma tagcall(FeelinBase,0x0a8,F_StrNew(a0,a1,a2))
#pragma tagcall(FeelinBase,0x0ba,F_Log(d0,a0,a1))
#pragma tagcall(FeelinBase,0x0c0,F_Alert(a0,a1,a2))
#pragma tagcall(FeelinBase,0x144,F_CreateClass(a0,a1))
#pragma tagcall(FeelinBase,0x150,F_Do(a0,d0,a1))
#pragma tagcall(FeelinBase,0x156,F_ClassDo(a2,a0,d0,a1))
#pragma tagcall(FeelinBase,0x15c,F_SuperDo(a2,a0,d0,a1))
#pragma tagcall(FeelinBase,0x162,F_NewObj(a0,a1))
#pragma tagcall(FeelinBase,0x168,F_MakeObj(d0,a0))
#endif
#ifdef __SASC_60
#pragma  tagcall FeelinBase F_CreatePool           01e 8002
#pragma  tagcall FeelinBase F_StrFmt               0a2 a9803
#pragma  tagcall FeelinBase F_StrNew               0a8 a9803
#pragma  tagcall FeelinBase F_Log                  0ba 98003
#pragma  tagcall FeelinBase F_Alert                0c0 a9803
#pragma  tagcall FeelinBase F_CreateClass          144 9802
#pragma  tagcall FeelinBase F_Do                   150 90803
#pragma  tagcall FeelinBase F_ClassDo              156 908a04
#pragma  tagcall FeelinBase F_SuperDo              15c 908a04
#pragma  tagcall FeelinBase F_NewObj               162 9802
#pragma  tagcall FeelinBase F_MakeObj              168 8002
#endif

#endif  /*  _PRAGMA_FEELIN_LIB_H  */

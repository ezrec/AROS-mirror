#ifndef PRAGMAS_CYBERGRAPHICS_PRAGMAS_H
#define PRAGMAS_CYBERGRAPHICS_PRAGMAS_H

#ifndef CLIB_CYBERGRAPHICS_PROTOS_H
#include <clib/cybergraphics_protos.h>
#endif

#pragma  libcall CyberGfxBase IsCyberModeID          036 001
#pragma  libcall CyberGfxBase BestCModeIDTagList     03c 801
#pragma  libcall CyberGfxBase CModeRequestTagList    042 9802
#pragma  libcall CyberGfxBase AllocCModeListTagList  048 901
#pragma  libcall CyberGfxBase FreeCModeList          04e 801
#pragma  libcall CyberGfxBase ScalePixelArray        05a 76543921080a
#pragma  libcall CyberGfxBase GetCyberMapAttr        060 0802
#pragma  libcall CyberGfxBase GetCyberIDAttr         066 1002
#pragma  libcall CyberGfxBase ReadRGBPixel           06c 10903
#pragma  libcall CyberGfxBase WriteRGBPixel          072 210904
#pragma  libcall CyberGfxBase ReadPixelArray         078 76543921080a
#pragma  libcall CyberGfxBase WritePixelArray        07e 76543921080a
#pragma  libcall CyberGfxBase MovePixelArray         084 543291007
#pragma  libcall CyberGfxBase InvertPixelArray       090 3210905
#pragma  libcall CyberGfxBase FillPixelArray         096 43210906
#pragma  libcall CyberGfxBase DoCDrawMethodTagList   09c a9803
#pragma  libcall CyberGfxBase CVideoCtrlTagList      0a2 9802
#pragma  libcall CyberGfxBase LockBitMapTagList      0a8 9802
#pragma  libcall CyberGfxBase UnLockBitMap           0ae 801
#pragma  libcall CyberGfxBase UnLockBitMapTagList    0b4 9802
#pragma  libcall CyberGfxBase ExtractColor           0ba 432109807
#pragma  libcall CyberGfxBase WriteLUTPixelArray     0c6 76543a921080b
#pragma  libcall CyberGfxBase WritePixelArrayAlpha   0d8 76543921080a
#pragma  libcall CyberGfxBase BltTemplateAlpha       0de 5432910808
#pragma  libcall CyberGfxBase ProcessPixelArray      0e4 a543210908
#pragma  libcall CyberGfxBase BltBitMapAlpha         0ea a5432910809
#pragma  libcall CyberGfxBase BltBitMapRastPortAlpha 0f0 a5432910809
#pragma  libcall CyberGfxBase ScalePixelArrayAlpha   102 76543921080a
#ifdef __SASC_60
#pragma  tagcall CyberGfxBase BestCModeIDTags        03c 801
#pragma  tagcall CyberGfxBase CModeRequestTags       042 9802
#pragma  tagcall CyberGfxBase AllocCModeListTags     048 901
#pragma  tagcall CyberGfxBase DoCDrawMethodTags      09c a9803
#pragma  tagcall CyberGfxBase CVideoCtrlTags         0a2 9802
#pragma  tagcall CyberGfxBase LockBitMapTags         0a8 9802
#pragma  tagcall CyberGfxBase UnLockBitMapTags       0b4 9802
#pragma  tagcall CyberGfxBase ProcessPixelArrayTags  0e4 a543210908
#endif

#endif	/*  PRAGMAS_CYBERGRAPHICS_PRAGMA_H  */

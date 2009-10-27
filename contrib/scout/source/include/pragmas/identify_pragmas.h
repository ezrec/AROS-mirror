#ifndef PRAGMAS_IDENTIFY_PRAGMAS_H
#define PRAGMAS_IDENTIFY_PRAGMAS_H

#ifndef CLIB_IDENTIFY_PROTOS_H
#include <clib/identify_protos.h>
#endif

#if defined(AZTEC_C) || defined(__MAXON__) || defined(__STORM__)
#pragma amicall(IdentifyBase,0x01E,IdExpansion(a0))
#pragma amicall(IdentifyBase,0x024,IdHardware(d0,a0))
#pragma amicall(IdentifyBase,0x02A,IdAlert(d0,a0))
#pragma amicall(IdentifyBase,0x030,IdFunction(a0,d0,a1))
#pragma amicall(IdentifyBase,0x036,IdHardwareNum(d0,a0))
#pragma amicall(IdentifyBase,0x03C,IdHardwareUpdate())
#pragma amicall(IdentifyBase,0x042,IdFormatString(a0,a1,d0,a2))
#pragma amicall(IdentifyBase,0x048,IdEstimateFormatSize(a0,a1))
#endif
#if defined(_DCC) || defined(__SASC)
#pragma  libcall IdentifyBase IdExpansion          01E 801
#pragma  libcall IdentifyBase IdHardware           024 8002
#pragma  libcall IdentifyBase IdAlert              02A 8002
#pragma  libcall IdentifyBase IdFunction           030 90803
#pragma  libcall IdentifyBase IdHardwareNum        036 8002
#pragma  libcall IdentifyBase IdHardwareUpdate     03C 00
#pragma  libcall IdentifyBase IdFormatString       042 A09804
#pragma  libcall IdentifyBase IdEstimateFormatSize 048 9802
#endif
#ifdef __STORM__
#pragma tagcall(IdentifyBase,0x01E,IdExpansionTags(a0))
#pragma tagcall(IdentifyBase,0x024,IdHardwareTags(d0,a0))
#pragma tagcall(IdentifyBase,0x02A,IdAlertTags(d0,a0))
#pragma tagcall(IdentifyBase,0x030,IdFunctionTags(a0,d0,a1))
#pragma tagcall(IdentifyBase,0x036,IdHardwareNumTags(d0,a0))
#pragma tagcall(IdentifyBase,0x042,IdFormatStringTags(a0,a1,d0,a2))
#pragma tagcall(IdentifyBase,0x048,IdEstimateFormatSizeTags(a0,a1))
#endif
#ifdef __SASC_60
#pragma  tagcall IdentifyBase IdExpansionTags      01E 801
#pragma  tagcall IdentifyBase IdHardwareTags       024 8002
#pragma  tagcall IdentifyBase IdAlertTags          02A 8002
#pragma  tagcall IdentifyBase IdFunctionTags       030 90803
#pragma  tagcall IdentifyBase IdHardwareNumTags    036 8002
#pragma  tagcall IdentifyBase IdFormatStringTags   042 A09804
#pragma  tagcall IdentifyBase IdEstimateFormatSizeTags 048 9802
#endif

#endif	/*  PRAGMAS_IDENTIFY_PRAGMA_H  */

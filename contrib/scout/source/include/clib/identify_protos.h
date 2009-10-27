#ifndef  CLIB_IDENTIFY_PROTOS_H
#define  CLIB_IDENTIFY_PROTOS_H

/*
**      $VER: identify_protos.h 11.0 (23.4.99)
**
**      (C) Copyright 1996-1999 Richard Körber
**          All Rights Reserved
*/

LONG IdAlert(ULONG, struct TagItem *);
#if !defined(USE_INLINE_STDARG)
LONG IdAlertTags(ULONG, ULONG,...);
#endif
ULONG IdEstimateFormatSize(STRPTR, struct TagItem *);
#if !defined(USE_INLINE_STDARG)
ULONG IdEstimateFormatSizeTags(STRPTR, ULONG,...);
#endif
LONG IdExpansion(struct TagItem *);
#if !defined(USE_INLINE_STDARG)
LONG IdExpansionTags(ULONG,...);
#endif
ULONG IdFormatString(STRPTR, STRPTR, ULONG, struct TagItem *);
#if !defined(USE_INLINE_STDARG)
ULONG IdFormatStringTags(STRPTR, STRPTR, ULONG, ...);
#endif
LONG IdFunction(STRPTR, LONG, struct TagItem *);
#if !defined(USE_INLINE_STDARG)
LONG IdFunctionTags(STRPTR, LONG, ULONG,...);
#endif
STRPTR IdHardware(ULONG, struct TagItem *);
#if !defined(USE_INLINE_STDARG)
STRPTR IdHardwareTags(ULONG, ULONG,...);
#endif
ULONG IdHardwareNum(ULONG, struct TagItem *);
#if !defined(USE_INLINE_STDARG)
ULONG IdHardwareNumTags(ULONG, ULONG,...);
#endif
void IdHardwareUpdate(void);

#endif

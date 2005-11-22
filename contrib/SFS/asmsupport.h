#ifndef ASMSUPPORT_H_
#define ASMSUPPORT_H_

#include <exec/types.h>

inline WORD bfffo(ULONG data, WORD bitoffset);
inline WORD bfflo(ULONG data, WORD bitoffset);
inline WORD bfflz(ULONG data, WORD bitoffset);
inline WORD bfffz(ULONG data, WORD bitoffset);
inline ULONG bfset(ULONG data, WORD bitoffset, WORD bits);
inline ULONG bfclr(ULONG data, WORD bitoffset, WORD bits);
ULONG bfcnto(ULONG v);
ULONG bfcntz(ULONG v);
LONG bmflo(ULONG *bitmap, LONG bitoffset);
LONG bmflz(ULONG *bitmap, LONG bitoffset);
LONG bmffo(ULONG *bitmap, LONG longs, LONG bitoffset);
LONG bmffz(ULONG *bitmap, LONG longs, LONG bitoffset);
LONG bmclr(ULONG *bitmap, LONG longs, LONG bitoffset, LONG bits);
LONG bmset(ULONG *bitmap, LONG longs, LONG bitoffset, LONG bits);
BOOL bmtsto(ULONG *bitmap, LONG bitoffset, LONG bits);
BOOL bmtstz(ULONG *bitmap, LONG bitoffset, LONG bits);
ULONG bmcnto(ULONG *bitmap, LONG bitoffset, LONG bits);
ULONG bmcntz(ULONG *bitmap, LONG bitoffset, LONG bits);

ULONG CALCCHECKSUM(ULONG, ULONG *);

#ifdef __AROS__
#define putChProc   (NULL)
#else
#define putChProc   (void (*)())"\x16\xC0\x4E\x75"
#endif

#endif /*ASMSUPPORT_H_*/

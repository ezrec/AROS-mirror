#ifndef _INCLUDE_GRAFFITI_H
#define _INCLUDE_GRAFFITI_H

/* $VER: graffiti.library 1.0 (12-Nov-96 ) */

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __SASC
#pragma libcall GraffitiBase OpenChunkyScreen 1e 001
#pragma libcall GraffitiBase CloseChunkyScreen 24 001
#pragma libcall GraffitiBase SetChunkyPalette 2a 1002
#pragma libcall GraffitiBase CopyChunkyScreen 30 1002
#else
#pragma amicall(GraffitiBase, 0x1E, OpenChunkyScreen(d0))
#pragma amicall(GraffitiBase, 0x24, CloseChunkyScreen(d0))
#pragma amicall(GraffitiBase, 0x2A, SetChunkyPalette(d0, d1))
#pragma amicall(GraffitiBase, 0x30, CopyChunkyScreen(d0, d1))
#endif
#ifdef __cplusplus
};
#endif
#endif
